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

// Kernel level parts of bitmap handler

/*
*/

#include "camtypes.h"
//#include "mario.h"
//#include "bitmap.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bitmpinf.h"
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "will.h"
//#include "phil.h"
//#include "simon.h"
//#include "resource.h"
//#include "nativeps.h"	// The old style EPS native filter, used in v1.1
#include "clipint.h"
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "grndrgn.h"
//#include "devcolor.h"
#include "gradtbl.h"
#include "bmpcomp.h"	// BitmapListComponent

#include "bitfilt.h"	// BaseBitmapFilter
#include "camelot.h"	// BaseCamelotFilter
//#include "ccfile.h"		// CCFile - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bmpsrc.h"		// BitmapSource

//#ifndef WEBSTER		// Code now in DIBUtil 10/11/97 Neville 
//#include "dibutil.h"	// put it back for DIBUtil::IsGreyscaleBitmap - in camtypes.h [AUTOMATICALLY REMOVED]
//#endif //WEBSTER
#include "bitmapfx.h"
//#include "xpehost.h"

#include "oilbitmap.h"	// for debug macros

CC_IMPLEMENT_DYNAMIC( OILBitmap, ListItem )
CC_IMPLEMENT_DYNAMIC( KernelBitmap, ListItem )
CC_IMPLEMENT_DYNAMIC( KernelBitmapRef, CCObject )
CC_IMPLEMENT_DYNAMIC( OriginalBitmapRef, CCObject )
CC_IMPLEMENT_DYNAMIC( BitmapPtr, ListItem )
CC_IMPLEMENT_DYNAMIC( KernelBmpListItem, ListItem )


// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW


// Static default bitmap pointer
OILBitmap *OILBitmap::Default;

List* KernelBitmap::s_pTrackerList = NULL;

/********************************************************************************************

>	OriginalBitmapRef::OriginalBitmapRef(BitmapSource* pSource, 
										 BaseBitmapFilter* pImportFilter = NULL)
					
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96

	Inputs:		pSource, a pointer to a BitmapSource
				pImportFilter, pointer to the filter to use to decode the bitmap

	Purpose:	Constructor for a reference to the original (usually compressed) bitmap
				that was used to create the bitmap containing this reference.
	SeeAlso:	OriginalBitmapRef::GetOriginalFile

********************************************************************************************/

OriginalBitmapRef::OriginalBitmapRef(BitmapSource* pSource, BaseBitmapFilter* pImportFilter)
{
	m_pBitmapSource	= pSource;
	m_pBitmapFilter	= pImportFilter;
}

/********************************************************************************************

>	OriginalBitmapRef::~OriginalBitmapRef()
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96

	Purpose:	Destructor for a reference to the original bitmap.
				THIS WILL FREE UP THE ORIGINAL FILE BUFFER.

	SeeAlso:	OriginalBitmapRef::GetOriginalFile

********************************************************************************************/

OriginalBitmapRef::~OriginalBitmapRef()
{
	if (m_pBitmapSource)
	{
		// Free up the original bitmap buffer
		delete m_pBitmapSource;
	}
}

/********************************************************************************************

>	void OriginalBitmapRef::SetOriginalSource(BitmapSource* pSource = NULL, 
											BaseBitmapFilter* pImportFilter = NULL)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96

	Inputs:		pFileBuffer, a pointer to a buffer containing the original file.
				pImportFilter, pointer to the filter to use to decode the bitmap

	Purpose:	Sets the original bitmap file associated with this reference.

	Notes:		The input file buffer should be CCMalloc'd, and will be CCFree'd when the
				reference is destroyed, or changed to another buffer.

********************************************************************************************/

void OriginalBitmapRef::SetOriginalSource(BitmapSource* pSource, BaseBitmapFilter* pImportFilter)
{
	if (m_pBitmapSource)
	{
		// Free up the original bitmap buffer
		delete m_pBitmapSource;
	}

	m_pBitmapSource	= pSource;
	m_pBitmapFilter	= pImportFilter;
}

/********************************************************************************************

>	BOOL OriginalBitmapRef::GetOriginalSource(BitmapSource** pSource = NULL,
											BaseBitmapFilter** ppImportFilter = NULL)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96

	Inputs:		pSource, ptr to a BitmapSource ptr to update to point at the original 
				BitmapSource
				ppFilterName, ptr to a BaseBitmapFilter ptr to update to point at the filter 
				to use to decode the bitmap
				
				If both these inputs are NULL, then the function will simply return
				whether an original file exists or not.

	Outputs:	The input pointers are set if an original file is available, otherwise they
				are left unchanged.

	Returns:	TRUE, if an original file is available, and the pointers were set if non NULL.
				FALSE, if no orginal files is available.

	Purpose:	Gets the original bitmap file buffer associated with this reference.

********************************************************************************************/

BOOL OriginalBitmapRef::GetOriginalSource(BitmapSource** pSource, BaseBitmapFilter** ppImportFilter)
{
	if (pSource == NULL && ppImportFilter == NULL)
	{
		// Just return whether a file is available or not, without setting the pointers
		return m_pBitmapSource != NULL;
	}

	if (m_pBitmapSource != NULL)
	{
		// Set the pointers
		*pSource		= m_pBitmapSource;
		*ppImportFilter	= m_pBitmapFilter;

		return TRUE;
	}

	// No original file available
	return FALSE;
}



/********************************************************************************************

>	KernelBitmap::KernelBitmap(OILBitmap *pBitmap = NULL)
					
	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/8/94
	Inputs:		Pointer to an OIL bitmap. If NULL then uses default built-in bitmap.
	Purpose:	Constructor for kernel bitmaps. Attaches to the OILBitmap and increases its
				usage count.
	Errors:		-
	SeeAlso:	KernelBitmap::IsOK;OILBitmap::Attach

********************************************************************************************/

KernelBitmap::KernelBitmap(OILBitmap *pBitmap, BOOL IsTemp)
{
	m_pParentList = NULL;
	m_bDontDeleteActualBitmap = FALSE;
	m_bFractalAttached = FALSE;
	m_bUsedByBrush = FALSE;
	if (pBitmap == NULL)
		ActualBitmap = OILBitmap::Attach();
	else
		ActualBitmap = OILBitmap::Attach(pBitmap, IsTemp);

	if (ActualBitmap && ActualBitmap->IsAFractal())
		m_bFractalAttached = TRUE;
}

/********************************************************************************************

>	KernelBitmap* KernelBitmap::MakeKernelBitmap(	OILBitmap *pBitmap = NULL, 
													Document *pDoc = NULL,
													BOOL bAlwaysNew = FALSE		)
					
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/95
	Inputs:		Pointer to an OIL bitmap. If NULL then uses default built-in bitmap.
				Pointer to the Document the bitmap is use in (NULL = Current).
	Purpose:	Makes a KernelBitmap from an OIL bitmap.
				Will use an existing one if the OILBitmap is already in use in the Doc.
	Errors:		-
	SeeAlso:	KernelBitmap::IsOK;OILBitmap::Attach

********************************************************************************************/

KernelBitmap* KernelBitmap::MakeKernelBitmap(OILBitmap *pBitmap, Document *pDoc, BOOL bAlwaysNew)
{
	KernelBitmap* NewBitmap;

	if (pBitmap == NULL)
		pBitmap = OILBitmap::Default;

	if (pDoc == NULL)
		pDoc = Document::GetCurrent();

	// Get the bitmap list from the document
	BitmapList* BmpList = NULL;
	if (pDoc) BmpList = pDoc->GetBitmapList();

	if (BmpList == NULL)
	{
		// This shouldn't really happen ...
		NewBitmap = new KernelBitmap(pBitmap);
		return NewBitmap;
	}

	KernelBitmap* Bmp = (KernelBitmap*)BmpList->GetHead();
	BOOL Exists = FALSE;

	// Scan through the Documents bitmap list to see if any of
	// the existing KernelBitmaps reference this OILBitmap.
	while (!bAlwaysNew && Bmp != NULL)
	{
		if (Bmp->ActualBitmap == pBitmap)
		{
			Exists = TRUE;
			break;
		}

		Bmp = (KernelBitmap*)BmpList->GetNext(Bmp);
	}

	if (Exists)
	{
		// Use the existing KernelBitmap.
		NewBitmap = Bmp;
	}
	else
	{
		// Make a new KernelBitmap.
		NewBitmap = new KernelBitmap(pBitmap);
	}	

	return NewBitmap;
}


/********************************************************************************************

>	KernelBitmap::KernelBitmap( const KernelBitmap& other)
					
	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/8/94
	Inputs:		Another kernel bitmap.
	Purpose:	Copy constructor for kernel bitmaps. MAintains usage count on OILBitmap.
	SeeAlso:	KernelBitmap::IsOK;OILBitmap::Attach

********************************************************************************************/

KernelBitmap::KernelBitmap( const KernelBitmap& other)
{
	m_pParentList = NULL;
	m_bDontDeleteActualBitmap = FALSE;
	m_bFractalAttached = FALSE;
	m_bUsedByBrush = FALSE;
	ActualBitmap = OILBitmap::Attach( other.ActualBitmap, other.ActualBitmap->IsTemp() );

	if (ActualBitmap && ActualBitmap->IsAFractal())
		m_bFractalAttached = TRUE;
}

/********************************************************************************************

>	KernelBitmap::KernelBitmap( UINT32 Width, UINT32 Height, UINT32 Depth, UINT32 dpi )
					
	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/8/94
	Inputs:		Bitmap pxiel width, height & depth and dpi setting.
	Purpose:	Constructor for kernel bitmaps which attaches it to a blank OILbitmap.
				Actually, the OILBitmap isn't really blank, it is entirely uninitialised
				i.e its contents will be random (including its palette if it has one).
	Errors:		Possible - SetError will be called, use IsOK for result.
	SeeAlso:	KernelBitmap::IsOK;OILBitmap::Create

********************************************************************************************/

KernelBitmap::KernelBitmap( UINT32 Width, UINT32 Height, UINT32 Depth, UINT32 dpi, BOOL bTemp )
{
	m_pParentList = NULL;
	m_bDontDeleteActualBitmap = FALSE;
	m_bFractalAttached = FALSE;
	m_bUsedByBrush = FALSE;
	OILBitmap *bm = OILBitmap::Create( Width, Height, Depth, dpi );
	if (bm)
		ActualBitmap = OILBitmap::Attach( bm, bTemp );
	else
		ActualBitmap = NULL;
}

/********************************************************************************************

>	BOOL KernelBitmap::IsOK() const
					
	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/8/94
	Returns:	TRUE if KernelBitmap has a valid attached OILBitmap, FALSE if it doesn't
				(e.g. its constructor failed).
	Purpose:	Tell if KernelBitmap is OK.

********************************************************************************************/

BOOL KernelBitmap::IsOK() const
{
	return ActualBitmap ? TRUE : FALSE;
}

/********************************************************************************************

>	KernelBitmap::~KernelBitmap()
					
	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/8/94
	Purpose:	Destructor. Detaches any current OILBitmap.
	SeeAlso:	OILBitmap::Detach

********************************************************************************************/

KernelBitmap::~KernelBitmap()
{
	if (m_pParentList != NULL)
	{
		// If we're still in the bitmap list, then remove us
		m_pParentList->RemoveItem(this);
		m_pParentList = NULL;
	}

	if (ActualBitmap)
	{
		ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap, 
						"Bitmap Error.  Found a reference to a deleted bitmap.");

		// JustinF says: made the above check in retail builds as well, or we'll crash!
		if (//(INT32) ActualBitmap->m_pColourVersion != 0xFEEEFEEE &&
			//(INT32) ActualBitmap->m_pColourVersion != 0xCDCDCDCD &&
			//(INT32) ActualBitmap->m_pColourVersion != 0xDDDDDDDD &&
			!m_bDontDeleteActualBitmap &&
			!m_bFractalAttached &&
			ActualBitmap->IsTemp())
		{
			delete ActualBitmap;
		}
		
		// AndyH says: what a load of rubbish.
		// ActualBitmap->m_pColourVersion never evaluates to the constants
		// given above in the release build. In fact bug 6712 occurs because
		// we try to delete a KernelBitmap whose OILBitmap has already been
		// deleted; in turn, this function deletes ActualBitmap (an OILBitmap)
		// and causes an access violation.

		// By removing the above 'bodge', such access violations will occur in
		// debug mode too, as they should.

		ActualBitmap = NULL;
	}
}

