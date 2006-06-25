// $Id: wxOil/grndprnt.cpp, 1, 01-Jan-2006, Anonymous $
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

// A GDraw render region for printing.

/*
*/

#include "camtypes.h"

#include "grndprnt.h"

#include "colcontx.h"
#include "colplate.h"
#include "dibutil.h"
#include "fixmem.h"
#include "osrndrgn.h"
#include "view.h"
#include "oilbitmap.h"

CC_IMPLEMENT_DYNAMIC(GRenderPrint, GRenderDIB)

DECLARE_SOURCE("$Revision: 3 $");

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	GRenderPrint::GRenderPrint(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, 
							   UINT32 Depth, double dpi)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/95
	Inputs:		-
	Purpose:	GRenderPrint constructor. Doesn't do anything actually, just calls base class.
				INT32 dpi changed to double dpi (12/12/95) to improve the range of values allowed
				at the < 1000dpi settings that we will be using.
	SeeAlso:	GRenderRegion::GRenderRegion

********************************************************************************************/

GRenderPrint::GRenderPrint(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, 
						   UINT32 Depth, double dpi)
	: GRenderDIB( ClipRegion, ConvertMatrix, ViewScale, Depth, dpi)
{
	// If nobody has found us a sensible colour context yet, we'll just go for a global
	// default context
	if (CurrentColContext == NULL)
		CurrentColContext = ColourContext::GetGlobalDefault(COLOURMODEL_RGBT);

	// We are printing
	RenderFlags.Printing = TRUE;

	// We have not poked about in the RenderView's ColourPlates (yet)
	OldPlateDisabledState = FALSE;
	HaveDisabledPlate = FALSE;

	TRACEUSER( "Tim", _T("ClipRegion = (%ld, %ld), (%ld, %ld)\n"),
			  ClipRegion.lo.x, ClipRegion.lo.y,
			  ClipRegion.hi.x, ClipRegion.hi.y);
}



/********************************************************************************************

>	virtual BOOL GRenderPrint::StartRender()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/95
	Returns:	TRUE if worked, FALSE if failed

	Purpose:	Calls GRenderDIB::StartRender
				Initialises the bitmap to white (0xff)
				Disables colour separation (this is done as a post-process on the
				final bitmap in DisplayBits())

	SeeAlso:	GRenderDIB::StartRender

********************************************************************************************/

BOOL GRenderPrint::StartRender()
{
	// call base class first
	const BOOL ok = GRenderDIB::StartRender();
	if (!ok)
		return FALSE;

	// we need a white background, which on non-palette devices (all we can cope
	// with currently) is 0xFF a lot.
	memset( pBits, 0xFF, pBitmapInfo->bmiHeader.biSizeImage );

	// If we're colour separating then we want to DISABLE separations for the entire
	// rendering-to-a-bitmap process, and we will then colour separate the resultant
	// bitmap as it is output. We remember the previous ColourPlate disabled state
	// so that we can restore it in StopRender (well, in DisplayBits() in fact)
	ERROR3IF(RenderView != NULL && RenderView->GetColourPlate() != NULL &&
			 HaveDisabledPlate && !RenderView->GetColourPlate()->IsDisabled(),
			 "ColourPlate seems to have myseteriously become re-enabled in GRenderPrint");
	if (!HaveDisabledPlate && RenderView != NULL && RenderView->GetColourPlate() != NULL)
	{
		// We must disable all colour plates for colour contexts attached to this view.
		// We find all the currently cached contexts for this view, and disable their ColourPlates.
		// These will be re-enabled in StopRender. Note that we assume that nobody outside this
		// rendering loop will be disabling/enabling colour plates for specific contexts - they
		// should be setting the state more globally by changing the View's ColourPlate.
		// (We should be as well, with a proper access function in view to make this possible
		// but I'm being a bit lazy here. If you're reading this and swearing right now, then
		// I apologise for any incovenience caused)
		for (INT32 i = 0; i < MAX_COLOURMODELS; i++)
		{
			ColourContext *Bob = RenderView->GetColourContext((ColourModel) i, TRUE);
			if (Bob != NULL && Bob->GetColourPlate() != NULL)
				Bob->GetColourPlate()->SetDisabled(TRUE);
		}

		// And poke the View's main ColourPlate (so that any new contexts created during
		// rendering use the new state)
		OldPlateDisabledState = RenderView->GetColourPlate()->IsDisabled();
		RenderView->GetColourPlate()->SetDisabled(TRUE);

		// Remember that we've poked at the ColourPlate so we can re-enable it in StopRender
		HaveDisabledPlate = TRUE;
	}

	// done OK
	return TRUE;
}



