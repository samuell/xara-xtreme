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

// The document component for the document's unit list.

// This is to allow the units for this document to be output to the new file format
// and loaded back in.

/*
*/

#include "camtypes.h"

#include "unitcomp.h"

//#include "resource.h"	// _R(IDS_OUT_OF_MEMORY)

#include "basedoc.h"
#include "app.h"

#include "camfiltr.h"	// BaseCamelotFilter - version 2 native filter
#include "cxftags.h"	// TAG_DEFINERGBCOLOUR TAG_DEFINECOMPLEXCOLOUR
#include "cxfdefs.h"	// SIZE_DEFINERGBCOLOUR SIZE_DEFINECOMPLEXCOLOUR
#include "cxfcols.h"	// REF_DEFAULTCOLOUR_TRANSPARENT
#include "cxfile.h"		// CXF_UNKNOWN_SIZE
//#include "expcol.h"		// ExportedColours handling class
#include "cxfrec.h"		// CXaraFileRecord handler
#include "cxfunits.h"	// default export unit types
//#include "filtrres.h"	// _R(IDS_NATIVE_UNITSWARNING)

#include "unittype.h"

DECLARE_SOURCE("$Revision$");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

CC_IMPLEMENT_DYNAMIC(UnitListComponentClass, 	DocComponentClass)
CC_IMPLEMENT_DYNAMIC(UnitListComponent, 		DocComponent)

/********************************************************************************************
********************************************************************************************/

/********************************************************************************************

>	BOOL UnitListComponentClass::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Returns:	TRUE if all went well;
				FALSE if not.
	Purpose:	Register the unit list document component with the main application.
	Errors:		Out of memory.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL UnitListComponentClass::Init()
{
	// Instantiate a component class to register with the application.
	UnitListComponentClass *pClass = new UnitListComponentClass;
	if (pClass == NULL)
		return FALSE;

	// Register it
	GetApplication()->RegisterDocComponent(pClass);

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL UnitListComponentClass::AddComponent(BaseDocument *pDocument)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pDocument - the document to add the unit list to.
	Returns:	TRUE if the unit list was added ok;
				FALSE if not.
	Purpose:	Add a unit list component to the specified document.
	Errors:		Out of memory
	SeeAlso:	UnitListComponentClass

********************************************************************************************/

