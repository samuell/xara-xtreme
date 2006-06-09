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
// A simple Dialog That does some Gavin Rendering into itself


#include "camtypes.h"
#include "bmpexprw.h"
#include "prvwmenu.h" // the context menu for the dialog
//#include "exprwres.h"
#include "giffiltr.h"	// for TI_GIFFilter
//#include "selop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h"		// for Document - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "webprvw.h"	// the resource strings for the web preview
#include "product.h"	// the product name
#include "backgrnd.h"
#include "bmpexdoc.h"
#include "prvwflt.h"
#include "fileutil.h"
#include "bitmpinf.h"
#include "sgliboil.h"
#include "makebmp.h"

#include "filtimag.h"	//The imagemap filter class
#include "filtimop.h"	//The ImagemapFilterOptions class
#include "filtrmgr.h"	//The Filter Manager - used to find the imagemap filter
//#include "resimmap.h"	//Imagemap resources
#include "menucmds.h"	// InvokeWeblink

//#include "bmpsdlgr.h"	// _R(IDS_JPEG)

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, 
// here at the start of the file
CC_IMPLEMENT_MEMDUMP(BrowserPreviewOptions, CCObject)
CC_IMPLEMENT_DYNAMIC(BitmapPreviewData, CCObject)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

// this is a pointer to the path where the bitmap page background was exported 
// to be displayed in the web stub
PathName * BitmapPreviewData::pPagePath = NULL;

////////////////////////////////////////////////////////////////////////////////////////////
// BrowserPreviewOptions section

/*******************************************************************************************
>	BrowserPreviewOptions::BrowserPreviewOptions()

  Author:	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
  Created:	1/7/97
  Purpose:	Default Constructor
  
*******************************************************************************************/

BrowserPreviewOptions::BrowserPreviewOptions()
{	
	m_Background	= BROWSER_BGR_NONE; 
	m_bInfo 		= TRUE; 
	m_bImagemap 	= TRUE; 
	m_pSpread		= NULL;
}


/*******************************************************************************************
>	BrowserPreviewOptions::BrowserPreviewOptions(BrowserBackground Bgr, BOOL bInfo, 
BOOL bImagemap, ImagemapFilterOptions ifoToUse)

  Author:	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
  Created:	1/7/97
  Purpose:	Constructor - set the default values
  
*******************************************************************************************/

BrowserPreviewOptions::BrowserPreviewOptions(BrowserBackground Bgr, BOOL bInfo, BOOL bImagemap
											 , ImagemapFilterOptions ifoOptions)
{	
	m_Background	= Bgr; 
	m_bInfo 		= bInfo; 
	m_bImagemap 	= bImagemap; 
	m_pSpread		= NULL;
	m_ifoImagemapOptions = ifoOptions;
}


/*******************************************************************************************
>	void BrowserPreviewOptions::Get(BrowserBackground *pBgr, BOOL *pbInfo, BOOL *pbImagemap)

  Author:	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
  Created:	1/7/97
  Purpose:	Gets the values of the member variables
  
*******************************************************************************************/

void BrowserPreviewOptions::Get(BrowserBackground *pBgr, BOOL *pbInfo, BOOL *pbImagemap,
								ImagemapFilterOptions* pifoOptions)
{	
	*pBgr = m_Background; 
	*pbInfo = m_bInfo; 
	*pbImagemap = m_bImagemap; 
	if (pifoOptions)
		*pifoOptions=m_ifoImagemapOptions;
}


/*******************************************************************************************
>	void BrowserPreviewOptions::Set(BrowserBackground Bgr, BOOL bInfo, BOOL bImagemap,
ImagemapFilterOPtions ifoToSet)

  Author:	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
  Created:	1/7/97
  Purpose:	Sets values for the member variables
  
*******************************************************************************************/

void BrowserPreviewOptions::Set(BrowserBackground Bgr, BOOL bInfo, BOOL bImagemap,
								ImagemapFilterOptions ifoToSet)
{	
	m_Background = Bgr; 
	m_bInfo = bInfo; 
	m_bImagemap = bImagemap; 
	m_ifoImagemapOptions=ifoToSet;
}

/********************************************************************************************

  > BrowserPreviewOptions::BrowserPreviewOptions(const BrowserPreviewOptions& obj)
  
	Author: 	Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/97
	Scope:		public
	
********************************************************************************************/

BrowserPreviewOptions::BrowserPreviewOptions(const BrowserPreviewOptions& obj)
{
	m_Background			=	obj.m_Background;
	m_bInfo 				=	obj.m_bInfo;
	m_bImagemap 			=	obj.m_bImagemap;
	m_pSpread				=	obj.m_pSpread;
	m_ifoImagemapOptions	=	obj.m_ifoImagemapOptions;
}


////////////////////////////////////////////////////////////////////////////////////////////
// BitmapPreviewData section

/*******************************************************************************************
>	BitmapPreviewData::BitmapPreviewData()

  Author:	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
  Created:	11/4/97
  Purpose:	Constructor
  
*******************************************************************************************/

BitmapPreviewData::BitmapPreviewData()
{
	m_pBitmap = NULL;
	m_FileSize = 0;
	m_pOptions = NULL;
	m_pTempHTMLPath = NULL;
	
	m_bIsSameBitmap = TRUE;
}

/*******************************************************************************************
>	BitmapPreviewData::~BitmapPreviewData()

  Author:	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
  Created:	11/4/97
  Purpose:	Destructor
  
*******************************************************************************************/

BitmapPreviewData::~BitmapPreviewData()
{
	DeleteBitmapData();
}


/*******************************************************************************************

  > void BitmapPreviewData::DeleteBitmapData()
  
	Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	11/4/97
	Purpose:	Removes all the data of the object, but only if its not shared with other 
				objects of the same type
				
*******************************************************************************************/

void BitmapPreviewData::DeleteBitmapData()
{
	if (!m_bIsSameBitmap)
	{
		// delete the bitmap
		if (m_pBitmap != NULL)
			delete m_pBitmap;
		m_pBitmap = NULL;
		
		// delete the options	
		// SMFIX - Please dont delete my options these are only place holders, this class is NOT responsible for these objects
		//if (m_pOptions != NULL)
		//	delete m_pOptions;
		//m_pOptions = NULL;
	}
	
	// delete the html stub file
	if (m_pTempHTMLPath != NULL)
	{
		// delete the temp file 
		FileUtil::DeleteFile(m_pTempHTMLPath);
		
		// delete the path name
		delete m_pTempHTMLPath;
	}
	m_pTempHTMLPath = NULL;
}


