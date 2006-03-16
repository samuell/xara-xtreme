// $Id: dibconv.cpp 662 2006-03-14 21:31:49Z alex $
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

// DIB conversion routines from one depth to another

/*
*/

#include "camtypes.h"
#include "dibconv.h"
#include "gdraw.h"					// temp bodge
#include "grndrgn.h"
//#include "grndbmp.h"				// GRenderBitmap
#include "GDrawIntf.h"
#include "fixmem.h"					// CCMalloc, CCFree
#include "dibutil.h"				// DIBUtil::ScanlineSize
//#include "palman.h"

CC_IMPLEMENT_MEMDUMP( DIBConvert, CC_CLASS_MEMDUMP )

#if defined(__WXGTK__)
#define GetLastError() 0
#elif defined(__WXMAC__)
#define GetLastError() 0
#endif

// these classes are never referred to outside this source file, so they live in the .cpp
class Conv32to32 : public DIBConvert
{
public:
	Conv32to32( UINT32 );
	~Conv32to32();

	BOOL Convert( LPBYTE Input, LPBYTE Output, UINT32 Height, BOOL FirstStrip );

private:
	BITMAPINFOHEADER SourceHeader;
	BITMAPINFOHEADER DestHeader;
};

// these classes are never referred to outside this source file, so they live in the .cpp
class Conv32to24 : public DIBConvert
{
public:
	Conv32to24( UINT32 );
	~Conv32to24();

	BOOL Convert( LPBYTE Input, LPBYTE Output, UINT32 Height, BOOL FirstStrip );

private:
	BITMAPINFOHEADER SourceHeader;
	BITMAPINFOHEADER DestHeader;
};

// these classes are never referred to outside this source file, so they live in the .cpp
class Conv32to8 : public DIBConvert
{
public:
	Conv32to8( UINT32, LPLOGPALETTE, UINT32 );
	~Conv32to8();

	BOOL Convert( LPBYTE Input, LPBYTE Output, UINT32 Height, BOOL FirstStrip );

private:
	BITMAPINFOHEADER SourceHeader;
	BITMAPINFOHEADER DestHeader;
	LPLOGPALETTE DestPalette;
	UINT32 Dither;
};

// these classes are never referred to outside this source file, so they live in the .cpp
class Conv32to4 : public DIBConvert
{
public:
	Conv32to4( UINT32, LPLOGPALETTE, UINT32 );
	~Conv32to4();

	BOOL Convert( LPBYTE Input, LPBYTE Output, UINT32 Height, BOOL FirstStrip );

private:
	BITMAPINFOHEADER SourceHeader;
	BITMAPINFOHEADER DestHeader;
	LPLOGPALETTE DestPalette;
	UINT32 Dither;
};

// these classes are never referred to outside this source file, so they live in the .cpp
class Conv32to1 : public DIBConvert
{
public:
	Conv32to1( UINT32, LPLOGPALETTE, UINT32 );
	~Conv32to1();

	BOOL Convert( LPBYTE Input, LPBYTE Output, UINT32 Height, BOOL FirstStrip );

private:
	BITMAPINFOHEADER SourceHeader;
	BITMAPINFOHEADER DestHeader;
	LPLOGPALETTE DestPalette;
	UINT32 Dither;
};

// these classes are never referred to outside this source file, so they live in the .cpp
class Conv24to8 : public DIBConvert
{
public:
	Conv24to8( UINT32, LPLOGPALETTE, UINT32 );
	~Conv24to8();

	BOOL Convert( LPBYTE Input, LPBYTE Output, UINT32 Height, BOOL FirstStrip );

protected:
	void Convert24to32( INT32 PixelWidth, LPBYTE InputBits, LPBYTE OutputBits );

private:
	BITMAPINFOHEADER SourceHeader;
	BITMAPINFOHEADER DestHeader;
	LPLOGPALETTE DestPalette;
	UINT32 Dither;

	LPBYTE InputBuffer32bpp;
};


/********************************************************************************************

>	class Conv8to32 : public DIBConvert

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/11/96
	Purpose:   	An 8 to 32 bpp converter

********************************************************************************************/
class Conv8to32 : public DIBConvert
{
public:
	Conv8to32( LPBITMAPINFO pSourceHeader );

