// $Id: ftfilter.cpp 662 2006-03-14 21:31:49Z alex $
// $$
//
// The Flare Template format
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

/*
*/

#include "camtypes.h"
#include "app.h"
#include "chapter.h"
#include "nodepath.h"

#include "ftfilter.h"
#include "oilfltrs.h"	// Web oil filter i.e. Windows UI specific details e.g. extension
//#include "filtrres.h"	// IDs
#include "webparam.h"
#include "xarprefs.h"
#include "webprefs.h"	// Web options dialog handler, WebPrefsDlgParam class
#include "cxfile.h"		// CXaraFile classes
#include "cxftags.h"	// The tags dude!

#include "cxftfile.h"

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNAMIC(FlareTemplateFilter, CamelotWebFilter);

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW



INT32 FlareTemplateFilter::FTBitmapCompression = 75;
BOOL FlareTemplateFilter::FTPreviewBitmapExport = FALSE;
BOOL FlareTemplateFilter::FTConvertTextToOutlines = FALSE;
BOOL FlareTemplateFilter::FTHTMLToClipboard = FALSE;
BOOL FlareTemplateFilter::FTConvertBlendsToOutlines = FALSE;




/********************************************************************************************

>	FlareTemplateFilter::FlareTemplateFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Purpose:	Constructor for the Native save/load filter.

********************************************************************************************/

FlareTemplateFilter::FlareTemplateFilter()
{
	// Set up filter descriptions.
	FilterNameID = _R(IDS_FILTERNAME_FLARETEMPLATE);
	FilterInfoID = _R(IDS_FLARETEMPLATE_FILTERINFO);
	ImportMsgID  = _R(IDS_IMPORTMSG_FLARETEMPLATE);
	ExportMsgID  = _R(IDS_EXPORTMSG_FLARETEMPLATE);

	FilterID = FILTERID_FLARETEMPLATE;

	Flags.CanImport = TRUE;
	Flags.CanExport = TRUE;

	// Message to show when a problem file is encountered
	BadFileMsgID = _R(IDS_NOT_A_FLARETEMPLATE_FILE);
}


/********************************************************************************************

>	FlareTemplateFilter::~FlareTemplateFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Purpose:	Tidies up when the filter is destroyed.

********************************************************************************************/

FlareTemplateFilter::~FlareTemplateFilter()
{

}

/********************************************************************************************

>	BOOL FlareTemplateFilter::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Initalises the Filter ready for use. Will fail if it can not get enough
				memory to work with.

********************************************************************************************/

BOOL FlareTemplateFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new FlareTemplateOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(FilterNameID);
	FilterInfo.Load(FilterInfoID);

	BOOL ok = TRUE;

	if (ok) ok = CreateRecordHandlers();

	if (Camelot.DeclareSection("Filters", 10))
	{
		// Preferences for the Flare Template options dialog box

		// These override the baseclass versions

		// Preference for bitmap compression requested by the user
		// 101 -201 means use PNG but remember old JPEG setting
		// 0-100 use JPEG with this quality setting
		Camelot.DeclarePref( NULL, "FTBitmapCompression", &FlareTemplateFilter::FTBitmapCompression, 0, 201 );
		// Preference for whether we export a preview bitmap in web files
		Camelot.DeclarePref( NULL, "FTExportPreviewBitmap", &FlareTemplateFilter::FTPreviewBitmapExport, 0, 1 );
		// Preference for whether we convert all text to outlines in web files (baring some assumed fonts)
		Camelot.DeclarePref( NULL, "FTConvertTextToOutlines", &FlareTemplateFilter::FTConvertTextToOutlines, 0, 1 );
		// Preference for whether we export an HTML tag to the clipboard
		Camelot.DeclarePref( NULL, "FTHTMLToClipboard", &FlareTemplateFilter::FTHTMLToClipboard, 0, 1 );
		// Preference for whether we convert all blends to outlines in web files
		Camelot.DeclarePref( NULL, "FTConvertBlendsToOutlines", &FlareTemplateFilter::FTConvertBlendsToOutlines, 0, 1 );
	}

	// All ok?
	return ok;
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::SetTotalProgressBarCount(UINT32 n)
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/97
	Inputs:		n - max number for progress bar

