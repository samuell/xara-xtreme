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

// A Windows BMP import/export filter (export only currently)


#include "camtypes.h"
#include "bmpfiltr.h"
//#include "andy.h"
#include "oilfltrs.h"
#include "app.h"
#include "dibutil.h"
#include "grndbmp.h"
#include "nodebmp.h"
#ifndef WEBSTER
#include "extfilts.h"	// Accusoft filters class
#endif //WEBSTER
#include "wbitmap.h"	// Windows specific bitmap information	 

#include "fixmem.h"

#include "accuflts.h"	// instead of imglib.h for 'BMP_UNCOMPRESSED'
#include "bmapprev.h"	// tab preview dialog
#include "cxfrec.h"		// for CXaraFileRecord
#include "exjpeg.h"

CC_IMPLEMENT_DYNAMIC(BMPFilter, BaseBitmapFilter)
CC_IMPLEMENT_DYNCREATE(BMPExportOptions, BitmapExportOptions)

#define	new	CAM_DEBUG_NEW

static double DefaultExportDPI = 96.0;
static UINT32 DefaultExportDepth = 24;
static UINT32 DefaultExportDither = 4;	// Webster<RanbirR> The default export dither type has been changed to No Dither.
static BOOL DefaultExportCompression = FALSE;
static PALETTE DefaultExportPalette = PAL_OPTIMISED;


/********************************************************************************************
>	BMPExportOptions::BMPExportOptions(const FILTER_ID FilterID, const StringBase* pFilterName)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Default constructor
********************************************************************************************/

BMPExportOptions::BMPExportOptions()
  : m_Dither(XARADITHER_NONE),
	m_Palette(DefaultExportPalette),
	m_Compression(BMP_RGB)
{
	// Empty.
}



/********************************************************************************************
>	BMPExportOptions::BMPExportOptions(const FILTER_ID FilterID, const StringBase* pFilterName)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Constructor for a BMPExportOptions object to provide BMP specific
				options
********************************************************************************************/

BMPExportOptions::BMPExportOptions(const FILTER_ID FilterID, const StringBase* pFilterName)
  : BitmapExportOptions(_R(IDD_EXPORTBMPOPTS), FilterID, pFilterName),
	m_Dither(XARADITHER_NONE),
	m_Palette(DefaultExportPalette),
	m_Compression(BMP_RGB)
{
	// Empty.
}



/********************************************************************************************

>	virtual BOOL BMPExportOptions::CopyFrom(BitmapExportOptions *pSource) 

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/97
	Inputs:		pSource - the other BitmapExportOptions object to copy the data from
	Purpose:	Sets the contents of this object from the passed object
	See Also:	BitmapExportOptions::MakeCopy()

********************************************************************************************/
BOOL BMPExportOptions::CopyFrom(BitmapExportOptions *pSource) 
{
	// copy the base class first
	if (!BitmapExportOptions::CopyFrom(pSource))
		return FALSE;

	// must be the same class, otherwise the base class  function above returns FALSE
	BMPExportOptions *pOther = (BMPExportOptions *)pSource;

	m_Dither		= pOther->m_Dither;
	m_Palette		= pOther->m_Palette;
	m_Compression	= pOther->m_Compression;

	return TRUE;
};


