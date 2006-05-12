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

// DIB utility routines

/*
*/

#include "camtypes.h"
#include "dibutil.h"
#include "ensure.h"
#include "fixmem.h"
#include "grndrgn.h"
#include "errors.h"
//#include "resource.h"
#include "ccfile.h"
//#include "andy.h"
//#include "outptdib.h"
#include "bitfilt.h"
#include "bitmpinf.h"	  	// kernel class for storing BitmapInfo
#include "progress.h"
#include "tunemem.h"
//#include "outptgif.h"	 	// GIF_TRANS_COLOUR
#include "bitmap.h"			// KernelBitmap class
#include "oilbitmap.h"		// CWxBitmap class
#include "app.h"
#include "camfiltr.h"		// BaseCamelotFilter
#include "gpalopt.h"
//#include "bmapprev.h"
#include "hardwaremanager.h"
using namespace oilHardwareManager;

// if this is non-zero, then extra scanlines get allocated to help find walking-past bitmap
// type bugs
#define	DIB_DEBUG	0

// These RenderDirection flags determine in which direction screen blits are done when subbanding
// during conversion in PlotBitmap. The 16 bit one is the other way round by default as it gives
// a perceived faster bit speed on some 16 bit machines.
static BOOL RenderDirection4=FALSE;
static BOOL RenderDirection8=FALSE;
static BOOL RenderDirection16=TRUE;
static BOOL RenderDirection24=FALSE;
// the size of the 24-bit temporary buffer, currently 64k. Not too big as usually used
// when there are large 32-bit DIBs around. Not too small as screen will seem slow.
// 64k=0x10000
// 1M =0x100000
//
// Should anyone try and do a 16 bit build the 'INT32' bit may be problematic but thats the
// prefs system for you
//
static UINT32 SubbandConversionSize=0x80000;


/********************************************************************************************

>	LPBITMAPINFO AllocDIB( UINT32 Width, UINT32 Height,	UINT32 Depth, LPBYTE *Bits, MemoryBlock *Blk=NULL,
							, BOOL UseLimitedHeap)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/94
	Inputs:		Size in pixels of DIB, Depth of bitmap (1,4,8,16,24,32)
				UseLimitedHeap - TRUE if we should use the Limited memory allocator
	Outputs:	Bits is updated to point to an array of bytes, or NULL if failed. If Blk is
				not NULL then the bitmap is allocated into it instead of using CCMalloc and
				Bits will point to that. If Bits is NULL then no memory for the bitmap bytes
				are allocated, just the bitmap header. Due to GDraw restrictions, we actually
				allocate an extra 2 bytes plus one scanline.
	Returns:	Pointer to suitable BITMAPINFO or NULL if failed.
	Purpose:	Allocate and initialise a Windows DIB. Note biclrUsed is set only for 8-bit
				and less deep bitmaps, 0 for high-colour ones. biClrImportant is zeroed.
	Errors:		If passed an invalid depth, will ensure and return NULL. If passed a zero
				width or height, it will return a 0x0 bitmap with four bytes allocated for
				the 'bits'.
	Scope:		Global
	SeeAlso:	FreeDIB & GetDIBBitsSize

	NB NB NB	If this changes please ensure mem allocation changes are reflected in
				GetDIBBitsSize as well.

********************************************************************************************/

LPBITMAPINFO AllocDIB( UINT32 Width, UINT32 Height,	UINT32 Depth, LPBYTE *Bits, MemoryBlock *Blk, BOOL UseLimitedHeap)
{
	LPBITMAPINFO lpInfo;

	ENSURE(Blk==NULL, "Cannot do blks yet");

	INT32 size = DIBUtil::ScanlineSize( Width, Depth ) * Height;
	#if DIB_DEBUG
	const INT32 dbsize = DIBUtil::ScanlineSize( Width, Depth ) * DIB_DEBUG*2;
	if (dbsize<16)
		dbsize = 16;
	#else
	const INT32 dbsize = 0;
	#endif

	if (size==0)
		size = 4;										// in case of zero w or h

	// Get the memory manager
	TunedMemory* pTuned = GetTunedMemManager();

	// get the bytes
	LPBYTE Bytes = NULL;
	if (Bits)
	{
		// in case of failure
		*Bits = NULL;

		// allocate the actual bits of the bitmap first
		if (UseLimitedHeap)
			Bytes = (LPBYTE) pTuned->LimitedCCMalloc(size + dbsize + EXTRA_GAVIN_BYTES + DIBUtil::ScanlineSize(Width, Depth));
		else
			Bytes = (LPBYTE)CCMalloc(size + dbsize + EXTRA_GAVIN_BYTES + DIBUtil::ScanlineSize(Width, Depth));

		// See what we got
		if (Bytes==NULL)
			return NULL;
	}

	// we need a BITMAPINFO structure
	size_t extras;
	INT32 used_cols = 0;

	switch (Depth)
	{
		case 1:
			extras = 2*sizeof(COLORREF);
			used_cols = 2;
			break;
		case 4:
			extras = 16*sizeof(COLORREF);
			used_cols = 16;
			break;
		case 8:
			extras = 256*sizeof(COLORREF);
			used_cols = 256;
			break;
		case 16:
			extras = 3*sizeof(COLORREF);
			break;
		case 24:
			extras = 0;
			break;
		case 32:
			extras = 3*sizeof(COLORREF);
			break;
		default:
			ENSURE(FALSE, "Bad bitmap depth");
			return NULL;
	}

	// get the bitmap info block
	if (UseLimitedHeap)
		lpInfo = (LPBITMAPINFO) pTuned->LimitedCCMalloc(sizeof(BITMAPINFO) + extras);
	else
		lpInfo = (LPBITMAPINFO) CCMalloc(sizeof(BITMAPINFO) + extras);
		
	// See if it worked
	if (lpInfo == NULL)
	{
		// Free the memory from the appropriate place
		if (UseLimitedHeap)
			pTuned->LimitedCCFree(Bytes);
		else
			CCFree(Bytes);

		return NULL;
	}

	lpInfo->bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	lpInfo->bmiHeader.biWidth		= Width;
	lpInfo->bmiHeader.biHeight		= Height;
	lpInfo->bmiHeader.biPlanes		= 1;
	lpInfo->bmiHeader.biBitCount	= Depth;
	lpInfo->bmiHeader.biCompression = BI_RGB;
	lpInfo->bmiHeader.biXPelsPerMeter 	= 3780;				// Default to 96 dpi
	lpInfo->bmiHeader.biYPelsPerMeter 	= 3780;
	lpInfo->bmiHeader.biClrUsed			= used_cols;
	lpInfo->bmiHeader.biClrImportant	= 0;
	lpInfo->bmiHeader.biSizeImage 		= size;

	if (extras > 0)
	{
		memset(&(lpInfo->bmiColors[0]), 0, extras);
	}

	if (Bits)
	{
		#if DIB_DEBUG
		const size_t count = DIBUtil::ScanlineSize( Width, Depth );	// bytes per scanline
		memset( Bytes, 0x42, count*DIB_DEBUG );						// top area
		memset( Bytes + count * (Height + DIB_DEBUG), 0x42, count*DIB_DEBUG );
		Bytes += count * DIB_DEBUG;
		PINT32 lpLong = (PINT32) (Bytes-4);
		*lpLong = count;									// store count
		lpLong[-1] = (INT32)(Bytes + count * Height); 		// and ptr to end
		#endif
		*Bits = Bytes;										// return for caller
	}

	return lpInfo;
}

/********************************************************************************************

>	void FreeDIB( LPBITMAPINFO lpInfo, LPBYTE Bits, MemoryBlock *Blk, BOOL UseLimitedHeap = FALSE)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/94
	Inputs:		Results of AllocDIB.
				UseLimitedHeap - TRUE if we should use the Limited heap. Defaults to FALSE
	Outputs:	-
	Returns:	-
	Purpose:	Free up a DIB allocated with AllocDIB.
	Errors:		-
	Scope:		Global
	SeeAlso:	AllocDIB

********************************************************************************************/

void FreeDIB( LPBITMAPINFO lpInfo, LPBYTE Bits, MemoryBlock *Blk, BOOL UseLimitedHeap)
{
	ENSURE(Blk==NULL, "Cannot free blocks");
	#if DIB_DEBUG
	if (Bits)
	{
		const size_t count = *(PINT32)(Bits-4);
		const LPBYTE endbuf= (LPBYTE)*(PINT32)(Bits-8);
		LPBYTE p = Bits - count * DIB_DEBUG;
		size_t i = count - 8;
		while (i--)
		{
			if (*p != 0x42)
				ENSURE(FALSE, "top of bitmap corrupted");
			p++;
		}
		p = endbuf;
		i = count;
		while (i--)
		{
			if (*p != 0x42)
				ENSURE(FALSE, "bottom of bitmap corrupted");
			p++;
		}
		Bits -= count * DIB_DEBUG;
	}
	#endif

	// Free the bits
	if (UseLimitedHeap)
	{
		// Get the memory manager
		TunedMemory* pTuned = GetTunedMemManager();
		pTuned->LimitedCCFree(Bits);
		pTuned->LimitedCCFree(lpInfo);
	}
	else
	{
		CCFree(Bits);
		CCFree(lpInfo);
	}
}

/********************************************************************************************

>	void FreeDIB( LPBITMAPINFO lpInfo, LPBYTE Bits, MemoryBlock *Blk, BOOL UseLimitedHeap = FALSE)

	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/00
	Inputs:		lpInfo - LPBITMAPINFO struct returned by AllocDIB
	Returns:	Actual size of bmp memory block allocated by AllocDIB for the bitmap bits
				described by the bmp header lpInfo
	Purpose:	Give actual mem size allocations made by AllocDIB - more mem is allocated than
				width * height * Bytes per pix
	Errors:		-
	Scope:		Global
	SeeAlso:	AllocDIB

	NOTES:		NB NB Don't use this on a LPBITMAPINFO that was not created by AllocDIB.
				The calculations are specific to the way AllocDIB allocs mem, so may give you
				inaccurate results for other bmp buffers

********************************************************************************************/
INT32 GetDIBBitsSize( LPBITMAPINFOHEADER bh )
{
	// Mem calc from Dibutils AllocDIB
	// Bytes = (LPBYTE)CCMalloc(size + dbsize + EXTRA_GAVIN_BYTES + DIBUtil::ScanlineSize(Width, Depth));
	return bh->biSizeImage;
/*	INT32 size = DIBUtil::ScanlineSize( bh->biWidth, bh->biBitCount ) * bh->biHeight;
	#if DIB_DEBUG
	const INT32 dbsize = DIBUtil::ScanlineSize( bh->biWidth, bh->biBitCount ) * DIB_DEBUG*2;
	if (dbsize<16)
		dbsize = 16;
	#else
	const INT32 dbsize = 0;
	#endif

	if (size==0)
		size = 4;										// in case of zero w or h

	return size + dbsize + EXTRA_GAVIN_BYTES + DIBUtil::ScanlineSize( bh->biWidth, bh->biBitCount );
*/
}

/********************************************************************************************

>	UINT32 DIBUtil::DIBScanlineSize( UINT32 PixelWidth, UINT32 Depth )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/94
	Inputs:		Width in pixels of DIB, Depth of bitmap (1,4,8,16,24,32)
	Outputs:	-
	Returns:	Size on bytes of a scanline in that format
	Purpose:	Used in calculating bitmap memory requirements. Based on Gavin's code in
				GDraw_SetDIBitmap, which it had better agree with!
	Errors:		If passed an invalid depth, will ensure and return 0.
	Scope:		Static

********************************************************************************************/

UINT32 DIBUtil::ScanlineSize( UINT32 Width, UINT32 Depth )
{
	switch (Depth)
	{
		case 1:
			return ((Width+31)&~31)>>3;
		case 2:
			// although Gavin doesn't support 2-bit DIBs, this function does.
			return ((Width+15)&~15)>>2;
		case 4:
			return ((Width+7)&~7)>>1;
		case 8:
			return ((Width+3)&~3);
		case 16:
			return ((Width+1)&~1)<<1;
		case 24:
			// although Gavin doesn't support 24-bit DIBs, this function does.
			return ((Width*3)+3)&~3;
		case 32:
			return Width<<2;
		default:
			ENSURE(FALSE, "Unknown bitmap depth");
			return 0L;
	}
}

/********************************************************************************************

>	UINT32 DIBUtil::ScanlineBytes( UINT32 PixelWidth, UINT32 Depth )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/94
	Inputs:		Width in pixels of DIB, Depth of bitmap (1,4,8,16,24,32)
	Outputs:	-
	Returns:	Size on bytes of a scanline in that format
	Purpose:	Used in calculating offsets along scanlines.
				NOTE! Differs from ScanlineSize in that it DOES NOT
				word align the result!!!
	Errors:		If passed an invalid depth, will ensure and return 0.
	Scope:		Static

********************************************************************************************/

UINT32 DIBUtil::ScanlineBytes( UINT32 Width, UINT32 Depth )
{
	switch (Depth)
	{
		case 1:
			return Width >> 3;
		case 2:
			return Width >> 2;
		case 4:
			return Width >> 1;
		case 8:
			return Width;
		case 16:
			return Width << 1;
		case 24:
			ENSURE(FALSE, "Aren't 24-bit bitmaps stored in 32BPP???");
			return Width * 3;
		case 32:
			return Width << 2;
		default:
			ENSURE(FALSE, "Unknown bitmap depth");
			return 0L;
	}
}

// this macro takes a 5-bit number and expands it into an 8-bit number by replicating
// bits i.e. 43210 expands to 43210432. The input arg had better be unsigned (or have its
// top bit cleared).
#define	EXPAND(x)	( (x<<3) | (x>>2) )



// Deep DIB Colour formats:
// regardless of endian-ness:322222222221111111111
//					  	10987654321098765432109876543210
// 32-bit as an INT32 is	00000000rrrrrrrrggggggggbbbbbbbb
// 16-bit as a word is	                0rrrrrgggggbbbbb





// convert 16-bit scanline to 24-bit
void DIBUtil::Convert16to24( INT32 PixelWidth, LPBYTE InputBits, LPBYTE OutputBits )
{
	LPWORD Source = (LPWORD)InputBits;

	while (PixelWidth--)
	{
		const WORD Data = *Source++;
		
		// get five-bit RGB values, which we then double-up just like Quickdraw does
		// to get an 8-bit result
		const BYTE Blue = Data & 0x1F;
		*OutputBits++ = EXPAND(Blue);

		const BYTE Green = (Data>>5) & 0x1F;
		*OutputBits++ = EXPAND(Green);

		const BYTE Red = (Data>>10) & 0x1F;
		*OutputBits++ = EXPAND(Red);
	}
}

// convert 32-bit scanline to 24-bit ignoring alpha
void DIBUtil::Convert32to24( INT32 PixelWidth, LPBYTE InputBits, LPBYTE OutputBits )
{
	// source form is B,G,R,spare
	// dest is B,G,R
	// Note: if not little-endian, is this still true?
	while (PixelWidth--)
	{
		OutputBits[0] = InputBits[0];
		OutputBits[1] = InputBits[1];
		OutputBits[2] = InputBits[2];
		OutputBits += 3;
		InputBits  += 4;
	}
}


// convert 32-bit scanline to 24-bit by alpha merging with the supplied colour
void DIBUtil::Convert32to24Alpha(INT32 PixelWidth, LPBYTE InputBits, LPBYTE OutputBits, COLORREF Colour)
{
	// source form is B,G,R,spare
	// dest is B,G,R
	// Note: if not little-endian, is this still true?
	const BYTE RedBG = (BYTE)(Colour & 0xFF);
	const BYTE GreenBG = (BYTE)((Colour >> 8) & 0xFF);
	const BYTE BlueBG = (BYTE)((Colour >> 16) & 0xFF);
	
	while (PixelWidth--)
	{
		const BYTE Trans = InputBits[3];
		const BYTE Alpha = 255 - Trans;
		OutputBits[0] = ((InputBits[0] * Alpha) + (RedBG * Trans)) / 255;
		OutputBits[1] = ((InputBits[1] * Alpha) + (GreenBG * Trans)) / 255;
		OutputBits[2] = ((InputBits[2] * Alpha) + (BlueBG * Trans)) / 255;
		OutputBits += 3;
		InputBits  += 4;
	}
}

// convert 32-bit scanline to 32-bit
// (actually justs zero the rgbReserved field) !!!! I THINK NOT!!! WELL HAVE LESS OF THAT THANK!
void DIBUtil::Convert32to32( INT32 PixelWidth, LPBYTE InputBits, LPBYTE OutputBits )
{
	// done as DWORDs for speed
	LPDWORD Source = (LPDWORD)InputBits;
	LPDWORD Dest = (LPDWORD)OutputBits;
	while (PixelWidth--)
	{
//		*Dest++ = *Source++ & 0x00FFFFFF; <- MarkH What for? We can deal with 32BMP Alpha channels!
		*Dest++ = *Source++;
	}
}

// convert 8-bit scanline to 8-bit
// (straight copy really)
void DIBUtil::Convert8to8( INT32 PixelWidth, LPBYTE InputBits, LPBYTE OutputBits )
{
	LPBYTE Source = (LPBYTE)InputBits;
	LPBYTE Dest = (LPBYTE)OutputBits;
	while (PixelWidth--)
	{
		*Dest++ = *Source++;
	}
}



/********************************************************************************************

>	BOOL DIBUtil::PlotDeepDIB( HDC hDC, LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits, INT32 Left, INT32 Top, INT32 Width, INT32 Height, INT32 SourceLeft, INT32 SourceTop, BitmapConvertHint Hint, HPALETTE hPal=NULL)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/94
	Inputs:		lpBitmapInfo must point to a 16- or 32-bpp DIB.
				Using Hint means you can get better dithering
				hPal only required when CONVHINT_SCREEN8/4
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed (due to lack of memory). Error will be set.
	Purpose:	Plot a deep DIB on a device which does not understand deep DIBs. Works by
				converting to a 24-bit DIB in slices. The DIB must be using the 'standard'
				RGB arrangements. If this is called on a non-True Colour device then it will
				take a very very long time, as 24->8 GDI conversions are terribly slow, unless
				CONVHINT_SCREEN8/4 is used.
	Errors:		Will ENSURE if not correct depth. Calls SetError on FALSE returns.
	Scope:		Static

********************************************************************************************/

/* Technical notes:
	CONVHINT_SCREEN8/4 work, but are incomplete:
		1. Bad brush alignment. No attempt is made to align the brushes between slices
			or plots in general. It looks prety bad.
		2. The wxDC* parameter that is cobbled together to pass to SetPaletteEntries is
			a bodge. The function concerned should be modified to take an HDC.
	CONVHINT_METAFILE isn't implemented. The idea is that this should produce 24-bit
		output, using a metafile-friendly way of plotting bitmaps.
*/

