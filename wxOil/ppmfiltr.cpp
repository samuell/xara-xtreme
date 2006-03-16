// $Id: ppmfiltr.cpp 662 2006-03-14 21:31:49Z alex $
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

// A PPM import/export filter 

#include "camtypes.h"
#include "ppmfiltr.h"
//#include "andy.h"
#include "oilfltrs.h"
#include "app.h"
#include "dibutil.h"
#include "grndbmp.h"
#include "nodebmp.h"
//#include "bmpfiltr.h"
#include "progress.h"
#include "wbitmap.h"	// Windows specific bitmap information	 
#include "docview.h"	// DocView
//#include "bitmap.h"		// kernel bitmap
//#include "bmpres.h"		// general bitmap filter based resources
//#include "resource.h"		// _R(IDS_OUTOFMEMORY)
//#include "resource.h"	// Inform Warning _R(IDS_OK)

CC_IMPLEMENT_DYNAMIC(BasePMFilter, BaseBitmapFilter)
CC_IMPLEMENT_DYNAMIC(PPMFilter, BasePMFilter)
CC_IMPLEMENT_DYNAMIC(PGMFilter, BasePMFilter)
CC_IMPLEMENT_DYNAMIC(PBMFilter, BasePMFilter)

#define	new	CAM_DEBUG_NEW

// General statics which we use
//OutputPPM 	PPMFilter::DestPPM;

enum PpmTokenIndex
{
	PpmTOKEN_NONE = -1,
	TOKEN_PPM_ASCII,
	TOKEN_PPM_BINARY,
	TOKEN_PGM_ASCII,
	TOKEN_PGM_BINARY,
	TOKEN_PBM_ASCII,
	TOKEN_PBM_BINARY,
	// Add new token indexs BEFORE NUM_TOKENS
	PpmNUM_TOKENS
};

static struct
{
	TCHAR*  Token;
} TokenTable[] = 
{
	"P3",
	"P6",
	"P2",
	"P5",
	"P1",
	"P4",
};

static PpmTokenIndex FindToken(const TCHAR* Token)
{
	for (INT32 i = 0; i < PpmNUM_TOKENS; i++)
		if (lstrcmp(TokenTable[i].Token,Token) == 0) return (PpmTokenIndex) i;

	return PpmTOKEN_NONE;
}

/********************************************************************************************
***** 			PPMFilter																*****
********************************************************************************************/


/********************************************************************************************

>	PPMOILFilter::PPMOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/95
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

PPMOILFilter::PPMOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_PPM));
	FilterExt.Load(_R(IDN_FILTEREXT_PPM));
}

/********************************************************************************************

>	PPMFilter::PPMFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Purpose:	Constructor for an PPMFilter object.  The object should be initialised
				before use.
	SeeAlso:	PPMFilter::Init

********************************************************************************************/

PPMFilter::PPMFilter() : BasePMFilter()
{
	ImportMsgID = _R(IDN_IMPORTMSG_PPM);
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
	FilterID = FILTERID_PPM;

	ExportRegion = NULL;
	ExportMsgID = _R(IDN_EXPORTMSG_PPM);			// "Preparing PPM file..."

	ExportingMsgID = _R(IDN_EXPORTINGMSG_PPM);		// "Exporting PPM file..."

//	CurrentSelection = DRAWING;
}

/********************************************************************************************

>	BOOL PPMFilter::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an PPMFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL PPMFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new PPMOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDN_PPM_FILTERNAME));
	FilterInfo.Load(_R(IDN_PPM_FILTERINFO));

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL PPMFilter::CheckString(TCHAR * pHeader)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Returns:	TRUE if the filter reckonises the string, FALSE otherwise.
	Purpose:	To see if the header of a PPM file is correct or not.
	SeeAlso:	BasePMFilter::HowCompatible;
	
********************************************************************************************/

BOOL PPMFilter::CheckString(TCHAR * pHeader)
{
	if (
		(_tcsncmp(pHeader, "P3", 2) == 0) ||		// ASCII PPM
		(_tcsncmp(pHeader, "P6", 2) == 0) 		// BINARY PPM
	   )
	{
		// finding PBM should be good enough to determine that there is
		// a high chance that this is the right file.

		return TRUE;
	}

	return FALSE;
}


/********************************************************************************************
***** 			PGMFilter																*****
********************************************************************************************/



/********************************************************************************************

>	PGMOILFilter::PGMOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/95
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

PGMOILFilter::PGMOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_PGM));
	FilterExt.Load(_R(IDN_FILTEREXT_PGM));
}

/********************************************************************************************

>	PGMFilter::PGMFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Purpose:	Constructor for an PGMFilter object.  The object should be initialised
				before use.
	SeeAlso:	PGMFilter::Init

********************************************************************************************/

PGMFilter::PGMFilter() : BasePMFilter()
{
	ImportMsgID = _R(IDN_IMPORTMSG_PGM);
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
	FilterID = FILTERID_PGM;

	ExportRegion = NULL;
	ExportMsgID = _R(IDN_EXPORTMSG_PGM);			// "Preparing PGM file..."

	ExportingMsgID = _R(IDN_EXPORTINGMSG_PGM);		// "Exporting PGM file..."

//	CurrentSelection = DRAWING;
}

/********************************************************************************************

>	BOOL PGMFilter::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an PPMFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL PGMFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new PGMOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDN_PGM_FILTERNAME));
	FilterInfo.Load(_R(IDN_PGM_FILTERINFO));

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL PGMFilter::CheckString(TCHAR * pHeader)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Returns:	TRUE if the filter reckonises the string, FALSE otherwise.
	Purpose:	To see if the header of a PBM file is correct or not.
	SeeAlso:	BasePMFilter::HowCompatible;
	
********************************************************************************************/

BOOL PGMFilter::CheckString(TCHAR * pHeader)
{
	if (
		(_tcsncmp(pHeader, "P2", 2) == 0) ||		// ASCII PGM
		(_tcsncmp(pHeader, "P5", 2) == 0) 		// BINARY PGM
	   )
	{
		// finding PBM should be good enough to determine that there is
		// a high chance that this is the right file.

		return TRUE;
	}

	return FALSE;
}



/********************************************************************************************
***** 			PBMFilter																*****
********************************************************************************************/