BOOL BMPExportOptions::FileTypeChangeCopyFrom(BitmapExportOptions *pSource)
{
	ASSERT(FALSE);
	/*
	// copy the base class first
	if (!BitmapExportOptions::FileTypeChangeCopyFrom(pSource))
		return FALSE;

	BMPExportOptions* pOther = (BMPExportOptions*)pSource;

	//  Coming from a jpeg image
	if( pOther->IS_KIND_OF(JPEGExportOptions) )
	{
		m_Dither	= XARADITHER_NONE;
		m_Palette	= DefaultExportPalette;
	}
	else
	{
		//  Coming from a gif or png image.
		m_Dither	= pOther->m_Dither;
		m_Palette	= pOther->m_Palette;
	}

	m_Compression	= BMP_RGB;

	// Ensure that the bitmap depth is less than 32 bpp.
	if( GetDepth() == 32 )
	{
		SetDepth( 24 );
	}
*/
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL BMPExportOptions::Equal(BitmapExportOptions *pSource) 

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/97
	Inputs:		pSource - the other BitmapExportOptions object to copy the data from
	Purpose:	Compares the contents of this and pOther objects
	Returns:	TRUE, if objects are equal, FALSE otherwise
	See Also:	BitmapExportOptions::MakeCopy()

********************************************************************************************/
/*
BOOL BMPExportOptions::Equal(BitmapExportOptions *pSource) 
{
	BOOL ok = TRUE;

	// compare the base classes first
	ok = BitmapExportOptions::Equal(pSource);

	// must be the same class, otherwise the base class  function above returns FALSE
	BMPExportOptions *pOther = (BMPExportOptions *)pSource;

	if (ok) ok = (m_Dither	== pOther->m_Dither);
	if (ok) ok = (m_Palette	== pOther->m_Palette);
	if (ok) ok = (m_Compression	== pOther->m_Compression);
	return ok;
};
*/

/********************************************************************************************

>	virtual BOOL BMPExportOptions::RetrieveDefaults()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Gets GIF specific preferences
	Notes:		See BitmapExportOptions for interface details

********************************************************************************************/
BOOL BMPExportOptions::RetrieveDefaults()
{
	if (!BitmapExportOptions::RetrieveDefaults())
	{
		return FALSE;
	}

	SetDither((DITHER)BMPFilter::GetDefaultExportDither());
	SetPalette(DefaultExportPalette);

	BOOL Compression = DefaultExportCompression;
	ERROR2IF(Compression > 1, FALSE, "Invalid Compression");
	SetCompression((BMP_COMPRESSION)Compression);

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL BMPExportOptions::SetAsDefaults() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Provides additional implementation to set GIF specific info as defaults
	See Also:	BitmapExportInfo::SetAsDefaults()

********************************************************************************************/
BOOL BMPExportOptions::SetAsDefaults() const
{
	if (!BitmapExportOptions::SetAsDefaults())
	{
		return FALSE;
	}

	BMPFilter::SetDefaultExportDither(GetDither());
	DefaultExportCompression	= (BOOL)GetCompression();
	DefaultExportPalette		= GetPalette();

	return TRUE;
}



/********************************************************************************************
>	virtual BOOL BMPExportOptions::Write(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/00
	Inputs:		pRec	---		record to write
	Returns:	TRUE if successful.
	Purpose:	Writes this object to the given record.
	SeeAlso:	BMPExportOptions::Read
********************************************************************************************/

BOOL BMPExportOptions::Write(CXaraFileRecord* pRec)
{
	return BitmapExportOptions::Write(pRec) &&
		   pRec->WriteINT16((INT16) m_Dither) &&
		   pRec->WriteINT16((INT16) m_Palette) &&
		   pRec->WriteINT16((INT16) m_Compression);
}



/********************************************************************************************
>	virtual BOOL BMPExportOptions::Read(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/00
	Inputs:		pRec	---		record to read
	Returns:	TRUE if successful.
	Purpose:	Reads this object from the given record.
	SeeAlso:	BMPExportOptions::Write
********************************************************************************************/

BOOL BMPExportOptions::Read(CXaraFileRecord* pRec)
{
	return BitmapExportOptions::Read(pRec) &&
		   pRec->ReadINT16((INT16*) &m_Dither) &&
		   pRec->ReadINT16((INT16*) &m_Palette) &&
		   pRec->ReadINT16((INT16*) &m_Compression);
}



/********************************************************************************************

>	DITHER BMPExportOptions::GetDither() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	The dither desired for the bitmap to export
	Purpose:	Support function to obtain export options

********************************************************************************************/
DITHER BMPExportOptions::GetDither() const
{
	return m_Dither;
}


/********************************************************************************************

>	BOOL BMPExportOptions::SetDither(const DITHER& Dither)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		Dither : The desired dither to be applied to the bitmap before being exported
	Returns:	TRUE if set successfully
				FALSE otherwise
	Purpose:	Support function to set up information for the ensuing export
	See Also:	GetDither()

********************************************************************************************/
BOOL BMPExportOptions::SetDither(const DITHER& Dither)
{
	if (m_Dither != Dither)
		SetTempFileMatchesExportOptions(FALSE);
	m_Dither = Dither;
	return TRUE;
}


/********************************************************************************************

>	PALETTE BMPExportOptions::GetPalette() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	The palette desired to be exported with the bitmap
	Purpose:	Support function to obtain export options

********************************************************************************************/
PALETTE BMPExportOptions::GetPalette() const
{
	return m_Palette;
}


/********************************************************************************************

>	BOOL BMPExportOptions::SetPalette(const PALETTE& Palette)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		Dither : The desired dither to be applied to the bitmap before being exported
	Returns:	TRUE if set successfully
				FALSE otherwise
	Purpose:	Support function to set up information for the ensuing export
	See Also:	GetPalette()

********************************************************************************************/
BOOL BMPExportOptions::SetPalette(const PALETTE& Palette)
{
	m_Palette = Palette;
	return TRUE;
}



/********************************************************************************************

>	BMP_COMPRESSION	BMPExportOptions::GetCompression() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	The compression desired for BMP bitmap export
	Purpose:	Support function to obtain export options

********************************************************************************************/
BMP_COMPRESSION	BMPExportOptions::GetCompression() const
{
	return m_Compression;
}


/********************************************************************************************

>	BOOL BMPExportOptions::SetCompression(const BMP_COMPRESSION& Compression)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		Compression : The compression to be used for the export
	Returns:	TRUE if set successfully
				FALSE otherwise
	Purpose:	Support function to set up information for the ensuing export
	See Also:	GetCompression()

********************************************************************************************/
BOOL BMPExportOptions::SetCompression(const BMP_COMPRESSION& Compression)
{
	ERROR2IF(Compression != BMP_RLE && Compression != BMP_RGB, FALSE, "Invalid Compression");

	m_Compression = Compression;

	return TRUE;
}



#ifdef DO_EXPORT
OutputDIB 	BMPFilter::DestDIB;
#endif


/********************************************************************************************

	Preference:		DefaultExportDPI
	Section:		Filters
	Range:			1 to 3000
	Purpose:		Allows a default export dots per inch size to be remembered when
					exporting either as a BMP or via the Accusoft loaders.
	SeeAlso:		-

********************************************************************************************/

/********************************************************************************************

	Preference:		DefaultExportDepth
	Section:		Filters
	Range:			1 to 32
	Purpose:		Allows a default export bits per pixel amount to be remembered when 
					exporting either as a BMP or via the Accusoft loaders.
	SeeAlso:		-

********************************************************************************************/

/********************************************************************************************

>	UINT32 BMPFilter::GetDefaultExportDepth()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		None
	Outputs:	None
	Returns:	The value currently set for the DefaultExportDepth
	Purpose:	Allows other bitmap export filters to get at this value. Needs to be here
				rather than in BaseBitmapFilter as that Init is never used.
	SeeAlso:	BMPFilter::Init; BMPFilter::GetDefaultExportDpi; BMPFilter::SetDefaultExportDepth; 

********************************************************************************************/

UINT32 BMPFilter::GetDefaultExportDepth()
{
	return DefaultExportDepth;
}

/********************************************************************************************

>	double BMPFilter::GetDefaultExportDPI()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		None
	Outputs:	None
	Returns:	The value currently set for the DefaultExportDpi
	Purpose:	Allows other bitmap export filters to get at this value. Needs to be here
				rather than in BaseBitmapFilter as that Init is never used.
	SeeAlso:	BMPFilter::Init; BMPFilter::GetDefaultExportDepth; BMPFilter::SetDefaultExportDPI; 

********************************************************************************************/

double BMPFilter::GetDefaultExportDPI()
{
	return DefaultExportDPI;
}

/********************************************************************************************

>	void BMPFilter::SetDefaultExportDepth( UINT32 Depth)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		The new value to set for the DefaultExportDepth
	Outputs:	None
	Returns:	None  
	Purpose:	Allows other bitmap export filters to set this value. Needs to be here
				rather than in BaseBitmapFilter as that Init is never used.
	SeeAlso:	BMPFilter::Init; BMPFilter::GetDefaultExportDepth; BMPFilter::GetDefaultExportDPI; 

********************************************************************************************/

void BMPFilter::SetDefaultExportDepth( UINT32 Depth)
{
	DefaultExportDepth = Depth;
}

/********************************************************************************************

>	void BMPFilter::SetDefaultExportDPI( double DPI)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		The new value to set for the DefaultExportDpi
	Outputs:	None
	Returns:	None  
	Purpose:	Allows other bitmap export filters to set this value. Needs to be here
				rather than in BaseBitmapFilter as that Init is never used.
	SeeAlso:	BMPFilter::Init; BMPFilter::GetDefaultExportDepth; BMPFilter::GetDefaultExportDPI; 

********************************************************************************************/

void BMPFilter::SetDefaultExportDPI( double DPI)
{
	DefaultExportDPI = DPI;
}

UINT32 BMPFilter::GetDefaultExportDither()
{
	return DefaultExportDither;
}

void BMPFilter::SetDefaultExportDither( UINT32 Dither )
{
	DefaultExportDither = Dither;
}

/********************************************************************************************

>	BMPFilter::BMPFilter()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Purpose:	Constructor for an BMPFilter object.  The object should be initialised
				before use.
	SeeAlso:	BMPFilter::Init

********************************************************************************************/

BMPFilter::BMPFilter() : BaseBitmapFilter()
{
	ImportMsgID = _R(IDT_IMPORTMSG_BMP);

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	Flags.CanExport = TRUE;
#else
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#endif

	FilterID = FILTERID_BMP;

	ExportRegion = NULL;
	ExportMsgID = _R(IDT_EXPORTMSG_BMP);

	ExportingMsgID = _R(IDT_EXPORTMSG_BMP);
}

/********************************************************************************************

>	BOOL BMPFilter::Init()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an BMPFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL BMPFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new BMPOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDT_BMP_FILTERNAME));
	FilterInfo.Load(_R(IDT_BMP_FILTERINFO));

	if (Camelot.DeclareSection("Filters", 10))
	{
//		Requested that we don't store DPI for export (see bug 6757)
//		Camelot.DeclarePref( NULL, "ExportBitmapDPI", &DefaultExportDPI, 1.0, 3000.0 );
		Camelot.DeclarePref( NULL, "ExportBitmapDepth", &DefaultExportDepth, 1, 32 );
	}

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL BMPFilter::ReadFromFile( OILBitmap* pOilBitmap, BaseCamelotFilter* pFilter,
							  CCLexFile* pFile, BOOL IsCompressed)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/96
	Inputs:		pOilBitmap	pointer to the oil bitmap data to be filled in
				pFilter			- the BaseCamelotFilter which provides functions like progress update
				pFile			- the file to read the data from
				IsCompressed	- Flag to say the bitmap is compressed or not.  
	Outputs:	Will have filled in BMInfo	pointer to the bitmap header to fill in
									BMBytes	pointer to the bitmap data to fill in
	Purpose:	Actually does the process of reading a bitmap from a file.
				Inherited classes override this to read in different file formats.
				It is used by the web/native filters to pull out a bitmap definition from
				inside a bitmap definition record.
				IsCompressed is only used for BMP/BMPZIP type bitmaps at present.
				Assumes:
					pFile has already been opened up for reading
					pFilter has been set up for reading the data e.g. progress bar 
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BMPFilter::ReadFromFile( OILBitmap* pOilBitmap, BaseCamelotFilter* pFilter,
							  CCLexFile* pFile, BOOL IsCompressed)
{
	ERROR2IF(pOilBitmap == NULL,FALSE,"BMPFilter::ReadFromFile null OilBitmap pointer");
	ERROR2IF(pFilter == NULL,FALSE,"BMPFilter::ReadFromFile null pFilter pointer");
	ERROR2IF(pFile == NULL,FALSE,"BMPFilter::ReadFromFile null pFile pointer");

	// Try to import bitmap as usual binary BMP file.
	WinBitmap* pWBitmap = (WinBitmap*)pOilBitmap;
	
	LPBITMAPINFO *pInfo = &(pWBitmap->BMInfo);
	LPBYTE *pBytes = &(pWBitmap->BMBytes);

	if (IsCompressed)
	{
		// We want compression on so turn it on
		if (!pFile->SetCompression(TRUE))
			return FALSE;
	}

	// Read from file, no header and using pFilter for progress bar updates
	if (!DIBUtil::ReadFromFile(pFile, pInfo, pBytes, FALSE, NULL, pFilter))
		return FALSE;

	if (IsCompressed)
	{
		// We have finished with compression, so turn it off
		if (!pFile->SetCompression(FALSE))
			return FALSE;
	}

	// Everything went ok and we imported the bitmap ok
	return TRUE;
}

/********************************************************************************************

>	BOOL BMPFilter::ReadFromFile( OILBitmap* pOilBitmap )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/95
	Inputs:		pOilBitmap	pointer to the oil bitmap data to be filled in
	Outputs:	Will have filled in BMInfo	pointer to the bitmap header to fill in
									BMBytes	pointer to the bitmap data to fill in
	Purpose:	Actually does the process of reading a bitmap from a file.
				Inherited classes override this to read in different file formats.
				
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BMPFilter::ReadFromFile( OILBitmap* pOilBitmap )
{
	ERROR2IF(pOilBitmap == NULL,FALSE,"BMPFilter::ReadFromFile null OilBitmap pointer");

	// Try to import bitmap as usual binary BMP file.
	// The only problem is that if the user has chosen to use the BMP filter explicitly
	// then this may not be what they really want. The BMP filter may not cope with all
	// BMP types, whereas we may possibly have an AccusoftFilter which might cope better.
	// If the user has not chosen explicitly then the BMP will be directed towards the 
	// correct filter by the HowCompatible call.
	// Generally, if the filter has been chosen at this point then HowCompatible will have
	// been called and so we can ask the BMP filter what it found when asked this and use
	// this result to determine whether to load the file or not. 

	UINT32 ImportMsgId = GetImportMsgID();		
	String_64 ProgressString(ImportMsgId);

	CCLexFile *pImportFile = GetImportFile();
	ERROR2IF(pImportFile==NULL,FALSE,"BMPFilter::ReadFromFile - No import file");

	ProgressString = GetImportProgressString(pImportFile, ImportMsgId);

	WinBitmap* pWBitmap = (WinBitmap*)pOilBitmap;
	
	LPBITMAPINFO *pInfo = &(pWBitmap->BMInfo);
	LPBYTE *pBytes = &(pWBitmap->BMBytes);

#ifndef EXCLUDE_FROM_RALPH
#ifndef WEBSTER
	if (
		(GetBmpCompatibility() == 10) || (AccusoftFilters::GetVersionNumber() == 0)
	   )
	{		
		// The BMP filter liked it very much and so use it, showing progress bar
		if (!DIBUtil::ReadFromFile(pImportFile, pInfo, pBytes, TRUE, &ProgressString))
			return FALSE;
	}
	else
	{
		// The BMP filter was not totally sure about that file and so we should	try and 
		// see if the Accusoft loader is present and if so give this an attempt at
		// loading that file. 		
		// Get a cast version of a pointer to the filter
		if (!AccusoftFilters::ReadFromFile(pImportFile, pInfo, pBytes, TRUE, &ProgressString))
			return FALSE;
	}
#else //WEBSTER
	//WEBSTER-Martin-07/01/97 all we can do is try to read it
	if (!DIBUtil::ReadFromFile(pImportFile, pInfo, pBytes, TRUE, &ProgressString))
		return FALSE;
#endif //WEBSTER
#else //EXCLUDE_FROM_RALPH
	if (!DIBUtil::ReadFromFile(pImportFile, pInfo, pBytes, TRUE, &ProgressString))
		return FALSE;
#endif //EXCLUDE_FROM_RALPH
	
	// Everything went ok and we imported the bitmap ok
	SetLastBitmap();		// can only import one bitmap at the moment
	return TRUE;
}

/********************************************************************************************

>	BOOL BMPFilter::GetExportOptions(BitmapExportOptions* pOptions)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	See BaseBitmapClass for interface details
	Notes:		Initializes BMP specific members
	Scope: 		Protected

********************************************************************************************/
BOOL BMPFilter::GetExportOptions(BitmapExportOptions* pOptions)
{
#ifdef DO_EXPORT
	ERROR2IF(pOptions == NULL, FALSE, "NULL Args");

	BMPExportOptions* pBMPOptions = (BMPExportOptions*)pOptions;
	ERROR3IF(!pBMPOptions->IS_KIND_OF(BMPExportOptions), "pBMPOptions isn't");

	WrittenHeader = FALSE;

	// Set up the variables which the export dialog box uses
	UINT32 BitmapDepth = 0;
	if (pBMPOptions->GetSelectionType() == ABITMAP)
	{
		// Exporting the bitmap itself
		// dpi should be set to the dpi of the bitmap itself
		BitmapDepth = pBMPOptions->GetDepth();
	}

	// Determine the filter type currently in use in Accusoft format
	INT32 FilterType = BMP_UNCOMPRESSED;

	// Sets the Accusoft filter type to the same specified value just in case
	// we wish to use the Accusoft DLL to do some of the BMP saving for us 
	//WEBSTER-Martin-07/01/97
#ifndef WEBSTER
	AccusoftFilters::SetFilterType(BMP_UNCOMPRESSED);
#endif

	// When saving a bitmap directly then only use the dialog box for 8 and 4 bpp export
	// as these are the only forms that can request a compression type.
	// Otherwise, always use it.
	BOOL Ok = TRUE;
	if ( pBMPOptions->GetSelectionType() != ABITMAP )
	{
		// This is ok as we are using a modal dialog box

		OpDescriptor* pOpDes = OpDescriptor::FindOpDescriptor(OPTOKEN_GIFTABDLG);
		if (pOpDes != NULL)
		{
			// set up the data for the export options dialog
			OpParam Param((INT32)pOptions, (INT32)this);

			// invoke the dialog
			pOpDes->Invoke(&Param);

			// SMFIX
			// we have brought the dlg up so get the options from the dlg as the graphic type may have changed
			pOptions = BmapPrevDlg::m_pExportOptions;

			// check for valid options
			//Ok = pOptions->IsValid();

			// This line decides if we will create a file on disk for the bmp or not
			Ok = BmapPrevDlg::m_bClickedOnExport;
		}
		else
		{	
			ERROR3("Unable to find OPTOKEN_BMAPPREVDLG");
			Ok = FALSE;
		} 
	}
	
	// Return with the ok/cancel state used on the dialog box
	return Ok;

#else
	return FALSE;
#endif
}


// SMFIX sjk 5/12/00 there used to be some junk in the call to GetExportOptions that assumed the
// filter type being used which could be changed by the GetExportOptions call itself
// therefore all this sort of stuff should be called on the correct known filter using this
// call afterwards
void BMPFilter::PostGetExportOptions(BitmapExportOptions* pOptions)
{
	// should be of this type
	BMPExportOptions* pBMPOptions = (BMPExportOptions*)pOptions;
	ERROR3IF(!pBMPOptions->IS_KIND_OF(BMPExportOptions), "pBMPOptions isn't");

	// do the baseclass first
	BaseBitmapFilter::PostGetExportOptions(pOptions);

	// do the filter specific stuff
	SetDepthToRender(pBMPOptions->GetDepth());
	PaletteType = pBMPOptions->GetPalette() ? 1 : 0;	// 1 for an optimised palette
	// Make sure that the Accusoft form of the stored dither is correctly set
	// as we may be calling this to do the exporting for us!
	//WEBSTER-Martin-07/01/97
#ifndef WEBSTER
	AccusoftFilters::SetDitherToUse(pBMPOptions->GetDither());
//Mark Howitt, 24/10/97. Reset the FilterType as import uses FilterType as something else!
	AccusoftFilters::SetFilterType(pBMPOptions->GetFilterType());
#endif
}


/********************************************************************************************

>	BOOL BMPFilter::ExportViaAccusoftFilters()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if going to use the Accusoft filter, FALSE if not
	Purpose:	Function to see if we should export via Accusoft filters or not as we cannot
				cope with this format.

********************************************************************************************/

BOOL BMPFilter::ExportViaAccusoftFilters()
{
#ifdef DO_EXPORT
	// Check if Accusoft filters are present
	// 8 and 4 as we cannot export as RLE in these formats
#ifndef WEBSTER
	return	(DefaultExportDepth==8 || DefaultExportDepth==4 || DefaultExportDepth==1) &&
			(AccusoftFilters::GetVersionNumber() > 0);
#else
	//WEBSTER-Martin-07/01/97
	return	FALSE;
#endif //WEBSTER
#else
	return FALSE;
#endif
}	

/********************************************************************************************

>	virtual BOOL BMPFilter::GetRenderBottomToTop()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if going to use the default of rendering bottom to top, FALSE otherwise
	Purpose:	Find out which way we need to render

********************************************************************************************/

BOOL BMPFilter::GetRenderBottomToTop()
{
	// We may be exporting via the Accusoft filters but when rendering BMPs they are
	// the correct way round already.
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BMPFilter::WriteToFile( BOOL End )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		End is TRUE if this is the last block of the file.
	Returns:	TRUE if worked, FALSE if errored.
	Purpose:	Physically put the bitmap into the disk.
				NOTE - ONLY COPES WITH End=TRUE currently
				Most of the work now is down by WriteDataToFile as this is a static form
				and there is a similar version in the Accusoft DLL. This can therefore be
				used to pass work onto the Accusoft DLL, for the cases we don't cope with. 
				End is ignored now and should always be set to TRUE.
	SeeAlso:	WriteDataToFile(); AccusoftFilters::WriteToFile; AccusoftFilters::WriteDataToFile;

********************************************************************************************/

BOOL BMPFilter::WriteToFile( BOOL End )
{
#ifdef DO_EXPORT
	// Functionality transferred to new static function so that we can share
	// the work between this built in BMP filter and the external Accusoft DLL.
	// if we are at the low depths of the bpp range then use the Accusoft DLL as on
	// 8 and 4 bpp the user may have chosen RLE compression and this filter cannot
	// cope with that.
	// Of course, only use the Accusoft DLL if it is present.
	if ( ExportViaAccusoftFilters() )
#ifndef WEBSTER
	{
		// Graeme (26-1-00) - I've added a call to the static SetFilterType method in the
		// AccusoftFilters class to set up the bitmap type to be an uncompressed bitmap.
		// This line isn't strictly necessary - the filter contains code to catch an unset
		// FilterType value, and set it to BMP_UNCOMPRESSED, but there potentially could be
		// problems if a JPEG or GIF value is passed in. Besides, it stops Camelot from
		// throwing an error message in debug builds, which adds just a little polish.
		AccusoftFilters::SetFilterType ( BMP_UNCOMPRESSED );

		return AccusoftFilters::WriteDataToFile ( End, DefaultExportDepth,
												  DefaultExportCompression,
												  GetBitmapExportOptions());
	}
#else
		//WEBSTER-Martin-07/01/97
		return FALSE;
#endif
	else	
		return WriteDataToFile(End, DefaultExportDepth, DefaultExportCompression);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL BMPFilter::EndWriteToFile( )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Inputs:		-
	Purpose:	Cleans up after writing the bitmap data out to a file. Inherited classes
				override this to write in different file formats.
	Returns:	TRUE if worked, FALSE if failed.
	SeeAlso:	WriteToFile

********************************************************************************************/

BOOL BMPFilter::EndWriteToFile( )
{
#ifdef DO_EXPORT
	// Must use the	same check here and trasnfer the clean up process to the same filter
	// that we used to actually write the data to the file
	if 	( ExportViaAccusoftFilters() )
#ifndef WEBSTER
		return AccusoftFilters::EndWriteDataToFile();
#else
		//WEBSTER-Martin-07/01/97
		return FALSE;
#endif //WEBSTER
	else	
		return EndWriteDataToFile();
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL BMPFilter::WriteDataToFile( BOOL End, UINT32 Bpp, UINT32 Compression)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/95
	Inputs:		End - TRUE if this is the last block of the file.
				Bpp - output depth in terms of bits per pixel
				Compression - True if compression required, False otherwise
	Returns:	TRUE if worked, FALSE if errored.
	Purpose:	Physically put the bitmap into the disk.
				NOTE - ONLY COPES WITH End=TRUE currently
				End is ignored now and should always be set to TRUE.
	SeeAlso:	WriteToFile(); AccusoftFilters::WriteToFile; AccusoftFilters::WriteDataToFile;

********************************************************************************************/

BOOL BMPFilter::WriteDataToFile( BOOL End, UINT32 Bpp, UINT32 Compression)
{
#ifdef DO_EXPORT
	if (GeneratingOptimisedPalette())
		return TRUE;		// No need to output anything


	BMPExportOptions* pBMPOptions = (BMPExportOptions*)GetBitmapExportOptions();
	ERROR2IF(pBMPOptions == NULL, FALSE, "NULL Args");
	ERROR3IF(!pBMPOptions->IS_KIND_OF(BMPExportOptions), "pBMPOptions isn't");

	LPBITMAPINFO lpBitmapInfo;
	LPBYTE lpBits;

	// Get the bitmap data from the Render Region
	ExportRegion->GetBitmapData(&lpBitmapInfo, &lpBits);

	ERROR3IF(lpBitmapInfo == NULL, "Bitmap has no info in BMPFilter::WriteDataToFile");
	if (lpBitmapInfo == NULL)
		return FALSE;

	ERROR3IF(lpBits == NULL, "Bitmap has no data in BMPFilter::WriteDataToFile");
	if (lpBits == NULL)
		return FALSE;

	// First time through, lets write out a suitable header.
	// As we write it in one big chunk (currently), its easy
	// If we are rendering in strips then the bitmap information in the render region will
	// be set for the height of the strip and not the height of the entire bitmap. 
	if (!WrittenHeader)
	{
		WrittenHeader = TRUE;

		LPBITMAPINFO lpInfo = lpBitmapInfo;
		LPLOGPALETTE lpPalette = NULL;
		ERROR2IF(lpInfo==NULL,FALSE,"BMPFilter::WriteDataToFile null export region bitmap");

		// Set up some useful variables giving details on the bitmap to save
		const UINT32 OutputDepth = Bpp;
		// First, get the size of the bitmap we are going to export not just the simple
		// lpInfo->bmiHeader.biHeight as this might just be the height of the first
		// strip to be exported. With the width we can use the old method.
		const UINT32 OutputWidth = lpInfo->bmiHeader.biWidth;
		const UINT32 OutputHeight = (INT32)ExportRegion->GetFullRegionHeight();
TRACEUSER( "Neville", _T("BMPFilter::WriteDataToFile Total output height = %d\n"),OutputHeight);
TRACEUSER( "Neville", _T("BMPFilter::WriteDataToFile height of 1st strip = %d\n"),lpInfo->bmiHeader.biHeight);

		if (OutputDepth==8 || OutputDepth==4 || OutputDepth==1 )
		{
			// SMFIX
			lpPalette = pBMPOptions->GetLogicalPalette();

			/*
			if (pOptimisedPalette)
			{
			 	lpPalette = pOptimisedPalette;
				TRACEUSER( "Will", _T("BMP filter is using an Optimised Palette\n"));
			}
			else
			{
				GDrawContext *GDC = GRenderRegion::GetDrawContext();
				// In the 4 or 1 bpp this will not necessarily be the palette that we
				// should export with as it will just be the default 8bpp palette.
				// Must tell it what depth (BPP) we want. The other information is superfluous
				// at present as this should be called again when the info is correct.
				GDC->SetupBitmap(OutputWidth, OutputHeight, OutputDepth, NULL );

				// WEBSTER - markn 8/2/97
				// Centralised the palette creation code for all bitmap filters
				lpPalette = Create8bppPalette();

				if (OutputDepth == 1)
				{
					// The easy one, I think black and white might be the correct choice here!
					OutputDIB::FixBlackAndWhitePalette(lpPalette);
				}
				else if	(OutputDepth == 4)
				{
					// Bit more tricky this one, Gavin uses a fixed 16 colour palette			
					OutputDIB::Fix16ColourPalette(lpPalette);
				}
			}
			*/
		}

		// Set up the compression flag only if the output depth is correct
		UINT32 Compress = BI_RGB;
		if (OutputDepth==4 && Compression)
			Compress = BI_RLE4;
		if (OutputDepth==8 && Compression)
			Compress = BI_RLE8;

		if (!DestDIB.StartFile(	OutputFile,
								&lpInfo->bmiHeader,				// pointer to BITMAPINFOHEADER 
								lpPalette,

								OutputDepth,					// actual file depth
								Compress,						// compression (BI_RGB = none)
								OutputHeight,					// all of it

								SizeOfExport,					// progress bar size
								FALSE,
								pBMPOptions->GetDither() )
			)
		{
//			if (lpPalette != pOptimisedPalette)
//				CCFree(lpPalette);

			return FALSE;
		}

//		if (lpPalette != pOptimisedPalette)
//			CCFree(lpPalette);
	}


	if (!End)
	{
		ENSURE(FALSE, "WriteToFile cannot do segmented writes");
		return FALSE;
	}

	// now lets write out our block
	const UINT32 StripHeight = lpBitmapInfo->bmiHeader.biHeight;
	if (!DestDIB.WriteBlock(	0,						// Ypos
								StripHeight,			// height of this strip
								lpBits,					// pointer to actual bitmap data
								32,						// input bpp
								ProgressOffset			// value to add to progress bar updats
							)
	   )
		return FALSE;

	// Add in the height of the strip that we have just exported to the progress offset value
	ProgressOffset += StripHeight;

#endif
	return TRUE;
}

/********************************************************************************************

>	static BOOL BMPFilter::EndWriteDataToFile( )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		-
	Purpose:	Actually does the job of cleaning up after writing the bitmap data out to a
				file. Usually called by EndWriteToFile. 
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BMPFilter::EndWriteDataToFile( )
{
#ifdef DO_EXPORT
	if (GeneratingOptimisedPalette())
		return TRUE;		// No need to output anything

	return DestDIB.TidyUp();
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	INT32 BMPFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, 
								 UINT32 FileSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/08/94
	Inputs:		Filename - name of the file.
				HeaderStart - Address of the first few bytes of the file.
				HeaderSize - the number of bytes in the header pointed to by FileStart.
				FileSize - the size of the whole file, in bytes.
	Returns:	0 => Not a BMP file.
				10 => It is a BMP file.
	Purpose:	Determine if this filter can load the specified file.

********************************************************************************************/



INT32 BMPFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, 
							 UINT32 FileSize)
{
	// We need to remember what we thought of this file in our class variable.
	// So, set it to a nice default value at the start.
	BMPHowCompatible = 0;

	// Check that we've got enough data to do our check
	if (HeaderSize < sizeof(BITMAPFILEHEADER))
		// Not enough data - ignore this file.
		return 0;

	// Check the header for the "BM" signature, and a sensible image size.
	LPBITMAPFILEHEADER pHeader = (LPBITMAPFILEHEADER) HeaderStart;

	if (pHeader->bfType != ( ('M'<<8) | 'B' ) )
		// No BMP signature - we don't want this file.
		return 0;

	// the other fields in the BITMAPFILEHEADER cannot be relied upon. The bfSize
	// field seems particularly random. We can check the size of the structure following
	// it though

	const DWORD HdrSize = *(DWORD*)(HeaderStart + sizeof(BITMAPFILEHEADER) );

	// If the header is of the correct size then now see what we think of this bitmap
	// header that we have been given. Cope with old OS/2 style headers and more up
	// to date ones.  
	// If the header was ok we used to just say 'we like it' immediately. What we want
	// to do now instead is only say we are really interested if it is special types of
	// BMP, such as 32bpp,  if it is anything else then say we are less interested in it.
	// This should allow the Accusoft BMP filter to take over the loading of BMPs which
	// we do not support. 
	// Pointer to the info header should be the next one after the file header 
	INT32 FileCompatibility = 0;	// default, didn't like this file value
	if (
		(HdrSize==sizeof( BITMAPCOREHEADER ))				// old OS/2 1.0
	   )
	{
		LPBITMAPCOREHEADER pCoreHeader = (LPBITMAPCOREHEADER)(pHeader+1);

		if ( DIBUtil::CanReadFromFile(pCoreHeader) )
			FileCompatibility = 10;
		else
			FileCompatibility = 9;
	}
	else if
	   ( 			
		(HdrSize==sizeof( BITMAPINFOHEADER ))				// Win 3.0+
	   )
	{
		LPBITMAPINFOHEADER pInfoHeader = (LPBITMAPINFOHEADER)(pHeader+1);

		// Use the function in DIBUtil to say if we like this file or not 
		if ( DIBUtil::CanReadFromFile(pInfoHeader) )
			FileCompatibility = 10;
		else
			FileCompatibility = 9;
	}

TRACEUSER( "Neville", _T("BMPFilter::HowCompatible FileCompatibility = %d\n"),FileCompatibility);
	// Remember what we thought in our class variable.
	BMPHowCompatible = FileCompatibility;
				
	// Return the found value to the caller.
	return FileCompatibility;
}

/********************************************************************************************

>	INT32 BMPFilter::GetBmpCompatibility()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/95
	Inputs:		-
	Returns:	The value we found in the HowCompatible call.
				0 => Not a BMP file.
				10 => It is a BMP file.
	Purpose:	Determine if this filter can load the specified file.

********************************************************************************************/

INT32 BMPFilter::GetBmpCompatibility()
{
	return BMPHowCompatible;
}

/********************************************************************************************

>	virtual BOOL BMPFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:		Pointer to the bitmap to save.
				Dpi of the bitmap to be saved
	Returns:	TRUE if worked, FALSE if errored.
	Purpose:	Physically put the bitmap into the disk.
	SeeAlso:	WriteDataToFile(); AccusoftFilters::WriteToFile; AccusoftFilters::WriteDataToFile;

********************************************************************************************/

BOOL BMPFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, double TheDpi)
{
#ifdef DO_EXPORT
	ERROR2IF(pKernelBitmap == NULL,FALSE,"BMPFilter::WriteBitmapToFile null bitmap pointer specified");

	// Get a pointer to the actual bitmap so that we can get some details from it.
	OILBitmap *pOilBitmap = pKernelBitmap->ActualBitmap;
	ERROR2IF(pOilBitmap == NULL,FALSE,"BMPFilter::WriteBitmapToFile null oil bitmap pointer");

	// Now get the pointer to the info header and actual bits data.
	// Need to use the actual bitmap pointer
	WinBitmap* pWBitmap = (WinBitmap*)pOilBitmap;
	LPBITMAPINFO Info = pWBitmap->BMInfo;
	LPBYTE Bytes = pWBitmap->BMBytes;
	UINT32 Bpp = pWBitmap->GetBPP();

	// Now, save the data out showing the correct progress string
	String_64 ProgressString(ExportingMsgID);
	BOOL ok = FALSE;
	// If the user has chosen a compressed BMP then we must use the Accusoft code,
	// if it is present as the built in one cannot do RLE compression.
	BOOL Compression = FALSE;
	//WEBSTER-Martin-07/01/97
#ifndef WEBSTER
	if (
		(DefaultExportCompression && (Bpp == 4)) ||
		(DefaultExportCompression && (Bpp == 8))
	   )		
	
	{
		// Set Compression TRUE and force the Accusoft filter type to be compressed.
		Compression = TRUE;
		AccusoftFilters::SetFilterType(BMP_COMPRESSED);
		AccusoftFilters::SetExportDpi(TheDpi);	
	}
	
	if 	( Compression && (AccusoftFilters::GetVersionNumber() > 0) )
		ok = AccusoftFilters::WriteToFile(OutputFile, Info, Bytes, &ProgressString);
	else
#endif //WEBSTER
		ok = DIBUtil::WriteToFile(OutputFile, Info, Bytes, &ProgressString);
	
	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL BMPFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, BaseCamelotFilter *pFilter,
											  CCLexFile *pFile, INT32 Compression);


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/96
	Inputs:		pKernelBitmap	- Pointer to the bitmap to be exported.
				pFilter			- Pointer to the BaseCamelot filter which provides progress functions
				pFile			- Pointer to the CCFile class to use for export
				Compression		- used to flag how much compression of the data is required.
	Returns:	TRUE if worked, FALSE if errored.
	Purpose:	Physically put the bitmap into the disk.  Inherited classes override this to write
				in different file formats.
				This is used by the native/web format to output the actual bitmap data content
				of a bitmap definition record. The function can assume that the CCFile is open
				and ready for writing and must use the functions provided by pFilter to update
				the progress system.
	SeeAlso:	BitmapListComponent::SaveBitmapDefinition;

********************************************************************************************/

BOOL BMPFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, BaseCamelotFilter *pFilter,
								  CCLexFile *pFile, INT32 Compression)
{
#ifdef DO_EXPORT
	ERROR2IF(pKernelBitmap == NULL,FALSE, "BMPFilter::WriteBitmapToFile null pKernelBitmap");
	ERROR2IF(pFilter == NULL,FALSE, "BMPFilter::WriteBitmapToFile null pFilter");
	ERROR2IF(pFile == NULL,FALSE, "BMPFilter::WriteBitmapToFile null pFile");

	// Get a pointer to the actual bitmap so that we can get some details from it.
	OILBitmap *pOilBitmap = pKernelBitmap->ActualBitmap;
	ERROR2IF(pOilBitmap == NULL,FALSE,"BMPFilter::WriteBitmapToFile null oil bitmap pointer");

	// Now get the pointer to the info header and actual bits data.
	// Need to use the actual bitmap pointer
	WinBitmap* pWBitmap = (WinBitmap*)pOilBitmap;
	LPBITMAPINFO Info = pWBitmap->BMInfo;
	LPBYTE Bytes = pWBitmap->BMBytes;
	UINT32 Bpp = pWBitmap->GetBPP();

	// Now, save the data out showing the correct progress string
	BOOL ok = FALSE;

	// We never worry about compresion of bmps for the present moment
   	// Just changed that so we zlib the bitmaps
	// Start up the compressor
	BOOL Ok = pFile->InitCompression();
	if (!Ok)
		return FALSE;
	// Now start the compression
	pFile->SetCompression(TRUE);
	
	// Write to file, no header and using pFilter for progress bar updates
	ok = DIBUtil::WriteToFile(pFile, Info, Bytes, NULL, FALSE, pFilter);
	
	// Now that we have finished, close the compressor down
	pFile->SetCompression(FALSE);

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL BMPFilter::IsThisBppOk(UINT32 Bpp)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/95
	Inputs:		Bpp or Colour depth.
	Returns:	TRUE if this filter can cope with this colour depth, FALSE otherwise.
	Purpose:	Check if this Bitmap filter can cope with saving at this Bpp/Colour depth.
	SeeAlso:	OpMenuExport::DoWithParam;

********************************************************************************************/

BOOL BMPFilter::IsThisBppOk(UINT32 Bpp)
{
	return (Bpp == 1 || Bpp == 4 || Bpp == 8|| Bpp == 24 || Bpp == 32);
}


/********************************************************************************************

>	virtual BitmapExportOptions* BMPFilter::CreateExportOptions() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	A pointer to a new BitmapExportInfo class
	Purpose:	Allows derived classes to override this function to provide their own class
				derived from BitmapExportInfo containing filter specific information.

********************************************************************************************/
BitmapExportOptions* BMPFilter::CreateExportOptions() const
{
	BMPExportOptions* pExportOptions = new BMPExportOptions(BMP_UNCOMPRESSED, &FilterName);

	return (BitmapExportOptions*)pExportOptions;
}


/********************************************************************************************

>	void BMPFilter::CleanUpAfterExport()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/07/97
	Returns:	-
	Purpose:	Allows the filter to clean up any allocated resources

********************************************************************************************/

void BMPFilter::CleanUpAfterExport()
{
	// call the base filter first
	BaseBitmapFilter::CleanUpAfterExport();

	WrittenHeader = FALSE;
}


void BMPFilter::AlterPaletteContents( LPLOGPALETTE pPalette )
{
	DestDIB.AlterExportPalette( pPalette );
}

