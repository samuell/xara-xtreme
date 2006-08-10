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

#include "rechcol.h"	// ColourRecordHandler for importing colours from v2 native/web files
#include "colcomp.h"	// Document colour component, handles import of colours
//#include "camfiltr.h"	// BaseCamelotFilter - version 2 native filter - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"	// TAG_DEFINERGBCOLOUR TAG_DEFINECOMPLEXCOLOUR
//#include "cxfrec.h"	// TAG_DEFINERGBCOLOUR TAG_DEFINECOMPLEXCOLOUR - in camtypes.h [AUTOMATICALLY REMOVED]

DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNAMIC(ColourRecordHandler,CamelotRecordHandler);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// #if NEW_NATIVE_FILTER	// New native filters, only available to those who need them at present

/********************************************************************************************

>	virtual BOOL ColourRecordHandler::BeginImport()

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		-
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Initialises the colour record handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ColourRecordHandler::BeginImport()
{
	pColComponent = GetColourDocComponent();

	return (pColComponent != NULL);
}

/********************************************************************************************

>	virtual UINT32* ColourRecordHandler::GetTagList()

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* ColourRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_DEFINERGBCOLOUR,TAG_DEFINECOMPLEXCOLOUR,CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL ColourRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL ColourRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"ColourRecordHandler::HandleRecord pCXaraFileRecord is NULL");

	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_DEFINERGBCOLOUR:
			// Ask the colour document component class to import that colour for us
			if (pColComponent)
				ok = pColComponent->ImportSimpleColour(pCXaraFileRecord);
			else
				ERROR3("ColourRecordHandler::HandleRecord no pColComponent");
			break;
		case TAG_DEFINECOMPLEXCOLOUR:
			// Ask the colour document component class to import that colour for us
			if (pColComponent)
				ok = pColComponent->ImportColourDefinition(pCXaraFileRecord);
			else
				ERROR3("ColourRecordHandler::HandleRecord no pColComponent");
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("ColourRecordHandler::HandleRecord I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}

/********************************************************************************************

>	virtual void ColourRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the define colour records.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#ifdef XAR_TREE_DIALOG
void ColourRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord, StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	// Call base class first
	// This outputs the tag and size
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	UINT32 Tag = pRecord->GetTag();
	BOOL ok = TRUE;
	TCHAR s[256];
	INT32 RecordNumber = pRecord->GetRecordNumber();
	switch (Tag)
	{
		case TAG_DEFINERGBCOLOUR:
		{
			camSprintf(s,_T("Define simple colour reference %d\r\n\r\n"),RecordNumber);
			(*pStr) += s;
			// Show what the simple RGB colour is
			BYTE Red = 0;
			if (ok) ok = pRecord->ReadBYTE(&Red);
			camSprintf(s,_T("Red\t= %d\r\n"),Red);
			(*pStr) += s;
			BYTE Green = 0;
			if (ok) ok = pRecord->ReadBYTE(&Green);
			camSprintf(s,_T("Green\t= %d\r\n"),Green);
			(*pStr) += s;
			BYTE Blue = 0;
			if (ok) ok = pRecord->ReadBYTE(&Blue);
			camSprintf(s,_T("Blue\t= %d\r\n"),Blue);
			(*pStr) += s;
			break;
		}
		
		case TAG_DEFINECOMPLEXCOLOUR:
		{
			camSprintf(s,_T("Define complex colour reference %d\r\n"),RecordNumber);
			(*pStr) += s;
			// Show what the simple RGB colour is
			BYTE Red = 0;
			if (ok) ok = pRecord->ReadBYTE(&Red);
			camSprintf(s,_T("Red\t= %d\r\n"),Red);
			(*pStr) += s;
			BYTE Green = 0;
			if (ok) ok = pRecord->ReadBYTE(&Green);
			camSprintf(s,_T("Green\t= %d\r\n"),Green);
			(*pStr) += s;
			BYTE Blue = 0;
			if (ok) ok = pRecord->ReadBYTE(&Blue);
			camSprintf(s,_T("Blue\t= %d\r\n"),Blue);
			(*pStr) += s;

			// read in the colour model
			ColourModel ColModel = COLOURMODEL_RGBT;
			BYTE temp;
			if (ok) ok = pRecord->ReadBYTE(&temp);
			ColModel = (ColourModel)temp;

			// read in the colour type
			ExportColourType ColType = EXPORT_COLOURTYPE_NORMAL;
			if (ok) ok = pRecord->ReadBYTE(&temp);
			ColType = (ExportColourType)temp;

			// read in the entry number that this colour should be in the list of colours
			UINT32 EntryNumber = 0;
			if (ok) ok = pRecord->ReadUINT32(&EntryNumber);

			// read in the record number of the parent colour
			UINT32 ParentColour = 0;
			if (ok) ok = pRecord->ReadUINT32(&ParentColour);

			// Read in the four colour components
			UINT32 Comp1 = 0;
			if (ok) ok = pRecord->ReadUINT32(&Comp1);
			UINT32 Comp2 = 0;
			if (ok) ok = pRecord->ReadUINT32(&Comp2);
			UINT32 Comp3 = 0;
			if (ok) ok = pRecord->ReadUINT32(&Comp3);
			UINT32 Comp4 = 0;
			if (ok) ok = pRecord->ReadUINT32(&Comp4);

			String_64 ColName; 
			if (ok) ok = pRecord->ReadUnicode(&ColName);//ColName, ColName.MaxLength());
			if (ColName.Length() == 0)
				(*pStr) += _T("Unnamed\r\n\r\n");
			else
			{
				(*pStr) += _T("Colour name:- ");
				(*pStr) += ColName;
				(*pStr) += _T("\r\n\r\n");
			}
			if (ParentColour != 0)
			{
				camSprintf(s,_T("Parent colour reference = %d\r\n"),ParentColour);
				(*pStr) += s;
			}
			camSprintf(s,_T("Entry number in list = %d\r\n\r\n"),EntryNumber);
			(*pStr) += s;

			// Read the colour components according to the colour model.
			BOOL Linked = (ColType == EXPORT_COLOURTYPE_LINKED);
			switch (ColType)
			{
				case EXPORT_COLOURTYPE_NORMAL:
					(*pStr) += _T("Normal colour\r\n");
					DecodeColour(ColModel, Comp1, Comp2, Comp3, Comp4, Linked, pStr);
					break;
				case EXPORT_COLOURTYPE_SPOT:
					(*pStr) += _T("Spot colour\r\n");
					DecodeColour(ColModel, Comp1, Comp2, Comp3, Comp4, Linked, pStr);
					break;
				case EXPORT_COLOURTYPE_LINKED:
					(*pStr) += _T("Linked colour\r\n");
					DecodeColour(ColModel, Comp1, Comp2, Comp3, Comp4, Linked, pStr);
					break;

				case EXPORT_COLOURTYPE_TINT:
					{
						(*pStr) += _T("Tint colour\r\n");
						FIXED24 Value24 = 0.0;
						Value24.SetAsFixed24(Comp1);
						camSprintf(s,_T("\tTint\t= %f\r\n"),Value24.MakeDouble());
						(*pStr) += s;
					}
					break;

				case EXPORT_COLOURTYPE_SHADE:
					{
						(*pStr) += _T("Shade colour\r\n");
						FIXED24 Value24 = 0.0;
						Value24.SetAsFixed24(Comp1);
						camSprintf(s,_T("\tSaturation\t= %f\r\n"),Value24.MakeDouble());
						(*pStr) += s;
						Value24.SetAsFixed24(Comp2);
						camSprintf(s,_T("\tValue\t= %f\r\n"),Value24.MakeDouble());
						(*pStr) += s;
					}
					break;

				default:
					(*pStr) += _T("Unknown type of colour\r\n");
					break;
			}
		}
		break;
	}

	return;
}

/********************************************************************************************

>	void ColourRecordHandler::DecodeColour(ColourModel ColModel, UINT32 Comp1, UINT32 Comp2, UINT32 Comp3, UINT32 Comp4,
										   BOOL Linked, StringBase* pStr)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/96
	Inputs:		ColModel	- colour model of colour
				Comp1, Comp2, Comp3, Comp4	- colour components
				Linked		- True if this is a linked colour
				pStr		- ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the normal, linked and spot colour records.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ColourRecordHandler::DecodeColour(ColourModel ColModel, UINT32 Comp1, UINT32 Comp2, UINT32 Comp3, UINT32 Comp4,
									   BOOL Linked, StringBase* pStr)
{
	// Decode the contents of the colour according to what model it is	
	switch (ColModel)
	{
		case COLOURMODEL_RGBT:
			(*pStr) += _T("\tRed\t= ");
			DecodeComponent(Comp1, Linked, pStr);
			(*pStr) += _T("\tGreen\t= ");
			DecodeComponent(Comp2, Linked, pStr);
			(*pStr) += _T("\tBlue\t= ");
			DecodeComponent(Comp3, Linked, pStr);
			break;

		case COLOURMODEL_CMYK:
			(*pStr) += _T("\tCyan\t= ");
			DecodeComponent(Comp1, Linked, pStr);
			(*pStr) += _T("\tMagenta\t= ");
			DecodeComponent(Comp2, Linked, pStr);
			(*pStr) += _T("\tYellow\t= ");
			DecodeComponent(Comp3, Linked, pStr);
			(*pStr) += _T("\tKey\t= ");
			DecodeComponent(Comp4, Linked, pStr);
			break;
		case COLOURMODEL_HSVT:
			(*pStr) += _T("\tHue\t\t= ");
			DecodeComponent(Comp1, Linked, pStr);
			(*pStr) += _T("\tSaturation\t= ");
			DecodeComponent(Comp2, Linked, pStr);
			(*pStr) += _T("\tValue\t\t= ");
			DecodeComponent(Comp3, Linked, pStr);
			break;

		case COLOURMODEL_GREYT:
			(*pStr) += _T("\tGreyscale\t= ");
			DecodeComponent(Comp1, Linked, pStr);
			break;
		default:
			(*pStr) += _T("\tUnknown colour model\r\n");
			break;
	}

	return;
}

/********************************************************************************************

>	void ColourRecordHandler::DecodeComponent(UINT32 Comp, BOOL Linked, StringBase* pStr)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/96
	Inputs:		Comp		- colour component
				Linked		- True if this is a linked colour
				pStr		- ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for an individual colour component.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ColourRecordHandler::DecodeComponent(UINT32 Comp, BOOL Linked, StringBase* pStr)
{
	TCHAR s[256];
	// read the Red colour component in
	FIXED24 LinkValue = FIXED24(-8.0);
	FIXED24 Value24 = 0.0;
	Value24.SetAsFixed24(Comp);
	if (Linked && (Value24 == LinkValue))
	{
		(*pStr) += _T("Inherits\r\n");
	}
	else
	{
		camSprintf(s,_T("%f\r\n"),Value24.MakeDouble());
		(*pStr) += s;
	}
}

#endif

// #endif // NEW_NATIVE_FILTER

