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

/*
*/

#include "camtypes.h"
#include "outptdib.h"
#include "ensure.h"
#include "dibutil.h"
#include "fixmem.h"
#include "errors.h"
//#include "andy.h"
#include "dibconv.h"
#include "progress.h"		// For hourglass stuff
#include "bmapprev.h"

#define	new	CAM_DEBUG_NEW

// Declare the statics that are required as we are using a call back function
LPBITMAPINFO		OutputDIB::lpBitmap;				// copy of header + palette, can be NULL
LPBITMAPINFOHEADER	OutputDIB::lpBitmapInfo;			// copy of header, can be NULL
BITMAPINFOHEADER	OutputDIB::BitmapInfo;				// copy of file header (includes output depth,
														// width & height)
// for debug purposes
INT32 				OutputDIB::HeightWritten 	= 0L;	// count of the height of the export
INT32 				OutputDIB::HeightWanted		= 0L;	// the actual height of the export required

LPBYTE 				OutputDIB::ExportBuffer 	= NULL;	// pointer to output buffer, can be NULL
DIBConvert 			*OutputDIB::DoExportConvert;		// the convert DIB
FNPTR_SCANLINE 		OutputDIB::ExportConvertFn 	= NULL;	// the convert function
size_t 				OutputDIB::ExportSourceWidth = 0;	// width of the source DIB
size_t 				OutputDIB::ExportDestWidth 	= 0;	// width of the destination DIB
LPBYTE 				OutputDIB::ExportData 		= NULL;	// pointer to the export data 
INT32 				OutputDIB::CurrentYPos 		= 0; 	// current line number
size_t 				OutputDIB::ExportChunkHeight = 0;	// Chunk height in use			

BOOL 				OutputDIB::OutputForward 	= FALSE;// start from bottom or top of DIB
UINT32 				OutputDIB::CurrentStripSize	= 0;   	// current size of the strip being rendered

BOOL 				OutputDIB::IsFirstStrip		= TRUE;

/********************************************************************************************
>	OutputDIB::OutputDIB()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Purpose:	Default constructor for the class. Just sets up the pointers to our buffers
				that may be used to be null so that the TidyUp function can do its job.
	SeeAlso:	OutputDIB::TidyUp

********************************************************************************************/
OutputDIB::OutputDIB()
{
	OutputFile = NULL;
	lpBitmap = NULL;
	ExportBuffer = NULL;
	DoExportConvert = NULL;
	OutputForward = FALSE;
	CurrentStripSize = 0;
	IsFirstStrip = TRUE;
	OutputPalette = NULL;
}


/********************************************************************************************

>	void OutputDIB::PokePaletteEntry(LPLOGPALETTE Palette, INT32 index, BYTE red, BYTE green, BYTE blue)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		Palette		palette to write to.
				index		address of variable holding palette entry to write to
				red			red value to enter into palette entry 
				green		green value to enter into palette entry 
				blue		blue value to enter into palette entry
	Outputs:	index incremented by one 
	Returns:	-
	Purpose:	Puts the required value	of rgb into the specified palette entry.
	SeeAlso:	OutputDIB::StartFile;

********************************************************************************************/
void OutputDIB::PokePaletteEntry(LPLOGPALETTE Palette, INT32 *index, BYTE red, BYTE green, BYTE blue)
{
	Palette->palPalEntry[*index].peRed = red;	
	Palette->palPalEntry[*index].peGreen = green;	
	Palette->palPalEntry[*index].peBlue = blue;	
	Palette->palPalEntry[*index].peFlags = 0;
	// increment the counter and return it
	(*index) ++;	
}


/******************************************************************************************

>	void OutputDIB::AlterExportPalette( LPLOGPALETTE pPalette )

	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>

	Created:	20/07/99

	Inputs:		pPalette - Palette information that we want to copy from

	Returns:	-

	Purpose:	Copies information from the palette object passed in to OutputPalette.
				This is used when the user has changed a colour using the bitmap
				preview dialog's palette control

******************************************************************************************/

void OutputDIB::AlterExportPalette( LPLOGPALETTE pPalette )
{
	if (pPalette == NULL)
		return;
/*
	// actually it appears it is safe to let this be filled in later if we havent got
	// an output palette yet

	// if we dont have an output palette we may as well create one now
	if (OutputPalette==NULL)
	{
		const size_t TotalPal = sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * 256 );
		OutputPalette = (LPLOGPALETTE)CCMalloc( TotalPal );
		OutputPalette->palNumEntries = 256;
	}
*/	if (OutputPalette==NULL)
		return;

	INT32 PreviousNoOfEntries = OutputPalette->palNumEntries;

	INT32 Colours = OutputPalette->palNumEntries;
	for( INT32 j=0;j<Colours;j++)
	{
		// Set all the colours to 'non-renderable' first
		OutputPalette->palPalEntry[j].peFlags = 255;
	}

	Colours = pPalette->palNumEntries;

	INT32 i = 0;
	for(; i < Colours; i++ )
	{
		//  Copy the colour over to the correct place in OutputPalette
		OutputPalette->palPalEntry[i].peRed		= pPalette->palPalEntry[i].peRed;
		OutputPalette->palPalEntry[i].peGreen	= pPalette->palPalEntry[i].peGreen;
		OutputPalette->palPalEntry[i].peBlue	= pPalette->palPalEntry[i].peBlue;

		//  Indicate that we want to use the colour in rendering the image
		OutputPalette->palPalEntry[i].peFlags = pPalette->palPalEntry[i].peFlags;
	}

	for(; i < PreviousNoOfEntries; i++ )
	{
		//  excess colours are black
		OutputPalette->palPalEntry[i].peRed		= 0;
		OutputPalette->palPalEntry[i].peGreen	= 0;
		OutputPalette->palPalEntry[i].peBlue	= 0;

		//  Indicate that we dont want to use the colour in rendering the image
		OutputPalette->palPalEntry[i].peFlags = 255;
	}
}
	
