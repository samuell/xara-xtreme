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
// A JPEG import filter 

#include "camtypes.h"
//#include "filters.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "oilfltrs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "oilbitmap.h"
//#include "dibutil.h"
//#include "filtrres.h"
//#include "resource.h"		// for _R(IDS_OUT_OF_MEMORY)
//#include "imglib.h"			// for JPEG
#include "grndbmp.h"
//#include "camfiltr.h"		// for BaseCamelotFilter progress update - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h"			// Camelot!! - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "convert.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "exjpeg.h"
#include "jpgdest.h"
#include "jpgermgr.h"

#include "bitmpinf.h"
#include "ccbuffil.h"

#include "progress.h"
#include "bmapprev.h"	// tab preview dialog
//#include "cxfrec.h"		// for CXaraFileRecord - in camtypes.h [AUTOMATICALLY REMOVED]

#include "exphint.h"

// Place any IMPLEMENT type statements here
CC_IMPLEMENT_DYNAMIC(JPEGExportFilter, BaseBitmapFilter)
CC_IMPLEMENT_DYNCREATE(JPEGExportOptions, BitmapExportOptions)


// We want better memory tracking
#define new CAM_DEBUG_NEW

static TCHAR g_szCompression[]	= _T("ExportJPEG\\Compression");
static TCHAR g_szDCTMethod[]	= _T("ExportJPEG\\DCTMethod");
static TCHAR g_szProgressive[]	= _T("ExportJPEG\\DoAsProgressive");
static TCHAR g_szOptimize[]		= _T("ExportJPEG\\Optimize");
static TCHAR g_szSmoothing[]	= _T("ExportJPEG\\Smoothing");

//RangePreference JPEGExportFilter::Compression(g_szCompression, 0, 100);
//RangePreference DCTMethod(g_szDCTMethod, &GetRefDefaultDCTMethod(), 0, 4);
//BoolPreference Progressive(g_szProgressive, &GetRefDoAsProgressive());
//RangePreference Xppm(g_szXDpi, 96, MinPelsPerMeter, );
//RangePreference Yppm(g_szYDpi, 96, MinPelsPerMeter, );

//JPEG_QUALITY	JPEGExportFilter::g_DefaultCompression	= 100;
//BOOL			JPEGExportFilter::g_bExportProgressive = FALSE;
//J_DCT_METHOD JPEGExportFilter::g_DefaultDCTMethod;

JPEG_QUALITY	g_DefaultCompression	= 75;
libJPEG::J_DCT_METHOD	g_DefaultDCTMethod		= libJPEG::JDCT_FLOAT;
BOOL			g_bExportProgressive	= FALSE;

BOOL			g_bOptimizeCoding = FALSE;
JPEG_QUALITY	g_Smoothing = 0;

KernelBitmap*	JPEGExportOptions::pKernelBitmap = NULL;
BOOL			JPEGExportOptions::m_JPEGPresentAndSelected = FALSE;


/********************************************************************************************
>	JPEGExportOptions::JPEGExportOptions()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Default constructor for a JPEGExportOptions object.
********************************************************************************************/

JPEGExportOptions::JPEGExportOptions()
  : m_bJPEGPresentAndSelected(0),
	m_Quality(100),
	m_DoAsProgressive(FALSE),
	m_DCTMethod(libJPEG::JDCT_DEFAULT),
	m_ColourModel(libJPEG::JCS_UNKNOWN),
	m_ColourComponents(0)
{
	// Empty.
}


/********************************************************************************************
>	JPEGExportOptions::JPEGExportOptions(const FilterType FilterID, 
										 const StringBase* pFilterName)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Constructor for a JPEGExportOptions object.
********************************************************************************************/

JPEGExportOptions::JPEGExportOptions(const FilterType FilterID, const StringBase* pFilterName)
  : BitmapExportOptions(_R(IDD_EXPORTJPEGOPTS), FilterID, pFilterName),
	m_bJPEGPresentAndSelected(0),
	m_Quality(100),
	m_DoAsProgressive(FALSE),
	m_DCTMethod(libJPEG::JDCT_DEFAULT),
	m_ColourModel(libJPEG::JCS_UNKNOWN),
	m_ColourComponents(0)
{
	// Empty.
}


/********************************************************************************************

>	virtual BOOL JPEGExportOptions::CopyFrom(BitmapExportOptions *pSource) 

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/97
	Inputs:		pSource - the other BitmapExportOptions object to copy the data from
	Purpose:	Sets the contents of this object from the passed object
	See Also:	BitmapExportOptions::MakeCopy()

********************************************************************************************/
BOOL JPEGExportOptions::CopyFrom(BitmapExportOptions *pSource) 
{
	// copy the base class first
	if (!BitmapExportOptions::CopyFrom(pSource))
		return FALSE;

	// must be the same class, otherwise the base class  function above returns FALSE
	JPEGExportOptions *pOther = (JPEGExportOptions *)pSource;

	m_Quality			= pOther->m_Quality;
	m_DoAsProgressive	= pOther->m_DoAsProgressive;
	m_DCTMethod			= pOther->m_DCTMethod;
	m_ColourModel		= pOther->m_ColourModel;
	m_ColourComponents	= pOther->m_ColourComponents;
	m_bJPEGPresentAndSelected = pOther->m_bJPEGPresentAndSelected;

	return TRUE;
};