/*******************************************************************************************

  > static BOOL BitmapPreviewData::ComposeHTMLColour(DocColour *pColour, String_32 &OutColour)
  
	Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	11/4/97
	Inputs: 	pColour -	pointer to a DocColour
	Outputs:	OutColour	- string, where the converted colour is stored.
	Purpose:	Converts from a DocColour to a colour string in html format:
				"#rrggbb", where rr - the red component, gg - the green, and rr - the blue.
				All the components specify a hexadecimal value in the 00 - ff range.
				
*******************************************************************************************/

BOOL BitmapPreviewData::ComposeHTMLColour(DocColour *pColour, String_32 &OutColour)
{
	if (pColour == NULL)
		return FALSE;
	
	// get the RGB values from the colour
	INT32 R, G, B;
	pColour->GetRGBValue(&R, &G, &B);
	
	// convert this into a html colour string
	OutColour = String_32("#");
	String_32 c;
	
	// add the red component
	if (R < 16) // is a leading 0 required
		OutColour += "0";
	c._MakeMsg("#1%X", R % 256);
	OutColour += c;
	
	// the green component
	if (G < 16)
		OutColour += "0";
	c._MakeMsg("#1%X", G % 256);
	OutColour += c;
	
	// the blue component
	if (B < 16)
		OutColour += "0";
	c._MakeMsg("#1%X", B % 256);
	OutColour += c;
	
	return TRUE;
}



/*******************************************************************************************

  > static BOOL BitmapPreviewData::ExportBrowserTypeBitmap(KernelBitmap *pBmp,PathName *pOutFile)
  
	Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	11/4/97
	Inputs: 	pKernelBitmap	-	pointer to a Bitmap to be exported
				pOutPath - the path to export the bitmap to
				Purpose:	Exports a kernel bitmap to a gif file, the gif preview filter is used, so 
				that no export options dialog box is displayed.
				
*******************************************************************************************/

BOOL BitmapPreviewData::ExportBrowserTypeBitmap(KernelBitmap *pBmp,PathName *pOutFile)
{
	ERROR3IF(pBmp == NULL, "NULL param passed in ExportPreviewBitmap");
	ERROR3IF(pOutFile == NULL, "NULL param passed in ExportPreviewBitmap");
	if ((pBmp == NULL) || (pOutFile == NULL))
		return FALSE;
	
	// create a disk file
	CCDiskFile TempDiskFile(1024, FALSE, TRUE);
	
	// Find the gif preview filter for export (so we don't get an options dialog box)
	Filter *pFilter = Filter::GetFirst();
	while (pFilter != NULL)
	{
		if (IS_A(pFilter,PreviewFilterGIF))
			// This is the filter!
			break;
		
		// Try the next filter
		pFilter = Filter::GetNext(pFilter);
	}
	
	if (pFilter == NULL)
		return FALSE;  // filter not found
	
	// get the preview bitmap filter, so no dialog box is displayed
	PreviewFilterGIF *pGIFFilter = (PreviewFilterGIF *)pFilter;
	
	BOOL ok = TRUE;
	
	// create an operation for the export
	SelOperation *pOp = new SelOperation;
	if (pOp != NULL)
	{
		ok = pGIFFilter->DoExportBitmap(pOp, &TempDiskFile, pOutFile, pBmp);
		
		// delete the created operation
		delete pOp;
	}
	else
		return FALSE;
	
	// close the file 
	if (TempDiskFile.isOpen())
		TempDiskFile.close();
	
	return ok;
}



/*******************************************************************************************

  > static BOOL BitmapPreviewData::SetBackgroundFromPage(CCDiskFile &DiskFile, Spread * pSpread = NULL)
  
	Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	11/4/97
	Inputs: 	DiskFile - the file to write to 
				pSpread  - the spread to pull the page background from
				Purpose:	Gets the current page background from the current document and generates an
				HTML string which displays the same background on the HTML page. If a spread
				has been specified then use that instead of the current document.
				See Also:	BitmapPreviewData::ComposeHTMLColour, BitmapPreviewData::ExportBrowserTypeBitmap
				
*******************************************************************************************/

BOOL BitmapPreviewData::SetBackgroundFromPage(CCDiskFile &DiskFile, Spread * pSpread)
{
	String_256 s; // to contain the generated html strings
	
	// If the user has not specified a spead then assume the selected spread in the current doc
	if (pSpread == NULL)
	{
		// get the current doc
		Document *pDoc = Document::GetCurrent();
		ERROR3IF(pDoc == NULL, "No Current Document");
		if (pDoc == NULL)
			return FALSE;
		pSpread = pDoc->GetSelectedSpread();
	}
	
	// get the page colour, or bitmap fill
	KernelBitmap *pPageBmp = NULL;
	DocColour *pPageColour = NULL;
	OpBackground::GetPageColour(pSpread, &pPageBmp, &pPageColour);
	
	BOOL Ok = TRUE;
	// check for a bitmap page background first
	if (pPageBmp != NULL) // the page background was a bitmap
	{
		// check if we haven't exported that bitmap before
		if (pPagePath == NULL)
		{
			// alocate the path name
			pPagePath = new PathName;
			
			if (pPagePath)
			{
				// create a new temporary file
				Ok = FileUtil::GetTemporaryPathName("gif", pPagePath);
			}
			else
				Ok = FALSE;
			
		}
		
		// check if we have a valid path
		if (Ok) Ok = pPagePath->IsValid();
		
		// export the background bitmap into the a file
		if (Ok && ExportBrowserTypeBitmap(pPageBmp, pPagePath))
		{
			// everything went ok, so add the link in the html file
			s.MakeMsg(_R(IDS_HTML_BGIMAGE), (const TCHAR *)pPagePath->GetFileName());
			DiskFile.write(s);
		}
		
		return Ok;
	}
	
	// If we recovered a page background colour then use that
	// otherwise Page is by default white so set this as a background
	BOOL AllocatedColour = FALSE;
	if (pPageColour == NULL)
	{
		// Neither colour nor bitmap.
		AllocatedColour = TRUE;
		pPageColour = new DocColour(COLOUR_WHITE);	// default colour of a page (white)
	}
	
	// convert to a browser colour
	String_32 col;
	if (ComposeHTMLColour(pPageColour,col))
	{
		// output the background colour
		s.MakeMsg(_R(IDS_HTML_BGCOLOUR), (TCHAR *)col);
		DiskFile.write(s);
	}
	else
		Ok = FALSE;
	
	if (AllocatedColour && pPageColour != NULL)
		delete pPageColour;
	
	return Ok;
}