/******************************************************************************************

>	LPBITMAPINFO OutputDIB::GetDestBitmapInfo ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/00
	Inputs:		pPalette - Palette information that we want to copy from
	Returns:	NULL
	Purpose:	Stub function to complete inheritance hierarchy.

******************************************************************************************/

LPBITMAPINFO OutputDIB::GetDestBitmapInfo ( void )
{
	// This function should never be called, so return NULL.
	return NULL;
}

/******************************************************************************************

>	LPBYTE OutputDIB::GetDestBitmapBits ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/00
	Inputs:		pPalette - Palette information that we want to copy from
	Returns:	NULL
	Purpose:	Stub function to complete inheritance hierarchy.

******************************************************************************************/

LPBYTE OutputDIB::GetDestBitmapBits ( void )
{
	// This function should never be called, so return NULL.
	return NULL;
}

/********************************************************************************************

>	void OutputDIB::FixBlackAndWhitePalette(LPLOGPALETTE Palette)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		Palette		palette to write to.
	Outputs:	-
	Returns:	-
	Purpose:	Corrects the first two palette entries to be a correct Gavin black and white
				palette.
	SeeAlso:	OutputDIB::StartFile; OutputDIB::StartExport;

********************************************************************************************/
void OutputDIB::FixBlackAndWhitePalette(LPLOGPALETTE Palette)
{
	Palette->palNumEntries = 2;

	INT32 i = 0;
PORTNOTE("BmapPrevDlg", "Removed use of bitmap preview dialog");
#ifndef EXCLUDE_FROM_XARALX
	if( BmapPrevDlg::GetNumberOfTransparentColours() > 0 )
	{
		PokePaletteEntry(Palette, &i, 0xff, 0xff, 0xff);
		PokePaletteEntry(Palette, &i, 0x00, 0x00, 0x00);
	}
	else
#endif
	{
		PokePaletteEntry(Palette, &i, 0x00, 0x00, 0x00);
		PokePaletteEntry(Palette, &i, 0xff, 0xff, 0xff);
	}
}

/********************************************************************************************

>	void OutputDIB::Fix16ColourPalette(LPLOGPALETTE Palette)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		Palette		palette to write to.
	Outputs:	-
	Returns:	-
	Purpose:	Corrects the first 16 palette entries to be a correct Gavin 16 colour palette.
	SeeAlso:	OutputDIB::StartFile; OutputDIB::StartExport;

********************************************************************************************/
void OutputDIB::Fix16ColourPalette(LPLOGPALETTE Palette)
{
	Palette->palNumEntries = 16;

	INT32 i = 0;
	// Bit more tricky this one, Gavin uses a fixed 16 colour palette			
	PokePaletteEntry(Palette, &i, 0x00, 0x00, 0x00); // black
	PokePaletteEntry(Palette, &i, 0x80, 0x00, 0x00); // brown
	PokePaletteEntry(Palette, &i, 0x00, 0x80, 0x00); // Half green
	PokePaletteEntry(Palette, &i, 0x80, 0x80, 0x00); // Half browny (red/green)
	PokePaletteEntry(Palette, &i, 0x00, 0x00, 0x80); // Dark blue
	PokePaletteEntry(Palette, &i, 0x80, 0x00, 0x80); // Mauve
	PokePaletteEntry(Palette, &i, 0x00, 0x80, 0x80); // greeny/blue
	PokePaletteEntry(Palette, &i, 0x80, 0x80, 0x80); // mid grey

	PokePaletteEntry(Palette, &i, 0xc0, 0xc0, 0xc0); // light grey
	PokePaletteEntry(Palette, &i, 0xff, 0x00, 0x00); // red
	PokePaletteEntry(Palette, &i, 0x00, 0xff, 0x00); // green
	PokePaletteEntry(Palette, &i, 0xff, 0xff, 0x00); // yellow
	PokePaletteEntry(Palette, &i, 0x00, 0x00, 0xff); // blue
	PokePaletteEntry(Palette, &i, 0xff, 0x00, 0xff); // magenta
	PokePaletteEntry(Palette, &i, 0x00, 0xff, 0xff); // cyan
	PokePaletteEntry(Palette, &i, 0xff, 0xff, 0xff); // white
}	

/********************************************************************************************

>	BOOL OutputDIB::StartFile( CCLexFile *File, LPBITMAPINFOHEADER lpHeader, LPRGBQUAD Palette,
							UINT32 OutputDepth, DWORD CompressionType,
							UINT32 FinalHeight, INT32 SizeOfExport, BOOL SecondPass = FALSE )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Inputs:		File	opened file we can write to.
				lpHeader		contains width & DPI of source & dest. biHeight & depth ignored.
				Palette			pointer to bmiColor struct, or NULL if not used (can be temporary)
				OutputDepth		depth of bitmap required on disk
				CompressionType	BI_RGB only supported (or CC_BMPTYPE for 32-bit with trans)
				FinalHeight		output of entire bitmap on disk
				ExportSize		The set progress bar size 
				SecondPass		Flag for whether it is a second pass export render. (Used by
								GIF export only at present). Defaults to False.
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Get ready to write a bmp to disk, maybe in chunks showing a progress bar as we
				write it.
				Currently supports:-
					source depth = 32, destintaiotn depth = 32, 24, 8, 4, 1
					source depth = 8, destintaiotn depth = 8
	Errors:		Calls SetError on FALSE returns.
	SeeAlso:	OutputDIB::WriteBlock; OutputDIB::TidyUp
	SeeAlso:	OutputGIF::StartFile;
	Scope:		Public

********************************************************************************************/

