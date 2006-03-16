// $Id: pngfiltr.cpp 662 2006-03-14 21:31:49Z alex $
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
//#include "progress.h"
#include "docview.h"	// DocView
#include "pngfiltr.h"
#include "pngutil.h"	// PNG utility class
#include "oilfltrs.h"
#include "oilbitmap.h"
//#include "bmpfiltr.h"
#include "dibutil.h"
#include "grndbmp.h"
#include "nodebmp.h"
//#include "wbitmap.h"	// Windows specific bitmap information	 
//#include "andy.h"
//#include "resource.h"	// Inform Warning _R(IDS_OK)
//#include "filtrres.h"	// Filter ids
//#include "will3.h"		// for _R(IDS_GENOPTPALMSGID)
#include "outptpng.h"	// PNG filter type, includes imglib.h 
#include "fixmem.h"
#include "maskfilt.h"	// MaskedFilter class
#include "bmapprev.h"	// tab preview dialog
#include "palman.h"		// PaletteManager::FindFirstDontUseColourInPalette
//#include "mrhbits.h"	//  For CBMPBits::RenderSelectionToBMP
#include "bitfilt.h"
#include "selall.h"		//  For OPTOKEN_EDITSELECTALL

CC_IMPLEMENT_DYNAMIC(PNGFilter, MaskedFilter)
PORTNOTE("filters","Removed PNGExportOptions implementation")
#ifndef EXCLUDE_FROM_XARALX
CC_IMPLEMENT_DYNCREATE(PNGExportOptions, MaskedFilterExportOptions)
#endif

#define	new	CAM_DEBUG_NEW

PORTNOTE("filters","Removed PNGFilter::DestPNG definition")
#ifndef EXCLUDE_FROM_XARALX
OutputPNG 	PNGFilter::DestPNG;
#endif
INT32 		PNGFilter::FilterType = PNG;	// Type of filter in use (PNG .. PNG_TRANSINTER)

#if 0

UINT32 PNGExportOptions::g_CompactedFlagsForDefaults = 0;

/********************************************************************************************

>	static BOOL PNGExportOptions::Declare()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Neville)
	Created:	29/10/96
	Returns:	TRUE if successfully declared preferences
				FALSE otherwise
	Purpose:	To declare preferences associated with these export options

********************************************************************************************/
BOOL PNGExportOptions::Declare()
{
	if (Camelot.DeclareSection("Filters", 10))
		Camelot.DeclarePref( NULL, "ExportPNGtype", &g_CompactedFlagsForDefaults, 0, 3 );

	// All ok
	return TRUE;
}

/********************************************************************************************

>	PNGExportOptions::PNGExportOptions(const FILTER_ID FilterID, const StringBase* pFilterName)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Default constructor for a PNGExportOptions object to provide PNG export
				options

********************************************************************************************/
PNGExportOptions::PNGExportOptions(const FILTER_ID FilterID, const StringBase* pFilterName) : 
						MaskedFilterExportOptions(_R(IDD_EXPORTBMPOPTS), FilterID, pFilterName)
{
	// just us rats in here
}

