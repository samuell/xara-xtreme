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
// The Native file format for Camelot first version.
// This should be the true all singing replacement for the bodge EPS form.

/*
*/

#include "camtypes.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camdoc.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "native.h"
//#include "rik.h"
//#include "oilfltrs.h" - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "filtrres.h"

#include "cxfile.h"
#include "cxftags.h"
#include "xarprefs.h"
#include "webprefs.h"	// Web options dialog handler, WebPrefsDlgParam class

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNAMIC(CamelotNativeFilter, BaseCamelotFilter);
CC_IMPLEMENT_DYNAMIC(CamelotNULLNativeFilter,CamelotNativeFilter);

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	CamelotNativeFilter::CamelotNativeFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/96
	Purpose:	Constructor for the Native save/load filter.

********************************************************************************************/

CamelotNativeFilter::CamelotNativeFilter()
{
	// Set up filter descriptions.
	FilterNameID = _R(IDS_FILTERNAME_NATIVE);
	FilterInfoID = _R(IDS_NATIVE_FILTERINFO);
	ImportMsgID  = _R(IDS_IMPORTMSG_NATIVE);
	ExportMsgID  = _R(IDS_EXPORTMSG_NATIVE);

	FilterID = FILTERID_NATIVE;

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	//WEBSTER-Martin-27/01/97
#ifdef WEBSTER
	Flags.CanExport = FALSE;
#else
	Flags.CanExport = TRUE;
#endif //WEBSTER
#else
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#endif

	// Message to show when a problem file is encountered
	BadFileMsgID = _R(IDS_NOT_A_NATIVE_FILE);
}




/********************************************************************************************

>	CamelotNativeFilter::~CamelotNativeFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/96
	Purpose:	Tidies up when the filter is destroyed.

********************************************************************************************/

CamelotNativeFilter::~CamelotNativeFilter()
{

}

/********************************************************************************************

>	BOOL CamelotNativeFilter::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/96
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Initalises the Filter ready for use. Will fail if it can not get enough
				memory to work with.

********************************************************************************************/

BOOL CamelotNativeFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new NativeOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(FilterNameID);
	FilterInfo.Load(FilterInfoID);

	BOOL ok = CreateRecordHandlers();

	if (Camelot.DeclareSection( _T("Filters"), 10))
	{
		// Preference to turn native file compression on or off
		Camelot.DeclarePref( NULL, _T("CompressV2Format"), &BaseCamelotFilter::CompressNative, 0, 1 );
		// Preference to turn xpe bitmap saving on or off
		Camelot.DeclarePref( NULL, _T("SaveXPEBitmaps"), &BaseCamelotFilter::SaveXPEBitmaps, 0, 1 );
		// Preference to decide what bitmap format to use for preview bitmaps 
		Camelot.DeclarePref( NULL, _T("PreviewBitmapFilterType"), &BaseCamelotFilter::PreviewBitmapFilterType, 0, 4 );
		// Preference for the optional export as web pathname when native saving
		Camelot.DeclarePref( NULL, _T("DefaultWebExportFilterPath"), &BaseCamelotFilter::DefaultExportPath);
		// Preference to turn native file checking for similar paths on or off
		Camelot.DeclarePref( NULL, _T("NativeCheckSimilarPaths"), &BaseCamelotFilter::NativeCheckSimilarPaths, 0, 1 );
	}

#ifdef DO_EXPORT

	// Initialise the native file export options dialog.
	if ( ok )
	{
		ok = NativePrefsDlg::Init ();
	}

#endif

	return ok;
}

/********************************************************************************************

>	virtual INT32 CamelotNativeFilter::GetBitmapCompression()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		-
	Returns:	The current state of the bitmap compression.
	Purpose:	Public access to the current bitmap compression which the user has requested
				to use when saving bitmaps in the web format.
				0 means maximum compression e.g. use JPEG filter at 0% lossy compression
				100 means minimum compression e.g. use JPEG filter at 100% lossy compression
				101 - 200 means lossless compression e.g. us PNG filter.
				Could use the no bitmap compression at all e.g. use BMP filter. Unused at present.
				(Very much like the JPEG compression percentage).
	Note:		This overrides the baseclass version to save in the proper native state

********************************************************************************************/

INT32 CamelotNativeFilter::GetBitmapCompression()
{
	// Always use a lossless format such as PNG in native files
	return 200;
}