BOOL OutputDIB::StartFile( CCLexFile *File, LPBITMAPINFOHEADER lpHeader, LPLOGPALETTE Palette,
						   UINT32 OutputDepth, DWORD CompressionType,
						   UINT32 FinalHeight, INT32 ExportSize, BOOL SecondPass,
						   UINT32 DitherType )
{
	// remember input args
	OutputFile = File;
	StartPos = OutputFile->tell();						// remember where in the file this starts
	BitmapInfo = *lpHeader;								// take a copy of user's header
	OutputPalette = NULL;								// our form of the palette, if required

	HeightWritten = 0;									// our count of the bitmap lines written
	CurrentExportSize = ExportSize;						// size set up for the progress bar
	HeightWanted = FinalHeight;							// the actual height of the export required

	INT32 PalSize = 0;									// how many entries in palette

	// Remember which dithering type to use
	Dither = DitherType;

	IsFirstStrip = TRUE;

	// Set up the information header for the dib
	if ( !SetUpInfoHeader(lpHeader, OutputDepth, CompressionType, FinalHeight, &PalSize) )
		return FALSE;									// SetError already called 

	// BITMAPFILEHEADER goes first
	BITMAPFILEHEADER Header;

	Header.bfType = ('M'<<8) | 'B';
	Header.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
					sizeof(RGBQUAD)*PalSize + BitmapInfo.biSizeImage;
	Header.bfReserved1 = 0;
	Header.bfReserved2 = 0;
	Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
						sizeof(RGBQUAD)*PalSize;

	OutputFile->write( &Header, sizeof(Header) );


	// then a local BITMAPINFOHEADER

	OutputFile->write( &BitmapInfo, sizeof(BITMAPINFOHEADER) );

	// then the RGBQUAD palette
	if (PalSize)
	{
		ERROR2IF(Palette == NULL,FALSE,"StartFile has a NULL Palette when one is required");
		// make a copy of the palette for possible later use
		const size_t TotalPal = sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * PalSize );
		OutputPalette = (LPLOGPALETTE)CCMalloc( TotalPal );
		if (OutputPalette==NULL)
			return FALSE;

		// Take a copy of that palette
		memcpy( OutputPalette, Palette, TotalPal );

		// write it to disk in RGBQUAD format
		RGBQUAD TempRGB;
		TempRGB.rgbReserved = 0;
		for (INT32 i=0; i<PalSize; i++)
		{
			TempRGB.rgbRed = OutputPalette->palPalEntry[i].peRed;
			TempRGB.rgbGreen = OutputPalette->palPalEntry[i].peGreen;
			TempRGB.rgbBlue = OutputPalette->palPalEntry[i].peBlue;
			OutputFile->write( &TempRGB, sizeof(RGBQUAD) );
		}
	}

	return !OutputFile->bad();
}

/********************************************************************************************

>	BOOL OutputDIB::StartFile ( LPBITMAPINFOHEADER lpHeader,
								LPLOGPALETTE Palette,
								UINT32 OutputDepth,
								DWORD CompressionType,
								UINT32 FinalHeight,
								INT32 ExportSize,
								UINT32 DitherType )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/00
	Inputs:		All are meaningless.
	Returns:	FALSE.
	Purpose:	Stub function to be inherited over.

********************************************************************************************/

BOOL OutputDIB::StartFile ( LPBITMAPINFOHEADER lpHeader,
							LPLOGPALETTE Palette,
							UINT32 OutputDepth,
							DWORD CompressionType,
							UINT32 FinalHeight,
							INT32 ExportSize,
							UINT32 DitherType )
{
	return FALSE;
}

/********************************************************************************************

>	BOOL OutputDIB::SetUpInfoHeader(const LPBITMAPINFOHEADER lpHeader, const UINT32 OutputDepth,
									const DWORD CompressionType, const UINT32 FinalHeight,
									INT32 * pPalSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/94
	Inputs:		lpHeader	contains width & DPI of source & dest. biHeight & depth ignored.
				OutputDepth	depth of bitmap required on disk
				CompressionType	BI_RGB only supported (or CC_BMPTYPE for 32-bit with trans)
				FinalHeight	output of entire bitmap on disk
	Outputs:	pPalSize	pointer to an integer PalSize variable which is the palette size
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Set up the information header for the DIB.
	Errors:		Calls SetError on FALSE returns.
	SeeAlso:	OutputDIB::StartFile; OutputDIB::WriteBlock; OutputDIB::TidyUp
	Scope:		Public

********************************************************************************************/

BOOL OutputDIB::SetUpInfoHeader(const LPBITMAPINFOHEADER lpHeader, const UINT32 OutputDepth,
								const DWORD CompressionType, const UINT32 FinalHeight,
								INT32 * pPalSize)
{
	if (
		(CompressionType != BI_RGB) &&					// uncompressed is OK
		(CompressionType != CC_BMPTYPE)				  	// so is RGBT
	   )
	{
		// cannot do compression yet
		Error::SetError( _R(IDE_FORMATNOTSUPPORTED) );
		return FALSE;
	}

	// Fix up our copy of the header sohat it corresponds to the actual required one
	BitmapInfo.biCompression = CompressionType;		// fixup compression type
	BitmapInfo.biHeight = FinalHeight;				// fixup height to be true height
	BitmapInfo.biBitCount = OutputDepth;

	SourceBitmapDepth = lpHeader->biBitCount;

	const WORD DestDepth = BitmapInfo.biBitCount;
	UINT32 LineWidth = DIBUtil::ScanlineSize( BitmapInfo.biWidth, OutputDepth );

	// source bitmap not necessarily same as required file depth
	// make sure we can handle conversion
	BOOL FormatOK = FALSE;

	switch (SourceBitmapDepth)
	{
		case 32:
			{
				switch (DestDepth)
				{
					case 32:
						{
							// 32->32 might use our special type CC_BMPTYPE
							FormatOK = TRUE;
						}
						break;
					case 24:
						{
							// 32->24 bit is fine
							if (CompressionType==BI_RGB)
							{
								// might be BITFIELDs for rendering purposes
								BitmapInfo.biCompression = BI_RGB;
								BitmapInfo.biBitCount = 24;
								FormatOK = TRUE;
							}
						}
						break;
					case 8:
						{
							// 32->8 is OK
							FormatOK = TRUE;
							*pPalSize = 256;
						}
						break;
					case 4:
						{
							// 32->4 is OK
							FormatOK = TRUE;
							*pPalSize = 16;
						}
						break;
					case 1:
						{
							// 32->1 is OK
							FormatOK = TRUE;
							*pPalSize = 2;
						}
						break;
					default:
						// other source formats here
						break;
				}
				
			}
			break;
		case 8:
			if (DestDepth==8)
			{
				// 8->8 is OK
				FormatOK = TRUE;
				*pPalSize = 256;
			}
			break;
		default:
			// other source formats here
			break;
	}

	if (!FormatOK)
	{
		Error::SetError( _R(IDE_FORMATNOTSUPPORTED) );
		return FALSE;
	}

	BitmapInfo.biSizeImage = LineWidth * FinalHeight;

	return TRUE;
}

