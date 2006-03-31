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

#include "rechunit.h"	// UnitsRecordHandler for importing units from v2 native/web files
#include "unitcomp.h"	// Document units component, handles import of units
#include "camfiltr.h"	// BaseCamelotFilter - version 2 native filter
#include "cxftags.h"	// TAG_DEFINERGBCOLOUR TAG_DEFINECOMPLEXCOLOUR
#include "cxfrec.h"		// TAG_DEFINERGBCOLOUR TAG_DEFINECOMPLEXCOLOUR
#include "cxfunits.h"	// default export unit types

DECLARE_SOURCE("$Revision$");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNAMIC(UnitsRecordHandler,CamelotRecordHandler);

// #if NEW_NATIVE_FILTER	// New native filters, only available to those who need them at present

/********************************************************************************************

>	virtual BOOL UnitsRecordHandler::BeginImport()

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		0
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Initialises the units record handler.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UnitsRecordHandler::BeginImport()
{
	pUnitsComponent = GetUnitDocComponent();

	return (pUnitsComponent != NULL);
}

/********************************************************************************************

>	virtual UINT32* UnitsRecordHandler::GetTagList()

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* UnitsRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_DEFINE_PREFIXUSERUNIT, TAG_DEFINE_SUFFIXUSERUNIT,
							  TAG_DEFINE_DEFAULTUNITS,
							  CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL UnitsRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL UnitsRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
// WEBSTER - markn 17/12/96
// No unit record handling needed
#ifdef WEBSTER
	return TRUE;
#else
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"UnitsRecordHandler::HandleRecord pCXaraFileRecord is NULL");

	BOOL ok = TRUE;
	Document * pDoc = GetDocument();
	INT32 Tag = pCXaraFileRecord->GetTag();
	switch (Tag)
	{
		 case TAG_DEFINE_DEFAULTUNITS:
			// Ask the units document component class to import those default display units for us
			// If we are just importing data from this document into an existing one,
			// then don't import this data.
			if (IsImporting())
				break;
			if (pUnitsComponent)
				pUnitsComponent->ImportDefaultDisplayUnits(pCXaraFileRecord, pDoc);
			else
				ERROR3("UnitsRecordHandler::HandleRecord no pUnitsComponent");
			break;
		 case TAG_DEFINE_PREFIXUSERUNIT:
		 case TAG_DEFINE_SUFFIXUSERUNIT:
			// Ask the units document component class to import this user unit definition for us
			if (pUnitsComponent)
				pUnitsComponent->ImportUserUnitDefinition(pCXaraFileRecord, pDoc, Tag);
			else
				ERROR3("UnitsRecordHandler::HandleRecord no pUnitsComponent");
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("UnitsRecordHandler::HandleRecord I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
#endif // WEBSTER
}

/********************************************************************************************

>	virtual void UnitsRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

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
void UnitsRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord, StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	// Call base class first
	// This outputs the tag and size
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	UINT32 Tag = pRecord->GetTag();
	BOOL ok = TRUE;
	char s[256];
	INT32 RecordNumber = pRecord->GetRecordNumber();
	switch (Tag)
	{
		case TAG_DEFINE_DEFAULTUNITS:
		{
			// Read in the export unit types
			INT32 ExportPageUnits = 0L;
			INT32 ExportFontUnits = 0L;
			if (ok) ok = pRecord->ReadINT32(&ExportPageUnits);
			(*pStr) += "Page units: ";
			DescribeDefaultUnit(ExportPageUnits, pStr);
			(*pStr) += "\r\n";
			if (ok) ok = pRecord->ReadINT32(&ExportFontUnits);
			(*pStr) += "Font units: ";
			DescribeDefaultUnit(ExportFontUnits, pStr);
			(*pStr) += "\r\n";
			break;
		}

		 case TAG_DEFINE_PREFIXUSERUNIT:
		 case TAG_DEFINE_SUFFIXUSERUNIT:
		{
			BOOL Prefix = TRUE;
			switch (Tag)
			{
				case TAG_DEFINE_PREFIXUSERUNIT:
					(*pStr) += "Define prefix unit: \r\n\r\n";
					break;
				case TAG_DEFINE_SUFFIXUSERUNIT:
					(*pStr) += "Define suffix unit: \r\n\r\n";
					break;
				default:
					ERROR3("Bad tag in ImportUserUnitDefinition");
			}
					
			// Read in the main full name of the unit
			String_32 Name;
			pRecord->ReadUnicode(&Name);//Name, Name.MaxLength());
			(*pStr) += "Name of unit: \t\t= ";
			(*pStr) += Name;
			(*pStr) += "\r\n";

			// And the abbreviation
			String_32 Abbrev;
			pRecord->ReadUnicode(&Abbrev);//Abbrev, Abbrev.MaxLength());
			(*pStr) += "Abbreviation for unit: \t= ";
			(*pStr) += Abbrev;
			(*pStr) += "\r\n\r\n";
			
			// Read in the size of this unit, 0 means based on
			double UnitSize = 0.0;
			pRecord->ReadDOUBLE(&UnitSize);
			_stprintf(s,"Size of unit: \t\t= %f\r\n",UnitSize);
			(*pStr) += s;

			// Read in the exported base unit type
			INT32 ExportBaseUnit = 0L;
			pRecord->ReadINT32(&ExportBaseUnit);
			(*pStr) += "Based on unit: \t\t= ";
			DescribeDefaultUnit(ExportBaseUnit, pStr);
			(*pStr) += "\r\n";

			// Read in the multipliers for this unit
			double BaseNumerator = 0.0;
			double BaseDenominator = 0.0;
			pRecord->ReadDOUBLE(&BaseNumerator);
			pRecord->ReadDOUBLE(&BaseDenominator);
			_stprintf(s,"Numerator: \t\t= %f\r\n",BaseNumerator);
			(*pStr) += s;
			_stprintf(s,"Denominator: \t\t= %f\r\n",BaseDenominator);
			(*pStr) += s;

			break;
		}
	} 

	return;
}

/********************************************************************************************

>	virtual void UnitsRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/96
	Inputs:		ExportUnit(*pStr) += default unit to describe
				pStr = ptr to string to update
	Returns:	-
	Purpose:	Converts the ExportUnitType into a textual description.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void UnitsRecordHandler::DescribeDefaultUnit(INT32 ExportUnitType, StringBase* pStr)
{
	char s[256];
	if (ExportUnitType < 0)
	{
		switch (ExportUnitType)
		{
			case REF_DEFAULTUNIT_MILLIMETRES:	(*pStr) += "MILLIMETRES";	break;
			case REF_DEFAULTUNIT_CENTIMETRES:	(*pStr) += "CENTIMETRES";	break;
			case REF_DEFAULTUNIT_METRES:		(*pStr) += "METRES";		break;
			case REF_DEFAULTUNIT_KILOMETRES:	(*pStr) += "KILOMETRES";	break;
			case REF_DEFAULTUNIT_MILLIPOINTS:	(*pStr) += "MILLIPOINTS";	break;
			case REF_DEFAULTUNIT_COMP_POINTS:	(*pStr) += "COMP_POINTS";	break;
			case REF_DEFAULTUNIT_PICAS:			(*pStr) += "PICAS";			break;
			case REF_DEFAULTUNIT_INCHES:		(*pStr) += "INCHES";		break;
			case REF_DEFAULTUNIT_FEET:			(*pStr) += "FEET";			break;
			case REF_DEFAULTUNIT_YARDS:			(*pStr) += "YARDS";			break;
			case REF_DEFAULTUNIT_MILES:			(*pStr) += "MILES";			break;
			case REF_DEFAULTUNIT_PIXELS:		(*pStr) += "PIXELS";		break;

			case REF_DEFAULTUNIT_NOTYPE:		(*pStr) += "NOTYPE";		break;
			default:
				_stprintf(s,"%d",ExportUnitType);
				(*pStr) += s;
				break;
		}
	}
	else
	{
		_stprintf(s,"%d",ExportUnitType);
		(*pStr) += s;
	}
}

#endif // _DEBUG

// #endif // NEW_NATIVE_FILTER