BOOL DIBUtil::PlotDeepDIB( wxDC *pDC, LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits, INT32 Left, INT32 Top, INT32 Width, INT32 Height, INT32 SourceLeft, INT32 SourceTop, BitmapConvertHint Hint, HPALETTE hPal)
{
	PORTNOTETRACE("other","DIBUtil::PlotDeepDIB - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	const WORD BitmapDepth = lpBitmapInfo->bmiHeader.biBitCount;

	ERROR2IF( BitmapDepth!=16 && BitmapDepth!=32, FALSE, "Wrong depth deep bitmap" );	

	INT32 DitherMode = 0;
	BOOL UseGavin = FALSE;
	UINT32 TempBitmapDepth = 24;

	// We use Gavin for 32->24 (for 16) so we can dither. We use us for 16->24 (cos its simple)
	if (BitmapDepth==32)
	{
		// 32->24 is us because it is easy.
		// 32->24 for 16 is Gavin, dither according to the hint
		switch (Hint)
		{
		case CONVHINT_NONE:
			// no hint means we do it
			break;
		case CONVHINT_PRINTER:
			// we assume printer is 24-bit, so that's easy
			break;
		case CONVHINT_FINAL24:
			// final output 24-bit means no dithering so we do it - not any more
			DitherMode=8;
			UseGavin=TRUE;
			break;
		case CONVHINT_FINAL16:
			DitherMode = 2; //0;							// default to 565 - now 555
			UseGavin = TRUE;
			break;
		case CONVHINT_FINAL555:
			DitherMode = 2;
			UseGavin = TRUE;
			break;
		case CONVHINT_FINAL565:
			DitherMode = 0;
			UseGavin = TRUE;
			break;
		case CONVHINT_FINAL655:
			DitherMode = 1;
			UseGavin = TRUE;
			break;
		case CONVHINT_FINAL664:
			DitherMode = 3;
			UseGavin = TRUE;
			break;
		case CONVHINT_SCREEN8:
			DitherMode = 1;
			UseGavin = TRUE;
			TempBitmapDepth = 8;
			break;
		case CONVHINT_SCREEN4:
			DitherMode = 1;
			UseGavin = TRUE;
			TempBitmapDepth = 4;
			break;
		default:
			ERROR3( "Strange hint in PlotDeepDIB" );
			return FALSE;
			break;
		}
	}

	if (BitmapDepth==16)
	{
		DitherMode=2;
		UseGavin=TRUE; // Forsooth! Gavin can do 16 to 24 bit conversions!
	}

	// how many bytes to a source scanline? Don't use Width here, get the real bitmap width
	const INT32 ScanlineBytes = ScanlineSize( lpBitmapInfo->bmiHeader.biWidth, BitmapDepth );

	BOOL PlotDownwards = RenderDirection8;

	// We use the hint to determine the screen bits per pixel. Not very reliable but more so than
	// TempBitmapDepth (for instance).
	switch (Hint)
	{
	case CONVHINT_SCREEN4:
		PlotDownwards = RenderDirection4; break;

	case CONVHINT_PRINTER:
		PlotDownwards = FALSE; break;	

	case CONVHINT_FINAL24:
		PlotDownwards = RenderDirection24; break;
				
	case CONVHINT_FINAL16:
	case CONVHINT_FINAL555:
	case CONVHINT_FINAL565:
	case CONVHINT_FINAL655:
	case CONVHINT_FINAL664:
		PlotDownwards = RenderDirection16; break;

	case CONVHINT_NONE:
	case CONVHINT_SCREEN8:
	default:
		PlotDownwards = RenderDirection8; break;
	}

	// allow for SourceLeft by adjusting address. Will always be DWORD aligned for 32-bit
	// DIBs, only WORD aligned for 16-bit ones

	// Unfortunately our source has to be DWORD aligned so fix it up
	if ( BitmapDepth==16 && UseGavin && (SourceLeft & 1)!=0 )
	{
		SourceLeft--;
		Left--;
		Width++; // DWORD align the plot so lpBits on a DWORD boundary
	}

	// allow for X
	lpBits += SourceLeft << ( BitmapDepth==16 ? 1 : 2 );
	// and allow for Y (remember first scanline in memory is bottom most one)
	lpBits += (lpBitmapInfo->bmiHeader.biHeight - ((PlotDownwards?0:Height) + SourceTop) ) * ScanlineBytes;
	
	// Gavins conversion routine can only do whole bitmaps, so the temp bitmap must be as wide
	// as the entire source bitmap
	INT32 SliceWidth = UseGavin ? lpBitmapInfo->bmiHeader.biWidth : Width;

	// how many bytes to a destination scanline
	const INT32 DestlineBytes = ScanlineSize( SliceWidth, TempBitmapDepth );

	// we want a buffer of a limited maximum. Work out how many scanlines that is.
	// (set this to 1 for debugging & even slower plotting)
	INT32	SliceHeight = SubbandConversionSize / DestlineBytes;
	if (SliceHeight<4)
		SliceHeight=4;
	else
		SliceHeight = (SliceHeight+3) & ~3;

	if (SliceHeight>Height) SliceHeight=Height;

	INT32 OurSliceHeight=SliceHeight; // The height of the 
	
	if (PlotDownwards)
	{
		// To get dither alignment correct we want the 1st slice to be the 'left over bit' which
		// isn't of height SliceHeight
		SliceHeight=Height;
		while (OurSliceHeight>SliceHeight) 
			OurSliceHeight-=Height;
	}

	// allocate a temporary 24-bit DIB. IMPORTANT: Due to the way we frig around with the bitmap
	// start address, we must allocate an additional scaline of memory to the source, else he will read past
	// the end of the bitmap (because that's actually what we're asking of him). AllocDIB handily
	// does this for us
	LPBYTE Buffer;
	LPBITMAPINFO BufferDIB = AllocDIB( SliceWidth, SliceHeight, TempBitmapDepth, &Buffer, NULL );

	ERROR1IF( BufferDIB==NULL, FALSE, _R(IDS_OUT_OF_MEMORY) );

	INT32 ColourFlag = 0;

	if (TempBitmapDepth<24)
	{
		// only need to do this once
		ColourFlag = GRenderRegion::SetPaletteEntries( BufferDIB, pDC );
	}

	// Note that the first scanline of a DIB in memory is the bottom, not the top
	INT32 DestY = Top + (PlotDownwards?0:Height);
	INT32 ThisSlice;

	FNPTR_SCANLINE ConvertFn = NULL;
	INT32 RealHeight = lpBitmapInfo->bmiHeader.biHeight;
	
	if (!UseGavin)
	{
		// calculate the correct function pointer
		ConvertFn = BitmapDepth==16 ? Convert16to24 : Convert32to24;
	}

	// now convert in chunks
	while (Height)
	{
		// how tall is this chunk going to be?
		ThisSlice = min(OurSliceHeight, Height);
		OurSliceHeight=SliceHeight; // now use strips of the correct size

		if (UseGavin)
		{
			// have to butcher the  bitmap params (source height restored at the end)
			lpBitmapInfo->bmiHeader.biHeight = ThisSlice;
			BufferDIB->bmiHeader.biHeight = ThisSlice;
			
			// then get the man to convert it. We assume a 5-6-5 RGB split for the moment - should
			// pass in a parameter to control it at some point. See gdraw.wri for list of alternatives
			if (PlotDownwards) lpBits -= ThisSlice * ScanlineBytes;

			GRenderRegion::GetStaticDrawContext()->ConvertBitmap( &lpBitmapInfo->bmiHeader, lpBits,
														&BufferDIB->bmiHeader, Buffer, DitherMode );

			if (!PlotDownwards) lpBits += ThisSlice * ScanlineBytes;
		
		}
		else
		{
			// convert that block of scanlines into 24-bit DIB (also 'upside-down')
			LPBYTE ConvertedBuffer = Buffer;
			if (PlotDownwards) lpBits -= ThisSlice * ScanlineBytes;
			for (INT32 i=0; i<ThisSlice; i++)
			{
				ConvertFn( Width, lpBits, ConvertedBuffer );
				lpBits += ScanlineBytes;
				ConvertedBuffer += DestlineBytes;
			}
			if (PlotDownwards) lpBits -= ThisSlice * ScanlineBytes;
		}

		// now plot our baby-DIB

		if (!PlotDownwards) DestY -= ThisSlice;

	PORTNOTETRACE("other","GRenderRegion::GetRecommendedPalette - removed printing code");
		if( Hint == CONVHINT_PRINTER )
		{ 
			// printers have to do it this way else they don't half-tone correctly
			StretchDIBits( hDC, Left, DestY,							// Dest top left
							Width, ThisSlice,						// Dest width/height
							0,0,									// Source xy
							Width, ThisSlice,						// Soruce w/h
							Buffer, BufferDIB,
							DIB_RGB_COLORS,
							SRCCOPY );
		}
		else if (TempBitmapDepth<24)
		{
			// output 4/8-bit to screen using most efficient mechanism possible
			GRenderRegion::PlotBitmap( hDC, ColourFlag, BufferDIB, Buffer, Left, DestY,
										Width, ThisSlice, hPal, 0,0 );
		}
		else
		{
			INT32 num = SetDIBitsToDevice( hDC, Left, DestY,							// dest top left
								Width, ThisSlice,						// dest w & h
								0,0, 									// source
								0,ThisSlice,							// scanlines (all)
								Buffer, BufferDIB,						// the temp 24-bit DIB
								DIB_RGB_COLORS );
//GdiFlush();
		}

		if (PlotDownwards) DestY += ThisSlice;
		Height -= ThisSlice;
	}

	if (UseGavin)
		lpBitmapInfo->bmiHeader.biHeight = RealHeight;

	FreeDIB( BufferDIB, Buffer );
#endif
	return TRUE;
}

#define RLE_ESCAPE  0
#define RLE_EOL     0
#define RLE_EOF     1
#define RLE_JMP     2
#define RLE_RUN     3

// this code based on DecodeRle286 from rle.c from RLEAPP from the MSDN
// it reads directly from the file. The variable names have been changed to protect the innocent

static BOOL UnpackRle8( CCLexFile *File, const BITMAPINFOHEADER& Header, LPBYTE Bits )
{
	BYTE	Buffer[2];
    BYTE    cnt;
    BYTE    b;
    WORD    x;
    WORD    dx,dy;
    DWORD    wWidthBytes;

    wWidthBytes = Header.biWidth+3 & ~3;

    x = 0;

    for(;;)
    {
		if (File->read( Buffer, 2 ).bad())
			return FALSE;
		cnt = Buffer[0];
		b   = Buffer[1];

        if (cnt == RLE_ESCAPE)
        {
            switch (b)
            {
                case RLE_EOF:
                    return TRUE;

                case RLE_EOL:
                    Bits += wWidthBytes - x;
                    x = 0;
                    break;

                case RLE_JMP:
					if (File->read( Buffer, 2 ).bad())
						return FALSE;
                    dx = (WORD)Buffer[0];
                    dy = (WORD)Buffer[1];

                    Bits += wWidthBytes * dy + dx;
                    x  += dx;

                    break;

                default:
                    cnt = b;
                    x  += cnt;
					if (File->read( Bits, cnt ).bad())
						return FALSE;
					Bits += cnt;

                    if (b & 1)
					{
                        if (File->read( Buffer, 1).bad())				// pad read
							return FALSE;
					}

                    break;
            }
        }
        else
        {
            x += cnt;

            while (cnt-- > 0)
                *Bits++ = b;
        }
    }
	return TRUE;
}


/********************************************************************************************

	static void ReadPalette( CCLexFile *File, INT32 HowMany, size_t SizeOfRGB, LPRGBQUAD Result )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/94
	Inputs:		A file, count of how many palette entries there are, and either
				sizeof(RGBTRIPLE) or sizeof(RGBQUAD).
	Outputs:	Result gets filled in with the palette.
	Returns:	-
	Purpose:	Read the palette section of a BMP, either in modern format or old OS 2
				format. Always ends up in RGBQUAD format.
	Scope:		Static

********************************************************************************************/

static void ReadPalette( CCLexFile *File, INT32 HowMany, size_t SizeOfRGB, LPRGBQUAD Result )
{
	if (SizeOfRGB==sizeof(RGBQUAD))
	{		
		// easy - standard Windows palette
#ifdef __WXMSW__
		File->read( Result, HowMany * (UINT32)SizeOfRGB );
#else
		// silly old OS2 format in chunks of three bytes
		RGBQUAD rgbx;

		// this isn't exactly efficient but it works, is endian-independent and reliable
		while (HowMany--)
		{
			File->read( &rgbx, sizeof(RGBQUAD) );
			Result->rgbBlue = rgbx.rgbRed;			// NOTE! We are swapping BGR for RGB
			Result->rgbGreen = rgbx.rgbGreen;
			Result->rgbRed = rgbx.rgbBlue;			// NOTE! We are swapping BGR for RGB
			Result++;
		}
#endif
	}
	else if (SizeOfRGB==sizeof(RGBTRIPLE))
	{
		// silly old OS2 format in chunks of three bytes
		RGBTRIPLE rgb;

		// this isn't exactly efficient but it works, is endian-independent and reliable
		while (HowMany--)
		{
			File->read( &rgb, sizeof(RGBTRIPLE) );
#ifdef __WXMSW__
			Result->rgbBlue = rgb.rgbtBlue;
			Result->rgbGreen = rgb.rgbtGreen;
			Result->rgbRed = rgb.rgbtRed;
#else
			Result->rgbBlue = rgb.rgbtRed;			// NOTE! We are swapping BGR for RGB
			Result->rgbGreen = rgb.rgbtGreen;
			Result->rgbRed = rgb.rgbtBlue;			// NOTE! We are swapping BGR for RGB
#endif
			Result++;
		}
	}
	#ifdef _DEBUG
	else
		ENSURE(FALSE, "Strange RGB size if ReadPalette");
	#endif
}

// 1 to make up transparency, 0 for normal
#define	FAKE_32_TRANS	0


/********************************************************************************************

>	BOOL DIBUtil::CanReadFromFile( const BitmapInfo *pInfo )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		pInfo points to a kernel (BitmapInfo) bitmap info structure to look at.
	Outputs:	- 
	Returns:	TRUE if ReadFromFile will cope with it, FALSE otherwise.
	Purpose:	To check to see if a .bmp file can be read into memory using ReadFromFile.
				At present, ReadFromFile only likes 8- & 24- & 32-bit BMPs and so we must
				tell the caller that this is the case.
				This is for the old OS/2 1.0 style BMPs.
	Scope:		Static
	SeeAlso:	DIBUtil::ReadFromFile;

********************************************************************************************/

BOOL DIBUtil::CanReadFromFile( const BitmapInfo *pInfo )
{
ERROR2IF(pInfo==NULL,FALSE,"CanReadFromFile BitmapInfo pointer is null");
	BOOL Understand = FALSE;

	// Check the depth in the supplied info header to see if we like it or not
	INT32 Depth = pInfo->PixelDepth;
TRACEUSER( "Neville", wxT("DIBUtil::CanReadFromFile depth = %d\n"), Depth );
	if (Depth == 32)
		Understand = TRUE;		// it is one of our 32bpp bmp so we definitely like it
	else
		Understand = FALSE;		// Not sure that we can cope with this

	return Understand;
}

/********************************************************************************************

>	BOOL DIBUtil::CanReadFromFile( const LPBITMAPCOREHEADER pCoreHeader )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		LPBITMAPCOREHEADER points to a bitmap info structure to look at.
	Outputs:	- 
	Returns:	TRUE if ReadFromFile will cope with it, FALSE otherwise.
	Purpose:	To check to see if a .bmp file can be read into memory using ReadFromFile.
				At present, ReadFromFile only likes 8- & 24- & 32-bit BMPs and so we must
				tell the caller that this is the case.
				This is for the old OS/2 1.0 style BMPs.
	Scope:		Static
	SeeAlso:	DIBUtil::ReadFromFile;

********************************************************************************************/

BOOL DIBUtil::CanReadFromFile( const LPBITMAPCOREHEADER pCoreHeader )
{
ERROR2IF(pCoreHeader==NULL,FALSE,"CanReadFromFile CoreHeader pointer is null");
	BOOL Understand = FALSE;

	// Check the depth in the supplied info header to see if we like it or not
	INT32 Depth = pCoreHeader->bcBitCount;
TRACEUSER( "Neville", wxT("DIBUtil::CanReadFromFile OS/2 type depth = %d\n"), Depth );
	if (Depth == 32)
		Understand = TRUE;		// it is one of our 32bpp bmp so we definitely like it
	else
		Understand = FALSE;		// Not sure that we can cope with this

	return Understand;
}

/********************************************************************************************

>	BOOL DIBUtil::CanReadFromFile( const LPBITMAPINFOHEADER pInfoHeader )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		LPBITMAPFILEINFO points to a bitmap info structure to look at.
	Outputs:	- 
	Returns:	TRUE if ReadFromFile will cope with it, FALSE otherwise.
	Purpose:	To check to see if a .bmp file can be read into memory using ReadFromFile.
				At present, ReadFromFile only likes 8- & 24- & 32-bit BMPs and so we must
				tell the caller that this is the case.
				This is for the Win 3.0+ style BMPs.
	Scope:		Static
	SeeAlso:	DIBUtil::ReadFromFile;

********************************************************************************************/

BOOL DIBUtil::CanReadFromFile( const LPBITMAPINFOHEADER pInfoHeader )
{
ERROR2IF(pInfoHeader==NULL,FALSE,"CanReadFromFile InfoHeader pointer is null");
	BOOL Understand = FALSE;

	// Check the depth in the supplied info header to see if we like it or not
	INT32 Depth = pInfoHeader->biBitCount;
TRACEUSER( "Neville", wxT("DIBUtil::CanReadFromFile Win 3.0+ type depth = %d\n"), Depth );
	if (Depth == 32)
		Understand = TRUE;		// it is one of our 32bpp bmp so we definitely like it
	else
		Understand = FALSE;		// Not sure that we can cope with this

	return Understand;
}

/********************************************************************************************

>	static BOOL DIBUtil::ReadFromFile( CCLexFile *File, LPBITMAPINFO *Info, LPBYTE *Bits,
									   BOOL ReadHeader = TRUE,
									   String_64 *ProgressString = NULL, BaseCamelotFilter *pFilter = NULL)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Inputs:		File			A opened CCFile that can be read from. It should be positioned at the
								start. Caller is responsible for closing it. The file needs to be in
								Binary mode.
				ReadHeader		TRUE then a BITMAPFILEHEADER struct is expected
								FALSE we skip straight to the BITMAPINFO. Defaults to TRUE.
				ProgressString	allows the user to specify whether they require a progress hourglass or not.
								If NULL then none is shown, otherwise an progress bar is shown
								using the text supplied. Defaults to NULL i.e. no progress bar.
				BaseCamelotFilter is an alternative way of handling the progress bar, assume the
								progress bar has been start and just call the IncProgressBarCount in BaseCamelotFilter
								to do the progress bar update. Defaults to NULL i.e. no progress bar.
	Outputs:	Info points to a new LPBITMAPINFO struct and Bits points to the bytes.
				These can be freed up with FreeDIB.
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Read a .bmp file into memory.
				***ONLY READS 8- & 24- & 32-bit BMPs currently***.
				***Errors on 16-bit builds***
				A progress hourglass can be shown if required.
	Errors:		Calls SetError on FALSE returns.
	Scope:		Static, Public
	SeeAlso:	AccusoftFilters::ReadFromFile;

********************************************************************************************/

BOOL DIBUtil::ReadFromFile( CCLexFile *File, LPBITMAPINFO *Info, LPBYTE *Bits, BOOL ReadHeader,
							String_64 *ProgressString, BaseCamelotFilter *pFilter )
{
	ERROR2IF(File == NULL,FALSE,"DIBUtil::ReadFromFile null File pointer");
	ERROR2IF(Info == NULL,FALSE,"DIBUtil::ReadFromFile null Info pointer");
	ERROR2IF(Bits == NULL,FALSE,"DIBUtil::ReadFromFile null Bits pointer");

	BITMAPINFOHEADER InfoHeader;
	UINT32 Depth;
	DWORD BitsSize=0;
	INT32 SizeOfHeader = 0;

	*Info = NULL;										// in case of early exit
	*Bits = NULL;

	// Must set the exception throwing flag to True and force reporting of errors to False.
	// This means that the caller must report an error if the function returns False.
	// Any calls to CCFile::GotError will now throw a file exception and should fall into
	// the catch handler at the end of the function.
	// Replaces the goto's that handled this before.
	BOOL OldThrowingState = File->SetThrowExceptions( TRUE );
	BOOL OldReportingState = File->SetReportErrors( FALSE );

	// If the caller has specified a string then assume they require a progress bar
	// Start it up.
	if (ProgressString != NULL)
	{
		BeginSlowJob(100, FALSE, ProgressString);
	}

	try
	{
		if (ReadHeader)
		{
			BITMAPFILEHEADER Header;

			File->read( &Header, sizeof(Header) );
			Header.bfType = LEtoNative(Header.bfType);
			Header.bfSize = LEtoNative(UINT32(Header.bfSize));
			Header.bfReserved1 = LEtoNative(Header.bfReserved1);
			Header.bfReserved2 = LEtoNative(Header.bfReserved2);
			Header.bfOffBits = LEtoNative(UINT32(Header.bfOffBits));
			
			if (File->bad())
				File->GotError( _R(IDE_FORMATNOTSUPPORTED) );

			if (Header.bfType != 0x4D42)						// "BM" in little-endian format
				File->GotError( _R(IDE_BADFORMAT) );
		}

		// some BMPs (eg 256color.bmp) don't have one of these, they have the old OS2 sort
		size_t SizeOfRGB=0;

		File->read( &SizeOfHeader, sizeof(INT32) );
		SizeOfHeader = LEtoNative(SizeOfHeader);
		if ( SizeOfHeader==sizeof(InfoHeader) )
		{
			// sensible BMP with normal BITMAPINFOHEADER structure
			SizeOfRGB = sizeof(RGBQUAD);
			File->read( &InfoHeader.biWidth, sizeof(InfoHeader)-sizeof(INT32) );

			InfoHeader.biWidth			= LEtoNative(InfoHeader.biWidth);
			InfoHeader.biHeight			= LEtoNative(InfoHeader.biHeight);
			InfoHeader.biPlanes			= LEtoNative(InfoHeader.biPlanes);
			InfoHeader.biBitCount		= LEtoNative(InfoHeader.biBitCount);
			InfoHeader.biCompression	= LEtoNative(UINT32(InfoHeader.biCompression));
			InfoHeader.biSizeImage		= LEtoNative(UINT32(InfoHeader.biSizeImage));
			InfoHeader.biXPelsPerMeter	= LEtoNative(InfoHeader.biXPelsPerMeter);
			InfoHeader.biYPelsPerMeter	= LEtoNative(InfoHeader.biYPelsPerMeter);
			InfoHeader.biClrUsed		= LEtoNative(UINT32(InfoHeader.biClrUsed));
			InfoHeader.biClrImportant	= LEtoNative(UINT32(InfoHeader.biClrImportant));
		}
		else if ( SizeOfHeader==sizeof(BITMAPCOREHEADER) )
		{
			// silly OS2 thing - read in and convert
			BITMAPCOREHEADER TempHeader;
			File->read( &TempHeader.bcWidth, sizeof(TempHeader)-sizeof(INT32) );

			TempHeader.bcWidth			= LEtoNative(TempHeader.bcWidth);
			TempHeader.bcHeight			= LEtoNative(TempHeader.bcHeight);
			TempHeader.bcPlanes			= LEtoNative(TempHeader.bcPlanes);
			TempHeader.bcBitCount		= LEtoNative(TempHeader.bcBitCount);

			SizeOfRGB = sizeof(RGBTRIPLE);
			InfoHeader.biWidth =	TempHeader.bcWidth;
			InfoHeader.biHeight =	TempHeader.bcHeight;
			InfoHeader.biPlanes =	TempHeader.bcPlanes;
			InfoHeader.biBitCount = TempHeader.bcBitCount;
			InfoHeader.biCompression = BI_RGB;
			InfoHeader.biSizeImage = 0L;
			InfoHeader.biXPelsPerMeter = 0L;
			InfoHeader.biYPelsPerMeter = 0L;
			InfoHeader.biClrImportant = 0;
			if (InfoHeader.biBitCount < 24 )
				InfoHeader.biClrUsed = 1<<InfoHeader.biBitCount;
			else
				InfoHeader.biClrUsed = 0;
		}
		else
			File->GotError( _R(IDE_FORMATNOTSUPPORTED) );


		if (File->bad())
			File->GotError( _R(IDE_FORMATNOTSUPPORTED) );

		if (InfoHeader.biWidth == 0 || InfoHeader.biHeight == 0 )
			File->GotError( _R(IDE_BADFORMAT) );

		if (InfoHeader.biPlanes != 1)
			File->GotError( _R(IDE_BADFORMAT) );

		Depth = InfoHeader.biBitCount;

		BOOL Convert16to24 = FALSE;
		if (InfoHeader.biBitCount == 16 && InfoHeader.biCompression == BI_BITFIELDS)
		{
			// we will create a 24bpp bitmap and then convert the data on entry in 24bpp
			Depth = 24;
			Convert16to24 = TRUE;
		}
		
		// temp kludge code for now - only copes with certain depth bitmaps
		if ( (Depth != 1) && (Depth != 4) && (Depth != 8) && (Depth != 24) && (Depth != 32))
			File->GotError( _R(IDE_FORMATNOTSUPPORTED) );

		// we know what sort of bitmap we are - lets allocate a new LPBITMAPINFO and some bytes
		*Info = AllocDIB( InfoHeader.biWidth, InfoHeader.biHeight, Depth,
							Bits, NULL );

		if (*Info == NULL)
			File->GotError( _R(IDS_OUT_OF_MEMORY) );

		// Copy interesting info from the file to the memory DIB

		
		// If PelsPerMeter is a dodgy value such as 39 (= very large pixels!)
		// then use 0 which means that we will display it at screen resolution.
TRACEUSER( "Neville", _T("DIBUtil::ReadFromFile XPelsPerMeter=%d\n"),InfoHeader.biXPelsPerMeter);
TRACEUSER( "Neville", _T("DIBUtil::ReadFromFile YPelsPerMeter=%d\n"),InfoHeader.biYPelsPerMeter);
		if (InfoHeader.biXPelsPerMeter > BaseBitmapFilter::MinPelsPerMeter)
		{
			(*Info)->bmiHeader.biXPelsPerMeter = InfoHeader.biXPelsPerMeter;
			(*Info)->bmiHeader.biYPelsPerMeter = InfoHeader.biYPelsPerMeter;
		}
		else
		{
			(*Info)->bmiHeader.biXPelsPerMeter = 0;
			(*Info)->bmiHeader.biYPelsPerMeter = 0;
		}

		// If the ClrUsed field is zero, put a sensible value in it
		// The read in value of ClrUsed also determines the number of palette entries in
		// the palette table.
		UINT32 PalColours = 0;
		if (Depth <= 8)
		{
			PalColours = InfoHeader.biClrUsed;
			// If zero then assume maximum amount of colours
			// otherwise, copy the value across into the new header.
			if (InfoHeader.biClrUsed == 0)
			{
				(*Info)->bmiHeader.biClrUsed = 1<<Depth;
				PalColours = 1<<Depth;
			}
			else
				(*Info)->bmiHeader.biClrUsed = InfoHeader.biClrUsed;
		}

		// read any palette info
		switch (Depth)
		{
			case 1:
			case 4:
			case 8:
				// read colour palette (Changed by Neville 14/10/97 to read biClrUsed)
				ReadPalette( File, PalColours, SizeOfRGB, (*Info)->bmiColors);
				break;

			case 24:
			case 32:
				// 24-bit files sometimes have a 'hint' palette (e.g country.bmp) which
				// we have to throw away (we can't load it cos the bitmap has no space alloced
				// for a colour table)
				if (InfoHeader.biClrUsed)
					File->seekIn( InfoHeader.biClrUsed * SizeOfRGB, ios::cur );
				break;

			default:
				File->GotError( _R(IDE_FORMATNOTSUPPORTED) );
		}

		if (File->bad())
			File->GotError( _R(IDE_FORMATNOTSUPPORTED) );

		// now read the bitmap data

		UINT32 RedColourMask	= 0;
		UINT32 GreenColourMask	= 0;
		UINT32 BlueColourMask	= 0;
		switch (InfoHeader.biCompression)
		{
			case BI_RGB:
				BitsSize = (*Info)->bmiHeader.biSizeImage;			// uncompressed
				break;

			case CC_BMPTYPE:
				if (Depth != 32)
					File->GotError( _R(IDE_FORMATNOTSUPPORTED) );
				BitsSize = (*Info)->bmiHeader.biSizeImage;			// uncompressed
				break;

			case BI_RLE8:
				if (!UnpackRle8( File, InfoHeader, *Bits ))
					File->GotError( _R(IDE_BADFORMAT) );
				BitsSize = 0L;										// no more to read
				break;

			case BI_BITFIELDS:
				// Read in the three special colour dword masks that follow the header
				BitsSize = (*Info)->bmiHeader.biSizeImage;			// uncompressed
				File->read( &RedColourMask,		sizeof(RedColourMask) );
				File->read( &GreenColourMask,	sizeof(GreenColourMask) );
				File->read( &BlueColourMask,	sizeof(BlueColourMask) );

				RedColourMask			= LEtoNative(RedColourMask);
				GreenColourMask			= LEtoNative(GreenColourMask);
				BlueColourMask			= LEtoNative(BlueColourMask);

				if (File->bad())
					File->GotError( _R(IDE_BADFORMAT) );
				TRACEUSER( "Neville", _T("DIBUtil::ReadFromFile BI_BITFIELDS RedColourMask = %d\n"),RedColourMask);
				TRACEUSER( "Neville", _T("DIBUtil::ReadFromFile BI_BITFIELDS GreenColourMask = %d\n"),GreenColourMask);
				TRACEUSER( "Neville", _T("DIBUtil::ReadFromFile BI_BITFIELDS BlueColourMask = %d\n"),BlueColourMask);
				// The 32bpp case is easy. With 16bpp we need to convert to 24bpp
				// We will take the Windows 95 in 32bpp mode such that it expects
				// The blue mask is 0x000000FF, the green mask is 0x0000FF00, and the red mask is 0x00FF0000.
				if (Depth == 32 && RedColourMask != 0xFF0000 && GreenColourMask != 0xFF00 && BlueColourMask != 0xFF)
					File->GotError( _R(IDE_FORMATNOTSUPPORTED) );
				else if (Depth != 24 && Depth != 32)
					File->GotError( _R(IDE_FORMATNOTSUPPORTED) );
				break;

			default:
				File->GotError( _R(IDE_FORMATNOTSUPPORTED) );			// other compression not supported
		}

		if (Convert16to24 == TRUE && Depth == 24)
		{
			// We need to read in a WORD and use the bitfield to convert this into 24 bit RGB data
			// and then put this in the 24bpp pixel
			// We take the Windows 95 route in that when the biCompression member is BI_BITFIELDS,
			// we only support the following 16bpp color masks:
			// A 5-5-5 16-bit image, where
			//	the blue mask is 0x001F, the green mask is 0x03E0, and the red mask is 0x7C00;
			// and a 5-6-5 16-bit image, where
			// the blue mask is 0x001F, the green mask is 0x07E0, and the red mask is 0xF800.
			WORD Pixel = 0;
			INT32 Bshift = 0; // Always 0
			if (BlueColourMask != 0x1F)	// Covers both 15bpp and 16bpp
				File->GotError( _R(IDE_FORMATNOTSUPPORTED) );

			INT32 Gshift = 0;
			if (GreenColourMask == 0x7e0 || GreenColourMask == 0x3e0)
				Gshift = 5;
			else
				File->GotError( _R(IDE_FORMATNOTSUPPORTED) );

			INT32 Rshift = 0;
			if (RedColourMask == 0x7c00)
				Rshift = 10;
			else if (RedColourMask == 0xf800)
				Rshift = 11;
			else
				File->GotError( _R(IDE_FORMATNOTSUPPORTED) );

			UINT32 ScanLineWidth = DIBUtil::ScanlineSize(InfoHeader.biWidth, Depth);
			LPBYTE pBits = *Bits;
			INT32 Height = InfoHeader.biHeight;
			INT32 Width = InfoHeader.biWidth;
			for (INT32 y = 0; y < Height; y++ )
			{
				for (INT32 x = 0; x < Width; x++ )
				{
					// read in the pixel
					File->read( &Pixel,	sizeof(Pixel) );
					
					Pixel = LEtoNative(Pixel);
					
					if (File->bad())
						File->GotError( _R(IDE_BADFORMAT) );
					// Convert it into the B G and R components
					UINT32 B = ((Pixel & BlueColourMask) >> Bshift);	// blue component
					UINT32 G = ((Pixel & GreenColourMask) >> Gshift);	// green component
					UINT32 R = ((Pixel & RedColourMask) >> Rshift);	// red component;
					// Need to shift the components into the MSBs leaving gap at bottom
					// then fill in the missing LSBs with the required number of MSBs
					pBits[x * 3 + 0] = (B << 3) | (B >> 2);
					if (GreenColourMask == 0x7e0) // 6 bit green
						pBits[x * 3 + 1] = (G << 2) | (G >> 4);
					else
						pBits[x * 3 + 1] = (G << 3) | (G >> 2);
					pBits[x * 3 + 2] = (R << 3) | (R >> 2);
				}
				
				// Move onto the next line
				pBits += ScanLineWidth;
				// Updating the progress bar as we go
				ContinueSlowJob((INT32)((100 * y)/Height));
			}
		}
		// Read the actual bytes, used to do it in one go but we really require some
		// progress bar indication so we will do it in chunks.
		else if (BitsSize > 0)
		{
			// If pFilter is NULL and hence not native/web loading then do it in 
			// chunks of bytes
			if (pFilter == NULL)
			{
				if (BitsSize < 1024 || ProgressString == NULL)
				{
					// File very small or no progress bar required, so load in one go
					File->read( *Bits, BitsSize );
				}
				else
				{
					// Load in chunks, for present split into 100 chunks
					DWORD ChunkSize = BitsSize/100;
					DWORD Position = 0;
					LPBYTE pBitInfo = *Bits;
					
					while (Position < BitsSize)
					{
						if ( (BitsSize - Position) > ChunkSize)
							File->read( pBitInfo, ChunkSize );
						else
						{
							ChunkSize = BitsSize - Position;
							File->read( pBitInfo, ChunkSize );
						}
								
						// Increment our counters/pointers
						Position += ChunkSize;
						pBitInfo += ChunkSize;
						ContinueSlowJob((INT32)(100*Position/BitsSize));
					}
				}
			}
			else
			{
				// Do the reading a scanline at a time
				// This makes progress bar update easier for native/web files
				LPBYTE pBitInfo = *Bits;
				UINT32 ScanLineWidth = DIBUtil::ScanlineSize(InfoHeader.biWidth, Depth);
				UINT32 height = InfoHeader.biHeight; 
				// Ask the filter what the record size for this bitmap is and hence
				// what the allocation we have for progress bar updates are.
				// We will then have to update our progress bar by
				//	current scanline/total number of scanlines * allocation
				// so that we get update by a proportion of the value.
				// We can assume no interlacing as in native/web files this is turned off.
				UINT32 RecordSize = pFilter->GetCurrentRecordSize();
				if (RecordSize == 0)
					RecordSize = 1;
				INT32 UpdateValue = RecordSize/height;
				// For each scaline in the file, read it in and update the progress bar count
				for (UINT32 i = 0; i < height; i++)
				{
					File->read( pBitInfo, ScanLineWidth );
					pBitInfo += ScanLineWidth;
					pFilter->IncProgressBarCount(UpdateValue);
				}
			}
		}
		
		// This shouldn't be required any more as the CATCH handler should be invoked
		// when problems happen.
		if (File->bad())
			File->GotError( _R(IDE_FORMATNOTSUPPORTED) );

		if (FAKE_32_TRANS && (Depth==32) && (InfoHeader.biCompression!=CC_BMPTYPE) )
		{
			// put some fake transparency in on a scanline by scanline basis
			LPRGBQUAD Data = (LPRGBQUAD)(*Bits);
//			size_t size = BitsSize;
			UINT32 Height = (*Info)->bmiHeader.biHeight;
			UINT32 h = Height;
			Height /= 2;

			while (h--)
			{
				INT32 TValue = (INT32)h - (INT32)Height;
				TValue = abs(TValue);
				TValue = TValue * 0xFF / Height;

				UINT32 w = (*Info)->bmiHeader.biWidth;
				while (w--)
				{
					Data->rgbReserved = (BYTE)TValue;
					Data++;
				}
				// 32-bit DIBs automatically DWORD aligned
			}
		}

		// If started, then stop then progress bar
		if (ProgressString != NULL)
		{
			EndSlowJob();
		}

		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		// er, we seem to have finished OK so say so
		return TRUE;
	}
	catch( CFileException )
	{
		// catch our form of a file exception
		TRACE( _T("DIBUtil::ReadFromFile CC catch handler\n"));

		FreeDIB( *Info, *Bits );							// free any alloced memory
		*Info = NULL;										// and NULL the pointers
		*Bits = NULL;

		// If started, then stop then progress bar
		if (ProgressString != NULL)
		{
			EndSlowJob();
		}

		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		return FALSE;
	}

	ERROR2( FALSE, "Escaped exception clause somehow" );
}

/********************************************************************************************

>	static BOOL DIBUtil::WriteToFile ( CCLexFile *File, LPBITMAPINFO Info, LPBYTE Bits,
									   String_64 *ProgressString = NULL, BOOL WriteHeader = TRUE,
									   BaseCamelotFilter *pFilter = NULL )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		File	An opened CCFile that can be written to. It should be positioned at the
						start. Caller is responsible for closing it. The file needs to be in
						Binary mode.
				Info	BITMAPINFO structure for the dib.
				Bits	The bitmap data itself
				ProgressString allows the user to specify whether they require a progress
						hourglass or not. If NULL then none is shown, otherwise an progress	bar 
						is shown using the text supplied. Defaults to NULL i.e. no progress bar.
				WriteHeader		TRUE then write a BITMAPFILEHEADER to file
								FALSE skip writing the BITMAPFILEHEADER to file. Defaults to TRUE.
				BaseCamelotFilter is an alternative way of handling the progress bar, assume the
								progress bar has been start and just call the IncProgressBarCount in BaseCamelotFilter
								to do the progress bar update. Defaults to NULL i.e. no progress bar.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Write a bitmap in memory straight out to file with now rendering or conversion.
				***Errors on 16-bit builds***
				A progress hourglass can be shown if required.
				This function is used by the save bitmap button on the bitmap gallery. All
				other bitmap export uses the OutputDIB class instead as this copes with using
				a render region and converting from 32 to the destination format.
				(caller should close file)
	Errors:		Calls SetError on FALSE returns.
	Scope:		Static, Public
	SeeAlso:	AccusoftFilters::WriteToFile; DIBUtil::ReadFromFile;

********************************************************************************************/
BOOL DIBUtil::WriteToFile ( CCLexFile *File, LPBITMAPINFO Info, LPBYTE Bits, String_64 *ProgressString, BOOL WriteHeader, BaseCamelotFilter *pFilter)
{
#ifndef WIN32
	Error::SetError( _R(IDE_BADFORMAT) );
	return FALSE;
#else
	ERROR2IF(File==NULL,FALSE,"DIBUtil::WriteToFile File pointer is null");
	ERROR2IF(Info==NULL,FALSE,"DIBUtil::WriteToFile BitmapInfo pointer is null");
	ERROR2IF(Bits==NULL,FALSE,"DIBUtil::WriteToFile Bits pointer is null");

	// Must set the exception throwing flag to True and force reporting of errors to False.
	// This means that the caller must report an error if the function returns False.
	// Any calls to CCFile::GotError will now throw a file exception and should fall into
	// the catch handler at the end of the function.
	// Replaces the goto's that handled this before.
	BOOL OldThrowingState = File->SetThrowExceptions( TRUE );
	BOOL OldReportingState = File->SetReportErrors( FALSE );

	// If the caller has specified a string then assume they require a progress bar
	// Start it up.
	if (ProgressString != NULL)
	{
		BeginSlowJob(100, FALSE, ProgressString);
	}

	try
	{
		// BITMAPINFO  consists of:-
		//    	BITMAPINFOHEADER    bmiHeader;
    	//		RGBQUAD             bmiColors[1];
		LPBITMAPINFOHEADER pInfoHeader = &Info->bmiHeader;
		ERROR2IF(pInfoHeader==NULL,FALSE,"DIBUtil::WriteToFile BitmapInfoHeader pointer is null");
		
		// Work out the palette size 
		INT32 PalSize = Info->bmiHeader.biClrUsed;		// How many entries in palette
TRACEUSER( "Neville", _T("DIBUtil::WriteToFile PalSize = %d\n"),PalSize);
TRACEUSER( "Neville", _T("DIBUtil::WriteToFile XPelsPerMeter=%d\n"),pInfoHeader->biXPelsPerMeter);
TRACEUSER( "Neville", _T("DIBUtil::WriteToFile YPelsPerMeter=%d\n"),pInfoHeader->biYPelsPerMeter);

		if (WriteHeader)
		{
			// BITMAPFILEHEADER goes first
			BITMAPFILEHEADER Header;

			Header.bfType = ('M'<<8) | 'B';
			Header.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
							sizeof(RGBQUAD)*PalSize + pInfoHeader->biSizeImage;
			Header.bfReserved1 = 0;
			Header.bfReserved2 = 0;
			Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
								sizeof(RGBQUAD)*PalSize;

			// Write that headerout ot the file
			File->write( &Header, sizeof(Header) );
		}

		// then a local BITMAPINFOHEADER

		File->write( pInfoHeader, sizeof(BITMAPINFOHEADER) );

		// then the RGBQUAD palette, if there is one
		if (PalSize)
		{
			// Work out the size of the palette that we should be saving
			const UINT32 TotalPal = sizeof(PALETTEENTRY) * PalSize;
			File->write( Info->bmiColors, TotalPal );
		}

		// Now write out the bitmap data itself.
		DWORD BitsSize = pInfoHeader->biSizeImage; 

		// Write the actual bytes out to file. Used to do it in one go but we really
		// require some progress bar indication so we will do it in chunks.
		if (BitsSize > 0)
		{
			// If pFilter is NULL and hence not native/web loading then do it in 
			// chunks of bytes
			if (pFilter == NULL)
			{
				if (BitsSize < 1024 || ProgressString == NULL)
				{
					// File very small or no progress bar required, so load in one go
					File->write( Bits, BitsSize);
				}
				else
				{
					// Load in chunks, for present split into 100 chunks
					DWORD ChunkSize = BitsSize/100;
					DWORD Position = 0;
					LPBYTE pBitInfo = Bits;
					
					while (Position < BitsSize)
					{
						if ( (BitsSize - Position) > ChunkSize)
							File->write( pBitInfo, ChunkSize);
						else
						{
							ChunkSize = BitsSize - Position;
							File->write( pBitInfo, ChunkSize);
						}
								
						// Increment our counters/pointers
						Position += ChunkSize;
						pBitInfo += ChunkSize;
						ContinueSlowJob((INT32)(100*Position/BitsSize));
					}
				}
			}
			else
			{
				// Do the reading a scanline at a time
				// This makes progress bar update easier for native/web files
				LPBYTE pBitInfo = Bits;
				UINT32 ScanLineWidth = DIBUtil::ScanlineSize(pInfoHeader->biWidth, pInfoHeader->biBitCount);
				UINT32 height = pInfoHeader->biHeight; 
				// For each scaline in the file, read it in and update the progress bar count
				for (UINT32 i = 0; i < height; i++)
				{
					File->write( pBitInfo, ScanLineWidth );
					pBitInfo += ScanLineWidth;
					pFilter->IncProgressBarCount(1);
				}
			}
		}

		// If started, then stop then progress bar
		if (ProgressString != NULL)
		{
			EndSlowJob();
		}

		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		// er, we seem to have finished OK so say so
		return TRUE;
	}
	catch( CFileException )
	{
		// catch our form of a file exception
		TRACE( _T("DIBUtil::WriteToFile CC catch handler\n"));

		// If started, then stop then progress bar
		if (ProgressString != NULL)
		{
			EndSlowJob();
		}

		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		return FALSE;
	}

	ERROR2( FALSE, "Escaped exception clause somehow" );

#endif
}



/********************************************************************************************
>	static BOOL DIBUtil::WriteToFile ( CCLexFile *, LPBYTE Bits, String_64 *ProgressString = NULL, UINT32 Width, UINT32 Height, UINT32 Depth)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/96
	Inputs:		File	An opened CCFile that can be written to. It should be positioned at the
						start. Caller is responsible for closing it. The file needs to be in
						Binary mode.
				Bits	The bitmap data itself
				ProgressString allows the user to specify whether they require a progress
						hourglass or not. If NULL then none is shown, otherwise an progress	bar 
						is shown using the text supplied. Defaults to NULL i.e. no progress bar.
				Width	The width of the bitmap data in pixels
				Height	The height of the bitmap data in pixels
				Depth	The depth of the bitmap (Only 1 is supported at the mo)
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Generates a BITMAPINFO structor and then saves out the DIB
********************************************************************************************/
BOOL DIBUtil::WriteToFile ( CCLexFile* pFile, LPBYTE pBits, UINT32 Width, UINT32 Height, UINT32 Depth, String_64 *pProgressString)
{
	ERROR3IF(Depth!=1, "Only 1bpp DIBs supported");
	ERROR3IF(Width==0, "Zero width");
	ERROR3IF(Height==0, "Zero height");
	
	// Create a new info
	const INT32 NumColours = 1 << Depth;
	BITMAPINFO* pBmpInfo = (BITMAPINFO*) CCMalloc (sizeof(BITMAPINFO) + sizeof(RGBQUAD)*NumColours);
	if (pBmpInfo == NULL)
		return FALSE;

	// Fill in the bitmap info
	pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pBmpInfo->bmiHeader.biWidth = Width;
	pBmpInfo->bmiHeader.biHeight = Height;
	pBmpInfo->bmiHeader.biPlanes = 1;
	pBmpInfo->bmiHeader.biBitCount = Depth;
	pBmpInfo->bmiHeader.biCompression = BI_RGB;
	pBmpInfo->bmiHeader.biXPelsPerMeter = 0;
	pBmpInfo->bmiHeader.biYPelsPerMeter = 0;
	pBmpInfo->bmiHeader.biClrImportant = 0;

	// It is these fields at are dependant on the 1bpp ness
	pBmpInfo->bmiHeader.biClrUsed = 2;
	pBmpInfo->bmiHeader.biSizeImage = Height*(((Width+31)/32)*4);	// Scanlines are word-aligned

	// Generate a black and white palette
	pBmpInfo->bmiColors[0].rgbRed = 0x00;	
	pBmpInfo->bmiColors[0].rgbGreen = 0x00;	
	pBmpInfo->bmiColors[0].rgbBlue = 0x00;	
	pBmpInfo->bmiColors[0].rgbReserved = 0;
	pBmpInfo->bmiColors[1].rgbRed = 0xff;	
	pBmpInfo->bmiColors[1].rgbGreen = 0xff;	
	pBmpInfo->bmiColors[1].rgbBlue = 0xff;	
	pBmpInfo->bmiColors[1].rgbReserved = 0;

	// Call saving code
	BOOL result = DIBUtil::WriteToFile ( pFile, pBmpInfo, pBits, pProgressString );

	CCFree(pBmpInfo);
		
	return result;
}



// 1 to export as 32-bit DIBs, 0 as 24-bit ones (which are more normal)
#define	WRITE_32BIT	0


// this function taken from the WinG Help file, except takes a pointer to a PALETTEENTRY

HPALETTE CreateIdentityPalette( PALETTEENTRY aRGB[], INT32 nColors )
{
	PORTNOTETRACE("other","CreateIdentityPalette - do nothing - no palette support anymore");
#ifndef EXCLUDE_FROM_XARALX
	INT32 i;
	struct {
		WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[256];
	} Palette =
	{
		0x300,
		256
	};

	//*** Just use the screen DC where we need it
	wxScreenDC			dc;

	//*** For SYSPAL_NOSTATIC, just copy the color table into
	//*** a PALETTEENTRY array and replace the first and last entries
	//*** with black and white
	if (GetSystemPaletteUse(hdc) == SYSPAL_NOSTATIC)

	{
		//*** Fill in the palette with the given values, marking each
		//*** as PC_NOCOLLAPSE
		for(i = 0; i < nColors; i++)
		{
			Palette.aEntries[i].peRed = aRGB[i].peRed;
			Palette.aEntries[i].peGreen = aRGB[i].peGreen;
			Palette.aEntries[i].peBlue = aRGB[i].peBlue;
//			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;
			Palette.aEntries[i].peFlags = 0;
		}

		//*** Mark any unused entries PC_NOCOLLAPSE
		for (; i < 256; ++i)
		{
//			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;
			Palette.aEntries[i].peFlags = 0;
		}

		//*** Make sure the last entry is white
		//*** This may replace an entry in the array!
		Palette.aEntries[255].peRed = 255;
		Palette.aEntries[255].peGreen = 255;
		Palette.aEntries[255].peBlue = 255;
		Palette.aEntries[255].peFlags = 0;

		//*** And the first is black
		//*** This may replace an entry in the array!
		Palette.aEntries[0].peRed = 0;
		Palette.aEntries[0].peGreen = 0;
		Palette.aEntries[0].peBlue = 0;
		Palette.aEntries[0].peFlags = 0;

	}
	else
	//*** For SYSPAL_STATIC, get the twenty static colors into
	//*** the array, then fill in the empty spaces with the
	//*** given color table
	{
		INT32 nStaticColors;
		INT32 nUsableColors;

		//*** Get the static colors from the system palette
		nStaticColors = GetDeviceCaps(hdc, NUMCOLORS);
		GetSystemPaletteEntries(hdc, 0, 256, Palette.aEntries);

		//*** Set the peFlags of the lower static colors to zero
		nStaticColors = nStaticColors / 2;

		for (i=0; i<nStaticColors; i++)
			Palette.aEntries[i].peFlags = 0;

		//*** Fill in the entries from the given color table
		nUsableColors = nColors - nStaticColors;
		for (; i<nUsableColors; i++)
		{
			Palette.aEntries[i].peRed = aRGB[i].peRed;
			Palette.aEntries[i].peGreen = aRGB[i].peGreen;
			Palette.aEntries[i].peBlue = aRGB[i].peBlue;
//			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;
			Palette.aEntries[i].peFlags = 0;
		}

		//*** Mark any empty entries as PC_NOCOLLAPSE

		for (; i<256 - nStaticColors; i++)
//			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;
			Palette.aEntries[i].peFlags = 0;

		//*** Set the peFlags of the upper static colors to zero
		for (i = 256 - nStaticColors; i<256; i++)
			Palette.aEntries[i].peFlags = 0;
	}

	//*** Return the palette
	return CreatePalette((LOGPALETTE *)&Palette);
#else
	return NULL;
#endif
}

// this function taken from the WinG Help file

void ClearSystemPalette(void)
{
	// This isn't particularly friendly to other palette using apps
	// so we'll remove it from Ralph...
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

	//*** A dummy palette setup
	struct
	{
		WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[256];
	} Palette =
	{
		0x300,
		256
	};

	HPALETTE ScreenPalette = 0;
	HDC ScreenDC;
	INT32 Counter;

	//*** Reset everything in the system palette to black
	for(Counter = 0; Counter < 256; Counter++)
	{
		Palette.aEntries[Counter].peRed = 0;
		Palette.aEntries[Counter].peGreen = 0;
		Palette.aEntries[Counter].peBlue = 0;


		Palette.aEntries[Counter].peFlags = PC_NOCOLLAPSE;
	}

	//*** Create, select, realize, deselect, and delete the palette
	ScreenDC = GetDC(NULL);
	ScreenPalette = CreatePalette((LOGPALETTE *)&Palette);
	if (ScreenPalette)
	{
		ScreenPalette = SelectPalette(ScreenDC,ScreenPalette,FALSE);
		RealizePalette(ScreenDC);
		ScreenPalette = SelectPalette(ScreenDC,ScreenPalette,FALSE);
		DeleteObject(ScreenPalette);
	}
	ReleaseDC(NULL, ScreenDC);
#endif // EXCLUDE_FROM_RALPH, EXCLUDE_FROM_XARALX
}

HPALETTE DIBUtil::MakeIdentityPalette(PALETTEENTRY aRGB[], INT32 nColors)
{
	ClearSystemPalette();
	return CreateIdentityPalette( aRGB, nColors );
}



/********************************************************************************************

>	static BitmapConvertHint DIBUtil::CalcConvertHint( DWORD ScreenBPP, HDC hDC )

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Rewrote Andy's original function)
	Created:	4/7/95 (21/2/95)
	Inputs:		ScreenBPP - Indicates the output BPP for which the hint is required
				hDC - should be a device that supports SetPixel. (May be NULL if ScreenBPP
				is less than 16)

	Returns:	Suitable value for hinting in bitmap conversions.
				(CONVHINT_SCREEN4 or _SCREEN8, or any of the 16 or 24 bit CONVHINT_FINAL? values)

	Purpose:	Determines RGB arrangements for 15/16-bit devices which is needed during
				dithering in those modes. User is responsible for preserving the state of
				the pixel at 0,0 if required. Returns CONVHINT_NONE if an error occurs.

	SeeAlso:	BitmapConvertHint; DIBUtil::GetGavinBlitFormat

********************************************************************************************/

BitmapConvertHint DIBUtil::CalcConvertHint( DWORD ScreenBPP, wxDC *pDC )
{
	switch(ScreenBPP)
	{
		case 4:
			return(CONVHINT_SCREEN4);

		case 8:
			return(CONVHINT_SCREEN8);

		case 16:
		case 24:
		case 32:
			// Drop through to the code below to determine a proper 16/24bpp hint
			// (Note: Some cards say they are 24bpp when in fact they're 16bpp, so we must
			// do this for both values)
			break;

		default:
			ERROR2(CONVHINT_NONE, "Unknown output screen BPP");
	}

	// Writes all 256 values to each gun, and counts the number of different values which are 
	// returned - i.e. a 5 bit gun cannot return more than 32 values
	BYTE LastValue[3] = {0, 0, 0};
	BYTE     Count[3] = {0, 0, 0};
	wxMemoryDC	memdc;
	wxBitmap	bitmap( 1, 1 );
	memdc.SelectObject( bitmap );
	for ( UINT32 Value=0x00; Value<0x100; Value++ )
	{
		wxColour colour;
		memdc.SetPen(wxPen(wxColour(Value,Value,Value)));
		memdc.DrawPoint(0,0);
		memdc.GetPixel(0,0,&colour);
		if ( colour.Red()!=LastValue[0] )
		{
			LastValue[0] = colour.Red();
			Count[0]++;
		}
		if ( colour.Green()!=LastValue[1] )
		{
			LastValue[1] = colour.Green();
			Count[1]++;
		}
		if ( colour.Blue()!=LastValue[2] )
		{
			LastValue[2] = colour.Blue();
			Count[2]++;
		}
	}
	memdc.SelectObject( wxNullBitmap );

	// Now determine how many bits would be needed to store the number of values we generated
	// for each gun. Note that this code will return (eg) 5 bits for all values between 17-32 inclusive
	for (INT32 Gun = 0; Gun < 3; Gun++)
	{
		INT32 BitPos = 0;
		while (Count[Gun] > 0)
		{
			BitPos++;
			Count[Gun] /= 2;
		}
		Count[Gun] = BitPos;
	}

	TRACEALL( wxT("16bpp RGB gun configuration auto-detected as %ld.%ld.%ld\n"),
				Count[0], Count[1], Count[2]);

	// OK. Now determine the enum constant which represents this configuration (if any)
	switch(Count[0])
	{
		case 5:
			if (Count[2] == 5)
			{
				if (Count[1] == 5)
					return(CONVHINT_FINAL555);
				
				if (Count[1] == 6)
					return(CONVHINT_FINAL565);
			}
			break;

		case 6:
			if (Count[1] == 5 && Count[2] == 5)
				return(CONVHINT_FINAL655);

			if (Count[1] == 6 && Count[2] == 4)
				return(CONVHINT_FINAL664);
			break;

		case 8:
			if (Count[1] == 8 && Count[2] == 8)
				return(CONVHINT_FINAL24);
			break;
	}

	// Nope. It's an unknown signature. We'll just have to try 555 and hope for the best
	TRACEALL( wxT("Unknown 16bpp RGB gun configuration (%ld.%ld.%ld). Defaulting to CONVHINT_NONE\n"),
				Count[0], Count[1], Count[2]);
	return(CONVHINT_NONE);
}


/********************************************************************************************

>	static DWORD DIBUtil::GetGavinBlitFormat(DWORD ScreenBPP, DWORD BitmapBPP,
												BitmapConvertHint ScreenHint)


	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/7/95

	Inputs:		ScreenBPP -		The depth of the screen/bitmap we'll be plotting to
				BitmapBPP - 	The depth of the Gavin bitmap we're plotting to
				ScreenHint -	The conversion hint as returned by DIBUtil::CalcConvertHint

	Returns:	An approprite hinting value for use. This will default to 2 (555 hint) if a
				better hint can't be found.

	Purpose:	Determines, from the given inputs, what number should be passed to Gavin
				routines SetSolidColour and SetUpBitmap (the BPP and BitmapFormat params)
				in order to get the best possible result (and/or to ensure no dithering
				occurs). Generally, this is only needed in 16bpp screen modes, but may
				be called at any time, as it returns safe defaults if not in 16bpp.

********************************************************************************************/

DWORD DIBUtil::GetGavinBlitFormat(DWORD ScreenBPP, DWORD BitmapBPP, BitmapConvertHint ScreenHint)
{
	BOOL Transparent = (BitmapBPP == 32);

	if (Transparent)
	{
		// Otherwise, we use the ConvertHint value to determine a suitable Gavin Format value
		switch (ScreenHint)
		{
			case CONVHINT_FINAL565:
				return(0);

			case CONVHINT_FINAL655:
				return(1);

			case CONVHINT_FINAL555:
			case CONVHINT_FINAL16:		// Unknown 16 bit mode - default to 555 hint
				return(2);

			case CONVHINT_FINAL664:
				return(3);

			case CONVHINT_SCREEN4:
			case CONVHINT_SCREEN8:
			case CONVHINT_FINAL24:
			case CONVHINT_NONE:
			default:
				// Drop through to the default return value (2)
				break;
		}
	}

	return(2);
}



/********************************************************************************************

>	static BOOL DIBUtil::MakeTransparentBitmap ( LPBITMAPINFO	pPseudoColourInfo,
												 LPBYTE			pPseudoColourBits,
												 LPBITMAPINFO	pMonochromeInfo,
												 LPBYTE			pMonochromeBits,
							 					 const BYTE		TransColour )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> (from Alex code)
	Created:	22/5/96
	Inputs:		pPseudoColourInfo	The info header for the image we're applying the mask to.
				pPseudoColourBits	The bits for the image we want to apply the mask to.
				pMonochromeInfo		The info header for the mask image.
				pMonochromeBits		The bits for the image mask.
				TransColour			The index of the transparent colour.
	Returns:	TRUE/FALSE for success/failure.
	Purpose:	Applies the monochrome bitmap as a mask to a bitmap, setting pixels in the
				to the image to the TransColour if the equivelent pixel in the mask is not
				set.
	SeeAlso:	-
	Scope:		Static

********************************************************************************************/

BOOL DIBUtil::MakeTransparentBitmap ( LPBITMAPINFO	pPseudoColourInfo,
									  LPBYTE		pPseudoColourBits,
									  LPBITMAPINFO	pMonochromeInfo,
									  LPBYTE		pMonochromeBits,
							 		  const BYTE	TransColour )
{
	// Get the number of transparent colours used from the bitmap preview dialogue.
PORTNOTE("dialog","Removed BmapPrevDlg usage")
#ifndef EXCLUDE_FROM_XARALX
	INT32 NumberOfTransparentColours = BmapPrevDlg::GetNumberOfTransparentColours();
#endif
	// Do we want to make the background transparent?
	BOOL bHaveBackground = FALSE;

	// If there is a transparent colour, then set the flag to indicate that there's
	// a transparent background used.
PORTNOTE("dialog","Removed BmapPrevDlg usage")
#ifndef EXCLUDE_FROM_XARALX
	for ( INT32 j=0; j < NumberOfTransparentColours; j++ )
	{
		if ( BmapPrevDlg::GetTransparentColour ( j ) == TransColour )
		{
			bHaveBackground = TRUE;
			break;
		}
	}
#endif

	// Parameter checks
	ERROR2IF ( pPseudoColourInfo == NULL || pPseudoColourBits == NULL, FALSE,
			   "DIBUtil::MakeTransparentBitmap missing psuedo colour bitmap" );
	ERROR2IF ( pMonochromeInfo   == NULL || pMonochromeBits   == NULL, FALSE,
			   "DIBUtil::MakeTransparentBitmap missing monochrome bitmap" );
	
	BITMAPINFOHEADER* pImageBMI = &( pPseudoColourInfo->bmiHeader );
	BITMAPINFOHEADER* pMaskBMI  = &( pMonochromeInfo->bmiHeader );

	ERROR2IF ( pMaskBMI->biBitCount != 1, FALSE,
			   "DIBUtil::MakeTransparentBitmap Monochrome not 1BPP");
	ERROR2IF ( ( pImageBMI->biWidth != pMaskBMI->biWidth ) ||
			   ( pImageBMI->biHeight != pMaskBMI->biHeight ), FALSE,
			   "DIBUtil::MakeTransparentBitmap hasn't been passed identical bitmaps." );
	
	BYTE* pImagePixel	= pPseudoColourBits;
	BYTE* pMaskPixel	= pMonochromeBits;

	switch (pImageBMI->biBitCount)
	{
	case 8:
		{
			// Pixels in the image are one per byte
			for (INT32 y = 0; y < pImageBMI->biHeight; y++)
			{
				// The monochrome bitmap uses one bit per pixel to store the mask data,
				// and bbit is used to extract the bit representing the current pixel.
				INT32 bbit = 7;

				for (INT32 x = 0; x < pImageBMI->biWidth; x++)
				{
					// If the mask contains a bit corresponding to the current pixel,
					// the mask is set for this point, so set the current colour to
					// match the transparent colour.
					if( bHaveBackground && ( ( *pMaskPixel ) & ( 1 << bbit ) ) )
					{
						*pImagePixel = TransColour;
					}

					// Otherwise compare the pixel to the transparent colours. If
					// they're the same then replace the colour's index with the
					// transparency index.
					else
					{
PORTNOTE("dialog","Removed BmapPrevDlg usage")
#ifndef EXCLUDE_FROM_XARALX
						for ( INT32 i=0; i < NumberOfTransparentColours; i++ )
						{
							if ( *pImagePixel == BmapPrevDlg::GetTransparentColour ( i ) )
							{
								*pImagePixel = TransColour;

								// Finished looking for this pixel.
								break;
							}
						}
#endif
					}

					// Increment the image pointer.
					pImagePixel++;

					// Increment the mask pointer.
					if (!(bbit--))
					{
						pMaskPixel++;
						bbit = 7;
					}
				}

				// Each scanline is word-aligned so adjust the pointers.
				pImagePixel	= reinterpret_cast<LPBYTE> ( ( reinterpret_cast<DWORD_PTR>
							  ( pImagePixel ) + 3 ) & ~3 );
				pMaskPixel	= reinterpret_cast<LPBYTE> ( ( reinterpret_cast<DWORD_PTR>
							  ( pMaskPixel ) + ( ( bbit==7 ) ? 3 : 4 ) ) & ~3 );
			}
		}

		ERROR3IF ( static_cast<DWORD> ( pImagePixel - pPseudoColourBits )
				   != pImageBMI->biSizeImage,
				   "Missed image pixels");

		break;

	case 4 :
		{
			// Pixels in the image are two per byte
			ERROR3IF(TransColour > 15, "Trans Colour was out of the 4bpp range");
			const BYTE RealTransColour = min( TransColour, BYTE(15) );
			const INT32 ScanlineWidth = DIBUtil::ScanlineSize(pImageBMI->biWidth, 4);

			for (INT32 y = 0; y < pImageBMI->biHeight; y++)
			{
				INT32 bbit = 7;
				BYTE* ScanLineStart = pImagePixel;
				for (INT32 x = 0; x < pImageBMI->biWidth; x++)
				{
					// pixel is in either lower or upper nibble
					BYTE UpperNibble = (*pImagePixel) >> 4;
					BYTE LowerNibble = (*pImagePixel) & 0xF;
//					BYTE Pixel = (x%2 == 0) ? UpperNibble : LowerNibble;
					if ((*pMaskPixel) & (1<<bbit))
					{
						if (x%2 == 0)
							*pImagePixel = (RealTransColour << 4) | LowerNibble;
						else
							*pImagePixel = (UpperNibble << 4) | RealTransColour;
					}
					
PORTNOTE("dialog","Removed BmapPrevDlg usage")
#ifndef EXCLUDE_FROM_XARALX
					for( INT32 i=0; i < NumberOfTransparentColours; i++ )
					{
						//  Go through all the transparent colours. If the bitmap is using one
						//  of these colours, then change its value to the transparent colour.
						if( Pixel == BmapPrevDlg::GetTransparentColour( i ) )
						{
							if (x%2 == 0)
								*pImagePixel = (RealTransColour << 4) | LowerNibble;
							else
								*pImagePixel = (UpperNibble << 4) | RealTransColour;
							//  Finished looking for this pixel.
							i = NumberOfTransparentColours;
						} 
					}
#endif

					if (x%2==1)
						pImagePixel++;

					if (!(bbit--))
					{
						pMaskPixel++;
						bbit = 7;
					}
				}
				// Each scanline is word-aligned so adjust the pointers
				pImagePixel = ScanLineStart+ScanlineWidth;
				pMaskPixel = LPBYTE( ( (DWORD_PTR(pMaskPixel)) + ( ( bbit == 7 ) ? 3 : 4 ) ) & ~ 3 );
			}
		}

		ERROR3IF ( static_cast<DWORD> ( pImagePixel - pPseudoColourBits )
				   != pImageBMI->biSizeImage,
				   "Missed image pixels");

		break;

	case 1 :
		{
			const INT32 MonoScanlineWidth = DIBUtil::ScanlineSize ( pImageBMI->biWidth, 1 );

			// One pixel = one bit so we can apply the mask via bitwise operations
			for (INT32 loop = 0; loop < MonoScanlineWidth*pImageBMI->biHeight; loop++)
			{
				//  Go through all the transparent colours. If the bitmap is using one
				//  of these colours, then change its value to the transparent colour.
				
PORTNOTE("dialog","Removed BmapPrevDlg usage")
#ifndef EXCLUDE_FROM_XARALX
				if( NumberOfTransparentColours == 2 )
				{
					pPseudoColourBits[loop] = 0;
				}
				else if( NumberOfTransparentColours == 1 )
				{
					if( BmapPrevDlg::m_bIsOrderedDither )
					{
						if( BmapPrevDlg::GetTransparentColour( 0 ) == 1 )
						{
							pPseudoColourBits[loop] = pPseudoColourBits[loop];
						}
						else //  Colour #1 is the transparent one.
						{
							if( pPseudoColourBits[loop] > 0 )
								pPseudoColourBits[loop] = 255 - pPseudoColourBits[loop];
							else
								pPseudoColourBits[loop] = 255;
						}
					}
					else
					{
						if( BmapPrevDlg::GetTransparentColour( 0 ) == 0 )
						{
							pPseudoColourBits[loop] = pPseudoColourBits[loop];
						}
						else //  Colour #1 is the transparent one.
						{
							if( pPseudoColourBits[loop] > 0 )
								pPseudoColourBits[loop] = 255 - pPseudoColourBits[loop];
							else
								pPseudoColourBits[loop] = 255;
						}
					}
				}
#endif
			}
		}

		break;

	default:
		ERROR2(FALSE, "DIBUtil::MakeTransparentBitmap PseudoColour bitmap not 8/4/1 BPP");
	}

	return TRUE;
}



/********************************************************************************************

>	static BOOL DIBUtil::MakeBitmapMask(LPBITMAPINFO pPseudoColourInfo, LPBYTE pPseudoColourBits,
									    LPBITMAPINFO pMonochromeInfo, LPBYTE pMonochromeBits)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:		pPseudoColourInfo	the info header for the image with transparent colour in
				pPseudoColourBits	the bits for the image with transparent col in
				pMonochromeColourInfo	the info header for the image to make into the mask
				pMonochromeColourBits	the bits for the image to make into the mask
				Transparency 		the oclour to use for transparent
	Returns:	True if converted it ok.
	Purpose:	Makes the monochrome bitmap (created with) into a mask for displaying the
				Pseudocolour bitmap.
				The mask must be 8bpp instead of the implied of 1bpp as applying it as a
				transparency requires this.
	SeeAlso:	-
	Scope:		Static

********************************************************************************************/

BOOL DIBUtil::MakeBitmapMask(LPBITMAPINFO pPseudoColourInfo, LPBYTE pPseudoColourBits,
							 LPBITMAPINFO pMonochromeInfo, LPBYTE pMonochromeBits,
							 const BYTE TransCol)
{
	//const BYTE TransCol = GIF_TRANS_COLOUR;

	ERROR2IF(pPseudoColourInfo==NULL || pPseudoColourBits==NULL, FALSE, "DIBUtil::MakeBitmapMask missing psuedo colour bitmap");
	ERROR2IF(pMonochromeInfo==NULL || pMonochromeBits==NULL, FALSE, "DIBUtil::MakeBitmapMask missing monochrome bitmap");
	
	BITMAPINFOHEADER * pABMI=&(pPseudoColourInfo->bmiHeader);
	BITMAPINFOHEADER * pBBMI=&(pMonochromeInfo->bmiHeader);
	LPBYTE pABytes=pPseudoColourBits;
	LPBYTE pBBytes=pMonochromeBits;

	ERROR2IF(pABMI->biBitCount != 8, FALSE, "DIBUtil::MakeBitmapMask PseudoColour bitmap not 8BPP");
	ERROR2IF(pBBMI->biBitCount != 8, FALSE, "DIBUtil::MakeBitmapMask Monochrome not 8BPP");
	ERROR2IF((pABMI->biWidth != pBBMI->biWidth) || (pABMI->biHeight != pBBMI->biHeight), FALSE,
				"DIBUtil::MakeBitmapMask needs identically sized bitmaps and that's not what you've given it.");
	
	LPBYTE A=pABytes;
	LPBYTE B=pBBytes;
	//
	// Gavin: I've not tested the following code change, so if there are problems,
	// revert to the old code.
	//
#if 1
	for (INT32 y = 0; y < pABMI->biHeight; y++)
	{
		for (INT32 x = 0; x < pABMI->biWidth; x++)
			B[x] = -(A[x]==TransCol) ;
		A += pABMI->biWidth ;
		B += pABMI->biWidth ;
		A = (LPBYTE)( (((DWORD_PTR)(A))+3) & ~ 3); // word align
		B = (LPBYTE)( (((DWORD_PTR)(B))+3) & ~ 3); // word align
	}
#else
	memset(B, 0, pBBMI->biSizeImage); // blank the mask
	
	for (INT32 y = 0; y < pABMI->biHeight; y++)
	{
		//INT32 bbit = 7;
		for (INT32 x = 0; x < pABMI->biWidth; x++)
		{
			// Change == to != if the mask comes out the wrong way around.
			if ((*(A++))==TransCol)
				(*B) = 0xFF;

			// Monochrome mask code
			//	(*B)|=1<<bbit;
			//if (!(bbit--))
			//{
			//	B++;
			//	bbit=7;
			//}
			B++;
		}
		A=(LPBYTE)( (((DWORD)(A))+3) & ~ 3); // word align
		B=(LPBYTE)( (((DWORD)(B))+3) & ~ 3); // word align
		//B=(LPBYTE)( (((DWORD)(B))+((bbit==7)?3:4)) & ~ 3); // word align
	}
#endif
	ERROR3IF( (((DWORD_PTR)A-(DWORD_PTR)pABytes)!=pABMI->biSizeImage), "Alex got his other algorithm wrong in one way");
	ERROR3IF( (((DWORD_PTR)B-(DWORD_PTR)pBBytes)!=pBBMI->biSizeImage), "Alex got his other algorithm wrong in another way");

	// Bet noone has thought to give the mono bitmap a palette yet
	RGBQUAD * Palette= pMonochromeInfo->bmiColors;
//	ERROR2IF((pBBMI->biClrUsed < 2), FALSE, "DIBUtil::MakeBitmapMask not passed a large enough mono palette");
//	Palette[0].rgbRed=Palette[0].rgbBlue=Palette[0].rgbGreen=0;
//	Palette[1].rgbRed=Palette[1].rgbBlue=Palette[1].rgbGreen=255;

	// Must ensure white is white and black is black!
	ERROR2IF((pBBMI->biClrUsed < 256), FALSE, "DIBUtil::MakeBitmapMask not passed a large enough mono palette");
	Palette[0].rgbRed=Palette[0].rgbBlue=Palette[0].rgbGreen=0;
	Palette[255].rgbRed=Palette[255].rgbBlue=Palette[255].rgbGreen=255;

	return TRUE;
}

/********************************************************************************************

>	static BOOL DIBUtil::MakeBitmapSmaller(UINT32 OldWidth, UINT32 OldHeight, UINT32 BaseX, UINT32 BaseY, 
									   	   UINT32 NewWidth, UINT32 NewHeight, UINT32 BPP, LPBYTE pBits)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:		OldWidth, OldHeight = the old height of the bitmap
				BaseX, BaseY = the coordinates in the old bitmap that map to (0,0) in the new bitmap
				NewWidth, NewHeight = the new height of the bitmap
				BPP = bits per pixel (must be 8, 16 or 32)
	Outputs:	pBits altered
	Returns:	TRUE if converted it ok else FALSE & error set
	Purpose:	Resizes a bitmap
	SeeAlso:	-
	Scope:		Static

This means that the rectangle (BaseX, BaseY) - (BaseX+NewWidth, BaseY+NewHeight) form the
o/p bitmap. A ReAlloc can be performed after this call. The rect is inclusive/exclusive.

********************************************************************************************/


BOOL DIBUtil::MakeBitmapSmaller(UINT32 OldWidth, UINT32 OldHeight, UINT32 BaseX, UINT32 BaseY, 
									   UINT32 NewWidth, UINT32 NewHeight, UINT32 BPP, LPBYTE pBits)
{
	ERROR2IF(!pBits, FALSE, "DIBUtil::MakeBitmapSmaller would really appreciate a bitmap");
	ERROR2IF((BPP!=8) && (BPP!=16) && (BPP!=32), FALSE, "DIBUtil::MakeBitmapSmaller passed invalid BPP");
	ERROR2IF(OldWidth<NewWidth, FALSE, "DIBUtil::MakeBitmapSmaller thinks you are making the bitmap wider");
	ERROR2IF(OldHeight<NewHeight, FALSE, "DIBUtil::MakeBitmapSmaller thinks you are making the bitmap taller");

	ERROR2IF((BaseX>OldWidth) || (BaseY>OldHeight), FALSE, "DIBUtil::MakeBitmapSmaller passed bad Base");
	ERROR2IF((BaseX+NewWidth>OldWidth) || (BaseY+NewHeight>OldHeight), FALSE, "DIBUtil::MakeBitmapSmaller passed bad rect");
	// as these are unsighned all we have to do is check against zero
	ERROR2IF(!OldWidth || !NewWidth || !OldHeight || !NewHeight, FALSE, "DIBUtil::MakeBitmapSmaller passed bad width/height");

	UINT32 OldWidthR=OldWidth;
	UINT32 NewWidthR=NewWidth; // with wastage values
	UINT32 BPPShift=2;

 	switch (BPP)
	{
		case 8:
			OldWidthR=(OldWidth+3)&~3;
			NewWidthR=(NewWidth+3)&~3;
			BPPShift=0;
			break;
		case 16:
			OldWidthR=(OldWidth+1)&~1;
			NewWidthR=(NewWidth+1)&~1;
			BPPShift=1;
			break;
		case 32:
		default:
			break;
	}


 	UINT32 NewByteW = DIBUtil::ScanlineSize(NewWidth, BPP);	
 	UINT32 OldByteW = DIBUtil::ScanlineSize(OldWidth, BPP);	

	LPBYTE pSrc=pBits+OldByteW*BaseY+(BaseX<<BPPShift);
	LPBYTE pDest=pBits;
	
	for (UINT32 y=0; y<NewHeight; y++)
	{
		memcpy(pDest, pSrc, NewByteW);
		pDest+=NewByteW;
		pSrc+=OldByteW;	
	}
	return TRUE;
}

/********************************************************************************************

>	static BOOL DIBUtil::Init()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if worked ok else FALSE & error set
	Purpose:	Reads our prefs
	SeeAlso:	-
	Scope:		Static

********************************************************************************************/

BOOL DIBUtil::Init()
{
	return
		Camelot.DeclareSection( wxT("DisplayKludges"), 10)
		&& Camelot.DeclarePref(NULL, wxT("RenderDirection4"),  &RenderDirection4, FALSE, TRUE )
		&& Camelot.DeclarePref(NULL, wxT("RenderDirection8"),  &RenderDirection8, FALSE, TRUE )
		&& Camelot.DeclarePref(NULL, wxT("RenderDirection16"), &RenderDirection16, FALSE, TRUE )
		&& Camelot.DeclarePref(NULL, wxT("RenderDirection24"), &RenderDirection24, FALSE, TRUE )
		&& Camelot.DeclarePref(NULL, wxT("SubbandConversionSize"), &SubbandConversionSize, 0x200, 0x1000000);
}

/***********************************************************************************************
>	static UINT32 DIBUtil::CalcPaletteSize(UINT32 Depth, bool bUsingBitFields, UINT32 UsedColours = 0)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>/Richard - Fixed by Jonathan (I removed the code duplication and added the
				bUsingBitFields flag)
	Created:	19/5/95 (& 12/2/2001)
	Inputs:		Depth			- The colour depth being used
				bUsingBitFields	- Are we using a bitfield (ie do we have a mask for true colour
								bitmaps - see docs for BITMAPINFOHEADER)
				UsedColours		- The number of colours the bitmap uses or 0 if we do not have
								a limited palette
	Returns: 	The size of the palette required for this bitmap depth
	Purpose:	Calculates the size of the 
	Notes:		Used to be in SGLibOil but moved 30/6/95 by Neville as required for 
				transparent GIF import.
	SeeAlso:	DIBUtil::CopyKernelBitmap;
***********************************************************************************************/
UINT32 DIBUtil::CalcPaletteSize(UINT32 Depth, bool bUsingBitFields, UINT32 UsedColours)
{
	ERROR3IF((bUsingBitFields && (Depth != 16 || Depth != 32)), "Invalid use of DIBUtil::CalcPaletteSize()");

	// Calculate the size of the DIB data
	UINT32 extras = 0;
	switch (Depth)
	{
		case 1:
			if (UsedColours > 0 && UsedColours <= 2)
				extras = UsedColours;
			else
				extras = 2;
			break;
		case 4:
			if (UsedColours > 0 && UsedColours <= 16)
				extras = UsedColours;
			else
				extras = 16;
			break;
		case 8:
			if (UsedColours > 0 && UsedColours <= 256)
				extras = UsedColours;
			else
				extras = 256;
			break;
		case 16:
		case 32:
			if (bUsingBitFields)
				extras = 3;
			break;
	}
	ERROR3IF(extras == 0 && Depth != 16 && Depth != 24 && Depth != 32, "Unsupported DIB depth!");

	return(extras * sizeof(RGBQUAD));
}


/***********************************************************************************************

>	static BOOL DIBUtil::CopyBitmap(LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits,
								    LPBITMAPINFO *pDestInfo, LPBYTE *pDestBits)
					  
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95

	Inputs:		pSourceInfo 	- A information header for the bitmap to be copied
				pSourceBits		- the actual bits for the bitmap to be copied
	Outputs:	pDestInfo		- pointer to the copied information block
				pDestBits		- pointer ot the copied bits data
	
	Returns: 	True if copied ok, false otherwise
	
	Purpose:	Allocates a new DIB information header block and bits data and copies the source
				bitmap data across to the new bitmap.
				At present, just copies the actual bits data, the header will be the defaults
				plus width, height and colour depth. Not interested in the palette and other
				information stored in the bitmap info header.

	SeeAlso:	TI_GIFFilter::RenderTransparencyMask
				
***********************************************************************************************/

//#ifdef _DEBUG
void DebugMemCopy(LPBYTE pDest, LPBYTE pSrc, UINT32 size)
{
	memcpy(pDest, pSrc, size);
}
//#endif

BOOL DIBUtil::CopyBitmap(LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits,
						 LPBITMAPINFO *pDestInfo, LPBYTE *pDestBits)
{
	// In case of early set these return pointers to NULL
	*pDestInfo = NULL;
	*pDestBits = NULL;

	ERROR2IF(pSourceInfo == NULL || pSourceBits == NULL, FALSE, "DIBUtil::CopyBitmap bad source bitmap");

	// Get a pointer to the useful bitmap header information for things like the size of the
	// bitmap and colour depth. 
	LPBITMAPINFOHEADER pSourceInfoHeader = &(pSourceInfo->bmiHeader); 
	DWORD biCompression = pSourceInfoHeader->biCompression;

	// Try and allocate the detsination bitmap to be the same size and colour depth as the source
	*pDestInfo = AllocDIB( pSourceInfoHeader->biWidth, pSourceInfoHeader->biHeight,
						   pSourceInfoHeader->biBitCount,
						   pDestBits );

	(*pDestInfo)->bmiHeader.biCompression = biCompression;

	// failed to allocate the bitmap so return false.
	if (*pDestInfo == NULL || *pDestBits == NULL )
		return FALSE;
	
	// Now copy the data from the source to the destination
	DWORD BitmapSize = pSourceInfo->bmiHeader.biSizeImage;
#ifdef _DEBUG
	DebugMemCopy(*pDestBits, pSourceBits, BitmapSize);
#else
	memcpy(*pDestBits, pSourceBits, BitmapSize);
#endif


//	LPBYTE A = pDestBMBytes;
//	LPBYTE B = pBMBytes;
//	for (INT32 y = 0; y < pBMInfoHeader->biHeight; y++)
//	{
//		for (INT32 x = 0; x < pBMInfoHeader->biWidth; x++)
//		{
//			*(A++) = *(B++);
//		}
//		A=(LPBYTE)( (((DWORD)(A))+3) & ~ 3); // word align
//		B=(LPBYTE)( (((DWORD)(B))+3) & ~ 3); // word align
//	}

	return TRUE;
}	

/***********************************************************************************************

>	static BOOL DIBUtil::CopyEntireBitmap(LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits,
										  LPBITMAPINFO *pDestInfo, LPBYTE *pDestBits)
					  
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/96

	Inputs:		pSourceInfo 	- A information header for the bitmap to be copied
				pSourceBits		- the actual bits for the bitmap to be copied
	Outputs:	pDestInfo		- pointer to the copied information block
				pDestBits		- pointer ot the copied bits data
	
	Returns: 	True if copied ok, false otherwise
	
	Purpose:	Allocates a new DIB information header block and bits data and copies the source
				bitmap data across to the new bitmap.
				At present, just copies the actual bits data, the header will be the defaults
				plus width, height and colour depth. Not interested in the palette and other
				information stored in the bitmap info header.
				Same as CopyBitmap but actually copies the rest of the data as well. This includes:-
					Palette information
					Header information
					Bits information

	SeeAlso:	DIBUtil::CopyBitmap; PhotoShopPlugIn::Apply;
				
***********************************************************************************************/

BOOL DIBUtil::CopyEntireBitmap(LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits,
							   LPBITMAPINFO *pDestInfo, LPBYTE *pDestBits)
{
	// In case of early set these return pointers to NULL
	*pDestInfo = NULL;
	*pDestBits = NULL;

	ERROR2IF(pSourceInfo == NULL || pSourceBits == NULL, FALSE, "DIBUtil::CopyBitmap bad source bitmap");

	// Get a pointer to the useful bitmap header information for things like the size of the
	// bitmap and colour depth. 
	LPBITMAPINFOHEADER pSourceInfoHeader = &(pSourceInfo->bmiHeader); 

	// Try and allocate the detsination bitmap to be the same size and colour depth as the source
	*pDestInfo = AllocDIB( pSourceInfoHeader->biWidth, pSourceInfoHeader->biHeight,
						   pSourceInfoHeader->biBitCount,
						   pDestBits );

	// failed to allocate the bitmap so return false.
	if (*pDestInfo == NULL || *pDestBits == NULL )
		return FALSE;
	
	// Now copy the data from the source to the destination
	DWORD BitmapSize = pSourceInfo->bmiHeader.biSizeImage;
	memcpy(*pDestBits, pSourceBits, BitmapSize);

	// The older slower way of doing that
//	LPBYTE A = *pDestBits;
//	LPBYTE B = pSourceBits;
//	for (INT32 y = 0; y < pSourceInfoHeader->biHeight; y++)
//	{
//		for (INT32 x = 0; x < pSourceInfoHeader->biWidth; x++)
//		{
//			*(A++) = *(B++);
//		}
//		A=(LPBYTE)( (((DWORD)(A))+3) & ~ 3); // word align
//		B=(LPBYTE)( (((DWORD)(B))+3) & ~ 3); // word align
//	}

	// Ensure remaining info header entries are correct
	LPBITMAPINFOHEADER pDestInfoHeader = &((*pDestInfo)->bmiHeader); 
	pDestInfoHeader->biPlanes			= pSourceInfoHeader->biPlanes;
	pDestInfoHeader->biCompression		= pSourceInfoHeader->biCompression;
	pDestInfoHeader->biXPelsPerMeter	= pSourceInfoHeader->biXPelsPerMeter;
	pDestInfoHeader->biYPelsPerMeter	= pSourceInfoHeader->biYPelsPerMeter;
	pDestInfoHeader->biClrImportant		= pSourceInfoHeader->biClrImportant;
	pDestInfoHeader->biClrUsed			= pSourceInfoHeader->biClrUsed;

	LPRGBQUAD pSourcePalette = &(pSourceInfo->bmiColors[0]);
	LPRGBQUAD pDestPalette = &((*pDestInfo)->bmiColors[0]);
	UINT32 NumberOfColours = pSourceInfoHeader->biClrUsed;
	// If we have zero colours on a bitmap which is 8bpp or less then this is bad.
	// This should be translated as the maximum number of colours allowed
	if (pSourceInfoHeader->biBitCount <= 8 && NumberOfColours == 0)
		NumberOfColours = 1 << pSourceInfoHeader->biBitCount;
	CopyPalette(pSourcePalette, pDestPalette, NumberOfColours);

	return TRUE;
}	

/***********************************************************************************************

>	static BOOL DIBUtil::CopyPalette(LPRGBQUAD pSourcePalette, LPRGBQUAD pDestPalette, UINT32 NumberOfColours)
					  
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/96
	Inputs:		pSourcePalette 	- the source palette in RGBQUAD form
				pDestPalette	- the destination palette in RGBQUAD form
				NumberOfColours	- number of palette entries to copy
	Returns:	True if copied correctly, False otherwise.
	Purpose:	Copy a palette from a bitmap which has been duplicated and so has the same colour
				depth and number of palette entries, to a destination bitmap. Assumes palettes
				already allocated as part of the duplication process.
	SeeAlso:	DIBUtil::CopyEntireBitmap;

***********************************************************************************************/

BOOL DIBUtil::CopyPalette(LPRGBQUAD pSourcePalette, LPRGBQUAD pDestPalette, UINT32 NumberOfColours)
{
	// Check if there is any palette information to copy, if not just return
	if (pSourcePalette == NULL || pDestPalette == NULL || NumberOfColours == 0)
		return TRUE;

	for (UINT32 i = 0; i < NumberOfColours; i++)
	{
        pDestPalette->rgbBlue		= pSourcePalette->rgbBlue;
        pDestPalette->rgbGreen		= pSourcePalette->rgbGreen;
        pDestPalette->rgbRed		= pSourcePalette->rgbRed;
        pDestPalette->rgbReserved	= pSourcePalette->rgbReserved;
		pDestPalette++;
		pSourcePalette++;
	}

	return TRUE;
}

/***********************************************************************************************

>	static KernelBitmap *DIBUtil::CopyKernelBitmap(KernelBitmap *pKernelBitmap, BOOL IsTemp = FALSE)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95

	Inputs:		pKernelBitmap 	- A kernel bitmap to copy
				IsTemp			- TRUE means only create the bitmap as temporary and hence
								  don't show in bitmap gallery
								  FALSE means create properly and show in bitmap gallery (default)
	Outputs:	-
	Returns: 	Pointer to newly allocated KernelBitmap structure
	
	Purpose:	Allocates a new CWxBitmap and KernelBitmap pointing to it.
				Note these are completely new versions containing the same data as the
				old ones.

				Deleting the KernelBitmap will handle the other stuff ok...
				
	Notes:		Most of the code in camelot assumes that the OILBitmap is always in
				memory, so creating a new kernelbitmap would just pop in a pointer
				to the OILBitmap. This is bad when thumbnails come into the equation
				since these come and go like no tomorrow...

				This should really be popped in dibutils at some point in the 'I want a 
				big rebuild'ing future...
				Used to be in SGLibOil but moved 30/6/95 by Neville as required for 
				transparent GIF import.

***********************************************************************************************/

KernelBitmap *DIBUtil::CopyKernelBitmap(KernelBitmap *pKernelBitmap, BOOL IsTemp)
{
	OILBitmap *pOilBitmap = pKernelBitmap->ActualBitmap;
	ERROR3IF(pOilBitmap == NULL, "Unattached kernel bitmap found!");
	CWxBitmap* pWBitmap = (CWxBitmap*)pOilBitmap;
	LPBITMAPINFO Info = pWBitmap->BMInfo;
	LPBYTE Bytes = pWBitmap->BMBytes;
	ERROR3IF(Info == NULL, "Bitmap info is null - oh god !");
	ERROR3IF(Info->bmiHeader.biSizeImage == 0, "Bitmap data size is zero - I'm about to screw up");

	LPBITMAPINFO NewInfo = NULL;

	LPBYTE NewBytes = NULL;
	NewInfo = AllocDIB(Info->bmiHeader.biWidth, Info->bmiHeader.biHeight, Info->bmiHeader.biBitCount, &NewBytes);

	if (NewInfo == NULL || NewBytes == NULL)
	{
		ERROR3("Couldn't allocate enough memory for new CWxBitmap");
		return(NULL);
	}

	UINT32 PalSize = CalcPaletteSize(pWBitmap->GetBPP(), Info->bmiHeader.biCompression == BI_BITFIELDS);
	DWORD HeaderSize = Info->bmiHeader.biSize +	PalSize;
	DWORD BitmapSize = Info->bmiHeader.biSizeImage;

	// Copy the DIB into the block of memory (copy the header and body separately as they
	// may not lie in contiguous memory)
	memcpy(NewInfo, Info, HeaderSize);
	memcpy(NewBytes, Bytes, BitmapSize);

	CWxBitmap *NewCWxBitmap = new CWxBitmap(NewInfo, NewBytes);
	ERROR3IF(NewCWxBitmap == NULL, "NULL CWxBitmap created");

	// Create the associated kernel object to go with the oil object
	KernelBitmap *TheKBToReturn = new KernelBitmap((OILBitmap *)NewCWxBitmap, IsTemp);

	return TheKBToReturn;
}

/***********************************************************************************************

>	static BOOL DIBUtil::CopyBitmapSection(LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits,
										  LPBITMAPINFO *pDestInfo, LPBYTE *pDestBits
										  INT32 SourceTop, INT32 SourceLeft)
					  
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
				Altered by Phil to clip sensibly 13/05/2004
	Created:	24/11/99

	Inputs:		pSourceInfo 	- A information header for the bitmap to be copied
				pSourceBits		- the actual bits for the bitmap to be copied
				pDestInfo		- An information header for the bitmap to be copied to
				pDestBits		- An initialised bitmap to be copied into
				SourceTop		- the y value of the the top left part of the section to be copied
				SourceLeft		- the x value of the top left part of the section to be copied
	Outputs:	pDestInfo		- pointer to the copied information block
				pDestBits		- pointer ot the copied bits data

	
	Returns: 	True if copied ok, false otherwise
	
	Purpose:	To copy a section of the source bitmap to the destination. Note that unlike the
				other copy functions in this file the destination header must in not allocated so  
				you must do it yourself. Likewise palette info.

	SeeAlso:	DIBUtil::CopyBitmap; PhotoShopPlugIn::Apply;
				
***********************************************************************************************/


BOOL DIBUtil::CopyBitmapSection(LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits,
								  LPBITMAPINFO pDestInfo,	LPBYTE pDestBits,
								  INT32 SourceTop, INT32 SourceLeft)
{
//TRACEUSER( "Phil", _T("CopyBitmapSection Source %d, %d, Dest %d %d\n"), pSourceInfo->bmiHeader.biWidth, pSourceInfo->bmiHeader.biHeight, pDestInfo->bmiHeader.biWidth, pDestInfo->bmiHeader.biHeight);
//TRACEUSER( "Phil", _T("CopyBitmapSection Source Compression %x\n"), pSourceInfo->bmiHeader.biCompression);
	if (pSourceInfo == NULL || pSourceBits == NULL || 
		pDestInfo == NULL || pDestBits == NULL)
	{
		ERROR3("One of the input pointers is NULL");
		return FALSE;
	}

	if (pSourceInfo->bmiHeader.biBitCount<8 || pDestInfo->bmiHeader.biBitCount<8)
	{
		ERROR3("This routine can't handle bit depths < 1 byte per pixel (no masking)");
		return FALSE;
	}

	if (pSourceInfo->bmiHeader.biBitCount != pDestInfo->bmiHeader.biBitCount)
	{
		ERROR3("This routine can't handle differing src and dest bit depths");
		return FALSE;
	}

	// Clip the dest rectangle against the source rectangle
	INT32 CopyWidth = pDestInfo->bmiHeader.biWidth;
	INT32 CopyHeight = pDestInfo->bmiHeader.biHeight;
	LPBYTE pDest = pDestBits;
	if (SourceLeft<0)
	{
		CopyWidth += SourceLeft;	// Subtract left offset from width to copy
		pDest += ScanlineBytes(-SourceLeft, pDestInfo->bmiHeader.biBitCount);
		SourceLeft = 0;
	}
	if (SourceTop<0)
	{
		CopyHeight += SourceTop;	// Subtract top offset from height to copy
		pDest += ScanlineSize(pDestInfo->bmiHeader.biWidth, pDestInfo->bmiHeader.biBitCount) * -SourceTop;
		SourceTop = 0;
	}
	if ((SourceLeft+CopyWidth) > pSourceInfo->bmiHeader.biWidth)
	{
		CopyWidth = (pSourceInfo->bmiHeader.biWidth - SourceLeft);
	}
	if ((SourceTop+CopyHeight) > pSourceInfo->bmiHeader.biHeight)
	{
		CopyHeight = (pSourceInfo->bmiHeader.biHeight - SourceTop);
	}

	// Check whether the clipping has left anything worth rendering...
	if (CopyWidth<=0 || CopyHeight<=0)
	{
		TRACEUSER("Phil", wxT("CopyBitmapSection clipped to NULL rectangle\n") );
		return TRUE;
	}
	
	// first move the source pointer to the correct location
	INT32 LeftOffset = ScanlineBytes(SourceLeft, pSourceInfo->bmiHeader.biBitCount);
	INT32 SourceScanlineBytes = ScanlineSize(pSourceInfo->bmiHeader.biWidth, pSourceInfo->bmiHeader.biBitCount);

	LPBYTE pSource = pSourceBits + LeftOffset;
	pSource += SourceScanlineBytes * SourceTop;

	// we are going to have to iterate through the sourcebitmap scanline by scanline and copy the
	// section that we want.  First work out how many bytes we want per scanline

	INT32 DestScanlineBytes = ScanlineSize(pDestInfo->bmiHeader.biWidth, pDestInfo->bmiHeader.biBitCount);
	INT32 CopyBytes = ScanlineBytes(CopyWidth, pSourceInfo->bmiHeader.biBitCount);
	for (INT32 Count = 0; Count < CopyHeight; Count++)
	{
		memcpy(pDest, pSource, CopyBytes);

		// move the destination pointer up by one scanline
		pDest += DestScanlineBytes;
		pSource += SourceScanlineBytes;
	}

	// If we have a 24-bit bitmap, no alpha channel, make sure we haven't inadvertently blitted
	// in a bunch of alpha values
	// Das ist eine Uber-bodge...
	if (pDestInfo->bmiHeader.biCompression == BI_RGB && pDestInfo->bmiHeader.biBitCount==32)
	{
		LPDWORD pBits = (LPDWORD) pDestBits;
		for (UINT32 i=0; i<pDestInfo->bmiHeader.biSizeImage; i+=sizeof(DWORD))
		{
			*pBits &= 0x00FFFFFF;
			pBits++;
		}
	}
	
	// if we have an 8 bit bitmap then copy the palette also
	if (pDestInfo->bmiHeader.biBitCount == 8 &&
		pSourceInfo->bmiHeader.biBitCount == 8)
	{
		for (INT32 i = 0; i < 256; i++)
			pDestInfo->bmiColors[i] = pSourceInfo->bmiColors[i];
	}

	return TRUE;
}

/***********************************************************************************************
>	static BOOL DIBUtil::CountColoursUsed(BITMAPINFO* pInfo, BYTE* pBits, UINT32** pResultsArray)
					  
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/96
	Inputs:		pInfo	Pointer to a BITMAPINFO describing the bitmap
				pBits	Pointer to the bitmap bits
	Outputs:	pResultsArray	Allocated and filled in with results
	Returns: 	TRUE/FALSE for success/failure
	Purpose:	Counts the usage for each palette index in the bitmap.
				NOTE: Caller is responsible for CCFreeing the results array.
***********************************************************************************************/
BOOL DIBUtil::CountColoursUsed(BITMAPINFO* pInfo, BYTE* pBits, UINT32** pResultsArray)
{
	ERROR2IF(pInfo==NULL || pBits==NULL, FALSE, "NULL entry param");
	ERROR3IF(*pResultsArray != NULL, "pResultsArray was not NULL (are you leaking memory?)");

	// Claim and initalise the results array.
	const UINT32 NumColours = 1 << pInfo->bmiHeader.biBitCount;
	ERROR2IF(NumColours>256, FALSE, "DIBUtil::CountColoursUsed only handles palletted DIBs");
	*pResultsArray = (UINT32*) CCMalloc(NumColours*sizeof(UINT32));
	if (*pResultsArray == NULL)
		return FALSE;
	memset(*pResultsArray, 0, NumColours*sizeof(UINT32));

	// Calculate the length of a scanline in bytes as they are word aligned.
	UINT32 ScanLineLength = 0;
	switch (pInfo->bmiHeader.biBitCount)
	{
		case 8:
			ScanLineLength = ((pInfo->bmiHeader.biWidth+3)/4)*4;
			break;
		case 4:
			ScanLineLength = ((pInfo->bmiHeader.biWidth+7)/8)*4;
			break;
		case 1:
			ScanLineLength = ((pInfo->bmiHeader.biWidth+31)/32)*4;
			break;
		default:
			ERROR2(FALSE, "Unknown depth");
	}

	// Run through the pixels, incrementing the counters
	for (INT32 y = 0; y < pInfo->bmiHeader.biHeight; y++)
	{
		BYTE* pCurrentPixel = pBits + y*ScanLineLength;
		BYTE CurrentBitMask = 0x80;

		for (INT32 x = 0; x < pInfo->bmiHeader.biWidth; x++)
		{
			switch (pInfo->bmiHeader.biBitCount)
			{
				case 8:
					// simple - one pixel per byte
					(*pResultsArray)[(*pCurrentPixel)]++;
					pCurrentPixel++;
					break;

				case 4:
					// Two pixels per byte
					if (x%2 == 0)
					{
						(*pResultsArray)[((*pCurrentPixel) >> 4)]++;
					}
					else
					{
						(*pResultsArray)[((*pCurrentPixel) & 0xF)]++;
						pCurrentPixel++;
					}
					break;

				case 1:
					// Tricker - one pixel per bit.
					if ((*pCurrentPixel) & CurrentBitMask)
						(*pResultsArray)[1]++;
					else
						(*pResultsArray)[0]++;

					if (CurrentBitMask == 1)
					{
						CurrentBitMask = 0x80;
						pCurrentPixel++;
					}
					else
						CurrentBitMask = CurrentBitMask >> 1;
					break;

				default:
					ERROR2(FALSE, "Unknown depth");
			}
		}
	}

	return TRUE;
}

/***********************************************************************************************

  >	static INT32 DIBUtil::FindLeastUsedColour(BITMAPINFO* pInfo, UINT32* pResultsArray)
					  
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Will code
	Created:	19/8/97
	Inputs:		pInfo			Pointer to a BITMAPINFO describing the bitmap
				pResultsArray	filled in with results from CountColoursUsed
	Returns: 	The index of the transparent colour to use
	Purpose:	Takes the results from DIBUtil::CountColoursUsed and works out which colour is
				the best one to use as a transparent colour by finding the least used. 
				NOTE: Caller is responsible for CCFreeing the results array.

***********************************************************************************************/

INT32 DIBUtil::FindLeastUsedColour(BITMAPINFO* pInfo, UINT32* pResultsArray)
{
	ERROR2IF(pInfo == NULL || pResultsArray == NULL, -1, "NULL entry param");
	INT32 TransIndex = -1;

	INT32 MinIndex = 0;

	// Attempt to find an unused colour in the results array
	INT32 Bpp = pInfo->bmiHeader.biBitCount;
	INT32 MaxColours = 1 << Bpp;
	for (INT32 loop = 0; loop < MaxColours; loop++)
	{
		if (pResultsArray[loop] == 0 && TransIndex == -1)	// For consistancy use the first available index
			TransIndex = loop;
		if (pResultsArray[loop] < pResultsArray[MinIndex])
			MinIndex = loop;
	}

	if (TransIndex == -1)
	{
		// Make white transparent for 1bpp images
		if (Bpp == 1)
			TransIndex = 1;
		else
			TransIndex = MinIndex;
	}

	return TransIndex;
}

#if !defined(EXCLUDE_FROM_RALPH)

/***********************************************************************************************

>	static size_t DIBUtil::GetOptimalPaletteWorkspaceSize()

					  
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96

	Inputs:		-
	Outputs:	-
	
	Returns: 	Size of workspace required
	
	Purpose:	Returns size of workspace required by optimal palette routines.

				This function just calls gavins C code in 'winoil\gpalopt.cpp'.

	SeeAlso:	DIBUtil::OptimalPaletteInitialise
				
***********************************************************************************************/

size_t DIBUtil::GetOptimalPaletteWorkspaceSize()
{
//	return ::GetOptimalPaletteWorkspaceSize();
	ASSERT(FALSE);
	return 0;
}

/***********************************************************************************************

>	static BOOL DIBUtil::OptimalPaletteInitialise( INT32* Stats )

					  
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96

	Inputs:		Stats, pointer to stats table to initialise
				Use GetOptimalPaletteWorkspaceSize() to create this pointer

	Outputs:	Memory pointed to be 'Stats' is initialised
	
	Returns: 	TRUE if all ok
	
	Purpose:	Initialises workspace required by optimal palette routines.
				Stats should have INT32 size of GetOptimalPaletteWorkspaceSize()

				This function just calls gavins C code in 'winoil\gpalopt.cpp'.

	SeeAlso:	DIBUtil::GetOptimalPaletteWorkspaceSize
				
***********************************************************************************************/

BOOL DIBUtil::OptimalPaletteInitialise( void* Stats )
{
	ERROR3IF(Stats == NULL, "NULL pointer passed to DIBUtil::OptimalPaletteInitialise");
	if (Stats == NULL)
		return FALSE;

//	::OptimalPaletteInitialise( Stats );
	
	ASSERT(FALSE);
	return TRUE;
}


BOOL DIBUtil::ExactPaletteInitialise( LPLOGPALETTE pExactPalette )
{
	ERROR3IF(pExactPalette == NULL, "NULL pointer passed to DIBUtil::ExactPaletteInitialise");
	if (pExactPalette == NULL)
		return FALSE;

//	::ExactPaletteInitialise( pExactPalette );	
//	return TRUE;
	ASSERT(0);
	return FALSE;

}


/***********************************************************************************************

>	static BOOL DIBUtil::GenOptimalPaletteStats( INT32* Stats, RGBQUAD* pBitmap, size_t Size )

					  
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96

	Inputs:		Stats should have INT32 size of GetOptimalPaletteWorkspaceSize() and should
				have been initialised by a previous call to OptimalPaletteInitialise.

				pBitmap points to an RGB bitmap of length Size.
								
	Outputs:	Stats table is updated to reflect the colours in the bitmap given
	
	Returns: 	TRUE if all ok
	
	Purpose:	The purpose of this function is to build a table of statistics about one
				or more bitmaps prior to calling GenOptimalPalette.

				One or more bitmaps can be processed a strip at a time by multple calls to
				this function, or each bitmap may be processed by a single call.

				This function just calls gavins C code in 'winoil\gpalopt.cpp'.

	SeeAlso:	DIBUtil::GenOptimalPalette
				
***********************************************************************************************/

BOOL DIBUtil::GenOptimalPaletteStats( void* Stats, RGBQUAD* pBitmap, size_t Size )
{
	ERROR3IF(Stats == NULL, "NULL Stats pointer passed to DIBUtil::GenOptimalPaletteStats");
	if (Stats == NULL)
		return FALSE;

	ERROR3IF(pBitmap == NULL, "NULL Bitmap pointer passed to DIBUtil::GenOptimalPaletteStats");
	if (pBitmap == NULL)
		return FALSE;

	ERROR3IF(Size == 0, "Bad size passed to DIBUtil::GenOptimalPaletteStats");
	if (Size == 0)
		return FALSE;

//	::GenOptimalPaletteStats( Stats, pBitmap, Size );
	ASSERT(FALSE);

	return TRUE;
}






/***********************************************************************************************

>	static BOOL DIBUtil::GenOptimalPalette( void* Stats, PLOGPALETTE pPalette, const size_t MaxColours )

					  
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96

	Inputs:		Stats should have size of GetOptimal8bppPaletteWorkspaceSize() and should
				have been initialised by a previous call to Optimal8bppPaletteInitialise
				and then setup by one or more calls to GenOptimal8bppPaletteStats.

				pPalette should point at a logical palette. If the number of entries is
				greater than MaxColours then the remaining palette entries will be cleared
				to black.

				MaxColours is the maximum number of colours to place into the palette.
				Note that the remaining colours are cleared to black. Sensible values
				would be
	
						256 - Full 8bpp palette
						255 - Allow room for a transparency colour
						252 - Allow room for the four windows colours
						251 - Allow room for a transparency colour and the four windows
							  colours.

						 16 - Full 4bpp palette
						 15 - Allow room for a transparency colour

						  2 - 1bpp palette
						  1 - 1bpp palette with transparency

				Note also that if more than 236 colours are added to the 8bpp palette
				then windows will not be able to correct display the palette on an 8bpp
				display since it will not be able to realize all the palette entries.
							
	Outputs:	An optimal palette is generated using the Stats provided
	
	Returns: 	TRUE if all ok
	
	Purpose:	The purpose of this function is to generate an optimal palette suitable
				for the bitmaps for which statistics have previously been generated.

				This function just calls gavins C code in 'winoil\gpalopt.cpp'.

	SeeAlso:	DIBUtil::GenOptimalPaletteStats
				
***********************************************************************************************/

BOOL DIBUtil::GenOptimalPalette( void* Stats, PLOGPALETTE pPalette, const size_t MaxColours )
{
	ERROR3IF(Stats == NULL, "NULL Stats pointer passed to DIBUtil::GenOptimalPalette");
	if (Stats == NULL)
		return FALSE;

	ERROR3IF(pPalette == NULL, "NULL Palette pointer passed to DIBUtil::GenOptimalPalette");
	if (pPalette == NULL)
		return FALSE;

	// the old call
	//::GenOptimalPalette( Stats, pPalette, MaxColours );
	// used to do this in one go.
	// The new call bellow splits this into two functions where the later one can be called
	// agian on its own later to reduce the number of colours
	//::GenOptimalPalette( Stats );
	//::GetOptimalPalette( pPalette, MaxColours );
	ASSERT(FALSE);

	return TRUE;
}


BOOL DIBUtil::CalculateNumberOfColoursInBitmap( LPLOGPALETTE pExactPalette, RGBQUAD* pBitmap, size_t Size )
{
	ERROR3IF(pExactPalette == NULL, "NULL palette pointer passed to DIBUtil::CalculateNumberOfColoursInBitmap");
	if (pExactPalette == NULL)
		return FALSE;

	ERROR3IF(pBitmap == NULL, "NULL Bitmap pointer passed to DIBUtil::CalculateNumberOfColoursInBitmap");
	if (pBitmap == NULL)
		return FALSE;

	ERROR3IF(Size == 0, "Bad size passed to DIBUtil::CalculateNumberOfColoursInBitmap");
	if (Size == 0)
		return FALSE;

//	return ::CalculateNumberOfColoursInBitmap( pExactPalette, pBitmap, Size );
	ASSERT(0);
	return FALSE;
}



BOOL DIBUtil::Optimal4bppPaletteInitialise_1stPass()
{
//	::Optimal4bppPaletteInitialise_1stPass();

	return TRUE;
}

BOOL DIBUtil::Optimal4bppPaletteInitialise_2ndPass()
{
//	::Optimal4bppPaletteInitialise_2ndPass();

	return TRUE;
}

BOOL DIBUtil::GenOptimal4bppPaletteStats_1stPass( RGBQUAD* pBitmap, size_t Size )
{
//	::GenOptimal4bppPaletteStats_1stPass( pBitmap, Size );

	return TRUE;
}

BOOL DIBUtil::GenOptimal4bppPaletteStats_2ndPass( RGBQUAD* pBitmap, size_t Size )
{
//	::GenOptimal4bppPaletteStats_2ndPass( pBitmap, Size );

	return TRUE;
}

BOOL DIBUtil::GenOptimal4bppPalette( PLOGPALETTE pPalette, const size_t MaxColours )
{
//	::GenOptimal4bppPalette( pPalette, MaxColours );

	return TRUE;
}


size_t DIBUtil::GetOptimal8bppPaletteWorkspaceSize()
{
//	return ::GetOptimal8bppPaletteWorkspaceSize();
	ASSERT(FALSE);
	return 0;
}

BOOL DIBUtil::Optimal8bppPaletteInitialise( INT32* Stats )
{
	ERROR3IF(Stats == NULL, "NULL pointer passed to DIBUtil::OptimalPaletteInitialise");
	if (Stats == NULL)
		return FALSE;

//	::Optimal8bppPaletteInitialise( Stats );
	ASSERT(FALSE);

	return TRUE;
}

BOOL DIBUtil::GenOptimal8bppPaletteStats( INT32* Stats, RGBQUAD* pBitmap, size_t Size )
{
//	::GenOptimal8bppPaletteStats( Stats,  pBitmap, Size );
	ASSERT(FALSE);

	return TRUE;
}

BOOL DIBUtil::GenOptimal8bppPalette( INT32* Stats, PLOGPALETTE pPalette, const size_t MaxColours )
{
//	::GenOptimal8bppPalette( Stats, pPalette, MaxColours );
	ASSERT(FALSE);

	return TRUE;
}

#endif // EXCLUDE_FROM_RALPH


/********************************************************************************************

>	static BOOL DIBUtil::GenGreyscalePalette(LPRGBQUAD lpPalette, const size_t PaletteSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/96
	Inputs:		PaletteSize		number of palette entries that need generating
				lpPalette		pointer to the palette defined in the DIB
				
	Returns:	True if worked ok, False otherwise.
	Purpose:	Generate a greyscale palette for a greyscale DIB.

********************************************************************************************/

BOOL DIBUtil::GenGreyscalePalette(LPRGBQUAD lpPalette, const size_t PaletteSize)
{
	ERROR2IF(lpPalette == NULL,FALSE,"PNGUtil::GenerateGreyPalette no lpPalette present");
	ERROR2IF(PaletteSize <= 0,FALSE,"PNGUtil::GenerateGreyPalette no PNG palette entries present");

	// Work out the value we require per step so on a:-
	//	256 colour palette we can have 256 steps of 1
	//	16 colour palette we have 16 steps of 16 (16 * 16 = 256)
	INT32					inc = INT32(256 / PaletteSize);
	INT32					value = 0;
	for( size_t i = 0; i < PaletteSize; i++ )
	{
		lpPalette->rgbBlue = value;
		lpPalette->rgbGreen = value;
		lpPalette->rgbRed = value;
		lpPalette->rgbReserved = 0;
		lpPalette++;
		value += inc;
		if (value > 255)
			value = 255;
	}
	
	return TRUE;
}

/********************************************************************************************

>	static BOOL DIBUtil::GenGreyscalePaletteTriple(RGBTRIPLE *pPalette, const size_t PaletteSize)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/97
	Inputs:		PaletteSize		number of palette entries that need generating
				pPalette		pointer to the palette defined in the DIB
				
	Returns:	True if worked ok, False otherwise.
	Purpose:	Generate a packed greyscale palette for a greyscale DIB.

				Based on Neville's routine DIBUtil::GenGreyscalePalette
				WEBSTER - markn 29/1/97

********************************************************************************************/

BOOL DIBUtil::GenGreyscalePaletteTriple(RGBTRIPLE* pPalette, const size_t PaletteSize)
{
	ERROR2IF(pPalette == NULL,FALSE,"PNGUtil::GenerateGreyPalette no pPalette present");
	ERROR2IF(PaletteSize <= 0,FALSE,"PNGUtil::GenerateGreyPalette no PNG palette entries present");

	// Work out the value we require per step so on a:-
	//	256 colour palette we can have 256 steps of 1
	//	16 colour palette we have 16 steps of 16 (16 * 16 = 256)
	INT32 inc = INT32(256/PaletteSize);
	INT32 value = 0;
	for (size_t i = 0; i < PaletteSize; i++ )
	{
		pPalette->rgbtBlue = value;
		pPalette->rgbtGreen = value;
		pPalette->rgbtRed = value;
		pPalette++;
		value += inc;
		if (value > 255)
			value = 255;
	}
	
	return TRUE;
}

/********************************************************************************************

>	static BOOL DIBUtil::Convert24to8(LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits,
									  LPBITMAPINFO *pDestInfo, LPBYTE *pDestBits)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/96
	Inputs:		pSourceInfo 	- A information header for the bitmap to be copied
				pSourceBits		- the actual bits for the bitmap to be copied
				pPalette	- pointer to the palette in RGBTIPLE format to use when converting
				NumberOfPaletteEntries - entries in this palette 

	Outputs:	pDestInfo		- pointer to the copied information block
				pDestBits		- pointer ot the copied bits data
	
	Returns: 	True if copied ok, false otherwise
	Purpose:	Generate an 8 bit form of a bitmap from a 24 bit form using the given palette.

********************************************************************************************/

BOOL DIBUtil::Convert24to8( LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits,
							LPBITMAPINFO *pDestInfo, LPBYTE *pDestBits,
							RGBTRIPLE *pPalette, UINT32 NumberOfPaletteEntries )
{
	// In case of early set these return pointers to NULL
	*pDestInfo = NULL;
	*pDestBits = NULL;

	ERROR2IF(pSourceInfo == NULL || pSourceBits == NULL, FALSE, "DIBUtil::Convert24to8 bad source bitmap");
	ERROR2IF(pPalette == NULL || NumberOfPaletteEntries == 0, FALSE, "DIBUtil::Convert24to8 bad palette info");

	// Get a pointer to the useful bitmap header information for things like the size of the
	// bitmap and colour depth. 
	LPBITMAPINFOHEADER pSourceInfoHeader = &(pSourceInfo->bmiHeader); 

	ERROR2IF(pSourceInfoHeader->biBitCount != 24,FALSE,"DIBUtil::Convert24to8 bad source depth");

	// Try and allocate the detsination bitmap to be the same size and colour depth as the source
	*pDestInfo = AllocDIB( pSourceInfoHeader->biWidth, pSourceInfoHeader->biHeight,
						   8, pDestBits );

	// failed to allocate the bitmap so return false.
	if (*pDestInfo == NULL || *pDestBits == NULL )
		return FALSE;

	LPBITMAPINFOHEADER pDestInfoHeader = &((*pDestInfo)->bmiHeader); 
	
	// We will need the palette in LOGPALETTE for use in the conversion
	// We must allocate the palette to be the maximum size as otherwise people like Gavin
	// blow up
	LPLOGPALETTE pLogPalette = NULL;
	const UINT32 MaxColours = 256;
	pLogPalette = DIBUtil::AllocateLogPalette(MaxColours);
	if (pLogPalette == NULL)
		return FALSE;

	// Now copy the RGBTRIPLE palette into this and into the destination bitmap
	// Get a pointer to the palette in the destination bitmap
	LPRGBQUAD lpPalette = NULL;
	lpPalette = &((*pDestInfo)->bmiColors[0]);	// pointer to colours table
	//pLogPalette->palVersion = 0x300;
	pLogPalette->palNumEntries = NumberOfPaletteEntries;
	RGBTRIPLE *pPal = pPalette;
	for (UINT32 i = 0; i < NumberOfPaletteEntries; i++)
	{
#if defined(__WXMSW__)
		lpPalette->rgbRed = pPal->rgbtRed;
#else
		lpPalette->rgbRed = pPal->rgbtBlue;
#endif
		pLogPalette->palPalEntry[i].peRed = pPal->rgbtRed;
		lpPalette->rgbGreen = pPal->rgbtGreen;
		pLogPalette->palPalEntry[i].peGreen = pPal->rgbtGreen;
#if defined(__WXMSW__)
		lpPalette->rgbBlue = pPal->rgbtBlue;
#else
		lpPalette->rgbBlue = pPal->rgbtRed;
#endif
		pLogPalette->palPalEntry[i].peBlue = pPal->rgbtBlue;
		pLogPalette->palPalEntry[i].peFlags = 0x00;
		lpPalette->rgbReserved = 0x00;
		lpPalette++;
		pPal++;
	}
	// Fill in all other entries as black and do not use
	for (UINT32 i = NumberOfPaletteEntries; i < MaxColours; i++)
	{
		pLogPalette->palPalEntry[i].peRed	= 0x00;
		pLogPalette->palPalEntry[i].peGreen = 0x00;
		pLogPalette->palPalEntry[i].peBlue	= 0x00;
		pLogPalette->palPalEntry[i].peFlags = 0xFF;		// mark as do not use
		lpPalette->rgbRed		= 0x00;
		lpPalette->rgbGreen		= 0x00;
		lpPalette->rgbBlue		= 0x00;
		lpPalette->rgbReserved	= 0xFF;	// mark as do not use
		lpPalette++;
	}

	UINT32 DitherType = XARADITHER_NONE;	// request no dithering
	DIBConvert *pDoConvert = NULL;		// the convert DIB

	// Do some conversion using the DIBConvert class
	pDoConvert = DIBConvert::Create( pSourceInfoHeader->biBitCount, pDestInfoHeader->biBitCount,
									 pSourceInfoHeader->biWidth, pLogPalette,  DitherType);
	// If this fails then exit now
	if (pDoConvert == NULL)
	{
		if (pLogPalette)
			CCFree(pLogPalette);
		return FALSE;
	}
	
	// Do the actual conversion process
	INT32 ChunkHeight = 16;		// We will do the conversion in chunks, in case we ask for dithering
	INT32 Height = pSourceInfoHeader->biHeight;
	INT32 SourceWidth = DIBUtil::ScanlineSize( pSourceInfoHeader->biWidth, pSourceInfoHeader->biBitCount ) * ChunkHeight;
	INT32 DestWidth = DIBUtil::ScanlineSize( pDestInfoHeader->biWidth, pDestInfoHeader->biBitCount ) * ChunkHeight;
	BOOL IsFirstStrip = TRUE;			// Whether we are on the first strip or not
	INT32 CurrentYPos = 0;				// current line number
	LPBYTE SourceData = pSourceBits;	// pointer to the source data
	LPBYTE DestData = *pDestBits;		// pointer to the destination data
	while (CurrentYPos < Height)
	{ 
		// Work out the size of the export chunk left, normally ExportChunkHeight but
		// if at the end of export may be a small strip left.
		const UINT32 ThisBit = min( UINT32(Height - CurrentYPos), (UINT32)ChunkHeight );
		if (!pDoConvert->Convert( SourceData, DestData, ThisBit, IsFirstStrip ))
			return FALSE;							// stop if conversion failed

		IsFirstStrip = FALSE;				// Done first strip
		SourceData	+= SourceWidth;			// Move on by a lines worth
		DestData	+= DestWidth;			// Move on by a lines worth
		CurrentYPos += INT32(ThisBit);		// Move down by a strips worth
	}
	
	// Put back the recommended palette by destructing the convert function.
	if (pDoConvert)
	{
		delete pDoConvert;
		pDoConvert = NULL;
	}	

	if (pLogPalette)
	{
		CCFree(pLogPalette);
		pLogPalette = NULL;
	}

	// If we got this far then more than likely we have waht we want
	return TRUE;
}



/********************************************************************************************

>	static BOOL DIBUtil::Convert32to8(LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits,
									  LPBITMAPINFO *pDestInfo, LPBYTE *pDestBits)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/2005
	Inputs:		pSourceInfo 	- A information header for the bitmap to be copied
				pSourceBits		- the actual bits for the bitmap to be copied
				pPalette	- pointer to the palette in RGBTIPLE format to use when converting
				NumberOfPaletteEntries - entries in this palette 

	Outputs:	pDestInfo		- pointer to the copied information block
				pDestBits		- pointer ot the copied bits data
	
	Returns: 	True if copied ok, false otherwise
	Purpose:	Generate an 8 bit form of a bitmap from a 32 bit form using the given palette.

********************************************************************************************/

BOOL DIBUtil::Convert32to8( LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits,
							LPBITMAPINFO *pDestInfo, LPBYTE *pDestBits,
							RGBTRIPLE *pPalette, UINT32 NumberOfPaletteEntries )
{
	// In case of early set these return pointers to NULL
	*pDestInfo = NULL;
	*pDestBits = NULL;

	ERROR2IF(pSourceInfo == NULL || pSourceBits == NULL, FALSE, "DIBUtil::Convert32to8 bad source bitmap");
	ERROR2IF(pPalette == NULL || NumberOfPaletteEntries == 0, FALSE, "DIBUtil::Convert32to8 bad palette info");

	// Get a pointer to the useful bitmap header information for things like the size of the
	// bitmap and colour depth. 
	LPBITMAPINFOHEADER pSourceInfoHeader = &(pSourceInfo->bmiHeader); 

	ERROR2IF(pSourceInfoHeader->biBitCount != 32,FALSE,"DIBUtil::Convert32to8 bad source depth");

	// Try and allocate the detsination bitmap to be the same size and colour depth as the source
	*pDestInfo = AllocDIB( pSourceInfoHeader->biWidth, pSourceInfoHeader->biHeight,
						   8, pDestBits );

	// failed to allocate the bitmap so return false.
	if (*pDestInfo == NULL || *pDestBits == NULL )
		return FALSE;

	LPBITMAPINFOHEADER pDestInfoHeader = &((*pDestInfo)->bmiHeader); 
	
	// We will need the palette in LOGPALETTE for use in the conversion
	// We must allocate the palette to be the maximum size as otherwise people like Gavin
	// blow up
	LPLOGPALETTE pLogPalette = NULL;
	const UINT32 MaxColours = 256;
	pLogPalette = DIBUtil::AllocateLogPalette(MaxColours);
	if (pLogPalette == NULL)
		return FALSE;

	// Now copy the RGBTRIPLE palette into this and into the destination bitmap
	// Get a pointer to the palette in the destination bitmap
	LPRGBQUAD lpPalette = NULL;
	lpPalette = &((*pDestInfo)->bmiColors[0]);	// pointer to colours table
	//pLogPalette->palVersion = 0x300;
	pLogPalette->palNumEntries = NumberOfPaletteEntries;
	RGBTRIPLE *pPal = pPalette;
	for (UINT32 i = 0; i < NumberOfPaletteEntries; i++)
	{
		lpPalette->rgbRed = pPal->rgbtRed;
		pLogPalette->palPalEntry[i].peRed = pPal->rgbtRed;
		lpPalette->rgbGreen = pPal->rgbtGreen;
		pLogPalette->palPalEntry[i].peGreen = pPal->rgbtGreen;
		lpPalette->rgbBlue = pPal->rgbtBlue;
		pLogPalette->palPalEntry[i].peBlue = pPal->rgbtBlue;
		pLogPalette->palPalEntry[i].peFlags = 0x00;
		lpPalette->rgbReserved = 0x00;
		lpPalette++;
		pPal++;
	}
	// Fill in all other entries as black and do not use
	for (UINT32 i = NumberOfPaletteEntries; i < MaxColours; i++)
	{
		pLogPalette->palPalEntry[i].peRed	= 0x00;
		pLogPalette->palPalEntry[i].peGreen = 0x00;
		pLogPalette->palPalEntry[i].peBlue	= 0x00;
		pLogPalette->palPalEntry[i].peFlags = 0xFF;		// mark as do not use
		lpPalette->rgbRed		= 0x00;
		lpPalette->rgbGreen		= 0x00;
		lpPalette->rgbBlue		= 0x00;
		lpPalette->rgbReserved	= 0xFF;	// mark as do not use
		lpPalette++;
	}

	UINT32 DitherType = XARADITHER_NONE;	// request no dithering
	DIBConvert *pDoConvert = NULL;		// the convert DIB

	// Do some conversion using the DIBConvert class
	pDoConvert = DIBConvert::Create( pSourceInfoHeader->biBitCount, pDestInfoHeader->biBitCount,
									 pSourceInfoHeader->biWidth, pLogPalette,  DitherType);
	// If this fails then exit now
	if (pDoConvert == NULL)
	{
		if (pLogPalette)
			CCFree(pLogPalette);
		return FALSE;
	}
	
	// Do the actual conversion process
	INT32 ChunkHeight = 16;		// We will do the conversion in chunks, in case we ask for dithering
	INT32 Height = pSourceInfoHeader->biHeight;
	INT32 SourceWidth = DIBUtil::ScanlineSize( pSourceInfoHeader->biWidth, pSourceInfoHeader->biBitCount ) * ChunkHeight;
	INT32 DestWidth = DIBUtil::ScanlineSize( pDestInfoHeader->biWidth, pDestInfoHeader->biBitCount ) * ChunkHeight;
	BOOL IsFirstStrip = TRUE;			// Whether we are on the first strip or not
	INT32 CurrentYPos = 0;				// current line number
	LPBYTE SourceData = pSourceBits;	// pointer to the source data
	LPBYTE DestData = *pDestBits;		// pointer to the destination data
	while (CurrentYPos < Height)
	{ 
		// Work out the size of the export chunk left, normally ExportChunkHeight but
		// if at the end of export may be a small strip left.
		const UINT32 ThisBit = min( UINT32(Height - CurrentYPos), (UINT32)ChunkHeight );
		if (!pDoConvert->Convert( SourceData, DestData, ThisBit, IsFirstStrip ))
			return FALSE;							// stop if conversion failed

		IsFirstStrip = FALSE;				// Done first strip
		SourceData	+= SourceWidth;			// Move on by a lines worth
		DestData	+= DestWidth;			// Move on by a lines worth
		CurrentYPos += INT32(ThisBit);		// Move down by a strips worth
	}
	
	// Put back the recommended palette by destructing the convert function.
	if (pDoConvert)
	{
		delete pDoConvert;
		pDoConvert = NULL;
	}	

	if (pLogPalette)
	{
		CCFree(pLogPalette);
		pLogPalette = NULL;
	}

	// If we got this far then more than likely we have waht we want
	return TRUE;
}



/********************************************************************************************

>	static BOOL DIBUtil::Convert8to32(LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits, LPQUAD pPalette
									  LPBITMAPINFO *pDestInfo, LPBYTE *pDestBits)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/2000
	Inputs:		pSourceInfo 	- A information header for the bitmap to be copied
				pSourceBits		- the actual bits for the bitmap to be copied
				pPalette		- pointer to the palette 
				pDestInfo		- pointer to an ALLOCATED bitmapinfo structure 
				pDestBits		- pointer to an ALLOCATED 32bit bitmap

	Outputs:	pDestInfo		- pointer to the copied information block
				pDestBits		- pointer ot the copied bits data
				
	
	Returns: 	True if copied ok, false otherwise
	Purpose:	Generates a 32bit form of the input 8bit bitmap, note that the destination 
				bitmapinfo and bitmap must be allocated prior to this function

********************************************************************************************/

BOOL DIBUtil::Convert8to32(LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits, RGBQUAD* pPalette,
									  LPBITMAPINFO pDestInfo, LPBYTE pDestBits)
{
	ERROR2IF(pSourceBits == NULL, FALSE, "NULL source bitmap");
	ERROR2IF(pSourceInfo == NULL, FALSE, "NULL source info");
	ERROR2IF(pDestInfo == NULL, FALSE, "NULL destination info");
	ERROR2IF(pDestBits == NULL, FALSE, "Null destination bitmap");
	
	// most info will be the same so memcpy is the quickest way
	//memcpy(pSourceInfo, pDestInfo, sizeof(BITMAPINFO));

	// these will all be the same
	pDestInfo->bmiHeader.biSize			 = sizeof(BITMAPINFOHEADER);
	pDestInfo->bmiHeader.biWidth		 = pSourceInfo->bmiHeader.biWidth;
	pDestInfo->bmiHeader.biHeight		 = pSourceInfo->bmiHeader.biHeight;
	pDestInfo->bmiHeader.biPlanes		 = pSourceInfo->bmiHeader.biPlanes;
	pDestInfo->bmiHeader.biCompression	 = pSourceInfo->bmiHeader.biCompression;
	pDestInfo->bmiHeader.biXPelsPerMeter = pSourceInfo->bmiHeader.biXPelsPerMeter;
	pDestInfo->bmiHeader.biYPelsPerMeter = pSourceInfo->bmiHeader.biYPelsPerMeter;
	pDestInfo->bmiHeader.biClrImportant	 = 0;

	// get the size of the new bitmap
	INT32 size = pDestInfo->bmiHeader.biWidth *  pDestInfo->bmiHeader.biHeight * 4;
	
	//these will be different from source
	pDestInfo->bmiHeader.biBitCount         = 32;
	pDestInfo->bmiHeader.biSizeImage        = size;
	pDestInfo->bmiHeader.biClrUsed          = 0;


	// convert from 8 bit to 32 bit
	INT32 bmpsize = pSourceInfo->bmiHeader.biWidth * pSourceInfo->bmiHeader.biHeight;
	INT32 padding = pSourceInfo->bmiHeader.biWidth % 4;
	if (padding)
		padding = 4 - padding;

	LPBYTE pSource = pSourceBits;
	LPBYTE pDest = pDestBits;
	for (INT32 i = 0; i < bmpsize; i ++)
	{
		if (i && i % pSourceInfo->bmiHeader.biWidth == 0)
			pSource += padding;
		*pDest = pPalette[*pSource].rgbBlue; 
		pDest++;
		*pDest = pPalette[*pSource].rgbGreen; 
		pDest++;
		*pDest = pPalette[*pSource].rgbRed; 
		pDest++;
		*pDest = pPalette[*pSource].rgbReserved;
		pDest++;
		pSource++;
	}
	return TRUE;

}

/********************************************************************************************

> static BOOL DIBUtil::ConvertTo8Greyscale(LPBITMAPINFO pSourceInfo,LPBYTE pSourceBits,
							  LPBITMAPINFO *ppDestInfo, LPBYTE *ppDestBits)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/1/97
	Inputs:		pSourceInfo 	- A information header for the bitmap to be copied
				pSourceBits		- the actual bits for the bitmap to be copied
	Outputs:	pDestInfo		- pointer to the 8bpp greyscale information block
				pDestBits		- pointer ot the 8bpp greyscale bits data
	
	Returns: 	True if copied ok, false otherwise
	Purpose:	Generates an 8bpp greyscale bitmap from the given 1bpp, 4bpp or 8bpp bitmap

********************************************************************************************/

BOOL DIBUtil::ConvertTo8Greyscale(LPBITMAPINFO pSourceInfo,LPBYTE pSourceBits,
							  LPBITMAPINFO *ppDestInfo, LPBYTE *ppDestBits)
{
	if (pSourceInfo == NULL || pSourceBits == NULL || ppDestInfo == NULL || ppDestBits == NULL)
	{
		ERROR3("Null parameters");
		return FALSE;
	}

	// Get useful source bitmap info
	UINT32 Width   = pSourceInfo->bmiHeader.biWidth;
	UINT32 Height  = pSourceInfo->bmiHeader.biHeight;
	UINT32 Depth	 = pSourceInfo->bmiHeader.biBitCount;

	// Calc number of colours in the palette
	UINT32 NumCols = 0;
	switch (Depth)
	{
		case 1:	NumCols = 2;  break;
		case 4:	NumCols = 16; break;
		case 8:	NumCols = 256; break;
		case 16: break;
		case 24: break;
		case 32: break;
		default:
			ERROR3("Routine can't cope");
			return FALSE;
			break;
	}

	// Create the destination 8bpp bitmap, & check it all worked
	*ppDestInfo = AllocDIB(Width,Height, 8, ppDestBits);
	if (*ppDestInfo == NULL || *ppDestBits == NULL)
		return FALSE;

	// We need a lookup table that maps the give bitmap's palette to the 256 greyscale palette
	BYTE LookUp[256];

	if (NumCols > 0)
	{
		// Generate the greyscale lookup table.
		for (UINT32 i=0;i < NumCols;i++)
		{
			// Get the RGB components of this palette entry
			double R = pSourceInfo->bmiColors[i].rgbRed;
			double G = pSourceInfo->bmiColors[i].rgbGreen;
			double B = pSourceInfo->bmiColors[i].rgbBlue;

			// Calculate the intensity of the palette entry - this maps onto a greyscale palette entry
			BYTE C = BYTE((R * 0.305) + (G * 0.586) + (B * 0.109));

			// For each pixel that references palette entry 'i', map it onto greyscale palette entry 'C'
			LookUp[i] = C;
		}
	}

	// Calc number of bytes in each scan line
	UINT32 SourceScanLineLength = DIBUtil::ScanlineSize(Width,Depth);
	UINT32 DestScanLineLength   = DIBUtil::ScanlineSize(Width,8);

	// Run through the pixels of the source, and create the pixels of the destination
	UINT32 x,y ;
	BYTE* pSourcePixel = pSourceBits ;
	BYTE*   pDestPixel = *ppDestBits ;
	switch (Depth)
	{
	case 1:
		for ( y = 0; y < Height; y++ )
		{
			BYTE* pSource = pSourcePixel ;
			BYTE Pixel;
			BYTE SourceBitMask = 0x80;
			for ( x = 0; x < Width; x++ )
			{
				// If the current bit is set, the pixel is 1, else 0
				if (*pSource & SourceBitMask)
					Pixel = 1;
				else
					Pixel = 0;
				// Look up the greyscale byte value for the 'Pixel' entry in the source palette
				pDestPixel[x] = LookUp[Pixel];
				SourceBitMask >>= 1;		// Shift mask for next bit
				if (SourceBitMask==0)
				{
					// Got last bit out of that byte, so reset mask and inc ptr to next byte
					SourceBitMask = 0x80;
					pSource++;
				}
			}
			pSourcePixel += SourceScanLineLength ;
			  pDestPixel +=   DestScanLineLength ;
		}
		break ;

	case 4:
		for ( y = 0; y < Height; y++ )
		{
			BYTE* pSource = pSourcePixel ;
			for ( x=0 ; x<Width ; x+=2 )
			{
				// Look up the greyscale byte value for the pixel entries in the source palette
				pDestPixel[x  ] = LookUp[*pSource >> 4] ;
				pDestPixel[x+1] = LookUp[*pSource & 0xF] ;
				pSource++;
			}
			pSourcePixel += SourceScanLineLength ;
			  pDestPixel +=   DestScanLineLength ;
		}
		break ;

	case 8:
		for ( y=0 ; y<Height ; y++ )
		{
			// Look up the greyscale byte values for the pixel entries in the source palette
			for ( x=0 ; x<Width ; x++ )
				pDestPixel[x] = LookUp[pSourcePixel[x]];
			pSourcePixel += SourceScanLineLength ;
			  pDestPixel +=   DestScanLineLength ;
		}
		break ;

	case 16:
		for ( y=0 ; y<Height ; y++ )
		{
			WORD* pSrc = (WORD*)pSourcePixel;
			// Convert the pixel to greyscale and store
			for ( x=0 ; x<Width ; x++ )
			{
				// 16bpp pixel value is pSrc[x]
				BYTE Grey = 0xFF;
				pDestPixel[x] = Grey;
				pSrc++;
			}
			pSourcePixel += SourceScanLineLength ;
			  pDestPixel +=   DestScanLineLength ;
		}
		break;

	case 24:
		for ( y=0 ; y<Height ; y++ )
		{
			BYTE* pSrc = pSourcePixel;
			// Convert the pixel to greyscale and store
			for ( x=0 ; x<Width ; x++ )
			{
				// 24bpp pixel is 
				BYTE Grey = (pSrc[2] * 78 + pSrc[1] * 150 + pSrc[0] * 28) >> 8;
				pDestPixel[x] = Grey;
				pSrc += 3;
			}
			pSourcePixel += SourceScanLineLength ;
			  pDestPixel +=   DestScanLineLength ;
		}
		break;

	case 32:
		for ( y=0 ; y<Height ; y++ )
		{
			// Convert the pixels to greyscale as if they are rendered on a white 
			// background and store
			BGRT* pSrc = (BGRT*)pSourcePixel;
			for ( x=0 ; x<Width ; x++ )
			{
				const BYTE Trans = pSrc->Transparency;
				const BYTE Alpha = 255 - Trans;
				BYTE Grey = (pSrc->Red * 78 + pSrc->Green * 150 + pSrc->Blue * 28) >> 8;
				pDestPixel[x] = ((Grey * Alpha) + (255 * Trans)) / 255;
				pSrc++;
			}
			pSourcePixel += SourceScanLineLength ;
			  pDestPixel +=   DestScanLineLength ;
		}
		break;
	}

//	BOOL ok = GenGreyscalePalette((*ppDestInfo)->bmiColors,256);

	return TRUE;
}


/********************************************************************************************

>	static OILBitmap* DIBUtil::Create8bppGreyscaleBitmap(KernelBitmap* pSrcBitmap)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/97
	Inputs:		pSrcBitmap = ptr to a kernel bitmap
	Returns:	ptr to an Oil bitmap containing an 8bpp greyscale image
	Purpose:	Designed to be used to create an 8bpp greyscale bitmap from a 24bpp, 4bpp or 1bpp bitmaps

				It will do nothing if the bitmap is already an 8bpp bitmap.
				In this case you should copy the kernel bitmap and call GenerateGreyscaleTable() on
				the copy.

********************************************************************************************/

OILBitmap* DIBUtil::Create8bppGreyscaleBitmap(KernelBitmap* pSrcBitmap)
{
	OILBitmap* pOilBitmap = pSrcBitmap->GetActualBitmap();
	ERROR2IF(pOilBitmap == NULL,NULL,"null OIL bitmap present!");

	// Try to import bitmap as usual binary BMP file.
	CWxBitmap* pCWxBitmap = (CWxBitmap*)pOilBitmap;
	ERROR2IF(pCWxBitmap == NULL,NULL,"null WINOIL bitmap present!");
	
	LPBITMAPINFO pSourceInfo	= pCWxBitmap->BMInfo;
	LPBYTE pSourceBits			= pCWxBitmap->BMBytes;
	
	LPBITMAPINFO pDestInfo		= NULL;
	LPBYTE pDestBits			= NULL;

	BOOL ok = FALSE;

	switch (pSrcBitmap->GetBPP())
	{
		case 1:
		case 4:
		case 8:
		case 24:
		case 32:
			ok = ConvertTo8Greyscale(pSourceInfo, pSourceBits, &pDestInfo, &pDestBits);
			break;

		default:
			ERROR3("How many bits? No chance mate");
			return NULL;
			break;
	}

	OILBitmap* pResultBitmap = NULL;

	if (ok)
	{
		// Create a new CWxBitmap to contain this data
		pResultBitmap = (OILBitmap*) new CWxBitmap(pDestInfo, pDestBits);
		if (pResultBitmap == NULL)
			FreeDIB( pDestInfo, pDestBits );
	}

	return pResultBitmap;
}


/********************************************************************************************

>	static BOOL	DIBUtil::GenerateDifferenceBitmap(LPBITMAPINFO pPreviousInfo,LPBYTE pPreviousBits,
												  LPBITMAPINFO pCurrentInfo,LPBYTE pCurrentBits,
												  LPBITMAPINFO *ppDestInfo, LPBYTE *ppDestBits,
												  INT32 TransColour,
												  BOOL * pFoundBadOverlay = NULL)
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/97
	Inputs:		pPreviousInfo 	- A information header for the bitmap of the previous frame
				pPreviousBits	- the actual bits for the bitmap of the previous frame
				pCurrentInfo	- pointer to the information block for the present frame
				pCurrentBits	- pointer to the bits data for the present frame
	Outputs:	pDestInfo		- pointer to the information block for the differenced frame
				pDestBits		- pointer to the bits data for the differenced frame
				pFoundBadOverlay - True if we cannot overlay the previous frame 
	Returns: 	True if copied ok, false otherwise
	Purpose:	We need to work out the smallest possible bitmap that needs to be output.
				Therefore, assuming that the previous bitmap or frame is the one given and
				given the present frame, then work out the differences between the two and 
				replace data which is the same with the specified transparent colour. This
				means these similar bits should compress well.
	Note:		This will return a new destination bitmap which is the difference between the
				two input bitmaps. It will be up to the caller to free up this data.

********************************************************************************************/

BOOL DIBUtil::GenerateDifferenceBitmap(LPBITMAPINFO pPreviousInfo,LPBYTE pPreviousBits,
									   LPBITMAPINFO pCurrentInfo,LPBYTE pCurrentBits,
									   LPBITMAPINFO *ppDestInfo, LPBYTE *ppDestBits, INT32 TransColour,
									   BOOL * pFoundBadOverlay)
{
	ERROR2IF(pPreviousInfo == NULL || pPreviousBits == NULL,FALSE,"GenerateDifferenceBitmap Bad previous pointers!" );
	ERROR2IF(pCurrentInfo == NULL || pCurrentBits == NULL,FALSE,"GenerateDifferenceBitmap Bad current pointers!" );
	ERROR2IF(ppDestInfo == NULL || ppDestBits == NULL,FALSE,"GenerateDifferenceBitmap Bad destination pointers!" );

	if (TransColour < 0 || TransColour > 256)
	{
		TRACEUSER( "Neville", wxT("GenerateDifferenceBitmap Bad transparent colour\n") );
		// return TRUE so that we do not fail
		return TRUE;
	}

	// Get useful source bitmap info
	UINT32 Width   = pPreviousInfo->bmiHeader.biWidth;
	UINT32 Height  = pPreviousInfo->bmiHeader.biHeight;
	UINT32 Depth	 = pPreviousInfo->bmiHeader.biBitCount;

	// Get useful destination bitmap info
	UINT32 CurrentWidth   = pCurrentInfo->bmiHeader.biWidth;
	UINT32 CurrentHeight  = pCurrentInfo->bmiHeader.biHeight;
	UINT32 CurrentDepth	 = pCurrentInfo->bmiHeader.biBitCount;

	// We can only check bitmaps that are the same size and colour depth
	if (Width != CurrentWidth || Height != CurrentHeight || Depth != CurrentDepth)
	{
		TRACEUSER("Neville", wxT("GenerateDifferenceBitmap Source and destination bitmaps different sizes and/or depths\n") );
		// return TRUE so that we do not fail
		return TRUE;
	}

	// At present, can only check bitmaps that are 8bpp
	if (Depth != 8)
	{ 
		TRACEUSER( "Neville", wxT("GenerateDifferenceBitmap Bitmap colour depth is not supported\n") );
		// return TRUE so that we do not fail
		return TRUE;
	}

	// We need to allocate the destination bitmap, so go and do that now
	// The easiest way is to produce an exact copy of the present bitmap
	BOOL ok = DIBUtil::CopyEntireBitmap(pCurrentInfo, pCurrentBits, ppDestInfo, ppDestBits);
	// If this failed then get out now
	if (!ok || *ppDestInfo == NULL || *ppDestBits == NULL)
		return FALSE;

	UINT32 ScanLineSize		= DIBUtil::ScanlineSize( Width, Depth );
	LPBYTE pPreviousData	= pPreviousBits;
	LPBYTE pCurrentData		= pCurrentBits;
	LPBYTE pDestData		= *ppDestBits;
	BOOL BadOverlay			= FALSE;
	for (UINT32 y = 0; y < Height && !BadOverlay; y++)
	{
		for (UINT32 x = 0; x < Width && !BadOverlay; x++)
		{
			// If the source and destination pixels are the same then
			// set the destination pixel to be the transparent colour
			if (pPreviousData[x] == pCurrentData[x])
				pDestData[x] = TransColour & 0xFF;
			// If the current frame has a transparent pixel and we have a 
			// non-transparent pixel in the previous frame then we must
			// stop and use a replace background type
			else if (pCurrentData[x] == (TransColour & 0xFF))
			{
				BadOverlay = TRUE;
				break;
			}
		}

		// move onto the next scanline in each bitmap
		pPreviousData	+= ScanLineSize;
		pCurrentData	+= ScanLineSize;
		pDestData		+= ScanLineSize;
	}

	// return the state of the bad overlay to the caller if they required it
	if (pFoundBadOverlay)
	{
		*pFoundBadOverlay = BadOverlay;
		
		if (BadOverlay)
		{
			// remove the diff bitmap
			if (*ppDestInfo && *ppDestBits)
			{
				FreeDIB(*ppDestInfo, *ppDestBits);
				*ppDestInfo = NULL;
				*ppDestBits = NULL;
			}
		}
	}

	// everything went ok
	return TRUE;
}

/********************************************************************************************

>	static BOOL DIBUtil::GenerateSubRegionBitmap(LPBITMAPINFO pSourceInfo,LPBYTE pSourceBits,
												 LPBITMAPINFO *ppDestInfo, LPBYTE *ppDestBits, INT32 TransColour,
												 UINT32 *pLeftOffset, UINT32 *pTopOffset)
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/97
	Inputs:		pSourceInfo 	- A information header for the bitmap of the previous frame
				pSourceBits		- the actual bits for the bitmap of the previous frame
				pDestInfo		- pointer to the information block for the present frame
				pDestBits		- pointer to the bits data for the present frame
	Outputs:	pDestInfo		- pointer to the information block for the differenced frame
				pDestBits		- pointer to the bits data for the differenced frame
				pLeftOffset		- the amount of whole columns we stripped from the left hand side
				pTopOffset		- the amount of whole rows we stripped from the top
	Returns: 	True if copied ok, false otherwise
	Purpose:	We need to work out the smallest possible bitmap that needs to be output.
				Therefore, given the present frame or bitmap, then work out the smallest
				possible bitmap that we can output which will be bordered on all four sides
				by the given transparent colour i.e. can we strip complete left hand side
				columns off the bitmap as they are all the transparent colour? Can we do the
				same for the furthest right hand side columns and the top most and bottom
				most rows.

********************************************************************************************/

BOOL DIBUtil::GenerateSubRegionBitmap(LPBITMAPINFO pSourceInfo,LPBYTE pSourceBits,
									  LPBITMAPINFO *ppDestInfo, LPBYTE *ppDestBits, INT32 TransColour,
									  UINT32 *pLeftOffset, UINT32 *pTopOffset)
{
	ERROR2IF(pSourceInfo == NULL || pSourceBits == NULL,FALSE,"GenerateSubRegionBitmap Bad source pointers!" );
	ERROR2IF(ppDestInfo == NULL || ppDestBits == NULL,FALSE,"GenerateSubRegionBitmap Bad destination pointers!" );
	ERROR2IF(pLeftOffset == NULL || pTopOffset == NULL,FALSE,"GenerateSubRegionBitmap Bad offset pointers!" );

	if (TransColour < 0 || TransColour > 255)
	{
		TRACEUSER( "Neville", wxT("GenerateSubRegionBitmap Bad transparent colour\n") );
		// return TRUE so that we do not fail
		return TRUE;
	}

	// Get useful source bitmap info
	UINT32 Width   = pSourceInfo->bmiHeader.biWidth;
	UINT32 Height  = pSourceInfo->bmiHeader.biHeight;
	UINT32 Depth	 = pSourceInfo->bmiHeader.biBitCount;

	// At present, can only check bitmaps that are 8bpp
	if (Depth != 8)
	{
		TRACEUSER( "Neville", wxT("GenerateSubRegionBitmap Bitmap colour depth is not supported\n") );
		// return TRUE so that we do not fail
		return TRUE;
	}

	UINT32 ScanLineSize		= DIBUtil::ScanlineSize( Width, Depth );

	//-------------
	// Scan through from the left hand side finding any columns that we can remove
	// Start in an illegal position so we can note any changes
	INT32 LeftMostData = -1;
	LPBYTE pSourceData	= pSourceBits;
	LPBYTE pPixel = NULL;
	BOOL FoundClearColumn = TRUE;
	for (UINT32 x = 0; x < Width; x++)
	{
		for (UINT32 y = 0; y < Height; y++)
		{
			// Check the next pixel in sequence
			pPixel = pSourceData + y * ScanLineSize + x;
			// If the pixel is not transparent then stop the search now
			if (*pPixel != (TransColour & 0xFF))
			{
				FoundClearColumn = FALSE;
				break;
			}
		}
		
		// If we found a full clear column then note this as a new left hand position
		// and move onto the next
		if (FoundClearColumn)
			LeftMostData = x;
		else
			break;
	}

	//-------------
	// Scan through from the right hand side finding any columns that we can remove
	// Start in an illegal position so we can note any changes
	UINT32 RightMostData = Width;
	pSourceData	= pSourceBits;
	FoundClearColumn = TRUE;
	for( UINT32 x = Width - 1; x >= 0 ; x-- )
	{
		for (UINT32 y = 0; y < Height; y++)
		{
			// Check the next pixel in sequence
			pPixel = pSourceData + y * ScanLineSize + x;
			// If the pixel is not transparent then stop the search now
			if (*pPixel != (TransColour & 0xFF))
			{
				FoundClearColumn = FALSE;
				break;
			}
		}
		
		// If we found a full clear column then note this as a new right hand position
		// and move onto the next
		if (FoundClearColumn)
			RightMostData = x;
		else
			break;
	}

	//-------------
	// Scan through from the top finding any rows that we can remove
	// Start in an illegal position so we can note any changes
	// REMEMBER bitmaps are stored from bottom line downwards in memory 
	INT32 TopMostData = -1;
	pSourceData	= pSourceBits + (Height - 1) * ScanLineSize;
	BOOL FoundClearRow = TRUE;
	for (UINT32 y = 0; y < Height; y++)
	{
		for (UINT32 x = 0; x < Width; x++)
		{
			// If the pixel is not transparent then stop the search now
			if (pSourceData[x] != (TransColour & 0xFF))
			{
				FoundClearRow = FALSE;
				break;
			}
		}

		// If we found a full clear column then note this as a new top position
		// and move onto the next
		if (FoundClearRow)
			TopMostData = y;
		else
			break;

		// move onto the next scanline in each bitmap
		pSourceData	-= ScanLineSize;
	}

	//-------------
	// Scan through from the bottom finding any rows that we can remove
	// Start in an illegal position so we can note any changes
	UINT32 BottomMostData = Height;
	pSourceData	= pSourceBits;
	FoundClearRow = TRUE;
	for (UINT32 y = Height - 1; y >= 0; y--)
	{
		for (UINT32 x = 0; x < Width; x++)
		{
			// If the pixel is not transparent then stop the search now
			if (pSourceData[x] != (TransColour & 0xFF))
			{
				FoundClearRow = FALSE;
				break;
			}
		}

		// If we found a full clear row then note this as a new bottom hand position
		// and move onto the next
		if (FoundClearRow)
			BottomMostData = y;
		else
			break;

		// move onto the next scanline in each bitmap
		pSourceData	+= ScanLineSize;
	}

	TRACEUSER("Neville", wxT("width = %d, left offset = %d, right offset = %d\n"), Width, LeftMostData,RightMostData);
	TRACEUSER("Neville", wxT("height = %d, top offset = %d, bottom offset = %d\n"), Height, TopMostData,BottomMostData);

	// We have detected that we can sub-region this bitmap
	if (LeftMostData >= 0 || RightMostData < Width || TopMostData >= 0 || BottomMostData < Height)
	{
		UINT32 LeftOffset = 0;
		if (LeftMostData > 0)
			LeftOffset = LeftMostData;
		UINT32 RightOffset = Width - 1;
		if (RightMostData < Width - 1)
			RightOffset = RightMostData;
		// sanity check to see if the offsets are crossed over
		if (RightOffset < LeftOffset)
			RightOffset = LeftOffset;

		UINT32 TopOffset = 0;
		if (TopMostData > 0)
			TopOffset = TopMostData;
		UINT32 BottomOffset = Height - 1;
		if (BottomMostData < Height - 1)
			BottomOffset = BottomMostData;
		// sanity check to see if the offsets are crossed over
		if (BottomOffset < TopOffset)
			BottomOffset = TopOffset;

		UINT32 NewWidth = RightOffset - LeftOffset + 1;
		UINT32 NewHeight = BottomOffset - TopOffset + 1;

		TRACEUSER("Neville", wxT("Creating sub-region bitmap new height = %d, new width = %d\n"), NewWidth,NewHeight);

		// Create the new sized bitmap && check it all worked
		LPBYTE pBits = NULL;
		LPBITMAPINFO pInfo = AllocDIB(NewWidth, NewHeight, Depth, &pBits);
		if (pInfo == NULL || pBits == NULL)
			return FALSE;

		// Ensure remaining info header entries are correct
		LPBITMAPINFOHEADER pSourceInfoHeader = &(pSourceInfo->bmiHeader);
		LPBITMAPINFOHEADER pDestInfoHeader	= &(pInfo->bmiHeader); 
		pDestInfoHeader->biPlanes			= pSourceInfoHeader->biPlanes;
		pDestInfoHeader->biCompression		= pSourceInfoHeader->biCompression;
		pDestInfoHeader->biXPelsPerMeter	= pSourceInfoHeader->biXPelsPerMeter;
		pDestInfoHeader->biYPelsPerMeter	= pSourceInfoHeader->biYPelsPerMeter;
		pDestInfoHeader->biClrImportant		= pSourceInfoHeader->biClrImportant;
		pDestInfoHeader->biClrUsed			= pSourceInfoHeader->biClrUsed;

		// Ensure that the palette information is copied across
		LPRGBQUAD pSourcePalette = &(pSourceInfo->bmiColors[0]);
		LPRGBQUAD pDestPalette = &(pInfo->bmiColors[0]);
		UINT32 NumberOfColours = pSourceInfoHeader->biClrUsed; 
		// If we have zero colours on a bitmap which is 8bpp or less then this is bad.
		// This should be translated as the maximum number of colours allowed
		if (pSourceInfoHeader->biBitCount <= 8 && NumberOfColours == 0)
			NumberOfColours = 1 << pSourceInfoHeader->biBitCount;
		CopyPalette(pSourcePalette, pDestPalette, NumberOfColours);

		// Now copy the bitmap data across to the new bitmap
		UINT32 DestScanLineSize = DIBUtil::ScanlineSize(NewWidth, Depth);
		// Move the pointers to the top of the bitmaps in memory
		LPBYTE pSourceData = pSourceBits + (Height - 1) * ScanLineSize;
		LPBYTE pDestData = pBits + (NewHeight - 1) * DestScanLineSize;
		for (UINT32 y = TopOffset; y <= BottomOffset; y++)
		{
			LPBYTE pThisSourceData	= pSourceData - (y * ScanLineSize) + LeftOffset;
			LPBYTE pThisDestData	= pDestData - ((y - TopOffset) * DestScanLineSize);
			memcpy(pThisDestData, pThisSourceData, NewWidth);
		}

		// remove the old bitmap
		if (*ppDestInfo && *ppDestBits)
			FreeDIB(*ppDestInfo, *ppDestBits);
		
		// return the new bitmap to the caller
		*ppDestInfo = pInfo;
		*ppDestBits = pBits;

		// return the left and top most positions to the caller
		*pLeftOffset = LeftOffset;
		*pTopOffset = TopOffset;
		TRACEUSER( "Neville",wxT("Creating sub-region bitmap left offset = %d, top offset = %d\n"),LeftOffset,TopOffset);
	}
	
	return TRUE;
}


/********************************************************************************************

>	static LPLOGPALETTE DIBUtil::AllocateLogPalette(const UINT32 PaletteSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/07/97
	Inputs:		The number of entries in the palette
	Returns:	A pointer to the allocated LOGPALETTE or null.
	Purpose:	To allocate a LOGPALETTE ready for use.
				The caller is responsible for cleaning up the memmory allocation

********************************************************************************************/

LPLOGPALETTE DIBUtil::AllocateLogPalette(const UINT32 PaletteSize)
{
	ERROR2IF(PaletteSize == 0 || PaletteSize > 256,NULL,"AllocateLogPalette Bad PaletteSize");

	LPLOGPALETTE pPalette = NULL;
	const size_t TotalPal = sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * PaletteSize );
	pPalette = (LPLOGPALETTE)CCMalloc( TotalPal );
	if (pPalette == NULL)
		return NULL;

	pPalette->palNumEntries		= PaletteSize;
	pPalette->palVersion 		= 0x300;
	return pPalette;
}