/********************************************************************************************

>	BOOL OutputDIB::WriteBlock( UINT32 YPos, UINT32 Height, LPBYTE BlockStart, UINT32 InputBPP = 32,
							INT32 ProgressOffset = 0)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Inputs:		YPos		pixel co-ord of the chunk of DIB we are about to write out (0=bottom,
							increasing as it goes up).
				Height		height in pixels of this chunk
				BlockStart	the start address of the bytes of this chunk
	(optional)	InputBPP	BPP of the input image (assumed 32 by default - 8 also works)
				ProgressOffset	value to add to value passed to ContinueSlowJob (default 0).
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Writes a chunk of bitmap data to the device.
				Assumes a progress hourglass is required and the caller has started an hourglass
				with a size of 100 and will end it.
	Notes:		Originally this routine assumed that it was being given a 32bpp bmp, but using
				the InputBPP param it will now handle 8bpp bmps as well...
	Errors:		Calls SetError on FALSE returns.
	Scope:		Public

********************************************************************************************/

BOOL OutputDIB::WriteBlock( UINT32 YPos, UINT32 Height, LPBYTE BlockStart, UINT32 InputBPP,
							INT32 ProgressOffset)
{
	FNPTR_SCANLINE ConvertFn = NULL;
	LPBYTE Buffer = NULL;
	size_t BufSize = 0L;
	size_t ChunkHeight = 1;
	DIBConvert *DoConvert = NULL;

	// Set up the size and other information for the dib block that we require. This is
	// dependent on the export depth or bpp required.
	if ( !SetUpBlock( &BufSize, &ChunkHeight, &DoConvert, &ConvertFn ) )
		return FALSE;			// Error details already set up

	if (BufSize)
	{
		Buffer = (LPBYTE)CCMalloc( BufSize );
		if (Buffer==NULL)
			return FALSE;
	}

	if ( DoConvert )
	{
		// use new classes to do it
		// 8bpp, 4bpp and 1bpp conversion
		INT32 h = Height;
		INT32 count = 0;
		LPBYTE Data = BlockStart;
		const size_t SourceWidth = DIBUtil::ScanlineSize( BitmapInfo.biWidth, InputBPP ) * ChunkHeight;
		const size_t DestWidth   = DIBUtil::ScanlineSize( BitmapInfo.biWidth, BitmapInfo.biBitCount );

		while (h)
		{
			ENSURE(h >= 0, "bad looping");

			const size_t ThisBit = min( h, (INT32)ChunkHeight );
			if (!DoConvert->Convert( Data, Buffer, ThisBit, IsFirstStrip ))
				break;								// stop if conversion failed

			IsFirstStrip = FALSE;

			OutputFile->write( Buffer, ThisBit * DestWidth );
			if (OutputFile->bad())
				break;								// stop if file errored
			Data += SourceWidth;
			h -= ThisBit;

			// now update the progress display, started with CurrentExportSize
			// CurrentExport size is now the point to go from in the export
			count++;
			ContinueSlowJob( (INT32)( ProgressOffset + count ));
			//ContinueSlowJob( (INT32)(100*count/(Height)) );
		}
	}
	// now the bytes (this is crying out for a virtual function or two)
	else if ( ConvertFn && Buffer )
	{
		// Write via conversion function
		// 24 bpp convert
		UINT32 h = Height;
		INT32 count = 0;
		LPBYTE Data = BlockStart;
		const size_t SourceWidth = DIBUtil::ScanlineSize( BitmapInfo.biWidth, InputBPP );

		while (h)
		{
			ConvertFn( BitmapInfo.biWidth, Data, Buffer );
			OutputFile->write( Buffer, BufSize );
			if (OutputFile->bad())
				break;								// stop if file errored
			Data += SourceWidth;
			h -= ChunkHeight;

			// now update the progress display, started with CurrentExportSize
			// ProgressOffset size is now the point to go from in the export
			count++;
			ContinueSlowJob( (INT32)( ProgressOffset + count ));
			//ContinueSlowJob( (INT32)((CurrentExportSize * count)/Height) );
		}
	}
	else
	{
		// Write it all out in one go
		//OutputFile->write( BlockStart, BitmapInfo.biSizeImage );
		// Write the actual bytes out to file. Used to do it in one go but we really
		// require some progress bar indication so we will do it in chunks.
		DWORD BitsSize = BitmapInfo.biSizeImage; 
		if (BitsSize > 0)
		{
			if (BitsSize < 1024)
			{
				// File very small or no progress bar required, so load in one go
				OutputFile->write( BlockStart, BitsSize);
			}
			else
			{
				// Load in chunks, for present split into 100 chunks
				DWORD ChunkSize = BitsSize/100;
				DWORD Position = 0;
				LPBYTE pBitInfo = BlockStart;
				
				while (Position < BitsSize)
				{
					if ( (BitsSize - Position) > ChunkSize)
						OutputFile->write( pBitInfo, ChunkSize);
					else
					{
						ChunkSize = BitsSize - Position;
						OutputFile->write( pBitInfo, ChunkSize);
					}
							
					// Increment our counters/pointers
					Position+=ChunkSize;
					pBitInfo+=ChunkSize;
					// Progress bar started with height of bitmap
					ContinueSlowJob( (INT32)(ProgressOffset + (Height * Position)/BitsSize) );
					//ContinueSlowJob( (INT32)((CurrentExportSize * Position)/BitsSize) );
				}
			}
		}
	}

	// If present, get rid of our export function
	if (DoConvert)
	{
		delete DoConvert;
	 	DoConvert = NULL;
	}

	CCFree( Buffer );

	HeightWritten += Height;						// remember we wrote this lot

	return !OutputFile->bad();
}

