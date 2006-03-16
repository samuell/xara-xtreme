// $Id: colcomp.cpp 662 2006-03-14 21:31:49Z alex $
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

// The document component for the document's colour list.

/*
*/

#include "camtypes.h"

#include "colcomp.h"

#include "basedoc.h"
#include "app.h"
#include "collist.h"
#include "colormgr.h"
#include "colmodel.h"
#include "aw_eps.h"
#include "cameleps.h"
#include "fixmem.h"
#include "ccdc.h"
#include "colourix.h"
#include "doccolor.h"
//#include "tim.h"
//#include "resource.h"
//#include "grndrgn.h"
#include "impcol.h"
//#include "jason.h"
#include "unicdman.h"
//#include "sgcolour.h"

#include "camfiltr.h"	// BaseCamelotFilter - version 2 native filter
#include "cxftags.h"	// TAG_DEFINERGBCOLOUR TAG_DEFINECOMPLEXCOLOUR
#include "cxfdefs.h"	// SIZE_DEFINERGBCOLOUR SIZE_DEFINECOMPLEXCOLOUR
#include "cxfcols.h"	// REF_DEFAULTCOLOUR_TRANSPARENT
#include "cxfile.h"		// CXF_UNKNOWN_SIZE
#include "expcol.h"		// ExportedColours handling class
#include "cxfrec.h"		// CXaraFileRecord handler
//#include "filtrres.h"	// _R(IDS_NATIVE_COLOURWARNING)

DECLARE_SOURCE("$Revision: 662 $");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

CC_IMPLEMENT_DYNAMIC(ColourListComponentClass, 	DocComponentClass)
CC_IMPLEMENT_DYNAMIC(ColourListComponent, 		DocComponent)

// The error limit to use when comparing colour definitions
#define COL_ACCURACY (0.00001)

// This is our special linked value for native/web files
const FIXED24 LinkValue = FIXED24(-8.0);

// If we do no load a colour definition, then this is the colour that we will use instead
#define REPLACEMENTCOLOUR COLOUR_BLACK

// WEBSTER - markn 14/2/97
// Introduced WEBSTER_IGNORE_NAME_MATCHING so that Webster can ignore the name when
// trying to find an identical colour
// Set to FALSE if you want it to behave the same as v1.5
#ifndef WEBSTER
#define WEBSTER_IGNORE_NAME_MATCHING FALSE
#else
#define WEBSTER_IGNORE_NAME_MATCHING TRUE
#endif // WEBSTER

/********************************************************************************************
********************************************************************************************/

/********************************************************************************************

>	BOOL ColourListComponentClass::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Returns:	TRUE if all went well;
				FALSE if not.
	Purpose:	Register the colour list document component with the main application.
	Errors:		Out of memory.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL ColourListComponentClass::Init()
{
	// Instantiate a component class to register with the application.
	ColourListComponentClass *pClass = new ColourListComponentClass;
	if (pClass == NULL)
		return FALSE;

	// Register it
	GetApplication()->RegisterDocComponent(pClass);

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL ColourListComponentClass::AddComponent(BaseDocument *pDocument)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pDocument - the document to add the colour list to.
	Returns:	TRUE if the colour list was added ok;
				FALSE if not.
	Purpose:	Add a colour list component to the specified document.
	Errors:		Out of memory
	SeeAlso:	ColourListComponentClass

********************************************************************************************/

BOOL ColourListComponentClass::AddComponent(BaseDocument *pDocument)
{
	// Check to see if this document already has a colour list; if so, leave it alone.
	if (pDocument->GetDocComponent(CC_RUNTIME_CLASS(ColourListComponent)) != NULL)
		return TRUE;

	// No colour list - try to create a new one for this document.
	ColourList *pList = new ColourList;
	if(pList == NULL)
		return FALSE;

	pList->Init( pDocument );		// Let the ColourList know what its parent document is

	// Ok - create the colour list component using this list.
	ColourListComponent *pComponent = new ColourListComponent(pList);
	if (pComponent == NULL)
	{
		// Out of memory...
		delete pList;
		return FALSE;
	}

	// All ok - add the component to the document.
	pDocument->AddDocComponent(pComponent);
	return true;
}



/********************************************************************************************

>	ColourListComponent::ColourListComponent()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/11/94
	Purpose:	Construct a colour list component. DO NOT use this constructor. It gives
				and ERROR3 (and sets up this in a semi-safe default state)
	SeeAlso:	ColourListComponent

********************************************************************************************/

ColourListComponent::ColourListComponent()
{
	ERROR3("ColourListComponent constructed with default constructor!?\n");
	
	// We don't have a colour list! Eek!
	pIndexedColours = NULL;

	// Initialise importer colour maps etc
	pNewColours	= NULL;

	// Initialise Exporter colour list
	pExportedColours = NULL;
	
	// Initialise IndexedColour ComponentCopy array pointers
	SourceColours	= NULL;
	DestColours		= NULL;
	ColourTableSize	= 32;

	// Our link to the BaseCamelotFilter
	pCamFilter = NULL;
	// Set our variable so that we warn about a possible replacement only once
	WarnedBefore = FALSE;
}



/********************************************************************************************

>	ColourListComponent::ColourListComponent(ColourList *pColList)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pColList - the colour list to use for this colour list componet.
	Purpose:	Construct a colour list component using the given colour list.

	Notes:		NOTE that the list you pass in now "belongs" to this CLC, and it
				will be automatically deleted when you delete this CLC object.

	Errors:		ENSURE if pColList is NULL.
	SeeAlso:	ColourListComponent

********************************************************************************************/

ColourListComponent::ColourListComponent(ColourList *pColList)
{
	ENSURE(pColList != NULL, "NULL colour list in colour list component constructor!");

	// Install this colour list.
	pIndexedColours = pColList;

	// Initialise importer colour tables etc
	pNewColours	= NULL;

	// Initialise Exporter colour list
	pExportedColours = NULL;

	// Initialise IndexedColour ComponentCopy array pointers
	SourceColours	= NULL;
	DestColours		= NULL;
	ColourTableSize	= 32;

	// Our link to the BaseCamelotFilter
	pCamFilter = NULL;
	// Set our variable so that we warn about a possible replacement only once
	WarnedBefore = FALSE;
}

/********************************************************************************************

>	ColourListComponent::~ColourListComponent()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/09/94
	Purpose:	Clean up a colour list component's data structures - deletes the colour
				list.
	Errors:		ENSURE if IndexedColour copying stuff has not been cleaned up (this will
				occur if End/AbortComponentCopy is not called after a component copy)
	SeeAlso:	ColourListComponent

********************************************************************************************/

ColourListComponent::~ColourListComponent()
{
	// Delete our colour list.
	if (pIndexedColours != NULL)
	{
		pIndexedColours->DeleteAll();

		delete pIndexedColours;
		pIndexedColours = NULL;
	}

	// Sanity check to see if we've done something terrible...
	ENSURE(SourceColours == NULL && DestColours == NULL,
			"Someone failed to EndComponentCopy before destructing a ColourListComponent");

	// As we have finished with it now, destroy the list of exported colours
	if (pExportedColours)
	{
		delete pExportedColours;
		pExportedColours = NULL;
	}
}

/********************************************************************************************
********************************************************************************************/

// Saving/loading code for the new native/web file format

/********************************************************************************************
********************************************************************************************/

/********************************************************************************************

>	BOOL ColourListComponent::StartImport(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
	Returns:	TRUE if the component was able to prepare for importing;
				FALSE if not (e.g. out of memory)
	Purpose:	Inform the colour list document component that a Native or Web import is
				about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL ColourListComponent::StartImport(BaseCamelotFilter *pFilter)
{
TRACEUSER( "Neville",_T("ColourListComponent::StartImport\n") );
	if (pFilter == NULL)
	{
		ERROR3("ColourListComponent::StartImport filter is null!");
		return TRUE;
	}

	// Save this in our link to the BaseCamelotFilter
	pCamFilter = pFilter;
	// Set our variable so that we warn about a possible replacement only once
	WarnedBefore = FALSE;
	
	// Get a colour table and don't do strict checking (as per Camelot EPS and Native EPS).
	pNewColours = new ImportedColours(this, FALSE);
	if ((pNewColours == NULL) || !pNewColours->Init())
		return FALSE;

	// We keep a record of how nested we are with respect to linking colours, e.g.
	// links of links of links.
	// A nesting of 0 is no nesting.
	// Do this so that we are consistent with the EPS form
	LinkNesting = 0;

	return TRUE;
}

/********************************************************************************************

>	BOOL ColourListComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.
	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)
	Purpose:	Inform the colour list document component that a Native or Web import has
				just finished.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL ColourListComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)
{
TRACEUSER( "Neville", _T("ColourListComponent::EndImport\n") );
	if (pFilter == NULL)
	{
		ERROR3("ColourListComponent::EndImport filter is null!");
		return TRUE;
	}

	// If we didn't even get to initialise, then return quietly.
	if (pNewColours == NULL)
	{
		ERROR3("ColourListComponent::EndImport pNewColours is null!");
		return TRUE;
	}

	// Import is finished, so add any outstanding colours to the document (if the
	// import was successful), and delete the colour table
	if (Success)
	{
		// Try and sort the imported colour list by the entry number that was saved
		// in the colour definition on export. This was the colour's position in 
		// the indexed colour list.
		pNewColours->SortColoursByEntryNumber();
		
		// Adds the colours to the system, with undo
		pNewColours->AddColoursToDocument();
	}
	else
	{
		// Import failed - destroy all the colours.
		pNewColours->DestroyColours();
	}

	// Finally, delete the IndexedColour table.
	delete pNewColours;
	pNewColours = NULL;

	// Null our link to the BaseCamelotFilter
	pCamFilter = NULL;
	// Set our variable so that we warn about a possible replacement only once
	WarnedBefore = FALSE;

	return TRUE;
}

/********************************************************************************************

>	BOOL ColourListComponent::GetDocColour(INT32 ColourRef, CXaraFileRecord* pCXFileRec, DocColour* pDocColour)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		ColourRef	- The colour reference or record number that the record handler has loaded
							from the record.
	Outputs:	pDocColour	- A pointer to return the required doc colour in
	Returns:	TRUE if the colour reference was succesfully converted into a DocColour;
				FALSE if not (e.g. out of memory)
	Purpose:	Try and convert a colour reference i.e. record number into a previously
				imported colour definition which should now be an indexed colour in the document.
	SeeAlso:	ColourRecordHandler; 

********************************************************************************************/

