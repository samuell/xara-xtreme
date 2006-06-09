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
// BmpPrefs.cpp
//
// This file implements the dialogue box that allows the user to set the export bitmaps
// preferences.

/*
*/

#include "camtypes.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "page.h"
#include "layer.h"
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "dlgtypes.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h"
#include "bmpprefs.h"
//#include "bmpres.h"		// general bitmap filter based resources
//#include "accures.h"	// general Accusoft dll resources
//#include "simon.h"
//#include "dialogop.h"	// DialogOp header - in camtypes.h [AUTOMATICALLY REMOVED]
#include "dlgmgr.h"		// Dialog manager class
//#include "msg.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rikdlg.h"		// _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER)
#include "osrndrgn.h"
#include "grndrgn.h"	// for max bitmap width function
#include "bitfilt.h"	// BaseBitmapFilter 
//#include "justin2.h"
//#include "units.h"		// for pixel units - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "will3.h"
//#include "reshlpid.h"
#include "makebmp.h"
#include "helpuser.h"
//#include "accuflts.h"	// PNG and TI-GIFF filter types
//#include "xshelpid.h"
//#include "filtrres.h"	// _R(IDS_BMPPREFS_DPITOOSMALL) _R(IDS_BMPPREFS_DPITOOBIG)
#include "exjpeg.h"
#include "bmpfiltr.h"
#include "pngfiltr.h"
#ifndef WEBSTER
//#include "extfilts.h"
#endif //WEBSTER
//#include "simon.h"
#include "impexpop.h"	// BitmapExportParam
#include "fileutil.h"
//#include "bmpsdlg.h"	// BmpDlgParam
//#include "cxfrec.h"		// for CXaraFileRecord - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ngprop.h"		// for the NamedExportProp::m_fApplyNotExport flag.
//#include "bmpreres.h"	// for _R(IDS_PREVIEW_APPLY)
//#include "registry.h"
//#include "desnotes.h"
#include "bmpalint.h"	// for BitmapExportPaletteInterface::InvalidateSortedPalette()
//#include "oilfltrs.h"	// for the find filter fn - in camtypes.h [AUTOMATICALLY REMOVED]
#include "slicehelper.h"
#include "fillattr2.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
PORTNOTE("other","Removed BmpPrefsDlg - derived from DialogOp")
#ifndef EXCLUDE_FROM_XARALX
CC_IMPLEMENT_DYNCREATE(BmpPrefsDlg, DialogOp)
CC_IMPLEMENT_DYNCREATE(JPEGExportPrefsDialog, BmpPrefsDlg)
CC_IMPLEMENT_DYNCREATE(PhotoCDDlg, DialogOp)
#endif

CC_IMPLEMENT_DYNCREATE(BitmapExportOptions, OpParam)
CC_IMPLEMENT_MEMDUMP(PhotoCDDlgParam, OpParam)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW
#define	OPTION_ONE 0
#define	OPTION_FOUR 4

TCHAR OPTOKEN_JPGPREFSDLG[] = _T("JPGPrefsDlg");

/********************************************************************************************

	Preference:	DefaultExportFilterPath
	Section:	Filters
	Range:		Any old rubbish really
	Purpose:	The path to default to when opening the Export File dialog

********************************************************************************************/ 
// String_256 BitmapExportOptions::g_DefaultPath;

/********************************************************************************************

	Preference:	HTMLToClipboard
	Section:	Filters
	Range:		FALSE, ranging all the way to TRUE
	Purpose:	Whether we should put an HTML tag on the clipboard. FALSE by default.

********************************************************************************************/ 
BOOL BitmapExportOptions::ms_fPutHTMLTagOnClipboard = FALSE;
TCHAR BitmapExportOptions::ms_strPutHTMLTagOnClipboard[]
											= _T("Filters\\PutHTMLTagOnClipboard");
BOOL BitmapExportOptions::ms_IsBackGroundTransparent = TRUE;
ANTIALIASING BitmapExportOptions::ms_Antialiasing = MINIMISE_VISIBLE_AA;
BOOL BitmapExportOptions::ms_bSeparateLayerFiles = FALSE;



/********************************************************************************************
>	BitmapExportOptions::BitmapExportOptions()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Default constructor.
	See Also:	BaseBitmapFilter::CreateBitmapOptions()
********************************************************************************************/

BitmapExportOptions::BitmapExportOptions() :
	m_SelectionType(SELECTION),			// BaseBitmapFilter uses these...
	m_OutputSize(0, 0),					// And initialise the output size
	m_PixelOutputSize(0, 0),
	m_FilterID(INVALID),
	m_pBmpDlgParam(0),
	m_bSeparateLayerFiles(FALSE),
	m_bTempFileFlag(FALSE),
	m_NumColsInPalette(0),
	m_UseSystemColours(FALSE),
	m_fPutHTMLTagOnClipboard(ms_fPutHTMLTagOnClipboard), // default HTML to clipboard
	m_bValid(FALSE),					// Always use this...
	m_DPI(0),
	m_Depth(0),
	m_TransparencyIndex(-1),			// WEBSTER - markn 5/2/97 - no transparent colour
	m_DialogID(0),						// BmpPrefsDlg uses these...
	m_pFilterName(0),
	m_bTempFileExported(FALSE),			// export preview uses this
	m_Antialiasing(MINIMISE_VISIBLE_AA)	// Always start with the highest quality option
{
	m_Palette.NumberOfColours = 0;
	memset(&(m_Palette.Data), 0, sizeof(ExtendedPaletteEntry)*256);
PORTNOTE("DesignNotes", "Removed use of DesignNotes")
#if !defined(EXCLUDE_FROM_XARALX)
	m_UseDesignNotes = GetRegistryFlagForDesignNotes();
#else
	m_UseDesignNotes = FALSE;
#endif
	m__pLogicalPalette = NULL;
	m__NumberOfColoursUserRequested = 256;
	m__UseBrowserPalette = FALSE;
	m__UseSystemColoursInPalette = FALSE;
	m__UseWebSnapPalette = 0;
	m__UsePrimarySnapPalette = 1;
	m__PaletteIsValid = FALSE;
	m__BackgroundIsTransparent = TRUE;
	m__pPaletteOptimiser = NULL;
	m__HavePrimedOptimier = FALSE;
	m__TempFileMatchesExportOptions = FALSE;
	m_bCMYK = FALSE;
}



/********************************************************************************************
>	BitmapExportOptions::BitmapExportOptions(const CDlgResID DialogID, 
										const FilterType FilterID, StringBase* pFilterName)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Allows options in the bitmap export dialog to be freely passed around
	See Also:	BaseBitmapFilter::CreateBitmapOptions()
********************************************************************************************/

BitmapExportOptions::BitmapExportOptions(
								const CDlgResID DialogID, 
								const FilterType FilterID,
								const StringBase* pFilterName) :
	m_SelectionType(SELECTION),			// BaseBitmapFilter uses these...
	m_OutputSize(0, 0),					// And initialise the output size
	m_PixelOutputSize(0, 0),
	m_FilterID(FilterID),
	m_pBmpDlgParam(0),
	m_bSeparateLayerFiles(FALSE),
	m_bTempFileFlag(FALSE),
	m_NumColsInPalette(0),
	m_UseSystemColours(FALSE),
	m_fPutHTMLTagOnClipboard(ms_fPutHTMLTagOnClipboard), // default HTML to clipboard
	m_bValid(FALSE),					// Always use this...
	m_DPI(0),
	m_Depth(0),
	m_TransparencyIndex(-1),			// WEBsTER - markn 5/2/97 - no transparent colour
	m_DialogID(DialogID),				// BmpPrefsDlg uses these...
	m_pFilterName(pFilterName),
	m_bTempFileExported(FALSE),			// export preview uses this
	m_Antialiasing(MINIMISE_VISIBLE_AA)	// Always start with the highest quality option
{
	m_Palette.NumberOfColours = 0;
	memset(&(m_Palette.Data), 0, sizeof(ExtendedPaletteEntry)*256);
PORTNOTE("DesignNotes", "Removed use of DesignNotes")
#if !defined(EXCLUDE_FROM_XARALX)
	m_UseDesignNotes = GetRegistryFlagForDesignNotes();
#else
	m_UseDesignNotes = FALSE;
#endif
	m__pLogicalPalette = NULL;
	m__NumberOfColoursUserRequested = 256;
	m__UseBrowserPalette = FALSE;
	m__UseSystemColoursInPalette = FALSE;
	m__UseWebSnapPalette = 0;
	m__UsePrimarySnapPalette = 1;
	m__PaletteIsValid = FALSE;
	m__BackgroundIsTransparent = TRUE;
	m__pPaletteOptimiser = NULL;
	m__HavePrimedOptimier = FALSE;
	m__TempFileMatchesExportOptions = FALSE;
	m_bCMYK = FALSE;
}

// distructor to release any claimed memory
BitmapExportOptions::~BitmapExportOptions()
{
	// the export options manages the logical palette so this is deleted when the export options are deleted
	if (m__pLogicalPalette)
	{
		delete m__pLogicalPalette;
		m__pLogicalPalette = NULL;
	}

	// the cache for the palette optimiser which holds the stats on the image
	if (m__pPaletteOptimiser)
	{
		delete m__pPaletteOptimiser;
		m__pPaletteOptimiser = NULL;
	}
}

/********************************************************************************************
>	BitmapExportOptions* BitmapExportOptions::MakeCopy()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/97
	Purpose:	Makes a copy object of this object
	See Also:	BitmapExportOptions::CopyFrom()
********************************************************************************************/

BitmapExportOptions* BitmapExportOptions::MakeCopy()
{
	// Get the runtime class info on this object
	CCRuntimeClass* pCCRuntimeClass = GetRuntimeClass();

	// Create another object of the same type
	BitmapExportOptions* p = (BitmapExportOptions*) pCCRuntimeClass->CreateObject();
	if (p != 0)
	{
		// clear the exported file flag to prevent attempts to delete any temp files
		p->m_bTempFileExported = FALSE;

		// copy the contents accross
		p->CopyFrom(this);
	}
	
	return p;
}



/********************************************************************************************
>	virtual BOOL BitmapExportOptions::CopyFrom(BitmapExportOptions* pOther) 
	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/97
	Inputs:		pOther - the other BitmapExportOptions object to copy the data from
	Purpose:	Sets the contents of this object from the passed object
	See Also:	BitmapExportOptions::MakeCopy()
	Returns:	TRUE if pOther is of the same type as 'this' else FALSE (this is
				a silly thing to return but the functions in derived classes kind of
				require this now so it is a bit difficult to change)
********************************************************************************************/
BOOL BitmapExportOptions::CopyFrom(BitmapExportOptions* pOther) 
{
	if (pOther == 0)
		return FALSE;

	// Copy all the data items that can be copied for any type of BitmapExportOptions
	
	m_SelectionType					= pOther->m_SelectionType;
	m_fPutHTMLTagOnClipboard		= pOther->m_fPutHTMLTagOnClipboard;
	m_Antialiasing					= pOther->m_Antialiasing;
	m_UseDesignNotes				= pOther->m_UseDesignNotes;
	m_bSeparateLayerFiles			= pOther->m_bSeparateLayerFiles;
	m_bTempFileFlag					= pOther->m_bTempFileFlag;


	if (IS_SAME_CLASS(pOther, this))
	{
		// Only copy these if we are copying from a class that is of the
		// same type
		
		m_OutputSize				= pOther->m_OutputSize;
		m_PixelOutputSize			= pOther->m_PixelOutputSize;
		m_FilterID					= pOther->m_FilterID;
		m_pBmpDlgParam				= pOther->m_pBmpDlgParam;		
		m_NumColsInPalette			= pOther->m_NumColsInPalette;
		m_UseSystemColours			= pOther->m_UseSystemColours;
		m_TransparencyIndex			= pOther->m_TransparencyIndex;
		m_DialogID					= pOther->m_DialogID;
		m_pFilterName				= pOther->m_pFilterName;
		m_bValid					= pOther->m_bValid;
		m_bTempFileExported			= pOther->m_bTempFileExported;
		m_TempPath					= pOther->m_TempPath;
		m_Depth						= pOther->m_Depth; // move to derived classes

		// Added to stop the name gallery killing itself on export
		m_DPI						= pOther->m_DPI;

		// It should be safe to copy the following for any file type but I have not tested this
		// so I am only copying them if they are of the same type for now - Jonathan.

		// smfix new ones added
		m__NumberOfColoursUserRequested	= pOther->m__NumberOfColoursUserRequested;
		m__UseBrowserPalette			= pOther->m__UseBrowserPalette;
		m__UseSystemColoursInPalette	= pOther->m__UseSystemColoursInPalette;
		m__UseWebSnapPalette			= pOther->m__UseWebSnapPalette;
		m__UsePrimarySnapPalette		= pOther->m__UsePrimarySnapPalette;
		m__BackgroundIsTransparent		= pOther->m__BackgroundIsTransparent;

		// Also need to copy all the palette information over.  It should also be safe to copy
		// this but again, I have not tested this so I am going for the safe option - Jonathan.
		CopyPaletteInformationFrom(pOther);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
};



/******************************************************************************************

>	BOOL BitmapExportOptions::FileTypeChangeCopyFrom(BitmapExportOptions *pOther)

	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>

	Created:	15/10/99

	Inputs:		pOther - A BitmapExportOptions object we want to make a copy of.

	Returns:	-

	Purpose:	Copies the information from the BitmapExportOptions object
				passed in as the argument to the palette object in this class.

******************************************************************************************/

BOOL BitmapExportOptions::FileTypeChangeCopyFrom(BitmapExportOptions *pOther)
{
	m_SelectionType		= pOther->m_SelectionType;
	m_OutputSize		= pOther->m_OutputSize;
	m_PixelOutputSize	= pOther->m_PixelOutputSize;
	m_pBmpDlgParam		= pOther->m_pBmpDlgParam;		
	m_NumColsInPalette	= pOther->m_NumColsInPalette;
	m_UseSystemColours	= pOther->m_UseSystemColours;
	m_DPI				= pOther->m_DPI;
	m_Depth				= pOther->m_Depth;
	m_TransparencyIndex = pOther->m_TransparencyIndex;
	m_DialogID			= pOther->m_DialogID;
	m_bValid			= pOther->m_bValid;
	m_bTempFileExported = pOther->m_bTempFileExported;
	m_TempPath			= pOther->m_TempPath;
	m_fPutHTMLTagOnClipboard	= pOther->m_fPutHTMLTagOnClipboard;
	m_Antialiasing		= pOther->m_Antialiasing;
	m_UseDesignNotes	= pOther->m_UseDesignNotes;
	m_bSeparateLayerFiles = pOther->m_bSeparateLayerFiles;
	m_bTempFileFlag		= pOther->m_bTempFileFlag;

	// smfix new ones added
	m__NumberOfColoursUserRequested =	pOther->m__NumberOfColoursUserRequested;
	m__UseBrowserPalette =				pOther->m__UseBrowserPalette;
	m__UseSystemColoursInPalette =		pOther->m__UseSystemColoursInPalette;
	m__UseWebSnapPalette =				pOther->m__UseWebSnapPalette;
	m__UsePrimarySnapPalette =			pOther->m__UsePrimarySnapPalette;
	m__BackgroundIsTransparent =		pOther->m__BackgroundIsTransparent;

	// Also need to copy all the palette information over.
	CopyPaletteInformationFrom( pOther );

	return TRUE;
}


/******************************************************************************************
>	void BitmapExportOptions::CopyPaletteInformationFrom( BitmapExportOptions* pOther )
	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/99
	Inputs:		pOther - A BitmapExportOptions object we want to make a copy of.
	Purpose:	Copies the palette information from the BitmapExportOptions object
				passed in as the argument to the palette object in this class.
******************************************************************************************/
void BitmapExportOptions::CopyPaletteInformationFrom( BitmapExportOptions* pOther )
{
	//  The first few times that this function is called, the palette passed in
	//  is uninitialised. If this is the case, then just make our palette a default one.
	if( ( pOther->m_Palette.NumberOfColours > 256 ) || ( pOther->m_Palette.NumberOfColours < 0 ) )
	{
		m_Palette.NumberOfColours = 0;

		INT32 i = 0;
		for( i=0; i < 256; i++ )
		{
			m_Palette.Data[ i ].Red					= 0;
			m_Palette.Data[ i ].Green				= 0;
			m_Palette.Data[ i ].Blue				= 0;
			m_Palette.Data[ i ].Flags				= 0;
		}
		//  Finished here, so return.
		return;
	}

	//  Simply copy the entire palette over from pOther.
	m_Palette.NumberOfColours	= pOther->m_Palette.NumberOfColours;

	INT32 i = 0;
	INT32 Colours = m_Palette.NumberOfColours;
	Colours = max( 256, Colours );
	for( i=0; i < Colours; i++ )
	{
		m_Palette.Data[ i ].Red					= ( pOther->m_Palette ).Data[ i ].Red;
		m_Palette.Data[ i ].Green				= ( pOther->m_Palette ).Data[ i ].Green;
		m_Palette.Data[ i ].Blue				= ( pOther->m_Palette ).Data[ i ].Blue;
		m_Palette.Data[ i ].Flags				= ( pOther->m_Palette ).Data[ i ].Flags;
	}

	// delete our old logical palette if we had one
	if (m__pLogicalPalette)
	{
		delete m__pLogicalPalette;
		m__pLogicalPalette = NULL;
	}

	// our copy is valid if the original is valid
	m__PaletteIsValid = pOther->IsPaletteValid();

	// copy the logical palette data if it was valid
	if (m__PaletteIsValid)
	{
		m__pLogicalPalette = (LOGPALETTE * ) new BYTE[sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * 256 )];

		memcpy(m__pLogicalPalette, pOther->GetLogicalPalette(), sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * 256 ));
	}

}



/******************************************************************************************
>	ExtendedPalette& BitmapExportOptions::GetPalette()
	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/99
	Returns:	The palette object of this class.
	Purpose:	Gives the calling function a copy of this class's palette.
******************************************************************************************/
ExtendedPalette* BitmapExportOptions::GetPalette()
{
	return &m_Palette;
}


/********************************************************************************************

>	BOOL BitmapExportOptions::IsValid() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	TRUE if this BitmapExportOptions object is valid for use in a subsequent
				export operation.
				FALSE otherwise
	Purpose:	Determines whether or not the values contained in this object are valid or
				whether the user has cancelled the export: using MarkValid() & MarkInvalid()
				will state the veracity of this fact.

********************************************************************************************/
BOOL BitmapExportOptions::IsValid() const
{
	return m_bValid;
}


void BitmapExportOptions::MarkInvalid()
{
	m_bValid = FALSE;
}


void BitmapExportOptions::MarkValid()
{
	m_bValid = TRUE;
}