/********************************************************************************************

>	PBMOILFilter::PBMOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/95
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

PBMOILFilter::PBMOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_PBM));
	FilterExt.Load(_R(IDN_FILTEREXT_PBM));
}

/********************************************************************************************

>	PBMFilter::PBMFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Purpose:	Constructor for an PBMFilter object.  The object should be initialised
				before use.
	SeeAlso:	PBMFilter::Init

********************************************************************************************/

PBMFilter::PBMFilter() : BasePMFilter()
{
	ImportMsgID = _R(IDN_IMPORTMSG_PBM);
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
	FilterID = FILTERID_PBM;

	ExportRegion = NULL;
	ExportMsgID = _R(IDN_EXPORTMSG_PBM);			// "Preparing PBM file..."

	ExportingMsgID = _R(IDN_EXPORTINGMSG_PBM);		// "Exporting PBM file..."

//	CurrentSelection = DRAWING;
}

/********************************************************************************************

>	BOOL PBMFilter::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an PBMFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL PBMFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new PBMOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDN_PBM_FILTERNAME));
	FilterInfo.Load(_R(IDN_PBM_FILTERINFO));

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL PBMFilter::CheckString(TCHAR * pHeader)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Returns:	TRUE if the filter reckonises the string, FALSE otherwise.
	Purpose:	To see if the header of a PBM file is correct or not.
	SeeAlso:	BasePMFilter::HowCompatible;
	
********************************************************************************************/

BOOL PBMFilter::CheckString(TCHAR * pHeader)
{
	if (
		(_tcsncmp(pHeader, "P1", 2) == 0) ||		// ASCII PBM
		(_tcsncmp(pHeader, "P4", 2) == 0)		// BINARY PBM
	   )
	{
		// finding PBM should be good enough to determine that there is
		// a high chance that this is the right file.

		return TRUE;
	}

	return FALSE;
}


/********************************************************************************************
***** 			BasePMFilter															*****
********************************************************************************************/

/********************************************************************************************

>	BasePMFilter::BasePMFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Purpose:	Constructor for an BasePMFilter object.  The object should be initialised
				before use.
	SeeAlso:	BasePMFilter::Init

********************************************************************************************/

BasePMFilter::BasePMFilter() : BaseBitmapFilter()
{
	// Make ourselves look like a PPM filter, even though we shouldn't be seen
	ImportMsgID = _R(IDN_IMPORTMSG_PPM);
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
	FilterID = FILTERID_PPM;

	ExportRegion = NULL;
	ExportMsgID = _R(IDN_EXPORTMSG_PPM);			// "Preparing PPM file..."

	ExportingMsgID = _R(IDN_EXPORTINGMSG_PPM);		// "Exporting PPM file..."

//	CurrentSelection = DRAWING;
}

/********************************************************************************************

>	BOOL BasePMFilter::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an BasePMFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL BasePMFilter::Init()
{
	// Make ourselves look like a PPM filter, even though we shouldn't be seen

	// Get the OILFilter object
	pOILFilter = new PPMOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDN_PPM_FILTERNAME));
	FilterInfo.Load(_R(IDN_PPM_FILTERINFO));

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL BasePMFilter::CheckString(TCHAR * pHeader)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Returns:	TRUE if the filter reckonises the string, FALSE otherwise.
	Purpose:	To see if the header of a PBM file is correct or not.
				Baseclass version so should not be used.
	SeeAlso:	BasePMFilter::HowCompatible;
	
********************************************************************************************/

BOOL BasePMFilter::CheckString(TCHAR * pHeader)
{
	ERROR2(FALSE,"BasePMFilter::ReadDataIntoBitmap calling baseclass version!")	
	return FALSE;
}

/********************************************************************************************

>	INT32 BasePMFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, 
									UINT32 FileSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		Filename - name of the file.
				HeaderStart - Address of the first few bytes of the file.
				HeaderSize - the number of bytes in the header pointed to by FileStart.
				FileSize - the size of the whole file, in bytes.
	Returns:	0 => Not a PPM file.
				10 => It is a PPM file.
	Purpose:	Determine if this filter can load the specified file.

********************************************************************************************/



INT32 BasePMFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, 
							 UINT32 FileSize)
{
TRACEUSER( "Neville", _T("BasePMFilter::HowCompatible"));	
	// We need to remember what we thought of this file in our class variable.
	// So, set it to a nice default value at the start.
	PPMHowCompatible = 0;

	// Check that we've got enough data to do our check
	if (HeaderSize < 4)
	{
		// Not enough data - ignore this file.
		return 0;
	}

	// Check the header for the "PPM" signature.
	TCHAR * pHeader = (TCHAR *) HeaderStart;

	if ( CheckString(pHeader) )
	{
		// finding PPM, PGM or PBM should be good enough to determine that there is
		// a high chance that this is the right file.

		// Remember what we thought in our class variable.
		PPMHowCompatible = 10;
	}
	else
	{
		// No PPM signature - we don't want this file.
		PPMHowCompatible = 0;
	}
				
TRACEUSER( "Neville", _T("BasePMFilter::HowCompatible returning = %d\n"),PPMHowCompatible);
	// Return the found value to the caller.
	return PPMHowCompatible;
}

/********************************************************************************************

>	INT32 BasePMFilter::GetPPMCompatibility()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		-
	Returns:	The value we found in the HowCompatible call.
				0 => Not a PPM file.
				10 => It is a PPM file.
	Purpose:	Determine if this filter can load the specified file.

********************************************************************************************/

//INT32 BasePMFilter::GetPPMCompatibility()
//{
//	return PPMHowCompatible;
//}