	BOOL Convert( LPBYTE pInput, LPBYTE pOutput, UINT32 Height, BOOL FirstStrip );

protected:
	LPBITMAPINFO	m_pSourceHeader;
};



/********************************************************************************************

>	class Conv24to32 : public DIBConvert

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/11/96
	Purpose:   	A 24 to 32 bpp converter

********************************************************************************************/
class Conv24to32 : public DIBConvert
{
public:
	Conv24to32( LPBITMAPINFO pSourceHeader );

	BOOL Convert( LPBYTE pInput, LPBYTE pOutput, UINT32 Height, BOOL FirstStrip );

protected:
	LPBITMAPINFO	m_pSourceHeader;
};







#define	new	CAM_DEBUG_NEW

GDrawContext* DIBConvert::ConvertContext = NULL;		// Pointer to conversion context for 8bpp


/********************************************************************************************

>	DIBConvert::DIBConvert()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>?
	Created:	29/06/96
	Purpose:	Base class to convert bitmap depths

********************************************************************************************/
DIBConvert::DIBConvert()
{
}

DIBConvert::~DIBConvert()
{
}



BOOL DIBConvert::Init()
{
//#pragma message( __LOCMSG__ "Removed GDrawAsm usage" )
	if (ConvertContext == NULL)
		ConvertContext = new GDrawAsm();

	if (ConvertContext)
		ConvertContext->Init();

	// We don't really care if this doesn't work as it is an optimisation
	return(TRUE);
}


void DIBConvert::DeInit()
{
//#pragma message( __LOCMSG__ "DIBConvert::DeInit - do nothing" )
//	TRACE( wxT("Warning - DIBConvert::DeInit called\n") );
	if (ConvertContext != NULL)
	{
		delete ConvertContext;
		ConvertContext = NULL;
	}
}


GDrawContext* DIBConvert::GetConvertContext(void)
{
//#pragma message( __LOCMSG__ "DIBConvert::GetConvertContext - do nothing" )
//	TRACE( wxT("Warning - DIBConvert::GetConvertContext called\n") );
//	return NULL;
	if (ConvertContext != NULL)
		return(ConvertContext);
	else
		return(GRenderRegion::GetStaticDrawContext());
}


DIBConvert *DIBConvert::Create( UINT32 SourceDepth, UINT32 DestDepth, UINT32 Width, LPLOGPALETTE Palette, UINT32 DitherType )
{
	ERROR2IF(DestDepth<=8 && Palette==NULL,NULL,"DIBConvert::Create null palette specified");
	
	switch(SourceDepth)
	{
	case 32:
		{
			switch(DestDepth)
			{
			case 24:
				return new Conv32to24( Width );
			case 8:
				return new Conv32to8( Width, Palette, DitherType );
			case 4:
				return new Conv32to4( Width, Palette, DitherType );
			case 1:
				return new Conv32to1( Width, Palette, DitherType );
			case 32:
				return new Conv32to32( Width );
			default:
				return NULL;
			}
		}
	case 24:
		{
			if (DestDepth==8)
				return new Conv24to8( Width, Palette, DitherType );
		}
	default:
		return NULL;
	}

	return NULL;
}


/********************************************************************************************

>	DIBConvert* DIBConvert::Create( LPBITMAPINFO pSourceHeader, UINT32 DestDepth, UINT32 DitherType )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/11/96
	Purpose:	Alternative Creation function

********************************************************************************************/
DIBConvert* DIBConvert::Create( LPBITMAPINFO pSourceHeader, UINT32 DestDepth, UINT32 DitherType )
{
	DitherType = DitherType;
	UINT32 SourceDepth = pSourceHeader->bmiHeader.biBitCount;

	if (SourceDepth == 8)
	{
		if (DestDepth==32)
			return new Conv8to32( pSourceHeader );
	}
	if (SourceDepth == 24)
	{
		if (DestDepth==32)
			return new Conv24to32( pSourceHeader );
	}
	return NULL;
}


/********************************************************************************************

>	Conv32to32::Conv32to32(UINT32 Width)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/99
	Purpose:	Constructor

********************************************************************************************/
Conv32to32::Conv32to32(UINT32 Width)
{
	SourceHeader.biPlanes = 1;
	SourceHeader.biBitCount = 32;
	SourceHeader.biWidth = Width;
	SourceHeader.biClrUsed = 0;

	DestHeader.biPlanes = 1;
	DestHeader.biBitCount = 32;
	DestHeader.biWidth = Width;
	DestHeader.biClrUsed = 0;
}

