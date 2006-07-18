// $Id: pngfiltr.cpp 1282 2006-06-09 09:46:49Z alex $
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

// A ImageMagick import/export filter 

#include "camtypes.h"

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "progress.h"
//#include "docview.h"	// DocView - in camtypes.h [AUTOMATICALLY REMOVED]
#include "imgmgkft.h"
#include "pngutil.h"	// ImageMagick utility class
//#include "oilfltrs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "oilbitmap.h"
//#include "bmpfiltr.h"
//#include "dibutil.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "grndbmp.h"
#include "nodebmp.h"
//#include "wbitmap.h"	// Windows specific bitmap information	 
//#include "andy.h"
//#include "resource.h"	// Inform Warning _R(IDS_OK)
//#include "filtrres.h"	// Filter ids
//#include "will3.h"		// for _R(IDS_GENOPTPALMSGID)
#include "outptpng.h"	// ImageMagick filter type, includes imglib.h 
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "maskfilt.h"	// MaskedFilter class
#include "bmapprev.h"	// tab preview dialog
#include "palman.h"		// PaletteManager::FindFirstDontUseColourInPalette
//#include "mrhbits.h"	//  For CBMPBits::RenderSelectionToBMP
#include "bitfilt.h"
#include "selall.h"		//  For OPTOKEN_EDITSELECTALL

CC_IMPLEMENT_DYNAMIC(ImageMagickFilter, MaskedFilter)
CC_IMPLEMENT_DYNCREATE(ImageMagickExportOptions, MaskedFilterExportOptions)

#define	new	CAM_DEBUG_NEW

OutputPNG 	ImageMagickFilter::DestImageMagick;
FilterType 	ImageMagickFilter::s_FilterType = IMAGEMAGICK;	// Type of filter in use (ImageMagick .. ImageMagick_TRANSINTER)

#if 1

UINT32 ImageMagickExportOptions::g_CompactedFlagsForDefaults = 0;

/********************************************************************************************

>	static BOOL ImageMagickExportOptions::Declare()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Neville)
	Created:	29/10/96
	Returns:	TRUE if successfully declared preferences
				FALSE otherwise
	Purpose:	To declare preferences associated with these export options

********************************************************************************************/
BOOL ImageMagickExportOptions::Declare()
{
	if (Camelot.DeclareSection(_T("Filters"), 10))
		Camelot.DeclarePref( NULL, _T("ExportImageMagicktype"), &g_CompactedFlagsForDefaults, 0, 3 );

	// All ok
	return TRUE;
}

/********************************************************************************************

>	ImageMagickExportOptions::ImageMagickExportOptions(const FilterType FilterID, const StringBase* pFilterName)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Default constructor for a ImageMagickExportOptions object to provide ImageMagick export
				options

********************************************************************************************/
ImageMagickExportOptions::ImageMagickExportOptions(const FilterType FilterID, const StringBase* pFilterName) :
						MaskedFilterExportOptions(_R(IDD_EXPORTBMPOPTS), FilterID, pFilterName)
{
	// just us rats in here
}

/********************************************************************************************

>	virtual BOOL ImageMagickExportOptions::RetrieveDefaults()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	See BitmapExportOptions for interface details
	Notes:		Gets GIF specific preferences

********************************************************************************************/
BOOL ImageMagickExportOptions::RetrieveDefaults()
{
	if (!MaskedFilterExportOptions::RetrieveDefaults())
		return FALSE;

	SetMakeInterlaced(g_CompactedFlagsForDefaults & 1);
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL ImageMagickExportOptions::SetAsDefaults() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Provides additional implementation to set ImageMagick specific options as defaults
	See Also:	BitmapExportOptions::SetAsDefaults()

********************************************************************************************/
BOOL ImageMagickExportOptions::SetAsDefaults() const
{
	if (!MaskedFilterExportOptions::SetAsDefaults())
		return FALSE;

	g_CompactedFlagsForDefaults = WantTransparent() ? 2 : 0;
	g_CompactedFlagsForDefaults |= WantInterlaced() ? 1 : 0;

	return TRUE;
}

#endif

/********************************************************************************************

>	ImageMagickFilter::ImageMagickFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Purpose:	Constructor for an ImageMagickFilter object.  The object should be initialised
				before use.
	SeeAlso:	ImageMagickFilter::Init

********************************************************************************************/
ImageMagickFilter::ImageMagickFilter() : MaskedFilter()
{
	ImportMsgID = _R(IDS_IMPORTMSG_IMAGEMAGICK);
	Flags.CanImport = TRUE;
	Flags.CanExport = TRUE;

	FilterID = IMAGEMAGICK;
	ExportMsgID = _R(IDS_EXPORTMSG_IMAGEMAGICK);			// "Preparing ImageMagick file..."
	ExportingMsgID = _R(IDS_EXPORTINGMSG_IMAGEMAGICK);		// "Exporting ImageMagick file..."

	// Special Mask prepartion stage ID
	Export2ndStageMsgID = _R(IDN_MASKINGMSG_IMAGEMAGICK);	// "Preparing mask for ImageMagick file..."
	ExportRegion = NULL;
	TempFile = NULL;
}

/********************************************************************************************

>	BOOL ImageMagickFilter::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an ImageMagickFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	EPSStack

********************************************************************************************/
BOOL ImageMagickFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new ImageMagickOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDS_IMAGEMAGICK_FILTERNAME));
	FilterInfo.Load(_R(IDS_IMAGEMAGICK_FILTERINFO));

	if (!ImageMagickExportOptions::Declare())
		return FALSE;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	INT32 ImageMagickFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, 
								 UINT32 FileSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Inputs:		Filename - name of the file.
				HeaderStart - Address of the first few bytes of the file.
				HeaderSize - the number of bytes in the header pointed to by FileStart.
				FileSize - the size of the whole file, in bytes.
	Returns:	0 => Not a ImageMagick file.
				10 => It is a ImageMagick file.
	Purpose:	Determine if this filter can load the specified file.

