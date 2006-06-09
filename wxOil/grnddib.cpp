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
//#include "dibutil.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "grnddib.h"
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "tunemem.h"
//#include "view.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "osrndrgn.h"
#include "colormgr.h"
#include "colplate.h"

CC_IMPLEMENT_DYNAMIC( GRenderDIB, GRenderRegion )

/********************************************************************************************

>	GRenderDIB::GRenderDIB(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GRenderDIB constructor. Doesn't do anything actually, just calls base class.
				INT32 dpi changed to double dpi (12/12/95) to improve the range of values allowed
				at the < 1000dpi settings that we will be using.
	Errors:		-
	SeeAlso:	GRenderRegion::GRenderRegion

********************************************************************************************/

GRenderDIB::GRenderDIB(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi)
	: GRenderRegion( ClipRegion, ConvertMatrix, ViewScale, Depth, dpi)
{
	m_test = 1;
}


/********************************************************************************************

>	LPBITMAPINFO GRenderDIB::GetLPBits( INT32 Width, INT32 Height, INT32 Depth, LPBYTE*lplpBits)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/94
	Inputs:		Bitmap characteristics.
	Outputs:	*lplpBits is made to point to the bytes of the bitmap.
	Returns:	The bitmap header, or NULL if not enough memory.
	Purpose:	Allocates a bitmap header and the bitmap bytes.
	Errors:		-
	SeeAlso:	GRenderDIB::FreeLPBits

********************************************************************************************/

LPBITMAPINFO GRenderDIB::GetLPBits( INT32 Width, INT32 Height, INT32 Depth, LPBYTE*lplpBits)
{
	// Get a DIB out of the limited heap
	LPBITMAPINFO bmInfo = NULL;

	// Wrapped render regions are not used in the normal process of rendering to screen
	// So don't use limited mem
	if(IsWrapped)
		bmInfo = AllocDIB( Width, Height, Depth, lplpBits, NULL, FALSE);
	else
		bmInfo = AllocDIB( Width, Height, Depth, lplpBits, NULL, TRUE);

	// return it.
	return bmInfo;
}

/********************************************************************************************

>	void GRenderDIB::FreeLPBits( LPBITMAPINFO lpBMI, LPBYTE lpB )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/94
	Inputs:		Bitmap characteristics.
	Outputs:	-
	Returns:	-
	Purpose:	Frees up a bitmap allocated with GetLPBits.
	Errors:		-
	SeeAlso:	GRenderDIB::FreeLPBits

********************************************************************************************/

void GRenderDIB::FreeLPBits( LPBITMAPINFO lpBMI, LPBYTE lpB )
{
	// If we are banded, then free the memory from the limited heap
	FreeDIB(lpBMI, lpB, NULL, TRUE);
}

/********************************************************************************************

>	LPBYTE GRenderDIB::ResizeBits( LPBYTE lpBits, INT32 NewSize)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/95
	Inputs:		Bitmap pointer, and the new size.
	Returns:	The newly resized bitmap data pointer.
	Purpose:	Reallocates the bitmap to be a new size.
	Errors:		-
	SeeAlso:	GRenderDIB::FreeLPBits

********************************************************************************************/

LPBYTE GRenderDIB::ResizeBits( LPBYTE lpBits, INT32 NewSize)
{
	// Get the memory manager
	TunedMemory* pTuned = GetTunedMemManager();
	if (pTuned == NULL)
		return NULL;

	return (LPBYTE)pTuned->LimitedCCRealloc(lpBits, NewSize);
}

/********************************************************************************************

>	GRenderDIB::~GRenderDIB()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GRenderDIB destructor. Frees up the bitmap.
	Errors:		-

********************************************************************************************/

GRenderDIB::~GRenderDIB()
{
	if (LocalBitmap)
	{
		FreeOffscreenState();
		pBitmapInfo = NULL;
		pBits = NULL;
	}
	// will call GRenderRegions destructor here
}

/********************************************************************************************

>	BOOL GRenderDIB::DisplayBits(LPBITMAPINFO lpDisplayBitmapInfo = NULL, LPBYTE lpDisplayBits = NULL)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Plots the DIB onto the owner device.
	Errors:		-

********************************************************************************************/

