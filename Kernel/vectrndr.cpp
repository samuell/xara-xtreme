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
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "vectrndr.h"

CC_IMPLEMENT_DYNAMIC( VectorFileRenderRegion, RenderRegion )

/********************************************************************************************

>	VectorFileRenderRegion::VectorFileRenderRegion( void );

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/00
	Purpose:	Default constructor for the VectorFileRenderRegion class.
	SeeAlso:	EPSOutputFilter::Init

********************************************************************************************/

VectorFileRenderRegion::VectorFileRenderRegion( void )
	: RenderRegion()
{
	// Null this pointer to prevent access violations.
	LastOutputAttrs = NULL;
}

/********************************************************************************************

>	VectorFileRenderRegion::VectorFileRenderRegion( DocRect	ClipRect,
													 Matrix		ConvertMatrix,
													 FIXED16	ViewScale )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/00
	Purpose:	Constructor for the VectorFileRenderRegion class.
	SeeAlso:	EPSOutputFilter::Init

********************************************************************************************/

VectorFileRenderRegion::VectorFileRenderRegion( DocRect	ClipRect,
												 Matrix		ConvertMatrix,
												 FIXED16	ViewScale )
	: RenderRegion( ClipRect, ConvertMatrix, ViewScale )
{
	// Null this pointer to prevent access violations.
	LastOutputAttrs = NULL;
}

/********************************************************************************************

>	VectorFileRenderRegion::~VectorFileRenderRegion( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/00
	Purpose:	Destructor for the VectorFileRenderRegion class.

********************************************************************************************/

VectorFileRenderRegion::~VectorFileRenderRegion( void )
{
	// Clear up any attributes that survived the rendering process.
	DeInitAttributes();
}

/********************************************************************************************

>	BOOL VectorFileRenderRegion::Init( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/00
	Returns:	TRUE	- Successful.
				FALSE	- An error occured.
	Purpose:	Constructor for the VectorFileRenderRegion class.

********************************************************************************************/

BOOL VectorFileRenderRegion::Init( void )
{
	// Clear up any previous attributes.
	DeInitAttributes();

	return TRUE;
}

/********************************************************************************************

>	BOOL VectorFileRenderRegion::StartRender()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Prepare the render region for rendering (exporting).
	SeeAlso:	EPSRenderRegion::Initialise; EPSRenderRegion::StopRender

********************************************************************************************/

BOOL VectorFileRenderRegion::StartRender()
{
	// Call base class first.
	if (!RenderRegion::StartRender())
		return FALSE;

	InitAttributes();

	return TRUE;
}

/********************************************************************************************

>	BOOL VectorFileRenderRegion::StopRender()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Purpose:	Deinitialise the render region after rendering (exporting).

********************************************************************************************/

BOOL VectorFileRenderRegion::StopRender()
{
	DeInitAttributes();
	return TRUE;
}

/********************************************************************************************

>	void VectorFileRenderRegion::DrawRect( DocRect *RectToRender )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Purpose:	None at present - this function should not be called during ink rendering.
	Errors:		ENSURE failure if called.

********************************************************************************************/

void VectorFileRenderRegion::DrawRect(DocRect *RectToRender)
{
	ENSURE( FALSE, "VectorFileRenderRegion::DrawRect called - this should not happen!" );
}

/********************************************************************************************

>	void VectorFileRenderRegion::DrawDragRect( DocRect *RectToRender )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Purpose:	Should not be called for this kind of render region.
	Errors:		ENSURE failure if called.
	SeeAlso:	RenderRegion::DrawDragRect

********************************************************************************************/

void VectorFileRenderRegion::DrawDragRect( DocRect *RectToRender )
{
	ENSURE(FALSE, "DrawDragRect called while exporting!");
}

/********************************************************************************************

>	void VectorFileRenderRegion::DrawLine( const DocCoord	&StartPoint,
											const DocCoord	&EndPoint )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Purpose:	None at present - this function should not be called during ink rendering.
	Errors:		ENSURE failure if called.

********************************************************************************************/

void VectorFileRenderRegion::DrawLine( const DocCoord	&StartPoint,
										const DocCoord	&EndPoint )
{
	ENSURE( FALSE, "EPSRenderRegion::DrawLine called - this should not happen!" );
}

/********************************************************************************************

>	void VectorFileRenderRegion::DrawPixel(const DocCoord &Point)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Purpose:	Should not be called for this kind of render region.
	Errors:		ENSURE failure if called.
	SeeAlso:	RenderRegion::DrawPixel

********************************************************************************************/