/********************************************************************************************

>	virtual BOOL PNGExportOptions::RetrieveDefaults()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	See BitmapExportOptions for interface details
	Notes:		Gets GIF specific preferences

********************************************************************************************/
BOOL PNGExportOptions::RetrieveDefaults()
{
	if (!MaskedFilterExportOptions::RetrieveDefaults())
		return FALSE;

	SetMakeInterlaced(g_CompactedFlagsForDefaults & 1);
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL PNGExportOptions::SetAsDefaults() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Provides additional implementation to set PNG specific options as defaults
	See Also:	BitmapExportOptions::SetAsDefaults()

********************************************************************************************/
BOOL PNGExportOptions::SetAsDefaults() const
{
	if (!MaskedFilterExportOptions::SetAsDefaults())
		return FALSE;

	g_CompactedFlagsForDefaults = WantTransparent() ? 2 : 0;
	g_CompactedFlagsForDefaults |= WantInterlaced() ? 1 : 0;

	return TRUE;
}

#endif

/********************************************************************************************

>	PNGFilter::PNGFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Purpose:	Constructor for an PNGFilter object.  The object should be initialised
				before use.
	SeeAlso:	PNGFilter::Init

********************************************************************************************/
PNGFilter::PNGFilter() : MaskedFilter()
{
	ImportMsgID = _R(IDS_IMPORTMSG_PNG);
	Flags.CanImport = TRUE;
	Flags.CanExport = TRUE;

	FilterID = FILTERID_PNG;
	ExportMsgID = _R(IDS_EXPORTMSG_PNG);			// "Preparing PNG file..."
	ExportingMsgID = _R(IDS_EXPORTINGMSG_PNG);		// "Exporting PNG file..."

	// Special Mask prepartion stage ID
	Export2ndStageMsgID = _R(IDN_MASKINGMSG_PNG);	// "Preparing mask for PNG file..."
	ExportRegion = NULL;
}

/********************************************************************************************

>	BOOL PNGFilter::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an PNGFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	EPSStack

********************************************************************************************/
BOOL PNGFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new PNGOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDS_PNG_FILTERNAME));
	FilterInfo.Load(_R(IDS_PNG_FILTERINFO));

	PORTNOTETRACE("filters","PNGFilter::Init - removed PNGExportOptions::Declare");
#ifndef EXCLUDE_FROM_XARALX
	if (!PNGExportOptions::Declare())
		return FALSE;
#endif

	// All ok
	return TRUE;
}

/********************************************************************************************

>	INT32 PNGFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, 
								 UINT32 FileSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Inputs:		Filename - name of the file.
				HeaderStart - Address of the first few bytes of the file.
				HeaderSize - the number of bytes in the header pointed to by FileStart.
				FileSize - the size of the whole file, in bytes.
	Returns:	0 => Not a PNG file.
				10 => It is a PNG file.
	Purpose:	Determine if this filter can load the specified file.

********************************************************************************************/
INT32 PNGFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, 
							 UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	// We need to remember what we thought of this file in our class variable.
	// So, set it to a nice default value at the start.
	PNGHowCompatible = 0;

	// Check that we've got enough data to do our check
	if (HeaderSize < 8)
		return 0; // Not enough data - ignore this file.

	// Check the header for the "PNG" signature.
	// The first eight bytes of a PNG file always contain the following (decimal) values: 
	// 137 80 78 71 13 10 26 10 
	if (
		(HeaderStart[0] == 137) && // 0x89
		(HeaderStart[1] == 80) && //  0x50
		(HeaderStart[2] == 78) && //  0x4E
		(HeaderStart[3] == 71) && //  0x47
		(HeaderStart[4] == 13) && // Carriage return
		(HeaderStart[5] == 10) && // Line feed
		(HeaderStart[6] == 26) && // end of file (0x1a)
		(HeaderStart[7] == 10)    // Line feed
	   )
	{
		// This should be a good enough check that this is a PNG file
		// Hence, we like this file
		PNGHowCompatible = 10;
	}
				
	// Return the found value to the caller.
	return PNGHowCompatible;
}

/********************************************************************************************

>	virtual BOOL PNGFilter::IsThisBppOk(UINT32 Bpp)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Inputs:		Bpp or Colour depth.
	Returns:	TRUE if this filter can cope with this colour depth, FALSE otherwise.
	Purpose:	Check if this Bitmap filter can cope with saving at this Bpp/Colour depth.
	SeeAlso:	OpMenuExport::DoWithParam;

********************************************************************************************/
BOOL PNGFilter::IsThisBppOk(UINT32 Bpp)
{
	return (Bpp == 1 || Bpp == 4 || Bpp == 8 || Bpp == 24 || Bpp == 32);
}