/*******************************************************************************************

  > static BOOL BitmapPreviewData::ExportImagemap(CCDiskFile &DiskFile, PathName *pPath,
		ImagemapFilterOptions ifoOptionsToUse)
		
		  Author:	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
		  Created:	24/6/97
		  Inputs:	-
		  Purpose:	Generate a imagemap for our bitmap, and writes it out to our html page
		  
*******************************************************************************************/

BOOL BitmapPreviewData::ExportImagemap(CCDiskFile &DiskFile, PathName *pPath, SelectionType Sel, DPI dpi,
									   ImagemapFilterOptions ifoOptionsToUse)
{
	// sanity checks
	ERROR3IF(pPath == NULL, "BitmapPreviewData::ExportImagemap - no file path to use");
	if (dpi <= 0.0)
		return FALSE;
	
	//Rewritten by Graham 23/7/97
	
	//First find the imagemap filter
	ImagemapFilter* pImagemapFilter = (ImagemapFilter*) Filter::FindFilterFromID(FILTERID_IMAGEMAP);
	
	if (pImagemapFilter == NULL)  // no imagemap filter found
		return FALSE;
	
	//Now, we're going to set some imagemap filter options into the filter
	//But before we do so, let's remember the old set of imagemap filter options
	//so that the user doesn't get her defaults overridden
	ImagemapFilterOptions ifoOld=pImagemapFilter->GetFilterOptions();
	
	//Now, base our new set of options on the set we have been given
	ImagemapFilterOptions ifoToSet=ifoOptionsToUse;
	
	//But make the following changes
	ifoToSet.m_strName="PreviewBitmapImagemap";
	ifoToSet.m_stExportArea=Sel;
	ifoToSet.m_dDPI=dpi;
	ifoToSet.m_fClipboard=FALSE;
	ifoToSet.m_fFile=TRUE;
	ifoToSet.m_pthFile=*pPath;
	ifoToSet.m_fInsert=TRUE;
	ifoToSet.m_pfileFile=&DiskFile;
	ifoToSet.m_fReportErrors=FALSE;
	
	//Then set our new options back into the imagemap filter
	pImagemapFilter->SetFilterOptions(ifoToSet);
	
	//And tell the filter to export the file
	BOOL ok= pImagemapFilter->PrepareAndWriteData(Document::GetCurrent());
	
	//Finally, set the previous set of options back into the filter
	pImagemapFilter->SetFilterOptions(ifoOld);
	
	return ok; // return the result of the export
}