********************************************************************************************/

BOOL FlareTemplateFilter::SetTotalProgressBarCount(UINT32 n)
{
	TotalProgressBarCount = n;

	if (TotalProgressBarCount == 0 && GetCCFile() != NULL)
		TotalProgressBarCount = GetCCFile()->Size();
	
	return(IncProgressBarCount(0));
}



/********************************************************************************************

>	virtual INT32 FlareTemplateFilter::HowCompatible(PathName& Filename,
													ADDR  HeaderStart,
													UINT32 HeaderSize,
													UINT32 FileSize)
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/07/97
	Inputs:		FileName	The name of the file being imported
				HeaderStart	The address of the start of the data from the file which needs checking
				HeaderSize	The size of this data that we should check
				FileSize	The total size of the file
	Returns:	10 if the file is fully recognised or 0 otherwise
	Purpose:	returns value in range 0 to 10 which indicates how much we like this file:
					0 => "Definitely Not"
					10 => "Yes Sir, that's my file format"

********************************************************************************************/

INT32 FlareTemplateFilter::HowCompatible(PathName& Filename, ADDR HeaderStart,
										UINT32 HeaderSize, UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	// How compatible we think the file is
	INT32 HowCompatible = 0;

	// Check that we've got enough data to do our check
	if (HeaderSize > 9)
	{
		// Check that the first eight bytes are our magic identifier
		BYTE* pByte = (BYTE*)HeaderStart;
		// If the first 9 bytes are FLARETEXT and the next is a CR or LF then
		// it is most likely our format

		if ((memcmp(pByte, "FLARETEXT", 9) == 0) &&
			((pByte[9] == 13) || (pByte[9] == 10)))
		{
			// We like this file
			HowCompatible = 9;
		}
	}				

//	TRACEUSER( "Gerry", _T("FTFilter::HowCompatible returning = %d\n"), HowCompatible);

	// Return the found value to the caller.
	return(HowCompatible);
}




/********************************************************************************************

>	virtual CXaraFile* FlareTemplateFilter::CreateCXaraFile()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/97
	Inputs:		-
	Returns:	ptr to a CXaraFile derived object that can be used for import or export
				NULL if there's an error
	Purpose:	This overrides the BaseCamelotFilter version to return a CXaraTempateFile

********************************************************************************************/

CXaraFile* FlareTemplateFilter::CreateCXaraFile()
{
	CXaraFile* pCXaraFile = new CXaraTemplateFile;

	if (pCXaraFile != NULL)
		pCXaraFile->SetFilter(this);

	return(pCXaraFile);
}



/********************************************************************************************
/ Start of Preference setting and getting code
********************************************************************************************/

/********************************************************************************************

>	virtual INT32 FlareTemplateFilter::GetBitmapCompression()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		-
	Returns:	The current state of the bitmap compression.
	Purpose:	Public access to the current bitmap compression which the user has requested
				to use when saving bitmaps in the web format.
				0 means maximum compression e.g. use JPEG filter at 0% lossy compression
				100 means minimum compression e.g. use JPEG filter at 100% lossy compression
				101 - 200 means lossless compression e.g. us PNG filter.
				Could use the no bitmap compression at all e.g. use BMP filter. Unused at present.
				(Very much like the JPEG compression percentage).
	Note:		Virtual so that can be overriden in native filter to save in the proper native
				state

********************************************************************************************/

INT32 FlareTemplateFilter::GetBitmapCompression()
{
	return FlareTemplateFilter::FTBitmapCompression;
}

