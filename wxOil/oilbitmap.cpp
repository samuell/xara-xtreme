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

// Windows bitmap class

/*
*/

#include "camtypes.h"

#include "cartprov.h"
#include "oilbitmap.h"
//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "dibutil.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camelot.h"
#include "bitmpinf.h"
//#include "rndrgn.h"
#include "nativeps.h"		// The old style EPS native filter, used in v1.1
#include "saveeps.h"
#include "cameleps.h"
//#include "bmpfiltr.h"
//#include "giffiltr.h"	// TI_GIFFilter
//#include "gifutil.h"
//#include "oilfiles.h"
//#include "andy.h"
//#include "extfilts.h"	// Accusoft filters (load that bitmap!)
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "prdlgctl.h"
#include "printctl.h"
//#include "richard2.h"
//#include "view.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "colcontx.h"
#include "colormgr.h"
#include "colplate.h"

#include "bitfilt.h"	// BaseBitmapFilter
//#include "camfiltr.h"	// BaseCamelotFilter - in camtypes.h [AUTOMATICALLY REMOVED]

// for CMX export
//#include "cmxrendr.h"
//#include "cmxexdc.h"
//#include "cmxform.h"

CC_IMPLEMENT_DYNAMIC( CWxBitmap, OILBitmap )

// Declare smart memory handling in Debug builds
//#define	new	CAM_DEBUG_NEW


// firstly the bits of the OILBitmap class that get implemented in the OIL layer


/********************************************************************************************

>	BOOL OILBitmap::Init()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/9/94
	Returns:	TRUE always. Cannot fail.
	Purpose:	Reads the default fill pattern, either from a file or internally from a resource.
	Scope:		Static

********************************************************************************************/

BOOL OILBitmap::Init()
{
	static String_256 FillBitmapName;						//  must be static as a Preference

	CamResource BitmapResource;
	BOOL ShouldDeleteFileObject=TRUE;

	CCLexFile *File = NULL;

	if (!Camelot.DeclareSection( wxT("Attributes"), 10))
		return TRUE;

	Camelot.DeclarePref( NULL, wxT("FillBitmap"), &FillBitmapName );

	if (!FillBitmapName.IsEmpty())
	{
		// name given so try and open disk file
		CCDiskFile *DiskFile = new CCDiskFile(16384);		// buffering makes a BIG difference

		// Check if the constructor failed
		if (DiskFile)
		{
			if ( DiskFile->IsInited() )
			{
				PathName WhichFile( FillBitmapName );

				// Check that the pathname supplied is valid as far as we are concerned
				if ( WhichFile.IsValid(FillBitmapName) )
				{
					if (!DiskFile->open( WhichFile, ios::in | ios::binary ))
					{
						delete DiskFile;
						DiskFile = NULL;
					}
				}
			}
			else
			{
				delete DiskFile;
				DiskFile = NULL;
			}

			if (DiskFile==NULL)
				InformError();
			else
				File = DiskFile;									// read from bitmap file
		}
	}

	BOOL ReadHeader = TRUE;

	if( File == NULL )
	{
		File = BitmapResource.Open(_R(IDB_DEFAULTFILL));
		ShouldDeleteFileObject=FALSE;
	}

	// whatever file we opened, lets read the bitmap from it

	if (File)
	{
		LPBITMAPINFO Info;
		LPBYTE Bytes;

		File->SetReportErrors(FALSE);

		if ( DIBUtil::ReadFromFile( File, &Info, &Bytes, ReadHeader ) )
		{
			// if this fails, it won't be fatal, we just won't have any default bitmap fills
			OILBitmap::Default = new CWxBitmap( Info, Bytes );

			if (!OILBitmap::Default)
				FreeDIB( Info, Bytes );								// free up if failed
			else
			{
				Info->bmiHeader.biXPelsPerMeter = 3780;
				Info->bmiHeader.biYPelsPerMeter = 3780;

				String_256 Str = _R(IDS_WBITMAP_DEFAULT); 				// "Default"
				OILBitmap::Default->SetName(Str);
				OILBitmap::Attach( OILBitmap::Default );			// set usage count to 1
			}
		}
		else
			InformError();

		// close file regardless
		if (File->isOpen())
			File->close();

		// and maybe, just maybe, delete it (that's the object, not the file) as well!!!
		if (ShouldDeleteFileObject) delete File;
	}

	return TRUE;
}

/********************************************************************************************

>	void OILBitmap::Deinit()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/9/94
	Purpose:	Cleans up the OIL bitmap class, designed to be called once on exit.
	Scope:		Static

********************************************************************************************/

void OILBitmap::Deinit()
{
	delete OILBitmap::Default;
	OILBitmap::Default = NULL;

	// perhaps should go through list at some point making sure everything is tidy?
}

/********************************************************************************************

>	OILBitmap *OILBitmap::Create()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/9/94
	Returns:	A new OILBitmap.
	Purpose:	Creates a lovely fresh OILBitmap. Some data needs putting into it before it
				will work.
	Scope:		Static

********************************************************************************************/

OILBitmap *OILBitmap::Create()
{
	// Create a new CWxBitmap object.
	return new CWxBitmap;
}


/********************************************************************************************

>	OILBitmap *OILBitmap::Create( UINT32 Width, UINT32 Height, UINT32 Depth, UINT32 dpi )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/9/94
	Input:		Pixel width, height and depth, and the dpi of the required bitmap.
	Returns:	A new OILBitmap (contents undefined), or NULL if failed.
	Purpose:	Creates a lovely fresh OILBitmap. Some data needs putting into it before it
				will work.
	Errors:		SetError will be called.
	Scope:		Static protected

********************************************************************************************/

OILBitmap *OILBitmap::Create( UINT32 Width, UINT32 Height, UINT32 Depth, UINT32 dpi )
{
	LPBITMAPINFO bmInfo;
	LPBYTE bmBytes;

	bmInfo = AllocDIB( Width, Height, Depth, &bmBytes );
	if (bmInfo)
	{
		//bmInfo->bmiHeader.biXPelsPerMeter = Mul32Div32( dpi, 100*100, 254 );
		double xppm = ((double)dpi * 10000.0)/254.0;
		bmInfo->bmiHeader.biXPelsPerMeter = (INT32)(xppm + 0.5);
		bmInfo->bmiHeader.biYPelsPerMeter = bmInfo->bmiHeader.biXPelsPerMeter;
		CWxBitmap *WinBM = new CWxBitmap( bmInfo, bmBytes );
		if (WinBM)
			return WinBM;
		else
			FreeDIB( bmInfo, bmBytes );
	}
	return NULL;
}

// now the actual CWxBitmap class


/********************************************************************************************

>	CWxBitmap::CWxBitmap()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/9/94
	Purpose:	Constructor.

********************************************************************************************/

CWxBitmap::CWxBitmap()
{
	BMInfo = NULL;
	BMBytes = NULL;

	// Set up things for plotting
	ScanLineByteWidth = 0;
	BitmapSize = 0; // in bytes

	CacheGeometry();
}

/********************************************************************************************

>	CWxBitmap::CWxBitmap(LPBITMAPINFO Info, LPBYTE Bytes)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/9/94
	Input:		Windows bitmap pointers to an existing bitmap. Sets up the CWxBitmap to
				contain said bitmap.
	Purpose:	Constructor. This must NOT be called from Kernel code.

********************************************************************************************/

CWxBitmap::CWxBitmap(LPBITMAPINFO Info, LPBYTE Bytes)
{
	BMInfo = Info;
	BMBytes = Bytes;

	ScanLineByteWidth = 0;
	BitmapSize = 0; // in bytes

	CacheGeometry();
}



/********************************************************************************************

>	CWxBitmap::~CWxBitmap()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/9/94
	Purpose:	Destructor. Frees up Windows resources for the bitmap.

********************************************************************************************/

CWxBitmap::~CWxBitmap()
{
	if (!HasBeenDeleted())
	{
		if (BMInfo != NULL && BMBytes != NULL)
		{
			FreeDIB( BMInfo, BMBytes );
			BMInfo = NULL;
			BMBytes = NULL;
		}
	}

	if (m_pGreyscaleTable)
		delete m_pGreyscaleTable;
}

// number of millipoints in a metre
// this is 100*72000/2.54 (approx)
#define	MILLIS_PER_METRE	2834646L
// Should be using M_MP_VAL defined in units.h to be more consistent

// many bitmaps have zero in the X/YPelsPerMeter field, so we default to a common value
// based on 96dpi (96*100/2.54) (96 comes from iPhotoPlus)
#define	DEFAULT_PIXEL_SIZE	3780



/********************************************************************************************

>	virtual BOOL CWxBitmap::GetInfo(BitmapInfo *Info, RenderRegion *pRegion = NULL) const

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/9/94

	Inputs:		pRegion - NULL to get the real BPP of this bitmap
				else points to the render region which will be rendering the bitmap - if
				the region is colour-separating, then the returned BPP will be set to
				the actual export BPP that will/should be used (i.e. deep colour bitmaps
				are generally colour separated to an 8bpp greyscale bitmap)

	Output:		*Info gets updated.

	Returns:   	TRUE if worked, FALSE if failed (e.g. no attached bitmap).

	Purpose:	Kernel code can call this to obtain information about the bitmap in a platform-
				independent way. This function is implemented differently on different OIL
				layers.

				If you only need width/height/bpp/paletteentries, then see SeeAlso

	Errors:		SetError NOT called.
	Scope:		Public

	SeeAlso:	CWxBitmap::GetWidth; CWxBitmap::GetHeight; CWxBitmap::GetBPP;
				CWxBitmap::GetNumPaletteEntries

********************************************************************************************/

BOOL CWxBitmap::GetInfo(BitmapInfo *Info, RenderRegion *pRegion)
{
	ERROR3IF(Info == NULL, "Illegal NULL param");
RebuildXPEBitmap();

	if (BMInfo && BMBytes && !m_bNeedsXPERebuild)
	{
		Info->PixelWidth		= GetWidth();
		Info->PixelHeight		= GetHeight();
		Info->PixelDepth		= GetBPP(pRegion);
		Info->NumPaletteEntries	= GetNumPaletteEntries();

		INT32 PelW = BMInfo->bmiHeader.biXPelsPerMeter;
		if (PelW==0)
			PelW = DEFAULT_PIXEL_SIZE;

		INT32 PelH = BMInfo->bmiHeader.biYPelsPerMeter;
		if (PelH==0)
			PelH = DEFAULT_PIXEL_SIZE;

		if ( (PelW<0) || (PelH<0) )
		{
			ERROR3("Bad pixel size");
			return FALSE;
		}

		// Karim 27/07/2000
		// If PelW and PelH is 3780, then we take that to mean 96 pix-per-inch,
		// which equates to 750 mp-per-pix.
		// This means that
		if (PelW == 3780 && PelH == 3780)
		{
			Info->RecommendedWidth	= Info->PixelWidth * 750;
			Info->RecommendedHeight	= Info->PixelHeight * 750;
		}

		// otherwise, use our non-special-case calculations.
		else
		{
			//Info->RecommendedWidth = Mul32Div32( Width,  MILLIS_PER_METRE, PelW );
			//Info->RecommendedHeight= Mul32Div32( Height, MILLIS_PER_METRE, PelH );
			// Try and round the value up or down rather than always rounding down which
			// gives problem on repeated loading/saving and potentially on the dpi calculation 
			double RecWidth = ((double)Info->PixelWidth * M_MP_VAL)/(double)PelW;
			double RecHeight = ((double)Info->PixelHeight * M_MP_VAL)/(double)PelH;
			Info->RecommendedWidth = (INT32)(RecWidth + 0.5);
			Info->RecommendedHeight= (INT32)(RecHeight + 0.5);
		}

		// this figure should probably include the BMInfo structure, but its size
		// is not easily obtained
		Info->MemoryUsed = BMInfo->bmiHeader.biSizeImage;

		return TRUE;
	}

	return FALSE;
}



/********************************************************************************************

>	virtual UINT32 CWxBitmap::GetWidth() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/96
	Returns:	The Pixel Width of this bitmap

	Purpose:	Retrieve information on this bitmap

	SeeAlso:	CWxBitmap::GetInfo

********************************************************************************************/

UINT32 CWxBitmap::GetWidth()
{
RebuildXPEBitmap();
	if (BMInfo == NULL)
		return(0);

	return(BMInfo->bmiHeader.biWidth);
}



/********************************************************************************************

>	virtual UINT32 CWxBitmap::GetHeight() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/96
	Returns:	The Pixel Height of this bitmap

	Purpose:	Retrieve information on this bitmap

	SeeAlso:	CWxBitmap::GetInfo

********************************************************************************************/

UINT32 CWxBitmap::GetHeight()
{
RebuildXPEBitmap();
	if (BMInfo == NULL)
		return(0);

	return(BMInfo->bmiHeader.biHeight);
}

/********************************************************************************************

>	virtual UINT32 CWxBitmap::GetHorizontalDPI() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/97
	Returns:	The Horizontal DPI of this bitmap
	Purpose:	Retrieve information on this bitmap
	SeeAlso:	CWxBitmap::GetInfo

********************************************************************************************/

UINT32 CWxBitmap::GetHorizontalDPI()
{
RebuildXPEBitmap();
	if (BMInfo == NULL)
		return 96;

	INT32 PelW = BMInfo->bmiHeader.biXPelsPerMeter;
	if (PelW <= 0)
		PelW = DEFAULT_PIXEL_SIZE;

	UINT32 dpi = (UINT32)((((double)PelW * 254.0)/10000.0) + 0.5); // round up the result
	return dpi;
}

/********************************************************************************************

>	virtual UINT32 CWxBitmap::GetVerticalDPI() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/97
	Returns:	The Vertical DPI of this bitmap
	Purpose:	Retrieve information on this bitmap
	SeeAlso:	CWxBitmap::GetInfo

********************************************************************************************/

UINT32 CWxBitmap::GetVerticalDPI()
{
RebuildXPEBitmap();
	if (BMInfo == NULL)
		return(0);

	INT32 PelH = BMInfo->bmiHeader.biYPelsPerMeter;
	if (PelH <= 0)
		PelH = DEFAULT_PIXEL_SIZE;

	UINT32 dpi = (UINT32)((((double)PelH * 254.0)/10000.0) + 0.5); // round up the result
	return dpi;
}


/********************************************************************************************

>	virtual UINT32 CWxBitmap::GetBPP(RenderRegion *pRegion = NULL) const
					
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95

	Inputs:		pRegion - NULL to get the real BPP of this bitmap
				else points to the render region which will be rendering the bitmap - if
				the region is colour-separating, then the returned BPP will be set to
				the actual export BPP that will/should be used (i.e. deep colour bitmaps
				are generally colour separated to an 8bpp greyscale bitmap)

	Returns:	The number of Bits per pixel of this bitmap

	Purpose:	Finds the colour resolution of this bitmap (or, if PRegion is specified,
				the colour resolution at which this bitmap should be output)

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 CWxBitmap::GetBPP(RenderRegion *pRegion)
{
RebuildXPEBitmap();
	// Currently, we colour-separate to the same number of BPP as the source bitmap,
	// so we always return the same value.

	if (BMInfo == NULL)
		return 0;

	return BMInfo->bmiHeader.biBitCount;
}

/********************************************************************************************

>	BOOL CWxBitmap::SetTransparencyIndex(INT32 Index)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/6/96
	Returns:	TRUE if the Index was set, FALSE if there is no transparent index
	Purpose:	Finds the Transparent index colour of a bitmap if one exists

********************************************************************************************/

