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

// Base class for bitmap filters (currently meaningful for export only)


#include "camtypes.h"
#include "bitfilt.h"

//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h"	// _R(IDS_OUT_OF_MEMORY)
//#include "camvw.h"								// sadly
#include "layer.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "page.h"
//#include "tim.h"
#include "grndbmp.h"
#include "osrndrgn.h"
#include "nodebmp.h"
#include "bitmpinf.h"
#include "progress.h"
//#include "nev.h"		// _R(IDN_USER_CANCELLED)
//#include "bmpres.h"		// general bitmap filter based resources
#include "prvwflt.h"
#ifndef WEBSTER
//#include "extfilts.h"	// Accusoft filters
#endif //WEBSTER
//#include "selop.h"
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bmpexdoc.h"
#include "impexpop.h"
#include "zoomops.h"
//#include "will3.h"		// for _R(IDS_GENOPTPALMSGID)
#include "nodetxtl.h"
#include "qualattr.h"
#include "ccbuffil.h"
//#include "palman.h"
#include "bmpprefs.h"
#include "exjpeg.h"
//#include "giffiltr.h"
#include "grndbmp.h"

#include "bmapprev.h"

//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "filtimag.h"	//The imagemap filter class
#include "filtrmgr.h"	//The Filter Manager - used to find the imagemap filter
//#include "resimmap.h"	//Imagemap resources
//#include "clipint.h"	//To put text on the clipboard

#include "htmlfltr.h"	//For HTMLFilter::ImportingHTML

#include "sprdmsg.h"	// SpreadMsg
#include "layermsg.h"	// UPDATE_ACTIVE_LAYER

#include "fileutil.h"
//#include "prevwres.h"	// _R(IDS_DEFAULTFRAMENAME)
//#include "will2.h"		// _R(IDS_K_EPSFILTER_IMPORTED)
//#include "phil.h"
//#include "simon.h"

//#include "opbevel.h"	// for bevel tools
//#include "registry.h"
//#include "sliceres.h"

#include "gpalopt.h"
#include "colplate.h"
#include "fillattr2.h"
#include "nodetext.h"

