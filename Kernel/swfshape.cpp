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
#include "camtypes.h"
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "swfexpdc.h"
#include "swfshape.h"

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	FlashShapeRecord::FlashShapeRecord ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		-
	Returns:	-
	Purpose:	Default constructor. Ensure that all the variables are set to values
				that won't cause problems.

********************************************************************************************/

FlashShapeRecord::FlashShapeRecord ( void )
{
	// Generate initial settings.
	DocCoord DefaultPosition ( 0, 0 );
	FlashColour White;
	DocRect DefaultBounds ( 0, 0, 0, 0 );

	// Set up the line values.
	White.Red = White.Green = White.Blue = 255;
	White.Alpha = 0;

	mLineColour = White;
	mLineWidth = 0;

	mShapeID = 0;
	mBitmapID = 0;

	mBitmapHeight = 0;
	mBitmapWidth = 0;

	mNumberColours = 0;

	mStartPoint = DefaultPosition;
	mEndPoint = DefaultPosition;
	mEndPoint2 = DefaultPosition;

	mIsCircular = FALSE;

	// By setting mFill to be FLASH_FLAT_FILL, I can make the transparency and fill
	// handling code in the FlashRenderRegion (swfrndr.cpp) symmetrical, because I
	// can always assume that a non-processed fill is a flat fill, and therefore can
	// be over-written by any style that is being applied.
	mFill = FLASH_FLAT_FILL;

	// Set the colour array to be blank.
	for ( INT32 i = 0; i < SWF_MAX_COLOURS; i++ )
	{
		mColours [i] = White;
		mRatios [i] = 0;
	}

	// And for the bounding box.
	mBoundingBox = DefaultBounds;

	mNumberCoords = 0;

	// Initialise all pointers to NULL.
	mpNext = NULL;
	mpCoordArray = NULL;
	mpPathVerbs = NULL;

	mInvalidSizeFound = FALSE;
}

/********************************************************************************************

>	FlashShapeRecord::~FlashShapeRecord ()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		-
	Returns:	-
	Purpose:	Default destructor.

********************************************************************************************/

FlashShapeRecord::~FlashShapeRecord ()
{
	// Delete the coordinate and verb arrays.
	delete [] mpCoordArray;
	delete [] mpPathVerbs;

	mpNext = NULL;
}

/********************************************************************************************

>	FlashShapeRecord* FlashShapeRecord::GetNext ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		-
	Returns:	A pointer to the next item in the list.
	Purpose:	Gets a pointer to the next item in the list.

********************************************************************************************/

FlashShapeRecord* FlashShapeRecord::GetNext ( void )
{
	return mpNext;
}

/********************************************************************************************

>	void FlashShapeRecord::SetNext ( FlashShapeRecord *pNext )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		pNext - A pointer to a FlashShapeRecord.
	Returns:	void
	Purpose:	Sets mpNext (the pointer to the next item in the list) to the
				FlashShapeRecord pointed to by pNext.

********************************************************************************************/

void FlashShapeRecord::SetNext ( FlashShapeRecord *pNext )
{
	mpNext = pNext;
}

/********************************************************************************************

>	FlashShapeRecord* FlashShapeRecord::AddNext ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		-
	Returns:	A pointer to the next item in the list.
	Purpose:	Creates a new item, adds it to the list after this node, and returns a
				pointer to this item.

********************************************************************************************/

FlashShapeRecord* FlashShapeRecord::AddNext ( void )
{
	mpNext = new FlashShapeRecord;

	return mpNext;
}

/********************************************************************************************

>	void FlashShapeRecord::SetPath ( DocCoord *pPath,
									 PathVerb *pVerbs,
									 INT32 NumberCoords )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/99
	Inputs:		-
	Returns:	pPath			- A pointer to a path array.
				pVerbs			- A pointer to a verb array.
				NumberCoords	- The number of entries in the arrays.
	Purpose:	Gets a pointer to mpCoordArray.

********************************************************************************************/