Conv32to32::~Conv32to32()
{
}

BOOL Conv32to32::Convert( LPBYTE Input, LPBYTE Output, UINT32 Height, BOOL FirstStrip )
{
	SourceHeader.biHeight = Height;

	DestHeader.biHeight = Height;

	DWORD DitherWord = 0;

	GDrawContext *GDC = GetConvertContext();
	const BOOL res = GDC->ConvertBitmap( &SourceHeader, Input, &DestHeader, Output, DitherWord);
	if (!res)
		TRACE( _T("GColour_ConvertBitmap did error %x\n"), GetLastError() );

	return TRUE;
}


/********************************************************************************************

>	Conv32to24::Conv32to24(UINT32 Width)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>?
	Created:	29/06/96
	Purpose:	Constructor

********************************************************************************************/
Conv32to24::Conv32to24(UINT32 Width)
{
	SourceHeader.biPlanes = 1;
	SourceHeader.biBitCount = 32;
	SourceHeader.biWidth = Width;
	SourceHeader.biClrUsed = 0;

	DestHeader.biPlanes = 1;
	DestHeader.biBitCount = 24;
	DestHeader.biWidth = Width;
	DestHeader.biClrUsed = 0;
}

Conv32to24::~Conv32to24()
{
}

BOOL Conv32to24::Convert( LPBYTE Input, LPBYTE Output, UINT32 Height, BOOL FirstStrip )
{
	SourceHeader.biHeight = Height;

	DestHeader.biHeight = Height;

	DWORD DitherWord = 8;	// Convert to 888 format

	GDrawContext *GDC = GetConvertContext();
	const BOOL res = GDC->ConvertBitmap( &SourceHeader, Input, &DestHeader, Output, DitherWord);
	if (!res)
		TRACE( _T("GColour_ConvertBitmap did error %x\n"), GetLastError() );

	return TRUE;
}



/********************************************************************************************

>	Conv32to8::Conv32to8(UINT32 Width, LPLOGPALETTE Palette, UINT32 DitherType)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>?
	Created:	29/06/96
	Purpose:	Constructor

********************************************************************************************/
Conv32to8::Conv32to8(UINT32 Width, LPLOGPALETTE Palette, UINT32 DitherType)
{
	ERROR3IF(Palette == NULL,"Conv32to8 null palette specified");

	SourceHeader.biPlanes = 1;
	SourceHeader.biBitCount = 32;
	SourceHeader.biWidth = Width;
	SourceHeader.biClrUsed = 0;

	DestHeader.biPlanes = 1;
	DestHeader.biBitCount = 8;
	DestHeader.biWidth = Width;
	DestHeader.biClrUsed = 256;

	DestPalette = Palette;

	// Remember which dithering to use
	Dither = DitherType;

	// lets use this palette for the conversion
	GDrawContext *GDC = GetConvertContext();

	GDC->SetupBitmap(Width, 16, 8, NULL);

	// This should return a palette plus tell it what dithering we require
	// 0 = ordered dither, not 0 for error diffusion or no dithering.

	pcLOGPALETTE lpPalette = GDC->SelectPalette((Dither == XARADITHER_SIMPLE ||
												 Dither == XARADITHER_ERROR_DIFFUSION ||
												 Dither == XARADITHER_NONE) ? 1 : 0 );
	if (lpPalette == NULL)
	{
		ENSURE(lpPalette, "Didnt get a palette for export");
	}


	// Graeme (25/9/00) - Removed to prevent a problem with creating a bitmap copy of an
	// existing bitmap.
/*
	// Added by Craig Hamilton 15/9/00.
	// This section of code will replace any flag value with null if it is not a valid value.
	INT32 i;

	for(i=0;i<DestPalette->palNumEntries;i++)
	{
		if(DestPalette->palPalEntry[i].peFlags != NULL && DestPalette->palPalEntry[i].peFlags != PC_EXPLICIT &&
			DestPalette->palPalEntry[i].peFlags != PC_NOCOLLAPSE && DestPalette->palPalEntry[i].peFlags != PC_RESERVED)
		{
			DestPalette->palPalEntry[i].peFlags = NULL;
		}
	}
	// End added.
*/

	GDC->InitialiseWithPalette(DestPalette);
}