/********************************************************************************************

>	static LPLOGPALETTE DIBUtil::CopyBitmapPaletteIntoLogPalette(KernelBitmap * pBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/97
	Inputs:		The bitmap to copy the palette from
	Returns:	A pointer to the allocated LOGPALETTE or null.
	Purpose:	To allocate a LOGPALETTE and then copy the palette from inside the bitmap
				across to the LOGPALETTE.
				The caller is responsible for cleaning up the memmory allocation

********************************************************************************************/

LPLOGPALETTE DIBUtil::CopyBitmapPaletteIntoLogPalette(KernelBitmap * pBitmap)
{
	ERROR2IF(pBitmap == NULL,NULL,"CopyBitmapPaletteIntoLogPalette Bad bitmap");

	LPRGBQUAD		pPal	= pBitmap->GetPaletteForBitmap();
	UINT32			Bpp		= pBitmap->GetBPP();
	UINT32			PaletteSize = UINT32( pow( 2.0, double(Bpp) ) );
	LPLOGPALETTE	pLogPal = NULL;

	// Bitmaps with more than 256 colours in usually do not require a palette
	if (Bpp > 8 || pPal == NULL)
	{
		ERROR2(NULL,"CopyBitmapPaletteIntoLogPalette bitmap has no palette");
		//return NULL;
	}

	pLogPal = DIBUtil::AllocateLogPalette(PaletteSize);
	if (pLogPal == NULL)
		return NULL;
	
	UINT32			ColoursInPal	= pBitmap->GetNumPaletteEntries();
	UINT32			ColoursInLogPal	= pLogPal->palNumEntries;
	PALETTEENTRY*	pPaletteEntry	= pLogPal->palPalEntry;
	for (UINT32 i = 0; i < ColoursInLogPal; i++)
	{
		if (i > ColoursInPal)
		{
			// Force any unused colours to be do not use black
			pPaletteEntry[i].peBlue 	= 0;
			pPaletteEntry[i].peGreen	= 0;
			pPaletteEntry[i].peRed	 	= 0;

			pPaletteEntry[i].peFlags	= 0xFF;
		}
		else
		{
			pPaletteEntry[i].peBlue		= pPal[i].rgbBlue;
			pPaletteEntry[i].peGreen	= pPal[i].rgbGreen;
			pPaletteEntry[i].peRed		= pPal[i].rgbRed;

			pPaletteEntry[i].peFlags	= 0x00;
		}
	}

	// Now fill in the actually number of palette entries
	// We must allocate the palette to be full sized as otherwise GDraw/CDraw will
	// more than likely fall over.
	pLogPal->palNumEntries = ColoursInPal;

	return pLogPal;
}

