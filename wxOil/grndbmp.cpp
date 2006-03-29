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

// RenderRegion to render into a bitmap

// This file used to use the ScaleFactor as part of its code to work
// out the size of a bitmap.  Due to changes I have made elsewhere in
// the code, the scale factor should always be one (DPI is used to
// generate bitmaps of different sizes).  Therefore, I have removed
// all references to ScaleFactor in this code and replaced them with
// ERROR3s.  -- Jonathan Payne, 8/9/2000

// The above comment fails to take into account the need for the 
// feathering code to know the scale factor so it can adjust the size of 
// its bitmaps correctly when rendering to a bitmap for printing or on-
// screen preview of separations.  This causes feathers to be rendered 
// incorrectly at any scale factor other than 100%!!!!!
// Gerry - 15/04/2004

#include "camtypes.h"
#include "grndbmp.h"
#include "colcontx.h"
#include "bitmap.h"
#include "oilbitmap.h"
#include "dibutil.h"
#include "dibconv.h"
#include "osrndrgn.h"
// GAT #include "outptdib.h"
#include "palman.h"
// GAT #include "progress.h"
#include "GDrawIntf.h"			// GAT

#include "fixmem.h"

#include "app.h"		// GetApplication
#include "bmapprev.h"

#define FIXEDBANDSIZE 512

CC_IMPLEMENT_DYNAMIC( GRenderBitmap, GRenderDIB )
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
CC_IMPLEMENT_DYNAMIC( GRenderOptPalette, GRenderBitmap )
#endif

#define new CAM_DEBUG_NEW

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
// Preference as to whether we round the colours of a 1bpp optimised palette
BOOL GRenderOptPalette::DoTwoColourRoundingToPrimary = TRUE;
#endif


// This is a static GDraw context, which is initialised to use a 256-colour greyscale palette
// This is used when rendering airbrushed strokes, which need to generate 8bpp transparency
// bitmaps. initialising this context can take 2 seconds or so, so we only do it once
// (on demand, the first time it is actually needed).
GDrawContext *GRenderBitmap::pGreyscaleContext = NULL;

/* GAT
BOOL GRenderOptPalette::DoGDrawConversion = TRUE;
BOOL GRenderOptPalette::UseOldPalette = TRUE;
*/
/********************************************************************************************

>	GRenderBitmap::GRenderBitmap(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, 
							 UINT32 Depth, INT32 dpi, BOOL Printing, UINT32 Dither, LPLOGPALETTE pPalette, 
							 BOOL AutoConvert)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GRenderBitmap constructor. Doesn't do anything actually, just calls base class.
				INT32 dpi changed to double dpi (12/12/95) to improve the range of values allowed
				at the < 1000dpi settings that we will be using.
	Errors:		-
	SeeAlso:	GRenderRegion::GRenderRegion

********************************************************************************************/

GRenderBitmap::GRenderBitmap(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, 
							 UINT32 Depth, double dpi, BOOL Printing, UINT32 Dither, LPLOGPALETTE pPalette, 
							 BOOL AutoConvert)
	: GRenderDIB( ClipRegion, ConvertMatrix, ViewScale, Depth, dpi)
{
	// normally our colour context is obtained by RenderRegion::InitDevice,
	// but sadly this requires a RenderWindow, which we ain't got. There must
	// be a better way than this, but don't knock it. This must be done BEFORE
	// InitDevice as that renders default attributes, which tend to rely on a colour context

	if (RenderView==NULL)
		CurrentColContext = ColourContext::GetGlobalDefault(COLOURMODEL_RGBT);

	// we do not want to overlap the bands by default
	OverlapBands = FALSE;

	// We do want to render bottom to top by default
	RenderBottomToTop = TRUE;

	// Render flag stuff
	RenderFlags.Printing = Printing;

 	// This next bit was added to cope with the conversion needed when using certain
	// dithering techniques
	DitherType = Dither;
	OutputDepth = Depth;  		// If different from the actual RR bpp, then the bitmap will
								// be converted before output
	lpOutputInfo = NULL;	
	lpOutputBits = NULL;	

	m_dXCentralAdjust = 0;
	m_dYCentralAdjust = 0;

	pConvPalette = pPalette;

	if (AutoConvert && OutputDepth<32)
	{
		// The dithering we are using requires us to render to 32bpp and then convert it down
		// to our output bpp at the end

		// Force rendering to 32bpp, we will convert it to 'Depth' at the end
		uBitmapDepth = 32;
	}

	pPreviousGDrawContext = NULL;
}

/********************************************************************************************

>	static BOOL GRenderBitmap::DitheringNeeds32bpp(UINT32 Dither)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-

********************************************************************************************/

BOOL GRenderBitmap::DitheringNeeds32bpp(UINT32 Dither)
{
	return (Dither != XARADITHER_ORDERED_GREY);
}

/********************************************************************************************

>	BOOL GRenderBitmap::StartRender()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94
	Inputs:		
	Outputs:	
	Returns:	TRUE if worked, FALSE if failed
	Purpose:	Uses base class to do normal stuff, then fills bitmap with White as a sort
				of paper background.
	Errors:		-
	SeeAlso:	GRenderDIB::StartRender

********************************************************************************************/

BOOL GRenderBitmap::StartRender()
{
	// call base class first
	const BOOL ok = GRenderDIB::StartRender();
	if (!ok)
		return FALSE;
/*
	In an attempt to standardise things, all bmp initing is now done by the
	InitBmpBits() virtual function. I am pretty sure that in the past, double
	bmp initialisation was occuring
	ie here, and in the GetDrawContext()->SetupBitmap(..) function as well.

*/
	// set bitmap size field so gets written out correctly
	pBitmapInfo->bmiHeader.biXPelsPerMeter = (INT32)((PixelsPerInch * 10000 ) / 254);
	//lpBitmapInfo->bmiHeader.biXPelsPerMeter = Mul32Div32( PixelsPerInch, 10000, 254 );
	pBitmapInfo->bmiHeader.biYPelsPerMeter = pBitmapInfo->bmiHeader.biXPelsPerMeter;

	// done OK
	return TRUE;
}

/********************************************************************************************

>	virtual WinRect GRenderBitmap::CalculateWinRect( const Matrix& RenderMatrix, const DocRect& docrect,
													 const double dpi)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/96
	Inputs:		DocRect is a rectangle on document co-ords.
				dpi is the resolution of the device we are rendering to
	Outputs:	RenderMatrix is the rendering matrix, may have e anf f components changed by the call
	Returns:	Object containing the new rectangle coordinates.     			
	Purpose:	To convert a rectangle in Doc coordinates to a rectangle in Win coordinates
				taking account of the destination dpi rather than assuming screen dpi.
				Virtual so that it can be overriden by different render regions if so required.
	SeeAlso:	OSRenderRegion::DocRectToWin;

********************************************************************************************/

WinRect GRenderBitmap::CalculateWinRect( Matrix& RenderMatrix, const DocRect& docrect,
										 const double dpi)
{
	// lets get the Rect into screen co-ords so we can use our big screen bitmap
	// Use a version of OSRenderRegion::DocRectToWin that uses the actual dpi we have stored
	// in PixelsPerInch and that tries to sort out the pixel problems
	return OSRenderRegion::BitmapDocRectToWin( RenderMatrix, docrect, dpi );
}

/********************************************************************************************

>	virtual GMATRIX GRenderBitmap::MakeGavinMatrix(Matrix NewRenderMatrix, DocRect ClipRect, double dPixelsPerInch, BOOL bMasterCapture)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> (from Will (from Phil code))
	Created:	??/??/2004
	Inputs:		The Kernel Matrix and cliprect to use to create the Gavin Matrix.
	Returns: 	TRUE if the GMatrix was created ok.
	Purpose:	Make a Gavin Matrix out of a Kernel Matrix
	SeeAlso:	GRenderRegion::SetGavinMatrix
	Notes:		Doesn't rely on current state of CurrentClipRect like the above version

********************************************************************************************/

