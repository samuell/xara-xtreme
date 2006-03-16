// $Id: pngfiltr.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_PNGFILTR
#define INC_PNGFILTR

#include "maskfilt.h"
#include "outptpng.h"
#include "bmpprefs.h"  	// SelectionType
//#include "filtrres.h"
//#include "imglib.h"

// Fake an const that would be defined in imglib.h
static INT32			PNG = 1;

class KernelBitmap;

/********************************************************************************************

>	class PNGExportOptions : public MaskedFilterExportOptions

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/96
	Purpose:   	Base class for storing information about the bitmap we're exporting.

********************************************************************************************/

PORTNOTE("other","Removed PNGExportOptions")
#ifndef EXCLUDE_FROM_XARALX
class PNGExportOptions : public MaskedFilterExportOptions
{
CC_DECLARE_DYNCREATE(PNGExportOptions)

public:
	static BOOL Declare();

    PNGExportOptions(const FILTER_ID FilterID, const StringBase* pFilterName);
	PNGExportOptions() {};

	virtual BOOL	GetSupportsImageMap() { return TRUE; }

	virtual BOOL	RetrieveDefaults();
	virtual BOOL	SetAsDefaults() const;
	
	virtual UINT32 GetFilterNameStrID() { return _R(IDS_FILTERNAME_PNG);}



protected:
	static	UINT32 g_CompactedFlagsForDefaults;
};
#endif

/********************************************************************************************

>	class PNGFilter : public MaskedFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Purpose:	Encapsulates a PNG export filter.
				Derives off the masked filter which provides funcitonality for doing masked
				bitmap export and import.

********************************************************************************************/

class PNGFilter : public MaskedFilter
{
	CC_DECLARE_DYNAMIC(PNGFilter);
	
public:

	PNGFilter();
	BOOL Init();

	virtual INT32 HowCompatible( PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize,
		UINT32 FileSize );

	virtual INT32 GetFilterType() const { return PNG; }

	// Import related functions
	virtual BOOL ReadFromFile(OILBitmap* pOilBitmap);
	// This is the function for native/web files to use
    virtual BOOL ReadFromFile(OILBitmap* pOilBitmap, BaseCamelotFilter* pFilter,
							  CCLexFile* pFile, BOOL IsCompressed);

	// Function for web/native filters to use to write out the bitmap data to file
	virtual BOOL WriteBitmapToFile(KernelBitmap* pKernelBitmap, BaseCamelotFilter* pFilter,
								   CCLexFile* pFile, INT32 Compression);

	// Check if this Bitmap filter can cope with saving at this Bpp/Colour depth
	virtual BOOL IsThisBppOk(UINT32 Bpp);

	// Public way of finding out how compatable the PNG filter thought the file was
	// We might want to bring in the Accusoft filter to help out.
	virtual INT32 GetPNGCompatibility() { return PNGHowCompatible; }

	// Virtual overrides
	virtual UINT32 GetExportMsgID();
	virtual void PostGetExportOptions(BitmapExportOptions* pOptions);

protected:

	// Invert the alpha channel.
	virtual void InvertAlpha ( LPBITMAPINFO	lpBitmapInfo,
							   LPBYTE		lpBits );

	// Get the correct output DIB.
	virtual OutputDIB* GetOutputDIB ( void );

	virtual BitmapExportOptions*	CreateExportOptions() const;

	virtual BOOL GetExportOptions( BitmapExportOptions* pOptions );
	virtual void CleanUpAfterExport();
	virtual BOOL EndWriteToFile();

	virtual BOOL WriteFileHeader(void) { return TRUE; }
	virtual BOOL WritePreFrame(void);
	virtual BOOL WritePreSecondPass(void);
	virtual BOOL WritePostOptimisedPalette(void) { return TRUE; }
	virtual BOOL WriteFrame(void);
	virtual BOOL WritePostFrame(void);
	virtual BOOL WriteFileEnd(void);

	virtual BOOL WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi);

	// this is used for the actual writing of the file
	static BOOL WriteDataToFile( BOOL End, UINT32 Bpp, UINT32 Compression);
	static BOOL WriteToFile ( CCLexFile*, LPBITMAPINFO Info, LPBYTE Bits,
						 	  String_64* ProgressString = NULL);

	// This is the form used for direct saving bitmaps into the native/web file format
	static BOOL WriteToFile ( CCLexFile*, LPBITMAPINFO Info, LPBYTE Bits,
							  BOOL Interlace, INT32 TransparentColour,
						 	  BaseCamelotFilter* pFilter = NULL);

	void AlterPaletteContents( LPLOGPALETTE pPalette );//ap

#ifdef DO_EXPORT
	// The class we use for actually outputting the PNG data and converting from 32 to n bpps
	static OutputPNG DestPNG;
#endif
	
	// This is so we can remember what we thought of the GIF file.
	INT32 PNGHowCompatible;

	static INT32 FilterType;						// Type of filter in use i.e. PNG

	// The string to display when exporting the second stage.
	UINT32 Export2ndStageMsgID;
};

#endif // INC_PNGFILTR