/********************************************************************************************

>	virtual BOOL BitmapExportOptions::RetrieveDefaults()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	TRUE if successful
				FALSE otherwise
	Purpose:	Retrieves defaults for accessible members from preferences 
	See Also:	BitmapExportOptions::SetAsDefaults()

********************************************************************************************/
BOOL BitmapExportOptions::RetrieveDefaults()
{
	SetDPI(BMPFilter::GetDefaultExportDPI());
	SetDepth(BMPFilter::GetDefaultExportDepth());

	SetPutHTMLTagOnClipboard(ms_fPutHTMLTagOnClipboard);
	m__BackgroundIsTransparent = ms_IsBackGroundTransparent;
	m_Antialiasing = ms_Antialiasing;
	m_bSeparateLayerFiles = ms_bSeparateLayerFiles;

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL BitmapExportInfo::SetAsDefaults() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Sets the current options as preferences which may be retrieved by a
				subsequent RetrieveDefaults()
	See Also:	BitmapExportOptions::RetrieveDefaults()

********************************************************************************************/
BOOL BitmapExportOptions::SetAsDefaults() const
{
	BMPFilter::SetDefaultExportDPI(GetDPI());
	BMPFilter::SetDefaultExportDepth(GetDepth());

	ms_fPutHTMLTagOnClipboard=m_fPutHTMLTagOnClipboard;
	ms_IsBackGroundTransparent = m__BackgroundIsTransparent;
	ms_Antialiasing = m_Antialiasing;
	ms_bSeparateLayerFiles = m_bSeparateLayerFiles;

	return TRUE;
}

/********************************************************************************************

>	static BOOL BitmapExportOptions::Declare()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/97
	Returns:	TRUE if successfully declared preferences
				FALSE otherwise
				Purpose:	Declares our base class preferences

********************************************************************************************/
BOOL BitmapExportOptions::Declare()
{
	if (Camelot.DeclareSection(_T("Filters"), 10))
	{
		Camelot.DeclarePref( NULL, ms_strPutHTMLTagOnClipboard, &ms_fPutHTMLTagOnClipboard, FALSE, TRUE );
	}

	return TRUE;
}



/********************************************************************************************
>	virtual BOOL BitmapExportOptions::Write(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/00
	Inputs:		pRec	---		record to write
	Returns:	TRUE if successful.
	Purpose:	Writes this object to the given record.
	SeeAlso:	BitmapExportOptions::Read
********************************************************************************************/

BOOL BitmapExportOptions::Write(CXaraFileRecord* pRec)
{
	INT16 Packed = 0x0000;

	if (m_Antialiasing) Packed += 1;
	if (m__BackgroundIsTransparent) Packed += 2;
	if (m__UseBrowserPalette) Packed += 4;

	INT16 AmountOfPrimarySnap = (m__UsePrimarySnapPalette & 15) << 4;
	Packed += AmountOfPrimarySnap;

	INT16 AmountOfWebSnap = (m__UseWebSnapPalette & 0xff) << 8;
	Packed += AmountOfWebSnap;

	UINT32 PackedRequestedColours = m__NumberOfColoursUserRequested;

	// Write out every user-definable data member.  Note that as we save only
	// the lowest byte of a bool, we double-not them to ensure they are saved
	// out only as either 0 or 1.
	if (!pRec->WriteINT16((INT16) m_SelectionType) ||
		!pRec->WriteCoord((const DocCoord&) m_OutputSize) ||
		!pRec->WriteCoord((const DocCoord&) m_PixelOutputSize) ||
		!pRec->WriteUINT32((UINT32) m_FilterID) ||
		!pRec->WriteBYTE((BYTE) !!m_UseDesignNotes) ||
		!pRec->WriteUINT32(PackedRequestedColours) ||
		!pRec->WriteBYTE((BYTE) !!m__UseSystemColoursInPalette) ||
		!pRec->WriteBYTE((BYTE) !!m_fPutHTMLTagOnClipboard) ||
		!pRec->WriteDOUBLE((double) m_DPI) ||
		!pRec->WriteUINT32((UINT32) m_Depth) ||
		!pRec->WriteINT32(m_TransparencyIndex) ||
		!pRec->WriteUINT32((UINT32) m_DialogID) ||
		!pRec->WriteINT16(Packed) || // was m_Antialiasing but I have now packed this data to fit in the new stuff
		!pRec->WriteINT32(m_Palette.NumberOfColours) ||
		!pRec->WriteUnicode(m_pFilterName ? ((LPTSTR) (LPCTSTR) *m_pFilterName) : (LPTSTR) _T("")))
			return FALSE;

	// Write out the palette.
	BYTE Dummy = 0;
	ExtendedPaletteEntry* peEnd = &m_Palette.Data[min(m_Palette.NumberOfColours, 256)];
	for (ExtendedPaletteEntry* pe = m_Palette.Data; pe < peEnd; pe++)
	{
		DWORD PackedPreEditedColours = pe->PreEditedRed + (pe->PreEditedGreen << 8) + (pe->PreEditedBlue << 16);
		if (!pRec->WriteBYTE(pe->Red) ||
			!pRec->WriteBYTE(pe->Green) ||
			!pRec->WriteBYTE(pe->Blue) ||
			!pRec->WriteBYTE(Dummy) ||
			!pRec->WriteINT32((INT32)PackedPreEditedColours) || // used to be popularity but there should be no clash
			!pRec->WriteINT32(pe->Flags))
				return FALSE;
	}

	return TRUE;
}



/********************************************************************************************
>	virtual BOOL BitmapExportOptions::Read(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/00
	Inputs:		pRec	---		record to read
	Returns:	TRUE if successful.
	Purpose:	Reads this object from the given record.
	SeeAlso:	BitmapExportOptions::Write
********************************************************************************************/

BOOL BitmapExportOptions::Read(CXaraFileRecord* pRec)
{
	// Make sure these bools are preset to zero as we only saved their
	// least significant byte.
	m_UseDesignNotes = m_UseSystemColours = m_fPutHTMLTagOnClipboard = FALSE;

	INT16 Packed = 0;
	UINT32 PackedRequestedColours = 0;

	// Read in every user-definable data member.
	String_256 strFilterName;
	if (!pRec->ReadINT16((INT16*) &m_SelectionType) ||
		!pRec->ReadCoord((DocCoord*) &m_OutputSize) ||
		!pRec->ReadCoord((DocCoord*) &m_PixelOutputSize) ||
		!pRec->ReadUINT32((UINT32*) &m_FilterID) ||
		!pRec->ReadBYTEtoBOOL(&m_UseDesignNotes) ||
		!pRec->ReadUINT32((UINT32*) &PackedRequestedColours) ||
		!pRec->ReadBYTEtoBOOL(&m__UseSystemColoursInPalette) ||
		!pRec->ReadBYTEtoBOOL(&m_fPutHTMLTagOnClipboard) ||
		!pRec->ReadDOUBLE((double*) &m_DPI) ||
		!pRec->ReadUINT32((UINT32*) &m_Depth) ||
		!pRec->ReadINT32(&m_TransparencyIndex) ||
		!pRec->ReadUINT32((UINT32*) &m_DialogID) ||
		!pRec->ReadINT16((INT16*) &Packed) ||
		!pRec->ReadINT32((INT32*) &m_Palette.NumberOfColours) ||
		!pRec->ReadUnicode(&strFilterName))
			return FALSE;

	// unpack the packed varibale to get the smfix data
	if (Packed & 1)
		m_Antialiasing = MINIMISE_VISIBLE_AA;
	else
		m_Antialiasing = MAINTAIN_SCREEN_AA;

	m__BackgroundIsTransparent = Packed & 2;
	m__UseBrowserPalette = Packed & 4;
	m__UsePrimarySnapPalette = (Packed >> 4) & 15;
	m__UseWebSnapPalette = (Packed >> 8) & 0xff;

	m__NumberOfColoursUserRequested = PackedRequestedColours;

	// Read in the palette.
	BYTE Dummy = 0;
	INT32 DummyPopularity = 0;
	ExtendedPaletteEntry* peEnd = &m_Palette.Data[min(m_Palette.NumberOfColours, 256)];
	for (ExtendedPaletteEntry* pe = m_Palette.Data; pe < peEnd; pe++)
	{
		if (!pRec->ReadBYTE(&pe->Red) ||
			!pRec->ReadBYTE(&pe->Green) ||
			!pRec->ReadBYTE(&pe->Blue) ||
			!pRec->ReadBYTE(&Dummy) ||
			!pRec->ReadINT32((INT32*) &DummyPopularity) ||
			!pRec->ReadINT32((INT32*) &pe->Flags))
				return FALSE;

		// unpack the pre-edited colours
		DWORD PackedPreEditedColours = DummyPopularity;
		pe->PreEditedRed = (PackedPreEditedColours && 0xff);
		pe->PreEditedGreen = ((PackedPreEditedColours >> 8) && 0xff);
		pe->PreEditedBlue = ((PackedPreEditedColours >> 16) && 0xff);
	}

	if (m_DPI == 0)
	{
		TRACE( _T("DPI not set in export options in the .xar file - setting it to 96.0\n"));
		m_DPI = 96.0;
	}


	// Fix up the filter name string (for some brain-damaged reason this bears
	// little relation to the saved-out filter ID).
	if (strFilterName.IsEmpty())
		strFilterName.Load(GetFilterNameStrID());

	for (Filter* pf = Filter::GetFirst(); pf != 0; pf = Filter::GetNext(pf))
		if (pf->FilterName == strFilterName)
		{
			// Success.
			m_pFilterName = &pf->FilterName;
			return TRUE;
		}
	
	// Couldn't find the filter with the given name.
	TRACE( _T("BitmapExportOptions::Read: can't find %s filter\n"), (LPCTSTR) strFilterName);
	return FALSE;
}



/********************************************************************************************
>	const StringBase* BitmapExportOptions::GetFilterName() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	The name of the filter associated with this BitmapExportOptions object
	Purpose:	Allows the export dialog to display the filter name in its title bar
********************************************************************************************/

const StringBase* BitmapExportOptions::GetFilterName() const
{
	return m_pFilterName;
}


/********************************************************************************************

  >	void BitmapExportOptions::SetPathName(PathName *pTempPath)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/96
	Inputs:		pTempPath - path name of a temporary file
	Returns:	None
	Purpose:	Allows the export options to keep the path to a temporary file, created 
				during bitmap preview, so the export function can simply rename this to 
				create the final exported file.

********************************************************************************************/
void BitmapExportOptions::SetPathName(PathName *pTempPath)
{
	// set the flag, to indicate that we have a temp file, but only if the path name is not null
	m_bTempFileExported = (pTempPath != NULL);

	if (m_bTempFileExported)
	{
		// remember the new path
		m_TempPath = *pTempPath;
	}
}

	
/********************************************************************************************

>	BOOL BitmapExportOptions::SetDPI(const DPI& Dpi)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		Dpi : The dpi of the bitmap to be exported
	Returns:	TRUE if the operation was successful
				FALSE otherwise
	Purpose:	Support function to set up information for the ensuing export
	See Also:	GetDpi()

********************************************************************************************/
BOOL BitmapExportOptions::SetDPI(const DPI& Dpi)
{
	if (m_DPI != Dpi)
		BitmapSourceHasChanged(); // flush cache

	m_DPI = Dpi;
	return TRUE;
}


/********************************************************************************************

>	DPI BitmapExportOptions::GetDPI() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	The desired dpi of the bitmap to export
	Purpose:	Support function to obtain bitmap info

********************************************************************************************/
DPI BitmapExportOptions::GetDPI() const
{
	return m_DPI;
}


/********************************************************************************************

>	void BitmapExportOptions::SetSelectionType(const SelectionType& Sel)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	long while ago - modified sjk 10/12/00
	Purpose:	sets the the item(s) being exported eg selection, page, drawing, etc

********************************************************************************************/
void BitmapExportOptions::SetSelectionType(const SelectionType& Sel)
{
	if (m_SelectionType != Sel)
		BitmapSourceHasChanged(); // flush cache
	m_SelectionType = Sel;
}

/********************************************************************************************

>	void BitmapExportOptions::SetAntiAliasing( ANTIALIASING aa )

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	long while ago - modified sjk 10/12/00
	Purpose:	Sets the anti-aliasing on the export bitmap

********************************************************************************************/
void BitmapExportOptions::SetAntiAliasing( ANTIALIASING aa )
{
	if (m_Antialiasing != aa)
		BitmapSourceHasChanged(); // flush cache
	m_Antialiasing = aa;
}


/********************************************************************************************

>	BMP_DEPTH BitmapExportOptions::GetDepth() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	The desired depth of the bitmap to export
	Purpose:	Support function to obtain bitmap options

********************************************************************************************/
BMP_DEPTH BitmapExportOptions::GetDepth() const
{
	///// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! /////
	// THIS IS JUST FOR TESTING! WHEN THE BITMAP COPY DIALOG GETS IT`s OWN 32BPP BUTTON
	// THEN THIS MUST BE REMOVED!!!!!!!
	//  I've change '>1' to '>=1' below, otherwise the 2 colour optimised palette
	//  is not available.
	if( m_Depth >= 1 )
	{
		return m_Depth;
	}
	else
	{
		return 32;
	}
}


/********************************************************************************************

>	BOOL BitmapExportOptions::SetDepth(const BMP_DEPTH& Depth)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		Depth : The depth of the bitmap to be exported
	Returns:	TRUE if the operation was successful
				FALSE otherwise
	Purpose:	Support function to set up information for the ensuing export
	See Also:	GetDepth()

********************************************************************************************/
BOOL BitmapExportOptions::SetDepth(const BMP_DEPTH& Depth)
{
	if (m_Depth != Depth)
	{
		InvalidatePalette();

		// sjk bodge if we did have two colours moving up sometimes needs to regather the stats
		if (m_Depth == 1)
			BitmapSourceHasChanged();
	}

	m_Depth = Depth;
	return TRUE;
}


/********************************************************************************************

>	void BitmapExportOptions::SetBitmapExportParam(BitmapExportParam* pParam)
	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/96
	Inputs:		PParam - Allows us to to save a reference to a BmpDlgParam, in
				the BitmapExportOptions.
	Returns:	-
	Purpose:	-
	See Also:	-

********************************************************************************************/

BOOL BitmapExportOptions::SetBitmapExportParam(BitmapExportParam* pParam)
{
	ERROR2IF(pParam == NULL,FALSE,"pParam is NULL");
	m_pBmpDlgParam=pParam->GetBmpDlgParam();

	if(m_pBmpDlgParam == NULL)
		return FALSE;

	return TRUE;
}


BOOL BitmapExportOptions::IsClipToPage()
{
	if( m_SelectionType == PAGE )
		return TRUE;
	else
		return FALSE;
}


BOOL BitmapExportOptions::IsCMYK()
{
	return m_bCMYK;
}

void BitmapExportOptions::SetCMYK(BOOL SetIt)
{
	m_bCMYK = SetIt;
}


/********************************************************************************************

>	BOOL BitmapExportOptions::CanExportSeparateLayers()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/2004
	Inputs:		-
	Returns:	-
	Purpose:	-
	See Also:	-

********************************************************************************************/

BOOL BitmapExportOptions::CanExportSeparateLayers()
{
	return (GetSeparateLayerFiles() && GetTempFileFlag()==FALSE && (GetFilterNameStrID()==_R(IDN_FILTERNAME_GIF) || GetFilterNameStrID()==_R(IDS_FILTERNAME_PNG)));
}



PORTNOTE("other", "Removed export dlgs");
#if !defined(EXCLUDE_FROM_XARALX)

/*******************************************************************************************/
PhotoCDDlgParam * PhotoCDDlg::pParams = NULL;	// Data passing class

// Set up the dialog box details stored as statics in the class
CDlgResID BmpPrefsDlg::IDD = _R(IDD_EXPORTBMPOPTS);	// default dialog box id
const CDlgMode BmpPrefsDlg::Mode = MODAL;		// This dialog is modal

const CDlgMode PhotoCDDlg::Mode = MODAL;		// This dialog is modal

// Tiff compression types we can cope with. Cannot use the Accusoft types as these are not
// contiguous and hence would not correspond to items in a list
//enum TiffTypes {UNCOMPRESSED_TIFF, LZW_TIFF, HUFFMAN_TIFF, G3FAX_TIFF, G4FAX_TIFF,
//			    PACKBITS_TIFF, TWODENCODING_TIFF };

// Andy Hills, 05-09-00
// The above enum is only applicable for 1bpp TIFF export.
// The options are different for 4/8/24 bpp.

// The values correspond to the positions of the respective compression types
// in the drop-down list.
// This will have to make do until someone does a vast re-write of the 
// TIFF export options code!

// Note that LZW_TIFF and HUFFMAN_TIFF are at the same position:
// this is because they don't actually appear at the same time.
// For 1bpp, the options are { uncompressed, huffman, g3 ... }
// For 8ppp, the options are { uncompressed, lzw }

#define UNCOMPRESSED_TIFF 0
#define LZW_TIFF 1
#define HUFFMAN_TIFF 1
#define G3FAX_TIFF 2
#define G4FAX_TIFF 3
#define PACKBITS_TIFF 4
#define TWODENCODING_TIFF 5


// BODGE: We get some bad pixel cropping on some images if we use doubles. So limit the doubles to
// fixed values until these problems are fixed.
// Hence the conversion of GetDoubleGadgetValues into GetLongGadgetValues.
static const double D_MAXDPI = 3000.0;
static const INT32 MAXDPI = 3000;

// Height of extra bit at the bottom of the dialogue with the Animation options
static const INT32 ANIMATION_EXTRABIT  = 76;
static const INT32 NUMCOLS_EXTRABIT	  = 56;
static const INT32 NUMCOLS_GROUP_SHIFT = 80;

// WEBSTER - markn 24/1/97
// The number of system colours added when the "Add system colours" switch is on
// (28 because there are 27 Gavin primary colours (which includes most of the windows colour)
// plus one windows colour that isn't a Gavin primary)
static const INT32 NUM_SYS_COLS = 28;


/******************************************************************************************

>	BmpPrefsDlg::BmpPrefsDlg(): DialogOp(BmpPrefsDlg::IDD, BmpPrefsDlg::Mode) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/08/94
	Purpose:	BmpPrefsDlg constructor. Creates a non-undoable operation

******************************************************************************************/

BmpPrefsDlg::BmpPrefsDlg(): DialogOp(BmpPrefsDlg::IDD, BmpPrefsDlg::Mode) 
{
	m_pOptions = NULL;

	Width		= 0;
	Height		= 0;
	PixelWidth	= 0;
	PixelHeight	= 0;
	Dpi			= 96.0;

	LastBppSelected	= 0;

	RecalculatingPixels	= FALSE;
	RecalculatingXDpi	= FALSE;
	RecalculatingYDpi	= FALSE;

	m_bDpiSupported	= FALSE;
	m_bLockAspect	= FALSE;
	IsDelayMany = FALSE;			
	IsDelayDefault = FALSE;		
	HasDelayChanged = FALSE;
	HasRestoreChanged = FALSE;
	IsManyDisplayed = FALSE;
	DisplayMany = FALSE;
	LoopCheckBoxEnabled = TRUE;
}


/******************************************************************************************

>	SelectionType BmpPrefsDlg::GetSelection(BmpPrefsDlg* pBmpPrefs) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Inputs:		-
	Outputs:	-
	Returns:	Selection, Drawing or Spread.
	Purpose:	Works out what the current selection/spread/drawing state is.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

SelectionType BmpPrefsDlg::GetSelection()
{
	// Get the state of the selection, drawing or spread buttons
	BOOL Valid = FALSE;

	// If opening specification was a bitmap then nothing doing
	if (GetOptions()->GetSelectionType() == ABITMAP)
		return ABITMAP;

	if (GetOptions()->GetSelectionType() == SOMEBITMAPS)
		return SOMEBITMAPS;

	SelectionType FoundSelection = DRAWING;
	
	BOOL ExportDrawing = GetLongGadgetValue(_R(IDC_BMPOPTS_DRAWING), 0, 1, 0, &Valid);
	// Use the state on entry to dictate the possible output values.
	if (GetOptions()->GetSelectionType() == SELECTION)
	{
		// If selection present then choose between Selection or Drawing
		if (ExportDrawing)
			FoundSelection = DRAWING;
		else		
			FoundSelection = SELECTION;
	}
	else
	{
		// Graham 24/7/97: If no selection, then set DRAWING automatically
		FoundSelection = DRAWING;		
	}
	
	// return what was found to the caller
	return FoundSelection;
}	

/******************************************************************************************

>	void	BmpPrefsDlg::RecalculateSize(BmpPrefsDlg* pBmpPrefs)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Calculates the size of the selection/spread/drawing and fills in the width
				and height fields accordingly. It then calculates the pixel size and/or dpi.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void	BmpPrefsDlg::RecalculateSize()
{    
	// Get the state of the selection, drawing or spread buttons
	SelectionType FoundSelection = GetSelection();

	// Construct our spread/drawing/selection size rectangle
	ClipRect;
	ClipRect.MakeEmpty(); 

	if (FoundSelection == ABITMAP)
	{
		// Show only the PixelWidth of the bitmap, size in units is irrelevent
		BaseBitmapFilter::GetSizeOfBitmap(&PixelWidth, &PixelHeight);

		RecalculatingPixels = TRUE;	 // flag no updates to dpi field from RecalculateDpiFromX, Y
		
		SetLongGadgetValue(_R(IDC_BMPOPTS_XPIXELS), PixelWidth);
		SetLongGadgetValue(_R(IDC_BMPOPTS_YPIXELS), PixelHeight);

		// WEBSTER - markn 5/2/97
		// Taken out the X & Y size gadgets
#ifndef WEBSTER
		SetStringGadgetValue(_R(IDC_BMPOPTS_XSIZE), &String_8("-"));
		SetStringGadgetValue(_R(IDC_BMPOPTS_YSIZE), &String_8("-"));
		EnableGadget(_R(IDC_BMPOPTS_XSIZE), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_YSIZE), FALSE);
#endif // WEBSTER
		EnableGadget(_R(IDC_BMPOPTS_XPIXELS), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_YPIXELS), FALSE);

		RecalculatingPixels = FALSE;	 // flag no updates to dpi field from RecalculateDpiFromX, Y

		// We need a DPI to work with, so we'll just use the default pixel unit size
		DocUnitList* pDocUnitList =	DocUnitList::GetCurrentDocUnitList();
		ERROR3IF(pDocUnitList == NULL, "BmpPrefsDlg::RecalculateSize() - no pDocUnitList!")
		Unit* pPixelUnit = pDocUnitList->FindUnit(PIXELS);
		ERROR3IF(pPixelUnit == NULL, "BmpPrefsDlg::RecalculateSize() - no pixel units!")
		Unit* pInchUnit = pDocUnitList->FindUnit(INCHES);
		ERROR3IF(pInchUnit == NULL, "BmpPrefsDlg::RecalculateSize() - no inch units!")
		double newDpi = (pPixelUnit->GetMillipoints()>0) ? pInchUnit->GetMillipoints() / pPixelUnit->GetMillipoints() : 96.0;
		Dpi = (INT32)newDpi;
		GetOptions()->SetDPI(newDpi);
	
		ClipRect.lo = DocCoord(0,0);
		ClipRect.hi = DocCoord(PixelWidth * (INT32)newDpi, PixelHeight * (INT32)newDpi);

		Width = ClipRect.Width();	
		Height = ClipRect.Height();	

		// All work is now complete so exit
		return;
	}	

	if (FoundSelection == SOMEBITMAPS)
	{
		// Setup size of animation frames
		ClipRect.lo = DocCoord(0,0);
		ClipRect.hi = DocCoord(GetOptions()->GetOutputSize().x, GetOptions()->GetOutputSize().y);
	}	

	// Find the application and selected document 
	Application* pApplication = GetApplication();
	Document* pDocument = Document::GetSelected();
	Spread* pSpread = NULL;

	// As long as we have an application and a selected document then go and work out
	// the rectangles
	if (pApplication && pDocument && FoundSelection != ABITMAP  && FoundSelection != SOMEBITMAPS)
	{
		// First, check if there is a selection present. ClipRect should be empty if not.
		SelRange* pSelection = pApplication->FindSelection();
		if (pSelection)
		{
			// Get the bounding rectangle for the selection
			ClipRect = pSelection->GetBoundingRect();

			// Work out the parent spread by finding the first node in the selection
			Node* pNode = pSelection->FindFirst();
			// if there is a first node then just find the parent spread
			// otherwise just use the first spread in the document
			if (pNode)
				pSpread = pNode->FindParentSpread();
			else
				pSpread = pDocument->FindFirstSpread();
		}

		if (pSpread)
		{
			switch (FoundSelection)
			{
				case ABITMAP:
					ERROR3IF(TRUE, "BmpPrefsDlg::RecalculateSize() - This should have already been taken care of")
					// This should have already been taken care of
					break;
				case SELECTION:
					// This should have already been taken care of
					break;
				case SPREAD:
				{
					// Work out the size of the rectangle encompassing the spread,
					// using the same function that the bitmap export code uses.
					ERROR3("Selection == Spread!");

					ClipRect = BaseBitmapFilter::GetSizeOfSpread(pSpread);
					//break;
				}
				
				default:
				{
					// Work out the size of the rectangle encompassing the drawing, (visible 
					// layers only), using the same function that the bitmap export code
					// uses.
					ClipRect = BaseBitmapFilter::GetSizeOfDrawing(pSpread);
					break;
				}
			} // End switch
		}
	}

	// Graeme 25/7/00 - If our clip rectangle is less than the minimum export size,
	// resize the clipping rectangle to encompass the entire page.
	if ( ClipRect.IsEmpty() ||
		 ClipRect.Width() < BaseBitmapFilter::MinExportSize ||
		 ClipRect.Height() < BaseBitmapFilter::MinExportSize )
	{
		// Set the ClipRect to be the size of the spread.
		ClipRect = pSpread->FindFirstPageInSpread ()->GetPageRect ();
	}   

	// Work out the size of this rectangle encompassing the speread/drawing/selection
	// in Millipoints and store these in the class variables for later reuse
	Width = ClipRect.Width();	
	Height = ClipRect.Height();	

	// Work out what the current units we should be displaying in
	DocUnitList* pDocUnitList = NULL;
	UnitType CurrentPageUnits = MILLIMETRES;
	if (pDocument)
	{
		pDocUnitList = pDocument->GetDocUnitList();
		if (pDocUnitList)
			CurrentPageUnits = pDocUnitList->GetPageUnits();
	}
	
	// WEBSTER - markn 5/2/97
	// Taken out the X & Y size gadgets
	#ifndef WEBSTER
	if (m_bDpiSupported)
	{
		SetUnitGadgetValue(_R(IDC_BMPOPTS_XSIZE), CurrentPageUnits, Width, FALSE, -1);
		SetUnitGadgetValue(_R(IDC_BMPOPTS_YSIZE), CurrentPageUnits, Height, FALSE, -1);
	}
	else
	{
		SetStringGadgetValue(_R(IDC_BMPOPTS_XSIZE), &String_8("-"));
		SetStringGadgetValue(_R(IDC_BMPOPTS_YSIZE), &String_8("-"));
	}
	#endif // WEBSTER
	
	// Work out what this size is in pixels and fill in the editable fields
	RecalculatePixels();
}

/******************************************************************************************

>	void	BmpPrefsDlg::RecalculatePixels()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Takes the size of the selection/spread/drawing and calculates the size that
				this will be in pixels given the current dpi field.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void	BmpPrefsDlg::RecalculatePixels()
{    

	// If the RecalculateDpiFromX() or RecalculateDpiFromY() functions are setting the
	// values then make sure we do not update the values as well. 
	if (RecalculatingXDpi || RecalculatingYDpi)
		return;

	// Now work out the pixel sizes which this will correspond to at the current dpi setting
	PixelWidth = 0;	
	PixelHeight = 0;	
	Dpi = 0;
	BOOL Valid = 0;			// Flag for validity of value

	// Get the dpi value from the resolution combo box, with no error reporting
	// Don't change the current values if the dpi is wrong
	if (!m_bDpiSupported)
	{
		Dpi = GetOptions()->GetDPI();
		Valid = TRUE;
	}
	else
	{
		Dpi = (double)GetLongGadgetValue(_R(IDC_BMPOPTS_RES), 5, MAXDPI, NULL ,&Valid);
	}
	
	if ( Valid ) 
	{
		// This is the calculation that we are effectively doing
		// Use 0.5 so that we are forced to round up to the next pixel, if possible
		//PixelWidth = (INT32)(((Width * Dpi)/72000) + 0.5);
		//PixelHeight = (INT32)(((Height * Dpi)/72000) + 0.5);
		// but use the same code that the bitmap code will use so that we are consistent
		Matrix Identity;	// default construction is an identity matrix
		WinRect	Rect = OSRenderRegion::BitmapDocRectToWin( Identity, ClipRect, Dpi );
		PixelWidth = Rect.Width();
		PixelHeight = Rect.Height();

		// More of a safety net here.
		if (PixelWidth < 0)
			PixelWidth = 0;
		if (PixelHeight < 0)
			PixelHeight = 0;

		RecalculatingPixels = TRUE;	 // flag no updates to dpi field from RecalculateDpiFromX, Y
		
		SetLongGadgetValue(_R(IDC_BMPOPTS_XPIXELS), PixelWidth);
		SetLongGadgetValue(_R(IDC_BMPOPTS_YPIXELS), PixelHeight);
		
		RecalculatingPixels = FALSE;	 // flag no updates to dpi field from RecalculateDpiFromX, Y
	}
	else
	{
		RecalculatingPixels = TRUE;	 // flag no updates to dpi field from RecalculateDpiFromX, Y

		// There is a problem with the dpi so blank the pixel width/height fields 
		SetStringGadgetValue(_R(IDC_BMPOPTS_XPIXELS), &String_8(""));
		SetStringGadgetValue(_R(IDC_BMPOPTS_YPIXELS), &String_8(""));

		RecalculatingPixels = FALSE;	 // flag no updates to dpi field from RecalculateDpiFromX, Y
	}
}

/******************************************************************************************

>	void	BmpPrefsDlg::RecalculateDpiFromX()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Takes the size of the selection/spread/drawing and the size in the pixel
				width field and calculates a new current dpi from this.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void	BmpPrefsDlg::RecalculateDpiFromX()
{    
	// If the RecalculatePixels() function is setting the values then make sure we
	// do not update the values as well. 
	if (RecalculatingPixels || RecalculatingYDpi)
		return;

	// Now work out the pixel sizes which this will correspond to at the current dpi setting
	// Get the x and y pixel sizes from the editable fields, with no error reporting
	// Use 9 as minimum so that when deleting, the dpi is not blanked when down to one digit
	BOOL Valid = FALSE;	// Flag for validity of value
	PixelWidth = GetLongGadgetValue(_R(IDC_BMPOPTS_XPIXELS), 9, INT32_MAX, NULL ,&Valid);

	if ( Valid ) 
	{
		RecalculatingXDpi = TRUE;

		double Resolution = (Width>0) ? ((double) PixelWidth * 72000.0) / ((double) Width) : 96.0;

		// A bit of a safety net here
		if ((Resolution < 5.0) || (Resolution > D_MAXDPI))
		{
			// There is a problem with the x pixels field so blank the pixel height
			// and dpi fields 
			SetStringGadgetValue(_R(IDC_BMPOPTS_YPIXELS), &String_8(""));
			if (m_bDpiSupported)
			{
				SetStringGadgetValue(_R(IDC_BMPOPTS_RES), &String_8(""), FALSE, -1);
			}
		}
		else
		{
			// Use 0.5 so that we are forced to round up to the next dpi, if possible
			INT32 b_dpi = (INT32)(Resolution + 0.5);
			Dpi = b_dpi; //Resolution;
			if (m_bDpiSupported)
			{
				//SetDoubleGadgetValue(_R(IDC_BMPOPTS_RES), Dpi, FALSE, -1);
				SetLongGadgetValue(_R(IDC_BMPOPTS_RES), (INT32)Dpi, FALSE, -1);
			}

			// Given the new dpi, recalculate the y pixels
			// This is once again the calculation we are doing
			// INT32 PixelHeight = (INT32)(((Height * Dpi)/72000) + 0.5);
			// but using the same code that wil be used for exporting
			Matrix Identity;	// default construction is an identity matrix
			WinRect	Rect = OSRenderRegion::BitmapDocRectToWin( Identity, ClipRect, Dpi );
			PixelHeight = Rect.Height();
			SetLongGadgetValue(_R(IDC_BMPOPTS_YPIXELS), PixelHeight);
		}
				
		RecalculatingXDpi = FALSE;
	}
	else
	{
		RecalculatingXDpi = TRUE;

		// There is a problem with the x pixels field so blank the pixel height
		// and dpi fields 
		SetStringGadgetValue(_R(IDC_BMPOPTS_YPIXELS), &String_8(""));
		if (m_bDpiSupported)
		{
			SetStringGadgetValue(_R(IDC_BMPOPTS_RES), &String_8(""), FALSE, -1);
		}

		RecalculatingXDpi = FALSE;
	}
}

/******************************************************************************************

>	void	BmpPrefsDlg::RecalculateDpiFromY()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Takes the size of the selection/spread/drawing and the size in the pixel
				height field and calculates a new current dpi from this.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void	BmpPrefsDlg::RecalculateDpiFromY()
{    
	// If the RecalculatePixels() function is setting the values then make sure we
	// do not update the values as well. 
	if (RecalculatingPixels || RecalculatingXDpi)
		return;

	// Now work out the pixel sizes which this will correspond to at the current dpi setting
	// Get the x and y pixel sizes from the editable fields, with no error reporting
	// Use 9 as minimum so that when deleting, the dpi is not blanked when down to one digit
	BOOL Valid = FALSE;		// Flag for validity of value
	PixelHeight = GetLongGadgetValue(_R(IDC_BMPOPTS_YPIXELS), 9, INT32_MAX, NULL ,&Valid);

	if ( Valid ) 
	{
		RecalculatingYDpi = TRUE;
		
		double Resolution = (Height>0) ? ((double) PixelHeight * 72000.0) / ((double) Height) : 96.0;

		// A bit of a safety net here
		if ((Resolution < 5.0) || (Resolution > D_MAXDPI))
		{
			// There is a problem with the y pixels field so blank the pixel width
			// and dpi fields 
			SetStringGadgetValue(_R(IDC_BMPOPTS_XPIXELS), &String_8(""));
			if (m_bDpiSupported)
			{
				SetStringGadgetValue(_R(IDC_BMPOPTS_RES), &String_8(""), FALSE, -1);
			}
		}
		else
		{
			// Use 0.5 so that we are forced to round up to the next dpi, if possible
			INT32 b_dpi = (INT32)(Resolution + 0.5);
			Dpi = b_dpi; //Resolution;
			{
				//SetDoubleGadgetValue(_R(IDC_BMPOPTS_RES), Dpi, FALSE, -1);
				SetLongGadgetValue(_R(IDC_BMPOPTS_RES), (INT32)Dpi, FALSE, -1);
			}
			
			// Given the new dpi, recalculate the y pixels
			// This is once again the calculation we are doing
			// INT32 PixelWidth = (INT32)(((Width * Dpi)/72000) + 0.5);
			// but using the same code that wil be used for exporting
			Matrix Identity;	// default construction is an identity matrix
			WinRect	Rect = OSRenderRegion::BitmapDocRectToWin( Identity, ClipRect, Dpi );
			PixelWidth = Rect.Width();
			SetLongGadgetValue(_R(IDC_BMPOPTS_XPIXELS), PixelWidth);
		}

		RecalculatingYDpi = FALSE;
	}
	else
	{
		RecalculatingYDpi = TRUE;

		// There is a problem with the y pixels field so blank the pixel width
		// and dpi fields 
		SetStringGadgetValue(_R(IDC_BMPOPTS_XPIXELS), &String_8(""));
		if (m_bDpiSupported)
		{
			SetStringGadgetValue(_R(IDC_BMPOPTS_RES), &String_8(""), FALSE, -1);
		}

		RecalculatingYDpi = FALSE;
	}
}

/******************************************************************************************

>	void	BmpPrefsDlg::SetBPPButtons(GadgetID ButtonClicked,BOOL CheckNumColValueRange)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/08/94
	Inputs:		the id of the clicked radio button
 				CheckNumColValueRange = TRUE if the value in the num col ed field should be checked. (added for WEBSTER - markn 1/2/97)
	Outputs:	-
	Returns:	-
	Purpose:	Sets all other radio buttons in the group off
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void BmpPrefsDlg::SetBPPButtons(CGadgetID ButtonClicked,BOOL CheckNumColValueRange)
{    
	BitmapExportOptions* pOptions = GetOptions();
	if (pOptions == NULL)
	{
		ERROR3("pOptions == NULL");
		return;
	}

	// WEBSTER - markn 23/1/97
	SetLongGadgetValue(ButtonClicked, TRUE);

	BOOL EnablePalette = FALSE;

	// If either 8, 4 or 1 bpp clicked then enable the Palette and Dithering Options
	if ( ButtonClicked == _R(IDC_BMPOPTS_8BPP) || ButtonClicked == _R(IDC_BMPOPTS_4BPP)  || ButtonClicked == _R(IDC_BMPOPTS_1BPP))
		EnablePalette = TRUE;

	EnableGadget(_R(IDC_BMPOPTS_PAL_STD), EnablePalette);
	EnableGadget(_R(IDC_BMPOPTS_PAL_OPT), EnablePalette);
	EnableGadget(_R(IDC_BMPOPTS_NODITHER),  EnablePalette);
	EnableGadget(_R(IDC_BMPOPTS_DIFFUSION), EnablePalette);

	if (EnablePalette)
	{
		BOOL Valid = 0;
		if ( GetLongGadgetValue(_R(IDC_BMPOPTS_PAL_OPT), 0, 1, 0, &Valid) )
			EnableGadget(_R(IDC_BMPOPTS_ORDDITHER), FALSE);
		else
			EnableGadget(_R(IDC_BMPOPTS_ORDDITHER), TRUE);
	}
	else
		EnableGadget(_R(IDC_BMPOPTS_ORDDITHER), FALSE);

	// WEBSTER - markn 17/1/97
	// Only update for relevant filters
	if (pOptions->UseSpecificNumColsInPalette())
		UpdateNumColoursGroup(CheckNumColValueRange);	// New parameter - WEBSTER - markn 1/2/97

	if (pOptions->GetFilterType() == MAKE_BITMAP_FILTER)
	{
		// Disable transparency button in same circustances as
		// the palette buttons are disabled
		EnableGadget(_R(IDC_BMPOPTS_TRANSPARENT), EnablePalette);

		// If we're greying out the button, turn off the transparency flag too
		// so that we take no notice of it when it's grey
		if (!EnablePalette)
		{
			SetLongGadgetValue(_R(IDC_BMPOPTS_TRANSPARENT), 0);
		}
		else
		{
			MaskedFilterExportOptions* pMaskOptions = (MaskedFilterExportOptions*)pOptions;
			ERROR3IF(!pMaskOptions->IS_KIND_OF(MaskedFilterExportOptions), "pMaskOptions isn't");

			SetLongGadgetValue(_R(IDC_BMPOPTS_TRANSPARENT), pMaskOptions->WantTransparent());
		}
	}

	// Cope with the transparency button on a PNG options dialog box	
	if (pOptions->GetFilterType() == PNG) 
	{
		PNGExportOptions* pPNGOptions = (PNGExportOptions*)pOptions;
		ERROR3IF(!pPNGOptions->IS_KIND_OF(PNGExportOptions), "pPNGOptions isn't");
		// Now enable or disable the transparency if we are in the wrong bpp mode
		// Transparency only available in 8 bpp mode or less
		BOOL Enable = FALSE;
		if ( ButtonClicked == _R(IDC_BMPOPTS_8BPP) || ButtonClicked == _R(IDC_BMPOPTS_4BPP) ||
			 ButtonClicked == _R(IDC_BMPOPTS_1BPP) )
			Enable = TRUE;

		// Enable should be True if 8bpp or 4bpp or 1bpp button is On and False if not 
		EnableGadget(_R(IDC_BMPOPTS_TRANSPARENT), Enable);
		if (!Enable)
		{
			SetLongGadgetValue(_R(IDC_BMPOPTS_TRANSPARENT), 0);
		}
		else
		{
			SetLongGadgetValue(_R(IDC_BMPOPTS_TRANSPARENT), pPNGOptions->WantTransparent());
		}
	}

	// WEBSTER - markn 23/1/97
	// Compression group always hidden in Webster
	#ifndef WEBSTER
	// Cope with the compression buttons on a BMP options dialog box	
	if (pOptions->GetFilterType() == BMP_UNCOMPRESSED) 
	{
		// Now enable or disable the switches that are dependent on the 8 bpp switch 
		// RLE compression switch, only available in 8 bpp mode or 4 bpp
		BOOL Enable = FALSE;
		if ( ButtonClicked == _R(IDC_BMPOPTS_8BPP) || ButtonClicked == _R(IDC_BMPOPTS_4BPP) )
			Enable = TRUE;

		// Enable should be True if 8bpp or 4bpp button is On and False if not 
		// Present only on the BMP export dialog box
		EnableGadget(_R(IDC_BMPOPTS_COMPRESS), Enable); 	// main group icon
		EnableGadget(_R(IDC_BMPOPTS_RLE), Enable);
		EnableGadget(_R(IDC_BMPOPTS_RGB), Enable);
	}
	#endif // WEBSTER

	// Cope with the compression buttons on a TIFF options dialog box	
	// Enable should be True if 8bpp or 4bpp button is On and False if not 
	if (pOptions->GetFilterType() == TIFF_UNCOMPRESSED) 
	{
	//WEBSTER-Martin-06/01/97
#ifndef WEBSTER
		TIFFExportOptions* pTIFFOptions = (TIFFExportOptions*)pOptions;
		ERROR3IF(!pTIFFOptions->IS_KIND_OF(TIFFExportOptions), "pTIFFOptions isn't");
	
		// Now enable or disable the compression options depending upon the BPP
		BOOL bEnableLZW = FALSE;
		BOOL bEnableMonoCompression = FALSE;

		if ( ButtonClicked == _R(IDC_BMPOPTS_1BPP))
			bEnableMonoCompression = TRUE;

		if ( ButtonClicked == _R(IDC_BMPOPTS_24BPP) || ButtonClicked == _R(IDC_BMPOPTS_8BPP) || ButtonClicked == _R(IDC_BMPOPTS_CMYK) )
			bEnableLZW = TRUE;


		// Cannot grey items in the list so we need to change the list itself.
		// But only change it if we need to. Check last state and see if we have changed.
		// from 1bpp or to 1bpp.
		if ( /*
			(LastBppSelected == _R(IDC_BMPOPTS_1BPP) && ButtonClicked != _R(IDC_BMPOPTS_1BPP)) ||
			(LastBppSelected != _R(IDC_BMPOPTS_1BPP) && ButtonClicked == _R(IDC_BMPOPTS_1BPP))
		   */ TRUE )
		{
			// So, set up the list of available TIFF compressions. Must be the same as the list
			// in TiffTypes.
			//TiffTypes CompressionType = (TiffTypes)GetSelectedValueIndex(_R(IDC_BMPOPTS_TIFF));
			
			// Andy Hills, 05-09-00
			// We can't use TiffTypes to determine the selection.
			// This is because the list of compression types changes between bpp settings.
			// Instead we will call the new function GetTIFFCompressionSelection
			// which works out which compression type is selected.
			// We need to tell it which bpp is selected.

			INT32 nBPP;
			switch( ButtonClicked )
			{
			case _R(IDC_BMPOPTS_1BPP):	nBPP=1;		break;
			case _R(IDC_BMPOPTS_4BPP):	nBPP=4;		break;
			case _R(IDC_BMPOPTS_8BPP):	nBPP=8;		break;
			case _R(IDC_BMPOPTS_24BPP):	nBPP=24;	break;
			case _R(IDC_BMPOPTS_CMYK):	nBPP=32;	break;
			default:
				ERROR3( "BmpPrefsDlg::SetBPPButtons - invalid bpp obtained" );
			}
			
			INT32 CompressionType = GetTIFFCompressionSelection(nBPP);

			// First delete all present items
			DeleteAllValues(_R(IDC_BMPOPTS_TIFF));
			// Set up the default value in the editable field
			BOOL ok = SetLongGadgetValue(_R(IDC_BMPOPTS_TIFF), pTIFFOptions->GetCompression(), FALSE, -1);

			// Set all the basic options (i.e. no compression)
			SetStringGadgetValue(_R(IDC_BMPOPTS_TIFF), &String_32(_R(IDN_TIFF_NONE)));

			// Andy Hills, 16-08-00: re-added LZW export
			if (bEnableLZW)
			{
				// 8 / 24-bit, so enable LZW
				SetStringGadgetValue(_R(IDC_BMPOPTS_TIFF), &String_32(_R(IDN_TIFF_LZW)));
			}

			if (bEnableMonoCompression)
			{
				// 1bpp so allow all options (except LZW)
				SetStringGadgetValue(_R(IDC_BMPOPTS_TIFF), &String_32(_R(IDN_TIFF_HUFFMAN)));
				SetStringGadgetValue(_R(IDC_BMPOPTS_TIFF), &String_32(_R(IDN_TIFF_G3FAX)));
				SetStringGadgetValue(_R(IDC_BMPOPTS_TIFF), &String_32(_R(IDN_TIFF_G4FAX)));
				SetStringGadgetValue(_R(IDC_BMPOPTS_TIFF), &String_32(_R(IDN_TIFF_PACKBITS)));
				//SetStringGadgetValue(_R(IDC_BMPOPTS_TIFF), &String_32(_R(IDN_TIFF_2DENCODING)));
			}

			SetComboListLength(_R(IDC_BMPOPTS_TIFF));

			// Select the correct item again

			if( nBPP == 1 && LastBppSelected != _R(IDC_BMPOPTS_1BPP) && LastBppSelected )
			{
				// changing to 1bpp
				// --> set compression to NONE
				ok = SetTIFFCompressionSelection( TIFF_UNCOMPRESSED, nBPP );
				
				// We should ALWAYS be able to select UNCOMPRESSED
				ERROR3IF( (!ok), "BmpPrefsDlg::SetBPPButtons - could not select uncompressed" );
			}
			else if( nBPP >= 8 && 
				(LastBppSelected == _R(IDC_BMPOPTS_1BPP) || LastBppSelected == _R(IDC_BMPOPTS_4BPP)) )
			{
				// changing to 8/24bpp (from 1/4bpp)
				// --> set compression to LZW
				ok = SetTIFFCompressionSelection( TIFF_LZW, nBPP );
				
				// We should ALWAYS be able to select LZW from 8/24 bpp
				ERROR3IF( (!ok), "BmpPrefsDlg::SetBPPButtons - could not select LZW" );
			}
			else
			{
				// otherwise, just leave compression as it is
				ok = SetTIFFCompressionSelection( CompressionType, nBPP );
				
				// if the previously selected compression is now disabled, select NONE
				if( ! ok )
				{
					//SetSelectedValueIndex(_R(IDC_BMPOPTS_TIFF), _R(IDN_TIFF_NONE));
					ok = SetTIFFCompressionSelection( TIFF_UNCOMPRESSED, nBPP );
					
					// We should ALWAYS be able to select UNCOMPRESSED
					ERROR3IF( (!ok), "BmpPrefsDlg::SetBPPButtons - could not select uncompressed" );
				}
			}
		}
	
		// Update the last button to be the one just selected
		LastBppSelected = ButtonClicked;