BOOL GRenderDIB::DisplayBits(LPBITMAPINFO lpDisplayBitmapInfo, LPBYTE lpDisplayBits)
{
	LPBITMAPINFO lpLocalInfo = pBitmapInfo;
	LPBYTE lpLocalBits = pBits;

	if (lpDisplayBitmapInfo && lpDisplayBits)
	{
		lpLocalInfo = lpDisplayBitmapInfo;
		lpLocalBits = lpDisplayBits;
	}

	if (lpLocalInfo && lpLocalBits && RenderDC)
	{
		// If we are doing a colour separation and are not in Mono mode
		// then we need to convert the bitmap from 32 bpp platescale to 
		// simulated printer colours
		// We do this conversion in place as separations are not background rendered
		// so we wont be rendering anything else into the bitmap
PORTNOTE("other","GRenderDIB::DisplayBits - removed separation code")
#ifndef EXCLUDE_FROM_XARALX
		ColourPlate* pPlate = CurrentColContext->GetColourPlate();
		if (pPlate && !pPlate->IsDisabled())
		{
			ColourPlateType Type = pPlate->GetType();
			if (Type != COLOURPLATE_COMPOSITE &&
				Type != COLOURPLATE_SPOT &&
				Type != COLOURPLATE_KEY && 
				Type != COLOURPLATE_NONE && 
				!(pPlate->IsMonochrome()))
			{
				if (BitmapDepth == 32)
				{
					// First we set up a colour conversion table from platescale value
					// to real RGB value
					RGBQUAD Lookup[256];
					DocColour temp;
					PColourRGBT Converted;
					DWORD* pLookup = (DWORD*)Lookup;

					XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
					if (lpCMSMan != NULL)
					{
						ColourRGBT Def;

						for (DWORD i = 0; i < 256; i++)
						{
							switch (Type)
							{
							case COLOURPLATE_CYAN:
								Def.Blue = 1;
								Def.Green = 1;
								Def.Red = (i / 255.0);
								Def.Transparent = 0;
								break;
							case COLOURPLATE_MAGENTA:
								Def.Blue = 1;
								Def.Green = (i / 255.0);
								Def.Red = 1;
								Def.Transparent = 0;
								break;
							case COLOURPLATE_YELLOW:
								Def.Blue = (i / 255.0);
								Def.Green = 1;
								Def.Red = 1;
								Def.Transparent = 0;
								break;
							}
							lpCMSMan->ConvertColourForPaperView(&Def);
							Lookup[i].rgbRed		= Def.Red.MakeDouble() * 0xFF;
							Lookup[i].rgbGreen		= Def.Green.MakeDouble() * 0xFF;
							Lookup[i].rgbBlue		= Def.Blue.MakeDouble() * 0xFF;
							Lookup[i].rgbReserved	= 0;
						}
					}

					DWORD PixMask = 0;
					INT32 ByteOffset = 0;
					switch (Type)
					{
					case COLOURPLATE_CYAN:
						PixMask = 0x00FFFF;
						ByteOffset = 2;
						break;
					case COLOURPLATE_MAGENTA:
						PixMask = 0xFF00FF;
						ByteOffset = 1;
						break;
					case COLOURPLATE_YELLOW:
						PixMask = 0xFFFF00;
						ByteOffset = 0;
						break;
					}
					INT32 Width = lpLocalInfo->bmiHeader.biWidth;
					INT32 Height = lpLocalInfo->bmiHeader.biHeight;
					INT32 NumPixels = Width * Height;
					DWORD* pPixel = (DWORD*)lpLocalBits;
					INT32 Pix;
					for (Pix = 0; Pix < NumPixels; Pix++)
					{
						if ((*pPixel & PixMask) == PixMask)
						{
							BYTE Val = *(((BYTE*)pPixel) + ByteOffset);
							*pPixel = pLookup[Val];
						}
						pPixel++;
					}
				}
			}
		}
#endif
// TEMP BODGE to avoid rewriting all these function to take LP bitmap info/bits parameters!
LPBITMAPINFO lpStoredInfo = pBitmapInfo;
LPBYTE lpStoredBits = pBits;
pBitmapInfo = lpLocalInfo;
pBits = lpLocalBits;

		if ( uBitmapDepth == (UINT32)ScreenDepth )
			Display1to1Bitmap();
//			Display1to1Bitmap(lpLocalInfo, lpLocalBits);
PORTNOTE("other","GRenderDIB::DisplayBits - removed BitmapDepth tests")
#ifdef EXCLUDE_FROM_XARALX
		else
			DisplayLtoHBitmap();
//			DisplayLtoHBitmap(lpLocalInfo, lpLocalBits);
#else
		else if ( BitmapDepth < ScreenDepth )
			DisplayLtoHBitmap();
//			DisplayLtoHBitmap(lpLocalInfo, lpLocalBits);
		else if ( (ScreenDepth==8 ) || (ScreenDepth==4) )
			DisplayHto8Bitmap();			// high-colour to 256/16 is special
//			DisplayHto8Bitmap(lpLocalInfo, lpLocalBits);			// high-colour to 256/16 is special
		else
			DisplayLtoHBitmap();			// fallback code to general blit
//			DisplayLtoHBitmap(lpLocalInfo, lpLocalBits);			// fallback code to general blit
#endif
pBitmapInfo = lpStoredInfo;
pBits = lpStoredBits;
	}
	return TRUE;
}