BOOL CWxBitmap::SetTransparencyIndex(INT32 Index)
{
RebuildXPEBitmap();
	ERROR3IF(BMInfo == NULL, "SetTransparencyIndex called when BMInfo is NULL");
	if (BMInfo == NULL)
		return FALSE;

	INT32 NumCols = 0;

	switch (GetBPP())
	{
		case 1:
			NumCols = 2;
			break;

		case 4:
			NumCols = 16;
			break;

		case 8:
			NumCols = 256;
			break;

		default:
			ERROR3("Bad colour depth in SetTransparencyIndex");
			break;
	}

	RGBQUAD* pCols = BMInfo->bmiColors;

	// Set the specified Index to be transparent
	for (INT32 i=0; i<NumCols; i++)
	{
		if (i == Index)
			pCols[i].rgbReserved = 0xFF;
		else
			pCols[i].rgbReserved = 0;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL CWxBitmap::GetTransparencyIndex(INT32* pIndex)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/6/96
	Returns:	TRUE if the Index was set, FALSE if there is no transparent index
	Purpose:	Finds the Transparent index colour of a bitmap if one exists

********************************************************************************************/

BOOL CWxBitmap::GetTransparencyIndex(INT32* pIndex)
{
	ERROR3IF(pIndex == NULL, "GetTransparencyIndex called with NULL index pointer");
	if (pIndex == NULL)
		return FALSE;

RebuildXPEBitmap();
	ERROR3IF(BMInfo == NULL, "GetTransparencyIndex called when BMInfo is NULL");
	if (BMInfo == NULL)
		return FALSE;

	INT32 NumCols = 0;

	switch (GetBPP())
	{
		case 1:
			NumCols = 2;
			break;

		case 4:
			NumCols = 16;
			break;

		case 8:
			NumCols = 256;
			break;

		default:
			ERROR3("Bad colour depth in GetTransparencyIndex");
			break;
	}

	RGBQUAD* pCols = BMInfo->bmiColors;

	*pIndex = -1;

	// Look through the Colour table for an entry that has the T channel set to 255
	for (INT32 i=0; i<NumCols; i++)
	{
	 	if (pCols[i].rgbReserved == 0xFF)
		{
			*pIndex = i;
		//	break;
		}
	}

	return ((*pIndex) >= 0);
}


/********************************************************************************************

>	BOOL CWxBitmap::IsTransparent()
				
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/03/2004
	Returns:	TRUE if the bitmap has any non-opaque pixels
	Purpose:	Determines if a bitmap is not totally opaque

********************************************************************************************/

BOOL CWxBitmap::IsTransparent()
{
	RebuildXPEBitmap();
	ERROR3IF(BMInfo == NULL, "IsTransparent called when BMInfo is NULL");

	if (BMInfo == NULL)
		return(FALSE);

	if (GetBPP() <= 8)
	{
		INT32 TranspIndex;

		// If the bitmap doesn't have a transparency index then it is opaque
		if (!GetTransparencyIndex(&TranspIndex))
			return(FALSE);

		switch (GetBPP())
		{
			case 1:
				{
					BYTE* pBits = (BYTE*)(GetBitmapBits());
					UINT32 FullBytes = GetWidth() >> 3;
					UINT32 Height = GetHeight();
					UINT32 ScanLen = GetScanlineSize();
					BYTE Mask = 0xFF >> (7-(GetWidth() - (FullBytes << 3)));
					if (TranspIndex == 0)
					{
						for (UINT32 y = 0; y < Height; y++)
						{
							UINT32 x;
							for (x = 0; x < FullBytes; x++)
							{
								if (pBits[x] != 0xFF)	// If the 8 pixels are not all the opaque index
									return(TRUE);		// then we are transparent
							}
							if (Mask)
							{
								if ((pBits[x] & Mask) != Mask)
									return(TRUE);
							}
							pBits += ScanLen;
						}
					}
					else
					{
						for (UINT32 y = 0; y < Height; y++)
						{
							UINT32 x;
							for( x = 0; x < FullBytes; x++)
							{
								if (pBits[x] != 0x00)	// If the 8 pixels are not all the opaque index
									return(TRUE);		// then we are transparent
							}
							if (Mask)
							{
								if ((pBits[x] & Mask) != 0x00)
								return(TRUE);
							}
							pBits += ScanLen;
						}
					}
				}
				break;

			case 4:
				{
					BYTE* pBits = (BYTE*)(GetBitmapBits());
					UINT32 Width = GetWidth();
					UINT32 ByteWidth = (Width + 1) >> 1;			// The number of used bytes in scanline
					UINT32 Height = GetHeight();
					UINT32 ScanLen = GetScanlineSize();
					INT32 TranspIndexHigh = TranspIndex << 4;
					for (UINT32 y = 0; y < Height; y++)
					{
						UINT32 x;
						for( x = 0; x < ByteWidth; x++)
						{
							if ((pBits[x] & 0x0F) == TranspIndex)		// If the pixel is the transparent colour
								return(TRUE);							// then we are transparent
							if ((pBits[x] & 0xF0) == TranspIndexHigh)	// If the pixel is the transparent colour
								return(TRUE);							// then we are transparent
						}
						if (Width % 2 == 1)								// If we have an odd pixel then test it
						{
							if ((pBits[x] & 0x0F) == TranspIndex)		// If the pixel is the transparent colour
								return(TRUE);							// then we are transparent
						}
						pBits += ScanLen;
					}
				}
				break;

			case 8:
				{
					BYTE* pBits = (BYTE*)(GetBitmapBits());
					UINT32 Width = GetWidth();
					UINT32 Height = GetHeight();
					UINT32 ScanLen = GetScanlineSize();
					for (UINT32 y = 0; y < Height; y++)
					{
						for (UINT32 x = 0; x < Width; x++)
						{
							if (pBits[x] == TranspIndex)	// If the pixel is the transparent colour
								return(TRUE);				// then we are transparent
						}
						pBits += ScanLen;
					}
				}
				break;

			default:
				ERROR3("Bad colour depth in IsTransparent");
				break;
		}
	}
	else if (GetBPP() == 32)
	{
		// if we have any non-opaque pixels then we must be complex
		DWORD* pBits = (DWORD*)(GetBitmapBits());
		UINT32 Width = GetWidth();
		UINT32 Height = GetHeight();
		DWORD* pEnd = pBits + (Width * Height);
		while (pBits < pEnd)
		{
			if (*pBits && 0xFF000000)		// If the pixel is not completely opaque
				return(TRUE);				// then we are transparent
			pBits++;
		}
	}

	return(FALSE);
}


/********************************************************************************************

>	INT32 CWxBitmap::GetRecommendedWidth()
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Returns:	The current recommended width of the bitmap in Millipoints
	Purpose:	Finds the recommended width of the bitmap

********************************************************************************************/

INT32 CWxBitmap::GetRecommendedWidth()
{
RebuildXPEBitmap();
	const UINT32 Width  = BMInfo->bmiHeader.biWidth;

	INT32 PelW = BMInfo->bmiHeader.biXPelsPerMeter;
	if (PelW==0)
		PelW = DEFAULT_PIXEL_SIZE;

	if ( (PelW<0) )
	{
		ERROR3IF(TRUE, "Bad pixel size");
		return 0;
	}

	double RecWidth = ((double)Width * M_MP_VAL)/(double)PelW;

	return (INT32)(RecWidth + 0.5);
}

/********************************************************************************************

>	INT32 CWxBitmap::GetRecommendedHeight()
			
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Returns:	The current recommended height of the bitmap in Millipoints
	Purpose:	Finds the recommended height of the bitmap

********************************************************************************************/

INT32 CWxBitmap::GetRecommendedHeight()
{
RebuildXPEBitmap();
	const UINT32 Height = BMInfo->bmiHeader.biHeight;

	INT32 PelH = BMInfo->bmiHeader.biYPelsPerMeter;
	if (PelH==0)
		PelH = DEFAULT_PIXEL_SIZE;

	if ( (PelH<0) )
	{
		ERROR3IF(TRUE, "Bad pixel size");
		return FALSE;
	}

	double RecHeight = ((double)Height * M_MP_VAL)/(double)PelH;

	return (INT32)(RecHeight + 0.5);
}

/********************************************************************************************

>	void CWxBitmap::SetRecommendedWidth(INT32 NewWidth)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		NewWidth, the new width of the bitmap in Millipoints
	Purpose:	Sets the recommended width of the bitmap

********************************************************************************************/

void CWxBitmap::SetRecommendedWidth(INT32 NewWidth)
{
RebuildXPEBitmap();
	if (BMInfo)
	{	
		double PelW = DEFAULT_PIXEL_SIZE;

		if (NewWidth > 0)
		{
			const UINT32 Width = BMInfo->bmiHeader.biWidth;
			PelW = ((double)Width * M_MP_VAL)/(double)NewWidth;
		}

		BMInfo->bmiHeader.biXPelsPerMeter = (INT32)(PelW + 0.5);
	}
}

/********************************************************************************************

>	void CWxBitmap::SetRecommendedHeight(INT32 NewHeight)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		NewHeight, the new height of the bitmap in Millipoints
	Purpose:	Sets the recommended height of the bitmap

********************************************************************************************/

void CWxBitmap::SetRecommendedHeight(INT32 NewHeight)
{
RebuildXPEBitmap();
	if (BMInfo)
	{	
		double PelH = DEFAULT_PIXEL_SIZE;

		if (NewHeight > 0)
		{
			const UINT32 Height = BMInfo->bmiHeader.biHeight;
			PelH = ((double)Height * M_MP_VAL)/(double)NewHeight;
		}

		BMInfo->bmiHeader.biYPelsPerMeter = (INT32)(PelH + 0.5);
	}
}

/********************************************************************************************

>	CWxBitmap* CWxBitmap::MakeCopy() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/2000
	Returns:	pointer to a copy of this bitmap, NULL if it fails
	Purpose:	To make an exact duplicate of this bitmap


********************************************************************************************/

OILBitmap* CWxBitmap::MakeCopy()
{
RebuildXPEBitmap();
	// check to see that theres something to copy
	if (BMInfo == NULL || BMBytes == NULL)
	{
		ERROR3("This bitmap has no bits or info to copy!");
		return NULL;
	}

	LPBYTE pNewBits = NULL;
	LPBITMAPINFO pNewInfo = NULL;

	// call our handy utility function which does the work
	DIBUtil::CopyEntireBitmap(BMInfo, BMBytes, &pNewInfo, &pNewBits);

	if (pNewInfo == NULL || pNewBits == NULL)
	{
		ERROR3("Failed to allocate bitmap structures");
		return NULL;
	}

	CWxBitmap* pNewBitmap = new CWxBitmap(pNewInfo, pNewBits);

	if (pNewBitmap == NULL)
	{
		ERROR3("Failed to allocate new CWxBitmap");
		FreeDIB(pNewInfo, pNewBits);
	}
	return pNewBitmap;
}

/********************************************************************************************

>	virtual UINT32 CWxBitmap::GetNumPaletteEntries() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/96
	Returns:	The number of palette entries used by this bitmap

				If this is a deep-colour bitmap (not paletted) will return 0
				If this is a non-paletted (greyscale) bitmap, will return 0

	Purpose:	Retrieve information on this bitmap

	SeeAlso:	CWxBitmap::GetInfo

********************************************************************************************/

UINT32 CWxBitmap::GetNumPaletteEntries()
{
RebuildXPEBitmap();
	if (BMInfo != NULL && BMInfo->bmiHeader.biBitCount <= 8)
	{
		// 8 bpp or less - can have a palette
		return(BMInfo->bmiHeader.biClrUsed);
	}

	// We've got a problem, or it has no palette - return 0
	return(0);
}



/********************************************************************************************

>	UINT32 CWxBitmap::GetScanlineSize() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/96
	Returns:	The size, in BYTES, of a scanline from this bitmap, including any padding
				bytes that may be necessary to achieve platform-dependant formatting of
				scanlines. (e.g. in Windows, scanlines are padded out to a word boundary)

				i.e. it returns the byte offset from the start of one scanline to the
				start of the next scanline.

	Purpose:	To determine the memory allocation needed to hold a single scanline of
				this bitmap. This is a convenient interface onto DIBUtil::ScanlineSize

	SeeAlso:	DIBUtil::ScanlineSize

********************************************************************************************/

UINT32 CWxBitmap::GetScanlineSize()
{
RebuildXPEBitmap();
	if (BMInfo == NULL)
		return(0);

	return(DIBUtil::ScanlineSize(GetWidth(), GetBPP()));
}



/********************************************************************************************

>	virtual BOOL CWxBitmap::ExportBitmap(RenderRegion *pRegion) const
					
	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?
	Returns:	TRUE for success
	Purpose:	Exports this bitmap to the given EPS RenderRegion
				Just calls ExportBitmapPalette then ExportBitmapData

	Notes:		If you want colour separated output, see...

	SeeAlso:	CWxBitmap::ExportSeparatedPalette; CWxBitmap::ExportSeparatedData;
				CWxBitmap::ExportBitmapPalette; CWxBitmap::ExportBitmapData

********************************************************************************************/

#define CMXREDUCEBPPBUFFERSIZE	(1024 * 4)	// in pixels

BOOL CWxBitmap::ExportBitmap(RenderRegion *pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
RebuildXPEBitmap();
	// Check the render region type
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(CamelotEPSRenderRegion)))
	{
		// It's Camelot EPS - export the bitmap.
		if (!ExportBitmapPalette(pRegion))
			// Error!
			return FALSE;

		if (!ExportBitmapData(pRegion))
			// Error!
			return FALSE;
	}
PORTNOTE("cmx", "Disabled CMXRenderRegion")
#ifndef EXCLUDE_FROM_XARALX
	else if (pRegion->IsKindOf(CC_RUNTIME_CLASS(CMXRenderRegion)))
	{
		// CMX render region... get the DC
		CMXExportDC *pDC = (CMXExportDC *) pRegion->GetRenderDC();

		// start the bitmap list thinguy
		if(!pDC->StartRIFFList(cmxRIFFLIST_ImageDesc))
			return FALSE;

		// start the bitmap info section
		if(!pDC->StartSection(CMXExportDC::CMXSECTION_IMAGINFO))
			return FALSE;

		// tag...
		if(!pDC->StartTag(cmxTAG_DescrSection_Image_ImageInfo))
			return FALSE;

		// non-palette flag
		BOOL NoPal = FALSE;
		if(BMInfo->bmiHeader.biBitCount >= 24)
			NoPal = TRUE;

		// 32 bit pixel flag -- needs to be squished into 24 bitness
		// and our bitmap size value
		BOOL eBitmapSize = BitmapSize;
		BOOL Is32Bit = FALSE;
		if(BMInfo->bmiHeader.biBitCount == 32)
		{
			Is32Bit = TRUE;

			// calculate a new eBitmapSize
			eBitmapSize = GetWidth() * 3;
			if((eBitmapSize & 0x3) != 0)
				eBitmapSize = (eBitmapSize + 4) & ~0x3;	// word align to 4 byte boundary
			eBitmapSize *= GetHeight();
		}

		// write header
		cmxImageInfo ii;
		cmxImageHeader ih;
		cmxRIMAGEHeader rh;
		cmxRIMAGEPaletteHeader ph;

		DWORD BitmapHeaderSize = sizeof(rh) + (NoPal?0:(sizeof(ph) + sizeof(cmxRIMAGEPaletteEntry) * BMInfo->bmiHeader.biClrUsed));

		ii.Type = cmxIMAGETYPE_RIMAGE;
		ii.Compression = 1;			// dunno what that'll do. Probably'll work.
		ii.Size = sizeof(ih) + BitmapHeaderSize + eBitmapSize;
		ii.CompressedSize = sizeof(ih) + BitmapHeaderSize + eBitmapSize;

		pDC->WriteData(&ii, sizeof(ii));

		// end tag
		if(!pDC->EndTag() || !pDC->WriteMinEndTag())
			return FALSE;

		// end the bitmap info section
		if(!pDC->EndSection())
			return FALSE;

		// start the bitmap data section
		if(!pDC->StartSection(CMXExportDC::CMXSECTION_IMAGDATA))
			return FALSE;

		// write the header -- we have to do the tag ourselves as it's got a 
		// 32 bit length value
		CMXFutureLength TagLen;
		if(pDC->IsThirtyTwoBit())
		{
			pDC->WriteByte(cmxTAG_DescrSection_Image_ImageData);
			DWORD len = 0;
			pDC->WriteData(&len, sizeof(len));
			TagLen.Init(pDC, - (INT32)sizeof(len), sizeof(len));
			TagLen.SetLengthStartPos(pDC, - (INT32)(sizeof(len) + sizeof(BYTE)));
		}	

		// do the image header
		ih.ImageType[0] = 'R';
		ih.ImageType[1] = 'I';
		ih.Size = eBitmapSize + sizeof(ih) + BitmapHeaderSize;
		ih.Reserved[0] = 0;
		ih.Reserved[1] = 0;
		ih.OffsetToPixels = sizeof(ih) + BitmapHeaderSize;
		
		if(!pDC->WriteData(&ih, sizeof(ih)))
			return FALSE;

		// make the bitmap header
		rh.Type = NoPal?cmxR_IMG_RGB:cmxR_IMG_PAL;
		rh.ComprType = 1;				// this probably means it's got no compression, but you never know
		rh.Width = GetWidth();
		rh.Height = GetHeight();
		rh.Planes = 1;								// probably right
		rh.BitsPerPlane = NoPal?24:BMInfo->bmiHeader.biBitCount;
		rh.BytesPerLine = (rh.Width * rh.BitsPerPlane) / 8;
		if((rh.BytesPerLine & 0x3) != 0)
			rh.BytesPerLine = (rh.BytesPerLine + 4) & ~0x3;	// word align to 4 byte boundary
		rh.BufSize = eBitmapSize;
		rh.ComprBufSize = 0;						// probably as we're not compressing
		rh.XPelsPerKM = rh.YPelsPerKM = 0x3d400;	// random value nicked from a corel CMX file
		rh.PaletteOffset = NoPal?0:sizeof(rh);		// starts immediatly after this
		rh.DataOffset = BitmapHeaderSize;			//after the palette
		rh.StartBufOffset = 0;						// no idea what this does
		rh.AlphaOffset = 0;							// nope, haven't got one of those mate
		rh.MaskOffset = 0;							// nor one of those

		if(rh.BitsPerPlane == 4 && !pDC->IsThirtyTwoBit())
		{
			rh.Type = cmxR_IMG_PAL4;
		}

		// write the bitmap header
		if(!pDC->WriteData(&rh, sizeof(rh)))
			return FALSE;

		if(!NoPal)
		{
			// is this contoned?
			CMXRenderRegion *pR = (CMXRenderRegion *)pRegion;
			RGBQUAD *pPal = pR->GetAreExportingContoneBitmap()
				?m_pContonePalette:BMInfo->bmiColors;

			// write the palette
			ph.Type = cmxR_PALETTE_RGB;
			ph.NumEntries = (WORD)BMInfo->bmiHeader.biClrUsed;
			if(!pDC->WriteData(&ph, sizeof(ph)))
				return FALSE;
			// palette entries
			for(DWORD l = 0; l < BMInfo->bmiHeader.biClrUsed; l++)
			{
				ERROR3IF(sizeof(cmxRIMAGEPaletteEntry) != 3, "wrong size of entry");
				cmxRIMAGEPaletteEntry pi;
				pi.b = pPal[l].rgbBlue;
				pi.g = pPal[l].rgbGreen;
				pi.r = pPal[l].rgbRed;
				if(!pDC->WriteData(&pi, sizeof(pi)))
					return FALSE;
			}
		}

		// should do it, I suppose.

		// write the bitmap data
		if(!Is32Bit)
		{
			// this isn't 32 bit pixels, so just slap everything to the file
			if(!pDC->WriteData(BMBytes, BitmapSize))
				return FALSE;
		}
		else
		{
			// corel doesn't support 32 bit pixels, so we have to go through
			// the image data reducing it to 24 bit. Oh well.
			BYTE Buffer[CMXREDUCEBPPBUFFERSIZE * 3];
			INT32 BufLoc = 0;
			INT32 PixelsToGo = GetWidth() * GetHeight();
			INT32 PixelsToGoInLine = GetWidth();
			BYTE *Pixels = (BYTE *)BMBytes;
			INT32 Bytes = 0;

			while(PixelsToGo > 0)
			{
				// bung a pixel in the buffer
				Buffer[BufLoc++] = *(Pixels++);
				Buffer[BufLoc++] = *(Pixels++);
				Buffer[BufLoc++] = *(Pixels++);
				Pixels++;
				Bytes += 3;

				// decrement counts
				PixelsToGo--;
				PixelsToGoInLine--;

				// check to see if we're at the end of the line
				if(PixelsToGoInLine <= 0)
				{
					// check to see if we need to word align here...
					if((Bytes & 0x3) != 0)
					{
						INT32 Need = ((Bytes + 4) & ~0x3) - Bytes;
						for(INT32 l = 0; l < Need; l++)
						{
							Buffer[BufLoc++] = 0;
							Bytes++;
						}
					}

					PixelsToGoInLine = GetWidth();	// reset counter
				}

				// test to see if we've done enough
				if(BufLoc >= ((CMXREDUCEBPPBUFFERSIZE * 3) - 8))		// enough for another pixel or so
				{
					// write it to the file
					pDC->WriteData(Buffer, BufLoc);

					// reset counter
					BufLoc = 0;
				}
			}

			// write anything left to the file
			pDC->WriteData(Buffer, BufLoc);
		}

		// write the length
		if(pDC->IsThirtyTwoBit())
			TagLen.Write(pDC);

		// end tag...
		if(!pDC->WriteMinEndTag())
			return FALSE;

		// end the bitmap data section
		if(!pDC->EndSection())
			return FALSE;

		// end the bitmap list thingy
		if(!pDC->EndRIFFList())
			return FALSE;

		return TRUE;
	}
#endif
#endif
	// If we got here, no errors occured.
	return TRUE;
}