BOOL UnitListComponentClass::AddComponent(BaseDocument *pDocument)
{
	// Check to see if this document already has a colour list; if so, leave it alone.
	if (pDocument->GetDocComponent(CC_RUNTIME_CLASS(UnitListComponent)) != NULL)
		return TRUE;

	// Ok - create the colour list component using this list.
	UnitListComponent *pComponent = new UnitListComponent();
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

>	UnitListComponent::UnitListComponent()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Construct a unit list component.

********************************************************************************************/

UnitListComponent::UnitListComponent()
{
	pDocUnitList = NULL;
	pDocument = NULL;

	// No hash tables yet
	pImpUserUnitMap = NULL;
	pExpUserUnitMap = NULL;

	// Our link to the BaseCamelotFilter
	pCamFilter = NULL;
	// Set our variable so that we warn about a possible replacement only once
	WarnedBefore = FALSE;
}

/********************************************************************************************

>	UnitListComponent::~UnitListComponent()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Clean up a unit list component's data structures.
	SeeAlso:	UnitListComponent

********************************************************************************************/

UnitListComponent::~UnitListComponent()
{
	// Trash the hash tables, if present
	if (pImpUserUnitMap)
	{
		delete pImpUserUnitMap;
		pImpUserUnitMap = NULL;
	}

	if (pExpUserUnitMap)
	{
		delete pExpUserUnitMap;
		pExpUserUnitMap = NULL;
	}
}

/********************************************************************************************
********************************************************************************************/


/********************************************************************************************
********************************************************************************************/

/********************************************************************************************

>	BOOL UnitListComponent::StartImport(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
	Returns:	TRUE if the component was able to prepare for importing;
				FALSE if not (e.g. out of memory)
	Purpose:	Inform the unit list document component that a Native or Web import is
				about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL UnitListComponent::StartImport(BaseCamelotFilter *pFilter)
{
TRACEUSER( "Neville", _T("UnitListComponent::StartImport\n"));
	if (pFilter)
	{
		// Get the document we are interested in
		pDocument = pFilter->GetDocument();

		// Get the unit list attached to the current document
		if (pDocument)
			pDocUnitList = pDocument->GetDocUnitList();
		ERROR3IF(pDocUnitList == NULL,"UnitListComponent::StartImport No doc unit list attached to this doc yet");

		// Save this in our link to the BaseCamelotFilter
		pCamFilter = pFilter;
		// Set our variable so that we warn about a possible replacement only once
		WarnedBefore = FALSE;

		// Get a hash table for the user units...
		try
		{
			pImpUserUnitMap = new CMapLongToPtr;
		}
		catch( CMemoryException )
		{
			ERROR( _R(IDS_OUT_OF_MEMORY), FALSE );
		}
	}
	else
	{
		pDocUnitList = NULL;
		pDocument = NULL;
		ERROR3("UnitListComponent::StartImport filter is null!");
	}
	
	return TRUE;
}

/********************************************************************************************

>	BOOL UnitListComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.
	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)
	Purpose:	Inform the unit list document component that a Native or Web import has
				just finished.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL UnitListComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)
{
TRACEUSER( "Neville", _T("UnitListComponent::EndImport\n"));
	if (pFilter == NULL)
	{
		ERROR3("UnitListComponent::EndImport filter is null!");
		return TRUE;
	}

	// Trash the import hash table, if present
	if (pImpUserUnitMap)
	{
		delete pImpUserUnitMap;
		pImpUserUnitMap = NULL;
	}

	// Null our link to the BaseCamelotFilter
	pCamFilter = NULL;
	// Set our variable so that we warn about a possible replacement only once
	WarnedBefore = FALSE;

	return TRUE;
}

/********************************************************************************************

>	Unit* UnitListComponent::GetReadUnitReference(INT32 UnitRef)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		UnitRef	- The unit reference or record number that the record handler has loaded
							from the record.
	Outputs:	-
	Returns:	pUnit	- A pointer to return the required document unit
	Purpose:	Try and convert a unit reference i.e. record number into a default or built in
				document unit or a previously imported user defined unit which should now be
				a user unit in the document.
				The public way of importing units and thier references for the new native/web filters
	SeeAlso:	- 

********************************************************************************************/

Unit* UnitListComponent::GetReadUnitReference(INT32 UnitRef)
{
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitListComponent::GetReadUnitReference called with no doc unit list pointer");

TRACEUSER( "Neville", _T("GetReadUnitReference for ref %d\n"), UnitRef);
	// First check to see if the unit refers to any of the built in units.
	// These will have negative record numbers
	Unit			   *pUnit = NULL;
	if (UnitRef <= 0)
	{
		UnitType type = GetUnitType(UnitRef);

		// If we succesfully converted the unit into a type then find the unit
		if (type != NOTYPE)
		{
			// Find this unit type in the current document list
			pUnit = pDocUnitList->FindUnit(type);
		}
		else
			ERROR2(NULL,"BitmapListComponent::GetReadBitmapReference negative UnitRef is unknown");
	}
	else
	{
		// Try and find the specified record number in our loaded user unit list
		CMapLongToPtr::const_iterator iter = pImpUserUnitMap->find( UnitRef );
		/*BOOL			ok =*/ pImpUserUnitMap->end();/* != iter;*/
		pUnit = (Unit *)iter->second;
		// if ok then pUnit will be the pointer to the required unit
		// so return this.
		// if not ok then pUnit will still be null.
				
		// If not found then this is a problem as a bitmap must have been defined before its being
		// referenced 
		if (pUnit == NULL)
		{
			// If we have a pFilter then ask the default warning handle to append our message
			if (pCamFilter)
			{
				if (!WarnedBefore)
				{
					pCamFilter->AppendWarning(_R(IDS_NATIVE_UNITSWARNING));
					WarnedBefore = TRUE;
				}

				// Now use a default unit instead, we will use the default page display units
				UnitType PageUnits = pDocUnitList->GetPageUnits();	// The units used to display page measurements
				pUnit = pDocUnitList->FindUnit(PageUnits);	
			}
			else
				ERROR2IF(pUnit == NULL,NULL,"BitmapListComponent::GetReadUnitReference UnitRef cannot be found");
		}
	}

	return pUnit;
}


/********************************************************************************************

>	BOOL UnitListComponent::ImportUserUnitDefinition(CXaraFileRecord* pCXaraFileRecord, Document * pDoc,
													 INT32 Tag)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
				pDoc			 = document to apply the information to
				Tag				 = the tag of the record
	Returns:	TRUE if the export happened correctly
				FALSE if not (e.g. out of memory)
	Purpose:	Import the user unit definition from the current record in the native/web
				file currently being loaded and add it to the document.

********************************************************************************************/

BOOL UnitListComponent::ImportUserUnitDefinition(CXaraFileRecord* pCXaraFileRecord, Document * pDoc,
													 INT32 Tag)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"UnitListComponent::ImportUserUnitDefinition called with no pCXaraFileRecord pointer");
	ERROR2IF(pDoc == NULL,FALSE,"UnitListComponent::ImportUserUnitDefinition called with no doc pointer");
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitListComponent::ImportUserUnitDefinition called with no doc unit list pointer");

	Unit* pUnit = NULL;

	BOOL ok = TRUE;
	ok = pDocUnitList->MakeNewUnit(&pUnit);

	if (ok && pUnit)
	{
		BOOL Prefix = TRUE;
		switch (Tag)
		{
			case TAG_DEFINE_PREFIXUSERUNIT:
				Prefix = TRUE;
				break;
			case TAG_DEFINE_SUFFIXUSERUNIT:
				Prefix = FALSE;
				break;
			default:
				ERROR3("Bad tag in ImportUserUnitDefinition");
		}
		pUnit->SetPrefixState(Prefix);
				
		// Read in the main full name of the unit
		String_32 Name;
		if (ok) ok = pCXaraFileRecord->ReadUnicode(&Name);//Name, Name.MaxLength());
		if (ok) ok = pUnit->SetToken(Name);

		// And the abbreviation
		String_32 Abbrev;
		if (ok) ok = pCXaraFileRecord->ReadUnicode(&Abbrev);//Abbrev, Abbrev.MaxLength());
		if (ok) ok = pUnit->SetSpecifier(Abbrev);
		
		// Read in the size of this unit, 0 means based on
		double UnitSize = 0.0;
		if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&UnitSize);
		if (ok) ok = pUnit->SetMillipoints(UnitSize);

		// Read in the exported base unit type
		INT32 ExportBaseUnit = 0L;
		if (ok) ok = pCXaraFileRecord->ReadINT32(&ExportBaseUnit);
		UnitType BasedOn = NOTYPE;
		// Convert this based on unit into the required forms
		Unit* pBasedOnUnit = GetReadUnitReference(ExportBaseUnit);
		if (pBasedOnUnit != NULL)
		{
			//BasedOn = GetUnitType(ExportBaseUnit);
			BasedOn = pBasedOnUnit->GetUnitType();
			if (ok) ok = pUnit->SetBaseUnitType(BasedOn);
		}
		else
		{
			ERROR3("UserUnit has a bad unit on which it is based");
		}

		// Read in the multipliers for this unit
		double BaseNumerator = 0.0;
		double BaseDenominator = 0.0;
		if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&BaseNumerator);
		if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&BaseDenominator);
		if (ok) ok = pUnit->SetBaseNumerator(BaseNumerator);
		if (ok) ok = pUnit->SetBaseDenominator(BaseDenominator);


		// Add the unit to the user units map so we can remember the references of the
		// imported units
		// Reference for this unit is the record number
		INT32 RecordNumber = pCXaraFileRecord->GetRecordNumber();
