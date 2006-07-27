// $Id: pngfiltr.h 809 2006-04-13 11:28:42Z phil $
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

#ifndef INC_IMGMGKFT_H
#define INC_IMGMGKFT_H

#include "maskfilt.h"
#include "outptpng.h"
#include "bmpprefs.h"  	// SelectionType
//#include "filtrres.h"

class KernelBitmap;

enum ImageMagickFlags
{
	IM_IMPORT		= 1<<0,		// Can import
	IM_EXPORT		= 1<<1,		// Can export
	IM_EX_MASK		= 1<<2,		// Can use mask on export
	IM_EX_INTERLACE = 1<<3,		// Can use interlace on export
	IM_EX_ALPHA		= 1<<3,		// Can use alpha on import
	IM_IM_DPI		= 1<<4, 	// Can use DPI on import (e.g. vector formats

	IM_DEFAULT		= 0
};

/********************************************************************************************

>	class ImageMagickExportOptions : public MaskedFilterExportOptions

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/96
	Purpose:   	Base class for storing information about the bitmap we're exporting.

********************************************************************************************/

// Need a type for the hash data or we get duplicate const error
typedef const TCHAR * IMFilterString;

// Declare the hash map from IMFilterString to UINT32
WX_DECLARE_HASH_MAP( IMFilterString, UINT32, wxStringHash, wxStringEqual, IMFilterStringToUINT32);

class ImageMagickExportOptions : public MaskedFilterExportOptions
{
CC_DECLARE_DYNCREATE(ImageMagickExportOptions)

public:
	ImageMagickExportOptions(const FilterType FilterID, const StringBase* pFilterName, ResourceID FilterTypeID);
	ImageMagickExportOptions() {};

	virtual BOOL	GetSupportsImageMap() { return TRUE; }

	virtual BOOL	RetrieveDefaults();
	virtual BOOL	SetAsDefaults() const;
	
	virtual UINT32 GetFilterNameStrID() { return m_FilterTypeID;}

	virtual UINT32 * GetConfigPtr(const TCHAR * FilterName) const
	{
		if (!s_pHash)
			return NULL;
		IMFilterStringToUINT32::iterator i=s_pHash->find(FilterName);
		return (UINT32 *)((i==s_pHash->end())?NULL:&(i->second));
	}

	virtual UINT32 GetConfig() const
	{
		UINT32 * pConfig = GetConfigPtr(FilterName);
		return pConfig?(*pConfig):0;
	}

	virtual void SetConfig(UINT32 value) const
	{
		UINT32 * pConfig = GetConfigPtr(FilterName);
		if (pConfig)
			*pConfig = value;
	}

protected:
	String_256 FilterName;
	ResourceID m_FilterTypeID;

	static	IMFilterStringToUINT32 * s_pHash;
};

/********************************************************************************************

>	class ImageMagickFilter : public MaskedFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Purpose:	Encapsulates a ImageMagick export filter.
				Derives off the masked filter which provides funcitonality for doing masked
				bitmap export and import.

********************************************************************************************/

class ImageMagickFilter : public MaskedFilter
{
	CC_DECLARE_DYNAMIC(ImageMagickFilter);
	
public:

	ImageMagickFilter();
	virtual ~ImageMagickFilter() {if (TempFile) delete (TempFile);}
	BOOL Init();

	virtual INT32 HowCompatible( PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize,
		UINT32 FileSize );

	virtual FilterType GetFilterType() const { return IMAGEMAGICK; }

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

	// Public way of finding out how compatable the ImageMagick filter thought the file was
	// We might want to bring in the Accusoft filter to help out.
	virtual INT32 GetImageMagickCompatibility() { return ImageMagickHowCompatible; }

	ImageMagickFlags GetCapabilities() {return m_Capabilities;}
	BOOL HasFlag(ImageMagickFlags f) {return (GetCapabilities() & f)!=0;}