/********************************************************************************************

>	BOOL BasePMFilter::ReadFromFile(OILBitmap* pOilBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		pOilBitmap	pointer to the oil bitmap data to be filled in
	Outputs:	Will have filled in BMInfo	pointer to the bitmap header to fill in
									BMBytes	pointer to the bitmap data to fill in
	Purpose:	Actually does the process of reading a bitmap from a file.
				Inherited classes override this to read in different file formats.
				
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL BasePMFilter::ReadFromFile(OILBitmap* pOilBitmap)
{
	ERROR2IF(pOilBitmap == NULL,FALSE,"BasePMFilter::ReadFromFile null OilBitmap pointer");
	
	// Try to import the bitmap as a PPM file.
		
	CCLexFile *pImportFile = GetImportFile();
	ERROR2IF(pImportFile==NULL,FALSE,"BasePMFilter::ReadFromFile - No import file");

	UINT32 ImportMsgId = GetImportMsgID();		
	String_64 ProgressString(ImportMsgId);
	ProgressString = GetImportProgressString(pImportFile, ImportMsgId);

	WinBitmap* pWBitmap = (WinBitmap*)pOilBitmap;
	
	LPBITMAPINFO *pInfo = &(pWBitmap->BMInfo);
	LPBYTE *pBytes = &(pWBitmap->BMBytes);
	
	// The PPM filter liked it very much and so use it, showing progress bar
	BOOL ok = ReadFromFile(pImportFile, pInfo, pBytes, &ProgressString);

	SetLastBitmap();		// can only import one bitmap at the moment
	return ok;
}


/********************************************************************************************

>	BOOL PPMFilter::ReadFromFile( CCLexFile* pLexFile, LPBITMAPINFO* Info, LPBYTE* Bits,
								 String_64* ProgressString )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		A opened CCFile that can be read from. It should be positioned at the
				start. Caller is responsible for closing it. The file needs to be in
				Binary mode.
				ProgressString allows the user to specify whether they require a progress
				hourglass or not. If NULL then none is shown, otherwise an progress bar is shown
				using the text supplied. Defaults to NULL i.e. no progress bar.
	Outputs:	Info points to a new LPBITMAPINFO struct and Bits points to the bytes.
				These can be freed up with FreeDIB.
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Reads a ppm, pgm or pbm file into memory decompressing it as it goes.
				***Errors on 16-bit builds***
				A progress hourglass can be shown if required.
	Errors:		Calls SetError on FALSE returns.
	Scope:		Public
	SeeAlso:	BasePMFilter::ReadFromFile; DIBUtil::ReadFromFile; AccusoftFilters::ReadFromFile;

********************************************************************************************/