TRACEUSER( "Neville", _T("Imported user unit reference %d\n"), RecordNumber);
		(*pImpUserUnitMap)[ RecordNumber ] = pUnit;
	}

	// We will ignore any errors as we shouldn't not load a document because there is an
	// error in the units definition. We will just warn the user about a problem
	// If we have a pFilter then ask the default warning handle to append our message
	if (!ok && pCamFilter)
	{
		if (!WarnedBefore)
		{
			pCamFilter->AppendWarning(_R(IDS_NATIVE_UNITSWARNING));
			WarnedBefore = TRUE;
		}
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL UnitListComponent::ImportDefaultUnits(CXaraFileRecord* pCXaraFileRecord, Document * pDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
				pDoc			 = document to apply the information to		
	Returns:	TRUE if the export happened correctly
				FALSE if not (e.g. out of memory)
	Purpose:	Import the default display units from the current record in the native/web
				file currently being loaded.

********************************************************************************************/

BOOL UnitListComponent::ImportDefaultDisplayUnits(CXaraFileRecord* pCXaraFileRecord, Document * pDoc)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"UnitListComponent::ImportDefaultUnits called with no pCXaraFileRecord pointer");
	ERROR2IF(pDoc == NULL,FALSE,"UnitListComponent::ImportDefaultUnits called with no doc pointer");

	// Read in the export unit types
	INT32 ExportPageUnits = 0L;
	INT32 ExportFontUnits = 0L;

	// Read the desired units from the record
	pCXaraFileRecord->ReadINT32(&ExportPageUnits);
	pCXaraFileRecord->ReadINT32(&ExportFontUnits);

	// Convert these read in references to their correct unit types
	// First the page units
	UnitType PageUnits = NOTYPE;
	Unit* pPageUnits = GetReadUnitReference(ExportPageUnits);
	if (pPageUnits != NULL)
		PageUnits = pPageUnits->GetUnitType();
	else
		ERROR3("The default page units have a bad unit on which it is based");

	// Now the font units
	UnitType FontUnits = NOTYPE;
	Unit* pFontUnits = GetReadUnitReference(ExportFontUnits);
	if (pFontUnits != NULL)
		FontUnits = pFontUnits->GetUnitType();
	else
		ERROR3("The default font units have a bad unit on which it is based");

	DocUnitList* pDocUnitList = pDoc->GetDocUnitList();
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitListComponent::ImportDefaultUnits called with no doc unit list pointer");

	// Set the new default settings
	// The units used to display page measurements
	if (PageUnits != NOTYPE)
		pDocUnitList->SetPageUnits(PageUnits);	
	// The units to display font measurements
	if (FontUnits != NOTYPE)
		pDocUnitList->SetFontUnits(FontUnits);

	return TRUE;
}

