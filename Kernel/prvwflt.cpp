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

// The preview filters. Basically the same as the normal filter but never puts
// a dialog up for the export options - always provides sensible defaults for
// the preview.


#include "camtypes.h"

#include "prvwflt.h"

#include "oilfltrs.h"
//#include "accures.h"	// Accusoft errors and messages - and lots of resource ids
//#include "rik.h"		// _R(IDS_BUILDINGPREVIEW)
//#include "filtrres.h"	// _R(IDS_IMPORTMSG_PNG)
//#include "andy.h"		// _R(IDT_IMPORTMSG_BMP)
//#include "tim.h"		// _R(IDT_AIEPS_FILTERNAME)
#include "grndbmp.h"	// For GRenderBitmap.
#include "ai_epsrr.h"	// For AIEPSRenderRegion.
#include "docview.h"
#include "view.h"
						
// This is not compulsory, registers the correct version of your file in the .exe
DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(PreviewFilter, Filter)

CC_IMPLEMENT_DYNAMIC(PreviewFilterBMP, BMPFilter)
CC_IMPLEMENT_DYNAMIC(PreviewFilterGIF, TI_GIFFilter)
CC_IMPLEMENT_MEMDUMP(PreviewFilterJPEG, JPEGExportFilter)
CC_IMPLEMENT_DYNAMIC(PreviewFilterPNG, PNGFilter)
CC_IMPLEMENT_DYNAMIC(ThumbnailFilterPNG, PreviewFilterPNG)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW


/********************************************************************************************

	Preference:		PreviewBitmapSize
	Section:		Filters
	Range:			1 to INT_MAX
	Purpose:		Allows people to set the approximate size of the Preview bitmap that can
					be included in Camelot Native EPS files and Camelot EPS files. It is
					measured in Millipoints and defaults to 96000 (about 128 pixels or just
					over an inch).

********************************************************************************************/

MILLIPOINT PreviewFilter::PreviewBitmapSize = 96000;

/********************************************************************************************
// Preview Bitmap filter definition
********************************************************************************************/

/********************************************************************************************

>	PreviewFilter::PreviewFilter()

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/01/97
	Purpose:	Constructor for an PreviewFilter object.  The object should be initialised
				to put it in the filter list, but NEVER used for anything.
	SeeAlso:	PreviewFilter::Init

********************************************************************************************/

PreviewFilter::PreviewFilter() : Filter()
{
	ImportMsgID = _R(IDN_IMPORTMSG_TIFF);	// the preview used to be a TIFF ?don't know what to do?
	
	// Set it up so that it can neither Import or Export so that it does not appear in the
	// Filter menus.
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
	FilterID = FILTERID_PREVIEW;

	ExportMsgID = _R(IDS_BUILDINGPREVIEW);
}

/********************************************************************************************

>	BOOL PreviewFilter::Init()

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/01/97
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise a PreviewFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	EPSStack; BMPFilter::Init()

********************************************************************************************/

BOOL PreviewFilter::Init()
{
	// Get an OILFilter object
	pOILFilter = new OILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDN_FILTERNOTPRESENT));

	// All ok
	return TRUE;
}


/********************************************************************************************

>	INT32 PreviewFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, 
								 UINT32 FileSize)

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/01/97
	Inputs:		Are all ignored
				Filename - name of the file.
				HeaderStart - Address of the first few bytes of the file.
				HeaderSize - the number of bytes in the header pointed to by FileStart.
				FileSize - the size of the whole file, in bytes.
	Returns:	0 = not compatible with anything.
	Purpose:	Override pure virtual function so we can have an instance of this object.

********************************************************************************************/

INT32 PreviewFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, 
							 UINT32 FileSize)
{
	//TRACEUSER( "Martin", __FILE__ "(%d) : warning: PreviewFilter::HowCompatible called\n", __LINE__ );
	// Not compatible with anything.
	return 0;
}

/********************************************************************************************

>	BOOL PreviewFilter::DoImport(SelOperation *Op, CCLexFile* pFile, 
						 		Document *DestDoc, BOOL AutoChosen, ImportPosition *Pos,
								KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/01/97
	Inputs:		Are all ignored
				Op - pointer to the operation that this input process is associated with.
				pFile - The file that we should load the bitmap from
				DestDoc - The Document object which should hold the data read in from
						  the bitmap.
				URL - original URL of the imported file
	Returns:	FALSE because we don't do any thing.
	Purpose:	Override pure virtual function so we can have an instance of this object.

********************************************************************************************/

