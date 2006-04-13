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

#ifndef INC_GRNDBMP
#define	INC_GRNDBMP

#include "grnddib.h"
#include "dibconv.h"

class OILBitmap;

/********************************************************************************************

>	class GRenderBitmap : public GRenderDIB

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/94
	Purpose:	A GDraw render region that uses normal DIBs for the bitmap and can save itself
				out into an OIL bitmap.

********************************************************************************************/

class GRenderBitmap : public GRenderDIB
{
//	friend class BMPFilter;
//	friend class AccusoftFilters;
//	friend class TI_GIFFilter;
//	friend class PNGFilter;

	CC_DECLARE_DYNAMIC(GRenderBitmap)

public:
	GRenderBitmap(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi, BOOL Printing = FALSE,
				  UINT32 Dither = XARADITHER_ORDERED_GREY, LPLOGPALETTE pPalette = NULL, BOOL AutoConvert = FALSE);
	~GRenderBitmap();

	static void Deinit(void);

public:
	// non-virtual fns which are unique to this class
	OILBitmap *ExtractBitmap(LPLOGPALETTE pPalette = NULL);
	OILBitmap *ExtractBitmapCopy(LPLOGPALETTE pPalette = NULL, 
								 LPBITMAPINFO pMaskInfo = NULL, LPBYTE pMaskData = NULL, INT32 MaskColour = -1,
								 BOOL LookForDuplicates = FALSE);

	// Access function for thr bitmap data
	// Always use this, NEVER access the data directly, as a conversion may be necessary
	// if using certain dithering techniques
	BOOL GetBitmapData(LPBITMAPINFO* pBitmapInfo, LPBYTE* pBitmapData, BOOL Convert = FALSE);

	LPBITMAPINFO GetBitmapInfo() { return pBitmapInfo; }

	BOOL StartRender();

	// Allow access to the default dither type in GRenderRegion
//	static INT32 GetDefaultDitherType();

	static BOOL DitheringNeeds32bpp(UINT32 Dither);

	// banding functions
	virtual BOOL SetFirstBand();
	virtual BOOL GetNextBand();
	INT32 GetNumBands();
	void SetOverlapBands(BOOL NewVal);
	BOOL SetRenderBottomToTop(BOOL NewVal);
	INT32 GetFullRegionHeight();

	// Used to calculate the size of the render region required
	virtual WinRect CalculateWinRect( Matrix& RenderMatrix, const DocRect& docrect,
									  const double dpi);

	// Special functions to make it safe for us to render when called from within another
	// GRenderRegion-derived rndrgn. If we don't use a different context, GDraw will explode
	// violently. This is used to render 8bpp greyscale bitmaps for stroke transparency masks
	// so the context also includes a hard-wired greyscale palette, to elimnate large setup delays.
	// DO NOT CALL THIS UNLESS YOU REALLY KNOW WHAT YOU'RE DOING. Danger, danger, Will Robinson!
	void UseGreyscaleContextDangerous(void);
	void StopUsingGreyscaleContextDangerous(void);

	virtual GMATRIX MakeGavinMatrix(Matrix NewRenderMatrix, DocRect ClipRect, double dPixelsPerInch, BOOL bMasterCapture);

private:
	GDrawContext *pPreviousGDrawContext;	// Memory of the normal GDrawContext we replaced
											// in a call to UseGreyscaleContextDangerous
	static GDrawContext *pGreyscaleContext;	// Context we replace the normal one with

protected:
	LPBITMAPINFO GetLPBits( INT32 Width, INT32 Height, INT32 Depth, LPBYTE*);
	void FreeLPBits( LPBITMAPINFO, LPBYTE );
	BOOL DisplayBits(LPBITMAPINFO lpDisplayBitmapInfo = NULL, LPBYTE lpDisplayBits = NULL);

	BOOL DoOutputBitmapConversion(LPBITMAPINFO* lpConvInfo, LPBYTE* lpConvBits, LPLOGPALETTE pPalette);

	LPLOGPALETTE GetConversionPalette();

// member vars
protected:
	BOOL OverlapBands;
	BOOL RenderBottomToTop;
	// The following are some new bits added to cope with the conversion needed when using certain
	// dithering techniques

	UINT32 DitherType;		// For 8bbp or less, this is the Dithering to use
	UINT32  uOutputDepth;  	// If different from the actual RR bpp, then the bitmap will
							// be converted before output

	LPLOGPALETTE    pConvPalette;	// Palette to use for conversion to 8bpp or less

	// These are used for the final output bitmap, if a conversion is needed
	LPBITMAPINFO	lpOutputInfo;	
	LPBYTE			lpOutputBits;	

	// Preserve the tiny offsets calculated by OSRenderRegion::CalculateGavinOffsetWinRect
	// so that Captures use the same offsets as the master capture
	//
	// Use of these variables is all done inside MakeGavinMatrix and RELIES upon the master
	// capture being created and set up (thus calling MakeGavinMatrix) before any other
	// captures are set up. This causes these variable to be initialised by the setting of
	// the master capture before they are used during the setting of other captures.
	//
	double			m_dXCentralAdjust;
	double			m_dYCentralAdjust;
};

#if !defined(EXCLUDE_FROM_RALPH)

/********************************************************************************************

>	class GRenderOptPalette : public GRenderBitmap

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Purpose:	A GDraw render region that generates an opimised palette for the output bitmap

********************************************************************************************/

class GRenderOptPalette : public GRenderBitmap
{
	CC_DECLARE_DYNAMIC(GRenderOptPalette)

public:
	GRenderOptPalette(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 SrcDepth, double dpi, BOOL Use8bpp);
	~GRenderOptPalette();

	virtual BOOL GetNextBand();

	// Function to read the optimised palette, when rendering is finished
	LPLOGPALETTE GetOptimisedPalette(UINT32 PaletteSize, UINT32 NumColours, UINT32 ReservedColours = 0, BOOL SnapToBrowserPalette = FALSE);

public:
	// Preference as to whether we round the colours of a 1bpp optimised palette
	static BOOL DoTwoColourRoundingToPrimary;

	static BOOL DoGDrawConversion;
	static BOOL UseOldPalette;

protected:

	void* Stats;	// Pointer to Statistics used to generate the optimised palette
	BOOL UseSpecial8bpp;	// Tells us whether to use 8bpp specific routines

	BOOL m_bTooManyColours;
	LPLOGPALETTE m_pExactPalette;
};

#endif

#endif