/********************************************************************************************

>	UnitType UnitListComponent::GetUnitType(INT32 ExportUnitType)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/96
	Inputs:		ExportUnitType	- the unit specified as a default export unit
	Returns:	The UnitType in the internal format used by Camelot
	Purpose:	Convert the built in unit types as defined in the file format into the ones
				used internally by Camelot.

********************************************************************************************/

UnitType UnitListComponent::GetUnitType(INT32 ExportUnitType)
{
	UnitType Type = NOTYPE;
	switch (ExportUnitType)
	{
		case REF_DEFAULTUNIT_MILLIMETRES:	Type = MILLIMETRES;	break;
		case REF_DEFAULTUNIT_CENTIMETRES:	Type = CENTIMETRES;	break;
		case REF_DEFAULTUNIT_METRES:		Type = METRES;		break;
		case REF_DEFAULTUNIT_KILOMETRES:	Type = KILOMETRES;	break;
		case REF_DEFAULTUNIT_MILLIPOINTS:	Type = MILLIPOINTS;	break;
		case REF_DEFAULTUNIT_COMP_POINTS:	Type = COMP_POINTS;	break;
		case REF_DEFAULTUNIT_PICAS:			Type = PICAS;		break;
		case REF_DEFAULTUNIT_INCHES:		Type = INCHES;		break;
		case REF_DEFAULTUNIT_FEET:			Type = FEET;		break;
		case REF_DEFAULTUNIT_YARDS:			Type = YARDS;		break;
		case REF_DEFAULTUNIT_MILES:			Type = MILES;		break;
		case REF_DEFAULTUNIT_PIXELS:		Type = PIXELS;		break;
		
		case REF_DEFAULTUNIT_NOTYPE:		Type = NOTYPE;		break;	
	}
	
	return Type; 
}

