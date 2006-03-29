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
// The Web file format for Camelot first version.
// This is the minimalistic form of the new native file filter 

/*
*/

#include "camtypes.h"
#include "app.h"
#include "chapter.h"
#include "nodepath.h"

#include "webfiltr.h"
#include "oilfltrs.h"	// Web oil filter i.e. Windows UI specific details e.g. extension
//#include "filtrres.h"	// IDs
#include "xarprefs.h"	// The native filter options dialogue.
#include "webprefs.h"	// Web options dialog handle.
#include "cxfile.h"		// CXaraFile classes
#include "cxftags.h"	// The tags dude!

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNAMIC(CamelotWebFilter, BaseCamelotFilter);
CC_IMPLEMENT_DYNAMIC(CamelotNULLWebFilter,CamelotWebFilter);

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	CamelotWebFilter::CamelotWebFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Purpose:	Constructor for the Native save/load filter.

********************************************************************************************/

CamelotWebFilter::CamelotWebFilter()
{
	// Set up filter descriptions.
	FilterNameID = _R(IDS_FILTERNAME_WEB);
	FilterInfoID = _R(IDS_WEB_FILTERINFO);
	ImportMsgID  = _R(IDS_IMPORTMSG_WEB);
	ExportMsgID  = _R(IDS_EXPORTMSG_WEB);

	FilterID = FILTERID_WEB;

	Flags.CanImport = TRUE;

#ifndef DO_EXPORT
	Flags.CanExport = FALSE;
#else
	//WEBSTER-Martin-27/01/97
#ifdef WEBSTER
	Flags.CanExport = FALSE;
#else
	Flags.CanExport = TRUE;
#endif //WEBSTER
#endif

	// Message to show when a problem file is encountered
	BadFileMsgID = _R(IDS_NOT_A_WEB_FILE);

	MinWebFormat = FALSE;
}


/********************************************************************************************

>	CamelotWebFilter::~CamelotWebFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Purpose:	Tidies up when the filter is destroyed.

********************************************************************************************/

CamelotWebFilter::~CamelotWebFilter()
{

}

/********************************************************************************************

>	BOOL CamelotWebFilter::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Initalises the Filter ready for use. Will fail if it can not get enough
				memory to work with.

********************************************************************************************/

BOOL CamelotWebFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new WebOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(FilterNameID);
	FilterInfo.Load(FilterInfoID);

	BOOL ok = TRUE;

#ifdef DO_EXPORT
	// Init the export web file options dialog
	ok = WebPrefsDlg::Init();
#endif

	if (ok) ok = CreateRecordHandlers();

	BaseCamelotFilter::DontConvertTheseFontsToOutlines = String_256(_R(IDS_WEB_GUARANTEED_FONTS));

	if (Camelot.DeclareSection("Filters", 15))
	{
		// Preferences form the web export options dialog box

		// Preference for bitmap compression requested by the user
		// 101 -201 means use PNG but remember old JPEG setting
		// 0-100 use JPEG with this quality setting
		Camelot.DeclarePref( NULL, "BitmapCompression", &BaseCamelotFilter::BitmapCompression, 0, 201 );
		// Preference for whether we export a preview bitmap in web files
		Camelot.DeclarePref( NULL, "ExportPreviewBitmap", &BaseCamelotFilter::PreviewBitmapExport, 0, 1 );
		// Preference for whether we convert all text to outlines in web files (baring some assumed fonts)
		Camelot.DeclarePref( NULL, "ConvertTextToOutlines", &BaseCamelotFilter::ConvertTextToOutlines, 0, 1 );
		// Preference for whether we export invisible layers in web files or not (only applicable in non-minimal mode)
		Camelot.DeclarePref( NULL, "RemoveInvisibleLayers", &BaseCamelotFilter::RemoveInvisibleLayers, 0, 1 );
		// Preference for whether we export unused colours in web files or not (only applicable in non-minimal mode)
		Camelot.DeclarePref( NULL, "RemoveUnusedColours", &BaseCamelotFilter::RemoveUnusedColours, 0, 1 );
		// Preference for whether we export an HTML tag to the clipboard
		Camelot.DeclarePref( NULL, "HTMLToClipboard", &BaseCamelotFilter::HTMLToClipboard, 0, 1 );
		// Preference for whether we export a minimal web file or a stripped down native file
		Camelot.DeclarePref( NULL, "MinimalWebFormat", &BaseCamelotFilter::MinimalWebFormat, 0, 1 );
		
		// WEBSTER - markn 28/1/97
		// Preference for whether we convert all blends to outlines in web files
		Camelot.DeclarePref( NULL, "ConvertBlendsToOutlines", &BaseCamelotFilter::ConvertBlendsToOutlines, 0, 1 );

		// Other preferences
		// Preference for whether we write relative paths or not
		Camelot.DeclarePref( NULL, "WriteRelativePaths", &BaseCamelotFilter::WriteRelativePaths, 0, 1 );
		// Preference which lists the fonts not to be converted into outlines
		Camelot.DeclarePref( NULL, "DontConvertTheseFontsToOutlines", &BaseCamelotFilter::DontConvertTheseFontsToOutlines);
		// Preference for the tolerence we use to test for similar paths
		Camelot.DeclarePref( NULL, "WebSimilarPathTolerence", &BaseCamelotFilter::WebSimilarPathTolerence, 0, 500 );

		// Webster - Neville 29/7/97
		// Whether non-frame layers in an imported file are combined onto the active layer or the
		// first non-frame layer in the import
		Camelot.DeclarePref( NULL, "ImportNonFramesOntoActivelayer", &BaseCamelotFilter::ImportNonFramesOntoActivelayer, 0, 1 );
	}

	// All ok?
	return ok;
}