/********************************************************************************************

>	virtual BOOL CamelotNativeFilter::GetPreviewBitmapExport()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		-
	Returns:	The current state of the export preview bitmap.
	Purpose:	Public access to the current export preview bitmap which the user has requested
				to use when saving bitmaps in the web format.
				True means export a preview bitmap, False means do not.
	Note:		This overrides the baseclass version to save in the proper native state

********************************************************************************************/

BOOL CamelotNativeFilter::GetPreviewBitmapExport()
{
#if (_OLE_VER >= 0x200)
	// Get some documents to check up on.
	ERROR2IF(!TheDocument, FALSE,
				"Null Document* in CamelotNativeFilter::GetPreviewBitmapExport");

	if(TheDocument->IsAClipboard())
		return FALSE;			//	 clipboard docs don't have previews

	CCamDoc* pDoc = TheDocument->GetOilDoc();
	ERROR2IF(!TheDocument->GetOilDoc(), FALSE,
				"Null CCamDoc* in CamelotNativeFilter::GetPreviewBitmapExport");

	// We *don't* export a preview bitmap if the associated document is embedded in an
	// OLE container.  Basically, let the document decide.
	return pDoc->ShouldExportPreview();
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	virtual BOOL CamelotNativeFilter::GetConvertTextToOutlines()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		-
	Returns:	The current state of the convert text to outlines.
	Purpose:	Public access to the current convert text to outlines which the user has requested
				to use when saving in the web format.
				True means convert text to outlines, False means do not.
	Note:		This overrides the baseclass version to save in the proper native state

********************************************************************************************/

BOOL CamelotNativeFilter::GetConvertTextToOutlines()
{
	// In native files ensure text is not converted to outlines
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL CamelotNativeFilter::GetRemoveInvisibleLayers()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		-
	Returns:	The current state of the remove invisible layers.
	Purpose:	Public access to the current remove invisible layers which the user has requested
				to use when saving in the web format.
				True means remove invisible layers, False means do not.
	Note:		This overrides the baseclass version to save in the proper native state

********************************************************************************************/

BOOL CamelotNativeFilter::GetRemoveInvisibleLayers()
{
	// In native files retain inivisible layers
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL CamelotNativeFilter::GetRemoveUnusedColours()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		-
	Returns:	The current state of the remove unused colours.
	Purpose:	Public access to the current remove unused colours which the user has requested
				to use when saving in the web format.
				True means remove unused colours, False means do not.
	Note:		This overrides the baseclass version to save in the proper native state

********************************************************************************************/

BOOL CamelotNativeFilter::GetRemoveUnusedColours()
{
	// In native files retain unused colours
	return FALSE;
}

/********************************************************************************************

>	virtual BaseCamelotFilter* CamelotNativeFilter::CreateNULLFilter(Document* pDocument)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/96
	Inputs:		pDocument = ptr to the document
	Returns:	ptr to a filter that can be used to count the number of exported bytes
				NULL if there's an error
	Purpose:	Creates the relevant NULL filter
	Scope: 		Protected.

********************************************************************************************/

BaseCamelotFilter* CamelotNativeFilter::CreateNULLFilter(Document* pDocument)
{
	BaseCamelotFilter* pFilter = new CamelotNULLNativeFilter(this,pDocument);

	return pFilter;
}

/********************************************************************************************

>	virtual BOOL CamelotNativeFilter::BeginDocumentExport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This function is called just before document-related records are written out
				to the file.

				The native filter writes out a TAG_DOCUMENT record to signify the start of
				the document.

	SeeAlso:	CamelotNativeFilter::EndDocumentExport()
	Scope: 		Protected

********************************************************************************************/

BOOL CamelotNativeFilter::BeginDocumentExport()
{
	BOOL ok = TRUE;

	// Writes out a document rec followed by a down rec
	// This will ensure that all doc-related records appear as a child of the doc record
	if (ok) ok = WriteZeroSizedRecord(TAG_DOCUMENT);
	if (ok) ok = WriteZeroSizedRecord(TAG_DOWN);

	return ok;
}

/********************************************************************************************

>	virtual BOOL CamelotNativeFilter::EndDocumentExport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This function is called after document-related records have been are written out
				to the file.  BeginDocumentExport() is called before the doc records are saved

				This should be called for each document that is saved out to the file.

	SeeAlso:	CamelotNativeFilter::BeginDocumentExport()
	Scope: 		Protected

********************************************************************************************/

BOOL CamelotNativeFilter::EndDocumentExport()
{
	BOOL ok = TRUE;

	// Writes out an up rec
	// This will ensure that all doc-related records appear as a child of the doc record
	// by matching the down rec written out by BeginDocumentExport()
	if (ok) ok = WriteZeroSizedRecord(TAG_UP);

	return ok;
}

INT32 CamelotNativeFilter::HowCompatibleIsFileType(char* pFileType)
{
	if (strcmp(pFileType,GetExportFileType()) == 0)
		return 10;
	else
		return 9;
}

char* CamelotNativeFilter::GetExportFileType()
{
	return EXPORT_FILETYPE_NATIVE;
}

double CamelotNativeFilter::GetSimilarPathTolerance(NodePath* pPath)
{
	return 0.0;
}

/********************************************************************************************

>	INT32 CamelotNativeFilter::GetMaxPathLookUp()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/96
	Inputs:		-
	Returns:	100 always
	Purpose:	The max number pf paths that should be looked at by the function FindSimilarPath().

	SeeAlso:	BaseCamelotFilter::GetMaxPathLookUp;
	Scope: 		Protected

********************************************************************************************/

INT32 CamelotNativeFilter::GetMaxPathLookUp()
{
	return -1;//100;
}




/********************************************************************************************

>	BOOL CamelotNativeFilter::WriteCurrentAttributes()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/03/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if worked, FALSE otherwise
	Purpose:	Write out all current attributes
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamelotNativeFilter::WriteCurrentAttributes()
{
	BOOL ok = TRUE;
	
	if (ok && TheDocument)
	{
		AttributeManager* pAttrMgr = &(TheDocument->GetAttributeMgr());
		if (pAttrMgr)
			ok = pAttrMgr->WriteCurrentAttributes(this);
	}

	return (ok);
}




//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

/********************************************************************************************

>	CamelotNULLNativeFilter::CamelotNULLNativeFilter(BaseCamelotFilter* pThisParentFilter,Document* pDoc)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pThisParentFilter = ptr to the filter that created this NULL filter
				pDoc = ptr to the source doc
	Purpose:	The default constructor

********************************************************************************************/

CamelotNULLNativeFilter::CamelotNULLNativeFilter(BaseCamelotFilter* pThisParentFilter,Document* pDoc)
{
	pParentFilter = pThisParentFilter;

	TheDocument = pDoc;
	pTheSpread = GetFirstSpread(pDoc);
	ERROR3IF(pTheSpread == NULL, "BaseCamelotFilter::DoExport no spread to export");
}
 
/********************************************************************************************

>	virtual CXaraFile* CamelotNULLNativeFilter::CreateCXaraFile()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/96
	Inputs:		-
	Returns:	ptr to a CXaraFile object that can be used for import or export
				NULL if there's an error
	Purpose:	This version creates a NULLXaraFile object that doesn't actually
				import or export any bytes

********************************************************************************************/

CXaraFile* CamelotNULLNativeFilter::CreateCXaraFile()
{
	CXaraFile* pCXaraFile = new NULLXaraFile;

	return pCXaraFile;
}

//--------------------------------------------------------------------------

// WEBSTER - markn 31/1/97
// Replaced with general system
/*
void CamelotNULLNativeFilter::AddTextStoryGroupRefToList(TextStory* pStory,NodeGroup* pGroup)
{
	if (pParentFilter != NULL)
		pParentFilter->AddTextStoryGroupRefToList(pStory,pGroup);
}


NodeGroup* CamelotNULLNativeFilter::FindTextStoryGroupRefGroup(TextStory* pStory)
{
	if (pParentFilter != NULL)
		return pParentFilter->FindTextStoryGroupRefGroup(pStory);

	return NULL;
}

TextStory* CamelotNULLNativeFilter::FindTextStoryGroupRefTextStory(NodeGroup* pGroup)
{
 	if (pParentFilter != NULL)
		return pParentFilter->FindTextStoryGroupRefTextStory(pGroup);

	return NULL;
}
*/

void CamelotNULLNativeFilter::AddNodeGroupRefToList(Node* pNode,NodeGroup* pGroup)
{
	if (pParentFilter != NULL)
		pParentFilter->AddNodeGroupRefToList(pNode,pGroup);
}


NodeGroup* CamelotNULLNativeFilter::FindGroupForThisNode(Node* pNode)
{
	if (pParentFilter != NULL)
		return pParentFilter->FindGroupForThisNode(pNode);

	return NULL;
}

// WEBSTER - markn 11/2/97
// System for writing tag descriptions to the file (introduced for the TAG_MOULD_BOUNDS record) /**/
void CamelotNULLNativeFilter::AddTagDescription(UINT32 Tag, UINT32 ID)
{
	if (pParentFilter != NULL)
		pParentFilter->AddTagDescription(Tag,ID);
}