/********************************************************************************************

>	BOOL UnitListComponent::StartExport(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Returns:	TRUE if the component was able to prepare for exporting;
				FALSE if not (e.g. out of memory)
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
	Purpose:	Inform the unit list document component that a Web or Native export is
				about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL UnitListComponent::StartExport(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR3IF(pExpUserUnitMap != NULL,"UnitListComponent::StartExport pExpUserUnitMap is non-NULL!");

	if (pFilter)
	{
		// Get the document we are interested in
		pDocument = pFilter->GetDocument();

		// Get the unit list attached to the current document
		if (pDocument)
			pDocUnitList = pDocument->GetDocUnitList();
		ERROR3IF(pDocUnitList == NULL,"UnitListComponent::StartExport No doc unit list attached to this doc yet");

		// Get a hash table for the user units...
/*		TRY
		{
			pExpUserUnitMap = new CMapPtrToPtr;
		}
		CATCH (CMemoryException, e)
		{
			ERROR1(FALSE, _R(IDS_OUT_OF_MEMORY));
		}
		END_CATCH
*/
		try
		{
			pExpUserUnitMap = new CMapPtrToLong;
		}
		catch (std::bad_alloc&)
		{
			ERROR1(FALSE, _R(IDS_OUT_OF_MEMORY));
		}
		catch (...)
		{
			ERROR1(FALSE, _R(IDS_UNKNOWN_ERROR));
		}

	}
	else
	{
		pDocUnitList = NULL;
		pDocument = NULL;
	}

#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL UnitListComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - True if everything went swimmingly, False if just a clean up is required.
	Purpose:	Inform the unit list document component that a Web or Native export has
				just finished.
	Returns:	TRUE if the export happened correctly
				FALSE if not (e.g. out of memory)
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL UnitListComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)
{
#ifdef DO_EXPORT

	BOOL ok = TRUE;

	// Only export if we everything went ok up to now and we are not in a clean
	// up operation and we are doing a native file
	if (Success && !pFilter->IsWebFilter())
	{
		// Export any user defined units
		if (ok) ok = ExportUserUnits(pFilter);

		// Now export the default display units that the user has set
		if (ok) ok = ExportDefaultDisplayUnits(pFilter);
	}

	// Clean up the hash table
	if (pExpUserUnitMap)
	{
		delete pExpUserUnitMap;
		pExpUserUnitMap = NULL;
	}

	return ok;
#else
	return TRUE;
#endif
}


/********************************************************************************************

>	INT32 UnitListComponent::GetWriteUnitReference(Unit* pUnit, BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		pUnit		- The unit to save
				pFilter		- The filter to use for saving
	Outputs:	-
	Returns:	The unit reference for the unit ready for writing in a record.
				This will be zero if a problem has occurred. May return -1 if it does not
				need to save anything i.e if the colour is not in use and not named.

	Purpose:	Return the unit reference to the caller, who will then use it to write the
				unit in the record that they are preparing to save to a Web or Native file.
				In the process of getting the unit reference, the user unit definition record
				for the unit reference will be generated and saved to the file if this is
				required. This is only required if the unit has been defined by the user as
				all others are assumed.
				NOTE: As this does save information into the record stream, it *MUST* be called
				before the caller has called StartRecord() as otherwise the records will become
				intertwined!
	SeeAlso:	DocComponent

********************************************************************************************/

INT32 UnitListComponent::GetWriteUnitReference(Unit* pUnit, BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pUnit == NULL,0L,"UnitListComponent::GetWriteUnitReference null pUnit");
	ERROR2IF(pFilter == NULL,0L,"UnitListComponent::GetWriteUnitReference null pFilter");

	INT32 RecordNumber = 0L;

	// Is the unit a user defined unit or one of the defaults?
	if (pUnit->IsDefault())
	{
		UnitType type = pUnit->GetUnitType();
		// Its a default unit so convert the type into the default reference
		RecordNumber = GetExportUnitType(type);
	}
	else
	{
		// See if we have saved the unit definition out by checking our has table
		// Try and find the specified record number in our loaded user unit list
		if (pExpUserUnitMap->find(pUnit)==pExpUserUnitMap->end())
		{
			// We have not seen this unit before so save out the definition
			/*BOOL ok =*/ ExportUnitDefinition(pFilter, pUnit);
		}
	}

	return RecordNumber;