/********************************************************************************************
//
// These are the forms used by the Accusoft filters and hence do not write out the data
// themselves but get called back to supply the data.  
//
********************************************************************************************/

// The following function is no longer used but retained for informational reasons
#if 0
/********************************************************************************************

>	BOOL OutputDIB::SetUpExport( CCLexFile *File, LPBITMAPINFOHEADER lpHeader, LPRGBQUAD Palette,
								 UINT32 DitherType, UINT32 OutputDepth, DWORD CompressionType, UINT32 FinalHeight,
								 LPBYTE BlockStart, UINT32 StripSize, INT32 ExportSize, BOOL Forward)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/94
	Inputs:		File			opened file we can write to.
				lpHeader		contains width & DPI of source & dest. biHeight & depth ignored.
				Palette			pointer to bmiColor struct, or NULL if not used (can be temporary)
				OutputDepth		depth of bitmap required on disk
				CompressionType	BI_RGB only supported (or CC_BMPTYPE for 32-bit with trans)
				FinalHeight		output of entire bitmap on disk
				BlockStart		pointer to the start of the bitmap data
				StripSize		size of the first strip to be rendered
				ExportSize		The set progress bar size 
				Forward			dictates whether we are to write the data out forwards or backwards
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Same as OutputDIB::StartFile but does not actually write the data to a file
				but instead tries to get ready to write a bmp out to disk via the Accusoft or
				other external filters, maybe in chunks showing a progress bar as we write it.
				Currently supports:-
					source depth = 32, destintaiotn depth = 32, 24, 8, 4, 1
					source depth = 8, destintaiotn depth = 8
				We need to be able to write data from the bottom to the top of the DIB as well
				as from the top to the bottom as it would appear that the Accusoft DLL tries to
				be clever and save DIBs forward and everything else backwards. 
	Errors:		Calls SetError on FALSE returns.
	SeeAlso:	OutputDIB::StartFile; OutputDIB::WriteBlock; OutputDIB::TidyUp; OutputDIB::GetNextBlock
	Scope:		Public

********************************************************************************************/

BOOL OutputDIB::SetUpExport( CCLexFile *File, LPBITMAPINFOHEADER lpHeader, LPLOGPALETTE Palette,
							 UINT32 DitherType, UINT32 OutputDepth, DWORD CompressionType, UINT32 FinalHeight,
							 LPBYTE BlockStart, UINT32 StripSize, INT32 ExportSize, BOOL Forward )
{
	ERROR2IF(File==NULL, FALSE, "OutputDIB::SetUpExport null File specified");
	ERROR2IF(lpHeader==NULL, FALSE, "OutputDIB::SetUpExport null lpHeader specified");
	ERROR2IF(BlockStart==NULL, FALSE, "OutputDIB::SetUpExport null BlockStart specified");

	// remember input args
	OutputFile = File;
	StartPos = OutputFile->tell();						// remember where in the file this starts
	BitmapInfo = *lpHeader;								// take a copy of user's header
	OutputPalette = NULL;								// No palette to start with
	HeightWritten = 0;									// zero height
	OutputForward = Forward;							// Note the output direction
	CurrentStripSize = StripSize;						// Note size of first strip
	CurrentExportSize = ExportSize;						// size set up for the progress bar
	HeightWanted = FinalHeight;							// the actual height of the export required

	// Remember which dithering type to use
	Dither = DitherType;

	IsFirstStrip = TRUE;

	ExportBuffer = NULL;	// Just in case of early random exit set this to NULL

	INT32 PalSize = 0;									// how many entries in palette

	// Set up the information header for the dib
	if ( !SetUpInfoHeader(lpHeader, OutputDepth, CompressionType, FinalHeight, &PalSize) )
		return FALSE;									// SetError already called 

	// then the RGBQUAD palette, if required
	if (PalSize)
	{
		ERROR2IF(Palette == NULL,FALSE,"SetUpExport has a NULL Palette when one is required");
		// If we are creating a palette then the Accusoft code requires that we specify an
		// bitmap info header which is followed immediately in memory by the palette.
		// if we just created a Palette then this would not be true so we must create a
		// new copy of the Bitmapinfo and then create the palette next.
		// This will not work in debug builds due to memory tracking information stored
		// in the allocation so we must allocate one big block and do the info header and
		// Palette from these.
		const size_t TotalHeader = sizeof(BITMAPINFOHEADER); 
		const size_t TotalPalSize = (sizeof(RGBQUAD) * PalSize );
		lpBitmap = (LPBITMAPINFO)CCMalloc( TotalHeader + TotalPalSize );
		if (lpBitmap==NULL)
			return FALSE;

		// make a copy of the palette for possible later use
		const size_t TotalPal = sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * PalSize );
		
		OutputPalette = (LPLOGPALETTE)CCMalloc( TotalPal );
		if (OutputPalette==NULL)
			return FALSE;
		memcpy( OutputPalette, Palette, TotalPal );
		
		// These are all freed in the TidyUp call.

		// Take a copy of the bitmap info header
		lpBitmapInfo = &(lpBitmap->bmiHeader);				// pointer to info header
		memcpy( lpBitmapInfo, &BitmapInfo, TotalHeader );	// copy header info across

		// Now get a pointer to the palette
		LPRGBQUAD lpPalette = NULL;
		lpPalette = &(lpBitmap->bmiColors[0]);				// pointer to colours table

		// Now copy the palette across into the bitmap info structure in RGQUAD form
		// OutputPalette should be set up by now so use this instead of Palette so if
		// we have bodged it, it will be the correct bodged palette that is put into the file
		// and not the 256 entry palette that seems to be supplied always.

		//  We might want to use the existing palette - e.g. when we are altering the value
		//  of a single colour.
			//  Don't want to use the existing palette.
		for (INT32 i = 0; i < PalSize; i++)
		{
			lpPalette->rgbRed	= OutputPalette->palPalEntry[i].peRed;
			lpPalette->rgbGreen = OutputPalette->palPalEntry[i].peGreen;
			lpPalette->rgbBlue	= OutputPalette->palPalEntry[i].peBlue;
			
			lpPalette++;
		}

	}

	// Now lets grab the memory and then set up the first block of data ready for output
	size_t BufSize = 0L;
	ExportChunkHeight = 1;

	DoExportConvert = NULL;		// Just in case the SetUpBlock fails 
	ExportConvertFn = NULL;		// Just in case the SetUpBlock fails
	
	// Set up the size and other information for the dib block that we require. This is
	// dependent on the export depth or bpp required.
	if ( !SetUpBlock( &BufSize, &ExportChunkHeight, &DoExportConvert, &ExportConvertFn ) )
		return FALSE;			// Error details already set up

