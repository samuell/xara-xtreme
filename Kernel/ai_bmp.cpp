// $Id: ai_bmp.cpp 662 2006-03-14 21:31:49Z alex $
// ai_bmp.cpp: implementation of the AIBitmapProcessor class.
//
//////////////////////////////////////////////////////////////////////
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
#include "ai_bmp.h"

#include "spread.h"
#include "page.h"

#include "ai5_eps.h"
#include "progress.h"
//#include "tim.h"

#include "nodebmp.h"

DECLARE_SOURCE("$Revision");

#define new CAM_DEBUG_NEW


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AIBitmapProcessor::AIBitmapProcessor() :
	mLastIndex(0),
	mpNewBitmap(0)
{

}

AIBitmapProcessor::~AIBitmapProcessor()
{
	delete mpNewBitmap;
	mpNewBitmap = 0;
}

/********************************************************************************************

>	BOOL AIBitmapProcessor::BeginRaster()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00

	Purpose:	Allows the bitmap process to intialize itself for a new bitmap
				signalled by the %BeginRaster comment.

	Returns:	TRUE if the definition was processed, 
				FALSE if not used by this filter..

********************************************************************************************/
BOOL AIBitmapProcessor::BeginRaster()
{
	ENSURE(mpNewBitmap == NULL, "mpNewBitmap is not NULL");

	return TRUE;
}