BOOL PreviewFilter::DoImport(SelOperation *Op, CCLexFile* pFile, 
						 		Document *DestDoc, BOOL AutoChosen, ImportPosition *Pos,
								KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)
{
	ERROR3( "PreviewFilter::DoImport called\n");

	return FALSE;
}

/********************************************************************************************

>	BOOL PreviewFilter::DoExport(Operation* pOp, CCLexFile* pFile, PathName* pPath,
								 Document* pDoc, BOOL ShowOptions)

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/01/97
	Inputs:		Are all ignored
				pOp - the operation that started the export off
				pDiskFile - the file to put the exported data into
				pPath - the pathname of the file to be exported to
				pDoc - the document to export
				ShowOptions - Show the export options dialogue?
	Returns:	FALSE because we don't do any thing.
	Purpose:	Override pure virtual function so we can have an instance of this object.

********************************************************************************************/

BOOL PreviewFilter::DoExport(Operation *pOp, CCLexFile* pFile,
							 PathName* pPath, Document* pDoc,
							 BOOL ShowOptions)
{
	ERROR3( "PreviewFilter::DoExport called\n");

	return FALSE;
}


/********************************************************************************************/

/********************************************************************************************

>	PreviewFilterBMP::PreviewFilterBMP()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Purpose:	Constructor for an PreviewFilterBMP object.  The object should be initialised
				before use.
	SeeAlso:	PreviewFilterBMP::Init

********************************************************************************************/

PreviewFilterBMP::PreviewFilterBMP() : BMPFilter()
{
	ImportMsgID = _R(IDT_IMPORTMSG_BMP);
	
	// Set it up so that it can neither Import or Export so that it does not appear in the
	// Filter menus.
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
	FilterID = FILTERID_PREVIEW_BMP;

	ExportRegion = NULL;
	ExportMsgID = _R(IDS_BUILDINGPREVIEW);
	ExportingMsgID = _R(IDS_BUILDINGPREVIEW);
}

/********************************************************************************************

>	BOOL PreviewFilterBMP::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an PreviewFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	BMPFilter::Init()

********************************************************************************************/

BOOL PreviewFilterBMP::Init()
{
	// Get the OILFilter object
	pOILFilter = new BMPOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDT_BMP_FILTERNAME));
	FilterInfo.Load(_R(IDT_BMP_FILTERINFO));

	// All ok
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL PreviewFilterBMP::DoExport(Operation*, CCLexFile*, PathName*, Document*)

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/97

	Purpose:	Do some set up before exporting so the BaseBitmapFilter base class knows
				it is really a preview. The BaseBitmapFilter can then do resizing as 
				necessary.

	Notes:		copied from BaseCamelotFilter::ExportPreviewBitmap which does the set up
				for us usually (why?). We repeat it here so the _BATCHING thumnail
				generation stuff is the right size as well.

********************************************************************************************/

BOOL PreviewFilterBMP::DoExport(Operation* pOp, CCLexFile* pFile, PathName* pPath, 
								Document* TheDocument, BOOL ShowOptions)
{
	INT32 OldPreviewSize = PreviewFilter::PreviewBitmapSize;
	// Set the Preview to be just over an inch accross
	PreviewFilter::PreviewBitmapSize = 96000;

	// set ourselves up - this flags the BaseBitmapFilter to do resizing, which needs to be
	// done there because of Accusoft.
	SetPreviewBitmap(TRUE);

	// Export the Preview to the file by doing what our base class does
	BOOL ok = BMPFilter::DoExport(pOp, pFile, pPath, TheDocument, TRUE);

	// Set ourselves back
	SetPreviewBitmap(FALSE);
	// Set the Preview back to its default setting
	PreviewFilter::PreviewBitmapSize = OldPreviewSize;

	return ok;
}

/********************************************************************************************

>	virtual BOOL PreviewFilterBMP::GetExportOptions(BitmapExportOptions* pOptions)

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/01/97
	Purpose:	Override base class so we don't put up a dialog but still provide fixed 
				options for preview bitmap filters.
	Notes:		See BaseBitmapFilter for interface details

********************************************************************************************/

