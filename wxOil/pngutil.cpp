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

// Contains useful routines for compressing a bitmap out to a PNG format file and
// routines to load that file back in.

#include "camtypes.h"
#include "pngutil.h"
#include "pngprgrs.h"

//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "camfiltr.h"		// BaseCamelotFilter - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h"		// _R(IDS_OUT_OF_MEMORY)
//#include "accures.h"		// _R(IDW_CANCELLEDBMPIMPORT)
//#include "andy.h"			// _R(IDE_FORMATNOTSUPPORTED)
//#include "filtrres.h"		// _R(IDS_UNKNOWN_PNG_ERROR)

// Header files supplied by PNG library
#include "png.h"

// Replacements for some libpng functions
#include "pngfuncs.h"

#define	new	CAM_DEBUG_NEW

// Create a global pointer to the instance of the progress
// bar created in the read function - needed as it is not
// possible to give a C style pointer to a function in a C++
// class
PORTNOTE("other","png_progress_bar_read - removed progressbar")
#ifndef EXCLUDE_FROM_XARALX
class PNGProgressBar * png_progress_bar_read;
#endif

/******************************************************************* 

>       void camelot_png_read_row_callback
			(png_structp png_ptr, png_uint_32 row_number, INT32 pass)

        Author: Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
        Created: 29/8/2000
        Inputs: png_ptr, row, pass
        Outputs: Status bar update
        Returns: void
        Purpose: Pointer to this function given to libpng to be
				 called when a row has been read.
*******************************************************************/

void camelot_png_read_row_callback
			(png_structp png_ptr, png_uint_32 row_number, INT32 pass)
{
PORTNOTE("other","camelot_png_read_row_callback - removed progressbar")
#ifndef EXCLUDE_FROM_XARALX
	png_progress_bar_read->UpdateCallback(png_ptr, row_number, pass);
#endif
}

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_MEMDUMP(PNGUtil, CC_CLASS_MEMDUMP)

// define the statics that we require
//BOOL 	PNGUtil::Interlace		= FALSE;	// Use interlace or not
INT32		PNGUtil::Transparent	= -1;		// colour or -1 = no transparency
CCLexFile* PNGUtil::pFile			= NULL;		// The CCLexFile class that we are using

/********************************************************************************************

>	PNGUtil::PNGUtil()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/05/96
	Purpose:	Default constructor for the class. 
	SeeAlso:	

********************************************************************************************/

PNGUtil::PNGUtil()
{
}

/********************************************************************************************
>	PNGUtil::~PNGUtil()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/05/96
	Purpose:	Default destructor for the class.
	SeeAlso:	

********************************************************************************************/

PNGUtil::~PNGUtil()
{
}

/********************************************************************************************

>	static BOOL PNGUtil::ReadFromFile( CCLexFile *File, LPBITMAPINFO *Info, LPBYTE *Bits,
									   INT32 *TransColour, 
									   String_64 *ProgressString = NULL,
									   BaseCamelotFilter *pFilter = NULL )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/05/96
	Inputs:		File			A opened CCLexFile that can be read from. It should be positioned at the
								start. Caller is responsible for closing it. The file needs to be in
								Binary mode.
				ProgressString	allows the user to specify whether they require a progress hourglass or 
								not. If NULL then none is shown, otherwise an progress bar is shown
								using the text supplied. Defaults to NULL i.e. no progress bar.
				pFilter			is an alternative way of handling the progress bar, assume the
								progress bar has been start and just call the IncProgressBarCount in 
								BaseCamelotFilter to do the progress bar update.
								Defaults to NULL i.e. no progress bar.
	Outputs:	Info points to a new LPBITMAPINFO struct and Bits points to the bytes.
				These can be freed up with FreeDIB.
				TransColour is either -1 == none or equal to the transparency colour found
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Reads a .PNG file into memory decompressing it as it goes.
				***Errors on 16-bit builds***
				A progress hourglass can be shown if required.
	Errors:		Calls SetError on FALSE returns.
	Scope:		Static, Public
	SeeAlso:	DIBUtil::ReadFromFile; AccusoftFilters::ReadFromFile;

********************************************************************************************/