********************************************************************************************/
INT32 ImageMagickFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, 
							 UINT32 FileSize)
{
	// We need to remember what we thought of this file in our class variable.
	// So, set it to a nice default value at the start.
	ImageMagickHowCompatible = ((Filename.GetType() == _T("miff")) ||
							    (Filename.GetType() == _T("miff")) ) ? 10:0;

	// Return the found value to the caller.
	return ImageMagickHowCompatible;
}

/********************************************************************************************

>	virtual BOOL ImageMagickFilter::IsThisBppOk(UINT32 Bpp)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Inputs:		Bpp or Colour depth.
	Returns:	TRUE if this filter can cope with this colour depth, FALSE otherwise.
	Purpose:	Check if this Bitmap filter can cope with saving at this Bpp/Colour depth.
	SeeAlso:	OpMenuExport::DoWithParam;

********************************************************************************************/
BOOL ImageMagickFilter::IsThisBppOk(UINT32 Bpp)
{
	return (Bpp == 1 || Bpp == 4 || Bpp == 8 || Bpp == 24 || Bpp == 32);
}

/********************************************************************************************

>	BOOL ImageMagickFilter::ReadFromFile( OILBitmap* pOilBitmap, BaseCamelotFilter* pFilter,
								  CCLexFile* pFile, BOOL IsCompressed)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/96
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
BOOL ImageMagickFilter::ReadFromFile( OILBitmap* pOilBitmap, BaseCamelotFilter* pFilter,
							  CCLexFile* pFile, BOOL IsCompressed)
{
	ERROR2IF(pOilBitmap == NULL,FALSE,"BMPFilter::ReadFromFile null OilBitmap pointer");
	ERROR2IF(pFilter == NULL,FALSE,"BMPFilter::ReadFromFile null pFilter pointer");
	ERROR2IF(pFile == NULL,FALSE,"BMPFilter::ReadFromFile null pFile pointer");

	// Try to import bitmap as usual binary BMP file.
	CWxBitmap* pWBitmap = (CWxBitmap*)pOilBitmap;
	
	LPBITMAPINFO *pInfo = &(pWBitmap->BMInfo);
	LPBYTE *pBytes = &(pWBitmap->BMBytes);

	INT32 TransColour = -1;

	// Read from file,using pFilter for progress bar updates
	if (!PNGUtil::ReadFromFile(pFile, pInfo, pBytes, &TransColour, NULL, pFilter))
		return FALSE;

	if(pWBitmap->BMInfo->bmiHeader.biBitCount == 32)
	{
		// If we`re exporting a 32Bit BMP then we need to make sure that we convert the
		// Alpha channel to Transparency! i.e. invert it!
		UINT32 BmpSize = pWBitmap->BMInfo->bmiHeader.biSizeImage;
		BYTE* Bits = pWBitmap->BMBytes;

		for(UINT32 i = 0; i < BmpSize; i+=4)
			Bits[i+3] = ~Bits[i+3];
	}

	// Everything went ok and we imported the bitmap ok
	SetTransColour(TransColour);
	UINT32 Bpp = pWBitmap->GetBPP();
	if (TransColour != -1 && Bpp <= 8)
		pOilBitmap->SetTransparencyIndex(TransColour);

	SetLastBitmap();		// can only import one bitmap at the moment
	return TRUE;
}

/********************************************************************************************

>	BOOL ImageMagickFilter::ReadFromFile(OILBitmap* pOilBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Inputs:		pOilBitmap	pointer to the oil bitmap data to be filled in
	Outputs:	Will have filled in BMInfo	pointer to the bitmap header to fill in
									BMBytes	pointer to the bitmap data to fill in
	Purpose:	Actually does the process of reading a bitmap from a file.
				Inherited classes override this to read in different file formats.
				
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/
BOOL ImageMagickFilter::ReadFromFile(OILBitmap* pOilBitmap)
{
	ERROR2IF(pOilBitmap == NULL,FALSE,"ImageMagickFilter::ReadFromFile null OilBitmap pointer");
	
	// Try to import the bitmap as a ImageMagick file.		
	CCLexFile *pImportFile = GetImportFile();
	ERROR2IF(pImportFile==NULL,FALSE,"ImageMagickFilter::ReadFromFile - No import file");

	UINT32 ImportMsgId = GetImportMsgID();		
	String_64 ProgressString(ImportMsgId);
	ProgressString = GetImportProgressString(pImportFile, ImportMsgId);

	CWxBitmap* pWBitmap = (CWxBitmap*)pOilBitmap;
	
	LPBITMAPINFO *pInfo = &(pWBitmap->BMInfo);
	LPBYTE *pBytes = &(pWBitmap->BMBytes);
	
	INT32 TransColour = -1;

	// The ImageMagick filter liked it very much and so use it, showing progress bar
	if (!PNGUtil::ReadFromFile(pImportFile, pInfo, pBytes, &TransColour, &ProgressString))
		return FALSE;

	SetTransColour(TransColour);
	UINT32 Bpp = pWBitmap->GetBPP();
	if (TransColour != -1 && Bpp <= 8)
		pOilBitmap->SetTransparencyIndex(TransColour);

	SetLastBitmap();		// can only import one bitmap at the moment
	
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL ImageMagickFilter::GetExportOptions(BitmapExportOptions* pOptions)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/96
	Purpose:	See BaseBitmapFilter for interface details

********************************************************************************************/
BOOL ImageMagickFilter::GetExportOptions(BitmapExportOptions* pOptions)
{
	ERROR2IF(pOptions == NULL, FALSE, "NULL Args");

	ImageMagickExportOptions* pImageMagickOptions = (ImageMagickExportOptions*)pOptions;
	ERROR3IF(!pImageMagickOptions->IS_KIND_OF(ImageMagickExportOptions), "pImageMagickOptions isn't");

	// the depth we ask GDraw to render is always 32-bit, so we can get transparency
	// we have to convert for other formats	when writing the actual bytes to the file
	SetDepthToRender(32);

	// We haven't written the header yet
	WrittenHeader = FALSE;

	// We are a first pass render and not doing the mask, by default
	SecondPass = FALSE;
	DoingMask = FALSE;

	// Determine the filter type currently in use in Accusoft format
	s_FilterType = IMAGEMAGICK;
	pImageMagickOptions->SetFilterType(IMAGEMAGICK);

	BOOL Ok = FALSE;

	OpDescriptor* pOpDes = OpDescriptor::FindOpDescriptor(OPTOKEN_GIFTABDLG);
	if (pOpDes != NULL)
	{
		// set up the data for the export options dialog
		OpParam Param((void *)pOptions, (void *)this);

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
		ERROR3("Unable to find OPTOKEN_BMAPPREVDLG");
	} 

	// Return with the ok/cancel state used on the dialog box
	return Ok;
}

