// $Id: swfrndr.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "vectrndr.h"
#include "attrmgr.h"
#include "nodetext.h"
#include "nodetxtl.h"
#include "fixedasm.h"		// For Div32by32 ().
#include "ccmaths.h"
#include "pathshap.h"
#include "webaddr.h"
#include "webattr.h"
#include "rndrgn.h"
#include "swfrndr.h"
#include "swfexpdc.h"
#include "slicehelper.h"	// For FindNextOfClass. ()
#include "fillattr.h"
#include "range.h"
#include "rendsel.h"
#include "gclips.h"

// Needed to get stroking working.
#include "brshattr.h"
#include "strkattr.h"
#include "ppstroke.h"
#include "brshbeca.h"

// needed for oversized object warning message (in ExportAll)
//#include "filtrres.h"

CC_IMPLEMENT_DYNAMIC( FlashRenderRegion, VectorFileRenderRegion )

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	FlashRenderRegion::FlashRenderRegion ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/99
	Purpose:	Constructor for the FlashRenderRegion class.
	SeeAlso:	FlashFilter::Init

********************************************************************************************/

// Use the default RenderRegion behaviour for now.
FlashRenderRegion::FlashRenderRegion ( void ) :
VectorFileRenderRegion ()
{
	// Ensure that member variable pointers are set to NULL.
	InitPointers ();
}

/********************************************************************************************

>	FlashRenderRegion::FlashRenderRegion ( DocRect ClipRect,
										   Matrix ConvertMatrix,
										   FIXED16 ViewScale )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/99
	Purpose:	Constructor for the FlashRenderRegion class.
	SeeAlso:	FlashFilter::Init

********************************************************************************************/

FlashRenderRegion::FlashRenderRegion ( DocRect ClipRect,
									   Matrix ConvertMatrix,
									   FIXED16 ViewScale ) :
VectorFileRenderRegion ( ClipRect, ConvertMatrix, ViewScale )
{
	// Ensure that member variable pointers are set to NULL.
	InitPointers ();
}

/********************************************************************************************

>	FlashRenderRegion::~FlashRenderRegion ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/99
	Purpose:	Destructor for the FlashRenderRegion class.
	SeeAlso:	-

********************************************************************************************/

FlashRenderRegion::~FlashRenderRegion ( void )
{
	// Insert clean up code here!
	if(mpFont)
	{
		delete mpFont;
		mpFont = NULL;
	}

	InitPointers ();
}

/********************************************************************************************

>	void FlashRenderRegion::DrawPathToOutputDevice ( Path *PathToRender,
													 PathShape shapePath = PATHSHAPE_PATH )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/99
	Purpose:	Outputs a path to the device context.
	Returns:	Void
	SeeAlso:	RenderRegion::InitDevice

********************************************************************************************/