BOOL BasePMFilter::ReadFromFile( CCLexFile* pLexFile, LPBITMAPINFO* Info, LPBYTE* Bits,
								 String_64* ProgressString )
{
#ifndef WIN32
	Error::SetError( _R(IDE_BADFORMAT) );
	return FALSE;
#else
	ERROR2IF(pLexFile == NULL,FALSE,"BasePMFilter::ReadFromFile bad file pointer");
	ERROR2IF(Info == NULL,FALSE,"BasePMFilter::ReadFromFile bad bitmap info pointer");
	ERROR2IF(Bits == NULL,FALSE,"BasePMFilter::ReadFromFile bad bitmap data pointer");

	*Info = NULL;		// in case of early exit
	*Bits = NULL;

	// place to store the global palette, if present, for later use
	LPRGBQUAD lpGlobalPalette 		= NULL;	// pointer to temporary palette store
	INT32 GlobalPaletteSize 			= 0;	// size of the global palette found	 
/*
	// See if we are capable of parsing this, must be at least a LexFile.
	// Check for DiskFile as derived off LexFile and required for binary reads
	if (!File->IS_KIND_OF(CCDiskFile)) return FALSE;
	CCLexFile* pLexFile = (CCLexFile*) File;
*/
	// Must set the exception throwing flag to True and force reporting of errors to False.
	// This means that the caller must report an error if the function returns False.
	// Any calls to CCFile::GotError will now throw a file exception and should fall into
	// the catch handler at the end of the function.
	BOOL OldThrowingState = pLexFile->SetThrowExceptions( TRUE );
	BOOL OldReportingState = pLexFile->SetReportErrors( FALSE );

	// If the caller has specified a string then assume they require a progress bar
	// Start it up.
	if (ProgressString != NULL) BeginSlowJob(100, FALSE, ProgressString);

	TRY
	{
		// A ppm file consists of:-
		// - A "magic number" for identifying the file type. A ppm file's magic
		//   number is the two characters "P3".
		//
		// - Whitespace (blanks, TABs, CRs, LFs).
		//
		// - A width, formatted as ASCII characters in decimal.
		//
		// - Whitespace.
		//
		// - A height, again in ASCII decimal.
		//
		// - Whitespace.
		//
		// - The maximum color-component value, again in ASCII decimal.
		//
		// - Whitespace.
		//
		// - Width * height pixels, each three ASCII decimal values  between  0  and
		//   the  specified  maximum  value,  starting at the top-left corner of the
		//   pixmap, proceeding in normal English reading order.  The  three  values
		//   for each pixel represent red, green, and blue, respectively; a value of
		//   0 means that color is off, and the maximum value means  that  color  is
		//   maxxed out.
		//
		// - Characters from a "#" to the next end-of-line are ignored (comments).
		//
		// - No line should be longer than 70 characters.
		//
		// Here is an example of a small pixmap in this format:
		// P3
		// # feep.ppm
		// 4 4
		// 15
		//  0  0  0    0  0  0    0  0  0   15  0 15
		//  0  0  0    0 15  7    0  0  0    0  0  0
		//  0  0  0    0  0  0    0 15  7    0  0  0
		// 15  0 15    0  0  0    0  0  0    0  0  0

		// Alternatively, P6 means binary data follow the header
		// PGM and PBM are just variations on this theme with different unique numbers and
		// the numbers are intepreted differently.
		BOOL finished = FALSE;
		BOOL ok = TRUE;	

		// Set the type to an illegal state until we recognise the file
		TypeOfPPM = PPM_BAD;

		// Initialise lexing routines, and aspects of the lexer
		ok = pLexFile->InitLexer();
		pLexFile->SetDelimiters("\r\n");			// Set token delimiting characters
		pLexFile->SetCommentMarker('#');			// Set comment marker char
		pLexFile->SetWhitespace(" \t");			// Set whitespace chars
		pLexFile->SetStringDelimiters("\"\"");	// Set string delimiters

		// Token buffer remains constant until lexer deinitialisation
		const TCHAR* TokenBuf = pLexFile->GetTokenBuf();
		
		INT32	Width 		  	= 0;	// Width of bitmap	
		INT32 Height 		  	= 0;	// Height of bitmap	
		INT32 BitsPerPixel	= 0;	// ColorResolution;	// Colour depth required
		INT32 BitsPerRGB		= 0;	// Number of bits present per RGB component
		INT32 ColoursPerRGB 	= 0;	// For PGM number of colours present in the image per RGB component
									// For PGM will be for number of greyscales
									// For PBM is not present		
		INT32 NumberOfColours = 0;	// Number of colours present, 3 for PPM, 1 for PBM and PGM
		INT32 MaxCount 		= 3;	// Number of items in the header, usually three
	
		// default to 1 pixel per number read
		PixelCounter = 1;
		
		// Grab a token
		ok = pLexFile->GetSimpleToken();

		// Should be P3 or P6
 		PpmTokenIndex Token;

		if (ok)
		{
			// Find out the type of the token
			// Look the token up in our table
			Token = FindToken(TokenBuf);

			switch (Token)
			{
				case TOKEN_PPM_ASCII:
					// Correct file header found so note the type
					TypeOfPPM = PPM_ASCII;
					NumberOfColours = 3;	// RGB
					break;

				case TOKEN_PPM_BINARY:
					// Correct file header found so note the type
					TypeOfPPM = PPM_BINARY;
					NumberOfColours = 3;   // RGB
					break;

				case TOKEN_PGM_ASCII:
					// Correct file header found so note the type
					TypeOfPPM = PGM_ASCII;
					NumberOfColours = 1;   // greyscale
					break;

				case TOKEN_PGM_BINARY:
					// Correct file header found so note the type
					TypeOfPPM = PGM_BINARY;
					NumberOfColours = 1;   // greyscale
					break;

				case TOKEN_PBM_ASCII:
					// Correct file header found so note the type
					TypeOfPPM = PBM_ASCII;
					MaxCount = 2;
					ColoursPerRGB = 1;
					NumberOfColours = 1;   // B & W
					break;

				case TOKEN_PBM_BINARY:
					// Correct file header found so note the type
					TypeOfPPM = PBM_BINARY;
					MaxCount = 2;
					ColoursPerRGB = 1;
					NumberOfColours = 1;   // B & W
					// if in binary mode we get 8 pixels per number read
					PixelCounter = 8;
					break;

				default:
					TRACEUSER( "Neville", _T("BasePMFilter: Unexpected token - %s\n"),TokenBuf);
					ok = FALSE;
					break;
			}
		}
		else
			pLexFile->GotError( _R(IDE_BADFORMAT) );

		// read in the rest of the header data

		INT32 Count = 0;
		
		while (ok && !finished && Count < MaxCount)
		{
			// Grab a token
			ok = pLexFile->GetSimpleToken();

			//if (!ok)
			//	File->GotError( _R(IDE_BADFORMAT) );

			LexTokenType TokenType = pLexFile->GetTokenType();

			switch (TokenType)
			{
				case TOKEN_NORMAL:
					{
						Token = FindToken(TokenBuf);
						switch (Token)
						{
							case TOKEN_NONE:
								{
									// Must be one of the header numbers
									INT32 Number; 
									ok = (_stscanf(TokenBuf,"%d",&Number) == 1);

									// Now work out what to do with this
									switch (Count)
									{
										case 0:
											Width = Number;
											break;

										case 1:
											Height = Number;
											break;

										case 2:
											ColoursPerRGB = Number;
											break;

										default:
											// Flag a bad token so that we stop parsing this line
											ok = FALSE;
											TRACEUSER( "Neville", _T("BasePMFilter: Didn't expect to get this number ('%s')\n"), TokenBuf);
											break;
									}
									
									// Increment our count and see if we have enough info yet
									Count++;
									if (Count == MaxCount)
										finished = TRUE;
								}
								break;
																									
							default:
								// Flag a bad token so that we stop parsing this line
								ok = FALSE;
								TRACEUSER( "Neville", _T("BasePMFilter: Didn't expect to get this token ('%s')\n"),TokenBuf);
								break;
						}
					}
					break;

				default:
					// bad token so stop parsing this line
					TRACEUSER( "Neville", _T("BasePMFilter: Unexpected token - %s\n"),TokenBuf);
					ok = FALSE;
					break;
			}
		}
		
TRACEUSER( "Neville", _T("Width = %d Height = %d\n"), Width, Height);
TRACEUSER( "Neville", _T("ColoursPerRGB = %d\n"), ColoursPerRGB);

		if (Width > 0 && Height > 0 && ColoursPerRGB > 0)
		{
			// Allocate the space that we require for this bitmap
			// Sanity checks on the file that we have been asked to load.
			
			// Now work out what to do with this
			switch (NumberOfColours)
			{
				case 3:
					// RGB
					// Bits per pixel = 3 * ColoursPerRGB.
					// So 1bpp is impossible with PPM files, use pbm instead
					if (ColoursPerRGB > 2 && ColoursPerRGB <= 255)
					{
						BitsPerPixel = 24;
						BitsPerRGB = 8;	
					}
					
					if (ColoursPerRGB > 1 && ColoursPerRGB <= 2)
					{
						BitsPerPixel = 8;
						BitsPerRGB = 2;	
					}
					
					if (ColoursPerRGB > 0 && ColoursPerRGB <= 1)
					{
						BitsPerPixel = 4;
						BitsPerRGB = 1;	
					}
					break;

				case 1:
					// Greyscale or B & W
					if (ColoursPerRGB > 15 && ColoursPerRGB <= 255)
					{
						BitsPerPixel = 8;
						BitsPerRGB = 8;	
					}
					
					if (ColoursPerRGB > 1 && ColoursPerRGB <= 15)
					{
						BitsPerPixel = 4;
						BitsPerRGB = 4;	
					}

					if (ColoursPerRGB > 0 && ColoursPerRGB <= 1)
					{
						BitsPerPixel = 1;
						BitsPerRGB = 1;
					}
					break;

				default:
					// Stop parsing as something bad has gone wrong
					TRACEUSER( "Neville", _T("BasePMFilter::ReadFromFile bad NumberOfColours %d\n"),NumberOfColours);
					pLexFile->GotError( _R(IDE_BADFORMAT) );
					break;
			}

TRACEUSER( "Neville", _T("BitsPerPixel = %d\n"), BitsPerPixel);

			if (
				(BitsPerPixel != 24) && (BitsPerPixel != 8) &&
				(BitsPerPixel != 4) && (BitsPerPixel != 1)
			   )
				pLexFile->GotError( _R(IDE_FORMATNOTSUPPORTED) );

			// we know what sort of bitmap we are - lets allocate a new LPBITMAPINFO and some bytes
			*Info = AllocDIB( Width, Height, BitsPerPixel, Bits, NULL );

			if (*Info == NULL || *Bits == NULL)
				pLexFile->GotError( _R(IDS_OUT_OF_MEMORY) );
			
			// If necessary, poke in a correct palette
			switch (BitsPerPixel)
			{
				case 8:
					// Make them all greyscale for now as we should only encounter this
					// on PGMs
					MakeGreyScalePalette((*Info)->bmiColors);				
					break;
				case 4:
					Make16GreyScalePalette((*Info)->bmiColors);				
					break;
				case 1:
					MakeBlackAndWhitePalette((*Info)->bmiColors);				
					break;
			}


			// Work out the word/byte rounded line width rather than the pixel width
			INT32 WidthOfLine = DIBUtil::ScanlineSize( Width, BitsPerPixel );

			switch (TypeOfPPM)
			{
				case PPM_ASCII:
				case PGM_ASCII:
				case PBM_ASCII:
					{
						// Read in the ASCII form of the bitmap data
						// Pass in the lex file form of the file pointer
						ok = ReadASCIIFromFile(pLexFile, *Bits, Width, Height,
												ColoursPerRGB, BitsPerRGB, WidthOfLine,
												ProgressString, FALSE);
					}
					break;

				case PPM_BINARY:
				case PGM_BINARY:
				case PBM_BINARY:
					{
						// First, as we want to read in in binary mode, turn the lexer off
						pLexFile->DeinitLexer();

						// If first byte in binary data is &FF then eof() will have been set
						// and so a random exception will be thrown. Hence, force all bits to
						// be set good.  
						// Nuke any existing open error states that might exist
						// This can cause a random error as when if the first binary byte
						// is an 0xFF, then the eof() bit is more than likely set. This will
						// then throw a random, file does not exist, error on the next read.
						// If we do this we force it into a known state.
						pLexFile->SetGoodState();

						// Read in the ASCII form of the bitmap data.
						ok = ReadBinaryFromFile(pLexFile, *Bits, Width, Height,
												ColoursPerRGB, BitsPerRGB, WidthOfLine,
												ProgressString, FALSE);
					}
				
					break;

				default:
					TRACEUSER( "Neville", _T("BasePMFilter: Bad PPM format\n"));
					pLexFile->GotError( _R(IDE_BADFORMAT) );
					break;
			}
		}		


		// If we reach here and the bitmap allocations are still null then no valid image
		// was found and so we should error now.
		// Might have just been a GIF file with extension tags in and no images!
		if (*Info == NULL || *Bits == NULL) pLexFile->GotError( _R(IDE_BADFORMAT) );

		// We are now finished with the lexer
		pLexFile->DeinitLexer();

		// If started, then stop then progress bar
		if (ProgressString != NULL) EndSlowJob();

		// Must set the exception throwing and reporting flags back to their entry states
		pLexFile->SetThrowExceptions( OldThrowingState );
		pLexFile->SetReportErrors( OldReportingState );

		// er, we seem to have finished OK so say so
		return TRUE;
	}

	CATCH( CFileException, e)
	{
		// catch our form of a file exception
		TRACEUSER( "Neville", _T("PPMFilter::ReadFromFile CC catch handler\n"));

		FreeDIB( *Info, *Bits );							// free any alloced memory
		*Info = NULL;										// and NULL the pointers
		*Bits = NULL;
		
		// We are now finished with the lexer
		pLexFile->DeinitLexer();

		// If started, then stop then progress bar
		if (ProgressString != NULL) EndSlowJob();

		// Must set the exception throwing and reporting flags back to their entry states
		pLexFile->SetThrowExceptions( OldThrowingState );
		pLexFile->SetReportErrors( OldReportingState );
		return FALSE;
	}
	END_CATCH

	ERROR2( FALSE, "Escaped exception clause somehow" );

#endif
}