#else
	return 0;
#endif
}

/********************************************************************************************

>	INT32 UnitListComponent::GetWriteUnitReference(Unit* pUnit, BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		UnitType	- The unit type to save
				pFilter		- The filter to use for saving
	Outputs:	-
	Returns:	The unit reference for the unit ready for writing in a record.
				This will be zero if a problem has occurred. May return -1 if it does not
				need to save anything i.e if the colour is not in use and not named.

	Purpose:	Return the unit reference to the caller, who will then use it to write the
				unit in the record that they are preparing to save to a Web or Native file.
				In the process of getting the unit reference, the user unit definition record
				for the unit reference will be generated and saved to the file if this is
				required. This is only required if the unit has been defined by the user as
				all others are assumed.
				NOTE: As this does save information into the record stream, it *MUST* be called
				before the caller has called StartRecord() as otherwise the records will become
				intertwined!
	SeeAlso:	DocComponent

********************************************************************************************/

INT32 UnitListComponent::GetWriteUnitReference(UnitType type, BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,0L,"UnitListComponent::GetWriteUnitReference null pFilter");
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitListComponent::GetWriteUnitReference called with no doc unit list pointer");

	INT32 RecordNumber = 0L;

	// Get a pointer to the unit
	Unit* pUnit = NULL;
	pUnit = pDocUnitList->FindUnit(type);	

	// Is the unit a user defined unit or one of the defaults?
	if (pUnit->IsDefault())
	{
		// Its a default unit so convert the type into the default reference
		RecordNumber = GetExportUnitType(type);
	}
	else
	{
		// See if we have saved the unit definition out by checking our has table
		// Try and find the specified record number in our loaded user unit list
		if (pExpUserUnitMap->find(pUnit)==pExpUserUnitMap->end())
		{
			// We have not seen this unit before so save out the definition
			/*BOOL ok =*/ ExportUnitDefinition(pFilter, pUnit);
		}
	}

	return RecordNumber;
#else
	return 0;
#endif
}


/********************************************************************************************

>	BOOL UnitListComponent::ExportUserUnits(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
	Returns:	TRUE if the export happened correctly
				FALSE if not (e.g. out of memory)
	Purpose:	Export any user defined units into the native/web file currently being saved.

********************************************************************************************/

BOOL UnitListComponent::ExportUserUnits(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"UnitListComponent::ExportUserUnits null pFilter");
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitListComponent::ExportUserUnits called with no doc unit list pointer");

	// Export any user defined units
	Unit* pUnit = NULL;
	BOOL ok = TRUE;
	INT32 RecNum = 0;

	pUnit = pDocUnitList->FindFirstUserUnit();
	while (pUnit != NULL)
	{
		RecNum = ExportUnitDefinition(pFilter, pUnit);
		if (RecNum == 0)
			break;

		pUnit = pDocUnitList->FindNextUserUnit(pUnit);
	}

	return ok;
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	INT32 UnitListComponent::ExportUnitDefinition(BaseCamelotFilter *pFilter, Unit* pUnit)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				pUnit	- the unit definition to be exported
	Returns:	The record number of the unit if the export happened correctly or if the unit
				definition has already happened.
				0 if not (e.g. out of memory)
	Purpose:	Export the definition for the specified unit into the native/web file
				currently being saved.

********************************************************************************************/