GMATRIX GRenderBitmap::MakeGavinMatrix(Matrix NewRenderMatrix, DocRect ClipRect, double dPixelsPerInch, BOOL bMasterCapture)
{
	GMATRIX gmat;
	// Get the scale factors from the original render matrix
	FIXED16 abcd[4];
	NewRenderMatrix.GetComponents( abcd, NULL );

	// Use them to call DocRect to win on the clipping region, without attempting any translation.
	// Thus, the DocCoord cliprect gets turned directly into a Windows pixel values cliprect!
	Matrix UnscrolledRender(abcd[0], abcd[1], abcd[2], abcd[3], 0, 0);

	// in order to get Gavin's matrix calcs to agree with ours, we have to force him to
	// round before shifting down. We do this by adding half a pixel to the offsets in
	// his matrices. There is a school of thought which says that his matrix code
	// should be made to round, but it might have unpleasant side-effects. For now, we do
	// it this way. A pixel is 1<<(FX+16), so half a pixel is this incantation:

	// sjk's (5/12/00) preferred version as this works with the decimal places that may be in PixelsPerInch
	const XLONG Mult = (INT32)(dPixelsPerInch*(double)(1 << FX) + 0.5);

	gmat.AX = ( (XLONG)abcd[0].GetRawLong() * Mult ) / XLONG(72000);
	gmat.AY = ( (XLONG)abcd[1].GetRawLong() * Mult ) / XLONG(72000);
	gmat.BX = ( (XLONG)abcd[2].GetRawLong() * Mult ) / XLONG(72000);
	gmat.BY = ( (XLONG)abcd[3].GetRawLong() * Mult ) / XLONG(72000);

	// Work out the offsets in the GMatrix, taking into account special handling
	// to centralise the drawing within the bitmap
	// If we're making the matrix for the master capture
	// Then DO centralise teh rendering within the whole-pixel bitmap
	// Else DON'T centralise again, just re-use the offsets we calculated the first time
	OSRenderRegion::CalculateGavinOffsetsWinRect(UnscrolledRender, ClipRect, dPixelsPerInch, &gmat, bMasterCapture, &m_dXCentralAdjust, &m_dYCentralAdjust);

	return gmat;
}

/********************************************************************************************

>	GRenderBitmap::~GRenderBitmap()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GRenderBitmap destructor. Frees up the bitmap.
	Errors:		-

********************************************************************************************/

GRenderBitmap::~GRenderBitmap()
{
	// we should NOT delete the CurrentColContext cos we didn't alloc it - we just
	// got a pointer to something allocated by somebody else

	if (lpOutputInfo!=NULL || lpOutputBits!=NULL)
	{
		// Free up the conversion bitmap
		FreeDIB(lpOutputInfo, lpOutputBits);
		lpOutputInfo = NULL;
		lpOutputBits = NULL;
	}

	// Free up the bitmap here, as the call to FreeLPBits in the
	// GRenderDIB will not call the correct version (the virtual-ness will be
	// broken as it is called from a destructor
	if (pBitmapInfo!=NULL)
	{
		// NB Export rendering can be cancelled by user
		// Hence can be in the middle of offscreen rendering at this point
		FreeOffscreenState();
		pBitmapInfo = NULL;
		pBits = NULL;
	}

	// Make sure we restore the state of the GDrawContext if our client forgot to 
	// turn off the context replacement. See UseGreyscaleContextDangerous(), below.
	if (pPreviousGDrawContext != NULL)
	{
		if (SetTempDrawContext(pPreviousGDrawContext) != pGreyscaleContext)
		{
			ERROR3("Not pGreyscaleContext when restoring");
		}
		pPreviousGDrawContext = NULL;
	}
}



/********************************************************************************************

>	INT32 GRenderBitmap::GetNumBands()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Returns:	The number of strips the banded region will take.
	Purpose:	The bitmap export and printing both need to know how many bands there will
				be when doing banded rendering. Since the bands are of a fixed size in
				GRenderBitmaps we are able to work out how many there will be before hand.
				This function tells you how many bands there would be if you were to
				banded render this region.

********************************************************************************************/

INT32 GRenderBitmap::GetNumBands()
{
	// Find out how big in millipoints we can do
	double TruePixelHeight = 72000.0 / PixelsPerInch; //.MakeDouble();

// Removed due to problems with printing feathers (see comment at top of file)
//	ERROR3IF(ScaleFactor.MakeDouble() != 1.0, "ScaleFactor not 1, see comment at top of grndbmp.cpp");

	// Work out how many scan lines That works out to be
	INT32 Height = (INT32)((double)RegionRect.Height() / TruePixelHeight);

	// count the bands
	INT32 Bands = 0;
	while (Height>0)
	{
		Bands++;
		Height -= FIXEDBANDSIZE;
	}

	// say how many we found
	return Bands;
}

/********************************************************************************************

>	INT32 GRenderBitmap::GetFullRegionHeight()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/95
	Returns:	The total height of the render region.
	Purpose:	The bitmap export needs to know what the total height of the bitmap is
				when doing banded rendering as the region will be created the height of the
				band and not the full height of the bitmap.

********************************************************************************************/

INT32 GRenderBitmap::GetFullRegionHeight()
{
	// Find out the actual size in pixels of this render region
	// Use the same method that is used to construct the matrix and we use in
	// the bitmap options dialog box to work out the size so that we are consistent 
	Matrix Identity;	// default construction is an identity matrix
	WinRect	Rect = OSRenderRegion::BitmapDocRectToWin( Identity, RegionRect, PixelsPerInch );
	return Rect.height;
}	


/********************************************************************************************

>	void GRenderBitmap::SetOverlapBands(BOOL NewVal)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95
	Inputs:		TRUE to start overlapping bands, FALSE to switch overlapping off
	Purpose:	This function allows to you switch band overlapping on and off.
				It is off by default. Bands need to be overlapped when printing to
				stop the error at the edge of dither pattens.

********************************************************************************************/

void GRenderBitmap::SetOverlapBands(BOOL NewVal)
{
	OverlapBands = NewVal;
}

/********************************************************************************************

>	void GRenderBitmap::SetOverlapBands(BOOL NewVal)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/95
	Inputs:		TRUE to render bottom to top, FALSE to render top to bottom
	Purpose:	This function allows to you switch the direction that the bands are rendered
				in.
				It is bottom to top by default. We need to switch the direction when exporting
				as a bitmap as the Accusoft code needs it in both directions.

********************************************************************************************/

BOOL GRenderBitmap::SetRenderBottomToTop(BOOL NewVal)
{
	BOOL OldVal = RenderBottomToTop;
	RenderBottomToTop = NewVal;
	return OldVal;	
}	


/********************************************************************************************

>	virtual BOOL GRenderBitmap::SetFirstBand()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Returns:	TRUE if it worked, FLASE if it failed
	Purpose:	Sets up for banded rendering. This class will always band the region into
				128 pixel high bands.

********************************************************************************************/