/*******************************************************************************************

  > BOOL BitmapPreviewData::GenerateHTMLStub(BrowserPreviewOptions BrowserOptions)
  
	Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	24/6/97
	Inputs: 	BrowserOptions - the options for generating the html page
	Purpose:	Generate a html file which includes our bitmap, together with some other optional 
				stuff. Used for browser preview.
				
*******************************************************************************************/
BOOL BitmapPreviewData::GenerateHTMLStub(BrowserPreviewOptions BrowserOptions)
{
	// sanity checks
	
	// check for export options
	
	ERROR3IF(m_pOptions == NULL, "BitmapPreviewData::GenerateHTMLStub - need export options");
	if (m_pOptions == NULL) // we need export options
		return FALSE;
	
	// get the path for the exported bitmap from the options
	PathName TempPath = m_pOptions->GetPathName();
	
	// check for exported bitmap
	ERROR3IF((m_pOptions->HasTempFile() == FALSE) || (TempPath.IsValid() == FALSE), 
		"BitmapPreviewData::GenerateHTMLStub - need exported bitmap");
	if ((m_pOptions->HasTempFile() == FALSE) || (TempPath.IsValid() == FALSE)) 
		return FALSE;
	
	// delete the previous temp file, if any
	if (m_pTempHTMLPath != NULL)
		FileUtil::DeleteFile(m_pTempHTMLPath);
	else
		m_pTempHTMLPath = new PathName;
	
	// safety check
	if (m_pTempHTMLPath == NULL)
		return FALSE;
	
	// create a new temp file 
	if (FileUtil::GetTemporaryPathName("htm",m_pTempHTMLPath) == FALSE)
	{
		delete m_pTempHTMLPath;
		m_pTempHTMLPath = NULL;
		
		return FALSE;
	}
	
	// start creating the html page
	
	// create a disk file
	CCDiskFile TempDiskFile(1024, FALSE, TRUE);
	
	// get path name to server, so we can find the logo bitmaps
	CString str;
	AfxGetModuleShortFileName(AfxGetInstanceHandle(), str);
	String_256 strPathName(str.GetBuffer(0));
	
	// create a path name object from the program name
	PathName ProgramPath(strPathName);
	
	TRY
	{
		// open it for reading
		if (!TempDiskFile.open(*m_pTempHTMLPath, ios::out))
			return FALSE;
		
		// output header
		String_256 s(_R(IDS_HTML_HEAD));
		TempDiskFile.write(s);
		
		// set the background attributes
		switch (BrowserOptions.m_Background)
		{
		case BROWSER_BGR_DOC:
			{
				// set the background from the document page
				if (!SetBackgroundFromPage(TempDiskFile, BrowserOptions.m_pSpread))
					ERROR3("Setting the html background from the document page failed\n");
			}
			break;
			
		case BROWSER_BGR_CHECKER:
			{
				// display the checkered bitmap as background
				
				// get the checkered bitmap name
				String_256 Checker(_R(IDS_HTML_CHECKER));
				
				// set it in the path
				ProgramPath.SetFileNameAndType(Checker);
				
				// set the checkered bitmap as background image
				s.MakeMsg(_R(IDS_CHECK_BACK), (TCHAR *)ProgramPath.GetWebAddress());
				TempDiskFile.write(s);
			}
			break;
			
		case BROWSER_BGR_BITMAP:
			{
				// set our bitmap as background image
				s.MakeMsg(_R(IDS_BITMAPED_BACKGROUND), (TCHAR *)TempPath.GetWebAddress());
				TempDiskFile.write(s);
				
				
			}
			break;
			
		case BROWSER_BGR_NONE:
			{
				String_256 background(_R(IDS_PLAIN_BACKGROUND));
				TempDiskFile.write(background);
			}
			
			break;
			
		default:
			ERROR3("Unknown type of BrowserOptions.m_Background");
			break;
		} // end of switch(BrowserOptions.m_Background) block
		
		// Get a pointer to the actual bitmap so that we can get some details from it.
		//		OILBitmap *pOilBitmap = m_pBitmap->ActualBitmap;
		//		ERROR3IF(pOilBitmap == NULL,"BitmapPreviewData::GenerateBitmapInfoStrings NULL oil bitmap pointer");
		
		// Get the details from the specified bitmap
		//		BitmapInfo BmInfo;
		//		pOilBitmap->GetInfo(&BmInfo);
		
		// output our bitmap, but only if it wasn't already set as background
		String_256 s2(_R(IDS_PAGE_TITLE));
		TempDiskFile.write(s2);
		
		if (BrowserOptions.m_Background != BROWSER_BGR_BITMAP)
		{
			// output the bitmap
			
			if (m_pOptions->GetFilterType() == MAKE_BITMAP_FILTER)
			{
				// GIF animation
				s.MakeMsg(_R(IDS_DISPLAY_PIC), (const TCHAR *)TempPath.GetFileName());
			}
			else
			{
				switch (m_pOptions->GetFilterNameStrID())
				{
				case _R(IDN_FILTERNAME_GIF):
				case _R(IDS_JPG_EXP_FILTERNAME): 
				case _R(IDT_FILTERNAME_BMP):
					s.MakeMsg(_R(IDS_DISPLAY_BMP), (const TCHAR *)TempPath.GetFileName());
					break;
				case _R(IDS_FILTERNAME_PNG):
					s.MakeMsg(_R(IDS_DISPLAY_PNG), (const TCHAR *)TempPath.GetFileName());
					break;
				}
			}	
			
			TempDiskFile.write(s);
		}
		else
		{
			s.MakeMsg(_R(IDS_EMPTY_BOX));
			TempDiskFile.write(s);
			
		}
		
		switch (m_pOptions->GetFilterNameStrID())
		{
		case _R(IDT_FILTERNAME_BMP):
			s2.MakeMsg(_R(IDS_BMP_MESSAGE));
			s.MakeMsg(_R(IDS_WARNING_BOX), (TCHAR *)s2);
			TempDiskFile.write(s);
			break;
		case _R(IDS_FILTERNAME_PNG):
			s2.MakeMsg(_R(IDS_PNG_MESSAGE));
			s.MakeMsg(_R(IDS_WARNING_BOX), (TCHAR *)s2);
			TempDiskFile.write(s);
			break;
		} // end of switch(m_pOptions->GetFilterNameStrID())
		
		s.MakeMsg(_R(IDS_DETAILS_BUILD));
		TempDiskFile.write(s);
		
		// output the bitmap info, if requested
		if (BrowserOptions.m_bInfo != FALSE)
		{
			String_64 ImageSize;
			String_64 FileSize;
			BOOL m_bTransparent = FALSE;
			INT32 count;
			count=0;
			// generate the info strings
			
			if (m_pOptions->GetFilterType() == MAKE_BITMAP_FILTER)
			{
				// This is actually the export animated GIF using the frame gallery system
				MakeBitmapExportOptions* pMkBOptions = (MakeBitmapExportOptions*)m_pOptions;
				ERROR3IF(!pMkBOptions->IS_KIND_OF(MakeBitmapExportOptions), "pMkBOptions isn't");
				
				PALETTE Palette = PAL_OPTIMISED;
				DITHER DitherType;
				UINT32 colDepth;
				String_64 sPalette;
				String_64 Dither;
				UINT32 NoOfColours;
				
				NoOfColours=m_pOptions->GetNumColsInPalette();
				//						Palette = pMkBOptions->GetPalette();
				DitherType = pMkBOptions->GetDither();
				colDepth = pMkBOptions->GetDepth();
				
				s2.MakeMsg(_R(IDS_ANIMATED_GIF));
				
				if (pMkBOptions->WantTransparent())
				{
					s2.MakeMsg(_R(IDS_TRANSPARENT),"animated GIF");
				}
				
				s.MakeMsg(_R(IDS_TEXTLINE_BR),(const TCHAR*)s2);
				TempDiskFile.write(s);
				
				switch (Palette)
				{
				case PAL_OPTIMISED:
					sPalette.MakeMsg(_R(IDS_OPTIMISED_PAL));
					break;
				case PAL_BROWSER:
				case PAL_STANDARD:
					sPalette.MakeMsg(_R(IDS_BROWSER_PAL));
					break;
				case PAL_GLOBALOPTIMISED:
					sPalette.MakeMsg(_R(IDS_GLOBAL_OPT));
					break;
					
				}
				
				if (colDepth == 8 && (Palette == PAL_STANDARD || Palette == PAL_BROWSER) )
				{
					NoOfColours = 216;
				}
				
				switch (colDepth)
				{
				case 32:
				case 24:
					s2.MakeMsg(_R(IDS_TRUECOLOUR),colDepth);
					break;
				case 8:
					{
						// Include the transparent colour in the colour depth check
						UINT32 RealNumberOfColours = NoOfColours;
						if (colDepth <= 8 && pMkBOptions->WantTransparent())
						{
							UINT32 MaxColours = UINT32(pow(2,colDepth));
							RealNumberOfColours++;
							if (RealNumberOfColours > MaxColours)
								RealNumberOfColours = MaxColours;
						}
						// We say 8 but we really mean the number of colours deep
						// We cannot say 2 colours = 2bpp as we save it as 10 colours due
						// to having 1 transparent colour and the usual lets pick 10 as the
						// transparent colour. Cannot allow the user to choose 1 as the code
						// then outputs 8bpp as GRenderOptPalette::GetOptimisedPalette has the
						// test if (ReservedColours < NumColours), which fails.
						/* if (RealNumberOfColours <= 2)
						s2.MakeMsg(_R(IDS_PALETTEINFO),2,NoOfColours,(const TCHAR*)sPalette);
						else */
						if (RealNumberOfColours <= 16)
							s2.MakeMsg(_R(IDS_PALETTEINFO),4,NoOfColours,(const TCHAR*)sPalette);
						else
							s2.MakeMsg(_R(IDS_AN_PALINFO),colDepth,NoOfColours,(const TCHAR*)sPalette);
						break;
					}
				case 1:
					s2.MakeMsg(_R(IDS_MONO),colDepth);
					break;
				case 4:
				default:
					s2.MakeMsg(_R(IDS_PALETTEINFO),colDepth,NoOfColours,(const TCHAR*)sPalette);					
					break;
				}
				
				TempDiskFile.write(s2); 				
				
				s.MakeMsg(_R(IDS_NODITHER));
				switch (DitherType)
				{
				case XARADITHER_ORDERED:
				case XARADITHER_ORDERED_GREY:
					Dither.MakeMsg(_R(IDS_DITH_ORDER));
					s.MakeMsg(_R(IDS_DITHERING),(const TCHAR*) Dither);
					break;
				case XARADITHER_ERROR_DIFFUSION:
				case XARADITHER_SIMPLE:
					Dither.MakeMsg(_R(IDS_DITH_ERROR));
					s.MakeMsg(_R(IDS_DITHERING),(const TCHAR*) Dither);
					break;
				case XARADITHER_NONE:
					s.MakeMsg(_R(IDS_NODITHER));
					break;
				}
				
				TempDiskFile.write(s);
			} // if (m_pOptions->GetFilterType() == MAKE_BITMAP_FILTER) block ends
			else
			{
				switch (m_pOptions->GetFilterNameStrID())
				{
				case _R(IDN_FILTERNAME_GIF):
					{
						GIFExportOptions* pGIFOptions = (GIFExportOptions*)m_pOptions;
						ERROR3IF(!pGIFOptions->IS_KIND_OF(GIFExportOptions), "pGIFOptions isn't");
						PALETTE Palette = PAL_OPTIMISED;
						DITHER DitherType;
						UINT32 colDepth;
						String_64 sPalette;
						String_64 Dither;
						UINT32 NoOfColours;
						
						NoOfColours=m_pOptions->GetNumColsInPalette();
						//						Palette = pGIFOptions->GetPalette();
						DitherType = pGIFOptions->GetDither();
						colDepth = pGIFOptions->GetDepth();
						
						
						if (pGIFOptions->WantTransparent())
							count=count+1;
						
						if (pGIFOptions->WantInterlaced())
							count=count+2;
						
						s.MakeMsg(_R(IDS_GIF));
						s2.MakeMsg(_R(IDS_A), (const TCHAR*)s);
						
						switch (count)
						{
						case 1:
							s2.MakeMsg(_R(IDS_TRANSPARENT),"GIF");
							break;
						case 2:
							s2.MakeMsg(_R(IDS_INTERLACED),"GIF");
							break;
						case 3:
							s2.MakeMsg(_R(IDS_INTER_TRANS),"GIF");
							break;
						case 0:
							break;
						}
						
						s.MakeMsg(_R(IDS_TEXTLINE_BR),(const TCHAR*)s2);
						TempDiskFile.write(s);
						
						switch (Palette)
						{
						case PAL_OPTIMISED:
							sPalette.MakeMsg(_R(IDS_OPTIMISED_PAL));
							break;
						case PAL_BROWSER:
						case PAL_STANDARD:
							sPalette.MakeMsg(_R(IDS_BROWSER_PAL));
							break;
						case PAL_GLOBALOPTIMISED:
							sPalette.MakeMsg(_R(IDS_GLOBAL_OPT));
							break;
							
						}
						
						if (colDepth == 8 && (Palette == PAL_STANDARD || Palette == PAL_BROWSER) )
						{
							NoOfColours = 216;
						}
						
						s2.MakeMsg(_R(IDS_PALETTEINFO),colDepth,NoOfColours,(const TCHAR*)sPalette);					
						
						if (colDepth == 8)
							s2.MakeMsg(_R(IDS_AN_PALINFO),colDepth,NoOfColours,(const TCHAR*)sPalette);
						
						if (colDepth == 1)
							s2.MakeMsg(_R(IDS_MONO),colDepth);
						
						if (colDepth > 8)
							s2.MakeMsg(_R(IDS_TRUECOLOUR),colDepth);
						
						TempDiskFile.write(s2); 				
						
						s.MakeMsg(_R(IDS_NODITHER));
						switch (DitherType)
						{
						case XARADITHER_ORDERED:
						case XARADITHER_ORDERED_GREY:
							Dither.MakeMsg(_R(IDS_DITH_ORDER));
							s.MakeMsg(_R(IDS_DITHERING),(const TCHAR*) Dither);
							break;
						case XARADITHER_ERROR_DIFFUSION:
						case XARADITHER_SIMPLE:
							Dither.MakeMsg(_R(IDS_DITH_ERROR));
							s.MakeMsg(_R(IDS_DITHERING),(const TCHAR*) Dither);
							break;
						case XARADITHER_NONE:
							s.MakeMsg(_R(IDS_NODITHER));
							break;
						}
						
						TempDiskFile.write(s);
						
						
					} // end case _R(IDN_FILTERNAME_GIF)
					break;
				case _R(IDS_JPG_EXP_FILTERNAME): 
					{
						JPEGExportOptions* pJPEGOptions = (JPEGExportOptions *)m_pOptions;
						ERROR3IF(!pJPEGOptions->IS_KIND_OF(JPEGExportOptions), "pJPEGOptions isn't");
						INT32 Quality = pJPEGOptions->GetQuality();	// Default Quality
						BOOL Progressive = pJPEGOptions->DoAsProgressive();
						if (Quality > 100)
							Quality = 100;
						
						
						s.MakeMsg(_R(IDS_JPEG));
						s2.MakeMsg(_R(IDS_A), (const TCHAR*)s);
						
						if (Progressive)
							s2.MakeMsg(_R(IDS_PROGRESSIVE));
						
						s.MakeMsg(_R(IDS_TEXTLINE_BR),(const TCHAR*)s2);
						TempDiskFile.write(s);
						
						
						s.MakeMsg(_R(IDS_JCOMPRESSION),Quality);
						TempDiskFile.write(s);					
					} // end case _R(IDS_JPG_EXP_FILTERNAME)
					break;
					
				case _R(IDT_FILTERNAME_BMP):
					{
						BMPExportOptions* pBMPOptions = (BMPExportOptions*)m_pOptions;
						ERROR3IF(!pBMPOptions->IS_KIND_OF(BMPExportOptions), "pBMPOptions isn't");
						PALETTE Palette;
						DITHER DitherType;
						UINT32 colDepth;
						String_64 sPalette;
						String_64 Dither;
						UINT32 NoOfColours;
						
						NoOfColours=m_pOptions->GetNumColsInPalette();
						Palette = pBMPOptions->GetPalette();
						DitherType = pBMPOptions->GetDither();
						colDepth = pBMPOptions->GetDepth();
						
						s.MakeMsg(_R(IDS_BMP));
						s2.MakeMsg(_R(IDS_A), (const TCHAR*)s);
						
						s.MakeMsg(_R(IDS_TEXTLINE_BR),(const TCHAR*)s2);
						TempDiskFile.write(s);
						
						switch (Palette)
						{
						case PAL_OPTIMISED:
							sPalette.MakeMsg(_R(IDS_OPTIMISED_PAL));
							break;
						case PAL_BROWSER:
						case PAL_STANDARD:
							sPalette.MakeMsg(_R(IDS_BROWSER_PAL));
							break;
						case PAL_GLOBALOPTIMISED:
							sPalette.MakeMsg(_R(IDS_GLOBAL_OPT));
							break;
						}
						
						if (colDepth == 8 && (Palette == PAL_STANDARD || Palette == PAL_BROWSER) )
						{
							NoOfColours = 216;
						}
						
						if (colDepth == 8 && NoOfColours > 256)
							NoOfColours = 256;
						
						s2.MakeMsg(_R(IDS_PALETTEINFO),colDepth,NoOfColours,(const TCHAR*)sPalette);					
						
						if (colDepth == 8)
							s2.MakeMsg(_R(IDS_AN_PALINFO),colDepth,NoOfColours,(const TCHAR*)sPalette);
						
						if (colDepth == 1)
							s2.MakeMsg(_R(IDS_MONO),colDepth);
						
						if (colDepth > 8)
							s2.MakeMsg(_R(IDS_TRUECOLOUR),colDepth);
						
						TempDiskFile.write(s2); 				
						
						s.MakeMsg(_R(IDS_NODITHER));
						switch (DitherType)
						{
						case XARADITHER_ORDERED:
						case XARADITHER_ORDERED_GREY:
							Dither.MakeMsg(_R(IDS_DITH_ORDER));
							s.MakeMsg(_R(IDS_DITHERING),(const TCHAR*) Dither);
							break;
						case XARADITHER_ERROR_DIFFUSION:
						case XARADITHER_SIMPLE:
							Dither.MakeMsg(_R(IDS_DITH_ERROR));
							s.MakeMsg(_R(IDS_DITHERING),(const TCHAR*) Dither);
							break;
						case XARADITHER_NONE:
							s.MakeMsg(_R(IDS_NODITHER));
							break;
						}
						
						TempDiskFile.write(s);
					} // end case _R(IDT_FILTERNAME_BMP)
					break;
					
				case _R(IDS_FILTERNAME_PNG):
					{
						PNGExportOptions* pPNGOptions = (PNGExportOptions*)m_pOptions;
						ERROR3IF(!pPNGOptions->IS_KIND_OF(PNGExportOptions), "pPNGOptions isn't");
						PALETTE Palette = PAL_OPTIMISED;
						DITHER DitherType;
						UINT32 colDepth;
						String_64 sPalette;
						String_64 Dither;
						UINT32 NoOfColours;
						
						NoOfColours=m_pOptions->GetNumColsInPalette();
						
						//						Palette = pPNGOptions->GetPalette();
						DitherType = pPNGOptions->GetDither();
						colDepth = pPNGOptions->GetDepth();
						
						
						if (pPNGOptions->WantTransparent())
							count=count+1;
						
						if (pPNGOptions->WantInterlaced())
							count=count+2;
						
						s.MakeMsg(_R(IDS_PNG));
						s2.MakeMsg(_R(IDS_A), (const TCHAR*)s);
						
						switch (count)
						{
						case 1:
							s2.MakeMsg(_R(IDS_TRANSPARENT),"PNG");
							break;
						case 2:
							s2.MakeMsg(_R(IDS_INTERLACED),"PNG");
							break;
						case 3:
							s2.MakeMsg(_R(IDS_INTER_TRANS),"PNG");
							break;
						case 0:
							break;
						}
						
						s.MakeMsg(_R(IDS_TEXTLINE_BR),(const TCHAR*)s2);
						TempDiskFile.write(s);
						
						switch (Palette)
						{
						case PAL_OPTIMISED:
							sPalette.MakeMsg(_R(IDS_OPTIMISED_PAL));
							break;
						case PAL_BROWSER:
						case PAL_STANDARD:
							sPalette.MakeMsg(_R(IDS_BROWSER_PAL));
							break;
						case PAL_GLOBALOPTIMISED:
							sPalette.MakeMsg(_R(IDS_GLOBAL_OPT));
							break;
						}
						
						if (colDepth == 8 && (Palette == PAL_STANDARD || Palette == PAL_BROWSER) )
						{
							NoOfColours = 216;
						}
						
						s2.MakeMsg(_R(IDS_PALETTEINFO),colDepth,NoOfColours,(const TCHAR*)sPalette);					
						
						if (colDepth == 8)
							s2.MakeMsg(_R(IDS_AN_PALINFO),colDepth,NoOfColours,(const TCHAR*)sPalette);
						
						if (colDepth == 1)
							s2.MakeMsg(_R(IDS_MONO),colDepth);
						
						if (colDepth > 8)
							s2.MakeMsg(_R(IDS_TRUECOLOUR),colDepth);
						
						TempDiskFile.write(s2); 				
						
						s.MakeMsg(_R(IDS_NODITHER));
						switch (DitherType)
						{
						case XARADITHER_ORDERED:
						case XARADITHER_ORDERED_GREY:
							Dither.MakeMsg(_R(IDS_DITH_ORDER));
							s.MakeMsg(_R(IDS_DITHERING),(const TCHAR*) Dither);
							break;
						case XARADITHER_ERROR_DIFFUSION:
						case XARADITHER_SIMPLE:
							Dither.MakeMsg(_R(IDS_DITH_ERROR));
							s.MakeMsg(_R(IDS_DITHERING),(const TCHAR*) Dither);
							break;
						case XARADITHER_NONE:
							s.MakeMsg(_R(IDS_NODITHER));
							break;
						}
						
						TempDiskFile.write(s);
					} // end case _R(IDS_FILTERNAME_PNG)
					
					break;
				} // end switch (m_pOptions->GetFilterNameStrID())
			} // end else block
			
			GenerateBitmapInfoStrings(ImageSize, FileSize);
			
			// output the bitmap info
			s.MakeMsg(_R(IDS_HTML_INFO1), (TCHAR *)ImageSize);
			TempDiskFile.write(s);
			
			// output the bitmap file size info
			s.MakeMsg(_R(IDS_HTML_INFO2), (TCHAR *)FileSize);
			TempDiskFile.write(s);
			
			s.MakeMsg(_R(IDS_CLOSE_CENTER));
			TempDiskFile.write(s);
			
			s.MakeMsg(_R(IDS_TDTR_CLOSE));
			TempDiskFile.write(s);
			
			s.MakeMsg(_R(IDS_BANNER_BOX));
			TempDiskFile.write(s);
			
			s.MakeMsg(_R(IDS_SPEED_BUILD));
			TempDiskFile.write(s);
			//			s.MakeMsg(_R(IDS_SPEED_BUILD2));
			//			TempDiskFile.write(s);
			
			s.MakeMsg(_R(IDS_ES_TR));
			TempDiskFile.write(s);
			
			String_64 times;
			
			s.MakeMsg(_R(IDS_TIME_ENTRY),"14.4k");
			TempDiskFile.write(s);
			CalculateTime(times,14400.0);
			TempDiskFile.write(times);
			s.MakeMsg(_R(IDS_ES_TR));
			TempDiskFile.write(s);
			
			s.MakeMsg(_R(IDS_TIME_ENTRY),"28.8k");
			TempDiskFile.write(s);
			CalculateTime(times,28800.0);
			TempDiskFile.write(times);
			s.MakeMsg(_R(IDS_ES_TR));
			TempDiskFile.write(s);
			
			s.MakeMsg(_R(IDS_TIME_ENTRY),"33.6k");
			TempDiskFile.write(s);
			CalculateTime(times,33600.0);
			TempDiskFile.write(times);
			s.MakeMsg(_R(IDS_ES_TR));
			TempDiskFile.write(s);
			
			s.MakeMsg(_R(IDS_TIME_ENTRY),"57.6k");
			TempDiskFile.write(s);
			CalculateTime(times,57600.0);
			TempDiskFile.write(times);
			s.MakeMsg(_R(IDS_ES_TR));
			TempDiskFile.write(s);
			
			s.MakeMsg(_R(IDS_TIME_ENTRY),"64k");
			TempDiskFile.write(s);
			CalculateTime(times,65536.0);
			TempDiskFile.write(times);
			s.MakeMsg(_R(IDS_ES_TR));
			TempDiskFile.write(s);
			
			s.MakeMsg(_R(IDS_TIME_ENTRY),"128k");
			TempDiskFile.write(s);
			CalculateTime(times,131072.0);
			TempDiskFile.write(times);
			
			//			s.MakeMsg(_R(IDS_ES_TR));
			//			TempDiskFile.write(s);
			
			s.MakeMsg(_R(IDS_SPEED_END));
			TempDiskFile.write(s);
			
			TRACE( _T("m_FileSize = %d"), m_FileSize);
		} // end of if (BrowserOptions.m_bInfo != FALSE)
		
		String_64 webaddy(_R(IDS_HTML_URL_PROGRAM));
		//Mark Howitt, 29/10/97. Setup the _R(IDS_HTML_URL_VISITMSG) To be a TCHAR* Also. It likes it that way!
		String_64 webvmess(_R(IDS_HTML_URL_VISITMSG));
		
		s.MakeMsg(_R(IDS_LINK_BOX),(TCHAR *)webaddy,(TCHAR *)webvmess);
		TempDiskFile.write(s);
		
		//end of my stuff
		
		s.MakeMsg(_R(IDS_END_TABLE));
		TempDiskFile.write(s);
		
		s.MakeMsg(_R(IDS_CLOSE_CENTER));
		TempDiskFile.write(s);
		
		// output the "Exported from" string
		
		// get the xara logo file name
		String_256 Logo(_R(IDS_HTML_XARALOGO));
		
		// set it in the path
		ProgramPath.SetFileNameAndType(Logo);
		
		// get the xara link
		String_256 URL(_R(IDS_HTML_URL_XARA));
		
		// output the xara link and the xara logo
		s.MakeMsg(_R(IDS_HTML_XARA), (TCHAR *)URL, (TCHAR *)ProgramPath.GetWebAddress());
		TempDiskFile.write(s);
		
		// Do not display the Xara X logo bottom right...
		/*
		// get the program logo file name
		Logo.Load(_R(IDS_HTML_PROGRAMLOGO));
		
		// set it in the path
		ProgramPath.SetFileNameAndType(Logo);
		
		// output the program link
		URL.Load(_R(IDS_HTML_URL_PROGRAM));
		s.MakeMsg(_R(IDS_HTML_PROGRAM), (TCHAR *)URL, (TCHAR *)ProgramPath.GetWebAddress());
		TempDiskFile.write(s);
		*/
		
		// output the end of the file
		s.Load(_R(IDS_HTML_END));
		TempDiskFile.write(s);
		
		// close the file
		TempDiskFile.close();
		
		// now export the image map, if the option for that is set
		if (BrowserOptions.m_bImagemap != FALSE)
		{
			// export the image map
			ExportImagemap( TempDiskFile, 
				m_pTempHTMLPath, 
				m_pOptions->GetSelectionType(), 
				m_pOptions->GetDPI(),
				BrowserOptions.m_ifoImagemapOptions);
		}
		
		TCHAR *FileWebAddress =  m_pTempHTMLPath->GetWebAddress();
		
		//Graham 17/9/97
		ProgramPath.SetFileNameAndType(PRODUCT_WEBLINKEXENAME);
		
		const TCHAR *PathToWebLink = (const TCHAR *)ProgramPath.GetPath();
		
		String_256 CmdLine = PathToWebLink + String_256(" -f ") + FileWebAddress;
		//TCHAR *CommandLine = (TCHAR *)CmdLine;
		
		if (!InvokeWeblink(CmdLine))
			return FALSE;
		
	} // end of TRY block
	
	CATCH(CFileException, e)
	{
		// any disk problems - come here
		
		// not much we can do really - just close the file and return FALSE
		if (TempDiskFile.isOpen())
			TempDiskFile.close();
		
		return FALSE;
	}
	END_CATCH
		
		return TRUE;
}