void FlashShapeRecord::SetPath ( DocCoord *pPath,
								 PathVerb *pVerbs,
								 INT32 NumberCoords )
{
	// Step 1:	Ensure that the pointers passed in are valid.
	ASSERT ( ( pPath != NULL ) && ( pVerbs != NULL ) );

	// Step 2:	Set up the member variables.
	mNumberCoords	= NumberCoords;
	mpCoordArray	= new DocCoord [ ( UINT32 ) mNumberCoords ];
	mpPathVerbs		= new PathVerb [ ( UINT32 ) mNumberCoords ];

	// Step 3:	Ensure that the arrays have been created.
	ASSERT ( ( mpCoordArray != NULL ) && ( mpPathVerbs != NULL ) );

	// Step 4:	Copy the values from pPath and pVerbs into mpCoordArray and mpPathVerbs.
	for ( INT32 i = 0; i < mNumberCoords; i++ )
	{
		mpCoordArray [i]	= pPath [i];
		mpPathVerbs [i]		= pVerbs [i];
	}
}

/********************************************************************************************

>	DocCoord* FlashShapeRecord::GetCoords ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		-
	Returns:	A pointer to the co-ordinate array mpCoordArray.
	Purpose:	Gets a pointer to mpCoordArray.

********************************************************************************************/

DocCoord* FlashShapeRecord::GetCoords ( void )
{
	return mpCoordArray;
}


/********************************************************************************************

>	PathVerb* FlashShapeRecord::GetVerbs ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		-
	Returns:	A pointer to the path descriptor array mpPathVerbs.
	Purpose:	Gets a pointer to mpPathVerbs.

********************************************************************************************/

PathVerb* FlashShapeRecord::GetVerbs ( void )
{
	return mpPathVerbs;
}

/********************************************************************************************

>	INT32 FlashShapeRecord::GetNumberCoords ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		-
	Returns:	The number of entries in the co-ordinate and verb arrays.
	Purpose:	Gets the value of mNumberCoords.

********************************************************************************************/

INT32 FlashShapeRecord::GetNumberCoords ( void )
{
	return mNumberCoords;
}

/********************************************************************************************

>	void FlashShapeRecord::SetLineWidth ( WORD Width )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		Width - The width of the line.
	Returns:	-
	Purpose:	Sets the line width.

********************************************************************************************/

void FlashShapeRecord::SetLineWidth ( INT32 Width )
{
	mLineWidth = ( WORD ) ( Width / FLASH_SCALE );
}

/********************************************************************************************

>	WORD FlashShapeRecord::GetLineWidth ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		-
	Returns:	The line width.
	Purpose:	Get's the width of the current line.

********************************************************************************************/

WORD FlashShapeRecord::GetLineWidth ( void )
{
	return mLineWidth;
}

/********************************************************************************************

>	void FlashShapeRecord::SetLineColour ( DocColour *pColour,
										   UINT32 *pAlpha )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		Colour - A FlashColour value for the line.
	Returns:	-
	Purpose:	Sets the line's colour.

********************************************************************************************/

void FlashShapeRecord::SetLineColour ( DocColour *pColour,
									   UINT32 *pAlpha )
{
	INT32 lRed, lGreen, lBlue;

	// Extract the colour value from DocColour.
	pColour->GetRGBValue ( &lRed, &lGreen, &lBlue );

	// And cast it into the record as an RGB triplet of BYTEs.
	mLineColour.Red		= ( BYTE ) lRed;
	mLineColour.Green	= ( BYTE ) lGreen;
	mLineColour.Blue	= ( BYTE ) lBlue;

	mLineColour.Alpha	= 255 - ( BYTE ) *pAlpha;
}

/********************************************************************************************

>	FlashColour FlashShapeRecord::GetLineColour ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		-
	Returns:	The colour of the line surrounding the shape.
	Purpose:	Gets the current line colour.

********************************************************************************************/

FlashColour FlashShapeRecord::GetLineColour ( void )
{
	return mLineColour;
}

/********************************************************************************************

>	void FlashShapeRecord::SetBoundingBox ( const DocRect &Bounds )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		Bounds - A DocRect containing the bounds of a path.
	Returns:	-
	Purpose:	Sets the bounding rectangle of the shape.

********************************************************************************************/