//TRACEUSER( "Neville", _T("SetUpExport BufSize = %d\n"),BufSize);
//TRACEUSER( "Neville", _T("SetUpExport ExportChunkHeight = %d\n"),ExportChunkHeight);
//TRACEUSER( "Neville", _T("SetUpExport FinalHeight = %d\n"),FinalHeight);
//TRACEUSER( "Neville", _T("SetUpExport CurrentStripSize = %d\n"),CurrentStripSize);
//TRACEUSER( "Neville", _T("SetUpExport DoExportConvert = %d\n"),DoExportConvert);
//TRACEUSER( "Neville", _T("SetUpExport ExportConvertFn = %d\n"),ExportConvertFn);

	// Check for the case where we are exporting a small thin strip which is smaller
	// than our normal strip  
	if (CurrentStripSize < ExportChunkHeight)
	{
		ExportChunkHeight = CurrentStripSize;
//TRACEUSER( "Neville", _T("New ExportChunkHeight = %d\n"),ExportChunkHeight);
	}

	if (BufSize)
	{
		ExportBuffer = (LPBYTE)CCMalloc( BufSize );
		if (ExportBuffer==NULL)
			return FALSE;
	}
	
	// Now get the actual class variables set up ready for this data
	SetUpForNextStrip(BlockStart, CurrentStripSize);

	// Now lets start the whole export process off by calling the StartSaveBitmap function
	// which will start the actual exporting process off with the supplied information.
	// This will then request blocks of data from the DestDIB in its call back routine
	// get_dib_data.

	// If we have created a palette then use the special header which has a valid palette
	// following immediately after it in memory
	LPBITMAPINFOHEADER lpSpBitmapInfo = NULL;
	if (PalSize)
		lpSpBitmapInfo = lpBitmapInfo;		// Use our specially create header followed by palette
	else
		lpSpBitmapInfo = &BitmapInfo;		// Use the proper header defined in the class

#ifndef WEBSTER
	//WEBSTER-Martin-07/01/97
	// no Accusoft stuff in WEBSTER, and I'm not sure it ever gets used in Camelot any
	// more (the DIB bits of Accusoft that is)
	// TRACEUSER( "Martin", _T("Not allowed to use Accusoft save bitmap\n"));
	if (!AccusoftFilters::StartSaveBitmap(	OutputFile,		// File to use
											lpSpBitmapInfo,	// pointer to info header
											OutputPalette,	// pointer to the palette
											OutputDepth,	// actual file depth
											BI_RGB,			// compression (BI_RGB = none)
											FinalHeight,	// all of it
											StripSize,		// size of the strip, might be FinalHeight
											OutputForward)	// direction we were told on entry
	   )
#endif //WEBSTER
	{
		return FALSE;	//always for WEBSTER
	}

	return TRUE;
}
#endif


/********************************************************************************************

>	static BOOL OutputDIB::GetNextBlock( UINT32 YPos, UINT32 Height, LPBYTE BlockStart,
										 LPBYTE *Buffer, UINT32 *CurrentBlockSize,
										 INT32 *CurrentStartYPos)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/94
	Inputs:		YPos				pixel co-ord of the chunk of DIB we are about to write out 
									(0 = bottom, increasing as it goes up).
				Height				height in pixels of this chunk
				BlockStart			the start address of the bytes of this chunk
	Outputs:	Buffer				pointer to the returned buffer filled up with DIB bytes
				CurrentBlockSize	current size of the block being rendered 
				CurrentStartYPos	current position of the start of this block of data
				WrittenSoFar		the amount of the bitmap written to date
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Gets the next chunk of bitmap data ready to be written out via the Accusoft or
				other external filters.
				SetUpExport is used to set up everything before this fucntion is called.
				OutputForward determines the direction of export, set up in SetUpExport
	Errors:		Calls SetError on FALSE returns.
	SeeAlso:	OutputDIB::WriteBlock; OutputDIB::SetUpExport;
	Scope:		Public

********************************************************************************************/