Conv32to8::~Conv32to8()
{
}

BOOL Conv32to8::Convert( LPBYTE Input, LPBYTE Output, UINT32 Height, BOOL FirstStrip )
{
	SourceHeader.biHeight = Height;
	  DestHeader.biHeight = Height;

	//  If we are converting to 8 bit, and the bitmap has less than 256 colours, then
	//  it is better to avoid the GDraw conversion function, and do our own instead.
PORTNOTE("other","Removed GRenderOptPalette usage")
#ifndef EXCLUDE_FROM_XARALX
	if( GRenderOptPalette::DoGDrawConversion && !GRenderOptPalette::UseOldPalette )
	{
		// GAT
		RGBQUAD aFastPalette[0x100] ;
		BYTE aIndex[0x100] ;
		UINT32 nEntries = 0 ;

		//  Go through the supplied bitmap strip.
		RGBQUAD* InputBytes = (RGBQUAD*)Input ;
		BYTE*   OutputBytes = Output ;
		UINT32 Width = SourceHeader.biWidth ;
		//  Need an offset if the width of the bitmap is not a multiple of 4.
		INT32 OffsetValue = Width % 4 ;
		if( OffsetValue > 0 )
			OffsetValue = 4 - OffsetValue ;

		for( UINT32 j=0 ; j<Height ; j++ )
		{
			for( UINT32 i=0 ; i<Width ; i++ )
			{
				// Get the RGB value for the current pixel.
				// Mask out the alpha channel.
				RGBQUAD rgb = *InputBytes++ ;
				(DWORD&)rgb &= 0xFFFFFF ;
				// Search through the palette to try and find this colour.
				// This code assumes that it will definitely not be more
				// than 256 colours in the palette.
				for( UINT32 k=0 ; k<nEntries ; k++ )
					if( (DWORD&)aFastPalette[k]==(DWORD&)rgb )
					{
						*OutputBytes++ = aIndex[k] ;
						goto done ;		// Use goto for efficiency.
					}
				// Pixel not found so add to tables.
				{
					PALETTEENTRY pe = {rgb.rgbRed,rgb.rgbGreen,rgb.rgbBlue,0} ;
					*OutputBytes++ = aIndex[nEntries] = PaletteManager::FindIndexOfNearestColour( pe, DestPalette ) ;
					aFastPalette[nEntries++] = rgb ;
					ASSERT(nEntries<=256) ;
				}
		done:	;
			}
			//  After the current row has been done, then apply any 'filler' bytes
			//  necessary.
			OutputBytes += OffsetValue ;
		}
		//  Finished now.
		return TRUE;
	}
	else
#endif
	{
		//  Go here if we have an optimised palette
		DWORD DitherWord = Dither;
		TRACEUSER( "Andy?", _T("Convert32to8, Strip = %d, dither = %d\n"), FirstStrip, Dither);

		if (Dither == XARADITHER_SIMPLE || Dither == XARADITHER_ERROR_DIFFUSION)
		{
			// We are using some form of error diffusion ....

			// Set second byte to 255 if this is the First strip,
			// so that the error buffer is cleared
	 		DitherWord |= (( FirstStrip ? 0 : 255 ) << 8);
		}

		GDrawContext *GDC = GetConvertContext();
		const BOOL res = GDC->ConvertBitmap( &SourceHeader, Input, &DestHeader, Output, DitherWord);

		if (!res)
			TRACE( _T("GColour_ConvertBitmap did error %x\n"), GetLastError() );

		return TRUE;
	}
}



/********************************************************************************************

>	Conv32to8::Conv32to8(UINT32 Width, LPLOGPALETTE Palette, UINT32 DitherType)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>?
	Created:	29/06/96
	Purpose:	Constructor to Convert 32bpp to 4bpp

********************************************************************************************/
Conv32to4::Conv32to4( UINT32 Width, LPLOGPALETTE Palette, UINT32 DitherType )
{
	ERROR3IF(Palette==NULL,"Conv32to4 null palette specified");

	SourceHeader.biPlanes = 1;
	SourceHeader.biBitCount = 32;
	SourceHeader.biWidth = Width;
	SourceHeader.biClrUsed = 0;

	DestHeader.biPlanes = 1;
	DestHeader.biBitCount = 4;
	DestHeader.biWidth = Width;
	DestHeader.biClrUsed = 16;

	DestPalette = Palette;

	// Remember which dithering to use
	Dither = DitherType;

	// lets use this palette for the conversion
	GDrawContext *GDC = GetConvertContext();
	GDC->SetConversionPalette(DestPalette);
}