BOOL PNGUtil::ReadFromFile( CCLexFile *File, LPBITMAPINFO *Info, LPBYTE *Bits,
							INT32 *TransColour, 
							String_64 *ProgressString,
							BaseCamelotFilter *pFilter)
{
	TRACEUSER( "Jonathan", _T("PNG read: Start\n"));
	*Info = NULL;		// in case of early exit
	*Bits = NULL;
	pFile = File;
	Transparent 	= -1;
	*TransColour 	= -1;	// in case of early exit set to none
	
	UINT32 sig_read = 0;
	png_uint_32 width, height;
	INT32 bit_depth, color_type, interlace_type;
	
   	BOOL OldThrowingState = File->SetThrowExceptions( TRUE );
	BOOL OldReportingState = File->SetReportErrors( FALSE );
	
	png_structp png_ptr		= 0;	// Must be zero to avoid bad free in case of exception
	png_infop info_ptr		= 0;	// Must be zero to avoid bad free in case of exception

	png_bytepp ppbRowPointers= 0;	// Must be zero to avoid bad free in case of exception

	try {
	/* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library.  REQUIRED
		*/
		//   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		//      png_voidp user_error_ptr, user_error_fn, user_warning_fn);
		png_ptr = png_create_read_struct_2(
			PNG_LIBPNG_VER_STRING,	// Version of libpng
			0,						// Optional pointer to be sent with errors
			camelot_png_error,		// Function called in case of error
			camelot_png_warning,		// Function called for warnings
			0,						// Optional pointer to be sent with mem ops
			camelot_png_malloc,		// Function called to alloc memory
			camelot_png_free			// Function called to free memory
			);
		
		if (png_ptr == NULL)
			File->GotError( _R(IDS_OUT_OF_MEMORY) );

		/* Allocate/initialize the memory for image information.  REQUIRED. */
		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL)
		{
			png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
			File->GotError( _R(IDS_OUT_OF_MEMORY) );
		}
		
		/* Set up the input control if you are using standard C streams */
		iostream* pFStream = File->GetIOFile();
		// Should use our own IO functions eg.
		png_set_read_fn(png_ptr, pFStream, camelot_png_read_data);
		//   png_init_io(png_ptr, pFStream);
		
		/* If we have already read some of the signature */
		// Not sure about this - JP
		png_set_sig_bytes(png_ptr, sig_read);
		
		/* The call to png_read_info() gives us all of the information from the
		* PNG file before the first IDAT (image data chunk).  REQUIRED */
		png_read_info(png_ptr, info_ptr);
		
		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
			&interlace_type, NULL, NULL);
		
		TRACEUSER( "Jonathan", _T("PNG read: Start transforms: %d channels of %d bits\n"),
			png_get_channels(png_ptr, info_ptr),
			png_get_bit_depth(png_ptr, info_ptr));
		
		/* tell libpng to strip 16 bit/color files down to 8 bits/color */
		png_set_strip_16(png_ptr);
		
		/* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
		/* JP - Changed so just 2 bit images are expanded */
		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth == 2)
			png_set_expand(png_ptr);
		
		/* Expand paletted images to the full 8 bits from 2 bits/pixel */
		if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 2)
			png_set_expand(png_ptr);
		
		if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png_ptr);

			/* Note that screen gamma is the display_exponent, which includes
		* the CRT_exponent and any correction for viewing conditions */
		/* A good guess for a PC monitors in a dimly lit room is 2.2 */
		const double screen_gamma = 2.2;  
		
		png_set_invert_alpha(png_ptr);
		
		INT32 intent;
		
		if (png_get_sRGB(png_ptr, info_ptr, &intent))
			png_set_gamma(png_ptr, screen_gamma, 0.45455);
		else
		{
			double image_gamma;
			if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
				png_set_gamma(png_ptr, screen_gamma, image_gamma);
			else
				png_set_gamma(png_ptr, screen_gamma, 0.45455);
		}
		
#if defined(__WXGTK__)
		/* flip the RGB pixels to BGR (or RGBA to BGRA) */
		if (color_type & PNG_COLOR_MASK_COLOR)
			png_set_bgr(png_ptr);