BOOL PreviewFilterBMP::GetExportOptions(BitmapExportOptions* pOptions)
{
	ERROR2IF(pOptions == NULL, FALSE, "NULL Args")
	ERROR3IF(!pOptions->IS_KIND_OF(BMPExportOptions), "pOptions isn't a BMPExportOptions");

	BMPExportOptions* pBMPOptions = (BMPExportOptions*)pOptions;

	// the depth we ask GDraw to render is always 32-bit, so we can get transparency
	// we have to convert for other formats	when writing the actual bytes to the file
	SetDepthToRender(32);

	// We haven't written the header yet
	WrittenHeader = FALSE;

	/////////////////////////////////////////////////////////////////////////
	// BitmapExportOptions
	//

	// this dpi will be used to scale the preview
	pBMPOptions->SetDPI(96.0);

	// the export depth of the preview is always 8
	pBMPOptions->SetDepth(8);

	// I'm fairly sure this isn't used for anything in WEBSTER-Martin-17/01/97
	// PreviewDither = ?;
	PreviewPalette = PAL_OPTIMISED;

	/////////////////////////////////////////////////////////////////////////
	// BMPExportOptions
	//

	pBMPOptions->SetDither( XARADITHER_NONE );

	// Previews have an optimized palette
	PaletteType = PreviewPalette;
	pBMPOptions->SetPalette( (PALETTE)PreviewPalette );

	return TRUE;
}


/********************************************************************************************/

/********************************************************************************************

>	PreviewFilterGIF::PreviewFilterGIF()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Purpose:	Constructor for an PreviewFilterGIF object.  The object should be initialised
				before use.
	SeeAlso:	PreviewFilterGIF::Init

********************************************************************************************/

PreviewFilterGIF::PreviewFilterGIF() : TI_GIFFilter()
{
	ImportMsgID = _R(IDN_IMPORTMSG_GIF);
	
	// Set it up so that it can neither Import or Export so that it does not appear in the
	// Filter menus.
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
	FilterID = FILTERID_PREVIEW_GIF;

	ExportRegion = NULL;

	ExportMsgID = _R(IDS_BUILDINGPREVIEW);
	ExportingMsgID = _R(IDS_BUILDINGPREVIEW);
}

/********************************************************************************************

>	BOOL PreviewFilterGIF::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an PreviewFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	BMPFilter::Init()

********************************************************************************************/

BOOL PreviewFilterGIF::Init()
{
	// Get the OILFilter object
	pOILFilter = new GIFOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDN_FILTERNAME_GIF));
	FilterInfo.Load(_R(IDN_GIF_FILTERINFO));

	// All ok
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL PreviewFilterGIF::DoExport(Operation*, CCLexFile*, PathName*, Document*)

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/97

	Purpose:	Do some set up before exporting so the BaseBitmapFilter base class knows
				it is really a preview. The BaseBitmapFilter can then do resizing as 
				necessary.

	Notes:		copied from BaseCamelotFilter::ExportPreviewBitmap which does the set up
				for us usually (why?). We repeat it here so the _BATCHING thumnail
				generation stuff is the right size as well.

********************************************************************************************/

BOOL PreviewFilterGIF::DoExport(Operation* pOp, CCLexFile* pFile, PathName* pPath, 
								Document* TheDocument, BOOL ShowOptions)
{
	INT32 OldPreviewSize = PreviewFilter::PreviewBitmapSize;
	// Set the Preview to be just over an inch accross
	PreviewFilter::PreviewBitmapSize = 96000;

	// set ourselves up - this flags the BaseBitmapFilter to do resizing, which needs to be
	// done there because of Accusoft.
	SetPreviewBitmap(TRUE);

	// Export the Preview to the file by doing what our base class does
	BOOL ok = TI_GIFFilter::DoExport(pOp, pFile, pPath, TheDocument, TRUE);

	// Set ourselves back
	SetPreviewBitmap(FALSE);
	// Set the Preview back to its default setting
	PreviewFilter::PreviewBitmapSize = OldPreviewSize;

	return ok;
}

/********************************************************************************************

>	virtual BOOL PreviewFilterGIF::GetExportOptions(BitmapExportOptions* pOptions)

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/01/97
	Purpose:	Override base class so we don't put up a dialog but still provide fixed 
				options for preview bitmap filters.
	Notes:		See BaseBitmapFilter for interface details

********************************************************************************************/