CC_IMPLEMENT_MEMDUMP(BitmapImportOptions, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_DYNAMIC(BaseBitmapFilter, BitmapFilter)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

GRenderBitmap*	BaseBitmapFilter::ExportRegion 	= NULL;	// The region used for export
CCLexFile*		BaseBitmapFilter::OutputFile 		= NULL;	// The file used for output
BOOL 			BaseBitmapFilter::WrittenHeader		= FALSE;// Have we done the first strip yet
	
UINT32 			BaseBitmapFilter::SizeOfExport		= 100;	// defined progress bar size.
UINT32 			BaseBitmapFilter::OurNumNodes		= 0;	// our copy of the number of nodes being exported

LPLOGPALETTE 	BaseBitmapFilter::pOptimisedPalette 		= NULL;

BOOL 			BaseBitmapFilter::WeAreGeneratingOptPalette = FALSE;

const INT32		BaseBitmapFilter::MinPelsPerMeter	= 1000;		// Minimum Pels per meter allowed

const INT32		BaseBitmapFilter::MinExportSize = 10;			// Minimum selection width/height allowed

KernelBitmap*	BaseBitmapFilter::pExportBitmap = NULL;			// Store for the bitmap pointr in DoExportBitmap

DPI				BaseBitmapFilter::m_DefaultExportDPI = 96.0;	// Used as the default DPI when exporting

LPLOGPALETTE 	BaseBitmapFilter::m_pBrowserPalette 		= NULL;

BOOL			BaseBitmapFilter::s_fZoomOnImport		= 9999;
BOOL			BaseBitmapFilter::s_fWarnedZoomOnImport	= 9999;

//  This constant gives you the ( square of the ) maximum distance that a colour has to be from a browser colour
//  to be snapped to that colour.
const INT32 WEB_SNAP_OPTIMISED_DISTANCE_SQUARED = 25;

void PixelAlignedFiddle(DocRect*);

// Added by Craig Hamilton 29/8/00.
INT32 BaseBitmapFilter::m_NumberOfPaletteEntries = 0;
// End added.

/********************************************************************************************

>	BaseBitmapFilter::BaseBitmapFilter()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Purpose:	Constructor for an BaseBitmapFilter object.  The object should be initialised
				before use. Base class version
	SeeAlso:	BaseBitmapFilter::Init

********************************************************************************************/

BaseBitmapFilter::BaseBitmapFilter()
{
	OutputFile = NULL;
	ExportRegion = NULL;
	pNewBitmap = NULL;
	InputFile = NULL;
	BackgroundRedrawState = FALSE;
	BackgroundRedrawStateSet = FALSE;
	ViewQualitySet = FALSE;
	IsPreviewBitmap = FALSE;
	pExportBitmap = NULL;
	pSpread = NULL;
	m_pImportOptions	= NULL;
	m_pExportOptions	= NULL;
	pOptimisedPalette = NULL;

	WrittenHeader		= FALSE;// Have we done the first strip yet
	RenderInStrips	= TRUE;	// Should we render in strips or not
	RenderBottomToTop = TRUE;	// control over which way we render
	BadExportRender	= FALSE;// whether the export rendering has gone ok or not
	
	SizeOfExport		= 100;	// defined progress bar size.
	OurNumNodes		= 0;	// our copy of the number of nodes being exported

	PaletteType		= PAL_OPTIMISED;

	//Should these lines be in or out - Martin 14/04/97
	PreviewDither	= XARADITHER_ERROR_DIFFUSION;
	PreviewPalette	= PAL_OPTIMISED;
			   
	WeAreGeneratingOptPalette = FALSE;
	ClipRect.MakeEmpty();

	m_pOptimisedPalette2 = NULL;
	m_pExportOptions = NULL;
}



/********************************************************************************************

>	BaseBitmapFilter::~BaseBitmapFilter()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Purpose:	Destructor for an BaseBitmapFilter object.

********************************************************************************************/

BaseBitmapFilter::~BaseBitmapFilter()
{
	//  Clear up some dynamically allocated memory
	if (m_pOptimisedPalette2)
	{
		CCFree(m_pOptimisedPalette2);
		m_pOptimisedPalette2 = 0;
	}

	if (m_pBrowserPalette)
	{
		CCFree(m_pBrowserPalette);
		m_pBrowserPalette = 0;
	}

	delete m_pImportOptions;
	m_pImportOptions = 0;

//	delete m_pExportOptions;
	m_pExportOptions = 0;
}



/********************************************************************************************

>	BOOL BaseBitmapFilter::InitBaseClass()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/97
	Returns:	TRUE if the filter was initialized ok
				FALSE otherwise
	Purpose:	Initialises any static member variables for the base bitmap filter
				class

********************************************************************************************/
BOOL BaseBitmapFilter::InitBaseClass()
{
	if( 9999 == s_fZoomOnImport &&
		Camelot.DeclareSection( _T("Filters"), 10 ) )
	{
		s_fZoomOnImport = TRUE;
		s_fWarnedZoomOnImport = FALSE;

		Camelot.DeclarePref( NULL, _T("ZoomToFitOnImport"), &s_fZoomOnImport, FALSE, TRUE );
		Camelot.DeclarePref( NULL, _T("WarnedZoomToFitOnImport"), &s_fWarnedZoomOnImport, FALSE, TRUE );
	}
	
	return BitmapExportOptions::Declare();
}


/********************************************************************************************

>	void BaseBitmapFilter::SetPreviewBitmap(BOOL IsPreviewBmp)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		isPreviewBmp - TRUE if this bitmap we are saving is actually a Preview bitmap
				FALSE if not.
	Purpose:	Allows you to set the state of this flag. Always set it back to FALSE after
				the export as the filter hangs around, ready for the next export.

********************************************************************************************/

void BaseBitmapFilter::SetPreviewBitmap(BOOL IsPreviewBmp)
{
	IsPreviewBitmap = IsPreviewBmp;
}


/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::ExportVisibleLayersOnly()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/02/95
	Inputs:		-
	Outputs:	-
	Returns:	True if this filter wants to exclude invisible layers and so export only
				visible items Or False if want to export everything.
	Purpose:	Determines if the filter wants to export only visible layers.
				In the base bitmap filters class version the default action will be to
				export only those layers which are visible.
	SeeAlso:	Filter::ExportRender;

********************************************************************************************/

BOOL BaseBitmapFilter::ExportVisibleLayersOnly()
{
	// only include all layer that are visible in this bitmap export
	return TRUE;
}	


/********************************************************************************************

>	BOOL BaseBitmapFilter::IsSecondPassRequired()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/95
	Returns:	True if this filter wants a second export rendering pass and so have a chance
				to setup semething like a mask. False if no second pass required.
	Purpose:	Determines if the a second rendering pass is required or not.
	SeeAlso:	DoExport;

********************************************************************************************/

BOOL BaseBitmapFilter::IsSecondPassRequired()
{
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		pcszPathName	pointer to the pathname to check
	Returns:	TRUE if the filter requires a default document, FALSE if not.
	Purpose:	Works out if opening a file of this type requires a default document to be
				loaded. If the file format supplies the document then return FALSE otherwise
				return TRUE. An example would be opening a bitmap file. This has no document
				defined in the file format and so we need to laod the default document before
				importing the bitmap into this file.
				In this baseclass version return FALSE and hence assume that the filters that
				need to will override this function to return TRUE.
	SeeAlso:	Filter; Filter::IsDefaultDocRequired; CCamDoc::OnOpenDocument;
	SeeAlso:	FilterFamily::DoImport; Filter::DoImport;

********************************************************************************************/

BOOL BaseBitmapFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)
{
	// No need to check the pathname, just return TRUE as all currently known bitmap filters
	// will require the default document.
	return TRUE;
}


/********************************************************************************************

>	virtual void BaseCamelotFilter::IncProgressBarCount(UINT32 n)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (Markn)
	Created:	5/6/96
	Inputs:		n = amount to inc the progress bar count by
	Returns:	-
	Purpose:	Func to incrementing the progress bar count.
				If there is a progress bar available, it is updated by this call.
	Scope: 		Protected.

********************************************************************************************/
BOOL BaseBitmapFilter::IncProgressBarCount(UINT32 n)
{
	return TRUE;
}


/********************************************************************************************

>	virtual void BaseBitmapFilter::SetTotalProgressBarCount(UINT32 n)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (Markn)
	Created:	5/6/96
	Inputs:		n = amount to set the total progress bar count to
	Purpose:	Func to set total the progress bar count.
				If there is a progress bar available, it is updated by this call.
	Scope: 		Protected.

********************************************************************************************/
void BaseBitmapFilter::SetTotalProgressBarCount(UINT32 n)
{
	ERROR3IF(n == 0,"Should only be set to a value greater that 0");

	m_TotalProgressBarCount = n;
}

/********************************************************************************************

>	virtual BitmapSource* BaseBitmapFilter::CreateBitmapSource(FilePos Size) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Inputs:		Size: The required size of the BitmapSource
	Returns:	A pointer to a new BitmapSource
	Purpose:	The PrepareToImport() function calls this member to create a BitmapSource
				for lossy file formats.
				The BaseBitmapFilter implementation returns NULL.
				Derived classes with lossy formats should override this function to provide
				a subclassed BitmapSource.
	SeeAlso:	IsFormatLossy()
	Scope: 		protected

********************************************************************************************/
BitmapSource* BaseBitmapFilter::CreateBitmapSource(OFFSET Size) const
{
	return NULL;
}


/********************************************************************************************

>	BOOL BaseBitmapFilter::PrepareBitmapSource()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Returns:	TRUE if successful
				FALSE otherwise
	Purpose:	The PrepareToImport() function calls this member to create a BitmapSource
				for lossy file formats, and a CCLexFile to work with it.
	SeeAlso:	IsFormatLossy()
	Scope: 		protected

********************************************************************************************/
BOOL BaseBitmapFilter::PrepareBitmapSource()
{
#ifdef DO_EXPORT
	OFFSET offset = GetDataStartOffset();
	OFFSET size = InputFile->Size() - offset;

	// Create the actual BitmapSource object
	m_pBitmapSource = CreateBitmapSource(size);
	if (m_pBitmapSource == NULL || !m_pBitmapSource->IsOK())
	{
		Error::SetError(_R(IDS_OUT_OF_MEMORY));
		return FALSE;
	}

	// Check for errors but let CCFile report them
	InputFile->SetThrowExceptions(FALSE);
	InputFile->SetReportErrors(TRUE);

	// Ignore those superfluous bits at the start of the file
	InputFile->seekIn(offset);
	if (!InputFile->good())
	{
		return FALSE;
	}

	// Now create a file to fill our BitmapSource & provide the filter with something
	// to work with
	CCBufferFile* pBufferFile = new CCBufferFile(InputFile);
	if (pBufferFile == NULL || !pBufferFile->IsInited())
	{
		Error::SetError(_R(IDS_OUT_OF_MEMORY));
		return FALSE;
	}

	// We have a file, so attach out BitmapSource to it
	m_pBitmapSource->AttachToBufferFile(pBufferFile);

	// Now ditch our current InputFile in favour of the BitmapSource's one
	// We don't need InputFile again
	InputFile = (CCLexFile*)pBufferFile;
#endif

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::PrepareToImport()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Returns:	TRUE if succeeded, FALSE if not.
	Purpose:	Prepare to import bitmap data using this filter.  This sets up the filter
				to a sensible state for reading.
	Errors:		Out of memory.
	SeeAlso:	BaseBitmapFilter::DoImport; BaseBitmapFilter::CleanUpAfterImport
	Scope: 		Private

********************************************************************************************/

BOOL BaseBitmapFilter::PrepareToImport()
{
	ERROR2IF(InputFile == NULL, FALSE, "InputFile is NULL");

	// Assume we won't want a BitmapSource
	m_pBitmapSource = NULL;

#ifdef DO_EXPORT
	// Do we need to keep the data to prevent it from deteriorating?
	if (IsFormatLossy())
	{
		// SetTotalProgressBarCount(InputFile->Size()); ...one day
		if (!PrepareBitmapSource())
		{
			return FALSE;
		}
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	void BaseBitmapFilter::CleanUpAfterImport()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Purpose:	Cleans up the memory allocated by BaseBitmapFilter::PrepareToImport() - used
				when the import process ends, either normally or abnormally.
	SeeAlso:	BaseBitmapFilter::PrepareToImport; BaseBitmapFilter::DoImport
	Scope: 		Private

********************************************************************************************/

void BaseBitmapFilter::CleanUpAfterImport()
{
	TheDocument = NULL;
	
	//WEBSTER-Martin-02/01/97
#ifndef WEBSTER
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Reset the filter type used by the Accusoft filters so that it can cache the
	// filter type correctly and so only call the Accusoft DLL on the first filter
	// rather than on all filters. 

	AccusoftFilters::ResetFilterType();

#endif
#endif //WEBSTER
}

/********************************************************************************************

>	BitmapImportOptions* BaseBitmapFilter::GetImportOptions()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		None
	Outputs:	None
	Returns:	A pointer to BitmapImportOptions with the options in it set.
				NULL if an error occured.
	Purpose:	To allow a filter to offer the user a choice over what gets imported.
				Derived BaseBitmapFilter classes should also derive a new BitmapImportOptions
				class and return a new copy on the heap with the user options in it.
				BaseBitmapFilter will delete the BitmapImportOptions class when no longer needed.
				At present used by PhotoCD filter to get image to import.
	SeeAlso:	BaseBitmapFilter::PrepareToImport; BaseBitmapFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BitmapImportOptions* BaseBitmapFilter::GetImportOptions()
{
	BitmapImportOptions* pOptions = new BitmapImportOptions;
	return pOptions;
}


/********************************************************************************************

 >	virtual BOOL BaseBitmapFilter::IsFormatLossy() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96

	Returns:	TRUE if the file format for the derived filter is lossy
			FALSE otherwise

	Purpose:	Provides information to the BaseBitmapFilter so that it knows whether
				or not to provide a BitmapSource to prevent progressive degradation of
				the bitmap when exported via the new file format.
	Notes:		Derived classes should override this function for their particular
				implementation.

********************************************************************************************/
BOOL BaseBitmapFilter::IsFormatLossy() const
{
	return FALSE;
}


/********************************************************************************************

 >	virtual OFFSET BaseBitmapFilter::GetDataStartOffset() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96

	Returns:	The offset from the HeaderStart parameter given in HowCompatible(), 
				from which data essential to the correct functionality of this filter 
				starts.

	Purpose:	Provides information to the BaseBitmapFilter, so that it knows where
				to start any required BitmapSource. It will only be useful to derived 
				classes that deal with a lossy file format.
	Notes:		The default implementation returns zero, assuming there is no useless 
				garbage before any actual header. Derived classes should override this
				for any special case.

	See Also:	IsFormatLossy()

********************************************************************************************/
OFFSET BaseBitmapFilter::GetDataStartOffset() const
{
	return 0;
}


/********************************************************************************************

>	BOOL BaseBitmapFilter::DoImport(SelOperation *Op, CCLexFile* pFile, 
						 			Document *DestDoc, BOOL AutoChosen, ImportPosition *Pos,
									KernelBitmap** ppImportedBitmap,
									DocCoord* pPosTranslate, String_256* URL)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		Op - pointer to the operation that this input process is associated with.
				pFile - The file that we should load the bitmap from
				DestDoc - The Document object which should hold the data read in from
						  the bitmap.
				ImportPosition -
				ppImportedBitmap - Used to set the bitmap background
				pPosTranslate - specify the position of the bitmap on the screen
	Returns:	TRUE if the input operation worked ok, FALSE if not.
	Purpose:	Read bitmap data from a file. This is the same for all bitmap filters; 
				the ImportBitmap() function should be over-ridden for actual bitmap
				import filters.
	Errors:		Fails (returns FALSE) if the document structure is incorrect, or if there
				is a problem with the bitmap.

********************************************************************************************/

BOOL BaseBitmapFilter::DoImport(SelOperation *Op, CCLexFile* pFile, 
						 		Document *DestDoc, BOOL AutoChosen, ImportPosition *Pos,
								KernelBitmap** ppImportedBitmap,
								DocCoord* pPosTranslate, String_256* URL)
{
	ERROR2IF(Op == NULL, FALSE, 		"BaseBitmapFilter::DoImport NULL operation supplied");
	ERROR2IF(pFile == NULL, FALSE, 	"BaseBitmapFilter::DoImport NULL diskfile supplied");
	ERROR2IF(DestDoc == NULL, FALSE, 	"BaseBitmapFilter::DoImport NULL doc supplied");

	// Make a note of the disk file we are given
	InputFile = pFile;

	// Set up the class variable which is used by the bitmap system to determine which document
	// to load the bitmap into. Added 24/9/96 as opening a PhotoCD causes the selection to change!
	// Removing the photocd import dialog box causes the selection to be put into an existing doc
	// rather than a new one.
	TheDocument = DestDoc;

	if (ProcessImportOptions() == FALSE)
		return FALSE;

	Spread *pSpread = NULL;
	DocCoord Origin;

	//So first find the current view
	DocView* pDocView=DocView::GetCurrent();
		
	if (Pos == NULL)
	{
		// For now, position Draw objects on 1st page of spread 1

		//Graham 30/6/97: We now position imported bitmaps in the top left of the view

		ERROR2IF(pDocView==NULL, FALSE, "BaseBitmapFilter::DoImport - no current view");

		//And get the centre coordinate
		Origin=pDocView->GetTopLeftCoord();

		// Find the first spread in that document.
		pSpread = TheDocument->FindFirstSpread ();
	}
	else
	{
		pSpread = Pos->pSpread;
		Origin = Pos->Position;
	}

	// If we use the PhotoCD import then current/selected doc/spread MAY be wrong. So force it again.
	// Must ensure the selected spread is in the destination document as otherwise we will insert the
	// object AFTER the insertion node and hence in an illegal position. 
	Document::SetSelectedViewAndSpread(DestDoc, DocView::GetCurrent(), pSpread);

	// Let's get ready
	if (!PrepareToImport())
		return FALSE;
	
	UINT32 nBitmapToRead = 0;
	Layer* pNewLayer = NULL;
	Layer* pNewActiveLayer = NULL;
	do	// for every bitmap in the file
	{
		KernelBitmap* pKernelBmp = NULL;
		++nBitmapToRead;

		// Import that bitmap, if first bitmap then nBitmapToRead will be set to 1
		BOOL Success = ImportBitmap(pFile, &pKernelBmp, nBitmapToRead);
		if (!Success)
		{
			if (m_pImportOptions != NULL)
			{
				delete m_pImportOptions;
				m_pImportOptions = NULL;
			}

			// Did not work - report this to the caller.
			return FALSE;
		}

		// If the caller wants to handle the imported bitmap, then set the return pointer up,
		// otherwise, we handle the bitmap by inserting a NodeBitmap into the document
		if (ppImportedBitmap != NULL && pPosTranslate == NULL)
			*ppImportedBitmap = pKernelBmp;
		else
		{
			// Get a new NodeBitmap object to import into.
			NodeBitmap *pNodeBitmap = new NodeBitmap;
			if ((pNodeBitmap == NULL) || (!pNodeBitmap->SetUpPath(12,12)))
			{
				if (m_pImportOptions != NULL)
				{
					delete m_pImportOptions;
					m_pImportOptions = NULL;
				}
				if (pNodeBitmap != NULL)
					delete pNodeBitmap;

				return FALSE;
			}

			pNodeBitmap->GetBitmapRef()->Attach(pKernelBmp, DestDoc); //GetDocument());

			if (pNodeBitmap->GetBitmap() != pKernelBmp)
			{
				// It didn't use the bitmap we gave it, so we can delete it
				delete pKernelBmp;
			}

			//And if the caller wants a pointer to the kernel bitmap, give them one...
			if (ppImportedBitmap)
			{
				*ppImportedBitmap=pNodeBitmap->GetBitmap();
			}

			// Import worked - try to add the bitmap object into the tree.
			// First, set the rectangle to the right size for the bitmap...
			BitmapInfo Info;
			pNodeBitmap->GetBitmap()->ActualBitmap->GetInfo(&Info);

			const INT32 HPixelSize = ( pDocView->GetPixelWidth() + 0.5 ).MakeLong();	// Size of output pixel in millipoints
			const INT32 VPixelSize = ( pDocView->GetPixelHeight() + 0.5 ).MakeLong();// Size of output pixel in millipoints

			DocRect BoundsRect;
			BoundsRect.lo = Origin;
			// Make sure that this is snapped to a pixel grid
			BoundsRect.lo.x = GridLock(BoundsRect.lo.x, HPixelSize);
			BoundsRect.lo.y = GridLock(BoundsRect.lo.y, VPixelSize);
			// And now add in the rest of the bounds
			BoundsRect.hi.x = BoundsRect.lo.x + Info.RecommendedWidth;
			BoundsRect.hi.y = BoundsRect.lo.y + Info.RecommendedHeight;
			BoundsRect.hi.x = GridLock(BoundsRect.hi.x, HPixelSize);
			BoundsRect.hi.y = GridLock(BoundsRect.hi.y, VPixelSize);

			// And set this in our bitmap node
			pNodeBitmap->CreateShape(BoundsRect);
			
			// Make sure that there is a layer to put the bitmap onto
			if (!MakeSureLayerExists(DestDoc))
			{
				// There is no layer and one could not be made, so we will have to fail
				delete pNodeBitmap;
				if (m_pImportOptions != NULL)
				{
					delete m_pImportOptions;
					m_pImportOptions = NULL;
				}
				return FALSE;
			}

			// Set the default attrs
			// This MUST be done before the NodeBitmap is Inserted into the tree
			if (!pNodeBitmap->ApplyDefaultBitmapAttrs(Op))
			{
				if (m_pImportOptions != NULL)
				{
					delete m_pImportOptions;
					m_pImportOptions = NULL;
				}
				return FALSE;
			}

			// Insert the node, but don't invalidate its region
			// If we are the only bitmap then just add it as the first child of the active layer
			// Could use Filter::ImportWithLayers to determine if we use layers or not
			BOOL Fail = FALSE;
			BOOL SingleBitmap = !AreBitmapsToRead() && nBitmapToRead <= 1;

			//Graham 30/5/97
			//If we're importing an animated bitmap as part of an 
			//HTML document
PORTNOTE("filters","Removed HTMLFilter usage")
			if( !SingleBitmap
#ifndef EXCLUDE_FROM_XARALX
				&& HTMLFilter::ImportingHTML
#endif
				)
			{
				//Then turn the "all layers visible" flag on in the document
				DestDoc->SetMultilayer(TRUE);
				DestDoc->SetAllVisible(TRUE);
			}

			if (SingleBitmap || !Filter::ImportBitmapsOntoLayers)
			{
				// This inserts the node as the last child of the active layer
				if (!Op->DoInsertNewNode(pNodeBitmap, pSpread, FALSE))
				{
					Fail = TRUE;
					// It didn't work - delete the sub-tree we just created, and report error.
					pNodeBitmap->CascadeDelete();
					// Of course, Cascade delete does not actually delete the node itself, so now do that
					delete pNodeBitmap;
					pNodeBitmap = NULL;
				}
			}
			else
			{
				// We are not the only bitmap in the loading process so for each bitmap try and create
				// a new layer and put the new bitmap there.
				// The first layer should be added after the active layer.
				// We could do this but this means that when multiple animations are being loaded you
				// end up with the first frames close together rather than the two animations being
				// separate.
				if (!InsertBitmapOnNewLayer(Op, pSpread, pNodeBitmap, &pNewLayer))
				{
					Fail = TRUE;
					// It didn't work - delete the sub-tree we just created, and report error.
					pNodeBitmap->CascadeDelete();
					// Of course, Cascade delete does not actually delete the node itself, so now do that
					delete pNodeBitmap;
					pNodeBitmap = NULL;
				}
				// Remember the layer we used for the first bitmap so that we can make it active
				if (nBitmapToRead == 1)
					pNewActiveLayer = pNewLayer;

				// Let the filter set things up from the bitmap on the layer
				// GIF filter can use disposal method to set overlay and solid flags
				KernelBitmap * pBitmap = pNodeBitmap->GetBitmap();
				SetFlagsFromBitmap(pNewLayer, pBitmap, nBitmapToRead);
			}
			// If we noted a failure then handle it now
			if (Fail)
			{
				if (m_pImportOptions != NULL)
				{
					delete m_pImportOptions;
					m_pImportOptions = NULL;
				}
				return FALSE;
			}

			// and move it to the correct location if we need to...
			DocCoord Offset(0,0);
			if (GetDragAndDropTranslation(Pos, BoundsRect, &Offset))
			{
				// See if the bitmap contained an offset and if it did add it in
				// Baseclass defaults to 0,0 offset
				AddOffsetFromBitmap(&Offset);
				
				Offset.x = GridLock(Offset.x, HPixelSize);
				Offset.y = GridLock(Offset.y, VPixelSize);

				// Build the matrix and transform the bitmap to the correct place
				Trans2DMatrix Xlate(Offset.x, Offset.y);
				pNodeBitmap->Transform(Xlate);
			}
			else
			{
				// No drag and drop - bitmap is currently positioned so its bottom left hand
				// corner is at the top left of the page - translate it down by its height so
				// that the top left corner of the bitmap is aligned to the top left corner
				// of the page.

				// Do we have an additional translation to apply?
				if (pPosTranslate != NULL)
				{
					Offset.x = pPosTranslate->x;
					Offset.y = pPosTranslate->y;
				}

				// See if the bitmap contained an offset as well and if it did add it in
				// Baseclass defaults to 0,0 offset
				AddOffsetFromBitmap(&Offset);

				Offset.x = GridLock(Offset.x, HPixelSize);
				Offset.y = GridLock(Offset.y, VPixelSize);

				Trans2DMatrix Xlate(Offset.x, -(Info.RecommendedHeight) + Offset.y);
				pNodeBitmap->Transform(Xlate);
			}

			// Invalidate the region
			Success = Op->DoInvalidateNodeRegion(pNodeBitmap, TRUE, FALSE);
			if (!Success)
			{
				if (m_pImportOptions != NULL)
				{
					delete m_pImportOptions;
					m_pImportOptions = NULL;
				}
				return FALSE;	// hmmm....
			}
	
			DocRect		docrectBounds = pNodeBitmap->GetBoundingRect();
			DocRect		docrectView = pDocView->GetDocViewRect( pSpread );
            pSpread->DocCoordToSpreadCoord( &docrectView );
			if( ( docrectBounds.lo.x < docrectView.lo.x ||
				  docrectBounds.lo.y < docrectView.lo.y ||
				  docrectBounds.hi.x > docrectView.hi.x ||
				  docrectBounds.hi.y > docrectView.hi.y ) &&
				s_fZoomOnImport )
			{
				OpZoomFitRectDescriptor* pOpDesc = (OpZoomFitRectDescriptor*)OpDescriptor::FindOpDescriptor( OPTOKEN_ZOOMRECT );
				if( NULL != pOpDesc )
				{
					pOpDesc->SetZoomRect( docrectBounds );
					pOpDesc->Invoke();
				}

				if( !s_fWarnedZoomOnImport )
				{
					ErrorInfo	Info;
					memset( &Info, 0, sizeof(Info) );
					Info.ErrorMsg	= _R(IDS_ZOOM_ON_IMAGE_IMPORT);
					Info.Button[0]  = _R(IDS_OK);
					Info.Button[1]  = _R(IDS_DONT_SHOW_AGAIN);
					Info.OK			= 1;
					if( _R(IDS_DONT_SHOW_AGAIN) == UINT32(InformMessage( &Info )) )
						s_fWarnedZoomOnImport = TRUE;
				}
			}
		}
	}
	while (AreBitmapsToRead());

	// If we have loaded bitmaps onto layers and hence requested a new active layer then set
	// it now.
	// Graham 1/8/97: If we are importing an animated GIF as part of an HTML page, we don't
	// change the active layer, because this may result in subsequent bitmaps in the HTML
	// page being imported on the wrong layer.
PORTNOTE("filters","Removed HTMLFilter usage")
	if( pNewActiveLayer
#ifndef EXCLUDE_FROM_XARALX
		&& !HTMLFilter::ImportingHTML
#endif
		 )
	{
		// If the old active layer is blank and has nothing on it, then delete it
		Layer * pActiveLayer = pSpread->FindActiveLayer();
		if (pActiveLayer)
		{ 
			// Scans the layer for children that render something on the screen.
			// It ignores attributes, hidden nodes, etc - nodes that don't render anything themselves.
			Node* pNode = pActiveLayer->FindFirstChild();
			BOOL ChildFound = FALSE;
			while (pNode != NULL && !ChildFound)
			{
				ChildFound = pNode->IsBounded();
				pNode = pNode->FindNext();
			}
			// Hide the old active layer
			if (!ChildFound)
				Op->DoHideNode(pActiveLayer, TRUE);
		}

		// Check that all visible layers are actually frame layers as we have
		// created some new frame layers for each of the bitmaps.		
PORTNOTE("dialog","Removed FrameSGallery usage")
#ifndef EXCLUDE_FROM_XARALX
		FrameSGallery::MakeActiveLayer(pNewActiveLayer);
#endif
		BROADCAST_TO_ALL( LayerMsg( pNewActiveLayer, LayerMsg::UPDATE_ACTIVE_LAYER ) );

		// Try and fix up the animation properties according to the bitmaps that have been loaded in
		SetAnimationPropertiesFromLoaded(pSpread);
	}
	
	// Ensure Sel Bounds are correct after translation
	GetApplication()->UpdateSelection();

	if (m_pImportOptions != NULL)
	{
		delete m_pImportOptions;
		m_pImportOptions = NULL;
	}
	// All ok

	return TRUE;
}

/********************************************************************************************

>	BOOL BaseBitmapFilter::InsertBitmapOnNewLayer(SelOperation *pOp, Spread * pSpread, NodeBitmap *pNodeBitmap,
												  Layer ** ppNewLayer)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/5/97
	Inputs:		pOp				the operation to use to insert new items
				pSpread			the spread we are using
				pNodeBitmap		the new node bitmap which is to be inserted
	Outputs:	ppNewlayer		the new layer that was inserted (ready to be used next time around)
	Returns:	TRUE if everything was insert ok, FALSE otherwise.
	Purpose:	When loading in multiple bitmaps then each bitmap should be inserted onto
				a new layer. The first bitmap is inserted as the last child on the active
				layer. Each subsequent bitmap is loaded onto a new layer after this.
				The routine returns the new layer so that this can be used next time as the
				insertion point.
	SeeAlso:	BaseBitmapFilter::DoImport

********************************************************************************************/

BOOL BaseBitmapFilter::InsertBitmapOnNewLayer(SelOperation *pOp, Spread * pSpread, NodeBitmap *pNodeBitmap,
											  Layer ** ppNewLayer)
{
	ERROR2IF(pOp == NULL || pSpread == NULL || pNodeBitmap == NULL || ppNewLayer == NULL,FALSE,"InsertBitmapOnNewLayer bad parameters!");
	
	Layer * pCurrentLayer = NULL;
	if (*ppNewLayer)
	{
		// We have been supplied with the last inserted new layer so use this.
		pCurrentLayer = *ppNewLayer;
	}
	else
	{
		// We are the first time through and so try and create
		// a layer after the active layer and put the bitmap there.
		// We could do this but we get problems when loading multiple animations. So 
		// do what we do elsewhere and load after the last layer.
		Layer * pInsertionLayer = pSpread->FindLastLayer();

		// No Active layer so get out now!
		if (pInsertionLayer == NULL)
			return FALSE;

		// Use the active layer as the insertion point
		pCurrentLayer = pInsertionLayer;
	}

	// Create the new layer
	Layer* pNewLayer = new Layer;
	if (pNewLayer == NULL)
		return FALSE;

	// Ensure that the layer name has a frame flavour about it
	// We also need to add an imported at the start so that if the original Frame 1
	// layer is deleted as it has nothing on it, then the layer is not called Frame 2
	// but Imported Frame 1
	String_256 LayerName(_R(IDS_K_EPSFILTER_IMPORTED));
	String_256 Suffix(_R(IDS_DEFAULTFRAMENAME));	// Frame
	LayerName += Suffix;
PORTNOTE("dialog","Removed FrameSGallery usage")
#ifndef EXCLUDE_FROM_XARALX
	String_256 NewLayerName = FrameSGallery::CreateUniqueLayerID(pSpread, &LayerName);
#else
	String_256 NewLayerName = ""; 
#endif
	pNewLayer->SetLayerID(NewLayerName);

	pNewLayer->SetVisible(TRUE);			// Ensure visible
	pNewLayer->SetLocked(FALSE);			// Ensure unlocked so insertion works
	pNewLayer->SetPrintable(FALSE); 
	pNewLayer->SetBackground(FALSE);		// Ensure it is foreground 
	pNewLayer->SetOutline(FALSE);
	pNewLayer->SetGuide(FALSE);
	pNewLayer->SetPageBackground(FALSE);
#ifdef WEBSTER
	// In Webster mode, always make the new layer a frame layer
	pNewLayer->SetFrame(TRUE);				// Mark it as a frame layer
#else
	// In Camelot mode, only make the new layer a frame layer if there are
	// already frame layers in the document
	Layer * pFrame = pSpread->FindFirstFrameLayer();
	if (pFrame != NULL)
		pNewLayer->SetFrame(TRUE);			// Mark it as a frame layer
	else
		pNewLayer->SetFrame(FALSE);			// Mark it as a layer
#endif
	// Insert the new layer as the next node after the last page.
	if (!pOp->DoInsertNewNode(pNewLayer, pCurrentLayer, NEXT, FALSE,FALSE,FALSE,FALSE))
	{
		delete pNewLayer;
		pNewLayer = NULL;
		// Return the new layer to the caller
		*ppNewLayer = pNewLayer;
		return FALSE;
	}

	// And now insert the bitmap node into the new layer
	if (!pOp->DoInsertNewNode(pNodeBitmap, pNewLayer, FIRSTCHILD,FALSE,FALSE,FALSE,FALSE))
	{
		// It didn't work - hide the layer sub-tree we just created
		pOp->DoHideNode(pNewLayer, TRUE);
		// Return the new layer to the caller as null
		*ppNewLayer = NULL;
		// Assume the caller deletes the node bitmap
		return FALSE;
	}

	pNewLayer->SetVisible(FALSE);		// Ensure it is invisible
	pNewLayer->SetLocked(TRUE);			// Ensure locked 

	// Set the fact that this layer is presently fully defined by this bitmap
	KernelBitmap * pBitmap = pNodeBitmap->GetBitmap();
	pNewLayer->SetReferencedBitmap(pBitmap);

	pNewLayer->EnsureUniqueLayerID();
	BROADCAST_TO_ALL( SpreadMsg( pSpread, SpreadMsg::LAYERCHANGES ) );

	// Return the new layer to the caller
	*ppNewLayer = pNewLayer;

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::SetFlagsFromBitmap(Layer * pLayer, KernelBitmap * pBitmap,
													  UINT32 nBitmapToRead)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/97
	Inputs:		pLayer			the layer that the bitmap is being put on
				pBitmap			the bitmap being placed on the layer
				nBitmapToRead	the number of the bitmap in the sequence
	Returns:	TRUE if was ok, FALSE otherwise.
	Purpose:	When loading in multiple bitmaps and placing them on new layers, we should
				give the filter an opportunity to set the layer flags from the bitmap. In the
				case of a GIF filter this would allow it to set the solid and overlay flags.
				This baseclass version does nothing.

********************************************************************************************/

BOOL BaseBitmapFilter::SetFlagsFromBitmap(Layer * pLayer, KernelBitmap * pBitmap,
										  UINT32 nBitmapToRead)
{
	// Do nothing
	return TRUE;
}

/********************************************************************************************

>	BOOL BaseBitmapFilter::AddOffsetFromBitmap(DocCoord * pOffset)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/97
	Inputs:		pOffset			the present offset in the bitmap
	Returns:	TRUE if was ok, FALSE otherwise.
	Purpose:	When loading in a bitmap, the bitmap definition may contain an offset. We need
				to add this to the offset that we are given so that the bitmap is loaded at
				the correct position.
				This baseclass version does nothing.

********************************************************************************************/

BOOL BaseBitmapFilter::AddOffsetFromBitmap(DocCoord * pOffset)
{
	// Do nothing
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::SetAnimationPropertiesFromLoaded(Spread * pSpread)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/97
	Inputs:		pSpread		the spread that the bitmaps have been put on
	Returns:	TRUE if was ok, FALSE otherwise.
	Purpose:	When loading in multiple bitmaps and placing them on new layers, we should
				give the filter an opportunity after all the bitmaps have been loaded, to go
				and look at the bitmaps and set the animation properties accordingly.
				In the case of a GIF filter this would allow it to set the dithering to none
				and the palette to something closer to what has been loaded.
				This baseclass version does nothing.

********************************************************************************************/

BOOL BaseBitmapFilter::SetAnimationPropertiesFromLoaded(Spread * pSpread)
{
	// Do nothing
	return TRUE;
}

/********************************************************************************************

>	BOOL BaseBitmapFilter::ImportBitmap(CCLexFile* pDiskFile, KernelBitmap** ppBitmap)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/94
	Inputs:		-
	Returns:	TRUE if the bitmap was imported ok (or if it wasn't imported because the
					 file format does not support it;
				FALSE if an error occured.
	Purpose:	-
	Errors:		Usual disk/file errors.
	SeeAlso:	CamelotEPSFilter::ExportBitmap

********************************************************************************************/

BOOL BaseBitmapFilter::ImportBitmap(CCLexFile* pDiskFile, KernelBitmap** ppBitmap)
{
	// Make a note of the disk file we are given
	InputFile = pDiskFile;

	if (ProcessImportOptions() == FALSE)
		return FALSE;

	// Let's get ready
	if (!PrepareToImport())
		return FALSE;

	if (!ImportBitmap(pDiskFile, ppBitmap, 1))
	{
		if (m_pImportOptions != NULL)
		{
			delete m_pImportOptions;
			m_pImportOptions = NULL;
		}
		return FALSE;
	}
	if (m_pImportOptions != NULL)
	{
		delete m_pImportOptions;
		m_pImportOptions = NULL;
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL BaseBitmapFilter::ImportBitmap(CCLexFile* pDiskFile, KernelBitmap** ppBitmap, const UINT32 nBitmapToRead)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/06/96
	Inputs:		pDiskFile - the CCLexFile from which the bitmap should be imported
				nBitmapToRead - the position of the bitmap in the CCLexFile
	Outputs:	ppBitmap - a pointer to the KernelBitmap into which the bitmap will be
				imported
	Returns:	TRUE if the bitmap was imported ok (or if it wasn't imported because the
					 file format does not support it;
				FALSE if an error occured.
	Purpose:	Imports the nBitmapToRead'th bitmap from the given file.
				This may be used to import a file either via the menu or from the EPS filters.
	Errors:		
	SeeAlso:	CamelotEPSFilter::ExportBitmap, DoImport(), SGLibOil::

********************************************************************************************/

BOOL BaseBitmapFilter::ImportBitmap(CCLexFile* pDiskFile, KernelBitmap** ppBitmap, const UINT32 nBitmapToRead)
{
	ERROR2IF(pDiskFile == NULL, FALSE, "pDiskFile - invalid");
	ERROR2IF(ppBitmap == NULL, FALSE, "ppBitmap is NULL");
	ERROR2IF(nBitmapToRead < 1, FALSE, "nBitmapToRead < 1");

	*ppBitmap = NULL;
	SetBitmapNumber(nBitmapToRead);

	KernelBitmap* KernelBmp;
	BOOL IsNew = TRUE;

	BOOL ReuseExisting = ShouldReuseExistingBitmaps();
	
	if (IsPreviewBitmap)	//for preview bitmaps always false
		ReuseExisting = FALSE;

	BOOL Success = KernelBitmap::ImportBitmap(this, NULL, 0, &KernelBmp, ReuseExisting ? &IsNew : NULL);

	if (Success && IsNew)
	{
		// Give the bitmap a name from the file path
		CreateBitmapName(pDiskFile, KernelBmp, nBitmapToRead);

		// If we have a BitmapSource then use it
		if (m_pBitmapSource != NULL)
		{
			KernelBmp->SetOriginalSource(m_pBitmapSource, this);
			delete InputFile;
			InputFile = NULL;
		}
	}
	else
	{
		// Throw away the source as we already have it or don't need it
		if (m_pBitmapSource != NULL)
		{
			delete m_pBitmapSource;
			delete InputFile;
			InputFile = NULL;
		}
	}			

	// Free up dynamic objects used for import and close the file
	CleanUpAfterImport();

	if (!Success)
	{
		// Did not work - report this to the caller.
		return FALSE;
	}

	*ppBitmap = KernelBmp;

	return TRUE;
}


/********************************************************************************************

>	BOOL BaseBitmapFilter::ProcessImportOptions()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/06/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if it is OK to continue processing
				FALSE if an error occured or the user cancelled the import, in which case
				Error::SetError is called signalling the fact.
	Purpose:	Validates the options returned by GetImportOptions
				Sets m_pImportOptions if OK
				Shouldn't really be overridden
	SeeAlso:	BaseBitmapFilter::DoImport()

********************************************************************************************/
BOOL BaseBitmapFilter::ProcessImportOptions()
{
	// Get any user options, if required
	ERROR3IF(m_pImportOptions != NULL, "m_pImportOptions not deleted");
	BitmapImportOptions* options = GetImportOptions();
	if (options == NULL)
	{
		ERROR3("options is NULL");
		return FALSE;
	}
	ERROR3IF(!options->IS_KIND_OF(BitmapImportOptions), "options is not");
	if (!options->m_bValid)
	{
		delete options;
		m_pImportOptions = NULL;
		return TRUE;
	}
	if (options->m_bCancelled)
	{
		delete options;
		m_pImportOptions = NULL;
		Error::SetError(_R(IDN_USER_CANCELLED),0);			// Expects error set on cancel
		return FALSE;									// if cancelled
	}
	m_pImportOptions = options;
	return TRUE;
}


/********************************************************************************************

>	BOOL BaseBitmapFilter::CreateBitmapName(const CCLexFile* pDiskFile, const KernelBitmap* pBitmap,
										 const UINT32 nBitmapNumber)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/06/96
	Inputs:		pDiskFile : file from which the bitmap was extracted
				pBitmap : the bitmap itself
				nBitmapNumber : the number of the bitmap in the file
	Outputs:	-
	Returns:	TRUE if name set OK, FALSE if failed.
	Purpose:	Sets the name of the given bitmap to that of the form "filename - number.ext"
				or "filename.ext" depending on whether we have assumed there to be more than
				one image in the imported file.
				Used by ImportBitmap to supply this name and avoid bizarre file names when
				re-exporting.
	Errors:		ERROR2 for invalid arguments
				ERROR3 if pDiskFile is not a kind of CCDiskFile
	Notes:		Suppose we should really set the names after when we know that there actually 
				are multiple bitmaps

********************************************************************************************/

BOOL BaseBitmapFilter::CreateBitmapName(const CCLexFile* pDiskFile, const KernelBitmap* pBitmap,
										 const UINT32 nBitmapNumber)
{
	ERROR2IF(pDiskFile == NULL, FALSE, "pDiskFile - invalid");
	ERROR2IF(pBitmap == NULL, FALSE, "pBitmap - invalid");

	PathName Path = pDiskFile->GetPathName();
	if (!Path.IsValid())
	{
		TRACE( _T("Invalid file path in BaseBitmapFilter::CreateBitmapName\n") );
		return FALSE;
	}

	String_256 Str;
	if ((nBitmapNumber > 1) || AreBitmapsToRead())
	{
		// give it a number ?
		String_256 shortName = Path.GetFileName(FALSE);	// without extension
		String_16 index;
		Convert::LongToString(INT32(nBitmapNumber), &index);
		Str.MakeMsg(_R(IDS_BITMAP_NAME), (TCHAR*)shortName, (TCHAR*)index);
		Path.SetFileName(Str);
	}
	Str = Path.GetFileName(TRUE);	// with extension
	pBitmap->ActualBitmap->SetName(Str);
	return TRUE;
}


/********************************************************************************************

>	BOOL BaseBitmapFilter::ReadFromFile( OILBitmap* pOilBitmap )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/95
	Inputs:		pOilBitmap	pointer to the oil bitmap data to be filled in
	Outputs:	Will have filled in BMInfo	pointer to the bitmap header to fill in
									BMBytes	pointer to the bitmap data to fill in
	Purpose:	Actually does the process of reading a bitmap from a file.
				Inherited classes override this to read in different file formats.
				
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BaseBitmapFilter::ReadFromFile( OILBitmap* pOilBitmap )
{
	ERROR3("Base class bitmap filter ReadFromFile called");
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::ReadFromFile( OILBitmap* pOilBitmap,
												 BaseCamelotFilter *pFilter,
												 CCLexFile* pFile, BOOL IsCompressed)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/96
	Inputs:		pOilBitmap	pointer to the oil bitmap data to be filled in
				pFilter			- the BaseCamelotFilter which provides functions like progress update
				pFile			- the CCFile class to use to read the data from
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

BOOL BaseBitmapFilter::ReadFromFile(OILBitmap* pOilBitmap, BaseCamelotFilter* pFilter,
									CCLexFile* pFile, BOOL IsCompressed)
{
	ERROR3("Base class bitmap filter ReadFromFile called");
	return FALSE;
}

/********************************************************************************************
>	UINT32 BaseBitmapFilter::GetBitmapNumber() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/96
	Inputs:		-
	Returns:	The next bitmap to read
				0 if the bitmap number is invalid
	Purpose:	On entry to the filter this member function returns the number of the desired
				bitmap in the file. Once the import is complete, either SetBitmapNumber is
				called with an argument of the next bitmap that can be read or SetLastBitmap
				is called indicating there are no more to read.
	Scope:		protected
	SeeAlso:	SetBitmapNumber(), SetLastBitmap(), ...ReadFromFile()

********************************************************************************************/
UINT32 BaseBitmapFilter::GetBitmapNumber() const
{
	return m_nBitmapToRead < 1 ? 0 : (UINT32) m_nBitmapToRead;
}



/********************************************************************************************
>	BOOL BaseBitmapFilter::SetBitmapNumber(UINT32 nBitmapToRead)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/96
	Inputs:		-
	Returns:	TRUE if set OK
				FALSE otherwise
	Purpose:	Once a filter has read a bitmap it should call this member function with
				the number of the next bitmap that can be read. If there are none, 
				SetLastBitmap() should be called instead.
	Scope:		protected
	SeeAlso:	SetLastBitmap(), GetBitmapNumber()

********************************************************************************************/
BOOL BaseBitmapFilter::SetBitmapNumber(const UINT32 nBitmapToRead)
{
	ERROR2IF(nBitmapToRead < 1, FALSE, "nBitmapToRead invalid");
	m_nBitmapToRead = nBitmapToRead;
	return TRUE;
}


/********************************************************************************************
>	BOOL BaseBitmapFilter::SetLastBitmap()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/96
	Inputs:		-
	Returns:	TRUE if set OK
				FALSE otherwise
	Purpose:	When the bitmap filter has no more bitmaps to read from its file it should
				call this member function.
	Scope:		protected
	SeeAlso:	SetBitmapNumber(), GetBitmapNumber()

********************************************************************************************/
BOOL BaseBitmapFilter::SetLastBitmap()
{
	m_nBitmapToRead = -1;
	return TRUE;
}


/********************************************************************************************
>	BOOL BaseBitmapFilter::AreBitmapsToRead() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/96
	Inputs:		-
	Returns:	TRUE if there are bitmaps in the set OK
				FALSE otherwise
	Purpose:	The BaseBitmapFilter uses this to determine whether or not it should try
				to read another bitmap from the file.
	Scope:		protected
	SeeAlso:	GetBitmapNumber(), SetBitmapNumber(), SetLastBitmap()

********************************************************************************************/
BOOL BaseBitmapFilter::AreBitmapsToRead() const
{
	return (m_nBitmapToRead != -1);
}


/********************************************************************************************

>	BOOL BaseBitmapFilter::SetPixelWidth(const BMP_SIZE& Width)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		Width : The width of the bitmap to be exported in pixels
	Returns:	TRUE if the operation was successful
				FALSE otherwise
	Purpose:	Support function to obtain bitmap info
	See Also:	GetExportHeight()

********************************************************************************************/
BOOL BaseBitmapFilter::SetPixelWidth(const BMP_SIZE& Width)
{
	m_PixelWidth = Width;
	return TRUE;
}


/********************************************************************************************

>	BMP_SIZE BaseBitmapFilter::GetPixelWidth() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	The width of the bitmap to export in pixels
	Purpose:	Support function to obtain bitmap info

********************************************************************************************/
BMP_SIZE BaseBitmapFilter::GetPixelWidth() const
{
	return m_PixelWidth;
}


/********************************************************************************************

>	BMP_SIZE BaseBitmapFilter::GetPixelHeight() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	The height of the bitmap to export in pixels
	Purpose:	Support function to obtain bitmap info

********************************************************************************************/
BMP_SIZE BaseBitmapFilter::GetPixelHeight() const
{
	return m_PixelHeight;
}


/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::SetPixelHeight(const BMP_SIZE& Height)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		Height : The pixel height of the bitmap to be exported
	Returns:	TRUE if the operation was successful
				FALSE otherwise
	Purpose:	Support function to obtain bitmap info
	See Also:	GetExportHeight()

********************************************************************************************/
BOOL BaseBitmapFilter::SetPixelHeight(const BMP_SIZE& Height)
{
	m_PixelHeight = Height;
	return TRUE;
}


/********************************************************************************************

>	BOOL BaseBitmapFilter::GetExportOptions(* pOptions)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/96
	Inputs:		pOptions : A pointer to some BitmapExportOptions
	Outputs:	pOptions : the same with some extra information in it
	Returns:	TRUE if OK
				FALSE if user pressed Cancel.

	Purpose:	Allows the user to be prompted to get export information which should be set
				in the given BitmapExportOptions.

	Notes:		Due to the existence of three public entry points into bitmap filters, 
				two of which are virtual, the BaseBitmapFilter::WriteBitmapToFile may not 
				be called and so may not create a BitmapExportOptions object, which would 
				not be disasterous in itself, unless a derived class calls BaseBitmapFilter 
				functions which use BitmapExportOptions. It is therefore at the filter 
				author's discretion whether or not to create one of these objects.

	Scope: 		protected

********************************************************************************************/
BOOL BaseBitmapFilter::GetExportOptions(BitmapExportOptions* pOptions)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR2IF(pOptions == NULL, FALSE, "NULL Args");

	WrittenHeader = FALSE;

	// When saving a bitmap directly then only use the dialog box for certain filter types
	// as it will be just requesting compression type and hence otherwise it is superfluous.
	// Otherwise, always use it.
	BOOL Ok = TRUE;
	if (pOptions->GetSelectionType() != ABITMAP)
	{
		// This is ok as we are using a modal dialog box
PORTNOTE("export", "Remove BmpPrefsDlg usage")
#if !defined(EXCLUDE_FROM_XARALX)
		Ok = BmpPrefsDlg::InvokeDialog(pOptions);
#endif
	}

	if (Ok)
	{
		// Only save the depth as the default if not saving a bitmap itself
		// from say the bitmap gallery. (keep this rubbish for now...)
		if (pOptions->GetSelectionType() != ABITMAP && pOptions->GetSelectionType() != SOMEBITMAPS)
		{
			pOptions->SetAsDefaults();
		}
		PaletteType		= PAL_OPTIMISED;
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
void BaseBitmapFilter::PostGetExportOptions(BitmapExportOptions* pOptions)
{
	pOptions->SetAsDefaults();
}




/********************************************************************************************

 >	virtual BOOL BaseBitmapFilter::SetUpClippingRectangleForExport(Spread *pSpread, SelectionType Selection)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Inputs:		pSpread		the spread to export
				Selection	the selection type to use during export
	Returns:	TRUE if the operation was successful
				FALSE otherwise
	Purpose:	Sets up the class variable ClipRect to be the clipping rectangle to be used
				during export.
	See Also:	PrepareToExport()

********************************************************************************************/

BOOL BaseBitmapFilter::SetUpClippingRectangleForExport(Spread *pSpread, SelectionType Selection)
{
	ERROR2IF(pSpread == NULL,FALSE,"SetUpClippingRectangleForExport Bad spread");

	// We need to decide what clipping rectangle to set up which we will then render the
	// bitmap through. The choice might be to export:-
	// - the selection bounds (if there is a selection),
	// - or the current spread
	// - or the current drawing
	// First, check if there is a selection present. ClipRect should be empty if not.

	// Karim 17/07/2000
	// We need to take controller nodes into account when we're finding this bbox,
	// which means temporarily setting PromoteToParent on the selection.
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel != NULL)
	{
		ClipRect = pSel->GetBoundingRect(TRUE);
	}

	// However, if this is a Preview Bitmap then use the spreads bounds as the clip rect
	// This should in fact be the bounding rectangle of all visible layers in the current
	// drawing.
	if (IsPreviewBitmap || (Selection==DRAWING))
	{
		// Work out the size of the rectangle encompassing the drawing (visible layers only)
		ClipRect = GetSizeOfDrawing(pSpread);
	}

	if (!IsPreviewBitmap && (Selection==SELECTION))
	{
		// Quick check to make sure the selection is something sensible ...
		SelRange* pSel = GetApplication()->FindSelection();
		if (pSel && 
			pSel->Count()==1)
		{
			// Only one thing selected ... Is it the Text Caret per chance ?
			Node* pSelNode = pSel->FindFirst();
			if (pSelNode && pSelNode->IsAVisibleTextNode())
			{
				VisibleTextNode* pTextNode = (VisibleTextNode*)pSelNode;
				if (pTextNode->IsACaret())
				{
				 	// Aha! It's the Caret that's selected.

					// We'll use the bounds of the parent text line instead then ...
					Node* pTextLine = pTextNode->FindParent();
					ERROR3IF(!IS_A(pTextLine, TextLine), "Caret doesn't have a parent text line in DoCreateBitmap");
				
					// Get the bounds of the text line
					ClipRect = ((TextLine*)pTextLine)->GetBoundingRect();				
					Selection = DRAWING;
				}		 		
			}
		}
	}

	// If this is the Preview bitmap, then we have to check for too big and too small cases
	if (IsPreviewBitmap)
	{
		// Find out how big the preview is at the moment
		MILLIPOINT Width = ClipRect.Width();
		MILLIPOINT Height = ClipRect.Height();

		// Note from Martin: This is probably a bodge connected with the scaling bodge which
		// has been corrected below, but I'm leaving this one in cos it leaves a nice 1 pixel
		// boarder round the thumbnail.
		// Inflate the rect a little as we can get odd clipping effects at the edges otherwise
		ClipRect.Inflate(Width/128, Height/128);
		
		// Define what too big and too small are
		// bigger the 2 feet or smaller than 1/2 inch
		MILLIPOINT MaxWidth = 72000 * 12 * 2;
		MILLIPOINT MinWidth = 72000 / 2;

		// See if the rectangle is too big or too small and fix it if it is
		MILLIPOINT WidthChange = 0;
		MILLIPOINT HeightChange = 0;
		// too big?
		WidthChange = Width>MaxWidth ? (MaxWidth - Width)/2 : 0;	// a -ve number
		HeightChange = Height>MaxWidth ? (MaxWidth - Height)/2 : 0;	// a -ve number
		// too little?
		WidthChange = Width<MinWidth ? (MinWidth - Width)/2 : 0;	// a +ve number
		HeightChange = Height<MinWidth ? (MinWidth - Height)/2 : 0;	// a +ve number
		// Shrink or expand the clip rect
		if ( WidthChange || HeightChange )
			ClipRect.Inflate( WidthChange, HeightChange );
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL BaseBitmapFilter::PrepareToExport(Spread *pSpread, UINT32 Depth, double DPI,
											SelectionType Selection,
											UINT32 Dither = XARADITHER_ORDERED_GREY)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		Pointer to the spread, colour depth in bpp, resolution in dpi and a selection
				flag which indicates whether there is a selection present (=1), no selection
				(=0) and if there is a selection whether we want to export the spread instead
				(=2).
	Returns:	TRUE if succeeded, FALSE if not (e.g. no memory)
	Purpose:	Prepare to export bitmap data using this filter.  This sets up the filter
				to a sensible state for reading. Assumes file is opened by caller. This
				should NOT be overridden by inherited classes.
	Errors:		Out of memory.
	SeeAlso:	BaseBitmapFilter::DoImport; BaseBitmapFilter::CleanUpAfterImport
	Scope: 		Private

********************************************************************************************/

#define PIXELWIDTH 750		// @ 96DPI @ 100%
#define PIXELHEIGHT 750		// @ 96DPI @ 100%

BOOL BaseBitmapFilter::PrepareToExport(Spread *pSpread, UINT32 Depth, double DPI,
									   SelectionType Selection, UINT32 Dither)
{
#ifdef DO_EXPORT
	ERROR2IF(pSpread==NULL,FALSE,"BaseBitmapFilter::PrepareToExport null spread pointer");

	// Check whether the selection contains objects that can't be represented
	// in a bitmap
	if (WarnIfIncompatible()==FALSE)
	{
		ERRORIF(TRUE, _R(IDN_USER_CANCELLED), FALSE);
	}

	// We need to decide what clipping rectangle to set up which we will then render the
	// bitmap through. The choice might be to export:-
	// - the selection bounds (if there is a selection),
	// - or the current spread
	// - or the current drawing
	// Ask the virtual function to set it up for us

	if (ClipRect.IsEmpty())
		SetUpClippingRectangleForExport(pSpread, Selection);

	// We can either have no selection present whereby the user may have chosen to export
	// the spread or the drawing OR we have a selection present whereby the user may have
	// chosen to export the spread or drawing instead. In the later case we must output the
	// spread instead of the selection but double check there is a selection. 
	if (ClipRect.IsEmpty() || ClipRect.Width() < MinExportSize || ClipRect.Height() < MinExportSize ||
		((Selection==SPREAD) && (!IsPreviewBitmap)))
	{
		// Use our function to work out the size of the spread
		ClipRect = GetSizeOfSpread(pSpread);
		
		if (ClipRect.IsEmpty())
		{
			ENSURE(FALSE, "Current spread has no pages");
			ERRORIF(TRUE, _R(IDT_DOC_BADSTRUCTURE), FALSE);
		}
	}

	// Don't use rendering matrix when exporting BMPs as it uses coordinates as is
	Matrix Identity;

	// Don't use view scale; set to 1
	FIXED16 Scale(1);

	// If this is the Preview bitmap, the scale may need changin
	if (IsPreviewBitmap)
	{
		// Find out the maximum dimension of the clip rect
		MILLIPOINT Width = ClipRect.Width();
		MILLIPOINT Height = ClipRect.Height();
		MILLIPOINT MaxSize = (Width>Height) ? Width : Height;

		// this can't happen considering the code above - but you never know.
		ERROR3IF( MaxSize==0, "BaseBitmapFilter::PrepareToExport both dimensions of preview are zero");
		// stop div zeros. We set it quite high here otherwise we get huge values in the DPI field
		if (MaxSize==0)
			MaxSize = 1000;

		// Set the scale factor to get this down into the required size
		//WEBSTER-Martin-02/01/97
		MILLIPOINT WantSize = PreviewFilter::PreviewBitmapSize;

		// Removed changes to 'Scale' variable so the scale factor is
		// now always 1.  Jonathan Payne, 8/9/2000.

		// The Scale Factor is not actually used, so we have to change the size of things
		// by effecting the DPI. Scale the DPI, but make sure it does not get too small.
		DPI = ((double)WantSize) / ((double)MaxSize) * DPI;
		if (DPI < 4.0)
			DPI = 4.0;

		//WEBSTER-Martin-30/01/97
		// It doesn't matter what rounding we do here, but there is a problem because the bitmap
		// is not exactly the same size as the cliprect need something like ??? Any region outside
		// the cliprect is rendered as black hence the black lines that appear at the top and bottom
		// of some previews. It is especially noticable when the original is smaller than the preview.
		// Here we are using the dpi to do some scaling we're not really concerned with the exact value
		INT32 b_dpi = (INT32) DPI;
		DPI = (double)b_dpi;	//round down
	}
	else
	{
		// Ensure Bitmap is at least 1 pixel square!
		if (ClipRect.Width()<PIXELWIDTH)	ClipRect.hi.x = ClipRect.lo.x + PIXELWIDTH;
		if (ClipRect.Height()<PIXELHEIGHT)	ClipRect.hi.y = ClipRect.lo.y + PIXELHEIGHT;
	}

	// Remember the size of the bitmap
	WinRect	Rect = OSRenderRegion::BitmapDocRectToWin( Identity, ClipRect, DPI );
	SetPixelWidth(Rect.GetWidth());
	SetPixelHeight(Rect.GetHeight());

	// Create the region
//	TRACEUSER( "Gerry", _T("PrepareToExport creating GRenderBitmap %d bpp   (cmyk = %d)\n"), Depth, GetBitmapExportOptions()->IsCMYK());
	ExportRegion = new GRenderBitmap(ClipRect, Identity, Scale, Depth, DPI, FALSE, Dither, NULL, TRUE);
	if (ExportRegion == NULL)
		// Error already set by new, so just return
		return FALSE;

	ExportRegion->SetFilter(this);

	// exporting a bitmap, so we will want any bitmaps used in there to be of the best quality - sjk
	ExportRegion->SetUsingSmoothedBitmaps(TRUE);

	// MarkH 23/7/99 - New Flag to say that we require to render using the biCompression Field
	if(GetBitmapExportOptions()->GetDepth() == 32)
	{
		ExportRegion->m_DoCompression = TRUE;
	}

	// we need a View to Attach to, so that:
	//		default Quality setting
	//		CalcPixelWidth etc get called

	DocView *View = DocView::GetCurrent();
	if (View)
	{
		// Must force background rendering off as otherwise we might get a partly
		// rendered drawing or object if it is complex.
		ForceBackgroundRedrawOff(View);

		if (GetBitmapExportOptions()->GetDepth() == 1)
		{
			// If we are outputting at 1bpp, then we'll force antialiasing off,
			// so that Gavin's palette optimisation can detect objects which 
			// are 'one solid colour'
			ForceAntialiasingOff(View);
		}

		// If we are in preview bitmap mode then force anti-aliasing to be full
		// rather than the default setting for the view
		if (IsPreviewBitmap)
		{
			ForceAntialiasingFull(View);
		}

		// Attach to the right device. (note no DC)
		ExportRegion->AttachDevice(View, NULL, pSpread);
	}
	else
	{
		ERROR2(FALSE,"BaseBitmapFilter::WriteToFile no current view");
	}
#endif
	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL BaseBitmapFilter::WarnIfIncompatible()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/2004
	Inputs:		-
	Returns:	TRUE is can continue
				FALSE if export should be aborted
	Purpose:	Checks for things that can't be represented using the current options
				E.g. non-mix transparencies in Alpha channel bitmap
	Errors:		-
	Scope: 		Protected

********************************************************************************************/

BOOL BaseBitmapFilter::WarnIfIncompatible()
{
	// Check whether the user has asked us to be silent about this
	if (bDontWarnBitmapNonMixTransp)
		return TRUE;

	// Check whether the export options will make transparencies difficult
	// Is export type 32BPP including alpha?
	// If not everything's OK
	if (m_pExportOptions->GetDepth()!=32)
	{
		return TRUE;
	}

	// Check whether the export selection contains non-mix transparency
	BOOL bFound = FALSE;
	Node* pNode = NULL;
	switch (m_pExportOptions->GetSelectionType())
	{
	case SELECTION:
		{
			Range* pscanrange;
			pscanrange = Camelot.FindSelection();
			pNode = pscanrange->FindFirst(TRUE);
			while (pNode!=NULL && !bFound)
			{
				if (pNode->IsAnAttribute())
				{
					if (((NodeAttribute*)pNode)->IsATranspFill())
					{
						UINT32 transptype = ((AttrFillGeometry*)pNode)->GetTranspType();
						if (transptype!=TT_Mix && transptype!=TT_NoTranspType)
							bFound = TRUE;
					}
				}
				pNode = pscanrange->FindNext(pNode, TRUE);
			}
		}
		break;
	case DRAWING:
	case PAGE:
	case SPREAD:
		{
			Node* pRootNode = pSpread;
			pNode = pRootNode->FindFirstDepthFirst();
			while (pNode!=NULL && !bFound)
			{
				if (pNode->IsAnAttribute())
				{
					if (((NodeAttribute*)pNode)->IsATranspFill())
					{
						UINT32 transptype = ((AttrFillGeometry*)pNode)->GetTranspType();
						if (transptype!=TT_Mix && transptype!=TT_NoTranspType)
							bFound = TRUE;
					}
				}

				pNode = pNode->FindNextDepthFirst(pRootNode);
			}
		}
		break;
	default:
		ERROR2RAW("Unknown selection type in WarnIfIncompatible");
		return FALSE;
	}

	if (!bFound)
		return TRUE;

	// We need to show the warning dialog...
	INT32 ret = InformWarning(_R(IDS_WARN_INCOMPATIBLETRANSPARENCY), _R(IDS_CONTINUE), _R(IDS_ALWAYSCONTINUE), _R(IDS_CANCEL));
	if (ret==3)
		return FALSE;

	if (ret==2)
		bDontWarnBitmapNonMixTransp = TRUE;

	return TRUE;
}




/********************************************************************************************

>	void BaseBitmapFilter::ForceBackgroundRedrawOff(DocView* pView)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/96
	Inputs:		pView, pointer to the View to change
	Returns:	-
	Purpose:	Forces background redraw off in the specified view, so we render all the
				objects in one pass.
				Use RestoreBackgroundRedrawState to restore the state.
	Errors:		-
	SeeAlso:	BaseBitmapFilter::RestoreBackgroundRedrawState
	Scope: 		Protected

********************************************************************************************/

void BaseBitmapFilter::ForceBackgroundRedrawOff(DocView* pView)
{
	ERROR3IF(pView == NULL, "NULL pointer passed into BaseBitmapFilter::ForceBackgroundRedrawOff");
	if (pView == NULL)
		return;

	// We wan't to make sure we only remember the old state once ...
	if (!BackgroundRedrawStateSet)
	{
		BackgroundRedrawState = pView->GetForeBackMode();
		BackgroundRedrawStateSet = TRUE; 	// So we don't do this more than once
	}

	pView->SetForeBackMode(FALSE);
}

/********************************************************************************************

>	void BaseBitmapFilter::RestoreBackgroundRedrawState(DocView* pView)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/96
	Inputs:		pView, pointer to the View to change
	Returns:	-
	Purpose:	Restores the background redraw state in the specified view, to the state
				before we called BaseBitmapFilter::ForceBackgroundRedrawOff.
	Errors:		-
	SeeAlso:	BaseBitmapFilter::ForceBackgroundRedrawOff
	Scope: 		Protected

********************************************************************************************/

void BaseBitmapFilter::RestoreBackgroundRedrawState(DocView* pView)
{
	ERROR3IF(pView == NULL, "NULL pointer passed into BaseBitmapFilter::RestoreBackgroundRedrawState");
	if (pView == NULL)
		return;

	// Only restore if we saved the state earlier ...
	if (BackgroundRedrawStateSet)
	{
		pView->SetForeBackMode(BackgroundRedrawState);
		BackgroundRedrawStateSet = FALSE;
	}
}

/********************************************************************************************

>	void BaseBitmapFilter::ForceAntialiasingOff(DocView* pView)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/96
	Inputs:		pView, pointer to the View to change
	Returns:	-
	Purpose:	Forces Antialiasing off in the specified view.
				Use RestoreViewQuality to restore the quality.
	Errors:		-
	SeeAlso:	BaseBitmapFilter::RestoreViewQuality
	Scope: 		Protected

********************************************************************************************/

void BaseBitmapFilter::ForceAntialiasingOff(DocView* pView)
{
	ERROR3IF(pView == NULL, "NULL pointer passed into BaseBitmapFilter::ForceAntialiasingOff");
	if (pView == NULL)
		return;

	// We don't need to do anything if the view quality is already not Antialiased
	if (!ViewQualitySet && pView->RenderQuality.GetAntialiasQuality() >= Quality::FullAntialias)
	{
		ViewQuality = pView->RenderQuality;
		ViewQualitySet = TRUE; 				// So we don't do this more than once

		pView->RenderQuality.SetQuality(QUALITY_DEFAULT);
	}
}

/********************************************************************************************

>	void BaseBitmapFilter::ForceAntialiasingFull(DocView* pView)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/96
	Inputs:		pView, pointer to the View to change
	Returns:	-
	Purpose:	Forces Antialiasing to be full in the specified view.
				Use RestoreViewQuality to restore the quality.
				Used when generating preview bitmaps, which should always be in full anti-
				aliasing mode.
	Errors:		-
	SeeAlso:	BaseBitmapFilter::RestoreViewQuality
	Scope: 		Protected

********************************************************************************************/

void BaseBitmapFilter::ForceAntialiasingFull(DocView* pView)
{
	ERROR3IF(pView == NULL, "NULL pointer passed into BaseBitmapFilter::ForceAntialiasingFull");
	if (pView == NULL)
		return;

	// We don't need to do anything if the view quality is already not Antialiased
	if (!ViewQualitySet && pView->RenderQuality.GetAntialiasQuality() < Quality::FullAntialias)
	{
		ViewQuality = pView->RenderQuality;
		ViewQualitySet = TRUE; 				// So we don't do this more than once

		pView->RenderQuality.SetQuality(QUALITY_MAX);
	}
}

/********************************************************************************************

>	void BaseBitmapFilter::RestoreViewQuality(DocView* pView)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/96
	Inputs:		pView, pointer to the View to change
	Returns:	-
	Purpose:	Restores the view quality to the state before we called ForceAntialiasingOff
	Errors:		-
	SeeAlso:	BaseBitmapFilter::ForceAntialiasingOff
	Scope: 		Protected

********************************************************************************************/

void BaseBitmapFilter::RestoreViewQuality(DocView* pView)
{
	ERROR3IF(pView == NULL, "NULL pointer passed into BaseBitmapFilter::RestoreViewQuality");
	if (pView == NULL)
		return;

	if (ViewQualitySet)
	{
		// Restore the previous view quality
		pView->RenderQuality = ViewQuality;
		ViewQualitySet = FALSE;
	}
}

/********************************************************************************************

>	void BaseBitmapFilter::CleanUpAfterExport()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Purpose:	Cleans up the memory allocated by BaseBitmapFilter::PrepareToImport() - used
				when the import process ends, either normally or abnormally. Override if
				extra things are required.
	SeeAlso:	BaseBitmapFilter::PrepareToImport; BaseBitmapFilter::DoImport
	Scope: 		Private

********************************************************************************************/

void BaseBitmapFilter::CleanUpAfterExport()
{
#ifdef DO_EXPORT
	// Get rid of our dynamic objects
	if (OutputFile)
	{
		OutputFile = NULL;
	}

	DocView *View = DocView::GetCurrent();
	if (View)
	{
		if( ViewQualitySet )
		{
			//  Only need to do this if the view quality has been set but not restored.
			RestoreViewQuality(View);
		}
	}

	if (ExportRegion)
	{
		// If we have create the export region then delete it
		delete ExportRegion;
		ExportRegion = NULL;

		// Must try and set our background rendering state back to the default 
		DocView *View = DocView::GetCurrent();
		if (View)
		{
			// Force background back to its entry state
			RestoreBackgroundRedrawState(View);
		}
	}

	if (pOptimisedPalette)
	{
	 	CCFree( pOptimisedPalette );
		pOptimisedPalette = NULL;
	}

#endif
}

/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::GetRenderBottomToTop()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if going to use the default of rendering bottom to top, FALSE otherwise
	Purpose:	Find out which way we need to render

********************************************************************************************/

BOOL BaseBitmapFilter::GetRenderBottomToTop()
{
	return TRUE;
}
	

/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::DoExport(Operation* pOp, CCLexFile* pFile, PathName* pPath,
											Document* pDoc)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Inputs:		pOp - the operation that started the export off
				pDiskFile - the file to put the exported data into
				pPath - the pathname of the file to be exported to
				pDoc - the document to export
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Exports the current selection as a bitmap, via the virtual fns of the
				inherited class. Do not override unless really necessary.
	SeeAlso:	GetExportOptions; PrepareToExport; ExportRenderNodes; CleanUpAfterExport;

********************************************************************************************/

BOOL BaseBitmapFilter::DoExport(Operation *pOp, CCLexFile* pFile,
								PathName* pPath, Document* pDoc,
								BOOL ShowOptions)
{
	// The success / failure flag.
	BOOL ok = TRUE;

	// smfix put back more or less this should be only used by the preview filters

#ifdef DO_EXPORT

	// Dreamweaver integration.
//	BOOL bDesignNotesSelectedValue = FALSE;

	ERROR2IF(pOp == NULL, FALSE,"BaseBitmapFilter::DoExport no export operation");
	ERROR2IF(pFile == NULL, FALSE,"BaseBitmapFilter::DoExport no file to export to");
	ERROR2IF(pPath == NULL, FALSE,"BaseBitmapFilter::DoExport no export pathname");
	ERROR2IF(pDoc == NULL, FALSE,"BaseBitmapFilter::DoExport no document to export");

	// Set the bitmap pointer to null just in case, usually only used by DoExportBitmap
	pExportBitmap = NULL;

	// Get pointer to the spread to export.
PORTNOTE("spread", "Multi-spread warning!")
	pSpread = GetFirstSpread(pDoc);
	ERROR2IF(pSpread == NULL, FALSE,"BaseCamelotFilter::DoExport no spread to export");

	// We must now check if there is a selection present so that we can set up whether the
	// user gets the choice of exporting the selection, drawing or spread if there is a 
	// selection present OR just a choice between the spread or drawing if no selection is
	// present.
	// If have a caret selected in a text story then the selection will be almost zero so
	// trap this case as well. 
	ClipRect = GetApplication ()->FindSelection ()->GetBoundingRect(TRUE);
	SelectionType Selection = DRAWING;

	// Determine the selection type.
	if ( !IsPreviewBitmap && !ClipRect.IsEmpty() && ClipRect.Width() >= MinExportSize && ClipRect.Height() >= MinExportSize )
	{
		Selection = SELECTION; // Something is selected, so use this.
	}
	else // The selection is either too small to export, or the clipping rectangle is empty.
	{
		// Work out the size of the rectangle encompassing the drawing (visible layers only).
		ClipRect = GetSizeOfDrawing(pSpread);

		// Check that that cliprect is ok, if not then set the spread as the export type.
 		if ( ClipRect.IsEmpty() || ClipRect.Width() < MinExportSize || ClipRect.Height() < MinExportSize )
			Selection = SPREAD;
	}

	// If there is no selection then warn the user that exporting the current spread may
	// take a lot of disc space, but only if this is not a Preview Bitmap.
	if ((!IsPreviewBitmap) && (Selection == SPREAD))
	{
		// Warn the user that there is no selection = large export
		ErrorInfo Info;
		Info.ErrorMsg = _R(IDT_BMPEXP_NOSELECTION);
		Info.Button[0] = _R(IDB_EXPQUERY_EXPORT);
		Info.Button[1] = _R(IDB_EXPQUERY_DONTEXPORT);
		if ((ResourceID)(AskQuestion(&Info)) == _R(IDB_EXPQUERY_DONTEXPORT))
		{
			Error::SetError(_R(IDN_USER_CANCELLED),0);		// Expects error set on cancel
			ok = FALSE;
		}
	}

	// Mark H 18/12/00
	// From here on, I`ve tidied up the code so that this function is responsible for it`s
	// own Bitmap Export Options pointer and not reliant on the member variable. This fixes
	//a 1.5MB!!! memory leak on exiting of the program!

	// Create a new Bitmap Export Options pointer for the current filter
	BitmapExportOptions* pExportOptions = CreateExportOptions();

	// Try to create the export options.
	if (ok && pExportOptions)
	{
		pExportOptions->RetrieveDefaults();
		pExportOptions->SetSelectionType(Selection);
		pExportOptions->SetBackgroundTransparency(FALSE);
	}
	else
		ok = FALSE;

	//Graham W 3/7/97: Tell the preview dialog what the path to save out to is
	BmapPrevDlg::m_pthExport=*pPath;

	if(ok)
		ok = GetExportOptions(pExportOptions);

	// This pointer is used to call the ExportBitmap function. If the filetype has changed,
	// then it will be set-up to be invoked from the new type.
	BaseBitmapFilter *pFilter = this;

	// Write out the bitmap.
	if (ok)
	{
		if (!pFilter->DoExportWithOptions(pOp, pFile, pPath, pDoc, pExportOptions))
		{
			// Export failed, therefore no valid export options.
			Error::SetError(_R(IDN_USER_CANCELLED),0);
		}
	}

	// Clear up the existing bitmap options.
	if(pExportOptions)
	{
		delete pExportOptions;
		pExportOptions = NULL;
	}

#endif

	return ok;
}


/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::DoExportDoc(Operation* pOp, CCLexFile* pFile, PathName* pPath,
											   Document* pDoc,	UINT32 Depth, double DPI,
											   const SelectionType& Selection , BOOL UseExistingPalette)
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Purpose:	Actually does the work of DoExport() above
				Code split out for use by DoExportBitmaps() function
				This DOES NOT call CleanUpAfterExport() if it fails
				If the call returns FALSE then the caller must call CleanUpAfterExport()
	Returns:	TRUE if worked, FALSE if failed.
	Scope:		protected
	SeeAlso:	GetExportOptions; PrepareToExport; ExportRenderNodes; CleanUpAfterExport;
	Additional information( ap ) - Added 'UseExistingPalette'. If this is TRUE, then the 
	existing palette in BmapPrevDlg is used instead of a newly generated one. This variable 
	is given the default value of FALSE.

********************************************************************************************/

BOOL BaseBitmapFilter::DoExportDoc(Operation* pOp, CCLexFile* pFile, PathName* pPath,
								   Document* pDoc, UINT32 RenderDepth, double DPI,
								   const SelectionType& Selection , BOOL UseExistingPalette)
{
#ifdef DO_EXPORT
	if (!WritePreFrame()) return(FALSE);

	/*
	//  Only try to do the exact palette if we are using an 8 bit optimised palette
	if( ( PaletteType == 1 ) && ( GetBitmapExportOptions()->GetDepth() == 8 ) )
		GRenderOptPalette::UseOldPalette = FALSE;
	else
		GRenderOptPalette::UseOldPalette = TRUE;
	*/

	// Set up device context and render region for this export.
	// This will show a progress hourglass for the objects being rendered
	// This will now also write the data out to file via our ExportRenderNodes function
	if (!PrepareToExport(pSpread, RenderDepth, DPI, Selection))
	{
		return FALSE;
	}

	// SMFIX
	// this is where we will build up the palette from the export options IF the one
	// that is already in the export options is NOT valid
	// the fn bellow export render needs the palette to be valid
//	m_pExportOptions->CreateValidPalette();

	// this call sets this palette in the lower reaches of the code from the palette held in the export options
//	AlterPaletteContents(m_pExportOptions->GetLogicalPalette());

	// Export the data to the file ensuring we render in strips, if required.
	RenderInStrips = TRUE;
	if (!ExportRender(ExportRegion))
	{
		// SMFIX - Make sure we reset the StripStart as it could stuff up further operations.
		m__StripStart = 0;
		return FALSE;
	}

	
	//SMFIX was calling	AlterPaletteContents( lpPalette ); on a palette that it built here



	// Ask the filter if it requires a second pass export render. This might be used for
	// say preparing a mask for transparency or something. Used by the GIF filter for this
	// purpose.
	/*
	if (IsSecondPassRequired())
	{
		WritePreSecondPass();

		// Set up device context and render region for this export.
		// This will show a progress hourglass for the objects being rendered
		// This will now also write the data out to file via our ExportRenderNodes function
		if (!PrepareToExport(pSpread, RenderDepth, DPI, Selection))
		{
			return FALSE;
		}

		// Export the data to the file esnuring we render in strips, if required.
		RenderInStrips = TRUE;
		if (!ExportRender(ExportRegion))
		{
			return FALSE;
		}
	}
	*/
	
	if (!WritePostFrame())
	{
		// SMFIX - Make sure we reset the StripStart as it could stuff up further operations.
		m__StripStart = 0;
		return FALSE;
	}

	// SMFIX - Make sure we reset the StripStart as it could stuff up further operations.
	m__StripStart = 0;
#endif
	return TRUE;
}




/********************************************************************************************

>	BOOL BaseBitmapFilter::DoExportBitmap(Operation* pOp, CCLexFile* pFile, PathName* pPath,
										  KernelBitmap* pBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:			
	Purpose:	Exports the specified bitmap straight out to file with none of the rendering
				that DoExport does. Uses the virtual fns of the	inherited class.
				Do not override unless really necessary.
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BaseBitmapFilter::DoExportBitmap(Operation* pOp, CCLexFile* pFile, PathName* pPath,
									  KernelBitmap* pBitmap)
{
#ifdef DO_EXPORT
	ERROR2IF(pBitmap == NULL,FALSE,"BaseBitmapFilter::DoExportBitmap null bitmap pointer specified");

	// Note this pointer for later use
	pExportBitmap = pBitmap;

	// Note this ptr for use in JPEG export.
	JPEGExportOptions::SetKernelBitmap(pBitmap);
	
	// Get a pointer to the actual bitmap so that we can get some details from it.
	OILBitmap *pOilBitmap = pBitmap->ActualBitmap;
	ERROR2IF(pOilBitmap == NULL,FALSE,"BaseBitmapFilter::DoExportBitmap null oil bitmap pointer");

	// Create a record of information about the export
	m_pExportOptions = CreateExportOptions();

	if (m_pExportOptions == NULL)
	{
		return FALSE;
	}

	// Get the default settings
	GetBitmapExportOptions()->RetrieveDefaults();

	// Specify to the user what the export options for depth etc are going to be
	GetBitmapExportOptions()->SetSelectionType(ABITMAP);

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

	if (PixWidth > 0)
	{
		GetBitmapExportOptions()->SetDPI((PixWidth * 72000.0)/(double)RecWidth);
	}

	// WEBSTER - markn 5/2/97
	// If the bitmap has a transparent colour index, store it in the options object
	// -1 means no transparent colour
	INT32 TransIndex = -1;
	if (!pBitmap->GetTransparencyIndex(&TransIndex)) TransIndex = -1;
	GetBitmapExportOptions()->SetTransparencyIndex(TransIndex);

	BOOL ok = TRUE;

/* It used to call the export options dlg here - but why bother just export as is
	BOOL ok = GetExportOptions(GetBitmapExportOptions());
	if (!ok)
	{
		delete m_pExportOptions;
		m_pExportOptions = 0;

		Error::SetError(_R(IDN_USER_CANCELLED),0);			// Expects error set on cancel
		return FALSE;									// if cancelled
	}

	if (GetBitmapExportOptions()->HasTempFile())
	{
		// the export options preview dialog has produced a temp file, so just rename that

		// get the temp file name
		PathName TempPath = GetBitmapExportOptions()->GetPathName();

		String_256 FinalName = pPath->GetPath();
		String_256 TempName = TempPath.GetPath();

		if (pFile->isOpen())
			pFile->close();

		// delete the empty file, if one was created
		FileUtil::DeleteFile(pPath);
					 
		// try to rename the file
		if (_trename(TempName, FinalName) != 0)
			ok = FALSE;
		else
			ok = TRUE;
	}
	else */
	{
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
				TRACEUSER( "JustinF", _T("Tried to open non-CCDiskFile in BaseBitmapFilter::DoExportBitmap\n") );
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
	}

	// All done - deallocate dynamic objects, stop the progress display/hourglass
	// and return success. (Also closes file).
	CleanUpAfterExport();

	// we created these here so we should delete them
	delete m_pExportOptions;
	m_pExportOptions = NULL;

	return ok;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::DoExportBitmaps(Operation* pOp, CCLexFile* pFile,
												   PathName* pPath, BitmapExportParam* pParam)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:			
	Purpose:	Exports the bitmaps specified by the BitmapExportParam object to a multi-
				image capable filter.
				It works creating a special document containing a single NodeBitmap
				It then sets this document to use each bitmap in turn and calls DoExport
				to actually export it.
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BaseBitmapFilter::DoExportBitmaps(Operation *pOp, CCLexFile* pFile, PathName* pPath,
										BitmapExportParam* pParam)
{
#ifdef DO_EXPORT
	ERROR2IF(pOp == NULL || pFile == NULL || pPath == NULL, FALSE,"NULL Parameters");
	ERROR2IF(pParam == NULL,FALSE,"BaseBitmapFilter::DoExportBitmaps null BitmapExportParam specified");

	// Create an object so we can ask the user for some options
	if (m_pExportOptions)
		delete m_pExportOptions;

	m_pExportOptions = CreateExportOptions();

	if (m_pExportOptions == NULL)
		return FALSE;

	// Get the first bitmap pointer
	KernelBitmap* pBitmap = pParam->GetBitmap(0);
	ERROR2IF(pBitmap == NULL,FALSE,"BaseBitmapFilter::DoExportBitmaps has no bitmaps to export");

	pExportBitmap = pBitmap;

	// Get a pointer to the actual bitmap so that we can get some details from it.
	OILBitmap *pOilBitmap = pBitmap->ActualBitmap;
	ERROR2IF(pOilBitmap == NULL,FALSE,"BaseBitmapFilter::DoExportBitmaps null oil bitmap pointer");

	// Specify to the user what the export options for depth etc are going to be
	GetBitmapExportOptions()->SetSelectionType(SOMEBITMAPS);

	GetBitmapExportOptions()->SetBitmapExportParam(pParam); 

	// Get the defaults from the first bitmap
	BitmapInfo BmInfo;
	pOilBitmap->GetInfo(&BmInfo);
	GetBitmapExportOptions()->SetDepth(BmInfo.PixelDepth);	// get the bitmaps bpp

	// Calculate the millipoint size from the pixel size assuming 96 dpi
	double DPI = 96.0;
	MILLIPOINT RecWidth = (MILLIPOINT) ((((double)BmInfo.PixelWidth * 72000.0) / DPI) + 0.5);
	MILLIPOINT RecHeight = (MILLIPOINT) ((((double)BmInfo.PixelHeight * 72000.0) / DPI) + 0.5);
	// Create a Coord for GetExportOptions
	GetBitmapExportOptions()->SetOutputSize(RecWidth, RecHeight);

	BOOL ok = GetExportOptions(GetBitmapExportOptions());
	if (!ok)
	{
		delete m_pExportOptions;
		m_pExportOptions = NULL;

		Error::SetError(_R(IDN_USER_CANCELLED),0);			// Expects error set on cancel
		return FALSE;									// if cancelled
	}

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
			TRACEUSER( "JustinF", _T("Tried to open non-CCDiskFile in BaseBitmapFilter::DoExportBitmaps\n") );
			return FALSE;
		}
	}

	// Make a note of the Disk file we are to use
	OutputFile = pFile;

	// Make sure the Region pointer is NULL for the time being
	ExportRegion = NULL;

	// TODO: Rampant change to Selection
	GetBitmapExportOptions()->SetSelectionType(DRAWING);

	// Output the file header
	if (!WriteFileHeader())
	{
		CleanUpAfterExport();
		return(FALSE);
	}

	// Remember the current document as we are just about to use a new document and
	// then delete it. Of course, systems like the bitmap system may just decide to
	// change current document just to screw things up. Put back before the end as the
	// EndOp will broadcast a message causing controls to update which require current
	// document to get the units list correctly.
	Document *pOldCurrent = Document::GetCurrent();

	// Create a document for the export
	BitmapExportDocument* pDoc = new BitmapExportDocument;

	if (pDoc != NULL)
	{
		if (!pDoc->Init(pBitmap, DocRect(0, 0, GetBitmapExportOptions()->GetOutputSize().x, GetBitmapExportOptions()->GetOutputSize().y)))
		{
			TRACEUSER( "Gerry", _T("BitmapExportDocument::Init() failed\n") );
			delete pDoc;
			pDoc = NULL;
			CleanUpAfterExport();
			if (pOldCurrent != NULL)
				pOldCurrent->SetCurrent();
			return(FALSE);
		}

		// Get pointer to the spread to export.
PORTNOTE("spread", "Multi-spread warning!")
		pSpread = GetFirstSpread(pDoc);

		UINT32 BitmapCount = pParam->GetBitmapCount();

		for (UINT32 Index = 0; ok && (Index < BitmapCount); Index++)
		{
			// Get the bitmap pointer
			pBitmap = pParam->GetBitmap(Index);

			// We have to render the bitmap so...
			// Update the BitmapExportDocument
			pDoc->SetBitmap(pBitmap);

			// Export the document
			// If it fails then break out of the image loop
			ok = DoExportDoc(	pOp, pFile, pPath, pDoc, GetRenderDepth(), 
								GetBitmapExportOptions()->GetDPI(), 
								GetBitmapExportOptions()->GetSelectionType());

			if (ExportRegion != NULL)
			{
				delete ExportRegion;
				ExportRegion = NULL;
			}
			if (pOptimisedPalette != NULL)
			{
			 	CCFree( pOptimisedPalette );
				pOptimisedPalette = NULL;
			}

		}

		delete pDoc;
		pDoc = NULL;
	}

	// Write any end of file stuff
	if (ok)
		ok = WriteFileEnd();

	// All done - deallocate dynamic objects, stop the progress display/hourglass
	// and return success. (Also closes file).
	CleanUpAfterExport();

	// Ensure that we return CurrentDoc to its original value
	// Leave QuickShape tool with item selected before exporting animated gif from bitmap
	// gallery. Convert/Unit system blows up as using current doc!
	if (pOldCurrent != NULL)
		pOldCurrent->SetCurrent();

	return ok;
#endif
	return FALSE;
}




/********************************************************************************************

>	BOOL BaseBitmapFilter::SetUpExportOptions(BitmapExportOptions **ppExportOptions, 
												BOOL OnlyDefaults = FALSE)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/97
	Inputs:		
	Outputs:	ppExportOptions - pointer, where a pointer to the export options object 
				will be returned
	Purpose:	Displays the export options dialog for this bitmap filter. This allows export
				options to be obtained by somebody else then the filter
	Returns:	TRUE if worked, FALSE if failed.

  NB. SMFIX sjk this function will if you pass it a NULL ptr new the correct options for the filter for
	the calling function. This is great. However in this case the caller function has responsibility
	to delete this export object when they are finished with it. They receive the export object through
	the static member variable BmapPrevDlg::m_pExportOptions which is a ptr to the base class BitmapExportOptions,
	rather than just filling in the users structure, since if the graphic type has changed the users class will have
	been deleted and the export object could be a different class derived from BitmapExportOptions.

********************************************************************************************/

BOOL BaseBitmapFilter::SetUpExportOptions(BitmapExportOptions **ppExportOptions, BOOL OnlyDefaults)
{
	ERROR2IF(ppExportOptions == NULL, FALSE,"BaseBitmapFilter::Can't Set up export options");

	Document *pDoc = Document::GetCurrent();

	// Get pointer to the spread to export.
PORTNOTE("spread", "Multi-spread warning!")
	pSpread = GetFirstSpread(pDoc);
	ERROR2IF(pSpread == NULL, FALSE,"BaseCamelotFilter::DoExport no spread to export");

	// We must now check if there is a selection present so that we can set up whether the
	// user gets the choice of exporting the selection, drawing or spread if there is a 
	// selection present OR just a choice between the spread or drawing if no selection is
	// present.
	// If have a caret selected in a text story then the selection will be almost zero so trap
	// this case as well. 
	DocRect ClipRect = GetApplication()->FindSelection()->GetBoundingRect(TRUE);
	SelectionType Selection = DRAWING;
 	if (
 		ClipRect.IsEmpty() ||
 		ClipRect.Width() < MinExportSize || ClipRect.Height() < MinExportSize
 	   )
		Selection = DRAWING;		// no selection present, so choose drawing by default
	else
		Selection = SELECTION;		// selection present, so choose this by default

	if (Selection == DRAWING)
	{
		// Work out the size of the rectangle encompassing the drawing (visible layers only)
		ClipRect = GetSizeOfDrawing(pSpread);

		// Check that that cliprect is ok, if not then set the spread as the export type
 		if (
 			ClipRect.IsEmpty() ||
 			ClipRect.Width() < MinExportSize || ClipRect.Height() < MinExportSize
 		   )
			Selection = SPREAD;
	}
	

	// If there is no selection then warn the user that exporting the current spread may
	// take a lot of disc space, but only if this is not a Preview Bitmap.
	if ((!IsPreviewBitmap) && (Selection == SPREAD))
	{
		// Warn the user that there is no selection = large export
		ErrorInfo Info;
		Info.ErrorMsg = _R(IDT_BMPEXP_NOSELECTION);
		Info.Button[0] = _R(IDB_EXPQUERY_EXPORT);
		Info.Button[1] = _R(IDB_EXPQUERY_DONTEXPORT);
		if ((ResourceID)AskQuestion(&Info) == _R(IDB_EXPQUERY_DONTEXPORT))
		{
			Error::SetError(_R(IDN_USER_CANCELLED),0);		// Expects error set on cancel
			return FALSE;								// if cancelled
		}
	}

	// Create a record of information about the export
	*ppExportOptions = CreateExportOptions();
	if (*ppExportOptions == NULL)
	{
		return FALSE;
	}

	(*ppExportOptions)->RetrieveDefaults();

	(*ppExportOptions)->SetSelectionType(Selection);

	// check whether only the defaults are needed
	if (OnlyDefaults)
		return TRUE;

	// get the user options for depth etc here
	BOOL ok = GetExportOptions( *ppExportOptions );

	// remember this ptr and make it externally available
	// dont do this any more the above call brings up the export dlg and sets the BmapPrevDlg::m_pExportOptions
	// even when the user has messed around with them
	//BmapPrevDlg::m_pExportOptions = *ppExportOptions;
	// so rather the *ppExportOptions should reflect the state that the dlg left them in
	if (BmapPrevDlg::m_pExportOptions)
		*ppExportOptions = BmapPrevDlg::m_pExportOptions;

	if (!ok)
	{
		if (*ppExportOptions != NULL)
		{
			delete *ppExportOptions;
			*ppExportOptions = NULL;
		}
		Error::SetError(_R(IDN_USER_CANCELLED),0);			// Expects error set on cancel
		BmapPrevDlg::m_pExportOptions = NULL;	// we have no export options

		return FALSE;									// if cancelled
	}

	return TRUE;
}




/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::DoExportWithOptions(Operation* pOp, CCLexFile* pFile, 
							PathName* pPath, Document* pDoc, BitmapExportOptions *pOptions)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/97
	Inputs:		pOp - the operation that started the export off
				pFile - the file to put the exported data into
				pPath - the pathname of the file to be exported to
				pDoc - the document to export
				pOptions - the bitmap export options to use when exporting. The options are 
				deleted after the export, so if you want to keep them make a copy first.
	Purpose:	Exports the document by using the export options passed as parameter, rather 
				then invoking an options dialog box to obtain them.
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BaseBitmapFilter::DoExportWithOptions(Operation* pOp, CCLexFile* pFile, PathName* pPath,
								   Document* pDoc, BitmapExportOptions *pOptions, DocRect *pRect)
{
	ERROR2IF(pOp == NULL, FALSE,"BaseBitmapFilter::DoExport no export operation");
	ERROR2IF(pFile == NULL, FALSE,"BaseBitmapFilter::DoExport no file to export to");
	ERROR2IF(pPath == NULL, FALSE,"BaseBitmapFilter::DoExport no export pathname");
	ERROR2IF(pDoc == NULL, FALSE,"BaseBitmapFilter::DoExport no document to export");

	// Have any valid export options been passed in?
	if (pOptions == NULL)
	{
		// Create a record of information about the export.
		SetUpExportOptions(&m_pExportOptions, TRUE);
		if (m_pExportOptions == NULL)
			return FALSE;
	}
	else
	{
		// Copy the existing export options.
		//m_pExportOptions = pOptions->MakeCopy ();
		m_pExportOptions = pOptions; // rather point at the options passed in
	}

	// set up the pSpread
PORTNOTE("spread", "Multi-spread warning!")
	pSpread = GetFirstSpread(pDoc);

	// deal with no selection
	SelRange* pSelection = GetApplication()->FindSelection();
	// deal with exporting no selection as the drawing
	if (pRect == NULL && pSelection->Count() == 0 && (m_pExportOptions->GetSelectionType() == SELECTION))
		m_pExportOptions->SetSelectionType(DRAWING);

	if (pRect != NULL)	// Force the cliping rect for the image to be the one passed in
						// and not just defined by the selection! sjk
	{
		ClipRect = *pRect;
	}
	else
	{
		// reassign the clipping rectangle to the selection's bounding rect
		SetUpClippingRectangleForExport(pSpread, m_pExportOptions->GetSelectionType());

		// Set the page limits to drawing if thats whats requested
		if (pSpread && m_pExportOptions->IsClipToPage() )
		{
			Page *pPage = pSpread->FindFirstPageInSpread(); 
			if (pPage)
				ClipRect = pPage->GetPageRect();
		}	

		// to use SJK's new fiddle the rect to maintain the antialiasing
		// as seen on screen call the function bellow
		// If you want it to work like it did in CX2 comment the line out
		if( m_pExportOptions->GetAntiAliasing() == MAINTAIN_SCREEN_AA )
			PixelAlignedFiddle(&ClipRect);
	}

	BOOL ok = TRUE;

	// Always export the whole thing into one composite file
	ok = DoExportHelper(pOp, pFile, pPath, pDoc);

	if (!IsPreviewBitmap && m_pExportOptions->CanExportSeparateLayers())
	{
		// We need to export each layer as a separate file
		Layer* pLayer = pSpread->FindFirstLayer();
		while (ok && pLayer!=NULL)
		{
			if (pLayer->IsVisible()
				&& !pLayer->IsGuide()
				&& pLayer->HasLayerGotRenderableChildren()
				)
			{
				SetSoleLayer(pLayer);

				PathName pathLayer = pLayer->MakeExportLayerName(*pPath);
				CCLexFile* pLayerFile = new CCDiskFile(pathLayer, ios::in | ios::out | ios::binary | ios::trunc);

				if (ok) ok = DoExportHelper(pOp, pLayerFile, &pathLayer, pDoc);

				delete pLayerFile;

				SetSoleLayer(NULL);
			}

			pLayer = pLayer->FindNextLayer();
		}
	}

	// Delete the newly created export options ONLY if we did create them
	if (pOptions == NULL)
	{
		delete m_pExportOptions;
		m_pExportOptions = NULL;
	}

	// Restore the file.
	OutputFile = pFile;

	return ok;
}



/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::DoExportWithOptions(Operation* pOp,
													   CCLexFile* pFile,
													   PathName* pPath,
													   Document* pDoc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> (from Stefan)
	Created:	24/03/2004
	Inputs:		pOp - the operation that started the export off
				pFile - the file to put the exported data into
				pPath - the pathname of the file to be exported to
				pDoc - the document to export
	Purpose:	Exports the document by using the export options passed as parameter, rather 
				then invoking an options dialog box to obtain them.
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BaseBitmapFilter::DoExportHelper(Operation* pOp,
									  CCLexFile* pFile,
									  PathName* pPath,
									  Document* pDoc)
{
	// Used to open the file up before starting DoExport. But this meant a cancel on the export
	// options dialog had filled the file, if it was already present. So now up up here if
	// not open already. In the PreviewBitmap case the file will already be open.
	if ( !pFile->isOpen() && 
		 pFile->IsKindOf ( CC_RUNTIME_CLASS ( CCDiskFile ) ) &&
		 !OpenExportFile ( static_cast<CCDiskFile*> ( pFile ), pPath) )
	{
		 return FALSE;
	}
	
	WrittenHeader = 0;
	SetDepthToRender(32);
	
	// set the palette type which might get changed during the preview
	if (m_pExportOptions->IS_KIND_OF(BMPExportOptions))
	{
		PaletteType = ((BMPExportOptions *)m_pExportOptions)->GetPalette() ? 1 : 0;	// 1 for an optimised palette
		BMPFilter::SetDefaultExportDPI(m_pExportOptions->GetDPI());
		BMPFilter::SetDefaultExportDepth(m_pExportOptions->GetDepth());
PORTNOTE("filter", "Removed use fo GIFFilter")
#ifndef EXCLUDE_FROM_XARALX
		// Need to set the dither for the accusoft filter here, otherwise the dither
		// changes by the user will not have any effect
		AccusoftFilters::SetDitherToUse(m_pExportOptions->GetDither());
#endif
	}
	else if (m_pExportOptions->IS_KIND_OF(MaskedFilterExportOptions))
	{
		PaletteType = ((MaskedFilterExportOptions *)m_pExportOptions)->GetPalette() ? 1 : 0;// 1 for an optimised palette
	}

	// set the render depth, which might change during the preview
	switch (m_pExportOptions->GetFilterType())
	{
PORTNOTE("filter", "Removed use fo GIFFilter")
#ifndef EXCLUDE_FROM_XARALX
		case TI_GIF:
			if (m_pExportOptions->GetDepth() > 8)
				m_pExportOptions->SetDepth(8);
			SetDepthToRender(m_pExportOptions->GetDepth());
			break;
#endif

		case JPEG:
			SetDepthToRender(24);
			break;

		default:
			SetDepthToRender(m_pExportOptions->GetDepth());
			break;
	}

//Mark Howitt, 24/10/97. Reset the FilterType variable as Importing uses this for something else!
PORTNOTE("filter", "Removed use of AccusoftFilters")
#if !defined(EXCLUDE_FROM_XARALX) && !defined(WEBSTER)
	// Andy Hills, 21-12-00
	// Warning: bodge alert!
	// At this stage, AccusoftFilters::FilterType may contain a useful value.
	// E.g. 9 for TIFF_LZW. This means that AccusoftFilters can make a special
	// case for exporting TIFFs, and export using CTIFFWriter instead of
	// calling Snowbound.
	// For whatever reason, the following line of code replaced this value with
	// m_pExportOptions->m_FilterID, which since the rewrite of the export dlg
	// contains the value 0.
	// This bodge ensures that we don't overwrite AccusoftFilters::FilterType
	// with 0.
	if (m_pExportOptions->GetFilterType() != 0)
		AccusoftFilters::SetFilterType(m_pExportOptions->GetFilterType());
#endif

	// Make a note of the Disk file we are to use
	OutputFile = pFile;

	// Actually export the document
	BOOL ok = WriteFileHeader();
		
	// Do we want to use the existing palette, or generate a new one? The
	// value of BmapPrevDlg::m_bUseExistingPalette determines whether or
	// not one is needed to be generated, so no need to use any switch
	// logic.
	if ( ok )
	{
		ok = DoExportDoc( pOp, pFile, pPath, pDoc, GetRenderDepth (), 
						   m_pExportOptions->GetDPI (), 
						   m_pExportOptions->GetSelectionType (),
						   BmapPrevDlg::m_bUseExistingPalette );
	}

	ok &= WriteFileEnd();

	// pOptimised memory freed here.
	CleanUpAfterExport();

	return ok;
}




/********************************************************************************************

>	BOOL BaseBitmapFilter::SetUpExportBitmapOptions(BitmapExportOptions **ppExportOptions, 
											KernelBitmap *pBitmap, BOOL OnlyDefaults = FALSE)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/97
	Inputs:		
	Outputs:	ppExportOptions - pointer, where a pointer to the export options object 
				will be returned
	Purpose:	Displays the export options dialog for this bitmap filter. This allows export
				options to be obtained by somebody else then the filter
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BaseBitmapFilter::SetUpExportBitmapOptions(BitmapExportOptions **ppExportOptions, 
												KernelBitmap *pBitmap, BOOL OnlyDefaults)
{
#ifdef DO_EXPORT
	ERROR2IF(pBitmap == NULL,FALSE,"BaseBitmapFilter::DoExportBitmap null bitmap pointer specified");

	// Note this pointer for later use
	pExportBitmap = pBitmap;

	// Note this ptr for use in JPEG export.
	JPEGExportOptions::SetKernelBitmap(pBitmap);
	
	// Get a pointer to the actual bitmap so that we can get some details from it.
	OILBitmap *pOilBitmap = pBitmap->ActualBitmap;
	ERROR2IF(pOilBitmap == NULL,FALSE,"BaseBitmapFilter::DoExportBitmap null oil bitmap pointer");

	// Create a record of information about the export
	*ppExportOptions = CreateExportOptions();
	if (*ppExportOptions == NULL)
	{
		return FALSE;
	}

	// Get the default settings
	(*ppExportOptions)->RetrieveDefaults();

	// Specify to the user what the export options for depth etc are going to be
	(*ppExportOptions)->SetSelectionType(ABITMAP);

	// Get the details from the specified bitmap
	BitmapInfo BmInfo;
	pOilBitmap->GetInfo(&BmInfo);
	(*ppExportOptions)->SetDepth(BmInfo.PixelDepth);	// get the bitmaps bpp
	
	// Should really save the dpi when we load the file itself rather than doing
	// all this conversion with possible rounding errors.
	// Use the original size that has been calculated in the info header
	UINT32 PixWidth  = BmInfo.PixelWidth;
//	UINT32 PixHeight = BmInfo.PixelHeight;
	MILLIPOINT	RecWidth = BmInfo.RecommendedWidth;
//	MILLIPOINT	RecHeight = BmInfo.RecommendedHeight;

	if (PixWidth > 0)
	{
		(*ppExportOptions)->SetDPI((PixWidth * 72000.0)/(double)RecWidth);
	}

	// WEBSTER - markn 5/2/97
	// If the bitmap has a transparent colour index, store it in the options object
	// -1 means no transparent colour
	INT32 TransIndex = -1;
	if (!pBitmap->GetTransparencyIndex(&TransIndex))
		TransIndex = -1;
	(*ppExportOptions)->SetTransparencyIndex(TransIndex);

	// check whether only the defaults are needed
	if (OnlyDefaults)
		return TRUE;

	BOOL ok = GetExportOptions(*ppExportOptions);
	if (!ok)
	{
		if (*ppExportOptions != NULL)
		{
			delete *ppExportOptions;
			*ppExportOptions = NULL;
		}
		Error::SetError(_R(IDN_USER_CANCELLED),0);			// Expects error set on cancel
		return FALSE;									// if cancelled
	}

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL BaseBitmapFilter::DoExportBitmapWithOptions(Operation* pOp, CCLexFile* pFile, PathName* pPath,
												KernelBitmap* pBitmap, BitmapExportOptions *pOptions)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/97
	Inputs:			
	Purpose:	Exports the specified bitmap straight out to file with none of the rendering
				that DoExport does. Uses the virtual fns of the	inherited class.
				Do not override unless really necessary. Similar to DoExportBitmap in the base
				class, but uses the passed options, rather then invoking a dialog to get ones.
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BaseBitmapFilter::DoExportBitmapWithOptions(Operation* pOp, CCLexFile* pFile, PathName* pPath,
												KernelBitmap* pBitmap, BitmapExportOptions *pOptions)
{
#ifdef DO_EXPORT
	ERROR2IF(pBitmap == NULL,FALSE,"BaseBitmapFilter::DoExportBitmap null bitmap pointer specified");

	if (pOptions == NULL)
	{
		// Create a record of information about the export
		SetUpExportBitmapOptions(&pOptions, pBitmap, TRUE);
		if (pOptions == NULL)
			return FALSE;
	}

	// remember the old export options
	BitmapExportOptions *pOldOptions = GetBitmapExportOptions();
	if (pOldOptions != NULL)
		pOldOptions->RetrieveDefaults();
	
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
			TRACEUSER( "JustinF", _T("Tried to open non-CCDiskFile in BaseBitmapFilter::DoExportBitmap\n") );
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
	if (pOldOptions != NULL)
	{
		SetExportOptions(pOldOptions);
		pOldOptions->SetAsDefaults();
	}

	// restore the file
	OutputFile = pFile;

	return ok;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::SaveExportBitmapsToFile(CCLexFile* pFile, PathName* pPath, BitmapExportParam* pParam,
														   BOOL DontShowFileName = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/97
	Inputs:		pDiskFile - the file to put the exported data into
				pPath - the pathname of the file to be exported to
				pParam - the data to use when exporting the bitmaps
				DontShowFileName - if set to true then don't show the filename in the progress string
	Purpose:	Exports the bitmaps specified by the BitmapExportParam object to a multi-
				image capable filter. Each of the bitmaps will be saved to file using the
				settings specified.
				This is the baseclass version and so is usually overriden
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BaseBitmapFilter::SaveExportBitmapsToFile(CCLexFile* pFile, PathName* pPath, BitmapExportParam* pParam,
											   BOOL DontShowFileName)
{
#ifdef DO_EXPORT
	ERROR2IF(pFile == NULL || pPath == NULL, FALSE,"NULL Parameters");
	ERROR2IF(pParam == NULL,FALSE,"TI_GIFFilter::DoExportBitmaps null BitmapExportParam specified");

	ERROR3("BaseBitmapFilter::SaveExportBitmapsToFile calling baseclass version!");
	
	return TRUE;
#endif
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::ExportRenderNodes(RenderRegion *pRegion, ExportDC *pDC,
													 BOOL VisibleLayersOnly = FALSE,
													 BOOL CheckSelected = FALSE,
													 BOOL ShowProgress = TRUE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Inputs:		pRegion 		- the render region to export to.
				pDc				- device context to use
				VisibleLayersOnly - use visible layers or not
				ShowProgress	- TRUE then start up a progress bar or FALSE assume the caller
								  has done it.
	Returns:	TRUE if the export process completed successfully, FALSE if an error occured.
	Purpose:	Actually export the nodes to the given render region showing a progress bar
				as we go. Assumes everything has been set up by ExportRender.
				Overrides the baseclass version so that we can render in strips.
				Ignores the setting of ShowProgress as we must be in charge of the progress
				bar in this version. 
	SeeAlso:	ExportRender

********************************************************************************************/

BOOL BaseBitmapFilter::ExportRenderNodes(RenderRegion *pRegion, ExportDC *pDC,
										 BOOL VisibleLayersOnly, BOOL CheckSelected,
										 BOOL ShowProgress)
{
#ifdef DO_EXPORT
	ERROR2IF(pRegion==NULL,FALSE,"BaseBitmapFilter::ExportRender null render region supplied");

	// Set up the Accusoft variables used for its redenring loop and call back form of the
	// do next strip. This is just in case we are going to change from using the Bitmap
	// filter to using the Accusoft filters as we are saving in a format which the Bitmap
	// filters do not understand. 
	//WEBSTER-Martin-03/01/97
PORTNOTE("ExportAccusoft", "Removed use fo Accusoft filters")
#ifndef EXCLUDE_FROM_XARALX
	AccusoftFilters::SetRenderVariables(this, pRegion, pDC, VisibleLayersOnly, CheckSelected);
#endif

	// Set the bad rendering flag to its default value
	BadExportRender = FALSE;

	BOOL ok = TRUE;
	DocRect rClipRect = pRegion->GetRegionRect();
	
	BitmapExportOptions * pExportOptions = GetBitmapExportOptions();
	BOOL DidASwap = FALSE;
		
	// Check our class variable to see if we should render in strips or not
	// We do this so that things like the GIF filters 1bpp masked rendering can turn the
	// rendering in strips off. Should be on by default.
	if (RenderInStrips)
	{
		OurNumNodes = GetNumNodes();
		//UINT32 OurUpdateEvery = UpdateEvery;
		
		// We have a useful function in the GRenderBitmap class which tells us the number
		// of strips we are going to render with.
		GRenderBitmap *pGRenderBitmap = (GRenderBitmap*)pRegion;
		INT32 NumberOfStrips = pGRenderBitmap->GetNumBands();
		if (NumberOfStrips <= 0)
			NumberOfStrips = 1;
TRACEUSER( "Gerry", _T("Number of bands is %d\n"), NumberOfStrips);
TRACEUSER( "Gerry", _T("Number of nodes is %d\n"), OurNumNodes);

		// We need the progress bar to just move up once rather than for every strip	
		// plus include the number of lines in the export bitmap so that we can show
		// a progress bar for this part of the export. 
		// First, get the size of the bitmap we are going to export
		INT32 HeightInPixels = pGRenderBitmap->GetFullRegionHeight();
TRACEUSER( "Gerry", _T("Full region height in pixels is %d\n"), HeightInPixels );
		
		SizeOfExport = OurNumNodes * NumberOfStrips;
		SetNumNodes(SizeOfExport);

		BitmapExportOptions* pOptions = GetBitmapExportOptions();

		// Start a progress update going
		// Extend it by the pixel height of the export
		String_64 ExportMessage(GetExportMsgID());
		ExportMessage = GetExportProgressString(OutputFile, GetExportMsgID());
		BeginSlowJob(200, FALSE, &ExportMessage);
TRACEUSER( "Gerry", _T("Size of progress bar = %d\n"), SizeOfExport + HeightInPixels );

		// Reset this back to zero so that when we update the progress bar in WriteToFile
		// we can work out the current point reached on the progress bar
		SizeOfExport = 0;
		ProgressOffset = 0;
		m__StripStart = 0;
		
		// Find out which way we want to render by asking the filters
		BOOL BottomToTop = GetRenderBottomToTop();
		pGRenderBitmap->SetRenderBottomToTop(BottomToTop);

		// SMFIX this is where we will generate a new optimal palette if we need to
		BOOL CreatingPalette = !pExportOptions->IsPaletteValid() && pExportOptions->GetDepth() <= 8;
		if (CreatingPalette)
		{
			if (!pExportOptions->CreatePaletteOptimiser())
			{
				// we didn't create an optimiser as we already have a good one
				// use it to create the palette NOW! without the stats gathering part as are stats are still valid
				pExportOptions->CreateValidPalette();
				// let the optimiser fill in the palette in the export options
				AlterPaletteContents(pExportOptions->GetLogicalPalette());
				CreatingPalette = FALSE;
			}
		}
		else if(pExportOptions->GetDepth() <= 8)
		{
		// this call sets this palette in the lower reaches of the code from the palette held in the export options
			AlterPaletteContents(pExportOptions->GetLogicalPalette());
		}

		if (pExportOptions->IsCMYK())
		{
			// If we are doing a CMYK export then we need to do a really cunning 4-pass render

			TRACEUSER( "Gerry", _T("BaseBitmapFilter::ExportRenderNodes starting CMYK export\n") );

			// We need to save away the current ColourPlate and restore it afterwards
			// This may be problematic as the render region has already been created at this point
			// and also, the view will redraw when the ColourPlate is changed
			
			View* pView = pRegion->GetRenderView();
			ColourPlate* pCurPlate = pView->GetColourPlate();
			if (pCurPlate)
			{
				pCurPlate = new ColourPlate(*pCurPlate);	// Make a copy of it
			}
			else
			{
				pCurPlate = new ColourPlate;				// Make a default
				pCurPlate->SetMonochrome(NULL, FALSE);		// Set to non-mono
			}

			// Get the first strip ready for action
			ok = pRegion->SetFirstBand();
			if (!ok)
			{
				BadExportRender = TRUE;		// flag that something has gone wrong
				EndWriteToFile();
				EndSlowJob();
				return FALSE;		
			}

			UINT32 nBandsCompleted = 0;

			// Now render all the other strips, if there are any, until we have completed
			// the entire region.
			do
			{
				// Make sure this gets set back to the correct value before doing any rendering
				pGRenderBitmap->SetDoBitmapConversion(TRUE);
				
				// Set up a destination bitmap of the strip size at 32 bpp
				// We must somehow get the size of the bitmap here before
				// it has actually been allocated in StartRender
				WinRect BitmapRect = ((GRenderRegion*)pGRenderBitmap)->CalculateWinRect(pGRenderBitmap->GetClipRect());

				TRACEUSER( "Gerry", _T("CMYK bitmap is (%d, %d)\n"), BitmapRect.GetWidth(), BitmapRect.GetHeight() );
				LPBYTE pCMYKBits = NULL;
				LPBITMAPINFO pCMYKInfo = AllocDIB(BitmapRect.GetWidth(), BitmapRect.GetHeight(), 32, &pCMYKBits);
				memset(pCMYKBits, 0, BitmapRect.GetWidth() * BitmapRect.GetHeight() * sizeof(DWORD));

				// Loop around the four plates
				for (UINT32 PlateType = COLOURPLATE_CYAN; PlateType <= COLOURPLATE_KEY; PlateType += 1)
				{
					// Set the relevant ColourPlate on the View
					ColourPlate* pPlate = new ColourPlate((ColourPlateType)PlateType, TRUE, FALSE);
					pView->SetColourPlate(pPlate, FALSE);
					delete pPlate;

					// This will cause a new ColourContext to be got from the view
					pRegion->ResetColourContext();

					// Call the base class version to do the actual node exporting
					// We have started the progress bar so tell it not to
					ok = Filter::ExportRenderNodes(pRegion, pDC, VisibleLayersOnly, CheckSelected, FALSE);
					// If a problem happened then get out quick.
					// Assume function has stopped the rendering.
					if (!ok)
					{
						if (pView)
						{
							pView->SetColourPlate(pCurPlate);
							delete pCurPlate;
						}
						BadExportRender = TRUE;		// flag that something has gone wrong
						EndWriteToFile();
						EndSlowJob();
						return FALSE;		
					}

					LPBITMAPINFO pRenderInfo = NULL;
					LPBYTE pRenderData = NULL;
					pGRenderBitmap->GetBitmapData(&pRenderInfo, &pRenderData, FALSE);
					TRACEUSER( "Gerry", _T("Strip bitmap for plate %d is (%d, %d)\n"), PlateType, pRenderInfo->bmiHeader.biWidth, pRenderInfo->bmiHeader.biHeight);
					BYTE* pSrcBits = pRenderData;
					UINT32 NumPixels = pRenderInfo->bmiHeader.biWidth * pRenderInfo->bmiHeader.biHeight;
					BYTE* pDestBits = ((BYTE*)pCMYKBits) + PlateType - COLOURPLATE_CYAN;	// Pointer arithmetic
					for (UINT32 Pixel = 0; Pixel < NumPixels; Pixel++)
					{
						*pDestBits = (255 - pSrcBits[0]) * (255 - pSrcBits[3]) / 255;
						pDestBits += 4;
						pSrcBits += 4;
					}
				}

				// Update our size of export variable
				SizeOfExport += OurNumNodes;
				// Update the filters stored progress offset value
				ProgressOffset += OurNumNodes;
				TRACEUSER( "Gerry", _T("BaseBitmapFilter::RenderNextStrip ProgressOffset = %d\n"), ProgressOffset);
							
				// Now we need to get WriteFrame to output the composited bitmap
				// The easiest way to do this will be to replace the bitmap in the render region
				// with the composited one and just call WriteFrame as normal
				DWORD* pSrcBits = (DWORD*)pGRenderBitmap->Get32BitRGBQuadData();
				UINT32 NumPixels = pGRenderBitmap->GetSizeOfRGBQuadData();
				memcpy(pSrcBits, pCMYKBits, NumPixels * sizeof(DWORD));

				// We need to stop GetBitmapData from doing another conversion when reading the CMYK data
				pGRenderBitmap->SetDoBitmapConversion(FALSE);

				// Free the temp bitmap
				FreeDIB(pCMYKInfo, pCMYKBits);

				// Save that data out to file, this should update SizeOfExport with the
				// current strip size
				// Note: it will also render all the remaining strips
				ok = WriteFrame();
				// If a problem happened then get out quick.
				if (!ok)
				{
					if (pView)
					{
						pView->SetColourPlate(pCurPlate);
						delete pCurPlate;
					}
					BadExportRender = TRUE;		// flag that something has gone wrong
					EndWriteToFile();
					EndSlowJob();
					return FALSE;		
				}

				// Advance the progress bar
				UINT32 nProgress = UINT32((double(nBandsCompleted) / double(NumberOfStrips)) * 200.0);
				TRACEUSER( "Gerry", _T("nProgress = %d\n"), nProgress);

				if ( !ContinueSlowJob (nProgress) )
				{
					// Error; close down and exit nicely.
					// Must set an error otherwise we will get the dreaded reporting error
					// when none set message.
					// If no error message ID set in this filter then use default

					// Andy, 13-12-00
					// N.B. At the moment, things go pear-shaped if we press ESC during export.
					// e.g. subsequent presses of 'preview' do not generate a preview.
					// However it wasn't exactly well behaved before, e.g. sometimes gave
					// internal error 'Failed to create an intermediate bitmap pointer!', or
					// access violated if the user clicked 'background transparency' after a
					// cancelled export.
					if (pView)
					{
						pView->SetColourPlate(pCurPlate);
						delete pCurPlate;
					}
					if ( StopExportMsgID == 0 )
						Error::SetError ( _R(IDW_CANCELEXPORT) );
					else
						Error::SetError ( StopExportMsgID );
					pRegion->StopRender ();
					if ( ShowProgress )
						EndSlowJob ();
					return FALSE;
				}

				nBandsCompleted ++;

				// Check if there are any more bands
			} while (pRegion->GetNextBand());

			if (pView)
			{
				pView->SetColourPlate(pCurPlate);
				delete pCurPlate;
			}
		}
		else
		{
			for (INT32 loop = 0; loop < 2; loop++) // never do this more than twice once for the palette and once to write it out to file
			{
				TRACEUSER( "Gerry", _T("BaseBitmapFilter::ExportRenderNodes loop = %d\n"), loop);

				if (!CreatingPalette && pExportOptions->GetDepth() <= 8)
				{
					DidASwap = pExportOptions->SwapEditedColoursInLogicalPalette();
					AlterPaletteContents(pExportOptions->GetLogicalPalette());
				}

				// Get the first strip ready for action
				ok = pRegion->SetFirstBand();
				if (!ok)
				{
					BadExportRender = TRUE;		// flag that something has gone wrong
					EndWriteToFile();
					EndSlowJob();
					return FALSE;		
				}

				UINT32 nBandsCompleted = 0;

				// Now render all the other strips, if there are any, until we have completed
				// the entire region.
				do
				{
					// Call the base class version to do the actual node exporting
					// We have started the progress bar so tell it not to
					ok = Filter::ExportRenderNodes(pRegion, pDC, VisibleLayersOnly, CheckSelected, FALSE);
					// If a problem happened then get out quick.
					// Assume function has stopped the rendering.
					if (!ok)
					{
						BadExportRender = TRUE;		// flag that something has gone wrong
						EndWriteToFile();
						EndSlowJob();
						return FALSE;		
					}

					if (!CreatingPalette)
					{
						// Update our size of export variable
						SizeOfExport += OurNumNodes;
						// Update the filters stored progress offset value
						ProgressOffset += OurNumNodes;
						TRACEUSER( "Gerry", _T("BaseBitmapFilter::RenderNextStrip ProgressOffset = %d\n"),ProgressOffset);
					}
								
					// Save that data out to file, this should update SizeOfExport with the
					// current strip size
					if (CreatingPalette)
					{
						// calc the stats for an optimised palette
						pExportOptions->GatherPaletteStats(	((GRenderRegion * )pRegion)->Get32BitRGBQuadData(),
											((GRenderRegion * )pRegion)->GetSizeOfRGBQuadData());
					}
					else
						ok = WriteFrame();
					// If a problem happened then get out quick.
					if (!ok)
					{
						BadExportRender = TRUE;		// flag that something has gone wrong
						EndWriteToFile();
						EndSlowJob();
						return FALSE;		
					}

					// Advance the progress bar
					UINT32 nProgress;
					// if this image doesn't need a palette, scale the progress value to
					// the full length of the bar
					if (!CreatingPalette && loop==0)
						nProgress = UINT32((double(nBandsCompleted) / double(NumberOfStrips)) * 200.0);
					// otherwise, scale the progress value to half the length of the bar
					// (as this rendering loop will occur twice)
					else
						nProgress = UINT32((double(nBandsCompleted) / double(NumberOfStrips)) * 100.0 + double(loop)*100.0);
					TRACEUSER( "Gerry", _T("nProgress = %d\n"), nProgress);

					if ( !ContinueSlowJob (nProgress) )
					{
						// Error; close down and exit nicely.
						// Must set an error otherwise we will get the dreaded reporting error
						// when none set message.
						// If no error message ID set in this filter then use default

						// Andy, 13-12-00
						// N.B. At the moment, things go pear-shaped if we press ESC during export.
						// e.g. subsequent presses of 'preview' do not generate a preview.
						// However it wasn't exactly well behaved before, e.g. sometimes gave
						// internal error 'Failed to create an intermediate bitmap pointer!', or
						// access violated if the user clicked 'background transparency' after a
						// cancelled export.
						if ( StopExportMsgID == 0 )
							Error::SetError ( _R(IDW_CANCELEXPORT) );
						else
							Error::SetError ( StopExportMsgID );
						pRegion->StopRender ();
						if ( ShowProgress )
							EndSlowJob ();
						return FALSE;
					}

					nBandsCompleted ++;

					// Check if there are any more bands
				} while (pRegion->GetNextBand());
			
				if (CreatingPalette)
				{
					CreatingPalette = FALSE;
					// let the optimiser fill in the palette in the export options
					pExportOptions->CreateValidPalette();
					// set this as the palette being used in output dib
					AlterPaletteContents(pExportOptions->GetLogicalPalette());
					// Reset the clip region to what it was when we started this
					pRegion->SetClipRect(rClipRect);
				}
				else
					break; // leave this for loop now
			}
		}

		if (DidASwap)
		{
			pOptions->SwapEditedColoursInLogicalPalette();
			AlterPaletteContents(pOptions->GetLogicalPalette());
		}
		// Close down progress display
		// Do it before ending as then the TI GIF filter can show a progress bar on
		// its possible masked rendering for transparency and export of the GIF data
		// which happens in the EndWriteToFile.
		EndSlowJob();
	}
	else
	{
		// Call the base class version to do the actual node exporting
		// Use the pass in version to say whether we need to start the progress bar or not.
		// Assume that we are the GIF filter renderer and so do not try and output the data
		// at all. We will ensure as this might be dangerous.
PORTNOTE("filter", "Removed use of GIFFilter")
#ifndef EXCLUDE_FROM_XARALX
		ERROR3IF(!(this->IS_KIND_OF(TI_GIFFilter)) && !(this->IS_KIND_OF(PNGFilter)),
				 "BaseBitmapFilter::ExportRenderNodes rendering in strips off");
#else
		ERROR3IF(!(this->IS_KIND_OF(PNGFilter)),
				 "BaseBitmapFilter::ExportRenderNodes rendering in strips off");
#endif

		ok = Filter::ExportRenderNodes(pRegion, pDC, VisibleLayersOnly,
									   CheckSelected, ShowProgress);

		// SMFIX optimise the palette in the no strip based manner
		BOOL CreatingPalette = !pExportOptions->IsPaletteValid();
		if (CreatingPalette)
		{
			// create a palette optimiser (or retrieve a valid one)
			if (pExportOptions->CreatePaletteOptimiser())
			{
				// we need to gather stats as we have made a virgin new optimiser
				pExportOptions->GatherPaletteStats(	((GRenderRegion * )pRegion)->Get32BitRGBQuadData(),
									((GRenderRegion * )pRegion)->GetSizeOfRGBQuadData());
			}

			// let the optimiser fill in the palette in the export options
			pExportOptions->CreateValidPalette();
			// let the optimiser fill in the palette in the export options
			AlterPaletteContents(pExportOptions->GetLogicalPalette());
		}

/*		// SMFIX
		if(pRegion->m_DoCompression)
		{
			BITMAPINFO* pBMPInfo = NULL;
			BYTE* pBMPData = NULL;
			((GRenderBitmap*)pRegion)->GetBitmapData(&pBMPInfo,&pBMPData);
			((GRenderBitmap*)pRegion)->GetDrawContext()->ConvertBitmap(&pBMPInfo->bmiHeader,pBMPData,&pBMPInfo->bmiHeader,pBMPData,0);
		}*/
	}

	// return outcome to caller
	return ok;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL BaseBitmapFilter::WriteToFile( BOOL AtEnd)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Inputs:		AtEnd is TRUE if this is the last chunk of the file, FALSE if not.
	Purpose:	Writes out the bitmap to a file. Inherited classes override this to write
				in different file formats.
				AtEnd is ignored now and should always be set to TRUE.
				
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BaseBitmapFilter::WriteToFile( BOOL AtEnd)
{
	ENSURE(FALSE, "Base class bitmap filter WriteToFile called");
	return FALSE;
}

/********************************************************************************************

>	BOOL BaseBitmapFilter::EndWriteToFile( )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Inputs:		-
	Purpose:	Cleans up after writing the bitmap data out to a file. Inherited classes
				override this to write in different file formats.
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BaseBitmapFilter::EndWriteToFile( )
{
	ENSURE(FALSE, "Base class bitmap filter EndWriteToFile called");
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:		Pointer to the bitmap to be exported.
	Returns:	TRUE if worked, FALSE if errored.
	Purpose:	Physically put the bitmap into the disk.  Inherited classes override this to write
				in different file formats.
	SeeAlso:	WriteDataToFile(); AccusoftFilters::WriteToFile; AccusoftFilters::WriteDataToFile;

********************************************************************************************/

BOOL BaseBitmapFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi)
{
	ERROR3("Base class bitmap filter WriteBitmapToFile called");
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, BaseCamelotFilter *pFilter,
													 CCLexFile *pFile, INT32 Compression)
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
				This is the baseclass version and hence needs overiding, hence the error.
	SeeAlso:	BitmapListComponent::SaveBitmapDefinition;

********************************************************************************************/

BOOL BaseBitmapFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, BaseCamelotFilter *pFilter,
										 CCLexFile *pFile, INT32 Compression)
{
	// Create a record of information about the export
	m_pExportOptions = CreateExportOptions();
	if (GetBitmapExportOptions() == NULL)
	{
		return FALSE;
	}

	OutputFile = pFile;

	return TRUE;
}

/********************************************************************************************

>	static	DocRect	BaseBitmapFilter::GetSizeOfDrawing(Spread *pSpread=NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Inputs:		pointer to the spread to use
	Purpose:	Works out the size of the rectangle encompassing the drawing.
				Code also used to work out the pixel size in the bitmap export options
				dialog box.

				Added by Graham 11/4/97: If pSpread is NULL, we get the first
				spread of the current document to use

	Returns:	Size of the rectangle encompassing the drawing.
	SeeAlso:	BmpPrefsDlg::RecalculateSize; BaseBitmapFilter::DoExport;

********************************************************************************************/

DocRect	BaseBitmapFilter::GetSizeOfDrawing(Spread *pSpread)
{
	// Start out with an empty clip rect
	DocRect SpreadRect;
	SpreadRect.MakeEmpty();

	//If we have not been passed a spread...
	if (pSpread == NULL)
	{
		//Then let's get the first spread of the current document
		Document* pdocCurrent=Document::GetCurrent();
		ERROR2IF(pdocCurrent==NULL, DocRect(0,0,0,0), "BaseBitmapFilter - no current document");

		//And finally the current spread
PORTNOTE("spread", "Multi-spread warning!")
		pSpread=GetFirstSpread(pdocCurrent);
		ERROR2IF(pSpread==NULL, DocRect(0,0,0,0), "BaseBitmapFilter - no spread");
	}

	// We need to take into account the fact that some layers may not be visible
	Layer* pLayer = pSpread->FindFirstLayer(); 
	while (pLayer != NULL)
	{
		// We've got a layer so check to see if it is visible
		// Added 8/4/97 extra checks to see if we are the special layers
		// and so shouldn't be included in the bounding calulations
		if (pLayer->IncludeLayerInBoundingCalcs())
		{
			// This one is visible, so union it in
			DocRect LayerRect = pLayer->GetBoundingRect();
			SpreadRect = SpreadRect.Union(LayerRect);
		}

		// Get the next sibling layer
		pLayer = pLayer->FindNextLayer(); 	
	}

	// return the found rectangle to the caller. 	
	return SpreadRect;
}

/********************************************************************************************

>	static	DocRect	BaseBitmapFilter::GetSizeOfSpread(Spread *pSpread)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Inputs:		pointer to the spread to use
	Purpose:	Works out the size of the rectangle encompassing the spread.
				Code also used to work out the pixel size in the bitmap export options
				dialog box.
	Returns:	Size of the rectangle encompassing the spread.
	SeeAlso:	BmpPrefsDlg::RecalculateSize; BaseBitmapFilter::PrepareToExport;

********************************************************************************************/

DocRect	BaseBitmapFilter::GetSizeOfSpread(Spread *pSpread)
{
	// Start out with an empty clip rect
	DocRect SpreadRect;
	SpreadRect.MakeEmpty();

	if (pSpread == NULL)
	{
		ERROR3("BaseBitmapFilter::GetSizeOfSpread null spread");
		return SpreadRect;
	}

	// build a rectangle out of all the pages in the selected spread
	Node *pPage = pSpread->FindFirstPageInSpread(); 
	ERROR3IF(pPage == NULL, "Spread has no pages");

	while (pPage)
	{
		if (pPage->GetRuntimeClass() == CC_RUNTIME_CLASS(Page) )
			SpreadRect = SpreadRect.Union(( (Page*)pPage)->GetPageRect() );

		pPage = pPage->FindNext();
	}

	ERROR3IF(SpreadRect.IsEmpty(),"Current spread has no pages");

	return SpreadRect;
}	 

/********************************************************************************************

>	static	BOOL	BaseBitmapFilter::GetSizeOfBitmap(INT32 *PixelWidth, INT32 *PixelHeight)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:		-
	Outputs:	PixelWidth	bitmap width in pixels
				PixelHeight	bitmap height in pixels
	Purpose:	Works out the size of the bitmap in the bitmap export options
				dialog box.
	Returns:	True if completed ok.
	SeeAlso:	BmpPrefsDlg::RecalculateSize; BaseBitmapFilter::PrepareToExport;

********************************************************************************************/
BOOL	BaseBitmapFilter::GetSizeOfBitmap(INT32 *PixelWidth, INT32 *PixelHeight)
{
#ifdef DO_EXPORT
	ERROR2IF(pExportBitmap==NULL,FALSE,"BaseBitmapFilter::GetSizeOfBitmap no bitmap");
	ERROR2IF(PixelWidth==NULL,FALSE,"BaseBitmapFilter::GetSizeOfBitmap no PixelWidth");
	ERROR2IF(PixelHeight==NULL,FALSE,"BaseBitmapFilter::GetSizeOfBitmap no PixelHeight");

	// Set return values in case of early exit
	*PixelWidth = 0;
	*PixelHeight = 0;

	// Get a pointer to the actual bitmap so that we can get some details from it.
	OILBitmap *pOilBitmap = pExportBitmap->ActualBitmap;
	ERROR3IF(pOilBitmap == NULL,"BaseBitmapFilter::GetSizeOfBitmap null oil bitmap pointer");
	if (pOilBitmap)
	{
		BitmapInfo BmInfo;
		pOilBitmap->GetInfo(&BmInfo);
		*PixelWidth  = BmInfo.PixelWidth;
		*PixelHeight = BmInfo.PixelHeight;
	}

	return TRUE;
#else
	return FALSE;
#endif
} 

/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::IsThisBppOk(UINT32 Bpp)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/95
	Inputs:		Bpp or Colour depth.
	Returns:	TRUE if this filter can cope with this colour depth, FALSE otherwise.
	Purpose:	Check if this Bitmap filter can cope with saving at this Bpp/Colour depth.
	SeeAlso:	OpMenuExport::DoWithParam;

********************************************************************************************/

BOOL BaseBitmapFilter::IsThisBppOk(UINT32 Bpp)
{
	// Base class version so always return FALSE.
	return FALSE;
}

/********************************************************************************************

>	virtual UINT32 BaseBitmapFilter::GetExportMsgID()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/95
	Returns:	The id of the message to put on the progress display whilst exporting.
	Purpose:	Used to get the message id to be used during export.
				Virtual, so that two stage exporters can change the message.
	SeeAlso:	DoExport;

********************************************************************************************/

UINT32 BaseBitmapFilter::GetExportMsgID()
{
	if (GeneratingOptimisedPalette())
		return _R(IDS_GENOPTPALMSGID);			// "Generating optimised palette..."
	else
		return Filter::GetExportMsgID();
}

/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::GenerateOptimisedPalette(Spread *pSpread, UINT32 Depth, double DPI, BOOL SnapToBrowserPalette, UINT32 NumColsInPalette = 0, BOOL UsePrimaryCols = TRUE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pSpread				= ptr to spread
				Depth				= The BPP of the bitmap
				DPI					= the dpi of the bitmap
				NumColsInPalette	= prefered number of cols in palette (0 = select maximum allowed)
									  (only applies when Depth == 8)
				UsePrimaryCols		= Put primary colours in palette
									  (only applies when Depth == 8)
				SnapToBrowserPalette= TRUE if the palette should be snapped to the browser palette after generation

	Returns:	TRUE if Palette generated ok
	Purpose:	Generates an optimised palette to use during export
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBitmapFilter::GenerateOptimisedPalette(Spread *pSpread, UINT32 Depth, double DPI, BOOL SnapToBrowserPalette, UINT32 NumColsInPalette, BOOL UsePrimaryCols)
{
#ifdef DO_EXPORT
	if (Depth > 8)
	{
		pOptimisedPalette = NULL;
		return TRUE;
	}

	TRACEUSER( "Neville", _T("Trying to generate an optimised palette ...\n") );

	WeAreGeneratingOptPalette = TRUE;

	// Don't use rendering matrix when exporting BMPs as it uses coordinates as is
	Matrix Identity;

	// Don't use view scale; set to 1
	FIXED16 Scale(1);

	// Only use special 8bpp palette if the UsePrimaryColours flag is TRUE
	BOOL use8bpp = ((Depth == 8) && UsePrimaryCols);
	GRenderOptPalette* pPalRegion = new GRenderOptPalette(ClipRect, Identity, Scale, 32, DPI, use8bpp);
	if (pPalRegion)
	{
		// we need a View to Attach to, so that:
		//		default Quality setting
		//		CalcPixelWidth etc get called

		DocView *View = DocView::GetCurrent();
		if (View)
		{
			// Attach to the right device. (note no DC)
			pPalRegion->AttachDevice(View, NULL, pSpread);
		}
		else
			ERROR2(FALSE,"BaseBitmapFilter::GenerateOptimisedPalette no current view");

		RenderInStrips = TRUE;

		// Now do an export render pass, using the Palette Region
		if (ExportRender(pPalRegion))
		{
			UINT32 MaxColours = 1U<<Depth; // let's not use floating point here - UINT32(pow(2,Depth));

			if (NumColsInPalette < 1)
				NumColsInPalette = MaxColours;

			if (use8bpp && NumColsInPalette < 20)
				NumColsInPalette = 20;

			if (NumColsInPalette > MaxColours)
				NumColsInPalette = MaxColours;

			// All went ok, so extract the Optimised palette from the Region
			pOptimisedPalette = pPalRegion->GetOptimisedPalette(MaxColours, NumColsInPalette, GetNumReservedColours(), SnapToBrowserPalette);
			
			if (pOptimisedPalette) TRACEUSER( "Neville", _T("Blimey. We got an optimised palette to use.\n") );
		}

		delete pPalRegion;
	}

	WeAreGeneratingOptPalette = FALSE;
#endif
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::GenerateOptimisedPalette(Spread *pSpread, BitmapExportOptions *pOptions, BOOL SnapToBrowserPalette)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/97
	Inputs:		pSpread				 = ptr to spread
				pOptions			 = ptr to bitmap export options
				SnapToBrowserPalette = If TRUE, snap all optimised palette entries to the browser palette

	Returns:	TRUE if Palette generated ok
	Purpose:	Generates an optimised palette to use during export

				This is an alternative interface to GenerateOptimisedPalette(Spread *pSpread, UINT32 Depth, double DPI, UINT32 NumColsInPalette, BOOL UsePrimaryCols);

				WEBSTER - markn 16/1/97
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBitmapFilter::GenerateOptimisedPalette(Spread *pSpread, BitmapExportOptions *pOptions, BOOL SnapToBrowserPalette)
{
	if (pSpread != NULL && pOptions != NULL)
	{
		return GenerateOptimisedPalette(pSpread,
										pOptions->GetDepth(), 
										pOptions->GetDPI(),
										SnapToBrowserPalette,
										pOptions->GetNumColsInPalette(),
										pOptions->GetUseSystemColours()
										);
	}

	return FALSE;
}

void BaseBitmapFilter::AlterPaletteContents( LPLOGPALETTE pPalette )
{
	return;
}

/********************************************************************************************

>	static BOOL BaseBitmapFilter::GeneratingOptimisedPalette()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		-
	Returns:	TRUE if we are in the middle of generating an optimised Palette
	Purpose:	Find out if we are currently generating an optimised palette or not
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBitmapFilter::GeneratingOptimisedPalette()
{
	return WeAreGeneratingOptPalette;
}


/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::WriteFileHeader(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out the file specific header data
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBitmapFilter::WriteFileHeader(void)
{
	return(TRUE);
}


/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::WritePreFrame(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out any frame specific info before the image
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBitmapFilter::WritePreFrame(void)
{
	return(TRUE);
}


/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::WriteFileHeader(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out the image itself
				This base class version actually calls the WriteToFile() function so that
				derived classes do not have to implement any of the multi-image stuff
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBitmapFilter::WriteFrame(void)
{
	return(WriteToFile(TRUE));
}


/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::WritePostFrame(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out any frame specific info after the image
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBitmapFilter::WritePostFrame(void)
{
	return(TRUE);
}


/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::WriteFileEnd(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out the file specific data at the end of the file
				This base class version calls EndWriteToFile() so that derived classes
				do not have to implement the multi-image stuff
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBitmapFilter::WriteFileEnd(void)
{
	return(EndWriteToFile());
}


/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::WritePreSecondPass(void)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	Called before the second pass of a two pass export so the filter can reset
				itself
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBitmapFilter::WritePreSecondPass(void)
{
	return(TRUE);
}


/********************************************************************************************

>	virtual BOOL BaseBitmapFilter::WritePostOptimisedPalette(void)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	Called after the optimised palette has been generated but before the export 
				of the bitmap.
	SeeAlso:	-

********************************************************************************************/

BOOL BaseBitmapFilter::WritePostOptimisedPalette(void)
{
	return(TRUE);
}


/********************************************************************************************
>	UINT32 BaseBitmapFilter::GetNumReservedColours()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		-
	Returns:	The numbers of colours the filter would like to reserve.  The base class
				reserves zero colours.
	Purpose:	Override this to leave spaces in the palette.  eg the GIF filter reserves an
				index for the transparency mask colour.
	SeeAlso:	-
********************************************************************************************/
UINT32 BaseBitmapFilter::GetNumReservedColours()
{
	return 0;
}


/********************************************************************************************

>	CCLexFile*	BaseBitmapFilter::GetExportFile() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	A pointer to the file provided in the filter entry point (DoExport,etc.)
	Purpose:	Support function to obtain file to export in WriteBitmapToFile

********************************************************************************************/
CCLexFile*	BaseBitmapFilter::GetExportFile() const
{
	return OutputFile;
}


/********************************************************************************************

>	BOOL BaseBitmapFilter::GetCurrentStripInfo(	ADDR* ppBits, 
											BMP_SIZE* pWidth, BMP_SIZE* pHeight,
											BMP_DEPTH* pDepth) const
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Outputs:	ppBits : address of ADDR variable to accept pointer to bitmap bits
				pWidth : address of BMP_SIZE variable to accept bitmap width
				pHeight: address of BMP_SIZE variable to accept bitmap height
				pDepth : address of BMP_DEPTH variable to accept bitmap depth
	Returns:	TRUE if data is valid
				FALSE otherwise
	Purpose:	Support function to obtain information regarding the strip we are currently
				exporting.
	Notes:		This information is only valid in the WriteFrame member

********************************************************************************************/
BOOL BaseBitmapFilter::GetCurrentStripInfo(	ADDR* ppBits, 
											BMP_SIZE* pWidth, BMP_SIZE* pHeight,
											BMP_DEPTH* pDepth) const
{
	LPBITMAPINFO	pBitmapInfo;
	LPBYTE			pBitmapBits;

	ExportRegion->GetBitmapData(&pBitmapInfo, &pBitmapBits);

	if (pBitmapInfo == NULL || pBitmapBits == NULL)
	{
		ERROR3("pBitmapInfo == NULL || pBitmapBits == NULL");
		return FALSE;
	}

	*ppBits		= pBitmapBits;
	*pWidth		= pBitmapInfo->bmiHeader.biWidth;
	*pHeight	= pBitmapInfo->bmiHeader.biHeight;
	*pDepth		= pBitmapInfo->bmiHeader.biBitCount;
	return TRUE;
}


/********************************************************************************************

>	BitmapExportOptions* BaseBitmapFilter::GetBitmapExportOptions() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	Details of the data to be exported
	Purpose:	Support function to obtain BitmapExportOptions

********************************************************************************************/
BitmapExportOptions* BaseBitmapFilter::GetBitmapExportOptions() const
{
	ERROR3IF(m_pExportOptions != NULL && !m_pExportOptions->IS_KIND_OF(BitmapExportOptions),
			"m_pExportOptions isn't");
	return m_pExportOptions;
}


/********************************************************************************************

>	BOOL BaseBitmapFilter::SetExportOptions ( BitmapExportOptions*	pOptions )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	TRUE if successful
				FALSE otherwise
	Purpose:	To allow that f!?@$%g MakeBitmapFilter have its wicked way

********************************************************************************************/

BOOL BaseBitmapFilter::SetExportOptions ( BitmapExportOptions*	pOptions )
{
	ERROR3IF ( pOptions != NULL && !pOptions->IS_KIND_OF ( BitmapExportOptions ),
			   "BitmapExportOptions is wrong kind" );

	// Don't delete the old options unless specifically requested. This is to avoid the nasty
	// access violations that might occur otherwise.

	//if (m_pExportOptions && pOptions != m_pExportOptions)
	//	delete m_pExportOptions;

	m_pExportOptions = pOptions;

	return TRUE;
}


/********************************************************************************************

>	virtual BitmapExportOptions* BaseBitmapFilter::CreateExportOptions() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	A pointer to a new BitmapExportOptions class
	Purpose:	Allows derived classes to override this function to provide their own class
				derived from BitmapExportOptions containing filter specific information.
	Notes:		This should never be called

********************************************************************************************/
BitmapExportOptions* BaseBitmapFilter::CreateExportOptions() const
{
	TRACE( _T("BaseBitmapFilter::CreateExportOptions() called\n"));

	BitmapExportOptions* pOptions = new BitmapExportOptions(_R(IDD_EXPORTBMPOPTS), FilterType(FilterID), &FilterName);

	return pOptions;
}


/********************************************************************************************

>	static LPLOGPALETTE BaseBitmapFilter::Create8bppPalette() const

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/97
	Returns:	A pointer to a 8bpp palette
	Purpose:	Creates an 8bpp palette used when generating an 8bpp bitmap
				Centralises common code previously found in BMP, GIF & PNG filters

				The palette returned is created using CCMalloc().  The caller should use
				CCFree() to discard the returned palette object when no longer needed.

				Introduced for WEBSTER - markn 8/2/97.
				Made static so that the Accusoft filter can use it
	Notes:		

********************************************************************************************/

LPLOGPALETTE BaseBitmapFilter::Create8bppPalette()
{
	/*
	LPLOGPALETTE lpPalette = NULL;
	GDrawContext *GDC = GRenderRegion::GetDrawContext();
	if (GDC != NULL)
	{
		// This should return a palette plus tell it what dithering we require
		// 0 = dither, not 0 for no dithering.
		LPLOGPALETTE lpGavPalette = GDC->SelectPalette( 0 );

		if (lpGavPalette != NULL)
		{
			const size_t TotalPal = sizeof(LOGPALETTE) + ( sizeof(PALETTEENTRY) * 256 );
			lpPalette = (LPLOGPALETTE)CCMalloc( TotalPal );
			if (lpPalette != NULL)
			{
				// Take a copy of that palette
				memcpy( lpPalette, lpGavPalette, TotalPal );

				// Always make 'browser' palette instead of 'standard' palette
				// - Harrison 8/10/97
				PaletteManager::MakePaletteBrowserCompatible(lpPalette,TRUE);
			}
		}
	}

	ERROR3IF(lpPalette == NULL, "Didnt get a palette");

	//  The section of code below is used when the user has deleted 1 or more colours, and then
	//  clicked on 'Preview'.
	//  Have to change the palette here before it is used to decide which palette colours
	//  match each pixel in the image.
	if( ( BmapPrevDlg::GetNumberOfDeletedColours() ) && ( BmapPrevDlg::m_bUseExistingPalette ) )
	{
		BOOL Deleted = FALSE;
		INT32 NewIndex = -1;
		INT32 Popularity = 0;

		//  Go through the palette in BmapPrevDlg.
		for( INT32 PaletteIndex = 0; PaletteIndex < ( lpPalette->palNumEntries ); PaletteIndex++ )
		{
			//  Has this colour been deleted by the user?
			Deleted = BmapPrevDlg::IsThisColourDeleted( PaletteIndex );
			if( Deleted )
			{
				//  If this colour has been deleted, then we need to get its
				//  original palette order.
				NewIndex = BmapPrevDlg::GetOriginalPaletteOrder( PaletteIndex );
				//  Set flags to 255 to tell the system that this colour is not
				//  to be used in generating the exported image.
				lpPalette->palPalEntry[NewIndex].peFlags = 255;
			}
			else
			{
				//  Not deleted, but is the popularity of this colour = 0?
				Popularity = BmapPrevDlg::GetPopularityValueForColour( PaletteIndex );
				if( Popularity <= 0 )
				{
					NewIndex = BmapPrevDlg::GetOriginalPaletteOrder( PaletteIndex );
					lpPalette->palPalEntry[NewIndex].peFlags = 255;
				}
			}
		}
	}

	ExtendedPalette LockedColoursInformation;
	if( BmapPrevDlg::GotLockedColours() && lpPalette)
	{
		//  Copy all the locked colours information over.
		LockedColoursInformation = BmapPrevDlg::m_LockedColoursPalette;
		
		ExtendedPalette LockedColoursInformation2 = BmapPrevDlg::m_LockedColoursPalette2;

		//  Go through all the locked colours that we have
		for( INT32 i = 0; i < LockedColoursInformation.NumberOfColours; i++ )
		{
			//  Make sure that we are not exceeding the number of colours in the current palette.
			if( i >= ( lpPalette->palNumEntries ) )
				continue;

			//  We want to find the nearest colour in the current palette to this locked colour.
			double SmallestDistance = 0.0;
			BOOL FirstDistanceCalc = TRUE;
			INT32 ClosestColourIndex = 0;

			//  Some variables needed below
			INT32 Red1, Green1, Blue1;
			INT32 Red2, Green2, Blue2;
			double RedDistance, GreenDistance, BlueDistance;
			double Distance;			

			INT32 LockedIndex = LockedColoursInformation.Data[ i ].ExtraInformation;
			Red1	= LockedColoursInformation2.Data[ LockedIndex ].Red;
			Green1	= LockedColoursInformation2.Data[ LockedIndex ].Green;
			Blue1	= LockedColoursInformation2.Data[ LockedIndex ].Blue;

			//  Go through all the colours in the current palette.
			for( INT32 j = 0; j < lpPalette->palNumEntries; j++ )
			{
				//  The components of the palette colour we are looking at.
				Red2	= lpPalette->palPalEntry[j].peRed;
				Green2	= lpPalette->palPalEntry[j].peGreen;
				Blue2	= lpPalette->palPalEntry[j].peBlue;

				//  Calculate the distance between each of the colour components
				//  ( Actually the square of this value - this does not matter since we
				//  are not interested in the absolute values, only relative ones ).
				RedDistance	  = pow( double( Red1 - Red2 ), 2 );
				GreenDistance = pow( double( Green1 - Green2 ), 2 );
				BlueDistance  = pow( double( Blue1 - Blue2 ), 2 );

				//  The overall distance.
				Distance = ( INT32 )( RedDistance + GreenDistance + BlueDistance );

				if( FirstDistanceCalc )
				{
					// First time around, so 'Distance' must be closest yet
					// However, there may be an occasion where this colour is already being used by one of 
					// the existing locked colours. In this case, we just want to completely ignore this
					// colour, and carry on with the next loop
					BOOL BeingUsed = FALSE;
					for( INT32 Index = 0; Index < i; Index++ )
					{
						if( LockedColoursInformation.Data[ Index ].ExtraInformation == j )
							BeingUsed = TRUE;
					}
					//  If it is being used, don't do anything.
					if( !BeingUsed )
					{
						FirstDistanceCalc	= FALSE;
						SmallestDistance	= Distance;
						ClosestColourIndex	= j;
					}
				}
				else if( Distance <= SmallestDistance )
				{
					//  Smallest distance so far?  if so remember the distance & index
					//  Do this only if this index is not being used by another locked colour
					//  already. To do this, have to look through the currently assigned locked
					//  colour indices. If this index is being used, then just do nothing.
					BOOL BeingUsed = FALSE;
					for( INT32 Index = 0; Index < i; Index++ )
					{
						if( LockedColoursInformation.Data[ Index ].ExtraInformation == j )
							BeingUsed = TRUE;
					}
					if( !BeingUsed )
					{
						SmallestDistance	= Distance;
						ClosestColourIndex	= j;
					}
				}
			}

			//  Put the colour index into the locked colour palette
			INT32 Information = LockedColoursInformation.Data[ i ].ExtraInformation;

			//  Update the palette which holds the original values of the locked colours.
			INT32 r = BmapPrevDlg::m_LockedColoursPalette2.Data[ ClosestColourIndex ].Red;
			INT32 g = BmapPrevDlg::m_LockedColoursPalette2.Data[ ClosestColourIndex ].Green;
			INT32 b = BmapPrevDlg::m_LockedColoursPalette2.Data[ ClosestColourIndex ].Blue;

			BmapPrevDlg::m_LockedColoursPalette2.Data[ ClosestColourIndex ].Red = 
				BmapPrevDlg::m_LockedColoursPalette2.Data[ Information ].Red;
			BmapPrevDlg::m_LockedColoursPalette2.Data[ ClosestColourIndex ].Green = 
				BmapPrevDlg::m_LockedColoursPalette2.Data[ Information ].Green;
			BmapPrevDlg::m_LockedColoursPalette2.Data[ ClosestColourIndex ].Blue = 
				BmapPrevDlg::m_LockedColoursPalette2.Data[ Information ].Blue;

			BmapPrevDlg::m_LockedColoursPalette2.Data[ Information ].Red = r;
			BmapPrevDlg::m_LockedColoursPalette2.Data[ Information ].Green = g;
			BmapPrevDlg::m_LockedColoursPalette2.Data[ Information ].Blue = b;

			BmapPrevDlg::m_LockedColoursPalette.Data[ i ].ExtraInformation = ClosestColourIndex;
			LockedColoursInformation = BmapPrevDlg::m_LockedColoursPalette;
		}
	}

	return lpPalette;
	*/
	return NULL;
}

/********************************************************************************************
>	BOOL BaseBitmapFilter::ExportImagemap(Operation *pOp, PathName* pPath, Document* pDoc)
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Returns:	TRUE if export was successful
				FALSE if there was a problem
	Purpose:	Calls on the Imagemap Filter to export an imagemap based on the current
				bitmap.	
********************************************************************************************/
BOOL BaseBitmapFilter::ExportImagemap(Operation *pOp, PathName* pPath, Document* pDoc)
{
	//Check our parameters
	ERROR2IF(pOp == NULL, FALSE,"BaseBitmapFilter::ExportImagemap no export operation");
	ERROR2IF(pPath == NULL, FALSE,"BaseBitmapFilter::ExportImagemap no export pathname");
	ERROR2IF(pDoc == NULL, FALSE,"BaseBitmapFilter::DoExport no document to export");

	//First, let's find the imagemap filter
	ImagemapFilter* pImagemapFilter=GetImagemapFilter();

	//And if we haven't found it, throw an error
	//Note that it is entirely possible that a future (modular) build might
	//not have an Imagemap filter, but for the moment it's worrying
	ERROR2IF(pImagemapFilter==NULL, FALSE, "OK. Own up. Who nicked the Imagemap filter?");

	//Create a file to export to
	CCDiskFile ImagemapFile(1024, FALSE, TRUE);

	//Now we need to set up some Imagemap Filter Options
	//We base these on the last set of ImagemapFilterOptions that the user exported
	//with, so get those options from the ImagemapFilter
	ImagemapFilterOptions ifoToSet=pImagemapFilter->GetFilterOptions();

	// Check we are exporting an imagemap either to a file or to the clipboard
	if (!(ifoToSet.m_fFile || ifoToSet.m_fClipboard))
		return TRUE; // Successfully done as requested (not exported an image map)
	
	if (!GetBitmapExportOptions())
		return FALSE; // can't export image map with out export options

	//Set the DPI from our bitmap options
	ifoToSet.m_dDPI=GetBitmapExportOptions()->GetDPI();	

	//And set the selection type
	ifoToSet.m_stExportArea=GetBitmapExportOptions()->GetSelectionType();


	// Use the path set previously in the image map options -- Jonathan 6/12/2000	
	//And give the filter a pointer to our export file
	ifoToSet.m_pfileFile=&ImagemapFile;

	// tell the filter where the graphic is that it is describing
	ifoToSet.m_GraphicPath = *pPath; 

	//Then set our new options back into the imagemap filter
	pImagemapFilter->SetFilterOptions(ifoToSet);

	//And tell the filter to export the file
	BOOL ok= pImagemapFilter->PrepareAndWriteData(pDoc);

	//Then close the file if it's still open
	if (ImagemapFile.isOpen())
		ImagemapFile.close();

	//And return
	return ok;
}

/********************************************************************************************

>	ImagemapFilter* BaseBitmapFilter::GetImagemapFilter()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Returns:	A pointer to the imagemap filter
	Purpose:	Gets a pointer to the imagemap filter

				This function may return NULL and the calling code should handle this case.

				This is in case we ever build a version of Camelot without an Imagemap Filter.
				All our code should cope with the Imagemap Filter not being there.

********************************************************************************************/
ImagemapFilter* BaseBitmapFilter::GetImagemapFilter()
{
	//First get a pointer to the filter manager
	FilterManager* pFilterManager=Camelot.GetFilterManager();

	ERROR2IF(pFilterManager==NULL, NULL, "BaseBitmapFilter::GetImagemapFilter - FilterManager does not exist");

	//Then return a pointer to the imagemap filter
	return ((ImagemapFilter*) pFilterManager->FindFilterFromID(FILTERID_IMAGEMAP));
	
}


/********************************************************************************************
>	void BaseBitmapFilter::ExportHTMLTag(PathName* ppthToUse)
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/97
	Purpose:	Exports an HTML tag to the clipboard that can be
				used in a page of HTML to embed the bitmap that is
				being exported.
  Technical notes:
				Note that the width and height parameters of the bitmap
				come from the width and height member variables of
				the BitmapExportOptions member variable.

				*But* Colin's comments say that it's at the filter author's 
				discretion whether to use a BitmapExportOptions object or not.
				So, it seems we shouldn't throw an error if we can't find
				a BitmapExportOptions object.

				So if we can't find a BFO object, we don't throw an error - 
				we simply don't export.
********************************************************************************************/
void BaseBitmapFilter::ExportHTMLTag(PathName* ppthToUse)
{
	//Check our parameter
	if (ppthToUse==NULL)
	{
		ERROR2RAW("BaseCamelotFilter::ExportHTMLTag - NULL parameter");
		return;
	}

	//First get the name of the file we are exporting to
	String_256 strFileName=ppthToUse->GetFileName(TRUE);

	//Now, do we have sensible bitmap export dimensions?
	ERROR3IF(m_PixelWidth<=0 || m_PixelHeight<=0, "Width and Height are both 0 in BaseBitmapFilter::ExportHTMLTag");
	
	//And if they make sense
	if (m_PixelWidth>0 && m_PixelHeight>0)
	{
		//Then export a tag based on that width and height

		String_256 strTag;
		strTag.MakeMsg(_R(IDS_BITMAPFILTER_HTMLTAG), &strFileName, m_PixelWidth, m_PixelHeight);

		//And put that string on the clipboard
PORTNOTE("clipboard","Removed InternalClipboard usage")
#ifndef EXCLUDE_FROM_XARALX
		InternalClipboard::CopyText(strTag);
#endif
	}
}

void PixelAlignedFiddle(DocRect* r)
{
	INT32 lox, loy, hix, hiy = 0;

#define INFLATE_ONLY

#ifdef INFLATE_ONLY
	INT32 fiddle = 0; // extends the export clipping box to surround the exported image
					 // maintaining the screen antialiasing
#else
	INT32 fiddle = 375; // shaves small amounts of aa if we think we dont need it
#endif
	
	lox = ((r->lo.x + fiddle)/750) * 750;
	loy = ((r->lo.y + fiddle)/750) * 750;
	hix = ((r->hi.x + 749 - fiddle)/750) * 750;
	hiy = ((r->hi.y + 749 - fiddle)/750) * 750;

	DocRect newr(lox, loy, hix, hiy);
	*r = newr;
}


//  Static function which is used by BmapPrevDlg to call the above function.
void BaseBitmapFilter::CallPixelAlignedFiddle( DocRect * r )
{
	PixelAlignedFiddle( r );
}

