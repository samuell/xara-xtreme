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
#ifndef INC_FLASH_EXPORT_DC
#define INC_FLASH_EXPORT_DC

#include "kerneldc.h"
#include "fillattr.h"

#include "swfshape.h"
#include "swfbitmp.h"
#include "swffont.h"
#include "swftext.h"
#include "swfbuttn.h"
#include "swfsprit.h"
#include "swfplace.h"

// Constants for Flash export.
#define FLASH_VERSION			4
#define FLASH_FRAME_RATE		0x0c00	// 12.0 frames per second.
#define FLASH_FRAMES			0x0001	// Only one frame.

#define FLASH_MAX_SHORT			63		// Max length of a short tag header.
#define FLASH_SCALE				50		// Scale to convert millipoints to twips.

#define FLASH_RAMP_WIDTH		32768	// Width of the colour ramp for gradient fills.
#define FLASH_FIXED_ONE			65536	// 1 in the 16.16 fixed point scheme used by Flash.

#define FLASH_NO_FILL			0xff	// Tag for no fills being used. (Not part of SWF.)
#define FLASH_FLAT_FILL			0x00	// Tag for a flat fill being used.
#define FLASH_LINEAR_FILL		0x10	// Tag for linear fills.
#define FLASH_RADIAL_FILL		0x12	// Tag for radial fills.
#define FLASH_TILED_BITMAP		0x40	// Tiled bitmap fill.
#define FLASH_CLIPPED_BITMAP	0x41	// Clipped bitmap fill.

#define FLASH_BACKGROUND_TAG	9		// Background colour Tag.
#define FLASH_SHAPE_TAG			2		// DefineShape Tag.
#define FLASH_SHAPE3_TAG		32		// DefineShape3 Tag.
#define FLASH_BITS_TAG			20		// DefineBitsLossless Tag.
#define FLASH_BITS2_TAG			36		// DefineBitsLossless2 Tag.
#define FLASH_END_FILE_TAG		0		// End of file tag.
#define FLASH_SHOW_FRAME_TAG	1		// Show the frame.
#define FLASH_PLACE_OBJECT_TAG	26		// Place an object. (Using PlaceObject2)
#define FLASH_TEXT_TAG			33		// DefineText2 Tag.
#define FLASH_FONT_TAG			10		// DefineFont Tag.
#define FLASH_FONT_INFO_TAG		13		// DefineFontInfo Tag.
#define FLASH_BUTTON_TAG		7		// DefineButton Tag.
#define FLASH_SPRITE_TAG		39		// DefineSprite tag.

#define FLASH_BUTTON_HIT		8		// Hit test this button.
#define FLASH_BUTTON_DOWN		4		// State when it's down.
#define FLASH_BUTTON_OVER		2		// State when cursor is over.
#define FLASH_BUTTON_UP			1		// State when button is released up.

#define FLASH_GET_URL			0x83	// Action tag for the "Get URL" script.

#define FLASH_FONT_SIZE			51200	// Default font size for exporting text shapes.

/********************************************************************************************

>	class FlashExportDC : public ExportDC

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/99
	Purpose:	Outline for a Flash export DC.
	SeeAlso:	-

********************************************************************************************/

class FlashExportDC : public ExportDC
{
public:
	FlashExportDC ( void );
	FlashExportDC ( Filter *Parent );					// Constructor.
	virtual ~FlashExportDC ( void );					// Destructor.
	BOOL Init ( CCLexFile *pFile );						// Use base class init() function.

	BOOL CreateHeader ( DocColour Background,
						DocRect ImageBounds );

	BOOL EndFile ( void );

	BOOL WritePath ( FlashShapeRecord *pPath );			// Write a path to the file.

	BOOL WriteMoveTo ( DocCoord Position,				// Write a move to.
					   UINT32 ChangeLine,
					   UINT32 ChangeFill0,
					   UINT32 ChangeFill1);

