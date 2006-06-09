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
#ifndef INC_FLASH_SHAPE_RECORD
#define INC_FLASH_SHAPE_RECORD

//#include "ccobject.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "doccolor.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#define SWF_MAX_COLOURS				8
#define SWF_MAX_PIXEL_DIMENSION		4350		// maximum size of an object in pixels. Actually this 
												//	is 4369.6, but it's probably better to use this 
												//	value, as it should avoid rounding errors.

// Holds RGBA values for rendering to a file.
typedef struct
{
	BYTE Red;
	BYTE Green;
	BYTE Blue;
	BYTE Alpha;
} FlashColour;

/********************************************************************************************

>	class FlashShapeRecord : public CCObject

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Purpose:	Contains the information necessary to render a DefineShape class to a file.

********************************************************************************************/

class FlashShapeRecord : public CCObject
{
public:
	FlashShapeRecord ();
	~FlashShapeRecord ();

	// Access functions.
	FlashShapeRecord* GetNext ( void );					// Get mpNext.
	void SetNext ( FlashShapeRecord *pNext );			// Set mpNext.
	FlashShapeRecord* AddNext ( void );					// Create a new element at mpNext.

	void SetPath ( DocCoord *pPath,
				   PathVerb *pVerbs,
				   INT32 NumberCoords );

	DocCoord* GetCoords ( void );						// Get mpCoordArray.

	PathVerb* GetVerbs ( void );						// Get mpPathVerbs.

	INT32 GetNumberCoords ( void );						// Get mNumberCoords.

	void SetLineWidth ( INT32 Width );
	WORD GetLineWidth ( void );

	void SetLineColour ( DocColour *pColour,
						 UINT32 *pAlpha );
	FlashColour GetLineColour ( void );

	void SetShapeID ( WORD ShapeID );					// Sets mShapeID.
	WORD GetShapeID ( void );							// Gets mShapeID.

	void SetBitmapID ( WORD BitmapID );					// Sets mBitmapID.
	WORD GetBitmapID ( void );							// Gets mBitmapID.

	void SetBoundingBox ( const DocRect &Bounds );
	DocRect GetBoundingBox ( void );

	void SetStartPoint ( const DocCoord &Point );		// Sets mStartPoint.
	void SetEndPoint ( const DocCoord &Point );			// Sets mEndPoint.
	void SetEndPoint2 ( const DocCoord &Point );		// Sets mEndPoint2.

	DocCoord GetStartPoint ( void );					// Gets mStartPoint.
	DocCoord GetEndPoint ( void );						// Gets mEndPoint.
	DocCoord GetEndPoint2 ( void );						// Gets mEndPoint2.

	void SetColour ( DocColour *pColour,				// Sets the colour, alpha, and ratio.
					 UINT32 FillTransparency,
					 BYTE Ratio,
					 UINT32 Index );

	FlashColour GetColour ( UINT32 i );					// Gets mColours [i].
	BYTE GetRatio ( UINT32 i );							// Gets mRatios [i].
	BYTE GetNumberColours ( void );						// Gets mNumberColours.

	void SetFill ( BYTE FillType );						// Sets mFill.
	BYTE GetFill ( void );								// Gets mFill.

	void SetIsCircular ( BOOL IsCircular );				// Sets mIsCircular.
	BOOL GetIsCircular ( void );						// Gets mIsCircular.

	void SetBitmapHeight ( INT32 Height );				// Sets mBitmapHeight.
	INT32 GetBitmapHeight ( void );						// Gets mBitmapHeight.

	void SetBitmapWidth ( INT32 Width );					// Sets mBitmapWidth.
	INT32 GetBitmapWidth ( void );						// Gets mBitmapWidth.

	void RemoveLines ( void );							// Removes lines from a drawing.

	BOOL WasInvalidSizeFound ();						// gets whether an invalid size has been exported
	BOOL CheckValidExportSize (const DocRect & Bounds);	// Checks whether an object is within the
														//	maximum flash size or not.


private:

	// Member variables.
	FlashShapeRecord *mpNext;				// Pointer to next record.
	DocCoord *mpCoordArray;					// Pointer to the start of the path coordinates.
	PathVerb *mpPathVerbs;					// Array of path verbs.
	INT32 mNumberCoords;						// The number of coordinates in the path array.
	WORD mLineWidth;						// Line width.
	FlashColour mLineColour;				// Line colour.
	DocRect mBoundingBox;					// The shape's bounding box.
	INT32 mBitmapHeight;						// The height of the bitmap used as a fill.
	INT32 mBitmapWidth;						// The width of the bitmap used as a fill.
	WORD mShapeID;							// The ID value of this shape.
	WORD mBitmapID;							// The ID value for the bitmap used.
	DocCoord mStartPoint;					// The position of the start of a fill.
	DocCoord mEndPoint;						// The end point of a fill.
	DocCoord mEndPoint2;					// The second end point, for complex fills.
	FlashColour mColours [SWF_MAX_COLOURS];	// An array of colours used by the shape's fill.
	BYTE mRatios [SWF_MAX_COLOURS];			// The ratios of where the colours are placed.
	BYTE mNumberColours;					// The number of colours used.
	BYTE mFill;								// The fill's fill style.
	BOOL mIsCircular;						// Is the path circular or not.

	BOOL mInvalidSizeFound;					// has a shape with an invalid size been found ?
};

#endif