// SMFIX sjk 5/12/00 there used to be some junk in the call to GetExportOptions that assumed the
// filter type being used which could be changed by the GetExportOptions call itself
// therefore all this sort of stuff should be called on the correct known filter using this
// call afterwards
void ImageMagickFilter::PostGetExportOptions(BitmapExportOptions* pOptions)
{
	// should be of this type
	ImageMagickExportOptions* pImageMagickOptions = (ImageMagickExportOptions*)pOptions;
	ERROR3IF(!pImageMagickOptions->IS_KIND_OF(ImageMagickExportOptions), "pImageMagickOptions isn't");

	// do the baseclass options
	MaskedFilter::PostGetExportOptions(pOptions);

	// do the specific to this class options
	// Filter type can be changed by the export options dialog box from say 
	// ImageMagick to ImageMagick_INTERLACED
	UINT32 Silliness = pImageMagickOptions->WantTransparent() ? 2 : 0;
	Silliness |= pImageMagickOptions->WantInterlaced() ? 1 : 0;
	if (Silliness >= 0 && Silliness <= 4)
	{
		Compression = Silliness;
		// Compression ranges from 0 .. 4 so map this onto our filter types
//		s_FilterType = ImageMagick + Silliness;
		switch (Silliness)
		{
		case 0:	s_FilterType = IMAGEMAGICK; break;
		case 1:	s_FilterType = IMAGEMAGICK_INTERLACED; break;
		case 2:	s_FilterType = IMAGEMAGICK_TRANSPARENT; break;
		case 3:	s_FilterType = IMAGEMAGICK_TRANSINTER; break;
		}

		if (pImageMagickOptions->WantTransparent() && pImageMagickOptions->GetSelectionType() == SELECTION)
			DoingMask = TRUE;
	}
}

/********************************************************************************************

>	virtual void ImageMagickFilter::CleanUpAfterExport()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/96
	Purpose:	Cleans up the memory allocated at the end of Exporting or when exporting has
				been aborted for some reason. Does its cleaning up and then calls the
				baseclass version to do its stuff,  - used
				when the import process ends, either normally or abnormally. Override if
				extra things are required.
	SeeAlso:	BaseBitmapFilter::PrepareToExport(); BaseBitmapFilter::CleanUpAfterExport();
	Scope: 		Protected

********************************************************************************************/
void ImageMagickFilter::CleanUpAfterExport()
{
	// Called right at the end of the export process or when the epxort has been aborted
	// Clean up any objects unique to this class.
	// Free up any DIBs that we might have left lying around on the export
	if (pDestBMInfo && pDestBMBytes)
	{
		FreeDIB( pDestBMInfo, pDestBMBytes );
		pDestBMInfo = NULL;
		pDestBMBytes = NULL;	
	}

	// the depth we ask GDraw to render is always 32-bit, so we can get transparency
	// we have to convert for other formats	when writing the actual bytes to the file
	SetDepthToRender(32);

	// We haven't written the header yet
	WrittenHeader = FALSE;

	// We are a first pass render and not doing the mask, by default
	SecondPass = FALSE;
	DoingMask = FALSE;

	// Now call the baseclass version to do its stuff
	BaseBitmapFilter::CleanUpAfterExport();
}