#endif

		/* Scan the palletes */
		png_colorp pPalette;
		INT32 num_palette;
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE))
			png_get_PLTE(png_ptr, info_ptr, &pPalette, &num_palette);
		
//		TRACEUSER( "Jonathan", _T("PNG read: Number of palette colours = %d\n"), num_palette);
		
		png_bytep pTrans;
		INT32 num_trans = 0; // if the 'if' statement is falue 0 is the correct value
		png_color_16p trans_values;
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_get_tRNS(png_ptr, info_ptr, &pTrans, &num_trans, &trans_values);
		
		TRACEUSER( "Jonathan", _T("PNG read: Number of transparent colours = %d\n"), num_trans);
		
		
		/* Take a look at the transparency list and see it there is only one
		colour which is not opaque and that that colour is fully transparent
		(so the exported bitmap can have one-bit transparency).  If this
		condition is not meet, convert the bitmap up to full alpha transparency
		(should really give a bitmap that has paletted RGBA but the interface
		to the PNG stuff appears to limit our options).
		
		Warning: If the PNG reading code is asked for 256 colour with a
		transparent colour, it must do this, otherwise the export dialog
		will go wrong when it re-imports the file for preview as it can not find
		a palette. */
		
		if (
			png_get_bit_depth(png_ptr, info_ptr) * png_get_channels(png_ptr, info_ptr) <= 8 
			&& png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_PALETTE
			)
		{
			if (num_trans > 0)
			{
				// See if the palette has the format
				//		Opaque (=256)
				//		Opaque (=256)
				//		Opaque (=256)
				//		....
				//		Fully transparent (=0)
				//	so we can use a one bit transparency paletted RGB image
				INT32 i = 0;
				png_byte * pCurrentTransEntry = pTrans;
				
				// Scan throught opaque entrys
				while (i <= num_trans && pCurrentTransEntry[i] == 255)
					i++;
				
				// This looks like a good bet for transparent colour
				if (pCurrentTransEntry[i] == 0)
					*TransColour = i;
				
				// Check any entrys after this to make sure they are not transparent
				if (i < num_trans)
					i++;
				
				while (i <= num_trans && pCurrentTransEntry[i] == 255)
					i++;
				
				// Check we got to the end of the palette without finding any other
				// transparent colours
				if (i != num_trans)
				{
					// There are other transparent colours so convert to RGBA
					*TransColour = -1;
					
					// Expand paletted or RGB images with transparency to full alpha channels
					// so the data will be available as RGBA quartets.
					if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
						png_set_expand(png_ptr);
					TRACEUSER( "Jonathan", _T("PNG read: Transparency: Converted to RGBA as there where multiple transparent colours\n"));
				}
				else
				{
					TRACEUSER( "Jonathan", _T("PNG read: Transparency: Only one transparent colour so keep the images as paletted\n"));
				}
			}
			else
			{
				TRACEUSER( "Jonathan", _T("PNG read: Transparency: No work needed as no transparency\n"));
			}
		}
		else
		{
			TRACEUSER( "Jonathan", _T("PNG read: Transparency: No work needed as no palette\n"));
		}
		
		TRACEUSER( "Jonathan", _T("PNG read: transparent colour = %d\n"), *TransColour);

		Transparent = *TransColour;

		/* Optional call to gamma correct and add the background to the palette
		* and update info structure.  REQUIRED if you are expecting libpng to
		* update the palette for you (ie you selected such a transform above). */
		png_read_update_info(png_ptr, info_ptr);
		
		/* Turn on interlace handling.  REQUIRED if you are not using
		* png_read_image().  To see how to handle interlacing passes,
		* see the png_read_row() method below: */
		//   INT32 number_passes = png_set_interlace_handling(png_ptr);
		
		
		TRACEUSER( "Jonathan", _T("PNG read: End transforms: %d channels of %d bits\n"),
			png_get_channels(png_ptr, info_ptr),
			png_get_bit_depth(png_ptr, info_ptr));
		
		/* Allocate the memory to hold the image using the fields of info_ptr. */
		INT32 bits_per_pixel = png_get_bit_depth(png_ptr, info_ptr) * png_get_channels(png_ptr, info_ptr);
		if (bits_per_pixel == 1 || bits_per_pixel == 4 || bits_per_pixel == 8 ||  bits_per_pixel == 16 || bits_per_pixel == 24 || bits_per_pixel == 32)
			*Info = AllocDIB(width, height, png_get_bit_depth(png_ptr, info_ptr) * png_get_channels(png_ptr, info_ptr), Bits, NULL);
		else
			// This should never happen!
			File->GotError( _R(IDS_UNKNOWN_PNG_ERROR) );
		
		if (*Info == NULL || *Bits == NULL)
			File->GotError( _R(IDS_OUT_OF_MEMORY) );
		
		/* Set the bitmap DPI */
		png_uint_32 x_res, y_res;
		INT32 unit_type;
		png_get_pHYs(png_ptr, info_ptr, &x_res, &y_res, &unit_type);
		TRACEUSER( "Jonathan", _T("PNG read: X dpi: %d, Y dpi %d, DPI type %d\n"),
			x_res, y_res, unit_type);

		// If the unit type is in meters then use it
		if (unit_type == PNG_RESOLUTION_METER)
		{
			(*Info)->bmiHeader.biXPelsPerMeter = x_res;
			(*Info)->bmiHeader.biYPelsPerMeter = y_res;
		}

		// Check if we require a palette if we are on low bpp images.
		// If so then allocate one
		if (
			png_get_bit_depth(png_ptr, info_ptr) * png_get_channels(png_ptr, info_ptr) <= 8 
			&& png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_PALETTE
			)
		{
			// Palette is at info_ptr->palette
			INT32 PaletteSize = 1 << info_ptr->bit_depth;
			// Read in palette into the palette of the DIB
			LPRGBQUAD lpPalette = (*Info)->bmiColors;
			TRACEUSER( "Jonathan", _T("PNG read: allocate palette and copy size %d\n"),PaletteSize);
			
			// Get our function to read the palette from the PNG definition to the one we are
			// going to apply to the bitmap.
			if (
				lpPalette == NULL ||
				!ReadColourMap(num_palette, pPalette, PaletteSize, lpPalette)
				)
			{
				File->GotError( _R(IDS_PNG_ERR_READ_PALETTE) ); // Should be bad palette error
			}

#if defined(__WXGTK__)
			// Convert palettes for Linuxs BGR ordering (would be nice if png_set_bgr
			// did this)
			for( INT32 ord = 0; ord < PaletteSize; ++ord )
			{
				std::swap( lpPalette[ord].rgbBlue, lpPalette[ord].rgbRed );
			}
#endif
		} 
		else if (png_get_bit_depth(png_ptr, info_ptr) * png_get_channels(png_ptr, info_ptr) <= 8 
			&& png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY)
		{
			// We have a greyscale image and so generate a greyscale palette
			// Palette is at info_ptr->palette
			INT32 PaletteSize = 1 << info_ptr->bit_depth;
			TRACEUSER( "Jonathan", _T("PNG read: Greyscale, so set up a greyscale palette for the DIB size %d\n"),PaletteSize);
			// Read in palette into the palette of the DIB
			LPRGBQUAD lpPalette = (*Info)->bmiColors;
			// Get our function to read the palette from the PNG definition to the one we are
			// going to apply to the bitmap.
			if (lpPalette == NULL || !GenerateGreyPalette(PaletteSize, lpPalette))
				File->GotError( _R(IDS_PNG_ERR_READ_PALETTE) ); // Should be bad palette error
		}
		
		/* and allocate memory for an array of row-pointers */
		if ((ppbRowPointers = (png_bytepp) png_malloc(png_ptr, (height) * sizeof(png_bytep))) == NULL)
			File->GotError(_R(IDS_OUT_OF_MEMORY));
		
		/* set the individual row-pointers to point at the correct offsets */
		UINT32 ulRowBytes = png_get_rowbytes(png_ptr, info_ptr);
		
		/* make the row finish on a word boundry */
		if (ulRowBytes % 4 != 0)
			ulRowBytes += 4 - (ulRowBytes % 4);
		
		for (UINT32 i = 0; i < height; i++)
			ppbRowPointers[height - 1 - i] = *Bits + i * ulRowBytes;
		
		bool interlace;
		switch (png_get_interlace_type(png_ptr, info_ptr))
		{
		case PNG_INTERLACE_NONE:
			interlace = false;
			break;

		case PNG_INTERLACE_ADAM7:
			interlace = true;
			break;

		default:
			File->GotError(_R(IDS_PNG_ERROR_BAD_INTERLACE));
			break;
		}

		/* Set up the progress bar */