/********************************************************************************************

>	BOOL CWxBitmap::ExportBitmapPaletteInternal(RenderRegion *pRegion) const
					
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Moved Tim code into this shared subroutine)
	Created:	1/7/96 (Original code date unknown)

	Returns:	TRUE for success

	Purpose:	Exports the given palette data to the given RenderRegion
				Used as an internal shared function for ExportBitmapPalette and
				ExportContonePalette.

				The palette is exported twice - first as a greyscale CLUT, then as a
				colour CLUT.

	Notes:		This function now handles colour separation and correction on export,
				by using the RenderRegion's RenderView to find an appropriate RGB output
				colour context to do the colour output for it.

				IF (pSrcCLUT == ContonePalette), then a GLOBAL DEFAULT colour context
				is used, so that the contone palette is NOT colour separated.
				It is expected that you will build and discard the contone palette
				very close to the ExportContonePalette call, so that it will be pre-
				separated for the current output context.

********************************************************************************************/

BOOL CWxBitmap::ExportBitmapPaletteInternal(RenderRegion *pRegion, INT32 NumColours, RGBQUAD *pSrcCLUT)
{
#if !defined(EXCLUDE_FROM_RALPH)
RebuildXPEBitmap();
	// Sanity check
	ERROR3IF((BMInfo->bmiHeader.biBitCount != 1) &&
			(BMInfo->bmiHeader.biBitCount != 4) &&
			(BMInfo->bmiHeader.biBitCount != 8) &&
			(BMInfo->bmiHeader.biBitCount != 24) &&
			(BMInfo->bmiHeader.biBitCount != 32), "Bad bitmap depth in ExportBitmap!");

	ERROR3IF(pRegion == NULL || pSrcCLUT == NULL, "Illegal NULL params");

	const INT32 ClutSize = 256;

	// Allocate an array in which we can construct the clut.
	LPBYTE pCLUT = (LPBYTE) CCMalloc(ClutSize * 3);
	if (pCLUT == NULL)
	{
		// Out of memory
		ERROR3("No memory");
		return FALSE;
	}

	// Find the output DC
	KernelDC *pDC = (KernelDC *) CCDC::ConvertFromNativeDC(pRegion->GetRenderDC());

	// First, clear all entries to black
	memset(pCLUT, 0, ClutSize);

	// Find an RGB colour context to colour-separate the colours for us (if it's necessary)
	// (Note: If we're exporting the ContonePalette, then we assume that it is pre-separated,
	// and just get a simple (non-separating) RGB context instead, to avoid double-separating
	ColourContextRGBT *ccRGB = NULL;
	if (pSrcCLUT == m_pContonePalette)
		ccRGB = (ColourContextRGBT *) ColourManager::GetColourContext(COLOURMODEL_RGBT);
	else
		ccRGB = (ColourContextRGBT *) pRegion->GetRenderView()->GetColourContext(COLOURMODEL_RGBT);
	ERROR2IF(ccRGB == NULL, FALSE, "Can't get a ColourContextRGBT for EPS bitmap export");

	DocColour TempCol;
	// BLOCK
	{
		ColourRGBT RGB;
		 
		// Build the greyscale CLUT first.
		for (INT32 i = 0; i < NumColours; i++)
		{
			TempCol.SetRGBValue(pSrcCLUT[i].rgbRed, pSrcCLUT[i].rgbGreen, pSrcCLUT[i].rgbBlue);
			ccRGB->ConvertColour(&TempCol, (ColourGeneric *) &RGB);

			double Grey =	(RGB.Red.MakeDouble()	* 0.305) +
							(RGB.Green.MakeDouble()	* 0.586) +
							(RGB.Blue.MakeDouble()	* 0.109);

			pCLUT[i] = (BYTE) (Grey * 255.0);
		}
	}

	// Greyscale palette complete - output it as a hex string
	pDC->OutputNewLine();
	pDC->OutputToken(_T("<"));
	pDC->OutputRawBinary(pCLUT, ClutSize);
	pDC->OutputToken(_T(">"));
	pDC->OutputNewLine();

	// Ok, now do the colour palette.
	// First, clear all entries to black
	memset(pCLUT, 0, ClutSize * 3);

	// BLOCK
	{
		PColourRGBT RGB;

		for (INT32 i = 0; i < NumColours; i++)
		{
			TempCol.SetRGBValue(pSrcCLUT[i].rgbRed, pSrcCLUT[i].rgbGreen, pSrcCLUT[i].rgbBlue);
			ccRGB->ConvertColour(&TempCol, (ColourPacked *) &RGB);
			
			pCLUT[i * 3 + 0] = RGB.Red;
			pCLUT[i * 3 + 1] = RGB.Green;
			pCLUT[i * 3 + 2] = RGB.Blue;
		}
	}

	// RGB palette complete - output it as a hex string
	pDC->OutputNewLine();
	pDC->OutputToken(_T("<"));
	pDC->OutputRawBinary(pCLUT, ClutSize * 3);
	pDC->OutputToken(_T(">"));
	pDC->OutputNewLine();

	// Clean up
	CCFree(pCLUT);
#endif
	// If we got here, no errors occured.
	return TRUE;
}



/********************************************************************************************

>	virtual BOOL CWxBitmap::ExportBitmapPalette(RenderRegion *pRegion) const
					
	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?
	Returns:	TRUE for success
	Purpose:	Exports this bitmap's palette to the given RenderRegion

	Notes:		This function outputs the data in 2 different ways, depending on
				whether it thinks the RenderRegion is for Renderable postscript or not.

				This function now handles colour separation and correction on export,
				by using the RenderRegion's RenderView to find an appropriate RGB output
				colour context to do the colour output for it.

	SeeAlso:	CWxBitmap::ExportBitmapPaletteInternal

********************************************************************************************/

BOOL CWxBitmap::ExportBitmapPalette(RenderRegion *pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
RebuildXPEBitmap();
	// Check the render region type
	if (!pRegion->IsKindOf(CC_RUNTIME_CLASS(CamelotEPSRenderRegion)))
	{
		ERROR3("ExportBitmapPalette called by non-CamelotEPSRenderRegion class");
		return(TRUE);
	}

	// Sanity check. Note that we allow deep bitmaps here, as this function exports nothing
	// when the bitmap has no palette, including deep images.
	ERROR3IF((BMInfo->bmiHeader.biBitCount != 1) &&
			(BMInfo->bmiHeader.biBitCount != 4) &&
			(BMInfo->bmiHeader.biBitCount != 8) &&
			(BMInfo->bmiHeader.biBitCount != 24) &&
			(BMInfo->bmiHeader.biBitCount != 32), "Bad bitmap depth in ExportBitmap!");

	// If it's a depp bitmap or it has no palette, then we return TRUE immediately
	if (BMInfo->bmiHeader.biBitCount > 8 || BMInfo->bmiHeader.biClrUsed < 1)
		return(TRUE);

	// Find out if it needs to be renderable (proper Adobe EPS)
	BOOL Renderable = (IS_A(pRegion, CamelotEPSRenderRegion) || pRegion->IsPrinting());

	if (Renderable)		// Renderable postscript, so output in proper Adobe format
		return(ExportBitmapPaletteInternal(pRegion, BMInfo->bmiHeader.biClrUsed, BMInfo->bmiColors));


	// Not renderable (it's CamelotEPS), so we can output the bitmap as raw binary
	KernelDC *pDC = (KernelDC *) CCDC::ConvertFromNativeDC(pRegion->GetRenderDC());

	pDC->OutputToken(_T("%Camelot: Bitmap palette"));
	pDC->OutputRawBinary((BYTE *) BMInfo->bmiColors, 
						 BMInfo->bmiHeader.biClrUsed * sizeof(RGBQUAD));
	pDC->OutputNewLine();
#endif
	// If we got here, no errors occured.
	return TRUE;
}



/********************************************************************************************

>	virtual BOOL CWxBitmap::ExportSeparatedPalette(RenderRegion *pRegion) const;
					
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/7/96

	Inputs:		pRegion - The render region to export to
	Returns:	TRUE for success

	Purpose:	Exports a greyscale 8bpp palette suitable for use by the bitmaps output
				by ExportSeparatedData.

	Notes:		This function is very specialised, for handling the output of a palette
				for an 8bpp greyscale seaparation plate of a bitmap to be output with
				OutputSeparatedData.

				See CamelotEPSRenderRegion::DrawParallelogramBitmap for the only code which
				should be calling this function.

	Errors:		Many ERROR3's - this bitmap must be deep, we must be printing, there
				must be a valid ColourPlate for separating the output.

	SeeAlso:	CWxBitmap::ExportBitmapPalette; CWxBitmap::ExportSeparatedData

********************************************************************************************/

BOOL CWxBitmap::ExportSeparatedPalette(RenderRegion *pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(pRegion == NULL, "Illegal NULL params");
	if (!pRegion->IsKindOf(CC_RUNTIME_CLASS(CamelotEPSRenderRegion)))
	{
		ERROR3("ExportBitmapPalette called by non-CamelotEPSRenderRegion class");
		return(TRUE);
	}

	// Sanity checks
	ERROR3IF(!pRegion->IsPrinting(), "ExportSeparatedBitmapPalette called when not printing?");

	ERROR3IF(pRegion->GetRenderView() == NULL, "No render view");
	ERROR3IF(pRegion->GetRenderView()->GetColourPlate() == NULL,
			"Not colour separating in ExportSeparatedBitmapPalette");

RebuildXPEBitmap();
	KernelDC *pDC = (KernelDC *) CCDC::ConvertFromNativeDC(pRegion->GetRenderDC());
	BYTE pCLUT[256 * 3];

	INT32 i;
	// Do a greyscale intensity CLUT
	for (i = 0; i < 256; i++)
		pCLUT[i] = i;

	pDC->OutputNewLine();
	pDC->OutputToken(_T("<"));
	pDC->OutputRawBinary(pCLUT, 256);
	pDC->OutputToken(_T(">"));
	pDC->OutputNewLine();

	// And do a 24bit RGB CLUT
	for (i = 0; i < 256*3; i+=3)
		pCLUT[i] = pCLUT[i+1] = pCLUT[i+2] = i/3;

	pDC->OutputNewLine();
	pDC->OutputToken(_T("<"));
	pDC->OutputRawBinary(pCLUT, 256 * 3);
	pDC->OutputToken(_T(">"));
	pDC->OutputNewLine();

#endif
	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL CWxBitmap::ExportContonePalette(RenderRegion *pRegion) const
					
	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?
	Returns:	TRUE for success
	Purpose:	Exports this bitmap's contone palette to the given RenderRegion
				
				You must have called BuildContonePalette before calling this function!

	Notes:		This funmction does NOT colour separate the contone palette. However,
				the colour is correctly separated when you call BuildContonePalette,
				so it is imperative that you build the contone palette immediately
				prior to export, and discard it immediately afterward (i.e. don't rely
				on a cached version of the contone palette).

	SeeAlso:	OILBitmap::BuildContonePalette; CWxBitmap::ExportBitmapPaletteInternal

********************************************************************************************/

BOOL CWxBitmap::ExportContonePalette(RenderRegion *pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
RebuildXPEBitmap();
	ERROR2IF(m_pContonePalette == NULL, FALSE, "No contone palette to export!");

	// If it's Camelot EPS, then export the bitmap
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(CamelotEPSRenderRegion)))
		return(ExportBitmapPaletteInternal(pRegion, BMInfo->bmiHeader.biClrUsed, m_pContonePalette));
#endif
	// All ok
	return TRUE;
}



/********************************************************************************************

>	virtual BOOL CWxBitmap::ExportBitmapData(RenderRegion *pRegion) const
					
	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?

	Inputs:		pRegion - The Postscript RenderRegion to export to. In fact, if this is
				not IsKindOf(CamelotEPSRenderRegion), this function will do nothing and
				return TRUE.

	Returns:	TRUE for success
	Purpose:	Exports this bitmap's image data to the given RenderRegion

	Notes:		The RenderRegion is responsible for outputting all header/trailer info
				about the bitmap, and for calling ExportBitmapPalette or ExportContonePalette
				as appropriate, if necessary. Be very careful when fiddling with this code,
				because it affects postscript printing as well as all EPS export formats,
				including Xara v1.1 "native" EPS.

				When exporting to postscript, this function can only handle
				1,2,4,8,24,32 bpp images.

				If you want to COLOUR SEPARATE the output, you should call
				ExportSeparatedData instead. 

				(BTW, This code is quite hideous, but I have not changed it, because it
				is too easy to break Postscript, AI EPS, Artworks EPS, and/or Camelot EPS
				by doing so)

	SeeAlso:	CWxBitmap::ExportSeparatedData;
				CamelotEPSRenderRegion::DrawParallelogramBitmap

********************************************************************************************/

