// $Id: infocomp.cpp 662 2006-03-14 21:31:49Z alex $
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

// The document component for the document's information.

// This is to allow details such as the comment for this document to be output
// to the new file format and loaded back in.

/*
*/

#include "camtypes.h"

#include "infocomp.h"

#include "basedoc.h"
#include "app.h"

#include "camfiltr.h"	// BaseCamelotFilter - version 2 native filter
#include "cxftags.h"	// TAG_DOCUMENTCOMMENT
#include "cxfdefs.h"	// 
#include "cxfile.h"		// CXF_UNKNOWN_SIZE
//#include "expcol.h"		// ExportedColours handling class
#include "cxfrec.h"		// CXaraFileRecord handler

//#include "exphint.h"	// ExportHint class

DECLARE_SOURCE("$Revision: 662 $");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

CC_IMPLEMENT_DYNAMIC(DocInfoComponentClass, 	DocComponentClass)
CC_IMPLEMENT_DYNAMIC(DocInfoComponent, 		DocComponent)

/********************************************************************************************
********************************************************************************************/

/********************************************************************************************

>	BOOL DocInfoComponentClass::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Returns:	TRUE if all went well;
				FALSE if not.
	Purpose:	Register the unit list document component with the main application.
	Errors:		Out of memory.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocInfoComponentClass::Init()
{
	// Instantiate a component class to register with the application.
	DocInfoComponentClass *pClass = new DocInfoComponentClass;
	if (pClass == NULL)
		return FALSE;

	// Register it
	GetApplication()->RegisterDocComponent(pClass);

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL DocInfoComponentClass::AddComponent(BaseDocument *pDocument)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pDocument - the document to add the unit list to.
	Returns:	TRUE if the unit list was added ok;
				FALSE if not.
	Purpose:	Add a unit list component to the specified document.
	Errors:		Out of memory
	SeeAlso:	DocInfoComponentClass

********************************************************************************************/

BOOL DocInfoComponentClass::AddComponent(BaseDocument *pDocument)
{
	// Check to see if this document already has a colour list; if so, leave it alone.
	if (pDocument->GetDocComponent(CC_RUNTIME_CLASS(DocInfoComponent)) != NULL)
		return TRUE;

	// Ok - create the colour list component using this list.
	DocInfoComponent *pComponent = new DocInfoComponent();
	if (pComponent == NULL)
	{
		// Out of memory...
		return FALSE;
	}

	// All ok - add the component to the document.
	pDocument->AddDocComponent(pComponent);
	return TRUE;
}



/********************************************************************************************

>	DocInfoComponent::DocInfoComponent()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Construct a unit list component.

********************************************************************************************/

DocInfoComponent::DocInfoComponent()
{
}

/********************************************************************************************

>	DocInfoComponent::~DocInfoComponent()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Purpose:	Clean up a unit list component's data structures.
	SeeAlso:	DocInfoComponent

********************************************************************************************/

DocInfoComponent::~DocInfoComponent()
{
}

/********************************************************************************************
********************************************************************************************/

// Remove the following code on all builds but those who want it

/********************************************************************************************
********************************************************************************************/