/********************************************************************************************

>	virtual BOOL GRenderDIB::SetFirstBand()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95
	Returns:	TRUE if all went well. FALSE if it did not
	Purpose:	This determines if banding is needed for this render region and sets up
				ready for it. If this region is already banded, then this function does
				nothing - it is safe to call this even if it has been called before.

********************************************************************************************/

BOOL GRenderDIB::SetFirstBand()
{
	// if we are already banded, or we already have a bitmap, then stop looking
	if (IsBanded() || pBitmapInfo!=NULL)
		return TRUE;

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

	// ok, how much ram do we need and how much ram is there to use
	TunedMemory* pTunedMem = GetTunedMemManager();
	if (pTunedMem==NULL)
		return FALSE;

	// If we are not limited, then do not bother with the bands
	if (pTunedMem->IsMemSizeInfinite())
	{
		// No Banding needed
		IsRegionBanded = FALSE;
		IsWaitingForRAM = FALSE;
		IsLastBand = TRUE;
		return TRUE;
	}

	// We are limited, so find out how much we are allowed
	size_t AvailableRAM = pTunedMem->GetAvailableTunedMem();
	
	// Work out a rough idea of the amount of ram needed
	INT32 ScanLineSize = DIBUtil::ScanlineSize(WRect.width, uBitmapDepth);
	if (ScanLineSize==0)
		return FALSE;

	// Work out how many scan lines we could fit into the available ram
	INT32 MaxScanLines = INT32(AvailableRAM / ScanLineSize);
	INT32 Height = WRect.height;

	// see if there are enought to bother with
	if (MaxScanLines<16)
	{
		// Not really enough memory to bother
		IsWaitingForRAM = TRUE;
		return TRUE;
	}

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
		double TruePixelHeight = 72000.0 / PixelsPerInch; //.MakeDouble();
		TruePixelHeight = TruePixelHeight / ScaleFactor.MakeDouble();
		MILLIPOINT MaxHeight = (MILLIPOINT)(TruePixelHeight * (double)MaxScanLines);

		// Set up the new clip rect ready
		DocRect NewClipRect = CurrentClipRect;
		NewClipRect.hi.y = NewClipRect.lo.y + MaxHeight;

		// Sanity check
		if (NewClipRect.hi.y>CurrentClipRect.hi.y)
			NewClipRect.hi.y = CurrentClipRect.hi.y;

		// Set the new clip rect up ready for rendering
		SetClipRect(NewClipRect);		
		return TRUE;
	}
}




/********************************************************************************************

>	virtual BOOL GRenderDIB::GetNextBand()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95
	Returns:	TRUE if there is another band to do, FALSE if there are no more bands
	Purpose:	This function decides if there are more bands to do. If there are it sets
				them up and returns TRUE.

********************************************************************************************/

