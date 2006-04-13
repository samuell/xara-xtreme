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
#include "outptgif.h"
#include "ensure.h"
#include "dibutil.h"
#include "fixmem.h"
#include "errors.h"
//#include "andy.h"
#include "dibconv.h"
#include "progress.h"		// For hourglass stuff
//#include "gifutil.h"		// GIF header definitions
//#include "string.h"			// memcpy
#include "camfiltr.h"		// BaseCamelotFilter

#define	new	CAM_DEBUG_NEW

/********************************************************************************************
>	OutputGIF::OutputGIF()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Purpose:	Default constructor for the class. Just sets up the pointers to our buffers
				that may be used to be null so that the TidyUp function can do its job.
	SeeAlso:	OutputGIF::TidyUp
	SeeAlso:	OutputDIB;

********************************************************************************************/

OutputGIF::OutputGIF()
{
	OutputFile = NULL;
	lpBitmap = NULL;
	ExportBuffer = NULL;
	DoExportConvert = NULL;
	OutputForward = FALSE;

	DestBitmapInfo = NULL;
	DestBitmapBytes = NULL;

	InterlacingOn = FALSE;

	pCamFilter = NULL;
}


/********************************************************************************************

>	void Putword( INT32 word, CCLexFile* pFile )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		Word	INT32 to write to file as 16bit value
				File 	file to write to
	Returns:	-
	Purpose:	Write out a word to the GIF file
	Scope:		private
	
********************************************************************************************/

void Putword( INT32 Word, CCLexFile *File)
{
    File->put( (Word & 0xff));
    File->put( ((Word / 256) & 0xff));
}



/********************************************************************************************

>	virtual BOOL OutputGIF::StartFile( LPBITMAPINFOHEADER lpHeader, LPLOGPALETTE Palette, UINT32 OutputDepth,
										DWORD CompressionType, UINT32 FinalHeight, INT32 ExportSize, UINT32 DitherType = XARADITHER_ORDERED_GREY);

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/6/96
	Inputs:		lpHeader	contains width & DPI of source & dest. biHeight & depth ignored.
				Palette		pointer to bmiColor struct, or NULL if not used (can be temporary)
				OutputDepth	depth of bitmap required on disk
				CompressionType	in this case is used to pass in the transparency and interlace
							information as GIFs are always compressed. Passed in as a number between
							0 .. 3 which maps onto the TI_GIF filter types.

				FinalHeight	output of entire bitmap on disk
				ExportSize		The set progress bar size 
				DitherType	

	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Prepare to start the export of a GIF.
	SeeAlso:	OutputGIF::WriteBlock; OutputGIF::TidyUp

********************************************************************************************/

BOOL OutputGIF::StartFile ( LPBITMAPINFOHEADER lpHeader,
							LPLOGPALETTE Palette,
							UINT32 OutputDepth,
							DWORD CompressionType,
							UINT32 FinalHeight,
							INT32 ExportSize,
							UINT32 DitherType )
{
	ERROR2IF( lpHeader==NULL, FALSE, "NULL lpHeader param");
	ERROR2IF( Palette==NULL, FALSE, "NULL Palette param");

	// Set member variables
	if (DestBitmapInfo && DestBitmapBytes)
	{
		FreeDIB( DestBitmapInfo, DestBitmapBytes );
		DestBitmapInfo = NULL;
		DestBitmapBytes = NULL;	
	}

	if (OutputPalette)
	{
		CCFree(OutputPalette);
		OutputPalette = NULL;
	}

	IsFirstStrip = TRUE;
	HeightWritten = 0;

	// remember input args
	BitmapInfo = *lpHeader;								// take a copy of user's header
	CurrentExportSize = ExportSize;						// size set up for the progress bar
	HeightWanted = FinalHeight;							// the actual height of the export required
	Dither = DitherType;

	// We will need to have the entire image present before writing out so that we can
	// cope with interlacing and transparency, so create that DIB
	// Set up the information header for the dib which we hold during export
	UINT32 LineWidth = DIBUtil::ScanlineSize( BitmapInfo.biWidth, OutputDepth );
	INT32 PalSize = 0;			
	BOOL ok = SetUpInfoHeader(lpHeader, OutputDepth, CompressionType, LineWidth, FinalHeight, &PalSize);

	// Claim memory for the bitmap
	if (ok)
	{
		DestBitmapInfo = AllocDIB( BitmapInfo.biWidth, FinalHeight, OutputDepth, &DestBitmapBytes );
		ok = (DestBitmapInfo != NULL) && (DestBitmapBytes != NULL);
	}
	
	// Transfer across the required other bits of info
	if (ok)
	{
		DestBitmapInfo->bmiHeader.biXPelsPerMeter = BitmapInfo.biXPelsPerMeter;
		DestBitmapInfo->bmiHeader.biYPelsPerMeter = BitmapInfo.biYPelsPerMeter;
		DestBitmapInfo->bmiHeader.biClrUsed = PalSize;
	}

	// Point the place to put the next strip of data at the start ready for the first strip
	pNextStrip = DestBitmapBytes;	
	
	// Take a copy of the palette
	if (ok)
	{
		const size_t TotalPal = sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * PalSize );
		OutputPalette = (LPLOGPALETTE)CCMalloc( TotalPal );
		if (OutputPalette != NULL)
			memcpy( OutputPalette, Palette, TotalPal );
		else
			ok = FALSE;
	}

	// Clean up if an error happened
	if (!ok)
	{
		// Free up the DIB that we have just created
		FreeDIB( DestBitmapInfo, DestBitmapBytes );
		DestBitmapInfo = NULL;
		DestBitmapBytes = NULL;	
		if (OutputPalette != NULL)
		{
			CCFree(OutputPalette);
			OutputPalette = NULL;
		}
	}

	return ok;
}



/********************************************************************************************
>	BOOL OutputGIF::ReStartFile(LOGPALETTE* pNewPal)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/96
	Inputs:		pNewPal - New (optimised) palette to use
	Returns:	TRUE if worked, FALSE if failed
	Purpose:	Called to reset output before outputing another in a run of multiple images
	SeeAlso:	OutputGIF::StartFile
********************************************************************************************/
BOOL OutputGIF::ReStartFile(LOGPALETTE* pNewPal)
{
	// Reset member variables
	IsFirstStrip = TRUE;
	HeightWritten = 0;
	pNextStrip = DestBitmapBytes;	

	if (pNewPal!=NULL)
	{
		ERROR2IF(OutputPalette==NULL, FALSE, "No output palette");
		ERROR3IF(pNewPal->palNumEntries != OutputPalette->palNumEntries, "Different sized palettes");

		const size_t PalSize = sizeof(PALETTEENTRY) * pNewPal->palNumEntries-1;
 		memcpy( OutputPalette->palPalEntry, pNewPal->palPalEntry, PalSize );
	}
	
	return TRUE;
}