/********************************************************************************************

>	static BOOL DIBUtil::IsGreyscaleBitmap(KernelBitmap * pKB);
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/95
	Inputs:		pKB the bitmap to operate on
	Outputs:	None
	Returns:	TRUE if a 8bit GS bitmap else FALSE
	Purpose:	Determines whether or not a bitmap is 32bpp
	Errors:		None
	Scope:		Public
	SeeAlso:	BfxALU::MakeGreyscale, BFXALU::MakeGreyscale32to8
	Note:		Transferred from bfxalu 10/11/97 Neville

This routine returns false if the bitmap is not a correctly formed 8 bpp bitmap. It does
not return or set errors

********************************************************************************************/

BOOL DIBUtil::IsGreyscaleBitmap(KernelBitmap * pKB)
{
	if ( (!pKB) || (!pKB->ActualBitmap) )
		return FALSE;

	return IsGreyscaleBitmap(pKB->ActualBitmap);
}

/********************************************************************************************

>	static BOOL DIBUtil::IsGreyscaleBitmap(OILBitmap * pKB);
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/95
	Inputs:		pKB the bitmap to operate on
	Outputs:	None
	Returns:	TRUE if a 8bit GS bitmap else FALSE
	Purpose:	Determines whether or not a bitmap is 32bpp
	Errors:		None
	Scope:		Public
	SeeAlso:	BfxALU::MakeGreyscale, BFXALU::MakeGreyscale32to8
	Note:		Transferred from bfxalu 10/11/97 Neville

This routine returns false if the bitmap is not a correctly formed 8 bpp bitmap. It does
not return or set errors

********************************************************************************************/