Conv32to4::~Conv32to4()
{
}

BOOL Conv32to4::Convert( LPBYTE Input, LPBYTE Output, UINT32 Height, BOOL FirstStrip )
{
	SourceHeader.biHeight = Height;

	DestHeader.biHeight = Height;

	DWORD DitherWord = Dither;
	TRACEUSER( "Andy?", _T("Convert32to4, Strip = %d, dither = %d\n"), FirstStrip, Dither);

	if (Dither == XARADITHER_SIMPLE || Dither == XARADITHER_ERROR_DIFFUSION)
	{
		// We are using some form of error diffusion ....

		// Set second byte to 255 if this is the First strip,
		// so that the error buffer is cleared
	 	DitherWord |= (( FirstStrip ? 0 : 255 ) << 8);
	}

	GDrawContext *GDC = GetConvertContext();
	const BOOL res = GDC->ConvertBitmap( &SourceHeader, Input, &DestHeader, Output, DitherWord );

	if (!res)
		TRACE( _T("GColour_ConvertBitmap did error %x\n"), GetLastError() );

	return TRUE;
}


/********************************************************************************************

>	Conv32to1::Conv32to1( UINT32 Width, LPLOGPALETTE Palette, UINT32 DitherType )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>?
	Created:	29/06/96
	Purpose:	Constructor to Convert 32bpp to 1bpp

********************************************************************************************/
Conv32to1::Conv32to1( UINT32 Width, LPLOGPALETTE Palette, UINT32 DitherType )
{
	ERROR3IF(Palette==NULL,"Conv32to1 null palette specified");

	SourceHeader.biPlanes = 1;
	SourceHeader.biBitCount = 32;
	SourceHeader.biWidth = Width;
	SourceHeader.biClrUsed = 0;

	DestHeader.biPlanes = 1;
	DestHeader.biBitCount = 1;
	DestHeader.biWidth = Width;
	DestHeader.biClrUsed = 2;

	DestPalette = Palette;

	TRACEUSER("Andy?", wxT("Creating Convert32to1, Pal@%x, Entries=%d\n"), DestPalette, DestPalette->palNumEntries);
	TRACEUSER("Andy?", wxT("Pal[0] = %dR,%dG,%dB\n"),	DestPalette->palPalEntry[0].peRed,
												DestPalette->palPalEntry[0].peGreen,
												DestPalette->palPalEntry[0].peBlue);
	TRACEUSER("Andy?", wxT("Pal[1] = %dR,%dG,%dB\n"),	DestPalette->palPalEntry[1].peRed,
												DestPalette->palPalEntry[1].peGreen,
												DestPalette->palPalEntry[1].peBlue);

	// Remember which dithering to use
	Dither = DitherType;

	// lets use this palette for the conversion
	GDrawContext *GDC = GetConvertContext();
	GDC->SetConversionPalette(DestPalette);
}

Conv32to1::~Conv32to1()
{
}

BOOL Conv32to1::Convert( LPBYTE Input, LPBYTE Output, UINT32 Height, BOOL FirstStrip )
{
	SourceHeader.biHeight = Height;
	DestHeader.biHeight = Height;

	DWORD DitherWord = Dither;
	TRACEUSER( "Andy?", _T("Convert32to1, Strip = %d, dither = %d\n"), FirstStrip, Dither);

	if (Dither == XARADITHER_SIMPLE || Dither == XARADITHER_ERROR_DIFFUSION)
	{
		// We are using some form of error diffusion ....

		// Set second byte to 255 if this is the First strip,
		// so that the error buffer is cleared
	 	DitherWord |= (( FirstStrip ? 0 : 255 ) << 8);
	}

	GDrawContext *GDC = GetConvertContext();
	const BOOL res = GDC->ConvertBitmap( &SourceHeader, Input, &DestHeader, Output, DitherWord );
	if (!res)
		TRACE( _T("GColour_ConvertBitmap did error %x\n"), GetLastError() );

	return TRUE;
}