/********************************************************************************************

>	void BasePMFilter::PokePaletteEntry(LPRGBQUAD Palette, BYTE red, BYTE green, BYTE blue)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		Palette		palette to write to.
				red			red value to enter into palette entry 
				green		green value to enter into palette entry 
				blue		blue value to enter into palette entry
	Outputs:	index incremented by one 
	Returns:	-
	Purpose:	Puts the required value	of rgb into the specified palette entry.
	SeeAlso:	BasePMFilter::MakeGreyScalePalette;
	SeeAlso:	BasePMFilter::MakeBlackAndWhitePalette;

********************************************************************************************/
void BasePMFilter::PokePaletteEntry(LPRGBQUAD *Palette, BYTE red, BYTE green, BYTE blue)
{
	if (Palette && (*Palette))
	{
		(*Palette)->rgbRed = red;	
		(*Palette)->rgbGreen = green;	
		(*Palette)->rgbBlue = blue;	
		// increment the counter and return it
		(*Palette) ++;	
	}
}	

/********************************************************************************************

>	void BasePMFilter::MakeGreyScalePalette(LPRGBQUAD Palette)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		Palette		palette to write to.
	Outputs:	-
	Returns:	-
	Purpose:	Makes up a greyscale palette for the specified palette.

********************************************************************************************/
void BasePMFilter::MakeGreyScalePalette(LPRGBQUAD Palette)
{
	if (Palette)
	{
		LPRGBQUAD Pal = Palette;
		 
		// Poke a greyscale palette into the specified palette
		for (INT32 i = 0; i < 256 ; i++)
		{
			PokePaletteEntry(&Pal, i, i, i);
		}
	}
}	

/********************************************************************************************

>	void BasePMFilter::Make16GreyScalePalette(LPRGBQUAD Palette)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		Palette		palette to write to.
	Outputs:	-
	Returns:	-
	Purpose:	Makes up a greyscale palette for the specified palette.

********************************************************************************************/
void BasePMFilter::Make16GreyScalePalette(LPRGBQUAD Palette)
{
	if (Palette)
	{
		INT32 Value = 0;
		LPRGBQUAD Pal = Palette;

		// Poke a greyscale palette into the specified palette
		for (INT32 i = 0; i < 16 ; i++)
		{
			Value = (i * 256)/16; 		
			PokePaletteEntry(&Pal, Value, Value, Value);
		}
	}
}	

/********************************************************************************************

>	void BasePMFilter::MakeBlackAndWhitePalette(LPRGBQUAD Palette)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		Palette		palette to write to.
	Outputs:	-
	Returns:	-
	Purpose:	Corrects the first two palette entries to be a correct Gavin black and white
				palette.
	SeeAlso:	OutputDIB::StartFile; OutputDIB::StartExport;

********************************************************************************************/
void BasePMFilter::MakeBlackAndWhitePalette(LPRGBQUAD Palette)
{
	if (Palette)
	{
		LPRGBQUAD Pal = Palette;

		PokePaletteEntry(&Pal, 0xff, 0xff, 0xff);
		PokePaletteEntry(&Pal, 0x00, 0x00, 0x00);
	}
}	