BOOL DIBUtil::IsGreyscaleBitmap(OILBitmap * pOilBmp)
{
	if ( (!pOilBmp) || (!pOilBmp->IsKindOf(CC_RUNTIME_CLASS(CWxBitmap))) )
		return FALSE;

	BITMAPINFOHEADER * pBMI=&(((CWxBitmap *)pOilBmp)->BMInfo->bmiHeader);
	if ((pBMI->biBitCount!=8) || (pBMI->biClrUsed!=0x100)) return FALSE;
	DWORD * pPal = (DWORD *)(void *)(pBMI +1 /*ptr arith*/);
	for (DWORD x=0; x<0x100; x++) if ((pPal[x]&0x00ffffff) != (x|(x<<8)|(x<<16))) return FALSE;
	return TRUE;
}


/********************************************************************************************

>	static void DIBUtil::InvertAlpha ( LPBITMAPINFO	lpBitmapInfo,
										  LPBYTE		lpBits )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/00 (moved here by Phil 06/02/2004)
	Purpose:	Camelot uses a different transparency scheme to the rest of the world, in
				that 255 is clear, and 0 is opaque. Until the rest of the world catches up,
				it's necessary to invert the alpha channel to make exported files compatible
				with other programs.

********************************************************************************************/

void DIBUtil::InvertAlpha(LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits)
{
	// Only 32 bpp bitmaps have an alpha channel.
	if ( lpBitmapInfo->bmiHeader.biBitCount == 32 )
	{
		// Cast the pointer into a DWORD.
		DWORD *lpRGBA = reinterpret_cast<DWORD*> ( lpBits );

		// If we`re exporting a 32Bit BMP then we need to make sure that we convert the
		// Alpha channel to Transparency! i.e. invert it!
		for ( UINT32 i = 0; i < lpBitmapInfo->bmiHeader.biSizeImage; i+=4 )
		{
			// XORing the bitmap's RGBA values with Oxff000000 will invert the alpha channel
			// bits.
			*lpRGBA ^= 0xff000000;
			lpRGBA ++;
		}
	}
}

