// $Id: rechcomp.cpp 662 2006-03-14 21:31:49Z alex $
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

#include "rechcomp.h"	// CompressionRecordHandler for handling compression for v2 native/web files

#include "colcomp.h"	// Document colour component, handles import of colours
#include "camfiltr.h"	// BaseCamelotFilter - version 2 native filter

#include "cxftags.h"	// TAG_DEFINERGBCOLOUR TAG_DEFINECOMPLEXCOLOUR
#include "cxfrec.h"		// TAG_STARTCOMPRESSION TAG_STOPCOMPRESSION

DECLARE_SOURCE("$Revision: 662 $");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNAMIC(CompressionRecordHandler,CamelotRecordHandler);

//#if NEW_NATIVE_FILTER	// New native filters, only available to those who need them at present

/********************************************************************************************

>	virtual UINT32* CompressionRecordHandler::GetTagList()

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* CompressionRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_STARTCOMPRESSION, TAG_ENDCOMPRESSION, CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL CompressionRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL CompressionRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"CompressionRecordHandler::HandleRecord pCXaraFileRecord is NULL");
	
	// This should be unused and we should error if this is called
	ERROR2(FALSE,"CompressionRecordHandler::HandleRecord called! Use Streamed form instead");
	
	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_STARTCOMPRESSION:
			// Ask the compression system to be turned on
			TRACEUSER( "Neville", _T("Turn compression on\n"));
			ok = SetCompression(TRUE);
			break;
		case TAG_ENDCOMPRESSION:
			// Ask the compression system to be turned off
			TRACEUSER( "Neville", _T("Turn compression on\n"));
			ok = SetCompression(FALSE);
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("CompressionRecordHandler::HandleRecord I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}

/********************************************************************************************

>	virtual BOOL CompressionRecordHandler::HandleStreamedRecord(CXaraFile * pCXFile, UINT32 Tag,UINT32 Size,UINT32 RecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96
	Inputs:		pCXFile = access to the CXaraFile class 
				Tag	 = the tag value
				Size = size of record
				RecordNumber = the record number in the file
	Returns:	TRUE if handled ok
				FALSE otherwise
	Purpose:	This is the streamed record handler. It is used when 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CompressionRecordHandler::HandleStreamedRecord(CXaraFile * pCXFile, UINT32 Tag,UINT32 Size,UINT32 RecordNumber)
{
	ERROR2IF(Tag <= 0,FALSE,"CompressionRecordHandler::HandleStreamedRecord Tag is negative!");
	
	BOOL ok = TRUE;

	switch (Tag)
	{
		case TAG_STARTCOMPRESSION:
			// Ask the compression system to be turned on
			TRACEUSER( "Neville", _T("Turn compression on\n"));
			ok = SetCompression(TRUE);
			break;
		case TAG_ENDCOMPRESSION:
			// Ask the compression system to be turned off
			TRACEUSER( "Neville", _T("Turn compression off\n"));
			ok = SetCompression(FALSE);
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("CompressionRecordHandler::HandleStreamedRecord I don't handle records with the tag (%d)\n",Tag));
			break;
	}

	return ok;
}


/********************************************************************************************

>	virtual BOOL CompressionRecordHandler::IsStreamed(UINT32 Tag)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96
	Inputs:		The tag of the record
	Returns:	TRUE if this is a streamed record
				FALSE otherwise
	Purpose:	Function to find out if the record is streamed or not.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CompressionRecordHandler::IsStreamed(UINT32 Tag)
{
	// We don't want these records read in or handled by any outsiders as the ZLib code
	// reads things like the CRC and uncompressed size itself when compression is stopped
	return TRUE;
}


/********************************************************************************************

>	virtual void CompressionRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the compression on and off records.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if XAR_TREE_DIALOG
void CompressionRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	// Call base class first
	// This outputs the tag and size
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	UINT32 Tag = pRecord->GetTag();
	char s[256];
	switch (Tag)
	{
		case TAG_STARTCOMPRESSION:
		{
			// Show what a compression on record looks like
			// Read the version number
			UINT32 Version = 0L;
			pRecord->ReadUINT32(&Version);
			
			UINT32 Type = 0L;
			UINT32 MajorVersion = Version / 100;
			UINT32 MinorVersion = Version - MajorVersion * 100;
			_stprintf(s,"Compression type\t= %d\r\n",Type);
			(*pStr) += s;
			_stprintf(s,"Compression version\t= %d.%d\r\n",MajorVersion,MinorVersion);
			(*pStr) += s;
			break;
		}
		case TAG_ENDCOMPRESSION:
		{
			// Show what a compression off record looks like
			// Read the number of bytes read 
			UINT32 Checksum = 0L;
			pRecord->ReadUINT32(&Checksum);
			_stprintf(s,"Compression checksum\t= %d\r\n",Checksum);
			(*pStr) += s;
			UINT32 BytesRead = 0L;
			pRecord->ReadUINT32(&BytesRead);
			_stprintf(s,"Compression bytes read\t= %d\r\n",BytesRead);
			(*pStr) += s;
			break;
		}
	}

	return;
}
#endif

//#endif // NEW_NATIVE_FILTER