void VectorFileRenderRegion::DrawPixel( const DocCoord &Point )
{
	ENSURE( FALSE, "DrawPixel called while exporting!" );
}

/********************************************************************************************

>	void VectorFileRenderRegion::DrawBlob( DocCoord	p,
											BlobType	type)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Purpose:	Should not be called for this kind of render region.
	Errors:		ENSURE failure if called.
	SeeAlso:	RenderRegion::DrawBlob

********************************************************************************************/

void VectorFileRenderRegion::DrawBlob( DocCoord	p,
										BlobType	type)
{
	ENSURE( FALSE, "DrawBlob called while exporting!" );
}

/********************************************************************************************

>	void VectorFileRenderRegion::DrawCross( const DocCoord	&Point,
											 const UINT32		Size )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Purpose:	Should not be called for this kind of render region.
	Errors:		ENSURE failure if called.
	SeeAlso:	RenderRegion::DrawCross

********************************************************************************************/

void VectorFileRenderRegion::DrawCross( const DocCoord	&Point,
										 const UINT32		Size )
{
	ENSURE( FALSE, "DrawCross called while exporting!" );
}

/********************************************************************************************

>	void VectorFileRenderRegion::DrawBitmap( const DocCoord	&Point,
											  KernelBitmap*		pBitmap )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/00
	Purpose:	Should not be called for this kind of render region.
	Errors:		ENSURE failure if called.
	SeeAlso:	RenderRegion::DrawBitmap

********************************************************************************************/

void VectorFileRenderRegion::DrawBitmap( const DocCoord	&Point,
										  KernelBitmap*		pBitmap )
{
	ENSURE( FALSE, "DrawBitmap called while exporting!" );
}

/********************************************************************************************

>	void VectorFileRenderRegion::DrawBitmap( const DocCoord	&Point,
											  UINT32				BitmapID,
											  UINT32				ToolID = NULL )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/00
	Purpose:	Should not be called for this kind of render region.
	Errors:		ENSURE failure if called.
	SeeAlso:	RenderRegion::DrawBitmap

********************************************************************************************/

void VectorFileRenderRegion::DrawBitmap( const DocCoord	&Point,
										  UINT32				BitmapID,
										  UINT32				ToolID )
{
	ENSURE( FALSE, "DrawBitmap called while exporting!" );
}

/********************************************************************************************

>	void VectorFileRenderRegion::DrawBitmap( const DocCoord	&Point,
											  KernelBitmap*		pBitmap )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/00
	Purpose:	Should not be called for this kind of render region.
	Errors:		ENSURE failure if called.
	SeeAlso:	RenderRegion::DrawBitmap

********************************************************************************************/

void VectorFileRenderRegion::DrawBitmapBlob( const DocCoord	&Point,
											  KernelBitmap*		BlobShape )
{
	ENSURE( FALSE, "DrawBitmapBlob called while exporting!" );
}

/********************************************************************************************

>	void VectorFileRenderRegion::DrawBitmapBlob( const DocCoord	&Point,
												  UINT32				BitmapID,
												  UINT32				ToolID = NULL )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/00
	Purpose:	Should not be called for this kind of render region.
	Errors:		ENSURE failure if called.
	SeeAlso:	RenderRegion::DrawBitmap

********************************************************************************************/

void VectorFileRenderRegion::DrawBitmapBlob( const DocCoord	&Point,
											  UINT32				BitmapID/*,
											  UINT32				ToolID*/ )
{
	ENSURE( FALSE, "DrawBitmapBlob called while exporting!" );
}

/********************************************************************************************

>	MILLIPOINT VectorFileRenderRegion::CalcPixelWidth()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Returns:	Width of pixels on millipoints.
	Purpose:	None for vector file render regions - just returns 1.
	SeeAlso:	VectorFileRenderRegion::CalcScaledPixelWidth

********************************************************************************************/

MILLIPOINT VectorFileRenderRegion::CalcPixelWidth()
{
	return 1;
}

/********************************************************************************************

>	MILLIPOINT VectorFileRenderRegion::CalcScaledPixelWidth()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Returns:	Scaled width of pixels on millipoints.
	Purpose:	None for vector file render regions - just returns 1.
	SeeAlso:	VectorFileRenderRegion::CalcPixelWidth

********************************************************************************************/

MILLIPOINT VectorFileRenderRegion::CalcScaledPixelWidth()
{
	return 1;
}

/********************************************************************************************

>	void VectorFileRenderRegion::InitClipping()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/00
	Purpose:	Stub function.
	SeeAlso:	VectorFileRenderRegion::InitAttributes

********************************************************************************************/