BOOL GRenderBitmap::SetFirstBand()
{
	// if we are already banded, or we already have a bitmap, then stop looking
	// SMFIX we do want to come in here twice - once sometimes to do the palette, the second time to do the render
//	if (IsBanded() || lpBitmapInfo!=NULL)
//		return TRUE;

//	ENSURE(GetCaptureDepth()==0, "Can't set a band while there are any captures running");
	ENSURE(GetCaptureDepth()==0 || MasterCaptureIsCurrent(), "Can't set a band while there are any non-master captures running\n");

	// They can be merged for the time being
	CanRegionBeMerged = TRUE;

	// Make sure the InitDevice has been called.
	if (!RenderFlags.ValidDevice)
	{
		// Device needs to be initialised
		if (!InitDevice())
		{
			// Something's gone wrong - inform the user.
			TRACE( _T("InitDevice failed in Render Region"));
			return FALSE;
		}

		// Make sure we don't do this again for this render region
		RenderFlags.ValidDevice = TRUE;
	}

	// Work out how many scan lines there are
	double TruePixelHeight = 72000.0 / PixelsPerInch; //.MakeDouble();
// Removed due to problems with printing feathers (see comment at top of file)
//	ERROR3IF(ScaleFactor.MakeDouble() != 1.0, "ScaleFactor not 1, see comment at top of grndbmp.cpp");
	INT32 Height = (INT32) ((double)(RegionRect.hi.y - RegionRect.lo.y) / TruePixelHeight);

	// Work out how many scan lines we could fit into the available ram
	INT32 MaxScanLines = FIXEDBANDSIZE;

	// See if we can do it
	if (MaxScanLines>=Height)
	{
		// No Banding needed
		IsRegionBanded = FALSE;
		IsWaitingForRAM = FALSE;
		IsLastBand = TRUE;
		return TRUE;
	}
	else
	{
		// Lets band this region then
		IsRegionBanded = TRUE;
		IsWaitingForRAM = FALSE;
		IsLastBand = FALSE;
		CanRegionBeMerged = FALSE;

		// Find out how big in millipoints we can do
		MILLIPOINT MaxHeight = (MILLIPOINT)(TruePixelHeight * (double)MaxScanLines);

		// Set up the new clip rect ready
		DocRect NewClipRect = CurrentClipRect;
		if (RenderBottomToTop)
		{
			// Render from the bottom upwards
			NewClipRect.hi.y = NewClipRect.lo.y + MaxHeight;

			// Sanity check
			if (NewClipRect.hi.y > CurrentClipRect.hi.y)
				NewClipRect.hi.y = CurrentClipRect.hi.y;
		}
		else
		{
			// Render from the top downwards
			NewClipRect.lo.y = NewClipRect.hi.y - MaxHeight;

			// Sanity check
			if (NewClipRect.lo.y < CurrentClipRect.lo.y)
				NewClipRect.lo.y = CurrentClipRect.lo.y;
		}

		// Set the new clip rect up ready for rendering
		SetClipRect(NewClipRect);		
		return TRUE;
	}
}



/********************************************************************************************

>	virtual BOOL GRenderBitmap::GetNextBand()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Returns:	FALSE
	Purpose:	Since this class does not do banded rendering, there are never any more bands

********************************************************************************************/

BOOL GRenderBitmap::GetNextBand()
{
	// Go find the next band, if there is one.
	// Give back the memory we already have
	if (pBitmapInfo!=NULL)
	{
		FreeOffscreenState();
		pBitmapInfo = NULL;
		pBits = NULL;
	}

	ENSURE(GetCaptureDepth()==0, "Can't set a band while there are any captures running");

	// if this is not a banded render region, then there are no more bands
	if (!IsBanded())
		return FALSE;

	if (RenderBottomToTop)
	{
		// Render from the bottom upwards
		// See if we are out of bands
		if ((CurrentClipRect.hi.y >= RegionRect.hi.y) || (IsLastBand))
			return FALSE;
	}
	else
	{
		// Render from the top downwards
		// See if we are out of bands
		if ((CurrentClipRect.lo.y <= RegionRect.lo.y) || (IsLastBand))
			return FALSE;
	}

	// we're going to need to be re-rendered
	IsPaperRendered = FALSE;
	IsInkRenderStarted = FALSE;

	// Get the number of scan lines we are prepared to do
	INT32 MaxScanLines = FIXEDBANDSIZE;

	// Work out how many scan lines there are left to do
	double TruePixelHeight = 72000.0 / PixelsPerInch; //.MakeDouble();
// Removed due to problems with printing feathers (see comment at top of file)
//	ERROR3IF(ScaleFactor.MakeDouble() != 1.0, "ScaleFactor not 1, see comment at top of grndbmp.cpp");

	INT32 Height = 0;
	if (RenderBottomToTop)
	{
		// Render from the bottom upwards
		Height = (INT32) ((double)(RegionRect.hi.y - CurrentClipRect.hi.y) / TruePixelHeight);
	}
	else
	{
		// Render from the top downwards
		Height = (INT32) ((double)(CurrentClipRect.lo.y - RegionRect.lo.y ) / TruePixelHeight);
	}

	// see if there are enought to bother with
	IsWaitingForRAM = FALSE;

	// Do what we can
	INT32 ScanLinesToDo = (MaxScanLines < Height) ? MaxScanLines : Height;
	if (ScanLinesToDo==Height)
		IsLastBand = TRUE;
	else
		IsLastBand = FALSE;

	// Find out how big in millipoints we can do
	MILLIPOINT MaxHeight = (MILLIPOINT) (TruePixelHeight * (double)ScanLinesToDo);

	DocRect NewClipRect = CurrentClipRect;
	if (RenderBottomToTop)
	{
		// Render from the bottom upwards
		// Set up the new clip rect ready
		NewClipRect.lo.y = CurrentClipRect.hi.y;
		NewClipRect.hi.y = NewClipRect.lo.y + MaxHeight;

		// if we want to overlap the bands, make an adjustment
		if (OverlapBands)
			NewClipRect.lo.y -= (INT32)(2 * TruePixelHeight);

		// Sanity check
		if ((NewClipRect.hi.y > RegionRect.hi.y) || (IsLastBand))
			NewClipRect.hi.y = RegionRect.hi.y;
	}
	else
	{
		// Render from the top downwards
		// Set up the new clip rect ready
		NewClipRect.hi.y = CurrentClipRect.lo.y;
		NewClipRect.lo.y = NewClipRect.hi.y - MaxHeight;

		// if we want to overlap the bands, make an adjustment
		if (OverlapBands)
			NewClipRect.hi.y -= (INT32)(2 * TruePixelHeight);

		// Sanity check
		if ((NewClipRect.lo.y < RegionRect.lo.y) || (IsLastBand))
			NewClipRect.lo.y = RegionRect.lo.y;
	}
		
	// Make sure that we are not dealing with an empty rect
	if (NewClipRect.Height()==0)
		return FALSE;

	// Set the new clip rect up ready for rendering
	SetClipRect(NewClipRect);

	// we're going to need to be re-rendered
	IsPaperRendered = FALSE;
	IsInkRenderStarted = FALSE;
	TRACEUSER("Gavin",_T("GRenderBitmap::GetNextBand - RenderFlags.Rendering = FALSE;\n"));
	RenderFlags.Rendering = FALSE;

	// Say it all worked
	return TRUE;
}



/********************************************************************************************

>	LPBITMAPINFO GRenderBitmap::GetLPBits( INT32 Width, INT32 Height, INT32 Depth, LPBYTE*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Inputs:		Width, Height - the width and height of the required bitmap
				Depth - the bpp of the bitmap
	Returns:	Pointer to a bitmap header block
	Purpose:	Allocates a bitmap from the CCMalloc heap

********************************************************************************************/

LPBITMAPINFO GRenderBitmap::GetLPBits( INT32 Width, INT32 Height, INT32 Depth, LPBYTE* lplpBits)
{
	// Get a DIB out of the CCMalloc heap
	LPBITMAPINFO bmInfo = NULL;
	bmInfo = AllocDIB( abs(Width), abs(Height), Depth, lplpBits, NULL, FALSE);

	// return it.
	return bmInfo;
}




/********************************************************************************************

>	void GRenderBitmap::FreeLPBits( LPBITMAPINFO, LPBYTE )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Purpose:	Frees the memory allocated in GetLPBits.

********************************************************************************************/

void GRenderBitmap::FreeLPBits(LPBITMAPINFO lpBMI, LPBYTE lpB)
{
	// Free up the memory. Not from Limited Heap
	FreeDIB(lpBMI, lpB, NULL, FALSE);
}




/********************************************************************************************

>	BOOL GRenderBitmap::DisplayBits(LPBITMAPINFO lpDisplayBitmapInfo = NULL, LPBYTE lpDisplayBits = NULL)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	None really, except that the base class needs one, so we supply it.
	Errors:		-

********************************************************************************************/

BOOL GRenderBitmap::DisplayBits(LPBITMAPINFO lpDisplayBitmapInfo, LPBYTE lpDisplayBits)
{
	return TRUE;
}