BOOL PreviewFilterGIF::GetExportOptions(BitmapExportOptions* pOptions)
{
	ERROR2IF(pOptions == NULL, FALSE, "NULL Args")
	ERROR3IF(!pOptions->IS_KIND_OF(GIFExportOptions), "pOptions isn't a GIFExportOptions");

	GIFExportOptions* pGIFOptions = (GIFExportOptions*)pOptions;

	// the depth we ask GDraw to render is always 32-bit, so we can get transparency
	// we have to convert for other formats	when writing the actual bytes to the file
	SetDepthToRender(32);

	// We haven't written the header yet
	WrittenHeader = FALSE;

	/////////////////////////////////////////////////////////////////////////
	// BitmapExportOptions
	//

	// this dpi will be used to scale the preview
	pGIFOptions->SetDPI(96.0);

	// the export depth of the preview is always 8
	pGIFOptions->SetDepth(8);

	// I'm fairly sure this isn't used for anything in WEBSTER-Martin-17/01/97
	//PreviewDither = ?;

	// WEBSTER - markn 12/2/97
	// Use the standard palette for the preview
	PreviewPalette = PAL_STANDARD;

	/////////////////////////////////////////////////////////////////////////
	// MaskedFilterExportOptions
	//

	// WEBSTER - markn 12/2/97
	// Use error diffusion
	pGIFOptions->SetDither( XARADITHER_ERROR_DIFFUSION );

	// Previews have an optimized palette
	PaletteType = PreviewPalette;
//	pGIFOptions->SetPalette( (PALETTE)PreviewPalette );

	// Previews have no interlace/transparency
	//pGIFOptions->SetMakeTransparent( FALSE );
	pGIFOptions->SetTransparencyIndex(-1);

	pGIFOptions->SetMakeInterlaced( FALSE );

	// We are a first pass render and not doing the mask, by default
	SecondPass = FALSE;
	DoingMask = FALSE;

	// Default will range from 0 to 4 types which correspond to the combinations of
	// on/off interlaced and on/off transparent
	Compression = 0;				// Previews have no interlace/transparency

	// Determine the filter type currently in use in Accusoft format
	FilterType = TI_GIF;
	pGIFOptions->SetFilterType(FilterType);

	m_DoingAnimation = FALSE;

	return TRUE;
}

/********************************************************************************************/

/********************************************************************************************

>	PreviewFilterJPEG::PreviewFilterJPEG()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Purpose:	Constructor for an PreviewFilterJPEG object.  The object should be initialised
				before use.
	SeeAlso:	PreviewFilterJPEG::Init

********************************************************************************************/

PreviewFilterJPEG::PreviewFilterJPEG() : JPEGExportFilter()
{
	ImportMsgID = _R(IDN_IMPORTMSG_JPEG);
	
	// Set it up so that it can neither Import or Export so that it does not appear in the
	// Filter menus.
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
	FilterID = FILTERID_PREVIEW_JPEG;

	ExportRegion = NULL;
	ExportMsgID = _R(IDS_BUILDINGPREVIEW);
	ExportingMsgID = _R(IDS_BUILDINGPREVIEW);
}

/********************************************************************************************

>	BOOL PreviewFilterJPEG::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an PreviewFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	BMPFilter::Init()

********************************************************************************************/

BOOL PreviewFilterJPEG::Init()
{
	// Get the OILFilter object
	pOILFilter = new JPEGOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDN_JPEG_FILTERNAME));
	FilterInfo.Load(_R(IDN_JPEG_FILTERINFO));

	// All ok
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL PreviewFilterJPEG::DoExport(Operation*, CCLexFile*, PathName*, Document*)

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/97

	Purpose:	Do some set up before exporting so the BaseBitmapFilter base class knows
				it is really a preview. The BaseBitmapFilter can then do resizing as 
				necessary.

	Notes:		copied from BaseCamelotFilter::ExportPreviewBitmap which does the set up
				for us usually (why?). We repeat it here so the _BATCHING thumnail
				generation stuff is the right size as well.

********************************************************************************************/