/********************************************************************************************

>	BOOL OutputGIF::SetUpInfoHeader(const LPBITMAPINFOHEADER lpHeader, const UINT32 OutputDepth,
									const DWORD CompressionType,
									const UINT32 LineWidth, const UINT32 FinalHeight,
									INT32 * pPalSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/94
	Inputs:		lpHeader	contains width & DPI of source & dest. biHeight & depth ignored.
				OutputDepth	depth of bitmap required on disk
				CompressionType	BI_RGB only supported (or CC_BMPTYPE for 32-bit with trans)
				LineWidth 	destination line width
				FinalHeight	output of entire bitmap on disk
	Outputs:	pPalSize	pointer to an integer PalSize variable which is the palette size
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Set up the information header for the DIB.
	Errors:		Calls SetError on FALSE returns.
	SeeAlso:	OutputGIF::StartFile; OutputGIF::WriteBlock; OutputGIF::TidyUp
	Scope:		Public

********************************************************************************************/
BOOL OutputGIF::SetUpInfoHeader(const LPBITMAPINFOHEADER lpHeader, const UINT32 OutputDepth, const DWORD CompressionType,
								const UINT32 LineWidth, const UINT32 FinalHeight, INT32 * pPalSize)
{
	SourceBitmapDepth = lpHeader->biBitCount;
	BitmapInfo.biBitCount = OutputDepth;

	const WORD DestDepth = BitmapInfo.biBitCount;

	// source bitmap not necessarily same as required file depth
	// make sure we can handle conversion
	BOOL FormatOK = FALSE;

	switch (SourceBitmapDepth)
	{
		case 32:
			{
				switch (DestDepth)
				{
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
>	BOOL OutputGIF::OutputGifFileHeader(CCLexFile *File, BOOL Enhanced, INT32 TransColour)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		File - output file object
				Enhanced - TRUE if enhanced GIF features are required (eg transpacency)
				TransColour - the index of the transparent colour (-1 for no transparency)
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed
	Purpose:	Writes out the file header for the cached bitmap within this class
	SeeAlso:	OutputGIF::OutputGifFileHeader (the other one)
********************************************************************************************/
BOOL OutputGIF::OutputGifFileHeader(CCLexFile *File, BOOL Enhanced, INT32 TransColour)
{
	return OutputGifFileHeader(File, &(DestBitmapInfo->bmiHeader), Enhanced, TransColour, OutputPalette);
}


/********************************************************************************************

>	BOOL OutputGIF::OutputGifFileHeader(CCLexFile *, LPBITMAPINFOHEADER, BOOL Enhanced, INT32 TransColour, PLOGPALETTE = NULL, LPRGBQUAD pQuadPalette = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/95
	Inputs:		File			file to output to
				pInfo			pointer to the bitmap info header
				Enhanced		TRUE if useing advanced GIF features such as transparency or animation
				TransColour		The transparent index or -1 if no transparency
				pPalette		pointer to a palette in LOGPALETTE form (defaults to NULL)
				OR
				pQuadPalette	pointer to a palette in RGBQUAD form (defaults to NULL)	
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Output a GIF file header for the specified bitmap to the file.
	Errors:		Calls SetError on FALSE returns.
	SeeAlso:	OutputGIF::StartFile; OutputGIF::WriteBlock; OutputGIF::TidyUp
	Scope:		static

********************************************************************************************/
BOOL OutputGIF::OutputGifFileHeader(CCLexFile *File, LPBITMAPINFOHEADER pInfo, BOOL Enhanced, INT32 TransColour, LPLOGPALETTE pPalette, LPRGBQUAD pQuadPalette)
{
PORTNOTETRACE("GIFFilter", "Removed use of GIF code");
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(File==NULL,FALSE,"OutputGIF::OutputGifHeader File pointer is null");
	ERROR2IF(pInfo==NULL,FALSE,"OutputGIF::OutputGifHeader BitmapInfo pointer is null");
	ERROR2IF(pPalette==NULL && pQuadPalette==NULL,FALSE,"OutputGIF::OutputGifHeader Bitmap palette pointer is null");

	// Note file in our class variable as used by all the low level routines
	OutputFile = File;

	// Must set the exception throwing flag to True and force reporting of errors to False.
	// This means that the caller must report an error if the function returns False.
	// Any calls to CCFile::GotError will now throw a file exception and should fall into
	// the catch handler at the end of the function.
	// Replaces the goto's that handled this before.
	BOOL OldThrowingState = File->SetThrowExceptions( TRUE );
	BOOL OldReportingState = File->SetReportErrors( FALSE );

	try
	{
		// Set up the class variables
		// First the width/height of the bitmap
	    Width = pInfo->biWidth;
	    Height = pInfo->biHeight;
		INT32 PalSize = pInfo->biClrUsed;
		// Translate this into a bits per pixel which is not limited to 8, 4 and 1.
		INT32 ColourDepth = GetColourDepth(PalSize);
		// ColourDepth will be rounded to nearest power of two so need to output
		// that many palette entries
		INT32 NewPalSize = 1 << ColourDepth;
		InitCodeSize = ColourDepth;

		BitsPerPixel = pInfo->biBitCount;

		TRACEUSER( "Neville", _T("OutputGifFileHeader - Colour depth = %d\n"),ColourDepth);

	    // Write out the unique GIF header
		GIFINFOHEADER Header;
		if (Enhanced)
			memcpy(Header.giName, "GIF89a", 6);	
		else
			memcpy(Header.giName, "GIF87a", 6);	
			
	    // Indicate that there is a global colour map
	    BYTE Flags = 0;
		// global colour table flag, colour resoltion sort flag and 
		// size of global colour table
		if ((pPalette || pQuadPalette) && (PalSize > 0))
		    Flags  = 0x80;       			// Flag there is a colour map
		// 3 bit colour resolution value = number of bits per primary colour - 1
	    Flags |= ((BitsPerPixel - 1) & 0x07) << 4;	// OR in the colour resolution
		// 3 bit size of global colour table = number of bits - 1
	    Flags |= (ColourDepth - 1) & 0x07;	// OR in global colour table size

		Header.giWidth 		= (WORD)Width;
		Header.giHeight 	= (WORD)Height;
		Header.giFlags 		= Flags;
		Header.giBackground = TransColour==-1 ? 0 : TransColour;
		Header.giAspect 	= 0;
		// This is really sizeof(GIFINFOHEADER) but this returns 14 instead of 13
		// as it rounds to the nearest word boundary
		const size_t HeaderSize = sizeof(char)* 6 + sizeof(WORD) * 2 + sizeof(BYTE) * 3;
		File->write( &Header, HeaderSize );

 		if (pQuadPalette && NewPalSize > 0)
		{
			// Palette supplied in RGBQUAD form 
			// write it to disk in GIFRGBTRIPLE format
			GIFRGBTRIPLE TempRGB;
			for (INT32 i = 0; i < NewPalSize; i++)
			{
				// If we are using transparency then bodge the transparent colour to
				// be White. Otherwise, if we load it back it then we could have a strange
				// colour being shown as we don't support transparency on loading.
				if (TransColour >= 0 && i == TransColour)
				{
					// Make transparent colour white
					TempRGB.grgbtRed 	= 0xFF;
					TempRGB.grgbtGreen 	= 0xFF;
					TempRGB.grgbtBlue 	= 0xFF;
				}
				else if (i > PalSize)
				{
					// Make blank palette entries black
					TempRGB.grgbtRed 	= 0x00;
					TempRGB.grgbtGreen 	= 0x00;
					TempRGB.grgbtBlue 	= 0x00;
				}
				else
				{
					TempRGB.grgbtRed 	= pQuadPalette->rgbRed;
					TempRGB.grgbtGreen 	= pQuadPalette->rgbGreen;
					TempRGB.grgbtBlue 	= pQuadPalette->rgbBlue;
				}
				
				File->write( &TempRGB, sizeof(GIFRGBTRIPLE) );
				// skip to the next palette entry
				pQuadPalette++;
			}
		}
		else if (pPalette && NewPalSize > 0)
		{
			// Palette supplied in LOGPALETTE form 
			// write it to disk in GIFRGBTRIPLE format
			GIFRGBTRIPLE TempRGB;
			for (INT32 i = 0; i < NewPalSize; i++)
			{
				// If we are using transparency then bodge the transparent colour to
				// be White. Otherwise, if we load it back it then we could have a strange
				// colour being shown as we don't support transparency on loading.
				if (TransColour >= 0 && i == TransColour)
				{
					// Make transparent colour white
					TempRGB.grgbtRed 	= 0xFF;
					TempRGB.grgbtGreen 	= 0xFF;
					TempRGB.grgbtBlue 	= 0xFF;
				}
				else if (i > PalSize)
				{
					// Make blank palette entries black
					TempRGB.grgbtRed 	= 0x00;
					TempRGB.grgbtGreen 	= 0x00;
					TempRGB.grgbtBlue 	= 0x00;
				}
				else
				{
					TempRGB.grgbtRed 	= pPalette->palPalEntry[i].peRed;
					TempRGB.grgbtGreen 	= pPalette->palPalEntry[i].peGreen;
					TempRGB.grgbtBlue 	= pPalette->palPalEntry[i].peBlue;
				}
				File->write( &TempRGB, sizeof(GIFRGBTRIPLE) );
			}
		}
		else
			ERROR3("OutputGIF::OutputGifHeader No palette specified");

		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		// er, we seem to have finished OK so say so
		return TRUE;
	}
	catch(...)
	{
		// catch our form of a file exception
		TRACE( _T("OutputGIF::OutputGifHeader CC catch handler\n"));

		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		return FALSE;
	}

	ERROR2( FALSE, "Escaped exception clause somehow" );
#else
	return FALSE;
#endif
}	



/********************************************************************************************
>	BOOL OutputGIF::OutputGifImageExtensionHeader(CCLexFile *File, BOOL InterlaceState, INT32 TransparentColour, UINT32 Delay, UINT32 RestoreType)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> - from Neville code
	Created:	14/6/96
	Inputs:		File			file to output to
				InterlaceState	TRUE if image should be interlaced
				TransparentColour	index of transparent colour or -1 for no transparency
				Delay			Delay in millseconds between this frame and the previous one
				RestoreType		GIF image restore type (0-3)
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed
	Purpose:	Output a GIF image header for the specified bitmap to the file.
********************************************************************************************/
BOOL OutputGIF::OutputGifImageExtensionHeader(CCLexFile *File, BOOL InterlaceState, INT32 TransparentColour, UINT32 Delay, UINT32 RestoreType)
{
PORTNOTETRACE("GIFFilter", "Removed use of GIF code");
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(File==NULL,FALSE,"OutputGIF::OutputGifHeader File pointer is null");
	ERROR3IF(RestoreType > 3, "Unknown GIF image restore type");

	// Note file in our class variable as used by all the low level routines
	OutputFile = File;
	InterlacingOn = InterlaceState;

	UINT32 TranspFlag = (TransparentColour >= 0) ? 1 : 0;

	// Must set the exception throwing flag to True and force reporting of errors to False.
	// This means that the caller must report an error if the function returns False.
	// Any calls to CCFile::GotError will now throw a file exception and should fall into
	// the catch handler at the end of the function.
	// Replaces the goto's that handled this before.
	BOOL OldThrowingState = File->SetThrowExceptions( TRUE );
	BOOL OldReportingState = File->SetReportErrors( FALSE );
	try
	{
		GIFTRANSBLOCK TransBlock;
		TransBlock.gtbBlockStart 	= EXTENSIONBLOCK;
		TransBlock.gtbIdentifier 	= TRANSPARENTBLOCK;
		TransBlock.gtbBlockSize 	= 4;
		TransBlock.gtbFlags 		= TranspFlag | ((RestoreType & 0x03) << 2);
		TransBlock.gtbDelay 		= Delay;			// 
		TransBlock.gtbTransparency	= TranspFlag ? TransparentColour : 0;
		TransBlock.gtbTerminator 	= 0;

		// This is really sizeof(GIFTRANSBLOCK) but this returns 14 instead of 13
		// as it rounds to the nearest word boundary
		const size_t TransBlockSize = sizeof(WORD) * 1 + sizeof(BYTE) * 6;
		File->write( &TransBlock, TransBlockSize );

		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		return TRUE;
	}
	catch(...)
	{
		// catch our form of a file exception
		TRACE( _T("OutputGIF::OutputGifHeader CC catch handler\n"));

		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		return FALSE;
	}

	ERROR2( FALSE, "Escaped exception clause somehow" );
#else
	return FALSE;
#endif
}	



/********************************************************************************************
>	BOOL OutputGIF::OutputAnimationControl(CCLexFile *File, unsigned short Repeats)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/96
	Inputs:		File - file to output to
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed
	Purpose:	Outputs a Netscape Animation Control block into the GIF file
				NOTE Must be inserted after file header but before image data
********************************************************************************************/
BOOL OutputGIF::OutputAnimationControl(CCLexFile *File, unsigned short Repeats)
{
	ERROR2IF(File==NULL,FALSE,"OutputGIF::OutputAnimationControl File pointer is null");

	BOOL OldThrowingState = File->SetThrowExceptions( TRUE );
	BOOL OldReportingState = File->SetReportErrors( FALSE );
	try
	{
		// Generate the Netscape Extension block
		BYTE Block[19];
		Block[0] = 0x21;	// GIF extension code
		Block[1] = 0xFF;	// Application extension
		Block[2] = 0x0B;	// Length of application string
		memcpy(Block+3, "NETSCAPE2.0", 0x0B);	// Application string
		Block[14] = 0x03;	// Length of data sub-block
		Block[15] = 0x01;	// ??
		Block[16] = Repeats & 0xFF;	// Low byte of repeats
		Block[17] = Repeats >> 8;	// High byte of repeats
		Block[18] = 0;		// Terminator

		File->write(Block, 19);

		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		return TRUE;
	}
	catch(...)
	{
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		return FALSE;
	}

	ERROR2( FALSE, "Escaped exception clause somehow" );
}	

/******************************************************************************************

>	LPBITMAPINFO OutputGIF::GetDestBitmapInfo ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/00
	Inputs:		pPalette - Palette information that we want to copy from
	Returns:	A pointer to the bitmap information structure.
	Purpose:	Access function to DestBitmapInfo.

******************************************************************************************/

LPBITMAPINFO OutputGIF::GetDestBitmapInfo ( void )
{
	return DestBitmapInfo;
}

/******************************************************************************************

>	LPBYTE OutputGIF::GetDestBitmapBits ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/00
	Inputs:		pPalette - Palette information that we want to copy from
	Returns:	A pointer to the bitmap itself.
	Purpose:	Access function to DestBitmapBytes.

******************************************************************************************/

LPBYTE OutputGIF::GetDestBitmapBits ( void )
{
	return DestBitmapBytes;
}

/********************************************************************************************

>	INT32 OutputGIF::GetColourDepth(INT32 NumberOfColours)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/97
	Inputs:		NumberOfColours		The number of colours in the palette of the bitmap
	Outputs:	-
	Returns:	The colour depth
	Purpose:	Converts the number of colours given into the colour depth that is required
				to output that number. This is not restricted to the colour depths of Window's
				DIBs i.e. 8, 4 and 1 but can be any colour depth less than 8 bpps.

********************************************************************************************/

INT32 OutputGIF::GetColourDepth(INT32 NumberOfColours)
{
	INT32 ColourDepth = 8;
	if (NumberOfColours <= 2)
		// Changed by Craig Hamilton 14/9/00.
		ColourDepth = 1;
		// End changed.
	else if (NumberOfColours <= 4)
		ColourDepth = 2;
	else if (NumberOfColours <= 8)
		ColourDepth = 3;
	else if (NumberOfColours <= 16)
		ColourDepth = 4;
	else if (NumberOfColours <= 32)
		ColourDepth = 5;
	else if (NumberOfColours <= 64)
		ColourDepth = 6;
	else if (NumberOfColours <= 128)
		ColourDepth = 7;
	else if (NumberOfColours <= 256)
		ColourDepth = 8;

	return ColourDepth;
}


/********************************************************************************************

>	BOOL OutputGIF::OutputGifImageBits(CCLexFile *File, LPBYTE pBlockStart, BOOL Interlace, BOOL LocalColourTable,
									   BaseCamelotFilter *pFilter = NULL,
									   UINT32 NewWidth = 0, UINT32 NewHeight = 0, UINT32 LeftOffset = 0, UINT32 TopOffset = 0,
									   LPRGBQUAD pDestPalette = NULL, UINT32 PaletteColourDepth = 0, UINT32 NewBitsPerPixel = 0)


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/95
	Inputs:		File				pointer to file to output to
				pBlockStart			actual bitmap data to write out
				Interlace			TRUE if image should be interlaced
				LocalColourTable	True if need to specify a local colour table, 
									False if the global one is all that is required
				pFilter				is an alternative way of handling the progress bar, assume the
									progress bar has been start and just call the IncProgressBarCount in 
									BaseCamelotFilter to do the progress bar update.
									Defaults to NULL i.e. no progress bar.
				NewWidth			The new width of this bitmap, defaults to 0 meaning none
				NewHeight			The new height of this bitmap, defaults to 0 meaning none
				LeftOffset			The offset of this bitmap from the left hand side, defaults to 0
				TopOffset			The offset of this bitmap from the top hand side, defaults to 0
				pDestPalette		The local palette to output
				NumberOfPalColours	The number of colours in this local palette
				NewBitsPerPixel		The new bits per pixel of the bitmap, defaults to 0 meaning none
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Output the actual bits of a bitmap as compressed GIF data. Assumes that a
				header has already been output, possibly using OutputGifHeader. Assumes that
				all the data is present.
				The GIF specifcation allows us to output the next bitmap in an animation such
				that it is a subregion of a previous image. In this case we need to specify a 
				new width and height plus a left and top offset.
	Errors:		Calls SetError on FALSE returns.
	SeeAlso:	OutputGIF::OutputGifHeader; 
	SeeAlo:		OutputGIF::StartFile; OutputGIF::WriteBlock; OutputGIF::TidyUp
	Scope:		static
	
	Note:		If we use the NewWidth and NewHeight input variables then we must ALWAYS
				input the height and width of the bitmap.

********************************************************************************************/
BOOL OutputGIF::OutputGifImageBits(CCLexFile *File, LPBYTE pBlockStart, BOOL Interlace, BOOL LocalColourTable,
								   BaseCamelotFilter *pFilter,
								   UINT32 NewWidth, UINT32 NewHeight, UINT32 LeftOffset, UINT32 TopOffset,
								   LPRGBQUAD pDestPalette, UINT32 PaletteColourDepth, UINT32 NewBitsPerPixel)

{
PORTNOTETRACE("GIFFilter", "Removed use of GIF code");
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(File==NULL,FALSE,"OutputGIF::OutputGifHeader File pointer is null");
	ERROR2IF(pBlockStart==NULL,FALSE,"OutputGIF::OutputGifHeader BitmapInfo pointer is null");

	// Note file in our class variable as used by all the low level routines
	OutputFile = File;
	InterlacingOn = Interlace;

	// Set up our pointer to the alternative way of handling the progress bar
	// If it is the NULL default then do not use.
	pCamFilter = pFilter;

	if (NewWidth > 0 && NewHeight > 0)
	{
		// Set the statics to the new values. If we use this method then we must ALWAYS input
		// the height and width of the bitmap.
		Width = NewWidth;
		Height = NewHeight;
	}

	ERROR3IF(NewBitsPerPixel > 8,"Bad BPP in OutputGifImageBits");
	if (NewBitsPerPixel > 0 && NewBitsPerPixel <= 8)
		BitsPerPixel = NewBitsPerPixel;

	// Must set the exception throwing flag to True and force reporting of errors to False.
	// This means that the caller must report an error if the function returns False.
	// Any calls to CCFile::GotError will now throw a file exception and should fall into
	// the catch handler at the end of the function.
	// Replaces the goto's that handled this before.
	BOOL OldThrowingState = File->SetThrowExceptions( TRUE );
	BOOL OldReportingState = File->SetReportErrors( FALSE );

	try
	{
	    // Write an Image separator
	    File->put(0x2C);

	    // The initial code size
	    if( BitsPerPixel <= 1 )
	    	InitCodeSize = 2;
	    else
	    	InitCodeSize = BitsPerPixel;

		UINT32 ColourDepth = 1;
		
		if (pDestPalette && PaletteColourDepth > 0)
		{
			// Translate the number of colours into a bits per pixel which is not limited to 8, 4 and 1.
			ColourDepth = GetColourDepth(PaletteColourDepth);
			InitCodeSize = ColourDepth;
		}
		else
		{
			// Direct saving a bitmap has this set to null
			//ERROR3IF(DestBitmapInfo == NULL,"Bad DestBitmapInfo in OutputGifImageBits");
			if (DestBitmapInfo)
			{
				ColourDepth = DestBitmapInfo->bmiHeader.biBitCount;
				InitCodeSize = GetColourDepth(DestBitmapInfo->bmiHeader.biClrUsed);
			}
			else
				ColourDepth = BitsPerPixel;
		}

		// Sanity check on the code size
		if (InitCodeSize > 8)
			InitCodeSize = 8;
		if (InitCodeSize < 2)
			InitCodeSize = 2;

		TRACEUSER( "Neville", _T("OutputGifImageBits - Colour depth = %d\n"),ColourDepth);
		
		// Write the Image header
		GIFIMAGEBLOCK ImageHeader;
		ImageHeader.gibLeft 	= (WORD)LeftOffset;	// Left Offset defaults to zero
		ImageHeader.gibTop 		= (WORD)TopOffset;	// Top Offset defaults to zero
		ImageHeader.gibWidth 	= (WORD)Width;
		ImageHeader.gibDepth 	= (WORD)Height;
		// Flags consists of:-
		//	Local Color Table Flag        1 Bit
		//	Interlace Flag                1 Bit
		//	Sort Flag                     1 Bit
		//	Reserved                      2 Bits
		//	Size of Local Color Table     3 Bits
		ImageHeader.gibFlags 	= (LocalColourTable ? 0x80 : 0) | (Interlace ? 0x40 : 0) | 
												(LocalColourTable ? (ColourDepth - 1) : 0);
		File->write(&ImageHeader, 9);

		// Save the local colour table - it follows straight on
		if (LocalColourTable)
		{
			if (pDestPalette && PaletteColourDepth > 0)
			{
				TRACEUSER( "Neville", _T("OutputGifImageBits - Output local palette table (Extended)\n"));
				// Somebody supplied a palette in RGBQUAD form for us to output so do so.
				// write it to disk in GIFRGBTRIPLE format
				GIFRGBTRIPLE TempRGB;
				for (INT32 loop = 0; loop < ((1 << ColourDepth)); loop++)
				{
					TempRGB.grgbtRed 	= pDestPalette->rgbRed;
					TempRGB.grgbtGreen 	= pDestPalette->rgbGreen;
					TempRGB.grgbtBlue 	= pDestPalette->rgbBlue;
					pDestPalette++;

					File->write( &TempRGB, sizeof(GIFRGBTRIPLE) );
				}
			}
			else if (OutputPalette && ColourDepth > 0)
			{
				TRACEUSER( "Neville", _T("OutputGifImageBits - Output local palette table\n"));
				//ERROR3IF(OutputPalette == NULL,"Bad OutputPalette in OutputGifImageBits")
				// Use the palette stored inside the DestBitmapInfo and OutputPalette class variables
				GIFRGBTRIPLE TempRGB;
				for (INT32 loop = 0; loop < ((1 << ColourDepth)); loop++)
				{
					TempRGB.grgbtRed	= OutputPalette->palPalEntry[loop].peRed;
					TempRGB.grgbtGreen	= OutputPalette->palPalEntry[loop].peGreen;
					TempRGB.grgbtBlue	= OutputPalette->palPalEntry[loop].peBlue;

					File->write( &TempRGB, sizeof(GIFRGBTRIPLE) );
				}
			}
			else
				ERROR3("Trying to output local palette when none specified!");
		}

	    // Write out the initial code size
	    File->put( InitCodeSize );

		// It is now ready to go and put that data onto the disc
		// so wait for the bitmap data to be prepared

	    // Go and actually compress the data
	    CompressBlockToFile( pBlockStart, InitCodeSize + 1 );

	    // Write out a Zero-length packet (to end the series)
	    File->put( 0 );

		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		return TRUE;
	}

	catch(...)
	{
		// catch our form of a file exception
		TRACE( _T("OutputGIF::OutputGifHeader CC catch handler\n"));

		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		return FALSE;
	}

	ERROR2( FALSE, "Escaped exception clause somehow" );
#else
	return FALSE;
#endif
}	



/********************************************************************************************
>	BOOL OutputGIF::OutputGifTerminator(CCLexFile *File)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/95
	Inputs:		File		pointer to file to output to
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed
	Purpose:	Outputs the GIF terminator sequence
********************************************************************************************/

BOOL OutputGIF::OutputGifTerminator(CCLexFile *File)
{
	ERROR2IF(File==NULL,FALSE,"OutputGIF::OutputGifTerminator File pointer is null");

	BOOL OldThrowingState = File->SetThrowExceptions( TRUE );
	BOOL OldReportingState = File->SetReportErrors( FALSE );

	try
	{
	    // Write the GIF file terminator
	    File->put( ';' );

		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		// er, we seem to have finished OK so say so
		return TRUE;
	}

	catch(...)
	{
		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		return FALSE;
	}

	ERROR2( FALSE, "Escaped exception clause somehow" );
}





/********************************************************************************************

>	BOOL OutputGIF::WriteBlock( UINT32 YPos, UINT32 Height, LPBYTE BlockStart, UINT32 InputBPP = 32,
							 	INT32 ProgressOffset = 0)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
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

BOOL OutputGIF::WriteBlock( UINT32 YPos, UINT32 Height, LPBYTE BlockStart, UINT32 InputBPP,
							INT32 ProgressOffset)
{
	ERROR2IF(DestBitmapInfo == NULL, FALSE,"OutputGIF::WriteBlock destination bitmap info null");
	ERROR2IF(DestBitmapBytes == NULL, FALSE,"OutputGIF::WriteBlock destination bitmap bits null");
	ERROR2IF(pNextStrip == NULL, FALSE,"OutputGIF::WriteBlock next strip pointer is null");
	
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

			// Copy this block to our destination bitmap
			// pNextStrip should be pointing at the next place to copy the data to
			memcpy(pNextStrip, Buffer, ThisBit * DestWidth);

			Data += SourceWidth;
			h -= ThisBit;
			pNextStrip += ThisBit * DestWidth;
			
			// now update the progress display, started with CurrentExportSize
			// CurrentExport size is now the point to go from in the export
			count++;
			//ContinueSlowJob( (INT32)(ProgressOffset + count) );
			//ContinueSlowJob( (INT32)(100*count/(Height)) );
		}
	}
	else if ( ConvertFn && Buffer )
	{
		// Write via conversion function
		// 24 bpp convert
		ERROR2(FALSE,"OutputGIF::WriteBlock trying to write using ConvertFunction");
	}
	else
	{
		ERROR2(FALSE,"OutputGIF::WriteBlock trying to write in one go");
	}

	// If present, get rid of our export function
	if (DoConvert)
	{
		delete DoConvert;
	 	DoConvert = NULL;
	}

	CCFree( Buffer );
	Buffer = NULL;

	HeightWritten += Height;						// remember we wrote this lot

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

BOOL OutputGIF::TidyUp()
{
	// Free up the DIB that we have just created
	if (DestBitmapInfo && DestBitmapBytes)
	{
		FreeDIB( DestBitmapInfo, DestBitmapBytes );
		DestBitmapInfo = NULL;
		DestBitmapBytes = NULL;	
	}

	// Call the baseclass version to do its stuff
	const BOOL ok = OutputDIB::TidyUp();

	return ok;
}


/********************************************************************************************
 *
 *  GIFCOMPR.C       - GIF Image compression routines
 *
 *  Lempel-Ziv compression based on 'compress'.  GIF modifications by
 *  David Rowley (mgardi@watdcsu.waterloo.edu)
 *
 ********************************************************************************************/

/*
 * GIF Image compression - modified 'compress'
 *
 * Based on: compress.c - File compression ala IEEE Computer, June 1984.
 *
 * By Authors:  Spencer W. Thomas       (decvax!harpo!utah-cs!utah-gr!thomas)
 *              Jim McKie               (decvax!mcvax!jim)
 *              Steve Davies            (decvax!vax135!petsd!peora!srd)
 *              Ken Turkowski           (decvax!decwrl!turtlevax!ken)
 *              James A. Woods          (decvax!ihnp4!ames!jaw)
 *              Joe Orost               (decvax!vax135!petsd!joe)
 *
 * To save much memory, we overlay the table used by compress() with those
 * used by decompress().  The tab_prefix table is the same size and type
 * as the codetab.  The tab_suffix table needs 2**GIFBITS characters.  We
 * get this from the beginning of htab.  The output stack uses the rest
 * of htab, and contains characters.  There is plenty of room for any
 * possible stack (stack used to be 8000 characters).
 *
 * Algorithm:  use open addressing double hashing (no chaining) on the
 * prefix code / next character combination.  We do a variant of Knuth's
 * algorithm D (vol. 3, sec. 6.4) along with G. Knott's relatively-prime
 * secondary probe.  Here, the modular division first probe is gives way
 * to a faster exclusive-or manipulation.  Also do block compression with
 * an adaptive reset, whereby the code table is cleared when the compression
 * ratio decreases, but after the table fills.  The variable-length output
 * codes are re-sized at this point, and a special CLEAR code is generated
 * for the decompressor.  Late addition:  construct the table according to
 * file size for noticeable speed improvement on small files.  Please direct
 * questions about this implementation to ames!jaw.
 */

/********************************************************************************************

> 	void OutputGIF::CompressBlockToFile( LPBYTE Buffer, INT32 init_bits )

	Inputs:		Buffer		pointer to the bits to be compressed
				init_bits	initial number of bits
	Outputs:	-
	Purpose:	Compress the specified bitmap out to the file specified in the class variable
				OutputFile.
				Assumes that variables such as Width, Height, WidthOfLine have been set up
				by the caller from the bitmap we are about to compress. This is done by the
				usual calling function of this OutputGifBits and OutputGifHeader.
	SeeAlso:	OutputGIF::OutputGifHeader; OutputGIF::OutputGifBits;

********************************************************************************************/

void OutputGIF::CompressBlockToFile( LPBYTE Buffer, INT32 init_bits )
{
    // These used to have register before them
    INT32 		fcode;
    code_int 	i; 	//= 0;
    INT32 		c;
    code_int 	ent;
    code_int 	disp;
    code_int 	hsize_reg;
    INT32 		hshift;

	// Note the passed in buffer and size in our class variables
	DataBuffer 	= Buffer;

    // Calculate number of bits we are expecting
    CountDown 	= (INT32)Width * (INT32)Height;

	// Work out the word/byte rounded line width rather than the pixel width
	WidthOfLine = DIBUtil::ScanlineSize( Width, BitsPerPixel );

    // Set up the globals:  g_init_bits - initial number of bits
    g_init_bits = init_bits;

    // Set up the necessary values
    offset 		= 0;
    out_count 	= 0;
    clear_flg 	= 0;
    in_count 	= 1;
    maxcode 	= MAXCODE(n_bits = init_bits);
    ClearCode 	= (1 << (init_bits - 1));
    EOFCode 	= ClearCode + 1;
    free_ent 	= ClearCode + 2;

    // Indicate which pass we are on (if interlace)
    Pass 		= 0;

    // Set up the current x and y position
    curx 		= 0;
    cury 		= 0;
	CurrentLine	= 0;

	cur_accum 	= 0;
	cur_bits 	= 0;

    CharacterInit();	// a_count = 0;

    ent = GIFNextPixel();

    hshift = 0;
    for ( fcode = (INT32) hsize;  fcode < 65536L; fcode *= 2L )
        ++hshift;
    hshift = 8 - hshift;                // set hash code range bound

	// clear hash table
    hsize_reg = hsize;
    ClearHashTable( (count_int) hsize_reg);	

    OutputCode( (code_int)ClearCode );

    while ( (c = GIFNextPixel()) != EOF )
    {
        ++in_count;

        fcode = (INT32) (((INT32) c << maxbits) + ent);
        i = (((code_int)c << hshift) ^ ent);    // xor hashing

        if ( HashTabOf(i) == fcode )
        {
            ent = CodeTabOf(i);
            continue;
        }
        else if ( (INT32)HashTabOf(i) < 0 )      // empty slot
            goto nomatch;

        disp = hsize_reg - i;           		// secondary hash (after G. Knott)
        if ( i == 0 )
            disp = 1;
			
probe:
        if ( (i -= disp) < 0 )
            i += hsize_reg;

        if ( HashTabOf(i) == fcode )
        {
            ent = CodeTabOf(i);
            continue;
        }
        if ( (INT32)HashTabOf(i) > 0 )
            goto probe;
			
nomatch:
        OutputCode( (code_int) ent );
        ++out_count;
        ent = c;
        if ( free_ent < maxmaxcode )
        {
            CodeTabOf(i) = free_ent++; /* code -> hashtable */
            HashTabOf(i) = fcode;
        }
        else
            ClearBlock();
    }
    // Put out the final code.
    OutputCode( (code_int) ent );
    ++out_count;
    OutputCode( (code_int) EOFCode );
}

/********************************************************************************************

> 	void 	OutputGIF::OutputCode( code_int code )

	Inputs:		code	A n_bits-bit integer. If == -1, then EOF.
						This assumes that n_bits =< (INT32)wordsize - 1.
 	Outputs:	Outputs code to the file.
 	Purpose:	Output the given code.
 				This assumes that chars are 8 bits long.
       			It maintains a GIFBITS character INT32 buffer (so that 8 codes will fit in it
       			exactly). When the buffer fills up empty it and start over.

********************************************************************************************/

void OutputGIF::OutputCode( code_int code )
{
PORTNOTETRACE("GIFFilter", "Removed use of GIF code");
#ifndef EXCLUDE_FROM_XARALX
    cur_accum &= masks[ cur_bits ];

    if( cur_bits > 0 )
        cur_accum |= ((INT32)code << cur_bits);
    else
        cur_accum = code;

    cur_bits += n_bits;

    while( cur_bits >= 8 )
    {
        OutputCharacter( (UINT32)(cur_accum & 0xff) );
        cur_accum >>= 8;
        cur_bits -= 8;
    }

	// If the next entry is going to be too big for the code size, then increase it,
	// if possible.
	if ( free_ent > maxcode || clear_flg )
	{
        if( clear_flg )
        {
            maxcode = MAXCODE(n_bits = g_init_bits);
            clear_flg = 0;
        }
        else
        {
            ++n_bits;
            if ( n_bits == maxbits )
                maxcode = maxmaxcode;
            else
                maxcode = MAXCODE(n_bits);
        }
    }

    if( code == EOFCode )
    {
        // At EOF, write the rest of the buffer.
        while( cur_bits > 0 )
        {
            OutputCharacter( (UINT32)(cur_accum & 0xff) );
            cur_accum >>= 8;
            cur_bits -= 8;
        }

        FlushCharacters();

        OutputFile->flush();
    }
#endif
}

/********************************************************************************************

>	void OutputGIF::ClearBlock()

	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Clear out the hash table for block compress
	
********************************************************************************************/

void OutputGIF::ClearBlock()
{
    ClearHashTable( (count_int) hsize );
    free_ent = ClearCode + 2;
    clear_flg = 1;

    OutputCode( (code_int)ClearCode );
}

/********************************************************************************************

>	void OutputGIF::ClearHashTable(count_int hsize)

	Inputs:		hsize	size of the maximum code size
	Outputs:	-
	Returns:	-
	Purpose:	reset the code table back to the default values

********************************************************************************************/

void OutputGIF::ClearHashTable(count_int hsize)          
{
    // These used to have register in front of them
    count_int *htab_p = htab + hsize;
    INT32 i;
    INT32 m1 = -1;

    i = hsize - 16;
    do
    {
        *(htab_p - 16) = m1;
        *(htab_p - 15) = m1;
        *(htab_p - 14) = m1;
        *(htab_p - 13) = m1;
        *(htab_p - 12) = m1;
        *(htab_p - 11) = m1;
        *(htab_p - 10) = m1;
        *(htab_p - 9) = m1;
        *(htab_p - 8) = m1;
        *(htab_p - 7) = m1;
        *(htab_p - 6) = m1;
        *(htab_p - 5) = m1;
        *(htab_p - 4) = m1;
        *(htab_p - 3) = m1;
        *(htab_p - 2) = m1;
        *(htab_p - 1) = m1;
        htab_p -= 16;
    } while ((i -= 16) >= 0);

    for ( i += 16; i > 0; --i )
    	*--htab_p = m1;
}

/********************************************************************************************
 *
 * GIF Specific routines
 *
 ********************************************************************************************/

/********************************************************************************************

>	void OutputGIF::CharacterInit() 

	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Set up the 'byte output' routine.
	
********************************************************************************************/

void OutputGIF::CharacterInit()
{
    a_count = 0;		// set accumlator to zero
}

/********************************************************************************************

>	void OutputGIF::OutputCharacter( INT32 c )

	Inputs:		c	character to add to the output stream 
	Outputs:	-
	Returns:	-
	Purpose:	Add a character to the end of the current packet, and if it is 254
				characters, flush the packet to disk.
				
********************************************************************************************/

void OutputGIF::OutputCharacter( INT32 c )
{
    accum[ a_count++ ] = c;
    if( a_count >= 255 )		// was 254 until 16/4/97 - Gavin suggested tweak
        FlushCharacters();
}

/********************************************************************************************

>	void OutputGIF::FlushCharacters()

	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Flush the packet to disk, and reset the accumulator (class variable a_count).

********************************************************************************************/

void OutputGIF::FlushCharacters()
{
    if( a_count > 0 )
    {
        OutputFile->put( a_count );
        OutputFile->write( &accum, a_count);
        a_count = 0;
    }
}

/********************************************************************************************

>	void OutputGIF::init_statics()

	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-

********************************************************************************************/

/*
void OutputGIF::init_statics()
{
*/
	// Some of these are properly initialized later. What I'm doing	here is making sure
	// code that depends on C's initialization of statics doesn't break when the code
	// gets called more	than once.
/*
	curx = 0;
	cury = 0;
	CountDown = 0;
	Pass = 0;
	a_count = 0;
	cur_accum = 0;
	cur_bits = 0;
	g_init_bits = 0;
	ClearCode = 0;
	EOFCode = 0;
	free_ent = 0;
	clear_flg = 0;
	offset = 0;
	in_count = 1;
	out_count = 0;	
	n_bits = 0;
	maxcode = 0;
} */

/********************************************************************************************

> 	INT32 OutputGIF::GIFNextPixel()

	Inputs:		-
	Outputs:	-
	Returns:	the value of the specified pixel or 0 if badly specified
	Purpose:	Return the next pixel from the image using the class variables curx, cury
				as the current positions and afterwards moving them to the next positions.
				Takes into account if interlacing is on.

********************************************************************************************/

INT32 OutputGIF::GIFNextPixel()
{
    INT32 Pixel;

    // First, check whether we have done all the pixels in the image
    if( CountDown == 0 )
    	return EOF;

	// Decrement our pixel count
    --CountDown;

    //r = GetPixel(curx, cury);
	if ( !(((cury < 0) || (cury >= Height)) || ((curx < 0) || (curx >= Width))) && DataBuffer )
	{
		// Our DIBs are the wrong way up so we must output the data from the last 
		// line of the image and go up to the start
		// -1 as height = 1 .. Height whereas y goes from 0 .. Height - 1
		// Use the word/byte rounded line width rather than the pixel width
		// always compresses a byte regardless of colour depth. 
		switch (BitsPerPixel)
		{
			case 8:
				{
					// If 8 bpp then just use the whole byte straight
					LPBYTE pData = DataBuffer + curx + ((Height - 1 - cury)  * WidthOfLine);
					Pixel = *(pData);
				}
				break;

			case 4:
				{
					// 4bpp so we must get the high or low nibble. This will be dependent on
					// whether we are on an odd or even pixel. So test the LSBit of the curx,
					// if set we will be odd. Only move onto next byte every other pixel hence
					// curx/2.
					LPBYTE pData = DataBuffer + curx/2 + ((Height - 1 - cury)  * WidthOfLine);
					Pixel = *(pData);
					if (curx & 1)
						Pixel = (Pixel & 0x0F);			// take low nibble 
					else
						Pixel = (Pixel & 0xF0) >> 4;	// take top nibble 
				}
				break;
			case 1:
				{
					// 1bpp - pixels are stored in bits.
					LPBYTE pData = DataBuffer + curx/8 + ((Height - 1 - cury)  * WidthOfLine);
					BYTE Mask = 0x80 >> (curx % 8);
					if (((*pData) & Mask) == 0)
						Pixel = 0;
					else
						Pixel = 1;
				}
				break;
			default:
				ERROR3("Unknown export depth");
				Pixel = 0;	// bad bpp but should not get here
			}
	}
	else
	{
		ERROR3("Something bad has happened");
		Pixel = 0;
	}
	
    // Move the curx, cury to the next pixel on the image taking into account interlacing
    BumpPixel();

	// Check that the pixel is not outside of the allowed range
	// If it is then what do we do with it? Set it 0 for now.
	ERROR3IF(Pixel >= ClearCode,"Trying to output pixel >= ClearCode");
	if (Pixel >= ClearCode)
		Pixel = 0;

    return Pixel;
}

/********************************************************************************************

>	void OutputGIF::BumpPixel()

	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Bump the class variables 'curx' and 'cury' to point to the next pixel
				taking into account interlacing if it is on.

********************************************************************************************/

void OutputGIF::BumpPixel()
{
    // Bump the current X position
    ++curx;

    // If we are at the end of a scan line, set curx back to the beginning
    // If we are interlaced, bump the cury to the appropriate spot,
    // otherwise, just increment it.
    if( curx == Width )
    {
        // Reset x to start of next line
        curx = 0;
		
		CurrentLine++;
		
		// If Interlacing off then just increment the line we are on
        if( !InterlacingOn )
        	++cury;
        else
        {
			// Otherwise get the next interlaced line in the sequence
			// Use the same style of code that we use for import in GIFUtil
			switch (Pass)
			{
				case 0:
				case 1:
					cury += 8;
					break;
				case 2:
					cury += 4;
					break;
				case 3:
 					cury += 2;
					break;
			}

			while (cury >= Height && Pass <= 3)
			{
				++Pass;
				switch (Pass)
				{
					case 1:
						cury = 4;
						break;
					case 2:
						cury = 2;
						break;
					case 3:
						cury = 1;
						break;
					default:
						cury = 0;
						break;
				}
			}
        }
	
		// Show a progress bar
		if (pCamFilter == NULL)
		{
			// Andy, 13-12-00: We don't want another progress bar if we're exporting
			//ContinueSlowJob( (INT32)(100 * CurrentLine/(Height)) );
		}
		else
			pCamFilter->IncProgressBarCount(1);
    }
}

/********************************************************************************************

>	BOOL OutputGIF::BoundsSafe(INT32 x, INT32 y)

	Inputs:		x	horizontal pixel offset into the image
				y	vertical pixel(line) offset into the image 
	Outputs:	-
	Returns:	True if specified pixel bounds ok otherwise False
	Purpose:	Check that the pixel we are about to get is within the bounds of the image
	
********************************************************************************************/

//BOOL OutputGIF::BoundsSafe(INT32 x, INT32 y)
//{
//	return ( !(((y < 0) || (y >= Height)) || ((x < 0) || (x >= Width))) );
//}

/********************************************************************************************

>	INT32 OutputGIF::GetPixel(INT32 x, INT32 y)

	Inputs:		x	horizontal pixel offset into the image
				y	vertical pixel(line) offset into the image 
	Outputs:	-
	Returns:	the value of teh specified pixel or 0 if badly specified
	Purpose:	Get the specified pixel from the image buffer checking that it is within
				the image bounds otherwise return 0.

********************************************************************************************/

//INT32 OutputGIF::GetPixel(INT32 x, INT32 y)
//{
//	if (BoundsSafe(x, y) && DataBuffer)
//	{
//		// Our DIBs are the wrong way up so we must output the data from the last 
//		// line of the image and go up to the start
//		// -1 as height = 1 .. Height whereas y goes from 0 .. Height - 1
//		LPBYTE pData = DataBuffer + x + ((Height - 1 - y)  * Width);
//		//return pixels[x][y];
//		// always compresses a byte regardless of colour depth. 
//		return *(pData);
//	}
//	else
//		return 0;
//}


