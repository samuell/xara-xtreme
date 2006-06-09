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
#include <math.h>
#include <string.h>

#include "camelot.h"
#include "camtypes.h"

#include "colormgr.h"
//#include "colmodel.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bevtrap.h"
//#include "fillval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "fillramp.h"
//#include "bitmap.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "pathtype.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "pathutil.h"

//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docrect.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "doccolor.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "zstream.h"

#include "fontman.h"
//#include "app.h"			// For FONTMANAGER - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "filters.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "prvwflt.h"
#include "pngfiltr.h"
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "swfrndr.h"
#include "kerneldc.h"
#include "swfexpdc.h"

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	FlashExportDC::FlashExportDC( Filter *Parent )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/99
	Purpose:	Default constructor: Creates a DC for exporting Flash files.
	SeeAlso:	ExportDC::ExportDC

********************************************************************************************/

FlashExportDC::FlashExportDC () : ExportDC ( NULL )
{
	// Set the output variables to their correct setting.
	m_bOutput = 0;
	m_bCurrentBit = 7;
	mDepth = 1;
	pFilter = NULL;

	mLowCorner.x = mLowCorner.y = 0;
}

/********************************************************************************************

>	FlashExportDC::FlashExportDC( Filter *Parent )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/99
	Purpose:	Constructor: Creates a DC for exporting Flash files.
	SeeAlso:	ExportDC::ExportDC

********************************************************************************************/

FlashExportDC::FlashExportDC ( Filter *Parent ) : ExportDC ( Parent )
{
	// Set the output variables to their correct setting.
	m_bOutput = 0;
	m_bCurrentBit = 7;
	mDepth = 1;
	pFilter = Parent;

	mLowCorner.x = mLowCorner.y = 0;
}

/********************************************************************************************

>	FlashExportDC::~FlashExportDC ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/99
	Purpose:	Destructor. I'm not sure if this is needed, since export files are cleaned
				up for me elsewhere.
	SeeAlso:	ExportDC::ExportDC

********************************************************************************************/

FlashExportDC::~FlashExportDC( void )
{
	// Any flash specific stuff will go in here.
}

/********************************************************************************************

>	FlashExportDC::Init ( CCLexFile *pFile )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/99
	Purpose:	Calls ExportDC::Init ().
	SeeAlso:	ExportDC::ExportDC

********************************************************************************************/


BOOL FlashExportDC::Init ( CCLexFile *pFile )
{
	if ( pFile == NULL )
	{
		return FALSE;
	}

	return ExportDC::Init ( pFile );
}

/********************************************************************************************

>	FlashExportDC::CreateHeader ( DocColour Background,
								  DocRect *PageRect )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/99
	Purpose:	Creates a Flash file header.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::CreateHeader ( DocColour Background,
								   DocRect ImageBounds )
{
	// Calculate the size of the image. To save complications, I've moved the image to
	// 0,0, rather than from whatever bounds Camelot uses.
	mLowCorner.x = ImageBounds.lox / FLASH_SCALE;
	mLowCorner.y = ImageBounds.hiy / FLASH_SCALE;

	Transform ( &ImageBounds );

	// Write the header for the Flash file.
	Write ( "FWS", 3 );						// File ID tag.
	WriteByte ( 4 );						// Version number.
	WriteLong ( 0 );						// Space for file size.
	WriteBoundingBox ( ImageBounds );		// Bounding rectangle for image.
	WriteByte ( 0 );						// Frames per second. (Decimal part.)
	WriteByte ( 12 );						// Frames per second. (Integer part.)
	WriteWord ( 1 );						// Number of frames.
	WriteBackgroundColour ( Background );	// Background colour.

	return TRUE;
}

/********************************************************************************************

>	FlashExportDC::EndFile ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/99
	Purpose:	Tidies up the file..
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::EndFile ( void )
{
	// Write show frame.
	WriteHeader ( FLASH_SHOW_FRAME_TAG, 0 );

	// Write end of file.
	WriteHeader ( FLASH_END_FILE_TAG, 0 );

	// Rewind, and add the file size.
	ExportFile->seekIn ( 4 );
	WriteLong ( (INT32) ExportFile->Size() );

	return TRUE;
}

/********************************************************************************************

>	FlashExportDC::BoundingBox ( DocRect *Bounds )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Purpose:	Create a bounding box, and write it into a file.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteBoundingBox ( DocRect Bounds )
{
	// Calculate the number of significant bits needed to store the co-ordinates.
	UINT32 Bits = CountBits ( Max ( Absol ( Bounds.lox ),
								  Absol ( Bounds.loy ),
								  Absol ( Bounds.hix ),
								  Absol ( Bounds.hiy ) ) );

	// Write the bounding box to a file.
	if ( WriteBits ( Bits, 5 ) &&
		 WriteBits ( Bounds.lox, Bits ) &&
		 WriteBits ( Bounds.hix, Bits ) &&
		 WriteBits ( Bounds.loy, Bits ) &&
		 WriteBits ( Bounds.hiy, Bits ) )
	{
		ByteAlign ();
		return TRUE;
	}
	else
		return FALSE;
}

/********************************************************************************************

>	UINT32 FlashExportDC::CountBits ( INT32 Value,
									BOOL IsSigned = TRUE )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/99
	Purpose:	Counts the number of significant bits in a value.
	Returns:	Number of significant bits.
	SeeAlso:	-

********************************************************************************************/