/********************************************************************************************

>	virtual UINT32 ImageMagickFilter::GetExportMsgID()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Returns:	The id of the message to put on the progress display whilst exporting.
	Purpose:	Used to get the message id to be used during export.
				Overides the baseclass form of the function so that during the two stage
				export process it can change the message.
	SeeAlso:	DoExport; TI_GIFFilter::GetExportMsgID;

********************************************************************************************/
UINT32 ImageMagickFilter::GetExportMsgID()
{
	if (GeneratingOptimisedPalette())
		return _R(IDS_GENOPTPALMSGID);				// "Generating optimised palette..."

	ImageMagickExportOptions* pImageMagickOptions = (ImageMagickExportOptions*)GetBitmapExportOptions();
	ERROR2IF(pImageMagickOptions == NULL, FALSE, "NULL Args");
	ERROR3IF(!pImageMagickOptions->IS_KIND_OF(ImageMagickExportOptions), "pImageMagickOptions isn't");

	// If we are exporting with transparency on and on first pass use the masking message
	// otherwise use the exporting message.
	if (pImageMagickOptions->GetSelectionType() == SELECTION && pImageMagickOptions->WantTransparent())
	{
		// Special 4 stage rendering operation
		// - Render selected objects to white background
		// - Render mask 1bpp
		// - Render all objects
		// - Save data out to disk
		if (!SecondPass)
			return Export2ndStageMsgID;			// "Preparing mask for ImageMagick file..."
		else
			return Filter::GetExportMsgID();	// "Preparing ImageMagick file..."
	}
	else
	{
		// Special 3 stage rendering operation
		// - Render objects to white background
		// - Render mask 1bpp
		// - Save data out to disk
		if (DoingMask)
			return Export2ndStageMsgID;			// "Preparing mask for ImageMagick file..."
		else
			return Filter::GetExportMsgID();	// "Preparing ImageMagick file..."
	}

	return ExportingMsgID;
}

/********************************************************************************************

>	BOOL ImageMagickFilter::EndWriteToFile( )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/96
	Inputs:		-
	Purpose:	Cleans up after writing the bitmap data out to a file. Inherited classes
				override this to write in different file formats.
				This is slightly different to most other bitmap filters in that it is here
				that the data actually gets written out to file, after doing the transparency
				translation, if required.
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/
BOOL ImageMagickFilter::EndWriteToFile()
{
	if (GeneratingOptimisedPalette())
		return TRUE;		// No need to output anything

	//  Can reset the band number now.
	m_BandNumber = 0;

	ImageMagickExportOptions* pImageMagickOptions = (ImageMagickExportOptions*)GetBitmapExportOptions();
	ERROR2IF(pImageMagickOptions == NULL, FALSE, "NULL Args");
	ERROR3IF(!pImageMagickOptions->IS_KIND_OF(ImageMagickExportOptions), "pImageMagickOptions isn't");

	// Do the transparency translation just before we write out the data as a ImageMagick.
	// This involves doing a 1 bpp export of the same area and using this to work
	// out which areas are transparent or not.
	// Only do this if the user has requested transparency and we outputting at 8bpp
	BOOL SaveDataOut = TRUE;

	if (BadExportRender)
	{
		// Delete our whitearea bitmap
		if (pTempBitmapMask != NULL)
			CCFree(pTempBitmapMask);

		pTempBitmapMask = NULL;
	}

	BOOL ok=FALSE;

	// Save the data out if required. Only if we exported ok.
	if (SaveDataOut && !BadExportRender)
	{
		ok = CreateTempFile();
	
		if (ok)
		{
			// Now that we know the transparent index we can output the ImageMagick header
			ok = DestImageMagick.OutputPNGHeader(TempFile, NULL, pImageMagickOptions->WantInterlaced(),
										pImageMagickOptions->GetTransparencyIndex(),
										pImageMagickOptions->GetDepth() <= 8 ? pImageMagickOptions->GetLogicalPalette() : NULL);
		}

		// Actually write the destination bitmap out to the file showing an hourglass
		// and/or progress bar as we go. Always show the Exporting message.
		// Need to do in one go due to interlacing
		if (ok)
		{
			String_64 ProgressString(ExportingMsgID);
			ProgressString = GetExportProgressString(TempFile, ExportingMsgID);
			BeginSlowJob(100, FALSE, &ProgressString);
			
			ok = DestImageMagick.OutputPNGBits(TempFile, DestImageMagick.GetDestBitmapBits());
			DestImageMagick.TidyUp();
			if (ok)
				ok=ProcessTempFile(OutputFile);

			EndSlowJob();
		}
		else
		{
			DestImageMagick.TidyUp();
		}
	}
	else
	{
		DestImageMagick.TidyUp();
	}


	ERROR1IF(!ok, FALSE, _R(IDE_IMAGEMAGICK_ERROR));

	TidyTempFile();
	
	return TRUE;
}

/********************************************************************************************

>	static BOOL ImageMagickFilter::WriteDataToFile( BOOL End, UINT32 Bpp, UINT32 Compression)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/96
	Inputs:		End - TRUE if this is the last block of the file.
				Bpp - output depth in terms of bits per pixel
				Compression - usually True if compression required, False otherwise
							  In the ImageMagick case this is used to pass in the transparency and
							  interlace state.
	Returns:	TRUE if worked, FALSE if errored.
	Purpose:	Physically put the bitmap into the disk.
				NOTE - ONLY COPES WITH End=TRUE currently
				AtEnd is ignored now and should always be set to TRUE.
				Unused at present due to static problems when cretaing the 1bpp bitmap. 
	SeeAlso:	WriteToFile(); AccusoftFilters::WriteToFile; AccusoftFilters::WriteDataToFile;

********************************************************************************************/
BOOL ImageMagickFilter::WriteDataToFile( BOOL End, UINT32 Bpp, UINT32 Compression)
{
	ERROR2(FALSE,"ImageMagickFilter::WriteDataToFile called when not implemented");
}