PORTNOTE("other","PNGUtil::ReadFromFile - removed progressbar")
#ifndef EXCLUDE_FROM_XARALX
		PNGProgressBar ProgressBar(ProgressString, interlace, height);
		png_progress_bar_read = &ProgressBar;
#endif
		png_set_read_status_fn(png_ptr, camelot_png_read_row_callback);

		/* now we can go ahead and just read the whole image */
		png_read_image(png_ptr, ppbRowPointers);
		
#ifndef EXCLUDE_FROM_XARALX
		png_progress_bar_read = 0;
#endif

		png_free(png_ptr, ppbRowPointers);

		/* read rest of file, and get additional chunks in info_ptr - REQUIRED */
		png_read_end(png_ptr, info_ptr);
		
		/* At this point the entire image has been read */
		
		/* clean up after the read, and free any memory allocated - REQUIRED */
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

		png_ptr = NULL;

		/* Must set the exception throwing and reporting flags back to their entry states */
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );
		
		/* Reset the file pointer back to null for safety */
		pFile = NULL;
		
		TRACEUSER( "Jonathan", _T("PNG read: Finshed\n"));
		
		/* that's it */
		return (OK);
   }
   catch( CFileException )
   {
	   // catch our form of a file exception
	   TRACE( _T("PNGUtil::ReadFromFile CC catch handler\n"));
	   
//GAT	png_progress_bar_read = 0;

		if (ppbRowPointers != 0)
			png_free(png_ptr, ppbRowPointers);

		/* clean up after the read, and free any memory allocated */
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

		png_ptr = NULL;
 
	   // Now our Camelot related bits
	   if (*Info != NULL && *Bits != NULL)
		   FreeDIB( *Info, *Bits );							// free any alloced memory
	   *Info = NULL;										// and NULL the pointers
	   *Bits = NULL;
	   
	   // Free up the bit of memory for a palette we grabbed, if present
	   //if (lpGlobalPalette)
	   //{
	   //	CCFree(lpGlobalPalette);
	   //		lpGlobalPalette = NULL;
	   //}
	   
	   // Must set the exception throwing and reporting flags back to their entry states
	   File->SetThrowExceptions( OldThrowingState );
	   File->SetReportErrors( OldReportingState );
	   
	   // Reset the file pointer back to null for safety
	   pFile = NULL;
	   
	   return FALSE;
   }
	   
   ERROR2( FALSE, "Escaped exception clause somehow" );
}