/********************************************************************************************

>	OILBitmap *GRenderBitmap::ExtractBitmap(LPLOGPALETTE pPalette)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94
	Returns:	A pointer to a fresh OILBitmap, or NULL if failed. This OILBitmap has
				Attach called on and will stay around after the RR is long gone. Returns
				NULL if something bad happened. The OilBitmap is attached as a tempory bitmap
				so if you want it to hang around as a permanent bmp, then don't forget to
				do something about it.
	Purpose:	So that the caller may do what he likes with the bitmap.

********************************************************************************************/
OILBitmap *GRenderBitmap::ExtractBitmap(LPLOGPALETTE pPalette)
{
	PORTNOTETRACE("other","GRenderBitmap::ExtractBitmap - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (pBitmapInfo && pBits)
	{
		LPBITMAPINFO lpConvInfo;	
		LPBYTE lpConvBits;	

		BOOL DeletePalette = FALSE;

		if(!pPalette)
		{
			pPalette = GetConversionPalette();
			if (pPalette && pPalette != pConvPalette)
				DeletePalette = TRUE;
		}

		BOOL ok = DoOutputBitmapConversion(&lpConvInfo, &lpConvBits, pPalette);
		ERROR3IF(!ok, "Output conversion failed in GRenderBitmap::ExtractBitmap");
		if (!ok) 
		{
			if(DeletePalette)
				CCFree(pPalette);
			return NULL;
		}

		CWxBitmap *OILBM = new CWxBitmap( lpConvInfo, lpConvBits );
		if (OILBM)
		{
			// Setup the Oil bitmap's pallete
			RGBQUAD* pOilPalette = OILBM->BMInfo->bmiColors;
 			PALETTEENTRY* pPaletteEntry = pPalette->palPalEntry; 

			// Copy the palette entries over, we cannot copy a structure at a time because 
			// the bytes are in different orders
			for (DWORD i=0; i< OILBM->BMInfo->bmiHeader.biClrUsed; i++)
			{
				pOilPalette[i].rgbBlue = pPaletteEntry[i].peBlue;
				pOilPalette[i].rgbGreen = pPaletteEntry[i].peGreen;
				pOilPalette[i].rgbRed = pPaletteEntry[i].peRed;
				pOilPalette[i].rgbReserved = pPaletteEntry[i].peFlags;
			}

			if (lpConvBits == pBits)
			{
				// ensure empty offscreen stack
//#ifdef _DEBUG
//				DumpCaptureStack();
//#endif
				ENSURE(MasterCaptureIsCurrent(), "Extract bitmap whilst rendering offscreen not finished!");

				// bytes owned by CWxBitmap now - zero them so we don't free them up
				SetBitmapPointers(NULL, NULL);
			}

			if (lpConvBits == lpOutputBits)
			{
				// bytes owned by CWxBitmap now - zero them so we don't free them up
				lpOutputInfo = NULL;
				lpOutputBits = NULL;
			}

			if(DeletePalette)
				CCFree(pPalette);

			return OILBM;
		}

		if(DeletePalette)
			CCFree(pPalette);
	}
#endif
	return NULL;
}

/********************************************************************************************

>	OILBitmap *GRenderBitmap::ExtractBitmapCopy(LPLOGPALETTE pPalette = NULL, 
												LPBITMAPINFO pMaskInfo = NULL, LPBYTE pMaskData = NULL,
												INT32 MaskColour = -1,
												BOOL LookForDuplicates = FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		pPalette
				pMaskInfo
				pMaskData
				MaskColour
				LookForDuplicates	- if True then see if this bitmap is a duplicate of an existing
									one in the system
	Returns:	A pointer to a fresh OILBitmap, or NULL if failed. This OILBitmap has
				Attach called on and will stay around after the RR is long gone. Returns
				NULL if something bad happened. The OilBitmap is attached as a tempory bitmap
				so if you want it to hang around as a permanent bmp, then don't forget to
				do something about it.

				The bitmap extracted is a COPY of the Regions bitmap data, which is left
				untouched.

	Purpose:	So that the caller may do what he likes with the bitmap.

********************************************************************************************/
OILBitmap *GRenderBitmap::ExtractBitmapCopy(LPLOGPALETTE pPalette, 
											LPBITMAPINFO pMaskInfo, LPBYTE pMaskData, 
											INT32 MaskColour, BOOL LookForDuplicates)
{
	ERROR3IF(MaskColour==-2,"Someone`s using a MaskColour of -2!!! Please Correct!");
	BOOL bBrowserPalette = FALSE;
	
	if(!pPalette)
		bBrowserPalette = TRUE;

	if (pBitmapInfo && pBits)
	{
		LPBITMAPINFO	lpCopyInfo = NULL;
		LPBYTE			lpCopyBits = NULL;
		LPBITMAPINFO	lpConvInfo = NULL;	
		LPBYTE			lpConvBits = NULL;	

		BOOL DeletePalette = FALSE;

		if (!pPalette)
		{
			pPalette = GetConversionPalette();
			DeletePalette =  (pPalette && pPalette != pConvPalette);
		}

		if (pPalette && MaskColour >= 0 && MaskColour < pPalette->palNumEntries)
		{
		 	pPalette->palPalEntry[MaskColour].peRed 	= 255;
		 	pPalette->palPalEntry[MaskColour].peGreen 	= 255;
		 	pPalette->palPalEntry[MaskColour].peBlue 	= 255;
		 	pPalette->palPalEntry[MaskColour].peFlags 	= 255;
		}

		BOOL ok = DoOutputBitmapConversion(&lpConvInfo, &lpConvBits, pPalette);
		ERROR3IF(!ok, "Output conversion failed in GRenderBitmap::ExtractBitmap");
		if (!ok)
		{
			if (DeletePalette)
				CCFree(pPalette);
			return NULL;
		}

		if (lpConvBits == pBits)
		{
			BOOL CopyOK = DIBUtil::CopyBitmap(pBitmapInfo, pBits, &lpCopyInfo, &lpCopyBits);

			if (!CopyOK)
			{
			 	ERROR3("Failed to copy bitmap in ExtractBitmapCopy");
				if (DeletePalette)
					CCFree(pPalette);
				return NULL;
			}
		}
		else
		{
			lpCopyInfo = lpConvInfo;
			lpCopyBits = lpConvBits;
		}

		if (pMaskInfo && pMaskData && MaskColour>=0)
		{
		 	// We've been given a mask for the bitmap ...
			if (!DIBUtil::MakeTransparentBitmap(lpCopyInfo, lpCopyBits, pMaskInfo, pMaskData, MaskColour))
			{
			 	ERROR3("Failed to apply mask in GRenderBitmap::ExtractBitmapCopy");
				if (DeletePalette)
					CCFree(pPalette);
				FreeDIB(lpCopyInfo, lpCopyBits);
				return NULL;
			}
		}

		CWxBitmap *OILBM = new CWxBitmap( lpCopyInfo, lpCopyBits );
		if (OILBM)
		{
			//  Make sure that the bitmap has a width and height which are an integral
			//  number of pixels.
			INT32 width  = (INT32)(( OILBM->BMInfo->bmiHeader.biWidth ) * 72000 / 96.0);
			INT32 height = (INT32)(( OILBM->BMInfo->bmiHeader.biHeight ) * 72000 / 96.0);
			OILBM->SetRecommendedWidth( width );
			OILBM->SetRecommendedHeight( height );

			// Setup the Oil bitmap's pallete
			RGBQUAD* pOilPalette = OILBM->BMInfo->bmiColors;
			// Only change the palette if there is on already present
			// REMEMBER bitmaps with more than 8bpp generally do not have palettes
			if (pPalette != NULL)
			{
				// This will be the optimised palette
				INT32 ColoursInPal = pPalette->palNumEntries;
				// The optimised palette will NOT contain the transparent colour
				// Therefore, if we have a transparent colour then increment our count
				if (MaskColour >= 0)
				{
					// We have to ensure that the number of colours we are requesting will
					// be enough to contain the transparent colour index.
					if (MaskColour > ColoursInPal)
					{
						// Not enough entries in out palette
						// So expand it to include the mask colour
						ColoursInPal = MaskColour + 1;
					}
					else
						ColoursInPal++;
				}

				// The bitmap header will contain the colour depth number of colours
				INT32 MaxColours = OILBM->BMInfo->bmiHeader.biClrUsed;
				// Ensure that we have not gone over this limit
				if (ColoursInPal > MaxColours)
					ColoursInPal = MaxColours;

				// Copy the palette entries over, we cannot copy a structure at a time because 
				// the bytes are in different orders
	 			PALETTEENTRY* pPaletteEntry = pPalette->palPalEntry;
				for (INT32 i = 0; i < MaxColours; i++)
				{
					if (i == MaskColour)
					{
						// Force the masked colour to White, so it looks right
						// even if we can't render it with transparency
						pOilPalette[i].rgbBlue 	= 255;
						pOilPalette[i].rgbGreen = 255;
						pOilPalette[i].rgbRed 	= 255;
						pOilPalette[i].rgbReserved = 255;
					}
					else if (i > ColoursInPal)
					{
						// Force any unused colours to be black
						pOilPalette[i].rgbBlue 	= 0;
						pOilPalette[i].rgbGreen = 0;
						pOilPalette[i].rgbRed 	= 0;
						pOilPalette[i].rgbReserved = 0;
					}
					else
					{
						pOilPalette[i].rgbBlue 	= pPaletteEntry[i].peBlue;
						pOilPalette[i].rgbGreen = pPaletteEntry[i].peGreen;
						pOilPalette[i].rgbRed 	= pPaletteEntry[i].peRed;
						pOilPalette[i].rgbReserved = 0;
					}
				}

				// Only set up the required number of palette entries
				OILBM->BMInfo->bmiHeader.biClrUsed = ColoursInPal;
			} 

			if (lpCopyBits == pBits)
			{
				// bytes owned by CWxBitmap now - zero them so we don't free them up
				SetBitmapPointers(NULL, NULL);

				// ensure empty offscreen stack
				ENSURE(MasterCaptureIsCurrent(), "Extract bitmap whilst rendering offscreen not finished!");
			}

			if (lpCopyBits == lpOutputBits)
			{
				// bytes owned by CWxBitmap now - zero them so we don't free them up
				lpOutputInfo = NULL;
				lpOutputBits = NULL;
			}

			if (LookForDuplicates)
			{
				// This check was not here before 25/4/97
				// Check to see if this bitmap exists in the global bitmap list
				// This will actually check the bitmap itself to see if it is the same
				// rather than doing a simple and explicit pointer check.
				GlobalBitmapList * pBmpList = GetApplication()->GetGlobalBitmapList();
				OILBitmap* pExistingBmp = NULL;
				if (pBmpList)
					pExistingBmp = pBmpList->FindDuplicateBitmap(OILBM);
				if (pExistingBmp)
				{
					TRACEUSER( "Neville", _T("This bitmap already exists.  Re-using existing OILBitmap.\n"));
					// There's already an existing OIL bitmap we can use,
					// so we'll make a new kernel bitmap out of it ...
					if (DeletePalette)
						CCFree(pPalette);
					delete OILBM;
					return pExistingBmp;
				}
			}
			if (DeletePalette)
				CCFree(pPalette);

			return OILBitmap::Attach(OILBM, FALSE);
		}

		if(DeletePalette)
			CCFree(pPalette);
	}

	return NULL;
}

/********************************************************************************************

>	BOOL GRenderBitmap::GetBitmapData(LPBITMAPINFO* pBitmapInfo, LPBYTE* pBitmapData, BOOL Convert)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Returns:	-
	Purpose:	-

********************************************************************************************/

BOOL GRenderBitmap::GetBitmapData(LPBITMAPINFO* pBmpInfo, LPBYTE* pBmpData, BOOL Convert)
{
	ERROR3IF(pBmpInfo == NULL, "NULL info pointer passed to GetBitmapData");
	if (pBmpInfo == NULL)
		return FALSE;

	ERROR3IF(pBmpData == NULL, "NULL data pointer passed to GetBitmapData");
	if (pBmpData == NULL)
		return FALSE;

	if (!Convert)
	{
		*pBmpInfo = pBitmapInfo;
		*pBmpData = pBits;
		
		if(m_bEnableConversion && m_DoCompression)
		{
			UINT32 Width = pBitmapInfo->bmiHeader.biWidth;
			UINT32 Height = pBitmapInfo->bmiHeader.biHeight;
			BYTE* lpOutput = *pBmpData;

			DIBConvert* Converter = DIBConvert::Create( 32, 32, Width, NULL, 0 );

			if(Converter != NULL)
			{
				Converter->Convert(pBits, lpOutput, Height);
				delete Converter;
				Converter = NULL;

				// Force the bitmap to be reinitialised when StartRender is next called
				ForceInitBmpBits = TRUE;

				ENSURE(GetMasterCapture()==NULL || MasterCaptureIsCurrent(), "Bitmap converted in-place while captures are running\n");
			}
		}

		return TRUE;
	}

	LPBITMAPINFO lpConvInfo=NULL;	
	LPBYTE lpConvBits=NULL;	

	LPLOGPALETTE pPalette = GetConversionPalette();

	BOOL ok = DoOutputBitmapConversion(&lpConvInfo, &lpConvBits, pPalette);
	ERROR3IF(!ok, "Output conversion failed in GRenderBitmap::GetBitmapData");

	*pBmpInfo = lpConvInfo;
	*pBmpData = lpConvBits;

	if (pPalette != pConvPalette)
	 	CCFree(pPalette);

	return ok;
}

/********************************************************************************************

>	BOOL GRenderBitmap::DoOutputBitmapConversion(LPBITMAPINFO* lpConvInfo, LPBYTE* lpConvBits, LPLOGPALETTE pPalette)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Returns:	-
	Purpose:	-

********************************************************************************************/

BOOL GRenderBitmap::DoOutputBitmapConversion(LPBITMAPINFO* lpConvInfo, LPBYTE* lpConvBits, LPLOGPALETTE pPalette)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	ERROR3IF(lpConvInfo == NULL, "NULL info pointer passed to GRenderBitmap::DoOutputBitmapConversion");
	if (lpConvInfo == NULL)
		return FALSE;

	ERROR3IF(lpConvBits == NULL, "NULL data pointer passed to GRenderBitmap::DoOutputBitmapConversion");
	if (lpConvBits == NULL)
		return FALSE;

	if (OutputDepth == BitmapDepth)
	{
		// No conversion is needed !!
		*lpConvInfo = lpBitmapInfo;
		*lpConvBits = lpBits;

		if(m_bEnableConversion && m_DoCompression)
		{
			UINT32 Width = lpBitmapInfo->bmiHeader.biWidth;
			UINT32 Height = lpBitmapInfo->bmiHeader.biHeight;
			BYTE* lpOutput = lpBits;

			DIBConvert* Converter = DIBConvert::Create( 32, 32, Width, NULL, 0 );

			if(Converter != NULL)
			{
				Converter->Convert(lpBits, lpOutput, Height);
				delete Converter;
				Converter = NULL;

				// Force the bitmap to be reinitialised when StartRender is next called
				ForceInitBmpBits = TRUE;

				ENSURE(GetMasterCapture()==NULL || MasterCaptureIsCurrent(), "Bitmap converted in-place while captures are running\n");
			}
		}

		return TRUE;
	}

	// Blank the pointer in case of an error
	*lpConvInfo = NULL;
	*lpConvBits = NULL;

	// We must need to do some Bitmap conversion then ....
	UINT32 Width = lpBitmapInfo->bmiHeader.biWidth;
	UINT32 Height = lpBitmapInfo->bmiHeader.biHeight;

	// Make a converter object
	DIBConvert* Converter = DIBConvert::Create( BitmapDepth, OutputDepth, Width, pPalette, DitherType );
	ERROR3IF(Converter == NULL, "Failed to create DIBConvert object in GRenderBitmap::DoOutputBitmapConversion");
	if (Converter == NULL)
		return FALSE;

	if (lpOutputInfo != NULL || lpOutputBits != NULL)
	{
		// Free the existing bitmap data
		FreeDIB(lpOutputInfo, lpOutputBits);

		lpOutputInfo = NULL;
		lpOutputBits = NULL;
	}

	// Allocate some memory for the converted bitmap
	lpOutputInfo = AllocDIB(Width, Height, OutputDepth, &lpOutputBits);
	ERROR3IF(lpOutputInfo == NULL, "Failed to allocate conversion info in GRenderBitmap::DoOutputBitmapConversion");
	ERROR3IF(lpOutputBits == NULL, "Failed to allocate conversion data in GRenderBitmap::DoOutputBitmapConversion");
	if (lpOutputInfo == NULL || lpOutputBits == NULL)
		return FALSE;

	// Do the conversion
	BOOL ok = Converter->Convert(lpBits, lpOutputBits, Height);
	delete Converter;

	if (ok)
	{
		*lpConvInfo = lpOutputInfo;
		*lpConvBits = lpOutputBits;
	}

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	LPLOGPALETTE GRenderBitmap::GetConversionPalette()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/96
	Returns:	-
	Purpose:	-

********************************************************************************************/

LPLOGPALETTE GRenderBitmap::GetConversionPalette()
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	if (OutputDepth>8)
		return NULL;	// Must be 8bpp or less

 	if (pConvPalette)
		return pConvPalette;	// Use specified palette if one exists

	//ASSERT(FALSE); // we should have a palette!! sjk SMFIX
	// most times we should have a palette but for now animated gifs like to use this code so we will leave it for now

	LPLOGPALETTE pPalette;
	size_t TotalPal;

	switch (OutputDepth)
	{
		case 8:
			// Get the standard 256 colour palette
			TotalPal = sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * 256 );
			pPalette = (LPLOGPALETTE)CCMalloc( TotalPal );
			if (pPalette != NULL)
			{
				LPLOGPALETTE pRecPalette = GetErrorDiffPalette();
				if (pRecPalette)
				{
					memcpy(pPalette, pRecPalette, TotalPal);
				}
				else
				{
					CCFree(pPalette);
					return NULL;
				}
			}
			break;

		case 4:
			TotalPal = sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * 16 );
			pPalette = (LPLOGPALETTE)CCMalloc( TotalPal );
			if (pPalette != NULL)
			{
				// Make a standard 16 colour palette
				OutputDIB::Fix16ColourPalette(pPalette);
			}
			break;

		case 1:
			TotalPal = sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * 2 );
			pPalette = (LPLOGPALETTE)CCMalloc( TotalPal );
			if (pPalette != NULL)
			{
				// Make a simple black and white palette
				OutputDIB::FixBlackAndWhitePalette(pPalette);
			}
			break;

		default:
			ERROR3("Bad colour depth in GRenderBitmap::GetConversionPalette");
			pPalette = NULL;
			break;
	}

	return pPalette;