#endif //WEBSTER
	}
}

/******************************************************************************************

>	void BmpPrefsDlg::SetPaletteButtons(GadgetID ButtonClicked)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/05/96
	Inputs:		the id of the clicked radio button
	Outputs:	-
	Returns:	-
	Purpose:	Sets all other radio buttons in the group off
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void BmpPrefsDlg::SetPaletteButtons(CGadgetID ButtonClicked)
{    
	if ( ButtonClicked == _R(IDC_BMPOPTS_PAL_OPT) )
	{
		BOOL Valid = 0;
		if ( GetLongGadgetValue(_R(IDC_BMPOPTS_ORDDITHER), 0, 1, 0, &Valid) )
		{
			SetLongGadgetValue(_R(IDC_BMPOPTS_ORDDITHER), FALSE);
			SetLongGadgetValue(_R(IDC_BMPOPTS_DIFFUSION), TRUE);
		}

		SetLongGadgetValue(_R(IDC_BMPOPTS_PAL_OPT), TRUE);
		
		EnableGadget(_R(IDC_BMPOPTS_ORDDITHER), FALSE);

		// WEBSTER - markn 17/1/97
		// Update num colours group
		UpdateNumColoursGroup(FALSE);
	}
	else
	{
		SetLongGadgetValue(_R(IDC_BMPOPTS_PAL_STD), TRUE);
			
		EnableGadget(_R(IDC_BMPOPTS_ORDDITHER), TRUE);

		// WEBSTER - markn 17/1/97
		UpdateNumColoursGroup(FALSE);
	}
}

/******************************************************************************************

>	void BmpPrefsDlg::SetDitherButtons(GadgetID ButtonClicked)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/05/96
	Inputs:		the id of the clicked radio button
	Outputs:	-
	Returns:	-
	Purpose:	Sets all other radio buttons in the group off
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void BmpPrefsDlg::SetDitherButtons(CGadgetID ButtonClicked)
{    
}


/******************************************************************************************

>	TIFF_COMPRESSION BmpPrefsDlg::GetTIFFCompressionSelection( INT32 nBPP )

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/09/00
	Inputs:		nBPP		Currently selected bits-per-pixel
	Outputs:	
	Returns:	The currently selected compression type.
	Purpose:	This function has been added because no allowances have been
				made for the fact that the compression options list is
				different for each BPP setting.
	Errors:		Returns -1 on failure.
	SeeAlso:	SetBPPButtons

******************************************************************************************/

TIFF_COMPRESSION BmpPrefsDlg::GetTIFFCompressionSelection( INT32 nBPP )
{
	// Get the state of the TIFF compression selection combo box
	TIFF_COMPRESSION Compression;
	INT32 nIndex = GetSelectedValueIndex(_R(IDC_BMPOPTS_TIFF));

	if( nBPP == 1 )
	{
		switch ( nIndex )
		{
			case TWODENCODING_TIFF:	Compression = TIFF_2D;				break;
			case PACKBITS_TIFF:		Compression = TIFF_PACK;			break;
			case HUFFMAN_TIFF:		Compression = TIFF_HUFFMAN;			break;
			case G3FAX_TIFF:		Compression = TIFF_G3_FAX;			break;
			case G4FAX_TIFF:		Compression = TIFF_G4_FAX;			break;
			default:				
			case UNCOMPRESSED_TIFF:	Compression = TIFF_UNCOMPRESSED;	break;
		}
	}
	else if( nBPP==8 || nBPP==24 || nBPP==32 )
	{
		switch ( nIndex )
		{
			case LZW_TIFF:			Compression = TIFF_LZW;				break;
			default:				
			case UNCOMPRESSED_TIFF:	Compression = TIFF_UNCOMPRESSED;	break;
		}
	}
	else if( nBPP == 4 )
	{
		Compression = TIFF_UNCOMPRESSED;
	}
	else
	{
		ERROR2( -1, "BmpPrefsDlg::GetTIFFCompressionSelection - invalid nBPP" );
	}
	
	return Compression;
}


/******************************************************************************************

>	BOOL BmpPrefsDlg::SetTIFFCompressionSelection( TIFF_COMPRESSION Compression, INT32 nBPP )

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/09/00
	Inputs:		Compression		Desired compression setting
				nBPP			Currently selected bits-per-pixel
	Outputs:	
	Returns:	TRUE on success
	Purpose:	This function has been added because no allowances have been
				made for the fact that the compression options list is
				different for each BPP setting.
	Errors:		Returns FALSE on failure
				e.g. if the specified compression type is not available with the current
				bpp setting.
	SeeAlso:	SetBPPButtons

******************************************************************************************/

BOOL BmpPrefsDlg::SetTIFFCompressionSelection( TIFF_COMPRESSION Compression, INT32 nBPP )
{
	if( nBPP == 1 )
	{
		if
		(
			Compression != TIFF_2D &&
			Compression != TIFF_PACK &&
			Compression != TIFF_HUFFMAN &&
			Compression != TIFF_G3_FAX &&
			Compression != TIFF_G4_FAX &&
			Compression != TIFF_UNCOMPRESSED
		) return FALSE;
	}
	else if( nBPP==8 || nBPP==24 || nBPP==32 )
	{
		if
		(
			Compression != TIFF_LZW &&
			Compression != TIFF_UNCOMPRESSED
		) return FALSE;
	}
	else if( nBPP == 4 )
	{
		if
		(
			Compression != TIFF_UNCOMPRESSED
		) return FALSE;
	}
	else
	{
		ERROR2( -1, "BmpPrefsDlg::GetTIFFCompressionSelection - invalid nBPP" );
	}
	
	// look up the index of the compression type
	INT32 nIndex;
	switch ( Compression )
	{
		case TIFF_UNCOMPRESSED: nIndex = UNCOMPRESSED_TIFF;	break;
		case TIFF_LZW:			nIndex = LZW_TIFF;			break;
		case TIFF_HUFFMAN:		nIndex = HUFFMAN_TIFF;		break;
		case TIFF_G3_FAX:		nIndex = G3FAX_TIFF;		break;
		case TIFF_G4_FAX:		nIndex = G4FAX_TIFF;		break;
		case TIFF_PACK:			nIndex = PACKBITS_TIFF;		break;
		case TIFF_2D:			nIndex = TWODENCODING_TIFF;	break;
		default: 				nIndex = UNCOMPRESSED_TIFF;	break;
	}

	// Actually select the compression type.
	return SetSelectedValueIndex(_R(IDC_BMPOPTS_TIFF), nIndex);
}


