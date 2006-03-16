// $Id: cdrbitm.cpp 662 2006-03-14 21:31:49Z alex $
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
// *********Bitmaps for the CDR file filter

/*
*/

#include "camtypes.h"
#include "cdrbitm.h"
#include "cdrform.h"
#include "dibutil.h"
#include "wbitmap.h"
#include "cdrfiltr.h"

DECLARE_SOURCE("$Revision: 662 $");

#define new CAM_DEBUG_NEW

// Note: As this is a winoil file, no use of the CDRDATA_* functions is made.
// be aware of converting byte orders!

#define PROGRESS_BLOCKSIZE 2048

/********************************************************************************************

>	static KernelBitmap *CDRBitmap::ConvertPattern(RIFFFile *RIFF, DWORD *Reference, CDRFilter *C)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/04/95
	Inputs:		RIFFFile with the chunk containing the pattern current, a pointer to the
				reference number of this bitmap to fill in
	Returns:	a kernel bitmap or zero
	Purpose:	Convert a CDR pattern
	SeeAlso:	CDRFilter

********************************************************************************************/

KernelBitmap *CDRBitmap::ConvertPattern(RIFFFile *RIFF, DWORD *Reference, CDRFilter *C)
{
TRACEUSER( "Ben", _T("Converting pattern\n"));
	// check various things
	ERROR3IF(RIFF->GetObjChunkType() != cdrT_bmpf, "ConvertPattern called for non-pattern RIFF chunk");
	ERROR3IF(RIFF->GetObjType() != RIFFOBJECTTYPE_CHUNK, "ConvertPattern called for a non-chunk RIFF object");

	if(RIFF->GetObjSize() < sizeof(cdrfPatternBitmapHeader))
	{
		// Empty 'if' statement
		// Anyone's guess if this was intended
		// Changed to this form to remove a compiler warning
		// Markn 19/3/99
	}

	// get some data from the RIFF file
	cdrfPatternBitmapHeader Hdr;

	// get the header from offset 0
	if(!RIFF->GetChunkData((ADDR)&Hdr, sizeof(Hdr), 0))
		return 0;

	// check the header...
	if(((Hdr.SizeX * Hdr.SizeY) / 8) > Hdr.DataSize || Hdr.DataSize > (DWORD)RIFF->GetObjSize())
	{
TRACEUSER( "Ben", _T("Header in CDR pattern doesn't check out\n"));
		return 0;
	}

	// store the reference
TRACEUSER( "Ben", _T("Pattern reference = %X\n"), Hdr.Reference);
	*Reference = Hdr.Reference;

	// OK, create a bitmap...
	// it's always a 1 bpp bitmap
	LPBYTE Image;
	LPBITMAPINFO ImageHeader;
	ImageHeader = AllocDIB(Hdr.SizeX, Hdr.SizeX, 1, &Image);

	if(ImageHeader == 0)
		return 0;

	// check and then fill in the image data
	if(ImageHeader->bmiHeader.biSizeImage < (Hdr.DataSize - (sizeof(DWORD) * 3)))
	{
		FreeDIB(ImageHeader, Image);
		return 0;
	}

	// set some palette data
	ImageHeader->bmiColors[0].rgbBlue = 0;
	ImageHeader->bmiColors[0].rgbGreen = 0;
	ImageHeader->bmiColors[0].rgbRed = 0;
	ImageHeader->bmiColors[0].rgbReserved = 0;
	ImageHeader->bmiColors[1].rgbBlue = 0xff;
	ImageHeader->bmiColors[1].rgbGreen = 0xff;
	ImageHeader->bmiColors[1].rgbRed = 0xff;
	ImageHeader->bmiColors[1].rgbReserved = 0;

	// corel bitmaps align the scanlines to the nearest byte, not the nearest word
	// how long is a corel scanline
	INT32 SourceScanlineLength = (Hdr.SizeX + 7)	/ 8;
	
	// how long is the destination scanline?
	INT32 DestScanlineLength = ((Hdr.SizeX + 31) / 32) * 4;

	// sanity check
	ERROR3IF(DestScanlineLength < SourceScanlineLength, "Problem with scanline length calculations");
	
	// set up offsets of current line
	INT32 SourceOffset = Hdr.DataOffset + (sizeof(DWORD) * 3);
	INT32 DestOffset = ImageHeader->bmiHeader.biSizeImage - DestScanlineLength;
	
	// grab the image data, backwards
	INT32 Line;
	for(Line = 0; Line < (INT32)Hdr.SizeY; Line++)
	{
		if(!RIFF->GetChunkData(((ADDR)Image) + DestOffset,
				SourceScanlineLength, SourceOffset))
		{
			FreeDIB(ImageHeader, Image);
			return 0;
		}

		SourceOffset += SourceScanlineLength;
		DestOffset -= DestScanlineLength;

		if((Line & 0xf) == 0)
			C->UpdateProgress(TRUE);
	}

	// OK, it's all filled in. Create a KernelBitmap from it.
	// first of all, get ourself a nice OIL bitmap
	WinBitmap *WinB = new WinBitmap(ImageHeader, Image);

	if(WinB == 0)
	{
		FreeDIB(ImageHeader, Image);
		return 0;
	}

	// and now get a KernelBitmap
	KernelBitmap *KerB = new KernelBitmap(WinB);

	if(KerB == 0)
	{
		delete WinB;
		return 0;
	}

	// and that's it for now, folks
	return KerB;
}


