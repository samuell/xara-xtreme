// $Id: ppmfiltr.h 662 2006-03-14 21:31:49Z alex $
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
//

#ifndef INC_PPMFILTR
#define INC_PPMFILTR

#include "bitfilt.h"
#include "outptdib.h"
#include "bmpprefs.h"  	// SelectionType
#include "maskedrr.h"	// MaskedRenderRegion derived off GRenderBitmap
#include "oilfltrs.h"

class KernelBitmap;
//class OILFilter;

enum PMFileType
{
	PPM_BAD,
	PPM_BINARY,
	PPM_ASCII,	
	PGM_BINARY,
	PGM_ASCII,	
	PBM_BINARY,
	PBM_ASCII,	
};	

/********************************************************************************************

>	class BasePMFilter : public BitmapFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Purpose:	Provides most of the the handling for the PPM, PGM and PBM filters.

********************************************************************************************/

class BasePMFilter : public BaseBitmapFilter
{
	CC_DECLARE_DYNAMIC(BasePMFilter);
	
public:
	BasePMFilter();

	BOOL Init();

	virtual INT32 HowCompatible( PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize,
							   UINT32 FileSize );
	// Import related functions
	virtual BOOL ReadFromFile(OILBitmap* pOilBitmap);

protected:
	//
	// Import related functions
	//
	//virtual BOOL PrepareToImport();
	//virtual void CleanUpAfterImport();

	BOOL ReadFromFile( CCLexFile *File, LPBITMAPINFO *Info, LPBYTE *Bits, String_64 *ProgressString = NULL);

	BOOL ReadASCIIFromFile( CCLexFile *pLexFile, LPBYTE pBits,
						    INT32	Width, INT32 Height, INT32 ColoursPerRGB, INT32 BitsPerRGB,
							INT32 WidthOfLine,
						    String_64 *ProgressString = NULL, BOOL Forwards = TRUE);
	BOOL ReadBinaryFromFile( CCLexFile *pFile, LPBYTE pBits,
							 INT32 Width, INT32 Height, INT32 ColoursPerRGB, INT32 BitsPerRGB,
							 INT32 WidthOfLine,
							 String_64 *ProgressString = NULL, BOOL Forwards = TRUE);

	// Useful palette functions
	void PokePaletteEntry(LPRGBQUAD * Palette, BYTE red, BYTE green, BYTE blue);
	void MakeGreyScalePalette(LPRGBQUAD Palette);
	void Make16GreyScalePalette(LPRGBQUAD Palette);
	void MakeBlackAndWhitePalette(LPRGBQUAD Palette);

	// Functions that a filter derived off this must provide
	virtual BOOL CheckString(TCHAR * pHeader);
	virtual BOOL ReadDataIntoBitmap(INT32 Number, INT32 * Count, LPBYTE * pData, BOOL * NextPixel);

	// What the file type we are reading is
	PMFileType TypeOfPPM;

	// This is so we can remember what we thought of the GIF file.
	INT32 PPMHowCompatible;	

	INT32 x;					// current x position across bitmap
	INT32 y;					// current y position down bitmap
	INT32 TotalWidth;			// width in pixels of bitmap
	INT32 TotalHeight;		// height in pixels of bitmap
	INT32 TotalWidthOfLine;	// width of lien rounded to required word boundaries
	INT32 RGBColours;			// number of colours per colour component
	INT32 RGBBits;			// bits per colour component e.g. R, G and B
	INT32 PixelCounter;		// how many pixels we get per read

	INT32 Red;				// red component for 24bit files
	INT32 Green;				// green component for 24bit files
	INT32 Blue;				// blue component for 24bit files

	BYTE Bits;				// accumulator for bits in 1bpp files

public:
	// Check if this Bitmap filter can cope with saving at this Bpp/Colour depth
	virtual BOOL IsThisBppOk(UINT32 Bpp);
};

/********************************************************************************************

>	class PPMFilter : public BitmapFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Purpose:	Encapsulates a Unix PPM import filter.

********************************************************************************************/

class PPMFilter : public BasePMFilter
{
	CC_DECLARE_DYNAMIC(PPMFilter);
	
public:
	PPMFilter();

	BOOL Init();

protected:
	virtual BOOL CheckString(TCHAR * pHeader);
	virtual BOOL ReadDataIntoBitmap(INT32 Number, INT32 * Count, LPBYTE * pData, BOOL * NextPixel);
};

/********************************************************************************************

>	class PPMFilter : public BitmapFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Purpose:	Encapsulates a Unix PGM import filter.

********************************************************************************************/

class PGMFilter : public BasePMFilter
{
	CC_DECLARE_DYNAMIC(PGMFilter);
	
public:
	PGMFilter();

	BOOL Init();

protected:
	virtual BOOL CheckString(TCHAR * pHeader);
	virtual BOOL ReadDataIntoBitmap(INT32 Number, INT32 * Count, LPBYTE * pData, BOOL * NextPixel);
};

/********************************************************************************************

>	class PBMFilter : public BitmapFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Purpose:	Encapsulates a Unix PBM import filter.

********************************************************************************************/

class PBMFilter : public BasePMFilter
{
	CC_DECLARE_DYNAMIC(PBMFilter);
	
public:
	PBMFilter();

	BOOL Init();

protected:
	virtual BOOL CheckString(TCHAR * pHeader);
	virtual BOOL ReadDataIntoBitmap(INT32 Number, INT32 * Count, LPBYTE * pData, BOOL * NextPixel);
};


/********************************************************************************************

>	class PPMOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/95
	Purpose:	Provide the OIL parts of the PPM filter which loads in the file as a BMP.
	SeeAlso:	PPMFilter

********************************************************************************************/

class PPMOILFilter : public OILFilter
{
public:
	PPMOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class PGMOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/95
	Purpose:	Provide the OIL parts of the PGM filter which loads in the file as a BMP.
	SeeAlso:	PGMFilter

********************************************************************************************/

class PGMOILFilter : public OILFilter
{
public:
	PGMOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class PBMOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/95
	Purpose:	Provide the OIL parts of the PBM filter which loads in the file as a BMP.
	SeeAlso:	PBMFilter

********************************************************************************************/

class PBMOILFilter : public OILFilter
{
public:
	PBMOILFilter(Filter *pFilter);
};



#endif // INC_PPMFILTR