/********************************************************************************************

>	static PNGUtil::SetCCFilePointer(CCLexFile* pNewFile)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/96
	Inputs:		the new CCLexFile pointer to used for exception handling
	Outputs:	-
	Returns:	-
	Purpose:	Public access to the pFile exception handling pointer
	SeeAlso:	DefaultErrorHandler

********************************************************************************************/

BOOL PNGUtil::SetCCFilePointer(CCLexFile* pNewFile)
{
	pFile = pNewFile;
	return TRUE;
}

/********************************************************************************************

>	static CCLexFile* PNGUtil::GetCCFilePointer()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/96
	Inputs:		-
	Outputs:	-
	Returns:	the CCLexFile pointer used for exception handling
	Purpose:	Public access to the pFile exception handling pointer
	SeeAlso:	DefaultErrorHandler

********************************************************************************************/

CCLexFile* PNGUtil::GetCCFilePointer()
{
	return pFile;
}

/********************************************************************************************

>	static void PNGUtil::DefaultErrorHandler(UINT32 MessageID, TCHAR * pMessage = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/96
	Inputs:		The id of the message
				An optional error string 
	Outputs:	-
	Returns:	-
	Purpose:	To handle errors within the PNG code in a nice fashion.
				Assumes a TRY CATCH block is present and a valid CCLexFile is specified as it
				uses the CCLexFile::GotError to throw an exception.
				This should become redundant as we replace the PNG error handling with IDs
	SeeAlso:	CCFile::GotError

********************************************************************************************/