/********************************************************************************************

>	virtual INT32 FlareTemplateFilter::SetBitmapCompression(INT32 NewBmpComp)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		New bitmap compression value
	Returns:	The old current state of the bitmap compression.
	Purpose:	Set up a new state for the the current bitmap compression.
				200 = no compression, 0 = full compression.
				(Very much like the JPEG compression percentage).

********************************************************************************************/

INT32 FlareTemplateFilter::SetBitmapCompression(INT32 NewBmpComp)
{
	INT32 Old = FlareTemplateFilter::FTBitmapCompression;
	FlareTemplateFilter::FTBitmapCompression = NewBmpComp;
	return Old;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::GetPreviewBitmapExport()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		-
	Returns:	The current state of the export preview bitmap.
	Purpose:	Public access to the current export preview bitmap which the user has requested
				to use when saving bitmaps in the web format.
				True means export a preview bitmap, False means do not.
	Note:		Virtual so that can be overriden in native filter to save in the proper native
				state

********************************************************************************************/

BOOL FlareTemplateFilter::GetPreviewBitmapExport()
{
	return FlareTemplateFilter::FTPreviewBitmapExport;
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::SetPreviewBitmapExport(BOOL NewExportPreviewBmp)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		New export preview bitmap state
	Returns:	The old current state of the export preview bitmap.
	Purpose:	Set up a new state for the the current export preview bitmap.
				True means export a preview bitmap, False means do not.

********************************************************************************************/

BOOL FlareTemplateFilter::SetPreviewBitmapExport(BOOL NewExportPreviewBmp)
{
	BOOL Old = FlareTemplateFilter::FTPreviewBitmapExport;
	FlareTemplateFilter::FTPreviewBitmapExport = NewExportPreviewBmp;
	return Old;
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::GetConvertTextToOutlines()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		-
	Returns:	The current state of the convert text to outlines.
	Purpose:	Public access to the current convert text to outlines which the user has requested
				to use when saving in the web format.
				True means convert text to outlines, False means do not.
	Note:		Virtual so that can be overriden in native filter to save in the proper native
				state

********************************************************************************************/

BOOL FlareTemplateFilter::GetConvertTextToOutlines()
{
	return FlareTemplateFilter::FTConvertTextToOutlines;
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::SetConvertTextToOutlines(BOOL NewConvertTextToOutlines)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		New convert text to outlines state
	Returns:	The old current state of the convert text to outlines.
	Purpose:	Set up a new state for the the convert text to outlines.
				True means convert text to outlines, False means do not.

********************************************************************************************/

BOOL FlareTemplateFilter::SetConvertTextToOutlines(BOOL NewConvertTextToOutlines)
{
	BOOL Old = FlareTemplateFilter::FTConvertTextToOutlines;
	FlareTemplateFilter::FTConvertTextToOutlines = NewConvertTextToOutlines;
	return Old;
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::GetConvertBlendsToOutlines()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Returns:	The current state of the convert blends to outlines.
	Purpose:	Public access to the current convert text to outlines which the user has requested
				to use when saving in the web format.
				True means convert blends to outlines, False means do not.

				Added for WEBSTER

	Note:		Virtual so that can be overriden in native filter to save in the proper native
				state

********************************************************************************************/

BOOL FlareTemplateFilter::GetConvertBlendsToOutlines()
{
	return FlareTemplateFilter::FTConvertBlendsToOutlines;
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::SetConvertBlendsToOutlines(BOOL NewConvertBlendsToOutlines)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		New convert blends to outlines state
	Returns:	The old current state of the convert blends to outlines.
	Purpose:	Set up a new state for the the convert blends to outlines.
				True means convert blends to outlines, False means do not.

				Added for WEBSTER

********************************************************************************************/

BOOL FlareTemplateFilter::SetConvertBlendsToOutlines(BOOL NewConvertBlendsToOutlines)
{
	BOOL Old = FlareTemplateFilter::FTConvertBlendsToOutlines;
	FlareTemplateFilter::FTConvertBlendsToOutlines = NewConvertBlendsToOutlines;
	return Old;
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::GetRemoveInvisibleLayers()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		-
	Returns:	The current state of the remove invisible layers.
	Purpose:	Public access to the current remove invisible layers which the user has requested
				to use when saving in the web format.
				True means remove invisible layers, False means do not.
	Note:		Virtual so that can be overriden in native filter to save in the proper native
				state

********************************************************************************************/

BOOL FlareTemplateFilter::GetRemoveInvisibleLayers()
{
	return(FALSE);
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::SetRemoveInvisibleLayers(BOOL NewRemoveInvisibleLayers);

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		New remove invisible layers state
	Returns:	The old current state of the remove invisible layers.
	Purpose:	Set up a new state for the the remove invisible layers.
				True means remove invisible layers, False means do not.

********************************************************************************************/

BOOL FlareTemplateFilter::SetRemoveInvisibleLayers(BOOL NewRemoveInvisibleLayers)
{
	return(FALSE);
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::GetRemoveUnusedColours()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		-
	Returns:	The current state of the remove unused colours.
	Purpose:	Public access to the current remove unused colours which the user has requested
				to use when saving in the web format.
				True means remove unused colours, False means do not.
	Note:		Virtual so that can be overriden in native filter to save in the proper native
				state

********************************************************************************************/

BOOL FlareTemplateFilter::GetRemoveUnusedColours()
{
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::SetRemoveUnusedColours(BOOL NewRemoveUnusedColours)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		New remove unused colours state
	Returns:	The old current state of the remove unused colours.
	Purpose:	Set up a new state for the the remove unused colours.
				True means remove unused colours, False means do not.

********************************************************************************************/

BOOL FlareTemplateFilter::SetRemoveUnusedColours(BOOL NewRemoveUnusedColours)
{
	return(FALSE);
}


/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::GetMinimalWebFormat()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		-
	Returns:	The current state of the min web format flag
	Purpose:	Public access function

********************************************************************************************/

BOOL FlareTemplateFilter::GetMinimalWebFormat()
{
	return(FALSE);
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::SetMinimalWebFormat(BOOL NewMinimalWebFormat)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		New min web format state
	Returns:	The old current state.
	Purpose:	Set up a new state for the min web format flag
				This is always disabled in this filter

********************************************************************************************/

BOOL FlareTemplateFilter::SetMinimalWebFormat(BOOL NewMinimalWebFormat)
{
	return(FALSE);
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::SetHTMLToClipboard(BOOL fNewValue)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		New HTMLToClipboard state
	Returns:	The old HTMLToClipboard state
	Purpose:	Set a new state for HTMLToClipboard

********************************************************************************************/

BOOL FlareTemplateFilter::SetHTMLToClipboard(BOOL fNewValue)
{
	BOOL Old = FlareTemplateFilter::FTHTMLToClipboard;
	FlareTemplateFilter::FTHTMLToClipboard = fNewValue;
	return Old;
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::GetHTMLToClipboard()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		-
	Returns:	The current state of the HTMLToClipboard
	Purpose:	Public access to the HTML to clipboard
	
********************************************************************************************/

BOOL FlareTemplateFilter::GetHTMLToClipboard()
{
	return(FlareTemplateFilter::FTHTMLToClipboard);
}

/********************************************************************************************

>	virtual BOOL FlareTemplateFilter::ShouldExportHTMLTag()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/97
	Inputs:		-
	Returns:	The current state of the HTMLToClipboard
	Purpose:	Public access to the HTML to clipboard
	
********************************************************************************************/

BOOL FlareTemplateFilter::ShouldExportHTMLTag()
{
	return(FlareTemplateFilter::FTHTMLToClipboard);
}



/********************************************************************************************
/ End of Preference setting and getting code
********************************************************************************************/