/********************************************************************************************

>	virtual BOOL GRenderPrint::StopRender()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/96
	Returns:	TRUE if worked, FALSE if failed
	Purpose:	Stops rendering in this RR
				Restores the ColourPlate separation options and calls GRenderDIB::StopRender
	SeeAlso:	GRenderDIB::StopRender

********************************************************************************************/

BOOL GRenderPrint::StopRender()
{
	// When colour separating we must restore the colour plate disabled state.
	// We assume that all contexts attached to the view should be using the same state as the
	// View's ColourPlate was, so we only store the one previous state for restoring from.
	if (HaveDisabledPlate && RenderView != NULL && RenderView->GetColourPlate() != NULL)
	{
		for (INT32 i = 0; i < MAX_COLOURMODELS; i++)
		{
			ColourContext *Bob = RenderView->GetColourContext((ColourModel) i, TRUE);
			if (Bob != NULL && Bob->GetColourPlate() != NULL)
				Bob->GetColourPlate()->SetDisabled(OldPlateDisabledState);
		}

		// And restore the main View's ColourPlate
		RenderView->GetColourPlate()->SetDisabled(OldPlateDisabledState);
		OldPlateDisabledState = FALSE;
		HaveDisabledPlate = FALSE;
	}

	// call base class *last*
	return GRenderDIB::StopRender();
}



/********************************************************************************************

>	GRenderPrint::~GRenderPrint()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/95
	Purpose:	GRenderPrint destructor. Frees up the bitmap.

********************************************************************************************/

GRenderPrint::~GRenderPrint()
{
	// we should NOT delete the CurrentColContext cos we didn't alloc it - we just
	// got a pointer to something allocated by somebody else

	// Free up the bitmap here, as the call to FreeLPBits in the
	// GRenderDIB will not call the correct version (the virtual-ness will be
	// broken as it is called from a destructor
	if (pBitmapInfo!=NULL)
	{
//		FreeLPBits( pBitmapInfo, pBits );
		FreeOffscreenState();
		pBitmapInfo = NULL;
		pBits = NULL;
	}
}



/********************************************************************************************

>	BOOL GRenderPrint::InitDevice()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/95
	Returns:	TRUE if initialised ok;
				FALSE if not.
	Purpose:	Initialise the device specific mechanisms for this render region.
				For a print region, it makes sure the StretchBlt mode is set up correctly
				so we get half-toning.
	Errors:		Same as base class.
	SeeAlso:	RenderRegion::InitDevice; GRenderRegion::AttachDevice;
				GRenderRegion::InitDevice

********************************************************************************************/

BOOL GRenderPrint::InitDevice()
{
	// Call the base class *first*
	BOOL Worked = GRenderDIB::InitDevice();

	if (!Worked)							  
		return FALSE;

	// if we don't do this, bitmaps printed at the same DPI as the printer come
	// out completely crap on NT drivers
	// Note: although this is claimed to be a Win32s-compatible function,
	// GDI16 generates an error for it (invalid value 4)
	// Note2: The MFC version of this function looks like the 16-bit API call,
	// so we call the API directly here.
PORTNOTE("printing", "Don't SetStretchBltMode")
#ifndef EXCLUDE_FROM_XARALX
	::SetStretchBltMode(RenderDC->m_hDC, HALFTONE);
#endif

	return TRUE;
}



/********************************************************************************************

>	BOOL GRenderPrint::DisplayBits(LPBITMAPINFO lpDisplayBitmapInfo = NULL, LPBYTE lpDisplayBits = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/95
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Output the bitmap in a printer-friendly way.

********************************************************************************************/

