// $Id: outptpng.h 662 2006-03-14 21:31:49Z alex $
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

// header for OutputPNG

#ifndef INC_OUTPTPNG
#define	INC_OUTPTPNG

#include "ccfile.h"						// needs FilePos
#include "dibconv.h"					// needs DIBConvert
#include "dibutil.h"					// needs FNPTR_SCANLINE
#include "outptdib.h"	
//#include "outptgif.h"					// TI_GIF_TRANSINTER	
#include <png.h>
//#include "accuflts.h"					// PNG

class BaseCamelotFilter;

/********************************************************************************************

>	class OutputPNG : public OutputDIB

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/96
	Purpose:	Contains functions to write a DIB to a PNG in segments, optionally
				with compression, optionally converting from one depth to another. To use:
				Open a CCFile for write. Call StartFile to write out the header. Call
				WriteBlock to write out each block of the bitmap, starting from low memory.
				When you're done, call TidyUp.
				Similar to OutputDIB and OutputGIF but strangely enough writes the data as
				a PNG instead of a BMP or via the AccusoftFilters. 
	SeeAlso:	OutputDIB; OutputGIF;

********************************************************************************************/

class OutputPNG : public OutputDIB
{
public:

	OutputPNG();

	virtual BOOL StartFile( LPBITMAPINFOHEADER lpHeader, LPLOGPALETTE Palette,
							UINT32 OutputDepth, DWORD CompressionType,
							UINT32 FinalHeight, INT32 ExportSize, UINT32 DitherType);
	virtual BOOL ReStartFile(LOGPALETTE* pNewPal);
	virtual BOOL WriteBlock( UINT32 YPos, UINT32 Height, LPBYTE BlockStart, UINT32 InputBPP = 32,
							 INT32 ProgressOffset = 0);
	virtual BOOL SetUpInfoHeader(const LPBITMAPINFOHEADER lpHeader, const UINT32 OutputDepth,
								 const DWORD CompressionType, const UINT32 LIneWidth, const UINT32 FinalHeight,
								 INT32 *PalSize); 
	virtual BOOL TidyUp();

	// Useful routines for calling to output the GIF data itself
	BOOL OutputPNGHeader(CCLexFile *File, INT32 TransColour, BOOL InterlaceState = 0);
	BOOL OutputPNGHeader(CCLexFile *File, LPBITMAPINFOHEADER pInfo,
						 BOOL InterlaceState, INT32 TransparentColour,
						 LPLOGPALETTE pPalette = NULL, LPRGBQUAD pQuadPalette = NULL);
	BOOL OutputPNGBits(CCLexFile *File, LPBYTE pBlockStart, BOOL OneBlock = TRUE,
					   BaseCamelotFilter *pFilter = NULL);

	virtual LPBITMAPINFO GetDestBitmapInfo ( void );
	virtual LPBYTE GetDestBitmapBits ( void );

private:
	// The buffer to hold the destination bitmap before we actually write it out to file
	LPBITMAPINFO DestBitmapInfo;
	LPBYTE DestBitmapBytes;
	LPBYTE pNextStrip;

	// PNG related items (NOTE: p at end means pointer and hence implied *)
	png_structp png_ptr;
	png_infop info_ptr;

	BOOL CleanUpPngStructures();

	// Some useful variables
	INT32 Width;						// Width of the image
	INT32 Height;						// Height of the image

	BOOL Interlace;					// Use interlace or not
	BOOL Transparent;				// Use transparency or not

	INT32 BitsPerPixel;				// Colour depth required
    INT32 InitCodeSize;				// starting size of the bitmap data to compress

	LPBYTE DataBuffer;				// Start of buffer to compress
	//INT32 DataBufferSize;				// size of the data buffer
	UINT32 WidthOfLine;				// word/byte rounded line width rather than the pixel width
};
#endif // INC_OUTPTPNG
