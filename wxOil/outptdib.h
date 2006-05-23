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

// header for OutputDIB

#ifndef INC_OUTPTDIB
#define	INC_OUTPTDIB

#include "ccfile.h"						// needs FilePos
#include "dibconv.h"					// needs DIBConvert
#include "dibutil.h"					// needs FNPTR_SCANLINE

/********************************************************************************************

>	class OutputDIB : public CC_CLASS_MEMDUMP

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Purpose:	Contains functions to write a DIB to a Windows BMP in segments, optionally
				with compression, optionally converting from one depth to another. To use:
				Open a CCFile for write. Call StartFile to write out the header. Call
				WriteBlock to write out each block of the bitmap, starting from low memory.
				When you're done, call TidyUp.

********************************************************************************************/

class OutputDIB : public CC_CLASS_MEMDUMP
{
public:

	OutputDIB();
	virtual ~OutputDIB() {}

	virtual BOOL StartFile( CCLexFile *File, LPBITMAPINFOHEADER lpHeader, LPLOGPALETTE Palette,
							UINT32 OutputDepth, DWORD CompressionType,
							UINT32 FinalHeight, INT32 ExportSize, BOOL SecondPass = FALSE,
							UINT32 DitherType = XARADITHER_ORDERED_GREY );

	virtual BOOL StartFile ( LPBITMAPINFOHEADER lpHeader,
							 LPLOGPALETTE Palette,
							 UINT32 OutputDepth,
							 DWORD CompressionType,
							 UINT32 FinalHeight,
							 INT32 ExportSize,
							 UINT32 DitherType = XARADITHER_ORDERED_GREY );

	virtual BOOL WriteBlock( UINT32 YPos, UINT32 Height, LPBYTE BlockStart, UINT32 InputBPP = 32,
							 INT32 ProgressOffset = 0);
	BOOL SetUpBlock( size_t *pBufSize, size_t *pChunkHeight, DIBConvert **pDoConvert,
					 FNPTR_SCANLINE *pConvertFn );
	virtual BOOL SetUpInfoHeader(const LPBITMAPINFOHEADER lpHeader, const UINT32 OutputDepth,
								 const DWORD CompressionType, const UINT32 FinalHeight, INT32 *); 
//	BOOL SetUpExport( CCLexFile *File, LPBITMAPINFOHEADER lpHeader, LPLOGPALETTE Palette, 
//					  UINT32 DitherType, UINT32 OutputDepth,
//					  DWORD CompressionType, UINT32, LPBYTE,
//					  UINT32 StripSize, INT32 ExportSize, BOOL Forward);
	static BOOL GetNextBlock( UINT32 YPos, UINT32 Height, LPBYTE BlockStart,
							  LPBYTE *Buffer, INT32 *CurrentBlockSize,
							  INT32 *CurrentStartYPos, UINT32 *WrittenSoFar);
	static BOOL SetUpForNextStrip(LPBYTE StartOfBytes, UINT32 StripHeight);
	virtual BOOL TidyUp();

	// Some useful palette fixing functions
	static void PokePaletteEntry(LPLOGPALETTE Palette, INT32 *index,
						 		 BYTE red, BYTE green, BYTE blue);
	static void FixBlackAndWhitePalette(LPLOGPALETTE Palette);
	static void Fix16ColourPalette(LPLOGPALETTE Palette);

	void AlterExportPalette( LPLOGPALETTE pPalette );

	virtual LPBITMAPINFO GetDestBitmapInfo ( void );
	virtual LPBYTE GetDestBitmapBits ( void );

protected:

	UINT32 SourceBitmapDepth;					// depth of source bitmap
	FilePos StartPos;						// where in the file the BMP starts
	CCLexFile *OutputFile;						// file we are writing to
	static BITMAPINFOHEADER BitmapInfo;		// copy of file header (includes output depth,
											// width & height)
	LPLOGPALETTE OutputPalette;				// can be NULL

	UINT32 Dither;							// Type of dithering to use

// for debug purposes
	static INT32 HeightWritten;				// count of the height of the export
	static INT32 HeightWanted;				// the actual height of the export required

	// Required by the Accusoft export functions
	static LPBITMAPINFO lpBitmap;			// can be NULL
	static LPBITMAPINFOHEADER lpBitmapInfo;	// can be NULL
	static LPBYTE ExportBuffer;				// can be NULL
	static DIBConvert *DoExportConvert;		// the convert DIB
	static FNPTR_SCANLINE ExportConvertFn;	// the convert function
	static size_t ExportSourceWidth;		// width of the source DIB
	static size_t ExportDestWidth;			// width of the destination DIB
	static LPBYTE ExportData;				// pointer to the export data 
	static INT32 CurrentYPos;					// current line number
	static size_t ExportChunkHeight;		// Chunk height in use			

	static BOOL OutputForward;				// start from bottom or top of DIB
	static UINT32 CurrentStripSize;			// current size of the strip being rendered
	INT32 CurrentExportSize;					// size set up for the progress bar

	static BOOL IsFirstStrip;
};

#endif