//void PNGUtil::DefaultErrorHandler(UINT32 MessageID, TCHAR * pMessage)
void PNGUtil::DefaultErrorHandler()
{
	// Use the CCLexFile GotError function to do the error handling for us 
	// Assumes TRY CATCH block in use
	if (pFile)
	{
//		if (pMessage)
//			pFile->GotError(MessageID, pMessage);
//		else
//			pFile->GotError(MessageID);
			pFile->GotError(_R(IDS_UNKNOWN_PNG_ERROR));
	}

	// This is the fall back position of oh my god we must stop execution as otherwise things
	// will go pear shaped.
//	ERROR2RAW(MessageID);
	ERROR2RAW(1);

	// Use the C++ exception handling
	//AfxThrowFileException(CFileException::generic, _R(IDS_UNKNOWN_PNG_ERROR));
}


/********************************************************************************************

>	static BOOL PNGUtil::ReadColourMap(INT32 Png_Entries, png_colorp pPNGPalette, INT32 number, LPRGBQUAD lpPalette)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/96
	Inputs:		pPNGPalette		pointer to the palette defined in the PNG data structures
	Outputs:	-
	Returns:	True if worked ok, False otherwise.
	Purpose:	Copy the palette entries across from the PNG palette to the bitmap palette
				in Camelot.
	SeeAlso:	

********************************************************************************************/

BOOL PNGUtil::ReadColourMap(INT32 Png_Entries, png_colorp pPNGPalette, INT32 number, LPRGBQUAD lpPalette)
{
	ERROR2IF(pPNGPalette == NULL,FALSE,"PNGUtil::ReadColourMap no PNG palette present");
	ERROR2IF(lpPalette == NULL,FALSE,"PNGUtil::ReadColourMap no lpPalette present");
	ERROR2IF(Png_Entries <= 0,FALSE,"PNGUtil::ReadColourMap no PNG palette entries present");

	for (INT32 i = 0; i < Png_Entries; i++)
	{
		lpPalette->rgbBlue = pPNGPalette->blue;
		lpPalette->rgbGreen = pPNGPalette->green;
		lpPalette->rgbRed = pPNGPalette->red;
		lpPalette->rgbReserved = 0;
		lpPalette++;
		pPNGPalette++;
	}
	
	return TRUE;
}

/********************************************************************************************

>	static BOOL PNGUtil::GenerateGreyPalette(INT32 PaletteSize, LPRGBQUAD lpPalette)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/96
	Inputs:		PaletteSize		number of palette entries that need generating
				lpPalette		pointer to the palette defined in the DIB
				
	Outputs:	-
	Returns:	True if worked ok, False otherwise.
	Purpose:	Generate a greyscale palette for a greyscale DIB.
	SeeAlso:	

********************************************************************************************/

BOOL PNGUtil::GenerateGreyPalette(INT32 PaletteSize, LPRGBQUAD lpPalette)
{
	ERROR2IF(lpPalette == NULL,FALSE,"PNGUtil::GenerateGreyPalette no lpPalette present");
	ERROR2IF(PaletteSize <= 0,FALSE,"PNGUtil::GenerateGreyPalette no PNG palette entries present");

	// Work out the value we require per step so on a:-
	//	256 colour palette we can have 256 steps of 1
	//	16 colour palette we have 16 steps of 16 (16 * 16 = 256)
	INT32 inc = 255 / (PaletteSize - 1); // Changed so palette has the right range - Jonathan
	INT32 value = 0;
	for (INT32 i = 0; i < PaletteSize; i++ )
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