BOOL OutputDIB::GetNextBlock( UINT32 YPos, UINT32 Height, LPBYTE BlockStart,
							  LPBYTE *Buffer, INT32 *CurrentBlockSize,
							  INT32 *CurrentStartYPos, UINT32 *WrittenSoFar)
{
	// Get the next block ready for export a line at a time
	*Buffer = NULL;				// Just in case of early random exit set this to NULL
	*CurrentBlockSize = 0;		// Just in case of early random exit set this to 0

	// Convert the next strip 
	if ( DoExportConvert )
	{
//TRACEUSER( "Neville", _T("GetNextBlock DoExportConvert\n"));
		// 8 bpp, 4bpp and 1bpp export
		ERROR2IF(ExportData < BlockStart, FALSE, "GetNextBlock ExportData < BlockStart");
		ERROR2IF(Height - CurrentYPos < 0, FALSE, "GetNextBlock Height - CurrentYPos < 0");
		// Work out the size of the export chunk left, normally ExportChunkHeight but
		// if at the end of export may be a small strip left.
		const size_t ThisBit = min( (INT32)(Height - CurrentYPos), (INT32)ExportChunkHeight );
		if (!DoExportConvert->Convert( ExportData, ExportBuffer, ThisBit, IsFirstStrip ))
			return FALSE;							// stop if conversion failed

		IsFirstStrip = FALSE;

		// We have in ExportData a size of data block = ThisBit * ExportDestWidth
		// On last strip may be less than ExportChunkHeight in depth 
		CurrentYPos += ThisBit;						// Move down by a strips worth

		if (OutputForward)
		{
			ExportData += ExportSourceWidth;		// Move on by a lines worth
		}
		else
		{
			if ((Height - CurrentYPos) < ExportChunkHeight )
				ExportData = BlockStart;				// On last strip so do from start
			else
				ExportData -= ExportSourceWidth;		// Move on by a lines worth
			if (ExportData < BlockStart)				// Just in case test
				ExportData = BlockStart;				
		}

		// Set up return variables
		*Buffer = ExportBuffer;						// return our new buffer to the caller
		*CurrentBlockSize = ThisBit;				// return size of this block
		*CurrentStartYPos = CurrentYPos;			// return current start of strip
		
		HeightWritten += ThisBit;					// remember we wrote this lot
	}
	// now the bytes (this is crying out for a virtual function or two)
	else if ( ExportConvertFn && ExportBuffer )
	{
		// 24 bpp convert
//TRACEUSER( "Neville", _T("GetNextBlock Do export via convert function\n"));
		ERROR2IF(ExportData < BlockStart, FALSE, "GetNextBlock ExportData < BlockStart");
		// write data via conversion function
		ExportConvertFn( BitmapInfo.biWidth, ExportData, ExportBuffer );

		if (OutputForward)
		{
			// We have in ExportBuffer an amount of BuffSize worth of data
			ExportData += ExportSourceWidth;
			CurrentYPos -= ExportChunkHeight;
		}
		else
		{
			// We have in ExportBuffer an amount of BuffSize worth of data
			ExportData -= ExportSourceWidth;
			CurrentYPos += ExportChunkHeight;
		}

		// Set up return variables
		*Buffer = ExportBuffer;						// return our new buffer to the caller
		*CurrentBlockSize = ExportChunkHeight;		// return size of this block
		*CurrentStartYPos = CurrentYPos;			// return current start of strip

		HeightWritten += ExportChunkHeight;			// remember we wrote this lot
	
	}
	else
	{
//TRACEUSER( "Neville", _T("GetNextBlock all in one go\n"));
		// Write it all out in one go, 32bpp export
		// We have in the DIB at BlockStart an amount of BitmapInfo.biSizeImage worth of data
		// Set up return variables
		*Buffer = BlockStart;						// return our new buffer to the caller
		*CurrentBlockSize = BitmapInfo.biSizeImage;	// return size of this block

		HeightWritten += Height;					// remember we wrote this lot
	}

	// Return the amount of data written so far  
	*WrittenSoFar = HeightWritten;

	return TRUE;
}

/********************************************************************************************

	static BOOL OutputDIB::SetUpForNextStrip(LPBYTE StartOfBytes, UINT32 StripHeight)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		StartOfBytes	start of the actual bitmap data (bits)
				StripHeight		height of this strip
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed
	Purpose:	Gets the class variables ready for the first/next strip of the render region
				as we are rendering in strips.
	Scope:		Public Static

********************************************************************************************/

BOOL OutputDIB::SetUpForNextStrip(LPBYTE StartOfBytes, UINT32 StripHeight)
{
	// We have rendered the next strip into the render region so reset everything back
	// to start outputting this strip instead of the old one
	CurrentStripSize = StripHeight;
//TRACEUSER( "Neville", _T("OutputDIB::SetUpForNextStrip StripHeight=%d\n"),StripHeight);
//TRACEUSER( "Neville", _T("OutputDIB::SetUpForNextStrip HeightWritten=%d\n"),HeightWritten);
	 
	// Now get the actual data ready
	if ( DoExportConvert )
	{
//TRACEUSER( "Neville", _T("OutputDIB::SetUpForNextStrip DoExportConvert\n"));
		// 8bpp, 4bpp and 1bpp conversion
		UINT32 LineWidth = DIBUtil::ScanlineSize( BitmapInfo.biWidth, 32 );
		ExportSourceWidth =  LineWidth * ExportChunkHeight;
		ExportDestWidth   = DIBUtil::ScanlineSize( BitmapInfo.biWidth, BitmapInfo.biBitCount );

		if (OutputForward)
		{
			// Set up the pointer to the first block of DIB bytes to export
			// In this forward direction it is the start of the first strip of the DIB
			ExportData = StartOfBytes;
		}
		else
		{
			// Set up the pointer to the first block of DIB bytes to export
			// Which is the start of the last strip of the DIB
			ExportData = StartOfBytes + ((StripHeight * LineWidth) - ExportSourceWidth);
		}
	}
	// now the bytes (this is crying out for a virtual function or two)
	else if ( ExportConvertFn && ExportBuffer )
	{
//TRACEUSER( "Neville", _T("OutputDIB::SetUpForNextStrip Do via convert function to ExportBuffer\n"));
		// 24 bpp convert
		ExportSourceWidth = DIBUtil::ScanlineSize( BitmapInfo.biWidth, 32 );

		if (OutputForward)
		{
			// Set up the pointer to the first block of DIB bytes to export
			// In this forward direction it is the start of the first strip of the DIB
			ExportData = StartOfBytes;
		}
		else
		{
			// Set up the pointer to the first block of DIB bytes to export
			// Which is the start of the last strip of the DIB
			ExportData = StartOfBytes + ((StripHeight - 1) * ExportSourceWidth);
		}
 	}
	else
	{
//TRACEUSER( "Neville", _T("OutputDIB::SetUpForNextStrip Do all in one go\n"));
		// We will write it all in one go
		ExportData = StartOfBytes;		// pointer to the DIB bytes
	}
//TRACEUSER( "Neville", _T("ExportSourceWidth = %d\n"),ExportSourceWidth);
//TRACEUSER( "Neville", _T("ExportDestWidth = %d\n"),ExportDestWidth);

	// Just in case the initial block is smaller than the usual output depth and hence
	// we will be trying to export data to before the start of the block. A quick check
	// is in order.
	if (ExportData < StartOfBytes)				// Just in case 
		ExportData = StartOfBytes;				

	// Set up the starting parameters of the export operation.
	// Remember we have to export from the end of the DIB up to the start otherwise
	// the output will be upside down.
	CurrentYPos = 0;					// size of DIB in scan lines

	return TRUE;
}	