	// These get overridden in the derived classes
	virtual BOOL CanDoTransparency() { return HasFlag(IM_EX_MASK); }
	virtual BOOL CanDoInterlace() { return HasFlag(IM_EX_INTERLACE); }
	virtual BOOL CanDoAlpha() { return HasFlag(IM_EX_ALPHA); }
	virtual BOOL CanDoImportDPI() { return HasFlag(IM_IM_DPI); }
	virtual BOOL CanDoImport() { return HasFlag(IM_IMPORT); }
	virtual BOOL CanDoExport() { return HasFlag(IM_EXPORT); }
	virtual wxString GetTag() { return _T("mmif"); }
	virtual TCHAR * GetExtension() { return _T("mmif"); }
	virtual INT32 GetCompatibility() { return 10; }

	virtual void SetImportDPI(INT32 ImportDPI) {m_ImportDPI = ImportDPI;}

	// Virtual overrides
	virtual UINT32 GetExportMsgID();
	virtual void PostGetExportOptions(BitmapExportOptions* pOptions);

	static BOOL CheckPath();

protected:
	virtual void SetCapabilities(ImageMagickFlags f) {m_Capabilities = f;}

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
	BOOL WriteDataToFile( BOOL End, UINT32 Bpp, UINT32 Compression);
	BOOL WriteToFile ( CCLexFile*, LPBITMAPINFO Info, LPBYTE Bits,
							String_64* ProgressString = NULL);

	// This is the form used for direct saving bitmaps into the native/web file format
	BOOL WriteToFile ( CCLexFile*, LPBITMAPINFO Info, LPBYTE Bits,
							BOOL Interlace, INT32 TransparentColour,
							BaseCamelotFilter* pFilter = NULL);

	void AlterPaletteContents( LPLOGPALETTE pPalette );//ap

	virtual BOOL CreateTempFile();
	virtual BOOL ConvertFromTempFile(CCLexFile * File);
	virtual BOOL ConvertToTempFile(CCLexFile * File);
	virtual BOOL TidyTempFile(BOOL Delete = TRUE);

#ifdef DO_EXPORT
	// The class we use for actually outputting the ImageMagick data and converting from 32 to n bpps
	static OutputPNG DestImageMagick;
#endif

	ResourceID FilterExtID;
	
	// This is so we can remember what we thought of the GIF file.
	INT32 ImageMagickHowCompatible;

	static FilterType s_FilterType;						// Type of filter in use i.e. ImageMagick

	// The string to display when exporting the second stage.
	UINT32 Export2ndStageMsgID;

	ImageMagickFlags m_Capabilities;
	INT32 m_ImportDPI;

	CCDiskFile * TempFile;
	wxString TempFileName;

	static String_256	s_ImageMagickPath;
	static BOOL s_HaveImageMagick;
	static BOOL s_HaveCheckedPath;
	static BOOL s_DoWarning;
	static BOOL s_Disable;

	// These allow us to use a single filter type
	static BOOL s_OutputTransparent;
	static BOOL s_OutputInterlaced;
};

/********************************************************************************************

>	class ImageMagickOILFilter : public OILFilter

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	18/07/2006
	Purpose:	The PNG File format Oil Filter (Extension etc)

********************************************************************************************/

class ImageMagickOILFilter : public OILFilter
{
public:
	ImageMagickOILFilter (Filter* pFilter, ResourceID FilterNameID, ResourceID FilterExtID);
};