BOOL CWxBitmap::ExportBitmapData(RenderRegion *pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
RebuildXPEBitmap();
	// Check the render region type
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(CamelotEPSRenderRegion)))
	{
		// It's Camelot EPS - export the bitmap.
		KernelDC *pDC = (KernelDC *) CCDC::ConvertFromNativeDC(pRegion->GetRenderDC());
		Filter *pFilter = pDC->GetParentFilter();

		// Find out if it needs to be renderable
		BOOL Renderable = (IS_A(pRegion, CamelotEPSRenderRegion) || pRegion->IsPrinting());

		// If it is renderable, find out if we can use Level 2 features.
		BOOL UseLevel2 = FALSE;

		if (Renderable)
		{
// WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
			if (pRegion->IsPrinting())
			{

#ifndef STANDALONE
				// Get the print info for this job.
				CCPrintInfo *pInfo = CCPrintInfo::GetCurrent();
				if (pInfo != NULL)
				{
					PrintControl *pPrCtrl = pInfo->GetPrintControl();
					if (pPrCtrl->GetPSLevel() == PSLEVEL_2)
						// Printing to a level 2 device - party on!
						UseLevel2 = TRUE;
				}
#else
	ERROR2(FALSE,"CWxBitmap::ExportBitmapData trying to print on Viewer version!");
#endif
			}
			else
#endif //webster
			{
				// Use the EPS preference.
				if (EPSFilter::XSEPSExportPSType == 2)
					UseLevel2 = TRUE;
			}
		}

		// Export the bitmap pixel data itself.
		if (!Renderable)
			pDC->OutputToken(_T("%Camelot: Bitmap data"));
		pDC->OutputNewLine();
		
		// Do each scanline...
		//
		// NB. We add 7 to make sure we round up to the next byte for fractional bytes,
		//	   otherwise we lose pixels at the end of scanlines for bitmaps with less
		//	   than 8bpp depth.
		INT32 Width  = ((BMInfo->bmiHeader.biWidth * BMInfo->bmiHeader.biBitCount) + 7) / 8;
		INT32 Height = BMInfo->bmiHeader.biHeight;

		INT32 Ofs = 0;

		// If renderable, then don't pad the data (except to the next byte boundary).
		INT32 Padding = Renderable ? 1 : 4;

		// If we're printiand trying to output 32bpp, we'll output it as 24bpp
		BOOL Do32bpp = pRegion->IsPrinting() && (BMInfo->bmiHeader.biBitCount == 32);
		if (Do32bpp)
			Width = BMInfo->bmiHeader.biWidth * 3;	// 32bpp is converted down to 24bpp

		// Swap RGB data as PostScript expects it in BGR format.
		BOOL SwapRGB = Renderable && (BMInfo->bmiHeader.biBitCount == 24);

		// Expand 4bpp images to 8bpp images as the Adobe SDK imaging library does
		// not seem to work on some printers with 4bpp images (e.g. Canon LBP4-PS).
		BOOL Expand4bpp = Renderable && (BMInfo->bmiHeader.biBitCount == 4);

		INT32 DataSize = Width;
		LPBYTE pAllocatedMem = NULL;		// Memory we must remember to free on exit
		if (SwapRGB || Do32bpp)
		{
			// We need to swap the R and B components around as PostScript expects to get
			// data like this:  BGRBGRBGR...
			// So we need a buffer in which to do this
			pAllocatedMem = (LPBYTE) CCMalloc(Width);
			if (pAllocatedMem == NULL)
				// No more memory!
				return FALSE;
		}
		else if (Expand4bpp)
		{
			// We need to expand the 4bpp data to 8bpp data.
			// So we need a buffer in which to do this
			DataSize = ((BMInfo->bmiHeader.biWidth * 8) + 7) / 8;
			pAllocatedMem = (LPBYTE) CCMalloc(DataSize);
			if (pAllocatedMem == NULL)
				// No more memory!
				return FALSE;

			// Double buffer size
		}

		LPBYTE pBuf = pAllocatedMem;		// Scanline buffer pointer - may point into the source bitmap

		UINT32 ScanlineSize = DIBUtil::ScanlineSize(BMInfo->bmiHeader.biWidth, 
												   BMInfo->bmiHeader.biBitCount);

		// If we are going to do ASCII85, get ready for it.
		if (UseLevel2)
		{
			if (!pDC->StartASCII85Output(TRUE))  // TRUE => RLE please
				// Error
				return FALSE;
		}


		for (INT32 i = 0; i < Height; i++)
		{
			if (Do32bpp)
			{
				// Take a copy of this scanline, skipping the Alpha channels, and reverse the
				// RGB ordering so that PostScript can use the data directly.
				INT32 k = Ofs;
				for (INT32 j = 0; j < Width; j += 3)
				{
					pBuf[j] 	= BMBytes[k + 2];
					pBuf[j + 1] = BMBytes[k + 1];
					pBuf[j + 2] = BMBytes[k];

					k += 4;
				}
			}
			else if (SwapRGB)
			{
				// Take a copy of this scanline and reverse the RGB ordering so that
				// PostScript can use the data directly.
				for (INT32 j = 0; j < Width; j += 3)
				{
					pBuf[j] 	= BMBytes[Ofs + j + 2];
					pBuf[j + 1] = BMBytes[Ofs + j + 1];
					pBuf[j + 2] = BMBytes[Ofs + j];
				}
			}
			else if (Expand4bpp)
			{
				// Take a copy of this 4bpp scanline and expand it to 8bpp.
				INT32 k = 0;
				for (INT32 j = 0; j < Width; j++)
				{
					pBuf[k++] = (BMBytes[Ofs + j] & 0xF0) >> 4;
					pBuf[k++] = BMBytes[Ofs + j] & 0xF;
				}
			}
			else
				// Just point to raw data
				pBuf = BMBytes + Ofs;

			// Write the data out, possibly encoded in ASCII85
			INT32 nBytes;

			if (UseLevel2)
				// No padding needed for renderable PostScript/EPS.
				nBytes = pDC->OutputASCII85(pBuf, DataSize);
			else
				// Normal ASCII Hex output (or binary in Native files).
				nBytes = pDC->OutputRawBinary(pBuf, DataSize, Padding);

 			if (nBytes == -1)
			{
				// Error - prob out of disc space
				if (pAllocatedMem != NULL)
					CCFree(pAllocatedMem);

				return FALSE;
			}

			// Update progress display
			if (pFilter != NULL)
				pFilter->UpdateExportedNodeCount(1);

			// Do next line
			Ofs += ScanlineSize;
		}

		if (UseLevel2)
		{
			if (!pDC->EndASCII85Output())
				// Error
				return FALSE;
		}

		// Free the temp buffer if necessary.
		if (pAllocatedMem != NULL)
			CCFree(pAllocatedMem);
	}
#endif
	// If we got here, no errors occured.
	return TRUE;
}



/********************************************************************************************

>	virtual BOOL CWxBitmap::ExportSeparatedData(RenderRegion *pRegion, BYTE *SepTables) const
					
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/96

	Inputs:		pRegion - The Postscript RenderRegion to export to. In fact, if this is
				not IsKindOf(CamelotEPSRenderRegion), this function will do nothing and
				return TRUE.

				SepTables - See ExportBitmapData. This function should ONLY be called
				with a valid SepTabls pointer, from ExportBitmapData.

	Returns:	TRUE for success

	Purpose:	Exports this bitmap's image data to the given RenderRegion, colour
				separating to an 8bpp image as it does so.

				This function is the colour-separating analogue of ExportBitmapData.

				It is only (currently) used when printing to postscript.

	Notes:		Separated bitmap data is ALWAYS in the form of a greyscale 8 bits per pixel

				This function can handle any input bitmap depth (1/4/8/24/32) which
				CWxBitmap::ReadPixel supports.

				This function may only be called when:
					SepTables != NULL
					pRegion->GetRenderView()->GetColourPlate()->GetType() is a separation
					plate (C/M/Y/K or Spot).

				Implementation: We use ReadScanline32bpp to get the bitmap data in a
				generic 32bpp format, then pass a generic colour separation engine over
				it to generate a generic 8bpp greyscale separated bitmap. We could be
				more efficient for low bpp bitmaps, but this would just introduce more
				special-cases, and more chance of it not working in some circumstances.

	SeeAlso:	CWxBitmap::ExportBitmapData

********************************************************************************************/

BOOL CWxBitmap::ExportSeparatedData(RenderRegion *pRegion, BYTE *SepTables)
{
#ifndef STANDALONE
	ERROR3IF(pRegion == NULL || SepTables == NULL, "Illegal NULL params");

	if (!pRegion->IsKindOf(CC_RUNTIME_CLASS(CamelotEPSRenderRegion)))
	{
		ERROR3("Not a CamelotEPSRenderRegion derived RenderRegion - bitmap not exported");
		return(TRUE);		// Return success - don't shaft the whole print just for one bitmap
	}

RebuildXPEBitmap();
	KernelDC *pDC = (KernelDC *) CCDC::ConvertFromNativeDC(pRegion->GetRenderDC());
	Filter *pFilter = pDC->GetParentFilter();

	// If it is renderable, find out if we can use Level 2 features.
	BOOL UseLevel2 = FALSE;
	BOOL Renderable = (IS_A(pRegion, CamelotEPSRenderRegion) || pRegion->IsPrinting());

	if (Renderable)
	{
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
		if (pRegion->IsPrinting())
		{
			// Get the print info for this job.
			CCPrintInfo *pInfo = CCPrintInfo::GetCurrent();
			if (pInfo != NULL)
			{
				PrintControl *pPrCtrl = pInfo->GetPrintControl();
				UseLevel2 = (pPrCtrl->GetPSLevel() == PSLEVEL_2);
			}
		}
		else
#endif //webster

			UseLevel2 = (EPSFilter::XSEPSExportPSType == 2);			// Use the EPS preference.
	}

	// Export the bitmap pixel data itself.
	if (!Renderable)
		pDC->OutputToken(_T("%Camelot: Bitmap data"));
	pDC->OutputNewLine();
	
	// Get some useful constants...
//	const INT32 BitsPerPixel	= BMInfo->bmiHeader.biBitCount;
	const INT32 PixelWidth	= BMInfo->bmiHeader.biWidth;
	const INT32 PixelHeight	= BMInfo->bmiHeader.biHeight;
	const INT32 ByteWidth	= PixelWidth;

	// If we have to change the scanlines in any way, we'll use a temporary scanline buffer
	// which can hold a full scanline of 8bpp pixels
	BYTE *pOutputBuffer = (BYTE *) CCMalloc(ByteWidth);
	if (pOutputBuffer == NULL)
	{
		ERROR3("Scanline buffer allocation failed");
		return(FALSE);
	}

	Pixel32bpp *pInputBuffer = (Pixel32bpp *) CCMalloc(PixelWidth * sizeof(Pixel32bpp));
	if (pInputBuffer == NULL)
	{
		ERROR3("No memory for 32bpp separation scanline");
		return(FALSE);
	}

	// If we are going to do ASCII85, get ready for it.
	if (UseLevel2 && !pDC->StartASCII85Output(TRUE))  // TRUE => RLE please
	{
		ERROR3("StartASCII85Output failed");
		return(FALSE);
	}

	// If separating, find a colour context for the separation to use
	ColourContext *OutputContext = NULL;
	if (SepTables != NULL)
	{
		ERROR3IF(pRegion->GetRenderView() == NULL, "Darn");
		OutputContext = pRegion->GetRenderView()->GetColourContext(COLOURMODEL_RGBT);

		ERROR3IF(OutputContext == NULL, "Double Darn");
	}

	INT32 nBytes;
	for (INT32 y = 0; y < PixelHeight; y++)
	{
		GetScanline32bpp(y, TRUE, pInputBuffer);
		ColourSeparateScanline32to8(OutputContext, SepTables, pOutputBuffer, pInputBuffer, PixelWidth);

		// Write the data out, possibly encoded in ASCII85
		if (UseLevel2)
			nBytes = pDC->OutputASCII85(pOutputBuffer, ByteWidth);
		else
			nBytes = pDC->OutputRawBinary(pOutputBuffer, ByteWidth, (Renderable) ? 1 : 4);

 		if (nBytes == -1)
		{
			ERROR3("EPS output failed - Out of disc space?");
			CCFree(pOutputBuffer);
			return(FALSE);
		}

		// Update progress display
		if (pFilter != NULL)
			pFilter->UpdateExportedNodeCount(1);
	}

	CCFree(pInputBuffer);
	CCFree(pOutputBuffer);

	if (UseLevel2 && !pDC->EndASCII85Output())
	{
		ERROR3("EndASCII85Output failed");
		return(FALSE);
	}
	return(TRUE);
#else
	ERROR2(FALSE,"CWxBitmap::SeparateBitmapData being called in Viewer version!");
#endif
}


/********************************************************************************************

>	virtual BOOL CWxBitmap::GetScanline32bpp(UINT32 YPos, BOOL UsePalette,
											Pixel32bpp *Scanline) const;

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/96

	Inputs:		YPos - The y position of the scanline in the bitmap
				UsePalette -Ignored for deep bitmaps
							For <=8bpp bitmaps, if this value is:
								FALSE, the bitmap's palette will be ignored, and the actual pixel
								(palette index) value will be returned as a greyscale RGB value
								TRUE, the pixel index value will be used to retrieve the 24bpp
								palette entry for the pixel, if a palette is available.

	Outputs:	Scanline - will be filled in with 32bpp pixel values on return

	Returns:	TRUE for success

	Purpose:	Copies a scanline out of this bitmap into the given 32bpp buffer.
				The buffer must be no less than CWxBitmap::GetWidth() entries wide.

				The scanline will be converted up from 1, 4, 8, 24, or 32bpp into
				the generic Pixel32bpp format.

	Notes:		This function basically just calls ReadPixel for all pixels in the scanline

	SeeAlso:	CWxBitmap::ReadPixel32bpp

********************************************************************************************/

BOOL CWxBitmap::GetScanline32bpp(UINT32 YPos, BOOL UsePalette, Pixel32bpp *Scanline)
{
RebuildXPEBitmap();
	ERROR3IF(YPos < 0 || YPos > GetHeight(), "Illegal Y position");
	ERROR3IF(Scanline == NULL, "Illegal NULL param");

	for (UINT32 XPos = 0; XPos < GetWidth(); XPos++)
		Scanline[XPos] = ReadPixel32bpp(XPos, YPos, UsePalette);

	return(TRUE);
}

/********************************************************************************************

>	virtual wxImage * CWxBitmap::MakewxImage(UINT32 ImageWidth=0, UINT32 ImageHeight=0, BOOL UsePalette=TRUE) const;

	Author:		Alex Bligh
	Created:	26/04/2006

	Inputs:		ImageWidth - Width of image to make (or zero for width of bitmap)
				ImageHeight - Depth of image to make (or zero for depth of bitmap)
				UsePalette -Ignored for deep bitmaps
							For <=8bpp bitmaps, if this value is:
								FALSE, the bitmap's palette will be ignored, and the actual pixel
								(palette index) value will be returned as a greyscale RGB value
								TRUE, the pixel index value will be used to retrieve the 24bpp
								palette entry for the pixel, if a palette is available.

	Outputs:	None

	Returns:	A pointer to a wxImage, or NULL for failure

	Purpose:	Makes a wxBitmap, slowly

				The scanline will be converted up from 1, 4, 8, 24, or 32bpp into
				the generic Pixel32bpp format.

	Notes:		This function basically just calls ReadPixel for all pixels in the scanline

	SeeAlso:	CWxBitmap::ReadPixel32bpp

********************************************************************************************/

wxImage * CWxBitmap::MakewxImage(UINT32 ImageWidth, UINT32 ImageHeight, BOOL UsePalette)
{
	if (!ImageWidth)
		ImageWidth=GetWidth();
	if (!ImageHeight)
		ImageHeight=GetHeight();

	wxImage * pImage = new wxImage(GetWidth(), GetHeight(), /*TYPENOTE: Correct*/ false);
	if (!pImage)
		return NULL;
#if 0
	pImage->SetAlpha();
	if (!pImage->HasAlpha())
	{
		delete pImage;
		return NULL;
	}
#endif

	RebuildXPEBitmap();

	double xstep = GetWidth()/ImageWidth;
	double ystep = GetHeight()/ImageHeight;
	double x=0;
	double y=0;

	for (UINT32 YPos = 0; YPos < ImageHeight; YPos++)
	{
		UINT32 YP=(UINT32)(y+0.5);
		x=0;
		for (UINT32 XPos = 0; XPos < ImageWidth; XPos++)
		{
			UINT32 XP=(UINT32)(x+0.5);
			Pixel32bpp p = ReadPixel32bpp(XP, YP, UsePalette);
			pImage->SetRGB(XPos, YPos, p.Red, p.Blue, p.Green);
//			pImage->SetAlpha(XPos, YPos, 255-p.Alpha); // wxImage alpha has 0 for fully transparent

			x+=xstep;
		}
		y+=ystep;
	}

	return(pImage);
}


/********************************************************************************************

>	void CWxBitmap::ColourSeparateScanline32to8(ColourContext *OutputContext, BYTE *SepTables,
												BYTE *DestBuffer, BYTE *SrcBuffer,
												INT32 PixelWidth)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/96

	Inputs:		DestBuffer	- points to a destination 24bpp RGB scanline buffer
				SrcBuffer	- points to a source 24bpp RGB scanline buffer
				PixelWidth	- The number of pixels to copy from Src to Dest

	Purpose:	Copies the scanline of 32bpp BGR values from SrcBuffer to DestBuffer,
				running the pixels through colour separation tables en route.
				NOTE that Src and Dest may point at the same buffer to convert in-place.

				The output pixels are produced in 8bpp greyscale (0=black, 255=white)
				format, and will occupy only the first PixelWidth bytes of the Dest buffer.

				If you're not colour-separating, then you should replace this function
				call with a simple memcpy or equivalent.

	Notes:		ONLY handles Pixel32bpp scanlines, and expects that SepTables points
				at the separation tables.

				The CMYK separation plates could have been written generically, but 
				I have separated out the cases in order to squeeze a bit of speed out
				of the inner pixel conversion loop.

********************************************************************************************/