INT32 UnitListComponent::ExportUnitDefinition(BaseCamelotFilter *pFilter, Unit* pUnit)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,0L,"UnitListComponent::ExportUnitDefinition null pFilter");
	ERROR2IF(pUnit == NULL,0L,"UnitListComponent::ExportUnitDefinition null pUnit");
	ERROR2IF(pDocUnitList == NULL,0L,"UnitListComponent::ExportUnitDefinition called with no doc unit list pointer");

	ERROR2IF(pUnit->IsDefault(),0L,"UnitListComponent::ExportUnitDefinition trying to save default unit definition");

	INT32 RecordNumber = 0L;

	// See if we have saved the unit definition out by checking our table
	// If so then do nothing
	CMapPtrToLong::iterator it = pExpUserUnitMap->find(pUnit);
	if (it!=pExpUserUnitMap->end())
		return it->second;

	// Export the definition for this unit
	// First get all the details
//	UnitType type = pUnit->GetUnitType();
	
	// Set up the prefix/suffix state for this user's unit.
	BOOL Prefix = pUnit->IsPrefix();
	INT32 Tag = 0L;
	INT32 Size = 0L;
	if (Prefix)
	{
		Tag = TAG_DEFINE_PREFIXUSERUNIT;
		Size = TAG_DEFINE_PREFIXUSERUNIT_SIZE;
	}
	else
	{
		Tag = TAG_DEFINE_SUFFIXUSERUNIT;
		Size = TAG_DEFINE_SUFFIXUSERUNIT_SIZE;
	}

	// The main full name of the unit
	String_32 Name = pUnit->GetToken();
	// And the abbreviation
	String_32 Abbrev = pUnit->GetSpecifier();

	// If there are string names, then add it to this size
	// REMEMBER: We save out unicode strings and so we need to double the length of the returned string length
	Size += (Name.Length() + 1) * SIZEOF_XAR_UTF16;
	Size += (Abbrev.Length() + 1) * SIZEOF_XAR_UTF16;

	BOOL ok = TRUE;
	CXaraFileRecord Rec(Tag, Size);
	ok = Rec.Init();

	// Write out the name and abbreviation for this unit
	if (ok) ok = Rec.WriteUnicode(Name);
	if (ok) ok = Rec.WriteUnicode(Abbrev);

	// The size of this unit, 0 if based on
	double UnitSize = pUnit->GetMillipoints();
	if (ok) ok = Rec.WriteDOUBLE(UnitSize);

	// What unit this is based on
	UnitType BaseUnit = pUnit->GetBaseUnitType();
	//INT32 ExportBaseUnit = GetExportUnitType(BaseUnit);
	INT32 ExportBaseUnit = GetWriteUnitReference(BaseUnit, pFilter);
	ERROR2IF(ExportBaseUnit == 0,FALSE,"bad base units reference!");
	if (ok) ok = Rec.WriteINT32(ExportBaseUnit);

	// Write out the multipliers for this unit
	double BaseNumerator = 0.0;
	double BaseDenominator = 0.0;
	BaseNumerator = pUnit->GetBaseNumerator();
	BaseDenominator = pUnit->GetBaseDenominator();
	if (ok) ok = Rec.WriteDOUBLE(BaseNumerator);
	if (ok) ok = Rec.WriteDOUBLE(BaseDenominator);

	// Finally, write the record out to file
	// In the process get the record number that this was written out as
	if (ok) RecordNumber = pFilter->Write(&Rec);	// Get the document comment

	// Add the unit to the user units map so we can remember the references of the
	// exported units and hence whether we have exported this before
	// Reference for this unit is the record number
TRACEUSER( "Neville", _T("Exported user unit reference %d\n"), RecordNumber);
	try
	{
		(*pExpUserUnitMap)[pUnit] = RecordNumber;
	}
	catch (std::bad_alloc&)
	{
		ERROR1(FALSE, _R(IDS_OUT_OF_MEMORY));
	}
	catch (...)
	{
		ERROR1(FALSE, _R(IDS_UNKNOWN_ERROR));
	}

	return RecordNumber;
#else
	return 0L;
#endif
}

/********************************************************************************************

>	BOOL UnitListComponent::ExportDefaultDisplayUnits(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
	Returns:	TRUE if the export happened correctly
				FALSE if not (e.g. out of memory)
	Purpose:	Export the default display units into the native/web file currently being saved.

********************************************************************************************/