/********************************************************************************************

>	BOOL PNGFilter::ReadFromFile( OILBitmap* pOilBitmap, BaseCamelotFilter* pFilter,
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
BOOL PNGFilter::ReadFromFile( OILBitmap* pOilBitmap, BaseCamelotFilter* pFilter,
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

>	BOOL PNGFilter::ReadFromFile(OILBitmap* pOilBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Inputs:		pOilBitmap	pointer to the oil bitmap data to be filled in
	Outputs:	Will have filled in BMInfo	pointer to the bitmap header to fill in
									BMBytes	pointer to the bitmap data to fill in
	Purpose:	Actually does the process of reading a bitmap from a file.
				Inherited classes override this to read in different file formats.
				
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/
BOOL PNGFilter::ReadFromFile(OILBitmap* pOilBitmap)
{
	ERROR2IF(pOilBitmap == NULL,FALSE,"PNGFilter::ReadFromFile null OilBitmap pointer");
	
	// Try to import the bitmap as a PNG file.		
	CCLexFile *pImportFile = GetImportFile();
	ERROR2IF(pImportFile==NULL,FALSE,"PNGFilter::ReadFromFile - No import file");

	UINT32 ImportMsgId = GetImportMsgID();		
	String_64 ProgressString(ImportMsgId);
	ProgressString = GetImportProgressString(pImportFile, ImportMsgId);

	CWxBitmap* pWBitmap = (CWxBitmap*)pOilBitmap;
	
	LPBITMAPINFO *pInfo = &(pWBitmap->BMInfo);
	LPBYTE *pBytes = &(pWBitmap->BMBytes);
	
	INT32 TransColour = -1;

	// The PNG filter liked it very much and so use it, showing progress bar
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

>	virtual BOOL PNGFilter::GetExportOptions(BitmapExportOptions* pOptions)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/96
	Purpose:	See BaseBitmapFilter for interface details

********************************************************************************************/
BOOL PNGFilter::GetExportOptions(BitmapExportOptions* pOptions)
{
	ERROR2IF(pOptions == NULL, FALSE, "NULL Args");
	PORTNOTETRACE("filters","PNGFilter::GetExportOptions - do nothing");
#ifndef EXCLUDE_FROM_XARALX

	PNGExportOptions* pPNGOptions = (PNGExportOptions*)pOptions;
	ERROR3IF(!pPNGOptions->IS_KIND_OF(PNGExportOptions), "pPNGOptions isn't");

	// the depth we ask GDraw to render is always 32-bit, so we can get transparency
	// we have to convert for other formats	when writing the actual bytes to the file
	SetDepthToRender(32);

	// We haven't written the header yet
	WrittenHeader = FALSE;

	// We are a first pass render and not doing the mask, by default
	SecondPass = FALSE;
	DoingMask = FALSE;

	// Determine the filter type currently in use in Accusoft format
	FilterType = PNG;
	pPNGOptions->SetFilterType(FilterType);

	BOOL Ok = FALSE;

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
		//  This may get messed up, so have to use the second line below.
		Ok = BmapPrevDlg::m_bClickedOnExport;
	}
	else
	{	
		ERROR3("Unable to find OPTOKEN_BMAPPREVDLG");
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
void PNGFilter::PostGetExportOptions(BitmapExportOptions* pOptions)
{
	PORTNOTETRACE("filters","PNGFilter::PostGetExportOptions - do nothing");
	// should be of this type
#ifndef EXCLUDE_FROM_XARALX
	PNGExportOptions* pPNGOptions = (PNGExportOptions*)pOptions;
	ERROR3IF(!pPNGOptions->IS_KIND_OF(PNGExportOptions), "pPNGOptions isn't");

	// do the baseclass options
	MaskedFilter::PostGetExportOptions(pOptions);

	// do the specific to this class options
	// Filter type can be changed by the export options dialog box from say 
	// PNG to PNG_INTERLACED
	UINT32 Silliness = pPNGOptions->WantTransparent() ? 2 : 0;
	Silliness |= pPNGOptions->WantInterlaced() ? 1 : 0;
	if (Silliness >= 0 && Silliness <= 4)
	{
		Compression = Silliness;
		// Compression ranges from 0 .. 4 so map this onto our filter types
		FilterType = Silliness + PNG;

		if (pPNGOptions->WantTransparent() && pPNGOptions->GetSelectionType() == SELECTION)
			DoingMask = TRUE;
	}
#endif
}

/********************************************************************************************

>	virtual void PNGFilter::CleanUpAfterExport()

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
void PNGFilter::CleanUpAfterExport()
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

>	virtual UINT32 PNGFilter::GetExportMsgID()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Returns:	The id of the message to put on the progress display whilst exporting.
	Purpose:	Used to get the message id to be used during export.
				Overides the baseclass form of the function so that during the two stage
				export process it can change the message.
	SeeAlso:	DoExport; TI_GIFFilter::GetExportMsgID;

********************************************************************************************/
UINT32 PNGFilter::GetExportMsgID()
{
	if (GeneratingOptimisedPalette())
		return _R(IDS_GENOPTPALMSGID);				// "Generating optimised palette..."

	PORTNOTETRACE("filters","PNGFilter::GetExportMsgID - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	PNGExportOptions* pPNGOptions = (PNGExportOptions*)GetBitmapExportOptions();
	ERROR2IF(pPNGOptions == NULL, FALSE, "NULL Args");
	ERROR3IF(!pPNGOptions->IS_KIND_OF(PNGExportOptions), "pPNGOptions isn't");

	// If we are exporting with transparency on and on first pass use the masking message
	// otherwise use the exporting message.
	if (pPNGOptions->GetSelectionType() == SELECTION && pPNGOptions->WantTransparent())
	{
		// Special 4 stage rendering operation
		// - Render selected objects to white background
		// - Render mask 1bpp
		// - Render all objects
		// - Save data out to disk
		if (!SecondPass)
			return Export2ndStageMsgID;			// "Preparing mask for PNG file..."
		else
			return Filter::GetExportMsgID();	// "Preparing PNG file..."
	}
	else
	{
		// Special 3 stage rendering operation
		// - Render objects to white background
		// - Render mask 1bpp
		// - Save data out to disk
		if (DoingMask)
			return Export2ndStageMsgID;			// "Preparing mask for PNG file..."
		else
			return Filter::GetExportMsgID();	// "Preparing PNG file..."
	}

	return ExportingMsgID;
#else
	return 0;
#endif
}

/********************************************************************************************

>	BOOL PNGFilter::EndWriteToFile( )

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
BOOL PNGFilter::EndWriteToFile()
{
	if (GeneratingOptimisedPalette())
		return TRUE;		// No need to output anything

	PORTNOTETRACE("filters","PNGFilter::EndWriteToFile - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	//  Can reset the band number now.
	m_BandNumber = 0;

	PNGExportOptions* pPNGOptions = (PNGExportOptions*)GetBitmapExportOptions();
	ERROR2IF(pPNGOptions == NULL, FALSE, "NULL Args");
	ERROR3IF(!pPNGOptions->IS_KIND_OF(PNGExportOptions), "pPNGOptions isn't");

	// Do the transparency translation just before we write out the data as a PNG.
	// This involves doing a 1 bpp export of the same area and using this to work
	// out which areas are transparent or not.
	// Only do this if the user has requested transparency and we outputting at 8bpp
	BOOL ok = TRUE;
	BOOL SaveDataOut = TRUE;

	if (BadExportRender)
	{
		// Delete our whitearea bitmap
		if (pTempBitmapMask != NULL)
			CCFree(pTempBitmapMask);

		pTempBitmapMask = NULL;
	}

	// Save the data out if required. Only if we exported ok.
	if (SaveDataOut && !BadExportRender)
	{
		if (ok)
		{
			// Now that we know the transparent index we can output the PNG header
			ok = DestPNG.OutputPNGHeader(OutputFile, NULL, pPNGOptions->WantInterlaced(),
										pPNGOptions->GetTransparencyIndex(),
										pPNGOptions->GetDepth() <= 8 ? pPNGOptions->GetLogicalPalette() : NULL);
		}

		// Actually write the destination bitmap out to the file showing an hourglass
		// and/or progress bar as we go. Always show the Exporting message.
		// Need to do in one go due to interlacing
		if (ok)
		{
			String_64 ProgressString(ExportingMsgID);
			ProgressString = GetExportProgressString(OutputFile, ExportingMsgID);
			BeginSlowJob(100, FALSE, &ProgressString);
			
			DestPNG.OutputPNGBits(OutputFile, DestPNG.GetDestBitmapBits());
			
			EndSlowJob();
		}
	}

	ASSERT(ok);
	
	return DestPNG.TidyUp();
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	static BOOL PNGFilter::WriteDataToFile( BOOL End, UINT32 Bpp, UINT32 Compression)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/96
	Inputs:		End - TRUE if this is the last block of the file.
				Bpp - output depth in terms of bits per pixel
				Compression - usually True if compression required, False otherwise
							  In the PNG case this is used to pass in the transparency and
							  interlace state.
	Returns:	TRUE if worked, FALSE if errored.
	Purpose:	Physically put the bitmap into the disk.
				NOTE - ONLY COPES WITH End=TRUE currently
				AtEnd is ignored now and should always be set to TRUE.
				Unused at present due to static problems when cretaing the 1bpp bitmap. 
	SeeAlso:	WriteToFile(); AccusoftFilters::WriteToFile; AccusoftFilters::WriteDataToFile;

********************************************************************************************/
BOOL PNGFilter::WriteDataToFile( BOOL End, UINT32 Bpp, UINT32 Compression)
{
	ERROR2(FALSE,"PNGFilter::WriteDataToFile called when not implemented");
}

/********************************************************************************************

>	virtual BOOL PNGFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/96
	Inputs:		Pointer to the bitmap to save.
				Dpi of the bitmap to be saved
	Returns:	TRUE if worked, FALSE if errored.
	Purpose:	Physically put the bitmap into the disk.  Inherited classes override this to write
				in different file formats.
	SeeAlso:	WriteDataToFile(); AccusoftFilters::WriteToFile; AccusoftFilters::WriteDataToFile;

********************************************************************************************/
BOOL PNGFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi)
{
	ERROR2IF(pKernelBitmap == NULL,FALSE,"PNGFilter::WriteBitmapToFile null bitmap pointer specified");

	// Get a pointer to the actual bitmap so that we can get some details from it.
	OILBitmap *pOilBitmap = pKernelBitmap->ActualBitmap;
	ERROR2IF(pOilBitmap == NULL,FALSE,"PNGFilter::WriteBitmapToFile null oil bitmap pointer");

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
	//  a PNG, then we have to undo the alpha channel reversing that we did above.
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

>	virtual BOOL PNGFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap,
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
BOOL PNGFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, BaseCamelotFilter* pFilter,
										 CCLexFile* pFile, INT32 Compression)
{
	ERROR2IF(pKernelBitmap == NULL,FALSE, "PNGFilter::WriteBitmapToFile null pKernelBitmap");
	ERROR2IF(pFilter == NULL,FALSE, "PNGFilter::WriteBitmapToFile null pFilter");
	ERROR2IF(pFile == NULL,FALSE, "PNGFilter::WriteBitmapToFile null pFile");

	// Get a pointer to the actual bitmap so that we can get some details from it.
	OILBitmap *pOilBitmap = pKernelBitmap->ActualBitmap;
	ERROR2IF(pOilBitmap == NULL,FALSE,"PNGFilter::WriteBitmapToFile null oil bitmap pointer");

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

>	static BOOL PNGFilter::WriteToFile ( CCLexFile* File, LPBITMAPINFO Info, LPBYTE Bits,
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
	Purpose:	Write a bitmap in memory straight out as a PNG to file with no rendering or
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
BOOL PNGFilter::WriteToFile( CCLexFile* File, LPBITMAPINFO Info, LPBYTE Bits,
							 BOOL Interlace, INT32 TransparentColour,
							 BaseCamelotFilter* pFilter )
{
#ifdef DO_EXPORT
#ifndef WIN32
	Error::SetError( _R(IDE_BADFORMAT) );
	return FALSE;
#else
	ERROR2IF(File==NULL,FALSE,"PNGFilter::WriteToFile File pointer is null");
	ERROR2IF(Info==NULL,FALSE,"PNGFilter::WriteToFile BitmapInfo pointer is null");
	ERROR2IF(Bits==NULL,FALSE,"PNGFilter::WriteToFile Bits pointer is null");

	// BITMAPINFO  consists of:-
	//    	BITMAPINFOHEADER    bmiHeader;
	//		RGBQUAD             bmiColors[1];
	LPBITMAPINFOHEADER pInfoHeader = &Info->bmiHeader;
	ERROR2IF(pInfoHeader==NULL,FALSE,"PNGFilter::WriteToFile BitmapInfoHeader pointer is null");
		
	LPRGBQUAD pPalette = &(Info->bmiColors[0]);
	ERROR2IF(pPalette==NULL,FALSE,"PNGFilter::WriteToFile palette pointer is null");

	// Set up our format type flags.
	if(Info->bmiHeader.biBitCount == 32)
	{
		// If we`re exporting a 32Bit BMP then we need to make sure that we convert the
		// Alpha channel to Transparency! i.e. invert it!
		UINT32 BmpSize = Info->bmiHeader.biSizeImage;

		for(UINT32 i = 0; i < BmpSize; i+=4)
			Bits[i+3] = ~Bits[i+3];
	}

	// Output a PNG header for this file, using the RGBQUAD palette rather than a LOGPALETTE
	DestPNG.OutputPNGHeader(File, pInfoHeader, Interlace, TransparentColour, NULL, pPalette);

	// Now write out the bitmap data itself.
	DestPNG.OutputPNGBits(File, Bits, TRUE, pFilter);

	// The above has set the OutputFile member variable of DestPNG. We desperately need to
	// reset this as otherwise the next bitmap export may go wrong as it calls the tidy up
	// and so will refer to the deleted CCFile. Oh Er!
	DestPNG.TidyUp();

	// er, we seem to have finished OK so say so
	return TRUE;
#endif
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	static BOOL PNGFilter::WriteToFile ( CCLexFile* File, LPBITMAPINFO Info, LPBYTE Bits,
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
	Purpose:	Write a bitmap in memory straight out as a PNG to file with no rendering or
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
BOOL PNGFilter::WriteToFile( CCLexFile* File, LPBITMAPINFO Info, LPBYTE Bits,
							 String_64* ProgressString )
{
#ifdef DO_EXPORT
#ifndef WIN32
	Error::SetError( _R(IDE_BADFORMAT) );
	return FALSE;
#else
	ERROR2IF(File==NULL,FALSE,"PNGFilter::WriteToFile File pointer is null");
	ERROR2IF(Info==NULL,FALSE,"PNGFilter::WriteToFile BitmapInfo pointer is null");
	ERROR2IF(Bits==NULL,FALSE,"PNGFilter::WriteToFile Bits pointer is null");

	// If the caller has specified a string then assume they require a progress bar
	// Start it up.
	if (ProgressString != NULL)
		BeginSlowJob(100, FALSE, ProgressString);

	// BITMAPINFO  consists of:-
	//    	BITMAPINFOHEADER    bmiHeader;
	//		RGBQUAD             bmiColors[1];
	LPBITMAPINFOHEADER pInfoHeader = &Info->bmiHeader;
	ERROR2IF(pInfoHeader==NULL,FALSE,"PNGFilter::WriteToFile BitmapInfoHeader pointer is null");
		
	LPRGBQUAD pPalette = &(Info->bmiColors[0]);
	ERROR2IF(pPalette==NULL,FALSE,"PNGFilter::WriteToFile palette pointer is null");

	// Set up our format type flags.
	BOOL Interlace = TRUE;	// Use interlace or not
	INT32 Transparent = -1;	// colour or -1 = no transparency
	BOOL WantTransparent = FALSE;

	switch (FilterType)
	{
		default:
		case PNG:
			Interlace 		= FALSE;
			WantTransparent = FALSE;
			break;
		case PNG_INTERLACED:
			Interlace 		= TRUE;
			WantTransparent = FALSE;
			break;
		case PNG_TRANSPARENT:
			Interlace 		= FALSE;
			WantTransparent = TRUE;
			break;
		case PNG_TRANSINTER:
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
				TRACEUSER( "Neville", _T("PNG output with transp index of %d\n"),Transparent);
				break;
			}		
		}	
	}

	// Output the PNG data
	BOOL ok = TRUE;
	// Output a PNG header for this file, using the RGBQUAD palette rather than a LOGPALETTE
	if (Transparent == -1)
		ok = DestPNG.OutputPNGHeader(File, pInfoHeader, Interlace, -1, NULL, pPalette);
	else
		ok = DestPNG.OutputPNGHeader(File, pInfoHeader, Interlace, Transparent, NULL, pPalette);

	// Now write out the bitmap data itself.
	if (ok)
		ok = DestPNG.OutputPNGBits(File, Bits, TRUE);
	
	// If started, then stop then progress bar
	if (ProgressString != NULL)
		EndSlowJob();

	// er, we seem to have finished OK so say so
	return TRUE;
#endif
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL PNGFilter::WritePreFrame(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out any frame specific info before the image
	SeeAlso:	GIFFilter::WritePreFrame(void)

********************************************************************************************/
BOOL PNGFilter::WritePreFrame(void)
{
	PORTNOTETRACE("filters","PNGFilter::WritePreFrame - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	return DestPNG.ReStartFile(NULL);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL PNGFilter::WriteFrame(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out the image itself
				This base class version actually calls the WriteToFile() function so that
				derived classes do not have to implement any of the multi-image stuff
	SeeAlso:	GIFFilter::WriteFrame(void)

********************************************************************************************/
BOOL PNGFilter::WriteFrame(void)
{
	return MaskedFilter::WriteToFile(TRUE);
}


/********************************************************************************************

>	virtual BOOL PNGFilter::WritePostFrame(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out any frame specific info after the image
	SeeAlso:	GIFFilter::WritePostFrame(void)

********************************************************************************************/
BOOL PNGFilter::WritePostFrame(void)
{
	return EndWriteToFile();
}

/********************************************************************************************

>	virtual BOOL PNGFilter::WriteFileEnd(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out the file specific data at the end of the file
				This base class version calls EndWriteToFile() so that derived classes
				do not have to implement the multi-image stuff
	SeeAlso:	GIFFilter::WriteFileEnd(void)

********************************************************************************************/
BOOL PNGFilter::WriteFileEnd(void)
{
	PORTNOTETRACE("filters","PNGFilter::WriteFileEnd - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	return DestPNG.TidyUp();
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL PNGFilter::WritePreSecondPass(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	Called to do any processing required after the first and before the second
				pass of a two pass export
	SeeAlso:	GIFFilter::WritePreSecondPass(void)

********************************************************************************************/
BOOL PNGFilter::WritePreSecondPass(void)
{
	return EndWriteToFile();
}

/********************************************************************************************

>	virtual void PNGFilter::InvertAlpha ( LPBITMAPINFO	lpBitmapInfo,
										  LPBYTE		lpBits )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/00
	Purpose:	Camelot uses a different transparency scheme to the rest of the world, in
				that 255 is clear, and 0 is opaque. Until the rest of the world catches up,
				it's necessary to invert the alpha channel to make exported files compatible
				with other programs.

********************************************************************************************/
void PNGFilter::InvertAlpha ( LPBITMAPINFO	lpBitmapInfo,
							  LPBYTE		lpBits )
{
	DIBUtil::InvertAlpha(lpBitmapInfo, lpBits);
}

/********************************************************************************************

>	virtual OutputDIB* PNGFilter::GetOutputDIB ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/00
	Returns		OutputDIB* - A pointer to DestPNG.
	Purpose:	Casts the current output DIB to be a generic OutputDIB class. This allows the
				same code to be re-used in the base class.

********************************************************************************************/
OutputDIB* PNGFilter::GetOutputDIB ( void )
{
	// Perform an upcast to allow the pointer to be used in a generic manner.
	PORTNOTETRACE("filters","PNGFilter::GetOutputDIB - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	return static_cast<OutputDIB*> ( &DestPNG );
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BitmapExportOptions* PNGFilter::CreateExportOptions() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	See BaseBitmapFilter for interface details
	Notes:		Provides a new PNGExportOptions instance

********************************************************************************************/
BitmapExportOptions* PNGFilter::CreateExportOptions() const
{
	PORTNOTETRACE("filters","PNGFilter::CreateExportOptions - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	PNGExportOptions* pPNGOptions = new PNGExportOptions(PNG, &FilterName);
	return (BitmapExportOptions*)pPNGOptions;
#else
	return NULL;
#endif
}

void PNGFilter::AlterPaletteContents( LPLOGPALETTE pPalette )
{
	PORTNOTETRACE("filters","PNGFilter::AlterPaletteContents - do nothing");
//	DestPNG.AlterExportPalette( pPalette );
}