/********************************************************************************************

>	BOOL OutputDIB::SetUpBlock( size_t *pBufSize, size_t *pChunkHeight, DIBConvert **pDoConvert,
								FNPTR_SCANLINE *pConvertFn )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/94
	Inputs:
	Outputs:	pBufSize	pointer to a size_t which gives the buffer size required for this
							output depth.
				pChunkHeight	pointer to a size_t which gives the size of chunk required for
								this output depth.
				pDoConvert	pointer to a pointer to a dib convert class for this depth.
				pConvertFn	pointer to a convert function pointer
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Gets the next chunk of bitmap data ready to be written out via the Accusoft or
				other external filters.
	Notes:		This routine will currently handle conversions from 32->32, 32->24, 32->8 and
				now 8->8.
	Errors:		Calls SetError on FALSE returns.
	Scope:		Public

********************************************************************************************/

BOOL OutputDIB::SetUpBlock( size_t *pBufSize, size_t *pChunkHeight, DIBConvert **pDoConvert,
							FNPTR_SCANLINE *pConvertFn )
{
	BOOL Problems = FALSE;

	switch (SourceBitmapDepth)
	{
		case 32:
		{
			switch (BitmapInfo.biBitCount)
			{
				case 32:
					{
						// real 32-bit BMPs here
						// our 'filter' zeros the transparency bits
						*pConvertFn = DIBUtil::Convert32to32;
						*pBufSize = DIBUtil::ScanlineSize( BitmapInfo.biWidth, 32 );
					}
					break;
				case 24:
					{
						// convert 32-bit BMPs to 24-bit ones so things can read them
						*pConvertFn = DIBUtil::Convert32to24;	  						// 32->24
						*pBufSize = DIBUtil::ScanlineSize( BitmapInfo.biWidth, 24 );	// size of 24-bit scanline
					}
					break;
				case 8:
					{
						// 32->8 we do in bigger chunks because of dithering
						*pDoConvert = DIBConvert::Create( SourceBitmapDepth, 8, BitmapInfo.biWidth, OutputPalette, Dither );
						if (*pDoConvert==NULL)
						{
							ENSURE(FALSE, "DIBConvert::Create returned NULL");
							return FALSE;
						}
						*pChunkHeight = 16;
						*pBufSize = DIBUtil::ScanlineSize( BitmapInfo.biWidth, 8 ) * (*pChunkHeight);
					}
					break;
				case 4:
					{
						// 32->4 we do in bigger chunks because of dithering
						*pDoConvert = DIBConvert::Create( SourceBitmapDepth, 4, BitmapInfo.biWidth, OutputPalette, Dither );
						if (*pDoConvert==NULL)
						{
							ENSURE(FALSE, "DIBConvert::Create returned NULL");
							return FALSE;
						}
						*pChunkHeight = 16;
						*pBufSize = DIBUtil::ScanlineSize( BitmapInfo.biWidth, 4 ) * (*pChunkHeight);
					}
					break;
				case 1:
					{
						// 32->1 we do in bigger chunks because of dithering
						*pDoConvert = DIBConvert::Create( SourceBitmapDepth, 1, BitmapInfo.biWidth, OutputPalette, Dither );
						if (*pDoConvert==NULL)
						{
							ENSURE(FALSE, "DIBConvert::Create returned NULL");
							return FALSE;
						}
						*pChunkHeight = 16;
						*pBufSize = DIBUtil::ScanlineSize( BitmapInfo.biWidth, 1 ) * (*pChunkHeight);
					}
					break;
				default:
					Problems = TRUE;
					break;
			}
			break;
		}
		case 8:
			if (BitmapInfo.biBitCount==8)
			{
				// real 8-bit BMPs here
				// we basically just do a memory copy from source to dest
				*pConvertFn = DIBUtil::Convert8to8;
				*pBufSize = DIBUtil::ScanlineSize( BitmapInfo.biWidth, 8 );
				break;

			}
			Problems = TRUE;
			break;

		default:
			Problems = TRUE;
			break;
	}

	if(Problems)
	{
		Error::SetError( _R(IDE_FORMATNOTSUPPORTED) );
		return FALSE;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL OutputDIB::TidyUp()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	When everything has been done via WriteBlock, call this to update the header
				etc. The caller is responsible for closing the file. The class CANNOT be used
				for further output until StartFile is called again.
	Errors:		Calls SetError on FALSE returns.
	Scope:		Public

********************************************************************************************/

BOOL OutputDIB::TidyUp()
{
	BOOL Result = TRUE;

	if(OutputFile != NULL)
	{
		// Check that what we wrote was what was required in terms of number of lines
		// used to use BitmapInfo.biHeight but this may now be the height of a strip
#ifdef _BATCHING         
		if ( HeightWritten != HeightWanted )
		{
			TRACEUSER( "Martin", _T("OutputDIB::TidyUp HeightReqd = %d  HeightWritten = %d\n"),
				HeightWanted,HeightWritten);
		}
#else	//cos this goes off
		ERROR3IF( HeightWritten != HeightWanted, "Wrote wrong amount of bitmap to disk");
#endif

		// could do fixups into the header here etc when using compression
		Result = !OutputFile->bad();

		OutputFile = NULL;								// stop usage until StartFile time

		// If 8, 4, 1bpp export then put back the recommended palette by destructing the
		// convert function.
		if (DoExportConvert)
		{
			delete DoExportConvert;
			DoExportConvert = NULL;
		}	
	}

	// Free up the bitmap info if present.
	// This contains the info header and palette
	if (lpBitmap)
	{
		CCFree(lpBitmap);
		lpBitmap = NULL;
	}

	// Free up the output palette buffer if present
	if (OutputPalette)
	{
		CCFree(OutputPalette);
		OutputPalette = NULL;
	}

	// Free up the export buffer if present
	if (ExportBuffer)
	{
		CCFree(ExportBuffer);
		ExportBuffer = NULL;
	}

	return Result;
}