/********************************************************************************************

>	BOOL DocInfoComponent::StartImport(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
	Returns:	TRUE if the component was able to prepare for importing;
				FALSE if not (e.g. out of memory)
	Purpose:	Inform the unit list document component that a Native or Web import is
				about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocInfoComponent::StartImport(BaseCamelotFilter *pFilter)
{
TRACEUSER( "Neville", _T("DocInfoComponent::StartImport\n"));
	if (pFilter == NULL)
	{
		ERROR3("DocInfoComponent::EndExport filter is null!");
		return TRUE;
	}
	
	return TRUE;
}

/********************************************************************************************

>	BOOL DocInfoComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.
	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)
	Purpose:	Inform the unit list document component that a Native or Web import has
				just finished.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocInfoComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)
{
TRACEUSER( "Neville", _T("DocInfoComponent::EndImport\n"));
	if (pFilter == NULL)
	{
		ERROR3("DocInfoComponent::EndImport filter is null!");
		return TRUE;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL DocInfoComponent::ImportDocumentComment(CXaraFileRecord* pCXaraFileRecord, Document * pDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
				pDoc			 = document to apply the information to		
	Returns:	TRUE if the component was able to import the data correctly;
				FALSE if not (e.g. out of memory)
	Purpose:	Import the document comment from the current record.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocInfoComponent::ImportDocumentComment(CXaraFileRecord* pCXaraFileRecord, Document * pDoc)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE," DocInfoComponent::ImportDocumentComment pCXaraFileRecord is NULL");

	String_256 Comment; 
	pCXaraFileRecord->ReadUnicode(&Comment);//Comment, Comment.MaxLength());
	if (pDoc)
	{
		pDoc->SetComment(&Comment);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL DocInfoComponent::ImportDocumentDates(CXaraFileRecord* pCXaraFileRecord, Document * pDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
				pDoc			 = document to apply the information to		
	Returns:	TRUE if the component was able to import the data correctly;
				FALSE if not (e.g. out of memory)
	Purpose:	Import the document information from the current record.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocInfoComponent::ImportDocumentDates(CXaraFileRecord* pCXaraFileRecord, Document * pDoc)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE," DocInfoComponent::ImportDocumentDates pCXaraFileRecord is NULL");

	// Read in the times/dates from the record
	INT32 Creation;
	INT32 LastSaved;

	pCXaraFileRecord->ReadINT32(&Creation);
	pCXaraFileRecord->ReadINT32(&LastSaved);
	if (pDoc)
	{
		pDoc->SetCreationTime((time_t)Creation);
		pDoc->SetLastSaveTime((time_t)LastSaved);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL DocInfoComponent::ImportUndoBufferSize(CXaraFileRecord* pCXaraFileRecord, Document * pDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
				pDoc			 = document to apply the information to		
	Returns:	TRUE if the component was able to import the data correctly;
				FALSE if not (e.g. out of memory)
	Purpose:	Import the document undo buffer size from the current record.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocInfoComponent::ImportUndoBufferSize(CXaraFileRecord* pCXaraFileRecord, Document * pDoc)
{
	PORTNOTETRACE("other","DocInfoComponent::ImportUndoBufferSize - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(pCXaraFileRecord == NULL,FALSE," DocInfoComponent::ImportUndoBufferSize pCXaraFileRecord is NULL");

	// Read the size of the operation history from the record
	UINT32 UndoBufSize = 0; 
	pCXaraFileRecord->ReadUINT32(&UndoBufSize);

	if (pDoc)
	{
		// Minimum size = 1K
		if (UndoBufSize < 1024)
			UndoBufSize = 1024;

		// Set the size of the Operation History in
		// the document.
		pDoc->GetOpHistory().SetNewMaxSize(UndoBufSize);
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL DocInfoComponent::ImportDocumentFlags(CXaraFileRecord* pCXaraFileRecord, Document * pDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
				pDoc			 = document to apply the information to		
	Returns:	TRUE if the component was able to import the data correctly;
				FALSE if not (e.g. out of memory)
	Purpose:	Import the document flags from the current record.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocInfoComponent::ImportDocumentFlags(CXaraFileRecord* pCXaraFileRecord, Document * pDoc)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE," DocInfoComponent::ImportDocumentFlags pCXaraFileRecord is NULL");

	// Find the flags we are interested in
	BOOL Multilayer			= FALSE;
	BOOL AllLayersVisible	= FALSE;

	// Read the flags from the record
	UINT32 DocFlags = 0; 
	pCXaraFileRecord->ReadUINT32(&DocFlags);

	if (DocFlags & 2)
		Multilayer = TRUE;
	if (DocFlags & 1)
		AllLayersVisible = TRUE;

	if (pDoc)
	{
		pDoc->SetMultilayer(Multilayer);
		pDoc->SetAllVisible(AllLayersVisible);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL DocInfoComponent::StartExportBeforeComp(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96
	Returns:	TRUE if the component was able to prepare for exporting;
				FALSE if not (e.g. out of memory)
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
	Purpose:	Inform the information document component that a Web or Native export is
				about to start.
				This version of StartExport is called before compression is started up.
				This means we can save the comment record in the uncompressed section of the
				file so that it is easy for the indexer to find.
	SeeAlso:	DocInfoComponent::StartExport;

********************************************************************************************/