void FlashShapeRecord::SetBoundingBox ( const DocRect &Bounds )
{
	CheckValidExportSize (Bounds);
	mBoundingBox = Bounds;
}

/********************************************************************************************

>	DocRect FlashShapeRecord::GetBoundingBox ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		-
	Returns:	The shape's bounding box.
	Purpose:	Returns the shape's bounding box.

********************************************************************************************/

DocRect FlashShapeRecord::GetBoundingBox ( void )
{
	return mBoundingBox;
}

/********************************************************************************************

>	WORD FlashShapeRecord::GetShapeID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/99
	Inputs:		-
	Returns:	WORD - The value of mShapeID.
	Purpose:	Gets the ID for this Shape record.

********************************************************************************************/

WORD FlashShapeRecord::GetShapeID ( void )
{
	return mShapeID;
}

/********************************************************************************************

>	void FlashShapeRecord::SetShapeID ( WORD ShapeID )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/99
	Inputs:		WORD - The ID value to be set.
	Returns:	-
	Purpose:	Sets the value of mShapeID.

********************************************************************************************/

void FlashShapeRecord::SetShapeID ( WORD ShapeID )
{
	mShapeID = ShapeID;
}

/********************************************************************************************

>	WORD FlashShapeRecord::GetBitmapID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	WORD - The value of mBitmapID.
	Purpose:	Gets the ID for the bitmap record being used..

********************************************************************************************/

WORD FlashShapeRecord::GetBitmapID ( void )
{
	return mBitmapID;
}

/********************************************************************************************

>	void FlashShapeRecord::SetBitmapID ( WORD BitmapID )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		WORD - The ID value to be set.
	Returns:	-
	Purpose:	Sets the value of mBitmapID.

********************************************************************************************/

void FlashShapeRecord::SetBitmapID ( WORD BitmapID )
{
	mBitmapID = BitmapID;
}

/********************************************************************************************

>	void FlashShapeRecord::SetStartPoint ( const DocCoord &Point )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		DocCoord - The coordinate value to be set.
	Returns:	-
	Purpose:	Sets the value of mStartPoint, which is the start point for a fill.

********************************************************************************************/

void FlashShapeRecord::SetStartPoint ( const DocCoord &Point )
{
	mStartPoint = Point;
}

/********************************************************************************************

>	void FlashShapeRecord::SetEndPoint ( const DocCoord &Point )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		DocCoord - The coordinate value to be set.
	Returns:	-
	Purpose:	Sets the value of mEndPoint, which is the end point for a fill.

********************************************************************************************/

void FlashShapeRecord::SetEndPoint ( const DocCoord &Point )
{
	mEndPoint = Point;
}

/********************************************************************************************

>	void FlashShapeRecord::SetEndPoint2 ( const DocCoord &Point )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		DocCoord - The coordinate value to be set.
	Returns:	-
	Purpose:	Sets the value of mEndPoint2, which is another point required for a fill.

********************************************************************************************/

void FlashShapeRecord::SetEndPoint2 ( const DocCoord &Point )
{
	mEndPoint2 = Point;
}

/********************************************************************************************

>	DocCoord FlashShapeRecord::GetStartPoint ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		-
	Returns:	DocCoord - The value of mStartPoint.
	Purpose:	Gets the start point of a complex fill.

********************************************************************************************/

DocCoord FlashShapeRecord::GetStartPoint ( void )
{
	return mStartPoint;
}

/********************************************************************************************

>	DocCoord FlashShapeRecord::GetEndPoint ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		-
	Returns:	DocCoord - The value of mEndPoint.
	Purpose:	Gets the end point of a complex fill.

********************************************************************************************/

DocCoord FlashShapeRecord::GetEndPoint ( void )
{
	return mEndPoint;
}

/********************************************************************************************

>	DocCoord FlashShapeRecord::GetEndPoint2 ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		-
	Returns:	DocCoord - The value of mEndPoint2.
	Purpose:	Gets the second end point of a complex fill.

********************************************************************************************/

