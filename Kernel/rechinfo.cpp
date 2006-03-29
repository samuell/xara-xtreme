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

// Implementation of the colour record handler class of the v2 file format

#include "camtypes.h"

#include "rechinfo.h"	// DocInfoRecordHandler for importing doc info from v2 native/web files
#include "infocomp.h"	// Document info component, handles import of document information
#include "camfiltr.h"	// BaseCamelotFilter - version 2 native filter
#include "cxftags.h"	// TAG_DOCUMENTCOMMENT
#include "cxfrec.h"		// 

//#include "localenv.h"	// LocalEnvironment

DECLARE_SOURCE("$Revision: 662 $");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNAMIC(DocInfoRecordHandler,CamelotRecordHandler);

// #if NEW_NATIVE_FILTER	// New native filters, only available to those who need them at present

/********************************************************************************************

>	virtual BOOL DocInfoRecordHandler::BeginImport()

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		-
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Initialises the units record handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DocInfoRecordHandler::BeginImport()
{
	pInfoComponent = GetDocInfoComponent();

	return (pInfoComponent != NULL);
}

/********************************************************************************************

>	virtual UINT32* DocInfoRecordHandler::GetTagList()

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* DocInfoRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_DOCUMENTCOMMENT, TAG_DOCUMENTDATES,
							  TAG_DOCUMENTUNDOSIZE,
							  TAG_DOCUMENTFLAGS,
							  CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL DocInfoRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL DocInfoRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"DocInfoRecordHandler::HandleRecord pCXaraFileRecord is NULL");

	BOOL ok = TRUE;
	Document * pDoc = GetDocument();
	INT32 Tag = pCXaraFileRecord->GetTag();
	if (pInfoComponent == NULL)
	{
		ERROR3("DocInfoRecordHandler::HandleRecord no pInfoComponent");
		return FALSE;
	}

	// If we are just importing data from this document into an existing one,
	// then don't import any of this data.
	if (IsImporting())
		return TRUE;

	switch (Tag)
	{
		case TAG_DOCUMENTCOMMENT:
			// Ask the document information component class to import that data for us
			pInfoComponent->ImportDocumentComment(pCXaraFileRecord, pDoc);
			break;

		case TAG_DOCUMENTDATES:
			// Ask the document information component class to import that data for us
			pInfoComponent->ImportDocumentDates(pCXaraFileRecord, pDoc);
			break;

		case TAG_DOCUMENTUNDOSIZE:
			// Ask the document information component class to import that data for us
			pInfoComponent->ImportUndoBufferSize(pCXaraFileRecord, pDoc);
			break;

		case TAG_DOCUMENTFLAGS:
			// Ask the document information component class to import that data for us
			pInfoComponent->ImportDocumentFlags(pCXaraFileRecord, pDoc);
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("DocInfoRecordHandler::HandleRecord I don't handle records with the tag (%d)\n",Tag));
			break;
	}

	return ok;
}

/********************************************************************************************

>	virtual void DocInfoRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the define units records.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if XAR_TREE_DIALOG
void DocInfoRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord, StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	// Call base class first
	// This outputs the tag and size
	CamelotRecordHandler::GetRecordDescriptionText(pRecord, pStr);

	UINT32 Tag = pRecord->GetTag();
	BOOL ok = TRUE;
	//char s[256];
	INT32 RecordNumber = pRecord->GetRecordNumber();
	switch (Tag)
	{
		case TAG_DOCUMENTCOMMENT:
		{
			String_256 Comment;
			if (ok) ok = pRecord->ReadUnicode(&Comment);//Comment, Comment.MaxLength());
			if (Comment.Length() == 0)
				(*pStr) += "None\r\n\r\n";
			else
			{
				(*pStr) += "Comment:- \r\n";
				(*pStr) += Comment;
				(*pStr) += "\r\n\r\n";
			}

			break;
		}

		case TAG_DOCUMENTDATES:
		{
			// Read in the times/dates from the record
			time_t Creation;
			time_t LastSaved;

			if (ok) ok = pRecord->ReadINT32(&Creation);
			if (ok) ok = pRecord->ReadINT32(&LastSaved);
			
#if !defined(EXCLUDE_FROM_RALPH)
			String_256 String;
			(*pStr) += "Document created:- \t";
			LocalEnvironment::SystemTimeToString(&String, &Creation);
			(*pStr) += String;
			(*pStr) += "\t";
			LocalEnvironment::SystemDateToString(&String, &Creation);
			(*pStr) += String;
			(*pStr) += "\r\n\r\n";
			(*pStr) += "Document last saved:- \t";
			LocalEnvironment::SystemTimeToString(&String, &LastSaved);
			(*pStr) += String;
			(*pStr) += "\t";
			LocalEnvironment::SystemDateToString(&String, &LastSaved);
			(*pStr) += String;
			(*pStr) += "\r\n\r\n";
#endif
			break;
		}

		case TAG_DOCUMENTUNDOSIZE:
		{
			UINT32 UndoBufSize = 0;
			if (ok) ok = pRecord->ReadUINT32(&UndoBufSize);
			if (UndoBufSize == UINT32_MAX)
			{
				(*pStr) += "Undo buffer size: \t= unlimited\r\n";
			}
			else
			{
				(*pStr) += "Undo buffer size:- ";
				String_256 Value;
				Convert::BytesToString(&Value, UndoBufSize);
				(*pStr) += Value;
				(*pStr) += "\r\n\r\n";
			}
			break;
		}

		case TAG_DOCUMENTFLAGS:
		{
			UINT32 DocFlags = 0;
			if (ok) ok = pRecord->ReadUINT32(&DocFlags);
			(*pStr) += "Document flags which are currently set are:- \r\n";
			if (DocFlags & 2)
				(*pStr) += "Multilayer ";
			if (DocFlags & 1)
				(*pStr) += "AllLayersVisible ";
			(*pStr) += "\r\n\r\n";
			break;
		}
	} 

	return;
}

#endif // _DEBUG

// #endif // NEW_NATIVE_FILTER