BOOL GRenderPrint::DisplayBits(LPBITMAPINFO lpDisplayBitmapInfo, LPBYTE lpDisplayBits)
{
	INT32 BitmapWidth = pBitmapInfo->bmiHeader.biWidth;
	INT32 BitmapHeight = pBitmapInfo->bmiHeader.biHeight;

	if (RenderView != NULL && RenderView->GetColourPlate() != NULL && 
		!RenderView->GetColourPlate()->IsDisabled())
	{
		// We currently can't handle anything less than 8bpp bitmaps here, as we
		// write the output data to our bitmap in 8bpp format.
		ERROR2IF(uBitmapDepth < 8, FALSE, "Unexpectedly low BPP in GRenderPrint::DisplayBits");

		// We're colour separating. We must separate the entire bitmap down to
		// an 8bpp greyscale format
		CWxBitmap Bitmap(pBitmapInfo, pBits);

		// We are doing a colour separation - find the separation tables
		BYTE *SepTables = NULL;
		ColourContextCMYK *cc = (ColourContextCMYK *)RenderView->GetColourContext(COLOURMODEL_CMYK);
		if (cc != NULL)
		{
			SepTables = (BYTE *) CCMalloc(5 * 256 * sizeof(BYTE));
			if (SepTables != NULL)
			{
				if (!cc->GetProfileTables(SepTables))
				{
					CCFree(SepTables);
					SepTables = NULL;
				}
			}
		}
		ERROR2IF(SepTables == NULL, FALSE, "Can't generate separation tables in GRenderPrint::DisplayBits");

		ColourContext *OutputContext = RenderView->GetColourContext(COLOURMODEL_RGBT);
		ERROR2IF(OutputContext == NULL, FALSE, "No RGB rendering ColourContext in GRenderPrint::DisplayBits");

		// Get a temporary 32bpp scanline
		const INT32 PixelWidth  = Bitmap.GetWidth();
		const INT32 PixelHeight = Bitmap.GetHeight();

		const INT32 ByteWidth	 = DIBUtil::ScanlineSize(PixelWidth, 8);	// Width of 8bit scanline including padding

		Pixel32bpp *TempScanline = (Pixel32bpp *) CCMalloc(PixelWidth * sizeof(Pixel32bpp));
		if (TempScanline == NULL)
		{
			ERROR3("No memory for temp scanline");
			return(FALSE);
		}

		BYTE *pOutputBuffer = pBits;				// We'll overwrite our bitmap with the separated data
		for (INT32 y = 0; y < PixelHeight; y++)
		{
			// Get this scanline as a 32bpp generic structure
			Bitmap.GetScanline32bpp(y, TRUE, TempScanline);

			Bitmap.ColourSeparateScanline32to8(OutputContext, SepTables, pOutputBuffer, TempScanline, PixelWidth);
			pOutputBuffer += ByteWidth;
		}

		// Make sure that the new 8bpp bitmap has a greyscale palette on it - if it was not 8bpp,
		// then we must realloc the header info to get enough room in it for a 256 colour palette.
		if (pBitmapInfo->bmiHeader.biBitCount != 8)
		{
			FreeDIB(pBitmapInfo, NULL, NULL, FALSE);								// Free the info (ONLY)
			pBitmapInfo = AllocDIB(PixelWidth, PixelHeight, 8, NULL, NULL, FALSE);	// Realloc the info
		}

		// And fill in the palette to a greyscale
		for (INT32 i = 0; i < 256; i++)
		{
			pBitmapInfo->bmiColors[i].rgbRed = 
				pBitmapInfo->bmiColors[i].rgbGreen = 
					pBitmapInfo->bmiColors[i].rgbBlue = i;

			pBitmapInfo->bmiColors[i].rgbReserved = 0;
		}

		// Free our separation tables and temporary scanline
		CCFree(SepTables);
		CCFree(TempScanline);

		// Finally, poke the CWxBitmap we created so that it doesn't delete OUR
		// bitmap info and bytes (which we only lent it) when it is deleted
		Bitmap.BMInfo  = NULL;
		Bitmap.BMBytes = NULL;
	}
	else
	{
		// Not colour separating, but if it's a 32bpp bitmap, we need to convert to something 
		// that StretchDIBits (below) can understand
		if (uBitmapDepth == 32)
		{
			// Can't plot 32bpp bitmaps to GDI as 16-bit GDI doesn't understand them,
			// so we convert to 24bpp bitmap in-situ and render that...

			// How many bytes to a source scanline?
			const INT32 ScanlineBytes = DIBUtil::ScanlineSize(BitmapWidth, uBitmapDepth );

			// How many bytes to a destination scanline
			const INT32 DestlineBytes = DIBUtil::ScanlineSize(BitmapWidth, 24);

			// Now convert the bitmap in-situ
			LPBYTE OriginalBuffer  = pBits;
			LPBYTE ConvertedBuffer = pBits;

			for (INT32 i = 0; i < BitmapHeight; i++)
			{
				DIBUtil::Convert32to24(BitmapWidth, OriginalBuffer, ConvertedBuffer);
				OriginalBuffer += (UINT_PTR) ScanlineBytes;
				ConvertedBuffer += (UINT_PTR) DestlineBytes;
			}

			// Update bitmap info to show it is now a 24bpp bitmap...
			pBitmapInfo->bmiHeader.biBitCount  = 24;
			pBitmapInfo->bmiHeader.biSizeImage = DestlineBytes * BitmapHeight;
		}
	}

	WinRect clip;
	//RenderDC->GetClipBox(&clip);
	clip = OSRenderRegion::DocRectToWin(RenderView, RenderMatrix, CurrentClipRect, 0, 0, 0, 0);

PORTNOTE("printing", "Attempt to use StaticPlotBitmap instead of StretchDIBits")
#ifndef EXCLUDE_FROM_XARALX
	INT32 Scanlines = StretchDIBits(RenderDC->m_hDC,
								  clip.left, clip.top,
								  clip.Width(), clip.Height(),
								  0, 0,
								  BitmapWidth, BitmapHeight,
								  pBits,
								  pBitmapInfo,
								  DIB_RGB_COLORS,
								  SRCCOPY);

	ERROR3IF(Scanlines == GDI_ERROR, "No scanlines copied in GRenderPrint::DisplayBits()!");
#else
	GRenderRegion::StaticPlotBitmap(RenderDC, DIB_RGB_COLORS, pBitmapInfo, pBits, clip.x, clip.y, clip.width, clip.height, NULL, 0, 0);
#endif

	return TRUE;
}