/*******************************************************************************************

  > BOOL BitmapPreviewData::DeleteTempFile(PathName *FileToRemove)
  
	Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> (based on code by Neville Humphrys)
	Created:	11/4/97
	Inputs: 	FileToRemove - the path name of the file to be removed.
	Returns:	TRUE if succesful, FALSE otherwise
	Purpose:	Removes a file
	
 *******************************************************************************************/
#if FALSE
 BOOL BitmapPreviewData::DeleteTempFile(PathName *FileToRemove)
 {
	 if (FileToRemove == NULL)
		 return FALSE;
	 
	 BOOL Exists = TRUE;
	 BOOL status = TRUE;
	 
	 // check if the file exists
	 Exists = SGLibOil::FileExists(FileToRemove);
	 
	 // remove it
	 if (Exists)
		 status = _tremove((const TCHAR *)FileToRemove->GetPath());
	 
	 return !status;
 }
#endif
 
 
 /********************************************************************************************
 
   >	void BitmapPreviewData::GenerateBitmapInfoStrings(String_64 &ImageSize, String_64 &FileSize,
   BOOL bIncludeBpp = FALSE)
   
	 Author:	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	 Created:	10/5/97
	 Inputs:	-
	 Outputs:	ImageSize - a string receiving the image size info for the current bitmap
	 FileSize - a string receiving the file size info for the current bitmap
	 Returns:	-
	 Purpose:	Generates image size and file size info strings
 ********************************************************************************************/
 
 void BitmapPreviewData::GenerateBitmapInfoStrings(String_64 &ImageSize, String_64 &FileSize,
	 BOOL bIncludeBpp)
 {
	 // sanity check
	 ERROR3IF(m_pBitmap == NULL, "BitmapPreviewData::GenerateBitmapInfoStrings no bitmap to work with");
	 ERROR3IF(m_pOptions == NULL, "BitmapPreviewData::GenerateBitmapInfoStrings no export options");
	 if ((m_pBitmap == NULL) || (m_pOptions == NULL))
		 return;
	 
	 // Get a pointer to the actual bitmap so that we can get some details from it.
	 OILBitmap *pOilBitmap = m_pBitmap->ActualBitmap;
	 ERROR3IF(pOilBitmap == NULL,"BitmapPreviewData::GenerateBitmapInfoStrings NULL oil bitmap pointer");
	 
	 // Get the details from the specified bitmap
	 BitmapInfo BmInfo;
	 pOilBitmap->GetInfo(&BmInfo);
	 
	 // make the image size string
	 
	 // the width and the height
	 ImageSize.MakeMsg(_R(IDS_IMAGE_SIZE), BmInfo.PixelWidth, BmInfo.PixelHeight);
	 
	 // the image depth
	 if (bIncludeBpp)
	 {
		 UINT32 Depth = m_pOptions->GetDepth();
		 if (Depth == 1)
		 {
			 // monochrome image
			 ImageSize += String_32(_R(IDS_IMAGE_DEPTH1));
		 }
		 else
		 {
			 String_16 s;
			 s.MakeMsg(_R(IDS_IMAGE_DEPTH2), Depth);
			 ImageSize += s;
		 }
	 }
	 
	 // make the file size message
	 FileSize.MakeMsg(_R(IDS_FILE_SIZE), m_FileSize);
 }
 
 
 
 /********************************************************************************************
 
   >	void BitmapPreviewData::SetNewBitmap(KernelBitmap *pKernelBitmap, PathName *TempPath, 
   UINT32 FileSize, BitmapExportOptions *pOptions)
   
	 Author:	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	 Created:	10/5/97
	 Inputs:	pKernelBitmap - pointer to a kernel bitmap
	 TempPath - The disk file of the bitmap
	 FileSize - the size of the temp disk file
	 pOptions - the export options for the bitmap
	 Outputs:	-
	 Returns:	-
	 Purpose:	Receives a new bitmap (and its file). Deletes the previous bitmap and file 
	 (if any) set for that object.
 ********************************************************************************************/
 
 void BitmapPreviewData::SetNewBitmap(KernelBitmap *pKernelBitmap, UINT32 FileSize, 
	 BitmapExportOptions *pOptions)
 {
	 // delete any previous data
	 DeleteBitmapData();
	 
	 // now remember the data
	 m_pBitmap = pKernelBitmap;
	 m_FileSize = FileSize;
	 m_pOptions = pOptions;
 }
 
 /********************************************************************************************
 
   >	void BitmapPreviewData::CalculateTime(String_64 &timestring,double speed)
   
	 Author:	Martin_Bell (Xara Group Ltd) <camelotdev@xara.com> Donnelly
	 Created:	12/7/97
	 Inputs:	speed - the download speed
	 
	   Outputs:	-
	   Returns:	timestring - The timeing to be returned as a string
	   Purpose:	Calculates the time to download the bitmap
 ********************************************************************************************/
 
 
 void BitmapPreviewData::CalculateTime(String_64 &timestring,double speed)
 {
	 // sanity check
	 
	 double dtime;
	 String_64 tempstring;
	 char buffer[16];
	 INT32 j;
	 String_16 times;
	 
	 dtime =(m_FileSize * 8.0)/speed;
	 
	 if (fabs(dtime) == fabs(1.000)) 
	 {
		 tempstring.MakeMsg(_R(IDS_1SEC));
		 timestring.MakeMsg(_R(IDS_TIME_ENTRY),(TCHAR *)tempstring);
	 }
	 else if (fabs(dtime) < fabs(1.000))
	 {
		 //Added by Graham 14/9/97
		 
		 tempstring.MakeMsg(_R(IDS_LESSTHANONESECOND));
		 timestring.MakeMsg(_R(IDS_TIME_ENTRY),(TCHAR *)tempstring);
		 
	 }
	 else
	 { 
		 /* Format and print various data: */
		 j  = sprintf( buffer,"%.2f",dtime);
		 
		 times.MakeMsg(_R(IDS_SECS_MOD), buffer);
		 timestring.MakeMsg(_R(IDS_TIME_ENTRY),(const TCHAR *)times);
	 }
	 
	 
 }