/********************************************************************************************

>	static KernelBitmap *CDRBitmap::ConvertBitmap5(RIFFFile *RIFF, DWORD *Reference, CDRFilter *C)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/95
	Inputs:		RIFFFile with the chunk containing the bitmap current, a pointer to the
				reference number of this bitmap to fill in
	Returns:	a kernel bitmap or zero
	Purpose:	Convert a CDR 5 bitmap
	SeeAlso:	CDRFilter

********************************************************************************************/

KernelBitmap *CDRBitmap::ConvertBitmap5(RIFFFile *RIFF, DWORD *Reference, CDRFilter *C)
{
	// check various things
	ERROR3IF(RIFF->GetObjChunkType() != cdrT_bmp, "ConvertBitmap called for non-bitmap RIFF chunk");
	ERROR3IF(RIFF->GetObjType() != RIFFOBJECTTYPE_CHUNK, "ConvertBitmap called for a non-chunk RIFF object");

	if(RIFF->GetObjSize() < sizeof(cdrfBitmapHeader))
	{
		// Empty 'if' statement
		// Anyone's guess if this was intended
		// Changed to this form to remove a compiler warning
		// Markn 19/3/99
	}

	// get some data from the RIFF file
	cdrfBitmapHeader Hdr;

	// get the header from offset 0
	if(!RIFF->GetChunkData((ADDR)&Hdr, sizeof(Hdr), 0))
		return 0;

	// check the header...
	UINT32 BitmapSize = DIBUtil::ScanlineSize( Hdr.SizeX, Hdr.Depth ) * Hdr.SizeY;

//	if((((Hdr.SizeX * Hdr.SizeY) / Hdr.Depth) + Hdr.ImageOffset - cdrfBitmapHeaderImageOffsetCorrect)
//					> (DWORD)RIFF->GetObjSize()
//					|| Hdr.Depth < 1 || Hdr.Depth > 32)
	if( (BitmapSize + Hdr.ImageOffset - cdrfBitmapHeaderImageOffsetCorrect)
					> (DWORD)RIFF->GetObjSize()
					|| Hdr.Depth < 1 || Hdr.Depth > 32)
	{
TRACEUSER( "Ben", _T("Header in CDR bitmap doesn't check out\n"));
		return 0;
	}

	// store the reference
TRACEUSER( "Ben", _T("Bitmap reference = %X\n"), Hdr.Reference);
	*Reference = Hdr.Reference;

	// find a plausible number of palette entries...
	INT32 NPaletteEntries = Hdr.NPaletteEntries;

	if(NPaletteEntries > (1 << Hdr.Depth))
		NPaletteEntries = (1 << Hdr.Depth);

	if(NPaletteEntries < 0)
		NPaletteEntries = 0;	

	// is it a greyscale type bitmap?
	BOOL Greyscale = FALSE;
	if(Hdr.Depth <= 8 && Hdr.ImageType == cdrfBITMAPTYPE_GREYSCALE)
		Greyscale = TRUE;

	// OK, create a bitmap...
	LPBYTE Image;
	LPBITMAPINFO ImageHeader;
	ImageHeader = AllocDIB(Hdr.SizeX, Hdr.SizeY, Hdr.Depth, &Image);

	if(ImageHeader == 0)
		return 0;

	// right then, sort out that there palette if we really want to
	if(Hdr.Depth <= 8)
	{
		if(Greyscale)
		{	// it's a greyscale image - create a nice palette
			UINT32 Entries = 1 << Hdr.Depth;
			UINT32 Inc = 256 / Entries;
			UINT32 Value = 0;
			UINT32 l;

			for(l = 0; l < Entries; l++)
			{
				ImageHeader->bmiColors[l].rgbRed = Value;
				ImageHeader->bmiColors[l].rgbGreen = Value;
		 		ImageHeader->bmiColors[l].rgbBlue = Value;
				ImageHeader->bmiColors[l].rgbReserved = 0;

				Value += Inc;
			}
		}
		else
		{	// it's a colour image - load it's palette from the file
			// load the palette data into the palette in the created bitmap
			if(!RIFF->GetChunkData((ADDR)ImageHeader->bmiColors,
					sizeof(cdrfBitmapPaletteEntry) * NPaletteEntries, sizeof(cdrfBitmapHeader)))
			{
				FreeDIB(ImageHeader, Image);
				return 0;
			}

			// run through the loaded entries expanding them to DIB entries
			INT32 l;
			cdrfBitmapPaletteEntry *Palette = (cdrfBitmapPaletteEntry *)ImageHeader->bmiColors;
			cdrfBitmapPaletteEntry TempCol;
			for(l = NPaletteEntries - 1; l >= 0; l--)
			{
				TempCol = Palette[l];
				ImageHeader->bmiColors[l].rgbRed = TempCol.Red;
				ImageHeader->bmiColors[l].rgbGreen = TempCol.Green;
		 		ImageHeader->bmiColors[l].rgbBlue = TempCol.Blue;
				ImageHeader->bmiColors[l].rgbReserved = 0;
			}

			if (Hdr.Depth == 1)
			{
				// Ensure the palette entries are not the same!
				if (ImageHeader->bmiColors[0].rgbRed == ImageHeader->bmiColors[1].rgbRed &&
					ImageHeader->bmiColors[0].rgbGreen == ImageHeader->bmiColors[1].rgbGreen &&
					ImageHeader->bmiColors[0].rgbBlue == ImageHeader->bmiColors[1].rgbBlue)
				{
					ImageHeader->bmiColors[0].rgbRed = 0x00;
					ImageHeader->bmiColors[0].rgbGreen = 0x00;
					ImageHeader->bmiColors[0].rgbBlue = 0x00;

					ImageHeader->bmiColors[1].rgbRed = 0xff;
					ImageHeader->bmiColors[1].rgbGreen = 0xff;
					ImageHeader->bmiColors[1].rgbBlue = 0xff;
				}
			}
		}
	}

	// work out the scanline lengths (aligns to 4 bytes)
	INT32 RawScanlineLength = Hdr.SizeX * Hdr.Depth;	// in bits
	INT32 ScanlineLength = DIBUtil::ScanlineSize( Hdr.SizeX, Hdr.Depth ); // in bytes 
	//INT32 ScanlineLength = ((RawScanlineLength + 31) / 32) * 4;

	// set up offsets of current line
	INT32 SourceOffset;
	
	if(Greyscale)
	{
		SourceOffset = cdrfBitmapGreyscaleImageStart;
	}
	else
	{
		SourceOffset = sizeof(cdrfBitmapHeader) + Hdr.ImageOffset - cdrfBitmapHeaderImageOffsetCorrect;
	}

	// grab the image data, it's in the same sort of format as a DIB file. Helpfully
	INT32 SizeLeftToGet = ScanlineLength * Hdr.SizeY;
	INT32 Size, Got = 0;

	while(SizeLeftToGet > 0)
	{
		if(SizeLeftToGet > PROGRESS_BLOCKSIZE)
			Size = PROGRESS_BLOCKSIZE;
		else
			Size = SizeLeftToGet;
		
		if(!RIFF->GetChunkData((ADDR)Image + Got, Size, SourceOffset + Got))
		{
			FreeDIB(ImageHeader, Image);
			return 0;
		}
	
		SizeLeftToGet -= Size;
		Got += Size;

		C->UpdateProgress(TRUE);
	}

	// OK, it's all filled in. Create a KernelBitmap from it.
	// first of all, get ourself a nice OIL bitmap
	WinBitmap *WinB = new WinBitmap(ImageHeader, Image);

	if(WinB == 0)
	{
		FreeDIB(ImageHeader, Image);
		return 0;
	}

	// get it to cache some interesting stuff
	WinB->CacheGeometry();

	// and now get a KernelBitmap
	KernelBitmap *KerB = new KernelBitmap(WinB);

	if(KerB == 0)
	{
		delete WinB;
		return 0;
	}

	// and that's it for now, folks
	return KerB;
}