DocCoord FlashShapeRecord::GetEndPoint2 ( void )
{
	return mEndPoint2;
}

/********************************************************************************************

>	void FlashShapeRecord::SetColour ( DocColour *pColour,
									   UINT32 FillTransparency,
									   BYTE Ratio,
									   UINT32 Index )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		Colour - The colour of the fill.
				Transparency - The Camelot transparency value.
				Ratio - The distance ( 0 - 255 ) along the fill vector.
				Index - The number of the colour.
	Returns:	-
	Purpose:	Fills in the colour values for the fill.

********************************************************************************************/

void FlashShapeRecord::SetColour ( DocColour *pColour,
								   UINT32 FillTransparency,
								   BYTE Ratio,
								   UINT32 Index )
{
	if ( Index < SWF_MAX_COLOURS )
	{
		// Convert the Camelot colour into a Flash colour.
		INT32 lRed, lGreen, lBlue;

		// Extract the colour value from DocColour.
		pColour->GetRGBValue ( &lRed, &lGreen, &lBlue );

		// And cast it into the record as an RGB triplet of BYTEs.
		mColours [ Index ].Red		= ( BYTE ) lRed;
		mColours [ Index ].Green	= ( BYTE ) lGreen;
		mColours [ Index ].Blue	= ( BYTE ) lBlue;
		mColours [ Index ].Alpha = 255 - ( BYTE) FillTransparency;

		mRatios [ Index ] = Ratio;

		// Record how many colours have been used to date.
		if ( Index >= mNumberColours )
			mNumberColours = ( BYTE ) ( Index + 1 );
	}
}

/********************************************************************************************

>	FlashColour FlashShapeRecord::GetColour ( UINT32 i )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		UINT32 - An index into the array.
	Returns:	FlashColour - The value of mColours [i].
	Purpose:	Gets the given colour from a fill.

********************************************************************************************/

FlashColour FlashShapeRecord::GetColour ( UINT32 i )
{
	if ( i < mNumberColours )
		return mColours [i];
	else
		return mColours [mNumberColours - 1];
}

/********************************************************************************************

>	BYTE FlashShapeRecord::GetRatio ( UINT32 i )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		UINT32 - An index into the array.
	Returns:	BYTE - The value of mRatios [i].
	Purpose:	Gets the given ratio from a fill.

********************************************************************************************/

BYTE FlashShapeRecord::GetRatio ( UINT32 i )
{
	if ( i < mNumberColours )
		return mRatios [i];
	else
		return mRatios [mNumberColours - 1];
}

/********************************************************************************************

>	BYTE FlashShapeRecord::GetNumberColours ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		-
	Returns:	DocCoord - The value of mNumberColours.
	Purpose:	Gets the number of colours within a complex fill.

********************************************************************************************/

BYTE FlashShapeRecord::GetNumberColours ( void )
{
	return mNumberColours;
}

/********************************************************************************************

>	void FlashShapeRecord::SetFill ( BYTE FillType )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		BYTE - The fill style to be used.
	Returns:	-
	Purpose:	Sets the fill style for this shape.

********************************************************************************************/

void FlashShapeRecord::SetFill ( BYTE FillType )
{
	mFill = FillType;
}

/********************************************************************************************

>	BYTE FlashShapeRecord::GetFill ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		-
	Returns:	Fill - The flash fill style.
	Purpose:	Gets the fill style (mFill).

********************************************************************************************/

BYTE FlashShapeRecord::GetFill ( void )
{
	return mFill;
}

/********************************************************************************************

>	void FlashShapeRecord::SetIsCircular ( BOOL IsCircular )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/99
	Inputs:		BOOL - Is the path circular or elliptical?
	Returns:	-
	Purpose:	Sets whether or not the path is basically circular.

********************************************************************************************/

void FlashShapeRecord::SetIsCircular ( BOOL IsCircular )
{
	mIsCircular = IsCircular;
}