/********************************************************************************************

>	virtual BOOL ImageMagickFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Inputs:		Pointer to the bitmap to save.
				Dpi of the bitmap to be saved
	Returns:	TRUE if worked, FALSE if errored.
	Purpose:	Physically put the bitmap into the disk.  Inherited classes override this to write
				in different file formats.
	SeeAlso:	WriteDataToFile(); AccusoftFilters::WriteToFile; AccusoftFilters::WriteDataToFile;

********************************************************************************************/
BOOL ImageMagickFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi)
{
	ERROR2IF(pKernelBitmap == NULL,FALSE,"ImageMagickFilter::WriteBitmapToFile null bitmap pointer specified");

	// Get a pointer to the actual bitmap so that we can get some details from it.
	OILBitmap *pOilBitmap = pKernelBitmap->ActualBitmap;
	ERROR2IF(pOilBitmap == NULL,FALSE,"ImageMagickFilter::WriteBitmapToFile null oil bitmap pointer");

	// Now get the pointer to the info header and actual bits data.
	// Need to use the actual bitmap pointer
	CWxBitmap* pWBitmap = (CWxBitmap*)pOilBitmap;
	LPBITMAPINFO pInfo = pWBitmap->BMInfo;
	LPBYTE pBytes = pWBitmap->BMBytes;
//	UINT32 Bpp = pWBitmap->GetBPP();

	// Now, save the data out showing the correct progress string
	String_64 ProgressString(ExportingMsgID);
	BOOL ok = FALSE;
//	BOOL Interlace = TRUE;	// Use interlace or not
//	BOOL Transparency = FALSE;	// Use transparency or not

	if(pInfo->bmiHeader.biBitCount == 32)
	{
		// If we`re exporting a 32Bit BMP then we need to make sure that we convert the
		// Alpha channel to Transparency! i.e. invert it!
		UINT32 BmpSize = pInfo->bmiHeader.biSizeImage;

		for(UINT32 i = 0; i < BmpSize; i+=4)
			pBytes[i+3] = ~pBytes[i+3];
	}

	ok = WriteToFile(OutputFile, pInfo, pBytes, &ProgressString);

	//  This function is used when saving from the bitmap gallery. If we save a 32-bit bitmap as
	//  a ImageMagick, then we have to undo the alpha channel reversing that we did above.
	//  Failure to do this will change the bitmap displayed in the bitmap gallery.
	if(pInfo->bmiHeader.biBitCount == 32)
	{
		UINT32 BmpSize = pInfo->bmiHeader.biSizeImage;

		for(UINT32 i = 0; i < BmpSize; i+=4)
			pBytes[i+3] = ~pBytes[i+3];
	}
	
	return ok;
}

