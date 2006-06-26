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

// Windows Bitmap object

#ifndef INC_WBITMAP
#define	INC_WBITMAP

//#include "bitmap.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class Filter;
class BitmapInfo;
class BaseBitmapFilter;
class BaseCamelotFilter;
class CCLexFile;
class ColourPlate;

/*********************************************************************************************

>	class CWxBitmap : public OILBitmap

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Purpose:	The Windows implementation of the OILBitmap class. Based on a non-compressed
				DIB. Kernel code should not call on functions in this class except via
				the OILBitmap interface.

**********************************************************************************************/ 

class CWxBitmap : public OILBitmap
{
	CC_DECLARE_DYNAMIC( CWxBitmap )

	friend BOOL OILBitmap::Init();
	friend void OILBitmap::Deinit();
	friend OILBitmap *OILBitmap::Create();
	friend OILBitmap *OILBitmap::Create(UINT32,UINT32,UINT32,UINT32);

public:
	CWxBitmap();
	CWxBitmap(LPBITMAPINFO, LPBYTE);
	~CWxBitmap();

	virtual INT32 operator==(const OILBitmap&);

	void CacheGeometry();

	virtual BOOL GetInfo(BitmapInfo *Info, RenderRegion *pRegion = NULL);

	virtual UINT32 GetWidth();
	virtual UINT32 GetHeight();
	virtual UINT32 GetBPP(RenderRegion *pRegion = NULL);
	virtual UINT32 GetNumPaletteEntries();
	virtual ADDR GetBitmapBits() {	return (ADDR)BMBytes;	}

	virtual UINT32 GetScanlineSize();

	virtual BOOL ExportBitmap(RenderRegion *pRegion);
	virtual BOOL ExportBitmapPalette(RenderRegion *pRegion);
	virtual BOOL ExportBitmapData(RenderRegion *pRegion);

	// For exporting bitmaps to postscript with colour-separation
	virtual BOOL ExportSeparatedData(RenderRegion *pRegion, BYTE *SepTables);
	virtual BOOL ExportSeparatedPalette(RenderRegion *pRegion);

	// Helper function used by ExportSeparatedData - this retrieves one scanline of the
	// bitmap as generic 24bpp RGB data.
	virtual BOOL GetScanline32bpp(UINT32 YPos, BOOL UsePalette, Pixel32bpp *Scanline);
	virtual wxImage * MakewxImage(UINT32 ImageWidth=0, UINT32 ImageHeight=0, BOOL UsePalette=TRUE);

	virtual OILBitmap* MakeCopy(); // returns a copy of this object
protected:
	BOOL ExportBitmapPaletteInternal(RenderRegion *pRegion, INT32 NumColours, RGBQUAD *pSrcCLUT);
			// Shared code used by ExportBitmapPalette and ExportContonePalette


public:		// Outputting bitmap scanlines in colour-separated form
	static void ColourSeparateScanline32to8(ColourContext *CurrentColContext, BYTE *SepTables,
												BYTE *DestBuffer, Pixel32bpp *SrcBuffer,
												INT32 PixelWidth);

	virtual void ColourSeparate32to32(ColourPlate* pPlate, BYTE *SepTables);
	virtual OILBitmap* MakeSeparatedCopy(ColourPlate* pPlate, BYTE *SepTables); // returns a copy of this object separated

	virtual INT32 GetRecommendedWidth();
	virtual INT32 GetRecommendedHeight();
	virtual void SetRecommendedWidth(INT32);
	virtual void SetRecommendedHeight(INT32);

	virtual BOOL SetTransparencyIndex(INT32);
	virtual BOOL GetTransparencyIndex(INT32*);

	virtual BOOL IsTransparent();

public:
	// Function used when exporting an 8bpp JPEG into the web file
	virtual BOOL WritePalette(BaseCamelotFilter *pFilter);
	// Function used when importing that 24bpp JPEG back in and wanting it in 8bpp format
	virtual BOOL Convert24To8(RGBTRIPLE *pPalette, UINT32 NumberOfPaletteEntries);