	BOOL WriteEdgeRecord ( DocCoord *Coords,
						   DocCoord *MoveTo,
						   PathVerb *Verb,
						   INT32 NumCoords,
						   UINT32 LineStyle,
						   UINT32 Fill0,
						   UINT32 Fill1,
						   double Tolerance = 100.0f,
						   BOOL IsCircular = FALSE );

	UINT32 CountBits ( INT32 Value,						// Count the number of set bits.
					 BOOL IsSigned = TRUE );

	BOOL WriteBoundingBox ( DocRect Bounds );
	BOOL WriteBits ( INT32 Value,						// Write Bits bits to an output file.
					 UINT32 Bits );
	BOOL WriteBackgroundColour ( DocColour Background );// Write background colour tag.
	BOOL WriteColour ( DocColour Col );					// Write RGB Colour value to disk.
	BOOL WriteColour ( FlashColour Col );				// Write RGB Colour value to disk.
	BOOL WriteHeader ( UINT8 sTag,						// Write a header tag to the file.
					   UINT32 lSize );
	FilePos WriteHeader ( UINT8 sTag,					// Write a header tag.
						  UINT32 lSize,
						  WORD ID );
	BOOL ByteAlign ( void );							// Align data in output file.
	BOOL Write ( const void *Value,						// Wrapper for the write function.
				 UINT32 Size );

	inline BOOL WriteByte ( BYTE Value );				// Write a BYTE value.
	inline BOOL WriteWord ( WORD Value );				// Write a WORD value.
	inline BOOL WriteLong ( INT32 Value );				// Write a INT32 value.

	BOOL WritePlaceObject ( FlashPlaceObject *pPlace );	// Write a place object tag.

	BOOL WritePlaceObject ( FlashPlaceObject *pPlace,	// Write a place object tag.
							WORD Depth );

	BOOL WriteLineTo ( DocCoord Start,					// Write a line to.
					   DocCoord End );


	BOOL WriteCurveTo ( DocCoord Start,					// Write a curve section.
						DocCoord Control,
						DocCoord End );

	BOOL ExportCurve ( DocCoord &Start,					// Prepares a curveto for export.
					   DocCoord &Control1,
					   DocCoord &Control2,
					   DocCoord &End,
					   double Tolerance );

	BOOL CalculateIntersection ( DocCoord &Start1,		// Calculates an intersection.
								 DocCoord &End1,
								 DocCoord &Start2,
								 DocCoord &End2,
								 DocCoord &Intersection );

	static DocCoord CalculateMidpoint ( DocCoord Point1,
										DocCoord Point2 );

	BOOL WriteText ( FlashTextRecord *pText );

	BOOL WriteDefineFont ( FlashFontRecord *pRecord );

	BOOL WriteDefineFontInfo ( FlashFontRecord *pRecord );

	BOOL WritePNG ( OILBitmap	*pBitmap,				// Writes a DefineBitsLossless tag.
					WORD		BitmapID,
					UINT32		Alpha,
					BOOL		IsContoned );

	BOOL WriteTransparentBitmap ( OILBitmap	*pBitmap,	// Writes a bitmap fill.
								  WORD		BitmapID,
								  DocColour	Colour );

	BOOL WriteShadowBitmap ( OILBitmap	*pBitmap,		// Writes a shadow bitmap.
							 WORD		BitmapID,
							 DocColour	Colour,
							 UINT32		Darkness );

	BOOL WriteURLScript ( FlashButtonRecord *pButton );	// Creates a button record.

	BOOL ProcessSprite ( FlashSprite *pSprite,			// Processes a sprite record before
						 FlashButtonRecord *pButton );	// writing it out.

	BOOL WriteSprite ( FlashSprite *pSprite );			// Creates a sprite record.

	// Static member functions.
	static double CalculateLength ( DocCoord Delta );		// Calculates the length of a line.
	static INT32 CalculateLongLength ( DocCoord Delta );		// Calculates the length of a line.

private:

	BOOL ProcessFill ( FlashShapeRecord *pShape,		// Set up the fill.
					   DocCoord *MoveTo );

	BOOL Write32BitBitmap ( OILBitmap	*pBitmap,		// Writes a true colour bitmap out.
							WORD		BitmapID,
							UINT32		Alpha );