void CWxBitmap::ColourSeparateScanline32to8(ColourContext *OutputContext, BYTE *SepTables,
												BYTE *DestBuffer, Pixel32bpp *SrcBuffer,
												INT32 PixelWidth)
{
	ERROR3IF(DestBuffer == NULL || SrcBuffer == NULL || PixelWidth < 1, "Illegal params");
	ERROR3IF(SepTables == NULL, "No separation tables!?");
	ERROR3IF(OutputContext == NULL || OutputContext->GetColourPlate() == NULL,
				"The separation ColourPlate has gone missing!");

//RebuildXPEBitmap();
	// --- A wee macro to find the Maximum of R,G,B, and place it into a variable called 
	// Temp, which we define right now in function scope so it's always available
	// (It's undefined at the end of this function)
	BYTE Temp;
	#define TEMPMAX(R,G,B)					\
	{										\
		Temp = ((R) > (G))  ? (R) : (G);	\
		Temp = (Temp > (B)) ? Temp : (B);	\
	}


	// --- Izzy, Wizzy, let's get busy
	INT32 i;									// Loop Index
	BYTE Ink;								// Temporary variable for storing the ink value in

	BYTE *UCR = SepTables + 768 + 255;		// UCR is the 4th table, but this points at the
											// END of the table for optimisation - precalcs (255 - Temp)

	ColourPlateType Plate = OutputContext->GetColourPlate()->GetType();
	BOOL bNegate = OutputContext->GetColourPlate()->IsNegative();
	BYTE *LUT = NULL;

	switch (Plate)
	{
		case COLOURPLATE_SPOT:
			for (i = 0; i < PixelWidth; i++)
				DestBuffer[i] = 255;
			return;
			break;
	
		case COLOURPLATE_CYAN:
			LUT = SepTables + 0;				// Cyan table is the 1st table
			break;

		case COLOURPLATE_MAGENTA:
			LUT = SepTables + 256;				// Magenta table is the 2nd table
			break;

		case COLOURPLATE_YELLOW:
			LUT = SepTables + 512;				// Yellow table is the 3rd table
			break;

		default:
			LUT = SepTables + 1024;				// Black generation is the 5th table
			break;
	}

PORTNOTE("cms", "Disabled XaraCMS")
#ifndef EXCLUDE_FROM_XARALX
	BOOL PrintRGBBlackAsKey = XaraCMS::PrintRGBBlackAsKey;
#else
	BOOL PrintRGBBlackAsKey = TRUE;
#endif

	for (i = 0; i < PixelWidth; i++)
	{
		// It's colour, so get the maximum of R,G,B into Temp
		TEMPMAX(SrcBuffer[i].Red, SrcBuffer[i].Green, SrcBuffer[i].Blue);

		if (!PrintRGBBlackAsKey || Temp > 0)
		{
			switch (Plate)
			{
				case COLOURPLATE_CYAN:
					Ink = 255 - SrcBuffer[i].Red;		// Cyan ink is (255 - Red)
					if (Ink>UCR[-Temp])
						Ink -= UCR[-Temp];
					else
						Ink=0;
					break;

				case COLOURPLATE_MAGENTA:
					Ink = 255 - SrcBuffer[i].Green;		// Magenta ink is (255 - Green)
					if (Ink>UCR[-Temp])
						Ink -= UCR[-Temp];
					else
						Ink=0;
					break;

				case COLOURPLATE_YELLOW:
					Ink = 255 - SrcBuffer[i].Blue;		// Yellow ink is (255 - Blue)
					if (Ink>UCR[-Temp])
						Ink -= UCR[-Temp];
					else
						Ink=0;
					break;

				default:
					Ink = 255 - Temp;					// Black ink is (255 - Temp)
					break;
			}

			Ink = LUT[Ink];
		}
		else
		{
			Ink = (Plate == COLOURPLATE_KEY) ? 255 : 0;
		}

		if (bNegate)
			DestBuffer[i] = Ink;
		else
			DestBuffer[i] = 255 - Ink;
	} 
	// And finally, vape our helper macro
	#undef TEMPMAX
}


/********************************************************************************************

>	void CWxBitmap::ColourSeparate32to32(ColourContext *OutputContext, BYTE *SepTables)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com> (from ColourSeparateScanline32to8)
	Created:	02/03/2004

	Inputs:		OutputContext	- The context to get the separation plate from
				SepTables		- points to the separation tables

	Purpose:	Converts the bitmap in-place from full colour 32 bpp RGB to 
				the relevant separation returning the result as 32 bpp with correct
				colours for the specified plate

********************************************************************************************/

void CWxBitmap::ColourSeparate32to32(ColourPlate *pPlate, BYTE *SepTables)
{
	ERROR3IF(SepTables == NULL, "No separation tables!?");
	ERROR3IF(pPlate == NULL, "The separation ColourPlate has gone missing!");

	RebuildXPEBitmap();
	
	ERROR3IF(GetBPP() != 32, "Bitmap not 32bpp!");

	DWORD NumPixels = GetWidth() * GetHeight();
	BGRT* pPixel = (BGRT*)GetBitmapBits();

	// --- A wee macro to find the Maximum of R,G,B, and place it into a variable called 
	// Temp, which we define right now in function scope so it's always available
	// (It's undefined at the end of this function)
	BYTE Temp;
	#define TEMPMAX(R,G,B)					\
	{										\
		Temp = ((R) > (G))  ? (R) : (G);	\
		Temp = (Temp > (B)) ? Temp : (B);	\
	}

	// --- Izzy, Wizzy, let's get busy
	DWORD i;									// Loop Index
	BYTE Ink;								// Temporary variable for storing the ink value in

	BYTE *UCR = SepTables + 768 + 255;		// UCR is the 4th table, but this points at the
											// END of the table for optimisation - precalcs (255 - Temp)

	ColourPlateType Plate = pPlate->GetType();
	ColourPlateType OutputPlate = pPlate->IsMonochrome() ? COLOURPLATE_KEY : Plate;
	BOOL bNegate = pPlate->IsNegative();
	BYTE *LUT = NULL;

	switch (Plate)
	{
		case COLOURPLATE_CYAN:
			LUT = SepTables + 0;				// Cyan table is the 1st table
			break;

		case COLOURPLATE_MAGENTA:
			LUT = SepTables + 256;				// Magenta table is the 2nd table
			break;

		case COLOURPLATE_YELLOW:
			LUT = SepTables + 512;				// Yellow table is the 3rd table
			break;

		default:
			LUT = SepTables + 1024;				// Black generation is the 5th table
			break;
	}

PORTNOTE("cms", "Disabled XaraCMS")
#ifndef EXCLUDE_FROM_XARALX
	BOOL PrintRGBBlackAsKey = XaraCMS::PrintRGBBlackAsKey;
#else
	BOOL PrintRGBBlackAsKey = TRUE;
#endif

	for (i = 0; i < NumPixels; i++)
	{
		// It's colour, so get the maximum of R,G,B into Temp
		TEMPMAX(pPixel[i].Red, pPixel[i].Green, pPixel[i].Blue);

		if (!PrintRGBBlackAsKey || Temp > 0)
		{
			switch (Plate)
			{
				case COLOURPLATE_CYAN:
					Ink = 255 - pPixel[i].Red;		// Cyan ink is (255 - Red)
					if (Ink>UCR[-Temp])
						Ink -= UCR[-Temp];
					else
						Ink=0;
					break;

				case COLOURPLATE_MAGENTA:
					Ink = 255 - pPixel[i].Green;	// Magenta ink is (255 - Green)
					if (Ink>UCR[-Temp])
						Ink -= UCR[-Temp];
					else
						Ink=0;
					break;

				case COLOURPLATE_YELLOW:
					Ink = 255 - pPixel[i].Blue;		// Yellow ink is (255 - Blue)
					if (Ink>UCR[-Temp])
						Ink -= UCR[-Temp];
					else
						Ink=0;
					break;

				default:
					Ink = 255 - Temp;					// Black ink is (255 - Temp)
					break;
			}

			Ink = LUT[Ink];
		}
		else
		{
			Ink = (Plate == COLOURPLATE_KEY) ? 255 : 0;
		}

		if (bNegate)
			Ink = 255 - Ink;

		switch (OutputPlate)
		{
			case COLOURPLATE_CYAN:
				pPixel[i].Red = 255 - Ink;
				pPixel[i].Green = 255;
				pPixel[i].Blue = 255;
				break;

			case COLOURPLATE_MAGENTA:
				pPixel[i].Red = 255;
				pPixel[i].Green = 255 - Ink;
				pPixel[i].Blue = 255;
				break;

			case COLOURPLATE_YELLOW:
				pPixel[i].Red = 255;
				pPixel[i].Green = 255;
				pPixel[i].Blue = 255 - Ink;
				break;

			default:
				pPixel[i].Red = 255 - Ink;
				pPixel[i].Green = 255 - Ink;
				pPixel[i].Blue = 255 - Ink;
				break;
		}
	} 
	// And finally, vape our helper macro
	#undef TEMPMAX
}


/********************************************************************************************

>	CWxBitmap* CWxBitmap::MakeSeparatedCopy(ColourPlate *pPlate, BYTE *SepTables)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/03/2004
	Returns:	pointer to a copy of this bitmap, NULL if it fails
	Purpose:	To make an colour separated copy of this bitmap


********************************************************************************************/

OILBitmap* CWxBitmap::MakeSeparatedCopy(ColourPlate *pPlate, BYTE *SepTables)
{
	ERROR3IF(SepTables == NULL, "No separation tables!?");
	ERROR3IF(pPlate == NULL, "The separation ColourPlate has gone missing!");

	if (SepTables == NULL || pPlate == NULL)
		return(NULL);

	RebuildXPEBitmap();
	// check to see that theres something to copy
	if (BMInfo == NULL || BMBytes == NULL)
	{
		ERROR3("This bitmap has no bits or info to copy!");
		return NULL;
	}

	LPBYTE pNewBits = NULL;
	LPBITMAPINFO pNewInfo = NULL;

	pNewInfo = AllocDIB(BMInfo->bmiHeader.biWidth, BMInfo->bmiHeader.biHeight, 32, &pNewBits);

	if (pNewInfo == NULL || pNewBits == NULL)
	{
		ERROR3("Failed to allocate bitmap structures");
		return NULL;
	}

	Pixel32bpp *pScanline = (Pixel32bpp *) CCMalloc(BMInfo->bmiHeader.biWidth * sizeof(Pixel32bpp));
	if (pScanline == NULL)
		return(NULL);
	BGRT* pPixel = (BGRT*)pNewBits;

	// --- A wee macro to find the Maximum of R,G,B, and place it into a variable called 
	// Temp, which we define right now in function scope so it's always available
	// (It's undefined at the end of this function)
	BYTE Temp;
	#define TEMPMAX(R,G,B)					\
	{										\
		Temp = ((R) > (G))  ? (R) : (G);	\
		Temp = (Temp > (B)) ? Temp : (B);	\
	}

	// --- Izzy, Wizzy, let's get busy
	INT32 scan;
	INT32 i;								// Loop Index
	BYTE Ink;								// Temporary variable for storing the ink value in

	BYTE *UCR = SepTables + 768 + 255;		// UCR is the 4th table, but this points at the
											// END of the table for optimisation - precalcs (255 - Temp)

	ColourPlateType Plate = pPlate->GetType();
	ColourPlateType OutputPlate = pPlate->IsMonochrome() ? COLOURPLATE_KEY : Plate;
	BOOL bNegate = pPlate->IsNegative();
	BYTE *LUT = NULL;

	switch (Plate)
	{
		case COLOURPLATE_CYAN:
			LUT = SepTables + 0;				// Cyan table is the 1st table
			break;

		case COLOURPLATE_MAGENTA:
			LUT = SepTables + 256;				// Magenta table is the 2nd table
			break;

		case COLOURPLATE_YELLOW:
			LUT = SepTables + 512;				// Yellow table is the 3rd table
			break;

		default:
			LUT = SepTables + 1024;				// Black generation is the 5th table
			break;
	}

PORTNOTE("cms", "Disabled XaraCMS")
#ifndef EXCLUDE_FROM_XARALX
	BOOL PrintRGBBlackAsKey = XaraCMS::PrintRGBBlackAsKey;
#else
	BOOL PrintRGBBlackAsKey = TRUE;
#endif

	for (scan = 0; scan < BMInfo->bmiHeader.biHeight; scan++)
	{
		GetScanline32bpp(scan, TRUE, pScanline);
		for (i = 0; i < BMInfo->bmiHeader.biWidth; i++)
		{
			// It's colour, so get the maximum of R,G,B into Temp
			TEMPMAX(pScanline[i].Red, pScanline[i].Green, pScanline[i].Blue);

			if (!PrintRGBBlackAsKey || Temp > 0)
			{
				switch (Plate)
				{
					case COLOURPLATE_CYAN:
						Ink = 255 - pScanline[i].Red;		// Cyan ink is (255 - Red)
						if (Ink>UCR[-Temp])
							Ink -= UCR[-Temp];
						else
							Ink=0;
						break;

					case COLOURPLATE_MAGENTA:
						Ink = 255 - pScanline[i].Green;		// Magenta ink is (255 - Green)
						if (Ink>UCR[-Temp])
							Ink -= UCR[-Temp];
						else
							Ink=0;
						break;

					case COLOURPLATE_YELLOW:
						Ink = 255 - pScanline[i].Blue;		// Yellow ink is (255 - Blue)
						if (Ink>UCR[-Temp])
							Ink -= UCR[-Temp];
						else
							Ink=0;
						break;

					default:
						Ink = 255 - Temp;					// Black ink is (255 - Temp)
						break;
				}

				Ink = LUT[Ink];
			}
			else
			{
				Ink = (Plate == COLOURPLATE_KEY) ? 255 : 0;
			}

			if (bNegate)
				Ink = 255 - Ink;

			switch (OutputPlate)
			{
				case COLOURPLATE_CYAN:
					pPixel[i].Red = 255 - Ink;
					pPixel[i].Green = 255;
					pPixel[i].Blue = 255;
					break;

				case COLOURPLATE_MAGENTA:
					pPixel[i].Red = 255;
					pPixel[i].Green = 255 - Ink;
					pPixel[i].Blue = 255;
					break;

				case COLOURPLATE_YELLOW:
					pPixel[i].Red = 255;
					pPixel[i].Green = 255;
					pPixel[i].Blue = 255 - Ink;
					break;

				default:
					pPixel[i].Red = 255 - Ink;
					pPixel[i].Green = 255 - Ink;
					pPixel[i].Blue = 255 - Ink;
					break;
			}
			pPixel[i].Transparency = pScanline[i].Alpha;
		}
		pPixel += BMInfo->bmiHeader.biWidth;		// Pointer arithmetic!!!
	}

	// And finally, vape our helper macro
	#undef TEMPMAX

	CWxBitmap* pNewBitmap = new CWxBitmap(pNewInfo, pNewBits);

	CCFree(pScanline);

	if (pNewBitmap == NULL)
	{
		ERROR3("Failed to allocate new CWxBitmap");
		FreeDIB(pNewInfo, pNewBits);
	}
	return pNewBitmap;
}



/********************************************************************************************

>	virtual BOOL CWxBitmap::WritePalette(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/96
	Inputs:		pFilter			- the filter to use to write the data.
	Outputs:	-
	Returns:	True if data saved ok, False otherwise
	Purpose:	Function used when exporting an 8bpp JPEG into the web file and we need to
				squirt the palette information into the record ready for reuse when the file
				is loaded back in.

********************************************************************************************/

BOOL CWxBitmap::WritePalette(BaseCamelotFilter* pFilter)
{
	ERROR2IF(pFilter == NULL,FALSE,"CWxBitmap::WritePalette null pFilter");
RebuildXPEBitmap();

	BOOL ok = TRUE;

	UINT32 NumberOfPaletteEntries = GetNumPaletteEntries();
	ERROR2IF(NumberOfPaletteEntries == 0 || NumberOfPaletteEntries > 256, FALSE,"Palette entries 0 or > 256!");
	// We will save the number of palette entries as a byte, 0 - 255
	BYTE Entries = NumberOfPaletteEntries - 1;
	if (ok) ok = pFilter->Write(Entries);
	for (UINT32 i = 0; i < NumberOfPaletteEntries; i++)
	{
#if !defined(__WXMSW__)
//TRACEUSER("Phil", _T("WritePalette r=%x g=%x b=%x\n"), BMInfo->bmiColors[i].rgbBlue, BMInfo->bmiColors[i].rgbGreen, BMInfo->bmiColors[i].rgbRed);
		if (ok) ok = pFilter->Write(BMInfo->bmiColors[i].rgbBlue);
		if (ok) ok = pFilter->Write(BMInfo->bmiColors[i].rgbGreen);
		if (ok) ok = pFilter->Write(BMInfo->bmiColors[i].rgbRed);
#else
//TRACEUSER("Phil", _T("WritePalette r=%x g=%x b=%x\n"), BMInfo->bmiColors[i].rgbRed, BMInfo->bmiColors[i].rgbGreen, BMInfo->bmiColors[i].rgbBlue);
		if (ok) ok = pFilter->Write(BMInfo->bmiColors[i].rgbRed);
		if (ok) ok = pFilter->Write(BMInfo->bmiColors[i].rgbGreen);
		if (ok) ok = pFilter->Write(BMInfo->bmiColors[i].rgbBlue);
#endif
	}

	// During the preparation process we told the system we would update by the
	// the number of entries in the palette, so do so
	pFilter->IncProgressBarCount(NumberOfPaletteEntries);

	return ok;
}