/******************************************************************************************

>	BOOL BmpPrefsDlg::CommitDialogValues(BmpPrefsDlg* pBmpPrefs)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/10/94
	Inputs:		pointer to the dialogue box
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the dialog box and sets the return values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL BmpPrefsDlg::CommitDialogValues()
{
	BitmapExportOptions* pOptions = GetOptions();
	ERROR2IF(pOptions == NULL, FALSE, "BmpPrefsDlg::CommitDialogValues called after duff initialisation?!");

	// Ok has been pressed so take the values and set up the static values so that the
	// caller can access them

	BOOL Valid = 0;	// Flag for validity of value

	//Added by Graham 27/5/97
	//First, let's set the pixel width and height into our OutputSize option
	pOptions->SetPixelOutputSize(PixelWidth, PixelHeight);

	// Although the file format may not support DPI, BaseBitmapFilter::PrepareToExport doesn't support
	// anything else. Since we may want to stretch the selection, we therefore need to supply a bogus dpi
	// based on the desired export size. dpi supporting formats can follow the mess...
	if (m_bDpiSupported)
	{
		// Get the dpi value from the resolution combo box
		// Minimum of 5dpi as anything lower causes major problems.  
		//double Value = GetDoubleGadgetValue(_R(IDC_BMPOPTS_RES), 5.0, D_MAXDPI, _R(IDS_BMPPREFS_INVALIDDPI) ,&Valid);
		double Value = (double)GetLongGadgetValue(_R(IDC_BMPOPTS_RES), 5, MAXDPI, _R(IDS_BMPPREFS_INVALIDDPI) ,&Valid);
		if (Valid) 
		{
			// If returned value in range then set the new default
			pOptions->SetDPI(Value);
		}
		else
			return FALSE;

		RecalculatePixels();
	}
	else
	{
		double Resolution = (Width>0) ? ((double) PixelWidth * 72000.0) / ((double) Width) : 96.0;
		// A bit of a safety net here
		if (Resolution < 5.0)
		{
			// User has set an illegal state so warn them about it.
			InformError(_R(IDS_BMPPREFS_DPITOOSMALL));
			//Dpi = 0;
			return FALSE;
		}
		else if (Resolution > D_MAXDPI)
		{
			// User has set an illegal state so warn them about it.
			InformError(_R(IDS_BMPPREFS_DPITOOBIG));
			//Dpi = 0;
			return FALSE;
		}
		else
		{
			INT32 b_dpi = (INT32)(Resolution + 0.5);
			Dpi = b_dpi; //Resolution;
		}
		pOptions->SetDPI(Dpi);
	}

	// Get the output depth from the bpp radio buttons
	BOOL State = 0;
	UINT32 Depth = 24;
	if ( GetLongGadgetValue(_R(IDC_BMPOPTS_CMYK), 0, 1, 0, &Valid) )
	{
		pOptions->SetCMYK(TRUE);
		Depth = 32;
	}
	else if ( GetLongGadgetValue(_R(IDC_BMPOPTS_24BPP), 0, 1, 0, &Valid) )
		Depth = 24;
	else if ( GetLongGadgetValue(_R(IDC_BMPOPTS_8BPP), 0, 1, 0, &Valid) )
		Depth = 8;
	else if ( GetLongGadgetValue(_R(IDC_BMPOPTS_4BPP), 0, 1, 0, &Valid) )
		Depth = 4;
	else if ( GetLongGadgetValue(_R(IDC_BMPOPTS_1BPP), 0, 1, 0, &Valid) )
		Depth = 1;
	pOptions->SetDepth(Depth);

	// WEBSTER - markn 17/1/97
	// NOTE! This can change the Depth value set earlier in this function
	if (!SetNumColoursInPalette(pOptions))
		return FALSE;

	// Get the Palette setting from the radio buttons
	PALETTE Palette = PAL_STANDARD;
	if ( GetLongGadgetValue(_R(IDC_BMPOPTS_PAL_OPT), 0, 1, 0, &Valid) )
	{
		TRACEUSER( "Will", _T("Optimised Palette selected\n"));
		Palette = PAL_OPTIMISED;
	}

	// Get the dither type from the radio buttons
	DITHER DitherType = XARADITHER_ERROR_DIFFUSION;
	if ( GetLongGadgetValue(_R(IDC_BMPOPTS_NODITHER), 0, 1, 0, &Valid) )
		DitherType = XARADITHER_NONE;
	else if ( GetLongGadgetValue(_R(IDC_BMPOPTS_ORDDITHER), 0, 1, 0, &Valid) )
		DitherType = XARADITHER_ORDERED_GREY;
	else if ( GetLongGadgetValue(_R(IDC_BMPOPTS_DIFFUSION), 0, 1, 0, &Valid) )
		DitherType = XARADITHER_ERROR_DIFFUSION;

	TRACEUSER( "Will", _T("Dither %d selected\n"), DitherType);

	// Check that we have not gone over our GDraw limits for pixel width
	// Pixel height should not be so much of a problem as we will strip the export. 

	// This cast is here because PixelWidth is a signed number (for some odd reason)
	if ((DWORD) PixelWidth > GRenderRegion::GetMaxBitmapWidth() ||
		(DWORD) PixelWidth < 1 || (DWORD) PixelHeight < 1 )
	{
		// There is a problem so warn the user that the value is incorrect
		String_256 WarnMsg;

		WarnMsg.MakeMsg(_R(IDE_BMP_BADPIXELWIDTH), GRenderRegion::GetMaxBitmapWidth());
		Error::SetError(0, WarnMsg, 0);
		InformWarning(0, _R(IDS_OK));
		return FALSE; 
	}

	// Mostly the compression fields change according to the filter type in use
	switch ( pOptions->GetFilterType() ) 
	{
		case TIFF_UNCOMPRESSED:
		{
			//WEBSTER-Martin-03/01/97
#ifndef WEBSTER
			TIFFExportOptions* pTIFFOptions = (TIFFExportOptions*)pOptions;
			ERROR3IF(!pTIFFOptions->IS_KIND_OF(TIFFExportOptions), "pTIFFOptions isn't");

			// Get the state of the TIFF compression selection combo box
			TIFF_COMPRESSION Compression = GetTIFFCompressionSelection(Depth);
			ERROR2IF( (Compression==-1), FALSE, "BmpPrefsDlg::CommitDialogValues - invalid compresion type" );
			pTIFFOptions->SetCompression(Compression);

//Mark Howitt, 27/10/97. We need to setup both the Dither & Palette for the TIFF Options, as well as
//						 the Accusoft DitherToUse variable to ensure proper functionality.
			pTIFFOptions->SetDither(DitherType);
			pTIFFOptions->SetPalette(Palette);
			AccusoftFilters::SetDitherToUse(DitherType);

#endif //WEBSTER
			break;
		}
		case PNG:
		{
			PNGExportOptions* pPNGOptions = (PNGExportOptions*)pOptions;
			ERROR3IF(!pPNGOptions->IS_KIND_OF(PNGExportOptions), "pPNGOptions isn't");

			// GIF radio buttons used to hold the transparent/interlaced settings
			BOOL Interlace = GetLongGadgetValue(_R(IDC_BMPOPTS_INTERLACED), 0, 1, 0, &Valid);
			BOOL Transparent = GetLongGadgetValue(_R(IDC_BMPOPTS_TRANSPARENT), 0, 1, 0, &Valid);
			// Transparency is not available at present
			// Only if we are exporting as a 32bpp bitmap.
			if (Valid)
			{
				//pPNGOptions->SetMakeTransparent(Transparent);
				pPNGOptions->SetMakeInterlaced(Interlace);

				// WEBSTER - markn 5/2/97
				// If the user has deselected Transparent, then make sure the cached index is set to -1
				if (!Transparent)
					pPNGOptions->SetTransparencyIndex(-1);
			}
			pPNGOptions->SetDither(DitherType);
//			pPNGOptions->SetPalette(Palette);
			break;
		}

		case MAKE_BITMAP_FILTER:
		{
			MakeBitmapExportOptions* pMkBOptions = (MakeBitmapExportOptions*)pOptions;
			ERROR3IF(!pMkBOptions->IS_KIND_OF(MakeBitmapExportOptions), "pMkBOptions isn't");

			pMkBOptions->SetDither(DitherType);
//			pMkBOptions->SetPalette(Palette);

			// radio buttons used to hold the transparent/interlaced settings
			BOOL Interlace = GetLongGadgetValue(_R(IDC_BMPOPTS_INTERLACED), 0, 1, 0, &Valid);
			BOOL Transparent = GetLongGadgetValue(_R(IDC_BMPOPTS_TRANSPARENT), 0, 1, 0, &Valid);
			//pMkBOptions->SetMakeTransparent(Transparent);
			pMkBOptions->SetMakeInterlaced(Interlace);

			break;
		}
		case TI_GIF:
		case TI_GIF_ANIM:
		{
			GIFExportOptions* pGIFOptions = (GIFExportOptions*)pOptions;
			ERROR3IF(!pGIFOptions->IS_KIND_OF(GIFExportOptions), "pGIFOptions isn't");
									
			pGIFOptions->SetDither(DitherType);
//			pGIFOptions->SetPalette(Palette);

			// GIF radio buttons used to hold the transparent/interlaced settings
			BOOL Interlace = GetLongGadgetValue(_R(IDC_BMPOPTS_INTERLACED), 0, 1, 0, &Valid);
			BOOL Transparent = GetLongGadgetValue(_R(IDC_BMPOPTS_TRANSPARENT), 0, 1, 0, &Valid);
			//pGIFOptions->SetMakeTransparent(Transparent);
			pGIFOptions->SetMakeInterlaced(Interlace);

			// WEBSTER - markn 5/2/97
			// If the user has deselected Transparent, then make sure the cached index is set to -1
			if (!Transparent)
				pGIFOptions->SetTransparencyIndex(-1);

			if (pOptions->GetFilterType() == TI_GIF)
			{
				HideGadget(_R(IDC_ANIMOPTS_LOOP), TRUE);	
				HideGadget(_R(IDC_ANIMOPTS_DELAY), TRUE);	
				HideGadget(_R(IDC_ANIMOPTS_ITERATIONS), TRUE);
				HideGadget(_R(IDC_ANIMOPTS_REMOVAL), TRUE);	
			}

			if (pOptions->GetFilterType() == TI_GIF_ANIM)
			{
				UINT32 List = 0;
				BmpDlgParam* pBmpDlgParam = pOptions->GetBmpDlgParam();
									
				if (pBmpDlgParam != NULL)
				{
					KernelBitmap** pListOfBitmaps = pBmpDlgParam->GetBitmapList();
					List = pBmpDlgParam->GetListSize();

					ERROR3IF(pListOfBitmaps == NULL && List > 0, "Where Have Our Bitmaps Gone?");
					
					if(pListOfBitmaps != NULL)
					{
						pGIFOptions->SetBitmapList(pListOfBitmaps);
						pGIFOptions->SetListSize(List);
					}
				}
				
				// Get the Animation Delay details.
				if (HasDelayChanged)
				{
					 CENTISECONDS Delay = GetLongGadgetValue(_R(IDC_ANIMOPTS_DELAY), 0, 65535, _R(IDS_BMPPREFS_INVALIDDELAY) ,&Valid);
									 
					if (Valid)
					{
						pBmpDlgParam->SetAnimDelay(Delay);
						pBmpDlgParam->SetAreDelayValuesSame(TRUE);

						UINT32 ListSize = pGIFOptions->GetListSize();
						KernelBitmap** pListOfBitmaps = pGIFOptions->GetBitmapList();

						ERROR3IF(pListOfBitmaps == NULL && ListSize > 0,"Do what");

						// A new delay value has been set, store it in the bitmaps.
			
						if (pListOfBitmaps != NULL)
						{
							while (ListSize > 0)
							{
								KernelBitmap* pBitmap = pListOfBitmaps[--ListSize] ;	

								if(pBitmap != NULL)
								{
									pBitmap->SetDelay(pBmpDlgParam->GetAnimDelay());
								}
							}
						}
					}
					else
						return FALSE;	
				}
		
				// Get the Animation Restore details.
				if (HasRestoreChanged)
				{
					INT32 Restore  = GetSelectedValueIndex(_R(IDC_ANIMOPTS_REMOVAL));
								
					if(Restore != OPTION_FOUR)
					{
						pBmpDlgParam->SetRestoreType((GIFDisposalMethod) Restore);
						pBmpDlgParam->SetSameRestoreType(TRUE);
						UINT32 ListSize = pGIFOptions->GetListSize();

						KernelBitmap** pListOfBitmaps = pGIFOptions->GetBitmapList();
						ERROR3IF(pListOfBitmaps == NULL && ListSize > 0,"Do what");

						// A new Restore type has been entered, store it in the bitmaps.
						if (pListOfBitmaps != NULL)
						{
							while (ListSize > 0)
							{
								KernelBitmap* pKernelBitmap = pListOfBitmaps[--ListSize] ;	

								if (pKernelBitmap != NULL)
								{
									OILBitmap* pOILBitmap = pKernelBitmap->GetActualBitmap();

									if (pOILBitmap != NULL)
									{
										pOILBitmap->SetAnimationRestoreType(pBmpDlgParam->GetRestoreType());
	
									}
								}
							}
						}
					}
				}
				// Get the Animation Loop details.
				BOOL Value = GetBoolGadgetSelected(_R(IDC_ANIMOPTS_LOOP));
				INT32 Loop = GetLongGadgetValue(_R(IDC_ANIMOPTS_ITERATIONS), 0, 65535, _R(IDS_BMPPREFS_INVALIDLOOP) ,&Valid);
				if (Valid)
				{
					pGIFOptions->SetAnimationLoopCount(Loop, Value);
				}
				else
					return FALSE;	

			}
			break;
		}

		case BMP_UNCOMPRESSED:
		{
			BMPExportOptions* pBMPOptions = (BMPExportOptions*)pOptions;
			ERROR3IF(!pBMPOptions->IS_KIND_OF(BMPExportOptions), "pBMPOptions isn't");

			// WEBSTER - markn 23/1/97
			// Can't do RLE without Accusoft
			#ifndef WEBSTER
			// Get the state of the compression buttons but only allowed in 4 and 8 bpp	modes
			BMP_COMPRESSION Compression;
			State = GetLongGadgetValue(_R(IDC_BMPOPTS_RLE), 0, 1, 0, &Valid);
			if (State && (Depth == 4 || Depth == 8))
				Compression = BMP_RLE;		// button on
			else
				Compression = BMP_RGB;	// button off

			if (Valid)
			{
				pBMPOptions->SetCompression(Compression);
			}
			#endif // WEBSTER

			pBMPOptions->SetCompression(BMP_RGB);

			pBMPOptions->SetDither(DitherType);
			pBMPOptions->SetPalette(Palette);
			break;
		}

		// The remaining AccusoftOnes:
		case WPG:
		case PHOTOCD:
		case IFF_ILBM:
		case XWD:
		case DCX:
		case PCX:
		case PICT:
		case RAST:
		case PHOTOSHOP:
		case TARGA:
		case MACPAINT:
		case MSP:
		{
			//WEBSTER-Martin-03/01/97
#ifndef WEBSTER
			AccusoftExportOptions* pAccyOptions = (AccusoftExportOptions*)pOptions;
			ERROR3IF(!pAccyOptions->IS_KIND_OF(AccusoftExportOptions), "pAccyOptions isn't");
		
			pAccyOptions->SetDither(DitherType);
			pAccyOptions->SetPalette(Palette);
#endif //WEBSTER
			break;
		}
			
		default:
			// At present, only BMP and TIFFs can change the compression type.
			// Just us mice here
			break;
	}

	// Get the state of the selection, drawing or spread buttons
	SelectionType FoundSelection = GetSelection();
	pOptions->SetSelectionType(FoundSelection);

	return TRUE;
}


/******************************************************************************************

>	MsgResult BmpPrefsDlg::Message(CDlgMessage DlgMsg, CGadgetID Gadget) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/08/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the bitmap preferences dialog's messages
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

MsgResult BmpPrefsDlg::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*) Message;
		MsgResult Result;
		BOOL EndDialog = FALSE;		// TRUE if we should quit the dialog

		BitmapExportOptions* pOptions = GetOptions();
		ERROR3IF(pOptions == NULL, "BmpPrefsDlg::Message - Parameters not been set up");

		// Should now handle the required messages that we respond to
		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				{
					// Make sure the updating variable is set to something sensible
					RecalculatingPixels = FALSE;
					RecalculatingXDpi = FALSE;
					RecalculatingYDpi = FALSE;

					// As it is a modal dialog box we are sent a message to say the dialog box
					// is being created, so we have a chance to set the initial control values
					// JPEG export has a special dialog box, all others use the same one 
					InitDialog();
					HasDelayChanged = FALSE;
					HasRestoreChanged = FALSE;
				}
			break;

			case DIM_COMMIT:
				{
					// (ok) accept all changes that the user has made to the settings
					// brackets make the ok local to this case 
					// JPEG export has a special dialog box, all others use the same one 
					BOOL ok;
					ok = CommitDialogValues();
					if (ok)
					{
						// Values found ok so quit dialogue then go ahead and export
						GetOptions()->MarkValid();		  	// Flag ok used
						EndDialog = TRUE;				// Flag to close and end
					}
				}
			break;

			case DIM_CANCEL:
				// Cancel all changes that the user has made to the settings
				GetOptions()->MarkInvalid();			// Flag cancel used
				EndDialog = TRUE;						// Flag to close and end
			break;

			case DIM_LISTDROPPED:
				{
					
					HasRestoreChanged = TRUE;

					if(IsManyDisplayed)
					{
						DeleteAllValues(_R(IDC_ANIMOPTS_REMOVAL));	
						SetStringGadgetValue(_R(IDC_ANIMOPTS_REMOVAL), _R(IDS_RESTORE_NOTHING));
						SetStringGadgetValue(_R(IDC_ANIMOPTS_REMOVAL), _R(IDS_RESTORE_LEAVEASIS));
						SetStringGadgetValue(_R(IDC_ANIMOPTS_REMOVAL), _R(IDS_RESTORE_BACKGROUND));
						SetStringGadgetValue(_R(IDC_ANIMOPTS_REMOVAL), _R(IDS_RESTORE_PREVIOUS));
						SetStringGadgetValue(_R(IDC_ANIMOPTS_REMOVAL), _R(IDS_MANY));
						SetSelectedValueIndex(_R(IDC_ANIMOPTS_REMOVAL), OPTION_FOUR);
						SetComboListLength(_R(IDC_ANIMOPTS_REMOVAL));
						IsManyDisplayed = FALSE;
					}
				}
			break;

			case DIM_LFT_BN_CLICKED:

				// If they clicked on a "help" button then invoke the help system for this dialog.
				if (Msg->GadgetID == _R(ID_CC_HELP_BUTTON))
				{
					UINT32 HelpID = GetFilterHelpID(GetOptions()->GetFilterType());

					if (HelpID > 0)
					{
						// Invoke the help system and mark this message as completely processed.
						HelpUserTopic(HelpID);
						return DLG_EAT_IF_HUNGRY(Msg);
					}
				}
				// A control on the dialog box has been clicked...
				switch (Msg->GadgetID)
				{
					if (pOptions->GetFilterType() != JPEG)
					{
						// Uses a generic routine which checks to see if the chosen option means
						// that other options become greyed or ungreyed
						// case _R(IDC_BMPOPTS_32BPP): // 32 bpp case no longer exists. Why? Some spappy reason or other.
						case _R(IDC_BMPOPTS_CMYK):
						case _R(IDC_BMPOPTS_24BPP):
						case _R(IDC_BMPOPTS_8BPP):
						case _R(IDC_BMPOPTS_4BPP):
						case _R(IDC_BMPOPTS_1BPP):
							SetBPPButtons(Msg->GadgetID,TRUE);
						break;

						case _R(IDC_BMPOPTS_PAL_STD):
						case _R(IDC_BMPOPTS_PAL_OPT):
							SetPaletteButtons(Msg->GadgetID);
						break;

						case _R(IDC_BMPOPTS_NODITHER):
						case _R(IDC_BMPOPTS_ORDDITHER):
						case _R(IDC_BMPOPTS_DIFFUSION):
							SetDitherButtons(Msg->GadgetID);
						break;
						case _R(IDC_ANIMOPTS_LOOP):
						{
							BOOL Value = GetBoolGadgetSelected(_R(IDC_ANIMOPTS_LOOP));
							if (Value)
							{
								SetLoopCheckBox(TRUE);
								SetBmpPrefsDlgState();
							}
							else
							{
								SetLoopCheckBox(FALSE);
								SetBmpPrefsDlgState();
							}
						}
						break;
					}
					// All other buttons should take care of themselves
					// Apart from if the user changes the selection/spread/drawing buttons
					// we must go and recalculate the sizes and resolution on display.  
					case _R(IDC_BMPOPTS_DRAWING):
					case _R(IDC_BMPOPTS_SELECT):
						RecalculateSize();
					break;
				}
			break; // DIM_LFT_BN_CLICKED		

			case DIM_SELECTION_CHANGED:
				// A control on the dialog box has been typed in...
				switch (Msg->GadgetID)
				{
					case _R(IDC_BMPOPTS_RES):
						RecalculatePixels();
					break;
				}
			break; // DIM_SELECTION_CHANGED		

			case DIM_TEXT_CHANGED:
				// A control on the dialog box has been typed in...
				switch (Msg->GadgetID)
				{
					case _R(IDC_BMPOPTS_RES):
						RecalculatePixels();
					break;
					case _R(IDC_BMPOPTS_XPIXELS):
						RecalculateDpiFromX();
					break;
					case _R(IDC_BMPOPTS_YPIXELS):
						RecalculateDpiFromY();
					break;
					case _R(IDC_ANIMOPTS_DELAY):
						HasDelayChanged = TRUE;
					break;
				}
			break; // DIM_TEXT_CHANGED		

		}

		// Allow the base class access to the message, it will do the
		// DLG_EAT_IF_HUNGRY(Msg) for us
		// Must do this before the Close and End
		Result = DialogOp::Message(Message);

		// End dialog here
		if (EndDialog) 
		{
			Close();				// Hide the dialog box
			End();					// Finish the operation
		}

		// The message was for our dialog box so return that we have handled it, if necessary
		//return (DLG_EAT_IF_HUNGRY(Msg)); 
		return Result;
	}

	return DialogOp::Message(Message); 
}

/******************************************************************************************

>	UINT32 BmpPrefsDlg::GetFilterHelpID(UINT32 FilterType)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Gets the Help Topic associated with the specified bitmap filter.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

UINT32 BmpPrefsDlg::GetFilterHelpID(UINT32 FilterType)
{
	UINT32 HelpID = 0;

	switch (FilterType)
	{
		case MAKE_BITMAP_FILTER:
			HelpID = _R(IDH_Command_Create_Bitmap_Copy);
			break;

		case JPEG:
			HelpID = _R(IDH_Dialog_Bitmap_Export_JPEG);
			break;

		case GIF:
		case TI_GIF:
			HelpID = _R(IDH_Dialog_Bitmap_Export_GIF);
			break;

		case TI_GIF_ANIM:
			HelpID = _R(IDH_Dialog_Bitmap_Export_Animated_GIF);
			break;

		case BMP_COMPRESSED:
		case BMP_UNCOMPRESSED:
			HelpID = _R(IDH_Dialog_Bitmap_Export_BMP);
			break;
	}

	return HelpID;
}

/******************************************************************************************

>	OpState	BmpPrefsDlg::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	BmpPrefsDlg GetState method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

OpState	BmpPrefsDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}


/******************************************************************************************

>	BOOL BmpPrefsDlg::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	BmpPrefsDlg Init method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL BmpPrefsDlg::Init()
{
	BOOL InitOK;

	InitOK = RegisterOpDescriptor(
								0,					/* Tool ID */
								_R(IDS_BMPPREFSDLG),
								CC_RUNTIME_CLASS(BmpPrefsDlg),
								OPTOKEN_BMPPREFSDLG,
								GetState,
								0,					/* help ID */
								0,  				/* bubble help */
								0,					/* resource ID */
								0					/* control ID */
								);

	if (InitOK)
	{
		InitOK = RegisterOpDescriptor(
								0,					/* Tool ID */
								_R(IDS_BMPPREFSDLG),
								CC_RUNTIME_CLASS(JPEGExportPrefsDialog),
								OPTOKEN_JPGPREFSDLG,
								GetState,
								0,					/* help ID */
								0,  				/* bubble help */
								0,					/* resource ID */
								0					/* control ID */
								);
	}
	return (InitOK);
}



/******************************************************************************************

>	static BOOL BmpPrefsDlg::InvokeDialog(BitmapExportOptions* pExportOptions)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/11/96
	Inputs:		pointer to some export options
	Returns:	True if Ok used or False if Cancel used.
	Purpose:	How to invoke or start a BmpPrefsDlg box.

******************************************************************************************/
BOOL BmpPrefsDlg::InvokeDialog(BitmapExportOptions* pExportOptions)
{
	ERROR2IF(pExportOptions == NULL, FALSE, "pExportOptions NULL");
	ERROR3IF(!pExportOptions->IS_KIND_OF(BitmapExportOptions), "pExportOptions isn't");

	IDD = pExportOptions->GetDialogType();

	OpDescriptor *OpDesc;
	if (IDD == _R(IDD_EXPORTJPEGOPTS))
		OpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_JPGPREFSDLG);
	else
		OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(BmpPrefsDlg));


	if (OpDesc == NULL)
	{
		ERROR3("BmpPrefsDlg::InvokeDialog is unable to find the BmpPrefsDlg OpDescriptor");
		pExportOptions->MarkInvalid();
	}
	else
	{
		OpDesc->Invoke((OpParam *)pExportOptions);
	}

	return pExportOptions->IsValid();
}