/********************************************************************************************

>	BOOL BasePMFilter::ReadASCIIFromFile( CCLexFile *pLexFile, LPBYTE pBits,
								  		  INT32 Width, INT32 Height, INT32 ColoursPerRGB, INT32 BitsPerRGB,
										  INT32 WidthOfLine,
										  String_64 *ProgressString = NULL, BOOL Forwards = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		pLexFile		An opened CCLexFile that can be read from, already set up to
								be lexed and at the position to read in the first item of
								ascii data defining the bitmap bits.
				pBits			Pointer to where to put the bytes of data
				Width			Read in width of the bitmap
				Height			Read in height of the bitmap
								(together define size of data to read in)
				ColoursPerRGB	Read in number of colours per RGB component
				BitsPerRGB		Calculated number of bits per RGB component
				WidthOfLine		Is the actual rounded width of the line in pixels
				ProgressString 	Allows the user to specify whether they require a progress
								hourglass or not. If NULL then none is shown, otherwise an
								progress bar is shown using the text supplied. Defaults to
								NULL i.e. no progress bar.
				Forwards		True if want data placed in bitmap top to bottom, otherwise
								put it in from bottom to top.
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Reads a bitmap data for the ppm, pgm or pbm file into memory. Assumes data
				is in ascii format.
				A progress hourglass can be shown if required.
	Errors:		Calls SetError on FALSE returns.
	Scope:		Static, Public
	SeeAlso:	BasePMFilter::ReadFromFile;

********************************************************************************************/

BOOL BasePMFilter::ReadASCIIFromFile( CCLexFile *pLexFile, LPBYTE pBits,
									  INT32	Width, INT32 Height, INT32 ColoursPerRGB, INT32 BitsPerRGB,
									  INT32 WidthOfLine,
									  String_64 *ProgressString, BOOL Forwards)
{
	ERROR2IF(pLexFile == NULL,FALSE,"PPMFilter::ReadASCIIFromFile bad file pointer");
	ERROR2IF(pBits == NULL,FALSE,"PPMFilter::ReadASCIIFromFile bad bitmap data pointer");
	ERROR2IF(Width == 0,FALSE,"PPMFilter::ReadASCIIFromFile bad Width");
	ERROR2IF(Height == 0,FALSE,"PPMFilter::ReadASCIIFromFile bad Height");
	ERROR2IF(ColoursPerRGB == 0,FALSE,"PPMFilter::ReadASCIIFromFile bad ColoursPerRGB");
	ERROR2IF(BitsPerRGB == 0,FALSE,"PPMFilter::ReadASCIIFromFile bad BitsPerRGB");

	// read in the rest of the header data
	BOOL finished = FALSE;
	BOOL ok = TRUE;	

	// Token buffer remains constant until lexer deinitialisation
	const TCHAR* TokenBuf = pLexFile->GetTokenBuf();
	PpmTokenIndex Token;

	// number of bits already written to byte (only relevant for <8bpp images)
	INT32 Count 	= 0;

	Red 		= 0;
	Green 		= 0;
	Blue	 	= 0;
	Bits 		= 0;

	x	 		= 0;
	// (ChrisG 24/01/01) start at the last line if we're going backwards, otherwise start 
	//	at the beginning.
	if (Forwards)
		y = 0;
	else
		y = Height - 1;

	TotalWidth 			= Width;
	TotalHeight 		= Height;
	TotalWidthOfLine 	= WidthOfLine;
	RGBColours 			= ColoursPerRGB;
	RGBBits				= BitsPerRGB;

	INT32 Number 	= 0; 

	LPBYTE pData;
	BYTE value 	= 0;

	BOOL NextPixel = TRUE;

	// (ChrisG 24/01/01) Start at the beginning if going forwards, or the end if going 
	//	backwards, 
	if (Forwards)
		pData = pBits;
	else
		pData = pBits + (y * WidthOfLine);

	while (ok && !finished)
	{
		// Grab a token
		ok = pLexFile->GetSimpleToken();

		LexTokenType TokenType = pLexFile->GetTokenType();

		switch (TokenType)
		{
			case TOKEN_NORMAL:
				{
					Token = FindToken(TokenBuf);

					switch (Token)
					{
						case TOKEN_NONE:
							{
								// Must be one of the header numbers
								ok = (_stscanf(TokenBuf,"%d",&Number) == 1);

								// Call the correct filters function to put the data into the bitmap itself
								// in its own unique way, so long as we haven't failed already.
								ok = ok && ReadDataIntoBitmap(Number, &Count, &pData, &NextPixel);

								// As a read might be part of a pixel or not, e.g. the R of a 24 bit pixel,
								// the recipient of the function call is allowed to say when we are at the next pixel
								if (NextPixel)
								{
									// Increment the required byte number, if at end of line
									// go to the start of the next
									x++;
									if (x >= Width)
									{
										x = 0;

										if (Forwards)
										{
											// Increment the current line number, if at end of lines
											// then we have supposedly read in all the data
											y++;
											if (y >= Height)
												finished = TRUE;
											
											// Update the progress system, if required
											if (ProgressString != NULL)
												ContinueSlowJob((INT32)(100*y/Height));
										}
										else				
										{
											// Decrement the current line number, if past first line
											// then we have supposedly read in all the data
											y--;
											if (y < 0)
												finished = TRUE;
											
											// Update the progress system, if required
											if (ProgressString != NULL)
												ContinueSlowJob((INT32)(100*(Height - y)/Height));
										}

										// (ChrisG 24/01/01) if we're halfway through a byte, pad the 
										//	remainder with zeroes before writing it out.
										while ((Count != 0) && (ok == TRUE))
										{
											ok = ok && ReadDataIntoBitmap(0, &Count, &pData, &NextPixel);
										}

										// realign the data pointer to the line start to 
										// account for word aligning of line starts.
										pData = pBits + (y * WidthOfLine);
									}
								}
							}
							break;
																								
						default:
							// Flag a bad token so that we stop parsing this line
							ok = FALSE;
							TRACEUSER( "Neville", _T("PPM: Didn't expect to get this token ('%s')\n"),TokenBuf);
							break;
					}
				}
				break;

			default:
				// bad token so stop parsing this line
				TRACEUSER( "Neville", _T("PPM: Unexpected token - %s\n"),TokenBuf);
				ok = FALSE;
				break;
		}
	}
		
	return TRUE;
}	