/********************************************************************************************

>	Conv24to8::Conv24to8(UINT32 Width, LPLOGPALETTE Palette, UINT32 DitherType)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>?
	Created:	29/06/96
	Purpose:	Constructor to Special convert 24 to 8 class 

********************************************************************************************/
Conv24to8::Conv24to8(UINT32 Width, LPLOGPALETTE Palette, UINT32 DitherType)
{
	ERROR3IF(Palette == NULL,"Conv32to8 null palette specified");

	SourceHeader.biPlanes = 1;
	SourceHeader.biBitCount = 32;
	SourceHeader.biWidth = Width;
	SourceHeader.biClrUsed = 0;

	DestHeader.biPlanes = 1;
	DestHeader.biBitCount = 8;
	DestHeader.biWidth = Width;
	DestHeader.biClrUsed = 256;

	DestPalette = Palette;

	// Remember which dithering to use
	Dither = DitherType;

	// lets use this palette for the conversion
	GDrawContext *GDC = GetConvertContext();

	GDC->SetupBitmap(Width, 16, 8, NULL);

	// This should return a palette plus tell it what dithering we require
	// 0 = ordered dither, not 0 for error diffusion or no dithering.

	pcLOGPALETTE lpPalette = GDC->SelectPalette((Dither == XARADITHER_SIMPLE ||
															Dither == XARADITHER_ERROR_DIFFUSION ||
															Dither == XARADITHER_NONE) ? 1 : 0 );
	if (lpPalette == NULL)
	{
		ENSURE(lpPalette, "Didnt get a palette for export");
	}

	GDC->InitialiseWithPalette(DestPalette);

	// GDraw can only cope with 32 bpp data and so we need to convert the data into 32bpp
	// before calling GDraw. This is the buffer we use for this
	InputBuffer32bpp = NULL;
}