	// Function used by the native/web filters to pull a bitmap from inside a bitmap
	// definition record
	virtual BOOL ImportBitmap(CCLexFile* pFile, BaseBitmapFilter* pBitmapFilter,
							  BaseCamelotFilter* pFilter, BOOL IsCompressed);
	
	// Function used by the EPS filters to import bitmaps and by the bitmap filter
	// to import a bitmap directly from a bitmap file
	virtual BOOL ImportBitmap(Filter*, const BitmapInfo*, INT32);

 	virtual BOOL LoadBitmap(UINT32 BitmapResourceID);
	virtual BOOL CreateFromwxBitmap(wxBitmap * pBitmap);
	virtual BOOL CreateFromwxImage(wxImage * pImage);

	virtual BOOL GetPaletteUsage(INT32 *);
	virtual BOOL PrepareMask(INT32 PaletteEntry);
	virtual BOOL GetScanlineMaskBits(INT32 ScanlineY, LPBYTE ScanlineBuf);
	virtual BOOL DestroyMask();
	virtual BOOL ExportContonePalette(RenderRegion *);

	virtual void PlotPixel(INT32 x, INT32 y, Pixel32bpp NewValue);
	virtual void PlotPixel(INT32 x, INT32 y, PixelGreyscale NewValue);
	virtual Pixel32bpp ReadPixel32bpp (INT32 x, INT32 y, BOOL UsePalette = TRUE);
	virtual INT32 ReturnPaletteIndexUsed( INT32 x, INT32 y );
	virtual PixelGreyscale ReadPixelGreyscale (INT32 x, INT32 y);

	virtual void DeleteData(BOOL bResetToDefault = TRUE);
	virtual BOOL HasBeenDeleted();

	// retrieves the actual bitmap and sets members to NULL
	void ExtractBitsAndInfo(LPBYTE*, LPBITMAPINFO*);

	virtual void GenerateGreyscaleTable();

	INT32 ScanLineByteWidth;		// Bytes per scanline, including padding (i.e. offset from scanline to next)
	INT32 BitmapSize;			// Memory usage in bytes

	virtual BOOL IsBrowserPalette();
	virtual BOOL ArePalettesTheSame(const LPLOGPALETTE pLogPalette, const INT32 TransColour = -1);

	virtual UINT32 GetHorizontalDPI();
	virtual UINT32 GetVerticalDPI();

	virtual void CopyFullyTransparentFrom(OILBitmap* pBitmap);

	// these are public but MUST NOT be used from kernel code

	LPBITMAPINFO		BMInfo;		// The information on the bitmap
	LPBYTE				BMBytes;	// The actually pixels that make up the bitmap

	// BITMAPINFO  consists of:-
	//    	BITMAPINFOHEADER    bmiHeader;
	//		RGBQUAD             bmiColors[1];
	// So allow access to them
	virtual LPBITMAPINFO		GetBitmapInfo();
	virtual LPRGBQUAD			GetPaletteForBitmap();
	virtual LPBITMAPINFOHEADER	GetBitmapInfoHeader();
	virtual void				ReplaceBitmap(LPBYTE pBytes, LPBITMAPINFO pInfo);
};


// These are some debug macros for detecting if a bitmap pointer is valid or not.

// They work by looking for specific values that the memory gets set to when the object
// gets deleted in a debug build. I'm not entirly sure why they get set to these particular
// values ... but who cares.  It works, and that's all that's important.
#define ERROR3IF_KBMP_PTR_INVALID(ptr, literal)\
		ERROR3IF( !IsValidAddress( ptr, 1 ), literal);
#define ERROR2IF_KBMP_PTR_INVALID(ptr, retvalue, literal)\
		ERROR2IF( !IsValidAddress( ptr, 1 ), retvalue, literal);

#define ERROR3IF_OILBMP_PTR_INVALID(ptr, literal)\
		ERROR3IF( !IsValidAddress( ptr, 1 ), literal );
#define ERROR2IF_OILBMP_PTR_INVALID(ptr, retvalue, literal)\
		ERROR2IF( !IsValidAddress( ptr, 1 ), retvalue, literal);

#endif
