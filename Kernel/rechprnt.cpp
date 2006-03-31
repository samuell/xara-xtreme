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
// The printer/imagesetting settings record handler for native and web files.

#include "camtypes.h"

#include "cxfrec.h"
#include "cxfrech.h"
#include "cxftags.h"
//#include "prnmkcom.h"
//#include "princomp.h"
#include "rechprnt.h"

DECLARE_SOURCE("$Revision$");


CC_IMPLEMENT_DYNAMIC(PrintingRecordHandler, CamelotRecordHandler);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	virtual BOOL PrintingRecordHandler::BeginImport()

 	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96

	Returns:	TRUE if ok
				FALSE otherwise

	Purpose:	Initialises the record handler ready for importing

********************************************************************************************/

BOOL PrintingRecordHandler::BeginImport()
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	pPrintComponent = GetPrintComponent();

	// Let's not faff around for the print marks component - just find it for ourselves
	Document *ScopeDoc = GetDocument();
	if (ScopeDoc != NULL)
	{
		pPrintMarksComponent = (PrintMarksComponent *)
							ScopeDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintMarksComponent));
	}

	return (pPrintComponent != NULL && pPrintMarksComponent != NULL);
#else
	return TRUE;
#endif
}


/********************************************************************************************

>	virtual UINT32* PrintingRecordHandler::GetTagList()

 	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96

	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END

	Purpose:	Provides the record handler system with a list of records handled by this
				handler

********************************************************************************************/

UINT32* PrintingRecordHandler::GetTagList()
{
	static UINT32 TagList[] =
	{
		TAG_PRINTERSETTINGS,
		TAG_IMAGESETTING,
		TAG_COLOURPLATE,
		TAG_PRINTMARKDEFAULT,
		TAG_PRINTMARKCUSTOM,
		CXFRH_TAG_LIST_END
	};

	return(TagList);
}



/********************************************************************************************

>	virtual BOOL PrintingRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96

	Inputs:		pCXaraFileRecord = ptr to record to handle

	Returns:	TRUE if handled successfuly
				FALSE otherwise

	Purpose:	Handles the given record.

********************************************************************************************/

BOOL PrintingRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL, FALSE, "PrintingRecordHandler::HandleRecord pCXaraFileRecord is NULL");

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// When we're "opening" a file, we load, but if we're "importing" then we ignore all
	// printing options from the imported file (we don't want to override the existing
	// settings when you import into a different document)
	if (IsImporting())
		return(TRUE);

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_PRINTERSETTINGS:
			if (pPrintComponent)
				pPrintComponent->ImportPrintSettings(pCXaraFileRecord);
			break;

		case TAG_IMAGESETTING:
			if (pPrintComponent)
				pPrintComponent->ImportImagesetting(pCXaraFileRecord);
			break;

		case TAG_COLOURPLATE:
			if (pPrintComponent)
				pPrintComponent->ImportColourPlate(pCXaraFileRecord, this);
			break;

		case TAG_PRINTMARKDEFAULT:
			if (pPrintMarksComponent)
				pPrintMarksComponent->ImportDefaultPrintMark(pCXaraFileRecord);
			break;

		case TAG_PRINTMARKCUSTOM:
			if (pPrintMarksComponent)
				pPrintMarksComponent->StartImportCustomPrintMark(this, pCXaraFileRecord);
			break;

		default:
			ERROR3_PF(("PrintingRecordHandler::HandleRecord doesn't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			return(FALSE);
	}
#endif
	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL PrintingRecordHandler::BeginSubtree(UINT32 Tag)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/96

	Inputs:		Tag = tag value of the tag this handler last handled
	Returns:	TRUE if this func wants exclusive handling of the tag
				FALSE otherwise
	Purpose:	Informs the record handler that a subtree is following a tag of type 'Tag'

				If you override this func and you do not wish other parts of the system to be informed of the
				subtree start, you should return TRUE

********************************************************************************************/

BOOL PrintingRecordHandler::BeginSubtree(UINT32 Tag)
{
	// We only want to know about following subtrees when doing custom print mark records.
	// If we are doing one, then we grab the subtree so nobody else can faff about with it
	if (Tag != TAG_PRINTMARKCUSTOM)
		return(FALSE);

	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL PrintingRecordHandler::EndSubtree(UINT32 Tag)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/96

	Inputs:		Tag = tag value of the tag this handler last handled
	Returns:	TRUE if this func wants exclusive handling of the tag
				FALSE otherwise

	Purpose:	Informs the record handler that a subtree that followed a tag of type 'Tag' has ended

				If you override this func and you do not wish other parts of the system to be informed of the
				subtree end, you should return TRUE

********************************************************************************************/

BOOL PrintingRecordHandler::EndSubtree(UINT32 Tag)
{
	// We only want to know about following subtrees when doing custom print mark records.
	// If we are doing one, then we grab the subtree so nobody else can faff about with it
	if (Tag != TAG_PRINTMARKCUSTOM)
		return(FALSE);

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// OK, it is the end of a custom print mark record, so complete the import
	if (pPrintMarksComponent && !IsImporting())
		pPrintMarksComponent->EndImportCustomPrintMark(this);
#endif
	
	return(TRUE);
}



/********************************************************************************************

>	virtual void PrintingRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96

	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the define colour records.

	Notes:		This funciton is only present in _DEBUG builds

********************************************************************************************/

#if XAR_TREE_DIALOG
void PrintingRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord, StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	// Call base class first to output the tag and size
	CamelotRecordHandler::GetRecordDescriptionText(pRecord, pStr);

	UINT32 Tag = pRecord->GetTag();
	INT32 RecordNumber = pRecord->GetRecordNumber();

	switch (Tag)
	{
		case TAG_PRINTERSETTINGS:
			break;

		case TAG_IMAGESETTING:
			break;

		case TAG_COLOURPLATE:
			break;

		case TAG_PRINTMARKDEFAULT:
			{
				BYTE ID;
				pRecord->ReadBYTE(&ID);
				String_16 Temp;
				Temp._MakeMsg("Printers mark, ID = #1%ld", (INT32)ID);
				*pStr += Temp;
			}
			break;

		case TAG_PRINTMARKCUSTOM:
			*pStr += "Custom printers mark subtree follows...";
			break;

		default:
			ERROR3("Unknown tag passed to PrintingRecordHandler");
			break;
	}
}

#endif		// _DEBUG

