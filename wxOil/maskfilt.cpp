// $Id: maskfilt.cpp 662 2006-03-14 21:31:49Z alex $
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

// A PNG import/export filter 

#include "camtypes.h"
#include "app.h"
#include "docview.h"	// DocView
#include "maskfilt.h"
#include "dibutil.h"
//#include "wbitmap.h"	// Windows specific bitmap information	 
#include "grndbmp.h"
#include "maskedrr.h"	// MaskedRenderRegion derived off GRenderBitmap
#include "bmpcomp.h"	// BitmapListComponent
//#include "richard2.h"	// resource strings _R(IDS_GIFFILTR_FAILED_MASK)
//#include "resource.h"	// Inform Warning _R(IDS_OK)
//#include "will3.h"		// for _R(IDS_GENOPTPALMSGID)
//#include "bmpres.h"		// general bitmap filter based resources (_R(IDE_TIGIFFILTER_MASKFAILED)/MASK)
//#include "bmpfiltr.h"	// for GetDefaultDither()
#include "fixmem.h"
#include "bmapprev.h"	// To get the current transparent colour
#include "exjpeg.h"
#include "cxfrec.h"		// for CXaraFileRecord
#include "maskfilt.h"	// For MaskedFilterExportOptions.
//#include "mrhbits.h"	// For CBMPBits.
#include "selall.h"		// For OPTOKEN_EDITSELECTALL.
#include "palman.h"		// For PaletteManager.
#include "ink.h"		// For DeselectAll.
#include "layer.h"
#include "spread.h"		// For the spread interface.
#include "page.h"		// For the page interface.

CC_IMPLEMENT_DYNAMIC(MaskedFilter, BaseBitmapFilter)

#define	new	CAM_DEBUG_NEW

PORTNOTE("filters","Removed MaskedFilterExportOptions implementation")
#ifndef EXCLUDE_FROM_XARALX
CC_IMPLEMENT_DYNCREATE(MaskedFilterExportOptions, BitmapExportOptions)

PALETTE MaskedFilterExportOptions::g_Palette	= PAL_OPTIMISED;
DITHER	MaskedFilterExportOptions::g_Dither		= XARADITHER_NONE;	// Webster <RanbiR> Intialise the
																	// global dither type to No Dither.
BOOL	MaskedFilterExportOptions::g_bTransparent;
BOOL	MaskedFilterExportOptions::g_bInterlaced;
// WEBSTER - markn 17/1/97
UINT32	MaskedFilterExportOptions::g_NumColsInPalette = 256;
BOOL	MaskedFilterExportOptions::g_UseSystemColours = FALSE;

/********************************************************************************************

>	static BOOL MaskedFilterExportOptions::Declare()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Peter)
	Created:	29/10/96
	Returns:	TRUE if successfully declared preferences
				FALSE otherwise
	Purpose:	To declare preferences associated with these export options

********************************************************************************************/
BOOL MaskedFilterExportOptions::Declare()
{
	if (Camelot.DeclareSection("Filters", 10))
	{
		Camelot.DeclarePref( NULL, "Palette", (INT32*)&g_Palette, 0, 1 );
		Camelot.DeclarePref( NULL, "NumColsInPalette", (INT32*)&g_NumColsInPalette, 0, 256 );
		Camelot.DeclarePref( NULL, "UseSystemColours", (INT32*)&g_UseSystemColours, 0, 1 );
	}

	// All ok
	return TRUE;
}

/********************************************************************************************
>	MaskedFilterExportOptions::MaskedFilterExportOptions()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		Dialog : 
				pFilterName : the name of the filter 
	Purpose:	Default constructor
	Notes:		This is a base class for GIF, PNG, etc.. Since MakedFilter doesn't provide a
				CreateExportOptions() member subclasses should either derive a new class from
				this one or provide objects of this class in their CreateExportOptions()
********************************************************************************************/
MaskedFilterExportOptions::MaskedFilterExportOptions()
  : m_Dither(XARADITHER_NONE),							// Dither type
	m_bInterlaced(FALSE)
{
	m_NumColsInPalette = g_NumColsInPalette;			// override base class
	m_UseSystemColours = g_UseSystemColours;
}

/********************************************************************************************
>	MaskedFilterExportOptions::MaskedFilterExportOptions(const CDlgResID& Dialog, 
									const FILTER_ID FilterID, const StringBase* pFilterName)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		Dialog : 
				pFilterName : the name of the filter 
	Purpose:	Constructor
	Notes:		This is a base class for GIF, PNG, etc.. Since MakedFilter doesn't provide a
				CreateExportOptions() member subclasses should either derive a new class from
				this one or provide objects of this class in their CreateExportOptions()
********************************************************************************************/
MaskedFilterExportOptions::MaskedFilterExportOptions(const CDlgResID& Dialog, 
								const FILTER_ID FilterID, const StringBase* pFilterName)
  : BitmapExportOptions(Dialog, FilterID, pFilterName),
	m_Dither(XARADITHER_NONE),							// Dither type
	m_bInterlaced(FALSE)
{
	m_NumColsInPalette = g_NumColsInPalette;			// override base class
	m_UseSystemColours = g_UseSystemColours;
}

/********************************************************************************************

>	virtual BOOL MaskedFilterExportOptions::CopyFrom(BitmapExportOptions *pSource) 

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/97
	Inputs:		pSource - the other BitmapExportOptions object to copy the data from
	Purpose:	Sets the contents of this object from the passed object
	See Also:	BitmapExportOptions::MakeCopy()

********************************************************************************************/
BOOL MaskedFilterExportOptions::CopyFrom(BitmapExportOptions *pSource) 
{
	// copy the base class first
	if (!BitmapExportOptions::CopyFrom(pSource))
		return FALSE;

	// must be the same class, otherwise the base class  function above returns FALSE
	MaskedFilterExportOptions *pOther = (MaskedFilterExportOptions *)pSource;

	m_Dither		= pOther->m_Dither;
	m_bInterlaced	= pOther->m_bInterlaced;

	return TRUE;
}