/********************************************************************************************

>	static KernelBitmap *CDRBitmap::ConvertBitmap4(RIFFFile *RIFF, DWORD *Reference, CDRFilter *C)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/95
	Inputs:		RIFFFile with the chunk containing the bitmap current, a pointer to the
				reference number of this bitmap to fill in
	Returns:	a kernel bitmap or zero
	Purpose:	Convert a CDR 4 bitmap
	SeeAlso:	CDRFilter

********************************************************************************************/

KernelBitmap *CDRBitmap::ConvertBitmap4(RIFFFile *RIFF, DWORD *Reference, CDRFilter *C)
{
	// check various things
	ERROR3IF(RIFF->GetObjChunkType() != cdrT_bmp, "ConvertBitmap called for non-bitmap RIFF chunk");
	ERROR3IF(RIFF->GetObjType() != RIFFOBJECTTYPE_CHUNK, "ConvertBitmap called for a non-chunk RIFF object");

	if(RIFF->GetObjSize() < sizeof(cdrfBitmapHeader4))
	{
		// Empty 'if' statement
		// Anyone's guess if this was intended
		// Changed to this form to remove a compiler warning
		// Markn 19/3/99
	}

	// get some data from the RIFF file
	cdrfBitmapHeader4 Hdr;

	// get the header from offset 0
	if(!RIFF->GetChunkData((ADDR)&Hdr, sizeof(Hdr), 0))
		return 0;

	// check the header...
	if((((Hdr.SizeX * Hdr.SizeY) / Hdr.Depth) + Hdr.ImageOffset - cdrfBitmapHeaderImageOffsetCorrect)
					> (DWORD)RIFF->GetObjSize()
					|| Hdr.Depth < 1 || Hdr.Depth > 32)
	{
TRACEUSER( "Ben", _T("Header in CDR bitmap doesn't check out\n"));
		return 0;
	}

	// store the reference
TRACEUSER( "Ben", _T("Bitmap reference = %X\n"), Hdr.Reference);
	*Reference = Hdr.Reference;

	// find a plausible number of palette entries...
	INT32 NPaletteEntries = Hdr.NPaletteEntries;

	if(NPaletteEntries > (1 << Hdr.Depth))
		NPaletteEntries = (1 << Hdr.Depth);

	if(NPaletteEntries < 0)
		NPaletteEntries = 0;	

	// is it a greyscale type bitmap?
	BOOL Greyscale = FALSE;
//	if(Hdr.Depth <= 8 && Hdr.ImageType == cdrfBITMAPTYPE_GREYSCALE)
//		Greyscale = TRUE;

	// OK, create a bitmap...
	LPBYTE Image;
	LPBITMAPINFO ImageHeader;
	ImageHeader = AllocDIB(Hdr.SizeX, Hdr.SizeY, Hdr.Depth, &Image);

	if(ImageHeader == 0)
		return 0;

	// right then, sort out that there palette if we really want to
	if(Hdr.Depth <= 8)
	{
		if(Greyscale)
		{	// it's a greyscale image - create a nice palette
			UINT32 Entries = 1 << Hdr.Depth;
			UINT32 Inc = 256 / Entries;
			UINT32 Value = 0;
			UINT32 l;

			for(l = 0; l < Entries; l++)
			{
				ImageHeader->bmiColors[l].rgbRed = Value;
				ImageHeader->bmiColors[l].rgbGreen = Value;
		 		ImageHeader->bmiColors[l].rgbBlue = Value;
				ImageHeader->bmiColors[l].rgbReserved = 0;

				Value += Inc;
			}
		}
		else
		{	// it's a colour image - load it's palette from the file
			// load the palette data into the palette in the created bitmap
			if(!RIFF->GetChunkData((ADDR)ImageHeader->bmiColors,
					sizeof(cdrfBitmapPaletteEntry4) * NPaletteEntries, sizeof(cdrfBitmapHeader4)))
			{
				FreeDIB(ImageHeader, Image);
				return 0;
			}
		}
	}

	// work out the scanline lengths (aligns to 4 bytes)
	INT32 RawScanlineLength = Hdr.SizeX * Hdr.Depth;	// in bits
	INT32 ScanlineLength = ((RawScanlineLength + 31) / 32) * 4;

	// set up offsets of current line
	INT32 SourceOffset;
	
	if(Greyscale)
	{
		SourceOffset = cdrfBitmapGreyscaleImageStart;
	}
	else
	{
		SourceOffset = sizeof(cdrfBitmapHeader) + Hdr.ImageOffset - cdrfBitmapHeaderImageOffsetCorrect;
	}

	// grab the image data, it's in the same sort of format as a DIB file. Helpfully
//	if(!RIFF->GetChunkData((ADDR)Image, ScanlineLength * Hdr.SizeY, SourceOffset))
//	{
//		FreeDIB(ImageHeader, Image);
//		return 0;
//	}
	INT32 SizeLeftToGet = ScanlineLength * Hdr.SizeY;
	INT32 Size, Got = 0;

	while(SizeLeftToGet > 0)
	{
		if(SizeLeftToGet > PROGRESS_BLOCKSIZE)
			Size = PROGRESS_BLOCKSIZE;
		else
			Size = SizeLeftToGet;
		
		if(!RIFF->GetChunkData((ADDR)Image + Got, Size, SourceOffset + Got))
		{
			FreeDIB(ImageHeader, Image);
			return 0;
		}
	
		SizeLeftToGet -= Size;
		Got += Size;

		C->UpdateProgress(TRUE);
	}

	// OK, it's all filled in. Create a KernelBitmap from it.
	// first of all, get ourself a nice OIL bitmap
	WinBitmap *WinB = new WinBitmap(ImageHeader, Image);

	if(WinB == 0)
	{
		FreeDIB(ImageHeader, Image);
		return 0;
	}

	// get it to cache some interesting stuff
	WinB->CacheGeometry();

	// and now get a KernelBitmap
	KernelBitmap *KerB = new KernelBitmap(WinB);

	if(KerB == 0)
	{
		delete WinB;
		return 0;
	}

	// and that's it for now, folks
	return KerB;
}



/********************************************************************************************

>	static KernelBitmap *GenerateBitmap(Node *Objects, DocRect *BBox, Document *pDocument)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/05/95
	Inputs:		pointer to some objects, a bounding box to render, and a document to use
				the default attributes of
	Returns:	a kernel bitmap or zero
	Purpose:	Generate a bitmap of some objects.
	SeeAlso:	CDRFilter

********************************************************************************************/

KernelBitmap *GenerateBitmap(Node *Objects, DocRect *BBox, Document *pDocument)
{
	return 0;
}