/********************************************************************************************

>	virtual BOOL GRenderPrint::SetFirstBand()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Returns:	TRUE 
	Purpose:	Sets up for banded rendering. This class does not do banded rendering as yet.
				This just sets things up to indicate that everything will be done in one band.

********************************************************************************************/

BOOL GRenderPrint::SetFirstBand()
{
	// No Banding needed
	IsRegionBanded = FALSE;
	IsWaitingForRAM = FALSE;
	IsLastBand = TRUE;
	return TRUE;
}



/********************************************************************************************

>	virtual BOOL GRenderPrint::GetNextBand()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Returns:	FALSE
	Purpose:	Since this class does not do banded rendering, there are never any more bands

********************************************************************************************/

BOOL GRenderPrint::GetNextBand()
{
	// No banding
	return FALSE;
}



/********************************************************************************************

>	LPBITMAPINFO GRenderPrint::GetLPBits( INT32 Width, INT32 Height, INT32 Depth, LPBYTE*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Inputs:		Width, Height - the width and height of the required bitmap
				Depth - the bpp of the bitmap
	Returns:	Pointer to a bitmap header block
	Purpose:	Allocates a bitmap from the CCMalloc heap

********************************************************************************************/

LPBITMAPINFO GRenderPrint::GetLPBits( INT32 Width, INT32 Height, INT32 Depth, LPBYTE* lplpBits)
{
	// Get a DIB out of the CCMalloc heap
	LPBITMAPINFO bmInfo = NULL;
	bmInfo = AllocDIB( Width, Height, Depth, lplpBits, NULL, FALSE);

	// return it.
	return bmInfo;
}




/********************************************************************************************

>	void GRenderPrint::FreeLPBits( LPBITMAPINFO, LPBYTE )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Purpose:	Frees the memory allocated in GetLPBits.

********************************************************************************************/

void GRenderPrint::FreeLPBits( LPBITMAPINFO lpBMI, LPBYTE lpB )
{
	// Free up the memory. Not from Limited Heap
	FreeDIB(lpBMI, lpB, NULL, FALSE);
}