#define DECLARE_IMAGEMAGICK_FILTER(IMType, IMFlags, IMTag, IMExtension, IMCompatibility)	\
class ImageMagickFilter ## IMType : public ImageMagickFilter					\
{																				\
public:																			\
	ImageMagickFilter ## IMType()												\
	{																			\
		SetCapabilities((ImageMagickFlags)(IMFlags));							\
		Flags.CanImport 	= HasFlag(IM_IMPORT);								\
		Flags.CanExport 	= HasFlag(IM_EXPORT);								\
		FilterID			= FILTERID_IMAGEMAGICK_ ## IMType ;					\
		FilterNameID		= _R(IDS_IMAGEMAGICK_ ## IMType ## _FILTERNAME);	\
		FilterInfoID		= _R(IDS_IMAGEMAGICK_ ## IMType ## _FILTERINFO);	\
		FilterExtID			= _R(IDS_IMAGEMAGICK_ ## IMType ## _FILTEREXT);		\
		ImportMsgID			= _R(IDS_IMAGEMAGICK_ ## IMType ## _IMPORTMSG);		\
		ExportMsgID			= _R(IDS_IMAGEMAGICK_ ## IMType ## _PREPAREMSG);	\
		ExportingMsgID		= _R(IDS_IMAGEMAGICK_ ## IMType ## _EXPORTMSG);		\
		Export2ndStageMsgID = _R(IDS_IMAGEMAGICK_ ## IMType ## _MASKINGMSG);	\
	}																			\
																				\
	virtual wxString GetTag() 			{ return IMTag; }						\
	virtual TCHAR * GetExtension() 		{ return IMExtension; }					\
	virtual INT32 GetCompatibility() 	{ return IMCompatibility; }				\
};

// Instructions on adding a new ImageMagickFilter
// ==============================================
//
// 1. Declare the filter below (let's say you give it the classname FOO)
// 2. Add "IMAGEMAGIK_FOO" to the enum in below
// 3. Add "ADD_FILTER(ImageMagickFilterFOO)" to filters.cpp
// 4. Add appropriate resources to wxOil/xrc/EN/imagemagick-strings.xrc
//    These would be
//		IDS_IMAGEMAGICK_FOO_FILTERNAME
//		IDS_IMAGEMAGICK_FOO_FILTERINFO
//		IDS_IMAGEMAGICK_FOO_FILTEREXT
//		IDS_IMAGEMAGICK_FOO_IMPORTMSG
//		IDS_IMAGEMAGICK_FOO_PREPAREMSG
//		IDS_IMAGEMAGICK_FOO_EXPORTMSG
//		IDS_IMAGEMAGICK_FOO_MASKINGMSG
//

enum ImageMagickFilterID
{
	FILTERID_IMAGEMAGICK = FILTERID_IMAGEMAGICK_START,		// This one should never be used
	FILTERID_IMAGEMAGICK_BMP,
	FILTERID_IMAGEMAGICK_CUR,
	FILTERID_IMAGEMAGICK_CUT,
	FILTERID_IMAGEMAGICK_DCM,
	FILTERID_IMAGEMAGICK_DCX,
	FILTERID_IMAGEMAGICK_DIB,
	FILTERID_IMAGEMAGICK_DNG,
	FILTERID_IMAGEMAGICK_EPDF,
	FILTERID_IMAGEMAGICK_EPI,
	FILTERID_IMAGEMAGICK_EPS,
	FILTERID_IMAGEMAGICK_EPS2,
	FILTERID_IMAGEMAGICK_EPS3,
	FILTERID_IMAGEMAGICK_EPSF,
	FILTERID_IMAGEMAGICK_EPSI,
	FILTERID_IMAGEMAGICK_EPT,
	FILTERID_IMAGEMAGICK_FAX,
	FILTERID_IMAGEMAGICK_FITS,
	FILTERID_IMAGEMAGICK_ICO,
	FILTERID_IMAGEMAGICK_JNG,
	FILTERID_IMAGEMAGICK_MIFF,
	FILTERID_IMAGEMAGICK_MPC,
	FILTERID_IMAGEMAGICK_OTB,
	FILTERID_IMAGEMAGICK_P7,
	FILTERID_IMAGEMAGICK_PALM,
	FILTERID_IMAGEMAGICK_PAM,
	FILTERID_IMAGEMAGICK_PBM,
	FILTERID_IMAGEMAGICK_PCD,
	FILTERID_IMAGEMAGICK_PCDS,
	FILTERID_IMAGEMAGICK_PCL,
	FILTERID_IMAGEMAGICK_PCX,
	FILTERID_IMAGEMAGICK_PDB,
	FILTERID_IMAGEMAGICK_PDF,
	FILTERID_IMAGEMAGICK_PGM,
	FILTERID_IMAGEMAGICK_PICT,
	FILTERID_IMAGEMAGICK_PIX,
	FILTERID_IMAGEMAGICK_PNM,
	FILTERID_IMAGEMAGICK_PPM,
	FILTERID_IMAGEMAGICK_PS,
	FILTERID_IMAGEMAGICK_PS2,
	FILTERID_IMAGEMAGICK_PS3,
	FILTERID_IMAGEMAGICK_PSD,
	FILTERID_IMAGEMAGICK_PTIF,
	FILTERID_IMAGEMAGICK_PWP,
	FILTERID_IMAGEMAGICK_RLA,
	FILTERID_IMAGEMAGICK_RLE,
	FILTERID_IMAGEMAGICK_SCT,
	FILTERID_IMAGEMAGICK_SFW,
	FILTERID_IMAGEMAGICK_SUN,
	FILTERID_IMAGEMAGICK_SVG,
	FILTERID_IMAGEMAGICK_TGA,
	FILTERID_IMAGEMAGICK_TIFF,
	FILTERID_IMAGEMAGICK_TIM,
	FILTERID_IMAGEMAGICK_TTF,
	FILTERID_IMAGEMAGICK_VICAR,
	FILTERID_IMAGEMAGICK_VIFF,
	FILTERID_IMAGEMAGICK_WBMP,
	FILTERID_IMAGEMAGICK_WPG,
	FILTERID_IMAGEMAGICK_XBM,
	FILTERID_IMAGEMAGICK_XCF,
	FILTERID_IMAGEMAGICK_XPM,
	FILTERID_IMAGEMAGICK_XWD,

	// add the others above this line
	FILTERID_IMAGEMAGICK_TOP
};

//						  ClassName	Import	Export	Transp	Int'lce	Tag			Extension	Compat
DECLARE_IMAGEMAGICK_FILTER(BMP,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("bmp"),	_T("bmp"),	10); // Microsoft Windows Bitmap
DECLARE_IMAGEMAGICK_FILTER(CUR,		IM_IMPORT|IM_EX_MASK,						_T("cur"),	_T("cur"),	10); // Microsoft Cursor Icon
DECLARE_IMAGEMAGICK_FILTER(CUT,		IM_IMPORT|IM_EX_MASK,						_T("cut"),	_T("cut"),	10); // DR Halo
DECLARE_IMAGEMAGICK_FILTER(DCM,		IM_IMPORT|IM_EX_MASK,						_T("dcm"),	_T("dcm"),	10); // DICOM Image
DECLARE_IMAGEMAGICK_FILTER(DCX,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("dcx"),	_T("dcx"),	10); // ZSoft Paintbrush
DECLARE_IMAGEMAGICK_FILTER(DIB,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("dib"),	_T("dib"),	10); // Microsoft Windows Device Independent Bitmap
DECLARE_IMAGEMAGICK_FILTER(DNG,		IM_IMPORT|IM_EX_MASK,						_T("dng"),	_T("dng"),	10); // Digital Negative
DECLARE_IMAGEMAGICK_FILTER(EPDF,	IM_IMPORT|IM_EXPORT|IM_EX_MASK|IM_IM_DPI,	_T("epdf"),	_T("epdf"),	10); // Encapsulated Portable Document Format
DECLARE_IMAGEMAGICK_FILTER(EPI,		IM_IMPORT|IM_EXPORT|IM_EX_MASK|IM_IM_DPI,	_T("epi"),	_T("epi"),	10); // Adobe Encapsulated PostScript Interchange format
DECLARE_IMAGEMAGICK_FILTER(EPS,		IM_IMPORT|IM_EXPORT|IM_EX_MASK|IM_IM_DPI,	_T("eps"),	_T("eps"),	10); // Adobe Encapsulated PostScript
DECLARE_IMAGEMAGICK_FILTER(EPS2,	IM_EXPORT|IM_EX_MASK|IM_IM_DPI,				_T("eps2"),	_T("eps2"),	10); // Adobe Level II Encapsulated PostScript
DECLARE_IMAGEMAGICK_FILTER(EPS3,	IM_EXPORT|IM_EX_MASK|IM_IM_DPI,				_T("eps3"),	_T("eps3"),	10); // Adobe Level III Encapsulated PostScript
DECLARE_IMAGEMAGICK_FILTER(EPSF,	IM_IMPORT|IM_EXPORT|IM_EX_MASK|IM_IM_DPI,	_T("epsf"),	_T("epsf"),	10); // Adobe Encapsulated PostScript
DECLARE_IMAGEMAGICK_FILTER(EPSI,	IM_IMPORT|IM_EXPORT|IM_EX_MASK|IM_IM_DPI,	_T("epsi"),	_T("epsi"),	10); // Adobe Encapsulated PostScript Interchange format
DECLARE_IMAGEMAGICK_FILTER(EPT,		IM_IMPORT|IM_EXPORT|IM_EX_MASK|IM_IM_DPI,	_T("ept"),	_T("ept"),	10); // Adobe Encapsulated PostScript Interchange format with TIFF preview
DECLARE_IMAGEMAGICK_FILTER(FAX,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("fax"),	_T("fax"),	10); // Group 3 TIFF
DECLARE_IMAGEMAGICK_FILTER(FITS,	IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("fits"),	_T("fits"),	10); // Flexible Image Transport System
DECLARE_IMAGEMAGICK_FILTER(ICO,		IM_IMPORT|IM_EX_MASK,						_T("ico"),	_T("ico"),	10); // Microsoft Icon
DECLARE_IMAGEMAGICK_FILTER(JNG,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("jng"),	_T("jng"),	10); // Multiple-image Network Graphics
DECLARE_IMAGEMAGICK_FILTER(MIFF,	IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("miff"),	_T("miff"),	10); // Magick image file format
DECLARE_IMAGEMAGICK_FILTER(MPC,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("mpc"),	_T("mpc"),	10); // Magick Persistent Cache image file format
DECLARE_IMAGEMAGICK_FILTER(OTB,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("otb"),	_T("otb"),	10); // On-the-air Bitmap
DECLARE_IMAGEMAGICK_FILTER(P7,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("p7"),	_T("p7"),	10); // Xv's Visual Schnauzer thumbnail format
DECLARE_IMAGEMAGICK_FILTER(PALM,	IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("palm"),	_T("palm"),	10); // Palm pixmap
DECLARE_IMAGEMAGICK_FILTER(PAM,		IM_EXPORT|IM_EX_MASK,						_T("pam"),	_T("pam"),	10); // Common 2-dimensional bitmap format
DECLARE_IMAGEMAGICK_FILTER(PBM,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("pbm"),	_T("pbm"),	10); // Portable bitmap format (black and white)
DECLARE_IMAGEMAGICK_FILTER(PCD,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("pcd"),	_T("pcd"),	10); // Photo CD
DECLARE_IMAGEMAGICK_FILTER(PCDS,	IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("pcds"),	_T("pcds"),	10); // Photo CD
DECLARE_IMAGEMAGICK_FILTER(PCL,		IM_EXPORT|IM_EX_MASK|IM_IM_DPI,				_T("pcl"),	_T("pcl"),	10); // HP Page Control Language
DECLARE_IMAGEMAGICK_FILTER(PCX,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("pcx"),	_T("pcx"),	10); // ZSoft IBM PC Paintbrush file
DECLARE_IMAGEMAGICK_FILTER(PDB,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("pdb"),	_T("pdb"),	10); // Palm Database ImageViewer Format
DECLARE_IMAGEMAGICK_FILTER(PDF,		IM_IMPORT|IM_EXPORT|IM_EX_MASK|IM_IM_DPI,	_T("pdf"),	_T("pdf"),	10); // Portable Document Format
DECLARE_IMAGEMAGICK_FILTER(PGM,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("pgm"),	_T("pgm"),	10); // Portable graymap format (gray scale)
DECLARE_IMAGEMAGICK_FILTER(PICT,	IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("pict"),	_T("pict"),	10); // Apple Macintosh QuickDraw/PICT file
DECLARE_IMAGEMAGICK_FILTER(PIX,		IM_IMPORT|IM_EX_MASK,						_T("pix"),	_T("pix"),	10); // Alias/Wavefront RLE image format
DECLARE_IMAGEMAGICK_FILTER(PNM,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("pnm"),	_T("pnm"),	10); // Portable anymap
DECLARE_IMAGEMAGICK_FILTER(PPM,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("ppm"),	_T("ppm"),	10); // Portable pixmap format (color)
DECLARE_IMAGEMAGICK_FILTER(PS,		IM_IMPORT|IM_EXPORT|IM_EX_MASK|IM_IM_DPI,	_T("ps"),	_T("ps"),	10); // Adobe PostScript file
DECLARE_IMAGEMAGICK_FILTER(PS2,		IM_IMPORT|IM_EXPORT|IM_EX_MASK|IM_IM_DPI,	_T("ps2"),	_T("ps2"),	10); // Adobe Level II PostScript file
DECLARE_IMAGEMAGICK_FILTER(PS3,		IM_IMPORT|IM_EXPORT|IM_EX_MASK|IM_IM_DPI,	_T("ps3"),	_T("ps3"),	10); // Adobe Level III PostScript file
DECLARE_IMAGEMAGICK_FILTER(PSD,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("psd"),	_T("psd"),	10); // Adobe Photoshop bitmap file
DECLARE_IMAGEMAGICK_FILTER(PTIF,	IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("ptif"),	_T("ptif"),	10); // Pyramid encoded TIFF
DECLARE_IMAGEMAGICK_FILTER(PWP,		IM_IMPORT|IM_EX_MASK,						_T("pwp"),	_T("pwp"),	10); // Seattle File Works multi-image file
DECLARE_IMAGEMAGICK_FILTER(RLA,		IM_IMPORT|IM_EX_MASK,						_T("rla"),	_T("rla"),	10); // Alias/Wavefront image file
DECLARE_IMAGEMAGICK_FILTER(RLE,		IM_IMPORT|IM_EX_MASK,						_T("rle"),	_T("rle"),	10); // Utah Run length encoded image file
DECLARE_IMAGEMAGICK_FILTER(SCT,		IM_IMPORT|IM_EX_MASK,						_T("sct"),	_T("sct"),	10); // Scitex Continuous Tone Picture
DECLARE_IMAGEMAGICK_FILTER(SFW,		IM_IMPORT|IM_EX_MASK,						_T("sfw"),	_T("sfw"),	10); // Seattle File Works image
DECLARE_IMAGEMAGICK_FILTER(SUN,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("sun"),	_T("sun"),	10); // SUN Rasterfile
DECLARE_IMAGEMAGICK_FILTER(SVG,		IM_IMPORT|IM_EXPORT|IM_EX_MASK|IM_IM_DPI,	_T("svg"),	_T("svg"),	10); // Scalable Vector Graphics
DECLARE_IMAGEMAGICK_FILTER(TGA,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("tga"),	_T("tga"),	10); // Truevision Targa image
DECLARE_IMAGEMAGICK_FILTER(TIFF,	IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("tiff"),	_T("tiff"),	10); // Tagged Image File Format
DECLARE_IMAGEMAGICK_FILTER(TIM,		IM_IMPORT|IM_EX_MASK,						_T("tim"),	_T("tim"),	10); // PSX TIM file
DECLARE_IMAGEMAGICK_FILTER(TTF,		IM_IMPORT|IM_EX_MASK,						_T("ttf"),	_T("ttf"),	10); // TrueType font file
DECLARE_IMAGEMAGICK_FILTER(VICAR,	IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("vicar"),_T("vicar"),10); // VICAR rasterfile format
DECLARE_IMAGEMAGICK_FILTER(VIFF,	IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("viff"),	_T("viff"),	10); // Khoros Visualization Image File Format
DECLARE_IMAGEMAGICK_FILTER(WBMP,	IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("wbmp"),	_T("wbmp"),	10); // Wireless bitmap
DECLARE_IMAGEMAGICK_FILTER(WPG,		IM_IMPORT|IM_EX_MASK,						_T("wpg"),	_T("wpg"),	10); // Word Perfect Graphics File
DECLARE_IMAGEMAGICK_FILTER(XBM,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("xbm"),	_T("xbm"),	10); // X Windows system bitmap, black and white only
DECLARE_IMAGEMAGICK_FILTER(XCF,		IM_IMPORT|IM_EX_MASK,						_T("xcf"),	_T("xcf"),	10); // GIMP image
DECLARE_IMAGEMAGICK_FILTER(XPM,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("xpm"),	_T("xpm"),	10); // X Windows system pixmap
DECLARE_IMAGEMAGICK_FILTER(XWD,		IM_IMPORT|IM_EXPORT|IM_EX_MASK,				_T("xwd"),	_T("xwd"),	10); // X Windows system window dump

#endif // INC_ImageMagickFILTR


