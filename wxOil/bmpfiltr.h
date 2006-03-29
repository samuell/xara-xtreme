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
//

#ifndef INC_BMPFILTR
#define INC_BMPFILTR

#include "bitfilt.h"
#include "outptdib.h"
#include "bmpprefs.h"  // SelectionType
//#include "imglib.h"
//#include "accures.h"
//#include "tim.h"

class CXaraFileRecord;

enum BMP_COMPRESSION
{
	BMP_RGB		= 0,
	BMP_RLE		= 1
};



/********************************************************************************************
>	class BMPExportOptions : public BitmapExportOptions

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/96
	Purpose:   	Base class for storing information about the bitmap we're exporting.
********************************************************************************************/

class BMPExportOptions : public BitmapExportOptions
{
	CC_DECLARE_DYNCREATE(BMPExportOptions)
public:
	BMPExportOptions();
    BMPExportOptions(const FILTER_ID FilterID, const StringBase* pFilterName);

	virtual BOOL	RetrieveDefaults();
	virtual BOOL	SetAsDefaults() const;

	// Save and load from named export set properties.
	virtual BOOL Write(CXaraFileRecord* pRec);
	virtual BOOL Read(CXaraFileRecord* pRec);

	virtual BOOL	GetSupportsImageMap() { return TRUE; }

	virtual DITHER	GetDither() const;
	virtual BOOL	SetDither(const DITHER& Dither);

	PALETTE	GetPalette() const;
	BOOL	SetPalette(const PALETTE& Palette);

	BMP_COMPRESSION	GetCompression() const;
	BOOL			SetCompression(const BMP_COMPRESSION& Compression);

	// for object copying and comparing
	virtual BOOL CopyFrom(BitmapExportOptions *pOther);
	virtual BOOL FileTypeChangeCopyFrom(BitmapExportOptions *pOther);
//	virtual BOOL Equal(BitmapExportOptions *pOther);
	
	virtual UINT32 GetFilterNameStrID() { return _R(IDT_FILTERNAME_BMP) /*_R(IDN_FILTERNAME_ACCUSOFTBMP)*/ /*_R(IDT_BMP_FILTERNAME) */;}


protected:
	// Parameters that the dialog can set
	DITHER				m_Dither;
	PALETTE				m_Palette;
	BMP_COMPRESSION		m_Compression;
};


/********************************************************************************************

>	class BMPFilter : public BitmapFilter

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Purpose:	Encapsulates a Windows BMP export filter. Will probably import one day too.

********************************************************************************************/

class BMPFilter : public BaseBitmapFilter
{
	CC_DECLARE_DYNAMIC(BMPFilter);
	
public:
	BMPFilter();
	BOOL Init();
	virtual INT32 HowCompatible( PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize,
							   UINT32 FileSize );

	// How to get and set details on the default export depth, dpi ...
	static UINT32 GetDefaultExportDepth();
	static double GetDefaultExportDPI();
	static void SetDefaultExportDepth( UINT32 );
	static void SetDefaultExportDPI( double );

	static UINT32 GetDefaultExportDither();
	static void SetDefaultExportDither( UINT32 );

PORTNOTE("filters", "Removed use of BMP_UNCOMPRESSED")
	virtual INT32 GetFilterType() const { return 0/*BMP_UNCOMPRESSED*/; }

	// Import related functions
	virtual BOOL ReadFromFile(OILBitmap* pOilBitmap);

	// This is the function for native/web files to use
    virtual BOOL ReadFromFile(OILBitmap* pOilBitmap, BaseCamelotFilter *pFilter,
							  CCLexFile * pFile, BOOL IsCompressed);

	void AlterPaletteContents( LPLOGPALETTE pPalette );
	virtual void PostGetExportOptions(BitmapExportOptions* pOptions);

protected:
	// Export related functions
	virtual BitmapExportOptions*	CreateExportOptions() const;

	virtual BOOL WriteToFile( BOOL End );
	virtual BOOL EndWriteToFile( );
	virtual BOOL GetExportOptions(BitmapExportOptions* pOptions);
	virtual void CleanUpAfterExport();

	virtual BOOL WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi);

	// this is used for the actual writing of the file
	virtual BOOL WriteDataToFile( BOOL End, UINT32 Bpp, UINT32 Compression);
	static BOOL EndWriteDataToFile( );
	static OutputDIB DestDIB;

	// This is so we can remember what we thought of the BMP file.
	INT32 BMPHowCompatible;

	// Function to see if we should export via Accusoft filters or not
	BOOL ExportViaAccusoftFilters();

	// Find out which way we need to render
	virtual BOOL GetRenderBottomToTop();

public:
	// Function for web/native filters to use to write out the bitmap data to file
	virtual BOOL WriteBitmapToFile(KernelBitmap* pKernelBitmap, BaseCamelotFilter *pFilter,
								   CCLexFile *pFile, INT32 Compression);

	// Check if this Bitmap filter can cope with saving at this Bpp/Colour depth
	virtual BOOL IsThisBppOk(UINT32 Bpp);

	// Public way of finding out how compatable the BMP filter thought the file was
	// We might want to bring in the Accusoft filter to help out.
	INT32 GetBmpCompatibility();
};

#endif