/********************************************************************************************

>	virtual BOOL CWxBitmap::Convert24To8(RGBTRIPLE *pPalette, UINT32 NumberOfPaletteEntries)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/96
	Inputs:		pPalette	- pointer to the palette in RGBTIPLE format to use when converting
				NumberOfPaletteEntries - entries in this palette 
	Outputs:	-
	Returns:	True if worked ok, False otherwise
	Purpose:	Converts the OILbitmap from its present 24bpp form into an 8bpp form which
				should closely match the one that was exported.
				
				Assumes that the bitmap is newly loaded in and hence can easily be deleted.

********************************************************************************************/

BOOL CWxBitmap::Convert24To8(RGBTRIPLE *pPalette, UINT32 NumberOfPaletteEntries)
{
	ERROR2IF(pPalette == NULL,FALSE,"KernelBitmap::Convert24To8 null pFilter");
	ERROR2IF(NumberOfPaletteEntries == 0,FALSE,"KernelBitmap::Convert24To8 NumberOfPaletteEntries = 0");
RebuildXPEBitmap();

	BOOL ok = TRUE;
TRACEUSER("Neville", wxT("CWxBitmap::Convert24To8 NumberOfPaletteEntries = %d\n"),NumberOfPaletteEntries);

	LPBITMAPINFO pDestBMInfo = NULL;
	LPBYTE pDestBMBytes = NULL;

	// Ask the DIB convert class to do the conversion for us
	ok = DIBUtil::Convert24to8(	BMInfo, BMBytes, &pDestBMInfo, &pDestBMBytes, 
								pPalette, NumberOfPaletteEntries);

	if (ok)
	{
		// Free up the 24bpp version
		FreeDIB( BMInfo, BMBytes );

		// Hot swap it for the 8bpp version
		BMInfo = pDestBMInfo;
		BMBytes = pDestBMBytes;

		pDestBMInfo = NULL;
		pDestBMBytes = NULL;

		ScanLineByteWidth = 0;
		BitmapSize = 0; // in bytes

		CacheGeometry();
	}

	if (pDestBMInfo != NULL)
	{
		delete pDestBMInfo;
	}

	if (pDestBMBytes != NULL)
	{
		delete pDestBMBytes;
	}

	return ok;
}

/********************************************************************************************

>	BOOL CWxBitmap::ImportBitmap(CCLexFile* pFile, BaseBitmapFilter* pBitmapFilter,
								 BaseCamelotFilter* pFilter, BOOL IsCompressed)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/96
	Inputs:		pFile			- the file to read the data from
				pBitmapFilter	- the bitmap filter object to import the bitmap with.
				pFilter			- the BaseCamelotFilter which provides functions like progress update
				IsCompressed	- Flag to say the bitmap is compressed or not.  
	Returns:	TRUE if the bitmap was imported ok;
				FALSE if not.
	Purpose:	Imports a bitmap object using the import filter provided. It is used by the
				web/native filters to pull out a bitmap definition from inside a bitmap
				definition record.
				IsCompressed is only used for BMP/BMPZIP type bitmaps at present.
				Assumes:
					pFile has already been opened up for reading
					pFilter has been set up for reading the data e.g. progress bar 
	Errors:		Out of memory, error in file data.
	SeeAlso:	CWxBitmap::ExportBitmap

********************************************************************************************/

BOOL CWxBitmap::ImportBitmap(CCLexFile* pFile, BaseBitmapFilter* pBitmapFilter,
							 BaseCamelotFilter* pFilter, BOOL IsCompressed)
{
	ERROR2IF(pFile == NULL,FALSE,"CWxBitmap::ImportBitmap null pFile");
	ERROR2IF(pBitmapFilter == NULL,FALSE,"CWxBitmap::ImportBitmap null pBitmapFilter");
	ERROR2IF(pFilter == NULL,FALSE,"CWxBitmap::ImportBitmap null pFilter");

	// Try to import bitmap using the required bitmap filter class.
	// Tell it to import into this CWxBitmap.
	if (pBitmapFilter->ReadFromFile(this, pFilter, pFile, IsCompressed))
	{
		CacheGeometry();
		return TRUE;
	}

	return FALSE;
}



/********************************************************************************************

>	BOOL CWxBitmap::ImportBitmap(Filter *pFilter, const BitmapInfo *pInfo, 
								 INT32 BitmapType)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		pFilter - the filter object to import with.
				pInfo - details of the bitmap to import, or NULL if this should be obtained
						from the file.
				BitmapType - the type of bitmap data in the file.  This is usually 0, and
							 exists as a way of supporting platform-dependent bitmap
							 arrangements on different platforms.
	Returns:	TRUE if the bitmap was imported ok;
				FALSE if not.
	Purpose:	Import a bitmap object using the given bitmap specification and the	
				import filter provided.
	Errors:		Out of memory, error in file data.
	SeeAlso:	CWxBitmap::ExportBitmap

********************************************************************************************/

BOOL CWxBitmap::ImportBitmap(Filter *pFilter, const BitmapInfo *pInfo, INT32 BitmapType)
{
	// Check to see what the specified filter type is and take the appropriate action
	// If a bitmap type of filter (derived off BaseBitmapFilter) e.g. BMPFilter or
	// AccusoftFilters then pInfo will be NULL and we just need to load the bitmap in
	// using the specified filter.
	if (pFilter->IsKindOf(CC_RUNTIME_CLASS(BaseBitmapFilter)))
	{
		BaseBitmapFilter* pBitmapFilter = (BaseBitmapFilter*)pFilter;

		// Try to import bitmap using the required bitmap filter class.
		// Tell it to import into this CWxBitmap.
		if (pBitmapFilter->ReadFromFile(this))
		{
			CacheGeometry();
			return TRUE;
		}
		else
			return FALSE;
	}
	
	if (!pFilter->IsKindOf(CC_RUNTIME_CLASS(CamelotEPSFilter)))
	{
		ENSURE(FALSE, "Trying to import a bitmap with a non-Camelot EPS filter");
		return FALSE;
	}
	
	// Sanity checks
	ENSURE((pInfo->PixelDepth == 1) ||
		   (pInfo->PixelDepth == 4) ||
		   (pInfo->PixelDepth == 8) ||
		   (pInfo->PixelDepth == 24) ||
		   (pInfo->PixelDepth == 32), "Bad bitmap depth in ImportBitmap!");

	if ((pInfo->NumPaletteEntries > 0) && (pInfo->PixelDepth > 8))
	{
		ENSURE(FALSE, "We don't support palettes on bitmaps >8bpp!");
	}

//	if (!IsUserName("Alex"))
//	{
//		ENSURE(BitmapType == 0, "Unknown bitmap type in CWxBitmap::ImportBitmap");
//		if (BitmapType != 0)
//			return FALSE;
//	}

	CamelotEPSFilter *pEPSFilter = (CamelotEPSFilter *) pFilter;

	ENSURE((BMInfo == NULL) && (BMBytes == NULL),
		   "Trying to import into an existing bitmap");

	// Allocate memory for bitmap
	BMInfo = AllocDIB(pInfo->PixelWidth, pInfo->PixelHeight, pInfo->PixelDepth, 
					  &BMBytes, NULL);
	if (BMInfo == NULL)
	{
		// Out of memory
		ENSURE(FALSE, "Failed to allocate memory to import bitmap!");
		return FALSE;
	}

	//BMInfo->bmiHeader.biXPelsPerMeter = Mul32Div32(pInfo->PixelWidth, MILLIS_PER_METRE, 
	//											   pInfo->RecommendedWidth);
	//BMInfo->bmiHeader.biYPelsPerMeter = Mul32Div32(pInfo->PixelHeight, MILLIS_PER_METRE, 
	//											   pInfo->RecommendedHeight);
	// Use the conversion value defined in units.h so that we are consistent
	// Also try and round up as well as down
	double xppm = ((double)pInfo->PixelWidth * M_MP_VAL)/ (double)(pInfo->RecommendedWidth);
	double yppm = ((double)pInfo->PixelHeight * M_MP_VAL)/ (double)(pInfo->RecommendedHeight);
	BMInfo->bmiHeader.biXPelsPerMeter = (INT32)(xppm + 0.5);
	BMInfo->bmiHeader.biYPelsPerMeter = (INT32)(yppm + 0.5);

	if ((BMInfo->bmiHeader.biXPelsPerMeter < 0) || 
		(BMInfo->bmiHeader.biYPelsPerMeter < 0))
	{
		ENSURE(FALSE, "Bad pixel size");
		return FALSE;
	}

	// Header is done; read the palette data if we need to 
	if (((pInfo->PixelDepth <= 8)) && (pInfo->NumPaletteEntries > 0))
	{
		if (!pEPSFilter->ImportBinary((BYTE *) BMInfo->bmiColors, 
									  BMInfo->bmiHeader.biClrUsed * sizeof(RGBQUAD)))
		{
			// Error in hex data
			return FALSE;
		}
	}

	// Set the information up
	CacheGeometry();

	// And read in the image data
//	if (IsUserName("Alex")) if (BitmapType !=0) return(TRUE);

	// Read in the Bitmap info
	if (pFilter->IsKindOf(CC_RUNTIME_CLASS(CamelotNativeEPSFilter)))
	{
		// Load it in scan line at a time please
		INT32 NumScanLines = pInfo->PixelHeight;
		for (INT32 i=0; i<NumScanLines; i++)
		{
			// read a scan line in
			if (!pEPSFilter->ImportBinary(BMBytes + (i*ScanLineByteWidth), ScanLineByteWidth))
				return FALSE;
		}

		// all worked
		return TRUE;
	}
	else
	{
		// Load it all in, in one go.
		return pEPSFilter->ImportBinary(BMBytes, BMInfo->bmiHeader.biSizeImage);
	}

	// Should not get here really.
	return FALSE;
}



/********************************************************************************************

>	virtual void CWxBitmap::PlotPixel(INT32 x, INT32 y, Pixel32bpp NewValue) = 0;
					
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (rewrote Alex's disgusting hack)
	Created:	3/7/96

	Inputs:		x, y - coordinates within the bitmap of the pixel to plot
				NewValue - a 32bpp pixel value.

	Outputs:	(changes attached oil bitmap)
	Returns:	Nothing
	Purpose:	Plots a 32bpp pixel to a deep bitmap (24, or 32bpp)

				Currently unimplemented for 16bpp
				Makes no sense for <= 8bpp

	Notes:		In debug builds, will check all kinds of stuff, but in release
				builds, it assumes that you are providing legal offsets in the bitmap,
				and are only using this call where it will work - this is to reduce
				retail overhead on each pixel operation.

				Will probably fail horribly if you haven't called CacheGeometry
		
	Errors:		ERROR3 if the bitmap is not 16, 24, or 32bpp

********************************************************************************************/

void CWxBitmap::PlotPixel(INT32 x, INT32 y, Pixel32bpp NewValue)
{
RebuildXPEBitmap();
	ERROR3IF(BMInfo == NULL || BMBytes == NULL, "PlotPixel called on duff bitmap");
	ERROR3IF(x < 0 || y < 0 || x >= (INT32)GetWidth() || y >= (INT32)GetHeight(),
				"Illegal pixel position in CWxBitmap::PlotPixel");

	BYTE *ScanlineStart = BMBytes + (y * ScanLineByteWidth);

	switch(BMInfo->bmiHeader.biBitCount)
	{
		case 16:
			ERROR3("Unimplemented code");
			break;

		case 24:
			ScanlineStart[x * 3 + 0] = NewValue.Red;
			ScanlineStart[x * 3 + 1] = NewValue.Green;
			ScanlineStart[x * 3 + 2] = NewValue.Blue;
			break;

		case 32:
			ScanlineStart[x * 4 + 0] = NewValue.Red;
			ScanlineStart[x * 4 + 1] = NewValue.Green;
			ScanlineStart[x * 4 + 2] = NewValue.Blue;
			ScanlineStart[x * 4 + 3] = NewValue.Alpha;
			break;

		default:	
			ERROR3("Unsupported (non-deep) bitmap format in PlotPixel(32bpp)");
			break;
	}
}



/********************************************************************************************

>	virtual void CWxBitmap::PlotPixel(INT32 x, INT32 y, PixelGreyscale NewValue) = 0;
					
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (rewrote Alex's disgusting hack)
	Created:	3/7/96

	Inputs:		x, y - coordinates within the bitmap of the pixel to plot
				NewValue - an 8bpp pixel value, presumed to be a greyscale.

	Outputs:	(changes attached oil bitmap)
	Returns:	Nothing

	Purpose:	Plots an 8bpp (0..255) pixel to any type of bitmap
				The pixel is taken to be a greyscale, and is truncated or expanded
				as appropriate to fit the greyscale value into the destination bitmap
				pixel format.

				(Only 8, 24, and 32bpp formats are currently supported)

	Notes:		In debug builds, will check all kinds of stuff, but in release
				builds, it assumes that you are providing legal offsets in the bitmap,
				and are only using this call where it will work - this is to reduce
				retail overhead on each pixel operation.

				Will probably fail horribly if you haven't called CacheGeometry
		
	Errors:		ERROR3 if the bitmap is not 16, 24, or 32bpp

********************************************************************************************/

void CWxBitmap::PlotPixel(INT32 x, INT32 y, PixelGreyscale NewValue)
{
RebuildXPEBitmap();
	ERROR3IF(BMInfo == NULL || BMBytes == NULL, "PlotPixel called on duff bitmap");
	ERROR3IF(x < 0 || y < 0 || x >= (INT32)GetWidth() || y >= (INT32)GetHeight(),
				"Illegal pixel position in CWxBitmap::PlotPixel");

	BYTE *ScanlineStart = BMBytes + (y * ScanLineByteWidth);

	switch(BMInfo->bmiHeader.biBitCount)
	{
		case 8:
			ScanlineStart[x] = NewValue;
			break;

		case 24:
			ScanlineStart[x * 3 + 0] = \
				ScanlineStart[x * 3 + 1] = \
					ScanlineStart[x * 3 + 2] = NewValue;
			break;

		case 32:
			ScanlineStart[x * 4 + 0] = \
				ScanlineStart[x * 4 + 1] = \
					ScanlineStart[x * 4 + 2] = NewValue;

			ScanlineStart[x * 4 + 3] = 0;
			break;

		default:	
			ERROR3("Unsupported bitmap format in PlotPixel(8bpp)");
			break;
	}
}



/********************************************************************************************

>	virtual Pixel32bpp CWxBitmap::ReadPixel32bpp(INT32 x, INT32 y, BOOL UsePalette = TRUE) const
					
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Inputs:		x, y - coordinates of the pixel to read
				UsePalette -Ignored for deep bitmaps
							For <=8bpp bitmaps, if this value is:
								FALSE, the bitmap's palette will be ignored, and the actual pixel
								(palette index) value will be returned as a greyscale RGB value
								TRUE, the pixel index value will be used to retrieve the 24bpp
								palette entry for the pixel, if a palette is available.

	Returns:	The pixel value of the given pixel, in 32bpp format

	Purpose:	reads a pixel

	Notes:		In debug builds, will check all kinds of stuff, but in release
				builds, it assumes that you are providing legal offsets in the bitmap,
				and are only using this call where it will work - this is to reduce
				retail overhead on each pixel operation.

				NOTE that for <=8bpp bitmaps, the behavior is as follows:
					* If paletted, the value returned is the 24bit RGB looked up in the
					  palette
					* Otherwise, the pixel value is assumed to be a greyscale intensity
					  and is converted into a 24bit RGB where R = G = B = intensity.

	SeeAlso:	CWxBitmap::ReadPixelGreyscale

********************************************************************************************/