/********************************************************************************************

>	virtual BOOL ImageMagickFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap,
											  BaseCamelotFilter*pFilter,
											  CCLexFile* pFile, INT32 Compression);
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/96
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
BOOL ImageMagickFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, BaseCamelotFilter* pFilter,
										 CCLexFile* pFile, INT32 Compression)
{
	ERROR2IF(pKernelBitmap == NULL,FALSE, "ImageMagickFilter::WriteBitmapToFile null pKernelBitmap");
	ERROR2IF(pFilter == NULL,FALSE, "ImageMagickFilter::WriteBitmapToFile null pFilter");
	ERROR2IF(pFile == NULL,FALSE, "ImageMagickFilter::WriteBitmapToFile null pFile");

	// Get a pointer to the actual bitmap so that we can get some details from it.
	OILBitmap *pOilBitmap = pKernelBitmap->ActualBitmap;
	ERROR2IF(pOilBitmap == NULL,FALSE,"ImageMagickFilter::WriteBitmapToFile null oil bitmap pointer");

	// Now get the pointer to the info header and actual bits data.
	// Need to use the actual bitmap pointer
	CWxBitmap* pWBitmap = (CWxBitmap*)pOilBitmap;
	LPBITMAPINFO Info = pWBitmap->BMInfo;
	LPBYTE Bytes = pWBitmap->BMBytes;
	UINT32 Bpp = pWBitmap->GetBPP();

	// Now, save the data out showing the correct progress string
	BOOL ok = FALSE;
	BOOL Interlace = FALSE;		// Must not use interlacing as it will screw the progress bar updates
//	BOOL Transparency = FALSE;	// Use transparency or not
	INT32 Transparent = -1;	// colour or -1 = no transparency
	if (Bpp <= 8)
		pOilBitmap->GetTransparencyIndex(&Transparent);
	
	if(Info->bmiHeader.biBitCount == 32)
	{
		// If we`re exporting a 32Bit BMP then we need to make sure that we convert the
		// Alpha channel to Transparency! i.e. invert it!
		UINT32 BmpSize = Info->bmiHeader.biSizeImage;

		for(UINT32 i = 0; i < BmpSize; i+=4)
			Bytes[i+3] = ~Bytes[i+3];
	}
	
	// Write to file, no header and using pFilter for progress bar updates
	ok = WriteToFile(pFile, Info, Bytes, Interlace, Transparent, pFilter);
	
	return ok;
}

/********************************************************************************************

>	static BOOL ImageMagickFilter::WriteToFile ( CCLexFile* File, LPBITMAPINFO Info, LPBYTE Bits,
										 BOOL Interlace, INT32 Transparent,
										 BaseCamelotFilter* pFilter = NULL )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/7/96
	Inputs:		File		An opened CCFile that can be written to. It should be positioned at the
							start. Caller is responsible for closing it. The file needs to be in
							Binary mode.
				Info		BITMAPINFO structure for the dib.
				Bits		The bitmap data itself
				Interlace	allows interlacing to be turned off or on
				Transparent if -1 then no transpparent colour, if not -1 then specifies the index
							number of the transparent colour in the palette
				pFilter		is an alternative way of handling the progress bar, assume the
							progress bar has been start and just call the IncProgressBarCount in BaseCamelotFilter
							to do the progress bar update. Defaults to NULL i.e. no progress bar.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Write a bitmap in memory straight out as a ImageMagick to file with no rendering or
				conversion between different colour depths (apart from 32 to 24) or resolution.
				***Errors on 16-bit builds***
				A progress hourglass can be shown if required using the BaseCamelotFilter as
				the controlling influence.
				
				This function is used by the new native/web  file format to do a straight save of 
				the data into the file.

				(caller should close file)
	Errors:		Calls SetError on FALSE returns.
	Scope:		Static
	SeeAlso:	AccusoftFilters::WriteToFile; DIBUtil::WriteToFile;

********************************************************************************************/
BOOL ImageMagickFilter::WriteToFile( CCLexFile* File, LPBITMAPINFO Info, LPBYTE Bits,
							 BOOL Interlace, INT32 TransparentColour,
							 BaseCamelotFilter* pFilter )
{
#ifdef DO_EXPORT
	ERROR2IF(File==NULL,FALSE,"ImageMagickFilter::WriteToFile File pointer is null");
	ERROR2IF(Info==NULL,FALSE,"ImageMagickFilter::WriteToFile BitmapInfo pointer is null");
	ERROR2IF(Bits==NULL,FALSE,"ImageMagickFilter::WriteToFile Bits pointer is null");

	// BITMAPINFO  consists of:-
	//    	BITMAPINFOHEADER    bmiHeader;
	//		RGBQUAD             bmiColors[1];
	LPBITMAPINFOHEADER pInfoHeader = &Info->bmiHeader;
	ERROR2IF(pInfoHeader==NULL,FALSE,"ImageMagickFilter::WriteToFile BitmapInfoHeader pointer is null");
		
	LPRGBQUAD pPalette = &(Info->bmiColors[0]);
	ERROR2IF(pPalette==NULL,FALSE,"ImageMagickFilter::WriteToFile palette pointer is null");

	// Set up our format type flags.
	if(Info->bmiHeader.biBitCount == 32)
	{
		// If we`re exporting a 32Bit BMP then we need to make sure that we convert the
		// Alpha channel to Transparency! i.e. invert it!
		UINT32 BmpSize = Info->bmiHeader.biSizeImage;

		for(UINT32 i = 0; i < BmpSize; i+=4)
			Bits[i+3] = ~Bits[i+3];
	}

	if (CreateTempFile())
	{
		// Output a ImageMagick header for this file, using the RGBQUAD palette rather than a LOGPALETTE
		DestImageMagick.OutputPNGHeader(TempFile, pInfoHeader, Interlace, TransparentColour, NULL, pPalette);
	
		// Now write out the bitmap data itself.
		DestImageMagick.OutputPNGBits(TempFile, Bits, TRUE, pFilter);
		// The above has set the OutputFile member variable of DestImageMagick. We desperately need to
		// reset this as otherwise the next bitmap export may go wrong as it calls the tidy up
		// and so will refer to the deleted CCFile. Oh Er!
		DestImageMagick.TidyUp();
		ProcessTempFile(File);
	}
	else
	{
		DestImageMagick.TidyUp();
	}

	TidyTempFile();

	// er, we seem to have finished OK so say so
	return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	static BOOL ImageMagickFilter::WriteToFile ( CCLexFile* File, LPBITMAPINFO Info, LPBYTE Bits,
										 String_64* ProgressString = NULL )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Inputs:		File	An opened CCFile that can be written to. It should be positioned at the
						start. Caller is responsible for closing it. The file needs to be in
						Binary mode.
				Info	BITMAPINFO structure for the dib.
				Bits	The bitmap data itself
				ProgressString allows the user to specify whether they require a progress
						hourglass or not. If NULL then none is shown, otherwise an progress	bar 
						is shown using the text supplied. Defaults to NULL i.e. no progress bar.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Write a bitmap in memory straight out as a ImageMagick to file with no rendering or
				conversion between different colour depths (apart from 32 to 24) or resolution.
				***Errors on 16-bit builds***
				A progress hourglass can be shown if required.
				This function is used by the save bitmap button on the bitmap gallery. All
				other bitmap export uses the OutputDIB class instead as this copes with using
				a render region and converting from 32 to the destination format.
				(caller should close file)
	Errors:		Calls SetError on FALSE returns.
	Scope:		Static
	SeeAlso:	AccusoftFilters::WriteToFile; DIBUtil::WriteToFile;

********************************************************************************************/
BOOL ImageMagickFilter::WriteToFile( CCLexFile* File, LPBITMAPINFO Info, LPBYTE Bits,
							 String_64* ProgressString )
{
#ifdef DO_EXPORT

	ERROR2IF(File==NULL,FALSE,"ImageMagickFilter::WriteToFile File pointer is null");
	ERROR2IF(Info==NULL,FALSE,"ImageMagickFilter::WriteToFile BitmapInfo pointer is null");
	ERROR2IF(Bits==NULL,FALSE,"ImageMagickFilter::WriteToFile Bits pointer is null");

	// If the caller has specified a string then assume they require a progress bar
	// Start it up.
	if (ProgressString != NULL)
		BeginSlowJob(100, FALSE, ProgressString);

	// BITMAPINFO  consists of:-
	//    	BITMAPINFOHEADER    bmiHeader;
	//		RGBQUAD             bmiColors[1];
	LPBITMAPINFOHEADER pInfoHeader = &Info->bmiHeader;
	ERROR2IF(pInfoHeader==NULL,FALSE,"ImageMagickFilter::WriteToFile BitmapInfoHeader pointer is null");
		
	LPRGBQUAD pPalette = &(Info->bmiColors[0]);
	ERROR2IF(pPalette==NULL,FALSE,"ImageMagickFilter::WriteToFile palette pointer is null");

	// Set up our format type flags.
	BOOL Interlace = TRUE;	// Use interlace or not
	INT32 Transparent = -1;	// colour or -1 = no transparency
	BOOL WantTransparent = FALSE;

	switch (s_FilterType)
	{
		default:
		case IMAGEMAGICK:
			Interlace 		= FALSE;
			WantTransparent = FALSE;
			break;
		case IMAGEMAGICK_INTERLACED:
			Interlace 		= TRUE;
			WantTransparent = FALSE;
			break;
		case IMAGEMAGICK_TRANSPARENT:
			Interlace 		= FALSE;
			WantTransparent = TRUE;
			break;
		case IMAGEMAGICK_TRANSINTER:
			Interlace 		= TRUE;
			WantTransparent = TRUE;
			break;
	}

	if (WantTransparent)
	{
		// We want to try and output the transparency if possible ...

		// Scan through the palette, and try and find an index with
		// the transparency flag set

		INT32 cols = Info->bmiHeader.biClrUsed;
		// If we have zero colours on a bitmap which is 8bpp or less then this is bad.
		// This should be translated as the maximum number of colours allowed
		if (Info->bmiHeader.biBitCount <= 8 && cols == 0)
			cols = 1 << Info->bmiHeader.biBitCount;

		for (INT32 i = 0; i < cols; i++)
		{
			if (Info->bmiColors[i].rgbReserved == 0xFF)
			{
			 	Transparent = i;
				TRACEUSER( "Neville", _T("ImageMagick output with transp index of %d\n"),Transparent);
				break;
			}		
		}	
	}

	BOOL ok = CreateTempFile();

	if (ok)
	{
		// Output the ImageMagick data
		// Output a ImageMagick header for this file, using the RGBQUAD palette rather than a LOGPALETTE
		if (Transparent == -1)
			ok = DestImageMagick.OutputPNGHeader(TempFile, pInfoHeader, Interlace, -1, NULL, pPalette);
		else
			ok = DestImageMagick.OutputPNGHeader(TempFile, pInfoHeader, Interlace, Transparent, NULL, pPalette);
	}

	// Now write out the bitmap data itself.
	if (ok)
		ok = DestImageMagick.OutputPNGBits(TempFile, Bits, TRUE);

	// Tidy up here anyway
	DestImageMagick.TidyUp();

	// process it
	if (ok)
		ok = ProcessTempFile(File);

	// If started, then stop then progress bar
	if (ProgressString != NULL)
		EndSlowJob();

	TidyTempFile();

	ERROR1IF(!ok, FALSE, _R(IDE_IMAGEMAGICK_ERROR));

	// er, we seem to have finished OK so say so
	return TRUE;
#endif
}

/********************************************************************************************

>	virtual BOOL ImageMagickFilter::WritePreFrame(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out any frame specific info before the image
	SeeAlso:	GIFFilter::WritePreFrame(void)

********************************************************************************************/
BOOL ImageMagickFilter::WritePreFrame(void)
{
	return DestImageMagick.ReStartFile(NULL);
}

/********************************************************************************************

>	virtual BOOL ImageMagickFilter::WriteFrame(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out the image itself
				This base class version actually calls the WriteToFile() function so that
				derived classes do not have to implement any of the multi-image stuff
	SeeAlso:	GIFFilter::WriteFrame(void)

********************************************************************************************/
BOOL ImageMagickFilter::WriteFrame(void)
{
	return MaskedFilter::WriteToFile(TRUE);
}


/********************************************************************************************

>	virtual BOOL ImageMagickFilter::WritePostFrame(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out any frame specific info after the image
	SeeAlso:	GIFFilter::WritePostFrame(void)

********************************************************************************************/
BOOL ImageMagickFilter::WritePostFrame(void)
{
	return EndWriteToFile();
}

/********************************************************************************************

>	virtual BOOL ImageMagickFilter::WriteFileEnd(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out the file specific data at the end of the file
				This base class version calls EndWriteToFile() so that derived classes
				do not have to implement the multi-image stuff
	SeeAlso:	GIFFilter::WriteFileEnd(void)

********************************************************************************************/
BOOL ImageMagickFilter::WriteFileEnd(void)
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL ImageMagickFilter::WritePreSecondPass(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	Called to do any processing required after the first and before the second
				pass of a two pass export
	SeeAlso:	GIFFilter::WritePreSecondPass(void)

********************************************************************************************/
BOOL ImageMagickFilter::WritePreSecondPass(void)
{
	return EndWriteToFile();
}

/********************************************************************************************

>	virtual void ImageMagickFilter::InvertAlpha ( LPBITMAPINFO	lpBitmapInfo,
										  LPBYTE		lpBits )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/00
	Purpose:	Camelot uses a different transparency scheme to the rest of the world, in
				that 255 is clear, and 0 is opaque. Until the rest of the world catches up,
				it's necessary to invert the alpha channel to make exported files compatible
				with other programs.

********************************************************************************************/
void ImageMagickFilter::InvertAlpha ( LPBITMAPINFO	lpBitmapInfo,
							  LPBYTE		lpBits )
{
	DIBUtil::InvertAlpha(lpBitmapInfo, lpBits);
}

/********************************************************************************************

>	virtual OutputDIB* ImageMagickFilter::GetOutputDIB ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/00
	Returns		OutputDIB* - A pointer to DestImageMagick.
	Purpose:	Casts the current output DIB to be a generic OutputDIB class. This allows the
				same code to be re-used in the base class.

********************************************************************************************/
OutputDIB* ImageMagickFilter::GetOutputDIB ( void )
{
	// Perform an upcast to allow the pointer to be used in a generic manner.
	return static_cast<OutputDIB*> ( &DestImageMagick );
}

/********************************************************************************************

>	virtual BitmapExportOptions* ImageMagickFilter::CreateExportOptions() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	See BaseBitmapFilter for interface details
	Notes:		Provides a new ImageMagickExportOptions instance

********************************************************************************************/
BitmapExportOptions* ImageMagickFilter::CreateExportOptions() const
{
	ImageMagickExportOptions* pImageMagickOptions = new ImageMagickExportOptions(IMAGEMAGICK, &FilterName);
	return (BitmapExportOptions*)pImageMagickOptions;
}

void ImageMagickFilter::AlterPaletteContents( LPLOGPALETTE pPalette )
{
	PORTNOTETRACE("filters","ImageMagickFilter::AlterPaletteContents - do nothing");
//	DestImageMagick.AlterExportPalette( pPalette );
}


/********************************************************************************************

>	BOOL ImageMagickFilter::CreateTempFile()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	18/07/2006
	Purpose:	Create a temporary file
	Inputs:		None
	Outputs:	None
	Returns:	TRUE on success, FALSE on error
	Notes:		-

********************************************************************************************/

BOOL ImageMagickFilter::CreateTempFile()
{
	if (TempFile)
		delete TempFile;

	TempFile = new CCDiskFile;
	if (!TempFile)
		return FALSE;

	wxFile dummyFile; // to prevent deletion race condition
	TempFileName = wxFileName::CreateTempFileName(wxEmptyString, &dummyFile);
	PathName pthFileName=String_256(TempFileName);
	
	if (!(TempFile->open(pthFileName, ios::out | ios::trunc | ios::binary)))
	{
		::wxRemoveFile(TempFileName);
		ERROR1(FALSE, _R(IDE_IMAGEMAGICK_ERROR));
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL ImageMagickFilter::ProcessTempFile(CCLexFile * File)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	18/07/2006
	Purpose:	Process the temporary file by calling ImageMagick
	Inputs:		file - the CCLexFile for the final file
	Outputs:	None
	Returns:	TRUE on success, FALSE on error
	Notes:		-

********************************************************************************************/

BOOL ImageMagickFilter::ProcessTempFile(CCLexFile * File)
{
	PathName OutputPath = File->GetPathName();
	ERROR2IF(!OutputPath.IsValid(), FALSE, "ImageMagickFilter::WriteToFile can only be used on real files");

	ERROR2IF(!TempFile || TempFileName.IsEmpty(), FALSE, "ImageMagickFilter::ProcessTempFile has no temporary file to process");
	TempFile->close();

	wxChar * cifn;
	wxChar * cofn;
	wxChar * pcommand=_T("/usr/bin/convert");
	wxChar * IMargv[4];

	// get filename in usable form
	cifn = camStrdup(wxString(_T("png:"))+TempFileName );
	cofn = camStrdup((const TCHAR *)(OutputPath.GetPath()));

	// Now convert the file
	IMargv[0]=pcommand;
	IMargv[1]=cifn;
	IMargv[2]=cofn;
	IMargv[3]=NULL;
	long /*TYPENOTE: Correct*/ ret = ::wxExecute((wxChar **)IMargv, wxEXEC_SYNC);
	
	free(cifn);
	free(cofn);

	if (ret)
	{
		TidyTempFile();
		::wxRemoveFile(wxString((const TCHAR *)(OutputPath.GetPath())));
		ERROR1(FALSE, _R(IDE_IMAGEMAGICK_ERROR));
	}

	TidyTempFile(); // ensures filename zapped so it isn't removed later

	return TRUE;		
}

/********************************************************************************************

>	BOOL ImageMagickFilter::TidyTempFile(BOOL Delete=TRUE)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	18/07/2006
	Purpose:	Closes any temporary file, and potentially removes it
	Inputs:		None
	Outputs:	None
	Returns:	TRUE on success, FALSE on error
	Notes:		-

********************************************************************************************/

BOOL ImageMagickFilter::TidyTempFile(BOOL Delete/*=TRUE*/)
{
	if (TempFile)
	{
		delete (TempFile);
		TempFile = NULL;
	}

	if (!TempFileName.IsEmpty())
	{
		if (Delete)
			::wxRemoveFile(TempFileName);
		TempFileName = wxEmptyString;
	}
	return TRUE;
}