#else
	return NULL;
#endif
}



/********************************************************************************************

>	void GRenderBitmap::UseGreyscaleContextDangerous(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/97

	Purpose:	DANGEROUS! Don't use this unless you really know what you're doing

				Replaces the static GRenderRegion::GD context with a new Greyscale
				paletted one for generating 8bpp greyscale images with. This has two
				purposes -
					1. to allow creation of 8bpp greyscale images without having to
					   allocate a 32bp bitmap or anything
					2. to allow a GRenderRegion-derived render region to be used to
					   render inside the rendering loop of another GRenderRegion.

				This is currently used by the airbrush path stroking code to draw
				transparency information into an 8bpp greyscale bitmap, on the fly,
				during normal GDraw rendering. If you want to do something similar,
				take a look at that code, because great care must be taken when using
				one GRndRgn inside the other to make sure no corruption occurs.

				There is only one symptom of getting it wrong: a nasty access violation.

				Before calling StartRender on your new GRenderBitmap, call this function
				to install the new context. Once finished, you must restore the normal
				GRenderRegion state by calling StopUsingGreyscaleContextDangerous()

********************************************************************************************/

void GRenderBitmap::UseGreyscaleContextDangerous(void)
{
	TRACEUSER( "Gerry", _T("GRenderBitmap::UseGreyscaleContextDangerous\n"));
	ERROR3IF(pPreviousGDrawContext != NULL, "UseGreyscaleContextDangerous called while context in use");
	if (pPreviousGDrawContext == NULL)
	{
		if (pGreyscaleContext == NULL)
		{
PORTNOTE("other","GRenderBitmap::UseGreyscaleContextDangerous - removed progress reporting")
#if !defined(EXCLUDE_FROM_XARALX)
			Progress::Start();
#endif

			// Create a new context the first time anyone actually wants one
			pGreyscaleContext = new GDrawAsm;

			if (pGreyscaleContext != NULL)
			{
//				pGreyscaleContext->Init();
				
				// And initialise the palette - this can unfortunately take a couple of seconds
				DWORD PaletteFlag = 0;
				if (WhichDither == 0 || WhichDither == 3)
					PaletteFlag = 1;

				if (pGreyscaleContext->SelectPalette(PaletteFlag) != NULL)
					pGreyscaleContext->InitialiseWithPalette(pConvPalette);
			}

#if !defined(EXCLUDE_FROM_XARALX)
			Progress::Stop();
#endif
		}

		if (pGreyscaleContext != NULL)
		{
			pPreviousGDrawContext = SetTempDrawContext(pGreyscaleContext);
		}
	}
}