inline Pixel32bpp CWxBitmap::ReadPixel32bpp(INT32 x, INT32 y, BOOL UsePalette)
{
RebuildXPEBitmap();
	ERROR3IF(BMInfo == NULL || BMBytes == NULL, "PlotPixel called on duff bitmap");
	ERROR3IF(x < 0 || y < 0 || x >= (INT32)GetWidth() || y >= (INT32)GetHeight(),
				"Illegal pixel position in CWxBitmap::ReadPixel");

	BYTE *ScanlineStart = BMBytes + (y * ScanLineByteWidth);

	Pixel32bpp Pix;
	Pix.Alpha = 0;						// Generally there is no alpha channel, so set it to a default

	if (UsePalette && BMInfo->bmiHeader.biClrUsed < 1)
		UsePalette = FALSE;								// No palette, so can't use one!

	switch(BMInfo->bmiHeader.biBitCount)
	{
		case 1:
			{
				BYTE Temp = ScanlineStart[x >> 3];		// Get the byte containing the pixel (x / 8)
				Temp &= 0x80 >> (x & 7);				// Mask out all bits except the pixel (x % 8)

				// Look up in the palette, if any, else assume it's a greyscale (b&w)
				if (UsePalette)
				{
					Temp = (Temp == 0) ? 0 : 1;
					Pix.Red		= BMInfo->bmiColors[Temp].rgbRed;
					Pix.Green	= BMInfo->bmiColors[Temp].rgbGreen;
					Pix.Blue	= BMInfo->bmiColors[Temp].rgbBlue;
				}
				else
				{
					Temp = (Temp == 0) ? 0 : 255;
					Pix.Red	= Pix.Green	= Pix.Blue = Temp;
				}
			}
			break;

		case 4:
			{
				BYTE Temp = ScanlineStart[x / 2];		// Get the byte containing the pixel

				// Look up in the palette, if any, else assume it's a greyscale
				if (UsePalette)
				{
					// Pull out the pixel's palette index
					if ((x & 1) == 0)
						Temp = (Temp >> 4) & 0x0f;							// High nybble
					else
						Temp &= 0x0f;										// Low nybble

					Pix.Red		= BMInfo->bmiColors[Temp].rgbRed;
					Pix.Green	= BMInfo->bmiColors[Temp].rgbGreen;
					Pix.Blue	= BMInfo->bmiColors[Temp].rgbBlue;
				}
				else
				{
					// And pull out the actual pixel value. Note that we repeat the nybble twice
					// into the final byte to get the correct 8bpp greyscale value (white is FF, not F0!)
					if ((x & 1) == 0)
						Temp = (Temp & 0xf0) | ((Temp & 0xf0) >> 4);		// High nybble
					else
						Temp = (Temp & 0x0f) | ((Temp & 0x0f) << 4);		// Low nybble

					Pix.Red	= Pix.Green	= Pix.Blue = Temp;
				}
			}
			break;

		case 8:
			// Look up in the palette, if any, else assume it's a greyscale
			if (UsePalette)
			{
				Pix.Red		= BMInfo->bmiColors[ScanlineStart[x]].rgbRed;
				Pix.Green	= BMInfo->bmiColors[ScanlineStart[x]].rgbGreen;
				Pix.Blue	= BMInfo->bmiColors[ScanlineStart[x]].rgbBlue;
			}
			else
				Pix.Red	= Pix.Green	= Pix.Blue = ScanlineStart[x];
			break;

		case 24:
			x *= 3;
			Pix.Blue	= ScanlineStart[x++];
			Pix.Green	= ScanlineStart[x++];
			Pix.Red		= ScanlineStart[x++];
			break;

		case 32:
			x *= 4;
			Pix.Blue	= ScanlineStart[x++];
			Pix.Green	= ScanlineStart[x++];
			Pix.Red		= ScanlineStart[x++];
			Pix.Alpha	= ScanlineStart[x++];
			break;

		default:	
			Pix.Red = Pix.Green = Pix.Blue = 0;
			ERROR3("Unsupported bitmap format in ReadPixel(32bpp)");
			break;
	}

	return(Pix);
}


/******************************************************************************************

>	INT32 CWxBitmap::ReturnPaletteIndexUsed( INT32 x, INT32 y ) const

	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>

	Created:	29/09/99

	Inputs:		x, y - The x and y co-ordinates of the pixel under consideration

	Returns:	-1 if something went wrong, otherwise the relevant palette index.

	Purpose:	When the user clicks in, or moves the mouse over a pixel in one of the 
				preview images, this function is called to work out the actual palette 
				index of the colour used.

	Notes:		This function is based on the function ReadPixel32bpp()

******************************************************************************************/

INT32 CWxBitmap::ReturnPaletteIndexUsed( INT32 x, INT32 y )
{
RebuildXPEBitmap();
	ERROR3IF(BMInfo == NULL || BMBytes == NULL, "PlotPixel called on duff bitmap");
	ERROR3IF(x < 0 || y < 0 || x >= (INT32)GetWidth() || y >= (INT32)GetHeight(),
				"Illegal pixel position in CWxBitmap::ReadPixel");

	BYTE *ScanlineStart = BMBytes + (y * ScanLineByteWidth);

	INT32 PaletteIndex;

	switch(BMInfo->bmiHeader.biBitCount)
	{
		case 1:
			{
				BYTE Temp = ScanlineStart[x >> 3];		// Get the byte containing the pixel (x / 8)
				Temp &= 0x80 >> (x & 7);				// Mask out all bits except the pixel (x % 8)
				Temp = (Temp == 0) ? 0 : 1;
				PaletteIndex = Temp;
			}
			break;

		case 4:
			{
				BYTE Temp = ScanlineStart[x / 2];		// Get the byte containing the pixel

				// Pull out the pixel's palette index
				if ((x & 1) == 0)
					Temp = (Temp >> 4) & 0x0f;
				else
					Temp &= 0x0f;

				PaletteIndex = Temp;
			}
			break;

		case 8:
			PaletteIndex = ScanlineStart[x];
			break;

		default:	
			PaletteIndex = -1;
			ERROR3( "Couldn't find the palette index" );
			break;
	}

	return PaletteIndex;
}


/********************************************************************************************

>	inline PixelGreyscale CWxBitmap::ReadPixelGreyscale(INT32 x, INT32 y) const
					
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Inputs:		x, y - coordinates of the pixel to read

	Returns:	The pixel value of the given pixel, as an 8bpp greyscale

	Purpose:	reads a pixel

	Notes:		In debug builds, will check all kinds of stuff, but in release
				builds, it assumes that you are providing legal offsets in the bitmap,
				and are only using this call where it will work - this is to reduce
				retail overhead on each pixel operation.

				Coloured pixels from deep bitmaps will be converted to greyscales
				Pixels from <=8bpp bitmaps will be read directly, and ASSUMED to be in
				a greyscale format (e.g. it does NOT read the palette entry for the
				pixel value and convert that to a greyscale)

	SeeAlso:	CWxBitmap::ReadPixel32bpp

********************************************************************************************/

PixelGreyscale CWxBitmap::ReadPixelGreyscale(INT32 x, INT32 y)
{
RebuildXPEBitmap();
	ERROR3IF(BMInfo == NULL || BMBytes == NULL, "PlotPixel called on duff bitmap");
	ERROR3IF(x < 0 || y < 0 || x >= (INT32)GetWidth() || y >= (INT32)GetHeight(),
				"Illegal pixel position in CWxBitmap::ReadPixel");

	BYTE *ScanlineStart = BMBytes + (y * ScanLineByteWidth);
	PixelGreyscale Pix;

	switch(BMInfo->bmiHeader.biBitCount)
	{
		case 1:
			{
				BYTE Temp = ScanlineStart[x >> 3];		// Get the byte containing the pixel (x / 8)
				Temp &= 0x80 >> (x & 7);				// Mask out all bits except the pixel (x % 8)

				Pix = (Temp == 0) ? 0 : 255;
			}
			break;

		case 4:
			{
				BYTE Temp = ScanlineStart[x / 2];		// Get the byte containing the pixel

				// And pull out the actual pixel value. Note that we repeat the nybble twice
				// into the final byte to get the correct 8bpp greyscale value (white is FF, not F0!)
				if ((x & 1) == 0)
					Temp = (Temp & 0xf0) | ((Temp & 0xf0) >> 4);		// High nybble
				else
					Temp = (Temp & 0x0f) | ((Temp & 0x0f) << 4);		// Low nybble

				Pix = Temp;
			}
			break;

		case 8:
			Pix = ScanlineStart[x];
			break;

		case 24:
			{
				double Temp;

				x *= 3;
				Temp  = 0.109 * (double) ScanlineStart[x++];		// B
				Temp += 0.586 * (double) ScanlineStart[x++];		// G
				Temp += 0.305 * (double) ScanlineStart[x];			// R

				Pix = (BYTE) Temp;
			}
			break;
 
		case 32:
			{
				double Temp;

				x *= 4;
				Temp  = 0.109 * (double) ScanlineStart[x++];		// B
				Temp += 0.586 * (double) ScanlineStart[x++];		// G
				Temp += 0.305 * (double) ScanlineStart[x];			// R
																	// Alpha
				Pix = (BYTE) Temp;
			}
			break;

		default:	
			Pix = 0;
			ERROR3("Unsupported bitmap format in ReadPixel(32bpp)");
			break;
	}

	return(Pix);
}



/********************************************************************************************

>	void CWxBitmap::CacheGeometry
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/94
	Inputs:		None
	Outputs:	Caches scanline widths etc in the CWxBitmap
	Returns:	Nothing
	Purpose:	Sets up the cached info in the header so the plot routines will work fast
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CWxBitmap::CacheGeometry()
{
	if (BMInfo)
	{
		// For now we need to restrict the size of the bitmaps
		// Hopefully a KernelBitmap member along the lines of PostProcessNewBitmap(pDocument)
		// will be provided at some stage to do something better
		// For now restrict it to the length of the shorter A0 side
		// The minimum size is the number of pixels as millipoints
		// This can probably done mathematically as well
		LPBITMAPINFOHEADER pHeader = &(BMInfo->bmiHeader);

		if (pHeader->biXPelsPerMeter != 0 || pHeader->biYPelsPerMeter != 0)
		{
			const double MaxSize	= 0.84;

			const double MaxWidth	= MaxSize;
			const double Width		= pHeader->biWidth;
			const double MinWidth	= Width / M_MP_VAL;
			const double XDensity	= pHeader->biXPelsPerMeter;
			const double RealWidth	= Width / XDensity;
			INT32 XRecommended = (INT32)(96 * (M_MP_VAL / IN_MP_VAL));

			const double MaxHeight	= MaxSize;
			const double Height		= pHeader->biHeight;
			const double MinHeight	= Height / M_MP_VAL;
			const double YDensity	= pHeader->biYPelsPerMeter;
			const double RealHeight = Height / YDensity;
			INT32 YRecommended = (INT32)(96 * (M_MP_VAL / IN_MP_VAL));

			const double AspectRatio = XDensity / YDensity;

			BOOL bIsTooBig		= FALSE;
			BOOL bIsTooSmall	= FALSE;

			// Check the width isn't too small or large
			if (pHeader->biXPelsPerMeter != 0)
			{
				if (RealWidth > MaxWidth)
				{
					XRecommended = (INT32)(pHeader->biWidth / MaxWidth);
					bIsTooBig = TRUE;
				}
				else if (RealWidth < MinWidth)
				{
					XRecommended = (INT32)(pHeader->biWidth / MinWidth);
					bIsTooSmall = TRUE;
				}
			}

			// Check the height is neither too large nor too small, but just right
			if (pHeader->biXPelsPerMeter != 0)
			{
				if (RealHeight > MaxHeight)
				{
					YRecommended = (INT32)(pHeader->biHeight / MaxHeight);
					bIsTooBig = TRUE;
				}
				else if (RealHeight < MinHeight)
				{
					YRecommended = (INT32)(pHeader->biHeight / MinHeight);
					bIsTooSmall = TRUE;
				}
			}
			// This is gorgeous...
			// If it's being awkward pick a density
			if (bIsTooBig || bIsTooSmall)
			{
				if ((bIsTooBig && bIsTooSmall) || AspectRatio != 1.0)
				{
					// It's being really awkward so forget it and use the default
					pHeader->biXPelsPerMeter = 0;
					pHeader->biYPelsPerMeter = 0;
				}
				// From here on we know the original densities were the same.
				// We'll pick the density that gives us the closest to the desired size
				else if ((bIsTooBig && XRecommended > YRecommended) ||
						(bIsTooSmall && XRecommended < YRecommended))
				{
					pHeader->biXPelsPerMeter = XRecommended;
					pHeader->biYPelsPerMeter = XRecommended;
				}
				else
				{
					pHeader->biXPelsPerMeter = YRecommended;
					pHeader->biYPelsPerMeter = YRecommended;
				}
			}
		}

		ScanLineByteWidth = DIBUtil::ScanlineSize( pHeader->biWidth, pHeader->biBitCount );
		BitmapSize = ScanLineByteWidth*(pHeader->biHeight); // in bytes
	}
	else
	{
		ScanLineByteWidth = 0;
		BitmapSize = 0;
	}
}



/********************************************************************************************

>	virtual INT32 CWxBitmap::operator==(const OILBitmap &Other)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		The other bitmap to compare this one with
	Returns:	TRUE, or FALSE.
	Purpose:	Checks to see if one bitmap is the same as another.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 CWxBitmap::operator==(const OILBitmap &Other)
{
	CWxBitmap* OtherBmp = (CWxBitmap*)&Other;
OtherBmp->RebuildXPEBitmap();

	if (BMInfo == NULL || OtherBmp->BMInfo == NULL)
		return FALSE; 	// If either of the Info fields are NULL, then 
						// they are either not the same, or we can't tell !!

	if (BMBytes == NULL || OtherBmp->BMBytes == NULL)
		return FALSE; 	// If either of the Bytes are NULL, then 
						// they are either not the same, or we can't tell !!

	// Lets check the Bitmap Header first
	if (BMInfo->bmiHeader.biSizeImage 	!= OtherBmp->BMInfo->bmiHeader.biSizeImage  ||
		BMInfo->bmiHeader.biWidth 		!= OtherBmp->BMInfo->bmiHeader.biWidth 		||
		BMInfo->bmiHeader.biHeight 		!= OtherBmp->BMInfo->bmiHeader.biHeight 	||
		BMInfo->bmiHeader.biBitCount 	!= OtherBmp->BMInfo->bmiHeader.biBitCount 	||
		BMInfo->bmiHeader.biClrUsed 	!= OtherBmp->BMInfo->bmiHeader.biClrUsed 	||

		BMInfo->bmiHeader.biXPelsPerMeter != OtherBmp->BMInfo->bmiHeader.biXPelsPerMeter ||
		BMInfo->bmiHeader.biYPelsPerMeter != OtherBmp->BMInfo->bmiHeader.biYPelsPerMeter)
	{
		return FALSE;
	}
	
	// Well they looks similar, but the only way to be sure, is to compare the Bits !!

	// First lets check the Palettes
	if (BMInfo->bmiHeader.biBitCount <= 8)
	{
		INT32 NumCols	= BMInfo->bmiHeader.biClrUsed;
		INT32* Pal 		= (INT32*)BMInfo->bmiColors;
		INT32* OtherPal 	= (INT32*)OtherBmp->BMInfo->bmiColors;

		// Is the palette the same ?
		if (memcmp(Pal, OtherPal, NumCols*4) != 0)
			return FALSE;
	}

	// Well the Palettes are the Same .... Lets check the actual data now.

	INT32 Width 	= BMInfo->bmiHeader.biWidth;
	INT32 Height = BMInfo->bmiHeader.biHeight;
	INT32 Bpp 	= BMInfo->bmiHeader.biBitCount;

/*
// Alas, this doesn't work.

	// go and compare 'em
	if ((BMInfo->bmiHeader.biSizeImage != OtherBmp->BMInfo->bmiHeader.biSizeImage) ||
		(memcmp(BMBytes, OtherBmp->BMBytes, BMInfo->bmiHeader.biSizeImage)!=0))
		return FALSE;

	// Blimey !! They're the Same !!
	return TRUE;
*/

	// How many (whole) bytes are there in each scan line ?
	INT32 ScanLineBytes = (Width * Bpp) / 8;

	INT32 ExtraBits = 0;
	BYTE BitMask = 0;

	if (Bpp < 8)
	{
		// For less than 8 bpp the scanlines could
		// end on a sub-byte point
		ExtraBits = (Width * Bpp) - (ScanLineBytes * 8);	// ie.(width * bpp) % 8
		//BitMask = (1<<ExtraBits) - 1;
		const BYTE BitMaskArray[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE};
		ERROR3IF (ExtraBits < 0 || ExtraBits > 7, "CWxBitmap::operator== - extraordinary ExtraBits");
		BitMask = BitMaskArray[ExtraBits];	// NB.ExtraBits == 0 not used
	}

	// The scan lines are always DWord or Word Aligned,
	// So now get the actual bytes need to skip over a scanline
	INT32 ScanAlign = DIBUtil::ScanlineSize(Width, Bpp);

	BYTE* BmpData = BMBytes;
	BYTE* OtherBmpData = OtherBmp->BMBytes;

	for (INT32 Scan = 0; Scan < Height; Scan++)
	{
		// Is this scan line the same ?
		if (memcmp(BmpData, OtherBmpData, ScanLineBytes) != 0)
			return FALSE;

		if (ExtraBits > 0)
		{
			// The scanlines are not exactly byte aligned, so we'll get the DWORD
			// after the whole number of bytes, and mask off the bits that we
			// are interested in
			// Actually as we've just compared ScanLineBytes we need only check the
			// last byte (which is byte aligned)
			BYTE BmpExtraByte = *(BmpData + ScanLineBytes) & BitMask;
			BYTE OtherExtraByte = *(OtherBmpData + ScanLineBytes) & BitMask;
			//INT32 ExtraBmpWord 	= (*((INT32*)(BmpData + ScanLineBytes))) & BitMask;
			//INT32 ExtraOtherWord = (*((INT32*)(OtherBmpData + ScanLineBytes))) & BitMask;

			// Are the extra bits the same ?
			if (BmpExtraByte != OtherExtraByte)
				return FALSE;
		}

		BmpData 		+= ScanAlign;
		OtherBmpData 	+= ScanAlign;
	}

	// Blimey !! They're the Same !!
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL CWxBitmap::LoadBitmap(UINT32 BitmapResourceID)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		The Resource ID of the bitmap to Load
	Returns:	TRUE, or FALSE (Loaded or Failed).
	Purpose:	Loads a bitmap from the resources.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CWxBitmap::LoadBitmap(UINT32 BitmapResourceID)
{
	wxBitmap * pBitmap=CamArtProvider::Get()->FindBitmap((ResourceID)BitmapResourceID);
	if (!pBitmap)
		return FALSE;

	return CreateFromwxBitmap(pBitmap);
}