BOOL PreviewFilterJPEG::DoExport(Operation* pOp, CCLexFile* pFile, PathName* pPath, 
								Document* TheDocument, BOOL ShowOptions)
{
	INT32 OldPreviewSize = PreviewFilter::PreviewBitmapSize;
	// Set the Preview to be just over an inch accross
	PreviewFilter::PreviewBitmapSize = 96000;

	// set ourselves up - this flags the BaseBitmapFilter to do resizing, which needs to be
	// done there because of Accusoft.
	SetPreviewBitmap(TRUE);

	// Export the Preview to the file by doing what our base class does
	BOOL ok = JPEGExportFilter::DoExport(pOp, pFile, pPath, TheDocument, TRUE);

	// Set ourselves back
	SetPreviewBitmap(FALSE);
	// Set the Preview back to its default setting
	PreviewFilter::PreviewBitmapSize = OldPreviewSize;

	return ok;
}

/********************************************************************************************

>	virtual BOOL PreviewFilterJPEG::GetExportOptions(BitmapExportOptions* pOptions)

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/01/97
	Purpose:	Override base class so we don't put up a dialog but still provide fixed 
				options for preview bitmap filters.
	Notes:		See BaseBitmapFilter for interface details

********************************************************************************************/

BOOL PreviewFilterJPEG::GetExportOptions(BitmapExportOptions* pOptions)
{
	ERROR2IF(pOptions == NULL, FALSE, "NULL Args")
	ERROR3IF(!pOptions->IS_KIND_OF(JPEGExportOptions), "pOptions isn't a JPEGExportOptions");

	JPEGExportOptions* pJPEGOptions = (JPEGExportOptions*)pOptions;

	// the depth we ask GDraw to render is always 32-bit, so we can get transparency
	// we have to convert for other formats	when writing the actual bytes to the file
	SetDepthToRender(32);

	// We haven't written the header yet
	WrittenHeader = FALSE;

	/////////////////////////////////////////////////////////////////////////
	// BitmapExportOptions
	//

	// this dpi will be used to scale the preview
	pJPEGOptions->SetDPI(96.0);

	// the export depth of the preview is always 8
	pJPEGOptions->SetDepth(8);

	// I'm fairly sure this isn't used for anything in WEBSTER-Martin-17/01/97
	//PreviewDither = ?;
	PreviewPalette = PAL_OPTIMISED;

	/////////////////////////////////////////////////////////////////////////
	// JPEGExportOptions
	//

	// Previews have an optimized palette
	PaletteType = PreviewPalette;

	return TRUE;
}


/********************************************************************************************/

/********************************************************************************************

>	PreviewFilterPNG::PreviewFilterPNG()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Purpose:	Constructor for an PreviewFilterPNG object.  The object should be initialised
				before use.
	SeeAlso:	PreviewFilterPNG::Init

********************************************************************************************/

PreviewFilterPNG::PreviewFilterPNG() : PNGFilter()
{
	ImportMsgID = _R(IDS_IMPORTMSG_PNG);
	
	// Set it up so that it can neither Import or Export so that it does not appear in the
	// Filter menus.
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
	FilterID = FILTERID_PREVIEW_PNG;

	ExportRegion = NULL;
	ExportMsgID = _R(IDS_BUILDINGPREVIEW);
	ExportingMsgID = _R(IDS_BUILDINGPREVIEW);
}

/********************************************************************************************

>	BOOL PreviewFilterPNG::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an PreviewFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	BMPFilter::Init()

********************************************************************************************/

BOOL PreviewFilterPNG::Init()
{
	// Get the OILFilter object
	pOILFilter = new PNGOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDS_PNG_FILTERNAME));
	FilterInfo.Load(_R(IDS_PNG_FILTERINFO));

	// All ok
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL PreviewFilterPNG::DoExport(Operation*, CCLexFile*, PathName*, Document*)

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/97

	Purpose:	Do some set up before exporting so the BaseBitmapFilter base class knows
				it is really a preview. The BaseBitmapFilter can then do resizing as 
				necessary.

	Notes:		copied from BaseCamelotFilter::ExportPreviewBitmap which does the set up
				for us usually (why?). We repeat it here so the _BATCHING thumnail
				generation stuff is the right size as well.

********************************************************************************************/

BOOL PreviewFilterPNG::DoExport(Operation* pOp, CCLexFile* pFile, PathName* pPath, 
								Document* TheDocument, BOOL ShowOptions)
{
	INT32 OldPreviewSize = PreviewFilter::PreviewBitmapSize;
	// Set the Preview to be just over an inch accross
	PreviewFilter::PreviewBitmapSize = 96000;

	// set ourselves up - this flags the BaseBitmapFilter to do resizing, which needs to be
	// done there because of Accusoft.
	SetPreviewBitmap(TRUE);

	// Export the Preview to the file by doing what our base class does
	BOOL ok = PNGFilter::DoExport(pOp, pFile, pPath, TheDocument, TRUE);

	// Set ourselves back
	SetPreviewBitmap(FALSE);
	// Set the Preview back to its default setting
	PreviewFilter::PreviewBitmapSize = OldPreviewSize;

	return ok;
}

