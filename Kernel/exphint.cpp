// $Id$
// $$
//
// Export hint class
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
#include "exphint.h"

#include "cxfrec.h"
#include "cxftags.h"
//#include "filtrres.h"

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_MEMDUMP(ExportHint, CC_CLASS_MEMDUMP);
CC_IMPLEMENT_DYNAMIC(ExportHintRecordHandler, CamelotRecordHandler);

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	ExportHint::ExportHint()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/97
	Purpose:	Default constructor
	SeeAlso:	-

********************************************************************************************/

ExportHint::ExportHint()
{
	m_Type = HINTTYPE_BAD;
	m_Width = 0;
	m_Height = 0;
	m_BPP = 0;

	// The contructor of the string should ensure this but...
	m_sOptions.Empty();
}


/********************************************************************************************

>	ExportHint::ExportHint(const ExportHint& other)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/97
	Purpose:	Copy constructor
	SeeAlso:	-

********************************************************************************************/

ExportHint::ExportHint(const ExportHint& other)
{
	// Use the assignment operator
	*(this) = other;
}


/********************************************************************************************

>	const ExportHint& ExportHint::operator=(const ExportHint& other)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/97
	Purpose:	Assignment operator
	SeeAlso:	-

********************************************************************************************/

const ExportHint& ExportHint::operator=(const ExportHint& other)
{
	m_Type = other.m_Type;
	m_Width = other.m_Width;
	m_Height = other.m_Height;
	m_BPP = other.m_BPP;
	m_sOptions = other.m_sOptions;

	return(*this);
}



/********************************************************************************************

>	BOOL ExportHint::InitFromRecord(CXaraFileRecord* pRecord)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/97
	Purpose:	Initialises the hint from a Flare record
	SeeAlso:	-

********************************************************************************************/

BOOL ExportHint::InitFromRecord(CXaraFileRecord* pRecord)
{
	String_256 sOptions;

	BOOL ok = pRecord->ReadUINT32(&m_Type);
	if (ok) ok = pRecord->ReadUINT32(&m_Width);
	if (ok) ok = pRecord->ReadUINT32(&m_Height);
	if (ok) ok = pRecord->ReadUINT32(&m_BPP);
	if (ok) ok = pRecord->ReadASCII(sOptions, 256);

	m_sOptions = sOptions;

	return(ok);
}



/********************************************************************************************

>	BOOL ExportHint::WriteExportHintRecord(BaseCamelotFilter* pFilter)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/97
	Purpose:	Writes the hint to a Flare record
	SeeAlso:	-

********************************************************************************************/

BOOL ExportHint::WriteExportHintRecord(BaseCamelotFilter* pFilter)
{
	// If the hint hasn't been set then don't save it
	if (m_Type == HINTTYPE_BAD)
		return(TRUE);
	
	// Add a description of the TAG_EXPORT_HINT record
	pFilter->AddTagDescription(TAG_EXPORT_HINT, _R(IDS_TAG_EXPORT_HINT));

	BOOL ok = TRUE;
	// Create a record (variable length)
	CXaraFileRecord Rec(TAG_EXPORT_HINT);
	ok = Rec.Init();

	if (ok) ok = Rec.WriteUINT32(m_Type);
	if (ok) ok = Rec.WriteUINT32(m_Width);
	if (ok) ok = Rec.WriteUINT32(m_Height);
	if (ok) ok = Rec.WriteUINT32(m_BPP);
	if (ok) ok = Rec.WriteASCII(m_sOptions);

	INT32 RecNum = 0;
	if (ok) RecNum = pFilter->Write(&Rec);	// Write the record
	if (RecNum == 0)
		ok = FALSE;

	return(ok);
}


/********************************************************************************************

>	virtual UINT32* ExportHintRecordHandler::GetTagList()

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/97
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler

********************************************************************************************/