BOOL JPEGExportOptions::FileTypeChangeCopyFrom(BitmapExportOptions *pOther)
{
	// copy the base class first
	if (!BitmapExportOptions::FileTypeChangeCopyFrom(pOther))
		return FALSE;

	//  No other options types has the JPEG options, so have to use the default values
	m_Quality			= 75;
	m_DoAsProgressive	= FALSE;
	m_DCTMethod			= libJPEG::JDCT_DEFAULT;
	m_ColourModel		= libJPEG::JCS_UNKNOWN;
	m_ColourComponents	= 0;
	m_bJPEGPresentAndSelected = 0;

	// Ensure that the JPEG depth is less than 32 bpp.
	if( GetDepth() == 32 )
	{
		SetDepth( 24 );
	}

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL JPEGExportOptions::Equal(BitmapExportOptions *pSource) 

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/97
	Inputs:		pSource - the other BitmapExportOptions object to copy the data from
	Purpose:	Compares the contents of this and pOther objects
	Returns:	TRUE, if objects are equal, FALSE otherwise
	See Also:	BitmapExportOptions::MakeCopy()

********************************************************************************************/
/*
BOOL JPEGExportOptions::Equal(BitmapExportOptions *pSource) 
{
	BOOL ok = TRUE;

	// compare the base classes first
	ok = BitmapExportOptions::Equal(pSource);

	// must be the same class, otherwise the base class  function above returns FALSE
	JPEGExportOptions *pOther = (JPEGExportOptions *)pSource;

	if (ok) ok = (m_Quality				== pOther->m_Quality);
	if (ok) ok = (m_DoAsProgressive		== pOther->m_DoAsProgressive);
	if (ok) ok = (m_DCTMethod			== pOther->m_DCTMethod);
	if (ok) ok = (m_ColourModel			== pOther->m_ColourModel);
	if (ok) ok = (m_ColourComponents	== pOther->m_ColourComponents);
	if (ok) ok = (m_bJPEGPresentAndSelected == pOther->m_bJPEGPresentAndSelected);
	return ok;
};
*/

/********************************************************************************************

>	virtual BOOL JPEGExportOptions::RetrieveDefaults()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Provides additional implementation required for retrieving JPEG specific
				defaults.
	See Also:	BitmapExportOptions::RetrieveDefaults()

********************************************************************************************/
BOOL JPEGExportOptions::RetrieveDefaults()
{
	if (!BitmapExportOptions::RetrieveDefaults())
	{
		return FALSE;
	}
	m_Quality			= g_DefaultCompression;
	m_DoAsProgressive	= g_bExportProgressive;
	m_DCTMethod			= g_DefaultDCTMethod;
	SetDepth(24); // all jpegs are 24 after all -sjk

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL JPEGExportOptions::SetAsDefaults() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Provides additional implementation to set JPEG specific info as defaults
	See Also:	BitmapExportOptions::SetAsDefaults()

********************************************************************************************/
BOOL JPEGExportOptions::SetAsDefaults() const
{
	if (!BitmapExportOptions::SetAsDefaults())
	{
		return FALSE;
	}
	
	// TODO: Fill in JPEG specific statics need sorting
	g_DefaultCompression	= m_Quality;
	g_bExportProgressive	= m_DoAsProgressive;
	g_DefaultDCTMethod		= m_DCTMethod;
	
	return TRUE;
}



/********************************************************************************************
>	virtual BOOL JPEGExportOptions::Write(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/00
	Inputs:		pRec	---		record to write
	Returns:	TRUE if successful.
	Purpose:	Writes this object to the given record.
	SeeAlso:	JPEGExportOptions::Read
********************************************************************************************/

BOOL JPEGExportOptions::Write(CXaraFileRecord* pRec)
{
	return BitmapExportOptions::Write(pRec) &&
		   pRec->WriteUINT32(m_Quality) &&
		   pRec->WriteBYTE((BYTE) !!m_DoAsProgressive) &&
		   pRec->WriteINT16((INT16) m_DCTMethod) &&
		   pRec->WriteINT16((INT16) m_ColourModel) &&
		   pRec->WriteUINT32(m_ColourComponents);
}



/********************************************************************************************
>	virtual BOOL JPEGExportOptions::Read(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/00
	Inputs:		pRec	---		record to read
	Returns:	TRUE if successful.
	Purpose:	Reads this object from the given record.
	SeeAlso:	JPEGExportOptions::Write
********************************************************************************************/

BOOL JPEGExportOptions::Read(CXaraFileRecord* pRec)
{
	m_DoAsProgressive = FALSE;
	return BitmapExportOptions::Read(pRec) &&
		   pRec->ReadUINT32((UINT32*) &m_Quality) &&
		   pRec->ReadBYTEtoBOOL(&m_DoAsProgressive) &&
		   pRec->ReadINT16((INT16*) &m_DCTMethod) &&
		   pRec->ReadINT16((INT16*) &m_ColourModel) &&
		   pRec->ReadUINT32((UINT32*) &m_ColourComponents);
}



/********************************************************************************************

>	BOOL JPEGExportOptions::DoAsProgressive() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	TRUE if the JPEG should be exported as a progressive file
				FALSE otherwise
	Purpose:	Provides a JPEG specific export parameter

********************************************************************************************/
BOOL JPEGExportOptions::DoAsProgressive() const
{
	return m_DoAsProgressive;
}


/********************************************************************************************

>	JPEG_QUALITY JPEGExportOptions::GetQuality() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	A JPEG_QUALITY value (0..100) indicating 
				FALSE otherwise
	Purpose:	Provides a JPEG specific export parameter

********************************************************************************************/
JPEG_QUALITY JPEGExportOptions::GetQuality() const
{
	return m_Quality;
}


/********************************************************************************************

>	J_DCT_METHOD JPEGExportOptions::GetDCTMethod() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	TRUE if the JPEG should be exported as a progressive file
				FALSE otherwise
	Purpose:	Provides a JPEG specific export parameter

********************************************************************************************/
libJPEG::J_DCT_METHOD JPEGExportOptions::GetDCTMethod() const
{
	return m_DCTMethod;
}


/********************************************************************************************

>	BOOL JPEGExportOptions::SetQuality(const JPEG_QUALITY& Quality)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		Quality : 0 - 100 indicating some "quality"
	Returns:	TRUE if set successfully
				FALSE otherwise
	Purpose:	Sets a JPEG specific export parameter
	Errors:		ERROR2IF Quality out of range

********************************************************************************************/
BOOL JPEGExportOptions::SetQuality(const JPEG_QUALITY& Quality)
{
	ERROR2IF(Quality < 0 || Quality > 100, FALSE, "Quality out of range");

	if (m_Quality != Quality)
		SetTempFileMatchesExportOptions(FALSE);
	m_Quality = Quality;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL JPEGExportOptions::SetMakeProgressive(BOOL bProgressive)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		bProgressive: TRUE if the JPEG should be exported as a progressive, i.e., can
				be rendered progressively if you've got the software. FALSE exports normally
	Returns:	TRUE if set successfully
				FALSE otherwise
	Purpose:	Sets a JPEG specific export parameter

********************************************************************************************/
BOOL JPEGExportOptions::SetMakeProgressive(BOOL bProgressive)
{
	if (m_DoAsProgressive != bProgressive)
		SetTempFileMatchesExportOptions(FALSE);
	m_DoAsProgressive = bProgressive;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL JPEGExportOptions::SetDCTMethod(const J_DCT_METHOD& DCTMethod)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		DCTMethod : The DCT method to be used during export (See IJG library for
				details)
	Returns:	TRUE if set successfully
				FALSE otherwise
	Purpose:	Sets an IJG specific parameter
	Notes:		NOT a user (interface) option
	Errors:		ERROR2IF DCTMethod not one of: fast,slow or float

********************************************************************************************/
BOOL JPEGExportOptions::SetDCTMethod(const libJPEG::J_DCT_METHOD& DCTMethod)
{
	ERROR2IF(DCTMethod != libJPEG::JDCT_ISLOW && DCTMethod != libJPEG::JDCT_IFAST && DCTMethod != libJPEG::JDCT_FLOAT,
				FALSE, "Invalid DCT method");

	m_DCTMethod = DCTMethod;

	return TRUE;
}


/********************************************************************************************

>	BOOL JPEGExportOptions::SetColourModel(const J_COLOR_SPACE& ColourModel)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		ColourModel : The J_COLOR_SPACE that the bitmap is to be exported as
	Returns:	TRUE if set successfully
				FALSE otherwise
	Purpose:	Sets an IJG specific parameter
	Notes:		The bitmap is expected to be of the correct format or a converter (DIBConvert)
				provided when the bitmap is written.
	Errors:		ERROR2IF DCTMethod not one of: JCS_RGB or JCS_GRAYSCALE
	See Also:	JPEGExportFilter::WriteRawBitmap()

********************************************************************************************/
BOOL JPEGExportOptions::SetColourModel(const libJPEG::J_COLOR_SPACE& ColourModel)
{
	switch (ColourModel)
	{
		case libJPEG::JCS_RGB:
			// This really should be RGB_PIXELSIZE (in jmorecfg.h), but this isn't exported!
			m_ColourComponents = 3;
			break;

		case libJPEG::JCS_GRAYSCALE:
			m_ColourComponents = 1;
			break;

		default:
			ERROR2(FALSE, "Invalid Colour Model method");
			
	}

	m_ColourModel = ColourModel;

	return TRUE;		// success
}


/********************************************************************************************

>	J_COLOR_SPACE JPEGExportOptions::GetColourModel() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	The J_COLOR_SPACE to be used for export
	Purpose:	Provides a IJG specific export parameter
	See Also:	GetColourComponentCount()

********************************************************************************************/
libJPEG::J_COLOR_SPACE JPEGExportOptions::GetColourModel() const
{
	return m_ColourModel;
}


/********************************************************************************************

>	libJPEG::J_COLOR_SPACE JPEGExportOptions::GetColourModel() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	The number of colour components associated with the J_COLOR_SPACE returned by
				GetColourModel()
	Purpose:	Provides an IJG specific export parameter
	See Also:	GetColourComponentCount()

********************************************************************************************/
UINT32 JPEGExportOptions::GetColourComponentCount() const
{
	return m_ColourComponents;
}

/********************************************************************************************

>	JPEGExportFilter::JPEGExportFilter()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Default constructor for a JPEGFilter object.

********************************************************************************************/
JPEGExportFilter::JPEGExportFilter() : BaseBitmapFilter()
{
	// Initialize members for class Filter
	ExportMsgID =		_R(IDS_EXPORTMSG_JPG);			// "Preparing JPEG file..."
	ExportingMsgID =	_R(IDS_EXPORTINGMSG_JPG);		// "Exporting JPEG file..."

	Flags.CanImport					= FALSE;
	Flags.CanExport					= TRUE;
	Flags.CanExportMultipleImages	= FALSE;
	Flags.ShowFilter				= TRUE;

	FilterID = FILTERID_EXPORT_JPEG;

	// Initialize our class members
	m_bOldReportErrors		= FALSE;
	m_bOldThrowExceptions	= FALSE;

	m_pFilterForUpdate	= NULL;

	m_pErrorHandler			= NULL;
	m_pDestinationHandler	= NULL;
	m_PostOperation = TRUE;
}


/********************************************************************************************

>	JPEGExportFilter::~JPEGExportFilter()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Default destructor for a JPEGExportFilter object.

********************************************************************************************/
JPEGExportFilter::~JPEGExportFilter()
{
	if (m_pErrorHandler != NULL)
		delete m_pErrorHandler;

	if (m_pDestinationHandler != NULL)
		delete m_pDestinationHandler;
}


/********************************************************************************************

>	BOOL JPEGExportFilter::Init()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	TRUE if the filter was initialized ok
				FALSE otherwise
	Purpose:	Initializes the JPEGExportFilter class.

********************************************************************************************/
BOOL JPEGExportFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new JPEGExportOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDS_JPG_IMP_FILTERNAME));
	FilterInfo.Load(_R(IDS_JPG_IMP_FILTERINFO));

	if (DeclareFilterPreferenceSection())
	{
		Camelot.DeclarePref( NULL, g_szCompression, &g_DefaultCompression, 0, 100 );
		Camelot.DeclarePref( NULL, g_szDCTMethod, (INT32*)&g_DefaultDCTMethod, 0, 2 );
		Camelot.DeclarePref( NULL, g_szProgressive, &g_bExportProgressive, 0, 1 );
		Camelot.DeclarePref( NULL, g_szOptimize, &g_bOptimizeCoding, 0, 1 );
		Camelot.DeclarePref( NULL, g_szSmoothing,  &g_Smoothing, 0, 100 );

//		DeclarePreference(Compression);
//		DeclarePreference(DCTMethod);
//		DeclarePreference(Progressive);
//		DeclarePreference(Xppm);
//		DeclarePreference(Yppm);
	}
	// All ok
	return TRUE;
}



/********************************************************************************************

>	virtual BOOL JPEGExportFilter::GetRenderBottomToTop()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	See BaseBitmapFilter for details
	Notes:		From IJG docs:
				"Image data should be written in top-to-bottom scanline order.
				The JPEG spec contains some weasel wording about how top and bottom are
				application-defined terms (a curious interpretation of the English 
				language...) but if you want your files to be compatible with everyone else's,
				you WILL use top-to-bottom order."

********************************************************************************************/
BOOL JPEGExportFilter::GetRenderBottomToTop()
{
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL JPEGExportFilter::IsThisBppOk(UINT32 Bpp)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		See BaseBitmapFilter for interface details
	Purpose:	Overriden for JPEG specifics: supports 24 & 8

********************************************************************************************/
BOOL JPEGExportFilter::IsThisBppOk(UINT32 Bpp)
{
	return (Bpp == 24 || Bpp == 8);
}


/********************************************************************************************

>	virtual BOOL JPEGExportFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	To duplicate code
	SeeAlso:	BaseBitmapFilter::WriteBitmapToFile

********************************************************************************************/
BOOL JPEGExportFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi)
{
	ERROR2IF(pKernelBitmap == NULL, FALSE, "NULL Args");
	ERROR3IF(!pKernelBitmap->IS_KIND_OF(KernelBitmap), "pKernelBitmap isn't");

	BOOL bSuccess = FALSE;
	JPEGExportOptions* pOptions = (JPEGExportOptions*)GetBitmapExportOptions();
	pOptions->RetrieveDefaults();
	pOptions->SetDPI(Dpi);

	// export the jpeg just as it is (sjk 21/12/00)
	pOptions->m_bJPEGPresentAndSelected = TRUE;

	SetFilterForUpdate(NULL);

	// Set up the progress bar
	ProgressOffset = 0;	// reset this Filter member!!!  so WriteBitmap() does progress correctly

	OILBitmap* pOILBitmap = pKernelBitmap->GetActualBitmap();
	String_64 ProgressString = GetExportProgressString(GetExportFile(), GetExportMsgID());
	BeginSlowJob(pOILBitmap->GetHeight(), FALSE, &ProgressString);

	bSuccess = DoFilter(pKernelBitmap, GetExportFile());

	EndSlowJob();
	return bSuccess;
}

/********************************************************************************************

>	virtual BOOL JPEGExportFilter::WriteBitmapToFile(	KernelBitmap* pKernelBitmap, 
														BaseCamelotFilter *pFilter,
														CCLexFile *pFile, INT32 Compression)
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	To duplicate code
	SeeAlso:	BaseBitmapFilter::WriteBitmapToFile

********************************************************************************************/
BOOL JPEGExportFilter::WriteBitmapToFile(	KernelBitmap* pKernelBitmap,
											BaseCamelotFilter* pFilter,
											CCLexFile* pFile, INT32 Compression)
{
	ERROR2IF(pKernelBitmap == NULL || pFilter == NULL || pFile == NULL, FALSE, "NULL Args");

	ERROR3IF(!pKernelBitmap->IS_KIND_OF(KernelBitmap), "pKernelBitmap isn't");
	ERROR3IF(!pFilter->IS_KIND_OF(BaseCamelotFilter), "pFilter isn't");
	ERROR3IF(!pFile->IS_KIND_OF(CCLexFile), "pFile isn't");

	if (!BaseBitmapFilter::WriteBitmapToFile(pKernelBitmap, pFilter, pFile, Compression))
	{
		return FALSE;
	}

	SetFilterForUpdate(pFilter);

	JPEGExportOptions* pOptions = (JPEGExportOptions*)GetBitmapExportOptions();
	pOptions->RetrieveDefaults();
	pOptions->SetQuality(Compression);

	if (!DoFilter(pKernelBitmap, pFile))
	{
		return FALSE;
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL JPEGExportFilter::WriteBitmapSource(const BitmapSource& Source, UINT32 Height, 
													BaseCamelotFilter* pThisFilter)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com> (From Colin)
	Created:	22/01/97
	Inputs:		Source : the BitmapSource to write out
				Height : the height of the bitmap in this source
				pThisFilter : the filter whose IncProgressBarCount we will use for the
				progress indicator
	Returns:	TRUE if written successfully
				FALSE otherwise
	Purpose:	Writes out the given BitmapSource to this file.

	Errors:		ERROR2's if Height is zero or pThisFilter NULL
	See Also:	CXaraFile::WriteBitmapSource

********************************************************************************************/

BOOL JPEGExportFilter::WriteBitmapSource(BitmapSource *Source, UINT32 Height,BaseCamelotFilter* pThisFilter,CCLexFile* pCCFile)
{
	ERROR2IF(pThisFilter == NULL, FALSE, "pThisFilter == NULL");
	ERROR2IF(Height == 0, FALSE, "Height == 0");
	
	CCBufferFile* pBufferFile = new CCBufferFile(pCCFile);
	// Now create a file that can fill it
	if (pBufferFile == NULL || !pBufferFile->IsInited())
	{
		return FALSE;
	}
	// Provide the filter with something to work with
	Source->AttachToBufferFile(pBufferFile);

	// Work out how much to increment the progress bar by on each loop
	OFFSET Size = Source->GetSize();
	UINT32 Increment = (Height * CCBufferFile::DEFAULT_BUFSIZ) / Size;
	if (Increment > Height)
	{
		Increment = Height;
	}
	else if (Increment < 1)
	{
		Increment = 1;
	}
	UINT32 UpdateEvery = Size / (Height * CCBufferFile::DEFAULT_BUFSIZ);

	// Write out the BitmapSource DEFAULT_BUFSIZ bytes at a time
	UINT32 WritesSinceUpdate = 0;
	while (!pBufferFile->IsAllWritten())
	{
		pBufferFile->write(NULL, CCBufferFile::DEFAULT_BUFSIZ);
		if (pBufferFile->bad() || pBufferFile->fail())
		{
			delete pBufferFile;
			return FALSE;
		}

		// We've made some progress so provide some user feedback
		++WritesSinceUpdate;
		if (WritesSinceUpdate >= UpdateEvery)
		{
			pThisFilter->IncProgressBarCount(Increment);
			WritesSinceUpdate = 0;
		}
	};

	// Destroy our file handle
	delete pBufferFile;

	return TRUE;
}



/********************************************************************************************

>	virtual BOOL BOOL JPEGExportFilter::DoFilter(	KernelBitmap* pKernelBitmap, 
													CCLexFile *pFile)
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Support function to reduce duplicate code
	SeeAlso:	JPEGExportFilter::WriteBitmapToFile

********************************************************************************************/
BOOL JPEGExportFilter::DoFilter(KernelBitmap* pKernelBitmap, CCLexFile *pFile)
{
	ERROR2IF(pKernelBitmap == NULL || pFile == NULL, FALSE, "NULL Args");
	ERROR3IF(!pKernelBitmap->IS_KIND_OF(KernelBitmap), "pKernelBitmap isn't");
	ERROR3IF(!pFile->IS_KIND_OF(CCLexFile), "pFile isn't");

	JPEGExportOptions* pOptions = (JPEGExportOptions*)GetBitmapExportOptions();

	if (pOptions == NULL)
		return FALSE;

	OILBitmap* pOily = pKernelBitmap->GetActualBitmap();

	if (pOptions->GetJPEGPresentAndSelected())
	{
		SetPostOperation(FALSE);

		// Get the origianl source. 
		BitmapSource* pSource = NULL;
		BaseBitmapFilter* pDummyFilter;

		KernelBitmap* pBitmap = pOptions->GetKernelBitmap();

		if (pBitmap == NULL)
			return FALSE;

		// set our KernelBitmap pointer to NULL.
		pOptions->SetKernelBitmap(NULL);

		pBitmap->GetOriginalSource(&pSource,&pDummyFilter);
		OILBitmap* pOILBitmap = pBitmap->ActualBitmap;

		if (pOILBitmap == NULL)
			return FALSE;

		BitmapInfo Info;
		pOILBitmap->GetInfo(&Info);
		UINT32 Height = Info.PixelHeight;
						
		// Write out the orginal JPEG image.
		if(WriteBitmapSource(pSource, Height, (BaseCamelotFilter*) pDummyFilter, pFile))
			return TRUE;
	}
	else
	{
		BOOL bOK = FALSE;
		bOK = PrepareForOperation();
		DIBConvert* pConverter = NULL;

		switch (pOily->GetBPP())
		{
			case 8:
				if (pKernelBitmap->IsGreyscale())
				{
					pOptions->SetColourModel(libJPEG::JCS_GRAYSCALE);
				}
				else
				{
					pOptions->SetColourModel(libJPEG::JCS_RGB);
				}
				break;

			case 24:
				pOptions->SetColourModel(libJPEG::JCS_RGB);
				break;
	
			default:
				ERROR3("DoFilter: invalid bpp");
				bOK = FALSE;
				break;
		}
		// Create a converter from bitmap depth to export depth
//		pConverter = DIBConvert::Create(((WinBitmap*)pOily)->BMInfo, 
//										(8 * pOptions->GetColourComponentCount()));
		pConverter = DIBConvert::Create(((CWxBitmap*)pOily)->BMInfo,
										(8 * pOptions->GetColourComponentCount()));

		if (bOK)
		{
			SetPixelWidth(pOily->GetWidth());
			SetPixelHeight(pOily->GetHeight());
			SetDepthToRender(pOily->GetBPP());

			bOK = InternalPrepareToExport();
		}

		if (bOK)
		{
			bOK = WriteRawBitmap(	pKernelBitmap->GetBitmapBits(), 
									GetPixelWidth(), GetPixelHeight(), pOily->GetBPP(),
									pConverter);
		}
	
		if (bOK)
		{
			bOK = WritePostFrame();
		}


		if (pConverter != NULL)
		{
			delete pConverter;
		}
		CleanUpAfterExport();

		// set our KernelBitmap pointer to NULL.
		pOptions->SetKernelBitmap(NULL);
		return bOK;
	}
	// set our KernelBitmap pointer to NULL.
	pOptions->SetKernelBitmap(NULL);
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL JPEGExportFilter::PrepareForOperation()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Support function to do something before any actual output

********************************************************************************************/
BOOL JPEGExportFilter::PrepareForOperation()
{
	// The error handling in the following should probably be replaced by exceptions at
	// some time, when somebody can decide on a standard.

	// Set up a JPEGErrorManager
	if (!InitErrorHandler())
	{
		return FALSE;
	}
	
	// Initialize the main JPG library structure
	try
	{
		using namespace libJPEG;
		jpeg_create_compress(&m_cinfo);
	}
	catch (...)
	{
		StringID errorString = m_pErrorHandler->GetStringIDForError();
		Error::SetError(errorString);
		return FALSE;
	}

	// Setup a JPEGDataDestination
	if (!InitFileHandler())
	{
		return FALSE;
	}

	// Don't let the CCLexFile report errors or throw exceptions
	m_bOldThrowExceptions	= GetExportFile()->SetThrowExceptions(FALSE);
	m_bOldReportErrors		= GetExportFile()->SetReportErrors(FALSE);

	// Shouldn't have to do this
	PaletteType		= PAL_STANDARD;	// don't - HATE this

	SetDepthToRender(24);	// immer but not necessarily what we get

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL JPEGExportFilter::GetExportOptions(BitmapExportOptions* pOptions)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Overridden version provides JPEG export dialog
	Scope: 		Protected
	See Also:	BaseBitmapFilter::GetBitmapExportOptions()

********************************************************************************************/
BOOL JPEGExportFilter::GetExportOptions(BitmapExportOptions* pOptions)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR2IF(pOptions == NULL, FALSE, "NULL Args");

	BOOL Ok = TRUE;

//	JPEGExportOptions* pJPEGOptions = (JPEGExportOptions*)pOptions;
//	ERROR3IF(!pJPEGOptions->IS_KIND_OF(JPEGExportOptions), "pJPEGOptions isn't");

	OpDescriptor* pOpDes = OpDescriptor::FindOpDescriptor(OPTOKEN_GIFTABDLG);
	if (pOpDes != NULL)
	{
		// set up the data for the export options dialog
		OpParam Param( pOptions, this );

		// invoke the dialog
		pOpDes->Invoke(&Param);

		// SMFIX
		// we have brought the dlg up so get the options from the dlg as the graphic type may have changed
		pOptions = BmapPrevDlg::m_pExportOptions;

		// check for valid options
		//  This may get messed up, so have to use the second line below.
		Ok = BmapPrevDlg::m_bClickedOnExport;	
		
	}
	else
	{	
		ERROR3("Unable to find OPTOKEN_GIFTABDLG");
	} 

/*	if (Ok)
	{
		// If Ok used then set the new defaults
		pJPEGOptions->SetAsDefaults();
	
		pJPEGOptions->SetDepth(24);		// Always output at 24bpp

		SetDepthToRender(24);
	}
*/
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
void JPEGExportFilter::PostGetExportOptions(BitmapExportOptions* pOptions)
{
	// should be of this type

	// do the baseclass options
	BaseBitmapFilter::PostGetExportOptions(pOptions);

	// do the specific to this class options
	pOptions->SetDepth(24);		// Always output at 24bpp
	SetDepthToRender(24);
}

/********************************************************************************************

>	BOOL JPEGExportFilter::PrepareToExport(Spread *pSpread, UINT32 Depth, double DPI,
									   SelectionType Selection, UINT32 Dither)
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	FALSE if failed else TRUE
	Purpose:	Provides additional processing to the base class to start the JPEG
				compression.
	SeeAlso:	BaseBitmapFilter::PrepareToExport()

********************************************************************************************/
BOOL JPEGExportFilter::PrepareToExport(Spread *pSpread, UINT32 Depth, double DPI,
									   SelectionType Selection, UINT32 Dither)
{
	if (!BaseBitmapFilter::PrepareToExport(pSpread, Depth, DPI, Selection, Dither))
	{
		return FALSE;
	}

	SetFilterForUpdate(NULL);

	if (!((JPEGExportOptions*)GetBitmapExportOptions())->SetColourModel(libJPEG::JCS_RGB))
	{
		ERROR2(FALSE,"Can't set colour model");
		return FALSE;
	}

	if (!InternalPrepareToExport())
	{
		return FALSE;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL JPEGExportFilter::InternalPrepareToExport()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	FALSE if failed else TRUE
	Purpose:	Support function setting up the IJG library options and starting the
				compression.
	SeeAlso:	PrepareToExport(), DoFilter() (callers)

********************************************************************************************/
BOOL JPEGExportFilter::InternalPrepareToExport()
{
	JPEGExportOptions* pExportInfo = (JPEGExportOptions*)GetBitmapExportOptions();
	ERROR3IF(!pExportInfo->IS_KIND_OF(JPEGExportOptions), "pExportInfo isn't");

	m_cinfo.image_width		= GetPixelWidth();
	m_cinfo.image_height	= GetPixelHeight();

	m_cinfo.in_color_space		= pExportInfo->GetColourModel();
	m_cinfo.input_components	= pExportInfo->GetColourComponentCount();

	try
	{
		libJPEG::jpeg_set_defaults(&m_cinfo);

		if (pExportInfo->DoAsProgressive())
		{
			libJPEG::jpeg_simple_progression(&m_cinfo);
		}

		m_cinfo.density_unit	= 1;	// that's JFIF for DPI
		m_cinfo.X_density		= UINT16 (pExportInfo->GetDPI());
		m_cinfo.Y_density		= UINT16 (pExportInfo->GetDPI());

		libJPEG::jpeg_set_quality (&m_cinfo, pExportInfo->GetQuality(), TRUE);

		m_cinfo.optimize_coding		= g_bOptimizeCoding;
		m_cinfo.smoothing_factor	= g_Smoothing;

		// DCT algorithm preference
		m_cinfo.dct_method = pExportInfo->GetDCTMethod();


		libJPEG::jpeg_start_compress(&m_cinfo, TRUE);	// "TRUE" for complete JPEG interchange datastream
	}
	catch (...)
	{
		StringID errorString = m_pErrorHandler->GetStringIDForError();
		Error::SetError(errorString);
		return FALSE;
	}

	return TRUE;
}



/********************************************************************************************

>	virtual BOOL JPEGExportFilter::WriteFileHeader(void)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out the file specific header data: actually sets up the JPEG filter
	SeeAlso:	BaseBitmapFilter::WriteFileHeader()

********************************************************************************************/
BOOL JPEGExportFilter::WriteFileHeader(void)
{
	BOOL ok = FALSE;

	ok = PrepareForOperation();

	return ok;
}


/********************************************************************************************

>	virtual BOOL JPEGExportFilter::WriteFrame(void)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	FALSE if failed else TRUE
	Purpose:	Overrides the  image itself
				This base class version actually calls the WriteToFile() function so that
				derived classes do not have to implement any of the multi-image stuff
	SeeAlso:	BaseBitmapFilter::WriteFrame(void)

8. Aborting.

If you decide to abort a compression cycle before finishing, you can clean up
in either of two ways:

* If you don't need the JPEG object any more, just call
  jpeg_destroy_compress() or jpeg_destroy() to release memory.  This is
  legitimate at any point after calling jpeg_create_compress() --- in fact,
  it's safe even if jpeg_create_compress() fails.

* If you want to re-use the JPEG object, call jpeg_abort_compress(), or
  jpeg_abort() which works on both compression and decompression objects.
  This will return the object to an idle state, releasing any working memory.
  jpeg_abort() is allowed at any time after successful object creation.

Note that cleaning up the data destination, if required, is your
responsibility.

********************************************************************************************/
BOOL JPEGExportFilter::WriteFrame(void)
{
	BOOL	Ok;

	BMP_SIZE	Width, Height;
	BMP_DEPTH	Depth;
	ADDR		pBitmapBits;
	
	Ok = GetCurrentStripInfo(&pBitmapBits, &Width, &Height, &Depth);

	// Create a converter from bitmap depth to export depth
	DIBConvert* pConverter = DIBConvert::Create( Depth, GetRenderDepth(), Width, NULL );

	if (Ok)
	{
		Ok = WriteRawBitmap( pBitmapBits, Width, Height, Depth, pConverter );
	}

	return Ok;
}



/********************************************************************************************

>	virtual BOOL JPEGExportFilter::WriteRawBitmap(	const ADDR& pBitmapData, 
										const BMP_SIZE& Width, const BMP_SIZE& Height,
										const BMP_DEPTH& Bpp,
										DIBConvert* pConverter)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		pBitmapData : The bits, as in "bitmap"
				Width:		The bitmap width in pixels
				Height:		The bitmap height in pixels
				Depth:		The bitmap depth in bits per pixel
				pConverter:	A pointer to a DIBConvert to convert the actual bitmap given to
							32bpp data
	Returns:	FALSE if failed else TRUE
	Purpose:	Support function for writing out the bitmap scanlines
				Called by WriteFrame() &  WriteBitmap()

********************************************************************************************/
BOOL JPEGExportFilter::WriteRawBitmap(	const ADDR& pBitmapData, 
										const BMP_SIZE& Width, const BMP_SIZE& Height,
										const BMP_DEPTH& Bpp,
										DIBConvert* pConverter)
{
//	ERROR2IF(pConverter == NULL && Bpp != (BMP_DEPTH)GetRenderDepth(), FALSE, "Parameters incorrect");

	const UINT32 ScanlineSize = DIBUtil::ScanlineSize( Width, Bpp );
	const UINT32 ExportlineSize = DIBUtil::ScanlineSize( Width, 32 );
	Filter* const pFilterForUpdate = GetFilterForUpdate();

	ADDR pExportBuffer = NULL;
	ADDR pBitmapLine = pBitmapData;
	ADDR pExportLine = NULL;


	if (pConverter != NULL)
	{
		pExportBuffer = new BYTE[ExportlineSize];
		pExportLine = pExportBuffer;
	}

	try
	{
		UINT32 ScanlinesRemaining = Height;
		pBitmapLine += (ScanlinesRemaining - 1) * ScanlineSize;

		// Do it one line at a time like it or not
		while (ScanlinesRemaining > 0)
		{
			// Convert the bitmap to 32bpp if necessary
			if (pConverter == NULL)
			{
				pExportLine = pBitmapLine;
			}
			else
			{
				pConverter->Convert(pBitmapLine, pExportBuffer, 1, FALSE);
			}

#if !defined(__WXMSW__) && defined(BIG_ENDIAN)
			// Component swapping must be done out-of-line, to stop original
			// being blatted
			RGBTRIPLE* pExportRGB = (RGBTRIPLE*)pExportLine;
			if( pExportLine == pBitmapLine )
			{
				pExportRGB = (RGBTRIPLE*)alloca( ExportlineSize );
				memcpy( pExportRGB, pExportLine, ExportlineSize );
				pExportLine = PBYTE(pExportRGB);
			}
			for( unsigned ord = 0; ord < Width; ++ord, ++ pExportRGB )
				std::swap( pExportRGB->rgbtBlue, pExportRGB->rgbtRed );
#endif

			libJPEG::jpeg_write_scanlines(&m_cinfo, &pExportLine, 1);

			if (pFilterForUpdate != NULL)
			{
				// Ask the pFilter to update the progress bar for us
				pFilterForUpdate->IncProgressBarCount(1);
			}
			else
			{
				ContinueSlowJob(ProgressOffset + (Height - ScanlinesRemaining));
			}

			pBitmapLine -= ScanlineSize;
			--ScanlinesRemaining;
		}
	}
	catch (...)
	{
		if (pExportBuffer != NULL)
		{
			delete pExportBuffer;
		}
		StringID errorString = m_pErrorHandler->GetStringIDForError();
		Error::SetError(errorString);

		return FALSE;
	}

	if (pExportBuffer != NULL)
	{
		delete pExportBuffer;
	}

	if (pFilterForUpdate == NULL)
	{
		// Add in the height of the strip that we have just exported to the progress offset value
		ProgressOffset += Height;
	}

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL JPEGExportFilter::WritePostFrame(void)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	FALSE if failed else TRUE
	Purpose:	Overrides base class to finish JPEG compression cycle of the current frame
	SeeAlso:	BaseBitmapFilter

********************************************************************************************/
BOOL JPEGExportFilter::WritePostFrame(void)
{
	try
	{
		libJPEG::jpeg_finish_compress(&m_cinfo);
	}
	catch (...)
	{
		StringID errorString = m_pErrorHandler->GetStringIDForError();
		Error::SetError(errorString);

		return FALSE;
	}

	return TRUE;
}



/********************************************************************************************

>	virtual BOOL JPEGExportFilter::EndWriteToFile(void)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Overrides base class: does nothing
	See Also:	BaseBitmapFilter::EndWriteToFile(void)

********************************************************************************************/
BOOL JPEGExportFilter::EndWriteToFile(void)
{
	return TRUE;
}


/********************************************************************************************

>	virtual void JPEGExportFilter::CleanUpAfterExport()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Overridden to clean up JPEG specific mess
	SeeAlso:	BaseBitmapFilter::CleanUpAfterExport()

********************************************************************************************/
void JPEGExportFilter::CleanUpAfterExport()
{
	if (GetPostOperation())
	{
		libJPEG::jpeg_destroy_compress(&m_cinfo);
	}

	if (m_pDestinationHandler != NULL)
	{
		delete m_pDestinationHandler;
		m_pDestinationHandler = NULL;
	}

	if (m_pErrorHandler != NULL)
	{
		delete m_pErrorHandler;
		m_pErrorHandler = NULL;
	}

	// Now call the baseclass version to do its stuff
	BaseBitmapFilter::CleanUpAfterExport();
}


/********************************************************************************************

>	BOOL JPEGExportFilter::InitErrorHandler()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Creates & initializes an error handler for this filter

********************************************************************************************/
BOOL JPEGExportFilter::InitErrorHandler()
{
	if (m_pErrorHandler == NULL)
	{
		m_pErrorHandler = new JPEGErrorManager;
		if (m_pErrorHandler == NULL)
		{
			Error::SetError(_R(IDS_OUT_OF_MEMORY));
			return FALSE;
		}

	}
	m_cinfo.err = m_pErrorHandler->GetErrorMgrStruct();
	return TRUE;
}


/********************************************************************************************

>	BOOL JPEGExportFilter::InitFileHandler()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Creates & initializes a source data provider for this filter.

********************************************************************************************/
BOOL JPEGExportFilter::InitFileHandler()
{
	SetPostOperation(TRUE);

	if (m_cinfo.dest == NULL)
	{
		if (m_pDestinationHandler == NULL)
		{
			m_pDestinationHandler = new JPEGDataDestination(GetExportFile());
			if (m_pDestinationHandler == NULL)
			{
				Error::SetError(_R(IDS_OUT_OF_MEMORY));
				return FALSE;
			}
		}
		m_pDestinationHandler->Init(&m_cinfo);
	}
	return TRUE;
}
		

/********************************************************************************************

>	virtual BitmapExportOptions* JPEGExportFilter::CreateExportOptions() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Overrides base class to provide JPEGExportOptions
	See Also:	BaseBitmapFilter::CreateExportOptions()

********************************************************************************************/
BitmapExportOptions* JPEGExportFilter::CreateExportOptions() const
{
	JPEGExportOptions* pExportOptions = new JPEGExportOptions(JPEG, &FilterName);

	return (BitmapExportOptions*)pExportOptions;
}


/********************************************************************************************

>	virtual BOOL JPEGExportFilter::SetExportHint(Document* pDoc)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/07/97
	Purpose:	Overrides base class to set correct hint
	See Also:	BaseBitmapFilter::SetExportHint()

********************************************************************************************/

BOOL JPEGExportFilter::SetExportHint(Document* pDoc)
{
	BOOL Ok = TRUE;

	if (pDoc != NULL)
	{
		ExportHint* pHint = pDoc->GetExportHint();	
		ERROR3IF(pHint == NULL, "NULL ExportHint");
		
		BitmapExportOptions* pBaseOptions = GetBitmapExportOptions();
		ERROR3IF(pBaseOptions == NULL, "NULL Options");

		if (pHint != NULL && pBaseOptions != NULL)
		{
			ERROR2IF(!(IS_A(pBaseOptions, JPEGExportOptions)), FALSE, "Incorrect type of options");
			JPEGExportOptions* pOptions = (JPEGExportOptions*) pBaseOptions;
			
			pHint->SetType(HINTTYPE_JPEG);
			pHint->SetWidth(GetPixelWidth());
			pHint->SetHeight(GetPixelHeight());
			pHint->SetBPP(pOptions->GetDepth());

			String_256 Opts;

			Opts._MakeMsg(_T("Q#1%ld"), pOptions->GetQuality());

			String_32 DPIString;
			Ok = Convert::DoubleToString( pOptions->GetDPI(), &DPIString, 3);

			Opts += _T(" D");
			Opts += DPIString;
			
			if (pOptions->DoAsProgressive())
			{
				Opts += _T(" P");
			}

			pHint->SetOptionsString(Opts);
		}
	}

	return Ok;
}


/********************************************************************************************

>	Filter* JPEGExportFilter::GetFilterForUpdate() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	A pointer to the filter whose member IncProgressBarCount() is called during
				the export for progress update or NULL if no filter is to be used for such
	Purpose:	Allows feedback to any primary filter

********************************************************************************************/
Filter* JPEGExportFilter::GetFilterForUpdate() const
{
	return m_pFilterForUpdate;
}


/********************************************************************************************

>	BOOL JPEGExportFilter::SetFilterForUpdate(Filter* const pFilterForUpdate)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	TRUE if successfully set,
				FALSE otherwise.
	Purpose:	Sets the filter for any progress update on export
	Notes:		Should be set to NULL for ordinary import
	Errors:		ERROR3IF invalid arg type

********************************************************************************************/
BOOL JPEGExportFilter::SetFilterForUpdate(Filter* const pFilterForUpdate)
{
	ERROR3IF(pFilterForUpdate != NULL && !pFilterForUpdate->IS_KIND_OF(Filter), 
			"pFilterForUpdate isn't");

	m_pFilterForUpdate = pFilterForUpdate;

	return TRUE;
}




/********************************************************************************************

>	BOOL JPEGExportFilter::DoExportBitmapWithOptions(Operation* pOp, CCLexFile* pFile, PathName* pPath,
												KernelBitmap* pBitmap, BitmapExportOptions *pOptions)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:			
	Purpose:	Exports the specified bitmap straight out to file with none of the rendering
				that DoExport does. Uses the virtual fns of the	inherited class.
				Do not override unless really necessary. Similar to DoExportBitmap in the base
				class, but uses the passed options, rather then invoking a dialog to get ones.
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL JPEGExportFilter::DoExportBitmapWithOptions(Operation* pOp, CCLexFile* pFile, PathName* pPath,
												KernelBitmap* pBitmap, BitmapExportOptions *pOptions)
{
#ifdef DO_EXPORT
	ERROR2IF(pBitmap == NULL,FALSE,"BaseBitmapFilter::DoExportBitmap null bitmap pointer specified");

	if (pOptions == NULL)
	{
		// Create a record of information about the export
		SetUpExportBitmapOptions(&pOptions, pBitmap, TRUE);
	}

	if (pOptions == NULL)
		return FALSE;

	// remember the old export options
//	BitmapExportOptions *pOldOptions = GetBitmapExportOptions();
//	if (pOldOptions != NULL)
//		pOldOptions->RetrieveDefaults();

	// Added by Craig Hamilton 11/9/00.
	// Make a copy of the old options since the pointer to the old options gets vaped by
	// SetExportOptions. The copy is used to reinitialise pOldOptions later.
//	BitmapExportOptions *tempOptions = pOldOptions->MakeCopy();
	// End added.
	
	// set the new ones
	SetExportOptions(pOptions);
	pOptions->SetAsDefaults();

	// Make a note of the Disk file we are to use
	OutputFile = pFile;

	// Note this pointer for later use
	pExportBitmap = pBitmap;

	// Note this ptr for use in JPEG export.
	JPEGExportOptions::SetKernelBitmap(pBitmap);
	
	// Get a pointer to the actual bitmap so that we can get some details from it.
	OILBitmap *pOilBitmap = pBitmap->ActualBitmap;
	ERROR2IF(pOilBitmap == NULL,FALSE,"BaseBitmapFilter::DoExportBitmap null oil bitmap pointer");

	// Get the details from the specified bitmap
	BitmapInfo BmInfo;
	pOilBitmap->GetInfo(&BmInfo);
	GetBitmapExportOptions()->SetDepth(BmInfo.PixelDepth);	// get the bitmaps bpp
	
	// Should really save the dpi when we load the file itself rather than doing
	// all this conversion with possible rounding errors.
	// Use the original size that has been calculated in the info header
	UINT32 PixWidth  = BmInfo.PixelWidth;
//	UINT32 PixHeight = BmInfo.PixelHeight;
	MILLIPOINT	RecWidth = BmInfo.RecommendedWidth;
//	MILLIPOINT	RecHeight = BmInfo.RecommendedHeight;
//	double DPI = 0.0;
	if (PixWidth > 0)
	{
		//DPI = Mul32Div32( PixWidth,  72000, RecWidth );
		GetBitmapExportOptions()->SetDPI((PixWidth * 72000.0)/(double)RecWidth);
	}

	BOOL ok = TRUE;

	// Used to open the file up before starting DoExport. But this meant a cancel on the export
	// options dialog had filled the file, if it was already present. So now up up here if
	// not open already. In the PreviewBitmap case the file will already be open.
	if (!pFile->isOpen())
	{
		if (pFile->IsKindOf(CC_RUNTIME_CLASS(CCDiskFile)))
		{
			ok = OpenExportFile((CCDiskFile*) pFile, pPath);
			if (!ok) return FALSE;
		}
		else
		{
			TRACEUSER( "JustinF", _T("Tried to open non-CCDiskFile in BaseBitmapFilter::DoExportBitmap\n"));
			return FALSE;
		}
	}

	// Make a note of the Disk file we are to use
	OutputFile = pFile;

	// We do not use an export region so specify null.
	ExportRegion = NULL;
	
	// Actually write to the file, showing progress hourglass as we go
	ok = WriteBitmapToFile(pBitmap, GetBitmapExportOptions()->GetDPI());

	if (ok)
		WriteFileEnd();

	// All done - deallocate dynamic objects, stop the progress display/hourglass
	// and return success. (Also closes file).
	CleanUpAfterExport();

	// restore the old export options
//	if (pOldOptions != NULL)
//	{
		// Added by Craig Hamilton 11/9/00.
		// With the call to SetExportOptions the memory pointed to by pOldOptions is vaped.
		// Use tempOptions to reinitialise or get an access violation.
//		pOldOptions = tempOptions->MakeCopy();
		// End added.

//		SetExportOptions(pOldOptions);
//		pOldOptions->SetAsDefaults();
//	}

	// restore the file
	OutputFile = pFile;

	return ok;
#else
	return FALSE;
#endif
}