/********************************************************************************************

>	KernelBitmap& KernelBitmap::operator=(const KernelBitmap& other)
					
	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/8/94
	Inputs:		Another kernel bitmap.
	Purpose:	Assigns one KernelBitmap to another while maintaining usage count on OILBitmap.

********************************************************************************************/

KernelBitmap& KernelBitmap::operator=(const KernelBitmap& other)
{
	if (other.ActualBitmap == NULL)
		ActualBitmap = OILBitmap::Attach( NULL );
	else
		ActualBitmap = OILBitmap::Attach( other.ActualBitmap, other.ActualBitmap->IsTemp() );

	if (ActualBitmap && ActualBitmap->IsAFractal())
		m_bFractalAttached = TRUE;
	
	return *this;
}

/********************************************************************************************

>	BOOL KernelBitmap::SetAsFractal()
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/96
	Returns:	FALSE if invalid pointer found
	Purpose:	Set this bitmap as a fractal bitmap

********************************************************************************************/

BOOL KernelBitmap::SetAsFractal()
{
	if (ActualBitmap == NULL)
		return FALSE;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	if (ActualBitmap)
	{
		m_bFractalAttached = TRUE;
		ActualBitmap->SetAsFractal();
	
		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL KernelBitmap::SetAsGreyscale()
			
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/97

	Returns:	FALSE if invalid pointer found

	Purpose:	Set this bitmap as a greyscale bitmap
				This is used to inform the bitmap that it is a greyscale 8bpp bitmap.
				This is because when we create a grteyscale OILBitmap and attach a KernelBitmap
				to it, there is no way to indicate to the system that it's a true greyscale
				bitmap, which is necessary to stop it farting around and making a new copy
				of the bitmap when someone calls GetGreyscaleVersion(). This is used by
				the airbrush stroke provider when it generates a greyscale bitmap for use
				as a transparency mask.

	Notes:		ERROR3 will be given if the bitmap is not 8bpp. This may not be strictly
				necessary (who can tell with this bitmap system - it's so confusing), but
				for transparency masks, they must (AFAIK) be 8bpp, so it's a good safety check.

********************************************************************************************/

BOOL KernelBitmap::SetAsGreyscale()
{
	if (ActualBitmap == NULL)
		return FALSE;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	ERROR3IF(GetBPP() != 8, "SetAsGreyscale - this isn't an 8bpp bitmap - how can it be a greyscale?");

	if (ActualBitmap)
	{
		ActualBitmap->SetAsGreyscale();
		return TRUE;
	}

	return FALSE;
}


/********************************************************************************************

>	BOOL KernelBitmap::SetAsLossy(BOOL bLossy = TRUE)
			
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/97

	Returns:	FALSE if invalid pointer found

	Purpose:	Set this bitmap' lossy flag

********************************************************************************************/

BOOL KernelBitmap::SetAsLossy(BOOL bLossy)
{
	if (ActualBitmap == NULL)
		return FALSE;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	if (ActualBitmap)
	{
		ActualBitmap->SetAsLossy(bLossy);
		return TRUE;
	}

	return FALSE;
}


/********************************************************************************************

>	BOOL KernelBitmap::IsDefaultBitmap()
					
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/96
	Returns:	TRUE if KernelBitmap is the default bitmap, FALSE if not.
	Purpose:	To find out if the bitmap is the default bitmap or not.
				This is used by the native/web saving code to see if the bitmap
				is the default one and if so save a reference to it.
	SeeAlso:	

********************************************************************************************/

BOOL KernelBitmap::IsDefaultBitmap()
{
	if (ActualBitmap == NULL)
		return FALSE;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	// Treat as the default bitmap, if we've been deleted
	if (HasBeenDeleted())
		return TRUE;

	// Check to see if its the default bitmap
	if (ActualBitmap == OILBitmap::Default)
		return TRUE;

	return FALSE;
}

/********************************************************************************************

>	BOOL KernelBitmap::HasBeenDeleted ()
					
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/96
	Returns:	TRUE	- The bitmap has already been deleted.
				FALSE	- The bitmap hasn't been deleted.
	Purpose:	Tests whether or not the OIL bitmap attached to the kernel bitmap class has
				been deleted.

********************************************************************************************/

BOOL KernelBitmap::HasBeenDeleted ()
{
	// This test is far better done BEFORE the ERROR3!
	// Graeme (21/6/00) - Added a test to see whether the pointer points at 0xdddddddd, as
	// this is the value that the memory is (re)set to by delete.
	// Luke (31/10/05) - Removed these tests, since I'd rather this excepts rather than
	// perform a dodgy read on some memory that may be returned to OS (or used for something 
	// else)
	if( ActualBitmap == NULL )
	{
		return TRUE;
	}
	else
	{
		ERROR3IF_OILBMP_PTR_INVALID ( ActualBitmap,
				"Bitmap Error. Found a reference to a deleted bitmap." );

		return ActualBitmap->HasBeenDeleted ();
	}
}

/********************************************************************************************

>	virtual BOOL KernelBitmap::WritePalette(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/96
	Inputs:		pFilter	- The filter to use to write the data with
	Outputs:	-
	Returns:	True if saved ok, False otherwise
	Purpose:	Exports the palette out to the file using the BaseCamelotFilter supplied. This
				means that it is being written to a web or native file.
				At present, used by the code when outputting an 8bpp JPEG to squirt the palette
				into the record so that we can convert it back to 8bpp using the palette on
				loading.

********************************************************************************************/

BOOL KernelBitmap::WritePalette(BaseCamelotFilter *pFilter)
{
	ERROR2IF(pFilter == NULL,FALSE,"KernelBitmap::WritePalette null pFilter");

	if (ActualBitmap == NULL)
		return FALSE;

	return ActualBitmap->WritePalette(pFilter);
}

/********************************************************************************************

>	virtual BOOL KernelBitmap::Convert24To8(RGBTRIPLE *pPalette, UINT32 NumberOfPaletteEntries)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/96
	Inputs:		pPalette	- pointer to the palette in RGBTIPLE format to use when converting
				NumberOfPaletteEntries - entries in this palette 
	Outputs:	-
	Returns:	True if worked ok, False otherwise
	Purpose:	Converts the OILbitmap from its present 24bpp form into an 8bpp form which
				should closely match the one that was exported.

********************************************************************************************/

BOOL KernelBitmap::Convert24To8(RGBTRIPLE *pPalette, UINT32 NumberOfPaletteEntries)
{
	ERROR2IF(pPalette == NULL,FALSE,"KernelBitmap::Convert24To8 null pFilter");
	ERROR2IF(NumberOfPaletteEntries == 0,FALSE,"KernelBitmap::Convert24To8 NumberOfPaletteEntries = 0");

	if (ActualBitmap == NULL)
		return FALSE;

	// If the bitmap is already 8bpp then do nothing
	if (GetBPP() == 8)
		return TRUE;

	return ActualBitmap->Convert24To8(pPalette, NumberOfPaletteEntries);
}

/********************************************************************************************

>	static BOOL KernelBitmap::ImportBitmap(CCLexFile* pFile, BaseBitmapFilter* pBitmapFilter,
										   BaseCamelotFilter* pFilter, BOOL IsCompressed,
										   RGBTRIPLE *pPalette, UINT32 NumberOfPaletteEntries,
										   KernelBitmap** ppImportedBitmap, BOOL* IsNew)
				
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/96
	Inputs:		pFile			- The file to read the data from
				pBitmapFilter	- The bitmap filter to use for importing the bitmap.
				pFilter			- The BaseCamelotFilter which provides functions for things like progress
								bar updates.
				IsCompressed	- Flag to say the bitmap is compressed or not.
				pPalette		- If non-null then palette to convert the 24bpp bitmap into an 8bpp bitmap
				NumberOfPaletteEntries	- Is the number of entries in the above palette  
	Outputs:	*ppImportedBitmap	- updated to point at new or existing bitmap.
				*IsNew = TRUE		- if a new bitmap was created, or
						 FALSE		- if an existing one has been returned.
	Purpose:	Imports a bitmap, and checks to see if the bitmap already exist, returning
				a pointer to the existing bitmap if possible, or creates a new one.
				
				This is used web/native filters to pull out a bitmap definition from inside
				a bitmap definition record.  IsCompressed is only used for BMP/BMPZIP type
				bitmaps, at present.

				Assumes:
					pFile has already been opened up for reading
					pFilter has been set up for reading the data e.g. progress bar 
	Errors:		-
	SeeAlso:	OILBitmap::ImportBitmap

********************************************************************************************/

BOOL KernelBitmap::ImportBitmap(CCLexFile* pFile, BaseBitmapFilter* pBitmapFilter,
								BaseCamelotFilter* pFilter, BOOL IsCompressed,
								RGBTRIPLE *pPalette, UINT32 NumberOfPaletteEntries,
								KernelBitmap** ppImportedBitmap, BOOL* IsNew)
{
	ERROR2IF(pFile == NULL,FALSE,"KernelBitmap::ImportBitmap null pFile");
	ERROR2IF(pBitmapFilter == NULL,FALSE,"KernelBitmap::ImportBitmap null pBitmapFilter");
	ERROR2IF(pFilter == NULL,FALSE,"KernelBitmap::ImportBitmap null pFilter");
	ERROR2IF(ppImportedBitmap == NULL,FALSE,"KernelBitmap::ImportBitmap null ppImportedBitmap");

	*ppImportedBitmap = NULL;
	
	if (IsNew)
		*IsNew = TRUE;

	// Get a new bitmap object to import into
	OILBitmap *pOILBitmap = OILBitmap::Create();
	if (pOILBitmap == NULL)
	{
		return FALSE;
	}

	// Import the bitmap data
	if (!pOILBitmap->ImportBitmap(pFile, pBitmapFilter, pFilter, IsCompressed))
	{
		delete pOILBitmap;
		return FALSE;
	}

	// Check if a palette has been supplied, if so then it is a JPEG which started out
	// as 8bpp colour but has been converted to 24bpp so that it can be JPEGed.
	// If this is the case then the palette is the one from the original bitmap. So use
	// it to go and convert the data back into the 8bpp data.
	if (pPalette && NumberOfPaletteEntries > 0 && NumberOfPaletteEntries <= 256)
	{	
		// If the loaded bitmap is not 24bpp then do nothing
		if (pOILBitmap->GetBPP() == 24)
			pOILBitmap->Convert24To8(pPalette, NumberOfPaletteEntries);
		// If a problem has happened then we will forget the conversion and use the
		// 24 bpp version
	}

	// Now check to see if this bitmap already exists ....
	Document* ImportDoc = ((Filter*)pFilter)->GetDocument();
	if (ImportDoc == NULL)
		ImportDoc = Document::GetSelected();

	if (ImportDoc == NULL)
	{
		delete pOILBitmap;
		return FALSE;
	}

	// The bitmap system relies on the Current Doc being correct,
	// so we'll set it to be on the safe side
	ImportDoc->SetCurrent();

	// Is there an existing KernelBitmap already in the bitmap list ?
	*ppImportedBitmap = TryAndUseExistingBitmap(pOILBitmap, ImportDoc, IsNew);

	return (*ppImportedBitmap != NULL);
}

/********************************************************************************************

>	BOOL KernelBitmap::ImportBitmap(Filter *pFilter, const BitmapInfo *pInfo, INT32 BitmapType,
					  			KernelBitmap** ppImportedBitmap, BOOL* IsNew)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		pFilter, The filter to use for importing the bitmap.
				pInfo, Optional bitmap info to use to create the bitmap
				IsNew, if NULL then no check will be made to see if the imported bitmap
				already exists in the filter's associated document. A check for the OILBitmap
				will always be made.
	Outputs:	*ppImportedBitmap, updated to point at new or existing bitmap.
				*IsNew = TRUE, if a new bitmap was created, or
						 FALSE, if an existing one has been returned.
	Purpose:	Imports a bitmap, and checks to see if the bitmap already exist,
				returning a pointer to the existing bitmap if possible, or
				creates a new one.
	Errors:		-
	SeeAlso:	OILBitmap::ImportBitmap

********************************************************************************************/

BOOL KernelBitmap::ImportBitmap(Filter *pFilter, const BitmapInfo *pInfo, INT32 BitmapType,
					  			KernelBitmap** ppImportedBitmap, BOOL* IsNew)
{
	ERROR2IF(pFilter == NULL,FALSE,"KernelBitmap::ImportBitmap null pFilter");
	ERROR2IF(ppImportedBitmap == NULL,FALSE,"KernelBitmap::ImportBitmap null ppImportedBitmap");

	*ppImportedBitmap = NULL;
	if (IsNew)
		*IsNew = TRUE;

	// Get a new bitmap object for this node.
	OILBitmap *pOILBitmap = OILBitmap::Create();
	if (pOILBitmap == NULL)
	{
		return FALSE;
	}

	if (!pOILBitmap->ImportBitmap(pFilter, pInfo, BitmapType))
	{
		delete pOILBitmap;
		return FALSE;
	}

	// Now check to see if this bitmap already exists if necessary....
	Document* ImportDoc = ((Filter*)pFilter)->GetDocument();
	if (ImportDoc == NULL)
		ImportDoc = Document::GetSelected();

	if (ImportDoc == NULL)
	{
//		delete pOILBitmap;
//		return FALSE;
//	}
		// no document, so don't try to re-use any existing bitmaps
		*ppImportedBitmap = TryAndUseExistingBitmap(pOILBitmap, NULL, NULL);
	}
	else
	{
		// The bitmap system relies on the Current Doc being correct,
		// so we'll set it to be on the safe side
		ImportDoc->SetCurrent();

		// Is there an existing KernelBitmap already in the bitmap list ?
		*ppImportedBitmap = TryAndUseExistingBitmap(pOILBitmap, ImportDoc, IsNew);
	}

	return (*ppImportedBitmap != NULL);
}

/********************************************************************************************

>	KernelBitmap* KernelBitmap::TryAndUseExistingBitmap(OILBitmap* pOILBitmap, Document* pDoc, 
														BOOL* IsNew)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/96
	Inputs:		pOILBitmap, 
				pDoc,
				IsNew, if NULL then no check will be made to see if the imported bitmap
				already exists in the filter's associated document. A check for the OILBitmap
				will always be made.
	Outputs:	*IsNew = TRUE, if a new bitmap was created, or
						 FALSE, if an existing one has been returned.
	Purpose:	Imports a bitmap, and checks to see if the bitmap already exist,
				returning a pointer to the existing bitmap if possible, or
				creates a new one.
	Errors:		-

********************************************************************************************/

KernelBitmap* KernelBitmap::TryAndUseExistingBitmap(OILBitmap* pOILBitmap, Document* pDoc, 
													BOOL* IsNew)
{
	// Is IsNew is NULL, then we won't try and use an existing bitmap
	if (IsNew != NULL)
	{	
		// Find this documents BitmapList
		BitmapList* pBmpList = pDoc->GetBitmapList();

		// Is there an existing KernelBitmap already in the bitmap list ?
		KernelBitmap* pExistingKBmp = pBmpList->FindDuplicateBitmap(pOILBitmap);

		if (pExistingKBmp)
		{
			TRACEUSER( "Neville", _T("This bitmap already exists.  Re-using existing KernelBitmap.\n") );
			
			// We'll use the one that's there already
			delete pOILBitmap;

			*IsNew = FALSE;		// We're using an existing Bitmap
			return pExistingKBmp;
		}

		// There is no existing version in this document,
		// but there might be one in another document somewhere
		// so we'll have a look in the global list.

		OILBitmap* pExistingBmp = 
			GetApplication()->GetGlobalBitmapList()->FindDuplicateBitmap(pOILBitmap);

		if (pExistingBmp)
		{
			TRACEUSER("Neville", _T("This bitmap already exists.  Re-using existing OILBitmap.\n") );

			// There's already an existing OIL bitmap we can use,
			// so we'll make a new kernel bitmap out of it ...
			delete pOILBitmap;

			KernelBitmap* KernelBmp = new KernelBitmap(pExistingBmp);
			if (KernelBmp == NULL)
			{
				return NULL;
			}

			*IsNew = FALSE;		// We're using an existing Bitmap

			return KernelBmp;
		}
	}

	TRACEUSER( "Neville", _T("This is a new bitmap\n") );

	BOOL Temp = FALSE;
	if (IsNew == NULL)
		Temp = TRUE; // mark it as temp

	// We'll make a completely new bitmap then
	KernelBitmap* NewKernelBmp = new KernelBitmap(pOILBitmap, Temp);
	if (NewKernelBmp == NULL)
	{
		delete pOILBitmap;
		return NULL;
	}

	if (IsNew)
		*IsNew = TRUE;			// This is a new Bitmap

	return NewKernelBmp;
}


/********************************************************************************************

>	BOOL KernelBitmap::SetTransparencyIndex(INT32 Index)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/6/96
	Returns:	TRUE if the Index was set, FALSE if there is no transparent index
	Purpose:	Finds the Transparent index colour of a bitmap if one exists

********************************************************************************************/

BOOL KernelBitmap::SetTransparencyIndex(INT32 Index)
{
	if (ActualBitmap == NULL)
	 	return FALSE;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	if (ActualBitmap->GetBPP() > 8)
		return FALSE;

	return ActualBitmap->SetTransparencyIndex(Index);
}

/********************************************************************************************

>	BOOL KernelBitmap::GetTransparencyIndex(INT32* pIndex)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/6/96
	Returns:	TRUE if the Index was set, FALSE if there is no transparent index
	Purpose:	Finds the Transparent index colour of a bitmap if one exists

********************************************************************************************/

BOOL KernelBitmap::GetTransparencyIndex(INT32* pIndex)
{
	if (ActualBitmap == NULL)
	 	return FALSE;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	if (ActualBitmap->GetBPP() > 8)
		return FALSE;

	return ActualBitmap->GetTransparencyIndex(pIndex);
}

/********************************************************************************************

>	BOOL KernelBitmap::IsTransparent()
				
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/03/2004
	Returns:	TRUE if the bitmap has any non-opaque pixels
	Purpose:	Determines if a bitmap is not totally opaque

********************************************************************************************/

BOOL KernelBitmap::IsTransparent()
{
	if (ActualBitmap == NULL)
	 	return FALSE;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	return ActualBitmap->IsTransparent();
}


/********************************************************************************************

>	INT32 KernelBitmap::GetRecommendedWidth()
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Returns:	The current recommended width of the bitmap in Millipoints
	Purpose:	Finds the recommended width of the bitmap

********************************************************************************************/

INT32 KernelBitmap::GetRecommendedWidth()
{
	INT32 Width = 0;

	if (ActualBitmap)
	{
		Width = ActualBitmap->GetRecommendedWidth();
	}

	return Width;
}

/********************************************************************************************

>	INT32 KernelBitmap::GetRecommendedHeight()
			
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Returns:	The current recommended height of the bitmap in Millipoints
	Purpose:	Finds the recommended height of the bitmap

********************************************************************************************/

INT32 KernelBitmap::GetRecommendedHeight()
{
	INT32 Height = 0;

	if (ActualBitmap)
	{
		Height = ActualBitmap->GetRecommendedHeight();
	}

	return Height;
}

/********************************************************************************************

>	void KernelBitmap::SetRecommendedWidth(INT32 NewWidth)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		NewWidth, the new width of the bitmap in Millipoints
	Purpose:	Sets the recommended width of the bitmap

********************************************************************************************/

void KernelBitmap::SetRecommendedWidth(INT32 NewWidth)
{
	if (ActualBitmap)
		ActualBitmap->SetRecommendedWidth(NewWidth);
}

/********************************************************************************************

>	void KernelBitmap::SetRecommendedHeight(INT32 NewHeight)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		NewHeight, the new height of the bitmap in Millipoints
	Purpose:	Sets the recommended height of the bitmap

********************************************************************************************/

void KernelBitmap::SetRecommendedHeight(INT32 NewHeight)
{
	if (ActualBitmap)
		ActualBitmap->SetRecommendedWidth(NewHeight);
}


/********************************************************************************************

>	ADDR KernelBitmap::GetBitmapBits() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/11/96
	Returns:	The address of the bits of this KernelBitmap
				NULL if an error occurred
	Purpose:	Helper function to get the actual bits (as in Bitmap)
	Errors:		Checks runtime class

********************************************************************************************/
ADDR KernelBitmap::GetBitmapBits() const
{
	OILBitmap* pOILy = GetActualBitmap();
	if (pOILy == NULL)
	{
		TRACE( _T("KernelBitmap::GetBitmapBits() - no OILBitmap") );
		return NULL;
	}

	ERROR3IF(!pOILy->IS_KIND_OF(OILBitmap), "pOILy isn't");
	
	return pOILy->GetBitmapBits();
}


/********************************************************************************************

>	void KernelBitmap::Attach(BitmapList* BmpList = NULL)
		
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		The bitmap list to attach this bitmap to.
				if NULL then the bitmap is Re-Attached to its old list.
	Outputs:	-
	Purpose:	Adds this kernel bitmap to the specified document bitmap list.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void KernelBitmap::Attach(BitmapList* BmpList)
{
	if (BmpList == NULL)
		BmpList = m_pParentList;

	ERROR3IF(BmpList == NULL, "Warning ... NULL BmpList in KernelBitmap::Attach");
	if (BmpList == NULL)	
		return;

	if (m_pParentList == NULL)
	{
		TRACEUSER( "Neville", _T("Attaching KBmp @ %x, to List at %x\n"), this, BmpList );
		m_pParentList = BmpList;
		BmpList->AddItem(this);

		if (KernelBitmap::s_pTrackerList)
		{
			KernelBmpListItem* pItem = new KernelBmpListItem(this);
			if (pItem)
				KernelBitmap::s_pTrackerList->AddTail(pItem);
		}
	}

	ActualBitmap->RebuildXPEBitmap();
}

/********************************************************************************************

>	void KernelBitmap::Detach()
		
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Purpose:	Removes this kernel bitmap from it's current document bitmap list.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void KernelBitmap::Detach()
{
	// For each document
	// If bitmap is used in document
	// Go though all the documents
	Document* pDoc = (Document*)GetApplication()->Documents.GetHead();
	while (pDoc != NULL)
	{
		if (ActualBitmap->IsUsedInDocument(pDoc))
		return;

		// Move onto the next document
		pDoc = (Document*)GetApplication()->Documents.GetNext(pDoc);
	}

	// If we got here then the bitmap is not used in any document
	// so we can safely vape its data if we can later rebuild it
	ActualBitmap->RemoveXPEBitmap();
}

/********************************************************************************************

>	UINT32 KernelBitmap::GetBPP()
					
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Returns:	The number of Bits per pixel of this bitmap
	Purpose:	Finds the colour resolution of this bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 KernelBitmap::GetBPP()
{
	if (ActualBitmap == NULL)
		return 0;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,"Bitmap Error.  Found a reference to a deleted bitmap.");

	return ActualBitmap->GetBPP();
}

/********************************************************************************************

>	UINT32 KernelBitmap::GetWidth() const
					
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/97
	Returns:	The width of this bitmap
	Purpose:	Finds the width of this bitmap

********************************************************************************************/

UINT32 KernelBitmap::GetWidth() const
{
	if (ActualBitmap == NULL)
		return 0;

	return ActualBitmap->GetWidth();
}

/********************************************************************************************

>	UINT32 KernelBitmap::GetHeight() const
					
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/97
	Returns:	The height of this bitmap
	Purpose:	Finds the height of this bitmap

********************************************************************************************/

UINT32 KernelBitmap::GetHeight() const
{
	if (ActualBitmap == NULL)
		return 0;

	return ActualBitmap->GetHeight();
}
	
/********************************************************************************************

>	UINT32 KernelBitmap::GetNumPaletteEntries() const
					
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/97
	Returns:	The number of palette entries of this bitmap
	Purpose:	Finds the number of colours colour in this bitmap

********************************************************************************************/

UINT32 KernelBitmap::GetNumPaletteEntries() const
{
	if (ActualBitmap == NULL)
		return 0;

	return ActualBitmap->GetNumPaletteEntries();
}

/********************************************************************************************

>	void KernelBitmap::SetOriginalSource(BitmapSource* pSource = NULL
									   BaseBitmapFilter* pImportFilter = NULL)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96

	Inputs:		pSource, a pointer to BitmapSource of the original file.
				pImportFilter, pointer to the filter to use to decode the bitmap

	Purpose:	Sets the original bitmap file associated with this reference.

********************************************************************************************/

void KernelBitmap::SetOriginalSource(BitmapSource* pSource, BaseBitmapFilter* pImportFilter)
{
	if (ActualBitmap == NULL)
		return;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	// The OILBitmap has the reference in it
	ActualBitmap->SetOriginalSource(pSource, pImportFilter);
}

/********************************************************************************************

>	BOOL KernelBitmap::GetOriginalSource(BitmapSource** pSource = NULL,
											BaseBitmapFilter** ppImportFilter = NULL)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96

	Inputs:		pSource, ptr to a void ptr to update to point at the original BitmapSource
				ppFilterName, ptr to a BaseBitmapFilter ptr to update to point at the filter 
				to use to decode the bitmap
				
				If both these inputs are NULL, then the function will simply return
				whether an original file exists or not.

	Outputs:	The input pointers are set if an original file is available, otherwise they
				are left unchanged.

	Returns:	TRUE, if an original file is available, and the pointers were set if non NULL.
				FALSE, if no orginal files is available.

	Purpose:	Gets the original bitmap file buffer associated with this reference.

********************************************************************************************/

BOOL KernelBitmap::GetOriginalSource(BitmapSource** pSource, BaseBitmapFilter** ppImportFilter)
{
	if (ActualBitmap == NULL)
		return FALSE;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	// The OILBitmap has the reference in it
	return ActualBitmap->GetOriginalSource(pSource, ppImportFilter);
}

/********************************************************************************************

>	void KernelBitmap::SetXPEInfo(KernelBitmap* pMasterBitmap, IXMLDOMDocumentPtr pNewEditList)
				
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/2004

	Inputs:		pNewEditList - pointer to XML edit list document to be stored with this bitmap

	Purpose:	Sets the XPE edit list document for this bitmap.

********************************************************************************************/

PORTNOTE("other","KernelBitmap::GetXPEInfo removed")
#ifndef EXCLUDE_FROM_XARALX
void KernelBitmap::SetXPEInfo(KernelBitmap* pMasterBitmap, IXMLDOMDocumentPtr pNewEditList)
{
	if (ActualBitmap)
	{
		OILBitmap* pOilMaster = NULL;
		if (pMasterBitmap) pOilMaster = pMasterBitmap->GetActualBitmap();
		ActualBitmap->SetXPEInfo(pOilMaster, pNewEditList);
	}
}
#endif

/********************************************************************************************

>	void KernelBitmap::GetXPEInfo(KernelBitmap*& refMasterBitmap,
								  IXMLDOMDocumentPtr& refEditList,
								  BOOL bEnsureList)
				
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/2004
	Inputs:		-
	Outputs:	Updates the passed-in reference variables!!!
				refMasterBitmap updated to contain pointer to the master bitmap for this bitmap
				refEditList updated to contain pointer the edits list for this bitmap
				NOTE:
				I'd prefer to use pointers but there are problems with IXMLDOMDocument2Ptr*
	Returns:	Pointer to XML document containing edit list for this bitmap.
	Purpose:	Gets the stored XPE information for this bitmap.

********************************************************************************************/

PORTNOTE("other","KernelBitmap::GetXPEInfo removed")
#ifndef EXCLUDE_FROM_XARALX
void KernelBitmap::GetXPEInfo(KernelBitmap*& refMasterBitmap,
							  IXMLDOMDocumentPtr& refEditList,
							  BOOL bEnsureList)
{
	if (ActualBitmap)
	{
		OILBitmap* pMasterOil = NULL;
		ActualBitmap->GetXPEInfo(pMasterOil, refEditList, bEnsureList);
		BitmapList* pBitmapList = m_pParentList;
		if (pBitmapList==NULL)
		{
			Document* pDoc = Document::GetCurrent();
			if (pDoc)
				pBitmapList = pDoc->GetBitmapList();
		}

		if (pMasterOil && pBitmapList)
		{
			KernelBitmap* pBmp = (KernelBitmap*)pBitmapList->GetHead();

			// Scan through the Documents bitmap list to see if any of
			// the existing KernelBitmaps reference this OILBitmap.
			while (pBmp != NULL)
			{
				if (pBmp->ActualBitmap == pMasterOil)
				{
					refMasterBitmap = pBmp;
					break;
				}

				pBmp = (KernelBitmap*)pBitmapList->GetNext(pBmp);
			}
		}
	}
}
#endif

/********************************************************************************************

>	BOOL KernelBitmap::IsXPEMaster()
				
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this bitmap is a used as a master by other bitmaps in the same list
	Purpose:	Gets the XPE master status of this bitmap by searching the bitmap list

********************************************************************************************/

BOOL KernelBitmap::IsXPEMaster()
{
	if (ActualBitmap)
		return ActualBitmap->IsXPEMaster();
	return FALSE;
}


/********************************************************************************************

>	BOOL KernelBitmap::DestroyXPEInfo()
				
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this bitmap has no XPE info or the user agreed it could be destroyed
				FALSE otherwise
	Purpose:	Aks the user whether he wants to destroy XPE info

********************************************************************************************/

BOOL KernelBitmap::DestroyXPEInfo()
{
	if (ActualBitmap==NULL)
		return TRUE;

	PORTNOTETRACE("other","KernelBitmap::DestroyXPEInfo - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	IXMLDOMDocumentPtr pEdits = NULL;
	OILBitmap* pMaster = NULL;
	ActualBitmap->GetXPEInfo(pMaster, pEdits);

	if (pEdits==NULL || pMaster==NULL)
		return TRUE;

	INT32 iButton = InformWarning(_R(IDS_XPE_DESTROY_MSG), _R(IDS_CONTINUE), _R(IDS_CANCEL));
	if (iButton==1)
	{
		ActualBitmap->DestroyXPEInfo();
	}
	return (iButton==1);
#else
	return TRUE;
#endif
}


/********************************************************************************************

>	void KernelBitmap::ReplaceOILBitmap(LPBYTE pBits, LPBITMAPINFO pInfo)
				
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/02/2004
	Inputs:		pBits - pointer to bitmap data
				pInfo - pointer to bitmap info header
	Outputs:	-
	Returns:	-
	Purpose:	Remove the OIL bitmap from memory if it can be constructed from master and edits

********************************************************************************************/

void KernelBitmap::ReplaceOILBitmap(LPBYTE pBits, LPBITMAPINFO pInfo)
{
	if (IsDefaultBitmap())
	{
		// We can't replace the default bitmap so make a new OilBitmap and attach that
		CWxBitmap	   *pOilBitmap = new CWxBitmap(pInfo, pBits);
		if (pOilBitmap && IsLossy())
			pOilBitmap->SetAsLossy();
		ActualBitmap = OILBitmap::Attach(pOilBitmap);
	}
	else
	{
		ActualBitmap->ReplaceBitmap(pBits, pInfo);
	}
}


/********************************************************************************************

>	BOOL KernelBitmap::CreateGreyscaleVersion(BOOL ForceGreyBmp = FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Purpose:	Creates a greyscale version of this bitmap

********************************************************************************************/

BOOL KernelBitmap::CreateGreyscaleVersion(BOOL ForceGreyBmp)
{
	if (ActualBitmap == NULL)
		return FALSE;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	if (!IsGreyscale())
	{
//		if (GetBPP() == 8 && !ForceGreyBmp)
		if (!ForceGreyBmp)
		{
			// If the bitmap is already a paletted bitmap, then we don't
			// need to generate a greyscale version, 'cus we can use some
			// clever palette manipulation
			GenerateGreyscaleTable();
		}
		else
		{
			// Make a new grey level version of the bitmap
			OILBitmap* pOILBmp = DIBUtil::Create8bppGreyscaleBitmap(this);
			if (pOILBmp == NULL)
				return FALSE;

			pOILBmp->m_bIsGreyscale = TRUE;
			ActualBitmap->SetGreyscaleVersion(pOILBmp);
		}
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL KernelBitmap::DestroyGreyscaleVersion()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Purpose:	Destroys the greyscale version of this bitmap

********************************************************************************************/

BOOL KernelBitmap::DestroyGreyscaleVersion()
{
	if (ActualBitmap == NULL)
		return FALSE;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	return ActualBitmap->DestroyGreyscaleVersion();
}

/********************************************************************************************

>	OILBitmap* KernelBitmap::ExtractOILBitmap()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Purpose:	Extracts the OILBitmap from this kernel bitmap
				AND DELETES THIS KERNEL BITMAP

********************************************************************************************/

OILBitmap* KernelBitmap::ExtractOILBitmap()
{
	if (ActualBitmap == NULL)
		return NULL;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	OILBitmap* pBmp = ActualBitmap;

	if (ActualBitmap->IsInGlobalList())
		ActualBitmap->RemoveFromGlobalList();

#ifdef _DEBUG
	ActualBitmap->m_bIsAttached = FALSE;
#endif

	m_bDontDeleteActualBitmap = TRUE;
	delete this;

	return pBmp;
}

/********************************************************************************************

>	OILBitmap* KernelBitmap::GetGreyscaleVersion(BOOL ForceGreyBmp = FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Purpose:	Gets the greyscale version of this bitmap

********************************************************************************************/

OILBitmap* KernelBitmap::GetGreyscaleVersion(BOOL ForceGreyBmp)
{
	if (ActualBitmap == NULL)
		return NULL;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	if (IsGreyscale())
		return ActualBitmap;

	OILBitmap* pGreyBmp = ActualBitmap->GetGreyscaleVersion();
	if (pGreyBmp == NULL)
	{
		// Make a greyscale bitmap
		CreateGreyscaleVersion(ForceGreyBmp);
	}

	return ActualBitmap->GetGreyscaleVersion();
}

/********************************************************************************************

>	void KernelBitmap::GenerateGreyscaleTable()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Purpose:	-

********************************************************************************************/

void KernelBitmap::GenerateGreyscaleTable()
{
//	if (ActualBitmap == NULL || GetBPP() != 8)
	if (ActualBitmap == NULL || GetBPP()>8)
		return;

	ActualBitmap->GenerateGreyscaleTable();
}

/********************************************************************************************

>	BOOL KernelBitmap::IsUsedInDocument(Document* pDoc, BOOL bIncludeHidden = FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Purpose:	Finds if this bitmap is being used in a specified document or not

********************************************************************************************/

BOOL KernelBitmap::IsUsedInDocument(Document* pDoc, BOOL bIncludeHidden)
{
	if (ActualBitmap == NULL)
		return FALSE;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	if (HasBeenDeleted())
		return OILBitmap::Default->IsUsedInDocument(pDoc, bIncludeHidden);

	return ActualBitmap->IsUsedInDocument(pDoc, bIncludeHidden) || IsXPEMaster();
}

/********************************************************************************************

>	BOOL KernelBitmap::IsGreyscale()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Purpose:	Finds if this bitmap is greyscale or not

********************************************************************************************/

BOOL KernelBitmap::IsGreyscale()
{
	if (ActualBitmap == NULL)
		return FALSE;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	if (HasBeenDeleted())
		return OILBitmap::Default->IsGreyscale();

	return ActualBitmap->IsGreyscale();
}


/********************************************************************************************

>	BOOL KernelBitmap::IsLossy()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Purpose:	Finds if this bitmap is lossy or not

********************************************************************************************/

BOOL KernelBitmap::IsLossy()
{
	if (ActualBitmap == NULL)
		return FALSE;

	ERROR3IF_OILBMP_PTR_INVALID(ActualBitmap,
				"Bitmap Error.  Found a reference to a deleted bitmap.");

	if (HasBeenDeleted())
		return OILBitmap::Default->IsLossy();

	return ActualBitmap->IsLossy();
}


/*******************************************************************************************
>	UINT32 KernelBitmap::GetDelay()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/96
	Purpose:	Returns m_AnimDelay for an OilBitmap. 

********************************************************************************************/

UINT32 KernelBitmap::GetDelay()
{
	if (ActualBitmap != NULL)
		return ActualBitmap->GetBitmapAnimDelay();

	return 0;	
}


/********************************************************************************************

>	void KernelBitmap::SetDelay(UINT32 Delay)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/96
	Purpose:	Sets the m_AnimDelay memeber for an OilBitmap. 

********************************************************************************************/
void KernelBitmap::SetDelay(UINT32 Delay)
{
	if (ActualBitmap != NULL)
		ActualBitmap->SetBitmapAnimDelay(Delay);
}

/*******************************************************************************************
>	BOOL KernelBitmap::GetInterpolation()

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/00
	Purpose:	Returns m_bInterpolation for an OilBitmap. 

********************************************************************************************/

BOOL KernelBitmap::GetInterpolation()
{
	if (ActualBitmap != NULL)
		return ActualBitmap->GetInterpolation();

	return 0;	
}


/********************************************************************************************

>	void KernelBitmap::SetInterpolation(BOOL Delay)

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/00
	Purpose:	Sets the m_bInterpolation member for an OilBitmap. 

********************************************************************************************/
void KernelBitmap::SetInterpolation(BOOL bInterpolation)
{
	if (ActualBitmap != NULL)
		ActualBitmap->SetInterpolation(bInterpolation);
}

/*******************************************************************************************

>	GIFDisposalMethod KernelBitmap::GetAnimationRestoreType () const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/97
	Purpose:	Returns the GIF restore type for an OilBitmap. 

********************************************************************************************/

GIFDisposalMethod KernelBitmap::GetAnimationRestoreType () const
{
	if (ActualBitmap != NULL)
		return ActualBitmap->GetAnimationRestoreType();

	return GDM_LEAVE;	
}

/********************************************************************************************

>	void KernelBitmap::SetAnimationRestoreType(GIFDisposalMethod Type)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/97
	Inputs:		new GIF disposal method
	Purpose:	Sets the GIF restore type for an OilBitmap. 

********************************************************************************************/

void KernelBitmap::SetAnimationRestoreType(GIFDisposalMethod Type)
{
	if (ActualBitmap != NULL)
		ActualBitmap->SetAnimationRestoreType(Type);
}

/********************************************************************************************

>	void KernelBitmap::SetLeftOffset(UINT32 LeftOffset)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/97
	Inputs:		new left offset
	Purpose:	Sets the left offset for an OilBitmap. 

********************************************************************************************/

void KernelBitmap::SetLeftOffset(UINT32 LeftOffset)
{
	if (ActualBitmap != NULL)
		ActualBitmap->SetLeftOffset(LeftOffset);
}

/********************************************************************************************

>	UINT32 KernelBitmap::GetLeftOffset() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/97
	Returns:	the current left offset
	Purpose:	Gets the left offset for an OilBitmap. 

********************************************************************************************/

UINT32 KernelBitmap::GetLeftOffset() const
{
	if (ActualBitmap != NULL)
		return ActualBitmap->GetLeftOffset();

	return 0;	
}

/********************************************************************************************

>	void KernelBitmap::SetTopOffset(UINT32 TopOffset)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/97
	Inputs:		new top offset
	Purpose:	Sets the top offset for an OilBitmap. 

********************************************************************************************/

void KernelBitmap::SetTopOffset(UINT32 TopOffset)
{
	if (ActualBitmap != NULL)
		ActualBitmap->SetTopOffset(TopOffset);
}

/********************************************************************************************

>	UINT32 KernelBitmap::GetTopOffset() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/97
	Returns:	the current top offset
	Purpose:	Gets the top offset for an OilBitmap. 

********************************************************************************************/

UINT32 KernelBitmap::GetTopOffset() const
{
	if (ActualBitmap != NULL)
		return ActualBitmap->GetTopOffset();

	return 0;	
}

/********************************************************************************************

>	BOOL KernelBitmap::IsBrowserPalette() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/06/97
	Returns:	True if the bitmap has a browser compatible palette.
	Purpose:	To see if the palette attached to this bitmap is browser compatible.

********************************************************************************************/

BOOL KernelBitmap::IsBrowserPalette() const
{
	if (ActualBitmap != NULL)
		return ActualBitmap->IsBrowserPalette();

	return FALSE;	
}

/********************************************************************************************

>	BOOL KernelBitmap::ArePalettesTheSame(const LPLOGPALETTE pLogPalette, const INT32 TransColour = -1)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/07/97
	Inputs:		pLogPalette		A log palette to check the palette against
				TransColour		The transparent colour in the palette
	Returns:	True if the bitmap has the same palette as the LOG palette.
	Purpose:	To see if the palette attached to this bitmap is browser compatible.
				It runs through the colours in a browser palette and checks to see if they
				are all present. The ordering is not checked and so the palette may be
				different from the one that we actually use.

********************************************************************************************/

BOOL KernelBitmap::ArePalettesTheSame(const LPLOGPALETTE pLogPalette, const INT32 TransColour)
{
	if (ActualBitmap != NULL)
		return ActualBitmap->ArePalettesTheSame(pLogPalette, TransColour);

	return FALSE;	
}

/********************************************************************************************

>	UINT32 KernelBitmap::GetHorizontalDPI() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Returns:	The Horizontal DPI of this bitmap
	Purpose:	Retrieve information on this bitmap

********************************************************************************************/

UINT32 KernelBitmap::GetHorizontalDPI() const
{
	if (ActualBitmap != NULL)
		return ActualBitmap->GetHorizontalDPI();

	return 96;	
}

/********************************************************************************************

>	UINT32 KernelBitmap::GetVerticalDPI() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Returns:	The Vertical DPI of this bitmap
	Purpose:	Retrieve information on this bitmap

********************************************************************************************/

UINT32 KernelBitmap::GetVerticalDPI() const
{
	if (ActualBitmap != NULL)
		return ActualBitmap->GetVerticalDPI();

	return 96;	
}

/********************************************************************************************

>	void KernelBitmap::SetName(String_256& NewName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/97
	Inputs:		The new name of this bitmap
	Purpose:	Set information on this bitmap

********************************************************************************************/

void KernelBitmap::SetName(String_256& NewName)
{
	if (ActualBitmap != NULL)
	{
		ActualBitmap->SetName(NewName);
		return;
	}

	return;	
}

/********************************************************************************************

>	String_256& KernelBitmap::GetName()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/97
	Returns:	The name of this bitmap
	Purpose:	Retrieve information on this bitmap

********************************************************************************************/

String_256& KernelBitmap::GetName()
{
	if (ActualBitmap != NULL)
		return ActualBitmap->GetName();


	// In the "oh no where's the ActualBitmap" case
	// The old code used to be:
	//		return String_256("");
	// This static dummy object is used for returning in the error case
	// The original used to return a ptr to a local variable, which is a tad dangerous.
	// This solution is not ideal, because there's a permanent instance of an object
	// that will probably never be used.
	static String_256 Dummy("");
	return Dummy;
}	

/********************************************************************************************

>	LPBITMAPINFO KernelBitmap::GetBitmapInfo()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/07/97
	Returns:	The BITMAPINFO for this bitmap or null.
	Purpose:	To get at the BITMAPINFO that this bitmap may have.
				
				BITMAPINFO  consists of:-
				    	BITMAPINFOHEADER    bmiHeader;
						RGBQUAD             bmiColors[1];
				This is the real bitmap info header giving the overall size of the export

	Note:		Only use from Winoil code

********************************************************************************************/

LPBITMAPINFO KernelBitmap::GetBitmapInfo()
{
	if (ActualBitmap != NULL)
		return ActualBitmap->GetBitmapInfo();

	return NULL;
}

/********************************************************************************************

>	LPRGBQUAD KernelBitmap::GetPaletteForBitmap()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/07/97
	Returns:	The palette for this bitmap or null.
	Purpose:	To get at the RGBQUAD palette that this bitmap may have.
				
				BITMAPINFO  consists of:-
				    	BITMAPINFOHEADER    bmiHeader;
						RGBQUAD             bmiColors[1];
				This is the real bitmap info header giving the overall size of the export

	Note:		Only use from Winoil code

********************************************************************************************/

LPRGBQUAD KernelBitmap::GetPaletteForBitmap()
{
	if (ActualBitmap != NULL)
		return ActualBitmap->GetPaletteForBitmap();

	return NULL;
}

/********************************************************************************************

>	LPBITMAPINFOHEADER KernelBitmap::GetBitmapInfoHeader()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/07/97
	Returns:	The bitmap header for this bitmap or null.
	Purpose:	To get at the BITMAPINFOHEADER that this bitmap should have.
				
				BITMAPINFO  consists of:-
				    	BITMAPINFOHEADER    bmiHeader;
						RGBQUAD             bmiColors[1];

	Note:		Only use from Winoil code

********************************************************************************************/

LPBITMAPINFOHEADER KernelBitmap::GetBitmapInfoHeader()
{
	if (ActualBitmap != NULL)
		return ActualBitmap->GetBitmapInfoHeader();

	return NULL;
}



#ifdef _DEBUG
/********************************************************************************************

>	BOOL KernelBitmap::AttachDebugCopyToCurrentDocument(String_256 strName)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/2000

	Inputs:		strName		the name you want the bitmap to have in the bitmap gallery.

	Outputs:	Attaches a copy of this bitmap to the bitmap gallery.

	Returns:	TRUE if successful,
				FALSE + TRACE output otherwise.

	Purpose:	When debugging, it's often useful to see what sort of bitmap you have.
				This method attaches a copy of this KernelBitmap to the current document,
				so that it will show up in the bitmap gallery.

	Notes:		This is DEBUG code - DON'T change it to compile for release, as it ISN'T
				TESTED and probably ISN'T STABLE or misses some initialisation somewhere.
				Go figure out how to do what you want and write your own function or use
				someone else's release code.

	Errors:		Will return FALSE and TRACE output if anything goes wrong.

********************************************************************************************/
BOOL KernelBitmap::AttachDebugCopyToCurrentDocument(String_256 strName)
{
	Document* pDoc = Document::GetCurrent();
	if (pDoc == NULL)
	{
		TRACEALL( _T("KernelBitmap::AttachDebugCopyToCurrentDocument; no current Doc!\n") );
		return FALSE;
	}

	KernelBitmap*	pCopyBitmap	= DIBUtil::CopyKernelBitmap(this, FALSE);
	if (pCopyBitmap == NULL)
	{
		TRACEALL( _T("KernelBitmap::AttachDebugCopyToCurrentDocument; Couldn't create copy Kernel bitmap!\n") );
		return FALSE;
	}

	OILBitmap*		pOILCopy	= (OILBitmap*)pCopyBitmap->GetActualBitmap();
	if (pOILCopy == NULL)
	{
		TRACEALL( _T("KernelBitmap::AttachDebugCopyToCurrentDocument; Couldn't get OIL bmp from Kernel bmp!\n") );
		return FALSE;
	}

	pOILCopy->SetName(strName);
	pCopyBitmap->Attach(pDoc->GetBitmapList());

	return TRUE;
}
#endif



/********************************************************************************************
/// OILBitmap
********************************************************************************************/



/********************************************************************************************

>	OILBitmap::OILBitmap()
					
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/8/94
	Purpose:	Constructor. Sets USage count to zero.

********************************************************************************************/

OILBitmap::OILBitmap()
{
	m_bTemp = TRUE;		// Not in global list yet

	m_pColourVersion = NULL;
	m_pGreyscaleVersion = NULL;
	m_pContonePalette = NULL;
	m_pGreyscaleTable = NULL;

	m_bIsAFractal = FALSE;
	m_bIsGreyscale = FALSE;
	m_bIsLossy = FALSE;

	SetBitmapAnimDelay(10);
	SetAnimationRestoreType(GDM_LEAVE);
	SetInterpolation(TRUE);

	m_LeftOffset = 0;
	m_TopOffset = 0;

PORTNOTE("other","Removed edit list")
#ifndef EXCLUDE_FROM_XARALX
	m_pEditList = NULL;
#endif
	m_pMasterBitmap = NULL;

	m_bUsedByBrush = FALSE;
#if !defined(EXCLUDE_FROM_RALPH)
	m_BitmapName.Load(_R(IDS_BITMAPNAME));
#else
	m_BitmapName = TEXT("Bitmap");
#endif
	m_bNeedsXPERebuild = FALSE;
	m_bHidden = FALSE;

#ifdef _DEBUG
	m_bIsAttached = FALSE;
#endif
}

/********************************************************************************************

>	OILBitmap::~OILBitmap()
					
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Purpose:	Destructor.

********************************************************************************************/

OILBitmap::~OILBitmap()
{
PORTNOTE("other","Removed edit list")
#ifndef EXCLUDE_FROM_XARALX
	if (m_pEditList)
	{
		m_pEditList = NULL;		// This is a smart pointer so just allow it to die naturally
	}
#endif

	if (m_pColourVersion)
	{
		ERROR3IF(m_pColourVersion->m_pGreyscaleVersion != this, "Bad greyscale version");

		// This must be a greyscale version of a colour bmp
		m_pColourVersion->m_pGreyscaleVersion = NULL;
	}

	// Ensure this bitmap is not in the global bitmap list
	RemoveFromGlobalList();

	if (m_pContonePalette)
		DestroyContonePalette();

	if (m_pGreyscaleVersion)
		DestroyGreyscaleVersion();

	// To do :- We could do a debug check here, to ensure that there are no references 
	//          to this bitmap anywhere
}

/********************************************************************************************

>	OILBitmap *OILBitmap::Attach( OILBitmap* pTheOILBitmap, BOOL Temp )
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/8/94
	Inputs:		A pointer to an OILBitmap.
	Purpose:	Increases the usage count on the OILBitmap. Designed to be called from
				KernelBitmap code.
	Returns:	The WhichOne pointer if it worked, or NULL if it failed.
	Errors:		A NULL return means that the WhichOne pointer was also NULL. SetError will
				NOT be called.
	SeeAlso:	OILBitmap::Detach
	Scope:		Static

********************************************************************************************/

OILBitmap *OILBitmap::Attach( OILBitmap* pTheOILBitmap, BOOL Temp )
{
	if (pTheOILBitmap == NULL)
		return NULL;

#ifdef _DEBUG

	ERROR3IF_OILBMP_PTR_INVALID(pTheOILBitmap,
				"Bitmap Error.  Trying to attach a deleted bitmap.");

	// Check for illegal things ...
	// Trap an attempt to attach a temp bitmap to more than one KernelBitmap
	if (pTheOILBitmap->m_bTemp && !pTheOILBitmap->m_bIsAFractal)
	{
		//ERROR3IF(pTheOILBitmap->m_bIsAttached, "Trying to attach a temp bitmap that is already attached");
		// Stefans change so that if IsNew is NULL in TryAndUseExistingKernelBitmap it will set the temp
		// flag means that this check goes off on animated bitmaps. Downgraded to trace all.
		if (pTheOILBitmap->m_bIsAttached)
			TRACEALL( _T("Trying to attach a temp bitmap that is already attached\n") );
	}

	pTheOILBitmap->m_bIsAttached = TRUE;

#endif

	if (!Temp && !pTheOILBitmap->IsInGlobalList())
	{
		ERROR3IF(pTheOILBitmap->IsAFractal(), "Someone's trying to attach a fractal to the global list !");

		if (!pTheOILBitmap->IsAFractal())
		{
			// It's not in the Global list yet, so add it now ...

			// First ensure the bitmap name is unique
			GetApplication()->GetGlobalBitmapList()->MakeNameUnique(&(pTheOILBitmap->m_BitmapName));

			// and the add it to the global bitmap list
			GetApplication()->GetGlobalBitmapList()->AddItem(pTheOILBitmap);

			// and flag that it's in the list
			pTheOILBitmap->m_bTemp = FALSE;

			// And set the flag if it's a greyscale bitmap
			// we can't tell if it is or not in WEBSTER-Martin-14/01/97
			// Code now in DIBUtil 10/11/97 Neville
//#ifdef WEBSTER
//			pTheOILBitmap->m_bIsGreyscale = FALSE;
//#else
			pTheOILBitmap->m_bIsGreyscale = DIBUtil::IsGreyscaleBitmap(pTheOILBitmap);
//#endif //WEBSTER
		}
	}

	return pTheOILBitmap;
}

/********************************************************************************************

>	BOOL OILBitmap::RemoveFromGlobalList()
			
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/96
	Purpose:	-
	Returns:	-
	Errors:		-

********************************************************************************************/

BOOL OILBitmap::RemoveFromGlobalList()
{
	if (!IsInGlobalList())
		return FALSE;		// Not in the list

	m_bTemp = TRUE;

	if (GetApplication()->GetGlobalBitmapList()->RemoveItem(this) == NULL)
	{
		ERROR3("Bitmap not found in Global bitmap list");
		return FALSE;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL OILBitmap::BuildContonePalette(DocColour &StartCol, DocColour &EndCol,
											EFFECTTYPE Effect, View *ScopeView)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com> or Will? (Fixed to handle colour seps, Jason)
	Created:	? (8/7/96)

	Inputs:		StartCol - The start colour for the palette (entry 0)
				EndCol - The end colour for the palette (entry 255)

				Effect - The grad fill effect (Mix, Rainbow, Alt-rainbow) to use

				ScopeView - The view to which this palette will be rendered. Used to
				determine current colour correction/separation settings for composite
				preview and separated views. Can be null, but it won't do any
				of your fancy colour seperationness for you.

	Purpose:	Builds a contone (graduated) palette between the start and end colours
				given, using the given effect (mix, rainbow, alt-rainbow), for output
				through the given view. See GradTable32::BuildGraduatedPalette for details
				of how the palette is built.

********************************************************************************************/

BOOL OILBitmap::BuildContonePalette(DocColour &StartCol, DocColour &EndCol,
										EFFECTTYPE Effect, View *ScopeView)
{
	UINT32 bpp = GetBPP();

	if (bpp > 8)
		// Can't do contone palette for this image
		return FALSE;

	RGBQUAD* pPalette = (RGBQUAD*)CCMalloc(256 * sizeof(RGBQUAD));
	if (pPalette == NULL)
		return FALSE;

	// Allocate the palette array (we always allocate enough for biggest (8bpp) palette)
//	m_pContonePalette = (RGBQUAD *) CCMalloc(256 * sizeof(RGBQUAD));
//	if (m_pContonePalette == NULL)
//		// Out of memory.
//		return FALSE;

	// Fill in the palette (we always use RGB space for now)
	INT32 NumCols = 256;
	switch (bpp)
	{
		case 1:
			NumCols = 2;
			break;

		case 2:
			NumCols = 4;
			break;

//		case 4:
//			// 4bpp images are expanded to 8bpp otherwise they don't work on
//			// some PostScript printers.
//		case 8:
//		default:
//			NumCols = 256;
//			break;
	}

	GradTable32::BuildGraduatedPalette(StartCol, EndCol, ScopeView, Effect, NumCols, pPalette);

	BYTE* pGreyTable = GetGreyscaleTable();
	if (pGreyTable)
	{
		m_pContonePalette = (RGBQUAD *) CCMalloc(256 * sizeof(RGBQUAD));
		if (m_pContonePalette == NULL)
			return(FALSE);

		// Copy the entries from the contone palette into the new one,
		// using the Grey table as a guide
		for (INT32 i=0; i<256; i++)
		{
			m_pContonePalette[i] = pPalette[pGreyTable[i]];
		}

		CCFree(pPalette);
	}
	else
	{
		m_pContonePalette = pPalette;
	}

	return(TRUE);
}



/********************************************************************************************

>	DocColour OILBitmap::GetContonePaletteEntry(UINT32 PaletteIndex)
					
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> ?
	Created:	?/7/96
	Purpose:	Read an entry from the Contone Palette

********************************************************************************************/

DocColour OILBitmap::GetContonePaletteEntry(UINT32 PaletteIndex)
{
	ERROR3IF(m_pContonePalette == NULL, "No contone palette to get data from!");
	if (m_pContonePalette == NULL)
		return DocColour(COLOUR_WHITE);		// Error - return arbitrary colour

	// Ok, get the entry and convert it to a DocColour
	return DocColour((INT32) m_pContonePalette[PaletteIndex].rgbRed,
					 (INT32) m_pContonePalette[PaletteIndex].rgbGreen,
					 (INT32) m_pContonePalette[PaletteIndex].rgbBlue);
}


/********************************************************************************************

>	BOOL OILBitmap::DestroyContonePalette()
					
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> ?
	Created:	?/7/96
	Purpose:	Destroys the contone palette

********************************************************************************************/

BOOL OILBitmap::DestroyContonePalette()
{
	// (ChrisG 4/1/01) - Only delete the contone palette if we actually have one.
	if (m_pContonePalette != NULL)
	{
		// Free and NULL our palette array pointer.
		CCFree(m_pContonePalette);
		m_pContonePalette = NULL;
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL OILBitmap::SetGreyscaleVersion(OILBitmap* pOILBmp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Purpose:	Creates a greyscale version of this bitmap

********************************************************************************************/

BOOL OILBitmap::SetGreyscaleVersion(OILBitmap* pOILBmp)
{
	ERROR3IF(pOILBmp && !pOILBmp->IsGreyscale(), "SetGreyscale called with non-grey bitmap");

	DestroyGreyscaleVersion();

	m_pGreyscaleVersion = pOILBmp;
	m_pGreyscaleVersion->m_pColourVersion = this;

//	GreyscaleBitmapList* pGreyList = 
//		GetApplication()->GetGlobalBitmapList()->GetGreyscaleBitmapList();
	
//	if (pGreyList)
//		pGreyList->AddItem(m_pGreyscaleVersion);

	return TRUE;
}

/********************************************************************************************

>	BOOL OILBitmap::DestroyGreyscaleVersion()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Purpose:	Destroys the greyscale version of this bitmap

********************************************************************************************/

BOOL OILBitmap::DestroyGreyscaleVersion()
{
	if (m_pGreyscaleVersion == NULL)
		return TRUE;

	ERROR3IF(m_pGreyscaleVersion->m_pColourVersion == NULL, "Greyscale bitmap has no colour pointer");
//	GreyscaleBitmapList* pGreyList = 
//		GetApplication()->GetGlobalBitmapList()->GetGreyscaleBitmapList();
	
//	if (pGreyList)
//		pGreyList->RemoveItem(m_pGreyscaleVersion);

	delete m_pGreyscaleVersion;
	m_pGreyscaleVersion = NULL;
	return TRUE;
}

/********************************************************************************************

>	virtual void OILBitmap::SetOriginalFile(void* pFileBuffer = NULL, 
											BaseBitmapFilter* pImportFilter = NULL)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96

	Inputs:		pFileBuffer, a pointer to a buffer containing the original file.
				pImportFilter, pointer to the filter to use to decode the bitmap

	Purpose:	Sets the original bitmap file associated with this reference.

	Notes:		The input file buffer should be CCMalloc'd, and will be CCFree'd when the
				reference is destroyed, or changed to another buffer.

********************************************************************************************/

void OILBitmap::SetOriginalSource(BitmapSource* pSource, BaseBitmapFilter* pImportFilter)
{
	// Let the reference do the hard work ...
	m_OriginalBitmap.SetOriginalSource(pSource, pImportFilter);
}

/********************************************************************************************

>	virtual BOOL OILBitmap::GetOriginalSource(BitmapSource** pSource = NULL
											BaseBitmapFilter** ppImportFilter = NULL)
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96

	Inputs:		pSource, ptr to a BitmapSource ptr to update to point at the BitmapSource
				ppFilterName, ptr to a BaseBitmapFilter ptr to update to point at the filter 
				to use to decode the bitmap
				
				If both these inputs are NULL, then the function will simply return
				whether an original file exists or not.

	Outputs:	The input pointers are set if an original file is available, otherwise they
				are left unchanged.

	Returns:	TRUE, if an original file is available, and the pointers were set if non NULL.
				FALSE, if no orginal files is available.

	Purpose:	Gets the original bitmap file buffer associated with this reference.

********************************************************************************************/

BOOL OILBitmap::GetOriginalSource(BitmapSource** pSource, BaseBitmapFilter** ppImportFilter)
{
	// Let the reference do the hard work ...
	return m_OriginalBitmap.GetOriginalSource(pSource, ppImportFilter);
}





/****************************************************************************

>	BOOL OILBitmap::IsUsedByNode(Node* pNode)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/2005

	Inputs:		pNode		- pointer to a Node
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	Determines if this bitmap is used by the specified node

****************************************************************************/

BOOL OILBitmap::IsUsedByNode(Node* pNode)
{
	INT32 Count = 0;

	// Does this node have any bitmaps in it ?
	KernelBitmap* pBitmap = pNode->EnumerateBitmaps(Count++);

	while (pBitmap != NULL)
	{
		// Check for a deleted bitmap
		if (pBitmap->HasBeenDeleted())
		{
			// Use the default bitmap instead
			pBitmap = pBitmap->GetParentBitmapList()->FindDefaultBitmap();

			// There should always be a default bitmap at the start of the list
			ERROR2IF(pBitmap == NULL, 0L, "Couldn't find the default bitmap");
		}

		// Found a bitmap reference, so ...
		// .. is it the same as this bitmap
		if (pBitmap->ActualBitmap == this)
		{
			return TRUE;	// Yep, this bitmap is being used
		}
		
		pBitmap = pNode->EnumerateBitmaps(Count++);				
	}

	return(FALSE);
}




/****************************************************************************

>	BOOL OILBitmap::IsUsedByChildren(Node* pNode, BOOL bIncludeHidden)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/2005

	Inputs:		pNode		- pointer to a Node
				bIncludeHidden- 
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	Determines if this bitmap is used by the children of the 
				specified node (or their children etc)

****************************************************************************/

BOOL OILBitmap::IsUsedByChildren(Node* pNode, BOOL bIncludeHidden)
{
	Node* pChild = pNode->FindFirstChild();

	while (pChild != NULL)
	{
		if (pChild->IsNodeHidden())
		{
			if (bIncludeHidden)
			{
				Node* pTestNode = ((NodeHidden*)pChild)->HiddenNd;

				if (IsUsedByNode(pTestNode))
					return(TRUE);

				if (pTestNode->FindFirstChild() && IsUsedByChildren(pTestNode, bIncludeHidden))
					return(TRUE);
			}
		}
		else
		{
			if (pChild->FindFirstChild() && IsUsedByChildren(pChild, bIncludeHidden))
				return(TRUE);

			if (IsUsedByNode(pChild))
				return(TRUE);
		}

		pChild = pChild->FindNext();
	}

	return(FALSE);
}



/********************************************************************************************

>	BOOL OILBitmap::IsUsedInDocument(Document* pDoc, BOOL bIncludeHidden = FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Purpose:	Finds if this bitmap is being used in a specified document or not

********************************************************************************************/

BOOL OILBitmap::IsUsedInDocument(Document* pDoc, BOOL bIncludeHidden)
{
	// Scan the document's tree for bitmap references
	Node* pNode = Node::DocFindFirstDepthFirst(pDoc);
	while (pNode != NULL)
	{
		// Handle hidden nodes
		if (pNode->IsNodeHidden())
		{
			if (bIncludeHidden)
			{
				Node* pTestNode = ((NodeHidden*)pNode)->HiddenNd;
				if (IsUsedByNode(pTestNode))
					return(TRUE);

				if (pTestNode->FindFirstChild() && IsUsedByChildren(pTestNode, bIncludeHidden))
					return(TRUE);
			}
		}
		else
		{
			if (IsUsedByNode(pNode))
				return(TRUE);
		}

		// Move onto the next node in the tree
		pNode = pNode->DocFindNextDepthFirst(); 
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL OILBitmap::InvalidateAllReferences(Document* pDoc)

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>, adapted from IsUsedInDocument above
	Created:	12-10-00
	Purpose:	Called when the interpolation setting is changed.
				Determines whether this bitmap is being used in the specified document.
				All references to the bitmap (e.g. shapes with bitmap fill) are invalidated
				so that they will be redrawn with the new interpolation setting.

********************************************************************************************/

BOOL OILBitmap::InvalidateAllReferences(Document* pDoc)
{
	// Scan the document's tree for bitmap references
	Node* pNode = Node::DocFindFirstDepthFirst(pDoc);
	while (pNode != NULL)
	{
		// Ignore hidden nodes
		if (!pNode->IsNodeHidden())
		{
			INT32 Count = 0;

			// Does this node have any bitmaps in it ?
			KernelBitmap* pBitmap = pNode->EnumerateBitmaps(Count++);

			while (pBitmap != NULL)
			{
				ERROR2IF(pBitmap->GetParentBitmapList() == NULL, 0L, "Bitmap has no parent list");
				ERROR2IF(pBitmap->GetParentBitmapList()->GetParentDocument() != pDoc, 0L, "Document mismatch");
					
				// Check for a deleted bitmap
				if (pBitmap->HasBeenDeleted())
				{
					// Use the default bitmap instead
					pBitmap = pBitmap->GetParentBitmapList()->FindDefaultBitmap();

					// There should always be a default bitmap at the start of the list
					ERROR2IF(pBitmap == NULL, 0L, "Couldn't find the default bitmap");
				}

				// Found a bitmap reference, so ...
				// .. is it the same as this bitmap
				if (pBitmap->ActualBitmap == this)
				{
					// We've found a node which refers to 'this' bitmap.
					// We need to find the bounding rectangle of the object which has
					// the bitmap property, so we can invalidate that area of the screen.
					
					// get a copy of the current node pointer
					Node* pCurrentNode = pNode;

					// if this is a bitmap fill or bitmap transparency...
					if( pCurrentNode->IsAnAttribute() )
					{
						// ...then get the parent instead
						pCurrentNode = pNode->FindParent();
					}

					// Invalidate it so that it will be redrawn.
					if( pCurrentNode->IsBounded() )
					{
						// Now we have either a bitmap, or an object which has a bitmap
						// fill/transparency attribute.

						// cast the node to a NodeRenderableBounded
						NodeRenderableBounded* pBoundedNode = (NodeRenderableBounded*) pCurrentNode;
						
						// get the spread of the node
						Spread* pSpread = NULL;
						pSpread = pBoundedNode->FindParentSpread();
						ERROR3IF( (pSpread==NULL), "OILBitmap::InvalidateAllReferences - can't determine node's parent spread" );

						// force it to be redrawn
						Document::GetSelected()->ForceRedraw(pSpread, pBoundedNode->GetBoundingRect(), FALSE, pBoundedNode, FALSE);
					}
				}
				
				pBitmap = pNode->EnumerateBitmaps(Count++);				
			}
		}

		// Move onto the next node in the tree
		pNode = pNode->DocFindNextDepthFirst(); 
	}

	return FALSE;
}

/********************************************************************************************

>	UINT32 OILBitmap::GetBitmapSize()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Purpose:	Finds the size in bytes, of this bitmap

********************************************************************************************/

UINT32 OILBitmap::GetBitmapSize()
{
	BitmapInfo Info;
	Info.MemoryUsed = 0;

	// Get the bitmap Info
	GetInfo(&Info);

	return Info.MemoryUsed;
}


/********************************************************************************************

>	void OILBitmap::SetXPEInfo(OILBitmap* pMasterBitmap, IXMLDOMDocumentPtr pNewEditList)
				
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/2004

	Inputs:		pNewEditList - pointer to XML edit list document to be stored with this bitmap

	Purpose:	Sets the XPE edit list document for this bitmap.

********************************************************************************************/

PORTNOTE("other","Removed OILBitmap::SetXPEInfo")
#ifndef EXCLUDE_FROM_XARALX
void OILBitmap::SetXPEInfo(OILBitmap* pMasterBitmap, IXMLDOMDocumentPtr pNewEditList)
{
//	ERROR3IF(pMasterBitmap==NULL || pNewEditList==NULL,
//				"Bitmap Error. Either master pointer or edit list NULL in SetXPEInfo");

	m_pMasterBitmap = pMasterBitmap;
	m_pEditList = pNewEditList;
}
#endif

/********************************************************************************************

>	void OILBitmap::GetXPEInfo(OILBitmap*& refMasterBitmap,
								  IXMLDOMDocumentPtr& refEditList,
								  BOOL bEnsureList)
				
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/2004
	Inputs:		-
	Outputs:	Updates the passed-in reference variables!!!
				refMasterBitmap updated to contain pointer to the master bitmap for this bitmap
				refEditList updated to contain pointer the edits list for this bitmap
	Returns:	Pointer to XML document containing edit list for this bitmap.
	Purpose:	Gets the stored XPE information for this bitmap.

********************************************************************************************/

PORTNOTE("other","Removed OILBitmap::GetXPEInfo")
#ifndef EXCLUDE_FROM_XARALX
void OILBitmap::GetXPEInfo(OILBitmap*& refMasterBitmap,
							  IXMLDOMDocumentPtr& refEditList,
							  BOOL bEnsureList)
{
	refMasterBitmap = m_pMasterBitmap;
	refEditList = m_pEditList;

	if (bEnsureList && refEditList==NULL)
	{
		refEditList = CXMLUtils::NewDocument();
	}

// I'd prefer not to use references but there are problems with IXMLDOMDocument2Ptr*
//	if (ppMasterBitmap) *ppMasterBitmap = m_pMasterBitmap;
//	if (ppEditList)		*ppEditList = m_pEditList;
}
#endif

/********************************************************************************************

>	BOOL OILBitmap::IsXPEMaster()
				
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this bitmap is a used as a master by other bitmaps in the same list
	Purpose:	Gets the XPE master status of this bitmap by searching the bitmap list

********************************************************************************************/

BOOL OILBitmap::IsXPEMaster()
{
	OILBitmap* poBmp = (OILBitmap*) GetApplication()->GetGlobalBitmapList()->GetHead();
	while (poBmp)
	{
		if (poBmp->m_pMasterBitmap == this)
			return TRUE;

		poBmp = (OILBitmap*) GetApplication()->GetGlobalBitmapList()->GetNext(poBmp);
	}

	return FALSE;
}


/********************************************************************************************

>	BOOL OILBitmap::DestroyXPEInfo()
				
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this bitmap has no XPE info or the user agreed it could be destroyed
				FALSE otherwise
	Purpose:	Aks the user whether he wants to destroy XPE info

********************************************************************************************/

BOOL OILBitmap::DestroyXPEInfo()
{
	PORTNOTETRACE("other","OILBitmap::DestroyXPEInfo - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (m_pMasterBitmap==NULL || m_pEditList==NULL)
		return TRUE;

	m_pMasterBitmap = NULL;		// This is just a pointer to another kernel bitmap
	m_pEditList = NULL;			// This is a smart ptr so NULL-ing it should release the XML doc
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL OILBitmap::RemoveXPEBitmap()
				
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this bitmap has no XPE info or the user agreed it could be destroyed
				FALSE otherwise
	Purpose:	Aks the user whether he wants to destroy XPE info

********************************************************************************************/

BOOL OILBitmap::RemoveXPEBitmap()
{
	PORTNOTETRACE("other","OILBitmap::RemoveXPEBitmap - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (m_pMasterBitmap==NULL || m_pEditList==NULL)
		return FALSE;

	if (IsDefaultBitmap())
		return FALSE;

	if (m_bNeedsXPERebuild)
		return TRUE;

	DeleteData(FALSE);
	m_bNeedsXPERebuild = TRUE;
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL OILBitmap::RebuildXPEBitmap()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the OIL bitmap was removed
				FALSE otherwise
	Purpose:	Remove the OIL bitmap from memory if it can be constructed from master and edits

********************************************************************************************/

BOOL OILBitmap::RebuildXPEBitmap()
{
PORTNOTE("other","OILBitmap::RebuildXPEBitmap - do nothing")
#ifndef EXCLUDE_FROM_XARALX
	TRACE( _T("Warning - OILBitmap::RebuildXPEBitmap called\n") );
	if (!m_bNeedsXPERebuild)
		return TRUE;

	// If the XPE info is not present then we're in BIG trouble!
	ERROR2IF(m_pMasterBitmap==NULL || m_pEditList==NULL, FALSE, "ARGH! Can't rebuild Oil bitmap")
	if (m_pMasterBitmap==NULL || m_pEditList==NULL)
		return FALSE;

	// Set the flag before calling DoProcessBitmap to prevent possible infinite recursion
	m_bNeedsXPERebuild = FALSE;

	// Regenerate the OIL bitmap from the master and the edits
	XPEEditOp::DoProcessBitmap(this);
#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL OILBitmap::IsHiddenInGallery()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the OIL bitmap should NOT be displayed in the bitmap gallery
				FALSE otherwise
	Purpose:	Hide some bitmaps from the user to avoid confusing the user

********************************************************************************************/

BOOL OILBitmap::IsHiddenInGallery()
{
	return (HasBeenDeleted() || NeedsXPERebuild() || m_bHidden);
}


/********************************************************************************************

>	BOOL OILBitmap::NeedsXPERebuild()
				
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/02/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this bitmap has no XPE info or the user agreed it could be destroyed
				FALSE otherwise
	Purpose:	Ask the user whether he wants to destroy XPE info

********************************************************************************************/

BOOL OILBitmap::NeedsXPERebuild()
{
	return m_bNeedsXPERebuild;
}



/********************************************************************************************

		Kernel Bitmap Reference

********************************************************************************************/

/********************************************************************************************

>	KernelBitmapRef::KernelBitmapRef()
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Purpose:	Constructor for kernel bitmap reference.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

KernelBitmapRef::KernelBitmapRef()
{
	m_pTheBitmap = NULL;
	m_bHidden = FALSE;
}

/********************************************************************************************

>	KernelBitmapRef::~KernelBitmapRef()
				
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Purpose:	Destructor for kernel bitmap reference.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

KernelBitmapRef::~KernelBitmapRef()
{
	Detach();
}

/********************************************************************************************

>	KernelBitmapRef& KernelBitmapRef::operator=(KernelBitmapRef& OtherBitmap)
			
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Purpose:	Make one bitmap ref the same as the other.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

KernelBitmapRef& KernelBitmapRef::operator=(KernelBitmapRef& OtherBitmap)
{
	Attach(OtherBitmap.GetBitmap());
	return (*this);
}

/********************************************************************************************

>	void KernelBitmapRef::Attach(KernelBitmap* NewBitmap, Document* BitmapDoc = NULL)
		
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		NewBitmap	Source kernel bitmap
				BitmapDoc	Destination document (Defaults to NULL)
	Outputs:	-
	Purpose:	Sets the bitmap that this object is referencing.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void KernelBitmapRef::Attach(KernelBitmap* NewBitmap, Document* BitmapDoc)
{
	if (NewBitmap == NULL)
	{
		// No bitmap specified, so lets just create a default one.
		NewBitmap = KernelBitmap::MakeKernelBitmap();
	}

	ERROR3IF(NewBitmap == NULL, "Trying to attach NULL bitmap to bitmap ref");
	if (NewBitmap == NULL)
		return;

	Detach(TRUE);

	ERROR3IF(IsHidden(), "Attaching a bitmap to a Hidden node, in KernelBitmapRef::Attach");

#if !defined(EXCLUDE_FROM_RALPH)
	if (InternalClipboard::CopyInProgress())
	{
		// Bit of a bodge this !!
		// If there is a clipboard copy op in progress, 
		// then we won't try and attach this to any lists.

		// Neville 12/8/97
		// Unfortunately, this leads to a memory leak for each bitmap that is copied
		// to the clipboard. At this point BitmapDoc is NULL and current doc is set to
		// the source doc. Hence, we cannot use the proper code and add it to the clipboard's
		// bitmap list.

		NewBitmap = new KernelBitmap(NewBitmap->ActualBitmap);
		TRACEUSER( "Neville", _T("ClipBoard Bmp at %x\n"), NewBitmap );
		m_pTheBitmap = NewBitmap;
		return;
	}
#endif

	// Whenever we attach or re-attach to the document bitmap list
	// we need to ensure that we have valid bitmap data
	NewBitmap->GetActualBitmap()->RebuildXPEBitmap();

	if (BitmapDoc == NULL)
		BitmapDoc = Document::GetCurrent();

	if (BitmapDoc == NULL)
		BitmapDoc = Document::GetSelected();

	BitmapList* ExistingList = NewBitmap->GetParentBitmapList();

	if (ExistingList == NULL || ExistingList->GetParentDocument() != BitmapDoc)
	{
		// Either the bitmap is not in a list at all, or it's in a list
		// in another document.  So ...

PORTNOTE("other","Removed IXMLDOMDocumentPtr usage")
#ifndef EXCLUDE_FROM_XARALX
		IXMLDOMDocumentPtr pEditInfo = NULL;
//		KernelBitmap* pMasterBitmap = NULL;
		KernelBitmap* pNewMasterBitmap = NULL;
//		NewBitmap->GetXPEInfo(pMasterBitmap, pEditInfo);
		// Can't call KernelBitmap->GetXPEInfo because it searches for a Master bitmap in the doc list
		// that matches the oil master bitmap. But here we know that that will always fail
		OILBitmap* pMasterOil = NULL;
		NewBitmap->ActualBitmap->GetXPEInfo(pMasterOil, pEditInfo);
		if (pMasterOil!=NULL && pEditInfo!=NULL)
		{
			// Need to copy the Master into the new doc as well!
			// ... we'll have to make a copy to attach to this document's list
			pNewMasterBitmap = KernelBitmap::MakeKernelBitmap(pMasterOil, BitmapDoc);

			// Get the bitmap list from the document
			BitmapList* BmpList = NULL;
			if (BitmapDoc)
				BmpList = BitmapDoc->GetBitmapList();

			// and then attach the bitmap (doesn't matter if BmpList is NULL)
			pNewMasterBitmap->Attach(BmpList);
		}
#endif

		//------------------------------------
		// ... we'll have to make a copy to attach to this document's list
		NewBitmap = KernelBitmap::MakeKernelBitmap(NewBitmap->ActualBitmap, BitmapDoc);

		// Get the bitmap list from the document
		BitmapList* BmpList = NULL;

		if (BitmapDoc)
			BmpList = BitmapDoc->GetBitmapList();

		// and then attach the bitmap (doesn't matter if BmpList is NULL)
		NewBitmap->Attach(BmpList);
		//------------------------------------


		// Make sure the new bitmap refers to the new master...
PORTNOTE("other","Removed XPEInfo usage")
#ifndef EXCLUDE_FROM_XARALX
		if (pNewMasterBitmap)
		{
			NewBitmap->SetXPEInfo(pNewMasterBitmap, pEditInfo);
		}
#endif
	}

	// Remember the bitmap pointer
	m_pTheBitmap = NewBitmap;
}

/********************************************************************************************

>	void KernelBitmapRef::SetBitmap(KernelBitmap* NewBmp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Purpose:	Sets the reference to a bitmap.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void KernelBitmapRef::SetBitmap(KernelBitmap* NewBmp)
{
//	ERROR3IF(m_pTheBitmap != NULL, "Warning, overwriting bitmap ref in KernelBitmapRef::SetBitmap");
	
	// Remember the bitmap pointer
	m_pTheBitmap = NewBmp;
}

/********************************************************************************************

>	void KernelBitmapRef::Detach(BOOL bTryRemoveFromDoc = FALSE)
	
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Purpose:	Removes the reference to a bitmap.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void KernelBitmapRef::Detach(BOOL bTryRemoveFromDoc)
{
	if (bTryRemoveFromDoc)
	{
		KernelBitmap* tempBitmap = m_pTheBitmap;
		Document* pBitmapDoc = NULL;

		pBitmapDoc = Document::GetCurrent();
		if (pBitmapDoc == NULL)
			pBitmapDoc = Document::GetSelected();

		m_pTheBitmap = NULL;

		if (tempBitmap && pBitmapDoc && !tempBitmap->IsUsedInDocument(pBitmapDoc))
		{
			tempBitmap->Detach();
			BROADCAST_TO_ALL(BitmapListChangedMsg(tempBitmap->GetParentBitmapList(), tempBitmap));
		}
	}
	else
		m_pTheBitmap = NULL;
}

/********************************************************************************************

>	void KernelBitmapRef::DeleteBmp()
	
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		-
	Outputs:	-
	Purpose:	Deletes the bitmap we are referencing.

********************************************************************************************/

void KernelBitmapRef::DeleteBmp()
{
	if (m_pTheBitmap!=NULL)
	{
		delete m_pTheBitmap;
		m_pTheBitmap=NULL;
	}
}

/********************************************************************************************

>	void KernelBitmapRef::RemoveFromTree()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Purpose:	Called when the object containing this reference is removed from the
				document tree.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void KernelBitmapRef::RemoveFromTree()
{
	if (m_pTheBitmap == NULL)
		return;

	TRACEUSER( "Neville", _T("Removing Bitmap Ref from tree\n") );
	m_bHidden = TRUE;

	m_pTheBitmap->Detach();
	BROADCAST_TO_ALL(BitmapListChangedMsg(m_pTheBitmap->GetParentBitmapList(), m_pTheBitmap));
}

/********************************************************************************************

>	void KernelBitmapRef::AddtoTree()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Purpose:	Called when the object containing this reference is added to the
				document tree.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void KernelBitmapRef::AddtoTree()
{
	if (m_pTheBitmap == NULL)
		return;

	TRACEUSER( "Neville", _T("Adding Bitmap Ref to tree\n") );
	m_bHidden = FALSE;

	m_pTheBitmap->Attach();
	BROADCAST_TO_ALL(BitmapListChangedMsg(m_pTheBitmap->GetParentBitmapList(), m_pTheBitmap));
}