UINT32* ExportHintRecordHandler::GetTagList()
{
	static const UINT32 TagList[] = {TAG_EXPORT_HINT, CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}


/********************************************************************************************

>	virtual BOOL ExportHintRecordHandler::HandleRecord(CXaraFileRecord* pRecord)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/97
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise

********************************************************************************************/

BOOL ExportHintRecordHandler::HandleRecord(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "pCXaraFileRecord is NULL");
	ERROR2IF(pRecord->GetTag() != TAG_EXPORT_HINT, FALSE, "I don't handle this tag type");

	BaseCamelotFilter* const pFilter = GetBaseCamelotFilter();
	ERROR2IF(pFilter == NULL, FALSE, "No filter in ExportHint::HandleRecord");

	Document* pDocument = pFilter->GetDocument();
	ERROR2IF(pDocument == NULL, FALSE, "No Document in ExportHint::HandleRecord");

	ExportHint* pHint = pDocument->GetExportHint();	
	ERROR2IF(pHint == NULL, FALSE, "No ExportHint in ExportHint::HandleRecord");

	return(pHint->InitFromRecord(pRecord));
}


#if XAR_TREE_DIALOG
/********************************************************************************************

>	virtual void ExportHintRecordHandler::GetRecordDescriptionText(
											CXaraFileRecord* pRecord, StringBase* pStr)
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/97
	Inputs:		pRecord - ptr to a record
				pStr - ptr to string to update
	Purpose:	Provides descriptve text for debug dialog

********************************************************************************************/

void ExportHintRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
	{
		TRACE( _T("ExportHintRecordHandler::GetRecordDescriptionText - NULL Args\n"));
		return;
	}
	if (pRecord->GetTag() != TAG_EXPORT_HINT)
	{
		TRACE( _T("ExportHintRecordHandler::GetRecordDescriptionText - Incorrect record\n"));
		return;
	}

	//	 Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord, pStr);

	UINT32 Type;
	UINT32 Width;
	UINT32 Height;
	UINT32 BPP;
	String_256 sOptions;
	String_16 sNumber;

	BOOL ok = pRecord->ReadUINT32(&Type);
	if (ok) ok = pRecord->ReadUINT32(&Width);
	if (ok) ok = pRecord->ReadUINT32(&Height);
	if (ok) ok = pRecord->ReadUINT32(&BPP);
	if (ok) ok = pRecord->ReadASCII(sOptions, 256);

	if (ok)
	{
		*pStr += TEXT("Type = ");
		ok = Convert::LongToString(Type, &sNumber);
		if (ok) *pStr += sNumber;
		if (ok) *pStr += _T("\r\n");
	}
	if (ok)
	{
		*pStr += TEXT("Width = ");
		ok = Convert::LongToString(Width, &sNumber);
		if (ok) *pStr += sNumber;
		if (ok) *pStr += _T("\r\n");
	}
	if (ok)
	{
		*pStr += TEXT("Height = ");
		ok = Convert::LongToString(Height, &sNumber);
		if (ok) *pStr += sNumber;
		if (ok) *pStr += _T("\r\n");
	}
	if (ok)
	{
		*pStr += TEXT("BPP = ");
		ok = Convert::LongToString(BPP, &sNumber);
		if (ok) *pStr += sNumber;
		if (ok) *pStr += _T("\r\n");
	}
	if (ok)
	{
		*pStr += TEXT("Options = ");
		if (ok) *pStr += sOptions;
		if (ok) *pStr += _T("\r\n");
	}
}


/********************************************************************************************

>	virtual void ExportHintRecordHandler::GetTagText(UINT32 Tag,String_256& Str)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/97
	Inputs:		Tag	- the tag value
	Outputs:	Str - place to put textual description of the tag
	Purpose:	Returns the tag description in the string

********************************************************************************************/

void ExportHintRecordHandler::GetTagText(UINT32 Tag, String_256& Str)
{
	Str = TEXT("TAG_EXPORT_HINT");
}
#endif	// _DEBUG