/********************************************************************************************

>	void GRenderBitmap::StopUsingGreyscaleContextDangerous(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/97

	Purpose:	DANGEROUS! Don't use this unless you really know what you're doing

				See StartUsingGreyscaleContextDangerous()

********************************************************************************************/

void GRenderBitmap::StopUsingGreyscaleContextDangerous(void)
{
	TRACEUSER( "Gerry", _T("GRenderBitmap::StopUsingGreyscaleContextDangerous\n"));
	ERROR3IF(pPreviousGDrawContext == NULL, "UseGreyscaleContextDangerous called while context not in use");
	if (pPreviousGDrawContext != NULL)
	{
		if (SetTempDrawContext(pPreviousGDrawContext) != pGreyscaleContext)
		{
			ERROR3("Not pGreyscaleContext when restoring");
		}
		pPreviousGDrawContext = NULL;
	}
}



/********************************************************************************************

>	static void GRenderBitmap::Deinit(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/97

	Purpose:	De-initialises the GRenderBitmap class.
				Mainly used to stop memory leaks after someone has been using the
				nasty greyscale context bodge (See StartUsingGreyscaleContextDangerous())

********************************************************************************************/

void GRenderBitmap::Deinit(void)
{
	ERROR3IF(GD != NULL && GD == pGreyscaleContext, "GRenderBitmap greyscale context still in use");

	if (pGreyscaleContext != NULL)
	{
		delete pGreyscaleContext;
		pGreyscaleContext = NULL;
	}
}









#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

/********************************************************************************************

	GRenderOptPalette ... a special Render Region for generating an optimised palette for
	                      use during export 

********************************************************************************************/


/********************************************************************************************

>	GRenderOptPalette::GRenderOptPalette(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi, BOOL Use8bpp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GRenderOptPalette constructor.
	Errors:		-
	SeeAlso:	GRenderBitmap::GRenderBitmap

********************************************************************************************/
GRenderOptPalette::GRenderOptPalette(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi, BOOL Use8bpp)
	: GRenderBitmap( ClipRegion, ConvertMatrix, ViewScale, Depth, dpi)
{
	ERROR3IF(Depth != 32, "Bad Depth for GRenderOptPalette, must be 32bpp");

	UseSpecial8bpp = Use8bpp;

	m_pExactPalette = NULL;

	if (Depth == 32)
	{
		if (UseSpecial8bpp)
		{
			Stats = CCMalloc(DIBUtil::GetOptimal8bppPaletteWorkspaceSize() * 4);
			if (Stats)
				DIBUtil::Optimal8bppPaletteInitialise((INT32*)Stats);
		}
		else
		{
			Stats = CCMalloc(DIBUtil::GetOptimalPaletteWorkspaceSize());
			if (Stats)
				DIBUtil::OptimalPaletteInitialise(Stats);

		}
		if( !UseOldPalette )
		{
			const size_t TotalPal = sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * 256 );
			m_pExactPalette = (LPLOGPALETTE)CCMalloc( TotalPal );
			if( m_pExactPalette )
				DIBUtil::ExactPaletteInitialise( m_pExactPalette );
		}
	}

	m_bTooManyColours = FALSE;

	//  Re-initialise
	DoGDrawConversion = TRUE;
}