/********************************************************************************************

>	static void DIBUtil::MakeAlphaIntoGreyscale ( LPBITMAPINFO	lpBitmapInfo,
										  LPBYTE		lpBits )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/00 (moved here by Phil 06/02/2004)
	Purpose:	Camelot uses a different transparency scheme to the rest of the world, in
				that 255 is clear, and 0 is opaque. Until the rest of the world catches up,
				it's necessary to invert the alpha channel to make exported files compatible
				with other programs.

********************************************************************************************/

void DIBUtil::MakeAlphaIntoGreyscale(LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits)
{
	// Only 32 bpp bitmaps have an alpha channel.
	if ( lpBitmapInfo->bmiHeader.biBitCount == 32 )
	{
		// Cast the pointer into a DWORD.
		DWORD *lpRGBA = reinterpret_cast<DWORD*> ( lpBits );

		// If we`re exporting a 32Bit BMP then we need to make sure that we convert the
		// Alpha channel to Transparency! i.e. invert it!
		for ( UINT32 i = 0; i < lpBitmapInfo->bmiHeader.biSizeImage; i+=4 )
		{
			// Multiply the alpha value by 0x010101 puts it in each of R, G and B
			*lpRGBA = 0x010101 * (*lpRGBA>>24);
			lpRGBA ++;
		}
	}
}


#ifdef _DEBUG
void DIBUtil::FillColour24(LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits)
{
	srand((unsigned)time(NULL));
	DWORD colour = (rand() % 0xff)<<16 | (rand() % 0xff)<<8 | (rand() % 0xff)<<0;

	// Only 32 bpp bitmaps have an alpha channel.
	if ( lpBitmapInfo->bmiHeader.biBitCount == 32 )
	{
		// Cast the pointer into a DWORD.
		DWORD *lpRGBA = reinterpret_cast<DWORD*> ( lpBits );

		for ( UINT32 i = 0; i < lpBitmapInfo->bmiHeader.biSizeImage; i+=4 )
		{
			if ((*lpRGBA & 0xffffff) == 0xffffff)
				*lpRGBA = colour;
			lpRGBA ++;
		}
	}
}
#endif