/********************************************************************************************

>	virtual BaseCamelotFilter* CamelotWebFilter::CreateNULLFilter(Document* pDocument)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/96
	Inputs:		pDocument = ptr to the document
	Returns:	ptr to a filter that can be used to count the number of exported bytes
				NULL if there's an error
	Purpose:	Creates the relevant NULL filter
	Scope: 		Protected.

********************************************************************************************/

BaseCamelotFilter* CamelotWebFilter::CreateNULLFilter(Document* pDocument)
{
	BaseCamelotFilter* pFilter = new CamelotNULLWebFilter(this,pDocument);

	return pFilter;
}

/********************************************************************************************

>	virtual BOOL CamelotWebFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96
	Inputs:		pcszPathName	pointer to the pathname to check
	Returns:	TRUE
	Purpose:	Web files need a document to live in, so TRUE is returned

				(Unless it's a stupid hybrid, in which case it returns FALSE).
	SeeAlso:	Filter; Filter::IsDefaultDocRequired; CCamDoc::OnOpenDocument;
	SeeAlso:	FilterFamily::DoImport; Filter::DoImport;

********************************************************************************************/

BOOL CamelotWebFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)
{
/*
	// If hybrid format, when destory any elegant code that exists in the system with
	// stupid special cases.
	// The compact native format doesn't need the default doc.
	if (IsCompactNativeFilter())
		return FALSE;

	return TRUE;
*/
	return MinWebFormat;	// if min format, default doc is required
}	

/********************************************************************************************

>	virtual BOOL CamelotWebFilter::BeginDocumentExport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This function is called just before document-related records are written out
				to the file.

				The web filter writes out a TAG_DOCUMENT record to signify the start of
				the document WHEN IT'S THE COMPACT NATIVE FILTER!.

	SeeAlso:	CamelotWebFilter::EndDocumentExport()
	Scope: 		Protected

********************************************************************************************/

BOOL CamelotWebFilter::BeginDocumentExport()
{
	BOOL ok = TRUE;

	// If it's the hybrid format, do as the native filter does
	if (IsCompactNativeFilter())
	{
		// Writes out a document rec followed by a down rec
		// This will ensure that all doc-related records appear as a child of the doc record
		if (ok) ok = WriteZeroSizedRecord(TAG_DOCUMENT);
		if (ok) ok = WriteZeroSizedRecord(TAG_DOWN);
	}

	return ok;
}

/********************************************************************************************

>	virtual BOOL CamelotWebFilter::EndDocumentExport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This function is called after document-related records have been are written out
				to the file.  BeginDocumentExport() is called before the doc records are saved

				This should be called for each document that is saved out to the file.

	SeeAlso:	CamelotWebFilter::BeginDocumentExport()
	Scope: 		Protected

********************************************************************************************/

BOOL CamelotWebFilter::EndDocumentExport()
{
	BOOL ok = TRUE;

	// If it's the hybrid format, do as the native filter does
	if (IsCompactNativeFilter())
	{
		// Writes out an up rec
		// This will ensure that all doc-related records appear as a child of the doc record
		// by matching the down rec written out by BeginDocumentExport()
		if (ok) ok = WriteZeroSizedRecord(TAG_UP);
	}

	return ok;
}

INT32 CamelotWebFilter::HowCompatibleIsFileType(char* pFileType)
{
	MinWebFormat = TRUE;		// Default to needing default doc
	INT32 compatibility = 10;

	if (strcmp(pFileType,EXPORT_FILETYPE_WEB) == 0)			// If web file, not minimal so no default doc required
		MinWebFormat = FALSE;
	else if (strcmp(pFileType,EXPORT_FILETYPE_MIN) == 0)	// If Minimal web file, default doc is required
		MinWebFormat = TRUE;
	else
		compatibility = 9;									// You what?

	return compatibility;
}