void VectorFileRenderRegion::InitClipping()
{
	// Do nothing.
}

/********************************************************************************************

>	void VectorFileRenderRegion::InitAttributes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Purpose:	Set up the default attributes for the render region.
				(Actually just marks the path attributes as invalid, so that we output
				some attribute commands before the first path we export).
	SeeAlso:	VectorFileRenderRegion::InitClipping

********************************************************************************************/

void VectorFileRenderRegion::InitAttributes()
{
	// We don't use pens and brushes.
	SetOSDrawingMode();
	SetLineAttributes();
	SetFillAttributes();

	// Set up the Last Output attributes for this device, first making sure that
	// any we might have lying around already are cleaned up.
	DeInitAttributes();
	LastOutputAttrs = AttributeManager::GetDefaultAttributes();

	if ( LastOutputAttrs != NULL )
	{
		for ( INT32 i = 0; i < NumCurrentAttrs; i++ )
		{
			// NULL all the pointers to begin with,
			// so that all default attribute are output
			// with the first object
			LastOutputAttrs[i].pAttr 	= NULL;
			LastOutputAttrs[i].Temp 	= FALSE;
			LastOutputAttrs[i].Ignore 	= FALSE;
		}
	}
}

/********************************************************************************************

>	void VectorFileRenderRegion::DeInitAttributes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Purpose:	Cleans up the default attributes for the render region.

********************************************************************************************/

void VectorFileRenderRegion::DeInitAttributes()
{
	// Release the 'Last Ouput' array
	if ( LastOutputAttrs != NULL )
	{
		for ( INT32 i = 0; i < NumCurrentAttrs; i++ )
		{
			if ( LastOutputAttrs[i].Temp )
			{
				delete LastOutputAttrs[i].pAttr;
			}
		}

		// We've finished with this array of attributes now
		CCFree( LastOutputAttrs );
		LastOutputAttrs = NULL;
	}
}

/********************************************************************************************

>	void VectorFileRenderRegion::SetOSDrawingMode ()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Purpose:	Ensures that the drawing mode is set to DM_COPYPEN - any other drawing
				mode doesn't make sense when exporting EPS.
	Errors:		If drawing mode is not DM_COPYPEN, ENSURE failure.

********************************************************************************************/

void VectorFileRenderRegion::SetOSDrawingMode()
{
	// EPS doesn't care about drawing modes...or at least, we should never set
	// anything other than COPYPEN - EOR is right out!
	ENSURE( DrawingMode == DM_COPYPEN, "Non-standard drawing mode in EPS output!" );
}

/********************************************************************************************

>	void VectorFileRenderRegion::SetQualityLevel()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>			( Modified by Graeme )
	Created:	30/03/94	( Moved here on 26/10/00 )
	Purpose:	Sets the quality level for the drawing. Unused.

********************************************************************************************/

void VectorFileRenderRegion::SetQualityLevel()
{
}

/********************************************************************************************

>	DocColour VectorFileRenderRegion::AlphaBlend( DocColour *pForeground,
												   DocColour *pBackground,
												   UINT32 Transp )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/00
	Returns:	A DocColour containing the blend of the two input colours.
	Purpose:	Blends the two colours (pForeground and pBackground) together using a
				simple alpha blending algorithm.

				Moved into the VectorFileRenderRegion on 20/12/00
	SeeAlso:	-

********************************************************************************************/

DocColour VectorFileRenderRegion::AlphaBlend( DocColour &Foreground,
											   DocColour &Background,
											   UINT32 Transp )
{
	// Step 1:	Declare local variables.
	DocColour	Result;
	INT32		FGColour [3];
	INT32		BGColour [3];
	INT32		ResultColour [3];

	// Step 2:	Extract the colours from the DocColour class.
	Foreground.GetRGBValue( &( FGColour [0] ), &( FGColour [1] ), &( FGColour [2] ) );
	Background.GetRGBValue( &( BGColour [0] ), &( BGColour [1] ), &( BGColour [2] ) );

	// Step 3:	Cycle through the list of colours, perform the alpha blend, and pass the
	//			result out.
	for ( INT32 i = 0; i < 3; i++ )
	{
		// Perform the alpha blend.
		ResultColour [i] = ( ( ( 255 - Transp ) * FGColour [i] ) +
							 ( Transp * BGColour [i] ) ) / 255;
	}

	// Step 4:	Set the colour value.
	Result.SetRGBValue( ResultColour [0], ResultColour [1], ResultColour [2] );

	// Step 5:	And return it.
	return Result;
}
