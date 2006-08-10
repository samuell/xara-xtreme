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

// Handles the reading of bitmap properties records

#include "camtypes.h"

#include "rechbmpp.h"	// BitmapPropertiesRecordHandler for importing bitmaps from v2 native/web files
#include "bmpcomp.h"	// Document bitmap component, handles import of bitmaps
//#include "camfiltr.h"	// BaseCamelotFilter - version 2 native filter - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxfile.h"		// CXaraFile file handling class for version 2 native filter 
#include "cxftags.h"	// TAG_DEFINEBITMAP_**** 
//#include "sgliboil.h"	// Wav stuff
//#include "cxfrec.h"		// The main record handling class - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "bitmap.h"		// KernelBitmap - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "xmlutils.h"	// CXMLUtils (and msxml2.h)
//#include "xpehost.h"	// XPEHost

DECLARE_SOURCE("$Revision$");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNAMIC(BitmapPropertiesRecordHandler,CamelotRecordHandler);


/********************************************************************************************

>	virtual UINT32* BitmapPropertiesRecordHandler::GetTagList()

 	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/00
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* BitmapPropertiesRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_BITMAP_PROPERTIES, TAG_XPE_BITMAP_PROPERTIES, CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}


/********************************************************************************************

>	virtual BOOL BitmapPropertiesRecordHandler::IsStreamed(UINT32 Tag)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/01/2004
	Inputs:		The tag of the record
	Returns:	TRUE if this is a streamed record
				FALSE otherwise
	Purpose:	Function to find out if the record is streamed or not.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BitmapPropertiesRecordHandler::IsStreamed(UINT32 Tag)
{
	// We handle both types so check what we need to do by the tag we have been given
	BOOL Streamed = FALSE;
	switch (Tag)
	{
		case TAG_BITMAP_PROPERTIES:
			Streamed = FALSE;
			break;
		case TAG_XPE_BITMAP_PROPERTIES:
			Streamed = FALSE;
			break;
		default:
			Streamed = FALSE;
			ERROR3_PF(("BitmapPropertiesRecordHandler::IsStreamed I don't handle records with the tag (%d)\n", Tag));
			break;
	}

	return Streamed;
}

/********************************************************************************************

>	virtual BOOL BitmapPropertiesRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/00
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL BitmapPropertiesRecordHandler::HandleRecord( CXaraFileRecord* pCXaraFileRecord )
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"BitmapPropertiesRecordHandler::HandleRecord pCXaraFileRecord is NULL");

	BOOL ok = TRUE;
	UINT32 tag = pCXaraFileRecord->GetTag();

	// read in the record ---------------------------------------------------------

	// read bitmap reference
	INT32 BmpRef = 0;
	INT32 MasterRef = 0;
	String_256 BitmapName;
	ok = pCXaraFileRecord->ReadINT32(&BmpRef);

	// read flags
	BYTE Flags;
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Flags);

	// read other stuff
	BYTE Temp;
	for( INT32 i=0; i<7; i++ )
	{
		if (ok) ok = pCXaraFileRecord->ReadBYTE(&Temp);
	}

	// Read XPE master ref and XML edits list
//	_bstr_t bstrXML;
	StringVar strXML;
	if (ok && tag==TAG_XPE_BITMAP_PROPERTIES)
	{
		ok = pCXaraFileRecord->ReadINT32(&MasterRef);

		if (ok) ok = pCXaraFileRecord->ReadUnicode(&BitmapName);

//		if (ok) ok = pCXaraFileRecord->ReadBSTR(&bstrXML, pCXaraFileRecord->GetSize());
		if (ok) ok = pCXaraFileRecord->ReadUTF16STR(&strXML, pCXaraFileRecord->GetSize());
	}

	// process the record ---------------------------------------------------------

	// look up the bitmap from BmpRef

	if (BmpRef == 0)
	{
		// no such bitmap
		ok = FALSE;
	}
	else
	{
		KernelBitmap *pBitmap = NULL;
		pBitmap = GetReadBitmapReference(BmpRef);

		if (pBitmap == NULL)
		{
			// no such bitmap
			ok = FALSE;
		}
		else
		{
			// process the flags
			BOOL bSmoothWhenScaledUp = Flags & 0x01;
			pBitmap->SetInterpolation(bSmoothWhenScaledUp);

			// Setup XPE info
			// (If we don't have it, it's not important - this bitmap will just lose
			//	it's XPE edit info and will become its own master)
PORTNOTETRACE("other","BitmapPropertiesRecordHandler::HandleRecord - remove XML code");
#ifndef EXCLUDE_FROM_XARALX
			if (tag==TAG_XPE_BITMAP_PROPERTIES && MasterRef!=0)
			{
				KernelBitmap* pMasterBitmap = NULL;
				pMasterBitmap = GetReadBitmapReference(MasterRef);

				VARIANT_BOOL varResult;
				IXMLDOMDocumentPtr pxmlDoc = CXMLUtils::NewDocument();
				HRESULT hr = pxmlDoc->loadXML(bstrXML, &varResult);
				if (pMasterBitmap && SUCCEEDED(hr) && VARIANT_TRUE == varResult)
				{
					// We now know that this master bitmap will be used
					// so add it to the document's bitmap list...
					// Get the bitmap list from the current document
					Document* pDoc = Document::GetCurrent();
					BitmapList* pBmpList = pDoc->GetBitmapList();
					if (!pBmpList->FindDuplicateBitmap(pMasterBitmap->ActualBitmap))
						// and then attach the bitmap
						pMasterBitmap->Attach(pBmpList);

					// See whether we need to regenerate this bitmap from the
					// Master and the Edits...
					if (pBitmap->IsDefaultBitmap())
					{
						XPEEditOp::DoProcessBitmap(pBitmap, pMasterBitmap, pxmlDoc);
					}

					pBitmap->SetXPEInfo(pMasterBitmap, pxmlDoc);	// Can only do this after DoProcessBitmap...
				}

				pBitmap->SetName(BitmapName);						// ...and this, because process may create new OILBitmap
			}
#endif
		}
	}

	return ok;
}




/********************************************************************************************

>	virtual BOOL BitmapPropertiesRecordHandler::HandleStreamedRecord(CXaraFile * pCXFile, UINT32 Tag,UINT32 Size,UINT32 RecordNumber)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/01/2004
	Inputs:		pCXFile = access to the CXaraFile class
				Tag	 = the tag value
				Size = size of record
				RecordNumber = the record number in the file
	Returns:	TRUE if handled ok
				FALSE otherwise
	Purpose:	This is the bitmap streamed record handler. It handles the loading of bitmap
				definitions.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BitmapPropertiesRecordHandler::HandleStreamedRecord(CXaraFile * pCXFile, UINT32 Tag,UINT32 Size,UINT32 RecordNumber)
{
	ERROR2IF(pCXFile == NULL,FALSE,"BitmapRecordHandler::HandleStreamedRecord pCXFile is NULL");

	if (Tag!=TAG_XPE_BITMAP_PROPERTIES)
	{
		ERROR3_PF(("BitmapPropertiesRecordHandler::HandleStreamedRecord I don't handle records with the tag (%d)\n", Tag));
		return FALSE;
	}
	ERROR3_PF(("Unimplemented!", Tag));

	return TRUE;
}




/********************************************************************************************

>	virtual void BitmapPropertiesRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/00
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the define bitmap properties records.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#ifdef XAR_TREE_DIALOG
void BitmapPropertiesRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord, StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	// Call base class first
	// This outputs the tag and size
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);
	return;
}
#endif // XAR_TREE_DIALOG

//#endif // NEW_NATIVE_FILTER