/********************************************************************************************

>	GRenderOptPalette::~GRenderOptPalette()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GRenderOptPalette destructor
	Errors:		-

********************************************************************************************/

GRenderOptPalette::~GRenderOptPalette()
{
	if (Stats)
		CCFree(Stats);
	// GAT
	if (m_pExactPalette)
		CCFree(m_pExactPalette) ;
}

/********************************************************************************************

>	BOOL GRenderOptPalette::GetNextBand()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Passes this band to Gavin's palette optimisation routine, before we move
				onto the next band
	Errors:		-

********************************************************************************************/

BOOL GRenderOptPalette::GetNextBand()
{
	// Before we move onto the next band, we'll pass the current strip to Gavin's
	// palette optimising code
	if (Stats)
	{
		INT32 Width = lpBitmapInfo->bmiHeader.biWidth;
		INT32 Height = lpBitmapInfo->bmiHeader.biHeight;

		INT32 BmpSize = Width * Height;

		//  Calculate the number of colours used by this strip of the bitmap.
		//  But only if we have not gone over 256 colours yet
		// GAT
		if( !m_bTooManyColours && !UseOldPalette )
		{
			BOOL bResult = DIBUtil::CalculateNumberOfColoursInBitmap( m_pExactPalette, ( RGBQUAD* )lpBits, BmpSize ); 
			if( !bResult )
			{
				m_bTooManyColours = TRUE;
				DoGDrawConversion = FALSE;
			}
		}

		BOOL ok;
		if (UseSpecial8bpp)
			ok = DIBUtil::GenOptimal8bppPaletteStats( (INT32*)Stats, (RGBQUAD*)lpBits, BmpSize );
		else
			ok = DIBUtil::GenOptimalPaletteStats( Stats, (RGBQUAD*)lpBits, BmpSize );

		ERROR3IF(!ok, "GenOptimalPaletteStats failed");
	}

	return GRenderBitmap::GetNextBand();									 	
}

/********************************************************************************************

>	LPLOGPALETTE GRenderOptPalette::GetOptimisedPalette(UINT32 PaletteSize, UINT32 NumColours, UINT32 ReservedColours = 0, BOOL SnapToBrowserPalette = FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (modified by MarkN)
	Created:	29/5/96
	Inputs:		PaletteSize		= Max number of entries that the palette can hold (should be 2, 16 or 256)
				NumColours		= Number of colours to put in the palette
				ReservedColours = , number of colours to reserve (eg. for transparency)
				SnapToBrowserPalette = TRUE if palette should be snapped to the browser palett
	Outputs:	-
	Returns:	A pointer to the optimised palette, or NULL if failed
				
				The reserved colours will appear as follows ...
				1bpp, at end of the palette,
				4bpp, at the start of the palette,
				8bpp, at the 11 entry in the palette (10 windows colours are always at the start).

				WEBSTER Changes (markn 16/1/97):
				This function has been modified so that you can specify the number colours you want in the
				palette
				
				WEBSTER v2 changes (markn 22/7/97)

	Purpose:	Generates an optimised palette from the Statistics generated from each band
				rendered so far
	Errors:		-

********************************************************************************************/