	BOOL Write8BitBitmap ( OILBitmap	*pBitmap,		// Writes a 256 colour bitmap out.
						   WORD			BitmapID,
						   UINT32			Transparency,
						   BOOL			Alpha );

	FilePos Write8BitHeader ( OILBitmap	*pBitmap,		// Writes the header for a bitmap.
							  WORD		BitmapID,
							  UINT32		NumColours,
							  BOOL		Transparent );

	BOOL Write8BitBody ( OILBitmap	*pBitmap,			// Writes out the bitmap's body.
						 FilePos	StartPos );

	BOOL WriteBitmapBits ( OILBitmap *pBitmap );

	void Transform ( DocCoord *Point,					// Puts a point in Flash coordinates.
					 DocCoord *MoveTo = NULL );

	void Transform ( DocRect *Bounds );					// Convert a BB into Flash coordinates.

	BOOL WriteMatrix ( DocCoord &Translate,				// Write a matrix to the file.
					   INT32 ScaleX = FLASH_FIXED_ONE,
					   INT32 ScaleY = FLASH_FIXED_ONE,
					   INT32 SkewX = 0,
					   INT32 SkewY = 0 );

	BOOL WriteRotationMatrix ( DocCoord &Translate,		// Write a rotation matrix.
							   INT32 ScaleX = FLASH_FIXED_ONE,
							   INT32 ScaleY = FLASH_FIXED_ONE,
							   double SinTheta = 0,
							   double CosTheta = 1.0f );

	void CalculateTrigValues ( DocCoord &StartPoint,	// Calculates the values for the matrix.
							   DocCoord &EndPoint,
							   INT32 &Length,
							   double &SinTheta,
							   double &CosTheta );

	BOOL WriteGradientFill ( FlashShapeRecord *pShape,
							 DocCoord *MoveTo );

	BOOL WriteBitmapFill ( FlashShapeRecord *pShape,
						   DocCoord *MoveTo );

	BOOL WriteTextMatrix ( INT32 ScaleX,
						   INT32 ScaleY,
						   INT32 SkewX,
						   INT32 SkewY,
						   const FIXED16 &Aspect );

	BOOL WriteTextRecord ( FlashTextRecord *pText );

	FilePos WriteChangeText ( const FlashColour &TextColour,
							  WORD Size,
							  WORD Style,
							  INT8 YOffset,
							  UINT32 HasColour,
							  UINT32 HasFont,
							  UINT32 HasYOffset );

	BOOL WriteGlyphHeader ( FilePos Start,
							BYTE GlyphCount );

	BOOL WriteTagSize ( FilePos StartPos );

	void BuildComplexRotation ( DocCoord &Position,		// Creates a rotation matrix.
								INT32 &ScaleX,
								INT32 &ScaleY,
								INT32 &SkewX,
								INT32 &SkewY,
								const DocCoord &Centre,
								INT32 SinTheta,
								INT32 CosTheta );

	BOOL WriteComplexRotation ( DocCoord Position,		// Writes a rotation matrix.
								INT32 ScaleX,
								INT32 ScaleY,
								INT32 SkewX,
								INT32 SkewY,
								const DocCoord &Centre,
								INT32 SinTheta,
								INT32 CosTheta );

	BOOL WriteButtonRecord ( WORD ObjectID,				// Writes a button record.
							 BYTE State,
							 DocCoord Position,
							 BOOL DoTransform );

	BOOL WriteGetURL ( FlashButtonRecord *pButton );	// Writes a get URL Flash script.

	// Member variables.
	BYTE		m_bOutput;								// Bitwise data to be outputted.
	INT32			m_bCurrentBit;							// Next bit to be written to.
	WORD		mDepth;									// Depth of current shape.
	INT32		mTranslateX;							// Used by Translate ().
	INT32		mTranslateY;							// Used by Translate ().
	DocCoord	mLowCorner;								// The low corner of the image.
	Filter		*pFilter;								// Pointer to parent.
};

#endif