/********************************************************************************************

>	BOOL BmpPrefsDlg::Create()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	BmpPrefsDlg create method 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

	   
BOOL BmpPrefsDlg::Create()
{                            
	if (DialogOp::Create())
	{ 
		// Set the initial control values 
		// Dialog now Modal so set up happens in the message handler
		//InitDialog(this);

		return TRUE; 
    }
    else
    {
		return FALSE; 
	}
}




/******************************************************************************************

>	void BmpPrefsDlg::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void BmpPrefsDlg::Do(OpDescriptor*)
{
	BOOL ok;
	
	// Force the dialog box to be created, as it is modal it will be opened via a message
	ok = Create();

	if ( !ok )
	{
		// Could not create the dialog box so call inform error 
		InformError();
  		End(); 		   // End the operation 
	}
}

/******************************************************************************************

>	void BmpPrefsDlg::DoWithParam(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user and
				allows values to be passed in and returned to the caller via the
				BmpPrefsDlgParam class.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void BmpPrefsDlg::DoWithParam(OpDescriptor*, OpParam* pParam)
{
	if (pParam == NULL)
	{
		ERROR3("BmpPrefsDlg::DoWithParam - NULL Args");
		return;
	}

	// Use the OpParam that has been passed in to us
	ERROR3IF(!pParam->IS_KIND_OF(BitmapExportOptions), "pParam isn't");
	m_pOptions = (BitmapExportOptions*) pParam;

	BOOL ok;
	
	// Force the dialog box to be created, as it is modal it will be opened via a message
	ok = Create();

	if ( !ok )
	{
		// Could not create the dialog box so call inform error 
		InformError();
  		End(); 		   // End the operation 
	}
}


/******************************************************************************************

>	BOOL BmpPrefsDlg::SetUpResolutionList()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets up the initial values in the resolution/dpi combo box. 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL BmpPrefsDlg::SetUpResolutionList()
{
	if (!m_bDpiSupported)
	{
		SetStringGadgetValue(_R(IDC_BMPOPTS_RES), &String_8(" -"), FALSE, -1);
		return TRUE;
	}

	// First, set up the resolution combo box
	// Set up the default value in the editable field
	SetLongGadgetValue(_R(IDC_BMPOPTS_RES), (INT32)GetOptions()->GetDPI(), FALSE, -1);
	// Set up the list
	SetLongGadgetValue(_R(IDC_BMPOPTS_RES), 50);
	SetLongGadgetValue(_R(IDC_BMPOPTS_RES), 75);
	SetLongGadgetValue(_R(IDC_BMPOPTS_RES), 96);
	SetLongGadgetValue(_R(IDC_BMPOPTS_RES), 100);
	SetLongGadgetValue(_R(IDC_BMPOPTS_RES), 150);
	SetLongGadgetValue(_R(IDC_BMPOPTS_RES), 200);
	SetLongGadgetValue(_R(IDC_BMPOPTS_RES), 250);
	SetLongGadgetValue(_R(IDC_BMPOPTS_RES), 300);
	SetComboListLength(_R(IDC_BMPOPTS_RES));
	
	return TRUE;
}	

/******************************************************************************************

>	BOOL BmpPrefsDlg::SetBmpPrefsDlgState()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	21/01/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE.
	Purpose:	This will be called When the "loop continuously" switch is turned off.
				It will grey/ungrey the "Loop" ed field. 
	Errors:		
	SeeAlso:	-

******************************************************************************************/
BOOL BmpPrefsDlg::SetBmpPrefsDlgState()
{
	BitmapExportOptions* pOptions = GetOptions();
	ERROR2IF(pOptions == NULL, FALSE, "pOptions is NULL");
	
	// The "loop continuously" switch is on.
	if (GetLoopCheckBox())
	{
		// Grey the Loop ed field.
		EnableGadget(_R(IDC_ANIMOPTS_ITERATIONS), FALSE);
	}

	// The "loop continuously" switch is off. 
	if (!GetLoopCheckBox())
	{
		GIFExportOptions* pGIFOptions = (GIFExportOptions*)pOptions;
		ERROR3IF(!pGIFOptions->IS_KIND_OF(GIFExportOptions), "pGIFOptions isn't");

		// Ungrey the Loop ed field.	
		EnableGadget(_R(IDC_ANIMOPTS_ITERATIONS), TRUE);
		SetLongGadgetValue(_R(IDC_ANIMOPTS_ITERATIONS), pGIFOptions->GetAnimationLoopCount());
	}
	return TRUE;
}

/******************************************************************************************

>	BOOL BmpPrefsDlg::InitDialog()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Neville)
	Created:	23/08/94
	Inputs:		pointer to the dialog box
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial dialog values 

******************************************************************************************/
BOOL BmpPrefsDlg::InitDialog()
{
	BitmapExportOptions* pOptions = GetOptions();
	ERROR2IF(pOptions == NULL, FALSE, "pOptions NULL");

	if (pOptions->GetFilterType() != MAKE_BITMAP_FILTER)
	{
		// Set up the title of the dialog box according to the passed in string which
		// is the name of the filter plus export bitmap options.
		String_256 Temp = *(pOptions->GetFilterName());
		Temp += String_256(_R(IDN_EXPORTBMPOPTS));

		SetTitlebarName(&Temp); 
	}

	// Default to a locked aspect ratio
	m_bLockAspect = TRUE;

	// Now set up the bits per pixel radio buttons
	// The various filters require different controls, most just require bits disabling
	// such as the bpp switches available
	UINT32 Depth = pOptions->GetDepth();
	BOOL bYoDither	= FALSE;
	PALETTE Palette;	// maybe want this pileo'sh'
	DITHER Dither;	// maybe want this pileo'sh'

	// WEBSTER - markn 23/1/97
	// Hide these in Webster
#ifdef WEBSTER
	HideGadget(_R(IDC_BMPOPTS_TIFF), TRUE);		// Hide the TIFF compression selection 
	HideGadget(_R(IDC_BMPOPTS_RLE), TRUE);		// Hide the BMP rgb compression
	HideGadget(_R(IDC_BMPOPTS_RGB), TRUE);		// Hide the BMP rle compression
	HideGadget(_R(IDC_BMPOPTS_COMPRESS), TRUE);	// Hide the compression group
#endif // WEBSTER

	// WEBSTER - markn 17/1/97
	// Display the 'number of colours in palette' group and extend the dlg if necessary
	// but only for export options of the correct type
	InitNumColoursGroup(pOptions);

	// oh ye virtuals
	switch ( pOptions->GetFilterType() ) 
	{
		case TIFF_UNCOMPRESSED:
		{
			//WEBSTER-Martin-03/01/97
#ifndef WEBSTER
			BOOL ok;
			TIFFExportOptions* pTIFFOptions = (TIFFExportOptions*)pOptions;
			ERROR3IF(!pTIFFOptions->IS_KIND_OF(TIFFExportOptions), "pTIFFOptions isn't");

			// Make sure we show the correct compression selection field
			HideGadget(_R(IDC_BMPOPTS_TIFF), FALSE);	// Show the TIFF compression selection 
			HideGadget(_R(IDC_BMPOPTS_RLE), TRUE);		// Hide the BMP rgb compression
			HideGadget(_R(IDC_BMPOPTS_RGB), TRUE);		// Hide the BMP rle compression
			HideGadget(_R(IDC_BMPOPTS_COMPRESS), FALSE);// Show the compression group

			// Check for illegal case of the the depth for this filter type
//			if (Depth == 32)
//				Depth = 24;

			HideGadget(_R(IDC_BMPOPTS_32BPP), TRUE);

			// Convert the Accusoft ID's into their equivalent list item numbers
			// If 1bpp then omit the 1bpp compression types to 1bpp images only.
			INT32 TiffCompression = pTIFFOptions->GetCompression();

			// Set up the TIFF compression types list
			// Now the resolution combo box and hence the default value in the editable field
			ok = SetLongGadgetValue(_R(IDC_BMPOPTS_TIFF), pTIFFOptions->GetCompression(), FALSE, -1);
			// Set up the list of available TIFF compressions. Must be the same as the list
			// in TiffTypes.
			// Assusoft seem to imply they support lots of save options but in fact they do not!
			SetStringGadgetValue(_R(IDC_BMPOPTS_TIFF), &String_32(_R(IDN_TIFF_NONE)));
			
			if( Depth==8 || Depth==24 || Depth==32 )
			{
				SetStringGadgetValue(_R(IDC_BMPOPTS_TIFF), &String_32(_R(IDN_TIFF_LZW)));
			}

			// Only allow the next options if 1bpp is selected as otherwise not appropriate
			if (Depth == 1)
			{
				// 1bpp export so enable these 1bpp compression types
				SetStringGadgetValue(_R(IDC_BMPOPTS_TIFF), &String_32(_R(IDN_TIFF_HUFFMAN)));
				SetStringGadgetValue(_R(IDC_BMPOPTS_TIFF), &String_32(_R(IDN_TIFF_G3FAX)));
				SetStringGadgetValue(_R(IDC_BMPOPTS_TIFF), &String_32(_R(IDN_TIFF_G4FAX)));
				SetStringGadgetValue(_R(IDC_BMPOPTS_TIFF), &String_32(_R(IDN_TIFF_PACKBITS)));
			}
			else
			{
				// Anything but 1bpp export.
				// Must ensure that only a valid compression type is allowed
				//if (TiffCompression != UNCOMPRESSED_TIFF && TiffCompression != LZW_TIFF)
				//	TiffCompression = LZW_TIFF;
			}
			SetComboListLength(_R(IDC_BMPOPTS_TIFF));

			ok = SetTIFFCompressionSelection( TiffCompression, Depth );

			// if the previously selected compression is now disabled, select NONE
			if( ! ok )
			{
				ok = SetTIFFCompressionSelection( TIFF_UNCOMPRESSED, Depth );
				
				// We should ALWAYS be able to select UNCOMPRESSED
				ERROR3IF( (!ok), "BmpPrefsDlg::InitDialog - could not select uncompressed" );
			}

			Palette = pTIFFOptions->GetPalette();
			Dither = pTIFFOptions->GetDither();
			bYoDither = TRUE;

			m_bDpiSupported = TRUE;
#endif //WEBSTER
			break;
		}
		case PNG:
		{
			PNGExportOptions* pPNGOptions = (PNGExportOptions*)pOptions;
			ERROR3IF(!pPNGOptions->IS_KIND_OF(PNGExportOptions), "pPNGOptions isn't");
			// 1, 4, 8, 24 and 32 bpp

			// WEBSTER - markn 23/1/97
			// Compression group always hidden in Webster
			#ifndef WEBSTER
			HideGadget(_R(IDC_BMPOPTS_TIFF), TRUE);		// Hide the TIFF compression selection 
			HideGadget(_R(IDC_BMPOPTS_RLE), TRUE);		// Hide the BMP rgb compression
			HideGadget(_R(IDC_BMPOPTS_RGB), TRUE);		// Hide the BMP rle compression
			HideGadget(_R(IDC_BMPOPTS_COMPRESS), TRUE);	// Hide the compression group
			#endif // WEBSTER

			HideGadget(_R(IDC_BMPOPTS_CMYK), TRUE);
			EnableGadget(_R(IDC_BMPOPTS_32BPP), FALSE);

			// We will use the special PNG radio buttons for interlaced and transparent.
			// These are hidden by default.
			// Now set up the list
			// Must correspond to the order PNG .. PNG_TRANSINTER defined in accuflts.h
			// Translate the compression type into the transparency and interlace states.
			BOOL Transparent = pPNGOptions->WantTransparent();
			BOOL Interlace 	 = pPNGOptions->WantInterlaced();
			// If we are at the deeper colour depths then transparency is not allowed
			if (Depth == 24 || Depth == 32)
				Transparent = FALSE;
			HideGadget(_R(IDC_BMPOPTS_GIFTYPE), FALSE);
			HideGadget(_R(IDC_BMPOPTS_TRANSPARENT), FALSE);
			HideGadget(_R(IDC_BMPOPTS_INTERLACED), FALSE);
			EnableGadget(_R(IDC_BMPOPTS_GIFTYPE), TRUE);
			EnableGadget(_R(IDC_BMPOPTS_INTERLACED), TRUE);
			SetLongGadgetValue(_R(IDC_BMPOPTS_TRANSPARENT), Transparent);
			SetLongGadgetValue(_R(IDC_BMPOPTS_INTERLACED), Interlace);

//			Palette = pPNGOptions->GetPalette();
			Dither = pPNGOptions->GetDither();
			bYoDither = TRUE;

			m_bDpiSupported = TRUE;
			break;
		}
		case GIF:
		case TI_GIF:
		case TI_GIF_ANIM:
		{
			GIFExportOptions* pGIFOptions = (GIFExportOptions*)pOptions;
			ERROR3IF(!pGIFOptions->IS_KIND_OF(GIFExportOptions), "pGIFOptions isn't");

			// 4 8 bpp only
			// WEBSTER - markn 23/1/97
			// Compression group always hidden in Webster
			#ifndef WEBSTER
			HideGadget(_R(IDC_BMPOPTS_TIFF), TRUE);		// Hide the TIFF compression selection 
			HideGadget(_R(IDC_BMPOPTS_RLE), TRUE);		// Hide the BMP rle compression
			HideGadget(_R(IDC_BMPOPTS_RGB), TRUE);		// Hide the BMP rgb compression
			HideGadget(_R(IDC_BMPOPTS_COMPRESS), TRUE);	// Hide the compression group
			#endif // WEBSTER

			m_bDpiSupported = FALSE;

			// Check for illegal case of the the depth for this filter type
			if (Depth == 32 || Depth == 24)
				Depth = 8;
			// Disable all illegal bpp buttons
			HideGadget(_R(IDC_BMPOPTS_CMYK), TRUE);
			EnableGadget(_R(IDC_BMPOPTS_32BPP), FALSE);
			EnableGadget(_R(IDC_BMPOPTS_24BPP), FALSE);

			// We will use the special GIF radio buttons for interlaced and transparent.
			// These are hidden by default.
			// But only if our special GIF filter.
			if (pOptions->GetFilterType() == TI_GIF ||
				pOptions->GetFilterType() == TI_GIF_ANIM)
			{
				// Now set up the list
				// Must correspond to the order TI_GIF .. TI_GIF_TRANSINTER defined in accuflts.h
				// Translate the compression type into the transparency and interlace states.

				BOOL Transparent = pGIFOptions->WantTransparent();
				BOOL Interlace 	 = pGIFOptions->WantInterlaced();
				HideGadget(_R(IDC_BMPOPTS_GIFTYPE), FALSE);
				HideGadget(_R(IDC_BMPOPTS_TRANSPARENT), FALSE);
				HideGadget(_R(IDC_BMPOPTS_INTERLACED), FALSE);
				EnableGadget(_R(IDC_BMPOPTS_GIFTYPE), TRUE);
				EnableGadget(_R(IDC_BMPOPTS_INTERLACED), TRUE);
				EnableGadget(_R(IDC_BMPOPTS_TRANSPARENT), TRUE);

				SetLongGadgetValue(_R(IDC_BMPOPTS_TRANSPARENT), Transparent);
				SetLongGadgetValue(_R(IDC_BMPOPTS_INTERLACED), Interlace);
			}
			if (pOptions->GetFilterType() == TI_GIF_ANIM)
			{
				HideGadget(_R(IDC_ANIMOPTS_ANIMGROUP) , FALSE);		// Un-hide the Animation Group
				HideGadget(_R(IDC_ANIMOPTS_REMOVAL)   , FALSE);
				HideGadget(_R(IDC_ANIMOPTS_DELAYFOR)  , FALSE);
				HideGadget(_R(IDC_ANIMOPTS_CS)        , FALSE);
				HideGadget(_R(IDC_ANIMOPTS_LOOPTEXT)  , FALSE);
				HideGadget(_R(IDC_ANIMOPTS_DELAY)     , FALSE);
				HideGadget(_R(IDC_ANIMOPTS_ITERATIONS), FALSE);
				HideGadget(_R(IDC_ANIMOPTS_TIMES)     , FALSE);
				HideGadget(_R(IDC_ANIMOPTS_LOOP)      , FALSE);
				HideGadget(_R(IDC_ANIMOPTS_RESTORE)   , FALSE);

				// Unlock the aspect ratio
				m_bLockAspect = FALSE;

				// ************************************************
				// This is Oily code.  Should be replaced with Dialogue Manager function call
				// Extend the dialogue height, so we can see the animation options

				WINDOWPLACEMENT Place;
				Place.length = sizeof(WINDOWPLACEMENT);
				if (GetWindowPlacement((HWND)WindowID, &Place))
				{
					Place.rcNormalPosition.bottom = Place.rcNormalPosition.bottom + (ANIMATION_EXTRABIT);
					SetWindowPlacement((HWND)WindowID, &Place);
				}

				// Added for Webster RanbirR 11\02\97
				BmpDlgParam* pBmpDlgParam = pOptions->GetBmpDlgParam();
				if (pBmpDlgParam == NULL)
					return FALSE;

				// Go through our list of bitmaps, to find if any are Transparent.

				// Get a list of our selected bitmaps.
				UINT32 ListSize =  pBmpDlgParam->GetListSize();
				KernelBitmap** pList = pBmpDlgParam->GetBitmapList();
				BOOL Transparent = FALSE;
				INT32 Index=0;

				// Added for Webster - RanbirR 11\02\97
				// Are any of our bitmaps Transparent ?
				while (ListSize > 0)
				{
					KernelBitmap* pKernelBitmap = pList[--ListSize];

					if (pKernelBitmap != NULL)
					{
						// Is this bitmap Transparent.
						if(pKernelBitmap->GetTransparencyIndex(&Index))
							Transparent = TRUE;
					}
				}

				HideGadget(_R(IDC_BMPOPTS_TRANSPARENT), FALSE);
				EnableGadget(_R(IDC_BMPOPTS_TRANSPARENT), TRUE);
				SetLongGadgetValue(_R(IDC_BMPOPTS_TRANSPARENT), Transparent);

				if (pBmpDlgParam->GetAreDelayValuesSame())
				{
					SetLongGadgetValue(_R(IDC_ANIMOPTS_DELAY), pBmpDlgParam->GetAnimDelay());	
					IsDelayMany=FALSE;
				}
				else
				{
					SetStringGadgetValue(_R(IDC_ANIMOPTS_DELAY), _R(IDS_MANY));	
					IsDelayMany=TRUE;
				}
				
				if (pBmpDlgParam->GetSameRestoreType())
				{
					SetStringGadgetValue(_R(IDC_ANIMOPTS_REMOVAL), &String_32(_R(IDS_RESTORE_NOTHING)));
					SetStringGadgetValue(_R(IDC_ANIMOPTS_REMOVAL), &String_32(_R(IDS_RESTORE_LEAVEASIS)));
					SetStringGadgetValue(_R(IDC_ANIMOPTS_REMOVAL), &String_32(_R(IDS_RESTORE_BACKGROUND)));
					SetStringGadgetValue(_R(IDC_ANIMOPTS_REMOVAL), &String_32(_R(IDS_RESTORE_PREVIOUS)));
					SetComboListLength(_R(IDC_ANIMOPTS_REMOVAL));
					SetSelectedValueIndex(_R(IDC_ANIMOPTS_REMOVAL), pBmpDlgParam->GetRestoreType());
				}
				else
				{
					SetStringGadgetValue (_R(IDC_ANIMOPTS_REMOVAL), _R(IDS_MANY));
					SetSelectedValueIndex(_R(IDC_ANIMOPTS_REMOVAL), OPTION_ONE);
					IsManyDisplayed = TRUE;
				}
		
				SetLongGadgetValue(_R(IDC_ANIMOPTS_ITERATIONS), pGIFOptions->GetAnimationLoopCount());
				EnableGadget(_R(IDC_ANIMOPTS_ITERATIONS), FALSE);
				SetBoolGadgetSelected(_R(IDC_ANIMOPTS_LOOP), TRUE);

			}

//			Palette = pGIFOptions->GetPalette();
			Dither = pGIFOptions->GetDither();
			bYoDither = TRUE;

			break;
		}
		case MAKE_BITMAP_FILTER:
		{
			MakeBitmapExportOptions* pMkBOptions = (MakeBitmapExportOptions*)pOptions;
			ERROR3IF(!pMkBOptions->IS_KIND_OF(MakeBitmapExportOptions), "pMkBOptions isn't");
			// 1 4 8 24 bpp
			HideGadget(_R(IDC_BMPOPTS_CMYK), TRUE);
			EnableGadget(_R(IDC_BMPOPTS_32BPP), FALSE);
			HideGadget(_R(IDC_BMPOPTS_CREATEOPTS), FALSE);
			HideGadget(_R(IDC_BMPOPTS_GIFTYPE), TRUE);
			// WEBSTER - markn 23/1/97
			// Compression group always hidden in Webster
			#ifndef WEBSTER
			HideGadget(_R(IDC_BMPOPTS_TIFF), TRUE);		// Hide the TIFF compression selection 
			HideGadget(_R(IDC_BMPOPTS_RLE), TRUE);		// Hide the BMP rle compression
			HideGadget(_R(IDC_BMPOPTS_RGB), TRUE);		// Hide the BMP rgb compression
			HideGadget(_R(IDC_BMPOPTS_COMPRESS), TRUE);	// Hide the compression group
			#endif // WEBSTER
			
			HideGadget(_R(IDC_ANIMOPTS_LOOP), TRUE);	
			HideGadget(_R(IDC_ANIMOPTS_DELAY), TRUE);	
			HideGadget(_R(IDC_ANIMOPTS_ITERATIONS), TRUE);
			HideGadget(_R(IDC_ANIMOPTS_REMOVAL), TRUE);	

			HideGadget(_R(IDC_TIMES_STATIC),TRUE);	
			HideGadget(_R(IDC_RESTORE_STATIC),TRUE);

			m_bDpiSupported = TRUE;

			// We will use the special GIF radio buttons for interlaced and transparent.
			// These are hidden by default.

			String_256 Temp(_R(IDS_MAKEBMPOPTS));
			SetTitlebarName(&Temp); 

			SetStringGadgetValue(IDOK, &String_32(_R(IDS_CREATEBMP)));
			SetStringGadgetValue(_R(IDC_BMPOPTS_SAVEAREA), &String_32(_R(IDS_CREATEFROM)));

			// Now set up the list
			// Must correspond to the order TI_GIF .. TI_GIF_TRANSINTER defined in outptgif.h
			// Translate the compression type into the transparency and interlace states.
			BOOL Transparent = pMkBOptions->WantTransparent();
			BOOL Interlace 	 = FALSE;
			HideGadget(_R(IDC_BMPOPTS_TRANSPARENT), FALSE);
			HideGadget(_R(IDC_BMPOPTS_INTERLACED), TRUE);
			EnableGadget(_R(IDC_BMPOPTS_INTERLACED), FALSE);
			EnableGadget(_R(IDC_BMPOPTS_TRANSPARENT), TRUE);

			SetLongGadgetValue(_R(IDC_BMPOPTS_TRANSPARENT), Transparent);

//			Palette = pMkBOptions->GetPalette();
			Dither = pMkBOptions->GetDither();
			bYoDither = TRUE;
			break;
		}
		case WPG:
			// 1 4 8 bpp only
			// WEBSTER - markn 23/1/97
			// Compression group always hidden in Webster
			#ifndef WEBSTER
			HideGadget(_R(IDC_BMPOPTS_TIFF), TRUE);		// Hide the TIFF compression selection 
			HideGadget(_R(IDC_BMPOPTS_RLE), TRUE);		// Hide the BMP rgb compression
			HideGadget(_R(IDC_BMPOPTS_RGB), TRUE);		// Hide the BMP rle compression
			HideGadget(_R(IDC_BMPOPTS_COMPRESS), TRUE);	// Hide the compression group
			#endif // WEBSTER

			// Check for illegal case of the the depth for this filter type
			if (Depth == 32 || Depth == 24)
				Depth = 8;
			m_bDpiSupported = FALSE;
			// Disable all illegal bpp buttons
			HideGadget(_R(IDC_BMPOPTS_CMYK), TRUE);
			EnableGadget(_R(IDC_BMPOPTS_32BPP), FALSE);
			EnableGadget(_R(IDC_BMPOPTS_24BPP), FALSE);

			//WEBSTER-Martin-06/01/97
			#ifndef WEBSTER
			Palette = ((AccusoftExportOptions*)pOptions)->GetPalette();
			Dither = ((AccusoftExportOptions*)pOptions)->GetDither();
			#endif //WEBSTER
			bYoDither = TRUE;

			break;
		case PHOTOCD:
			// 24 bpp only
			// WEBSTER - markn 23/1/97
			// Compression group always hidden in Webster
			#ifndef WEBSTER
			HideGadget(_R(IDC_BMPOPTS_TIFF), TRUE);		// Hide the TIFF compression selection 
			HideGadget(_R(IDC_BMPOPTS_RLE), TRUE);		// Hide BMP rgb compression
			HideGadget(_R(IDC_BMPOPTS_RGB), TRUE);		// Hide BMP rle compression
			HideGadget(_R(IDC_BMPOPTS_COMPRESS), TRUE);	// Hide compression group
			#endif // WEBSTER

			// Check for illegal case of the the depth for this filter type
			if (Depth != 24)
				Depth = 24;
			// Disable all illegal bpp buttons
			HideGadget(_R(IDC_BMPOPTS_CMYK), TRUE);
			EnableGadget(_R(IDC_BMPOPTS_8BPP), FALSE);
			EnableGadget(_R(IDC_BMPOPTS_4BPP), FALSE);
			EnableGadget(_R(IDC_BMPOPTS_1BPP), FALSE);
			m_bDpiSupported = TRUE;
			break;
		case IFF_ILBM:
		case XWD:
		case DCX:
		case PCX:
		case PICT:
			// 1 4 8 24 bpp
			// WEBSTER - markn 23/1/97
			// Compression group always hidden in Webster
			#ifndef WEBSTER
			HideGadget(_R(IDC_BMPOPTS_TIFF), TRUE);		// Hide the TIFF compression selection 
			HideGadget(_R(IDC_BMPOPTS_RLE), TRUE);		// Hide BMP rgb compression
			HideGadget(_R(IDC_BMPOPTS_RGB), TRUE);		// Hide BMP rle compression
			HideGadget(_R(IDC_BMPOPTS_COMPRESS), TRUE);	// Hide compression group
			#endif // WEBSTER

			// Check for illegal case of the the depth for this filter type
			if (Depth == 32)
				Depth = 24;
			// Disable all illegal bpp buttons
			HideGadget(_R(IDC_BMPOPTS_CMYK), TRUE);
			EnableGadget(_R(IDC_BMPOPTS_32BPP), FALSE);
			m_bDpiSupported = FALSE;

			//WEBSTER-Martin-06/01/97
			#ifndef WEBSTER
			Palette = ((AccusoftExportOptions*)pOptions)->GetPalette();
			Dither = ((AccusoftExportOptions*)pOptions)->GetDither();
			#endif // WEBSTER
			bYoDither = TRUE;
			break;

		case RAST:
		case PHOTOSHOP:
			// 1 8 24 bpp
			// WEBSTER - markn 23/1/97
			// Compression group always hidden in Webster
			#ifndef WEBSTER
			HideGadget(_R(IDC_BMPOPTS_TIFF), TRUE);		// Hide the TIFF compression selection 
			HideGadget(_R(IDC_BMPOPTS_RLE), TRUE);		// Hide BMP rgb compression
			HideGadget(_R(IDC_BMPOPTS_RGB), TRUE);		// Hide BMP rle compression
			HideGadget(_R(IDC_BMPOPTS_COMPRESS), TRUE);	// Hide compression group
			#endif // WEBSTER

			// Check for illegal case of the the depth for this filter type
			if (Depth == 32 || Depth == 4)
				Depth = 24;
			// Disable all illegal bpp buttons
			HideGadget(_R(IDC_BMPOPTS_CMYK), TRUE);
			EnableGadget(_R(IDC_BMPOPTS_32BPP), FALSE);
			EnableGadget(_R(IDC_BMPOPTS_4BPP), FALSE);
			m_bDpiSupported = FALSE;

			//WEBSTER-Martin-06/01/97
			#ifndef WEBSTER
			Palette = ((AccusoftExportOptions*)pOptions)->GetPalette();
			Dither = ((AccusoftExportOptions*)pOptions)->GetDither();
			#endif // WEBSTER
			bYoDither = TRUE;

			break;
		case TARGA:
			// 8 24 bpp
			// WEBSTER - markn 23/1/97
			// Compression group always hidden in Webster
			#ifndef WEBSTER
			HideGadget(_R(IDC_BMPOPTS_TIFF), TRUE);		// Hide the TIFF compression selection 
			HideGadget(_R(IDC_BMPOPTS_RLE), TRUE);		// Hide BMP rgb compression
			HideGadget(_R(IDC_BMPOPTS_RGB), TRUE);		// Hide BMP rle compression
			HideGadget(_R(IDC_BMPOPTS_COMPRESS), TRUE);	// Hide compression group
			#endif // WEBSTER

			// Check for illegal case of the the depth for this filter type
			if (Depth == 32  || Depth == 4 || Depth == 1)
				Depth = 24;
			// Disable all illegal bpp buttons
			HideGadget(_R(IDC_BMPOPTS_CMYK), TRUE);
			EnableGadget(_R(IDC_BMPOPTS_32BPP), FALSE);
			EnableGadget(_R(IDC_BMPOPTS_4BPP), FALSE);
			EnableGadget(_R(IDC_BMPOPTS_1BPP), FALSE);
			m_bDpiSupported = FALSE;

			//WEBSTER-Martin-06/01/97
			#ifndef WEBSTER
			Palette = ((AccusoftExportOptions*)pOptions)->GetPalette();
			Dither = ((AccusoftExportOptions*)pOptions)->GetDither();
			#endif // WEBSTER
			bYoDither = TRUE;
			break;

		case MACPAINT:
		case MSP:
			// 1bpp only
			return FALSE;
			break;
		case BMP_UNCOMPRESSED:
		{
			BMPExportOptions* pBMPOptions = (BMPExportOptions*)pOptions;
			ERROR3IF(!pBMPOptions->IS_KIND_OF(BMPExportOptions), "pBMPOptions isn't");

			// WEBSTER - markn 23/1/97
			// Compression group always hidden in Webster
			#ifndef WEBSTER
			HideGadget(_R(IDC_BMPOPTS_TIFF), TRUE);		// Hide the TIFF compression selection 
			HideGadget(_R(IDC_BMPOPTS_RLE), FALSE);		// Show the BMP rgb compression
			HideGadget(_R(IDC_BMPOPTS_RGB), FALSE);		// Show the BMP rle compression
			HideGadget(_R(IDC_BMPOPTS_COMPRESS), FALSE);// Show the compression group

			// Now set up the other buttons according to the values set in the variables
			if (pBMPOptions->GetCompression())
				SetLongGadgetValue(_R(IDC_BMPOPTS_RLE), TRUE);
			else
				SetLongGadgetValue(_R(IDC_BMPOPTS_RGB), TRUE);
			#endif // WEBSTER

			// Check for illegal case of the the depth for this filter type
			if ( Depth == 16 )
				Depth = 24;
			EnableGadget(_R(IDC_BMPOPTS_32BPP), FALSE);
			HideGadget(_R(IDC_BMPOPTS_CMYK), FALSE);
			Palette = pBMPOptions->GetPalette();
			Dither = pBMPOptions->GetDither();
			bYoDither = TRUE;

			m_bDpiSupported = TRUE;
			break;
		}
		default:
			return FALSE;
	}

	// Override the Export button if being invoked from the Name gallery.
	if (NamedExportProp::m_fApplyNotExport)
		SetStringGadgetValue(IDOK, &String(_R(IDS_PREVIEW_APPLY)));

	// If the default depth's not supported give a sensible one
	if (Depth != pOptions->GetDepth())
	{
		pOptions->SetDepth(Depth);
	}

	if (!m_bDpiSupported)							// Doesn't support dpi...
	{
		// and ensure that its always the number of dpi as defined by a pixel unit
		DocUnitList* pDocUnitList =	DocUnitList::GetCurrentDocUnitList();
		ERROR3IF(pDocUnitList == NULL, "BmpPrefsDlg::InitDialog() - no pDocUnitList!")
		Unit* pPixelUnit = pDocUnitList->FindUnit(PIXELS);
		ERROR3IF(pPixelUnit == NULL, "BmpPrefsDlg::InitDialog() - no pixel units!")
		Unit* pInchUnit = pDocUnitList->FindUnit(INCHES);
		ERROR3IF(pInchUnit == NULL, "BmpPrefsDlg::InitDialog() - no inch units!")
		double newDpi = (pPixelUnit->GetMillipoints()>0) ? pInchUnit->GetMillipoints() / pPixelUnit->GetMillipoints() : 96.0;
		Dpi = (INT32)newDpi;
		pOptions->SetDPI(newDpi);

		// Now, set up the resolution combo box
		SetUpResolutionList();

		// so disallow changes to dpi field
		EnableGadget(_R(IDC_BMPOPTS_RES), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_DPI), FALSE);
	}
	else
	{
		// Now, set up the resolution combo box
		SetUpResolutionList();
	}

	// blooooody classes!!!! foookin virtuals!!!
	InitDepthRadioGroup();
	InitSelectionRadioGroup();
	if (bYoDither)
	{
		InitPaletteRadioGroup(Palette);
		InitDitherRadioGroup(Dither);		// GetDitherGadget()->Init()???
	}

	// Make sure that size fields are displaying the values correctly
	RecalculateSize();

	if (m_bDpiSupported)
	{
		// Start off with input focus in the dpi field
		SetKeyboardFocus(_R(IDC_BMPOPTS_RES));
	}
	else
	{
		// Start off with input focus in the x pixel size
		SetKeyboardFocus(_R(IDC_BMPOPTS_XPIXELS));
	}

	return TRUE;
}