LPLOGPALETTE GRenderOptPalette::GetOptimisedPalette(UINT32 PaletteSize, UINT32 NumColours, UINT32 ReservedColours, BOOL SnapToBrowserPalette)
{
	PORTNOTETRACE("other","GRenderOptPalette::GetOptimisedPalette - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(!(PaletteSize==2 || PaletteSize==16 || PaletteSize==256), "Bad palette size passed to GRenderOptPalette::GetOptimisedPalette");
	ERROR3IF(ReservedColours > PaletteSize, "Too many colours reserved in GRenderOptPalette::GetOptimisedPalette");

	LPLOGPALETTE pPalette = NULL;

	if (Stats)
	{
		const size_t TotalPal = sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * PaletteSize );
		pPalette = (LPLOGPALETTE)CCMalloc( TotalPal );
		if (pPalette)
		{
			// Initialise the palette with zeros
			memset(pPalette, 0, TotalPal);

			pPalette->palVersion 	= 0x300;
			pPalette->palNumEntries = PaletteSize;

			if (NumColours > PaletteSize)
				NumColours = PaletteSize;

			if (ReservedColours < NumColours)
			{
				BOOL ok = TRUE;
				// GAT
				//  If the bitmap has less than 256 colours, then we want to bypass the 
				//  optimisation code, and just use the exact palette.
				if( !m_bTooManyColours && m_pExactPalette && !UseOldPalette 
					&& m_pExactPalette->palNumEntries < BmapPrevDlg::m_NumOfColoursUserAskedFor ) // and we want this many colours
				{
					//  Transfer the exact palette into the variable we want, and then clear up the memory.
					INT32 blackEntry = 10;
					pPalette->palNumEntries = m_pExactPalette->palNumEntries;
					for( INT32 i = 0; i < m_pExactPalette->palNumEntries; i++ )
					{
						pPalette->palPalEntry[i].peRed	 = m_pExactPalette->palPalEntry[i].peRed;
						pPalette->palPalEntry[i].peGreen = m_pExactPalette->palPalEntry[i].peGreen;
						pPalette->palPalEntry[i].peBlue	 = m_pExactPalette->palPalEntry[i].peBlue;

						if (pPalette->palPalEntry[i].peRed == 255 &&
							pPalette->palPalEntry[i].peGreen == 255 &&
							pPalette->palPalEntry[i].peBlue == 255 )
							blackEntry = i;
					}
					if (blackEntry != 10 && ReservedColours == 1)
					{
						// swap the black entry for number 10 as this is the transparent entry for some odd reason
						pPalette->palPalEntry[blackEntry].peRed = pPalette->palPalEntry[10].peRed;
						pPalette->palPalEntry[blackEntry].peGreen = pPalette->palPalEntry[10].peGreen;
						pPalette->palPalEntry[blackEntry].peBlue = pPalette->palPalEntry[10].peBlue;

						pPalette->palPalEntry[10].peRed = 255;
						pPalette->palPalEntry[10].peGreen = 255;
						pPalette->palPalEntry[10].peBlue = 255;
					}
					for( ; i<0x100 ; i++ )
					{
						pPalette->palPalEntry[i].peRed	 =
						pPalette->palPalEntry[i].peGreen =
						pPalette->palPalEntry[i].peBlue	 = 0 ;
					}

//					BmapPrevDlg::m_bNeedPaletteUpdated = TRUE;
					BmapPrevDlg::m_bUseExistingPalette = FALSE;
				}
				else if (UseSpecial8bpp)
				{
					if (PaletteSize != 256)
					{
						ERROR3("Palette size is wrong for an 8bpp bitmap");
						CCFree( pPalette );
						return NULL;
					}

					// There has to be at least 20 colours in the palette to accomodate the system colours
					if (NumColours < 20)
						NumColours = 20;

					// Gavin adds 16 system colours, then later we remove these and add the full set of 20 system colours
					// Therefore, get Gavin to generate 4 fewer colours so we end up with the correct number of
					// colours in the palette.
					NumColours -= 4;

					// We can only generate a maximum of 252 colours (i.e. (256-20)+16 = 252)
					if (NumColours > 252)
						NumColours = 252;

					// Leave room for the reserved colours
					NumColours -= ReservedColours;

					ok = DIBUtil::GenOptimal8bppPalette((INT32*) Stats, pPalette, NumColours);
				}
				else
				{
					// Leave room for the reserved colours
					NumColours -= ReservedColours;

					ok = DIBUtil::GenOptimalPalette( Stats, pPalette, (NumColours==1) ? 2 : NumColours );
				}
				// GAT
				if( m_pExactPalette )
				{
					//  Get some memory back.
					CCFree( m_pExactPalette );
					m_pExactPalette = NULL;
				}
			
				// WEBSTER - markn - 22/7/97
				if (ok && SnapToBrowserPalette)
				{
					PaletteManager::SnapToBrowserPalette(pPalette);
				}

				if (ok)
				{
					// Now we need to shift the colours about a bit ....
					switch (PaletteSize)
					{
						INT32 index;

					 	case 2:
							if (NumColours == 1)
							{
							 	// 1 colour needs some special treatment
								if (pPalette->palPalEntry[1].peRed   == 0xFF &&
									pPalette->palPalEntry[1].peGreen == 0xFF &&
									pPalette->palPalEntry[1].peBlue  == 0xFF)
								{
									// First entry is white
								 	pPalette->palPalEntry[1] = pPalette->palPalEntry[0];
								 	pPalette->palPalEntry[1].peFlags = 0;


								 	pPalette->palPalEntry[0].peRed   = 255;
								 	pPalette->palPalEntry[0].peGreen = 255;
								 	pPalette->palPalEntry[0].peBlue  = 255;
								 	pPalette->palPalEntry[0].peFlags = 255;		// This makes Gavin ignore this
																				// colour when converting the bitmap
								}
								else if	(pPalette->palPalEntry[0].peRed   == 0xFF &&
										 pPalette->palPalEntry[0].peGreen == 0xFF &&
										 pPalette->palPalEntry[0].peBlue  == 0xFF)
								{
									// Second entry is white
								 	pPalette->palPalEntry[0].peFlags = 255;		// This makes Gavin ignore this
																				// colour when converting the bitmap
								}
								else
								{
									// Neither entry is white !
								 	pPalette->palPalEntry[0].peRed   = 255;
								 	pPalette->palPalEntry[0].peGreen = 255;
								 	pPalette->palPalEntry[0].peBlue  = 255;
								 	pPalette->palPalEntry[0].peFlags = 255;		// This makes Gavin ignore this
																				// colour when converting the bitmap
								}
								
								pPalette->palNumEntries = 2;
							}
							else
								pPalette->palNumEntries += ReservedColours;
							break;

					 	case 16:
							// Shift the palette entries down, so the reserved colours are at the start
							for (index = NumColours - 1; index >= 0; index--)
							{
							 	pPalette->palPalEntry[index + ReservedColours] = pPalette->palPalEntry[index];
							}

							// Blank the reserved colours to white
							for (index = 0; index < INT32(ReservedColours); index++)
							{
							 	pPalette->palPalEntry[index].peRed   = 255;
							 	pPalette->palPalEntry[index].peGreen = 255;
							 	pPalette->palPalEntry[index].peBlue  = 255;
							 	pPalette->palPalEntry[index].peFlags = 255;		// This makes Gavin ignore this
																				// colour when converting the bitmap
							}

							pPalette->palNumEntries += ReservedColours;
							break;

					 	case 256:
							#ifdef _BATCHING
							{
									for (UINT32 index = 0;index < NumColours;index++)
									{
						 				BYTE R = pPalette->palPalEntry[index].peRed;
						 				BYTE G = pPalette->palPalEntry[index].peGreen;
						 				BYTE B = pPalette->palPalEntry[index].peBlue;
										if (R == 254 && G == 254 && B == 254)
										{
						 					pPalette->palPalEntry[index].peRed = 255;
						 					pPalette->palPalEntry[index].peGreen = 255;
						 					pPalette->palPalEntry[index].peBlue = 255;
										}											
									}
							}
							#endif
							
							if (UseSpecial8bpp && !( !m_bTooManyColours && !UseOldPalette ) )
							{
								LPLOGPALETTE pWinPalette;

								// Now we move the colours.
								UINT32 CurStart = 16;
								UINT32 NewStart = 10 + ReservedColours;

								if (CurStart < NumColours)
								{
									BOOL ColoursMoved = FALSE;

									UINT32 NumColsToMove = NumColours - CurStart;
									ERROR3IF((NumColsToMove+CurStart) > 256,"No room for system colours");
									ERROR3IF((NumColsToMove+NewStart) > 256,"No room for system colours");
									
									if (NewStart < CurStart)
									{
										// Move the non-system colours down
										for (index = 0; index < INT32(NumColsToMove); index++)
							 				pPalette->palPalEntry[index + NewStart] = pPalette->palPalEntry[index + CurStart];

										// Blank out unused colours that are not system colours
										for (index = NumColsToMove+NewStart;index < 246;index++)
										{
							 				pPalette->palPalEntry[index].peRed   = 0;
							 				pPalette->palPalEntry[index].peGreen = 0;
							 				pPalette->palPalEntry[index].peBlue  = 0;
										}

										ColoursMoved = TRUE;
									}

									ERROR3IF(!ColoursMoved,"Rewrite code to accomodate system colours");
								}

								ERROR3IF(CurStart > NumColours,"Not enough room for system colours");

								// Now fill in the rest of the palette with the 20 system colours
								// (10 at the start, and 10 at the end)
								pWinPalette = GetErrorDiffPalette();

								for (index = 0; index < 10; index++)
								{
							 		pPalette->palPalEntry[index] = pWinPalette->palPalEntry[index];
								}

								for (index = 246; index < 256; index++)
								{
							 		pPalette->palPalEntry[index] = pWinPalette->palPalEntry[index];
								}
							
								// Blank the reserved colours to white
								for (index = 10; index < INT32(10 + ReservedColours); index++)
								{
							 		pPalette->palPalEntry[index].peRed   = 255;
							 		pPalette->palPalEntry[index].peGreen = 255;
							 		pPalette->palPalEntry[index].peBlue  = 255;
							 		pPalette->palPalEntry[index].peFlags = 255;		// This makes Gavin ignore this
																					// colour when converting the bitmap
								}
							}
							else
							{
								// WEBSTER - markn 15/2/97
								// If there's a reserved colour (i.e. creating a transparent bitmap)
								// make sure Gavin doen't use it
								// To do this, shift all the colours up one place, and flag the reserved colour
								// (hard wired to 10 for some reason unknown to myself) so Gavin ignores it
								if (ReservedColours == 1)
								{
									for (index = 255;index > 10;index--)
							 			pPalette->palPalEntry[index] = pPalette->palPalEntry[index-1];

							 		pPalette->palPalEntry[10].peRed   = 255;
							 		pPalette->palPalEntry[10].peGreen = 255;
							 		pPalette->palPalEntry[10].peBlue  = 255;
							 		pPalette->palPalEntry[10].peFlags = 255;		// This makes Gavin ignore this
								}

								ERROR3IF(ReservedColours > 1,"This code won't work with more than one reserved colour");

								// As this code is assuming that the transparent colour index is 10,
								// if the user has chosen less than 10 colours, we need to ensure that
								// the palette actually includes the transparent colour index. Otherwise,
								// the GIF export code will assume it can output the requested colours using
								// the minimum bpp. E.g. user request 2 colours, output will be 1 bpp and
								// so the 10 will cause problems.
								// This fixing of the number of entries is actually done in ExtractBitmapCopy
							}
							break;
						default:
							ERROR3("Bad palette size in GRenderOptPalette::GetOptimisedPalette");
					}
				}
				else
				{
				 	ERROR3("Failed to generate optimised palette");
					CCFree( pPalette );
					pPalette = NULL;
				}
			}
		}

		// Reinitialise the Stats
		if (UseSpecial8bpp)
			DIBUtil::Optimal8bppPaletteInitialise((INT32*)Stats);
		else
			DIBUtil::OptimalPaletteInitialise(Stats);
	}

 	return pPalette;
#else
	return NULL;
#endif
}
#endif