UINT32 FlashExportDC::CountBits ( INT32 Value,
							    BOOL IsSigned )
{
	UINT32 BitCount = 0;
	
	// Add an extra bit in for negative values.
	if ( IsSigned )
		BitCount = 1;

	while ( Value )
	{
		Value >>= 1;
		BitCount ++;
	}

	return BitCount;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteBits ( INT32 Value )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/99
	Purpose:	Writes individual bits to the output file.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteBits ( INT32 Value,
								UINT32 Bits )
{
	BOOL	Result = TRUE;

	// CompareBit is bitwise ANDed with the current value. If the result is non-zero, then
	// that bit has been set, and is written to the file. CompareBit is decreased with
	// every iteration through the loop, until it equals zero, and no further bits are to
	// be compared.

	// If a zero bit bitfield is attempted to be written, don't do anything. Otherwise
	// write out the bits.
	if ( Bits > 0 )
	{
		INT32 CompareBit = 1 << ( Bits - 1 );

		while ( CompareBit )
		{
			// If that bit is set, write it to the file.
			if ( CompareBit & Value )
				m_bOutput += 1 << m_bCurrentBit;

			CompareBit >>= 1;
			m_bCurrentBit --;

			// Write out the value once the byte is full, and reset all variables.
			if ( m_bCurrentBit < 0 )
			{
				// Write bitfield value to a file.
				Result = Write ( &m_bOutput, 1 );

				// Reset the appropriate variables.
				m_bOutput = 0;
				m_bCurrentBit = 7;
			}
		}
	}
	return Result;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteBackgroundColour ( DocColour dCol )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/99
	Purpose:	Writes the background colour to the file.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteBackgroundColour ( DocColour Background )
{
	// Write the Flash tag and background colour.
	WriteHeader ( FLASH_BACKGROUND_TAG, 3 );

	WriteColour ( Background );

	return TRUE;
}
	INT32 lRed, lGreen, lBlue;

/********************************************************************************************

>	BOOL FlashExportDC::WriteColour ( DocColour Col )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Purpose:	Writes a DocColour to the file in the flash format.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteColour ( DocColour Col )
{
	INT32 lRed, lGreen, lBlue;

	// Extract the colour value from DocColour.
	Col.GetRGBValue ( &lRed, &lGreen, &lBlue );

	WriteByte ( (BYTE) lRed );
	WriteByte ( (BYTE) lGreen );
	WriteByte ( (BYTE) lBlue );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteColour ( FlashColour Col )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/99
	Purpose:	Writes the colour to the file.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteColour ( FlashColour Col )
{
	WriteByte ( Col.Red );
	WriteByte ( Col.Green );
	WriteByte ( Col.Blue );
	WriteByte ( Col.Alpha );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteHeader ( UINT8 sTag,
									  UINT32 lSize )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/99
	Purpose:	Writes a section header to the file.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteHeader ( UINT8 sTag,
								  UINT32 lSize )
{
	BOOL Result = FALSE;
	UINT8 sHeader = 0;

	ByteAlign ();	// Ensure that everything is in order.

	// Headers in a SWF file come in a long form, and a short form. The short form contains
	// a file size of up to 62 bytes, and the ID tag in one sixteen bit entry...
	if ( lSize < FLASH_MAX_SHORT )
	{
		sHeader = ( UINT8) lSize + ( sTag << 6 );
		Result = Write ( &sHeader, 2 );
	}
	// ... whilst the longer version has a successive 32 bit value giving its length.
	else
	{
		sHeader = FLASH_MAX_SHORT + ( sTag << 6 );
		if ( Write ( &sHeader, 2 ) &&
			 Write ( &lSize, 4 ) )
		{
			Result = TRUE;
		}
	}

	return Result;
}

/********************************************************************************************

>	FilePos FlashExportDC::WriteHeader ( UINT8 sTag,
										 UINT32 lSize,
										 WORD ID )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/99
	Purpose:	Writes a section header to the file, along with the ID tag.
	Returns:	The file position at the end of the header.
	SeeAlso:	-

********************************************************************************************/

FilePos FlashExportDC::WriteHeader ( UINT8 sTag,
									 UINT32 lSize,
									 WORD ID )
{
	FilePos EndOfHeader = 0;

	WriteHeader ( sTag, lSize );		// Call the original WriteHeader method.
	EndOfHeader = ExportFile->tell();	// Get the start position of the file.
	WriteWord ( ID );					// Write the tag's ID out.
	return EndOfHeader;
}

/********************************************************************************************

>	BOOL FlashExportDC::ByteAlign ()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/99
	Purpose:	Aligns bitwise values to byte boundaries..
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::ByteAlign ( void )
{
	BOOL Result = TRUE;

	if ( m_bCurrentBit != 7 )
		Result = WriteBits ( 0, m_bCurrentBit + 1 );

	return Result;
}

/********************************************************************************************

>	BOOL FlashExportDC::Write ( const void *Value,
								UINT32 Size )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/99
	Purpose:	Forms a wrapper around all disk writes, with a small degree of error
				checking.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::Write ( const void *Value,
							UINT32 Size )
{
	// I'm assuming that CCFile will return a NULL pointer if it fails, and this can be
	// caught by this code.
	if ( ExportFile->write ( Value, Size ) == NULL )
		return FALSE;
	else
		return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteByte ( BYTE Value )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/99
	Purpose:	Writes a BYTE value to the disk file.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteByte ( BYTE Value )
{
	return Write ( &Value, 1 );
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteWord ( WORD Value )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/99
	Purpose:	Writes a short value (16 bits) to the disk file.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteWord ( WORD Value )
{
	return Write ( &Value, 2 );
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteLong ( INT32 Value )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/99
	Purpose:	Writes an INT32 value (32 bits) to the disk file.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteLong ( INT32 Value )
{
	return Write ( &Value, 4 );
}

/********************************************************************************************

>	BOOL FlashExportDC::WritePath ( FlashShapeRecord *pPath )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/99
	Purpose:	Converts and writes a path to a Flash file.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WritePath ( FlashShapeRecord *pPath )
{
	// Extract the variables from the FlashPathRecord.
	DocCoord	*Coords		= pPath->GetCoords ();
	INT32			NumCoords	= pPath->GetNumberCoords ();
	PathVerb	*Verb		= pPath->GetVerbs ();
	DocRect		Bounds		= pPath->GetBoundingBox ();
	INT32			Fills		= 0;	
	INT32			Lines		= 0;
	FilePos		StartPos	= 0;
	WORD		ID			= FLASH_FIRST_ID + FlashPlaceObject::GetBitmapCount () +
							  FlashPlaceObject::GetFontCount () +
							  FlashPlaceObject::GetTextCount () + pPath->GetShapeID ();
	DocCoord	MoveTo		( Bounds.lox, Bounds.hiy );
	double		Tolerance	= 100.0f;
	FlashColour	LineColour	= pPath->GetLineColour ();
	WORD		LineWidth	= pPath->GetLineWidth ();

	Transform ( &MoveTo, &mLowCorner );			// Create the position for the low BB corner.
	Transform ( &Bounds );						// Put the bounds into Flash coordinates.

	MoveTo.x += mLowCorner.x;
	MoveTo.y = mLowCorner.y - MoveTo.y;

	// Write a header. We'll assume that the size is 64 bytes to begin with.
	// Check that transparency isn't needed:
	// Use DefineShape3, which includes support for transparency.
	StartPos = WriteHeader ( FLASH_SHAPE3_TAG, FLASH_MAX_SHORT, ID );

	// Write the bounding box to the file.
	WriteBoundingBox ( Bounds );

	// Write a shape record.

	// Fill style first.
	if ( pPath->GetFill () != FLASH_NO_FILL )
	{
		Fills = 1;
		ProcessFill ( pPath, &MoveTo );
	}
	else
	{
		// The shape is unfilled, therefore has no fill styles.
		WriteByte ( 0 );						// No fill styles.
	}

	// And now define the line style:
	if ( LineColour.Alpha == 0 || LineWidth == 0 )
	{
		WriteByte ( 0 );
	}
	else
	{
		WriteByte ( 1 );						// Number of line styles in LINESTYLEARRAY.
		WriteWord ( pPath->GetLineWidth () );	// Width.
		WriteColour ( pPath->GetLineColour () );// Line colour.
		Lines = 1;
	}

	// Write the numbers of index bits.
	WriteBits ( Fills, 4 );						// Number of fill style index bits.
	WriteBits ( Lines, 4 );						// Number of line style index bits.

	// Calculate the difference between the original curve and the generated curve. This
	// creates it as 5% of the diagonal of the bounding box.
	Tolerance = ( sqrt ( ( Bounds.hix * Bounds.hix ) + ( Bounds.hiy * Bounds.hiy ) ) ) / 20;

	// Write the edge record.
	WriteEdgeRecord ( Coords, &MoveTo, Verb, NumCoords, Lines, Fills, 0,
					  Tolerance, TRUE );

	// Go back, and fill in the number of bytes written.
	WriteTagSize ( StartPos );

	// Export the edge record.
	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteEdgeRecord ( DocCoord *Coords,
										  DocCoord *MoveTo,
										  PathVerb *Verb,
										  INT32 NumCoords,
										  UINT32 LineStyle,
										  UINT32 Fill0,
										  UINT32 Fill1,
										  double Tolerance = 100.0f,
										  BOOL IsCircular = FALSE )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/10/99
	Purpose:	Writes out a Flash edge record.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteEdgeRecord ( DocCoord *Coords,
									  DocCoord *MoveTo,
									  PathVerb *Verb,
									  INT32 NumCoords,
									  UINT32 LineStyle,
									  UINT32 Fill0,
									  UINT32 Fill1,
									  double Tolerance,
									  BOOL IsCircular )
{
	// Transform all of the co-ordinates first.
	for ( INT32 i = 0; i < NumCoords; i++ )
	{
		Transform ( &Coords [i], MoveTo );
	}

	// Write the initial move to, which includes the change of style.
	WriteMoveTo ( Coords [0], LineStyle, Fill0, Fill1 );

	// Determine which kind of edge record to write.
	for ( INT32 j = 1; j < NumCoords; j++ )
	{
		// Is it a move to?
		if ( ( Verb[j] == PT_MOVETO ) || ( Verb[j] == PT_MOVETO + PT_CLOSEFIGURE ) )
		{
			WriteMoveTo ( Coords [j], 0, 0, 0 );
		}

		// Is it a Bezier curve?
		else if ( ( Verb[j] == PT_BEZIERTO ) || ( Verb[j] == PT_BEZIERTO + PT_CLOSEFIGURE ) )
		{
			 if ( IsCircular )
			 {
				// Split the curve in half, and render each half separately.
				PathVerb OutVerbs [6];		// Unused.
				DocCoord OutCoords [7];
				UINT32 Points;

				if ( PathUtil::SplitCurve ( 0.5f, &Coords[j-1], &Points, OutVerbs,
					 OutCoords + 1 ) )
				{
					OutCoords [0] = Coords[j-1];

					// Split curve, so recursively call ExportCurve.
					ExportCurve ( OutCoords [0], OutCoords [1],
								  OutCoords [2], OutCoords [3],
								  Tolerance );

					ExportCurve ( OutCoords [3], OutCoords [4],
								  OutCoords [5], OutCoords [6],
								  Tolerance );
				}
				else
				{
					// There's been a problem, so write a line between the start and
					// end points, and continue.
					WriteLineTo ( Coords [j-1], Coords [j+2] );
				}
			}
			else
			{
				// Render as normal.
				ExportCurve ( Coords[j-1], Coords[j],
							  Coords[j+1], Coords[j+2],
							  Tolerance );
			}

			// Move j on past the Bezier record, which contains three elements.
			j += 2;
		}

		// Default: Treat it as a line.
		else
		{
			// Treat as a line to.
			WriteLineTo ( Coords[j-1], Coords[j] ); 
		}
	}

	// End of shape.
	WriteBits  ( 0, 6 );
	ByteAlign ( );			// Align bits to byte boundary.

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WritePlaceObject ( FlashPlaceObject *pPlace )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/99
	Purpose:	It's a wrapper around the other WritePlaceObject () function, using the
				member variable mDepth to determine the "depth" in the image that the
				shape is to be rendered.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WritePlaceObject ( FlashPlaceObject *pPlace )
{
	WritePlaceObject ( pPlace, mDepth );		// Call the other WritePlaceObject function.
	mDepth ++;									// Increment the value of mDepth.
	return TRUE;								// Indicate success.
}

/********************************************************************************************

>	BOOL FlashExportDC::WritePlaceObject ( FlashPlaceObject *pPlace,
										   WORD Depth )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/99
	Purpose:	Writes a place object tag using the data contained in pPlace, and with an
				image depth of Depth.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WritePlaceObject ( FlashPlaceObject *pPlace,
									   WORD Depth )
{
	DocCoord	Position	= pPlace->GetPosition ();
	FilePos		StartPos	= 0;

	// Step 1:	Write a space in the file where the header should be written.
	WriteWord ( 0 );							// Place-holder value.
	StartPos = ExportFile->tell();				// Get the start position of the file.

	// Step 2:	Fill in the relevant details pertaining to the PlaceObject tag.

	// Flags.
	WriteBits ( 0, 2 );							// Reserved flags. Always 0
	WriteBits ( 0, 1 );							// Does the object have a name? Unused.
	WriteBits ( 0, 1 );							// Does the object have a ratio? Unused.
	WriteBits ( 0, 1 );							// Does the object have a colour transform?
	WriteBits ( 1, 1 );							// Does the object have a matrix? Always 1.
	WriteBits ( 1, 1 );							// Does the object have an ID? Always 1.
	WriteBits ( 0, 1 );							// Does the object move? Unused.

	// The depth, and ID.
	WriteWord ( Depth );						// The depth in the image.
	WriteWord ( pPlace->GetID () );				// The ID number of the object to be placed.

	// Step 2a:	Add a matrix to the PlaceObject record.
	if ( pPlace->GetDoTransform () )
		Transform ( &Position, &mLowCorner );	// Get position in Flash space.

	WriteMatrix ( Position );					// Write the matrix out.

	// Step 3:	Go back to the start of the record, and fill in the record size.
	FilePos EndPos = ExportFile->tell ();		// Get the file position.
	ExportFile->seekIn ( ( StartPos - 2 ) );	// Go back to the start of the header.
	WriteHeader ( FLASH_PLACE_OBJECT_TAG,		// Write the initial header.
				  EndPos - StartPos );
	ExportFile->seekIn ( EndPos );				// Run to the end of the file.

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteMoveTo ( DocCoord Position,
									  INT32 ShapeY,
									  UINT32 ChangeLine,
									  UINT32 ChangeFill0,
									  UINT32 ChangeFill1 )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Purpose:	Writes a Flash Move To edge record.
	Returns:	TRUE if written succesfully.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteMoveTo ( DocCoord Position,
								  UINT32 ChangeLine,
								  UINT32 ChangeFill0,
								  UINT32 ChangeFill1 )
{
	// Calculate the number of bits required for the Delta fields.
	UINT32 MoveSize = CountBits ( Max ( Absol ( Position.x ), Absol ( Position.y ) ) );

	// Declare segment header.
	WriteBits ( 0, 1 );					// Is a shape record.
	WriteBits ( 0, 1 );					// Unused by DefineShape1.
	WriteBits ( ChangeLine, 1 );		// Change Line style.
	WriteBits ( ChangeFill0, 1 );		// Change Fill0 style.
	WriteBits ( ChangeFill1, 1 );		// Change Fill1 style.

	// Details for MoveTo.
	WriteBits ( 1, 1 );					// Is a move to.
	WriteBits ( MoveSize, 5 );			// Number of bits for move value.
	
	// Set control points.
	WriteBits ( Position.x, MoveSize );
	WriteBits ( Position.y, MoveSize );

	if ( ChangeLine )
		WriteBits ( ChangeLine, 1 );	// Line style.

	if ( ChangeFill0 )
		WriteBits ( ChangeFill0, 1 );	// Fill0 style.

	if ( ChangeFill1 )					// Fill1 style.
		WriteBits ( ChangeFill1, 1 );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteLineTo ( DocCoord Start,
									  DocCoord End )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Purpose:	Writes a Flash Line To edge record.
	Returns:	TRUE if written succesfully.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteLineTo ( DocCoord Start,
								  DocCoord End )
{
	// Calculate the distance moved.
	DocCoord Delta  = End - Start;

	if ( Delta.x == 0 && Delta.y == 0 )
		return TRUE;					// Zero length line - so don't bother.

	// Calculate how many bits are needed to store the result.
	UINT32 DeltaSize = CountBits ( Max ( Absol ( Delta.x ), Absol ( Delta.y ) ) );

	// Declare segment header...
	WriteBits ( 1, 1 );					// Is an edge record.
	WriteBits ( 1, 1 );					// Is a straight edge.
	WriteBits ( ( DeltaSize - 2 ), 4 );	// Number of bits ( -2 ) per value.

	if ( Delta.x == 0 )
	{
		// Write a vertical line.
		WriteBits ( 0, 1 );				// Not a general line.
		WriteBits ( 1, 1 );				// Is a vertical line.
		WriteBits ( Delta.y, DeltaSize );
	}
	else if ( Delta.y == 0 )
	{
		// Write a vertical line.
		WriteBits ( 0, 1 );				// Not a general line.
		WriteBits ( 0, 1 );				// Is a horizontal line.
		WriteBits ( Delta.x, DeltaSize );
	}
	else
	{
		WriteBits ( 1, 1 );				// Is a general line.

		// Set control points.
		WriteBits ( Delta.x, DeltaSize );
		WriteBits ( Delta.y, DeltaSize );
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteCurveTo ( DocCoord Start,
									   DocCoord Control,
									   DocCoord End )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Purpose:	Writes a Flash Curve To edge record.
	Returns:	TRUE if written succesfully.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteCurveTo ( DocCoord Start,
								   DocCoord Control,
								   DocCoord End )
{
	// Convert to Flash co-ordinates.
	DocCoord ToControl = Control - Start;
	DocCoord ToAnchor = End - Control;

	UINT32 BezierSize = CountBits ( Max ( Absol ( ToAnchor.x ),
										Absol ( ToAnchor.y ),
										Absol ( ToControl.x ),
										Absol ( ToControl.y ) ) );

	// Declare segment header...
	WriteBits ( 1, 1 );						// Is an edge record.
	WriteBits ( 0, 1 );						// Is not a straight edge.
	WriteBits ( ( BezierSize - 2 ), 4 );	// Number of bits ( -2 ) per value.

	// Write the anchor and control points to the file.
	WriteBits ( ToControl.x, BezierSize );
	WriteBits ( ToControl.y, BezierSize );
	WriteBits ( ToAnchor.x, BezierSize );
	WriteBits ( ToAnchor.y, BezierSize );

	return TRUE;
}

/********************************************************************************************

>	DocCoord FlashExportDC::ExportCurve ( DocCoord &Start,
										  DocCoord &Control1,
										  DocCoord &Control2,
										  DocCoord &End,
										  double Tolerance )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/99
	Purpose:	Prepares a Bezier for export to a Flash file by converting it into a
				quadratic, and splitting the curve where necessary.
	Returns:	TRUE if success
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::ExportCurve ( DocCoord &Start,
								  DocCoord &Control1,
								  DocCoord &Control2,
								  DocCoord &End,
								  double Tolerance )
{
	// Calculate the point of intersection between the lines created between each
	// anchor point, and its nearest control point. This then acts as the control point
	// for the quadratic form of the Bezier.
	INT32		Length	= CalculateLongLength ( End - Start );
	DocCoord	Control;

	// Write subpixel lines out as a line.
	if ( Length < 20 )
	{
		return WriteLineTo ( Start, End );
	}
	// Otherwise calculate the control point, and render as a quadratic curve.
	else if ( CalculateIntersection ( Start, Control1, End, Control2, Control ) )
	{
		// Now compare the midpoints of the quadratic and cubic forms of the Beziers.
		// This algorithm is based on a divide and conquer method of curve rendering,
		// and can be found in Ken Joy's online computer graphics notes at:
		// http://graphics.cs.ucdavis.edu/GraphicsNotes/Graphics-Notes.html

		// The technique is to first calculate the midpoints between the start and
		// control points of a quadratic bezier, and then the midpoint between the
		// midpoints. This point lies in the middle of the curve.

		DocCoord MidStartControl = CalculateMidpoint ( Start, Control );
		DocCoord MidControlEnd = CalculateMidpoint ( Control, End );

		DocCoord QuadMidpoint = CalculateMidpoint ( MidStartControl, MidControlEnd );

		// Then process the cubic bezer using the method from PathUtil:
		DocCoord CubicMidpoint = PathUtil::PointOnCurve ( 0.5f, &Start );

		// Compare the values:
		DocCoord Difference = CubicMidpoint - QuadMidpoint;

		// Originally I was using CalculateLength here, but there were some problems
		// with it, so I've effectively inlined the code from that function, and now
		// it seems to be running properly. I suspect that the problem was that when
		// an indefinate value is returned, the INT32 would be set as a strange value,
		// which caused problems for the if... statement below.
		double Distance = sqrt ( ( Difference.x * Difference.x )
								 + ( Difference.y * Difference.y ) );

		// I've been having trouble with values of Distance where it's an infinite value,
		// which Visual C stores as -1.#IND. This should keep it under control
		if ( ( Distance < Tolerance ) && ( Distance >= 0 ) )
		{
			WriteCurveTo ( Start, Control, End );
			return TRUE;
		}
	}

	// If you get here, there's been a problem with rendering the curve. Therefore
	// split it in half, and attempt to write each half out to a flash file.
	PathVerb OutVerbs [6];		// Unused.
	DocCoord OutCoords [7];
	UINT32 Points;

	if ( PathUtil::SplitCurve ( 0.5f, &Start, &Points, OutVerbs, OutCoords + 1 ) )
	{		
		// Since I'm using references, I need to make a copy of Start for the
		// recursive call to work. Otherwise the program spins into an
		// infinite loop.
		OutCoords [0] = Start;

		// Split curve, so recursively call ExportCurve.
		ExportCurve ( OutCoords [0], OutCoords [1],
					  OutCoords [2], OutCoords [3],
					  Tolerance );

		ExportCurve ( OutCoords [3], OutCoords [4],
					  OutCoords [5], OutCoords [6],
					  Tolerance );
	}
	else
	{
		// We've got a problem! Render the curve as a straight line.
		WriteLineTo ( Start, End );
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::CalculateIntersection ( DocCoord &Start1,
												DocCoord &End1,
												DocCoord &Start2,
												DocCoord &End2
												DocCoord &Intersection )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/99
	Purpose:	Calculates the point of intersection between two lines with defined by
				a pair of points lying on each one.
	Returns:	DocCoord containing the point of intersection.
	SeeAlso:	CCreateBevelTrapezoids::CalculateIntersection

********************************************************************************************/

BOOL FlashExportDC::CalculateIntersection ( DocCoord &Start1,
											DocCoord &End1,
											DocCoord &Start2,
											DocCoord &End2,
											DocCoord &Intersection )
{
	double p, q;		// Unused values describing where the lines meet.
	BOOL Result;		// Return value.

	// Calculate the vectors describing the direction of the two lines.
	NormCoord Dir1 ( End1.x - Start1.x, End1.y - Start1.y );
	NormCoord Dir2 ( End2.x - Start2.x, End2.y - Start2.y );

	// Now normalise them, so that they are unit vectors.
	Dir1.Normalise ();
	Dir2.Normalise ();

	// Calculate the point of intersection between the lines created between each
	// anchor point, and its nearest control point.
	Result = CCreateBevelTrapezoids::CalculateIntersection ( &Start1, &Dir1,
															 &Start2, &Dir2,
															 &Intersection,
															 &p, &q );

	return Result;
}

/********************************************************************************************

>	DocCoord FlashExportDC::CalculateMidpoint ( DocCoord Point1,
												DocCoord Point2 )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/99
	Purpose:	Calculates the point of intersection between two lines with defined by
				a pair of points lying on each one.
	Returns:	DocCoord containing the midpoint.
	SeeAlso:	CCreateBevelTrapezoids::CalculateIntersection

********************************************************************************************/

DocCoord FlashExportDC::CalculateMidpoint ( DocCoord Point1,
											DocCoord Point2 )
{
	DocCoord Midpoint;

	Midpoint.x = Point1.x + ( ( Point2.x - Point1.x ) / 2 );
	Midpoint.y = Point1.y + ( ( Point2.y - Point1.y ) / 2 );

	return Midpoint;
}

/********************************************************************************************

>	BOOL FlashExportDC::ProcessFill ( FlashShapeRecord *pShape,
									  DocCoord *MoveTo )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Purpose:	Determines which kind of fill is being used, and takes appropriate action.
	Returns:	TRUE if success.
	SeeAlso:	WriteGradientFill, WriteBitmapFill

********************************************************************************************/

BOOL FlashExportDC::ProcessFill ( FlashShapeRecord *pShape,
								  DocCoord *MoveTo )
{
	BYTE Fill = pShape->GetFill ();

	// Write in the fill type.
	WriteByte ( 1 );					// The number of fills.
	WriteByte ( Fill );					// The fill style.

	// Use a switch to determine the correct fill processing routine to use.
	switch ( Fill )
	{
	case FLASH_LINEAR_FILL: case FLASH_RADIAL_FILL:
		WriteGradientFill ( pShape, MoveTo );
		break;

	case FLASH_TILED_BITMAP: case  FLASH_CLIPPED_BITMAP:
		WriteBitmapFill ( pShape, MoveTo );
		break;

	default:
		// Process as a linear fill.
		WriteColour ( pShape->GetColour ( 0 ) );
		break;
	}

	return TRUE;
}

/********************************************************************************************

>	void FlashExportDC::Transform ( DocCoord *Point,
									DocCoord *MoveTo = NULL )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/99
	Purpose:	Transforms the co-ordinates of Point from Camelot space into Flash space.
				It's really a hack to replace the transformation matrix, until I work out
				how to initialise that properly.
	Returns:	Void.
	SeeAlso:	FlashExportDC::PointOnCubic

********************************************************************************************/

void FlashExportDC::Transform ( DocCoord *Point,
								DocCoord *MoveTo )
{
	// Catch rounding errors.
	if ( ( Point->x % FLASH_SCALE ) > ( FLASH_SCALE / 2 ) )
	{
		Point->x = ( Point->x / FLASH_SCALE ) + 1;
	}
	else
	{
		Point->x = ( Point->x / FLASH_SCALE );
	}

	if ( ( Point->y % FLASH_SCALE ) > ( FLASH_SCALE / 2 ) )
	{
		Point->y = - ( Point->y / FLASH_SCALE ) - 1;
	}
	else
	{
		Point->y = - ( Point->y / FLASH_SCALE );
	}

	// If any transforms have been passed in...
	if ( MoveTo != NULL )
	{
		Point->x -= MoveTo->x;
		Point->y += MoveTo->y;
	}
}

/********************************************************************************************

>	void FlashExportDC::Transform ( DocRect *Bounds )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/99
	Purpose:	Transforms the co-ordinates of Bounds from Camelot space into Flash space.
				The rectangle is resized so that lo is (0,0).
	Returns:	Void.
	SeeAlso:	FlashExportDC::Transform ( DocCoord *Point )

********************************************************************************************/

void FlashExportDC::Transform ( DocRect *Bounds )
{
	// Calculate the new hi co-ordinates for the bounding box.
	INT32 HiX;
	INT32 HiY;
	INT32 dX = Bounds->hix - Bounds->lox;
	INT32 dY = Bounds->hiy - Bounds->loy;
	
	// Catch rounding errors.
	if ( ( dX % FLASH_SCALE ) > ( FLASH_SCALE / 2 ) )
	{
		HiX= ( dX / FLASH_SCALE ) + 1;
	}
	else
	{
		HiX= dX / FLASH_SCALE;
	}

	if ( ( dY % FLASH_SCALE ) > ( FLASH_SCALE / 2 ) )
	{
		HiY= ( dY / FLASH_SCALE ) + 1;
	}
	else
	{
		HiY= dY / FLASH_SCALE;
	}

	// And write them into the data structure.
	Bounds->lox = 0;
	Bounds->loy = 0;
	Bounds->hix = HiX;
	Bounds->hiy = HiY;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteMatrix ( DocCoord &Translate,
									  INT32 ScaleX = FLASH_FIXED_ONE,
									  INT32 ScaleY = FLASH_FIXED_ONE,
									  INT32 SkewX = 0,
									  INT32 SkewY = 0 )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/99
	Purpose:	Writes a Flash format matrix to the output file. Flash takes these values,
				and uses them to compose a matrix with this format:
				| ScaleX	SkewY		Translate.x |
				| SkewX		ScaleY		Translate.y |
	Returns:	Void.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WriteMatrix ( DocCoord &Translate,
								  INT32 ScaleX,
								  INT32 ScaleY,
								  INT32 SkewX,
								  INT32 SkewY )
{
	// Write scale field. First check whether to write scale field.
	if ( ScaleX == FLASH_FIXED_ONE && ScaleY == FLASH_FIXED_ONE )
	{
		WriteBits ( 0, 1 );					// Matrix has no scale terms.
	}
	else
	{
		// Calculate the size of the bitfield.
		UINT32 ScaleBits = CountBits ( Max ( Absol ( ScaleX ), Absol ( ScaleY ) ) );
		
		WriteBits ( 1, 1 );					// Matrix has scale.
		WriteBits ( ScaleBits, 5 );			// Size of scale value.
		WriteBits ( ScaleX, ScaleBits );	// Write scale along the X axis.
		WriteBits ( ScaleY, ScaleBits );	// Write scale along the Y axis.
	}

	// Process the rotate / skew fields of the matrix.
	if ( SkewY == 0 && SkewX == 0 )			// Check whether to write skew field.
	{
		WriteBits ( 0, 1 );					// Matrix has no rotate terms.
	}
	else
	{
		// Calculate the size of the bitfield.
		UINT32 SkewBits = CountBits ( Max ( Absol ( SkewY ), Absol ( SkewX ) ) );
		
		WriteBits ( 1, 1 );					// Matrix has rotate / skew.
		WriteBits ( SkewBits, 5 );			// Size of the bitfield.
		WriteBits ( SkewY, SkewBits );		// Value for SkewY.
		WriteBits ( SkewX, SkewBits );		// Value for SkewX.
	}

	// Define values for translation part of the matrix.

	// If no translation, don't reserve any bits.
	if ( Translate.x == 0 && Translate.y == 0 )
	{
		WriteBits ( 0, 5 );
	}
	// Otherwise write in the record.
	else
	{
		UINT32 TransBits = CountBits ( Max ( Absol ( Translate.x ), Absol ( Translate.y ) ) );
		WriteBits ( TransBits, 5 );			// Size of translation value.
		WriteBits ( Translate.x, TransBits );
		WriteBits ( Translate.y, TransBits );
	}

	ByteAlign ();

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteRotationMatrix ( DocCoord &Translate,
											  INT32 ScaleX = FLASH_FIXED_ONE,
											  INT32 ScaleY = FLASH_FIXED_ONE,
											  double SinTheta = 0,
											  double CosTheta = 1.0f )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/99
	Purpose:	This is a wrapper around the WriteMatrix method (above), except that it
				takes a cosine and sine, and assembles the appropriate rotation matrix.
	Returns:	TRUE if successful.
	SeeAlso:	FlashExportDC::WriteRotationMatrix

********************************************************************************************/

BOOL FlashExportDC::WriteRotationMatrix ( DocCoord &Translate,
										  INT32 ScaleX,
										  INT32 ScaleY,
										  double SinTheta,
										  double CosTheta )
{
	// Multiply the values together to generate the appropriate values for the matrix.
	double dScaleX = ScaleX * CosTheta;
	double dScaleY = ScaleY * CosTheta;
	double dSkewX = -ScaleY * SinTheta;
	double dSkewY =  ScaleX * SinTheta;

	// Write the values to the matrix.
	return WriteMatrix ( Translate, ( INT32 ) dScaleX, ( INT32 ) dScaleY,
						 ( INT32 ) dSkewX, ( INT32 ) dSkewY );
}

/********************************************************************************************

>	static double FlashExportDC::CalculateLength ( DocCoord Delta )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/99
	Purpose:	Calculates the length of a line between two points.
	Returns:	The length, as a double.
	SeeAlso:	FlashExportDC::WriteRotationMatrix

********************************************************************************************/

double FlashExportDC::CalculateLength ( DocCoord Delta )
{
	// Convert the co-ordinates from longs to doubles. This prevents the variables over
	// flowing.
	double xSquared	= ( ( double ) Delta.x ) * ( ( double ) Delta.x );
	double ySquared	= ( ( double ) Delta.y ) * ( ( double ) Delta.y );

	// Calculate the distance.
	double Result	= sqrt ( xSquared + ySquared );

	// And return it.
	return Result;
}

/********************************************************************************************

>	static INT32 FlashExportDC::CalculateLongLength ( DocCoord Delta )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/99
	Purpose:	Calculates the length of a line between two points.
	Returns:	The length, as an INT32.
	SeeAlso:	FlashExportDC::WriteRotationMatrix

********************************************************************************************/

INT32 FlashExportDC::CalculateLongLength ( DocCoord Delta )
{
	// Call the other CalculateLength function.
	return ( INT32 ) CalculateLength ( Delta );
}

/********************************************************************************************

>	INT32 FlashExportDC::CalculateTrigValues ( DocCoord &StartPoint,
											  DocCoord &EndPoint,
											  INT32 &Length,
											  double &SinTheta,
											  double &CosTheta )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/9/99
	Purpose:	Takes two points, and calculates the length, and the sine and cosine values
				for the angle of rotation from the x-axis.
	Returns:	Void.
	SeeAlso:	-

********************************************************************************************/

void FlashExportDC::CalculateTrigValues ( DocCoord &StartPoint,
										  DocCoord &EndPoint,
										  INT32 &Length,
										  double &SinTheta,
										  double &CosTheta )
{
	DocCoord Delta = EndPoint - StartPoint;
	Length = CalculateLongLength ( Delta );

	// Check that the length of the vector isn't zero, and thus avoid any nasty divide by
	// zero errors.
	if ( Length == 0 )
	{
		// Set the angle to be 0 radians, and load the variables with the appropriate
		// values.
		SinTheta = 0;
		CosTheta = 1;
	}
	else
	{
		// Calculate the trignometric functions using the lengths of the sides of a
		// triangle.
		SinTheta = ( double ) Delta.y / ( double )Length;
		CosTheta = ( double ) Delta.x / ( double )Length;
	}
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteGradientFill ( FlashShapeRecord *pShape,
											DocCoord *MoveTo )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Purpose:	Writes out a gradient fill to disk.
	Returns:	TRUE if success.
	SeeAlso:	ProcessFill

********************************************************************************************/

BOOL FlashExportDC::WriteGradientFill ( FlashShapeRecord *pShape,
										DocCoord *MoveTo )
{
	// This covers both circular and linear fills.
	BYTE		Colours		= pShape->GetNumberColours ();
	DocCoord	StartPoint	= pShape->GetStartPoint ();
	DocCoord	EndPoint	= pShape->GetEndPoint ();
	DocCoord	EndPoint2	= pShape->GetEndPoint2 ();
	double		CosTheta	= 0;
	double		SinTheta	= 0;
	INT32		ScaleX		= 0;

	// The first order of business is to get the start and end points of the first
	// fill vector:
	Transform ( &StartPoint, MoveTo );
	Transform ( &EndPoint, MoveTo );
	Transform ( &EndPoint2, MoveTo );

	CalculateTrigValues ( StartPoint, EndPoint, ScaleX, SinTheta, CosTheta );

	// Build the scale values to resize the colour ramp to fit onto the shape being
	// rendered. The value comes about from the following equation:
	// Scale = ( Width of Shape / Width of Ramp ) << 16.
	// The left shift converts the INT32 value into a fixed bit number, of the form
	// 16.16, which is used extensively by Flash.
	// Since the size of the Flash colour ramp is 2^15, or 1 << 15, then the pair
	// of shifts can be cancelled out, and replaced by a single times two.
	if ( pShape->GetFill () == FLASH_LINEAR_FILL )
	{
		ScaleX *= 2;
		StartPoint = CalculateMidpoint ( StartPoint, EndPoint );

		// Use a basic scaling matrix.
		WriteRotationMatrix ( StartPoint, ScaleX, ScaleX, SinTheta, CosTheta );
	}

	// Again a piece of inexplicable maths, and again it is a simplification of
	// Length / Size of colour ramp, which is being converted to a 16 bit fixed
	// value.
	else
	{
		INT32 ScaleY = CalculateLongLength ( EndPoint2 - StartPoint );

		ScaleX *= 4;
		ScaleY *= 4;

		// Check to see whether the lengths of the vectors are identical. If they
		// are, then the fill is a circular fill by another name, and we can make
		// our resulting file smaller.
		if ( ScaleX != ScaleY )
		{
			// Write the values out into a matrix.
			WriteRotationMatrix ( StartPoint, ScaleX, ScaleY, SinTheta, CosTheta );
		}
		else
		{
			// It's a circular fill, so use the circular fill matrix.
			WriteMatrix ( StartPoint, ScaleX, ScaleX );
		}
	}

	// Write out the colour record.
	WriteByte ( Colours );							// Number of colours in the gradient.

	for ( BYTE i = 0; i < Colours; i++ )
	{
		WriteByte ( pShape->GetRatio (i) );			// Ratio for this colour.
		WriteColour ( pShape->GetColour (i) );		// The actual colour value.
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteBitmapFill ( FlashShapeRecord *pShape,
										  DocCoord *MoveTo )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Purpose:	Writes out a bitmap fill to disk.
	Returns:	TRUE if success.
	SeeAlso:	ProcessFill

********************************************************************************************/

BOOL FlashExportDC::WriteBitmapFill ( FlashShapeRecord *pShape,
									  DocCoord *MoveTo )
{
	// Step 1:	Extract the start and end points of the fill from the shape record, and
	//			transform them into Flash co-ordinates.
	DocCoord	Start		= pShape->GetStartPoint ();		// Low corner of the bitmap.
	DocCoord	End1		= pShape->GetEndPoint ();		// Max X corner.
	DocCoord	End2		= pShape->GetEndPoint2 ();		// Max Y corner.

	INT32		BMPWidth	= pShape->GetBitmapWidth ();	// The width of the bitmap.
	INT32		BMPHeight	= pShape->GetBitmapHeight ();	// The height of the bitmap.

	// Catch BMPWidth or BMPHeight set to zero. This shouldn't happen...
	if ( BMPWidth == 0 )
		BMPWidth = 1;

	if ( BMPHeight == 0 )
		BMPHeight = 1;

	Transform ( &Start, MoveTo );							// Transform low corner.
	Transform ( &End1, MoveTo );							// Transform max x, min y.
	Transform ( &End2, MoveTo );							// Transform min x, max y.

	// Step 2:	Calculate the normalised dot product of the two vectors, and this forms
	//			the cosine of their internal angle.
	DocCoord	WidthVect	= End1 - Start;
	DocCoord	HeightVect	= End2 - Start;
	double		Width		= CalculateLength ( WidthVect );
	double		Height		= CalculateLength ( HeightVect );

	// The dot product calculation.
	double		DotProduct	= ( double ) ( ( WidthVect.x * HeightVect.x ) +
							  ( HeightVect.y * WidthVect.y ) ) / ( Height * Width );
	double		DotSine		= sqrt ( 1 - ( DotProduct * DotProduct ) );

	// Set DotSine to be something if it equals zero to prevent zero divide errors.
	if ( DotSine == 0 )
	{
		DotSine = 0.01f;
	}

	// The cross product, when applied to a 3D vector, gives the direction of this
	// vector. In the case of a 2D vector, only one of the three elements evaluates
	// to a non-zero term, and we use this to determine the sign of DotSine.
	if ( ( ( WidthVect.x * HeightVect.y ) - ( HeightVect.x * WidthVect.y ) ) > 0 )
	{
		DotSine = - DotSine;
	}

	// Step 3:	Calculate the angle of rotation.
	double		WidthCosine	= ( double ) ( WidthVect.x * FLASH_FIXED_ONE ) / Width;
	double		WidthSine	= ( double ) ( WidthVect.y * FLASH_FIXED_ONE ) / Width;

	// Step 4:	Set all the values, and write them to the matrix.
	double		ScaleX		= ( Width * FLASH_FIXED_ONE ) / BMPWidth;
	double		ScaleY		= ( DotSine * Height * FLASH_FIXED_ONE ) / BMPHeight;
	double		SkewX		= - ( DotProduct / DotSine ) * ScaleY;

	//  Step 5:	Write the details of the bitmap fill into the file.
	WriteWord ( FLASH_FIRST_ID + pShape->GetBitmapID () );				// Bitmap ID.

	WriteComplexRotation ( End2, ( INT32 ) ScaleX, ( INT32 ) ScaleY,		// Bitmap matrix.
						   ( INT32 ) SkewX, 0, End2,
						   ( INT32 ) WidthSine, ( INT32 ) WidthCosine );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WritePNG ( OILBitmap	*pBitmap,
								   WORD			BitmapID,
								   UINT32			Alpha,
								   BOOL			IsContoned )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/99
	Purpose:	Writes a DIB into a Flash file as a PNG.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashExportDC::WritePNG ( OILBitmap	*pBitmap,
							   WORD			BitmapID,
							   UINT32			Alpha,
							   BOOL			IsContoned )
{
	// Local variable definitions.
	UINT32			BPP			= pBitmap->GetBPP ();

	// Find out the colour depths for the bitmap, and store Flash code.

	// Use a switch to process the values that need to be written to the file itself.
	if ( BPP == 8 || IsContoned)
	{
		return Write8BitBitmap ( pBitmap, BitmapID, Alpha, IsContoned );
	}
	else
	{
		return Write32BitBitmap ( pBitmap, BitmapID, Alpha );
	}
}

/********************************************************************************************

>	BOOL FlashExportDC::Write32BitBitmap ( OILBitmap	*pBitmap,
										   WORD			BitmapID,
										   UINT32			Alpha )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		pBitmap - A pointer a true colour kernel bitmap.
	Returns:	TRUE if success, FALSE otherwise.
	Purpose:	Writes out a Zlib compressed bitmap record to the Flash file.

********************************************************************************************/

BOOL FlashExportDC::Write32BitBitmap ( OILBitmap	*pBitmap,
									   WORD			BitmapID,
									   UINT32			Alpha )
{
	UINT32		Width		= pBitmap->GetWidth ();
	UINT32		Height		= pBitmap->GetHeight ();
	FilePos		StartPos	= 0;
	BOOL		Transparent	= FALSE;

	// Currently the alpha channel support for bitmaps is disabled. To re-enable it,
	// replace FLASH_BITS_TAG with FLASH_BITS2_TAG. The reasons for this are described
	// in swfexpdc.h, method FlashRenderRegion::ProcessTransp ().
	StartPos = WriteHeader ( FLASH_BITS_TAG, FLASH_MAX_SHORT, BitmapID + FLASH_FIRST_ID );
	WriteByte ( 5 );

	// Height and width of the bitmap image.
	WriteWord ( ( WORD ) Width );
	WriteWord ( ( WORD ) Height );

	// First initialise the file...
	ExportFile->InitCompression ( TRUE );
	ExportFile->SetCompression	( TRUE );

	// Browse through the image, extract the pixel colour values as 32 bit values, and
	// store them in the bitmap buffer.
	for ( INT32 y = ( Height - 1 ) ; y >= 0; y-- )
	{
		for ( UINT32 x = 0; x < Width; x++ )
		{
			// Get the colour. It's necessary to create the value for the alpha channel as a
			// UINT32 first before scaling it down to a BYTE to prevent rendering artefacts due
			// to overflows. (I do the same thing with the shadow code.)
			Pixel32bpp	Colour		= pBitmap->ReadPixel32bpp ( x, y );
			BYTE		BitmapAlpha	= ~static_cast <BYTE>	( Colour.Alpha );
			BYTE		ByteAlpha	= static_cast <BYTE>	( Alpha );
			UINT32		ActualAlpha	= ( BitmapAlpha * ByteAlpha ) / 255;

			// Check the written byte. If it comes out as 255 for every pixel then the bitmap
			// is opaque, and it can be stored as a non-transparent bitmap record. The Flash
			// player has trouble with too many overlapping transparencies, so keeping the
			// number used down will result in better image quality.
			if ( ActualAlpha != 255 )
			{
				Transparent = TRUE;
			}

			// And store it. The first line blends any transparency set on the bitmap, with
			// its own alpha channel.
			WriteByte ( static_cast <BYTE> ( ActualAlpha ) );
			WriteByte ( Colour.Red );
			WriteByte ( Colour.Green );
			WriteByte ( Colour.Blue );
		}
	}

	// Finally close down GZipFile.
	ExportFile->SetCompression ( FALSE );

	// If the file contains a transparent bitmap, go back and fill in the header.
	if ( Transparent )
	{
		// Set up the endpoint of the file.
		FilePos EndPos = ExportFile->tell ();

		// Go back to the beginning of the header.
		ExportFile->seekIn ( ( StartPos - 6 ) );

		// The header for a transparent bitmap record. This needs to be done manually to
		// prevent a defualt value for the size being written in.
		WriteHeader ( FLASH_BITS2_TAG, FLASH_MAX_SHORT );

		// Return to the end of the file.
		ExportFile->seekIn ( EndPos );
	}

	// Go back, and fill in the number of bytes written.
	WriteTagSize ( StartPos );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::Write8BitBitmap ( OILBitmap	*pBitmap,
										  WORD		BitmapID,
										  BOOL		IsContoned,
										  UINT32		Alpha )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/99
	Inputs:		pBitmap			- A pointer a 256 colour kernel bitmap.
				BitmapID		- The ID for this bitmap.
				IsContoned		- Is the bitmap contoned.
				Transparency	- The transparency value for the bitmap.
	Returns:	TRUE if success, FALSE otherwise.
	Purpose:	Writes out a Zlib compressed bitmap record to the Flash file.

********************************************************************************************/

BOOL FlashExportDC::Write8BitBitmap ( OILBitmap	*pBitmap,
									  WORD		BitmapID,
									  UINT32		Alpha,
									  BOOL		IsContoned )
{
	// Set up the local variables.
	UINT32	NumColours	= pBitmap->GetNumPaletteEntries ();			// Number of colours.
	BOOL	Transparent	= Alpha != 255;
	FilePos	StartPos	= Write8BitHeader ( pBitmap, BitmapID, NumColours - 1, Transparent );
	BYTE	ByteAlpha	= static_cast <BYTE> ( Alpha );

	// And then write out the palette entries themselves.
	if ( IsContoned )
	{
		for ( UINT32 i = 0; i < NumColours; i++ )
		{
			// Write the colour out as a contoned palette entry.
			WriteColour ( pBitmap->GetContonePaletteEntry ( i ) );

			// Only write out the alpha channel if the bitmap is transparent.
			if ( Transparent )
			{
				WriteByte	( ByteAlpha );
			}
		}
	}
	else
	{
		LPRGBQUAD	pPalette	= pBitmap->GetPaletteForBitmap ();	// Pointer to palette.

		for ( UINT32 i = 0; i < NumColours; i++ )
		{
			// I'm writing them out individually to ensure that they're saved to the file
			// in the correct bit and byte order.
			WriteByte ( pPalette[i].rgbRed );
			WriteByte ( pPalette[i].rgbGreen );
			WriteByte ( pPalette[i].rgbBlue );

			// Only write out the alpha channel if the bitmap is transparent.
			if ( Transparent )
			{
				WriteByte	( ByteAlpha );
			}
		}
	}

	// Write out the body of the bitmap itself.
	return Write8BitBody ( pBitmap, StartPos );
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteTransparentBitmap ( OILBitmap	*pBitmap,
												 WORD		BitmapID,
												 DocColour	Colour )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/12/99
	Inputs:		pBitmap		- A pointer a 256 colour OIL bitmap.
				BitmapID	- The ID number for the bitmap.
				Colour		- The colour of the transparency.
	Returns:	TRUE if success, FALSE otherwise.
	Purpose:	Writes out a Zlib compressed bitmap record to the Flash file. This contains
				a transparent Camelot fill.

********************************************************************************************/

BOOL FlashExportDC::WriteTransparentBitmap ( OILBitmap	*pBitmap,
											 WORD		BitmapID,
											 DocColour	Colour )
{
	// Generate the header for the bitmap tag.
	FilePos	StartPos = Write8BitHeader ( pBitmap, BitmapID, 255, TRUE );

	// Write out the palette.
	for ( UINT32 i = 0; i < 256; i++ )
	{
		// I'm writing them out individually to ensure that they're saved to the file
		// in the correct bit and byte order.
		WriteColour	( Colour );
		WriteByte	( ~static_cast <BYTE> ( i ) );
	}

	// Write out the body of the bitmap itself.
	return Write8BitBody ( pBitmap, StartPos );
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteShadowBitmap ( OILBitmap	*pBitmap,
											WORD		BitmapID,
											DocColour	Colour,
											UINT32		Darkness )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/00
	Inputs:		pBitmap		- A pointer a 256 colour OIL bitmap.
				BitmapID	- The ID number for the bitmap.
				Colour		- The colour of the transparency.
				Darkenss	- The darkness of the shadow.
	Returns:	TRUE if success, FALSE otherwise.
	Purpose:	Writes out a Zlib compressed bitmap record to the Flash file. This contains
				a transparent bitmap representing the shadow.

********************************************************************************************/

BOOL FlashExportDC::WriteShadowBitmap ( OILBitmap	*pBitmap,
										WORD		BitmapID,
										DocColour	Colour,
										UINT32		Darkness )
{
	// Generate the header for the bitmap tag.
	FilePos	StartPos = Write8BitHeader ( pBitmap, BitmapID, 255, TRUE );

	// Write out the palette.
	for ( UINT32 i = 0; i < 256; i++ )
	{
		// Calculate the darkness of the shadow.
		BYTE NotI		= ~static_cast <BYTE> ( i );
		BYTE NotDark	= ~static_cast <BYTE> ( Darkness );
		UINT32 Alpha		= ( NotI  * NotDark ) / 255;

		// I'm writing them out individually to ensure that they're saved to the file
		// in the correct bit and byte order.
		WriteColour	( Colour );
		WriteByte	( static_cast <BYTE> ( Alpha ) );
	}

	// Write out the body of the bitmap itself.
	return Write8BitBody ( pBitmap, StartPos );
}

/********************************************************************************************

>	BOOL FlashExportDC::Write8BitHeader ( OILBitmap	*pBitmap,
										  WORD		BitmapID,
										  UINT32		NumColours,
										  BOOL		Transparent )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/00
	Inputs:		pBitmap		- A pointer a 256 colour OIL bitmap.
				BitmapID	- The ID number for the bitmap.
	Returns:	StartPos	- The position of the start of this Flash tag.
	Purpose:	Writes out the header for a 256 colour bitmap.

********************************************************************************************/

FilePos FlashExportDC::Write8BitHeader ( OILBitmap	*pBitmap,
										 WORD		BitmapID,
										 UINT32		NumColours,
										 BOOL		Transparent )
{
	// Write out a DefineBitsLossless2 header.
	FilePos StartPos = 0;
	
	// Is it a transparent bitmap?
	if ( Transparent )
	{
		// Yes, write out a transparent bitmap header.
		StartPos = WriteHeader ( FLASH_BITS2_TAG, FLASH_MAX_SHORT,
								 BitmapID + FLASH_FIRST_ID );
	}

	else
	{
		// No, write out an opaque bitmap header.
		StartPos = WriteHeader ( FLASH_BITS_TAG, FLASH_MAX_SHORT,
								 BitmapID + FLASH_FIRST_ID );
	}

	// Colour depth. (3 = 8 bits per pixel.)
	WriteByte ( 3 );

	// Height and width of the bitmap image.
	WriteWord ( static_cast <WORD> ( pBitmap->GetWidth () ) );
	WriteWord ( static_cast <WORD> ( pBitmap->GetHeight () ) );

	// Write the number of colours to the file.
	WriteByte ( static_cast <BYTE> ( NumColours ) );

	// First initialise the file...
	ExportFile->InitCompression	( TRUE );
	ExportFile->SetCompression	( TRUE );

	// Return the start position of the file.
	return StartPos;
}

/********************************************************************************************

>	BOOL FlashExportDC::Write8BitBody ( OILBitmap	*pBitmap,
										FilePos		StartPos )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/00
	Inputs:		pBitmap		- A pointer a 256 colour OIL bitmap.
			:	StartPos	- The position of the start of this Flash tag.
	Returns:	TRUE if success, FALSE otherwise.
	Purpose:	Writes out the header for a 256 colour bitmap.

********************************************************************************************/

BOOL FlashExportDC::Write8BitBody ( OILBitmap	*pBitmap,
									FilePos		StartPos )
{
	UINT32	Width	= pBitmap->GetWidth ();					// The bitmap's width.
	UINT32	Height	= pBitmap->GetHeight ();				// The bitmap's height.
	INT32		Offset	= ( 4 - ( Width % 4 ) ) % 4;			// To align the bitmap.

	// Browse through the image, extract the pixel colour values as 32 bit values, and
	// store them in the bitmap buffer.
	for ( INT32 y = ( Height - 1 ) ; y >= 0; y-- )
	{
		for ( UINT32 x = 0; x < Width; x++ )
		{
			// Write the colour out.
			WriteByte	( static_cast < BYTE > ( pBitmap->ReturnPaletteIndexUsed ( x, y ) ) );
		}

		// Flash bitmaps have to be aligned to a 32 bit boundary along their
		// width. This adds in the padding.
		for ( INT32 i = 0; i < Offset; i++ )
		{
			WriteByte ( 0 );
		}
	}

	// Finally close down GZipFile.
	ExportFile->SetCompression ( FALSE );

	// Go back, and fill in the number of bytes written.
	WriteTagSize ( StartPos );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteDefineFont ( FlashFontRecord *pRecord )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		pRecord - A pointer to the head of a FlashFontRecord linked list.
	Returns:	TRUE if success, FALSE otherwise.
	Purpose:	Writes out a string DefineFont records.

********************************************************************************************/

BOOL FlashExportDC::WriteDefineFont ( FlashFontRecord *pRecord )
{
	// Declare and define all local variables.
	WCHAR		*pChars		= pRecord->GetGlyphs ();
	UINT32		i			= 0;
	UINT32		j			= 0;
	FilePos		StartPos	= 0;
	FilePos		OffsetPos	= 0;
	FilePos		Counter		= 0;
	WORD		ID			= pRecord->GetFontID () + FlashPlaceObject::GetBitmapCount ()
							  + FLASH_FIRST_ID;

	// Write the Flash file header.
	StartPos = WriteHeader ( FLASH_FONT_TAG, FLASH_MAX_SHORT, ID );
	
	// Write the font data segment.

	// Set offset return point.
	OffsetPos = ExportFile->tell();

	// Offset table

	// There are as many offsets as there are glyphs, therefore I'm counting the
	// number of entries in the glyph string. Since it's terminated by a zero, it's
	// easy to spot the end.
	while ( pChars [i] )
	{
		WriteWord ( 0 );
		i++;
	}

	// Set up an index variable, and a pointer to the path handles in the FlashTextRecord.

	while ( pChars [j] )
	{
		// Scoped variables.
		FilePos		CurrentPos	= 0;
		Path		**pPath		= pRecord->GetPaths ();
		DocCoord	*Coords		= NULL;

		// Jump back to offset table, and fill in offset.
		CurrentPos = ExportFile->tell ();

		ExportFile->seekIn ( OffsetPos + Counter);			// Go back to the offset table.
		WriteWord ( ( WORD ) ( CurrentPos - OffsetPos ) );	// Write in the appropriate size.
		ExportFile->seekIn ( CurrentPos );					// Return to the end of the file.

		// Write out the paths, ignoring NULL paths.
		if ( pPath [j] != NULL )
		{
			Coords = pPath [j]->GetCoordArray ();
		}

		// Write a font style definition.
		WriteBits ( 1, 4 );		// One bit for Fill...
		WriteBits ( 0, 4 );		// ... and nothing for Line Style.

		// Only write a path out if there's a path stored with the glyph. Otherwise do an
		// empty edge record (which is how Flash deals with spaces).
		if ( Coords != NULL )
		{
			// Write the edge record.
			WriteEdgeRecord ( Coords, NULL, pPath [j]->GetVerbArray(),
							  pPath [j]->GetNumCoords (), 0, 1, 0);
		}
		else
		{
			// End of shape.
			WriteBits  ( 0, 6 );
			ByteAlign ( );			// Align bits to byte boundary.
		}

		j++;						// Increment the character that's being pointed to.
		Counter += 2;				// Increment the counter to the offsets by 2 bytes.
	}

	// Write in the size of the DefineFontInfo record after everything's been done.
	WriteTagSize ( StartPos );

	// Call WriteDefineFontInfo directly, to ensure that the records are in order.
	return WriteDefineFontInfo ( pRecord );
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteDefineFontInfo ( FlashFontRecord *pRecord )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		pRecord - A pointer to the head of a FlashFontRecord linked list.
	Returns:	TRUE if success, FALSE otherwise.
	Purpose:	Writes out a string DefineFontInfo records.

********************************************************************************************/

BOOL FlashExportDC::WriteDefineFontInfo ( FlashFontRecord *pRecord )
{
	WORD		Typeface	= pRecord->GetTypeface ();
	WORD		FontID		= pRecord->GetFontID ();
	WORD		ID			= FontID + FlashPlaceObject::GetBitmapCount () + FLASH_FIRST_ID;
	FilePos		StartPos	= 0;
	TCHAR		*pFontName	= NULL;
	String_64	FontName;

	// Now add the DefineFontInfo structure that is needed to glue this thing together.
	StartPos = WriteHeader ( FLASH_FONT_INFO_TAG, FLASH_MAX_SHORT, ID );

	// Check that the font handle is valid.
	if ( !( FONTMANAGER->IsFontValid ( Typeface ) ) )
	{ 
		// Force the program to use Times New Roman.
		Typeface = DEFAULTHANDLE;
	}

	// Extract the font name from the font manager class.
	FONTMANAGER->GetFontName ( Typeface, FontName );

	pFontName = ( TCHAR* ) FontName;
	WriteByte ( ( BYTE ) FontName.Length () );

	// Write the font name to the file.
	while ( *pFontName )
	{
		WriteByte ( ( BYTE ) *pFontName );
		pFontName ++;
	}

	// Write the definition flags to the file.
	WriteBits ( 0, 2 );							// Reserved flags.
	WriteBits ( 0, 1 );							// Unicode format.
	WriteBits ( 0, 1 );							// Shift-JIS character codes.
	WriteBits ( 0, 1 );							// ANSI character codes.
	WriteBits ( pRecord->GetIsItalic (), 1 );	// Italic.
	WriteBits ( pRecord->GetIsBold (), 1 );		// Bold.
	WriteBits ( 0, 1 );							// Using 8-bit character codes.

	// Write a list of the characters used. These must be in the same order as the paths
	// that are written in the DefineFont record.
	WCHAR *pGlyphs = pRecord->GetGlyphs ();

	// Find the size of the array.
	UINT32 Size = wcslen ( pGlyphs );

	// Create an array to store the characters in.
	char ASCIIChars [FLASH_TEXT_ARRAY_SIZE];

	// Convert the unicode characters into ASCII characters.
	if ( WideCharToMultiByte ( CP_ACP, 0, pGlyphs, Size, ASCIIChars, FLASH_TEXT_ARRAY_SIZE,
							   NULL, NULL ) )
	{
		// Write the string out to disk.
		Write ( ASCIIChars, Size );
	}

	WriteTagSize ( StartPos );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteText ( FlashTextRecord *pText )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/99
	Inputs:		pText - A pointer to a FlashTextRecord containing the string.
	Returns:	TRUE if success, FALSE otherwise.
	Purpose:	Writes the contents of the FlashTextRecord into a file in the correct
				format for a Flash file.

********************************************************************************************/

BOOL FlashExportDC::WriteText ( FlashTextRecord *pText )
{
	// Calculate the text ID.
	WORD		TextID		= FLASH_FIRST_ID + FlashPlaceObject::GetBitmapCount () +
							  FlashPlaceObject::GetFontCount () + pText->GetTextID ();
	FilePos		StartPos	= 0;
	DocRect		Bounds		= pText->GetBounds ();

	// Now add the DefineText structure that is needed to glue this thing together.
	StartPos = WriteHeader ( FLASH_TEXT_TAG, FLASH_MAX_SHORT, TextID );

	// Do the Text export here.
	Transform ( &Bounds );

	// Write the bounding box and matrix.
	WriteBoundingBox ( Bounds );			// Write the bounding box.

	WriteTextMatrix ( pText->GetScaleX (),	// Create and write the text matrix.
					  pText->GetScaleY (),
					  pText->GetSkewX (),
					  pText->GetSkewY (),
					  pText->GetAspect () );

	WriteTextRecord ( pText );

	WriteTagSize ( StartPos );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteTextMatrix ( INT32 ScaleX,
										  INT32 ScaleY,
										  INT32 SkewX,
										  INT32 SkewY,
										  FIXED16 Aspect )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/99
	Inputs:		Aspect - The font's aspect ratio.
				ScaleX, ScaleY, SkewY, SkewX - Components of the rotation matrix.
	Returns:	TRUE if success.
	Purpose:	Creates and writes the correct matrix for the current text string to the
				file.

********************************************************************************************/

BOOL FlashExportDC::WriteTextMatrix ( INT32 ScaleX,
									  INT32 ScaleY,
									  INT32 SkewX,
									  INT32 SkewY,
									  const FIXED16 &Aspect )
{
	// Convert the aspect to a double for extra accuracy. (52 bits, as opposed to 32 bits.)
	double		dAspect		= ( ( double ) Aspect.GetRawLong () ) / FLASH_FIXED_ONE;

	// Multiply in the aspect ratio value as the new variables are declared.
	double		dScaleX		= ( ( double ) ScaleX * dAspect );
	double		dSkewY		= ( ( double ) SkewY * dAspect );

	// And set the position.
	DocCoord	Position	( 0, 0 );

	// Finally, write the matrix to the file.
	return WriteMatrix ( Position, ( INT32 ) dScaleX, ScaleY, SkewX, ( INT32 ) dSkewY );
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteTextRecord ( FlashTextRecord *pText )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/99
	Inputs:		pText - A pointer to the current FlashTextRecord.
	Returns:	TRUE if success.
	Purpose:	Writes the DefineText's text record into the file.

********************************************************************************************/

BOOL FlashExportDC::WriteTextRecord ( FlashTextRecord *pText )
{
	// Count the number of bits in the offsets and glyph indicies, and store these.
	INT32			*Text		= pText->GetString ();		// Pointer to the text string.
	INT32		*Advances	= pText->GetAdvances ();	// The advance values for the font.
	WORD		*Styles		= pText->GetFontStyles ();	// An array of styles.
	WORD		*Sizes		= pText->GetSizes ();		// The sizes for the text items.
	FlashColour	*Colours	= pText->GetColours ();		// The colours of the letters.
	INT8		*Offsets	= pText->GetOffsets ();		// The Y offsets.
	INT32			MaxText		= 0;						// The largest text identifier.
	UINT32		TextBits	= 0;						// Bits required for text ID.
	INT32		MaxAdvance	= 0;						// The largest text advance value.
	UINT32		AdvanceBits	= 0;						// Bits required for advance.
	FilePos		StartGlyphs	= 0;
	BYTE		GlyphCount	= 0;

	// Calculate the aspect ratio.
	double		Aspect		= ( ( double ) ( pText->GetAspect () ).GetRawLong() )
								/ FLASH_FIXED_ONE;

	// Count the number of bits required to store the text, and the Advances.
	for ( INT32 i = 0; i < pText->GetCount (); i++ )
	{
		// Adjust the advances so that they take the aspect ratio into account.
		Advances [i] = ( INT32 ) ( ( double ) Advances [i] / Aspect );

		// Compare the raw text code with the maximum value encountered so far.
		if ( Text [i] > MaxText )
			MaxText = Text [i];

		// And also compare the Advance values.
		if ( Advances [i] > MaxAdvance )
			MaxAdvance = Advances [i];
	}

	TextBits	= CountBits ( MaxText , FALSE );		// Calculate the number of text bits.
	AdvanceBits	= CountBits ( MaxAdvance, TRUE );		// And for the Advances.

	WriteByte ( ( BYTE ) TextBits );					// Now write the text value to the file.
	WriteByte ( ( BYTE ) AdvanceBits );					// And then the Advance values.

	// This for loop does most of the work in this loop. The algorithm is:
	// - Write a define text style record for the current character style.
	// - Write a glyph record header.
	// - Cycle through the fonts, whilst the style remains identical, and
	//	 write them to the file.
	// - If a new style is required, 
	for ( INT32 j = 0; j < pText->GetCount (); j++ )
	{
		// Determine whether either the size, font style, or colour have
		// been changed. The code relies on the fact that the binary
		// operators return 1 iff true, otherwise they return 0. Thus
		// HasFont and HasColour have values of either 1 or 0.
		UINT32 HasFont	= 1;
		UINT32 HasColour	= 1;
		UINT32 HasYOffset	= 1;

		// Ignore any offsets that are 0.
		if ( Offsets [j] == 0 )
			HasYOffset = 0;

		// Determine whether or not the text style has changed.
		if ( j > 0 )
		{
			HasFont		= ( Styles [j] != Styles [j-1] ) ||
						  ( Sizes [j] != Sizes [j-1] );

			HasColour	= ( Colours [j].Red != Colours [j-1].Red ) ||
						  ( Colours [j].Green != Colours [j-1].Green ) ||
						  ( Colours [j].Blue != Colours [j-1].Blue ) ||
						  ( Colours [j].Alpha != Colours [j-1].Alpha );

			HasYOffset	= ( Offsets [j] != Offsets [j-1] );
		}

		// If a change has been made, write a record to encode the changes.
		if ( HasFont || HasColour || HasYOffset )
		{
			if ( j > 0 )
			{
				WriteGlyphHeader ( StartGlyphs, GlyphCount );
				GlyphCount = 0;
			}

			// Write a new Glyph Type.
			StartGlyphs = WriteChangeText ( Colours [j], Sizes [j], Styles [j], Offsets [j],
											HasColour, HasFont, HasYOffset );
		}

		// Write out the font.
		WriteBits ( Text [j], TextBits );

		// Write out the advances, correcting for the current aspect ratio.
		// WriteBits ( ( INT32 ) ( ( double ) Advances [j] * dInvAsp ), AdvanceBits );
		WriteBits ( Advances [j], AdvanceBits );

		GlyphCount ++;
	}

	// Complete the file record.
	WriteGlyphHeader ( StartGlyphs, GlyphCount );
	WriteByte ( 0 );

	return TRUE;
}

/********************************************************************************************

>	FilePos FlashExportDC::WriteChangeText ( const FlashColour &Colour,
											 WORD Size,
											 WORD Style,
											 INT8 YOffset,
											 UINT32 HasColour,
											 UINT32 HasFont,
											 UINT32 HasYOffset )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/99
	Inputs:		FlashColour	- The current colour of the text.
				Size		- The size of the typeface of the current character.
				Style		- The typeface being used.
				YOffset		- The current y offset value.
				HasColour	- Does the font have a colour value set?
				HasFont		- Has the font style changed, or has it been resized?
				HasYOffset	- Has the offset of the font been changed?
	Returns:	filepos		- The position of the start of the new glyph record.
	Purpose:	Writes a record to the file to change the font style being used.

********************************************************************************************/

FilePos FlashExportDC::WriteChangeText ( const FlashColour &TextColour,
										 WORD Size,
										 WORD Style,
										 INT8 YOffset,
										 UINT32 HasColour,
										 UINT32 HasFont,
										 UINT32 HasYOffset )
{
	FilePos NewRecord = 0;				// Return value.

	// Write a new Glyph Type.
	WriteBits ( 1, 1 );					// Is a TextRecord Type 1.
	WriteBits ( 0, 3 );					// Reserved flags.
	WriteBits ( HasFont, 1 );			// Is the font style changed?
	WriteBits ( HasColour, 1 );			// Has the colour changed?
	WriteBits ( HasYOffset, 1 );		// Has the font got a Y offset?
	WriteBits ( 0, 1 );					// Has the font got an X offset?

	// Write the typeface style.
	if ( HasFont )
	{
		WriteWord ( Style + FlashPlaceObject::GetBitmapCount ()
					+ FLASH_FIRST_ID );
	}

	// Write the font's colour.
	if ( HasColour )
	{
		WriteColour ( TextColour );
	}

	// Write the current Y offset value.
	if ( HasYOffset )
	{
		WriteWord ( YOffset );
	}

	// Write the font's size.
	if ( HasFont )
	{
		WriteWord ( ( Size ) );
	}

	NewRecord = ExportFile->tell ();	// Get the start of the next record.
	WriteByte ( 0 );					// Placeholder value.

	return NewRecord;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteGlyphHeader ( FilePos Start,
										   BYTE GlyphCount )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/99
				Start		- The start of the glyph record.
	Inputs:		GlyphCount	- The number of glyphs used.
	Returns:	TRUE if success.
	Purpose:	Fills in the number of glyphs used.

********************************************************************************************/

BOOL FlashExportDC::WriteGlyphHeader ( FilePos Start,
									   BYTE GlyphCount )
{
	FilePos EndOfFile;

	ByteAlign ();					// Align the bits to nearest byte boundary.

	EndOfFile = ExportFile->tell ();
	// Go back to the start of the last glyph record.
	ExportFile->seekIn ( Start );

	WriteBits ( 0, 1 );				// Is a TextRecord Type 0.
	WriteBits ( GlyphCount, 7 );	// The number of glyphs used.

	ExportFile->seekIn ( EndOfFile );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteTagSize ( FilePos StartPos )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/99
	Inputs:		StartPos	- The file position at the start of the tag.
	Returns:	TRUE if success.
	Purpose:	Fills in the number of byte used by the tag.

********************************************************************************************/

BOOL FlashExportDC::WriteTagSize ( FilePos StartPos )
{
	// Go back to the start of the tag, and write in the size of the Flash record.
	FilePos EndPos = ExportFile->tell ();		// Get the current position in the file.

	ExportFile->seekIn ( ( StartPos - 4 ) );	// Go back to the start of the tag's header.
	WriteLong ( (INT32)EndPos-(INT32)StartPos );	// Write in the appropriate size value.
	ExportFile->seekIn ( EndPos );				// Run to the end of the file.

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::BuildComplexRotation ( DocCoord &Position,
											   INT32 &ScaleX,
											   INT32 &ScaleY,
											   INT32 &SkewX,
											   INT32 &SkewY,
											   const DocCoord &Centre,
											   INT32 SinTheta,
											   INT32 CosTheta )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/99
	Inputs:		Position	- The x and y translation values in the original matrix.
				ScaleX		- The scale value for the x axis.
				ScaleY		- The scale value for the y axis.
				SkewY		- First skew value.
				SkewX		- Second skew value.
				( The above correspond to the basic Flash matrix values. )
				Centre		- The centre of the rotation.
				SinTheta	- The sine of the rotation.
				CosTheta	- The cosine of the rotation.
	Returns:	TRUE if success.
	Purpose:	Creates a matrix which combines the the transformation in the passed
				in matrix, with the rotation described by CosTheta and SinTheta around
				the point Center. The inputted values (ScaleX, ScaleY, SkewY, SkewX,
				Position) are then reassigned the new matrix values, and since
				they're passed in as references, they return these to the calling
				function.

********************************************************************************************/

void FlashExportDC::BuildComplexRotation ( DocCoord &Position,
										   INT32 &ScaleX,
										   INT32 &ScaleY,
										   INT32 &SkewX,
										   INT32 &SkewY,
										   const DocCoord &Centre,
										   INT32 SinTheta,
										   INT32 CosTheta )
{
	// Reassign the variables to a series of doubles (as usual for extra precision),
	// and multiply the two matrices together in the process. The composition of this
	// new matrix is:
	//
	// | dScaleX	dSkewX		dPositionX |
	// | dSkewY		dScaleY		dPositionY |
	// | 0			0			1		   |
	//
	// dShiftX and dShiftY are the values from the rotation matrix. Basically, you
	// shift the image so that it's centred around (0, 0), rotate it, and put it
	// back where it was. Effectively this involves compositing three matrices, and
	// the net result is identical to the usual rotation matrix, with the exception
	// of the translation values.
	double dShiftX		= ( ( ( ( double ) - CosTheta * ( double ) Centre.x ) +
						  ( ( double ) SinTheta * ( double ) Centre.y ) ) / FLASH_FIXED_ONE )
						  + ( double ) Centre.x;

	double dShiftY		= ( ( ( ( double ) - SinTheta * ( double ) Centre.x ) -
						  ( ( double ) CosTheta * ( double ) Centre.y ) ) / FLASH_FIXED_ONE )
						  + ( double ) Centre.y;

	// Now calculate the body of the matrix.
	double dScaleX		= ( ( ( double ) ScaleX * ( double ) CosTheta ) -
						  ( ( double ) SkewY * ( double ) SinTheta ) ) /
						  FLASH_FIXED_ONE;

	double dScaleY		= ( ( ( double ) SkewX * ( double ) SinTheta ) +
						  ( ( double ) ScaleY * ( double ) CosTheta ) ) /
						  FLASH_FIXED_ONE;

	double dSkewX		= ( ( ( double ) SkewX * ( double ) CosTheta ) -
						  ( ( double ) ScaleY * ( double ) SinTheta ) ) /
						  FLASH_FIXED_ONE;

	double dSkewY		= ( ( ( double ) ScaleX * ( double ) SinTheta ) +
						  ( ( double ) SkewY * ( double ) CosTheta ) ) /
						  FLASH_FIXED_ONE;

	double dPositionX	= ( ( ( ( double ) Position.x * ( double ) CosTheta ) -
						  ( ( double ) Position.y * ( double ) SinTheta ) ) / FLASH_FIXED_ONE ) +
						  dShiftX;

	double dPositionY	= ( ( ( ( double ) Position.x * ( double ) SinTheta ) +
						  ( ( double ) Position.y * ( double ) CosTheta ) ) / FLASH_FIXED_ONE ) +
						  dShiftY;

	// Cast the values back into longs, and the function is complete.
	ScaleX		= ( INT32 ) dScaleX;
	ScaleY		= ( INT32 ) dScaleY;
	SkewX		= ( INT32 ) dSkewX;
	SkewY		= ( INT32 ) dSkewY;
	Position.x	= ( INT32 ) dPositionX;
	Position.y	= ( INT32 ) dPositionY;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteComplexRotation ( DocCoord Position,
											   INT32 ScaleX,
											   INT32 ScaleY,
											   INT32 SkewX,
											   INT32 SkewY,
											   const DocCoord &Centre,
											   INT32 SinTheta,
											   INT32 CosTheta )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/99
	Inputs:		Position	- The x and y translation values in the original matrix.
				ScaleX		- The scale value for the x axis.
				ScaleY		- The scale value for the y axis.
				SkewY		- First skew value.
				SkewX		- Second skew value.
				( The above correspond to the basic Flash matrix values. )
				Centre		- The centre of the rotation.
				SinTheta	- The sine of the rotation.
				CosTheta	- The cosine of the rotation.
	Returns:	TRUE if success.
	Purpose:	Creates a matrix (using the BuildComplexRotation methord), and
				writes it out the the Flash file being created.

********************************************************************************************/

BOOL FlashExportDC::WriteComplexRotation ( DocCoord Position,
										   INT32 ScaleX,
										   INT32 ScaleY,
										   INT32 SkewX,
										   INT32 SkewY,
										   const DocCoord &Centre,
										   INT32 SinTheta,
										   INT32 CosTheta )
{
	// Create the complete rotation matrix.
	BuildComplexRotation ( Position, ScaleX, ScaleY, SkewX, SkewY, Centre,
						   SinTheta, CosTheta );

	// And write it.
	return WriteMatrix ( Position, ScaleX, ScaleY, SkewX, SkewY );
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteURLScript ( FlashButtonRecord *pButton )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		pButton - A record of the button that invokes the URL.
	Returns:	TRUE if success.
	Purpose:	Creates a Flash button from the details stored in the FlashButtonRecord.
				This allows the file to call other URLs by clicking on any shape designated
				as a button.

********************************************************************************************/

BOOL FlashExportDC::WriteURLScript ( FlashButtonRecord *pButton )
{
	// Step 1:	Initialise local variables.
	FilePos		StartPos	= 0;
	WORD		ButtonID	= FLASH_FIRST_ID + FlashPlaceObject::GetBitmapCount () +
							  FlashPlaceObject::GetFontCount () +
							  FlashPlaceObject::GetTextCount () +
							  FlashPlaceObject::GetShapeCount () +
							  FlashPlaceObject::GetSpriteCount () +
							  pButton->GetButtonID ();

	// Step 2:	Write the header and declaration for the record.
	StartPos = WriteHeader ( FLASH_BUTTON_TAG, FLASH_MAX_SHORT, ButtonID );

	// Step 3:	Write out the button records.
	WriteButtonRecord ( pButton->GetHitID (),	// Create a record for the hit state.
						FLASH_BUTTON_HIT,
						pButton->GetHitPosition (),
						pButton->GetTransformHit () );

	WriteButtonRecord ( pButton->GetDownID (),	// Create a record for the down state.
						FLASH_BUTTON_DOWN,
						pButton->GetDownPosition (),
						pButton->GetTransformDown () );

	WriteButtonRecord ( pButton->GetOverID (),	// Create a record for the over state.
						FLASH_BUTTON_OVER,
						pButton->GetOverPosition (),
						pButton->GetTransformOver () );

	WriteButtonRecord ( pButton->GetUpID (),	// Create a record for the up state.
						FLASH_BUTTON_UP,
						pButton->GetUpPosition (),
						pButton->GetTransformUp () );

	WriteByte ( 0 );							// End of button record.

	// Step 4:	Write out the action records.
	WriteGetURL ( pButton );					// Create a record of the actions invoked.
	WriteByte ( 0 );							// End of action record.

	// Step 5:	Rewind to the start of the record, and fill in the details of its size.
	WriteTagSize ( StartPos );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteButtonRecord ( WORD ObjectID,
											BYTE State,
											DocCoord Position,
											BOOL DoTransform )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		ObjectID	- The ID of the object to be used for this state.
				State		- The state to be used.
	Returns:	TRUE if success.
	Purpose:	Links a Flash shape record with the button, and sets this to be one of the
				states in use.

********************************************************************************************/

BOOL FlashExportDC::WriteButtonRecord ( WORD ObjectID,
										BYTE State,
										DocCoord Position,
										BOOL DoTransform )
{
	WriteBits ( 0, 4 );							// Reserved flags.
	WriteBits ( State, 4 );						// State flags.
	WriteWord ( ObjectID );						// ID of button character.
	WriteWord ( 1 );							// Layer ID.

	if ( DoTransform )							// Transform co-ordinates to Flash space.
		Transform ( &Position, &mLowCorner );	// ( Only if necessary! )

	WriteMatrix ( Position );					// Position matrix.

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteGetURL ( FlashButtonRecord *pButton )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Inputs:		pButton - A pointer to a record containing the URL of the web page.
	Returns:	TRUE if success.
	Purpose:	Creates an action script within the Flash file that fetches the URL (pointed
				to by pButton), and displays it in the browser window.

********************************************************************************************/

BOOL FlashExportDC::WriteGetURL ( FlashButtonRecord *pButton )
{
	// Initialise local variables.
	FilePos	StartPos	= 0;					// The start of the string record.
	FilePos EndPos		= 0;					// The end of the string record.
	WORD	URLLength	= 0;					// The length of the stored strings.
	TCHAR	*pURL		= pButton->GetURL ();	// Get the URL to go to.
	TCHAR	*pFrame		= pButton->GetFrame ();	// Get the frame to open the URL in.

	// Step 2:	Record as a get URL action.
	WriteByte ( FLASH_GET_URL );				// Is a get URL record.

	// Step 3:	Set up the size field.
	WriteWord ( 0 );							// Place-holder value.
	StartPos = ExportFile->tell();				// Get the start position of the file.

	// Step 4:	Record the strings to the file.
	Write ( ( BYTE* ) pURL, strlen ( pURL ) );	// Write a string of the URL.
	WriteByte ( 0 );							// Terminate the string.
	Write ( ( BYTE* ) pFrame,					// Write the frame to insert the URL into.
			strlen ( pFrame ) );

	// Step 5:	Fill in the record length.
	EndPos = ExportFile->tell ();				// Get the file position.
	ExportFile->seekIn ( ( StartPos - 2 ) );	// Go back to the start of the record.
	URLLength = ( WORD ) ( EndPos - StartPos );	// Calculate the size of the record...
	WriteWord ( URLLength );					// ... and write it in.
	ExportFile->seekIn ( EndPos );				// Run to the end of the file.

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::ProcessSprite ( FlashSprite *pSprite,
										FlashButtonRecord *pButton )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Inputs:		pSprite	- A record of the sprite to be rendered.
				pButton	- A pointer to the head of the button record list.
	Returns:	TRUE if success.
	Purpose:	Parses through the list of sprites, and replaces any records containing a
				single shape with a direct reference to the shape in the relevant button
				record. If the sprite record contains multiple shapes, then it is written
				out to disk directly.

********************************************************************************************/

BOOL FlashExportDC::ProcessSprite ( FlashSprite *pSprite,
									FlashButtonRecord *pButton )
{
	// Step 1:	Declare local variables.
	INT32					Count			= 0;
	FlashPlaceObject	*pFirstPlace	= pSprite->GetPlace ();
	FlashPlaceObject	*pPlace			= pFirstPlace;

	// Step 2:	Return FALSE if no button records have been passed in. This stops the
	//			processing, and thus catches any errors.
	if ( pButton == NULL )
		return FALSE;

	// Step 2:	Loop through the list, and count the number of FlashPlaceObject records
	//			within the sprite record.
	while ( pPlace != NULL )
	{
		Count ++;
		pPlace = pPlace->GetNext ();
	}

	// Step 3a:	If there is only one object within the sprite, spool through the button list,
	//			and replace all references to sprites with those to objects.
	if ( Count == 1 )
	{
		WORD		SpriteID	= pSprite->GetID ();
		WORD		ShapeID		= pFirstPlace->GetRawID ();
		FlashType	Type		= pFirstPlace->GetType ();
		DocCoord	Position	= pFirstPlace->GetPosition ();
		BOOL		DoTransform	= pFirstPlace->GetDoTransform ();

		// Spool through the list of buttons, and match the sprite against all states.
		while ( pButton != NULL )
		{
			// Check the hit state, and replace if necessary.
			if ( pButton->GetHitType () == FLASH_SPRITE )
			{
				WORD ID = pButton->GetRawHitID ();

				if ( ID == SpriteID )
				{
					// Change the state's contents.
					pButton->SetHitID ( ShapeID, Type );
					pButton->SetHitPosition ( Position, DoTransform );
				}
				else if ( ID > SpriteID )
				{
					// Reduce the ID value for future sprites, so that there are no gaps in
					// the ID numbers.
					pButton->SetHitID ( ID - 1, FLASH_SPRITE );
				}
			}

			// Check the down state, and replace if necessary.
			if ( pButton->GetDownType () == FLASH_SPRITE )
			{
				WORD ID = pButton->GetRawDownID ();

				if ( ID == SpriteID )
				{
					// Change the state's contents.
					pButton->SetDownID ( ShapeID, Type );
					pButton->SetDownPosition ( Position, DoTransform );
				}
				else if ( ID > SpriteID )
				{
					// Reduce the ID value for future sprites, so that there are no gaps in
					// the ID numbers.
					pButton->SetDownID ( ID - 1, FLASH_SPRITE );
				}
			}

			// Check the over state, and replace if necessary.
			if ( pButton->GetOverType () == FLASH_SPRITE )
			{
				WORD ID = pButton->GetRawOverID ();

				if ( ID == SpriteID )
				{
					// Change the state's contents.
					pButton->SetOverID ( ShapeID, Type );
					pButton->SetOverPosition ( Position, DoTransform );
				}
				else if ( ID > SpriteID )
				{
					// Reduce the ID value for successive sprites, so that there are no gaps
					// in the ID numbers.
					pButton->SetOverID ( ID - 1, FLASH_SPRITE );
				}
			}

			// Check the up state, and replace if necessary.
			if ( pButton->GetUpType () == FLASH_SPRITE )
			{
				WORD ID = pButton->GetRawUpID ();

				if ( ID == SpriteID )
				{
					// Change the state's contents.
					pButton->SetUpID ( ShapeID, Type );
					pButton->SetUpPosition ( Position, DoTransform );
				}
				else if ( ID > SpriteID )
				{
					// Reduce the ID value for future sprites, so that there are no gaps in
					// the ID numbers.
					pButton->SetUpID ( ID - 1, FLASH_SPRITE );
				}
			}

			// Move onto the next button record.
			pButton = pButton->GetNext ();
		}

		// Decrease the sprite count to indicate that an element has been removed.
		FlashPlaceObject::DecSpriteCount ();

		// And parse through the sprite list from this point onwards, decreasing the ID
		// numbers of successive sprites.
		while ( pSprite = pSprite->GetNext (), pSprite != NULL )
		{
			pSprite->SetID ( pSprite->GetID () - 1 );
		}
	}

	// Step 3b:	Otherwise export the sprite using the WriteSprite method.
	else
	{
		WriteSprite ( pSprite );
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashExportDC::WriteSprite ( FlashSprite *pSprite )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/99
	Inputs:		pSprite - A record of the sprite to be rendered.
	Returns:	TRUE if success.
	Purpose:	Writes out a sprite record to the file, which allows several shapes to be
				combined into a single button.

********************************************************************************************/

BOOL FlashExportDC::WriteSprite ( FlashSprite *pSprite )
{
	// Step 1:	Define local variables.
	WORD				ID			= FLASH_FIRST_ID + FlashPlaceObject::GetBitmapCount () +
									  FlashPlaceObject::GetFontCount () +
									  FlashPlaceObject::GetTextCount () +
									  FlashPlaceObject::GetShapeCount () +
									  pSprite->GetID ();
	FilePos				StartPos	= 0;
	FlashPlaceObject	*pPlace		= pSprite->GetPlace ();
	WORD				Depth		= FLASH_FIRST_ID;

	// Step 2:	Write out the sprite header.
	StartPos = WriteHeader ( FLASH_SPRITE_TAG,	// The header record itself.
							 FLASH_MAX_SHORT,
							 ID );
	WriteWord ( 1 );							// Frame count for sprite movie.

	// Step 3:	Write out the place object records.
	while ( pPlace != NULL )
	{
		WritePlaceObject ( pPlace, Depth );
		pPlace = pPlace->GetNext ();
		Depth ++;
	}

	// Step 4:	Remember to show the frame!
	WriteHeader ( FLASH_SHOW_FRAME_TAG, 0 );

	// Step 5:	Write out the end of record data.
	WriteHeader ( FLASH_END_FILE_TAG, 0 );

	WriteTagSize ( StartPos );

	return TRUE;
}