BOOL CWxBitmap::CreateFromwxBitmap(wxBitmap * pBitmap)
{
	wxImage i = pBitmap->ConvertToImage();
	return CreateFromwxImage(&i);
}

BOOL CWxBitmap::CreateFromwxImage(wxImage * pImage)
{
	LPBITMAPINFO bmInfo;
	LPBYTE bmBytes;

	bmInfo = AllocDIB( pImage->GetWidth(), pImage->GetHeight(), 32, &bmBytes );
	if (!bmInfo || !bmBytes)
		return FALSE;

	BMInfo=bmInfo;
	BMBytes=bmBytes;

	INT32 dpi=96;

	double xppm = ((double)dpi * 10000.0)/254.0;
	bmInfo->bmiHeader.biXPelsPerMeter = (INT32)(xppm + 0.5);
	bmInfo->bmiHeader.biYPelsPerMeter = bmInfo->bmiHeader.biXPelsPerMeter;

	CacheGeometry();

	unsigned char * pData=pImage->GetData();
	unsigned char * pAlpha=NULL;
	if (pImage->HasAlpha())
		pAlpha=pImage->GetAlpha();

	// Reasonably rapid conversion to internal format
	for (UINT32 YPos = 0; YPos < GetHeight(); YPos++)
	{
		// DIBs are the wrong way up
		BYTE *ScanlineStart = BMBytes + ((GetHeight()-YPos-1) * ScanLineByteWidth);
		INT32 off=0;

		for (UINT32 XPos = 0; XPos < GetWidth(); XPos++)
		{
			ScanlineStart[off++] = pData[2]; //Red
			ScanlineStart[off++] = pData[1]; //Green
			ScanlineStart[off++] = pData[0]; //Blue
			pData+=(UINT_PTR)3;
			ScanlineStart[off++] = pAlpha?(255-*(pAlpha++)):0; // Alpha
		}
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL CWxBitmap::GetPaletteUsage(INT32 *pUsageBuf)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/95
	Outputs:	pUsageBuf - array of longs big enough to hold the usage stats of the 
				palette for this bitmap.
	Returns:	TRUE if ok;
				FALSE if an error occured (e.g. this is not a paletted bitmap!)
	Purpose:	Find out how many times each palette entry is used in the bitmap.
				This is currently used when rendering paletted images to PostScript.
	Errors:		Not a paletted bitmap.
	SeeAlso:	CWxBitmap::GetScanlineMask

********************************************************************************************/

BOOL CWxBitmap::GetPaletteUsage(INT32 *UsageBuf)
{
	ERROR3("CWxBitmap::GetPaletteUsage is unimplemented");
	return(FALSE);

/*
	I can't see ANYTHING in this code which is even remotely likely to work!

	---

	// Make sure this is a paletted image
	if ((BMInfo->bmiHeader.biBitCount < 4) ||
		(BMInfo->bmiHeader.biBitCount > 8) ||
		(BMInfo->bmiHeader.biClrUsed == 0))
 	{
		// This bitmap does not have a palette!
		return FALSE;
	}

	CacheGeometry();

	if (BytesPerPixelShift !=-1)
		return FALSE;
			
	if ((BytesPerPixelShift < 0) || (BytesPerPixelShift > 2))
		return FALSE;

	const UINT32 Width  = BMInfo->bmiHeader.biWidth;
	const UINT32 Height = BMInfo->bmiHeader.biHeight;
	INT32 Index = 0;

	// Special case for 4bpp bitmaps
	BOOL Is4bpp = (BMInfo->bmiHeader.biBitCount == 4);

	for (UINT32 x = 0; x < Width; x++)
	{
		for (UINT32 y = 0; y > Height; y++)
		{
			INT32 p = (y * ScanLineByteWidth) + (x << BytesPerPixelShift);

			if (p < BitmapSize)
			{
				if (Is4bpp)
				{
				}
				else
				{
					switch (BytesPerPixelShift)
					{
						case 0:
							Index = (INT32)(*( ((LPBYTE)(BMBytes + p))) <<24 );
							break;

						case 1:
							Index = (INT32)(*( ((LPWORD)(BMBytes + p))) <<16 );
							break;

						case 2:
							Index = (INT32)(*( ((LPDWORD)(BMBytes + p))) );
							break;
					}
				}

				UsageBuf[Index]++;
			}
		}
	}

	ENSURE(FALSE,"CWxBitmap::ReadPixel memory address out of range");
	return(0);
*/
}

BOOL CWxBitmap::PrepareMask(INT32 PaletteEntry)
{
	return FALSE;
}

/********************************************************************************************

>	BOOL CWxBitmap::GetScanlineMaskBits(INT32 ScanlineY, LPBYTE ScanlineBuf)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/95
	Inputs:		PaletteEntry - which entry to build a mask for.
				ScanlineY - the scanline to get the mask for. (0 => bottom line of bitmap)
	Outputs:	ScanlineBuf - where to put the 1bpp mask - this should be large enough to
							  hold one scanline's worth of mask.
	Returns:	TRUE if successful;
				FALSE if not (e.g. this is not a paletted bitmap).
	Purpose:	Given a palette
	Errors:		Not a paletted bitmap
	SeeAlso:	CWxBitmap::GetPaletteUsage

********************************************************************************************/

BOOL CWxBitmap::GetScanlineMaskBits(INT32 ScanlineY, LPBYTE ScanlineBuf)
{
	return TRUE;
}

BOOL CWxBitmap::DestroyMask()
{
	return FALSE;
}


/********************************************************************************************

>	void CWxBitmap::DeleteData(BOOL bResetToDefault = TRUE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/96
	Purpose:	Deletes the actual bitmap data.

********************************************************************************************/

void CWxBitmap::DeleteData(BOOL bResetToDefault)
{
//	ERROR2IF(this == OILBitmap::Default, FALSE, "Someone is trying to delete the default bitmap data");

	FreeDIB( BMInfo, BMBytes );
	BMInfo = NULL;
	BMBytes = NULL;

	if (bResetToDefault)
	{
		BMInfo  = ((CWxBitmap*)OILBitmap::Default)->BMInfo;
		BMBytes = ((CWxBitmap*)OILBitmap::Default)->BMBytes;
		
		ScanLineByteWidth	= ((CWxBitmap*)OILBitmap::Default)->ScanLineByteWidth;
		BitmapSize			= ((CWxBitmap*)OILBitmap::Default)->BitmapSize;

		DestroyGreyscaleVersion();
		DestroyContonePalette();
	}

	return;
}

/********************************************************************************************

>	BOOL CWxBitmap::HasBeenDeleted()


	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/96
	Purpose:	Returns whether or not the actual bitmap data has been deleted.

********************************************************************************************/

BOOL CWxBitmap::HasBeenDeleted()
{ 
	if (this == OILBitmap::Default)
		return FALSE;		// Can't delete the default bitmap

	ERROR3IF(OILBitmap::Default == NULL, "The default bitmap is NULL");
	if (OILBitmap::Default == NULL)
		return TRUE;

	// If we are using the default bitmap data, then we must have been deleted
	return (BMBytes == ((CWxBitmap*)OILBitmap::Default)->BMBytes);
}

/********************************************************************************************

>	void CWxBitmap::GenerateGreyscaleTable()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Purpose:	-

********************************************************************************************/

void CWxBitmap::GenerateGreyscaleTable()
{
RebuildXPEBitmap();
//	if (GetBPP() != 8)
	if (GetBPP()>8)
		return;
	UINT32 bpp = GetBPP() ;

	if (m_pGreyscaleTable != NULL)
		return;

	m_pGreyscaleTable = new BYTE[1<<bpp];
	if (m_pGreyscaleTable == NULL)
		return;

	for (INT32 i=0; i<1<<bpp; i++)
	{
		double R = BMInfo->bmiColors[i].rgbRed;
		double G = BMInfo->bmiColors[i].rgbGreen;
		double B = BMInfo->bmiColors[i].rgbBlue;

		m_pGreyscaleTable[i] = BYTE((R * 0.305) + (G * 0.586) + (B * 0.109));
//		TRACEUSER( "Will", _T("Intensity[%d] = %d\n"), i, m_pGreyscaleTable[i]);
	}
}

/********************************************************************************************

>	BOOL CWxBitmap::IsBrowserPalette() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/06/97
	Returns:	True if the bitmap has a browser compatible palette.
	Purpose:	To see if the palette attached to this bitmap is browser compatible.
				It runs through the colours in a browser palette and checks to see if they
				are all present. The ordering is not checked and so the palette may be
				different from the one that we actually use.

********************************************************************************************/

BOOL CWxBitmap::IsBrowserPalette()
{
	// Not interested in deep bitmaps
	if (GetBPP() != 8)
		return FALSE;

RebuildXPEBitmap();
	UINT32 PaletteEntries = GetNumPaletteEntries();
	if (PaletteEntries < 216)
		return FALSE;
	
	// Borrow the code from PaletteManager::MakePaletteBrowserCompatible
	TRACEUSER( "Neville", wxT("Checking 216 colours\n") );
	BOOL found = FALSE;
	for (INT32 r = 0; r <= 100; r += 20)
	{
		for (INT32 g = 0; g <=100; g += 20)
		{
			for (INT32 b = 0; b <= 100; b += 20)
			{
				BYTE ra = BYTE((double(r)/100)*255.0);
				BYTE ga = BYTE((double(g)/100)*255.0);
				BYTE ba = BYTE((double(b)/100)*255.0);

				for (UINT32 i = 0; i < PaletteEntries && !found; i++)
				{
					found = BMInfo->bmiColors[i].rgbRed   == ra &&
							BMInfo->bmiColors[i].rgbGreen == ga &&
							BMInfo->bmiColors[i].rgbBlue  == ba;
				}

				if (!found)
				{
					TRACEUSER( "Neville", wxT("Colour not found - %d %d %d\n") ,r , g, b );
					return FALSE;
				}
			}
		}
	}

	// reached here so found all 216 colours
	TRACEUSER( "Neville", wxT("216 colours check finished\n") );
	return TRUE;	
}

/********************************************************************************************

>	LPRGBQUAD CWxBitmap::GetPaletteForBitmap()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/07/97
	Returns:	The BITMAPINFO for this bitmap or null.
	Purpose:	To get at the BITMAPINFO palette that this bitmap may have.
				
				BITMAPINFO  consists of:-
				    	BITMAPINFOHEADER    bmiHeader;
						RGBQUAD             bmiColors[1];

********************************************************************************************/

LPBITMAPINFO CWxBitmap::GetBitmapInfo()
{
RebuildXPEBitmap();
	return BMInfo;
}

/********************************************************************************************

>	LPRGBQUAD CWxBitmap::GetPaletteForBitmap()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/07/97
	Returns:	The palette for this bitmap or null.
	Purpose:	To get at the RGBQUAD palette that this bitmap may have.
				
				BITMAPINFO  consists of:-
				    	BITMAPINFOHEADER    bmiHeader;
						RGBQUAD             bmiColors[1];

********************************************************************************************/

LPRGBQUAD CWxBitmap::GetPaletteForBitmap()
{
	// Not interested in deep bitmaps
	if (GetBPP() > 8)
		return NULL;

RebuildXPEBitmap();
	if (BMInfo == NULL)
		return NULL;

	// Return the palette from the BITMAPINFO structure
	return &(BMInfo->bmiColors[0]);
}

/********************************************************************************************

>	LPBITMAPINFOHEADER CWxBitmap::GetBitmapInfoHeader()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/07/97
	Returns:	The bitmap header for this bitmap or null.
	Purpose:	To get at the BITMAPINFOHEADER that this bitmap should have.
				
				BITMAPINFO  consists of:-
				    	BITMAPINFOHEADER    bmiHeader;
						RGBQUAD             bmiColors[1];

********************************************************************************************/

LPBITMAPINFOHEADER CWxBitmap::GetBitmapInfoHeader()
{
RebuildXPEBitmap();
	if (BMInfo == NULL)
		return NULL;

	// Return the BITMAPINFOHEADER for this bitmap
	return &BMInfo->bmiHeader;
}

/********************************************************************************************

>	BOOL CWxBitmap::ArePalettesTheSame(const LPLOGPALETTE pLogPalette, const INT32 TransColour = -1)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/07/97
	Inputs:		pLogPalette		A log palette to check the palette against
				TransColour		The transparent colour in the palette
	Returns:	True if the bitmap has the same palette as the LOG palette.
	Purpose:	To see if the palette attached to this bitmap is browser compatible.
				It runs through the colours in a browser palette and checks to see if they
				are all present. The ordering is not checked and so the palette may be
				different from the one that we actually use.
				Doesn't check the flags byte, just the RGB bytes.

********************************************************************************************/

BOOL CWxBitmap::ArePalettesTheSame(const LPLOGPALETTE pLogPalette, const INT32 TransColour)
{
	ERROR2IF(pLogPalette == NULL,FALSE,"ArePalettesTheSame Bad LOGPALETTE param");

	// Not interested in deep bitmaps
	UINT32 bpp = GetBPP();
	if (bpp != 8)
		return FALSE;

RebuildXPEBitmap();
	// Check the palette of this bitmap against this browser palette
	LPRGBQUAD pPalette = GetPaletteForBitmap();
	if (pPalette == NULL)
		return FALSE;

	UINT32 PaletteEntries = GetNumPaletteEntries();
	if (PaletteEntries != pLogPalette->palNumEntries)
		return FALSE;

	// Check this against our browser palette
	BOOL PaletteOk = TRUE;
	for (INT32 i = 0; i < pLogPalette->palNumEntries; i++)
	{
		// If it is the transparent colour then the entry must be white
		if (i == TransColour)
		{
			if (pPalette[i].rgbRed != 0xFF && 
				pPalette[i].rgbGreen != 0xFF &&
				pPalette[i].rgbBlue != 0xFF)
			{
				PaletteOk = FALSE;
				break;
			}
		}
		else if (pPalette[i].rgbRed != pLogPalette->palPalEntry[i].peRed &&
				 pPalette[i].rgbGreen != pLogPalette->palPalEntry[i].peGreen &&
				 pPalette[i].rgbBlue != pLogPalette->palPalEntry[i].peBlue)
		{
			PaletteOk = FALSE;
			break;
		}
	}

	return PaletteOk;
}



/********************************************************************************************

>	void CWxBitmap::ExtractBitsAndInfo(LPBYTE pBytes, LPBITMAPINFO pInfo)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/2000
	Outputs:	the bytes and bitmapinfo of this bitmap
	Returns:	-
	Purpose:	To extract the bitmap data for use elsewhere, sets them to null so that
				the CWxBitmap object can be deleted

********************************************************************************************/

void CWxBitmap::ExtractBitsAndInfo(LPBYTE* ppBytes, LPBITMAPINFO* ppInfo)
{
RebuildXPEBitmap();
	*ppBytes = BMBytes;
	*ppInfo = BMInfo;

	BMBytes = NULL;
	BMInfo = NULL;
}




/********************************************************************************************

>	void CWxBitmap::ReplaceBitmap(LPBYTE pBytes, LPBITMAPINFO pInfo)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/02/2004
	Inputs:		the bytes and bitmapinfo of the new bitmap
	Returns:	-
	Purpose:	To reset the bitmap stored in this CWxBitmap object

********************************************************************************************/

void CWxBitmap::ReplaceBitmap(LPBYTE pBytes, LPBITMAPINFO pInfo)
{
	if (IsDefaultBitmap())
	{
		ERROR2RAW("ReplaceBitmap asked to replace the default bitmap!");
		return;
	}
	// Do we have valid inputs?
	if (pBytes==NULL && pInfo==NULL)
	{
		ERROR2RAW("Call to ReplaceBitmap with bad parameters");
		return;
	}

	// Remove the old data from memory
	FreeDIB( BMInfo, BMBytes );
	DestroyGreyscaleVersion();
	DestroyContonePalette();

	// Set the new data pointers
	BMInfo = pInfo;
	BMBytes = pBytes;

	// Set the information up
	CacheGeometry();
}


void CWxBitmap::CopyFullyTransparentFrom(OILBitmap* pBitmap)
{
	if (GetWidth() != pBitmap->GetWidth() ||
		GetHeight() != pBitmap->GetHeight() ||
		GetBPP() != 32 ||
		pBitmap->GetBPP() != 32)
	{
		ERROR3("Incompatible bitmaps passed to CopyFullyTransparentFrom");
		return;
	}

	// Pointers to the pixel we are dealing with
	UINT32* pMask = (UINT32*)(pBitmap->GetBitmapBits());
	UINT32* pDest = (UINT32*)GetBitmapBits();

	// Loop through all the pixels
	for (UINT32 j=0; j < pBitmap->GetHeight(); j++)
	{
		for (UINT32 i=0; i < pBitmap->GetWidth(); i++)
		{
			// If this bit is background then set the corresponding pixel 
			// in the output bitmap to fully transparent black
			if (*pMask == 0xFF000000)
				*pDest = 0xFF000000;

			// Move to the next pixel
			pMask++;
			pDest++;
		}
	}
}