void FlashRenderRegion::DrawPathToOutputDevice ( Path *PathToRender,
												 PathShape shapePath )
{
	// Step 1:	Define, and set up all the local variables.
	FlashButtonRecord		*pButton		= NULL;
	DocRect					Bounds;
	DocCoord				LowCorner;
	WebAddressAttribute		*pWebAddress	= ( WebAddressAttribute * ) GetCurrentAttribute
											  ( ATTR_WEBADDRESS );

	// Step 2:	Initialise the path, and use the clipping routines to re-order the path to
	//			wind in the correct manner for the Flash editing tools to understand. Though
	//			the player operates with an even-odd winding rule, the editing tools don't,
	//			so this operations is necessary.

	// We only clip if the path is closed, otherwise rendering artefacts occur.
	if ( PathToRender->IsClosed () )
	{
		// Declare a path for storing the clipped path in.
		Path ClippedPath;

		// Initialise the path before using it.
		ClippedPath.Initialise ( PathToRender->GetNumCoords () );

		// Set up the path to point in the right direction using ClipPathToPath.
		ClippedPath.ClipPathToPath ( *PathToRender, &ClippedPath, 3, 20 );

		// Store the path.
		ProcessPath ( &ClippedPath );
	}

	else
	{
		// Store the path.
		ProcessPath ( PathToRender );
	}

	// Step 3:	If the shape has a web address, create a FlashButtonRecord, and add the
	//			shape's ID number to this instead. Because exporting button states can
	//			alter the list of FlashPlaceObjects, it is necessary to create any buttons
	//			here.
	pButton = ProcessURL ( pWebAddress );

	// Step 4:	Add a FlashPlaceObject record.

	// Set up the position for the place object record...
	Bounds = mpShapeTail->GetBoundingBox ();
	LowCorner.x = Bounds.lox;
	LowCorner.y = Bounds.hiy;

	// ... and create it.
	PlaceObject ( FlashPlaceObject::GetShapeCount (), FLASH_SHAPE, &LowCorner, TRUE );

	// Step 5:	Export any button states that have been created.
	ExportButtonState ( pButton );
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ProcessPath ( Path *pPath )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/00
	Input:		pPath	- A pointer to the path to be stored.
	Returns:	TRUE if success.
	Purpose:	Writes all the path attributes to a Flash shape record object, in preparation
				for storing it in the file.
	SeeAlso:	FlashRenderRegion::DrawPathToOutputDevice

********************************************************************************************/

BOOL FlashRenderRegion::ProcessPath ( Path *pPath )
{
	// Step 1:	Define, and set up all the local variables.
	LineWidthAttribute		*pLineWidth		= NULL;
	FillGeometryAttribute	*pFill			= NULL;
	StrokeTranspAttribute	*pStrokeTransp	= NULL;
	StrokeColourAttribute	*pStrokeColour	= NULL;
	TranspFillAttribute		*pTransp		= NULL;
	DocColour				*pColour		= NULL;
	FlashExportDC			*FlashDC		= ( FlashExportDC * ) RenderDC;

	// Step 2:	If this is the first path to be processed, mpShape will be NULL. Therefore
	//			create a new element. Otherwise add one to the end of the existing list.
	if ( mpShape == NULL )
	{
		// Create the head of the list.
		mpShape = new FlashShapeRecord;
		ASSERT ( mpShape != NULL );
		mpShapeTail = mpShape;
	}
	else
	{
		// Add an item onto the tail of the list.
		mpShapeLast = mpShapeTail;
		mpShapeTail = mpShapeTail->AddNext ();
		ERROR2IF ( mpShapeTail == NULL, FALSE, "Unable to create path record." );
	}

	// Step 3:	Populate the shape record with the relevant values from the path.
	mpShapeTail->SetBoundingBox ( pPath->GetBoundingRect () );
	mpShapeTail->SetPath ( pPath->GetCoordArray (), pPath->GetVerbArray(),
						   pPath->GetNumCoords () );

	// Tell Camelot to split all curves before rendering. This makes the resulting
	// image closer to the original Camelot drawing.
	mpShapeTail->SetIsCircular ( TRUE );

	// Step 4:	Get line width and colour.

	// Extract the line width.
	pLineWidth = (LineWidthAttribute *) GetCurrentAttribute ( ATTR_LINEWIDTH );

	// Get the stroke colour attribute...
	pStrokeColour = (StrokeColourAttribute *) GetCurrentAttribute ( ATTR_STROKECOLOUR );
	pColour = pStrokeColour->GetStartColour ();

	// Get the stroke transparency attribute...
	pStrokeTransp = (StrokeTranspAttribute *) GetCurrentAttribute ( ATTR_STROKETRANSP );

	// Now set the colour. If the line is transparent, or we are ignoring stroking
	// information, then export a zero width, transparent line.
	if ( pColour->IsTransparent () || mSupressLines )
	{
		UINT32 Transp = 255;

		// If the stroke is transparent, don't set any colours!
		mpShapeTail->SetLineColour ( pStrokeColour->GetStartColour(),
									 &Transp );

		mpShapeTail->SetLineWidth ( 0 );
	}
	else
	{
		// Otherwise treat as a normal stroke.
		mpShapeTail->SetLineColour ( pStrokeColour->GetStartColour(),
									 pStrokeTransp->GetStartTransp () );

		mpShapeTail->SetLineWidth ( pLineWidth->LineWidth );
	}

	// Step 5:	Set the fill attributes.

	// Are we exporting a stroke path?
	if ( mExportStroke )
	{
		// The fill is to be a flat fill, based on the stroke colour.
		ProcessFlatTransp ( *( pStrokeTransp->GetStartTransp () ) );
		ProcessFlatFill ( pStrokeColour->GetStartColour () );
	}

	// If the line is closed, set the fill.
	else if ( pPath->IsFilled )
	{
		// Set the fill to the current fill attribute.
		pFill	= ( FillGeometryAttribute * ) GetCurrentAttribute ( ATTR_FILLGEOMETRY );
		pTransp	= ( TranspFillAttribute* ) GetCurrentAttribute ( ATTR_TRANSPFILLGEOMETRY );

		// Currently the transparency attribute is processed before the fill attribute, since
		// this appears to give better results than fill then transparency. Since I'm trying
		// to avoid adding any UI to the Flash filter, I've hardcoded this order in. However,
		// in a previous version, I had a ProcessFill () function which performed a switch,
		// which would have enabled the user to choose their preferences were a UI to be
		// implemented. This function can be found in a previous check in to Sourcesafe, and
		// the file details are:
		//
		// File:	swfrndr.cpp		swfrndr.h
		// User:	Graeme			Graeme
		// Date:	22/12/99		22/12/99
		// Version:	27				18
		ProcessTransparency ( pTransp, pFill );			// First process the transparency.
		ProcessColour ( pFill );						// Then the colour.
	}
	
	// Otherwise it's an unfilled path.
	else
	{
		mpShapeTail->SetFill ( FLASH_NO_FILL );
	}

	// Step 6:	Set the shape's ID value.
	mpShapeTail->SetShapeID ( FlashPlaceObject::GetShapeCount () );

	// It worked!
	return TRUE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ExportCharacter ( TextChar *pTheLetter )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/99
	Input:		TheLetter - A Unicode character, which is to be stored
	Returns:	TRUE if success.
	Purpose:	Adds the character into the two lists that maintain the strings to be
				exported to a Flash file.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashRenderRegion::ExportCharacter ( TextChar *pTheLetter )
{
	// Assume pointers (member variables) to:
	// - mpFont:	A list of font styles being used by the program.
	// - mpText:	A list of the text strings being used.
	// - mpPlace:	A list of the place object tags.

	// Define local variables.
	TextLine				*pTextLine	= pTheLetter->FindParentLine ();
	Node					*pTextStory	= pTextLine->FindParent ();
	Node					*pNode		= pTextStory->FindFirstChild ();
	FlashFontRecord			*pFont		= NULL;
	DWORD					FlashFontID = 0;
	INT32						CharCode	= 0;
	FillGeometryAttribute	*pFill		= static_cast <FillGeometryAttribute*>
										  ( GetCurrentAttribute ( ATTR_FILLGEOMETRY ) );
	TranspFillAttribute		*pTransp	= static_cast <TranspFillAttribute*>
										 ( GetCurrentAttribute ( ATTR_TRANSPFILLGEOMETRY ) );
	StrokeColourAttribute	*pStroke	= static_cast <StrokeColourAttribute*>
										  ( GetCurrentAttribute ( ATTR_STROKECOLOUR ) );

	// If the text has a gradient or bitmap fill, or any transparency other than flat,
	// export it as a path.
	if ( !pFill->IsAFlatFill () ||
		 !pTransp->IsAFlatFill () ||
		 !pStroke->GetStartColour ()->IsTransparent () )
	{
		// The fill is too complex to be exported as a straight Flash text character, so
		// export it as a path.
		return FALSE;
	}

	// Cycle through the list of children for the text node. If a NodePath is found, then
	// return FALSE. This will cause the letter to be exported as a shape rather than a
	// character, and so keep the alignment correct. This is here to export text on a curve,
	// which would otherwise not be correctly rendered.
	while ( pNode != NULL )
	{
		if ( pNode->IsNodePath () || pNode->IsARegularShape () )
		{
			// The text story contains a path, therefore it has to be exported as a set
			// of curves.
			return FALSE;
		}
		else
		{
			// Find the next node.
			pNode = pNode->FindNextNonHidden ();
		}
	}

	// Grab a pointer to the previous VTN in the TextStory. This is used for determining
	// whether or not to create a new FlashTextRecord for the successive characters.
	VisibleTextNode	*pLastVTN	= pTheLetter->FindPrevVTNInLine ();

	// Get the character to be exported from the TextChar record.
	WCHAR Glyph = pTheLetter->GetUnicodeValue ();

	// VisibleTextNodes are either renderable characters, carets, or end of line nodes.
	// I'm not interested in carets, so I'll skip these.
	if ( ( pLastVTN != NULL ) && ( pLastVTN->IsACaret () ) )
	{
		pLastVTN = pLastVTN->FindPrevVTNInLine ();
	}

	// Step 1:	Get the details of the font.

	// First the font's handle.
	WORD FontName = ( ( TxtFontTypefaceAttribute* ) GetCurrentAttribute
						( ATTR_TXTFONTTYPEFACE ) )->HTypeface ;

	// Secondly, whether or not it's bold.
	BOOL IsBold = ( ( TxtBoldAttribute* ) GetCurrentAttribute
					( ATTR_TXTBOLD ) )->BoldOn;

	// Thirdly, whether or not it's italic.
	BOOL IsItalic = ( ( TxtItalicAttribute* ) GetCurrentAttribute
					  ( ATTR_TXTITALIC ) )->ItalicOn;

	// And finally, it's offset and relative size values.
	TxtScriptAttribute *pScript = ( TxtScriptAttribute* ) GetCurrentAttribute
								  ( ATTR_TXTSCRIPT );


	// Flash doesn't seem to support fonts with obscure aspect ratios, so maybe set
	// that into the text matrix, along with rotates.

	// Step 2:	Determine whether a new FlashTextRecord is needed. A new one is
	//			required if either there isn't an existing FlashTextRecord to add the
	//			characters to, or there's been a new line. Since the first character
	//			to be stored will, by definition, be at the start of a new line, and
	//			hence this is the only check that I need to make.
	if ( pLastVTN == NULL )
	{
		// Step 2a:	Create the new FlashTextRecord to contain the string. If none
		//			exist at present, create it at mpText. Otherwise use add element.
		CreateTextRecord ( pTheLetter );
	}

	// Step 2b:	For each font, cycle through the list mpFontStyles, and
	//			compare existing resources with the current font style. If a pre
	//			-existing style	matches, reuse this one, otherwise create a new
	//			record.
	pFont = mpFont;

	// Spool through the list.
	while ( pFont != NULL )
	{
		// Compare font names and styles.
		if ( ( pFont->GetTypeface () == FontName ) &&
			 ( pFont->GetIsBold () == IsBold ) &&
			 ( pFont->GetIsItalic () == IsItalic ) )
		{
			// Break out of the while loop.
			break;
		}
		else
		{
			pFont = pFont->GetNext ();		// Move on to the next item in the list.
			FlashFontID ++;					// Increment ID number.
		}
	}

	// If pFont is NULL, no matching font styles have been stored, so create a
	// new record.
	if ( pFont == NULL )
	{
		// If there are no existing font records, create one.
		if ( mpFont == NULL )
		{
			mpFont = new FlashFontRecord;
			ASSERT ( mpFont != NULL );
			pFont = mpFontTail = mpFont;
		}
		else
		{
			mpFontTail = mpFontTail->AddElement ();
			ASSERT ( mpFontTail != NULL );
			pFont = mpFontTail;
		}

		// Record the necessary font metrics. There's no need to store FlashFontID
		// since that is implicit in the order of the elements in the list. (i.e.
		// the first item is #0, and the second is #1, etc.)
		pFont->SetTypeface ( FontName );
		pFont->SetIsBold ( IsBold );
		pFont->SetIsItalic ( IsItalic );

		// Set the ID number for the font.
		pFont->SetFontID ( FlashPlaceObject::GetFontCount () );
		FlashPlaceObject::IncFontCount ();
	}

	// And add the path.
	if ( pFont->AddGlyph ( Glyph, CharCode ) )
	{
		Matrix FontMatrix;

		TxtFontSizeAttribute FlashFontSize ( FLASH_FONT_SIZE );
		TxtFontSizeAttribute *pLastSize = ( TxtFontSizeAttribute* ) GetCurrentAttribute
										  ( ATTR_TXTFONTSIZE );

		TxtAspectRatioAttribute FlashAspect ( 1 );
		TxtAspectRatioAttribute *pLastAspect = ( TxtAspectRatioAttribute* )
											   GetCurrentAttribute ( ATTR_TXTASPECTRATIO );

		// Set the font size and aspect to the correct values to render the path.
		SetTxtFontSize ( &FlashFontSize, FALSE );
		SetTxtAspectRatio ( &FlashAspect, FALSE );

		// Add it to the record.
		pFont->AddPath ( CreateCharPath ( Glyph, &FontMatrix ), CharCode );

		// Restore the current render region settings.
		RestoreTxtFontSize ( pLastSize, FALSE );
		RestoreTxtAspectRatio ( pLastAspect, FALSE );
	}

	// Step 3:	Add the character to the end of the string, and the font style to the list
	//			of characters used in that particular font.
	INT32 Index = mpTextTail->AddChar ( pTheLetter, CharCode );

	// Fill in the font's characteristics:
	
	// Font size.
	INT32		Offset	= ( pScript->Offset ).GetRawLong ();
	INT32		Ratio	= ( pScript->Size ).GetRawLong ();

	MILLIPOINT	Size	= ( ( TxtFontSizeAttribute* ) GetCurrentAttribute
						  ( ATTR_TXTFONTSIZE ) )->FontSize;

	// Set the font size.
	mpTextTail->SetSize ( Size, Index );

	// And the ascent.
	mpTextTail->SetAscent ( pTheLetter->GetFontAscent () );

	// Calculate the baseline of the font.
	double		dDiv	= ( double ) Ratio * ( double ) FLASH_FIXED_ONE;
	double		dBase	= ( double ) ( Size - pTheLetter->GetFontDescent () ) / dDiv;

	// And the offset.
	double		dOffset	= Size * ( ( double ) Offset / ( double ) FLASH_FIXED_ONE );

	// Use these to calculate the y offset value for the text.
	double		dYOff	= dBase - dOffset - ( double ) pTheLetter->GetBaseLineShift ();

	// mpTextTail->SetBaseline ( ( INT32 ) dYOff, Index );
	mpTextTail->SetOffset ( 0 , Index );

	// Font colour.
	mpTextTail->SetColour ( pFill->GetStartColour (),
							pTransp->GetStartTransp (),
							Index );

	// Typeface. Note: I'm using the FlashFontID numbers because these map the font
	// definitions (within the Flash file) onto the file's text definitions.
	mpTextTail->SetStyle ( ( WORD ) FlashFontID, Index );

	// Step 4:	Return TRUE. This allows me to use the standard error checking macros, which
	//			will return FALSE if they hit a problem.

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ExportBevel ( NodeBevel *pBevel )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/99
	Input:		pBevel - A pointer to a NodeBevel object.
	Returns:	TRUE if success.
	Purpose:	Creates a shape record for the bevel's path, and a bitmap fill to do the
				bevel. It can currently only accurately reproduce bevels with flat fills,
				since Flash's support of transparent bitmaps is broken at the moment.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashRenderRegion::ExportBevel ( NodeBevel *pBevel )
{
	// Set up the range control flags.
	RangeControl			ControlFlags	( TRUE, TRUE );
	Range					ToRender		( pBevel, pBevel, ControlFlags );
	OILBitmap				*pBitmap		= NULL;
	KernelBitmap			*pKernBitmap	= NULL;
	Path					*pSourcePath	= &( pBevel->InkPath );
	Path					BevelPath;

	// Create and render the bitmap.
	pKernBitmap = pBevel->CreateBitmapCopy(-1.0,FALSE);
	pBitmap = pKernBitmap->ActualBitmap;

	// Initialise the path, and copy the data into it.
	BevelPath.Initialise ( pSourcePath->GetNumCoords () );

	if ( BevelPath.CopyPathDataFrom ( &( pBevel->InkPath ) ) == FALSE )
	{
		TRACEUSER( "Graeme", _T("Error, and exiting ExportBevel!\n") );
		return FALSE;
	}

	DrawPathToOutputDevice ( &BevelPath, PATHSHAPE_CIRCLE );

	// Ensure that we're not writing to a NULL pointer.
	if ( mpShapeTail != NULL )
	{
		// Variables for calculating the transparency.
		UINT32	Alpha		= 0;
		UINT32	Colours		= mpShapeTail->GetNumberColours ();

		// Sum all the alpha values for the path. I'm assuming that the bevel's
		// transparency has already been noted.
		for ( UINT32 i = 0; i < Colours; i++ )
		{
			Alpha += mpShapeTail->GetColour ( i ).Alpha;
		}

		// Get the average colour for the bevel.
		Alpha /= Colours;

		// There are probably more elegant ways of doing this, but they would involve me
		// rewriting a lot of code for one special case. Instead, I'm going to overwrite
		// the existing fill colour with the bitmap.
		WORD	BitmapID	= GetBitmapID ( pBitmap, NULL, NULL, 255 );
		DocRect	Bounds		= pBevel->GetBoundingRect ();

		// Remove the boundary lines from the bevel paths.
		mpShapeTail->RemoveLines ();

		// Some tweaks to the object inside the bevel.
		if ( mpShapeLast != NULL )
		{
			mpShapeLast->RemoveLines ();
		}

		// Record the details of the fill.
		mpShapeTail->SetStartPoint	( Bounds.lo );
		mpShapeTail->SetEndPoint	( DocCoord ( Bounds.hix, Bounds.loy ) );
		mpShapeTail->SetEndPoint2	( DocCoord ( Bounds.lox, Bounds.hiy ) );
		mpShapeTail->SetFill		( FLASH_CLIPPED_BITMAP );

		// Record the size and ID number of the bitmap used.
		mpShapeTail->SetBitmapID	( BitmapID );
		mpShapeTail->SetBitmapWidth ( pBitmap->GetWidth () );
		mpShapeTail->SetBitmapHeight ( pBitmap->GetHeight () );
	}

	// Clean up the bitmap created.
	if(pKernBitmap)
	{
		pKernBitmap->DestroyGreyscaleVersion();
		delete pKernBitmap;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ExportBevelBegin ( NodeBevelBegin *pBevel )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/99
	Input:		pBevel - A pointer to a NodeBevelBegin object.
	Returns:	TRUE if success.
	Purpose:	Stub function to prevent NodeBevelBegin from rendering.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashRenderRegion::ExportBevelBegin ( NodeBevelBegin *pBevel )
{
	return TRUE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ExportShadow ( Path *pShadowPath,
										   OILBitmap *pBitmap,
										   UINT32 Darkness )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/00
	Input:		pShadowPath - The path used by the shadow object.
				pBitmap - The bitmap containing the shadow's shape.
				pShadowColour - The colour of the shadow.
	Returns:	TRUE if success.
	Purpose:	Creates a shape record for a shadow, and stores it inside the Flash file.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashRenderRegion::ExportShadow ( Path *pShadowPath,
									   OILBitmap *pBitmap,
									   UINT32 Darkness )
{
	// Step 1:	Set up the local variables.
	DocColour		ShadColour	= *( ( ( FillGeometryAttribute* ) GetCurrentAttribute
								 ( ATTR_FILLGEOMETRY ) )->GetStartColour () );
	DocRect			PathBounds	= pShadowPath->GetBoundingRect ();
	DocCoord		EndPoint1	( PathBounds.hix, PathBounds.loy );
	DocCoord		EndPoint2	( PathBounds.lox, PathBounds.hiy );
	WORD			BitmapID	= FlashPlaceObject::GetBitmapCount ();
	FlashExportDC	*pFlashDC	= static_cast <FlashExportDC*> ( GetRenderDC() );

	TRACEUSER( "Graeme", _T("Entering ExportShadow!\n") );

	// Step 2:	Export the path.
	DrawPathToOutputDevice ( pShadowPath );

	// Step 3:	Create the bitmap record.

	// Shadow bitmaps are temporary, and so I don't need to store it in a bitmap record,
	// since these exist to allow for the re-use of existing bitmaps.
/*
	// Create a bitmap record, and add it to the tail of the bitmap list.
	if ( mpBitmap == NULL )
	{
		mpBitmap = new FlashBitmapRecord;
		ASSERT ( mpBitmap != NULL );
		mpBitmapTail = mpBitmap;
	}
	else
	{
		mpBitmapTail = mpBitmapTail->AddElement ();
		ASSERT ( mpBitmapTail != NULL );
	}

	// Set a pointer to the bitmap within the bitmap record. This prevents confusion with
	// exporting other bitmaps.
	mpBitmapTail->SetBitmap ( pBitmap );
*/
	// Step 4:	Export the shadow bitmap.
	pFlashDC->WriteShadowBitmap ( pBitmap, BitmapID, ShadColour, Darkness );

	// Step 5:	Replace the fill value for mpShapeTail with a bitmap reference.
	mpShapeTail->RemoveLines ();							// Remove lines around shadow.
	mpShapeTail->SetFill ( FLASH_CLIPPED_BITMAP );			// Bitmap style.
	mpShapeTail->SetBitmapID ( BitmapID );					// ID value.
	mpShapeTail->SetBitmapWidth ( pBitmap->GetWidth () );	// Width of bitmap.
	mpShapeTail->SetBitmapHeight ( pBitmap->GetHeight () );	// Height of bitmap.
	mpShapeTail->SetStartPoint ( PathBounds.lo );			// Startpoint of fill.
	mpShapeTail->SetEndPoint ( EndPoint1 );					// Endpoint of fill.
	mpShapeTail->SetEndPoint2 ( EndPoint2 );				// Endpoint of fill.

	TRACEUSER( "Graeme", _T("Exiting ExportShadow!\n") );

	// Step 6:	Tidy up, and return TRUE.
	FlashPlaceObject::IncBitmapCount ();

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ExportRenderableNode ( NodeRenderableInk *pInk )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/00
	Input:		pInk	- The ink node to be exported.
	Returns:	TRUE	- The node contained a stroke, and was exported by this method.
				FALSE	- Export using the default export code.
	Purpose:	Renders a stroked path to the file.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashRenderRegion::ExportRenderableNode ( NodeRenderableInk *pInk )
{
	// Graeme (10/11/00) - I don't think that this is a particularly nice way to export a
	// stroked path to a Flash file. The EPS render region will export strokes automatically,
	// without any of this messing around, and as a result I suspect that the Flash render
	// region hasn't been properly initialised.
	//
	// There is, however, an advantage to doing it this way: By calling RetroSmoothMe on the
	// path, the overall file size required to store a given path is halved, which for is a
	// big advantage for the Flash file format, in which the file size is crucial.

	// Pointer to the stroke type attribute.
	AttrStrokeType	*pStroke	= NULL;
/*
	NodeAttribute	*pAttr		= NULL;
	AttrBrushType	*pBrush		= NULL;

	// Find out if we have an applied brush node.
	pInk->FindAppliedAttribute ( CC_RUNTIME_CLASS ( AttrBrushType ), &pAttr );

	// Cast it into a brush type attribute to avoid multiple casts in the following code.
	pBrush = static_cast <AttrBrushType*> ( pAttr );

	if ( pBrush != NULL &&
		 pBrush->GetBrushHandle () != BrushHandle_NoBrush )
	{
		SimpleBecomeA BecomeA ( BECOMEA_PASSBACK, CC_RUNTIME_CLASS ( NodePath ), NULL );

		if ( pBrush->DoBecomeA ( &BecomeA, pInk ) )
		{
			NodePath *pNodePath = BecomeA.GetNodePath ();

			if ( pNodePath != NULL )
			{
				// Supress rendering the stroke around the shape.
				mSupressLines = TRUE;

				DrawPathToOutputDevice ( &( pNodePath->InkPath ) );

				// Supress rendering the stroke around the shape.
				mSupressLines = FALSE;

				return TRUE;
			}
		}
	}
*/
	// Find out whether this node has an applied stroke attribute.
	pInk->FindAppliedAttribute ( CC_RUNTIME_CLASS ( AttrStrokeType ),
								 reinterpret_cast <NodeAttribute**> ( &pStroke ) );

	if ( pStroke != NULL && pStroke->HasPathProcessor () )
	{
		// Use a SimpleBecomeA to get the path from the parent object
		SimpleBecomeA BecomeA ( BECOMEA_PASSBACK, CC_RUNTIME_CLASS ( NodePath ), NULL );
		
		if ( pInk->DoBecomeA ( &BecomeA ) )
		{
			NodePath *pNodePath = BecomeA.GetNodePath ();

			if ( pNodePath )
			{
				NodePath *pStrokePath = pStroke->GetPathProcessor ()->GetProcessedPath
														( &( pNodePath->InkPath ), pInk );

				// If the path was successfully created, export it.
				if ( pStrokePath != NULL )
				{
					// Smooth the path first, to reduce the file size.
					pStrokePath->RetroSmoothMe ( 15.0 );

					// Supress rendering the stroke around the shape.
					mSupressLines = TRUE;

					// Only render the ink path if it is itself closed.
					if ( pNodePath->InkPath.IsClosed () )
					{
						// Render the node to the file.
						DrawPathToOutputDevice ( &( pNodePath->InkPath ) );
					}

					// Inform the render region to use the line colour as the fill colour.
					mExportStroke = TRUE;

					// Export the path directly. This avoids calling the path processors
					// that can mess up the line.
					DrawPathToOutputDevice ( &( pStrokePath->InkPath ) );

					// We are no longer exporting the stroke path, so reset the control flag.
					mExportStroke = FALSE;

					// Restore stroke rendering.
					mSupressLines = FALSE;

					// Tell the program not to export the node using the default code.
					return TRUE;
				}
			}
		}
	}

	// The path wasn't rendered using the custom stroking code, so tell Camelot to use the
	// default path rendering code.
	return FALSE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ExportAll ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/99
	Input:		-
	Returns:	TRUE if success.
	Purpose:	Most of the functions within the FlashRenderRegion class are used to create
				and populate a series of linked lists which contain a processed version of
				the Camelot tree. This function passes all of these lists to the Render DC,
				so that they can be outputted to a file, and then cleans up the objects to
				prevent any memory leaks.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashRenderRegion::ExportAll ( void )
{
	// These functions all depend on the objects used by the linked list having the pointer
	// to the next instance being initialised to NULL when they are created. If this isn't
	// done, this function will throw an error, since the code will attempt to access
	// uncreated objects.
	
	// (ChrisG 18/1/01) Added an error box to warn if a shape is detected that's too big to 
	//	render properly when imported into Flash/Internet Browsers, etc...
	BOOL invalidSizeFound = FALSE;

	// Step 1:	Get a device context to the Flash file.
	FlashExportDC *pFlashDC = ( FlashExportDC * ) RenderRegion::GetRenderDC();

	TRACEUSER( "Graeme", _T("Entering ExportAll!\n") );
	FlashBitmapRecord *pLastBitmap = NULL;

	// Step 2:	Delete the bitmap records.
	while ( mpBitmap != NULL )
	{
		// The bitmap records have already been exported, so just delete the list.
		pLastBitmap = mpBitmap;
		mpBitmap = mpBitmap->GetNext();

		// (ChrisG 4/1/01) - These two calls have been removed, as they cause access violations
		//	with temporary bitmaps (e.g. in blends), as the bitmaps have already been deleted.
//		pLastBitmap->GetBitmap()->DestroyContonePalette();
//		pLastBitmap->GetBitmap()->DestroyGreyscaleVersion();
		delete pLastBitmap;
	}

	FlashFontRecord *pLastFont = NULL;

	// Step 3:	Export and delete the font records.
	while ( mpFont != NULL )
	{
		pLastFont = mpFont;

		// Write out the font definitions to the file.
		pFlashDC->WriteDefineFont ( mpFont );

		mpFont = mpFont->GetNext ();
		delete pLastFont;
	}

	FlashTextRecord *pLastText = NULL;

	// Step 4:	Export and delete the text string records.
	while ( mpText != NULL )
	{
		pLastText = mpText;

		// Write out the text strings to the file.
		pFlashDC->WriteText ( mpText );

		mpText = mpText->GetNext ();
		delete pLastText;
	}

	// Step 5:	Export and delete the shape records.
	while ( mpShape != NULL )
	{
		FlashShapeRecord *pLastShape = mpShape;

		// Write out the shapes.
		pFlashDC->WritePath ( mpShape );

		// (ChrisG 18/1/01) Check to see whether the shape's size is valid
		if (pLastShape->WasInvalidSizeFound () == TRUE)
		{
			invalidSizeFound = TRUE;
		}

		mpShape = mpShape->GetNext ();
		delete pLastShape;
	}

	// Step 6:	Export and delete the sprite records.
	while ( mpSprite != NULL )
	{
		FlashSprite *pLastSprite = mpSprite;

		// Write out the sprites.
		pFlashDC->ProcessSprite ( mpSprite, mpButton );

		mpSprite = mpSprite->GetNext ();
		delete pLastSprite;
	}

	// Step 7:	Export and delete the button records.
	while ( mpButton != NULL )
	{
		FlashButtonRecord *pLastButton = mpButton;

		// Write out the buttons.
		pFlashDC->WriteURLScript ( mpButton );

		mpButton = mpButton->GetNext ();
		delete pLastButton;
	}

	// Step 8:	Export and delete the place object classes.
	while ( mpPlace != NULL )
	{
		FlashPlaceObject *pLastPlace = mpPlace;

		// Write out the place object tags.
		pFlashDC->WritePlaceObject ( mpPlace );

		mpPlace = mpPlace->GetNext ();
		delete pLastPlace;
	}

	// Step 9:	Reset all pointer values.
	InitPointers ();

	// Warn of any invalid shapes that were detected.
	if (invalidSizeFound == TRUE)
	{
		// Display error message.
		String_256 WarnMsg;

		WarnMsg.MakeMsg(_R(IDT_FLASH_INVALID_SIZE_WARNING));
		Error::SetError(0, WarnMsg, 0);
	}

	return TRUE;
}

/********************************************************************************************

>	void FlashRenderRegion::SetButtonName ( TCHAR *pButtonName )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Input:		pButtonName - A pointer to a sting containing this button's name.
	Returns:	-
	Purpose:	Sets the name of the current button being exported. If the object being
				exported doesn't have this attribute, the string is cleared to prevent
				problems with old button names persisting.
	SeeAlso:	FlashRenderRegion::SetLayerState ()

********************************************************************************************/

void FlashRenderRegion::SetButtonName ( TCHAR *pButtonName )
{
	if ( pButtonName != NULL )
		mButtonName = pButtonName;
	else
		mButtonName.Empty ();
}

/********************************************************************************************

>	void FlashRenderRegion::SetLayerState ( LayerState State )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Input:		State - The button state of the current layer being exported.
	Returns:	-
	Purpose:	Sets the value to mButtonState to be State. This is used to determine which
				of the roll-over states of a button and which object should be used.
	SeeAlso:	FlashRenderRegion::SetButtonName ()

********************************************************************************************/

void FlashRenderRegion::SetLayerState ( LayerState State )
{
	mButtonState = State;
}

/********************************************************************************************

>	void FlashRenderRegion::InitPointers ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/99
	Purpose:	Initialises all pointer values to NULL
	SeeAlso:	-

********************************************************************************************/

void FlashRenderRegion::InitPointers ( void )
{
	// Set all the member pointers to be NULL to avoid problems later.
	mpBitmap		= NULL;
	mpBitmapTail	= NULL;
	mpFont			= NULL;
	mpFontTail		= NULL;
	mpText			= NULL;
	mpTextTail		= NULL;
	mpShape			= NULL;
	mpShapeTail		= NULL;
	mpShapeLast		= NULL;
	mpButton		= NULL;
	mpButtonTail	= NULL;
	mpSprite		= NULL;
	mpSpriteTail	= NULL;
	mpPlace			= NULL;
	mpPlaceTail		= NULL;

	// Other global variables that need resetting, but aren't pointers.
	mButtonState	= NO_BUTTON;
	mButtonName.Empty ();
	mSupressLines	= FALSE;
	mExportStroke	= FALSE;

}

/********************************************************************************************

>	BOOL FlashRenderRegion::CreateTextRecord ( TextChar *pTheLetter )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/99
	Purpose:	Creates a new FlashTextRecord, which is used to store a text character.
	Returns:	TRUE if success.
	SeeAlso:	FlashRenderRegion::ExportCharacter ()

********************************************************************************************/

BOOL FlashRenderRegion::CreateTextRecord ( TextChar *pTheLetter )
{
	// Define local variables.
	TextLine				*pTextLine		= pTheLetter->FindParentLine ();
	DocRect					Bounds			= pTextLine->GetBoundingRect ();
	WORD					ID				= FlashPlaceObject::GetTextCount ();
	FlashType				ToRender		= FLASH_TEXT;
	WebAddressAttribute		*pWebAddress	= ( WebAddressAttribute * ) GetCurrentAttribute
											  ( ATTR_WEBADDRESS );
	FlashButtonRecord		*pButton		= NULL;
	Matrix					TextMatrix;
	FIXED16					MatrixValues [4];
	INT32					Translate [2];
	DocCoord				LowCorner;

	// If no text records, create one.
	if ( mpText == NULL )
	{
		mpText = new FlashTextRecord;
		ASSERT ( mpText != NULL );
		mpTextTail = mpText;
	}
	else
	{
		// mpTextTail points at the tail of the list.
		mpTextTail = mpTextTail->AddElement ();
		ASSERT ( mpTextTail != NULL );
	}

	// Set ID.
	mpTextTail->SetTextID ( FlashPlaceObject::GetTextCount () );

	// Set aspect ratio.
	mpTextTail->SetAspect ( ( ( TxtAspectRatioAttribute* ) GetCurrentAttribute
							( ATTR_TXTASPECTRATIO ) )->AspectRatio );

	// Set the bounding box.
	mpTextTail->SetBounds ( Bounds );

	// Get the text story matrix...
	pTheLetter->GetStoryAndCharMatrix ( &TextMatrix );
	TextMatrix.GetComponents ( MatrixValues, Translate );

	// ... and store it.
	mpTextTail->SetScaleX ( MatrixValues [0].GetRawLong () );
	mpTextTail->SetScaleY ( MatrixValues [3].GetRawLong () );
	mpTextTail->SetSkewX  ( MatrixValues [1].GetRawLong () );
	mpTextTail->SetSkewY  ( MatrixValues [2].GetRawLong () );

	// Record the text's web address.
	pButton = ProcessURL ( pWebAddress );

	// Add a PlaceObject record.
	LowCorner.x = Translate [0];					// Calculate the x position.
	LowCorner.y = Translate [1];					// And also the y position.

	PlaceObject ( ID, ToRender, &LowCorner, TRUE );	// Then create the FlashPlaceObject.

	// Export any button states that might have been created.
	ExportButtonState ( pButton );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ProcessTransparency ( TranspFillAttribute *pTransp,
												  FillGeometryAttribute *pFill )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Purpose:	Processes the TranspFillAttribute of a shape, and fills in the correct
				fill attributes into the FlashShapeRecord referenced by mpShapeTail.
	Returns:	TRUE if success.
	SeeAlso:	FlashRenderRegion::ProcessColour ()

********************************************************************************************/

BOOL FlashRenderRegion::ProcessTransparency ( TranspFillAttribute *pTransp,
											  FillGeometryAttribute *pFill )
{
	// Step 1:	Extract values from the FillGeometryAttribute.
	DocCoord	*StartPoint		= pTransp->GetStartPoint ();
	DocCoord	*EndPoint1		= pTransp->GetEndPoint ();
	DocCoord	*EndPoint2		= pTransp->GetEndPoint2 ();

	UINT32		*StartTransp	= pTransp->GetStartTransp ();
	UINT32		*EndTransp1		= pTransp->GetEndTransp ();
	UINT32		*EndTransp2		= pTransp->GetEndTransp2 ();
	UINT32		*EndTransp3		= pTransp->GetEndTransp3 ();

	BYTE		CurrentFill		= mpShapeTail->GetFill ();

	// Step 2:	Determine what kind of fill is being used, and take appropriate action.

	// Step 2a:	Bitmap transparencies have priority, so do them whenever they're encountered.
	//			Bitmap transparencies are currently disabled.
	if ( ( pTransp->IsABitmapFill () || pTransp->IsAFractalFill () ) &&
		 CurrentFill == FLASH_FLAT_FILL )
	{
		// Versions of the Flash player prior to Flash 5 have had trouble rendering
		// transparent bitmaps. This appears to have been fixed in the new version, and as a
		// consequence, I've re-enabled bitmap transparencies.
		ProcessBitmapTransp ( pTransp, pFill );
	}

	// Step 2b:	If the current transparency is approximated by a linear fill, and the current
	//			fill is a flat fill, process as a linear fill.
	else if ( ( pTransp->IsALinearFill () || pTransp->IsAConicalFill () ) &&
		 CurrentFill == FLASH_FLAT_FILL )
	{
		ProcessGradientTransp ( StartTransp, NULL, EndTransp1, StartPoint, EndPoint1, NULL,
								FLASH_LINEAR_FILL );
	}

	// Step 2c: If it's a radial or diamond fill, output as a radial fill.
	else if ( ( pTransp->IsARadialFill () || pTransp->IsASquareFill () ) &&
				CurrentFill == FLASH_FLAT_FILL )
	{
		ProcessGradientTransp ( StartTransp, NULL, EndTransp1, StartPoint, EndPoint1,
								EndPoint2, FLASH_RADIAL_FILL );
	}

	// Step 2d:	If it's a four transparency fill, treat as a Linear fill. This comes before
	//			the three colour version because the IsAThreeColFill method returns TRUE for
	//			four colour fills.
	else if ( pTransp->IsAFourColFill ()  && CurrentFill == FLASH_FLAT_FILL )
	{
		DocCoord	NewEndPoint		( *EndPoint1 + ( *EndPoint2 - *StartPoint ) );
		UINT32		MidTransp		= ( *EndTransp1 + *EndTransp2 ) / 2;

		ProcessGradientTransp ( StartTransp, &MidTransp, EndTransp3, StartPoint,
								&NewEndPoint, NULL, FLASH_LINEAR_FILL );
	}

	// Step 2e: If it's a three transparency fill, convert it into a linear fill, and process
	//			it that way.
	else if ( pTransp->IsAThreeColFill () && CurrentFill == FLASH_FLAT_FILL )
	{
		DocCoord	NewEndPoint		( *EndPoint1 + ( *EndPoint2 - *StartPoint ) );
		UINT32		NewEndTransp	= ( *EndTransp1 + *EndTransp2 ) / 2;

		ProcessGradientTransp ( StartTransp, NULL, &NewEndTransp, StartPoint, &NewEndPoint,
								NULL, FLASH_LINEAR_FILL );
	}

	// Step 2f:	Default - process as a flat fill.
	else
	{
		// Get the average transparency for the whole fill.
		UINT32		Transparency	= BlendTransparencies ( StartTransp, EndTransp1,
															EndTransp2, EndTransp3 );
		// Process as a flat fill.
		ProcessFlatTransp ( Transparency );
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ProcessColour ( FillGeometryAttribute *pFill )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Purpose:	Processes the FillGeometryAttribute of a shape, and fills in the correct
				fill attributes into the FlashShapeRecord referenced by mpShapeTail.
	Returns:	TRUE if success.
	SeeAlso:	FlashRenderRegion::ProcessTransparency ()

********************************************************************************************/
 
BOOL FlashRenderRegion::ProcessColour ( FillGeometryAttribute *pFill )
{
	// Step 1:	Extract values from the FillGeometryAttribute.
	DocCoord	*StartPoint		= pFill->GetStartPoint ();
	DocCoord	*EndPoint1		= pFill->GetEndPoint ();
	DocCoord	*EndPoint2		= pFill->GetEndPoint2 ();

	DocColour	*StartColour	= pFill->GetStartColour ();
	DocColour	*EndColour1		= pFill->GetEndColour ();
	DocColour	*EndColour2		= pFill->GetEndColour2 ();
	DocColour	*EndColour3		= pFill->GetEndColour3 ();

	ColourRamp	*pRamp			= pFill->GetColourRamp ();
	BYTE		CurrentFill		= mpShapeTail->GetFill ();

	// Step 1a:	If the start colour hasn't been set, return TRUE, and stop processing
	//			the fill.
	if ( StartColour == NULL && !pFill->IsABitmapFill () )
		return TRUE;

	// Step 2:	Determine what kind of fill is being used, and take appropriate action.

	// Step 2a:	Bitmap fills have priority, so do them whenever they're encountered.
	if ( ( pFill->IsABitmapFill () ||
		   pFill->IsAFractalFill () ||
		   pFill->IsANoiseFill () ) &&
		 ( CurrentFill != FLASH_CLIPPED_BITMAP ||
		   CurrentFill != FLASH_TILED_BITMAP ) )
	{
		// Process as a bitmap fill.
		ProcessBitmapFill ( pFill );
	}

	// Step 2b:	If the current fill type is approximated by a linear fill, and the current
	//			fill is a flat fill, process as a linear fill.
	else if ( ( pFill->IsALinearFill () || pFill->IsAConicalFill () )
			  && CurrentFill == FLASH_FLAT_FILL )
	{
		ProcessGradientFill ( StartColour, NULL, EndColour1, pRamp, StartPoint, EndPoint1,
							  NULL, FLASH_LINEAR_FILL );
	}

	// Step 2c: If it's a radial or diamond fill, output as a radial fill.
	else if ( ( pFill->IsARadialFill () || pFill->IsASquareFill () ) &&
				CurrentFill == FLASH_FLAT_FILL )
	{
		ProcessGradientFill ( StartColour, NULL, EndColour1, pRamp, StartPoint, EndPoint1,
							  EndPoint2, FLASH_RADIAL_FILL );
	}

	// Step 2d:	If it's a four colour fill, treat as a Linear fill.
	else if ( pFill->IsAFourColFill ()  && CurrentFill == FLASH_FLAT_FILL )
	{
		DocCoord	NewEndPoint		( *EndPoint1 + ( *EndPoint2 - *StartPoint ) );
		DocColour	MidColour;

		// Create a colour for the midpoint of the fill.
		MidColour.Mix ( EndColour1, EndColour2, 0.5f, NULL, FALSE, NULL );

		ProcessGradientFill ( StartColour, &MidColour, EndColour3, NULL, StartPoint,
							  &NewEndPoint, NULL, FLASH_LINEAR_FILL );
	}

	// Step 2e: If it's a three colour fill, convert it into a linear fill, and process
	//			it that way.
	else if ( pFill->IsAThreeColFill () && CurrentFill == FLASH_FLAT_FILL )
	{
		DocCoord	NewEndPoint		( *EndPoint1 + ( *EndPoint2 - *StartPoint ) );
		DocColour	NewEndColour;

		// Mix the colours to create a new end colour.
		NewEndColour.Mix ( EndColour1, EndColour2, 0.5f, NULL, FALSE, NULL );

		ProcessGradientFill ( StartColour, NULL, &NewEndColour, NULL, StartPoint,
							  &NewEndPoint, NULL, FLASH_LINEAR_FILL );
	}

	// Step 2f:	Default - process as a flat colour fill.
	else
	{
		// If no fill colour has been set, or the shape is declared transparent, store as
		// an unfilled shape.
		if ( StartColour != NULL && StartColour->IsTransparent () )
		{
			mpShapeTail->SetFill ( FLASH_NO_FILL );
		}

		// Otherwise treat as a flat fill.
		else
		{
			// Get the blend of all colours.
			DocColour	Colour	= BlendColours ( StartColour, EndColour1, EndColour2,
												 EndColour3, pRamp );

			// Process as a flat fill.
			ProcessFlatFill ( &Colour );
		}
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ProcessFlatFill ( DocColour *pColour )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/99
	Purpose:	Processes a flat fill, and writes the correct values to the file record.
	Returns:	TRUE if success.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashRenderRegion::ProcessFlatFill ( DocColour *pColour )
{
	UINT32 NumberColours = mpShapeTail->GetNumberColours ();

	// If no colours have been set, increment NumberColours so that a single value can be
	// set.
	if ( NumberColours == 0 )
	{
		NumberColours = 1;
	}

	// Parse through the list of fill colours, and set all fill colours to be pStartColour.
	for ( BYTE i = 0; i < NumberColours; i++ )
	{
		mpShapeTail->SetColour ( pColour, 255 - ( mpShapeTail->GetColour ( i ).Alpha ),
								 mpShapeTail->GetRatio ( i ), i );
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ProcessGradientFill ( DocColour *pStartColour,
												  DocColour *pMidColour,
												  DocColour *pEndColour,
												  ColourRamp *pRamp,
												  DocCoord *pStartPoint,
												  DocCoord *pEndPoint,
												  DocCoord *pEndPoint2,
												  BYTE FillType )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/99
	Purpose:	Processes a gradient fill, and writes the correct values to the file record.
	Returns:	TRUE if success.
	SeeAlso:	FlashRenderRegion::DrawPathToOutputDevice ()

********************************************************************************************/

BOOL FlashRenderRegion::ProcessGradientFill ( DocColour *pStartColour,
											  DocColour *pMidColour,
											  DocColour *pEndColour,
											  ColourRamp *pRamp,
											  DocCoord *pStartPoint,
											  DocCoord *pEndPoint,
											  DocCoord *pEndPoint2,
											  BYTE FillType )
{
	// Step 1:	Set up the local variables.
	BYTE Alpha = 255 - ( mpShapeTail->GetColour ( 0 ).Alpha );
	BYTE Index = 0;

	// Step 2:	Set the start and end points, and the fill type.
	mpShapeTail->SetStartPoint ( *pStartPoint );
	mpShapeTail->SetEndPoint ( *pEndPoint );

	// Only set EndPoint2 if a NULL pointer hasn't been passed in.
	if ( pEndPoint2 != NULL )
	{
		mpShapeTail->SetEndPoint2 ( *pEndPoint2 );
	}

	// Set the fill type.
	mpShapeTail->SetFill ( FillType );

	// Step 3:	Set the fill colours.

	// Set the start colour for the fill.
	mpShapeTail->SetColour ( pStartColour, Alpha, 0, Index ++ );

	// If a middle colour has been set, write that in.
	if ( pMidColour != NULL )
	{
		mpShapeTail->SetColour ( pMidColour, Alpha, 128, Index ++ );
	}

	// Otherwise process the colour ramp.
	if ( pRamp != NULL )
	{
		ColRampItem *pColour = pRamp->GetFirstCol ();

		// Write in the ramp's colour values.
		while ( ( pColour != NULL ) && ( Index < 7 ) )
		{
			// Get the position of the colour in the fill.
			BYTE		Ratio	= ( BYTE ) ( 255.0f * pColour->GetPosition () );
			DocColour	Colour	= pColour->GetColour ();

			// Set the colour.
			mpShapeTail->SetColour ( &Colour, Alpha, Ratio, Index ++ );

			// Increment the pointer onto the next item.
			pColour = pRamp->GetNextCol ( pColour );
		}
	}

	// Write in the last colour.
	mpShapeTail->SetColour ( pEndColour, Alpha, 255, Index );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ProcessBitmapFill ( FillGeometryAttribute *pFill )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/99
	Purpose:	Processes a bitmap fill, and writes the correct values to the file record.
	Returns:	TRUE if success.
	SeeAlso:	FlashRenderRegion::DrawPathToOutputDevice ()

********************************************************************************************/

BOOL FlashRenderRegion::ProcessBitmapFill ( FillGeometryAttribute *pFill )
{
	BitmapFillAttribute*	pBMFill			= ( BitmapFillAttribute * ) pFill;
	OILBitmap				*pBMP			= ( pBMFill->GetBitmap () )->GetActualBitmap ();
	DocColour				*pStartColour	= pBMFill->GetStartColour ();
	DocColour				*pEndColour		= pBMFill->GetEndColour ();
	WORD					BitmapID		= 0;
	UINT32					Alpha			= 0;
	UINT32					Colours			= mpShapeTail->GetNumberColours ();

	// Sum all the alpha values for the bitmap.
	for ( UINT32 i = 0; i < Colours; i++ )
	{
		Alpha += mpShapeTail->GetColour ( i ).Alpha;
	}

	// And divide by the number of colour entries to get an average. Note: This will
	// under-represent a four colour transparency, since the second and third colours
	// will already have been averaged out.
	Alpha /= Colours;

	// Write the bitmap out to the file.
	BitmapID = GetBitmapID ( pBMP, pStartColour, pEndColour, Alpha );

	// Record the details of the fill.
	RecordBitmapFill ( pBMFill, BitmapID );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::RecordBitmapFill ( BitmapFillAttribute *pFill,
											   WORD BitmapID )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/99
	Purpose:	Records the details of a bitmap fill into a shape record, so that it can
				be stored within a Flash file.
	Returns:	TRUE if success.
	SeeAlso:	FlashExportDC::ProcessBitmapFill ()

********************************************************************************************/

BOOL FlashRenderRegion::RecordBitmapFill ( BitmapFillAttribute *pFill,
										   WORD BitmapID )
{
	OILBitmap				*pBitmap	= ( pFill->GetBitmap () )->GetActualBitmap ();
	FillMappingAttribute	*pFillMap	= ( FillMappingAttribute* ) GetCurrentAttribute
											( ATTR_FILLMAPPING );

	// Record the fill type, and the start and end points of the fill.
	mpShapeTail->SetStartPoint	( *( pFill->GetStartPoint () ) );
	mpShapeTail->SetEndPoint	( *( pFill->GetEndPoint () ) );
	mpShapeTail->SetEndPoint2	( *( pFill->GetEndPoint2 () ) );

	// Determine whether or not it's a clipped bitmap.
	if ( ( pFillMap != NULL ) && ( pFillMap->Repeat == RT_Simple ) )
	{
		// Single bitmap clipped to the shape.
		mpShapeTail->SetFill ( FLASH_CLIPPED_BITMAP );
	}
	else
	{
		// Repeating bitmap fill.
		mpShapeTail->SetFill ( FLASH_TILED_BITMAP );
	}

	// Record the size and ID number of the bitmap used.
	mpShapeTail->SetBitmapID ( BitmapID );
	mpShapeTail->SetBitmapWidth ( pBitmap->GetWidth () );
	mpShapeTail->SetBitmapHeight ( pBitmap->GetHeight () );

	return TRUE;
}

/********************************************************************************************

>	WORD FlashRenderRegion::GetBitmapID ( OILBitmap *pBitmap,
										  DocColour *pStartColour,
										  DocColour *pEndColour,
										  UINT32		Transparency )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/99
	Purpose:	Spools through the list of bitmaps, and returns the appropriate bitmap
				reference. Note: Bevel bitmaps are treated as being contoned, though the
				process by which their palette is generated is slightly different.

				The title  of this function is slightly misleading because it actually
				exports the bitmap, rather than just IDing it.
	Returns:	TRUE if success.
	SeeAlso:	FlashExportDC::ProcessBitmapFill ()

********************************************************************************************/

WORD FlashRenderRegion::GetBitmapID ( OILBitmap *pBitmap,
									  DocColour *pStartColour,
									  DocColour *pEndColour,
									  UINT32		Transparency )
{
	BOOL				IsContone	= ( pStartColour != NULL && pEndColour != NULL );
	WORD				BitmapID	= FlashPlaceObject::GetBitmapCount ();
	FlashExportDC		*pFlashDC	= ( FlashExportDC * ) RenderDC;
	FlashBitmapRecord	*pRecord	= mpBitmap;

	// If the bitmap has been contoned, but the underlying bitmap is a 32 bit image,
	// build an 8 bit greyscale bitmap.
	if ( ( IsContone  ) && ( pBitmap->GetBPP () > 8 ) )
	{
		// Convert the bitmap into a greyscale image.
		OILBitmap *pGreyscaleBitmap = pBitmap->GetGreyscaleVersion ();

		if ( pGreyscaleBitmap != NULL )
			pBitmap = pGreyscaleBitmap;
	}

	// Spool through the list of bitmaps, and compare with existing records. If there's
	// a match, return the bitmap's ID number, otherwise create a new record.
	while ( pRecord != NULL )
	{
		if ( ( pBitmap == pRecord->GetBitmap () ) &&
			 ( Transparency == pRecord->GetTransparency () ) &&
			 ( IsContone == pRecord->GetIsContone () ) )
		{
			// If the bitmap is contoned, and the colours don't match.
			if ( IsContone &&
				 ( ( *pStartColour != pRecord->GetContoneStart () ) ||
				 ( *pEndColour != pRecord->GetContoneEnd () ) ) )
			{
				// Continue.
				pRecord = pRecord->GetNext ();
			}
			else
			{
				BitmapID = pRecord->GetBitmapID ();
				break;
			}
		}
		else
		{
			// Continue.
			pRecord = pRecord->GetNext ();
		}
	}

	// No matching record was found, so create a new one.
	if ( pRecord == NULL )
	{
		// Create a bitmap record, and add it to the tail of the bitmap list.
		if ( mpBitmap == NULL )
		{
			mpBitmap = new FlashBitmapRecord;
			ASSERT ( mpBitmap != NULL );
			mpBitmapTail = mpBitmap;
		}
		else
		{
			mpBitmapTail = mpBitmapTail->AddElement ();
			ASSERT ( mpBitmapTail != NULL );
		}
		
		mpBitmapTail->SetBitmap ( pBitmap );
		mpBitmapTail->SetBitmapID ( BitmapID );
		mpBitmapTail->SetIsContone ( IsContone );

		// Set data for contone bitmaps.
		if ( IsContone )
		{
			// Create a contone bitmap palette.
			pBitmap->BuildContonePalette ( *( pStartColour ), *( pEndColour ), GetFillEffect (),
										   RenderView );

			// Record the colours in the bitmap record, so multiple versions aren't
			// created.
			mpBitmapTail->SetContoneColours ( *pStartColour, *pEndColour );
		}

		// Write the bitmap out at once.
		pFlashDC->WritePNG ( pBitmap, BitmapID, Transparency, IsContone );

		FlashPlaceObject::IncBitmapCount ();

		pBitmap->DestroyContonePalette();
	}

	return BitmapID;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ProcessFlatTransp ( UINT32 Transparency )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Purpose:	Processes a flat transparency, and writes the correct values to the file
				record.
	Returns:	TRUE if success.
	SeeAlso:	FlashRenderRegion::ProcessTransp ()

********************************************************************************************/

BOOL FlashRenderRegion::ProcessFlatTransp ( UINT32 Transparency )
{
	UINT32 NumberColours = mpShapeTail->GetNumberColours ();

	// If no colours have been set, increment NumberColours so that a single value can be
	// set.
	if ( NumberColours == 0 )
	{
		NumberColours = 1;
	}

	// Parse through the list of fill colours, and set all alpha values to be Transparency.
	for ( BYTE i = 0; i < NumberColours; i++ )
	{
		FlashColour	CurrentCol	= mpShapeTail->GetColour ( i );
		DocColour	NewColour	( CurrentCol.Red, CurrentCol.Green, CurrentCol.Blue );

		mpShapeTail->SetColour ( &NewColour, Transparency, mpShapeTail->GetRatio ( i ), i );
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ProcessGradientTransp ( UINT32 *pStartTransparency,
													UINT32 *pMidTransparency,
													UINT32 *pEndTransparency,
													DocCoord *pStartPoint,
													DocCoord *pEndPoint,
													DocCoord *pEndPoint2,
													BYTE FillType )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Purpose:	Processes a gradient transparency, and writes the correct values to the file
				record.
	Returns:	TRUE if success.
	SeeAlso:	FlashRenderRegion::ProcessTransparency ()

********************************************************************************************/

BOOL FlashRenderRegion::ProcessGradientTransp ( UINT32 *pStartTransparency,
												UINT32 *pMidTransparency,
												UINT32 *pEndTransparency,
												DocCoord *pStartPoint,
												DocCoord *pEndPoint,
												DocCoord *pEndPoint2,
												BYTE FillType )
{
	// Step 1:	Set up the local variables.
	FlashColour	CurrentCol	= mpShapeTail->GetColour ( 0 );
	DocColour	NewColour	( CurrentCol.Red, CurrentCol.Green, CurrentCol.Blue );
	BYTE		Index		= 0;

	// Step 2:	Set the start and end points, and the fill type.
	mpShapeTail->SetStartPoint ( *pStartPoint );
	mpShapeTail->SetEndPoint ( *pEndPoint );

	// Only set EndPoint2 if a NULL pointer hasn't been passed in.
	if ( pEndPoint2 != NULL )
	{
		mpShapeTail->SetEndPoint2 ( *pEndPoint2 );
	}

	// Set the fill type.
	mpShapeTail->SetFill ( FillType );

	// Step 3:	Set the fill colours.

	// Set the start colour for the fill.
	mpShapeTail->SetColour ( &NewColour, *pStartTransparency, 0, Index ++ );

	// If a middle colour has been set, write that in.
	if ( pMidTransparency != NULL )
	{
		mpShapeTail->SetColour ( &NewColour, *pMidTransparency, 128, Index ++ );
	}

	// Write in the last colour.
	mpShapeTail->SetColour ( &NewColour, *pEndTransparency, 255, Index );

	return TRUE;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ProcessBitmapTransp ( TranspFillAttribute *pTransparency,
												  FillGeometryAttribute *pFill )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/12/99
	Returns:	TRUE if success.
	Purpose:	Processes a bitmap fill, and writes the correct values to the file record.
				There were problems with Flash 4 to do with rendering transparent bitmaps,
				and so this was removed. Flash 5 seems to have cured them, and so this
				function has ben re-instated.
	SeeAlso:	FlashRenderRegion::ProcessFill ()

********************************************************************************************/

BOOL FlashRenderRegion::ProcessBitmapTransp ( TranspFillAttribute *pTransparency,
											  FillGeometryAttribute *pFill )
{
	BitmapFillAttribute	*pBMFill		= ( BitmapFillAttribute * ) pTransparency;
	WORD				BitmapID		= FlashPlaceObject::GetBitmapCount ();
	OILBitmap			*pBitmap		= ( pBMFill->GetBitmap () )->GetActualBitmap ();
	DocColour			*pStartColour	= pFill->GetStartColour ();
	FlashExportDC		*pFlashDC		= ( FlashExportDC * ) RenderDC;
	BOOL				IsContone		= TRUE;
	FlashBitmapRecord	*pRecord		= mpBitmap;

	// If we've got a bitmap fill applied too, we want to export that. So just set the
	// object up with a flat fill.
	if ( pFill->IsABitmapFill () ||
		 pFill->IsAFractalFill () ||
		 pFill->IsANoiseFill () )
	{
		// Calculate the average transparency across the bitmap. In this case, the end
		// transparency is 255 (i.e. clear), so add 255 to the value before dividing it.
		UINT32 Transparency = ( *( pTransparency->GetStartTransp () + 255 ) ) / 2;

		// Process it as a flat fill instead.
		return ProcessFlatTransp ( Transparency );
	}

	if ( pBitmap->GetBPP () > 8 )
	{
		// Convert the bitmap into a greyscale image.
		OILBitmap *pGreyscaleBitmap = pBitmap->GetGreyscaleVersion ();

		if ( pGreyscaleBitmap != NULL )
			pBitmap = pGreyscaleBitmap;
	}

	// Record the fill type, and the start and end points of the fill.
	mpShapeTail->SetStartPoint	( *( pTransparency->GetStartPoint () ) );
	mpShapeTail->SetEndPoint	( *( pTransparency->GetEndPoint () ) );
	mpShapeTail->SetEndPoint2	( *( pTransparency->GetEndPoint2 () ) );

	// Store repeating bitmaps as the appropriate flavour.
	if ( pTransparency->GetTesselation () == RT_Repeating ||
		 pTransparency->GetTesselation () == RT_RepeatInverted )
	{
		// Repeating bitmap fill.
		mpShapeTail->SetFill ( FLASH_TILED_BITMAP );
	}
	else
	{
		// Single bitmap clipped to the shape.
		mpShapeTail->SetFill ( FLASH_CLIPPED_BITMAP );
	}

	// Every transparency will have its own record, unlike normal bitmaps.
	FlashPlaceObject::IncBitmapCount ();

	mpShapeTail->SetBitmapID ( BitmapID );
	mpShapeTail->SetBitmapWidth ( pBitmap->GetWidth () );
	mpShapeTail->SetBitmapHeight ( pBitmap->GetHeight () );

	// Write the bitmap out at once.
	pFlashDC->WriteTransparentBitmap ( pBitmap, BitmapID, *pStartColour );

	return TRUE;
}

/********************************************************************************************

>	UINT32 FlashRenderRegion::BlendTransparencies ( UINT32 *pStart,
												  UINT32 *pEnd1,
												  UINT32 *pEnd2,
												  UINT32 *pEnd3 )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Purpose:	Calculates the average value for the transparency of a shape.
	Returns:	The average value of pStart, pEnd1, pEnd2, and pEnd3.
	SeeAlso:	FlashRenderRegion::ProcessTransparency ()

********************************************************************************************/

UINT32 FlashRenderRegion::BlendTransparencies ( UINT32 *pStart,
											  UINT32 *pEnd1,
											  UINT32 *pEnd2,
											  UINT32 *pEnd3 )
{
	// Declare and initialise the local variables.
	UINT32 Result	= *pStart;
	UINT32 Count	= 1;

	// I've nested the following if statements for efficiency, since (e.g.) if pEnd1 is
	// NULL, pEnd2 and pEnd3 will also be NULL.
	if ( pEnd1 != NULL )
	{
		Result += *pEnd1;
		Count ++;

		// Call the second if, for pEnd2.
		if ( pEnd2 != NULL )
		{
			Result += *pEnd2;
			Count ++;

			// And finally for pEnd3.
			if ( pEnd3 != NULL )
			{
				Result += *pEnd3;
				Count ++;
			}
		}
	}

	// Divide Result by Count to get the average transparency.
	Result /= Count;

	return Result;
}

/********************************************************************************************

>	DocColour FlashRenderRegion::BlendColours ( DocColour *pStart,
												DocColour *pEnd1,
												DocColour *pEnd2,
												DocColour *pEnd3,
												ColourRamp *pRamp )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Purpose:	Calculates the average value for the fill colour of a shape, along with the
				contents of any colour ramp.
	Returns:	The average value of pStart, pEnd1, pEnd2, and pEnd3.
	SeeAlso:	FlashRenderRegion::ProcessTransparency ()

********************************************************************************************/

DocColour FlashRenderRegion::BlendColours ( DocColour *pStart,
											DocColour *pEnd1,
											DocColour *pEnd2,
											DocColour *pEnd3,
											ColourRamp *pRamp )
{
	// Declare and initialise the local variables.
	DocColour	Result;
	INT32		TotalRed	= 0;
	INT32		TotalGreen	= 0;
	INT32		TotalBlue	= 0;
	INT32		Count		= 1;
	INT32		Red, Green, Blue;

	// Extract the colours from the DocColour.
	if ( pStart != NULL )
	{
		pStart->GetRGBValue ( &TotalRed, &TotalGreen, &TotalBlue );
	}

	// I've nested the following if statements for efficiency, since (e.g.) if pEnd1 is
	// NULL, pEnd2 and pEnd3 will also be NULL.
	if ( pEnd1 != NULL )
	{
		// Extract the colours from the DocColour.
		pEnd1->GetRGBValue ( &Red, &Green, &Blue );

		TotalRed	+= Red;
		TotalGreen	+= Green;
		TotalBlue	+= Blue;
		Count ++;

		// Call the second if, for pEnd2.
		if ( pEnd2 != NULL )
		{
			// Extract the colours from the DocColour.
			pEnd2->GetRGBValue ( &Red, &Green, &Blue );

			TotalRed	+= Red;
			TotalGreen	+= Green;
			TotalBlue	+= Blue;
			Count ++;

			// And finally for pEnd3.
			if ( pEnd3 != NULL )
			{
				// Extract the colours from the DocColour.
				pEnd3->GetRGBValue ( &Red, &Green, &Blue );

				TotalRed	+= Red;
				TotalGreen	+= Green;
				TotalBlue	+= Blue;
				Count ++;
			}
		}
	}

	// Process the colour ramp.
	if ( pRamp != NULL )
	{
		ColRampItem *pColour = pRamp->GetFirstCol ();

		// Write in the ramp's colour values.
		while ( pColour != NULL )
		{
			// Get the position of the colour in the fill.
			DocColour	RampColour	= pColour->GetColour ();

			// Extract the colour.
			RampColour.GetRGBValue ( &Red, &Green, &Blue );

			TotalRed	+= Red;
			TotalGreen	+= Green;
			TotalBlue	+= Blue;
			Count ++;

			// Increment the pointer onto the next item.
			pColour = pRamp->GetNextCol ( pColour );
		}
	}

	// Divide the Total* values by Count to get the average colour.
	TotalRed	/= Count;
	TotalGreen	/= Count;
	TotalBlue	/= Count;

	Result.SetRGBValue ( TotalRed, TotalGreen, TotalBlue );

	return Result;
}

/********************************************************************************************

>	FlashButtonRecord* FlashRenderRegion::ProcessURL ( WebAddressAttribute *pWebAddress )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/99
	Purpose:	Processes a WebAddressAttribute, and stores the details in a
				FlashButtonRecord for processing in the Export DC.
	Returns:	A pointer to the button record used.
	SeeAlso:	FlashExportDC::WriteURLScript

********************************************************************************************/

FlashButtonRecord* FlashRenderRegion::ProcessURL ( WebAddressAttribute *pWebAddress )
{
	FlashButtonRecord	*pButton	= NULL;

	// Ensure that a valid web address has been set.
	if ( ( pWebAddress ) != NULL && ( pWebAddress->HasURL () ) )
	{
		WebAddress	URL	= pWebAddress->m_url;
		
		// Get a pointer to an existing matching button, otherwise create a new one.
		pButton = MatchButton ();

		// Create the button record.
		pButton->SetURL ( URL.GetWebAddress () );		// Target URL.
		pButton->SetFrame ( pWebAddress->m_pcFrame );	// Window to open it in.
	}

	return pButton;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::ExportButtonState ( FlashButtonRecord *pButton )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/00
	Purpose:	This method is part of the button rollover support that is being added to
				the Flash export filter. Due to the way that the render region has been
				implemented, and the fact that I've been asked to do this after writing
				virtually all of the filter code, it's algorithm is a little strange.
				
				The Flash filter works by creating a series of linked lists containing a
				record of the objects to be exported during the pass through the rendering
				tree, which is a necessary first step to record reuseable resources such as
				bitmap images, and font definitions. This method is unique in that it goes
				back to previously created nodes and changes them by removing PlaceObject
				tags from this list.

				A more efficient way to do this would have been to pick up on whether a node
				was a part of a button before exporting. However, this would require
				rewriting a lot of code, and would probably end up being much more complex,
				so I'm willing to tolerate some eccentricity.
	Returns:	TRUE if success, FALSE if an error is encountered.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashRenderRegion::ExportButtonState ( FlashButtonRecord *pButton )
{
	// Define local variables.
	FlashPlaceObject	*pLast		= mpPlaceTail;
	FlashSprite			*pSprite	= NULL;

	// If the function has been passed a NULL pointer, check whether a button name has been
	// set. If not, return from this function.
	if ( pButton == NULL )
	{
		// If a name has been set, then this shape is part of a button array, so go and
		// process it.
		if ( ( mButtonState == NO_BUTTON ) &&
			 ( mButtonName.IsEmpty () ) )
		{
			return FALSE;
		}
	}

	// Remove the object from the tail of the list. Rewind the PlaceObject tail pointer.
	mpPlaceTail = mpPlaceTail->GetLast ();

	// Catch NULL pointers to avoid access violations.
	if ( mpPlaceTail != NULL )
	{
		mpPlaceTail->SetNext ( NULL );
	}
	else
	{
		mpPlace = NULL;
	}

	// If no button exists at this point, get a match.
	pButton = MatchButton ();

	// Get the sprite record ID from the list.
	pSprite = MatchSprite ( pButton );

	// Re-attatch the place object record to the sprite record.
	pSprite->SetTail ( pLast );

	return TRUE;
}

/********************************************************************************************

>	FlashButtonRecord* FlashRenderRegion::MatchButton ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/00
	Returns:	A pointer to the button record that has the same name string as the one
				passed into this function.
	Purpose:	Spools through the list of button records (pointed to by mpButton), and
				attempts to match the sprite with the input parameters. If no match is
				made, a new button record is created, and populated with the correct values.
	SeeAlso:	FlashFilter::ExportButtonState, FlashFilter::MatchSprite

********************************************************************************************/

FlashButtonRecord* FlashRenderRegion::MatchButton ( void )
{
	FlashButtonRecord *pButton = mpButton;

	// Loop through the list until a match is made.
	while ( pButton != NULL && !pButton->IsAMatch ( ( TCHAR* ) mButtonName ) )
	{
		pButton = pButton->GetNext ();
	}

	// Check to see whether there has been a match made. If not, create a new record.
	if ( pButton == NULL )
	{
		WORD ID = FlashPlaceObject::GetButtonCount ();

		// Create a new button record, and make sure that one exists first!
		if ( mpButton == NULL )
		{
			pButton = mpButtonTail = mpButton = new FlashButtonRecord;
		}
		else
		{
			pButton = mpButtonTail = mpButtonTail->AddElement ();
		}

		// Set the name to act as an ID value.
		mpButtonTail->SetName ( ( TCHAR* ) mButtonName );

		// Create a new FlashPlaceObject.
		PlaceObject ( ID, FLASH_BUTTON );

		// Set the ID for the button object.
		mpButtonTail->SetButtonID ( ID );
	}

	return pButton;
}

/********************************************************************************************

>	FlashSprite* FlashRenderRegion::MatchSprite ( FlashButtonRecord *pButton )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/99
	Returns:	A pointer to the sprite record that has the same name and LayerState to the
				values passed in.
	Purpose:	Spools through the list of sprite records (pointed to by mpSprite), and
				attempts to match the sprite with the input parameters. If no match is made,
				a new sprite record is created, and populated with the correct name and
				LayerState.
	SeeAlso:	FlashFilter::ExportButtonState, FlashFilter::MatchButton

********************************************************************************************/

FlashSprite* FlashRenderRegion::MatchSprite ( FlashButtonRecord *pButton )
{
	FlashSprite	*pSprite	= mpSprite;

	// Loop through the list of sprites until a match is made, or the end of the list is
	// reached.
	while ( pSprite != NULL && !pSprite->IsAMatch ( ( TCHAR* ) mButtonName, mButtonState ) )
	{
		pSprite = pSprite->GetNext ();
	}

	// Check to see whether there has been a match made. If not, create a new record,
	// and insert it into the button record at the appropriate point.
	if ( pSprite == NULL )
	{
		WORD ID = FlashPlaceObject::GetSpriteCount ();

		// Check to see whether or not a record has been created, and if not, create one.
		if ( mpSpriteTail != NULL )
		{
			pSprite = mpSpriteTail = mpSpriteTail->AddNext ();
		}
		else
		{
			pSprite = mpSpriteTail = mpSprite = new FlashSprite;
			ASSERT ( mpSprite != NULL );
		}

		mpSpriteTail->SetID ( ID );					// Set the sprite's ID value.
		mpSpriteTail->SetName ( mButtonName );		// Set the name to act as an ID value.
		mpSpriteTail->SetLayer ( mButtonState );	// Set the layer value as another ID.

		// Determine which layer we're dealing with here, and attach it to the appropriate
		// button state.
		switch ( mButtonState )
		{
		case CLICKED:
			pButton->SetDownID ( ID, FLASH_SPRITE );
			break;

		case MOUSE:
			pButton->SetOverID ( ID, FLASH_SPRITE );
			break;

		case SELECTED:
			pButton->SetHitID ( ID, FLASH_SPRITE );
			break;

		default:	// i.e. DEFAULT or NO_BUTTON.
			pButton->SetUpID ( ID, FLASH_SPRITE );
			break;
		}

		FlashPlaceObject::IncSpriteCount ();	// Increment the sprite count.
	}

	return pSprite;
}

/********************************************************************************************

>	BOOL FlashRenderRegion::PlaceObject ( WORD ID,
										  FlashType ToRender,
										  DocCoord *pPosition = NULL,
										  BOOL DoTransform = FALSE )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Returns:	TRUE if success.
	Purpose:	Creates a FlashPlaceObject record for the object with the corresponding ID.
	SeeAlso:	-

********************************************************************************************/

BOOL FlashRenderRegion::PlaceObject ( WORD ID,
									  FlashType ToRender,
									  DocCoord *pPosition,
									  BOOL DoTransform )
{
	// If no records exist, create the head of the list.
	if ( mpPlace == NULL )
	{
		mpPlace = new FlashPlaceObject;
		ASSERT ( mpPlace != NULL );
		mpPlaceTail = mpPlace;
	}

	// Otherwise add an element onto the tail of the list.
	else
	{
		mpPlaceTail = mpPlaceTail->AddNext ();
		ASSERT ( mpPlaceTail != NULL );
	}

	mpPlaceTail->SetID ( ID );
	mpPlaceTail->SetType ( ToRender );

	// Don't reset the position if a NULL DocCoord pointer has been passed in.
	if ( pPosition != NULL )
	{
		mpPlaceTail->SetPosition ( *pPosition );
		mpPlaceTail->SetDoTransform ( DoTransform );
	}

	// Remember to increment the counter for the appropriate shape type.
	switch ( ToRender )
	{
	case FLASH_TEXT:
		FlashPlaceObject::IncTextCount ();
		break;

	case FLASH_SHAPE:
		FlashPlaceObject::IncShapeCount ();
		break;

	case FLASH_SPRITE:
		FlashPlaceObject::IncSpriteCount ();
		break;

	case FLASH_BUTTON:
		FlashPlaceObject::IncButtonCount ();
		break;
	}

	return TRUE;
}