/********************************************************************************************

>	BOOL AIBitmapProcessor::DecodeXI( AI5EPSFilter& filter )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00
	Returns:	TRUE if the definition was processed, 
				FALSE if not used by this filter..
	Purpose:	Decodes the EPS XI, bitmap definition in an Illustrator 5 file

	The format of the operator is:

	[ a b c d tx ty ] llx lly urx ury h w bits ImageType AlphaChannelCount reserved bin-ascii ImageMask XI

********************************************************************************************/
BOOL AIBitmapProcessor::DecodeXI( AI5EPSFilter& filter )
{
	// Graeme (18/4/00) - This code isn't very pretty, but it's necessary because of the way
	// in which the bitmap is stored. If I try a GetCoordPair () to get the bitmap positions,
	// the image will be misrendered because the transformation should take place due to the
	// tx and ty components of the matrix. Note also that the bitmap's position in Adobe
	// Illustrator is taken from the top left corner, whilst we use the bottom left.

	/////////////////
	// Get the page origin.
	/////////////////

	// Graeme (18/4/00) - Declare variables to get the origin of the page within Camelot's
	// co-ordinate space.
	Document	*pDocument	= filter.GetDocument ();
	Spread		*pSpread	= pDocument->FindFirstSpread ();
	Page		*pPage		= pSpread->FindFirstPageInSpread ();
	DocCoord	Origin		= pPage->GetPageRect ().lo;

	/////////////////
	// decode the bitmap parameters
	/////////////////

	Matrix ImageMatrix;
	DocCoord Translation;

	INT32 h, w, bits, ImageType, AlphaChannelCount, reserved, bin_ascii, ImageMask;
	INT32 llx, lly, urx, ury;

	INT32 nLength = 0;
	INT32 nLineLength = 0;
	INT32 nMaxLineOffset = 0;

	INT32 nChannels = 0;

	NodeBitmap* pNodeBitmap = NULL;
	DocCoord p;

	// Graeme (18/4/00) - I've replaced the Pop with PopCoordPair for extracting the
	// bounding co-ordinates of the bitmap. This means that they will be scaled up
	// into the Xara co-ordinate space. I've also reversed the popping of h and w
	// from the stack - their order is incorrect in the AI documentation.
	if (	!filter.GetStack().Pop(&ImageMask) 	  || 
			!filter.GetStack().Pop(&bin_ascii) || 
			!filter.GetStack().Pop(&reserved) || 
			!filter.GetStack().Pop(&AlphaChannelCount) || 
			!filter.GetStack().Pop(&ImageType) || 
			!filter.GetStack().Pop(&bits) || 
			!filter.GetStack().Pop(&h) || 
			!filter.GetStack().Pop(&w) || 
			!filter.GetStack().PopCoord(&ury) || 
			!filter.GetStack().PopCoord(&urx) || 
			!filter.GetStack().PopCoord(&lly) || 
			!filter.GetStack().PopCoord(&llx) || 
			!filter.GetStack().Pop( &ImageMatrix, TRUE )
		)
		goto EPSError;

	/////////////////
	// create space for the tentative bitmap
	/////////////////

	///////////////////
	//	ImageType gives the number of channels per pixel
	//	bits is the bits per channel
	//	However we will convert CMYK bitmaps to RGB
	///////////////////

	switch ( ImageType )
	{
		case 1:	// greyscale
			nChannels = 1;
			break;
		case 3:	// rgb
			nChannels = 3;
			break;
		case 4:	// CMYK
			nChannels = 3;
			break;
		default: // unknown
			goto EPSError;
	}

	mpNewBitmap = new KernelBitmap( w, h, bits * nChannels, 96 );
	if ( !mpNewBitmap )
		goto EPSError;

	///////////////////
	//	We can import greyscale bitmaps as well
	///////////////////

	if ( ImageType == 1 )
		mpNewBitmap->SetAsGreyscale();


	/////////////////
	// get the binary data
	/////////////////

	nLength = mpNewBitmap->GetActualBitmap()->GetBitmapSize();
	nLineLength = mpNewBitmap->GetActualBitmap()->GetScanlineSize();
	nMaxLineOffset = (( w * mpNewBitmap->GetActualBitmap()->GetBPP() ) / 8) - 1;

	if ( !ReadImageData( filter, ImageType, mpNewBitmap->GetBitmapBits(), nLength, nLineLength, nMaxLineOffset ) )
		goto EPSError;

	/////////////////////
	//	insert the image into the document
	/////////////////////

	// Get a new NodeBitmap object to import into. Don't know what the 12,12 bit does
	pNodeBitmap = new NodeBitmap;

	if ( !pNodeBitmap || !pNodeBitmap->SetUpPath(12,12) )
		goto EPSError;

	pNodeBitmap->GetBitmapRef()->Attach( mpNewBitmap, filter.GetDocument() );

	/////////////////
	//	set up the bounds of the shape containing the bitmap
	/////////////////

	// Graeme (18/4/00) - Adjust the values of lly and ury before they're transformed.
	lly -= h * EPSScaleFactor;
	ury -= h * EPSScaleFactor;

	// Graeme (18/4/00) - Modify the matrix to place the bitmap in the correct place.
	ImageMatrix.GetTranslation ( Translation );		// Extract the translation component.
	Translation += Origin;							// Add the page origin to it.
	ImageMatrix.SetTranslation ( Translation );		// And reset the value in the matrix.

	// Graeme (17/4/00) - Colin overlooked setting up the bounding parallelogram when he
	// wrote this code, and I've just added this.
	p.x = llx;
	p.y = ury;
	ImageMatrix.transform( &p );
	pNodeBitmap->InkPath.InsertMoveTo( p );
	pNodeBitmap->Parallel [0] = p;

	p.x = urx;
	p.y = ury;
	ImageMatrix.transform( &p );
	pNodeBitmap->InkPath.InsertLineTo( p );
	pNodeBitmap->Parallel [1] = p;

	p.x = urx;
	p.y = lly;
	ImageMatrix.transform( &p );
	pNodeBitmap->InkPath.InsertLineTo( p );
	pNodeBitmap->Parallel [2] = p;

	p.x = llx;
	p.y = lly;
	ImageMatrix.transform( &p );
	pNodeBitmap->InkPath.InsertLineTo( p );
	pNodeBitmap->Parallel [3] = p;

	p.x = llx;
	p.y = ury;
	ImageMatrix.transform( &p );
	pNodeBitmap->InkPath.InsertLineTo( p );

	pNodeBitmap->InkPath.CloseSubPath();

	// Graeme (18/4/00) - It is necessary to set the default attributes up for a
	// new node bitmap before inserting it into the tree. Otherwise it's rendered
	// as a greyscale image, even if it is colour, because there's a start colour
	// value set.
	pNodeBitmap->ApplyDefaultBitmapAttrs ( NULL );

	filter.AddNewNode( pNodeBitmap );

	return TRUE;

EPSError:
	if ( mpNewBitmap )
	{
		delete mpNewBitmap;
		mpNewBitmap = 0;
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL AIBitmapProcessor::EndRaster()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00

	Purpose:	Allows the bitmap processor to complete the bitmap
				signalled by the %EndRaster comment.

	Returns:	TRUE if the definition was processed, 
				FALSE if not used by this filter..

********************************************************************************************/
BOOL AIBitmapProcessor::EndRaster()
{
	ENSURE(mpNewBitmap != NULL, "mpNewBitmap is NULL");

	/////////////////
	// add bitmap to tentative bitmap list, giving an index number
	/////////////////

	mBitmaps.AddBitmap( mpNewBitmap, mLastIndex );

	// up the index number
	++mLastIndex;

	// We're done with this bitmap.
	mpNewBitmap		= NULL;

	return TRUE;
}







////////////////////////// IMPLEMENTATION /////////////////////////////////////////////////////

/********************************************************************************************

>	BOOL AIBitmapProcessor::ReadImageData( AI5EPSFilter* const pFilter, const INT32 ImageType, ADDR pData, INT32 nLength ) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00
	Returns:	TRUE if the image was read correctly
				FALSE if the data length differed from the expected given in nLength
	in,out:		pData:		A pointer to an allocated block of memory to hold the decoded
							hexadecimal data.
	in:			nLength:	The expected number of bytes decoded from the stream
				ImageType:	The AI ImageType given in the XI definition

	Purpose:	Decodes the hex string style data following an image (XI) operator
				placing it in the given buffer. This allows the bitmap data to be read into
				a bitmap.

				Each line begins %XXXX where XXXX are hex numbers.

				Assumes that the first % token has not yet been read.

********************************************************************************************/
BOOL AIBitmapProcessor::ReadImageData( AI5EPSFilter& filter, const INT32 ImageType, ADDR pDataStart, INT32 nLength, INT32 nLineLength, INT32 nMaxLineOffset )
{
	////////////////
	// ignore the whitespace (EOL)
	////////////////

	filter.GetEPSFile()->GetToken();

	////////////////
	// read the first % line
	////////////////
	filter.GetEPSFile()->GetToken();

	/////////////////
	//	We need some running values
	/////////////////
	BYTE ColourVal[4] = {0,0,0,0};
	UINT32 nShift = 0;				// which colour component is being read

	////////////////
	//	start with last line
	////////////////

	ADDR pCurrentLine = pDataStart + nLength - nLineLength;
	INT32 nLineOffset = 0;

	while ( pCurrentLine >= pDataStart && !filter.GetEPSFile()->eof() )
	{
		// the data ends with a comment
		if ( _tcsncmp( filter.GetTokenBuf(), _T("%%EndData"), 9 ) == 0 )
			break;

		////////////////
		// update progress
		////////////////
		const INT32 nCharsRead = filter.GetEPSFile()->GetCharsRead();
		if ( nCharsRead > (filter.GetLastProgressUpdate() + 2048) )
		{
			if ( !ContinueSlowJob( nCharsRead ) )
			{
				// Abort operation - make sure nodes are deleted and not added to the tree.
				ERROR(_R(IDT_IMPORT_USERABORT), FALSE);
			}
			else
			{
				filter.GetLastProgressUpdate() = nCharsRead;
			}
		}

		////////////////
		// Decode the token into hex data (starting after the %)
		////////////////

		INT32 nBytes = 0;

		switch ( ImageType )
		{
			case 1:	// greyscale
				nBytes = filter.DecodeHexString( pCurrentLine, nLength, 1 );
				break;

			case 3:	// rgb
				nBytes = DecodeHexStringAsRGB( filter, pCurrentLine, nLineOffset, nLineLength, nMaxLineOffset, 1, ColourVal, nShift );
				break;

			case 4:	// CMYK
				nBytes = DecodeHexStringAsCMYK( filter, pCurrentLine, nLineOffset, nLineLength, nMaxLineOffset, 1, ColourVal, nShift );
				break;
		}

		if ( nBytes == -1 )
		{
			// Error
			TRACE( _T("Error in AI5 image data\n") );
			break;
		}

		///////////////
		//	get the next token
		///////////////
		filter.GetEPSFile()->GetToken();
	}

	return ( pDataStart - pCurrentLine  == nLineLength ) ? TRUE : FALSE;
}


/********************************************************************************************

>	bool inline CharToHex( const char& cDigit, BYTE& lNum )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00

	Purpose:	Converts a hex character to a numeric byte.

	Inputs:		cDigit - the hex character (0-9,A-F)
	Outputs:	nNum	the original value with the hex character ORred into it

	Returns:	true if the character were valid
				false otherwise

********************************************************************************************/
bool inline CharToHex( const char& cDigit, BYTE& nNum )
{
	char ch = _totupper(cDigit);

	if ( (ch >= '0') && (ch <= '9') )
	{
		nNum |= (BYTE) (ch - '0');
	}
	else if ((ch >= 'A') && (ch <= 'F'))
	{
		nNum |= (BYTE) (ch - 'A') + 10;
	}
	else
	{
		// Error in hex data
		return false;
	}
	return true;
}

/********************************************************************************************

>	BYTE ColourValueToByte(const double& Value)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00


	Inputs:		Value	- The colour value, as a double.
	Returns:	BYTE	- The colour value, as a byte.

	Purpose:	Converts a colour value from a double to a BYTE.

********************************************************************************************/

BYTE ColourValueToByte(const double& Value)
{
	return BYTE(floor(Value * double(255) + 0.5));
}

/********************************************************************************************

>	INT32 AIBitmapProcessor::DecodeHexStringAsCMYK( AI5EPSFilter& filter, ADDR& pCurrentLine, 
		INT32& nLineOffset, INT32 nLineLength, INT32 nMaxLineOffset, UINT32 nStart, BYTE CMYKval[], UINT32& nShift )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00

	Purpose:	Treat the current token as a hex string, and decode it into a BGR bitmap.

	Inputs:		filter			the filter doing the import
				nLineLength		the maximum number of bytes to read from the hex string.
				nMaxLineOffset	the width of the bitmap - 1
				nStart			where in TokenBuf (the member that contains the semantic value of
								the token) to start the decoding

	in,out:		pCurrentLine	the line in the bitmap to place the binary data into
				nLineOffset		the offset into the current line that will be read next
				CMYKval			the accumulated CMYK value
				nShift			the index into CMYKVal giving the next value to read

	Returns:	If an error occurs (i.e. invalid hex data) then -1 is returned.

				This is way too complicated but I copied the code from the base class and
				worked from there.
				How about read the bitmap line by line getting an RGB value from the appropriate
				"BitmapParser", say.

********************************************************************************************/
INT32 AIBitmapProcessor::DecodeHexStringAsCMYK( AI5EPSFilter& filter, ADDR& pCurrentLine, INT32& nLineOffset, INT32 nLineLength, INT32 nMaxLineOffset, UINT32 nStart, BYTE CMYKval[], UINT32& nShift )
{
	UINT32 nTokenLen = cc_strlenBytes(filter.GetTokenBuf() + nStart);

	// Assume hex strings are even-numbered in length for the moment
	if ( (nTokenLen & 1) != 0 )
	{
		TRACE( _T("Bad hex string length in DecodeHexString\n") );
		return -1;
	}

	/////////////////
	// Decode the string two characters at a time keeping a running CMYK value
	/////////////////

	double dRed, dGreen, dBlue;
	double dKey;

	for ( UINT32 i = nStart; i < nTokenLen; i += 2 )
	{
		/////////////////
		//	read in the CMYK value one component at at time
		//	first clear the next value
		///////////////////
		CMYKval[nShift] = 0;

		if ( !CharToHex( filter.GetTokenBuf()[i], CMYKval[nShift] ) )
			return -1;

		CMYKval[nShift] <<= 4;

		if ( !CharToHex( filter.GetTokenBuf()[i + 1], CMYKval[nShift] ) )
			return -1;

		++nShift;

		///////////////////
		//	once we have a complete CMYK value, stick it in the bitmap
		///////////////////
		if ( nShift > 3 )
		{
			/////////////////
			// Convert the colour into an RGB format.
			/////////////////

			dKey	= double(CMYKval[3]) / 255.0;

			dRed	= 1.0 - (min(1.0, (double(CMYKval[0]) / 255.0 + dKey )));
			dGreen	= 1.0 - (min(1.0, (double(CMYKval[1]) / 255.0 + dKey )));
			dBlue	= 1.0 - (min(1.0, (double(CMYKval[2]) / 255.0 + dKey )));

			/////////////////
			// Set the value for the current pixel in the bitmap.
			/////////////////

			pCurrentLine[nLineOffset++] = ColourValueToByte( dBlue );
			pCurrentLine[nLineOffset++] = ColourValueToByte( dGreen );
			pCurrentLine[nLineOffset++] = ColourValueToByte( dRed );

			if ( nLineOffset > nMaxLineOffset )
			{
				pCurrentLine -= nLineLength;
				nLineOffset = 0;
			}

			/////////////////
			// start the next CMYK value
			/////////////////

			nShift = 0;
		}
	}

	// How much data did we read?
	return i;
}


/********************************************************************************************

>	INT32 AIBitmapProcessor::DecodeHexStringAsRGB( AI5EPSFilter& filter, ADDR& pCurrentLine, 
			INT32& nLineOffset, INT32 nLineLength, INT32 nMaxLineOffset, UINT32 nStart, BYTE RGBVal[], UINT32& nShift )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00

	Purpose:	Treat the current token as a hex string, and decode it into a BGR bitmap.

	Inputs:		filter			the filter doing the import
				nLineLength		the maximum number of bytes to read from the hex string.
				nMaxLineOffset	the width of the bitmap - 1
				nStart			where in TokenBuf (the member that contains the semantic value of
								the token) to start the decoding

	in,out:		pCurrentLine	the line in the bitmap to place the binary data into
				nLineOffset		the offset into the current line that will be read next
				RGBVal			the accumulated RGB value
				nShift			the index into RGBVal giving the next value to read

	Returns:	If an error occurs (i.e. invalid hex data) then -1 is returned.

				This is way too complicated but I copied the code from the base class and
				worked from there.
				How about read the bitmap line by line getting an RGB value from the appropriate
				"BitmapParser", say.

********************************************************************************************/
INT32 AIBitmapProcessor::DecodeHexStringAsRGB( AI5EPSFilter& filter, ADDR& pCurrentLine, INT32& nLineOffset, INT32 nLineLength, INT32 nMaxLineOffset, UINT32 nStart, BYTE RGBVal[], UINT32& nShift )
{
	UINT32 nTokenLen = cc_strlenBytes(filter.GetTokenBuf() + nStart);

	// Assume hex strings are even-numbered in length for the moment
	if ( (nTokenLen & 1) != 0 )
	{
		TRACE( _T("Bad hex string length in DecodeHexString\n") );
		return -1;
	}

	/////////////////
	// Decode the string two characters at a time 
	/////////////////

	for ( UINT32 i = nStart; i < nTokenLen; i += 2 )
	{
		RGBVal[nShift] = 0;

		if ( !CharToHex( filter.GetTokenBuf()[i], RGBVal[nShift] ) )
			return -1;

		RGBVal[nShift] <<= 4;
			
		if ( !CharToHex( filter.GetTokenBuf()[i + 1], RGBVal[nShift] ) )
			return -1;

		++nShift;

		///////////////////
		//	once we have a complete RGB value, stick it in the bitmap
		//	reversing it to BGR
		///////////////////

		if ( nShift > 2 )
		{
			pCurrentLine[nLineOffset++] = RGBVal[2];
			pCurrentLine[nLineOffset++] = RGBVal[1];
			pCurrentLine[nLineOffset++] = RGBVal[0];

			if ( nLineOffset > nMaxLineOffset )
			{
				pCurrentLine -= nLineLength;
				nLineOffset = 0;
			}

			nShift = 0;
		}
	}

	// How much data did we read?
	return i;
}