Conv24to8::~Conv24to8()
{
	PORTNOTETRACE("other","DIBConvert::~Conv24to8 - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	GRenderRegion::EnsurePalette(-1);
	if (InputBuffer32bpp != NULL)
		CCFree(InputBuffer32bpp);
#endif
}

BOOL Conv24to8::Convert( LPBYTE Input, LPBYTE Output, UINT32 Height, BOOL FirstStrip )
{
	// Assumes first strip will be the largest and so allocates a conversion buffer of this size
	
	SourceHeader.biHeight = Height;

	DestHeader.biHeight = Height;

	INT32 Width24bpp = DIBUtil::ScanlineSize(SourceHeader.biWidth, 24);
	INT32 Width32bpp = DIBUtil::ScanlineSize(SourceHeader.biWidth, 32);
	if (InputBuffer32bpp == NULL)
	{
		// If we haven't done so already then allocate ourselves our conversion buffer
		InputBuffer32bpp = (LPBYTE)CCMalloc(Width32bpp * Height);
		if (InputBuffer32bpp == NULL)
			return FALSE;
	}

	// Copy the data across from the source 24bpp buffer into the 32bpp buffer
	INT32 PixelWidth = SourceHeader.biWidth;
	LPBYTE InputBits = Input;
	LPBYTE OutputBits = InputBuffer32bpp;
	// Convert it a pixel line at a time
	for (UINT32 i = 0; i < Height; i++)
	{
		Convert24to32(PixelWidth, InputBits, OutputBits);
		InputBits += Width24bpp;
		OutputBits += Width32bpp;
	}

	DWORD DitherWord = Dither;
	if (FirstStrip)
		TRACEUSER( "Neville", _T("Convert24to8, Strip = %d, dither = %d\n"), FirstStrip, Dither);

	if (Dither == XARADITHER_SIMPLE || Dither == XARADITHER_ERROR_DIFFUSION)
	{
		// We are using some form of error diffusion ....

		// Set second byte to 255 if this is the First strip,
		// so that the error buffer is cleared
	 	DitherWord |= (( FirstStrip ? 0 : 255 ) << 8);
	}

	GDrawContext *GDC = GetConvertContext();
	const BOOL res = GDC->ConvertBitmap( &SourceHeader, InputBuffer32bpp, &DestHeader, Output, DitherWord);
	if (!res)
		TRACE( _T("GColour_ConvertBitmap did error %x\n"), GetLastError() );

	return TRUE;
}

void Conv24to8::Convert24to32( INT32 PixelWidth, LPBYTE InputBits, LPBYTE OutputBits )
{
	// Convert a scanline from 24 to 32 bpp so that GDraw can work with it
	// source form is B,G,R
	// dest is B,G,R,spare
	// Note: if not little-endian, is this still true?
	while (PixelWidth--)
	{
		OutputBits[0] = InputBits[0];
		OutputBits[1] = InputBits[1];
		OutputBits[2] = InputBits[2];
		OutputBits[3] = 0xFF;
		OutputBits += 4;
		InputBits  += 3;
	}
}


/********************************************************************************************

>	Conv8to32::Conv8to32( LPBITMAPINFO pSourceHeader )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/11/96
	Purpose:	Constructor to convert 8 to 32

********************************************************************************************/
Conv8to32::Conv8to32( LPBITMAPINFO pSourceHeader )
{
	ERROR3IF(pSourceHeader->bmiHeader.biBitCount != 8,"Conv8to32 not 8bpp");

	m_pSourceHeader = pSourceHeader;
}



/********************************************************************************************

>	BOOL Conv8to32::Convert( LPBYTE pInputBits, LPBYTE pOutputBits, UINT32 Height, BOOL FirstStrip )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/11/96
	Purpose:	Sub-optimal conversion routine

********************************************************************************************/
BOOL Conv8to32::Convert( LPBYTE pInputBits, LPBYTE pOutputBits, UINT32 Height, BOOL FirstStrip )
{
	ERROR3IF(pInputBits == NULL || pOutputBits == NULL, "Conv8to32: NULL Inputs");

	const RGBQUAD* pPalette	= m_pSourceHeader->bmiColors;
	const UINT32 Width = m_pSourceHeader->bmiHeader.biWidth;

	RGBQUAD* pCurrentOutput	= (RGBQUAD*)pOutputBits;
	LPBYTE pCurrentInput	= pInputBits;

	while (Height--)
	{
		UINT32 CurrentX = Width;
		while (CurrentX--)
		{
			RGBQUAD PaletteEntry = pPalette[*pCurrentInput];
			PaletteEntry.rgbReserved = 0xFF;

			*pCurrentOutput = PaletteEntry;

			++pCurrentOutput;
			++pCurrentInput;
		}
	}

	return TRUE;
}


/********************************************************************************************

>	Conv24to32::Conv24to32( LPBITMAPINFO pSourceHeader )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/11/96
	Purpose:	Constructor to convert 24 to 32

********************************************************************************************/
Conv24to32::Conv24to32( LPBITMAPINFO pSourceHeader )
{
	ERROR3IF(pSourceHeader->bmiHeader.biBitCount != 24,"Conv24to32 not 24bpp");

	m_pSourceHeader = pSourceHeader;
}



/********************************************************************************************

>	BOOL Conv24to32::Convert(	LPBYTE pInputBits, LPBYTE pOutputBits, UINT32 Height, 
								BOOL FirstStrip )
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/11/96
	Purpose:	Sub-optimal conversion routine

********************************************************************************************/
BOOL Conv24to32::Convert( LPBYTE pInputBits, LPBYTE pOutputBits, UINT32 Height, BOOL FirstStrip )
{
	ERROR3IF(pInputBits == NULL || pOutputBits == NULL, "Conv24to32: NULL Inputs");

	// Convert a scanline from 24 to 32 bpp so that GDraw can work with it
	// source form is B,G,R
	// dest is B,G,R,spare
	// Note: if not little-endian, is this still true?
	const UINT32 Width = m_pSourceHeader->bmiHeader.biWidth;

	for (UINT32 CurrentY = Height; CurrentY != 0; --CurrentY)
	{
		for (UINT32 CurrentX = Width; CurrentX != 0; --CurrentX)
		{
			pOutputBits[0] = pInputBits[0];
			pOutputBits[1] = pInputBits[1];
			pOutputBits[2] = pInputBits[2];
			pOutputBits[3] = 0xFF;
			pOutputBits += 4;
			pInputBits  += 3;
		}
	}

	return TRUE;
}
