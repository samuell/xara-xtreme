// $Id: rechattr.cpp 662 2006-03-14 21:31:49Z alex $
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
    
#include "camtypes.h"                          
#include "rechattr.h"	// Record handling classes for attributes
#include "qualattr.h"
#include "webattr.h"
#include "userattr.h"


//#include "hotlink.h"		//For USERATTRKEY_WEBADDRESS

#include "cxftags.h"
#include "cxfrec.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC(GeneralAttrRecordHandler, CamelotRecordHandler)

// Declare smart memory handling in Debug builds
// Give this file in memory dumps
#define new CAM_DEBUG_NEW
	

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/********************************************************************************************

>	virtual UINT32* GeneralAttrRecordHandler::GetTagList()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/96
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* GeneralAttrRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_QUALITY,
								TAG_USERVALUE,
								TAG_WEBADDRESS,
								TAG_WEBADDRESS_BOUNDINGBOX,
								CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL GeneralAttrRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralAttrRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_QUALITY:
			ok = HandleQualityRecord(pCXaraFileRecord);
			break;

		case TAG_USERVALUE:
			ok = HandleUserValueRecord(pCXaraFileRecord);
			break;

		case TAG_WEBADDRESS:
			ok = HandleWebAddressRecord(pCXaraFileRecord);
			break;

		case TAG_WEBADDRESS_BOUNDINGBOX:
			ok = HandleWebAddressBoundingBoxRecord(pCXaraFileRecord);
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}


/********************************************************************************************

>	BOOL GeneralAttrRecordHandler::HandleQualityRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a quality record (and I don't mean a record of high grade)
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralAttrRecordHandler::HandleQualityRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_QUALITY,FALSE,"I don't handle this tag type");

	INT32 Quality = 0;
	BOOL ok = pCXaraFileRecord->ReadINT32(&Quality);

	if (ok)
	{
		AttrQuality* pQual = new AttrQuality;
		if (pQual != NULL)
		{
			pQual->Value.QualityValue.SetQuality(Quality);
			ok = InsertNode(pQual);
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL GeneralAttrRecordHandler::HandleUserValueRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/96 - amended by Graham 23/3/97 to use the new Web Address record
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a UserAttribute (ValueAttribute) record
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralAttrRecordHandler::HandleUserValueRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_USERVALUE,FALSE,"I don't handle this tag type");

	String_256 Key;
	String_256 Value;
	BOOL ok = TRUE;
	if (ok) ok = pCXaraFileRecord->ReadUnicode((LPTSTR)Key, 256);
	if (ok) ok = pCXaraFileRecord->ReadUnicode((LPTSTR)Value, 256);

	if (ok)
	{
		//We need to test the Key, to see if this is an old-style Web Address
		String_256 strKey=Key;
		String_256 strWebAddressKey(_R(IDS_USERATTRKEY_WEBADDRESS));

		//If this is an old-style Web Address
		if (strKey==strWebAddressKey)
		{
			//Then put it in the node tree
		 	AttrWebAddress* pAttrWebAddress = new AttrWebAddress((TCHAR*)Value);
		
			if (pAttrWebAddress)
				ok = InsertNode(pAttrWebAddress);
			else
				ok = FALSE;
		}
		else
		{
			// We wrote out the INT32 key, so Key is actually that
		 	AttrUser* const pNewAttr = new AttrUser(Key, Value);
		
			if (pNewAttr != NULL)
			{
				ok = InsertNode(pNewAttr);
			}
			else
			{
				ok = FALSE;
			}
		}

	}

	return ok;
}

/********************************************************************************************

>	BOOL GeneralAttrRecordHandler::HandleWebAddressRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/97
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a WebAddress record
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralAttrRecordHandler::HandleWebAddressRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_WEBADDRESS,FALSE,"I don't handle this tag type");

	//First let's get the size of the record
	UINT32 ulSize=pCXaraFileRecord->GetSize();

	//And allocate two buffers of that size to put the URL and Frame strings in
	TCHAR* pcURL=new TCHAR[ulSize];
	TCHAR* pcFrame=new TCHAR[ulSize];
	BOOL ok = TRUE;

	//Read in the strings
	if (ok) ok = pCXaraFileRecord->ReadUnicode(pcURL, ulSize);
	if (ok) ok = pCXaraFileRecord->ReadUnicode(pcFrame, ulSize);

	//And create a new Web Address attribute out of them
	if (ok)
	{
		AttrWebAddress* pAttrWebAddress = new AttrWebAddress(pcURL, FALSE, pcFrame);
		
		if (pAttrWebAddress)
			ok = InsertNode(pAttrWebAddress);
		else
			ok = FALSE;
	}

	delete pcURL;
	delete pcFrame;

	return ok;
}

/********************************************************************************************

>	BOOL GeneralAttrRecordHandler::HandleWebAddressBoundingBoxRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/97
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a WebAddressBoundingBox record
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralAttrRecordHandler::HandleWebAddressBoundingBoxRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_WEBADDRESS_BOUNDINGBOX,FALSE,"I don't handle this tag type");

	//There will be two coordinates at the start of the file, which will describe the 
	//bounding box of the clickable area of this attribute

	//We don't need these: the bounding box of the clickable area of this attribute
	//is the bounding box of the this attribute's parent node. So we throw them away.
	DocCoord dcThrowAway;
	pCXaraFileRecord->ReadCoordInterleaved(&dcThrowAway);
	pCXaraFileRecord->ReadCoordInterleaved(&dcThrowAway);
		
	//Now get the size of the record
	UINT32 ulSize=pCXaraFileRecord->GetSize();

	//And allocate two buffers of that size to put the URL and Frame strings in
	TCHAR* pcURL=new TCHAR[ulSize];
	TCHAR* pcFrame=new TCHAR[ulSize];
	BOOL ok = TRUE;

	//Read in the strings
	if (ok) ok = pCXaraFileRecord->ReadUnicode(pcURL, ulSize);
	if (ok) ok = pCXaraFileRecord->ReadUnicode(pcFrame, ulSize);

	//And create a new Web Address attribute out of them
	if (ok)
	{
		AttrWebAddress* pAttrWebAddress = new AttrWebAddress(pcURL, TRUE, pcFrame);
		
		if (pAttrWebAddress)
			ok = InsertNode(pAttrWebAddress);
		else
			ok = FALSE;
	}

	delete pcURL;
	delete pcFrame;

	return ok;
}



/********************************************************************************************

>	virtual void GeneralAttrRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the misc attr records.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#ifdef XAR_TREE_DIALOG
void GeneralAttrRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	char s[256];

	//	 Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	switch (pRecord->GetTag())
	{
		case TAG_QUALITY:
		{
			INT32 Quality = 0;
			pRecord->ReadINT32(&Quality);

			_stprintf(s,"Quality = %d\r\n",Quality);
			(*pStr) += s;
		}
		break;

		case TAG_USERVALUE:
		{
			TCHAR Key[256];
			TCHAR Value[256];
			BOOL ok = TRUE;
			pRecord->ReadUnicode(Key, 256);
			pRecord->ReadUnicode(Value, 256);

			_stprintf(s,"Key   = %s\r\n",Key);
			(*pStr) += s;
			_stprintf(s,"Value = %s\r\n",Value);
			(*pStr) += s;
		}
		break;

		case TAG_WEBADDRESS:
		{
			TCHAR URL[256];
			TCHAR Frame[256];
			BOOL ok = TRUE;
			pRecord->ReadUnicode(URL, 256);
			pRecord->ReadUnicode(Frame, 256);

			_stprintf(s,"URL   = %s\r\n",URL);
			(*pStr) += s;
			_stprintf(s,"Frame = %s\r\n",Frame);
			(*pStr) += s;
		}
		break;

		case TAG_WEBADDRESS_BOUNDINGBOX:
		{
			TCHAR URL[256];
			TCHAR Frame[256];
			BOOL ok = TRUE;
			DocCoord dcThrowAway;
			pRecord->ReadCoordInterleaved(&dcThrowAway);
			pRecord->ReadCoordInterleaved(&dcThrowAway);

			pRecord->ReadUnicode(URL, 256);
			pRecord->ReadUnicode(Frame, 256);

			_stprintf(s,"URL   = %s\r\n",URL);
			(*pStr) += s;
			_stprintf(s,"Frame = %s\r\n",Frame);
			(*pStr) += s;
		}
		break;
	}
}
#endif