BOOL GRenderDIB::GetNextBand()
{
	// if this is not a banded render region, then there are no more bands
	if (!IsBanded())
		return FALSE;

//	ENSURE(GetCaptureDepth()==0, "Can't set a band while there are any captures running");
	ENSURE(GetCaptureDepth()==0 || MasterCaptureIsCurrent(), "Can't set a band while there are any non-master captures running\n");

	// ensure all offscreen rendering in previous band finished rendering
	// NB if it didn't then GetNextBand shouldn't have been called.
//	if(IsUserName("Ilan"))
	ERROR3IF(!MasterCaptureIsCurrent(),"GetNextBand called before previous band finished!");

	// Free previous band's bmp memory
	FreeOffscreenState();
	pBitmapInfo = NULL;
	pBits = NULL;

	// See if we are out of bands
	if ((CurrentClipRect.hi.y>=RegionRect.hi.y) || (IsLastBand))
		return FALSE;

	// Go find the next band, if there is one.
	// ok, how much ram do we need and how much ram is there to use
	TunedMemory* pTunedMem = GetTunedMemManager();

	// we're going to need to be re-rendered
	IsPaperRendered = FALSE;
	IsInkRenderStarted = FALSE;

	// Find out how much free memory there is now
	size_t AvailableRAM = pTunedMem->GetAvailableTunedMem();
	
	// Work out a rough idea of the amount of ram needed
	INT32 ScanLineSize = DIBUtil::ScanlineSize(WRect.width, uBitmapDepth);
	if (ScanLineSize==0)
		return FALSE;

	// Work out how many scan lines we could fit into the available ram
	INT32 MaxScanLines = INT32(AvailableRAM / ScanLineSize);

	// Work out how many scan lines there are left to do
	double TruePixelHeight = 72000.0 / PixelsPerInch; //.MakeDouble();
	TruePixelHeight = TruePixelHeight / ScaleFactor.MakeDouble();
	INT32 Height = (INT32) ((double)(RegionRect.hi.y - CurrentClipRect.hi.y) / TruePixelHeight);

	// see if there are enought to bother with
	IsWaitingForRAM = FALSE;
	if ((Height>=16) && (MaxScanLines<16))
	{
		// Not really enough memory to bother
		IsWaitingForRAM = TRUE;
		return TRUE;
	}

	// Do what we can
	INT32 ScanLinesToDo = (MaxScanLines<Height) ? MaxScanLines : Height;
	if (ScanLinesToDo==Height)
		IsLastBand = TRUE;
	else
		IsLastBand = FALSE;

	// Find out how big in millipoints we can do
	MILLIPOINT MaxHeight = (MILLIPOINT) (TruePixelHeight * (double)ScanLinesToDo);

	// Set up the new clip rect ready
	DocRect NewClipRect = CurrentClipRect;
	NewClipRect.lo.y = CurrentClipRect.hi.y;
	NewClipRect.hi.y = NewClipRect.lo.y + MaxHeight;

	// Sanity check
	if ((NewClipRect.hi.y>RegionRect.hi.y) || (IsLastBand))
		NewClipRect.hi.y = RegionRect.hi.y;
		
	// Make sure that we are not dealing with an empty rect
	if (NewClipRect.Height()==0)
		return FALSE;

	// Set the new clip rect up ready for rendering
	SetClipRect(NewClipRect);

	// we're going to need to be re-rendered
	IsPaperRendered = FALSE;
	IsInkRenderStarted = FALSE;
	TRACEUSER("Gavin",_T("GRenderDIB::GetNextBand - RenderFlags.Rendering = FALSE;\n"));
	RenderFlags.Rendering = FALSE;

	// Say it all worked
	return TRUE;
}




/********************************************************************************************

>	virtual void GRenderDIB::ResizeRegion(DocRect &NewClipRect)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/7/95
	Purpose:	Called when a Region is changed in size (eg. clipped to the client area)

********************************************************************************************/