BOOL DocInfoComponent::StartExportBeforeComp(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	if (pFilter == NULL)
	{
		ERROR3("DocInfoComponent::StartExportBeforeComp filter is null!");
		return TRUE;
	}

	// The document comment should be amongst the first bits of data to be exported
	// and must be in the uncompressed section as the gallery searches for it. 
	Document * pDoc = pFilter->GetDocument();
	if (pDoc)
	{
		BOOL ok = ExportDocumentComment(pFilter, pDoc);
	}

#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL DocInfoComponent::StartExport(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Returns:	TRUE if the component was able to prepare for exporting;
				FALSE if not (e.g. out of memory)
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
	Purpose:	Inform the information document component that a Web or Native export is
				about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocInfoComponent::StartExport(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL, TRUE, "DocInfoComponent::StartExport filter is null!");

	BOOL ok = TRUE;

	// The export hint gets written out here (if the document has one)
	Document * pDoc = pFilter->GetDocument();
	if (pDoc)
	{
		ExportHint* pHint = pDoc->GetExportHint();
		
		// If there is a hint then write it out...
		if (pHint)
			ok = pHint->WriteExportHintRecord(pFilter);
	}

	return(ok);
#else
	return(TRUE);
#endif
}


/********************************************************************************************

>	BOOL DocInfoComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - True if everything went swimmingly, False if just a clean up is required.
	Purpose:	Inform the information document component that a Web or Native export has
				just finished.
	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocInfoComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)
{
#ifdef DO_EXPORT

	// Export any remaining document related items.
	BOOL ok = TRUE;
	Document * pDoc = pFilter->GetDocument();
	if (pDoc && Success)
	{
		if (ok) ok = ExportDocumentDates(pFilter, pDoc);
		if (ok) ok = ExportUndoBufferSize(pFilter, pDoc);
		if (ok) ok = ExportDocumentFlags(pFilter, pDoc);
	}

	return ok;
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	BOOL DocInfoComponent::ExportDocumentComment(BaseCamelotFilter *pFilter, Document * pDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				pDoc	- The document we are currently exporting
	Purpose:	Save out a document comment record to the file.
	Returns:	TRUE if the comment was saved correctly
				FALSE if not (e.g. out of memory)
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocInfoComponent::ExportDocumentComment(BaseCamelotFilter *pFilter, Document * pDoc)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL || pDoc == NULL,FALSE,"DocInfoComponent::ExportDocumentComment Null pFilter or pDoc");

	// Only export information in native files
	if (pFilter->IsWebFilter())
		return TRUE;

	// Write the data out to the file
	BOOL ok = TRUE;
	String_256 Comment = pDoc->GetComment();
	// Only save the record if there is any data to save
	if (Comment.Length() == 0)
		return TRUE;
	
	// Write the data out to the file
	UINT32 Size = 0;
	// If there is a string name, then add it to this size
	// REMEMBER: We save out unicode strings and so we need to double the length of the returned string length
	// but only if we are not being compiled as Unicode
#ifdef UNICODE
	Size += Comment.Length() + 1;
#else
	Size += (Comment.Length() + 1) * 2;
#endif	
	CXaraFileRecord Rec(TAG_DOCUMENTCOMMENT, Size);
	ok = Rec.Init();

	if (ok) ok = Rec.WriteUnicode(Comment);

	// Finally, write the record out to file
	// In the process get the record number that this was written out as
	INT32 RecordNumber = 0L;
	if (ok) RecordNumber = pFilter->Write(&Rec);	// Get the document comment
	
	// If we have had a problem at any of the stages then return that to the caller
	if (!ok || RecordNumber <= 0)
		return 0L;
	
	return ok;

#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL DocInfoComponent::ExportDocumentDates(BaseCamelotFilter *pFilter, Document * pDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				pDoc	- The document we are currently exporting
	Purpose:	Save out a document information record to the file.
	Returns:	TRUE if the information record was saved correctly
				FALSE if not (e.g. out of memory)
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocInfoComponent::ExportDocumentDates(BaseCamelotFilter *pFilter, Document * pDoc)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL || pDoc == NULL,FALSE,"DocInfoComponent::ExportDocumentDates Null pFilter or pDoc");

	// Only export information in native files
	if (pFilter->IsWebFilter())
		return TRUE;

	// Write the data out to the file
	BOOL ok = TRUE;

	// Set the last saved date to now
	if (pDoc)
		pDoc->SetLastSaveTime();

	// Find out what they are
	time_t Creation =  pDoc->GetCreationTime();
	time_t LastSaved = pDoc->GetLastSaveTime();

	// Write the data out to the file
	CXaraFileRecord Rec(TAG_DOCUMENTDATES, TAG_DOCUMENTDATES_SIZE);
	ok = Rec.Init();

	if (ok) ok = Rec.WriteINT32(Creation);
	if (ok) ok = Rec.WriteINT32(LastSaved);

	// Finally, write the record out to file
	// In the process get the record number that this was written out as
	INT32 RecordNumber = 0L;
	if (ok) RecordNumber = pFilter->Write(&Rec);	// Get the document comment
	
	// If we have had a problem at any of the stages then return that to the caller
	if (!ok || RecordNumber <= 0)
		return 0L;
	
	return ok;

#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL DocInfoComponent::ExportUndoBufferSize(BaseCamelotFilter *pFilter, Document * pDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				pDoc	- The document we are currently exporting
	Purpose:	Save out a document undo buffer size record to the file.
	Returns:	TRUE if the undo buffer size record was saved correctly
				FALSE if not (e.g. out of memory)
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocInfoComponent::ExportUndoBufferSize(BaseCamelotFilter *pFilter, Document * pDoc)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL || pDoc == NULL,FALSE,"DocInfoComponent::ExportUndoBufferSize Null pFilter or pDoc");

	// Only export information in native files
	if (pFilter->IsWebFilter())
		return TRUE;

	// Write the data out to the file
	BOOL ok = TRUE;
	
	// Find the size of the operation history
	UINT32 UndoBufSize = pDoc->GetOpHistory().GetMaxSize();

	CXaraFileRecord Rec(TAG_DOCUMENTUNDOSIZE, TAG_DOCUMENTUNDOSIZE_SIZE);
	ok = Rec.Init();

	if (ok) ok = Rec.WriteUINT32(UndoBufSize);

	// Finally, write the record out to file
	// In the process get the record number that this was written out as
	INT32 RecordNumber = 0L;
	if (ok) RecordNumber = pFilter->Write(&Rec);	// Get the document comment
	
	// If we have had a problem at any of the stages then return that to the caller
	if (!ok || RecordNumber <= 0)
		return 0L;
	
	return ok;

#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL DocInfoComponent::ExportDocumentFlags(BaseCamelotFilter *pFilter, Document * pDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				pDoc	- The document we are currently exporting
	Purpose:	Save out a document flags record to the file.
	Returns:	TRUE if the document flags record was saved correctly
				FALSE if not (e.g. out of memory)
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocInfoComponent::ExportDocumentFlags(BaseCamelotFilter *pFilter, Document * pDoc)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL || pDoc == NULL,FALSE,"DocInfoComponent::ExportDocumentFlags Null pFilter or pDoc");

	// Only export information in native files
	if (pFilter->IsWebFilter())
		return TRUE;

	// Write the data out to the file
	BOOL ok = TRUE;
	
	// Find the flags we are interested in
	BOOL Multilayer			= pDoc->IsMultilayer();
	BOOL AllLayersVisible	= pDoc->IsAllVisible();

	UINT32 DocFlags = (Multilayer ? 2 : 0) | (AllLayersVisible ? 1 : 0);

	CXaraFileRecord Rec(TAG_DOCUMENTFLAGS, TAG_DOCUMENTFLAGS_SIZE);
	ok = Rec.Init();

	if (ok) ok = Rec.WriteUINT32(DocFlags);

	// Finally, write the record out to file
	// In the process get the record number that this was written out as
	INT32 RecordNumber = 0L;
	if (ok) RecordNumber = pFilter->Write(&Rec);	// Get the document comment
	
	// If we have had a problem at any of the stages then return that to the caller
	if (!ok || RecordNumber <= 0)
		return 0L;
	
	return ok;

#endif
	return TRUE;
}