/********************************************************************************************

>	BOOL BasePMFilter::ReadBinaryFromFile( CCLexFile *pFile, LPBYTE pBits,
									 	   INT32	Width, INT32 Height, INT32 ColoursPerRGB, INT32 BitsPerRGB,
										   INT32 WidthOfLine,
										   String_64 *ProgressString = NULL, BOOL Forwards = FALSE )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		pFile			An opened CCFile that can be read from, already set up to
								the position to read in the first item of binary data.
				pBits			Pointer to where to put the bytes of data
				Width			Read in width of the bitmap
				Height			Read in height of the bitmap
								(together define size of data to read in)
				ColoursPerRGB	Read in number of colours per RGB component
				BitsPerRGB		Calculated number of bits per RGB component
				WidthOfLine		Is the actual rounded width of the line in pixels
				ProgressString 	Allows the user to specify whether they require a progress
								hourglass or not. If NULL then none is shown, otherwise an
								progress bar is shown using the text supplied. Defaults to
								NULL i.e. no progress bar.
				Forwards		True if want data placed in bitmap top to bottom, otherwise
								put it in from bottom to top.
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Reads a bitmap data for the ppm, pgm or pbm file into memory. Assumes data
				is in binary format.
				A progress hourglass can be shown if required.
	Errors:		Calls SetError on FALSE returns.
	Scope:		Static, Public
	SeeAlso:	BasePMFilter::ReadFromFile;

********************************************************************************************/

BOOL BasePMFilter::ReadBinaryFromFile( CCLexFile *pFile, LPBYTE pBits,
									   INT32	Width, INT32 Height, INT32 ColoursPerRGB, INT32 BitsPerRGB,
									   INT32 WidthOfLine,
									   String_64 *ProgressString, BOOL Forwards)
{
	ERROR2IF(pFile == NULL,FALSE,"PPMFilter::ReadBinaryFromFile bad file pointer");
	ERROR2IF(pBits == NULL,FALSE,"PPMFilter::ReadBinaryFromFile bad bitmap data pointer");
	ERROR2IF(Width == 0,FALSE,"PPMFilter::ReadBinaryFromFile bad Width");
	ERROR2IF(Height == 0,FALSE,"PPMFilter::ReadBinaryFromFile bad Height");
	ERROR2IF(ColoursPerRGB == 0,FALSE,"PPMFilter::ReadBinaryFromFile bad ColoursPerRGB");
	ERROR2IF(BitsPerRGB == 0,FALSE,"PPMFilter::ReadBinaryFromFile bad BitsPerRGB");

	// read in the rest of the bitmap data
	BOOL finished = FALSE;
	BOOL ok = TRUE;	

	INT32 Count 	= 0;

	Red 		= 0;
	Green 		= 0;
	Blue	 	= 0;
	Bits 		= 0;

	x	 		= 0;
	y 			= 0;
	if (!Forwards)
		y = Height;

	TotalWidth 			= Width;
	TotalHeight 		= Height;
	TotalWidthOfLine 	= WidthOfLine;
	RGBColours 			= ColoursPerRGB;
	RGBBits				= BitsPerRGB;
			
	INT32 Number		= 0; 
	TCHAR chr;

	BOOL NextPixel 	= TRUE;

	LPBYTE pData 	= pBits;

	while (ok && !finished)
	{
		// Grab a token
		pFile->read(&chr);
		
		Number = (INT32)chr;

		// Call the correct filters function to put the data into the bitmap itself
		// in its own unique way
		ok = ReadDataIntoBitmap((INT32)Number, &Count, &pData, &NextPixel);

		// As a read might be part of a pixel or not, e.g. the R of a 24 bit pixel,
		// the recipient of the function call is allowed to say when we are at the next pixel
		if (NextPixel)
		{
			// Increment the required byte number, if at end of line
			// go to the start of the next
			x = x + PixelCounter;
			if (x >= Width)
			{
				x = 0;

				if (Forwards)
				{
					// Increment the current line number, if at end of lines
					// then we have supposedly read in all the data
					y++;
					if (y >= Height)
						finished = TRUE;
					
					// Update the progress system, if required
					if (ProgressString != NULL)
						ContinueSlowJob((INT32)(100*y/Height));
				}
				else				
				{
					// Decrement the current line number, if at first line
					// then we have supposedly read in all the data
					y--;
					if (y <= 0)
						finished = TRUE;
					
					// Update the progress system, if required
					if (ProgressString != NULL)
						ContinueSlowJob((INT32)(100*(Height - y)/Height));
				}

				// realign the data pointer to the line start to 
				// account for word aligning of line starts
				pData = pBits + y * WidthOfLine;
			}
		}
	}

	return TRUE;
}	


/********************************************************************************************

>	virtual BOOL BasePMFilter::ReadDataIntoBitmap(INT32 Number, INT32 * Count, LPBYTE * pData,
												  BOOL * NextPixel)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		Number	value read in;
	Outputs:	Count	can be used to distinquish between R, G and B data (can be incremented)
				pData	place to store the data (can be incremented)
	Returns:	TRUE if everything went ok, FALSE otherwise.
	Purpose:	Place the number read in into the specified place in the bitmap.
				Baseclass version, should not be called.
	SeeAlso:	BasePMFilter::ReadBinaryFromFile; BasePMFilterReadASCIIFromFile;

********************************************************************************************/

BOOL BasePMFilter::ReadDataIntoBitmap(INT32 Number, INT32 * Count, LPBYTE * pData, BOOL * NextPixel)
{
	ERROR2(FALSE,"BasePMFilter::ReadDataIntoBitmap calling baseclass version!")	
	return FALSE;	
}


/********************************************************************************************

>	virtual BOOL PPMFilter::ReadDataIntoBitmap(INT32 Number, INT32 * Count, LPBYTE * pData)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		Number	value read in;
	Outputs:	Count	can be used to distinquish between R, G and B data (can be incremented)
				pData	place to store the data (can be incremented)
	Returns:	TRUE if everything went ok, FALSE otherwise.
	Purpose:	Place the number read in into the specified place in the bitmap.
	SeeAlso:	BasePMFilter::ReadBinaryFromFile; BasePMFilterReadASCIIFromFile;

********************************************************************************************/