void GRenderDIB::ResizeRegion(DocRect &NewClipRect)
{
	DocRect NewRegionRect 	= NewClipRect;

	BOOL HaveABitmap = TRUE;

	if (IsRegionBanded)
	{
		// The Region is banded.  That's gunna complicate things.

		if (CurrentClipRect.IsIntersectedWith(NewRegionRect))
		{
			// 'Clip' the current clip rect to the new Region Rect
			NewClipRect	= CurrentClipRect.Intersection(NewRegionRect);
		}
		else
		{
			// The current band has disappeared outside the Region
			if (NewRegionRect.lo.y >= CurrentClipRect.hi.y)
			{
			 	// The area visible, is completely un-rendered,
				// so we may as well start again.
				ResetRegion(NewRegionRect);
				return;
			}
			else
			{
				// We've already render everything beneath the band
				// that has just gone outside the region, so this 
				// region is finished with

				// Fake it so it doesn't render anything, by setting
				// the clip rect to zero height
				NewClipRect = NewRegionRect;
				NewClipRect.lo.y = NewClipRect.hi.y;				

				ResetRegion(NewClipRect);
				return;
			}
		}

		if (IsWaitingForRAM)
		{
			// This region is waiting for memory to be freed, 
			// so there is no bitmap to resize !!
			HaveABitmap = FALSE;
		}
	}

	if (!MasterCaptureIsCurrent())
	{
		// midway through offscreen rendering. Free memory and start again.
		ResetRegion(NewRegionRect);
		return;
	}

	// Make a new Render Matrix, in case the View has changed position
	Matrix ViewMatrix = RenderView->ConstructRenderingMatrix(RenderSpread);

	// Get windows rects, using the new RenderMatrix.
	wxRect NRect = OSRenderRegion::DocRectToWin( ViewMatrix, NewClipRect, PixelsPerInch );
	const INT32 NewWidth 	= NRect.width;
	const INT32 NewHeight = NRect.height;

	if (HaveABitmap)
	{
		if (uBitmapDepth < 8)
		{
			// We are about to re-size the bitmap, but we can't
			// do this for anything less than 8bpp, so we'll just
			// reset instead.
			ResetRegion(NewClipRect);
			return;
		}

		// Get the current clip rect in windows coords
		wxRect ORect = OSRenderRegion::DocRectToWin( ViewMatrix, CurrentClipRect, PixelsPerInch );

		const INT32 OldWidth 	= ORect.width;
		const INT32 OldHeight = ORect.height;

		// Its changed size, so we'll try and re-size the bitmap
		// If this fails, then we'll just re-set
		if (pBits!=NULL && pBitmapInfo!=NULL &&
			(NewWidth!=OldWidth || NewHeight!=OldHeight))
		{
//			TRACEUSER("Gerry", _T("OldRect is (%d, %d) [%d, %d]\n"),ORect.x,ORect.y,ORect.width,ORect.height);
//			TRACEUSER("Gerry", _T("NewRect is (%d, %d) [%d, %d]\n"),NRect.x,NRect.y,NRect.width,NRect.height);

			UINT32 BaseX = NRect.x> ORect.x ? OldWidth  - NewWidth  : 0;
			UINT32 BaseY = NRect.y> ORect.y ? OldHeight - NewHeight : 0;

			// DIB are upside down to windows rects, ie 0,0 is in the bottom left
			BaseY=OldHeight-NewHeight-BaseY; // Use Windows type origin from top left.

//			TRACEUSER("Gerry", _T("Base X,Y is %d,%d\n"), BaseX, BaseY);

			if (DIBUtil::MakeBitmapSmaller(OldWidth, OldHeight, BaseX, BaseY, 
							   			   NewWidth, NewHeight, uBitmapDepth, pBits))
			{
				// Bitmap resized ok, so we can re-allocate the block to the new size
				INT32 NewBitmapSize = 
						DIBUtil::ScanlineSize(NewWidth, uBitmapDepth) * NewHeight;

				pBitmapInfo->bmiHeader.biWidth		= NewWidth;
				pBitmapInfo->bmiHeader.biHeight		= NewHeight;
				pBitmapInfo->bmiHeader.biSizeImage	= NewBitmapSize;

				// Gavin needs an extra bit of memory, on top of the actual
				// bitmap size, so lets work out how much we need ...
				INT32 size = NewBitmapSize;

				#define DIB_DEBUG 0

				#if DIB_DEBUG
				const INT32 dbsize = DIBUtil::ScanlineSize( NewWidth, uBitmapDepth ) * DIB_DEBUG*2;
				if (dbsize<16)
					dbsize = 16;
				#else
				const INT32 dbsize = 0;
				#endif

				if (size==0)
					size = 4;										// in case of zero w or h

				// Added extra bit on
				INT32 NewSize = size + dbsize + EXTRA_GAVIN_BYTES + DIBUtil::ScanlineSize(NewWidth, uBitmapDepth);

				// Resize the DIB to the new size (always smaller)
				pBits = ResizeBits(pBits, NewSize);
//				TRACEUSER("Gerry", _T("ReAlloc called. Bits now at %x, size=%d\n"), pBits, NewSize);

				if (pBits == NULL)
				{
					ResetRegion(NewRegionRect);
					return;
				}

				// Set the new Gavin clip rect
				GetDrawContext()->ClipRectangle( (LPRECT)&NewClipRect );
			}
			else
			{
				// Make smaller failed, so we'll do it the old lazy way.
				ResetRegion(NewRegionRect);
			}
		}
	}

	// set new rectangle size
	WRect = NRect;

	// Update the clip rect if one was given
	CurrentClipRect = NewClipRect;
	RegionRect 		= NewRegionRect;

	// Update the master capture
	Capture* pMasterCapture = GetMasterCapture();
	pMasterCapture->CaptureRect = NewClipRect;
	pMasterCapture->CaptureMatrix = ViewMatrix;
	pMasterCapture->lpBitmapInfo = pBitmapInfo;
	pMasterCapture->lpBits = pBits;

	GMATRIX gmat = MakeGavinMatrix(RenderMatrix, CurrentClipRect, pMasterCapture->dPixelsPerInch, TRUE);
	SetGavinMatrix(&gmat);

	// Since we have changed the clipping rect the GDraw changed bounding box may no longer be
	// correct so we must enlarge it to be full size...
	RECT bbox;
	bbox.left = 0;
	bbox.right = NewWidth;
	bbox.top = 0;
	bbox.bottom = NewHeight;
	BOOL ok = GetDrawContext()->SetChangedBBox(&bbox);
	ERROR2IF(!ok, (void)0, "GDraw_SetChangedBBox failed");

}