/******************************************************************************************

>	void BmpPrefsDlg::InitNumColoursGroup(BitmapExportOptions* pOptions)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/97
	Inputs:		pOptions = The options for this bitmap export
	Returns:	-
	Purpose:	Inits the "number of colour in palette" group in the bitmap export dlg

				NOTE: If the options don't allow a specific number of colours to be show,
				then this function will hide them instead

				Introduced in WEBSTER

******************************************************************************************/

void BmpPrefsDlg::InitNumColoursGroup(BitmapExportOptions* pOptions)
{
	if (pOptions == NULL)
		return;

	// Don't show these options if we can't support a specific number of colours
	if (!pOptions->UseSpecificNumColsInPalette())
	{
		HideNumColoursGroup();
		return;
	}

	INT32 Gadgets[] = {	_R(IDC_BMPOPTS_NUMCOLS_GROUP),
						_R(IDC_BMPOPTS_NUMCOLS_EDIT),
						_R(IDC_BMPOPTS_NUMCOLS_SYSCOLS),
						_R(IDC_BMPOPTS_NUMCOLS_EDITTEXT),
						0};

	// ************************************************
	// This is Oily code.  Should be replaced with Dialogue Manager function call
	// It moves the numcols gadgets into the correct place on the dialog and
	// extends the dialog so you can see them.  Lovely!

	{
		INT32 Index = 0;
		for (Index = 0;Gadgets[Index] != 0;Index++)
		{
			HWND hGadget = GetDlgItem((HWND)WindowID, (INT32)Gadgets[Index]);
			RECT MoveRect;
			POINT TopLeft;

			if (GetWindowRect(hGadget, &MoveRect))
			{
				TopLeft.x = MoveRect.left;		// Convert TopLeft coord into client coords
				TopLeft.y = MoveRect.top;
				ScreenToClient((HWND)WindowID, &TopLeft);
				TopLeft.y -= NUMCOLS_GROUP_SHIFT;

				SetWindowPos(hGadget, NULL,
								TopLeft.x, TopLeft.y, 0, 0,
								SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
			}
		}

		for (Index = 0;Gadgets[Index] != 0;Index++)
			HideGadget(Gadgets[Index],FALSE);

		WINDOWPLACEMENT Place;
		Place.length = sizeof(WINDOWPLACEMENT);
		if (GetWindowPlacement((HWND)WindowID, &Place))
		{
			Place.rcNormalPosition.bottom = Place.rcNormalPosition.bottom + (NUMCOLS_EXTRABIT);
			SetWindowPlacement((HWND)WindowID, &Place);
		}
	}

	// Init the gadgets
	INT32 NumCols = 0;
	BOOL UseSysCols = TRUE;
	NumCols		= pOptions->GetNumColsInPalette();
	UseSysCols	= pOptions->GetUseSystemColours();

	// Adjust in case we added the system colours
	if (UseSysCols)
		NumCols -= NUM_SYS_COLS;
  
	SetLongGadgetValue(_R(IDC_BMPOPTS_NUMCOLS_EDIT),NumCols);
	SetBoolGadgetSelected(_R(IDC_BMPOPTS_NUMCOLS_SYSCOLS),UseSysCols);
}

/******************************************************************************************

>	void BmpPrefsDlg::HideNumColoursGroup()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/97
	Inputs:		-
	Returns:	-
	Purpose:	Hides the "number of colour in palette" group in the bitmap export dlg

				Introduced in WEBSTER

******************************************************************************************/

void BmpPrefsDlg::HideNumColoursGroup()
{
	HideGadget(_R(IDC_BMPOPTS_NUMCOLS_EDIT)    ,TRUE);
	HideGadget(_R(IDC_BMPOPTS_NUMCOLS_SYSCOLS) ,TRUE);
	HideGadget(_R(IDC_BMPOPTS_NUMCOLS_GROUP)   ,TRUE);
	HideGadget(_R(IDC_BMPOPTS_NUMCOLS_EDITTEXT),TRUE);
}

/******************************************************************************************

>	void BmpPrefsDlg::UpdateNumColoursGroup(BOOL CheckValueRange)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/97
	Inputs:		CheckNumColValueRange = TRUE if the value in the num col ed field should be range checked.
	Returns:	-
	Purpose:	Enables/disables the "number of colour in palette" group in the bitmap export dlg

				It also makes sure that the number in the ed field is in range

				Introduced in WEBSTER

******************************************************************************************/

void BmpPrefsDlg::UpdateNumColoursGroup(BOOL CheckValueRange)
{
	BOOL OptPal = GetBoolGadgetSelected(_R(IDC_BMPOPTS_PAL_OPT));
	BOOL Bpp8   = GetBoolGadgetSelected(_R(IDC_BMPOPTS_8BPP));
	BOOL Bpp4   = GetBoolGadgetSelected(_R(IDC_BMPOPTS_4BPP));

	EnableGadget(_R(IDC_BMPOPTS_NUMCOLS_EDIT)    ,OptPal && (Bpp8 || Bpp4));
	EnableGadget(_R(IDC_BMPOPTS_NUMCOLS_EDITTEXT),OptPal && (Bpp8 || Bpp4));
	EnableGadget(_R(IDC_BMPOPTS_NUMCOLS_SYSCOLS) ,OptPal && Bpp8);			// 8 bpp only
	EnableGadget(_R(IDC_BMPOPTS_NUMCOLS_GROUP)   ,TRUE);					// Always show group

	// Check that the value in the ed string is not out of range
	if ((Bpp8 || Bpp4))
	{
		BOOL SystemColours = (Bpp8 && GetBoolGadgetSelected(_R(IDC_BMPOPTS_NUMCOLS_SYSCOLS)));
		UINT32 Max = 256;
		UINT32 Min = 2;
		if (Bpp4)
			Max = 16;

		// If we are adding system colours, then the smallest number the user can enter is 0
		if (SystemColours)
			Min = 0;

		// Slight bodge fix here - changed so that every time the user changes the bpp, it resets
		// the number of colours to the max colours available
		// Comment this line out if you don't want this functionality
		if (CheckValueRange)
			Min = Max;

		UINT32 NumCols = GetLongGadgetValue(_R(IDC_BMPOPTS_NUMCOLS_EDIT), Min, Max);
		if (NumCols > Max)	NumCols = Max;
		if (NumCols < Min)	NumCols = Min;
		SetLongGadgetValue(_R(IDC_BMPOPTS_NUMCOLS_EDIT),NumCols);
	}
}

/******************************************************************************************

>	BOOL BmpPrefsDlg::SetNumColoursInPalette(BitmapExportOptions* pOptions)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/97
	Inputs:		pOptions = ptr to the export options to use
	Returns:	-
	Purpose:	Extracts the number of colours in palette & the system colours switch data
				from the relevant gadgets, and stuffs the data in pOptions.

				Introduced in WEBSTER

******************************************************************************************/

BOOL BmpPrefsDlg::SetNumColoursInPalette(BitmapExportOptions* pOptions)
{
	// Don't bother unless the user wants an optimised palette
	if (!GetBoolGadgetSelected(_R(IDC_BMPOPTS_PAL_OPT)))
		return TRUE;

	// Only do this for filters that support this feature
	if (pOptions != NULL && pOptions->UseSpecificNumColsInPalette())
	{
		// Set the default values for an 8bpp bitmap
		UINT32 MaxNumCols = 256;
		UINT32 MinNumCols = 2;
		BOOL SystemColours = GetBoolGadgetSelected(_R(IDC_BMPOPTS_NUMCOLS_SYSCOLS));
		UINT32 Depth = pOptions->GetDepth();
		BOOL Valid;

		// 4bpp has a max num colours of 16, and can't include the system colours
		if (Depth == 4)
		{
			MaxNumCols = 16;
			SystemColours = FALSE;
		}

		// If we are adding system colours, then the smallest number the user can enter is 0
		if (SystemColours)
			MinNumCols = 0;

		UINT32 NumCols = GetLongGadgetValue(_R(IDC_BMPOPTS_NUMCOLS_EDIT), MinNumCols, MaxNumCols, 0, &Valid);

		if (!Valid)
		{
			// Only report an err message in 8bpp & 4bpp modes
			if ((Depth == 8) || (Depth == 4))
			{
				String_256 ErrMsg;
				ErrMsg.MakeMsg(_R(IDS_BMPPREFS_NUMCOLSWRONG),MinNumCols,MaxNumCols);
				Error::SetError(0,ErrMsg,0);
				InformError();
				return FALSE;
			}
			NumCols = MaxNumCols;
		}

		// If the user wants to add the system colours, then we need to increase the number of 
		// total colours to create

		// The number of sys colours is either NUM_SYS_COLS, or 0 if the switch is off
		UINT32 NumSysCols = NUM_SYS_COLS;
		if (!SystemColours)
			NumSysCols = 0;

		NumCols += NumSysCols;
		if (NumCols > MaxNumCols)
			NumCols = MaxNumCols;

		// Are we to include a transparent colour?
		UINT32 NumTransparentCols = 0;
		if (GetBoolGadgetSelected(_R(IDC_BMPOPTS_TRANSPARENT)))
			NumTransparentCols = 1;

		// Can we reduce the number of bpp automatically?
		// Do it only for optimised palettes
		if (Depth == 8)
		{
			// If the total number of colours is 16 or less, choose 4bpp
			if (NumCols <= (16-NumTransparentCols))
				GetOptions()->SetDepth(4);

			// If the total number of colours is 2 or less, choose 1bpp
			if (NumCols <= (2-NumTransparentCols))
				GetOptions()->SetDepth(1);
		}

		pOptions->SetNumColsInPalette(NumCols);
		pOptions->SetUseSystemColours(SystemColours);
	}

	return TRUE;
}

void BmpPrefsDlg::InitDepthRadioGroup()
{
	// Set up the Bits per pixel buttons
	// Filtered above for illegal cases
	switch ( GetOptions()->GetDepth() ) 
	{
		case 1:
			SetBPPButtons(_R(IDC_BMPOPTS_1BPP),FALSE);
			SetLongGadgetValue(_R(IDC_BMPOPTS_1BPP), TRUE);
			LastBppSelected = _R(IDC_BMPOPTS_1BPP);
			break;
		case 4:
			SetBPPButtons(_R(IDC_BMPOPTS_4BPP),FALSE);
			SetLongGadgetValue(_R(IDC_BMPOPTS_4BPP), TRUE);
			LastBppSelected = _R(IDC_BMPOPTS_4BPP);
			break;
		case 8:
			SetBPPButtons(_R(IDC_BMPOPTS_8BPP),FALSE);
			SetLongGadgetValue(_R(IDC_BMPOPTS_8BPP), TRUE);
			LastBppSelected = _R(IDC_BMPOPTS_8BPP);
			break;
		case 24:
			SetBPPButtons(_R(IDC_BMPOPTS_24BPP),FALSE);
			SetLongGadgetValue(_R(IDC_BMPOPTS_24BPP), TRUE);
			LastBppSelected = _R(IDC_BMPOPTS_24BPP);
			break;
		default:
			SetBPPButtons(_R(IDC_BMPOPTS_CMYK),FALSE);
			SetLongGadgetValue(_R(IDC_BMPOPTS_CMYK), TRUE);
			LastBppSelected = _R(IDC_BMPOPTS_CMYK);
			break;
	}
}


void BmpPrefsDlg::InitPaletteRadioGroup(const PALETTE& Palette)
{
// WEBSTER - markn 17/1/97
// Calls SetPaletteButtons() to do the hard work now.

	switch (Palette)
	{
		case PAL_OPTIMISED:
			SetPaletteButtons(_R(IDC_BMPOPTS_PAL_OPT));
			break;

		case PAL_STANDARD:
			SetPaletteButtons(_R(IDC_BMPOPTS_PAL_STD));
			break;

		default:
			TRACE( _T("Non-standard palette set"));
			break;
	}
}


void BmpPrefsDlg::InitDitherRadioGroup(const DITHER& DitherType)
{
	// Set up the dithering selection radio buttons
	switch (DitherType)
	{
	 	case XARADITHER_NONE:
			SetLongGadgetValue(_R(IDC_BMPOPTS_NODITHER), TRUE);
			break;

	 	case XARADITHER_ORDERED_GREY:
			SetLongGadgetValue(_R(IDC_BMPOPTS_ORDDITHER), TRUE);
			break;

	 	case XARADITHER_ERROR_DIFFUSION:
			SetLongGadgetValue(_R(IDC_BMPOPTS_DIFFUSION), TRUE);
			break;

		default:
			SetLongGadgetValue(_R(IDC_BMPOPTS_DIFFUSION), TRUE);
			break;
	}

	if (GetOptions()->GetSelectionType() == ABITMAP)
	{
		// If we are outputing a bitmap directly, then we
		// don't need the palette and dithering options
		EnableGadget(_R(IDC_BMPOPTS_NODITHER), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_ORDDITHER), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_DIFFUSION), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_PAL_STD), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_PAL_OPT), FALSE);
	}
}


	// Set up the save spread/selection radio buttons
	// If selection is True then enable the spread/selection/drawing buttons otherwise
	// disable the selection button and allow choice between spread or drawing, drawing
	// being the default.
void BmpPrefsDlg::InitSelectionRadioGroup()
{
	//  Always disable this button.
	EnableGadget(_R(IDC_BMPOPTS_SPREAD), FALSE);

	// Set up the save spread/selection radio buttons
	if (GetOptions()->GetFilterType() == MAKE_BITMAP_FILTER ||
		GetOptions()->GetFilterType() == TI_GIF_ANIM)
	{
		SetLongGadgetValue(_R(IDC_BMPOPTS_SELECT), TRUE);
		// Disable all buttons
		EnableGadget(_R(IDC_BMPOPTS_SELECT), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_DRAWING), FALSE);
	}
	else
	{
		// The drawing option is always available.
		EnableGadget(_R(IDC_BMPOPTS_DRAWING), TRUE);

		// If selection is True then enable the spread/selection/drawing buttons otherwise
		// disable the selection button and allow choice between spread or drawing, drawing
		// being the default.
		switch (GetOptions()->GetSelectionType())
		{
			case SELECTION:
				// If selection present then choose export selection by default
				SetLongGadgetValue(_R(IDC_BMPOPTS_SELECT), TRUE);

				// Selection present, so enable that button.
				EnableGadget(_R(IDC_BMPOPTS_SELECT), TRUE);
				break;

			default:
				// If no selection present then choose export drawing by default		
				SetLongGadgetValue(_R(IDC_BMPOPTS_DRAWING), TRUE);

				// No selection present, so let all options be used
				EnableGadget(_R(IDC_BMPOPTS_SELECT), FALSE);
				break;
		}
	}

	// Finally, smash it over the head with a bodge to disable the whole
	// group of gadgets when being invoked from the Name gallery.
	if (NamedExportProp::m_fApplyNotExport)
	{
		EnableGadget(_R(IDC_BMPOPTS_SAVEAREA), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_SELECT), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_DRAWING), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_SPREAD), FALSE);
	}
}


/******************************************************************************************

>	BitmapExportOptions* BmpPrefsDlg::GetOptions() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/96
	Returns:	A pointer to the BitmapExportOptions to be filled in by this dialog
	Purpose:	Helper function

******************************************************************************************/
BitmapExportOptions* BmpPrefsDlg::GetOptions() const
{
	return m_pOptions;
}

/******************************************************************************************
*** Photo CD import options dialog box
******************************************************************************************/