BOOL UnitListComponent::ExportDefaultDisplayUnits(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitListComponent::ExportDisplayDefaultUnits called with no doc unit list pointer");

	// Get the current default settings
	UnitType PageUnits   = pDocUnitList->GetPageUnits();	// The units used to display page measurements
	//UnitType ScaledUnits = pDocUnitList->GetScaleUnits();	// The units to display scaled measurements
	UnitType FontUnits   = pDocUnitList->GetFontUnits();	// The units to display font measurements

	// Convert these into the export unit types
	//INT32 ExportPageUnits = GetExportUnitType(PageUnits);
	//INT32 ExportFontUnits = GetExportUnitType(FontUnits);
	INT32 ExportPageUnits = GetWriteUnitReference(PageUnits, pFilter);
	ERROR2IF(ExportPageUnits == 0,FALSE,"UnitListComponent::ExportDisplayDefaultUnits bad page units reference!");
	INT32 ExportFontUnits = GetWriteUnitReference(FontUnits, pFilter);
	ERROR2IF(ExportFontUnits == 0,FALSE,"UnitListComponent::ExportDisplayDefaultUnits bad font units reference!");

	BOOL ok = TRUE;

	if (ExportPageUnits !=0 && ExportFontUnits != 0)
	{
			CXaraFileRecord Rec(TAG_DEFINE_DEFAULTUNITS, TAG_DEFINE_DEFAULTUNITS_SIZE);
			ok = Rec.Init();

			if (ok) ok = Rec.WriteINT32(ExportPageUnits);
			if (ok) ok = Rec.WriteINT32(ExportFontUnits);

			// Finally, write the record out to file
			// In the process get the record number that this was written out as
			INT32 RecordNumber = 0L;
			if (ok) RecordNumber = pFilter->Write(&Rec);	// Get the document comment

			// If we have had a problem at any of the stages then return that to the caller
			if (RecordNumber <= 0)
				ok = FALSE;
	}
	else
		ERROR3("Bad conversion of default font or page units");

	return ok;
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	INT32 UnitListComponent::GetExportUnitType(UnitType type)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/96
	Inputs:		type	- the unit type used internally by Camelot
	Returns:	The default export unit type as used in the native/web file format
	Purpose:	Convert the built in unit types used internally by Camelot into the ones
				defined in the native/web file format.

********************************************************************************************/

INT32 UnitListComponent::GetExportUnitType(UnitType type)
{
#ifdef DO_EXPORT
	INT32 DefUnitType = 0;
	switch (type)
	{
		case MILLIMETRES:	DefUnitType = REF_DEFAULTUNIT_MILLIMETRES;	break;
		case CENTIMETRES:	DefUnitType = REF_DEFAULTUNIT_CENTIMETRES;	break;
		case METRES:		DefUnitType = REF_DEFAULTUNIT_METRES;		break;
		case KILOMETRES:	DefUnitType = REF_DEFAULTUNIT_KILOMETRES;	break;
		case MILLIPOINTS:	DefUnitType = REF_DEFAULTUNIT_MILLIPOINTS;	break;
		case COMP_POINTS:	DefUnitType = REF_DEFAULTUNIT_COMP_POINTS;	break;
		case PICAS:			DefUnitType = REF_DEFAULTUNIT_PICAS;		break;
		case INCHES:		DefUnitType = REF_DEFAULTUNIT_INCHES;		break;
		case FEET:			DefUnitType = REF_DEFAULTUNIT_FEET;			break;
		case YARDS:			DefUnitType = REF_DEFAULTUNIT_YARDS;		break;
		case MILES:			DefUnitType = REF_DEFAULTUNIT_MILES;		break;
		case PIXELS:		DefUnitType = REF_DEFAULTUNIT_PIXELS;		break;

		case NOTYPE:		DefUnitType = REF_DEFAULTUNIT_NOTYPE;		break;

		case AUTOMATIC:		
		case NUM_DEFAULT_UNIT_TYPES:
							ERROR3("This shouldn't happen!");
							DefUnitType = 0; break;
	}

	return DefUnitType; 
#else
	return 0L; 
#endif
}