BOOL ColourListComponent::GetDocColour(INT32 ColourRef, DocColour* pDocColour)
{
	ERROR2IF(pDocColour == NULL,FALSE,"ColourListComponent::GetDocColour NULL pCXFileRec");
	ERROR2IF(pNewColours == NULL,FALSE,"ColourListComponent::ImportSimpleColour NULL pNewColours");
//TRACEUSER( "Neville", _T("GetDocColour for ref %d\n"), ColourRef);
	
	// First check to see if the colour refers to any of the built in colours.
	// These will have negative record numbers
	if (ColourRef < 0)
	{
		// At present, we only use the one colour reference
		StockColour defaultcolour = COLOUR_NONE;
		switch (ColourRef)
		{
			case REF_DEFAULTCOLOUR_TRANSPARENT:
				defaultcolour = COLOUR_TRANS;
				break;
			case REF_DEFAULTCOLOUR_BLACK:
				defaultcolour = COLOUR_BLACK;
				break;
			case REF_DEFAULTCOLOUR_WHITE:
				defaultcolour = COLOUR_WHITE;
				break;
			case REF_DEFAULTCOLOUR_RED:
				defaultcolour = COLOUR_RED;
				break;
			case REF_DEFAULTCOLOUR_GREEN:
				defaultcolour = COLOUR_GREEN;
				break;
			case REF_DEFAULTCOLOUR_BLUE:
				defaultcolour = COLOUR_BLUE;
				break;
			case REF_DEFAULTCOLOUR_CYAN:
				defaultcolour = COLOUR_CYAN;
				break;
			case REF_DEFAULTCOLOUR_MAGENTA:
				defaultcolour = COLOUR_MAGENTA;
				break;
			case REF_DEFAULTCOLOUR_YELLOW:
				defaultcolour = COLOUR_YELLOW;
				break;
			default:
			{
				defaultcolour = REPLACEMENTCOLOUR;
				// It is an unknown default colour reference
				// We will warn the user and use a replacement colour instead
				// If we have a pFilter then ask the default warning handle to append our message
				if (pCamFilter)
				{
					if (!WarnedBefore)
					{
						pCamFilter->AppendWarning(_R(IDS_NATIVE_COLOURWARNING));
						WarnedBefore = TRUE;
					}

				}
				else
					ERROR2(FALSE,"ColourListComponent::GetDocColour negative ColourRef is unknown");
				break;
			}
		}
		
		// Make ourselves the required colour
		DocColour * pReplacementCol = new DocColour(defaultcolour);
		// Make the return colour this
		*pDocColour = *pReplacementCol;
		// And now delete that temp replacement colour
		delete pReplacementCol;
	}
	else
	{
		// In case of early exit set the return DocColour pointer to NULL, i.e. nothing found
		//*pDocColour = NULL;

		// Try and find the specified record number in our colour list
		IndexedColour *pCol = pNewColours->GetColour(ColourRef);
		// If not found then this is a problem as a colour must have been defined before its being
		// referenced 
		// We will warn the user and use a replacement colour instead
		if (pCol == NULL)
		{
			// If we have a pFilter then ask the default warning handle to append our message
			if (pCamFilter)
			{
				if (!WarnedBefore)
				{
					pCamFilter->AppendWarning(_R(IDS_NATIVE_COLOURWARNING));
					WarnedBefore = TRUE;
				}

				// Make ourselves a colour to use instead
				DocColour * pReplacementCol =  new DocColour(REPLACEMENTCOLOUR);
				// Make the return colour this
				*pDocColour = *pReplacementCol;
				// And now delete that temp replacement colour
				delete pReplacementCol;
			}
			else
				ERROR2(FALSE,"ColourListComponent::GetDocColour ColourRef cannot be found");
		}
		else
		{		
			// Make the specified DocColour referencing this IndexedColour.
			pDocColour->MakeRefToIndexedColour(pCol);
		}
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL ColourListComponent::ImportSimpleColour(CXaraFileRecord* pCXFileRec)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pCXFileRec - The CXaraFileRecord that is being used to provide read functions etc.
	Returns:	TRUE if the record was succesfully imported;
				FALSE if not (e.g. out of memory)
	Purpose:	Try and import a simple RGB colour definition record into the document.
	SeeAlso:	ColourRecordHandler; 

********************************************************************************************/

BOOL ColourListComponent::ImportSimpleColour(CXaraFileRecord* pCXFileRec)
{
	ERROR2IF(pCXFileRec == NULL,FALSE,"ColourListComponent::ImportSimpleColour NULL pCXFileRec");
	ERROR2IF(pNewColours == NULL,FALSE,"ColourListComponent::ImportSimpleColour NULL pNewColours");

//TRACEUSER( "Neville", _T("ImportSimpleColour\n"));
	// Read in the simple RGB colour
	BYTE Red;
	BYTE Green;
	BYTE Blue;
	BOOL ok = pCXFileRec->ReadBYTE(&Red);
	if (ok) ok = pCXFileRec->ReadBYTE(&Green);
	if (ok) ok = pCXFileRec->ReadBYTE(&Blue);

	NewColourInfo ColourInfo;
	// Flag we are importing a web/native style colour and so use the specified parent and not
	// the context form.
	ColourInfo.RecordNumber = pCXFileRec->GetRecordNumber();
	ColourInfo.WebNativeColour = TRUE;
	ColourRGBT NewRGBT;
	String_64 ColName; //(TEXT("_"));
	// Make the name unique by adding a text form of the record number 
	TCHAR			   *pNewName = (TCHAR *)ColName;
	tsprintf( pNewName, 64, _T("_%d"), ColourInfo.RecordNumber );

	NewRGBT.Red = (double)((double)Red/256.0);	
	NewRGBT.Green = (double)((double)Green/256.0);	
	NewRGBT.Blue = (double)((double)Blue/256.0);	
	if (ok) ok = pNewColours->AddColour(&ColName, &NewRGBT, &ColourInfo);

	return ok;
}

/********************************************************************************************

>	BOOL ColourListComponent::ImportColourDefinition(CXaraFileRecord* pCXFileRec)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		pCXFileRec - The CXaraFileRecord that is being used to provide read functions etc.
	Returns:	TRUE if the record was succesfully imported;
				FALSE if not (e.g. out of memory)
	Purpose:	Try and import a colour definition record into the document.
	SeeAlso:	ColourRecordHandler; 

********************************************************************************************/

BOOL ColourListComponent::ImportColourDefinition(CXaraFileRecord* pCXFileRec)
{
	ERROR2IF(pCXFileRec == NULL,FALSE,"ColourListComponent::ImportColourDefinition NULL pCXFileRec");
	ERROR2IF(pNewColours == NULL,FALSE,"ColourListComponent::ImportColourDefinition NULL pNewColours");
	

	// Check for special types (NB a colour cannot be both a spot colour and a linked colour).
	NewColourInfo ColourInfo;
//	BOOL Linked = FALSE;
	// Flag we are importing a web/native style colour and so use the specified parent and not
	// the context form.
	ColourInfo.RecordNumber = pCXFileRec->GetRecordNumber();
	ColourInfo.WebNativeColour = TRUE;
	//IndexedColour *pParent = NULL;

	// Read in the simple RGB colour, which we won't need
	BYTE Red;
	BYTE Green;
	BYTE Blue;
	BOOL ok = pCXFileRec->ReadBYTE(&Red);
	if (ok) ok = pCXFileRec->ReadBYTE(&Green);
	if (ok) ok = pCXFileRec->ReadBYTE(&Blue);

	// read in the colour model
	ColourModel ColModel = COLOURMODEL_RGBT;
	BYTE temp;
	if (ok) ok = pCXFileRec->ReadBYTE(&temp);
	ColModel = (ColourModel)temp;

	// read in the colour type
	ExportColourType ColType = EXPORT_COLOURTYPE_NORMAL;
	if (ok) ok = pCXFileRec->ReadBYTE(&temp);
	ColType = (ExportColourType)temp;

	// read in the entry number that this colour should be in the list of colours
	UINT32 EntryNumber = 0;
	if (ok) ok = pCXFileRec->ReadUINT32(&EntryNumber);
	ColourInfo.EntryNumber = EntryNumber;

	// read in the record number of the parent colour
	UINT32 ParentColour = 0;
	if (ok) ok = pCXFileRec->ReadUINT32(&ParentColour);
	// Now convert this into a indexed colour pointer, if required
	if (ok && (ColType == EXPORT_COLOURTYPE_LINKED || ColType == EXPORT_COLOURTYPE_TINT ||
		ColType == EXPORT_COLOURTYPE_SHADE))
	{
		TRACEUSER( "Neville", _T("Trying to find parent reference %d\n"),ParentColour);
		ColourInfo.pParentCol = pNewColours->GetColour(ParentColour);
		if (ColourInfo.pParentCol == NULL)
		{
			TRACEUSER( "Neville", _T("Cannot convert parent colour reference %d into an IndexedColour\n"),ParentColour);
			ERROR2(FALSE,"Cannot find parent IndexedColour");
		}
	}

	// Read in the four colour components
	UINT32 Comp1 = 0;
	if (ok) ok = pCXFileRec->ReadUINT32(&Comp1);
	UINT32 Comp2 = 0;
	if (ok) ok = pCXFileRec->ReadUINT32(&Comp2);
	UINT32 Comp3 = 0;
	if (ok) ok = pCXFileRec->ReadUINT32(&Comp3);
	UINT32 Comp4 = 0;
	if (ok) ok = pCXFileRec->ReadUINT32(&Comp4);
	
	String_64 ColName; 
	if (ok) ok = pCXFileRec->ReadUnicode(&ColName);//, ColName.MaxLength());
	
	if (ColName.Length() == 0 )
	{	
		// We are an unnamed colour and so make up a name based on the record number
		// This is so that all the existing import code can be reused.
		// Make the name unique by adding a text form of the record number 
		TCHAR		   *pNewName = (TCHAR *)ColName;
		tsprintf( pNewName, 64, _T("_%d"), ColourInfo.RecordNumber );
	}
TRACEUSER( "Neville", _T("ImportColourDefinition Name %s RecordNumber %d, EntryNumber %d\n"),(TCHAR*)ColName,ColourInfo.RecordNumber,EntryNumber);
	
	// We had a problem reading the colour definition so stop now
	if (!ok)
		return FALSE;

	// Read the colour components according to the colour model.
	switch (ColType)
	{
		case EXPORT_COLOURTYPE_NORMAL:
				// Set the correct colour type
				ColourInfo.Type = COLOURTYPE_NORMAL;
				// Fall through below
		case EXPORT_COLOURTYPE_SPOT:
		case EXPORT_COLOURTYPE_LINKED:
			// Set the correct colour type
			if (ColType == EXPORT_COLOURTYPE_SPOT)
				ColourInfo.Type = COLOURTYPE_SPOT;
			else if (ColType == EXPORT_COLOURTYPE_LINKED)
				ColourInfo.Type = COLOURTYPE_LINKED;

			// These all require the same handling
			// Linked colours save out the components in the normal fashion
			switch (ColModel)
			{
				case COLOURMODEL_RGBT:
					{
						ColourRGBT NewRGBT;
						ReadRGB(&NewRGBT, &ColourInfo, Comp1, Comp2, Comp3, Comp4);
						if (!pNewColours->AddColour(&ColName, &NewRGBT, &ColourInfo))
							return FALSE;
					}
					break;

				case COLOURMODEL_CMYK:
					{
						ColourCMYK NewCMYK;
						ReadCMYK(&NewCMYK, &ColourInfo, Comp1, Comp2, Comp3, Comp4);
						if (!pNewColours->AddColour(&ColName, &NewCMYK, &ColourInfo))
							return FALSE;
					}
					break;

				case COLOURMODEL_HSVT:
					{
						ColourHSVT NewHSVT;
						ReadHSV(&NewHSVT, &ColourInfo, Comp1, Comp2, Comp3, Comp4);
						if (!pNewColours->AddColour(&ColName, &NewHSVT, &ColourInfo))
							return FALSE;
					}
					break;

				case COLOURMODEL_GREYT:
					{
						ColourGreyT NewGreyT;
						ReadGrey(&NewGreyT, &ColourInfo, Comp1);
						if (!pNewColours->AddColour(&ColName, &NewGreyT, &ColourInfo))
							return FALSE;
					}
					break;
				
					default:
						ERROR3("ColourListComponent::ImportColourDefinition unknown colour model!");
						break;
			}
			break;

		case EXPORT_COLOURTYPE_TINT:
			{
				// Set the correct colour type
				ColourInfo.Type = COLOURTYPE_TINT;

				// Read in the tint value from the colour definition record
				ReadTint(&ColourInfo, Comp1);

				// Now, try and add that to the system
				if (!pNewColours->AddTintOrShade(&ColName, &ColourInfo))
					return FALSE;
			}
			break;

		case EXPORT_COLOURTYPE_SHADE:
			{
				// Set the correct colour type
				ColourInfo.Type = COLOURTYPE_TINT;

				// Read in the shade value from the colour definition record
				ReadShade(&ColourInfo, Comp1, Comp2);

				// Now, try and add that to the system
				if (!pNewColours->AddTintOrShade(&ColName, &ColourInfo))
					return FALSE;
			}
			break;

		default:
			ERROR3("ColourListComponent::ImportColourDefinition unknown colourtype!");
			break;
	}

	return ok;
}

/********************************************************************************************

>	BOOL ColourListComponent::ReadRGB(ColourRGBT *pCol, NewColourInfo *pColourInfo,
									  UINT32 Comp1, UINT32 Comp2, UINT32 Comp3, UINT32 Comp4)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		pCol		- RGB colour data to fill in
				pColourInfo	- pointer to the colour info to fill in
				Comp1 .. 4	- colour components read in from the record 
	Outputs:	The data in the pCol and pColourInfo will have been filled in
	Returns:	True if value read ok, False otherwise.
	Purpose:	Read the RGB values from a Camelot RGB colour definition record.
	SeeAlso:	ColourListComponent::ImportColourDefinition;

********************************************************************************************/

BOOL ColourListComponent::ReadRGB(ColourRGBT *pCol, NewColourInfo *pColourInfo,
								  UINT32 Comp1, UINT32 Comp2, UINT32 Comp3, UINT32 Comp4)
{
	ERROR2IF(pCol == NULL,FALSE,"ColourListComponent::ReadRGB NULL pCol");
	ERROR2IF(pColourInfo == NULL,FALSE,"ColourListComponent::ReadRGB NULL pColourInfo");

	BOOL Linked = (pColourInfo != NULL) && (pColourInfo->Type == COLOURTYPE_LINKED);

	// read the Red colour component in
	FIXED24 Value24 = 0.0;
	Value24.SetAsFixed24(Comp1);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[0] = TRUE;
	else
		pCol->Red = Value24;

	// read the Green colour component in
	Value24.SetAsFixed24(Comp2);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[1] = TRUE;
	else
		pCol->Green = Value24;

	// read the Blue colour component in
	Value24.SetAsFixed24(Comp3);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[2] = TRUE;
	else
		pCol->Blue = Value24;

	// No transparency
	pCol->Transparent = 0.0;

	return TRUE;
}

/********************************************************************************************

>	BOOL ColourListComponent::ReadCMYK(ColourCMYK *pCol, NewColourInfo *pColourInfo,
									   UINT32 Comp1, UINT32 Comp2, UINT32 Comp3, UINT32 Comp4)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		pCol	- RGB colour data to fill in
				pColourInfo	- pointer to the colour info to fill in				
				Comp1 .. 4	- colour components read in from the record 
	Outputs:	The data in the pCol and pColourInfo will have been filled in
	Returns:	True if value read ok, False otherwise.
	Purpose:	Read the CMYK values from a Camelot CMYK colour definition record.
	SeeAlso:	ColourListComponent::ImportColourDefinition;

********************************************************************************************/

BOOL ColourListComponent::ReadCMYK(ColourCMYK *pCol, NewColourInfo *pColourInfo,
								   UINT32 Comp1, UINT32 Comp2, UINT32 Comp3, UINT32 Comp4)
{
	ERROR2IF(pCol == NULL,FALSE,"ColourListComponent::ReadCMYK NULL pCol");
	ERROR2IF(pColourInfo == NULL,FALSE,"ColourListComponent::ReadCMYK NULL pColourInfo");

	BOOL Linked = (pColourInfo != NULL) && (pColourInfo->Type == COLOURTYPE_LINKED);

	// read the Cyan colour component in
	FIXED24 Value24 = 0.0;
	Value24.SetAsFixed24(Comp1);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[0] = TRUE;
	else
		pCol->Cyan = Value24;

	// read the Magenta colour component in
	Value24.SetAsFixed24(Comp2);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[1] = TRUE;
	else
		pCol->Magenta = Value24;

	// read the Yellow colour component in
	Value24.SetAsFixed24(Comp3);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[2] = TRUE;
	else
		pCol->Yellow = Value24;

	// read the Key colour component in
	Value24.SetAsFixed24(Comp4);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[3] = TRUE;
	else
		pCol->Key = Value24;

	return TRUE;
}

/********************************************************************************************

>	BOOL ColourListComponent::ReadHSV(ColourHSVT *pCol, NewColourInfo *pColourInfo,
									  UINT32 Comp1, UINT32 Comp2, UINT32 Comp3, UINT32 Comp4)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		pCol	- HSVT colour data to fill in
				pColourInfo	- pointer to the colour info to fill in				
				Comp1 .. 4	- colour components read in from the record 
	Outputs:	The data in the pCol and pColourInfo will have been filled in
	Returns:	True if value read ok, False otherwise.
	Purpose:	Read the HSV values from a Camelot HSV colour definition record.
	SeeAlso:	ColourListComponent::ImportColourDefinition;

********************************************************************************************/

BOOL ColourListComponent::ReadHSV(ColourHSVT *pCol, NewColourInfo *pColourInfo,
								  UINT32 Comp1, UINT32 Comp2, UINT32 Comp3, UINT32 Comp4)
{
	ERROR2IF(pCol == NULL,FALSE,"ColourListComponent::ReadHSV NULL pCol");
	ERROR2IF(pColourInfo == NULL,FALSE,"ColourListComponent::ReadHSV NULL pColourInfo");

	BOOL Linked = (pColourInfo != NULL) && (pColourInfo->Type == COLOURTYPE_LINKED);

	// read the Hue colour component in
	FIXED24 Value24 = 0.0;
	Value24.SetAsFixed24(Comp1);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[0] = TRUE;
	else
		pCol->Hue = Value24;

	// read the Saturation colour component in
	Value24.SetAsFixed24(Comp2);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[1] = TRUE;
	else
		pCol->Saturation = Value24;

	// read the Value colour component in
	Value24.SetAsFixed24(Comp3);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[2] = TRUE;
	else
		pCol->Value = Value24;
	
	// No transparency
	pCol->Transparent = 0.0;

	return TRUE;
}

/********************************************************************************************

>	BOOL ColourListComponent::ReadGrey(ColourGreyT *pCol, NewColourInfo *pColourInfo, UINT32 Comp1)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		pCol	- Grey colour data to fill in
				pColourInfo	- pointer to the colour info to fill in				
				Comp1	- the first of the colour components read in from the record 
	Outputs:	The data in the pCol and pColourInfo will have been filled in
	Returns:	True if value read ok, False otherwise.
	Purpose:	Read the Grey values from a Camelot Grey colour definition record.
	SeeAlso:	ColourListComponent::ImportColourDefinition;

********************************************************************************************/

BOOL ColourListComponent::ReadGrey(ColourGreyT *pCol, NewColourInfo *pColourInfo, UINT32 Comp1)
{
	ERROR2IF(pCol == NULL,FALSE,"ColourListComponent::ReadGrey NULL pCol");
	ERROR2IF(pColourInfo == NULL,FALSE,"ColourListComponent::ReadGrey NULL pColourInfo");

	BOOL Linked = (pColourInfo != NULL) && (pColourInfo->Type == COLOURTYPE_LINKED);

	// read the grey Intensity colour component in
	FIXED24 Value24 = 0.0;
	Value24.SetAsFixed24(Comp1);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[0] = TRUE;
	else
		pCol->Intensity = Value24;

	// Clear the reserved words to 0.
	pCol->Reserved1 = 0.0;
	pCol->Reserved2 = 0.0;

	// No transparency
	pCol->Transparent = 0.0;

	return TRUE;
}

/********************************************************************************************

>	BOOL ColourListComponent::ReadTint(NewColourInfo *pColourInfo, UINT32 Comp1)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		pColourInfo	- pointer to the colour info to fill in				
				Comp1		- the first of the colour components read in from the record 
	Outputs:	The data in the pCol and pColourInfo will have been filled in
	Returns:	True if value read ok, False otherwise.
	Purpose:	Read the tint values from a Camelot tint colour definition record.
	SeeAlso:	ColourListComponent::ImportColourDefinition;

********************************************************************************************/

BOOL ColourListComponent::ReadTint(NewColourInfo *pColourInfo, UINT32 Comp1)
{
	ERROR2IF(pColourInfo == NULL,FALSE,"ColourListComponent::ReadTint NULL pColourInfo");

	BOOL Linked = (pColourInfo != NULL) && (pColourInfo->Type == COLOURTYPE_LINKED);

	// read the grey Intensity colour component in
	FIXED24 Value24 = 0.0;
	Value24.SetAsFixed24(Comp1);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[0] = TRUE;

	pColourInfo->TintValue = Value24;

	pColourInfo->Type = COLOURTYPE_TINT;
	// And flag the fact that this "tint" is a tint rather than a shade!
	pColourInfo->TintIsShade = FALSE;

	return TRUE;
}

/********************************************************************************************

>	BOOL ColourListComponent::ReadShade(NewColourInfo *pColourInfo, UINT32 Comp1, UINT32 Comp2)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		pColourInfo	- pointer to the colour info to fill in				
				Comp1 .. 2	- the first two colour components read in from the record 
	Outputs:	The data in the pCol and pColourInfo will have been filled in
	Returns:	True if value read ok, False otherwise.
	Purpose:	Read the shade values from a Camelot shade colour definition record.
	SeeAlso:	ColourListComponent::ImportColourDefinition;

********************************************************************************************/

BOOL ColourListComponent::ReadShade(NewColourInfo *pColourInfo, UINT32 Comp1, UINT32 Comp2)
{
	ERROR2IF(pColourInfo == NULL,FALSE,"ColourListComponent::ReadShade NULL pColourInfo");

	BOOL Linked = (pColourInfo != NULL) && (pColourInfo->Type == COLOURTYPE_LINKED);

	FIXED24 Value24 = 0.0;
	Value24.SetAsFixed24(Comp1);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[0] = TRUE;
	pColourInfo->TintValue = Value24;

	Value24.SetAsFixed24(Comp2);
	if (Linked && (Value24 == LinkValue))
		pColourInfo->Inherits[1] = TRUE;
	pColourInfo->ShadeValue = Value24;

	pColourInfo->Type = COLOURTYPE_TINT;
	// And flag the fact that this "tint" is a really a shade!
	pColourInfo->TintIsShade = TRUE;

	return TRUE;
}


/********************************************************************************************
********************************************************************************************/


/********************************************************************************************

>	BOOL ColourListComponent::StartExport(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Returns:	TRUE if the component was able to prepare for exporting;
				FALSE if not (e.g. out of memory)
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
	Purpose:	Inform the colour list document component that a WEb or Native export is
				about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL ColourListComponent::StartExport(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
TRACEUSER( "Neville", _T("ColourListComponent::StartExport\n"));
	// Mark all colours as unsaved 
	if (pFilter == NULL)
	{
		ERROR3("ColourListComponent::StartExport filter is null!");
		return TRUE;
	}

	// Save this in our link to the BaseCamelotFilter
	pCamFilter = pFilter;

	// Set up and start the exported colour list
	pExportedColours = new ExportedColours();
	if (pExportedColours == NULL)
		return FALSE;
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL ColourListComponent::EndExport(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - True if everything went swimmingly, False if just a clean up is required.
	Purpose:	Inform the colour list document component that a Web or Native export has
				just finished.
	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL ColourListComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)
{
#ifdef DO_EXPORT
//	ERROR2IF(pExportedColours == NULL, FALSE, "ColourListComponent::EndExport no pExportedColours!");
	// If we error about the pExportedColours then we assume that the EndImport has been called.
	// This may not be the case if we are in clean up mode. So just handle it!

TRACEUSER( "Neville", _T("ColourListComponent::EndExport\n"));
	if (pFilter == NULL)
	{
		ERROR3("ColourListComponent::EndExport filter is null!");
		return TRUE;
	}

	// Check if we should be saving unused colours or not. Usually this is controlled by the
	// user in Web files and always done in Native files.
	// Do not do if we are just in clean up mode or have no pExportedColours (indicating in clean up mode)
	if (!pFilter->IsWebFilter() && !pFilter->GetRemoveUnusedColours() && Success && pExportedColours)
	{
TRACEUSER( "Neville", _T("ColourListComponent::EndExport exporting unsaved colours\n"));
		// Iterate over the document's list of named IndexedColours...
		IndexedColour *pCol = (IndexedColour *) pIndexedColours->GetHead();
		BOOL ok = TRUE;
		while (ok && pCol != NULL)
		{
			if (!pCol->IsDeleted())
			{
				// The colour is in use - save it to the file.
				ok = SaveColourAndParents(pCol, pFilter, 0);
			}

			// Try the next colour
			pCol = (IndexedColour *) pIndexedColours->GetNext(pCol);
		}

		// Iterate over the document's list of unnamed IndexedColours...
		List *pUnnamedColours = pIndexedColours->GetUnnamedColours();

		pCol = (IndexedColour *) pUnnamedColours->GetHead();

		ok = TRUE;
		while (ok && pCol != NULL)
		{
			if (!pCol->IsDeleted() && pCol->IsInUse())
			{
				// The colour is in use and is not deleted
				// - save it to the file.
				ok = SaveColourAndParents(pCol, pFilter, 0);
			}

			// Try the next colour
			pCol = (IndexedColour *) pUnnamedColours->GetNext(pCol);
		}
	}

#endif		
	return TRUE;
}



/********************************************************************************************

>	virtual void ColourListComponent::CleanUpAfterExport(BOOL Success)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/8/96
	Inputs:		Success - True if everything went swimmingly, False if the export failed

	Purpose:	Inform the document component that a Web or Native export has just finished.

	Notes:		This is called after EndExport, so that the component can clean up any 
				during-export data structures. This allows situations like: PrintComponent
				wants to save a colour reference in it's EndExport call, but is called after
				the ColourComponent EndExport. The ColourComponent EndExport does not delete
				its data structures, so is still able to supply colour references to other
				components' EndExport functions.

	SeeAlso:	ColourListComponent::EndExport; DocComponent::CleanUpAfterExport

********************************************************************************************/

void ColourListComponent::CleanUpAfterExport(BOOL Success)
{
#ifdef DO_EXPORT
	// As we have finished with it now, destroy the list of exported colours
	if (pExportedColours)
	{
		delete pExportedColours;
		pExportedColours = NULL;
	}

	// Null our link to the BaseCamelotFilter
	pCamFilter = NULL;

#endif		
	return;
}



/********************************************************************************************

>	INT32 ColourListComponent::GetWriteColourReference(DocColour* pDocColour, BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/96
	Inputs:		pDocCololour	- The document colour to save
				pFilter			- The filter to use for saving
	Outputs:	-
	Returns:	The colour reference for the doccolour ready for writing in a record.
				This will be zero if a problem has occurred. May return -1 if it does not
				need to save anything i.e if the colour is not in use and not named.

	Purpose:	Return the colour reference to the caller, who will then use it to write the
				colour in the record that they are preparing to save to a Web or Native file.
				In the process of getting the colour reference, the colour definition record
				for the colour reference will be generated and saved to the file. In Web format
				this will just be the simple colour record, in native files this will be the
				full colour definition, possibly preceeded by the parents for the colour, as
				these must be saved before the children as the children must have a valid
				colour reference.
				NOTE: As this does save information into the record stream, it *MUST* be called
				before the caller has called StartRecord() as otherwise the records will become
				intertwined!
	Errors:		-
	SeeAlso:	IndexedColour; SaveComplexColour; SaveSimpleColour; SaveColourAndParents;

********************************************************************************************/

INT32 ColourListComponent::GetWriteColourReference(DocColour* pDocColour, BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pDocColour == NULL,0L,"ColourListComponent::GetWriteColourReference null pDocColour");
	ERROR2IF(pFilter == NULL,0L,"ColourListComponent::GetWriteColourReference null pFilter");

	INT32 RecordNumber = 0L;
	
	// First, check to see if the doc colour is transparent. If so then just return the default
	// default reference for this
	if (pDocColour->IsTransparent())
		return REF_DEFAULTCOLOUR_TRANSPARENT;
	
	// Get the indexed colour which this DocColour refers to, if any.
	IndexedColour * pCol = pDocColour->FindParentIndexedColour();
	if (pCol)
	{
		// Its an indexed colour so either save it as a simple colour or a complex colour
		// depending on whether we are in Web or Native mode.
		if (pFilter->IsWebFilter())
		{
			// We are in Web mode so save the simple colour
			RecordNumber = SaveSimpleColour(pCol, pFilter);
		}
		else
		{
			// We are in Web mode so save the simple colour
			RecordNumber = SaveColourAndParents(pCol, pFilter);
		}
	}
	else
	{
		// its a simple colour, so save this out as a simple colour. 
		if (pFilter->IsWebFilter())
		{
			// its a simple colour, so just save this as a simple colour. 
			// We must add the RGB colours to our exported list, so that we only save the
			// definition out once. 
			RecordNumber = SaveSimpleColour(pDocColour, pFilter, TRUE);
		}
		else
		{
			// TODO:
			// Could change this later to save out a complex colour in native files so that
			// these are converted to indexed colours on loading. Jason reckons this is favourable.
			// If we save it as a simple colour then there are bound to be conversion errors as
			// we are going from 24 bit components down to 8 bits and then back.

			// Find a global colour context to convert this colour into RGB for us.
			ColourContextRGBT *cc = (ColourContextRGBT *)ColourManager::GetColourContext(COLOURMODEL_RGBT);
			ERROR3IF(cc == NULL, "ColourListComponent::GetWriteColourReference No global RGB colour context!? Something is very broken");
			if (cc == NULL)
				return 0L;
			
			// And convert the colour into a *packed* (8-bit components) colour structure
			PColourRGBT Result;
			cc->ConvertColour(pDocColour, (ColourPacked *)&Result);

			// Only write this record out if we have not done so already
			// Check by seeing if the colour is already in our exported colour list
			ExportColour* pExportCol = NULL;
			pExportCol = pExportedColours->GetColour(&Result);
			// If it is then do not save this colour
			if (pExportCol)
			{
				// Just return the record number as the colour reference to the caller
				RecordNumber = pExportCol->RecordNumber;
			}
			else
			{	
				// Create ourselves a new indexed colour to put our data in
				// Use the DocColour we have been supplied with, this should create an unnamed colour by default
				IndexedColour *pNewCol = new IndexedColour(*pDocColour);
				if (pNewCol)
				{
					// DY 3/5/2000 Now that I am now using the Reserved flag to indicate whether or not a doccolour
					// was created from a named colour and then de-indexed. For details see LineDefinition::ConvertIndexedColours
					// This means that if we get a doccolour with that flag set it was originally created by an indexed colour, so
					// we can call our function to find the parent
	
					
					IndexedColour* pIndexedCol = NULL;
					if (pDocColour->IsNamed())
					{
						// Diccon, this is a named colour that was originally generated from an indexed colour but
						// subsequently de-indexed for various reasons. Its original parent should be in the list
						// somewhere
						pIndexedCol = GetIndexedParentOfColour(*pDocColour);
					
					}
					else
						pNewCol->SetUnnamed();
		
					// Now save out the colour data in a complex colour record out to file
					// We cannot ask the routine to add this colour to the exported list as it is
					// not a REAL indexed colour in the indexed colour list, so say False.
					if (pIndexedCol == NULL)
						RecordNumber = SaveComplexColour(pNewCol, pFilter, FALSE);
					else
					// if we found an indexed colour then save out normally - DY
						RecordNumber = SaveColourAndParents(pIndexedCol, pFilter);	

					if (RecordNumber > 0)
					{
						// Add this colour to our list of exported items
						BOOL ok = pExportedColours->AddColour(&Result, RecordNumber);
						// Should return this error to the caller but not fatal so just continue
					}

					// We have finished with our indexed colour and so remove it
					delete pNewCol;
				}
				else
				{
					// We failed to create the complex colour so we have no choice but to save
					// it as a simple colour. 
					// We must add the RGB colours to our exported list, so that we only save the
					// definition out once. 
					RecordNumber = SaveSimpleColour(pDocColour, pFilter, TRUE);
				}
			}
		}
	}
	
	return RecordNumber;
#endif
	return 0L;
}

/********************************************************************************************

>	INT32 ColourListComponent::SaveSimpleColour(IndexedColour *pCol, BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		pCol		- The (root) colour to save
				pFilter		- The filter to use for saving
	Outputs:	-
	Returns:	The record number for this colour, if it has been saved correctly or zero
				if a problem has occurred. May return -1 if it does not need to save anything
				i.e if the colour is not in use and not named.
	Purpose:	Save out an indexed colour to a Web or Native file. Returns a record number or
				colour reference to the caller ready for use by somebody else to save in their
				record.
				This is the Web file form of saving colours.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 ColourListComponent::SaveSimpleColour(IndexedColour *pCol, BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pCol == NULL || pFilter == NULL, 0L, "ColourListComponent::SaveSimpleColour no pCol or pFilter supplied!");
	ERROR2IF(pExportedColours == NULL, 0L, "ColourListComponent::SaveSimpleColour no pExportedColours!");
	
	// Use same condition as SaveColourAndChildren so that if the colour is unnamed
	// and it's not actually used anywhere, don't bother saving it.
	// This optimises out temporary locals that are no longer needed. At the worst,
	// if something goes horribly wrong, this just means that some locals might turn up
	// as CMYK definitions on reloading, but this shouldn't cause any problems unless
	// something deeply scary has happened to the colour system.
	if (!pCol->IsNamed() && !pCol->IsInUse())
		return -1L;

	// Only write this record out if we have not done so already
	// Check by seeing if the colour is already in our exported colour list
	ExportColour* pExportCol = NULL;
	pExportCol = pExportedColours->GetColour(pCol);
	// If it is then do not save this colour
	if (pExportCol)
	{
		// Just return the record number as the colour reference to the caller
		return pExportCol->RecordNumber;
	}	

	// Create a DocColour referencing this IndexedColour.
	DocColour SimpleColour;
	SimpleColour.MakeRefToIndexedColour(pCol);

	// Save this as a document colour, don't check the export colour list as we are an indexed colour
	// really and so have cope with the list ourselves.
	INT32 RecordNumber = SaveSimpleColour(&SimpleColour, pFilter, FALSE);
	// returns 0 if there was a problem, exit now
	if (RecordNumber == 0)
		return RecordNumber;

	// Add this colour to our list of exported items
	BOOL ok = pExportedColours->AddColour(pCol, RecordNumber);
	// If we have had a problem at any of the stages then return that to the caller
	if (!ok)
		return 0L;

	// Everything went tickety boo, so return the record number to the caller.
	return RecordNumber;
#else
	return 0L;
#endif
}

/********************************************************************************************

>	INT32 ColourListComponent::SaveSimpleColour(DocColour *pDocCol, BaseCamelotFilter *pFilter,
											   BOOL CheckSimpleColourList = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		pDocCol		- The document colour to save
				pFilter		- The filter to use for saving
				CheckSimpleColourList - If TRUE then check the export colour list to see if we
										have saved this simple colour before
										If FALSE then this is an indexed colour so don't search
										the export colour list
	Outputs:	-
	Returns:	The record number for this colour, if it has been saved correctly or zero
				if a problem has occurred. May return -1 if it does not need to save anything
				i.e if the colour is not in use and not named.
	Purpose:	Save out a document colour to a Web or Native file. Returns a record number or
				colour reference to the caller ready for use by somebody else to save in their
				record.
				This is the Web file form of saving colours.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 ColourListComponent::SaveSimpleColour(DocColour *pDocCol, BaseCamelotFilter *pFilter,
										   BOOL CheckSimpleColourList)
{
#ifdef DO_EXPORT
	ERROR2IF(pDocCol == NULL || pFilter == NULL, 0L, "ColourListComponent::SaveSimpleColour no pCol or pFilter supplied!");

	// Find a global colour context to convert this colour into RGB for us.
	ColourContextRGBT *cc = (ColourContextRGBT *)ColourManager::GetColourContext(COLOURMODEL_RGBT);
	ERROR3IF(cc == NULL, "ColourListComponent::SaveSimpleColour No global RGB colour context!? Something is very broken");
	if (cc == NULL)
		return 0L;
	
	// And convert the colour into a *packed* (8-bit components) colour structure
	PColourRGBT Result;
	cc->ConvertColour(pDocCol, (ColourPacked *)&Result);

	// If this is a simple colour rather than a simple colour of an indexed colour then
	// check to see if we have saved it already. If so then just return the record number
	if (CheckSimpleColourList)
	{
		// Only write this record out if we have not done so already
		// Check by seeing if the colour is already in our exported colour list
		ExportColour* pExportCol = NULL;
		pExportCol = pExportedColours->GetColour(&Result);
		// If it is then do not save this colour
		if (pExportCol)
		{
			// Just return the record number as the colour reference to the caller
			return pExportCol->RecordNumber;
		}	
	}

	// And finally, read the bytes out of the packed colour struct.
	BYTE RedByteValue = Result.Red;
	BYTE GreenByteValue = Result.Green;
	BYTE BlueByteValue = Result.Blue;

	INT32 RecordNumber = 0L;
	if (RedByteValue == 0xFF && GreenByteValue == 0xFF && BlueByteValue == 0xFF)
	{
		RecordNumber = REF_DEFAULTCOLOUR_WHITE;
	}
	else if (RedByteValue == 0x00 && GreenByteValue == 0x00 && BlueByteValue == 0x00)
	{
		RecordNumber = REF_DEFAULTCOLOUR_BLACK;
	}
	else if (RedByteValue == 0xFF && GreenByteValue == 0x00 && BlueByteValue == 0x00)
	{
		RecordNumber = REF_DEFAULTCOLOUR_RED;
	}
	else if (RedByteValue == 0x00 && GreenByteValue == 0xFF && BlueByteValue == 0x00)
	{
		RecordNumber = REF_DEFAULTCOLOUR_GREEN;
	}
	else if (RedByteValue == 0x00 && GreenByteValue == 0x00 && BlueByteValue == 0xFF)
	{
		RecordNumber = REF_DEFAULTCOLOUR_BLUE;
	}
	else if (RedByteValue == 0x00 && GreenByteValue == 0xFF && BlueByteValue == 0xFF)
	{
		RecordNumber = REF_DEFAULTCOLOUR_CYAN;
	}
	else if (RedByteValue == 0xFF && GreenByteValue == 0x00 && BlueByteValue == 0xFF)
	{
		RecordNumber = REF_DEFAULTCOLOUR_MAGENTA;
	}
	else if (RedByteValue == 0xFF && GreenByteValue == 0xFF && BlueByteValue == 0x00)
	{
		RecordNumber = REF_DEFAULTCOLOUR_YELLOW;
	}
	else
	{
		BOOL ok = TRUE;
		// Write the data out to the file
		//INT32 RecordNumber = pFilter->StartRecord(TAG_DEFINERGBCOLOUR, TAG_DEFINERGBCOLOUR_SIZE);
		CXaraFileRecord Rec(TAG_DEFINERGBCOLOUR,TAG_DEFINERGBCOLOUR_SIZE);
		ok = Rec.Init();

		if (ok) ok = Rec.WriteBYTE(RedByteValue);
		if (ok) ok = Rec.WriteBYTE(GreenByteValue);
		if (ok) ok = Rec.WriteBYTE(BlueByteValue);

		// Finally, write the record out to file
		// In the process get the record number that this was written out as
		if (ok) RecordNumber = pFilter->WriteDefinitionRecord(&Rec);
		
		// If we have had a problem at any of the stages then return that to the caller
		if (!ok || RecordNumber <= 0)
			return 0L;
	}

	// If this is a simple colour rather than a simple colour of an indexed colour then
	// add it to the list so that we do not save out the definition twice!
	if (CheckSimpleColourList)
	{
		// Add this colour to our list of exported items
		BOOL ok = pExportedColours->AddColour(&Result, RecordNumber);
		// If we have had a problem at any of the stages then return that to the caller
		if (!ok)
			return 0L;
	}

	// Everything went tickety boo, so return the record number to the caller.
	return RecordNumber;
#else
	return 0L;
#endif
}

// Simple macro to make the code in SaveColourAndParents() a bit neater.
#define LINKED_COLOUR(ID) (Linked && pCol->InheritsComponent((ID)))

/********************************************************************************************

>	INT32 ColourListComponent::SaveColourAndParents(IndexedColour *pCol, BaseCamelotFilter *pFilter,
													INT32 NestingLevel = 0)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		pCol			- The (root) colour to save
				pFilter			- The filter to use for saving
				NestingLevel	- How many parental levels up we have gone so far
	Outputs:	-
	Returns:	The record number for this colour, if it has been saved correctly or zero
				if a problem has occurred. May return -1 if it does not need to save anything
				i.e if the colour is not in use and not named.
	Purpose:	Save out an indexed colour to a Web or Native file. Returns a record number or
				colour reference to the caller ready for use by somebody else to save in their
				record.
				This is the Web file form of saving colours.
	Purpose:	Save out an indexed colour in its complex form usually to a Native file.
				If it has any parents then these must be saved out before it, as the colour
				reference for the parent *MUST* be a real record number which has been 
				previously saved into the stream.
				Returns a record number or colour reference to the caller ready for use by
				somebody else to save in their record.
				This is the Native file form of saving colours.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 ColourListComponent::SaveColourAndParents(IndexedColour *pCol, BaseCamelotFilter *pFilter,
												INT32 NestingLevel)
{
#ifdef DO_EXPORT
	ERROR2IF(pCol == NULL || pFilter == NULL, 0L, "ColourListComponent::SaveColourAndParents no pCol or pFilter!");
	ERROR2IF(pExportedColours == NULL, 0L, "ColourListComponent::SaveColourAndParents no pExportedColours!");
	
	// Use same condition as SaveColourAndChildren so that if the colour is unnamed
	// and it's not actually used anywhere, don't bother saving it.
	// This optimises out temporary locals that are no longer needed. At the worst,
	// if something goes horribly wrong, this just means that some locals might turn up
	// as CMYK definitions on reloading, but this shouldn't cause any problems unless
	// something deeply scary has happened to the colour system.
	if (!pCol->IsNamed() && !pCol->IsInUse())
		return -1L;

	// Check for silly recursion errors	[not that you can get circular references with
	// the amazingly strict and careful checking done by the IndexedColour class.
	// Same test as SaveColourAndChildren.
	ERROR2IF(NestingLevel > 200, 0L, "ColourListComponent::SaveColourAndParents - Rampant linked colour nesting detected!");

	// ****
	// **** First, save parents of this colour, if any and if not saved previously
	// ****

	// Before we save out this colour, we must check if it has a parent, if so then
	// we must save out the parents first as we must reference parents by a record
	// number and hence it must have already been saved!

	// So, recurse and save out any parents of this colour (links or tints)...

	// Get all the named colours that are immediate children of this colour
	// This only returns a parent if this is a colour with a valid parent e.g. tint
	IndexedColour *pParent = pCol->FindLinkedParent();
	//IndexedColour *pParentCol = pParent;
	ExportColour* pExportCol = NULL;
	while (pParent != NULL)
	{
		if (!pParent->IsDeleted())
		{
			// Found a parent - save out via recursion so we save all its ascendants in the
			// right order...
			// To save time, check here if this parent is already in our list, if it this
			// then no point in recursing through the chain of parents as they all have already
			// been saved.
			pExportCol = pExportedColours->GetColour(pParent);
			// If it is NULL then we have not already saved this colour, so do so.
			if (pExportCol == NULL)
				SaveColourAndParents(pParent, pFilter, NestingLevel + 1);
		}

		// Try the next colour
		pParent = pParent->FindLinkedParent();
	}

	// ****
	// **** Saved this colour before?
	// ****

	// Only write this record out if we have not done so already
	// Check by seeing if the colour is already in our exported colour list
	pExportCol = pExportedColours->GetColour(pCol);
	// If it is then do not save this colour
	if (pExportCol)
	{
		// Just return the record number as the colour reference to the caller
		return pExportCol->RecordNumber;
	}	

	// Now save out the colour data in a complex colour record out to file
	// We want this indexed colour adding to the exported list and so say TRUE
	INT32 RecordNumber = SaveComplexColour(pCol, pFilter, TRUE);

	// Everything went tickety boo, so return the record number to the caller.
	return RecordNumber;
#else
	return 0L;
#endif
}

/********************************************************************************************

>	INT32 ColourListComponent::SaveComplexColour(IndexedColour *pCol, BaseCamelotFilter *pFilter,
												BOOL AddColourToList = TRUE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/96
	Inputs:		pCol			- The (root) colour to save
				pFilter			- The filter to use for saving
				AddColourToList	- True if we are to add this colour to the exported colour list
								- False if the caller has already done this
	Outputs:	-
	Returns:	The record number for this colour, if it has been saved correctly or zero
				if a problem has occurred. May return -1 if it does not need to save anything
				i.e if the colour is not in use and not named.
	Purpose:	Actually save out an indexed colour to a Web or Native file in a complex colour
				record. Returns a record number or colour reference to the caller ready for use
				by somebody else to save in their record.
				If it has any parents then these must be saved out before it, as the colour
				reference for the parent *MUST* be a real record number which has been 
				previously saved into the stream.
				Returns a record number or colour reference to the caller ready for use by
				somebody else to save in their record.
				This is the Native file form of saving colours.
	Errors:		-
	SeeAlso:	SaveColourAndParents; GetWriteColourReference;

********************************************************************************************/
INT32 ColourListComponent::SaveComplexColour(IndexedColour *pCol, BaseCamelotFilter *pFilter,
											BOOL AddColourToList)
{
#ifdef DO_EXPORT

	ERROR2IF(pCol == NULL || pFilter == NULL, 0L, "ColourListComponent::SaveComplexColour no pCol or pFilter!");
	ERROR2IF(pExportedColours == NULL, 0L, "ColourListComponent::SaveComplexColour no pExportedColours!");

	// When GetColourLinePosition is TRUE, it means that we want to save the colour's pos on the colour
	// line, so that it can be imported in the correct order.
	//
	// This is set to AddColourToList by default - i.e. assume that if it's being added to the list of exported
	// colours, then we probably want to preserve the colour' pos
	BOOL GetColourLinePosition = AddColourToList;

	// ****
	// **** Get the information on the colour
	// ****

	// Get information on the colour - ask for its full identifier rather than just the name,
	// so we get a unique id for unnamed colours rather than just "Local colour"
	ColourModel Model = pCol->GetColourModel();
	ColourGeneric GenCol;
	IndexedColourType ColType = pCol->GetType();
	pCol->GetSourceColour(&GenCol);
	String_64 ColName = *(pCol->GetName(TRUE));
	if (ColName[0] == '_')
	{
		// We have an unnamed colour and so the name should be blank
		ColName.Empty();

		// If it's an unnamed colour, then it's assumed it's not on the colour line, so we
		// won't need to find it's pos on the colour line
		GetColourLinePosition = FALSE;
	}

	//TCHAR *ColName = (TCHAR *) (*(pCol->GetName(TRUE)));

	// Find a global colour context to convert this colour into RGB for us.
	ColourContextRGBT *cc = (ColourContextRGBT *)ColourManager::GetColourContext(COLOURMODEL_RGBT);
	ERROR3IF(cc == NULL, "ColourListComponent::SaveComplexColour No global RGB colour context!? Something is very broken");
	if (cc == NULL)
		return 0L;

	BOOL ok = FALSE;
	// Create a DocColour referencing this IndexedColour.
	DocColour SimpleColour;
	SimpleColour.MakeRefToIndexedColour(pCol);

	// And convert the colour into a *packed* (8-bit components) colour structure
	PColourRGBT Result;
	cc->ConvertColour(&SimpleColour, (ColourPacked *)&Result);

	// And finally, read the bytes out of the packed colour struct.
	BYTE RedByteValue = Result.Red;
	BYTE GreenByteValue = Result.Green;
	BYTE BlueByteValue = Result.Blue;

	// ****
	// **** Save the record
	// ****

	// Write the data out to the file
	UINT32 Size = TAG_DEFINECOMPLEXCOLOUR_SIZE;
	// If there is a string name, then add it to this size
	// REMEMBER: We save out unicode strings and so we need to double the length of the returned string length
	// but only if we are not being compiled as Unicode
#ifdef UNICODE
	Size += ColName.Length() + 1;
#else
	Size += (ColName.Length() + 1) * 2;
#endif	
//	INT32 RecordNumber = pFilter->StartRecord(TAG_DEFINECOMPLEXCOLOUR, Size);
	CXaraFileRecord Rec(TAG_DEFINECOMPLEXCOLOUR, Size);
	ok = Rec.Init();

	// Start with the simple RGB colour
	if (ok) ok = Rec.WriteBYTE(RedByteValue);
	if (ok) ok = Rec.WriteBYTE(GreenByteValue);
	if (ok) ok = Rec.WriteBYTE(BlueByteValue);

	// Now write out the colour model...
	if (ok) ok = Rec.WriteBYTE((BYTE)Model);

	// Now write out the colour type...
	// Include flags about linking in colour type?
	// If the colour is linked, we save out the component as an illegal known value
	// Convert the colour types into export colour types
	// Should be a straight conversion but just in case...
	ExportColourType ExpColType;
	switch (ColType)
	{
		case COLOURTYPE_NORMAL:	ExpColType = EXPORT_COLOURTYPE_NORMAL;	break;
		case COLOURTYPE_SPOT:	ExpColType = EXPORT_COLOURTYPE_SPOT;	break;
		case COLOURTYPE_LINKED:	ExpColType = EXPORT_COLOURTYPE_LINKED;	break;
		case COLOURTYPE_TINT:
			if (pCol->TintIsShade())
				ExpColType = EXPORT_COLOURTYPE_SHADE;
			else
				ExpColType = EXPORT_COLOURTYPE_TINT;
			break;
		default:
			ERROR3("ColourListComponent::SaveColour - Unknown colour model!");
			break;
	}
	if (ok) ok = Rec.WriteBYTE((BYTE)ExpColType);

	// Write out the entry number this colour is in the indexed colour list
	UINT32 ColourListNumber = 0L;
	// If GetColourLinePosition is False then the indexed colour is not on the colour line.
	// so do NOT try and go through the list of colours to find its pos number, just use zero.
	if (GetColourLinePosition)
		ColourListNumber = GetIndexColourNumber(pCol); 
	if (ok) ok = Rec.WriteUINT32(ColourListNumber);

	// Write out the reference number of the parent colour
	UINT32 ParentColourRef = 0L; 
	IndexedColour *pParentCol = pCol->FindLinkedParent();
	ExportColour* pExportCol = NULL;
	if (pParentCol)
	{
		// If there is a parent then recover the record number from the exported colours list 
		pExportCol = pExportedColours->GetColour(pParentCol);
		ERROR2IF(pExportCol == NULL,FALSE,"ColourListComponent::SaveComplexColour Not exported parent before child!")
		ParentColourRef = pExportCol->RecordNumber;
		TRACEUSER( "Neville", _T("Saving parent reference %d\n"),ParentColourRef);
	}

	if (ok) ok = Rec.WriteReference(ParentColourRef);

	BOOL Linked = (ColType == COLOURTYPE_LINKED);
	switch (ColType)
	{
		
		case COLOURTYPE_NORMAL:
		case COLOURTYPE_SPOT:
		case COLOURTYPE_LINKED:
		{
			// These all require the same handling
			// Linked colours save out the components in the normal fashion

			// Now write out the colour component values depending on the model in use...
			switch (Model)
			{
				case COLOURMODEL_RGBT:
					// Now add the RGB components.
					// Stored as FIXED24s.
					ColourRGBT *pRGBT;
					pRGBT = (ColourRGBT *) &GenCol;
					if (ok) ok = Rec.WriteUINT32(AddComponentValue(pRGBT->Red, LINKED_COLOUR(1)));
					if (ok) ok = Rec.WriteUINT32(AddComponentValue(pRGBT->Green,	LINKED_COLOUR(2)));
					if (ok) ok = Rec.WriteUINT32(AddComponentValue(pRGBT->Blue, LINKED_COLOUR(3)));
					if (ok) ok = Rec.WriteUINT32(AddComponentValue(pRGBT->Transparent, LINKED_COLOUR(4)));
					break;

				case COLOURMODEL_HSVT:
					// Now add the HSV components.
					ColourHSVT *pHSVT;
					pHSVT = (ColourHSVT *) &GenCol;
					if (ok) ok = Rec.WriteUINT32(AddComponentValue(pHSVT->Hue, LINKED_COLOUR(1)));
					if (ok) ok = Rec.WriteUINT32(AddComponentValue(pHSVT->Saturation, LINKED_COLOUR(2)));
					if (ok) ok = Rec.WriteUINT32(AddComponentValue(pHSVT->Value, LINKED_COLOUR(3)));
					if (ok) ok = Rec.WriteUINT32(0);	// pad to 4 colour components
					break;

				case COLOURMODEL_CMYK:
					// Now add the CMYK components.
					ColourCMYK *pCMYK;
					pCMYK = (ColourCMYK *) &GenCol;
					if (ok) ok = Rec.WriteUINT32(AddComponentValue(pCMYK->Cyan, LINKED_COLOUR(1)));
					if (ok) ok = Rec.WriteUINT32(AddComponentValue(pCMYK->Magenta, LINKED_COLOUR(2)));
					if (ok) ok = Rec.WriteUINT32(AddComponentValue(pCMYK->Yellow, LINKED_COLOUR(3)));
					if (ok) ok = Rec.WriteUINT32(AddComponentValue(pCMYK->Key, LINKED_COLOUR(4)));
					break;

				case COLOURMODEL_GREYT:
					// greyscale colour.
					ColourGreyT *pGreyT;
					pGreyT = (ColourGreyT *) &GenCol;
					if (ok) ok = Rec.WriteUINT32(AddComponentValue(pGreyT->Intensity, LINKED_COLOUR(1)));
					if (ok) ok = Rec.WriteUINT32(0);	// pad to 4 colour components
					if (ok) ok = Rec.WriteUINT32(0);	// pad to 4 colour components
					if (ok) ok = Rec.WriteUINT32(0);	// pad to 4 colour components
					break;

				default:
					ERROR3("ColourListComponent::SaveColour - Unknown colour model!");
					break;
			}
			break;
		}

		case COLOURTYPE_TINT:
		{
			// If we have a tint colour then this needs special handling
			FIXED24 TintVal(0.0);
			FIXED24 ShadeVal(0.0);
			// Is the tint a tint or is it really a shade
			// Add tint identifier to colour definition
			if (pCol->TintIsShade())
			{
				ShadeVal = pCol->GetShadeValueX();
				TintVal  = pCol->GetShadeValueY();
				if (ok) ok = Rec.WriteUINT32(AddComponentValue(ShadeVal, LINKED_COLOUR(1)));
				if (ok) ok = Rec.WriteUINT32(AddComponentValue(TintVal, LINKED_COLOUR(1)));
				if (ok) ok = Rec.WriteUINT32(0);	// pad to 4 colour components
				if (ok) ok = Rec.WriteUINT32(0);	// pad to 4 colour components
			}
			else
			{
				TintVal  = pCol->GetTintValue();
				if (ok) ok = Rec.WriteUINT32(AddComponentValue(TintVal, LINKED_COLOUR(1)));
				if (ok) ok = Rec.WriteUINT32(0);	// pad to 4 colour components
				if (ok) ok = Rec.WriteUINT32(0);	// pad to 4 colour components
				if (ok) ok = Rec.WriteUINT32(0);	// pad to 4 colour components
			}
			break;
		}
		default:
			ERROR3("ColourListComponent::SaveColour - Unknown colour type!");
			break;
	}

	// Write out the name of the colour
	if (ok) ok = Rec.WriteUnicode(ColName);

	// Finally, write the record out to file
	INT32 RecordNumber = 0L;
	if (ok) RecordNumber = pFilter->WriteDefinitionRecord(&Rec);

	// Has the caller handled adding this to the colour list already? No, so do it oursleves.
	if (AddColourToList && RecordNumber > 0)
	{
		// Add this colour to our list of exported items
		ok = pExportedColours->AddColour(pCol, RecordNumber);
	}
//TRACEUSER( "Neville", _T("Saving colour %s, number %d, entry number %d\n"),(TCHAR*)ColName, RecordNumber, ColourListNumber);

	// If we have had a problem at any of the stages then return that to the caller
	if (!ok)
		return 0L;

	// Everything went tickety boo, so return the record number to the caller.
	return RecordNumber;
#else
	return 0L;
#endif
}

/********************************************************************************************

>	UINT32 ColourListComponent::AddComponentValue(FIXED24 ComponentValue, BOOL Inherited)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		ComponentValue - the value to be returned.
				Inherited - TRUE => This component is inherited, so just output a known illegal value
						   FALSE => This component is not inherited, so output the
									actual value to the string
	Outputs:	-
	Returns:	The colour component as a UINT32 ready to write out to file.
	Purpose:	Returns either the colour component value or if it is inherited, a known illegal
				value which we can intepret on import to be an overridden value. 
				Doing it in a function centralises the syntax of inherited components.
	Errors:		-
	SeeAlso:	ColourListComponent::SaveColourAndParents

********************************************************************************************/

UINT32 ColourListComponent::AddComponentValue(FIXED24 ComponentValue, BOOL Inherited)
{
	// Construct the colour component value
	UINT32 ColourValue = 0L;

	// If this is a linked colour, and this component is inherited, then just add a '-'.
	if (Inherited)
	{
		ColourValue = FIXED24(-8.0).GetAsFixed24();
	}
	else
	{
		// We have to output the component....
		ColourValue = ComponentValue.GetAsFixed24();
	}

	return ColourValue;
}

/********************************************************************************************

>	INT32 ColourListComponent::GetIndexColourNumber(IndexedColour *pCol)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		pCol - the indexed colour to search out the number for.
	Returns:	The number of this indexed colour in the list of colours (i.e.position)
	Purpose:	Work out the index of this colour in the list of indexed colours so that we
				can try and load back the colours in the same order.
				Unfortunately, the only way it seems that we can work out this number is by
				counting back to the start of the indexed colour list. Doh!
	SeeAlso:	IndexedColour

********************************************************************************************/

INT32 ColourListComponent::GetIndexColourNumber(IndexedColour *pCol)
{
	ERROR2IF(pCol==NULL,0,"ColourListComponent::GetIndexColourNumber no Colour pointer");
	ERROR2IF(pIndexedColours==NULL,0,"ColourListComponent::GetIndexColourNumber no pIndexedColours");

	INT32 number = 0;

	// Due to pants list class, we need to check that there is a previous memember before we
	// call GetPrev, otherwise it ensures all over the place. Why it can't just return NULL!
	if (pIndexedColours->IsEmpty())
		return number;

	// Go from this item to the start of the inddexed colour list, counting items as we go
	while (pCol)
	{
		pCol = (IndexedColour *) pIndexedColours->GetPrev(pCol);
		// If the colour is not deleted then count it
		if (pCol && !pCol->IsDeleted())
			number ++;
	}

	return number;
}


/********************************************************************************************
********************************************************************************************/

// Remove the preceeding code on all builds but those who want it

/********************************************************************************************
********************************************************************************************/

/********************************************************************************************

>	BOOL ColourListComponent::EPSStartImport(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
	Purpose:	Inform the colour list document component that an EPS import is about to 
				start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL ColourListComponent::EPSStartImport(EPSFilter *pFilter)
{
	PORTNOTETRACE("filters","ColourListComponent::EPSStartImport - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Is this a Camelot EPS filter?
	BOOL CamelotEPS = pFilter->IS_KIND_OF(CamelotEPSFilter);

	// Get a colour table (and don't do strict checking if we are loading Camelot EPS).
	pNewColours = new ImportedColours(this, !CamelotEPS);
	if ((pNewColours == NULL) || !pNewColours->Init())
		return FALSE;

	// We keep a record of how nested we are with respect to linking colours, e.g.
	// links of links of links.
	// A nesting of 0 is no nesting.
	LinkNesting = 0;

	// Must have worked
	return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	void ColourListComponent::EPSEndImport(EPSFilter *pFilter, BOOL Success)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.
	Purpose:	Inform the colour list document component that an EPS import has just 
				finished.
	SeeAlso:	DocComponent

********************************************************************************************/

void ColourListComponent::EPSEndImport(EPSFilter *pFilter, BOOL Success)
{
	PORTNOTETRACE("filters","ColourListComponent::EPSEndImport - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(pNewColours == NULL, "No colour table in EndEPSImport");

	// If we didn't even get to initialise, then return quietly.
	if (pNewColours == NULL)
		return;

	// Import is finished, so add any outstanding colours to the document (if the
	// import was successful), and delete the colour table
	if (Success)
	{
		pNewColours->AddColoursToDocument();
	}
	else
	{
		// Import failed - destroy all the colours.
		pNewColours->DestroyColours();
	}

	// Finally, delete the IndexedColour table.
	delete pNewColours;
	pNewColours = NULL;
#endif
}



/********************************************************************************************

>	void ColourListComponent::AddNewColour(IndexedColour *pNewCol)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/94
	Inputs:		pNewCol - the colour to add.
	Purpose:	Adds an indexed colour to the colour list (which in turn ensures that
				it has a unique colour name)
	SeeAlso:	ColourList::AddItem

********************************************************************************************/

void ColourListComponent::AddNewColour(IndexedColour *pNewCol)
{
	ERROR3IF(pNewCol == NULL,
				"ColourListComponent::AddNewColour: The colour to add is NULL!");
	ERROR3IF(pIndexedColours == NULL,
				"ColourListComponent::AddNewColour: I have no IndexedColour list!");

	// Add the colour, ensuring that it has a unique name
	pIndexedColours->AddItem(pNewCol);
}



/********************************************************************************************

>	BOOL ColourListComponent::WriteEPSComments(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pFilter - the filter being used to import the EPS file.
	Returns:	TRUE if the colour table was written successfully;
				FALSE if not.
	Purpose:	Writes out the colour table in the form of EPS comments in the 'header'
				comments of the EPS file.

	Notes:		IMPORTANT: Version 1.1 file extensions (Jason, 7/9/95 & 14/10/95)
				For this version, Shade colours have been added. 
				To retain backward compatability,
				all shades and their children are written into a second colour table
				(a %%JWColourTable). This means old loader code will skip them and use
				the inlined CMYK definitions, thus retaining the appearance of the document
				while only losing unsupported colour linking info.
				Thus, we do a 2-pass scheme. The first pass will (by use of default params
				in SaveColourAndChildren) fail to save out any shades and children.
				The second pass then saves all shades and their parents and children.
				Note this means parent colours can be saved twice, but these will be
				happily merged on loading, so no bad effects occur.

				Shades are only saved out in CamelotEPS. In Artworks EPS mode, they
				become standalone named colours.

	Errors:		Out of disk space, and similar disk errors.
	SeeAlso:	DocComponent::WriteEPSComments;
				DocComponent

********************************************************************************************/

BOOL ColourListComponent::WriteEPSComments(EPSFilter *pFilter)
{
#ifdef DO_EXPORT
	if (pFilter->IsKindOf(CC_RUNTIME_CLASS(ArtWorksEPSFilter)))
	{
		// Is it a Camelot specific filter?
		BOOL CamelotEPS = pFilter->IsKindOf(CC_RUNTIME_CLASS(CamelotEPSFilter));

		// Output the colour table in ArtWorks format.
		EPSExportDC *pDC = pFilter->GetExportDC();

		// The ArtWorks EPS comment:
		pDC->OutputToken("%%AWColourTable");
		pDC->OutputNewLine();

		// Iterate over the document's list of named IndexedColours...
		IndexedColour *pCol = (IndexedColour *) pIndexedColours->GetHead();

		while (pCol != NULL)
		{
			if (!pCol->IsDeleted() && 
				((pCol->GetType() == COLOURTYPE_NORMAL) || 
				 (pCol->GetType() == COLOURTYPE_SPOT)))
			{
				// The colour is in use and is not a linked colour or tint - save it to 
				// the EPS file.
				SaveColourAndChildren(pCol, pDC, CamelotEPS);

			}

			// Try the next colour
			pCol = (IndexedColour *) pIndexedColours->GetNext(pCol);
		}

		// Iterate over the document's list of unnamed IndexedColours...
		List *pUnnamedColours = pIndexedColours->GetUnnamedColours();

		pCol = (IndexedColour *) pUnnamedColours->GetHead();

		while (pCol != NULL)
		{
			if (!pCol->IsDeleted() && pCol->IsInUse() &&
				((pCol->GetType() == COLOURTYPE_NORMAL) || 
				 (pCol->GetType() == COLOURTYPE_SPOT)))
			{
				// The colour is in use and is not a linked colour or tint - save it to 
				// the EPS file.
				SaveColourAndChildren(pCol, pDC, CamelotEPS);
			}

			// Try the next colour
			pCol = (IndexedColour *) pUnnamedColours->GetNext(pCol);
		}


		// --- PASS 2 - version 1.1 second colour table for shades and other extensions
		if (CamelotEPS)
		{
			// This variable is used to stop us outputting the colour table section
			// if it is not needed - we only output the header line when we write the
			// first colour out to this table.
			BOOL HaveOutputHeader = FALSE;

			// Iterate over the document's list of named IndexedColours...
			IndexedColour *pCol = (IndexedColour *) pIndexedColours->GetHead();

			while (pCol != NULL)
			{
				if (!pCol->IsDeleted() && 
					((pCol->GetType() == COLOURTYPE_NORMAL) || 
					 (pCol->GetType() == COLOURTYPE_SPOT)))
				{
					// The colour is in use and is not a linked colour or tint - save it to 
					// the EPS file.
					if (ColourOrChildIsShade(pCol))
					{
						if (!HaveOutputHeader)
						{
							// Output the header comment if we haven't already done so
							pDC->OutputToken("%%JWColourTable");
							pDC->OutputNewLine();
							HaveOutputHeader = TRUE;
						}

						SaveColourAndChildren(pCol, pDC, CamelotEPS, 0, TRUE);
					}
				}

				// Try the next colour
				pCol = (IndexedColour *) pIndexedColours->GetNext(pCol);
			}

			// Iterate over the document's list of unnamed IndexedColours...
			List *pUnnamedColours = pIndexedColours->GetUnnamedColours();

			pCol = (IndexedColour *) pUnnamedColours->GetHead();

			while (pCol != NULL)
			{
				if (!pCol->IsDeleted() && pCol->IsInUse() &&
					((pCol->GetType() == COLOURTYPE_NORMAL) || 
					 (pCol->GetType() == COLOURTYPE_SPOT)))
				{
					// The colour is in use and is not a linked colour or tint - save it to 
					// the EPS file.
					if (ColourOrChildIsShade(pCol))
					{
						if (!HaveOutputHeader)
						{
							// Output the header comment if we haven't already done so
							pDC->OutputToken("%%JWColourTable");
							pDC->OutputNewLine();
							HaveOutputHeader = TRUE;
						}

						SaveColourAndChildren(pCol, pDC, CamelotEPS, 0, TRUE);
					}
				}

				// Try the next colour
				pCol = (IndexedColour *) pUnnamedColours->GetNext(pCol);
			}
			
		}
	}
#endif
	// All ok.
	return TRUE;
}

/********************************************************************************************

>	void ColourListComponent::AddComponentValue(char *ColDef, char *Format, 
												double ComponentValue, BOOL Inherited)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
	Inputs:		ColDef - the current colour definition string, which this routine should
						 append the component value to.
				Format - the printf format to use when converting the value to ASCII.
				ComponentValue - the value to add to the string.
				Inherited - TRUE => This component is inherited, so just output a '-'
						   FALSE => This component is not inherited, so output the
									actual value to the string
	Outputs:	ColDef - the updated colour definition string, with the value converted to 
						 ASCII (taking into account the Inherited flag), and appended (with 
						 a leading space so the values don't join together).
	Purpose:	Add a colour component value to a colour definition string.  This is used
				to build up the entries in the EPS colour table.  It also centralises the
				syntax of inherited components.
	Errors:		-
	SeeAlso:	ColourListComponent::SaveColourAndChildren

********************************************************************************************/

void ColourListComponent::AddComponentValue( PTSTR ColDef, PCTSTR Format, 
											double ComponentValue, BOOL Inherited)
{
	// Construct the colour component value
	TCHAR				CompVal[20];

	// If this is a linked colour, and this component is inherited, then just add a '-'.
	if (Inherited)
	{
		_tcscpy( CompVal, _T("-") );
	}
	else
	{
		// We have to output the component....
		tsprintf( CompVal, 20, const_cast<TCHAR *>(Format), ComponentValue );
	}

	// Add the value onto the end of the string
	_tcscat( ColDef, _T(" ") );
	_tcscat( ColDef, CompVal );
}



/********************************************************************************************

>	BOOL ColourListComponent::ColourOrChildIsShade(IndexedColour *pCol)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		pCol - The (root) colour to check

	Returns:	TRUE if this colour or a descendant is a Shade colour

	Purpose:	To determine if a colour is a Shade or has descendants which are Shades.

********************************************************************************************/

BOOL ColourListComponent::ColourOrChildIsShade(IndexedColour *pCol)
{
	// Check if this colour exists
	if (pCol->IsDeleted())
		return(FALSE);

	// Check if this colour is a shade
	if (pCol->GetType() == COLOURTYPE_TINT && pCol->TintIsShade())
		return(TRUE);

	// If this has no children, then no point in scanning further
	if (!pCol->HasLinkedChildren())
		return(FALSE);


	// Ask all the named colours that are immediate children of this colour
	IndexedColour *pChild = (IndexedColour *) pIndexedColours->GetHead();

	while (pChild != NULL)
	{
		if (!pChild->IsDeleted() && pChild->FindLinkedParent() == pCol)
		{
			// Found a child - recurse on it
			if (ColourOrChildIsShade(pChild))
				return(TRUE);
		}

		// Try the next colour
		pChild = (IndexedColour *) pIndexedColours->GetNext(pChild);
	}

	// Get all the unnamed colours that are immediate children of this colour
	List *pUnnamedList = pIndexedColours->GetUnnamedColours();
	pChild = (IndexedColour *) pUnnamedList->GetHead();

	while (pChild != NULL)
	{
		if (!pChild->IsDeleted() && pChild->FindLinkedParent() == pCol)
		{
			// Found a child - recurse on it
			if (ColourOrChildIsShade(pChild))
				return(TRUE);
		}

		// Try the next colour
		pChild = (IndexedColour *) pUnnamedList->GetNext(pChild);
	}

	return(FALSE);
}



// Simple macro to make the code in SaveColourAndChildren() a bit neater.
#define LINKED(ID) (Linked && pCol->InheritsComponent((ID)))

/********************************************************************************************

>	BOOL ColourListComponent::SaveColourAndChildren(IndexedColour *pCol, EPSExportDC *pDC, 
												BOOL CamelotEPS, INT32 NestingLevel = 0,
												BOOL SaveShades = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
	Inputs:		pCol		- The (root) colour to save
				pDC			- The EPS export DC to save to
				CamelotEPS	- TRUE if exporting Camelot EPS
				NestingLevel- The nesting level of tint/shade/link colours. Used in
							  recursion - external callers should pass in 0
				SaveShades	- Indicates if Shades and their children should be saved. Used
							  in version 1.1 CamelotEPS to save out 2 colour tables, the
							  second of which includes the shades. (In this way, we could
							  retain load-compatability with older versions)
	Outputs:	-
	Returns:	-
	Purpose:	Save out an indexed colour to an EPS file, and all the colours that rely
				on it, i.e. any other indexed colours which are tints of, or links to, this
				colour.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ColourListComponent::SaveColourAndChildren(IndexedColour *pCol, EPSExportDC *pDC, 
												BOOL CamelotEPS, INT32 NestingLevel,
												BOOL SaveShades)
{
#ifdef DO_EXPORT

	// (Jason 27/10/95) If the colour is unnamed and it's not actually used anywhere,
	// don't bother saving it. This optimises out temporary locals that are no longer
	// needed. At the worst, if something goes horribly wrong, this just means that
	// some locals might turn up as CMYK definitions on reloading, but this shouldn't
	// cause any problems unless something deeply scary has happened to the colour system.
	if (!pCol->IsNamed() && !pCol->IsInUse())
		return(TRUE);

	// Check for silly recursion errors	[not that you can get circular references with
	// the amazingly strict and careful checking done by the IndexedColour class - JW ;-]
	ERROR2IF(NestingLevel > 200, FALSE, "Rampant linked colour nesting detected!");

	// First, save out this indexed colour.
	ColourModel Model = pCol->GetColourModel();

	// Buffer to build up colour defs in.
	char ColDef[256];
	_tcscpy(ColDef, "%%+");

	// Buffer for temporary strings
	char TmpBuf[256];

	// Get information on the colour - ask for its full identifier rather than just the name,
	// so we get a unique id for unnamed colours rather than just "Local colour"
	ColourGeneric GenCol;
	IndexedColourType ColType = pCol->GetType();
	pCol->GetSourceColour(&GenCol);
	TCHAR *ColName = (TCHAR *) (*(pCol->GetName(TRUE)));

	// Special check - Shades are only properly preserved in CamelotEPS. In all others, they
	// are treated as unlinked, normal colours, so that they will be read happily.
	if (!CamelotEPS && ColType == COLOURTYPE_TINT && pCol->TintIsShade())
	{
		ColType = COLOURTYPE_NORMAL;
	}

	// If we shouldn't be saving shade colours or their children on this pass, then don't
	// do anything for them this time.
	if (!SaveShades && ColType == COLOURTYPE_TINT && pCol->TintIsShade())
		return(TRUE);

	// If saving shades, then don't bother with non-shade colours which have no children, as
	// they do not need to be output.
	if (SaveShades && (ColType != COLOURTYPE_TINT || !pCol->TintIsShade()) && !pCol->HasLinkedChildren())
		return(TRUE);

	// Check for tints
	if (ColType == COLOURTYPE_TINT)
	{
		FIXED24 TintVal(0.0);
		FIXED24 ShadeVal(0.0);

		// Add tint identifier to colour definition
		if (pCol->TintIsShade())
		{
			_tcscat(ColDef, "d");		// Is a shaDe
			TintVal = pCol->GetShadeValueY();		// Note that it is written out as "Y X"
			ShadeVal = pCol->GetShadeValueX();		// to make backward compatability easier
		}
		else
		{
			_tcscat(ColDef, "t");		// Is a Tint
			TintVal = pCol->GetTintValue();
		}

		ERROR3IF(NestingLevel < 1, "Bad nesting when saving a tint/shade colour");

		// Get tint/shade value as an integer in range 0 - 100.
		INT32 Tint = (INT32) ((TintVal.MakeDouble() * 100.0) + ((TintVal < 0) ? -0.5 : 0.5));
		INT32 Shade = (INT32) ((ShadeVal.MakeDouble() * 100.0) + ((ShadeVal < 0) ? -0.5 : 0.5));

		if (NestingLevel > 1)
		{
			// Write out the nesting level
			_stprintf(TmpBuf, " %d", NestingLevel);
			_tcscat(ColDef, TmpBuf);
		}

		// Add the colour name and tint value
		if (pCol->TintIsShade())
			_stprintf(TmpBuf, " (%s) %d %d", ColName, Tint, Shade);
		else
			_stprintf(TmpBuf, " (%s) %d", ColName, Tint);
			_tcscat(ColDef, TmpBuf);
	}
	else
	{
		// Is this a linked colour?
		BOOL Linked = (ColType == COLOURTYPE_LINKED);

		// We only do linked colours in CamelotEPS
		Linked = Linked && CamelotEPS;

		
		if (Linked)
		{
			// Mark this as a linked colour
			_tcscat(ColDef, "l");
		}
		else if (CamelotEPS && (ColType == COLOURTYPE_SPOT))
		{
			// Mark this as a spot colour
			_tcscat(ColDef, "s");
		}

		// Save out colour model, nesting level and colour name
		switch (Model)
		{
			case COLOURMODEL_RGBT:
				_tcscat(ColDef, "r");
				break;

			case COLOURMODEL_HSVT:
				_tcscat(ColDef, "h");
				break;

			case COLOURMODEL_CMYK:
				_tcscat(ColDef, "c");
				break;

			case COLOURMODEL_GREYT:
				// Special case - only Camelot can handle greyscales - ArtWorks can't.
				if (CamelotEPS)
					_tcscat(ColDef, "g");
				else
					_tcscat(ColDef, "r");
				break;
		}

		// Add nesting level, if needed
		if (NestingLevel > 1)
		{
			// Write out the nesting level
			_stprintf(TmpBuf, " %d", NestingLevel);
			_tcscat(ColDef, TmpBuf);
		}

		// Add colour name
		_tcscat(ColDef , " (");
		_tcscat(ColDef, ColName);
		_tcscat(ColDef, ")");

		// Now write out the colour component values...
		switch (Model)
		{
			case COLOURMODEL_RGBT:
				// Now add the RGB components.
				ColourRGBT *pRGBT;
				pRGBT = (ColourRGBT *) &GenCol;

				AddComponentValue(ColDef, "%.3f", pRGBT->Red.MakeDouble(),   LINKED(1));
				AddComponentValue(ColDef, "%.3f", pRGBT->Green.MakeDouble(), LINKED(2));
				AddComponentValue(ColDef, "%.3f", pRGBT->Blue.MakeDouble(),  LINKED(3));
				break;

			case COLOURMODEL_HSVT:
				// Now add the HSV components.
				ColourHSVT *pHSVT;
				pHSVT = (ColourHSVT *) &GenCol;

				AddComponentValue(ColDef, "%.1f", pHSVT->Hue.MakeDouble() * 360.0, LINKED(1));
				AddComponentValue(ColDef, "%.1f", pHSVT->Saturation.MakeDouble() * 100.0, LINKED(2));
				AddComponentValue(ColDef, "%.1f", pHSVT->Value.MakeDouble() * 100.0, LINKED(3));
				break;

			case COLOURMODEL_CMYK:
				// Now add the CMYK components.
				ColourCMYK *pCMYK;
				pCMYK = (ColourCMYK *) &GenCol;

				AddComponentValue(ColDef, "%.3f", pCMYK->Cyan.MakeDouble(),    LINKED(1));
				AddComponentValue(ColDef, "%.3f", pCMYK->Magenta.MakeDouble(), LINKED(2));
				AddComponentValue(ColDef, "%.3f", pCMYK->Yellow.MakeDouble(),  LINKED(3));
				AddComponentValue(ColDef, "%.3f", pCMYK->Key.MakeDouble(),     LINKED(4));
				break;

			case COLOURMODEL_GREYT:
				// ArtWorks doesn't have explicit greyscale colours - use RGB model.
				ColourGreyT *pGreyT;
				pGreyT = (ColourGreyT *) &GenCol;
				double Intensity;
				Intensity = pGreyT->Intensity.MakeDouble();
				if (CamelotEPS)
				{
					// General greyscale format...just add the intensity component
					AddComponentValue(ColDef, "%.3f", Intensity, LINKED(1));
				}
				else
				{
					// Fake a greyscale for ArtWorks with an RGB colour
					// where R = G = B.
					for (INT32 i = 1; i <= 3; i++)
					{
						AddComponentValue(ColDef, "%.3f", Intensity, FALSE);
					}
				}
				break;

			default:
				ERROR3("Unknown colour type found in EPS export!");
				break;
		}
	}

	// Output colour definition to the EPS file
	pDC->OutputToken(ColDef);
	pDC->OutputNewLine();


	// (Jason 27/10/95) If it doesn't have linked children, don't recurse.
	// On large files, this saves a *lot* of time (O(n*n) becomes O(n)), because
	// (a) linked colours are only usually used sporadically, and (b) local colours,
	// which make up the bulk of saved colours, cannot have any children.
	if (!pCol->HasLinkedChildren())
		return(TRUE);


	// ---------- recurse ----------


	// Now recurse to save out any children of this colour (links or tints)...

	// Get all the named colours that are immediate children of this colour
	IndexedColour *pChild = (IndexedColour *) pIndexedColours->GetHead();

	while (pChild != NULL)
	{
		if (!pChild->IsDeleted() && pChild->FindLinkedParent() == pCol)
		{
			// Found a child - save out via recursion so we save all its descendants in the
			// right order...
			SaveColourAndChildren(pChild, pDC, CamelotEPS, NestingLevel + 1, SaveShades);
		}

		// Try the next colour
		pChild = (IndexedColour *) pIndexedColours->GetNext(pChild);
	}

	// Get all the unnamed colours that are immediate children of this colour
	List *pUnnamedList = pIndexedColours->GetUnnamedColours();

	pChild = (IndexedColour *) pUnnamedList->GetHead();

	while (pChild != NULL)
	{
		if (!pChild->IsDeleted() && pChild->FindLinkedParent() == pCol)
		{
			// Found a child - save out via recursion so we save all its descendants in the
			// right order...
			SaveColourAndChildren(pChild, pDC, CamelotEPS, NestingLevel + 1, SaveShades);
		}

		// Try the next colour
		pChild = (IndexedColour *) pUnnamedList->GetNext(pChild);
	}
#endif
	return TRUE;
}


/********************************************************************************************

>	ProcessEPSResult ColourListComponent::ProcessEPSComment(EPSFilter *pFilter, 
														    const char *pComment)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
				pComment - pointer to the comment to process.
	Returns:	EPSCommentUnknown	 - This EPS comment is not recognised by the document
									   component.
				EPSCommentError		 - This EPS comment was recognised by this document
									   component, but it contained an error.
				EPSCommentOK		 - This EPS comment was recognised as a legal comment by 
									   this document component, and was processed
									   successfully.
	Purpose:	Process an EPS comment - if this is a colour table comment, this component
				will claim it and try to decode it.

	Notes:		IMPORTANT
				There is a new colour field (which should only be put in a JWColourTable
				in CamelotEPS) which starts a comment line with "%%+x". The remainder of the
				line is undefined, but is treated as a valid line. This will make it possible
				to add data to the colour list in future without losing backward compatability
				or having to add yet another colour table type!
				If you use this, make sure your code happily *ignores* unknown x extensions.

	Errors:		Badly formed EPS comment.
	SeeAlso:	DocComponent::ProcessEPSComment

********************************************************************************************/

ProcessEPSResult ColourListComponent::ProcessEPSComment(EPSFilter *pFilter, 
														PCTSTR pComment)
{
	PORTNOTETRACE("filters","ColourListComponent::ProcessEPSComment - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (pFilter->IsKindOf(CC_RUNTIME_CLASS(ArtWorksEPSFilter)))
	{
		// Is it a Camelot specific filter?
		BOOL CamelotEPS = pFilter->IsKindOf(CC_RUNTIME_CLASS(CamelotEPSFilter));

		// ArtWorks EPS (or derivative)
		if (_tcsncmp(pComment, "%%AWColourTable", 15) == 0)
		{
			// Found a colour table
			return EPSCommentOK;
		}
		else if (_tcsncmp(pComment, "%%JWColourTable", 15) == 0)
		{
			// Found a version 1.1 extended colour table
			return EPSCommentOK;			
		}
		else if (_tcsncmp(pComment, "%%+", 3) == 0)
		{
			// Found a colour - add it to the colour list for the document.

			// Take a copy of this comment
			char Comment[256];
			_tcscpy(Comment, pComment);

			// Find out the colour model (c, h, r, or t)
			INT32 i = 3;
			char ColModel = Comment[i++];

			// Check for special types (NB a colour cannot be both a spot colour and
			// a linked colour).
			NewColourInfo ColourInfo;
			BOOL Linked = FALSE;

			// Is it a spot colour?
			if  (ColModel == 's')
			{
				ColourInfo.Type = COLOURTYPE_SPOT;
				ColModel = Comment[i++];
			}
			else if (ColModel == 'l')
			{
				// It's a link, then find out the colour model of the linked colour.
				Linked = TRUE;
				ColourInfo.Type = COLOURTYPE_LINKED;
				ColModel = Comment[i++];
			}

			// If a tint/shade or link, find the nesting level, if any
			INT32 ColourNesting = 0;
			if ((ColModel == 't') || (ColModel == 'd') || Linked)
			{
				// Skip until we find a digit, a '(' or a terminator
				while (!isdigit(Comment[i]) && (Comment[i] != '(') && (Comment[i] != 0))
					i++;

				// Is there a nesting level?
				if (isdigit(Comment[i]))
				{
					// Yes - extract it and skip past it.
					ColourNesting = _ttol(Comment+i);

					while (isdigit(Comment[i]))
						i++;
				}
				else
				{
					// Must be level 1
					ColourNesting = 1;
				}
			}

			// Do we need to alter the link context?
			if (ColourNesting == 0)
			{
				// Normal colour
				pNewColours->RestoreContextTo(0);
			}
			else if (ColourNesting <= LinkNesting)
			{
				// Falling back to a previous nesting level - restore context to one
				// less than the nesting of this link/tint
				// (because we want to link/tint the colour one level below this one
				// in the nesting).
				LinkNesting = ColourNesting - 1;
				pNewColours->RestoreContextTo(LinkNesting);
			}

			// Find the colour name
			while ((Comment[i] != '(') && (Comment[i] != 0))
				i++;

			// Got it - copy it out of the string (skip the opening parenthesis)
			char ColName[128];
			i = ExtractString(Comment, i+1, ColName);
			String_64 ColNameS = ColName;

			// Copied the name - skip any whitespace
			while (((Comment[i] == ' ') || (Comment[i] == '\t')) &&
				   (Comment[i] != 0))
			{
				i++;
			}
			
			// Check for the overprint flag
			if (Comment[i] == 'o')
			{
				// Ignore it for now...
				i++;
				while (((Comment[i] == ' ') || (Comment[i] == '\t')) &&
					   (Comment[i] != 0))
				{
					i++;
				}
			}


			// Validate the colour model for this filter.
			if ((ColModel != 'r') && (ColModel != 'c') && 			// Rgb, Cmyk
			    (ColModel != 'h') && (ColModel != 't'))				// Hsv, Tint
			{
				// Reject outright if this is an ArtWorks file
				if (!CamelotEPS)
				{
					pFilter->HandleEPSError();
					return EPSCommentSyntaxError;
				}
			}

			// Do we need to save the context before adding this colour?
			if (Linked || (ColModel == 't') || (ColModel == 'd'))
			{
				pNewColours->SaveContext();
				LinkNesting++;
			}

			// Read the colour components according to the colour model.
			switch (ColModel)
			{
				case 'r':
				case 'R':
					{
						ColourRGBT NewRGBT;
						ReadEPS_RGB(&NewRGBT, Comment + i, &ColourInfo);
						if (!pNewColours->AddColour(&ColNameS, &NewRGBT, &ColourInfo))
							return EPSCommentSystemError;
					}
					break;

				case 'c':
					{
						ColourCMYK NewCMYK;
						ReadEPS_CMYK(&NewCMYK, Comment + i, &ColourInfo);
						if (!pNewColours->AddColour(&ColNameS, &NewCMYK, &ColourInfo))
							return EPSCommentSystemError;
					}
					break;

				case 'h':
				case 'H':
					{
						ColourHSVT NewHSVT;
						ReadEPS_HSV(&NewHSVT, Comment + i, &ColourInfo);
						if (!pNewColours->AddColour(&ColNameS, &NewHSVT, &ColourInfo))
							return EPSCommentSystemError;
					}
					break;

				case 't':		// Tint
					{
						UINT32 Tint = ReadEPS_Tint(Comment + i);

						if (!pNewColours->AddTint(&ColNameS, Tint))
							return EPSCommentSystemError;
					}
					break;

				case 'd':		// Shade - saved in same format as tint, so re-use ReadEPS_Tint method
					{
						INT32 ShadeX;
						INT32 ShadeY;
						ReadEPS_Shade(Comment + i, &ShadeX, &ShadeY);

						if (!pNewColours->AddShade(&ColNameS, ShadeX, ShadeY))
							return EPSCommentSystemError;
					}
					break;

				case 'G':
				case 'g':
					{
						ColourGreyT NewGreyT;
						ReadEPS_Grey(&NewGreyT, Comment + i, &ColourInfo);

						// Bodge for version 2
						// We really want black to be defined as CMYK so that it colour separates
						// more cleanly (and also merges on import better with new docs). Prior to v2,
						// the default black was defined as a greyscale so we sit on it during import
						// and turn it into a CMYK colour.
						if (NewGreyT.Intensity == 0 && ColNameS == String_64(TEXT("Black")))
						{
							ColourCMYK Bodge;
							Bodge.Cyan = Bodge.Magenta = Bodge.Yellow = 0;
							Bodge.Key = 1.0;

							if (!pNewColours->AddColour(&ColNameS, &Bodge, &ColourInfo))
								return EPSCommentSystemError;
						}
						else
						{
							if (!pNewColours->AddColour(&ColNameS, &NewGreyT, &ColourInfo))
								return EPSCommentSystemError;
						}
					}
					break;
			}

			// Parsed this colour ok.
			return EPSCommentOK;
		}
	}
#endif
	// Ignore all other comments
	return EPSCommentUnknown;
}


/********************************************************************************************

>	INT32 ColourListComponent::ExtractString(const char *Comment, INT32 Start, char *String)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/94
	Inputs:		Comment - pointer to the comment.
				Start - offset into Comment of the first character of the string (not
						including the first opening parenthesis).
	Outputs:	String - the PostScript string.
	Returns:	The offset to the first character after the closing parenthesis of
				the string.
	Purpose:	Copy a PostScript string out of the given EPS comment.  It handles
				nested parentheses - e.g. a string like (Hello (there)) will be
				correctly returned in String as "Hello (there)".
				The end of the comment is also taken as terminating the string.

********************************************************************************************/

INT32 ColourListComponent::ExtractString( PCTSTR Comment, INT32 Start, PTSTR String)
{
//	INT32 i = Start;
//	INT32 j = 0;
//	INT32 StringNesting = 1;
	
	// Look for the last closing Parenthesis in this string...
	const TCHAR		   *End = _tcsrchr(Comment, ')');

	if ((End == NULL) || (End < (Comment + Start)))
	{
		// Something's gone wrong - don't extract string and put a warning where the colour name
		// should be.
		_tcscpy( String, _T("Colour Import Error") );
		return Start;
	}

	// Copy the colour name from the comment into the string provided, and NULL
	// terminate it.
	INT32 NameLength = End - Comment;
	NameLength -= Start;

	_tcsncpy( String, Comment + Start, NameLength );
	String[NameLength] = 0;

	// Return the next character position after the string's closing parenthesis.
	return Start + NameLength + 1;
}

/********************************************************************************************

>	void ColourListComponent::ReadEPS_RGB(ColourRGBT *pCol, char *pComment, 
									  NewColourInfo *pColourInfo)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Inputs:		pCol - the colour to initialise from the comment.
				pComment - pointer to the comment fields to use.
				pColourInfo - if NULL, then this is not a linked colour
							  if non-NULL, then this is a linked colour, and the fields
							    should be filled in according to the inheritance flags
							    found in the colour definition.
	Purpose:	Read the RGB components from an ArtWorks EPS colour table comment, and
				initialise an indexed colour object using these values.
				The comment passed in is expected to be a pointer to the character just
				after the colour name/overprint flag, i.e. the start of the colour
				description.  Leading white space is permissible.
	SeeAlso:	ColourListComponent::ReadEPS_CMYK; ColourListComponent::ReadEPS_HSV;
				ColourListComponent::ReadEPS_Tint

********************************************************************************************/

void ColourListComponent::ReadEPS_RGB( ColourRGBT *pCol, PTSTR pComment, 
								  NewColourInfo *pColourInfo )
{
	BOOL Linked = (pColourInfo != NULL) && (pColourInfo->Type == COLOURTYPE_LINKED);

	PTSTR pszTokMark = NULL ;
	PTSTR Value = tcstok(pComment, _T(" \t"), &pszTokMark);
	if (Value != NULL)
	{
		if (Linked && (_tcscmp(Value, _T("-") ) == 0))
			pColourInfo->Inherits[0] = TRUE;
		else
			pCol->Red = tcstof( Value, &pszTokMark );
	}

	Value = tcstok( NULL, _T(" \t"), &pszTokMark );
	if (Value != NULL)
	{
		if (Linked && (_tcscmp(Value, _T("-") ) == 0))
			pColourInfo->Inherits[1] = TRUE;
		else
			pCol->Green = tcstof( Value, &pszTokMark );
	}

	Value = tcstok( NULL, _T(" \t"), &pszTokMark );
	if (Value != NULL)
	{
		if (Linked && (_tcscmp(Value, _T("-") ) == 0))
			pColourInfo->Inherits[2] = TRUE;
		else
			pCol->Blue = tcstof( Value, &pszTokMark );
	}

	// No transparency
	pCol->Transparent = 0.0;
}

/********************************************************************************************

>	void ColourListComponent::ReadEPS_CMYK(ColourCMYK *pCol, char *pComment, 
									   NewColourInfo *pColourInfo)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Inputs:		pCol - the colour to initialise from the comment.
				pComment - pointer to the comment fields to use.
	Purpose:	Read the CMYK components from an ArtWorks EPS colour table comment, and
				initialise an indexed colour object using these values.
				The comment passed in is expected to be a pointer to the character just
				after the colour name/overprint flag, i.e. the start of the colour
				description.  Leading white space is permissible.
	SeeAlso:	ColourListComponent::ReadEPS_RGB; ColourListComponent::ReadEPS_HSV;
				ColourListComponent::ReadEPS_Tint

********************************************************************************************/

void ColourListComponent::ReadEPS_CMYK(ColourCMYK *pCol, PTSTR pComment, 
								   NewColourInfo *pColourInfo)
{
	PORTNOTETRACE("filters","ColourListComponent::ReadEPS_CMYK - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL Linked = (pColourInfo != NULL) && (pColourInfo->Type == COLOURTYPE_LINKED);

	char *Value = _tcstok(pComment, " \t");
	if (Value != NULL)
	{
		if (Linked && (_tcscmp(Value, "-") == 0))
			pColourInfo->Inherits[0] = TRUE;
		else
			pCol->Cyan = atof(Value);
	}

	Value = _tcstok(NULL, " \t");
	if (Value != NULL)
	{
		if (Linked && (_tcscmp(Value, "-") == 0))
			pColourInfo->Inherits[1] = TRUE;
		else
			pCol->Magenta = atof(Value);
	}

	Value = _tcstok(NULL, " \t");
	if (Value != NULL)
	{
		if (Linked && (strcmp(Value, "-") == 0))
			pColourInfo->Inherits[2] = TRUE;
		else
			pCol->Yellow = atof(Value);
	}

	Value = _tcstok(NULL, " \t");
	if (Value != NULL)
	{
		if (Linked && (_tcscmp(Value, "-") == 0))
			pColourInfo->Inherits[3] = TRUE;
		else
			pCol->Key = atof(Value);
	}
#endif
}

/********************************************************************************************

>	void ColourListComponent::ReadEPS_HSV(ColourHSVT *pCol, char *pComment,
									  NewColourInfo *pColourInfo)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Inputs:		pCol - the colour to initialise from the comment.
				pComment - pointer to the comment fields to use.
	Purpose:	Read the HSV components from an ArtWorks EPS colour table comment, and
				initialise an indexed colour object using these values.
				The comment passed in is expected to be a pointer to the character just
				after the colour name/overprint flag, i.e. the start of the colour
				description.  Leading white space is permissible.
	SeeAlso:	ColourListComponent::ReadEPS_RGB; ColourListComponent::ReadEPS_CMYK;
				ColourListComponent::ReadEPS_Tint

********************************************************************************************/

void ColourListComponent::ReadEPS_HSV(ColourHSVT *pCol, PTSTR pComment,
								  NewColourInfo *pColourInfo)
{
	PORTNOTETRACE("filters","ColourListComponent::ReadEPS_HSV - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL Linked = (pColourInfo != NULL) && (pColourInfo->Type == COLOURTYPE_LINKED);

	char *Value = _tcstok(pComment, " \t");
	if (Value != NULL)
	{
		if (Linked && (_tcscmp(Value, "-") == 0))
			pColourInfo->Inherits[0] = TRUE;
		else
			pCol->Hue = atof(Value) / 360.0;
	}

	Value = _tcstok(NULL, " \t");
	if (Value != NULL)
	{
		if (Linked && (_tcscmp(Value, "-") == 0))
			pColourInfo->Inherits[1] = TRUE;
		else
			pCol->Saturation = atof(Value) / 100.0;
	}

	Value = _tcstok(NULL, " \t");
	if (Value != NULL)
	{
		if (Linked && (_tcscmp(Value, "-") == 0))
			pColourInfo->Inherits[2] = TRUE;
		else
			pCol->Value = atof(Value) / 100.0;
	}

	// No transparency
	pCol->Transparent = 0.0;
#endif
}

/********************************************************************************************

>	void ColourListComponent::ReadEPS_Grey(ColourGreyT *pCol, char *pComment,
									   NewColourInfo *pColourInfo)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Inputs:		pCol - the colour to initialise from the comment.
				pComment - pointer to the comment fields to use.
	Purpose:	Read the HSV components from an ArtWorks EPS colour table comment, and
				initialise an indexed colour object using these values.
				The comment passed in is expected to be a pointer to the character just
				after the colour name/overprint flag, i.e. the start of the colour
				description.  Leading white space is permissible.
	SeeAlso:	ColourListComponent::ReadEPS_RGB; ColourListComponent::ReadEPS_CMYK;
				ColourListComponent::ReadEPS_Tint

********************************************************************************************/

void ColourListComponent::ReadEPS_Grey(ColourGreyT *pCol, PTSTR pComment,
								   NewColourInfo *pColourInfo)
{
	PORTNOTETRACE("filters","ColourListComponent::ReadEPS_Grey - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL Linked = (pColourInfo != NULL) && (pColourInfo->Type == COLOURTYPE_LINKED);

	char *Value = _tcstok(pComment, " \t");
	if (Value != NULL)
	{
		if (Linked && (_tcscmp(Value, "-") == 0))
			pColourInfo->Inherits[0] = TRUE;
		else
			pCol->Intensity = atof(Value);
	}

	// Clear the reserved words to 0.
	pCol->Reserved1 = 0.0;
	pCol->Reserved2 = 0.0;

	// No transparency
	pCol->Transparent = 0.0;
#endif
}

/********************************************************************************************

>	void ColourListComponent::ReadEPS_Tint(char *pComment)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/94
	Inputs:		pComment - pointer to the comment fields to use.
	Purpose:	Read the tint value from an ArtWorks style tint colour definition comment.

				(OR Read the shade value out of a CamelotEPS shade ("d") comment. Shades
				are stored in identical; format to tints)

	SeeAlso:	ColourListComponent::ReadEPS_RGB; ColourListComponent::ReadEPS_CMYK;
				ColourListComponent::ReadEPS_HSVT

********************************************************************************************/

UINT32 ColourListComponent::ReadEPS_Tint( PTSTR pComment )
{
	PTSTR pszTokMark = NULL;
	PTSTR Value = tcstok( pComment, _T(" \t"), &pszTokMark );
	INT32 Tint = 0;
	if (Value != NULL)
	{
		// Get the tint value
		Tint = tcstol( Value, &pszTokMark, 10 );
		if (Tint < 0)
			Tint = 0;
	}

	return (UINT32) Tint;
}



/********************************************************************************************

>	void ColourListComponent::ReadEPS_Shade(char *pComment, INT32 *ResultX, INT32 *ResultY)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/95
	Inputs:		pComment - pointer to the comment fields to use.

	Outputs:	ResultX,ResultY - returned with the shade values

	Purpose:	Read the shade values from a Camelot shade colour definition comment.

				(OR Read the shade value out of a CamelotEPS shade ("d") comment. Shades
				are stored in identical; format to tints)

	Notes:		Old style shades had only one relative value (Y). If this is missing,
				then an appropriate default is chosen such that it all still works.

	SeeAlso:	ColourListComponent::ReadEPS_RGB; ColourListComponent::ReadEPS_CMYK;
				ColourListComponent::ReadEPS_HSVT

********************************************************************************************/

void ColourListComponent::ReadEPS_Shade( PTSTR pComment, INT32 *ResultX, INT32 *ResultY)
{
	PTSTR pszTokMark = NULL;
	PTSTR Value = tcstok( pComment, _T(" \t"), &pszTokMark );
	INT32  Val = 0;
	if( Value != NULL )
		Val = tcstol( Value, &pszTokMark, 10 );

	*ResultY = (INT32)Val;

	Value = tcstok( NULL, _T(" \t"), &pszTokMark );
	if (Value != NULL)
	{
		Val = tcstol( Value, &pszTokMark, 10 );

TRACEUSER( "Jason", _T("Load new shade %ld %ld\n"), Val, (INT32)*ResultY);
	}
	else
	{
		// There is no second token - it must be in the old save format
		// The old value was a 0-100 shade-to-black, so to convert into the new
		// format we just negate it. Happy Happy Bodge Bodge.
		*ResultY = -(*ResultY);
		
		// And set a sensible X value
		Val = 0;
TRACEUSER( "Jason", _T("Load OLD shade %ld %ld\n"), Val, (INT32)*ResultY);
	}

	*ResultX = (INT32)Val;
}




/********************************************************************************************

>	void ColourListComponent::EndOfEPSComment(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
	Purpose:	Informs the document component that the comment it has been decoding has
				now finished, and it should take whatever action required to act on
				the comment.
	SeeAlso:	DocComponent::EndOfPESComment

********************************************************************************************/

void ColourListComponent::EndOfEPSComment(EPSFilter *pFilter)
{
	// Do any cleanup we need to here after loading in a colour table...
}



/********************************************************************************************

>	IndexedColour *ColourListComponent::FindNamedColour(const char *pName, 
														DocColour *ColDef = NULL,
													    UINT32 Tint = 100,
													    BOOL Strict = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/08/94
	Inputs:		pName - the name of the colour to find.
				ColDef - the colour definition that it should match.
				Tint - the tint value applied to ColDef (NB it should already have been
					   applied); 100 = same colour, 0 = white.

				Strict - not used (obsolete)

	Returns:	Pointer to the indexed colour that corresponds to the file's named colour;
				or NULL if the colour could not be found (the caller should fall back to
				   an immediate colour in this case).
	Purpose:	Given the name of an imported named colour, return the relevant indexed
				colour.  This allows attributes to be added when importing that use
				named colours.
				If ColDef is not NULL, then the Indexed colour found is checked to see
				if it agrees with the definition in ColDef - if not, then a new indexed
				colour is made, and a pointer to that is returned instead.  This means we
				can cope with brain-damaged formats that have two or more named colours
				with the same format.  It also makes it easy to cope with formats that
				use unnamed colours, as we just keep making colours called "Unnamed colour"
				or similar, and this function works it all out for us.

				If the named colour is not found, and ColDef is not NULL, then again a
				new Indexed colour is created, and a pointer to it is returned.

				NB. The name of the indexed colour returned MAY NOT have the same name as
				specified in pName, because it may clash with colours already in the
				document, and hence may have been mangled to make it unambiguous.

********************************************************************************************/

IndexedColour *ColourListComponent::FindNamedColour( PCTSTR pName, 
													DocColour *ColDef,
													UINT32 Tint,
													BOOL Strict)
{
	// Sanity check
	ENSURE(pNewColours != NULL, "Someone asked for an imported colour when there is no"
							   "import going on!");

	if (pNewColours == NULL)
		return NULL;

	// Special case for tinted colours - if the Tint is not 100%, then we must look for
	// or create a tinted version of the colour.
	if (Tint != 100)
	{
		// Create a new name to indicate this is tinted
		String_64 NewName(pName);

		// Work out where to add the tint comment to the colour name.
		// Comment is of the form: " (xx% tint)", which is 11 characters long.
		INT32 Len = NewName.Length();
		if (Len > 50)
		{
			// This should give us enough room for our tint comment
			Len = 50;
		}

		TCHAR		   *pNewName = (TCHAR *)NewName;
		tsprintf( pNewName + Len, 64 - Len, _T(" (%d%% tint)"), Tint );

		// Ok - we have a name - recurse so we can get a new colour (or use an existing
		// one created by this bit of code).
		return FindNamedColour(NewName, ColDef);
	}

	// Find the colour in the imported colour table.
	IndexedColour *pColour;
	PColourCMYK CMYK;
	ColDef->GetCMYKValue(&CMYK);
	pColour = pNewColours->GetColour( pName, &CMYK );

	// Is there one that fits the bill?
	if (pColour != NULL)
		return pColour;

	// Colour not found - can we make a new one?
	if (ColDef != NULL)
	{
		// Yes - add a new colour...

		// Make a proper definition:
		ColourCMYK FullCMYK;
		FullCMYK.Cyan    = FIXED24( ((double) CMYK.Cyan) 	/ 255.0 );
		FullCMYK.Magenta = FIXED24( ((double) CMYK.Magenta) / 255.0 );
		FullCMYK.Yellow  = FIXED24( ((double) CMYK.Yellow) 	/ 255.0 );
		FullCMYK.Key     = FIXED24( ((double) CMYK.Key) 	/ 255.0 );

		// And use it to add the colour
		String_64 NewName(pName);
		if (pNewColours->AddColour(&NewName, &FullCMYK))
		{
			// It worked - return the colour created
			return pNewColours->GetColour( pName, &CMYK );
		}
		else
		{
			// Failed to add a new colour
			return NULL;
		}
	}

	// Can't make a new one without a colour definition - give up.
	return NULL;
}


/********************************************************************************************

>	BOOL ColourListComponent::FindIdenticalColour(IndexedColour *pColour,
												  IndexedColour **pResult)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/94
	Inputs:		pColour - the colour to match with.
	Outputs:	pResult - pointer to the matching colour, if found;
						  NULL otherwise.
	Returns:	TRUE if a colour with this *name* was found in the document;
				FALSE if not.
	Purpose:	Find out if there is already a colour exactly the same as the one passed
				in in the existing colour list.  If so, return a pointer to it, otherwise
				return NULL.
				Also indicates whether or not a colour with this name already exists in the
				document, regardless of whether the colour data is the same.

********************************************************************************************/

BOOL ColourListComponent::FindIdenticalColour(IndexedColour *pColour,
											  IndexedColour **pResult)
{
	IndexedColour *pItem = (IndexedColour *) pIndexedColours->GetHead();

	BOOL FoundSameName = FALSE;

	// Get the name/ID for this colour. TRUE is passed in so that unnamed colours return
	// their unique id string rather than "Local colour"
	String_64 *Name1 = pColour->GetName(TRUE);

	while (pItem != NULL)
	{
		// Don't bother with deleted colours
		if (!pItem->IsDeleted())
		{
			// WEBSTER - markn 14/2/97
			// Introduced WEBSTER_IGNORE_NAME_MATCHING so that Webster can ignore the name when
			// trying to find an identical colour

			#if WEBSTER_IGNORE_NAME_MATCHING 
			if (pIndexedColours != NULL)
			{
				if (!pItem->IsDifferent(*pColour, COL_ACCURACY))
				{
					if (ColourSGallery::CanAddColourToGallery(pItem,pIndexedColours))
					{
						*pResult = pItem;
						return TRUE;
					}
				}
			}
			#else
			// Compare names
			String_64 *Name2 = pItem->GetName(TRUE);
			if ((*Name1) == (*Name2))
			{
				FoundSameName = TRUE;

				// Name is the same - what about colour model + definition?
				if (!pItem->IsDifferent(*pColour, COL_ACCURACY))
				{
					// The colour defn the same too - it's a match
					*pResult = pItem;
					return TRUE;
				}
			}
			#endif // WEBSTER_IGNORE_NAME_MATCHING 
		}

		// Try the next colour
		pItem = (IndexedColour *) pIndexedColours->GetNext(pItem);
	}

	// No match - tell caller whether we found one with the same name.
	*pResult = NULL;
	return FoundSameName;
}








/********************************************************************************************

>	IndexedColour *ColourListComponent::ExactMatchExists(IndexedColour *SourceColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/94

	Inputs:		SourceColour - The colour to match
	Returns:	A pointer to the IndexedColour in this document which exactly matches the
				given SourceColour, or NULL if no matching colour exists.

	Purpose:	Determine if a colour is already available in the paste-destination document
				This allows us to merge IndexedColours on the clipboard with those in the
				target document when doing a paste operation.

	Notes:		Exact match means: Another non-deleted colour which is not IsDifferent
				and which has an identical (sortof - see below) name string.

				SPECIAL behaviour: The name string match now ignores digits on the end
				of the string. This is so that if you copy and paste "Red" such that the pasted
				colour is "Red 2", a subsequent paste will match the source "Red" and the
				destination "Red 2", rather than adding an identical "Red 3"!

				Unnamed colours will never match.

	SeeAlso:	ColourListComponent::CopyColourAcross

********************************************************************************************/

IndexedColour *ColourListComponent::ExactMatchExists(IndexedColour *SourceColour)
{
	// If it's an unnamed (local) colour, then we don't bother looking for a match
	if (!SourceColour->IsNamed())
		return(NULL);

	IndexedColour *pItem = (IndexedColour *) pIndexedColours->GetHead();

	// --- First check for any *exact* matches
	// Get the name/ID for this colour. TRUE is passed in so that unnamed colours return
	// their unique id string rather than "Local colour" (hmmm... we aren't checking 
	// unnamed colours, so this is a bit of a moot point! ;-)
	String_64 SourceName = *(SourceColour->GetName(TRUE));

	while (pItem != NULL)
	{
		if (!pItem->IsDeleted() && !pItem->IsDifferent(*SourceColour))	// not different, so check name
		{
			if (SourceName.CompareTo(*pItem->GetName(TRUE)) == 0)		// If has same name then...
				return(pItem);											// Return it as match
		}

		// Try the next colour
		pItem = (IndexedColour *) pIndexedColours->GetNext(pItem);
	}


	// --- Second, check for near-matches.
	// This is an exact match except it ignores the space and digits appended to a name
	// in the quest for a unique name. This means if the name "Red" is in use, and we paste
	// a different colour called "Red" it will add "Red 2" the first paste, but subsequent
	// pastes will match "Red 2" and simply merge with that colour (rather than adding Red 2
	// Red 3, Red 4, ect ad infinitum!)
	pItem = (IndexedColour *) pIndexedColours->GetHead();
	while (pItem != NULL)
	{
		if (!pItem->IsDeleted() && !pItem->IsDifferent(*SourceColour))			// not different, so check name
		{
			// Get the string as a TCHAR array so we can actually do something with it (grr)
			TCHAR *StrPtr = (TCHAR *) (*pItem->GetName(TRUE));

			// Find the zero terminator
			INT32 Index = 0;
			while (StrPtr[Index] != (TCHAR)'\0')
				Index++;

			// Now make index point at the last character
			Index -= 1;
			if (Index > 0)
			{
				if (UnicodeManager::IsDBCSLeadByte(StrPtr[Index-1]))
					Index -= 1;
			}

			// Scan backwards, skipping all digits that we find
			INT32 EndIndex = Index;
			while (Index > 0)
			{
				if (!StringBase::IsNumeric(StrPtr[Index]))
					break;

				Index -= 1;
				if (Index > 0)
				{
					if (UnicodeManager::IsDBCSLeadByte(StrPtr[Index-1]))
						Index -= 1;
				}
			}

			// Now, if we found digits, scan back over this char if it is a space
			if (Index > 0 && Index < EndIndex && StrPtr[Index] == (TCHAR)' ')
			{
				// Rightoh. This looks like an auto-generated unique-name, so does the text
				// without the unique-ness number match our name?
				String_64 ToCheck;
				pItem->GetName(TRUE)->Left(&ToCheck, Index);

				if (SourceName.CompareTo(ToCheck) == 0)
					return(pItem);					// Yep - the names match
			}
		}

		// Try the next colour
		pItem = (IndexedColour *) pIndexedColours->GetNext(pItem);
	}

	// No match was found for this colour
	return(NULL);
}



/********************************************************************************************

>	virtual BOOL ColourListComponent::StartComponentCopy();

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if it succeeded. FALSE if the copy must be aborted
	Purpose:	This function gets called to prepare for a copy of indexed colours into
				the document. It sets up the various arrays and other thangs which the
				colour copy will need.
	Errors:		ENSURE failure if called when a component copy is already under way.
				Errors will be reported if memory is unavailable
	SeeAlso:	ColourListComponent::EndComponentCopy;
				ColourListComponent::AbortComponentCopy;
				ColourListComponent::CopyColourAcross

********************************************************************************************/

BOOL ColourListComponent::StartComponentCopy()
{
	// Make sure the fn has not been called before
	if (SourceColours != NULL || DestColours != NULL)
	{
		ERROR3("StartComponentCopy already called");
		return(TRUE);
	}

	ColourTableSize = 64;			// Start off with 64-entry table. This may grow if
									// we find we have a lot of colours to copy over

	SourceColours = (IndexedColour **) CCMalloc(ColourTableSize * sizeof(IndexedColour *));
	if (SourceColours == NULL)
	{
		InformError();				// Out of memory - inform the user
		return(FALSE);
	}

	DestColours = (IndexedColour **) CCMalloc((ColourTableSize+1) * sizeof(IndexedColour *));
	if (DestColours == NULL)
	{
		InformError();				// Out of memory - inform the user

		CCFree(SourceColours);		// And release our other memory - we can't use it now
		SourceColours = NULL;
		return(FALSE);
	}

	MergeColours = (BOOL *) CCMalloc(ColourTableSize * sizeof(BOOL));
	if (MergeColours == NULL)
	{
		InformError();				// Out of memory - inform the user

		CCFree(SourceColours);		// And release our other memory - we can't use it now
		CCFree(DestColours);
		SourceColours = DestColours = NULL;
		return(FALSE);
	}

	// Initialise the arrays
	for (INT32 i = 0; i < ColourTableSize; i++)
	{
		SourceColours[i] = DestColours[i] = NULL;
		MergeColours[i]  = FALSE;
	}

	DestColours[ColourTableSize] = NULL;	// And add a NULL terminator to dest. list

	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL ColourListComponent::EndComponentCopy()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/94

	Returns:	TRUE if it succeeded
	Purpose:	Will commit all changes made by doing the component copy, returning TRUE.
				Any new colours will be inserted as instructed by the InsertBefore parameter.
				If some colours will not be insterted because they were merged, then a report
				to this effect can be given if desired.

	Notes:		May be called multiple times - subsequent calls will be ignored

				Overrides the base class method. Calls an internal variant on this method.

	SeeAlso:	ColourListComponent::StartComponentCopy;
				ColourListComponent::AbortComponentCopy;
				ColourListComponent::CopyColourAcross

********************************************************************************************/

BOOL ColourListComponent::EndComponentCopy()
{
	return(ColourListComponent::EndComponentCopy(NULL, FALSE));
}



/********************************************************************************************

>	virtual BOOL ColourListComponent::EndComponentCopy(IndexedColour *InsertBefore,
														BOOL Report);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/94
	Inputs:		InsertBefore - NULL to insert at the end of the colour list, else a pointer
				the the IndexedColour before which you wish to insert the colours.

				Report - FALSE to do it quietly, TRUE if you'd like a report given if any colours
				were merged (used by the colour gallery when copying colours, as merged colours
				will fail to appear in the chunk of copied colours, which can confuse the user)

	Returns:	TRUE if it succeeded
	Purpose:	Will commit all changes made by doing the component copy, returning TRUE.
				Any new colours will be inserted as instructed by the InsertBefore parameter.
				If some colours will not be insterted because they were merged, then a report
				to this effect can be given if desired.

	Notes:		May be called multiple times - subsequent calls will be ignored

				WARNING: This is NOT an override of the base class method. Be careful

	SeeAlso:	ColourListComponent::StartComponentCopy;
				ColourListComponent::AbortComponentCopy;
				ColourListComponent::CopyColourAcross

********************************************************************************************/

BOOL ColourListComponent::EndComponentCopy(IndexedColour *InsertBefore, BOOL Report)
{
	BOOL NeedToReport = FALSE;

	if (SourceColours != NULL)	// Free the source mapping array
	{
		CCFree(SourceColours);
		SourceColours = NULL;
	}

	if (DestColours != NULL)
	{
		// Add the colours in the 'Destination' array to the ColourList for the dest. doc.
		// We only add those colours which have not been merged with existing colours in the
		// destination list. The AddItem() call ensures their names are unique in this doc.
		INT32				i;
		for ( i = 0; i < ColourTableSize; i++)
		{
			if (DestColours[i] != NULL && !MergeColours[i])
			{
				if (InsertBefore == NULL || !DestColours[i]->IsNamed())
					pIndexedColours->AddItem(DestColours[i]);
				else
					pIndexedColours->InsertBefore(InsertBefore, DestColours[i]);
			}
		}

		// Now, remove all merged colours from the list of colours to be 'unhidden' for undo
		// NOTE that DestColours is guaranteed to be NULL terminated

		// We delete each merged colour from the list by copying the first valid list item
		// over the top of the merged item; we keep track of the valid list start with
		// 'ListStart'. Once finished, we pass any remainder of the array to UnHideColours
		INT32 ListStart = 0;
		for (i = 0; i < ColourTableSize; i++)
		{
			if (DestColours[i] == NULL)			// Stop at NULL terminator
				break;

			if (MergeColours[i] || !DestColours[i]->IsNamed())
			{
				// A merged or unnamed colour - copy list item '0' over it, so that we do not
				// try to do any undo record for it.
				DestColours[i] = DestColours[ListStart];
				DestColours[ListStart] = NULL;
				ListStart++;

				NeedToReport = TRUE;
			}
		}

		if (DestColours[ListStart] != NULL)		// If there are non-merged named colours, create undo
			ColourManager::UnHideColours(pIndexedColours, &DestColours[ListStart]);

		// Lose our workspace array of pointers to these colours
		CCFree(DestColours);
		DestColours = NULL;
	}

	if (MergeColours != NULL)	// Free the 'merged'-flag array
	{
		CCFree(MergeColours);
		MergeColours = NULL;
	}

	// If the caller wants us to, report (if necessary) that some of the colours
	// were not copied, as they were already present in the dest. doc
	if (Report && NeedToReport)
		InformMessage(_R(IDS_COLOURSMERGED), _R(IDS_OK));

	return(TRUE);
}



/********************************************************************************************

>	virtual void ColourListComponent::AbortComponentCopy();

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Will abort all changes made for component copies.
				This means that things such as the colours referenced by the nodes
				you have just copied do not exist, so you must clean up to the state
				the document was in before you strated copying.
	Errors:		-
	Notes:		May be called even if StartComponentCopy has not been called.
				May be called multiple times

	SeeAlso:	ColourListComponent::EndComponentCopy;
				ColourListComponent::AbortComponentCopy;
				ColourListComponent::CopyColourAcross

********************************************************************************************/

void ColourListComponent::AbortComponentCopy()
{
	// Clean up by deleting all the new indexedColours we created. We hope that by the
	// time we do this, all DocColours referencing these colours will have been vaped.
	if (DestColours != NULL)
	{
		for (INT32 i = 0; i < ColourTableSize; i++)
		{
			if (DestColours[i] != NULL && !MergeColours[i])
			{
				delete DestColours[i];
				DestColours[i] = NULL;
			}
		}
	}

	// And release our working arrays
	if (SourceColours != NULL)
	{
		CCFree(SourceColours);
		SourceColours = NULL;
	}

	if (DestColours != NULL)
	{
		CCFree(DestColours);
		DestColours = NULL;
	}

	if (MergeColours != NULL)
	{
		CCFree(MergeColours);
		MergeColours = NULL;
	}
}



/********************************************************************************************

>	INT32 ColourListComponent::ExtendTables(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94 (Separated from CopyColourAcross)
	Inputs:		-
	Outputs:	(The mapping arrays may have been moved by realloc)
	Returns:	-1 if it fails (it will have already reported the error
				else the index of the first free entry in the mapping tables

	Purpose:	Called by Component Copy routines to extend the size of the copy mapping
				tables (SourceColours[], DestColours[], and MergeColours[]). This returns
				the index of the first free entry in the new tables, or -1 if it fails.

	Errors:		Memory errors are reported immediately

	SeeAlso:	ColourListComponent::StartComponentCopy;
				ColourListComponent::AbortComponentCopy;
				ColourListComponent::EndComponentCopy

********************************************************************************************/

INT32 ColourListComponent::ExtendTables(void)
{
	INT32 i = ColourTableSize;	// Set the index to the first new array entry

	ColourTableSize += 64;		// Increase the size of our mapping arrays
	IndexedColour **TempArray;

	// Realloc the Source array, with error handling
	TempArray = (IndexedColour **) CCRealloc(SourceColours,
											ColourTableSize * sizeof(IndexedColour *));
	if (TempArray == NULL)
	{
		InformError();
		return(-1);
	}

	SourceColours = TempArray;	// Realloc succeeded - set the new SourceColour ptr
	
	// Realloc the Dest array, with error handling
	TempArray = (IndexedColour **) CCRealloc(DestColours,
											(ColourTableSize+1) * sizeof(IndexedColour *));
	if (TempArray == NULL)
	{
		InformError();
		return(-1);
	}

	DestColours = TempArray;	// Realloc succeeded - set the new DestColour ptr

	// Realloc the 'Merged'-flag array, with error handling
	BOOL *TempBoolArray = (BOOL *) CCRealloc(MergeColours,
											ColourTableSize * sizeof(BOOL));
	if (TempBoolArray == NULL)
	{
		InformError();
		return(-1);
	}

	MergeColours = TempBoolArray;	// Realloc succeeded - set the new MergeColour ptr

	// Initialise the new portions of the arrays
	for (INT32 j = i; j < ColourTableSize; j++)
	{
		SourceColours[j] = DestColours[j] = NULL;
		MergeColours[j] = FALSE;
	}

	DestColours[ColourTableSize] = NULL;	// And add a NULL terminator to dest. list

	return(i);
}



/********************************************************************************************

>	IndexedColour *ColourListComponent::MapColour(IndexedColour *SourceColour,
													BOOL *AlreadyThere = NULL,
													BOOL SourceIsUnique = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/94

	Inputs:		SourceColour - pointer to an IndexedColour to map

				(AlreadyThere is an output only)

				SourceIsUnique - FALSE (the default) if you're copying existing document
								 colours around, and therefore could attempt to copy the
								 same colour multiple times.

								 TRUE if you can guarantee that this colour will only be
								 copied once. This is used only for copying library colours
								 into a document in the colour gallery (where it uses a temporary
								 IndexedColour, which causes problems when several different
								 colours accidentally map to the same temporary memory allocation,
								 thus causing rampant incorrect colour merging to occur).

	Outputs:	AlreadyThere - (if non-NULL) is filled in with TRUE if the colour was merged
				or FALSE if a new colour had to be created.

	Returns:	NULL if it fails (the error will have been reported)
				Otherwise, a pointer to the IndexedColour resulting from the mapping

	Purpose:	Used internally in ColourListComponent Component Copy routines.
				This method keeps track of mappings between source document and destination
				document colours. It determines the mapping from source to dest, and returns
				the appropriate destination-docuemnt IndexedColour. This may be a new colour
				which will later be added to the destination document or a colour already in
				the dest. doc (merged colour). Once an IndexedColour is mapped, future calls
				to this method will be very efficient (a simple table search and lookup of
				the previous mapping)

	Notes:		This method is recursive. This allows it to cope with a chain of parent
				tint/link colours which may or may not be mapped already. Special care
				is taken to ensure that only destination-document references are left
				at the end of the copy operation (as obviously, references to the source
				doc are highly dangerous, and usually fatal)

	SeeAlso:	ColourListComponent::StartComponentCopy;
				ColourListComponent::AbortComponentCopy;
				ColourListComponent::EndComponentCopy

********************************************************************************************/

IndexedColour *ColourListComponent::MapColour(IndexedColour *SourceColour,
												BOOL *AlreadyThere, BOOL SourceIsUnique)
{
	if (AlreadyThere != NULL)	// Set a safe return value
		*AlreadyThere = FALSE;

	// First, check if the colour is already mapped; if so, just return its mapping
	// If SourceIsUnique, then we know no two source colours will never be the same,
	// and to fix a temporary-colour-reuse (2 colours in the same memory location)
	// problem in colour gallery library colour copies, we don't cache the mappings.
	// This is fine, because those copies only copy each colour once, so there is
	// no need to cache mappings for them anyway.
	if (!SourceIsUnique)
	{
		INT32 i = 0;
		while (i < ColourTableSize && SourceColours[i] != NULL && SourceColours[i] != SourceColour)
			i++;

		if (i < ColourTableSize && SourceColours[i] == SourceColour)
		{
			// If it was merged, return a value indicating that	case
			if (AlreadyThere != NULL && MergeColours[i])
				*AlreadyThere = TRUE;

			return(DestColours[i]);
		}
	}

	// Next, copy the colour being mapped
	IndexedColour *DestColour = new IndexedColour(*SourceColour);
	if (DestColour == NULL)
		return(NULL);


	// Now ensure our parent/ancestors are mapped, ...
	IndexedColour *SourceParent = SourceColour->FindLinkedParent();
	IndexedColour *DestParent = NULL;
	if (SourceParent != NULL)
	{
		DestParent = MapColour(SourceParent);
		if (DestParent == NULL)
		{
			delete DestColour;
			return(NULL);
		}
	}

	// ... and set the copied colour's parent to the mapped (dest doc) parent, rather than
	// the source doc version of the parent.
	// Note that if this is not a tint/linked colour, this has the side effect of ensuring
	// that Parent is NULL, rather than referencing a parent in a different document
	DestColour->SetLinkedParent(DestParent, DestColour->GetType());
	

	// Finally, map ourselves...
	// Find a blank entry in the mapping tables
	INT32 i = 0;
	while (i < ColourTableSize && SourceColours[i] != NULL)
		i++;

	if (i >= ColourTableSize)	// If no free entries, extend them and get first free entry
		i = ExtendTables();

	if (i < 0)		 			// We failed - no memory for tables
	{
		delete DestColour;
		return(NULL);
	}

	// Check if there is already a matching colour in the dest. document.
	// If there is one, then chuck away DestColour, and use the matching colour instead
	IndexedColour *MatchColour = ExactMatchExists(DestColour);
	if (MatchColour != NULL)	
	{
		delete DestColour;
		DestColour = MatchColour;

		if (AlreadyThere != NULL)	// And return info to indicate that the colour is merged
			*AlreadyThere = TRUE;
	}

	// And finally, write the new entry into the mapping tables
	// (but only if we need to cache the mappings - NOTE that we need to store a
	// mapping (at least to the extent of having a destination part of the mapping)
	// or the new colours won't be added in EndComponentCopy. Unfortunately this
	// means we need a source mapping that isn't NULL or it all falls apart.
	if (SourceIsUnique)
		SourceColours[i] = (IndexedColour *) 1;		// Yeek! Have to stop this being null
	else
		SourceColours[i] = SourceColour;

	DestColours[i]   = DestColour;
	MergeColours[i]  = (MatchColour != NULL);

	return(DestColour);		// And return the result of the mapping operation

	return NULL;
}



/********************************************************************************************

>	ColCompCopyResult CopyColourAcross(DocColour* pDocColour, BOOL SourceIsUnique = FALSE);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/94

	Inputs:		pDocColour - The colour which resulted from copying across from one document
				to our document.

				SourceIsUnique - FALSE (the default) if you're copying existing document
								 colours around, and therefore could attempt to copy the
								 same colour multiple times.

								 TRUE if you can guarantee that this colour will only be
								 copied once. This is used only for copying library colours
								 into a document in the colour gallery (where it uses a temporary
								 IndexedColour, which causes problems when several different
								 colours accidentally map to the same temporary memory allocation,
								 thus causing rampant incorrect colour merging to occur).
								 ** ONLY USE THIS IF YOU REALLY REALLY BELIEVE YOU MUST - **
								 ** IT SHOULD NOT BE NECESSARY UNDER NORMAL CIRCUMSTANCES **

	Returns:	CCCOPY_FAILED = 0/FALSE	- if it failed, in which case the error has been reported

				CCCOPY_NEWCOLOUR		- if it succeeded by creating a new colour

				CCCOPY_MERGEDCOLOUR		- if it succeeded by merging (using an existing colour)

	Purpose:	If the DocColour references an IndexedColour then a copy of that IxColour is
				made and remembered. The DocColour is then changed to reference the new
				indexed colour. On calling EndComponentCopy, these new IndexedColours will
				be added to the document. If a failure occurs at any stage, you must
				*immediately* undo the entire copy operation, deleting all the nodes
				referencing the new colours we've created, before calling AbortComponentCopy

				Chains of linked/tint IndexedColours are also handled properly.

	Notes:		If this function returns FALSE, you MUST clean up and then call 
				AbortComponentCopy to clean up.

	Errors:		Memory errors are reported immediately
				If StartComponentCopy has not been called, or an internal consistency failure
				occurs (our working array pointer(s) are NULL), an ERROR2 will be reported.

	SeeAlso:	ColourListComponent::StartComponentCopy;
				ColourListComponent::AbortComponentCopy;
				ColourListComponent::EndComponentCopy

********************************************************************************************/

ColCompCopyResult ColourListComponent::CopyColourAcross(DocColour* pDocColour, BOOL SourceIsUnique)
{
	IndexedColour *SourceColour = pDocColour->FindParentIndexedColour(); 

	if (SourceColour == NULL)	// Doesn't reference an IndexedColour, so return success
		return(CCCOPY_MERGEDCOLOUR);

	if (SourceColours == NULL || DestColours == NULL || MergeColours == NULL)
	{
		ERROR2RAW("ColourListComponent::CopyColourAcross called without a prior StartComponentCopy");
		InformError();
		return(CCCOPY_FAILED);
	}

	BOOL WasMerged = FALSE;
	IndexedColour *NewColour = MapColour(SourceColour, &WasMerged, SourceIsUnique);
	if (NewColour == NULL)		// An error (no memory) occurred - abort component copy
		return(CCCOPY_FAILED);

	// Fix the colour to reference the destination copy of the indexed colour
	pDocColour->MakeRefToIndexedColour(NewColour);
	return((WasMerged) ? CCCOPY_MERGEDCOLOUR : CCCOPY_NEWCOLOUR);

	return CCCOPY_FAILED;
}


/********************************************************************************************

>	IndexedColour* ColourListComponent::GetIndexedParentOfColour(DocColour Colour)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/2000
	Inputs:		Colour - the doccolour that was originally created from an indexed colour
	Outputs:	-
	Returns:	the indexed colour that originally created Colour, if there is one, else NULL
	Purpose:	It is possible for us to create doccolours from indexed colours and then convert
				them so that they are standalone.  This method goes through all the indexed
				colours to try and find out which indexed colour (if any) originally created the input

********************************************************************************************/

IndexedColour* ColourListComponent::GetIndexedParentOfColour(DocColour Colour)
{
	ColourList* pList = GetColourList();
	if (pList == NULL)
		return NULL;

	ColourGeneric ColDef;
	ColourContext *cc = ColourManager::GetColourContext(Colour.GetColourModel());
	ERROR2IF(cc == NULL, NULL,  "Can't find colour context?!");
	DocColour ConvertedDocCol;
	
	IndexedColour* pItem = (IndexedColour*)pList->GetHead();

	// what we have to do is get each indexed colour as a standalone definition then
	// ask it to make a doccolour. If this is the same as our input then we have our match
	while (pItem != NULL)
	{
		// Get the IndexedColour definition as a standalone colour definition
		cc->ConvertColour(pItem, &ColDef);

		// Make the DocColour into a simple standalone "lookalike" of the parent colour
		ConvertedDocCol = DocColour(Colour.GetColourModel(), &ColDef);
		
		// now test to see if its the same, if so we've got what we want
		if (ConvertedDocCol == Colour)
			break;

		pItem = (IndexedColour*)pList->GetNext(pItem);
		
	
	}
	return pItem;
}