/******************************************************************************************

>	BOOL PhotoCDDlg::InvokeDialog(UINT32 * Page) 
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		Number of image pages available in the file
	Outputs:	Chosen page from the selection of pages
	Returns:	True if Ok used or False if Cancel used.
	Purpose:	How to invoke or start a PhotoCDDlg box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PhotoCDDlg::InvokeDialog(UINT32 * Page)
{
	// Params is defined to be the PhotoCDDlgParams class to nicely encapsulate
	// all the parameters that we need to pass back and forth.
	PhotoCDDlgParam Params(*Page);
	pParams = &Params;			// Save a reference to the params for use in the dialog code

	Params.ImageNumber = 0;		// Set default page to be zero
	Params.PhotoCDOk = FALSE;	// Ensure ok is False by default

	OpDescriptor *OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(PhotoCDDlg));

	ERROR3IF(OpDesc == NULL,"PhotoCDDlg::InvokeDialog is unable to find the PhotoCDDlg OpDescriptor");

	if (OpDesc != NULL)
		OpDesc->Invoke((OpParam *)&Params);

	// If ok used then alter the page to the one that the user has chosen
	if (Params.PhotoCDOk)
		*Page = Params.ImageNumber;

	return Params.PhotoCDOk;
}

/******************************************************************************************

>	PhotoCDDlg::PhotoCDDlg(): DialogOp(PhotoCDDlg::IDD, PhotoCDDlg::Mode) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PhotoCDDlg constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

PhotoCDDlg::PhotoCDDlg(): DialogOp(PhotoCDDlg::IDD, PhotoCDDlg::Mode) 
{
	// At present, do nothing.
}

/******************************************************************************************

>	BOOL PhotoCDDlg::CommitDialogValues(BmpPrefsDlg* pBmpPrefs)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		pointer to the dialogue box
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the dialog box and sets the return values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PhotoCDDlg::CommitDialogValues(PhotoCDDlg* pPhotoCDPrefs)
{
	ERROR3IF(pParams == NULL, "PhotoCDDlg::CommitDialogValues called after duff initialisation?!");

	// Ok has been pressed so take the values and set up the static values so that the
	// caller can access them

	// Get the image that the user has chosen from the radio buttons
	BOOL Valid = 0;		// Flag for validity of value
	BOOL State = 0;
	UINT32 Page = 0;
	State = GetLongGadgetValue(_R(IDC_PHOTOCD_BASE), 0, 1, 0, &Valid);
	if (State)
		Page = 0;
	State = GetLongGadgetValue(_R(IDC_PHOTOCD_BASEDIV4), 0, 1, 0, &Valid);
	if (State)
		Page = 1;
	State = GetLongGadgetValue(_R(IDC_PHOTOCD_BASEDIV16), 0, 1, 0, &Valid);
	if (State)
		Page = 2;
	State = GetLongGadgetValue(_R(IDC_PHOTOCD_BASE4), 0, 1, 0, &Valid);
	if (State)
		Page = 3;
	State = GetLongGadgetValue(_R(IDC_PHOTOCD_BASE16), 0, 1, 0, &Valid);
	if (State)
		Page = 4;
	pParams->ImageNumber = Page;

	return TRUE;
}


/******************************************************************************************

>	MsgResult PhotoCDDlg::Message(CDlgMessage DlgMsg, CGadgetID Gadget) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/08/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the photocd import options dialog's messages
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

MsgResult PhotoCDDlg::Message(Msg* Message)
{
	ERROR3IF(pParams == NULL, "PhotoCDDlg::Message - Parameters not been set up");

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		MsgResult Result;

		// Main handler for dialog type messages
		BOOL EndDialog = FALSE;						// TRUE if we should quit the dialog

		// Should now handle the required messages that we respond to
		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				// As it is a modal dialog box we are sent a message to say the dialog box
				// is being created, so we have a chance to set the initial control values.
				InitDialog(this);
			break;

			case DIM_COMMIT:
				// (ok) accept all changes that the user has made to the settings
				// brackets make the ok local to this case 
				{
					BOOL ok;
					ok = CommitDialogValues(this);
					if (ok)
					{
						// Values found ok so quit dialogue then go ahead and export
						pParams->PhotoCDOk = TRUE;		  	// Flag ok used
						EndDialog = TRUE;
					}
				}
			break;

			case DIM_CANCEL:
				// Cancel all changes that the user has made to the settings
				pParams->PhotoCDOk = FALSE;		  		// Flag cancel used
				EndDialog = TRUE;
			break;
		}

		// Allow the base class access to the message, it will do the
		// DLG_EAT_IF_HUNGRY(Msg) for us
		// Must do this before the Close and End
		Result = DialogOp::Message(Message);

		// End dialog here
		if (EndDialog) 
		{
			Close();				// Hide the dialog box
			End();					// Finish the operation
		}

		// The message was for our dialog box so return that we have handled it, if necessary
		return Result;
	}
	return DialogOp::Message(Message); 
}

/******************************************************************************************

>	OpState	PhotoCDDlg::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PhotoCDDlg GetState method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

OpState	PhotoCDDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}


/******************************************************************************************

>	BOOL PhotoCDDlg::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PhotoCDDlg Init method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PhotoCDDlg::Init()
{
	BOOL InitOK;

	InitOK = RegisterOpDescriptor(
								0,					/* Tool ID */
								_R(IDS_PHOTOCDDLG),
								CC_RUNTIME_CLASS(PhotoCDDlg),
								OPTOKEN_PHOTOCDDLG,
								GetState,
								0,					/* help ID */
								0,  				/* bubble help */
								0,					/* resource ID */
								0					/* control ID */
								);

	return (InitOK);
}


/********************************************************************************************

>	BOOL PhotoCDDlg::Create()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	PhotoCDDlg create method 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	   
BOOL PhotoCDDlg::Create()
{                            
	if (DialogOp::Create())
	{ 
		// Set the initial control values 
		// Dialog now Modal so set up happens in the message handler
		//InitDialog(this);

		return TRUE; 
    }
    else
    {
		return FALSE; 
	}
}

/******************************************************************************************

>	void PhotoCDDlg::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void PhotoCDDlg::Do(OpDescriptor*)
{
	ERROR3("PhotoCDDlg::Do called when should be using DoWithParam!");
	End(); 		   // End the operation 
}

/******************************************************************************************

>	void PhotoCDDlg::DoWithParam(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user and
				allows values to be passed in and returned to the caller via the
				PhotoCDDlgParam class.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void PhotoCDDlg::DoWithParam(OpDescriptor*, OpParam* Param)
{
	ERROR3IF(Param == NULL, "PhotoCDDlg::DoWithParam - NULL Param passed in");

	// Use the OpParam that has been passed in to us
	PhotoCDDlgParam * BmpDlgParam = (PhotoCDDlgParam *) Param;

	BOOL ok;
	
	// Force the dialog box to be created, as it is modal it will be opened via a message
	ok = Create();

	if ( !ok )
	{
		// Could not create the dialog box so call inform error 
		InformError();
  		End(); 		   // End the operation 
	}
}


/******************************************************************************************

>	BOOL PhotoCDDlg::InitDialog(PhotoCDDlg* pBmpPrefs)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		pointer to the dialog box
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial dialog values 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PhotoCDDlg::InitDialog(PhotoCDDlg* pBmpPrefs)
{
	ERROR3IF(pParams == NULL, "PhotoCDDlg::InitDialog called after duff initialisation?!");

	// Check how many pages were found and if necessary whether to grey some of the
	// buttons
	// Bodge the Base16 so that ungreys using same condition as the Base4 as once again
	// Accusoft are lying and tell us 4 images instead of 5.
	EnableGadget(_R(IDC_PHOTOCD_BASE16),	(pParams->NumberOfPages > 3)); //4));
	EnableGadget(_R(IDC_PHOTOCD_BASE4), 	(pParams->NumberOfPages > 3));
	EnableGadget(_R(IDC_PHOTOCD_BASEDIV16), (pParams->NumberOfPages > 2));
	EnableGadget(_R(IDC_PHOTOCD_BASEDIV4), 	(pParams->NumberOfPages > 1));
	EnableGadget(_R(IDC_PHOTOCD_BASE), 		(pParams->NumberOfPages > 0));

	switch (pParams->ImageNumber)
	{
		case 4:
			SetLongGadgetValue(_R(IDC_PHOTOCD_BASE16), TRUE);    
			break;
		case 3:
			SetLongGadgetValue(_R(IDC_PHOTOCD_BASE4), TRUE);    
			break;
		case 2:
			SetLongGadgetValue(_R(IDC_PHOTOCD_BASEDIV16), TRUE);
			break;
		case 1:
			SetLongGadgetValue(_R(IDC_PHOTOCD_BASEDIV4), TRUE); 
			break;
		case 0:
		default:
			SetLongGadgetValue(_R(IDC_PHOTOCD_BASE), TRUE);     
			break;
	}

	return TRUE;
}


/******************************************************************************************

>	JPEGExportPrefsDialog::JPEGExportPrefsDialog()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/96
	Purpose:	JPEGExportPrefsDialog constructor.

******************************************************************************************/

JPEGExportPrefsDialog::JPEGExportPrefsDialog(): BmpPrefsDlg() 
{
	m_OriginalSourcePresent = FALSE;
}


/******************************************************************************************

>	virtual BOOL JPEGExportPrefsDialog::InitDialog()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Neville)
	Created:	23/08/94
	Inputs:		pointer to the BmpPrefs
	Outputs:	
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial dialog values 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/
BOOL JPEGExportPrefsDialog::InitDialog()
{
	JPEGExportOptions* pOptions = (JPEGExportOptions*)GetOptions();
	ERROR2IF(pOptions == NULL,FALSE, "BmpPrefsDlg::InitJPEGDialog called after duff initialisation?!");

	// Set up the title of the dialog box according to the passed in string which
	// is the name of the filter plus export bitmap options.
	String_256 Temp = *(pOptions->GetFilterName());
	Temp += String_256(_R(IDN_EXPORTBMPOPTS));

	SetTitlebarName(&Temp); 

	// Is this already a compressed JPEG? 
	
	pOptions->SetJPEGPresentAndSelected(FALSE);

	KernelBitmap* pKernelBitmap = pOptions->GetKernelBitmap();

	if (pKernelBitmap != NULL)
	{
		BitmapSource* pSource = NULL;
		BaseBitmapFilter* pDummyFilter;

		pOptions->SetJPEGPresentAndSelected(pKernelBitmap->GetOriginalSource(&pSource, &pDummyFilter));
	}

	if (pOptions->GetJPEGPresentAndSelected())
	{
		SetBoolGadgetSelected(_R(IDC_JPEG_ORIGINAL), TRUE);

		// Disable the other gadgets/
//		EnableGadget(_R(IDC_BMPOPTS_SPREAD), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_DRAWING), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_SELECT), FALSE);
		EnableGadget(_R(IDC_JPGOPTS_PROGRESSIVE), FALSE);

		// Set the range of the slider control plus a step value of 1
		SetGadgetRange(_R(IDC_BMPOPTS_QUALITY), 0, 100, 1);
		SetGadgetBitmaps(_R(IDC_BMPOPTS_QUALITY), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
		SetLongGadgetValue(_R(IDC_BMPOPTS_QUALITY), 0);
		EnableGadget(_R(IDC_BMPOPTS_QUALITY), FALSE);

		EnableGadget(_R(IDC_BMPOPTS_RES), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_XPIXELS), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_YPIXELS), FALSE);
		// WEBSTER - markn 5/2/97
		// Taken out the X & Y size gadgets
#ifndef WEBSTER
		SetStringGadgetValue(_R(IDC_BMPOPTS_XSIZE), &String_8("-"));
		SetStringGadgetValue(_R(IDC_BMPOPTS_YSIZE), &String_8("-"));
		EnableGadget(_R(IDC_BMPOPTS_XSIZE), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_YSIZE), FALSE);
#endif // WEBSTER
		EnableGadget(_R(IDC_BMPOPTS_PERCENT), FALSE);
	}
	else
	{
		// Disable the "Use Original Source" control.
		EnableGadget(_R(IDC_JPEG_ORIGINAL), FALSE);

		// Flag that DPI is supported by JPEGs
		m_bDpiSupported = TRUE;

		// First, set up the resolution combo box
		SetUpResolutionList();
	
		// Set up the quality slider and percentage display
		INT32 Quality = pOptions->GetQuality();	// Default Quality
		// Check that value is not bigger than maximum allowed
		if (Quality > 100)
			Quality = 100;

		// Set the range of the slider control plus a step value of 1
		SetGadgetRange(_R(IDC_BMPOPTS_QUALITY), 0, 100, 1);
		SetGadgetBitmaps(_R(IDC_BMPOPTS_QUALITY), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
		SetLongGadgetValue(_R(IDC_BMPOPTS_QUALITY), 100 - Quality);
		SetBoolGadgetSelected(_R(IDC_JPGOPTS_PROGRESSIVE), pOptions->DoAsProgressive());
	
		// Set the percentage string
		TCHAR Str[32];
		String_32 jcf(_R(IDS_PERCENT_FORMAT));
		camSprintf(Str, jcf, Quality);
		String_32 PercentStr(Str);
		SetStringGadgetValue(_R(IDC_BMPOPTS_PERCENT), &PercentStr);

		InitSelectionRadioGroup();
	
		// Make sure that size fields are displaying the values correctly
		RecalculateSize();
	}

		return TRUE;
}


/******************************************************************************************

>	void JPEGExportPrefsDialog::SetJPEGDlgState()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com> (from Neville)
	Created:	21/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This will be called to set JPEG dlg controls if the user decides 
				not to export using the the Origianl JPEG image.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/
void JPEGExportPrefsDialog::SetJPEGDlgState()
{
	JPEGExportOptions* pOptions = (JPEGExportOptions*)GetOptions();
	ERROR3IF(pOptions == NULL, "pOptions is NULL");

	if (pOptions->GetJPEGPresentAndSelected())
	{
		SetBoolGadgetSelected(_R(IDC_JPEG_ORIGINAL), TRUE);

		// Disable the other gadgets.
//		EnableGadget(_R(IDC_BMPOPTS_SPREAD), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_DRAWING), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_SELECT), FALSE);
		EnableGadget(_R(IDC_JPGOPTS_PROGRESSIVE), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_QUALITY), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_RES), FALSE);

		EnableGadget(_R(IDC_BMPOPTS_XPIXELS), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_YPIXELS), FALSE);
		// WEBSTER - markn 5/2/97
		// Taken out the X & Y size gadgets
#ifndef WEBSTER
		SetStringGadgetValue(_R(IDC_BMPOPTS_XSIZE), &String_8("-"));
		SetStringGadgetValue(_R(IDC_BMPOPTS_YSIZE), &String_8("-"));
		EnableGadget(_R(IDC_BMPOPTS_XSIZE), FALSE);
		EnableGadget(_R(IDC_BMPOPTS_YSIZE), FALSE);
#endif // WEBSTER
		EnableGadget(_R(IDC_BMPOPTS_PERCENT), FALSE);
	}
	else
	{
		// Enbale the quality & progress controls.
		EnableGadget(_R(IDC_BMPOPTS_QUALITY), TRUE);
		EnableGadget(_R(IDC_JPGOPTS_PROGRESSIVE), TRUE);
		EnableGadget(_R(IDC_BMPOPTS_PERCENT), TRUE);

		// Flag that DPI is supported by JPEGs
		m_bDpiSupported = TRUE;

		// First, set up the resolution combo box
		SetUpResolutionList();
	
		// Set up the quality slider and percentage display
		INT32 Quality = pOptions->GetQuality();	// Default Quality
		// Check that value is not bigger than maximum allowed
		if (Quality > 100)
			Quality = 100;

		// Set the range of the slider control plus a step value of 1
		SetGadgetRange(_R(IDC_BMPOPTS_QUALITY), 0, 100, 1);
		SetGadgetBitmaps(_R(IDC_BMPOPTS_QUALITY), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
		SetLongGadgetValue(_R(IDC_BMPOPTS_QUALITY), 100 - Quality);
		SetBoolGadgetSelected(_R(IDC_JPGOPTS_PROGRESSIVE), pOptions->DoAsProgressive());
	
		// Set the percentage string
		TCHAR Str[32];
		String_32 jcf(_R(IDS_PERCENT_FORMAT));
		camSprintf(Str, jcf, Quality);
		String_32 PercentStr(Str);
		SetStringGadgetValue(_R(IDC_BMPOPTS_PERCENT), &PercentStr);

		InitSelectionRadioGroup();
	
		// Make sure that size fields are displaying the values correctly
		RecalculateSize();
	}
};

/******************************************************************************************

>	virtual MsgResult JPEGExportPrefsDialog::Message(Msg* Message)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/96
	Inputs:		See DialogOp for interface details
	Purpose:	Handles JPEG dialog specific messages

******************************************************************************************/
MsgResult JPEGExportPrefsDialog::Message(Msg* pMessage)
{
	ERROR3IF(GetOptions() == NULL, "BmpPrefsDlg::Message - GetOptions() NULL");

	if (IS_OUR_DIALOG_MSG(pMessage))
	{
		DialogMsg* pMsg = (DialogMsg*)pMessage;
		ERROR3IF(!pMsg->IS_KIND_OF(DialogMsg), "pMsg isn't");

		// Should now handle the required messages that we respond to
		switch (pMsg->DlgMsg)
		{
			case DIM_SLIDER_POS_CHANGING:
				// special message for the JPEG qualty control
				// Messages to all the controls, handled individually
				switch (pMsg->GadgetID)
				{
					case _R(IDC_BMPOPTS_QUALITY):
					{
						// Find the current quality scroller's position
						TCHAR Str[32];
						BOOL Valid;
						INT32 Result = GetLongGadgetValue(_R(IDC_BMPOPTS_QUALITY), 0, 100, 0, &Valid);
						Result = 100 - Result;

						// Build the Percentage string and set it
						String_32 jcf(_R(IDS_PERCENT_FORMAT));
						wsprintf(Str, jcf, (INT32) Result);
						String_32 PercentStr(Str);
						SetStringGadgetValue(_R(IDC_BMPOPTS_PERCENT), &PercentStr);
					}
					break;
				}
			break; // DIM_SLIDER_POS_CHANGING		
			
			case DIM_LFT_BN_CLICKED:
				{
					switch (pMsg->GadgetID)
					{
						case _R(IDC_JPEG_ORIGINAL):
						{
							BOOL Value = GetBoolGadgetSelected(_R(IDC_JPEG_ORIGINAL));
							if (Value)
							{
								JPEGExportOptions::SetJPEGPresentAndSelected(TRUE);
								SetJPEGDlgState();
							}
							else
							{
								JPEGExportOptions::SetJPEGPresentAndSelected(FALSE);
								SetJPEGDlgState();
							}
						}
						break;
					}
					break;
				}
				break; //DIM_LFT_BN_CLICKED

				case DIM_CANCEL:
				{
					// The Export has been cancelled, 
					// set our KernelBitmap pointer to NULL.
					JPEGExportOptions::SetKernelBitmap(NULL);
				}
				break;	// DIM_CANCEL
		}
	}
	return BmpPrefsDlg::Message(pMessage);
}


/********************************************************************************************

>	virtual BOOL JPEGExportPrefsDialog::CommitDialogValues()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Neville)
	Created:	03/10/94
	Inputs:		pointer to the options to commit
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the dialog box and sets the return values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	BmpPrefsDlg::CommitDialogValues;

********************************************************************************************/
BOOL JPEGExportPrefsDialog::CommitDialogValues()
{
	JPEGExportOptions* pOptions = (JPEGExportOptions*)GetOptions();
	ERROR3IF(pOptions == NULL, "JPEGExportPrefsDialog::CommitDialogValues called after duff initialisation?!");

	//Added by Graham 27/5/97
	//First, let's set the pixel width and height into our OutputSize option
	pOptions->SetPixelOutputSize(PixelWidth, PixelHeight);

	// We only want new values, if the "Use Origianl JPEG image" switvh is OFF.
	if (!pOptions->GetJPEGPresentAndSelected())
	{
		// Ok has been pressed so take the values and set up the static values so that the
		// caller can access them

		BOOL Valid = 0;	// Flag for validity of value
		BOOL State = 0;	// Flag for reading the state of gadgets

		// Get the dpi value from the resolution combo box 
		// Minimum of 5dpi as anything lower causes major problems.  
		INT32 Value = GetLongGadgetValue(_R(IDC_BMPOPTS_RES), 5, MAXDPI, _R(IDS_BMPPREFS_INVALIDDPI) ,&Valid);
		if ( Valid) 
		{
			// If returned value in range then set the new default
			pOptions->SetDPI(Value);
		}
		else
			return FALSE; 

		// Check that we have not gone over our GDraw limit for pixel width
		// Pixel height should not be so much of a problem as we will strip the export. 
		RecalculatePixels();
		if ((DWORD) PixelWidth > GRenderRegion::GetMaxBitmapWidth() ||
			(DWORD) PixelWidth < 1 || (DWORD) PixelHeight < 1 )
		{
			// There is a problem so warn the user that the value is incorrect
			String_256 WarnMsg;

			WarnMsg.MakeMsg(_R(IDE_BMP_BADPIXELWIDTH), GRenderRegion::GetMaxBitmapWidth());
			Error::SetError(0, WarnMsg, 0);
			InformWarning(0, _R(IDS_OK));
			return FALSE; 
		}

		// Get quality slider value
		State = GetLongGadgetValue(_R(IDC_BMPOPTS_QUALITY), 0, 100, 0, &Valid);
		if (Valid)
		{
			// Return compression to caller
			pOptions->SetQuality(100 - State);
		}
			
		// Work out what is currently selected
		SelectionType FoundSelection = GetSelection();
		pOptions->SetSelectionType(FoundSelection);

		// Get whether a progressive file is wanted or not
		State = GetBoolGadgetSelected(_R(IDC_JPGOPTS_PROGRESSIVE), 0, &Valid);
		if (Valid)
		{
			pOptions->SetMakeProgressive(State);
		}
	}
		return TRUE;
}


DocRect RandomSelection::GetSize()			{ return DocRect(0,0,0,0); }
DocRect SingleBitmapSelection::GetSize()	{ return DocRect(0,0,0,0); }
DocRect MultipleBitmapSelection::GetSize()	{ return DocRect(0,0,0,0); }
DocRect SpreadSelection::GetSize()			{ return BaseBitmapFilter::GetSizeOfSpread(m_pSpread); }
DocRect DrawingSelection::GetSize()			{ return BaseBitmapFilter::GetSizeOfDrawing(m_pSpread); }

#endif



/**************************************************************************************************
Access functions add on 20/10/00 for use by the palette optimiser sjk
// SMFIX

  All added in the cpp file so we can breakpoint on them nicely
**************************************************************************************************/
 // is the palette in this export object completely valid and perfect?
BOOL BitmapExportOptions::IsPaletteValid()
{
	return m__PaletteIsValid;
}

void BitmapExportOptions::InvalidatePalette()
{
	m__PaletteIsValid = FALSE;
	SetTempFileMatchesExportOptions(FALSE);
	BitmapExportPaletteInterface::InvalidateSortedPalette();
}

void BitmapExportOptions::SetTransparencyIndex(INT32 TransparencyIndex)
{
	if (m_TransparencyIndex != TransparencyIndex)
		SetTempFileMatchesExportOptions(FALSE);
	m_TransparencyIndex = TransparencyIndex; 
}

void BitmapExportOptions::SetBackgroundTransparency(BOOL t)
{
	if (m__BackgroundIsTransparent != t)
		InvalidatePalette();
	m__BackgroundIsTransparent = t;
}


INT32	BitmapExportOptions::GetNumberOfUserRequestedColours()
{
	return m__NumberOfColoursUserRequested;
}

void BitmapExportOptions::SetNumberOfUserRequestedColours(INT32 n)
{
	if (m__NumberOfColoursUserRequested != n)
		InvalidatePalette();
	m__NumberOfColoursUserRequested = n;
}

BOOL BitmapExportOptions::IsUsingBrowserPalette()
{
	return m__UseBrowserPalette;
}

void BitmapExportOptions::SetToUseBrowserPalette(BOOL SetIt)
{
	if (m__UseBrowserPalette != SetIt)
	{
		m__UseBrowserPalette = SetIt;
		InvalidatePalette();
	}
}

BOOL BitmapExportOptions::IsUsingSystemPalette()
{
	return m__UseSystemColoursInPalette;
}

void BitmapExportOptions::SetToUseSystemPalette(BOOL SetIt)
{
	if (m__UseSystemColoursInPalette != SetIt)
	{
		m__UseSystemColoursInPalette = SetIt;
		InvalidatePalette();
	}
}

INT32 BitmapExportOptions::GetWebSnapPaletteAmount()
{
	return m__UseWebSnapPalette;
}

void BitmapExportOptions::SetWebSnapPaletteAmount(INT32 SetIt)
{
	if (m__UseWebSnapPalette != SetIt)
	{
		m__UseWebSnapPalette = SetIt;
		InvalidatePalette();
	}
}

INT32 BitmapExportOptions::GetPrimarySnapPaletteAmount()
{
	return m__UsePrimarySnapPalette;
}

void BitmapExportOptions::SetPrimarySnapPaletteAmount(INT32 SetIt)
{
	if (m__UsePrimarySnapPalette != SetIt)
	{
		m__UsePrimarySnapPalette = SetIt;
		InvalidatePalette();
	}
}


// create palette Optimiser
BOOL BitmapExportOptions::CreatePaletteOptimiser()
{
	if (m__pPaletteOptimiser == NULL)
	{
		m__HavePrimedOptimier = FALSE;
		m__pPaletteOptimiser = new PaletteOptimiser();

		ERROR2IF(m__pPaletteOptimiser == NULL, FALSE, "Low on memory, couldn't create palette optimiser");

		m__pPaletteOptimiser->Initialise();

		return TRUE; // we created a new one
	}

	return FALSE; // we kept the old one
}

// flush the palette optimiser if the bitmap source pixels have changed
// since this will affect the stats that were fed into it
void BitmapExportOptions::BitmapSourceHasChanged()
{
	m__HavePrimedOptimier = FALSE;
	if (m__pPaletteOptimiser != NULL)
		delete m__pPaletteOptimiser;

	m__pPaletteOptimiser = NULL;
	InvalidatePalette(); // the palette we are holding will be wrong too
}

// feed the optimiser with stats from the image
void BitmapExportOptions::GatherPaletteStats(const RGBQUAD* pBitmap, UINT32 nSize )
{
	if (m__pPaletteOptimiser)
	{
		m__pPaletteOptimiser->AddStats(pBitmap, nSize);
	}
}


// fills in a logical palette from the stored extended palette
LOGPALETTE * BitmapExportOptions::GetLogicalPalette()
{
	ERROR3IF(!m__PaletteIsValid, "Palette not validated");

	// the logical palette is valid and current
	if (m__pLogicalPalette)
		return m__pLogicalPalette;

	// build a logical palette based on the extended palette
	// calling functions can read and use this data, but they should not delete it
	// the logical palette is managed by the export options
	m__pLogicalPalette = (LOGPALETTE * ) new BYTE[sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * 256 )];
	if (m__pLogicalPalette == NULL)
		return NULL;

	// pick a colour to duplicate for deleted colours to be
//	INT32 OptimisedCommonColour = m_Palette.NumberOfColours-1;

	m__pLogicalPalette->palNumEntries = m_Palette.NumberOfColours;
	m__pLogicalPalette->palVersion = 0x300;
	INT32 i = 0;
	for( ; i < m__pLogicalPalette->palNumEntries; i++ )
	{
		m__pLogicalPalette->palPalEntry[ i ].peRed = m_Palette.Data[ i ].Red;
		m__pLogicalPalette->palPalEntry[ i ].peGreen = m_Palette.Data[ i ].Green;			
		m__pLogicalPalette->palPalEntry[ i ].peBlue = m_Palette.Data[ i ].Blue;
		BOOL NonRenderable = (m_Palette.Data[i].Flags & (DELETED_COLOUR | TRANSPARENT_COLOUR));
		m__pLogicalPalette->palPalEntry[ i ].peFlags = NonRenderable ? 255 : 0;
	}

	return m__pLogicalPalette;
}

