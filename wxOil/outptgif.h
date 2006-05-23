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

// header for OutputGIF

#ifndef INC_OUTPTGIF
#define	INC_OUTPTGIF

#include "ccfile.h"						// needs FilePos
#include "dibconv.h"					// needs DIBConvert
#include "dibutil.h"					// needs FNPTR_SCANLINE
#include "outptdib.h"	
//#include "gifutil.h"					// GIF header definitions

// From gifutil.h
enum GIFDisposalMethod
{
	GDM_NONE		= 0,	// No disposal specified. The decoder is not required to take any action.
	GDM_LEAVE		= 1,	// Do not dispose. The graphic is to be left in place.
	GDM_BACKTOBACK	= 2,	// Restore to background color. The area used by the graphic must be restored to the background color.
	GDM_PREVIOUS	= 3		// Restore to previous. The decoder is required to restore the area overwritten by the graphic with what was there prior to rendering the graphic.
};

#define GIFBITS    		12
#define MAX_LWZ_BITS	12
#define HSIZE  			5003            // 80% occupancy

typedef INT32	code_int;
typedef INT32	count_int;

const INT32 maxbits = GIFBITS;            				// user settable max # bits/code
const code_int maxmaxcode = (code_int)1 << GIFBITS; 	// should NEVER generate this code

const code_int hsize = HSIZE;              		// for dynamic table sizing


#define MAXCODE(n_bits)        (((code_int) 1 << (n_bits)) - 1)

#define HashTabOf(i)       htab[i]
#define CodeTabOf(i)    codetab[i]

#define tab_prefixof(i) 		CodeTabOf(i)
#define tab_suffixof(i)        ((char_type*)(htab))[i]
#define de_stack               ((char_type*)&tab_suffixof((code_int)1<<GIFBITS))

class BaseCamelotFilter;
 
/********************************************************************************************

>	class OutputGIF : public OutputDIB

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Purpose:	Contains functions to write a DIB to a GIF in segments, optionally
				with compression, optionally converting from one depth to another. To use:
				Open a CCLexFile for write. Call StartFile to write out the header. Call
				WriteBlock to write out each block of the bitmap, starting from low memory.
				When you're done, call TidyUp.
				Similar to OutputDIB but strangely enough writes the data as a GIF instead
				of a BMP or via the AccusoftFilters. 
	SeeAlso:	OutputDIB;

********************************************************************************************/

class OutputGIF : public OutputDIB
{
public:

	OutputGIF();
	virtual ~OutputGIF() {}

	virtual BOOL StartFile( LPBITMAPINFOHEADER lpHeader, LPLOGPALETTE Palette, UINT32 OutputDepth, DWORD CompressionType,
							UINT32 FinalHeight, INT32 ExportSize, UINT32 DitherType);
	virtual BOOL ReStartFile(LOGPALETTE* pNewPal);
	virtual BOOL WriteBlock( UINT32 YPos, UINT32 Height, LPBYTE BlockStart, UINT32 InputBPP = 32,
							 INT32 ProgressOffset = 0);
	virtual BOOL SetUpInfoHeader(const LPBITMAPINFOHEADER lpHeader, const UINT32 OutputDepth,
								 const DWORD CompressionType, const UINT32 LIneWidth, const UINT32 FinalHeight,
								 INT32 *PalSize); 
	virtual BOOL TidyUp();

	// Useful routines for calling to output the GIF data itself
	BOOL OutputGifFileHeader(CCLexFile *File, LPBITMAPINFOHEADER pInfo, BOOL Enhanced, INT32 TransColour,
							 LPLOGPALETTE pPalette = NULL, LPRGBQUAD pQuadPalette = NULL);
	BOOL OutputGifFileHeader(CCLexFile *File, BOOL Enhanced, INT32 TransColour);
	BOOL OutputGifImageExtensionHeader(CCLexFile *File, BOOL InterlaceState, INT32 TransparentColour,
									   UINT32 Delay, UINT32 RestoreType);
	BOOL OutputGifImageBits(CCLexFile *File, LPBYTE pBlockStart, BOOL Interlace, BOOL LocalColourTable,
							BaseCamelotFilter *pFilter = NULL,
							UINT32 NewWidth = 0, UINT32 NewHeight = 0, UINT32 LeftOffset = 0, UINT32 TopOffset = 0,
							LPRGBQUAD pDestPalette = NULL, UINT32 PaletteColourDepth = 0, UINT32 NewBitsPerPixel = 0);
	BOOL OutputGifTerminator(CCLexFile *File);
	BOOL OutputAnimationControl(CCLexFile *File, unsigned short Repeats);

	virtual LPBITMAPINFO GetDestBitmapInfo ( void );
	virtual LPBYTE GetDestBitmapBits ( void );

	INT32 GetColourDepth(INT32 NumberOfColours);

protected:

	// The buffer to hold the destination bitmap before we actually write it out to file
	LPBITMAPINFO DestBitmapInfo;
	LPBYTE DestBitmapBytes;
	LPBYTE pNextStrip;

	// This stores the pointer to the BaseCamelotFilter, which might be required for progress
	// bar updates.
	BaseCamelotFilter * pCamFilter;

	// Some useful variables
	INT32 Width;						// Width of the image
	INT32 Height;						// Height of the image
	INT32 curx;						// current x position
	INT32 cury;						// current y position
	INT32 CurrentLine;				// count of the lines output so far, used for progress bar
	INT32 CountDown;
	BOOL InterlacingOn;
	INT32 Pass;						// interlaced pass that we are on

	WORD BitsPerPixel;				// Colour depth required
    INT32 InitCodeSize;				// starting size of the bitmap data to compress
	INT32 Background;					// background colour number

	LPBYTE DataBuffer;				// Start of buffer to compress
	//INT32 DataBufferSize;				// size of the data buffer
	UINT32 WidthOfLine;				// word/byte rounded line width rather than the pixel width

	typedef unsigned char char_type;

	INT32 n_bits;                     	// number of bits/code
	code_int maxcode;               	// maximum code, given n_bits

	count_int htab [HSIZE];
	unsigned short codetab [HSIZE];

	code_int free_ent;		// first unused entry

	// block compression parameters -- after all codes are used up,
 	// and compression rate changes, start over.
	INT32 clear_flg;

	INT32 offset;
	INT32 in_count;       // length of input
	INT32 out_count; 		// # of codes output (for debugging)

	// For compress()
	INT32 g_init_bits;

	INT32 ClearCode;
	INT32 EOFCode;

	// for output()
	UINT32 cur_accum;
	INT32 cur_bits;

	INT32 a_count;		// Number of characters so far in this 'packet' (accumulator)
	char accum[ 256 ];	// Define the storage for the packet accumulator

	// Useful routines
	void CompressBlockToFile( LPBYTE Buffer, INT32 init_bits);
	void OutputCode( code_int code );
	void ClearBlock ();
	void ClearHashTable(count_int hsize);
	void CharacterInit();
	void OutputCharacter( INT32 c );
	void FlushCharacters();
	//void init_statics();
	INT32 GIFNextPixel();
	void BumpPixel();
	//BOOL BoundsSafe(INT32 x, INT32 y);
	//INT32 GetPixel(INT32 x, INT32 y);
};

#endif // INC_OUTPTGIF