/********************************************************************************************

>	virtual BOOL PreviewFilterPNG::GetExportOptions(BitmapExportOptions* pOptions)

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/01/97
	Purpose:	Override base class so we don't put up a dialog but still provide fixed 
				options for preview bitmap filters.
	Notes:		See BaseBitmapFilter for interface details

********************************************************************************************/

BOOL PreviewFilterPNG::GetExportOptions(BitmapExportOptions* pOptions)
{
	ERROR2IF(pOptions == NULL, FALSE, "NULL Args")
	ERROR3IF(!pOptions->IS_KIND_OF(PNGExportOptions), "pOptions isn't a PNGExportOptions");

	PNGExportOptions* pPNGOptions = (PNGExportOptions*)pOptions;

	// the depth we ask GDraw to render is always 32-bit, so we can get transparency
	// we have to convert for other formats	when writing the actual bytes to the file
	SetDepthToRender(32);

	// We haven't written the header yet
	WrittenHeader = FALSE;

	/////////////////////////////////////////////////////////////////////////
	// BitmapExportOptions
	//

	// this dpi will be used to scale the preview
	pPNGOptions->SetDPI(96.0);

	// the export depth of the preview is always 8
	pPNGOptions->SetDepth(8);

	// I'm fairly sure this isn't used for anything in WEBSTER-Martin-17/01/97
	//PreviewDither = ?;
	PreviewPalette = PAL_OPTIMISED;

	/////////////////////////////////////////////////////////////////////////
	// MaskedFilterExportOptions
	//

	pPNGOptions->SetDither( XARADITHER_NONE );

	// Previews have an optimized palette
	PaletteType = PreviewPalette;
//	pPNGOptions->SetPalette( (PALETTE)PreviewPalette );

	// Previews have no interlace/transparency
//	pPNGOptions->SetMakeTransparent( FALSE );
	pPNGOptions->SetTransparencyIndex(-1);
	pPNGOptions->SetMakeInterlaced( FALSE );

	// We are a first pass render and not doing the mask, by default
	SecondPass = FALSE;
	DoingMask = FALSE;

	// Default will range from 0 to 4 types which correspond to the combinations of
	// on/off interlaced and on/off transparent
	Compression = 0;				// Previews have no interlace/transparency

	// Determine the filter type currently in use in Accusoft format
	FilterType = PNG;
	pPNGOptions->SetFilterType(FilterType);

	return TRUE;
}



/********************************************************************************************/

/********************************************************************************************

>	BOOL ThumbnailFilterPNG::DoExport(Operation* pOp, CCLexFile* pFile, PathName* pPath, 
								Document* TheDocument)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/97
	Purpose:	Exports a document into a png file. Creates and attaches a DocView to the 
				document before calling the DoExport function of the PNG filter, because it 
				depends on a view being available. Unlike the preview filters no 
				PreviewBitmapSize value is set in this function, so the caller should set it 
				if something different then the default size is required
	SeeAlso:	BaseBitmapFilter::DoExport

********************************************************************************************/

BOOL ThumbnailFilterPNG::DoExport(Operation* pOp, CCLexFile* pFile, PathName* pPath, 
								Document* TheDocument, BOOL ShowOptions)
{
	// set ourselves up - this flags the BaseBitmapFilter to do resizing, which needs to be
	// done there because of Accusoft.
	SetPreviewBitmap(TRUE);

	// create a new view for the document
	DocView *pView = new DocView(TheDocument);

	if (pView && pView->Init())
	{
		// set the view as current, but remember the last current one
		View *pOldView = View::GetCurrent();
		pView->SetCurrent();

		// Export the thumbnail to the file by doing what our base class does
		BOOL ok = PNGFilter::DoExport(pOp, pFile, pPath, TheDocument, TRUE);

		// restore the last view
		if (pOldView != NULL)
			pOldView->SetCurrent();
		else
			View::SetNoCurrent();

		// delete our view
		delete pView;

		return ok;
	}
	else
		return FALSE; // view creation failed
}