// swap the current colours in the palette for the 
BOOL BitmapExportOptions::SwapEditedColoursInLogicalPalette()
{
	if (!m__pLogicalPalette)
		return FALSE;

	BOOL DidASwap = FALSE;
	for( INT32 i = 0; i < m__pLogicalPalette->palNumEntries; i++ )
	{
		if (m_Palette.Data[ i ].Flags & LOCKED_COLOUR)
		{
			BYTE t;
			t = m_Palette.Data[ i ].Red;
			m_Palette.Data[ i ].Red = m_Palette.Data[ i ].PreEditedRed;
			m_Palette.Data[ i ].PreEditedRed = t;
			m__pLogicalPalette->palPalEntry[ i ].peRed = m_Palette.Data[ i ].Red;

			t = m_Palette.Data[ i ].Green;
			m_Palette.Data[ i ].Green = m_Palette.Data[ i ].PreEditedGreen;
			m_Palette.Data[ i ].PreEditedGreen = t;
			m__pLogicalPalette->palPalEntry[ i ].peGreen = m_Palette.Data[ i ].Green;

			t = m_Palette.Data[ i ].Blue;
			m_Palette.Data[ i ].Blue = m_Palette.Data[ i ].PreEditedBlue;
			m_Palette.Data[ i ].PreEditedBlue = t;
			m__pLogicalPalette->palPalEntry[ i ].peBlue = m_Palette.Data[ i ].Blue;

			DidASwap = TRUE;
		}
	}

	return DidASwap;
}

// find the next locked colour in the extended palette
// returns the index of the palette entry or -1 if not found
// starts looking from entry pos+1
INT32 BitmapExportOptions::FindNextLockedColour(INT32 pos)
{
	pos++;
	while (pos < m_Palette.NumberOfColours)
	{
		if (m_Palette.Data[pos].Flags & LOCKED_COLOUR || m_Palette.Data[pos].Flags & DELETED_COLOUR)
			return pos;

		pos++;
	}
	return -1;
}


// This is the function which creates the optimal palette
// It is passed in a ptr to the palette optimiser which has been initiallised and has gathered stats
// on each strip of the bitmap.
// This function then looks at what locked colours the user has set and the other palette
// options the user wants and gives all these to the optimiser.
// It then tries to recreate the options extended palette (including locked colours) from the
// logical palette that the optimiser returns.
// It sets its internal logical palette up and sets the ValidPalette flag to being valid.
void BitmapExportOptions::CreateValidPalette()
{
	ASSERT(m__pPaletteOptimiser);

	// tell the export dlg palette ctrl that a new palette is on the horizon
	BitmapExportPaletteInterface::InvalidateSortedPalette();

	// delete our old logical palette if we had one
	if (m__pLogicalPalette)
	{
		delete m__pLogicalPalette;
		m__pLogicalPalette = NULL;
	}

	// allocate space for a logical palette
	m__pLogicalPalette = (LOGPALETTE * ) new BYTE[sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * 256 )];
	if (m__pLogicalPalette == NULL)
		return;
	INT32 NewFlags[256];
	BYTE EditedColours[256][3];
	INT32 BackGroundTransp = m__BackgroundIsTransparent? 1: 0;

	// init the flags for the new palette
	memset(&NewFlags, 0, sizeof(INT32)*256);
	// init the edited colours for the new palette
	memset(&EditedColours, 0, 3*256);

	INT32 i = 0;
	// count the locked colours that we had in the last palette,
	// these are still the same locked colours for the new palette 
	// as the flags would have been removed if they were no longer required
	m__pPaletteOptimiser->RemoveLockedColours();
	INT32 LockedColours = 0;
	for (i = 0; i < m_Palette.NumberOfColours; i++)
	{
		if (m_Palette.Data[i].Flags & LOCKED_COLOUR || m_Palette.Data[i].Flags & DELETED_COLOUR) // found a locked colour?
		{
			// add it as a locked colour to the palette
			m__pPaletteOptimiser->AddLockedColour(m_Palette.Data[i].Red, m_Palette.Data[i].Green, m_Palette.Data[i].Blue);
			// count the locked colours so far
			LockedColours++;
		}
	}
	// how many colours can we fit in the palette
	INT32 MaxColoursInPalette = 256;
	if (m_Depth == 4)
		MaxColoursInPalette = 16;
	else if (m_Depth == 1)
		MaxColoursInPalette = 2;

	MaxColoursInPalette = max(2, min(MaxColoursInPalette, m__NumberOfColoursUserRequested));

	// work out how many spare colours we have within the user defined restraint
	// once we have kept these locked colours and have account for the transparency
//	INT32 NoOfColoursToOptimise = MaxColoursInPalette - LockedColours - BackGroundTransp;

	m__pPaletteOptimiser->SnapToBrowserPalette(m__UseWebSnapPalette);
	m__pPaletteOptimiser->SnapToPrimaries(m__UsePrimarySnapPalette);
	m__pPaletteOptimiser->AddSystemColours((m__UseSystemColoursInPalette == TRUE));
	m__pPaletteOptimiser->UseBrowserPalette((m__UseBrowserPalette == TRUE));

	// tell the optimiser that we have finished gathering the stats
	//if (!m__HavePrimedOptimier) // can I get away with putting this optimisation back gav?
	{
		m__pPaletteOptimiser->GenPalette(max(2,MaxColoursInPalette - BackGroundTransp));
		m__HavePrimedOptimier = TRUE;
	}

	m__pLogicalPalette->palNumEntries = MaxColoursInPalette;
	m__pLogicalPalette->palVersion = 0x300;

	// get the optimised palette from the optimser
	/*BOOL JustBmpColours =*/ m__pPaletteOptimiser->GetPalette( m__pLogicalPalette, max(2,MaxColoursInPalette - BackGroundTransp));

//	if (JustBmpColours)
//		TRACE( _T("Just bmp colours\n"));
//	else
//		TRACE( _T("fully optimised colours\n"));

	// reset the flags in the new palette from the locked/deleted colours we had in the old palette
	// they may not be in the new palette, but they are LIKELY to be there
	INT32 index = -1;
	while ((index = FindNextLockedColour(index)) != -1) // find the next locked colour
	{
		// does this match in the palette
		for (i = 0; i < m__pLogicalPalette->palNumEntries; i++)
			if (NewFlags[i] == 0 &&
				m_Palette.Data[index].Red   == m__pLogicalPalette->palPalEntry[ i ].peRed &&
				m_Palette.Data[index].Green == m__pLogicalPalette->palPalEntry[ i ].peGreen &&
				m_Palette.Data[index].Blue  == m__pLogicalPalette->palPalEntry[ i ].peBlue
				)
			{
				NewFlags[i] = m_Palette.Data[index].Flags;
				EditedColours[i][0] = m_Palette.Data[index].PreEditedRed;
				EditedColours[i][1] = m_Palette.Data[index].PreEditedGreen;
				EditedColours[i][2] = m_Palette.Data[index].PreEditedBlue;
				break;
			}
	}

	// special case for 1 colour - dont mix them up
	if (MaxColoursInPalette - BackGroundTransp == 1)
	{
		m_Palette.NumberOfColours = m__pLogicalPalette->palNumEntries = 2;
		// bias against the background colour and use the other colour if 
		// the background colour is more predominant
		DocColour dcol = Page::GetPageColour();
		INT32 bgr = 255, bgg = 255, bgb = 255;

		// the above call the GetPageColour doesn't work very well
		// this code gets it better if the spread and the layer exist
		Spread* pSelSpread = Document::GetSelectedSpread();
		if (pSelSpread)
		{
			Layer * pLayer = pSelSpread->FindFirstPageBackgroundLayer();
			if (pLayer)
			{
				Node * pNode = SliceHelper::FindNextOfClass(pLayer, pLayer, CC_RUNTIME_CLASS(AttrFlatColourFill));
				if (pNode)
					dcol = *(((AttrFlatFill *)pNode)->GetStartColour());
			}
		}

		dcol.GetRGBValue(&bgr,&bgg,&bgb);

		// pick which of the two colours to use
		if (BackGroundTransp && 
			(m__pLogicalPalette->palPalEntry[ 0 ].peRed == (BYTE)bgr || m__pLogicalPalette->palPalEntry[ 0 ].peRed == (BYTE)bgr-1) &&
			(m__pLogicalPalette->palPalEntry[ 0 ].peGreen == (BYTE)bgg || m__pLogicalPalette->palPalEntry[ 0 ].peGreen == (BYTE)bgg-1) &&
			(m__pLogicalPalette->palPalEntry[ 0 ].peBlue == (BYTE)bgb || m__pLogicalPalette->palPalEntry[ 0 ].peBlue == (BYTE)bgb-1))
		{
			m__pLogicalPalette->palPalEntry[ 0 ].peRed = m__pLogicalPalette->palPalEntry[ 1 ].peRed;
			m__pLogicalPalette->palPalEntry[ 0 ].peGreen = m__pLogicalPalette->palPalEntry[ 1 ].peGreen;
			m__pLogicalPalette->palPalEntry[ 0 ].peBlue = m__pLogicalPalette->palPalEntry[ 1 ].peBlue;
		}
	}
	else if (MaxColoursInPalette == 2) // special case for two colour none transp
	{
		// make it black and white
		m__pLogicalPalette->palPalEntry[ 0 ].peRed = 0;
		m__pLogicalPalette->palPalEntry[ 0 ].peGreen = 0;
		m__pLogicalPalette->palPalEntry[ 0 ].peBlue = 0;
		m__pLogicalPalette->palPalEntry[ 0 ].peFlags = 0;
		m__pLogicalPalette->palPalEntry[ 1 ].peRed = 255;
		m__pLogicalPalette->palPalEntry[ 1 ].peGreen = 255;
		m__pLogicalPalette->palPalEntry[ 1 ].peBlue = 255;
		m__pLogicalPalette->palPalEntry[ 1 ].peFlags = 0;
	}

	// make room for transp as the first colour
	// by moving the other colours up
	if (BackGroundTransp)
	{
		// add the background colour to the palette which is one more colour in there
		if (m__pLogicalPalette->palNumEntries <  MaxColoursInPalette)
			m__pLogicalPalette->palNumEntries++;

		// move all the colours up a place
		for (INT32 i = m__pLogicalPalette->palNumEntries-1; i > 0; i--)
		{
			m__pLogicalPalette->palPalEntry[ i ].peRed = m__pLogicalPalette->palPalEntry[ i-1 ].peRed;
			m__pLogicalPalette->palPalEntry[ i ].peGreen = m__pLogicalPalette->palPalEntry[ i-1 ].peGreen;
			m__pLogicalPalette->palPalEntry[ i ].peBlue = m__pLogicalPalette->palPalEntry[ i-1 ].peBlue;
			NewFlags[i] = NewFlags[i-1];
			EditedColours[i][0] = EditedColours[i-1][0];
			EditedColours[i][1] = EditedColours[i-1][1];
			EditedColours[i][2] = EditedColours[i-1][2];
		}

		// the transp coluor is white 255,255,255
		m__pLogicalPalette->palPalEntry[ 0 ].peRed = 255;
		m__pLogicalPalette->palPalEntry[ 0 ].peGreen = 255;
		m__pLogicalPalette->palPalEntry[ 0 ].peBlue = 255;

		NewFlags[0] = TRANSPARENT_COLOUR;
		SetTransparencyIndex(0);
	}
	else
	{
		// we dont have background transparancy but do we have any other colour that is
		// set to being transparent?
		INT32 TranspIndex = -1; // the default no transp
		// find a transp marker in the new flags
		for (i = 0; i < m__pLogicalPalette->palNumEntries && TranspIndex == -1; i++)
		{
			if (NewFlags[i] & TRANSPARENT_COLOUR)
				TranspIndex = i;

		}
		// set the transp index
		SetTransparencyIndex(TranspIndex);
	}

	// the palette is now valid
	m__PaletteIsValid = TRUE;

	// set the number of colours in the extended palette to match those in the new palette that
	// the optimiser returned to us
	m_Palette.NumberOfColours = m__pLogicalPalette->palNumEntries;

	// fill in the extended palette from the logical palette that is now optimal for this export
	for (i = 0; i < 256; i++)
	{
		m_Palette.Data[i].Red = m__pLogicalPalette->palPalEntry[ i ].peRed;
		m_Palette.Data[i].Green = m__pLogicalPalette->palPalEntry[ i ].peGreen;
		m_Palette.Data[i].Blue = m__pLogicalPalette->palPalEntry[ i ].peBlue;
		m_Palette.Data[i].Flags = NewFlags[i];
		m_Palette.Data[i].PreEditedRed = EditedColours[i][0];
		m_Palette.Data[i].PreEditedGreen = EditedColours[i][1];
		m_Palette.Data[i].PreEditedBlue = EditedColours[i][2];

		if (i >= m__pLogicalPalette->palNumEntries || 
			(m_Palette.Data[i].Flags & (DELETED_COLOUR | TRANSPARENT_COLOUR)))
			m__pLogicalPalette->palPalEntry[ i ].peFlags = 255; // meaning dont display this colour
		else
			m__pLogicalPalette->palPalEntry[ i ].peFlags = 0; // meaning do display this colour
	}

}


// This pairs with the function above and creates a valid palette from a known
// logical palette where one colour can be marked as transparent
void BitmapExportOptions::CreateValidPalette(LOGPALETTE * pPal, INT32 TranspIndex)
{
	// tell the export dlg palette ctrl that a new palette is on the horizon
	BitmapExportPaletteInterface::InvalidateSortedPalette();
	// delete our old logical palette if we had one
	if (m__pLogicalPalette)
	{
		delete m__pLogicalPalette;
		m__pLogicalPalette = NULL;
	}

	m__PaletteIsValid = (pPal != NULL);

	if (!m__PaletteIsValid)
		return;

	// copy the logical palette data if it was valid
	m__pLogicalPalette = (LOGPALETTE * ) new BYTE[sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * 256 )];

	if (m__pLogicalPalette == NULL)
	{
		InvalidatePalette();
		return;
	}

	m__pLogicalPalette->palVersion = 0x300;

	// set the number of colours in the extended palette to match those in the new palette passed in
	m_Palette.NumberOfColours = m__pLogicalPalette->palNumEntries = pPal->palNumEntries;
	// fill in the extended palette from the new logical palette from the logical palette passed in
	for (INT32 i = 0; i < pPal->palNumEntries; i++)
	{
		m_Palette.Data[i].Red = pPal->palPalEntry[ i ].peRed;
		m_Palette.Data[i].Green = pPal->palPalEntry[ i ].peGreen;
		m_Palette.Data[i].Blue = pPal->palPalEntry[ i ].peBlue;
		m_Palette.Data[i].Flags = 0; // no flags set this will loose any locked colours etc as these are not part of a logical palette

		m__pLogicalPalette->palPalEntry[ i ].peRed = m_Palette.Data[i].Red;
		m__pLogicalPalette->palPalEntry[ i ].peGreen = m_Palette.Data[i].Green;
		m__pLogicalPalette->palPalEntry[ i ].peBlue = m_Palette.Data[i].Blue;
		m__pLogicalPalette->palPalEntry[ i ].peFlags = 0;
	}

	if (TranspIndex >= 0 && TranspIndex < pPal->palNumEntries)
	{
		m_Palette.Data[TranspIndex].Flags = TRANSPARENT_COLOUR;
		m__pLogicalPalette->palPalEntry[ TranspIndex ].peFlags = 255;
		SetTransparencyIndex(TranspIndex);
	}
}

// this likewise sets the extended palette to a preset browser palette
// which is only used in the creation of a global palette for all animated frames
void BitmapExportOptions::CreateValidBrowserPalette(BOOL WantTransp)
{
	// tell the export dlg palette ctrl that a new palette is on the horizon
	BitmapExportPaletteInterface::InvalidateSortedPalette();
	// delete our old logical palette if we had one
	if (m__pLogicalPalette)
	{
		delete m__pLogicalPalette;
		m__pLogicalPalette = NULL;
	}

	m__PaletteIsValid = TRUE;

	// copy the logical palette data if it was valid
	m__pLogicalPalette = (LOGPALETTE * ) new BYTE[sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * 256 )];

	if (m__pLogicalPalette == NULL)
	{
		InvalidatePalette();
		return;
	}

	m__pLogicalPalette->palVersion = 0x300;

	// set the number of colours in the extended palette to match those in the new palette passed in
	m_Palette.NumberOfColours = m__pLogicalPalette->palNumEntries = WantTransp ? 217 : 216;
	// fill in the extended palette from the new logical palette from the logical palette passed in

	m_Palette.Data[0].Red = 255 ;
	m_Palette.Data[0].Green = 255;
	m_Palette.Data[0].Blue = 255;
	m_Palette.Data[0].Flags = WantTransp ? TRANSPARENT_COLOUR : 0;
	m__pLogicalPalette->palPalEntry[ 0 ].peRed = m_Palette.Data[0].Red;
	m__pLogicalPalette->palPalEntry[ 0 ].peGreen = m_Palette.Data[0].Green;
	m__pLogicalPalette->palPalEntry[ 0 ].peBlue = m_Palette.Data[0].Blue;
	m__pLogicalPalette->palPalEntry[ 0 ].peFlags = WantTransp ? 255 : 0;

	SetTransparencyIndex(WantTransp ? 0 : -1);
	
	INT32 i = 1;
	for (INT32 r = 0; r < 256; r += 51)
		for (INT32 g = 0; g < 256; g += 51)
			for (INT32 b = 0; b < 256; b += 51)
			{
				m_Palette.Data[i].Red = r ;
				m_Palette.Data[i].Green = g;
				m_Palette.Data[i].Blue = b;
				m_Palette.Data[i].Flags = 0; // no flags set this will loose any locked colours etc as these are not part of a logical palette

				m__pLogicalPalette->palPalEntry[ i ].peRed = m_Palette.Data[i].Red;
				m__pLogicalPalette->palPalEntry[ i ].peGreen = m_Palette.Data[i].Green;
				m__pLogicalPalette->palPalEntry[ i ].peBlue = m_Palette.Data[i].Blue;
				m__pLogicalPalette->palPalEntry[ i ].peFlags = 0;
				i++;
			}
}

/***************************************************************************************
	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/00
	Returns:	TRUE if supported or FALSE if not!
	Purpose:	Return true if the current filter allows palette info

******************************************************************************************/
BOOL BitmapExportOptions::GetSupportsPalette()
{
	// Set up the return variable
	BOOL Supported = FALSE;
	UINT32 FilterStrID = GetFilterNameStrID();

	// We can only use palettes with Bitmaps of 8 BPP or less, so quick check first!
	if(m_Depth <= 8)
	{
		// Find out what Filter Type we are
		Supported = (FilterStrID == _R(IDN_FILTERNAME_GIF) || FilterStrID == _R(IDT_FILTERNAME_BMP) || FilterStrID == _R(IDS_FILTERNAME_PNG));
	}

	return Supported;
}

/******************************************************************************************

  >	DWORD BitmapExportOptions::GetSupportedDithers()

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/00
	Returns:	ENUM of the avaliable dither types, 0 if dithering not supported
	Purpose:	Returns the allowed dither types the current filter supports.

******************************************************************************************/
DWORD BitmapExportOptions::GetSupportedDithers()
{
	// Set up the return variable
	DWORD Supported = 0;
	UINT32 FilterStrID = GetFilterNameStrID();

	// We can only use palettes with Bitmaps of 8 BPP or less, so quick check first!
	if(m_Depth <= 8)
	{
		// Find out what Filter Type we are
//		switch (FilterStrID)
//		{
//		case _R(IDN_FILTERNAME_GIF):
//		case _R(IDS_FILTERNAME_PNG):
//		case _R(IDT_FILTERNAME_BMP):
		if (FilterStrID==_R(IDN_FILTERNAME_GIF) || FilterStrID==_R(IDS_FILTERNAME_PNG) || FilterStrID==_R(IDT_FILTERNAME_BMP))
			{
				Supported |= ERROR_DITHER;

				if (m__UseBrowserPalette && m_Depth != 4)
					Supported |= ORDERED_DITHER;

//				break;
			}
//		case _R(IDS_JPG_EXP_FILTERNAME):
//		default:
//			break; // Not supported so don`t do anything!
//		}
	}

	return Supported;
}

/******************************************************************************************

  >	DWORD BitmapExportOptions::GetSupportedColourDepths()

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/00
	Returns:	BitField value of the avaliable colours allowed
	Purpose:	Returns the all the colour depths the current filter supports.

******************************************************************************************/
DWORD BitmapExportOptions::GetSupportedColourDepths()
{
	// Set up the return variable
	DWORD Supported = 0;
	UINT32 FilterStrID = GetFilterNameStrID();

	if(FilterStrID == _R(IDN_FILTERNAME_GIF))
		Supported = (COLOUR_2 | COLOUR_16 | COLOUR_256 );
	else if(FilterStrID == _R(IDT_FILTERNAME_BMP))
		Supported = (COLOUR_2 | COLOUR_16 | COLOUR_256 | COLOUR_24BIT);
	else if(FilterStrID == _R(IDS_FILTERNAME_PNG))
		Supported = (COLOUR_2 | COLOUR_16 | COLOUR_256 | COLOUR_24BIT | COLOUR_32BIT);
	else if (FilterStrID == _R(IDS_JPG_EXP_FILTERNAME))
		Supported = COLOUR_24BIT;

	return Supported;
}

/******************************************************************************************

  >	BOOL BitmapExportOptions::SupportsTransparency()

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/00
	Returns:	TRUE if Supported else FALSE!
	Purpose:	Used to see if the current filter allows transparencies.

******************************************************************************************/
BOOL BitmapExportOptions::GetSupportsTransparency()
{
	// Set up the return variable
	BOOL Supported = FALSE;
	UINT32 FilterStrID = GetFilterNameStrID();

	// We can only use Transparency with Bitmaps of 8 BPP or less, so quick check first!
	if(m_Depth <= 8)
	{
		Supported = (FilterStrID == _R(IDN_FILTERNAME_GIF) || FilterStrID == _R(IDS_FILTERNAME_PNG));
	}

	return Supported;
}

/******************************************************************************************

  >	DWORD BitmapExportOptions::GetInterlaceType()

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/00
	Returns:	TRUE if supported or FALSE if not!
	Purpose:	Return true if the current filter allows palette info

******************************************************************************************/
DWORD BitmapExportOptions::GetInterlaceType()
{
	// Set up the return variable
	DWORD Supported = 0;
	UINT32 FilterStrID = GetFilterNameStrID();

	if(FilterStrID == _R(IDN_FILTERNAME_GIF))
		Supported = GIF_INTERLACE;
	else if(FilterStrID == _R(IDS_FILTERNAME_PNG))
		Supported = PNG_INTERLACE;
	else if(FilterStrID == _R(IDS_JPG_EXP_FILTERNAME))
		Supported = JPG_PROGRESSIVE;

	return Supported;
}


/******************************************************************************************

  >	BaseBitmapFilter * BitmapExportOptions::FindBitmapFilterForTheseExportOptions()

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/12/00
	Returns:	ptr to a filter that can be used to export using these export options

******************************************************************************************/
BaseBitmapFilter * BitmapExportOptions::FindBitmapFilterForTheseExportOptions()
{
	// get the name this filter thinks it is
	const StringBase * pFilterName = GetFilterName();
	String_256 PreferedFiltersName = "";
	
	// check this ptr can be NULL
	if (pFilterName)
		PreferedFiltersName = *(GetFilterName());

	// fill it with the name that the export options thinks should be using the options
	// if the filters name hasn't been put in
	if (PreferedFiltersName.IsEmpty() || GetFilterNameStrID() == _R(IDT_FILTERNAME_BMP)) // bodge for _R(IDT_FILTERNAME_BMP) since the program is using multiple strings for this filter BMP/Windows bitmaps
	{
		PreferedFiltersName.Load(GetFilterNameStrID());
	}


	Filter *pFilter = Filter::GetFirst();
	while (pFilter != NULL)
	{
		// If we are filtering for bitmap filters then check that if a Bpp has been
		// specified that the bitmap filter can cope with that.
		FilterFlags Flags = pFilter->GetFlags();
		if (Flags.CanExport && pFilter->IsKindOf(CC_RUNTIME_CLASS(BaseBitmapFilter)))
		{
			BaseBitmapFilter* pBitmapFilter = (BaseBitmapFilter*)pFilter;
			if (PreferedFiltersName.CompareTo(pFilter->pOILFilter->FilterName, FALSE) == 0)
				return pBitmapFilter;
		}
		// Try the next filter
		pFilter = Filter::GetNext(pFilter);
	}

	return NULL;
}

/**************************************************************************************************
End of Access functions add on 20/10/00 for use by the palette optimiser sjk
**************************************************************************************************/