BOOL PPMFilter::ReadDataIntoBitmap(INT32 Number, INT32 * Count, LPBYTE * pData, BOOL * NextPixel)
{
	// By default, do not move onto the next pixel
	*NextPixel = FALSE;

	BOOL ok = TRUE;

	BYTE value 	= 0;

	// Now work out what to do with this
	switch (*Count)
	{
		case 0:
			Red = Number;
			break;

		case 1:
			Green = Number;
			break;

		case 2:
			Blue = Number;
			break;

		default:
			// Flag a bad token so that we stop parsing this line
			ok = FALSE;
			TRACEUSER( "Neville", _T("PPM: Didn't expect to get this number ('%d')\n"), Number);
			break;
	}
	
	// Increment our count and see if we have enough info yet
	(*Count)++;

	if ((*Count) == 3)
	{
		// We have read in the three colour components so stuff
		// them in the required byte
		// Now work out what to do with this
		switch (RGBBits)
		{
			case 8:
				// Poke those RGB bytes into the next three bytes in memory
				*((*pData)++) = Blue & RGBColours;
				*((*pData)++) = Green & RGBColours;
				*((*pData)++) = Red & RGBColours;
				break;

			case 2:
				value = (Red & RGBColours);
				value = (value << RGBBits) + (Green & RGBColours);
				value = (value << RGBBits) + (Blue & RGBColours);
				*((*pData)++) = value;
				break;

			default:
				// Shouldn't get here so error 2
				ERROR2(FALSE, "PPMFilter::ReadDataIntoBitmap bad ColoursPerRGB");
				break;
		}

		// reset the count
		*Count = 0;

		// Say we want to move onto the next pixel
		*NextPixel = TRUE;
	}

	return ok;	
}

/********************************************************************************************

>	virtual BOOL PGMFilter::ReadDataIntoBitmap(INT32 Number, INT32 * Count, LPBYTE * pData,
											   BOOL * NextPixel)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		Number	value read in;
	Outputs:	Count	can be used to distinquish between R, G and B data
				pData	place to store the data (can be incremented)
	Returns:	TRUE if everything went ok, FALSE otherwise.
	Purpose:	Place the number read in into the specified place in the bitmap.
	SeeAlso:	BasePMFilter::ReadBinaryFromFile; BasePMFilterReadASCIIFromFile;

********************************************************************************************/

BOOL PGMFilter::ReadDataIntoBitmap(INT32 Number, INT32 * Count, LPBYTE * pData, BOOL * NextPixel)
{
	// By default, do not move onto the next pixel
	*NextPixel = FALSE;

	// Work out what to do with the data
	switch (RGBBits)
	{
		case 8:
			// Poke those RGB bytes into the next three bytes in memory
			*((*pData)++) = Number & RGBColours;
			// Always move onto the next pixel
			*NextPixel = TRUE;
			break;

		case 4:
			// 4bpp so we must put the data into either the high or low nibble.
			// This will be dependent on whether we are on an odd or even pixel.
			// So test the LSBit of the curx, if set we will be odd.
			// Only move onto next byte every other pixel hence curx/2.
			// Get whole present byte 
			if (x & 1)
			{
				**(pData) = ((**(pData)) & 0xF0) | (Number & 0x0F);		 // add into low nibble 
				*NextPixel = FALSE;
			}
			else
			{
				**(pData) = ((**(pData)) & 0x0F) | ((Number << 4) & 0xF0); // add into top nibble
				(*pData)++;
				*NextPixel = TRUE;
			}
			break;

		default:
			// Shouldn't get here so error 2
			ERROR2(FALSE, "PGMFilter::ReadDataIntoBitmap bad ColoursPerRGB");
			break;
	}

	return TRUE;	
}

/********************************************************************************************

>	virtual BOOL PBMFilter::ReadDataIntoBitmap(INT32 Number, INT32 * Count, LPBYTE * pData,
											   BOOL * NextPixel)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		Number		value read in;
	Outputs:	Count		number of bytes already read in
				pData		place to store the data (can be incremented)
				NextPixel	whether this pixel is finished or not
	Returns:	TRUE if everything went ok, FALSE otherwise.
	Purpose:	Place the number read in into the specified place in the bitmap.
	SeeAlso:	BasePMFilter::ReadBinaryFromFile; BasePMFilterReadASCIIFromFile;

********************************************************************************************/

BOOL PBMFilter::ReadDataIntoBitmap(INT32 Number, INT32 * Count, LPBYTE * pData, BOOL * NextPixel)
{
	// (ChrisG 24/01/01) NextPixel should never be set to false for single component images,
	//	as it is supposed to show whether we're halfway through a pixel (e.g. we've written
	//	the R and G parts of an RGB pixel, but still have to write the B part), NOT halfway
	//	through a byte. That's what the Count variable is for.
	*NextPixel = TRUE;

	if (TypeOfPPM == PBM_BINARY)
	{
		// The bits are stored eight per byte, high bit first low bit last.
		// Work out what to do with the data
		switch (RGBBits)
		{
			case 1:
				// Poke this monochrome byte into the next byte in memory
				*((*pData)++) = Number;
				break;

			default:
				// Shouldn't get here so error 2
				ERROR2(FALSE, "PBMFilter::ReadDataIntoBitmap bad ColoursPerRGB");
				break;
		}

		// Reset the count
		*Count = 0;
	}
	else
	{
		// Must be 1 bpp
		Bits = (Bits << 1) | (Number & RGBBits);

		// Increment our count and see if we have enough info yet
		(*Count)++;

		if (*Count == 8)
		{
			// Work out what to do with the data
			switch (RGBBits)
			{
				case 1:
					// Poke those RGB bytes into the next three bytes in memory
					*((*pData)++) = Bits;
					break;

				default:
					// Shouldn't get here so error 2
					ERROR2(FALSE, "PBMFilter::ReadDataIntoBitmap bad ColoursPerRGB");
					break;
			}

			// Reset the count
			*Count = 0;
			
			// Reset the bits accumulator
			Bits = 0;
		}
	}

	return TRUE;	
}





/********************************************************************************************

>	virtual BOOL BasePMFilter::IsThisBppOk(UINT32 Bpp)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/95
	Inputs:		Bpp or Colour depth.
	Returns:	TRUE if this filter can cope with this colour depth, FALSE otherwise.
	Purpose:	Check if this Bitmap filter can cope with saving at this Bpp/Colour depth.
	SeeAlso:	OpMenuExport::DoWithParam;

********************************************************************************************/

BOOL BasePMFilter::IsThisBppOk(UINT32 Bpp)
{
	return (Bpp == 1 || Bpp == 4 || Bpp == 8 || Bpp == 24);
}