/******************************************************************************************

>	BOOL MaskedFilterExportOptions::FileTypeChangeCopyFrom(BitmapExportOptions *pOther)

	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/99
	Inputs:		pOther - A BitmapExportOptions object we want to make a copy of.
	Returns:	-
	Purpose:	Copies the information from the BitmapExportOptions object
				passed in as the argument to the variables in this class

******************************************************************************************/
BOOL MaskedFilterExportOptions::FileTypeChangeCopyFrom(BitmapExportOptions *pOther)
{
	//  Do the base class piece first
	if ( !BitmapExportOptions::FileTypeChangeCopyFrom( pOther ) )
		return FALSE;

	MaskedFilterExportOptions* pOther2 = ( MaskedFilterExportOptions* )pOther;

	if ( pOther->IS_KIND_OF(BMPExportOptions) )
	{
		m_Dither		= pOther2->m_Dither;
		//  The following 2 don't exist in the bmp options.
		m_bInterlaced	= 0;
	}
	else if ( pOther->IS_KIND_OF(JPEGExportOptions) )
	{
		//  None of these exist in the jpeg options.
		//  So, give them default values.
		m_Dither		= g_Dither;
		m_bInterlaced	= 0;
	}
	else
	{
		//  Just copy them over
		m_Dither		= pOther2->m_Dither;
		m_bInterlaced	= pOther2->m_bInterlaced;
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL MaskedFilterExportOptions::Equal(BitmapExportOptions *pSource) 

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/97
	Inputs:		pSource - the other BitmapExportOptions object to copy the data from
	Purpose:	Compares the contents of this and pOther objects
	Returns:	TRUE, if objects are equal, FALSE otherwise
	See Also:	BitmapExportOptions::MakeCopy()

********************************************************************************************/
/*
BOOL MaskedFilterExportOptions::Equal(BitmapExportOptions *pSource) 
{
	BOOL ok = TRUE;

	// compare the base classes first
	ok = BitmapExportOptions::Equal(pSource);

	// must be the same class, otherwise the base class  function above returns FALSE
	MaskedFilterExportOptions *pOther = (MaskedFilterExportOptions *)pSource;

	if (ok) ok = (m_Dither	== pOther->m_Dither);
	if (ok) ok = (m_bInterlaced	== pOther->m_bInterlaced);
	return ok;
}
*/

/********************************************************************************************

>	BOOL MaskedFilterExportOptions::CopyFromMasked(MaskedFilterExportOptions *pOther) 

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/07/97
	Inputs:		pOther - the other MaskedFilterExportOptions object to copy the data from
	Purpose:	Sets the contents of this object from the passed object. This differs form the
				more general function in that it  doesn't require the two objects to be of the
				same class. Used to convert from/to PNG options and MakeBitmapExportOptions.
	See Also:	BitmapExportOptions::MakeCopy()

********************************************************************************************/
BOOL MaskedFilterExportOptions::CopyFromMasked(MaskedFilterExportOptions *pOther) 
{
	if (pOther == NULL)
		return FALSE;
	
	//BitmapExportOptions members
	m_SelectionType		= pOther->m_SelectionType;
	m_OutputSize		= pOther->m_OutputSize;
	m_PixelOutputSize	= pOther->m_PixelOutputSize;
	m_pBmpDlgParam		= pOther->m_pBmpDlgParam;		
	m_NumColsInPalette	= pOther->m_NumColsInPalette;
	m_UseSystemColours	= pOther->m_UseSystemColours;
	m_bSeparateLayerFiles = pOther->m_bSeparateLayerFiles;
	m_bTempFileFlag		= pOther->m_bTempFileFlag;

	SetDPI(pOther->GetDPI());
	SetDepth(pOther->GetDepth());
	SetTransparencyIndex(pOther->GetTransparencyIndex());
	if (pOther->IsValid())
		MarkValid();
	else
		MarkInvalid();

	//MaskedFilterExportOptions
	m_Dither		= pOther->m_Dither;
	m_bInterlaced	= pOther->m_bInterlaced;

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL MaskedFilterExportOptions::RetrieveDefaults()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	See BitmapExportOptions for interface details
	Notes:		Gets MaskedFilter specific preferences (compression mainly)

********************************************************************************************/
BOOL MaskedFilterExportOptions::RetrieveDefaults()
{
	if (!BitmapExportOptions::RetrieveDefaults())
		return FALSE;

	UINT32 Dither = BMPFilter::GetDefaultExportDither();
	ERROR2IF(Dither > 4, FALSE, "Dither Invalid");
	SetDither((DITHER)Dither);

	m_Dither  = g_Dither;
	m_NumColsInPalette = g_NumColsInPalette;
	m_UseSystemColours = g_UseSystemColours;

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL MaskedFilterExportOptions::SetAsDefaults() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	See BitmapExportOptions for interface details
	Notes:		Sets MaskedFilter specific preferences

********************************************************************************************/
BOOL MaskedFilterExportOptions::SetAsDefaults() const
{
	if (!BitmapExportOptions::SetAsDefaults())
		return FALSE;

	BMPFilter::SetDefaultExportDither(GetDither());

	// Derived classes can do their own thing with these...
	// m_bInterlaced;
	g_Dither  = m_Dither;
	g_NumColsInPalette = m_NumColsInPalette;
	g_UseSystemColours = m_UseSystemColours;

	return TRUE;
}

/********************************************************************************************
>	virtual BOOL MaskedFilterExportOptions::Write(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/00
	Inputs:		pRec	---		record to write
	Returns:	TRUE if successful.
	Purpose:	Writes this object to the given record.
	SeeAlso:	MaskedFilterExportOptions::Read
********************************************************************************************/
BOOL MaskedFilterExportOptions::Write(CXaraFileRecord* pRec)
{
	return BitmapExportOptions::Write(pRec) &&
		   pRec->WriteINT16((INT16) m_Dither) &&
		   pRec->WriteINT16((INT16) 0 /*m_Palette*/) &&
		   pRec->WriteBYTE((BYTE) TRUE) &&
		   pRec->WriteBYTE((BYTE) !!m_bInterlaced);
}

/********************************************************************************************
>	virtual BOOL MaskedFilterExportOptions::Read(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/00
	Inputs:		pRec	---		record to read
	Returns:	TRUE if successful.
	Purpose:	Reads this object from the given record.
	SeeAlso:	MaskedFilterExportOptions::Write
********************************************************************************************/
BOOL MaskedFilterExportOptions::Read(CXaraFileRecord* pRec)
{
	BYTE Unused;
	INT16 AlsoUnused;
	m_bInterlaced = FALSE;
	return BitmapExportOptions::Read(pRec) &&
		   pRec->ReadINT16((INT16*) &m_Dither) &&
		   pRec->ReadINT16((INT16*) &AlsoUnused /*m_Palette*/) &&
		   pRec->ReadBYTE(&Unused) &&
		   pRec->ReadBYTEtoBOOL(&m_bInterlaced);
}

/********************************************************************************************

>	BOOL MaskedFilterExportOptions::UseSpecificNumColsInPalette()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Returns:	TRUE if the options define a specific number of colours in the palette
	Purpose:	Masked filters have the ability to specify the exact number of colours in the
				generated optimised palette.

				This only returns TRUE if we are generating a bitmap from objects in the document.
				It is not available when exporting a bitmap directly, as it will already have a palette

				Introduced for WEBSTER

********************************************************************************************/
BOOL MaskedFilterExportOptions::UseSpecificNumColsInPalette()
{
	// Don't use specific num colours when exporting bitmaps
	return (GetSelectionType() != ABITMAP && GetSelectionType() != SOMEBITMAPS);
}

/********************************************************************************************

>	BOOL MaskedFilterExportOptions::SetDither(const DITHER& Dither)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		Dither : The desired dither to be applied to the bitmap before being exported
	Returns:	TRUE if set successfully
				FALSE otherwise
	Purpose:	Support function to set up information for the ensuing export
	See Also:	GetDither()

********************************************************************************************/
BOOL MaskedFilterExportOptions::SetDither(const DITHER& Dither)
{
	if (m_Dither != Dither)
		SetTempFileMatchesExportOptions(FALSE);
	m_Dither = Dither;
	return TRUE;
}

/********************************************************************************************

>	BOOL MaskedFilterExportOptions::WantTransparent() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	TRUE if the bitmap should be exported interlaced, FALSE not
	Purpose:	Support function to obtain export options
	See Also:	SetMakeTransparent()

********************************************************************************************/
BOOL MaskedFilterExportOptions::WantTransparent() const
{
	// smfix - only go on the transp index
	return GetTransparencyIndex() != -1;
}

/********************************************************************************************

>	BOOL MaskedFilterExportOptions::SetMakeInterlaced(BOOL bInterlaced)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		bInterlaced : TRUE to indicate the exported bitmap should be interlaced
				...somehow; FALSE for normal non-interlaced mode
	Returns:	TRUE if set successfully
				FALSE otherwise
	Purpose:	Support function to set up information for the ensuing export
	See Also:	WantInterlaced()

********************************************************************************************/
BOOL MaskedFilterExportOptions::SetMakeInterlaced(BOOL bInterlaced)
{
	if (m_bInterlaced != bInterlaced)
		SetTempFileMatchesExportOptions(FALSE);
	m_bInterlaced = bInterlaced;
	return TRUE;
}

#endif

/********************************************************************************************

>	MaskedFilter::MaskedFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/7/96
	Purpose:	Constructor for an MaskedFilter object.  The object should be initialised
				before use.
	SeeAlso:	MaskedFilter::Init

********************************************************************************************/

MaskedFilter::MaskedFilter()
{
	// Used for storing the 8bpp mask until after the second pass is completed
	pDestBMInfo = NULL;
	pDestBMBytes = NULL;
	pTempBitmapMask = NULL;

	// Initalise flags
	SecondPass = FALSE;
	DoingMask = FALSE;
	TransColour = -1;					// -1 == none, or the transparent colour.
	CurrentScanline = 0;

	//  We don't have any bands to work with yet.
	m_BandNumber = 0;
	m_bSelection = TRUE;
}

/********************************************************************************************

>	BOOL MaskedFilter::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/7/96
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise a MaskedFilter object.

********************************************************************************************/
BOOL MaskedFilter::Init()
{
	PORTNOTETRACE("filters","MaskedFilter::Init - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	return MaskedFilterExportOptions::Declare();
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	BOOL MaskedFilter::ApplyTransparentColoursToBitmap(KernelBitmap* pExportBitmap,
													   BitmapExportOptions* pExportOptions)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/00
	Inputs:		pExportBitmap - the Bitmap to do the work on.
				pExportOptions - A pointer to the options to be applied.
	Returns:	TRUE if everything went ok!
	Purpose:	Same as the other flavour of this function.

********************************************************************************************/
BOOL MaskedFilter::ApplyTransparentColoursToBitmap(KernelBitmap* pExportBitmap, BitmapExportOptions* pExportOptions)
{
	// Check the passed in Variables
	if(!pExportBitmap || !pExportOptions)
	{
		ERROR3("Failed to get valid pointers!");
		return FALSE;
	}

	// Now get the info and data from the passed in Bitmap
	BITMAPINFOHEADER* pInfoHeader = pExportBitmap->GetBitmapInfoHeader();
	BYTE* pBMPBits = pExportBitmap->GetBitmapBits();

	// Call the second version of this function
	return ApplyTransparentColoursToBitmap(pInfoHeader,pBMPBits,pExportOptions);
}

/********************************************************************************************

>	BOOL MaskedFilter::ApplyTransparentColoursToBitmap(BITMAPINFOHEADER* pInfoHeader,
													   BYTE* pBMPBits,
													   BitmapExportOptions* pExportOptions,
													   UINT32 YOffset, UINT32 StripHeight)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/00
	Inputs:		pInfoHeader - the header to the Bitmap to do the work on.
				pBMPBits - the bits to the Bitmap to do the work on.
				pExportOptions - A pointer to the options to be applied.
				YOffset - The number of lines into the bitmap to start from.
				StripHeight - The height of the current clip.
	Returns:	TRUE if everything went ok!
	Purpose:	Sets the relavent bits of the bitmap to transparent depending
				on the given options.

********************************************************************************************/
BOOL MaskedFilter::ApplyTransparentColoursToBitmap(BITMAPINFOHEADER* pInfoHeader, BYTE* pBMPBits,
												   BitmapExportOptions* pExportOptions, UINT32 YOffset, UINT32 StripHeight)
{
	// Check the passed in pointers!
	if(!pInfoHeader || !pBMPBits || !pExportOptions)
	{
		ERROR3("Failed to get valid pointers!");
		return FALSE;
	}

	PORTNOTETRACE("filters","MaskedFilter::ApplyTransparentColoursToBitmap - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Get the current Export palette and find out how many colours we`re using.
	ExtendedPalette* pPal = pExportOptions->GetPalette();
	ERROR3IF(!pPal,"Failed to get a valid palette pointer!");

	INT32 NumOfTranspColours = 0;
	INT32 TransColour[256];

	// First get the Transparency index and the check the background status;
	INT32 TranspIndex = pExportOptions->GetTransparencyIndex();
	BOOL TransparentBackground = pExportOptions->IsBackgroundTransparent(); 
	INT32 flag = 0;

	if(pPal)
	{
		// Loop through the colours getting all the one that are marked transparent!
		for(INT32 i = 0; i < pPal->NumberOfColours; i++)
		{
			flag = pPal->Data[i].Flags;

			// If we have the backgroung as transparent then we need to make sure we don`t
			// include it in the locked colour checking!
			if((flag & TRANSPARENT_COLOUR) && !(TransparentBackground && i == TranspIndex))
				TransColour[NumOfTranspColours++] = i;
		}
	}
	
	// Make sure we`re not doing something stupid!
	if(NumOfTranspColours == 0 && !TransparentBackground)
	{
		ERROR3("What are we tring to do? Durrrr!");
		return FALSE;
	}

	GRenderBitmap* pMaskedRegion = NULL;
	BITMAPINFO* pAlphaInfo = NULL;
	BYTE* pAlphaBits = NULL;

	// If we want a transparent background then we need to get a 32Bit Alpha BMP!
	if(TransparentBackground)
	{
		// Create a new 32bpp export region with alpha channel info!
		// Don't use rendering matrix when exporting BMPs as it uses coordinates as is
		Matrix Identity;
		FIXED16 Scale(1);
		pMaskedRegion = new GRenderBitmap(ClipRect, Identity, Scale, 32, pExportOptions->GetDPI(),FALSE,0,NULL,TRUE);

		// If the render region was not created, drop out of this function.
		if ( pMaskedRegion == NULL )
			return FALSE;

		pMaskedRegion->SetFilter(this);

		// Now make sure we set the compression flag to enable transpareny info!
		pMaskedRegion->m_DoCompression = TRUE;
		pMaskedRegion->SetRenderComplexShapes(TRUE);
		RRCaps	NewCaps;
		NewCaps.CanDoNothing();

		DocView *View = DocView::GetCurrent();
		
		if (View)
		{
			// Must force background rendering off as otherwise we might get a partly
			// rendered drawing or object if it is complex.
			// We have already remembered the entry state so just force it.
			ForceBackgroundRedrawOff(View);

			ERROR3IF ( GetBitmapExportOptions()->GetDepth() == -1,
					   "Illegal Output Depth in MaskedFilter::RenderTransparencyMask" );

			if (GetBitmapExportOptions()->GetDepth() == 1)
			{
				// If we are outputting at 1bpp, then we'll force antialiasing off,
				// so that Gavin's palette optimisation can detect objects which 
				// are 'one solid colour'
				ForceAntialiasingOff(View);
			}

			// Attach to the right device. (note no DC)
			pMaskedRegion->AttachDevice(View, NULL, pSpread);
		}
		else
			ERROR2(FALSE,"MaskedFilter::RenderTransparencyMask no current view");

		// Export the data from the view into the 1bpp region we have, ensuring rendering
		// in strips is off
		RenderInStrips = FALSE;

		// Now render everything into the new 32Bit Alpha channel region!
		if (!ExportRender(pMaskedRegion, TRUE))
		{
			// If we have created the export region then delete it so that we free up
			// its huge wadges of memory (as its 32bpp deep!).
			delete pMaskedRegion;
			pMaskedRegion = NULL;

			return FALSE;
		}

		// Now get the info from the 32bit render region
		if(!pMaskedRegion->GetBitmapData(&pAlphaInfo,&pAlphaBits))
		{
			delete pMaskedRegion;
			pMaskedRegion = NULL;
			return FALSE;
		}

		// Check to see if we`ve got Identically dimensioned Bitmaps!
		if(pInfoHeader->biWidth != pAlphaInfo->bmiHeader.biWidth ||
		   (YOffset + StripHeight) > (UINT32)pAlphaInfo->bmiHeader.biHeight)
		{
			delete pMaskedRegion;
			pMaskedRegion = NULL;
			return FALSE;
		}
	}

	// make a DWORD Pointer to the Alpha Channel bits
	DWORD* pAlpha = (DWORD*)pAlphaBits;

	// Now get the scanline width for the current bitmap. This will be in multiples of BYTEs
	INT32 ScanLineWidth = DIBUtil::ScanlineSize(pInfoHeader->biWidth,pExportOptions->GetDepth());
	INT32 Depth = pExportOptions->GetDepth();
	INT32 PixelsPerByte = 8 / Depth;

 	// Get the width in pixels for the 32 bit bitmap
	INT32 Width32 = pInfoHeader->biWidth / PixelsPerByte;
	INT32 SubWidth32 = pInfoHeader->biWidth % PixelsPerByte;
	INT32 Scanline = 0;
	INT32 StartingPos = 0;
	INT32 PixelPos = 0;
	INT32 SubPixPos = 0;
	INT32 NumOfSubPixels = 0;
	INT32 PixelsPerLine = 0;
	INT32 i = 0;
	INT32 j = 0;
	DWORD ByteData = 0x0;
	BOOL Padding = FALSE;
	UINT32 StripOffset = 0;
	UINT32 NoOfLines = StripHeight;

	// As 8Bit, 4Bit and 1Bit BMPs are DWORD Alligned, we need to be a little clever in making sure we don`t
	// Over run the 32Bit Scanlines! What we need to do is workout exactly how many bytes/bits we really have
	// as bitmap!
	// As 8Bit BMPs are BYTE Alligned, we don`t need to anything special and just use the bitmap width as is!
	// 4Bit is a little more probmatic in that it uses Half Bytes and even worse is 1Bit BMPs!

	if(NoOfLines == 0)
		NoOfLines = pInfoHeader->biHeight;

	if(YOffset > 0)
	{
		i = ((Width32 * PixelsPerByte) + SubWidth32) * YOffset;
		StripOffset = YOffset;
	}

	// Loop through line by line setting up the transparency information
	for (Scanline = StripOffset; (UINT32)Scanline < NoOfLines + StripOffset; Scanline++)
	{
		StartingPos = Scanline * ScanLineWidth;
		PixelsPerLine = StartingPos + Width32;

		if(SubWidth32 > 0)
		{
			PixelsPerLine++;
		}

		for(PixelPos = StartingPos; PixelPos < PixelsPerLine; PixelPos++)
		{
			// Get the current Byte of BMP Data at the current Position
			ByteData = _rotr(pBMPBits[PixelPos],8);

			NumOfSubPixels = PixelsPerByte;

			if(PixelPos == StartingPos + Width32)
				NumOfSubPixels = SubWidth32;

			// Now loop through all the Sub Byte Pixels setting their appropriate values
			for(SubPixPos = 0; SubPixPos < PixelsPerByte; SubPixPos++)
			{
				ByteData = _rotl(ByteData,Depth);
				Padding = (SubPixPos >= NumOfSubPixels);

				// First check to see if the alpha channel is fully transparent
				if((pAlpha && ((pAlpha[i++] & 0xFF000000) >> 24) == 0xFF) || Padding)
				{
					if(Padding)
						i--;

					if(Depth == 8)
					{
						ByteData = TranspIndex;
					}
					else if(Depth == 4)
					{
						ByteData &= 0xFFFFFFF0; // Clear the bits we`re going to replace!
						ByteData |= (0xF & TranspIndex); // Now AND it with the TranspIndex
					}
					else if(Depth == 1)
					{
						ByteData &= 0xFFFFFFFE; // Clear the bits we`re going to replace!
						ByteData |= (0x1 & TranspIndex); // Now AND it with the TranspIndex
					}
				}
				else // If it`s not transparent then check the users transparent colours!
				{
					for(j = 0; j < NumOfTranspColours; j++)
					{
						if(Depth == 8 && ByteData == (BYTE)TransColour[j])
						{
							ByteData = TranspIndex;
							break;
						}
						else if(Depth == 4 && (0xF & ByteData) == (BYTE)TransColour[j])
						{
							ByteData &= 0xFFFFFFF0; // Clear the bits we`re going to replace!
							ByteData |= (0xF & TranspIndex); // Now AND it with the TranspIndex
							break;
						}
						else if(Depth == 1 && (0x1 & ByteData) == (BYTE)TransColour[j])
						{
							ByteData &= 0xFFFFFFFE; // Clear the bits we`re going to replace!
							ByteData |= (0x1 & TranspIndex); // Now AND it with the TranspIndex
							break;
						}
					}
				}
			}

			// Replace the current data with the processed version!
			pBMPBits[PixelPos] = (BYTE)ByteData;
		}
	}

	// Now delete the masked region if we created one!
	if(pMaskedRegion)
	{
		delete pMaskedRegion;
		pMaskedRegion = NULL;
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL MaskedFilter::WriteToFile ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>	(From Neville's code.)
	Created:	27/6/00	(Originally 24/4/95.)
	Returns:	TRUE	- The export was successful.
				FALSE	- An error occured.
	Purpose:	Physically put the bitmap into the disk.

				Most of the work now is down by WriteDataToFile as this is a static form
				and there is a similar version in the Accusoft DLL. This can therefore be
				used to pass work onto the Accusoft DLL, for the cases we don't cope with. 

				This function was originally duplicated in the TI_GIFFilter, and PNG_Filter,
				but have now been rolled into a single function. This makes maintenance
				easier, and should reduce code size.

********************************************************************************************/
BOOL MaskedFilter::WriteToFile ( BOOL End )
{
#ifdef DO_EXPORT

	if(GeneratingOptimisedPalette())
		return TRUE; // No need to output anything.

	BitmapExportOptions* pBMPOptions = GetBitmapExportOptions();
	OutputDIB* pDestDIB	= GetOutputDIB();

	LPBITMAPINFO lpBitmapInfo = NULL;
	LPBYTE		 lpBitmapBits = NULL;
	LPLOGPALETTE lpPalette = NULL;

	// Make sure that valid values have been set.
	ERROR2IF ( pBMPOptions == NULL,	FALSE, "NULL Args" );
	ERROR2IF ( pDestDIB == NULL,	FALSE, "No DIB available for export" );

	// Check that the class variable for the spread to use has been set up in.
	ERROR2IF(pSpread==NULL,FALSE,"PNGFilter::WriteToFile spread pointer is null");

	//  Increment the band number.
	m_BandNumber++;

	// Get the bitmap data from the Render Region
	ExportRegion->GetBitmapData(&lpBitmapInfo, &lpBitmapBits);

	ERROR3IF(lpBitmapBits == NULL, "Bitmap has no data in BMPFilter::WriteDataToFile");

	if (lpBitmapInfo == NULL || lpBitmapBits == NULL)
		return FALSE;

	// Now call InvertAlpha() - This is for the PNG Derived filter
	InvertAlpha(lpBitmapInfo,lpBitmapBits);

	// Lets write out a suitable header.
	// As we write it in one big chunk (currently), its easy
	if (!WrittenHeader)
	{
		WrittenHeader = TRUE;
		
		LPBITMAPINFO lpInfo = lpBitmapInfo;
		ERROR2IF(lpInfo==NULL,FALSE,"PNGFilter::WriteToFile null export region bitmap");
		if (pTempBitmapMask != NULL)
		{
			TRACEUSER( "SimonK", _T("pTempBitmapMask was not deleted when it should have been"));
			CCFree(pTempBitmapMask);
			pTempBitmapMask = NULL;
		}
		
		ERROR3IF ( pBMPOptions->GetDepth() == -1, "Illegal OutputDepth in PNGFilter::WriteToFile" );

		// First, get the size of the bitmap we are going to export not just the simple
		// lpInfo->bmiHeader.biHeight as this might just be the height of the first
		// strip to be exported. With the width we can use the old method.
		const UINT32 OutputWidth = lpInfo->bmiHeader.biWidth;
		const UINT32 OutputHeight = (INT32)ExportRegion->GetFullRegionHeight();

		if ( pBMPOptions->GetDepth () == 8 ||
			 pBMPOptions->GetDepth () == 4 ||
			 pBMPOptions->GetDepth () == 1 )
		{
			// SMFIX
			// get the logical palette from the options
			lpPalette = pBMPOptions->GetLogicalPalette();
		}
		
		// Start up the export process and prepare all the bitmaps and files ready for
		// the actual bitmap data
		// For transparent export we will do this twice so flag which pass we are on. 
		if ( !pDestDIB->StartFile ( &lpInfo->bmiHeader, 
									lpPalette,
									pBMPOptions->GetDepth(),		// actual file depth
									Compression,					// interlace/transparency
									OutputHeight,					// all of it
									SizeOfExport,					// progress bar size
									pBMPOptions->GetDither() ) )	// dithering to use
		{
			return FALSE;
		}
	}

	// Now lets write out our block. This actually just copies the data into our
	// destination bitmap. Need to do this so cope with interlacing and transparency
	const UINT32 StripHeight = lpBitmapInfo->bmiHeader.biHeight;

	// SMFIX - Mark Howitt
	// New code to make sure we export with transparency correctly!
	INT32 InputBPP = 32;
	BYTE* pActualBits = lpBitmapBits;
	if ( !pDestDIB->WriteBlock ( 0,						// Ypos
								 StripHeight,			// height of this strip
								 pActualBits,			// pointer to actual bitmap data 
								 InputBPP,				// input bpp
								 ProgressOffset ) )		// value to add to progress update
	{
		return FALSE;
	}

	// if we have a transparency and we are 8 bit or less
	if (pBMPOptions->GetTransparencyIndex() > -1 && pBMPOptions->GetDepth() <= 8)
	{
		// Now send the intermediate bitmap to the ApplyTransparentColoursToBitmap function
		if(!ApplyTransparentColoursToBitmap(&pDestDIB->GetDestBitmapInfo()->bmiHeader,pDestDIB->GetDestBitmapBits(),pBMPOptions,m__StripStart,StripHeight))
		{
			ERROR2(FALSE,"Failed to create an intermediate bitmap pointer!");
		}
	}

	// Add in the height of the strip that we have just exported to the progress offset value
	ProgressOffset += StripHeight;
	m__StripStart += StripHeight;
#endif
	return TRUE;
}

/********************************************************************************************

>	INT32	MaskedFilter::SetTransColour(INT32 NewColour)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/95
	Inputs:		NewColour	colour to set as the new transparent colour, -1 if none
	Returns:	The old transparent colour.
	Purpose:	To set a new transparent colour. This has most likely been read in from a
				GIF file and so needs applying to the bitmap that has just been loaded.

				Moved from TI_GIFFilter to MaskedFilter 10/7/96

	SeeAlso:	TI_GIFFilter::GetTransColour();

********************************************************************************************/
INT32	MaskedFilter::SetTransColour(INT32 NewColour)
{
	INT32 OldTransColour = TransColour;
	TransColour = NewColour;
	
	return OldTransColour;	
}

/********************************************************************************************

>	virtual BOOL MaskedFilter::ExportSelectionOnly()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/95
	Inputs:		-
	Outputs:	-
	Returns:	True if this filter wants check if an object is selected or not	and so export
				only the selected items Or False if want to export everything.
	Purpose:	Determines if the filter wants to export only selected items.
				In the base filters class version the default action will be to
				export all objects by default

				Moved from TI_GIFFilter to MaskedFilter 10/7/96

	SeeAlso:	Filter::ExportRender;

********************************************************************************************/
BOOL MaskedFilter::ExportSelectionOnly(BOOL MaskedRender)
{
	// we dont do any of this secondpass / GeneratingOptimisedPalette bussiness anymore
	// that is all done in a nice tight loop. So we would never want to just render the selection
	// otherwise the background sometimes disappears which would be odd. sjk 3/1/01
	PORTNOTETRACE("filters","MaskedFilter::ExportSelectionOnly - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	MaskedFilterExportOptions* pMaskedOptions = (MaskedFilterExportOptions*)GetBitmapExportOptions();
	ERROR2IF(pMaskedOptions == NULL, FALSE, "NULL Args");
	ERROR3IF(!pMaskedOptions->IS_KIND_OF(MaskedFilterExportOptions), "pMaskedOptions isn't");
	return (pMaskedOptions->GetSelectionType() == SELECTION && 
		((pMaskedOptions->IsBackgroundTransparent() && MaskedRender)
		|| pMaskedOptions->GetDepth() == 32));
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL MaskedFilter::CheckSingleBitmapsOnSpread ( Spread	*pSpread,
															BOOL	*pSamePalettes,
															BOOL	*pOurBrowserPalette,
															BOOL	*pAllHaveBitmaps,
															BOOL	*pAllWithinCount,
															UINT32	*pPaletteEntries )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/97
	Inputs:		pSpread				the spread that the bitmaps have been put on
				pPaletteEntries		Set to the currently requested number of palette entries
	Outputs:	pSamePalettes		Set if the bitmaps have all the same palette
				pOurBrowserPalette	Set if all bitmaps have our browser palette
				pAllHaveBitmaps		Set if all the relevant layers have bitmaps on them
				pAllWithinCount		Set if all the palettes have less entries than required.
				pPaletteEntries		Set to the max colours in palette found. or number of
									palette entries if SamePalettes is true.
	Returns:	TRUE if was ok, FALSE otherwise.
	Purpose:	This goes through all bitmaps that are on the layers and sees whether they
				have the same palette and whether the palette is the same as our browser
				Assumes that each bitmap has been added to a separate layer and that the
				referenced bitmap in the layer is set to this loaded bitmap.
				SamePalettes and OurBrowserPalettes are just set for the bitmaps found.

********************************************************************************************/
BOOL MaskedFilter::CheckSingleBitmapsOnSpread ( Spread	*pSpread,
												BOOL	*pSamePalettes,
												BOOL	*pOurBrowserPalette,
												BOOL	*pAllHaveBitmaps,
												BOOL	*pAllWithinCount,
												UINT32	*pPaletteEntries )
{
	ERROR2IF ( pSpread == NULL,FALSE,"CheckSingleBitmapsOnSpread pSpread = NULL" );
	ERROR2IF ( pSamePalettes == NULL ||
			   pOurBrowserPalette == NULL ||
			   pAllHaveBitmaps == NULL,
			   FALSE, "CheckSingleBitmapsOnSpread bad params!" );
	
	// Walk through the layers looking at the referenced bitmaps, which are the newly added
	// bitmaps.
	BOOL SamePalettes		= TRUE;
	BOOL OurBrowserPalette	= TRUE;
	BOOL AllHaveBitmaps		= TRUE;
	BOOL BitmapFound		= FALSE;
	BOOL AllWithinCount		= TRUE;
	UINT32 Bpp				= 0;

	KernelBitmap *	pBitmap					= NULL;
	LPLOGPALETTE	pBrowserPalette			= NULL;
	LPRGBQUAD		pPreviousPalette		= NULL;
	UINT32			PreviousPalEntries		= 0;
	UINT32			ReqNumberOfPalEntries	= 0;
	UINT32			MaxPaletteEntries		= 0;
	if (pPaletteEntries != NULL)
		ReqNumberOfPalEntries = *pPaletteEntries;

	Layer * pLayer = pSpread->FindFirstLayer();
	while (pLayer != NULL)
	{
		// Only check pseudo frame layers as these are what the grab frame ops use
		if (!pLayer->IsBackground() && !pLayer->IsGuide() &&
			!pLayer->IsPageBackground())
		{
			pBitmap = pLayer->GetReferencedBitmap();
			if (pBitmap)
			{
				BitmapFound = TRUE;

				Bpp = pBitmap->GetBPP();
				if (Bpp <= 8)
				{
					UINT32 PalEntries = pBitmap->GetNumPaletteEntries();
					// Note this as a possible new maximum number of entries
					if (PalEntries > MaxPaletteEntries)
						MaxPaletteEntries = PalEntries;
					LPRGBQUAD pPalette = pBitmap->GetPaletteForBitmap();
					if (PalEntries > ReqNumberOfPalEntries)
						AllWithinCount = FALSE;

					// Obviously, browser palettes can only happen on 8bpp bitmaps
					if (Bpp == 8 && OurBrowserPalette)
					{
						// If we haven't done so already, allocate ourselves a browser
						// palette.
						// This MUST be the same one that we will use on saving
						if (pBrowserPalette == NULL)
						{
							UINT32 PaletteSize = UINT32( pow( 2.0, double(Bpp) ) );
							pBrowserPalette = DIBUtil::AllocateLogPalette(PaletteSize);
							if (pBrowserPalette != NULL)
							{
								// Now force it into a browser compatible state
								BOOL AvoidSystemColours = TRUE;
								PaletteManager::MakePaletteBrowserCompatible
												( pBrowserPalette, AvoidSystemColours );
							}
						}

						INT32 TransColour = -1;
						pBitmap->GetTransparencyIndex(&TransColour);

						if (pBrowserPalette != NULL && OurBrowserPalette)
							OurBrowserPalette = pBitmap->ArePalettesTheSame(pBrowserPalette,TransColour);
						else
							OurBrowserPalette = FALSE;
					}
					else if (SamePalettes)
					{
						OurBrowserPalette = FALSE;

						// If we have two palettes to compare then do so,
						// Otherwise wait for a second palette
						if (pPalette && pPreviousPalette)
						{
							if (PalEntries == PreviousPalEntries)
							{
								for (UINT32 i = 0; (i < PalEntries) && SamePalettes; i++)
								{
									if ( pPalette[i].rgbRed		!= pPreviousPalette[i].rgbRed &&
										 pPalette[i].rgbGreen	!= pPreviousPalette[i].rgbGreen &&
										 pPalette[i].rgbBlue	!= pPreviousPalette[i].rgbBlue
										)
										SamePalettes = FALSE;
								}
							}
							else
								SamePalettes = FALSE;
						}
					}
					else
						OurBrowserPalette = FALSE;
				}
				else
				{
					OurBrowserPalette = FALSE;
					SamePalettes = FALSE;
				}

				// Note this bitmap as the previous one
				PreviousPalEntries = pBitmap->GetNumPaletteEntries();
				pPreviousPalette = pBitmap->GetPaletteForBitmap();
			}
			else
				AllHaveBitmaps = FALSE;
		}

		pLayer = pLayer->FindNextLayer();
	}

	// If no bitmaps have been found then we must reset the palette flags as otherwise
	// they will be showing the wrong value.
	if (!BitmapFound)
	{
		SamePalettes		= FALSE;
		OurBrowserPalette	= FALSE;
		AllWithinCount		= FALSE;
	}

	// return the results in the variables supplied
	if (pSamePalettes)
		*pSamePalettes = SamePalettes;
	if (pOurBrowserPalette)
		*pOurBrowserPalette = OurBrowserPalette;
	if (pAllHaveBitmaps)
		*pAllHaveBitmaps = AllHaveBitmaps;
	if (pAllWithinCount)
		*pAllWithinCount = AllWithinCount;

	if (pPaletteEntries)
	{
		// If we have the same palette on all bitmaps then set the number of palette
		// entries to be correct, otherwise set to the maximum found.
		if (SamePalettes)
			*pPaletteEntries = PreviousPalEntries;
		else
			*pPaletteEntries = MaxPaletteEntries;
	}

	// Free up our allocated browser palette
	if (pBrowserPalette != NULL)
		CCFree(pBrowserPalette);

	return TRUE;
}

/********************************************************************************************
>	UINT32 MaskedFilter::GetNumReservedColours()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		-
	Returns:	The numbers of colours the filter would like to reserve.
	Purpose:	If exporting a transparent GIF then reserve 1 palette index for the
				transpacency mask index.

				Moved from TI_GIFFilter to MaskedFilter 10/7/96
	SeeAlso:	-
********************************************************************************************/
UINT32 MaskedFilter::GetNumReservedColours()
{
	PORTNOTETRACE("filters","MaskedFilter::GetNumReservedColours - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	MaskedFilterExportOptions* pMaskedOptions = (MaskedFilterExportOptions*)GetBitmapExportOptions();
	ERROR2IF(pMaskedOptions == NULL, FALSE, "NULL Member");
	ERROR3IF(!pMaskedOptions->IS_KIND_OF(MaskedFilterExportOptions), "pMaskedOptions isn't");

	if (pMaskedOptions->WantTransparent())
		return 1;
	else
#else
		return 0;
#endif
}

/********************************************************************************************

>	INT32 MaskedFilter::FindUnusedColour(LPBITMAPINFO pBMInfo, LPBYTE pBMBits)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/96
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Selects an index to use as the transparent colour from the bitmap data in the
				output DIB.  If an index is unused then this index is used.
				If there is not an unused index then it is a bit more tricky - the least used 
				colour is made transparent except for 1bpp in which case white becomes
				transparent.

				Moved from TI_GIFFilter to MaskedFilter 10/7/96

********************************************************************************************/
INT32 MaskedFilter::FindUnusedColour(LPBITMAPINFO pBMInfo, LPBYTE pBMBits)
{
	ERROR2IF(pBMBits==NULL || pBMInfo==NULL, -1, "NULL bitmap pointer");

	// Count the colours used in the bitmap
	UINT32* pResults = NULL;
	BOOL ok = DIBUtil::CountColoursUsed(pBMInfo, pBMBits, &pResults);
	ERROR2IF(pResults == NULL, -1, "NULL results array");

	INT32 TransIndex = -1;
	if (ok)
		TransIndex = DIBUtil::FindLeastUsedColour(pBMInfo, pResults);

	CCFree(pResults);
	pResults = NULL;

	return TransIndex;
}


/********************************************************************************************

>	virtual BOOL MaskedFilter::SetExportHint(Document* pDoc)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/07/97
	Purpose:	Overrides base class to set correct hint
	See Also:	BaseBitmapFilter::SetExportHint()

********************************************************************************************/
BOOL MaskedFilter::SetExportHint(Document* pDoc)
{
	PORTNOTETRACE("filters","MaskedFilter::SetExportHint - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (pDoc != NULL && !IsPreviewBitmap)
	{
		ExportHint* pHint = pDoc->GetExportHint();	
		ERROR3IF(pHint == NULL, "NULL ExportHint");
		
		BitmapExportOptions* pBaseOptions = GetBitmapExportOptions();
		ERROR3IF(pBaseOptions == NULL, "NULL Options");

		if (pHint != NULL && pBaseOptions != NULL)
		{
			ERROR3IF(!(pBaseOptions->IsKindOf(CC_RUNTIME_CLASS(MaskedFilterExportOptions))), "Wrong options type");
			MaskedFilterExportOptions* pOptions = (MaskedFilterExportOptions*) pBaseOptions;

			// If derived class doesn't wat to hint then exit
			if (GetHintType() == HINTTYPE_BAD)
				return(TRUE);

			pHint->SetType(GetHintType());
			pHint->SetWidth(GetPixelWidth());
			pHint->SetHeight(GetPixelHeight());
			pHint->SetBPP(pOptions->GetDepth());

			String_256 Opts;
			Opts._MakeMsg(_T("D#1%ld"), pOptions->GetDither());

			if (pOptions->GetDepth() <= 8)
			{
				String_256 TempStr;
				TempStr._MakeMsg(_T(" P#1%ld N#2%ld"),
								pOptions->GetPalette(),
								pOptions->GetNumColsInPalette());
				Opts += TempStr;

				if (pOptions->GetUseSystemColours())
					Opts += _T(" S");
			}
			
			if (pOptions->WantTransparent())
				Opts += _T(" T");
			if (pOptions->WantInterlaced())
				Opts += _T(" I");

			pHint->SetOptionsString(Opts);
		}
	}
#endif
	return(TRUE);
}