char* CamelotWebFilter::GetExportFileType()
{
	if (IsCompactNativeFilter())
		return EXPORT_FILETYPE_WEB;
	else
		return EXPORT_FILETYPE_MIN;
}

double CamelotWebFilter::GetSimilarPathTolerance(NodePath* pPath)
{
	double Tolerance = 0.0;

	if (pPath != NULL)
	{
		// Don't get attribute-related bounds because the path might not be in the tree
		// (e.g. it may be generated by the 'convert text to outlines' system which keeps
		// all paths created from text stories in out-of-line groups)
		// The TRUE parameter means don't bother with attributes
		DocRect Rect = pPath->GetBoundingRect(TRUE);
		INT32 MaxDist = min(Rect.Width(),Rect.Height());

		Tolerance = double(MaxDist)*0.01;
		if (Tolerance > GetWebSimilarPathTolerence())
			Tolerance = GetWebSimilarPathTolerence(); 
	}

	return Tolerance;
}

/********************************************************************************************

>	virtual BOOL CamelotWebFilter::ShouldExportHTMLTag()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/97
	Inputs:		-
	Returns:	The current state of the HTMLToClipboard
	Purpose:	Public access to the HTML to clipboard
	
********************************************************************************************/

BOOL CamelotWebFilter::ShouldExportHTMLTag()
{
	return BaseCamelotFilter::HTMLToClipboard;
}

/********************************************************************************************

>	INT32 CamelotWebFilter::GetMaxPathLookUp()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/97
	Inputs:		-
	Returns:	100 always
	Purpose:	The max number pf paths that should be looked at by the function FindSimilarPath().

	// WEBSTER - markn 11/2/97
	// Added to Webster so saving .web files isn't so slow for certain git files

	SeeAlso:	BaseCamelotFilter::GetMaxPathLookUp;
	Scope: 		Protected

********************************************************************************************/

INT32 CamelotWebFilter::GetMaxPathLookUp()
{
#ifdef WEBSTER
	return 100;
#else
	return -1;
#endif // WEBSTER
}

/********************************************************************************************

>	virtual OpDescriptor* CamelotWebFilter::GetDialogueOp ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/00
	Inputs:		-
	Returns:	The OpDescriptor pointing to the dialogue used by the export proceedure.
	Purpose:	Obtains a pointer to the OpDescriptor to be used to access the relevant
				export dialogue.

********************************************************************************************/

OpDescriptor* CamelotWebFilter::GetDialogueOp ( void )
{
	// Just return the found OpDescriptor.
	return OpDescriptor::FindOpDescriptor ( CC_RUNTIME_CLASS ( WebPrefsDlg ) );
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

/********************************************************************************************

>	CamelotNULLWebFilter::CamelotNULLWebFilter(BaseCamelotFilter* pThisParentFilter,Document* pDoc)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pThisParentFilter = ptr to the filter that created this NULL filter
				pDoc = ptr to the source doc
	Purpose:	The default constructor

********************************************************************************************/

CamelotNULLWebFilter::CamelotNULLWebFilter(BaseCamelotFilter* pThisParentFilter,Document* pDoc)
{
	pParentFilter = pThisParentFilter;

	TheDocument = pDoc;
	pTheSpread = GetFirstSpread(pDoc);
	ERROR3IF(pTheSpread == NULL,"BaseCamelotFilter::DoExport no spread to export");
}
 
/********************************************************************************************

>	virtual CXaraFile* CamelotNULLWebFilter::CreateCXaraFile()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/96
	Inputs:		-
	Returns:	ptr to a CXaraFile object that can be used for import or export
				NULL if there's an error
	Purpose:	This version creates a NULLXaraFile object that doesn't actually
				import or export any bytes

********************************************************************************************/

CXaraFile* CamelotNULLWebFilter::CreateCXaraFile()
{
	CXaraFile* pCXaraFile = new NULLXaraFile;

	return pCXaraFile;
}

void CamelotNULLWebFilter::AddNodeGroupRefToList(Node* pNode,NodeGroup* pGroup)
{
	if (pParentFilter != NULL)
		pParentFilter->AddNodeGroupRefToList(pNode,pGroup);
}


NodeGroup* CamelotNULLWebFilter::FindGroupForThisNode(Node* pNode)
{
	if (pParentFilter != NULL)
		return pParentFilter->FindGroupForThisNode(pNode);

	return NULL;
}
  
// WEBSTER - markn 11/2/97
// System for writing tag descriptions to the file (introduced for the TAG_MOULD_BOUNDS record) /**/
void CamelotNULLWebFilter::AddTagDescription(UINT32 Tag, UINT32 ID)
{
	if (pParentFilter != NULL)
		pParentFilter->AddTagDescription(Tag,ID);
}