/********************************************************************************************

>	BOOL FlashShapeRecord::GetIsCircular ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/99
	Inputs:		-
	Returns:	mIsCircular - Is the path circular or elliptical.
	Purpose:	Gets the shape's roundness (mIsCircular).

********************************************************************************************/

BOOL FlashShapeRecord::GetIsCircular ( void )
{
	return mIsCircular;
}

/********************************************************************************************

>	void FlashShapeRecord::SetBitmapWidth ( INT32 Width )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/99
	Inputs:		INT32 - The width of the bitmap.
	Returns:	-
	Purpose:	Sets the value of mBitmapWidth.

********************************************************************************************/

void FlashShapeRecord::SetBitmapWidth ( INT32 Width )
{
	mBitmapWidth = Width;
}

/********************************************************************************************

>	INT32 FlashShapeRecord::GetBitmapWidth ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/99
	Inputs:		-
	Returns:	mBitmapWidth - The width of the bitmap.
	Purpose:	Gets the width of the bitmap being used as the fill.

********************************************************************************************/

INT32 FlashShapeRecord::GetBitmapWidth ( void )
{
	return mBitmapWidth;
}

/********************************************************************************************

>	void FlashShapeRecord::SetBitmapHeight ( INT32 Height )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/99
	Inputs:		INT32 - The height of the bitmap.
	Returns:	-
	Purpose:	Sets the value of mBitmapHeight.

********************************************************************************************/

void FlashShapeRecord::SetBitmapHeight ( INT32 Height )
{
	mBitmapHeight = Height;
}

/********************************************************************************************

>	INT32 FlashShapeRecord::GetBitmapHeight ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/99
	Inputs:		-
	Returns:	mBitmapHeight - The height of the bitmap.
	Purpose:	Gets the Height of the bitmap being used as the fill.

********************************************************************************************/

INT32 FlashShapeRecord::GetBitmapHeight ( void )
{
	return mBitmapHeight;
}

/********************************************************************************************

>	void FlashShapeRecord::RemoveLines ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/99
	Inputs:		-
	Returns:	-
	Purpose:	Removes the edge lines from an output by setting their width to 0, and making
				them transparent.

********************************************************************************************/

void FlashShapeRecord::RemoveLines ( void )
{
	mLineColour.Alpha = 0;
	mLineWidth = 0;
}


/********************************************************************************************

>	void FlashShapeRecord::CheckValidExportSize ( const DocRect & Bounds )

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/01
	Inputs:		Bounds - a bounding box for the object being checked
	Returns:	BOOL - True if the object is within the maximum bounds,
					 - False if it isn't
	Purpose:	Determines whether an object's bounding box is smaller than the maximum size 
				that Flash can cope with. If the object exceeds this, it can still be exported,
				but may cause black blocks to be drawn around the valid objects in the file, 
				when it is viewed.

********************************************************************************************/

BOOL FlashShapeRecord::CheckValidExportSize (const DocRect & Bounds)
{
	BOOL valid = FALSE;
	INT32 scaledHeight;
	INT32 scaledWidth;

	// calculate the height and width of the objects in pixels.
	scaledHeight = (Bounds.Height ()) / 750;
	scaledWidth  = (Bounds.Width ()) / 750;

	// If both the height and the width are less than the maximum, then the size is valid,
	//	If one is greater, then the bounds are invalid, and this may cause black blocks to be 
	//	shown.
	if ((scaledHeight < SWF_MAX_PIXEL_DIMENSION) &&
		(scaledWidth  < SWF_MAX_PIXEL_DIMENSION))
	{
		valid = TRUE;
	}
	else
	{
		mInvalidSizeFound = TRUE;
		valid = FALSE;
	}

	return valid;
}


/********************************************************************************************

>	BOOL FlashShapeRecord::WasInvalidSizeFound ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/01
	Returns:	BOOL - True if an object with an invalid size has been detected,
					 - False if it hasn't
	Purpose:	Shows whether the export has detected an object with an invalid size.

********************************************************************************************/

BOOL FlashShapeRecord::WasInvalidSizeFound ()
{
	return mInvalidSizeFound;
}

