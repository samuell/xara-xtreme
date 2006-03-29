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

// Encapsulates a set of colours imported from a file.

/*
*/

#include "camtypes.h"

#include "impcol.h"

#include "colcomp.h"	// colour document component
//#include "resource.h"
#include "fixmem.h"
#include "colormgr.h"
#include "progress.h"
//#include "ben.h"
//#include "filtrres.h"	// _R(IDS_NATIVE_SORTCOLOURS)

DECLARE_SOURCE("$Revision: 662 $");


NewColourInfo::NewColourInfo()
{
	// Initially a normal colour
	Type = COLOURTYPE_NORMAL;

	// If linked, defaults to no inheritance.
	for (INT32 i = 0; i < 4; i++)
		Inherits[i] = FALSE;

	// If tinted, then no change in colour
	TintValue = 100.0;

	// By default tints are normal tints, not shades.
	TintIsShade = FALSE;

// New bits for native/web file filter
	// By default we are not defining a web or native style colour
	WebNativeColour = FALSE;
	// By default this colour has not been imported yet so flag this in our variables
	pParentCol = NULL;
	RecordNumber = 0L;
	EntryNumber = 0L;
}

/********************************************************************************************

>	class ContextItem : public ListItem

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/94
	Purpose:	Provides information on a context level - see ColourImportContext for
				more detail.
	SeeAlso:	ColourImportContext

********************************************************************************************/

class ContextItem : public ListItem
{
	CC_DECLARE_MEMDUMP(ContextItem)

public:
	ContextItem(IndexedColour *pContext) : pCol(pContext) {}

public:
	IndexedColour *pCol;

private:
	// Prevent default constructor from being used.
	ContextItem();
};




CC_IMPLEMENT_MEMDUMP(NewColour, ListItem)
CC_IMPLEMENT_MEMDUMP(NewColourList, List)
CC_IMPLEMENT_MEMDUMP(ImportedColours, CCObject)
CC_IMPLEMENT_MEMDUMP(ColourImportContext, List)
CC_IMPLEMENT_MEMDUMP(ContextItem, ListItem)


// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


ColourImportContext::ColourImportContext()
{
}

ColourImportContext::~ColourImportContext()
{
	// Just lose all the items in our list.
	DeleteAll();
}

BOOL ColourImportContext::Init()
{
	// Add an initial context item
	ContextItem *pItem = new ContextItem(NULL);
	if (pItem == NULL)
		return FALSE;

	// Add to the list
	AddTail(pItem);

	// All ok
	return TRUE;
}


void ColourImportContext::SetContext(IndexedColour *pCol)
{
	// Get the current context item (always the last)
	ContextItem *pItem = (ContextItem *) GetTail();

	// Update its colour pointer
	pItem->pCol = pCol;
}


IndexedColour *ColourImportContext::GetContext()
{
	// Get the current context item (always the last)
	ContextItem *pItem = (ContextItem *) GetTail();

	// Return its colour pointer
	return pItem->pCol;
}

BOOL ColourImportContext::SaveContext()
{
	// Get a pointer to the current context node
	ContextItem *pCurrentItem = (ContextItem *) GetTail();

	// Add a new context node to the end of the list, and give it the same
	// context as the current context node.
	ContextItem *pItem = new ContextItem(pCurrentItem->pCol);
	if (pItem == NULL)
		return FALSE;

	// Add to the list
	AddTail(pItem);

	// All ok
	return TRUE;
}

BOOL ColourImportContext::RestoreContext()
{
	// Make sure we can do this - must always have at least one node
	if (GetCount() <= 1)
	{
		ERROR3("Bad call to ColourImportContext::RestoreContext()");
		return FALSE;
	}

	// Remove and delete the last context item
	delete RemoveTail();

	// All ok
	return TRUE;
}

BOOL ColourImportContext::RestoreContextTo(UINT32 NewLevel)
{
	// How many should we delete?
	INT32 NumToDelete = (INT32) (GetCount() - NewLevel) - 1;

	// Make sure this is a legal request.
	if (NumToDelete < 0)
		// No - return failure
		return FALSE;

	// Ok - delete this many items
	while (NumToDelete > 0)
	{
		if (!RestoreContext())
		{
			ERROR3("Bad call to ColourImportContext::RestoreContextTo()");
			return FALSE;
		}

		NumToDelete--;
	}

	// All ok
	return TRUE;
}



/********************************************************************************************

>	NewColour::NewColour(IndexedColour *pNewCol, BOOL AlreadyExists)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		pNewCol - the colour to put in the list.
				AlreadyExists - TRUE if pNewCol points to a colour that already exists in
								the destination document;
								FALSE if pNewCol poinst to a brand new colour.
	Purpose:	Create a new colour list item from the given indexed colour.
	SeeAlso:	NewColourList; NewColourList::AddColour

********************************************************************************************/

NewColour::NewColour(IndexedColour *pNewCol, BOOL AlreadyExists)
{
	// Initialise to sensible values.
	pCol = pNewCol;
	AlreadyExistsInDoc = AlreadyExists;
	Duplicate = FALSE;
	pNextDuplicate = NULL;

	// Get CMYK version of this colour for comparisons - this looks a bit nasty, but 
	// actually I think it's the cleanest way of doing this.
	DocColour TempCol;
	TempCol.MakeRefToIndexedColour(pNewCol);
	TempCol.GetCMYKValue(&ColDefn);

	// New web/native filter bits
	RecordNumber = 0L;
	EntryNumber = 0L;
}


/********************************************************************************************

>	BOOL NewColour::AddDuplicateColour(IndexedColour **pNewCol,
									   BOOL AlreadyExists)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		pNewCol - (pointer to a) pointer to the IndexedColour object to link to pMaster
						  because they both have the same colour name.

				AlreadyExists - TRUE => pNewCol matches a colour that is already in the
										destination document.
							   FALSE => pNewCol does not match any colours in the 
										destination document.

	Outputs:	pNewCol pointer is updated to point at the colour to use for the colour
				passed in (see below).

	Returns:	TRUE if the duplicate could be added;
				FALSE if out of memory => ERROR1

	Purpose:	Add another definition of a colour to an existing colour definition.
				i.e. We have a new colour but we have already found a definition of this
				colour name while importing, so we have duplicate colour names, so the
				IndexedColour object pointed to by pNewCol should be linked to the
				existing item (with which it shares a colour name).

	Notes:		AddDuplicateColour handles 2 types of duplicates:
				
				1) Duplicated name, but different colour definitions. In this case, the
				dupes are remembered and added to the document (the colour list will ensure
				they have unique names when finally added)

				2) Exact duplicate (now possible with parents of shade colours which might
				be defined twice in a CamEPS document). These are deleted out of hand,
				and the pNewCol pointer is updated to point at the original/first definition
				of the identical colour.

	Errors:		Out of memory => ERROR1
	SeeAlso:	NewColour; ImportedColours::AddColour

********************************************************************************************/

BOOL NewColour::AddDuplicateColour(IndexedColour **pNewCol, BOOL AlreadyExists)
{
	ERROR3IF(pNewCol == NULL || *pNewCol == NULL, "Illegal NULL param");

	// --- First, check if this colour is a real duplicate or just a duplicate in name
	// If it is a duplicate name, then we keep a copy around, but if it is a complete
	// duplicate, then we want to vape it and NOT put it in the dupes list - Jason

	// NOTE: We *ONLY* do this if the colour is not already part of the existing document
	// (i.e. was merged with an existing exact match colour in the doc), i.e. if !AlreadyExists

	if (!AlreadyExists)
	{
		NewColour *Ptr = this;
		while (Ptr != NULL)
		{
			if (!(*pNewCol)->IsDifferent(*(Ptr->pCol)))
			{
				delete (*pNewCol);			// delete the identical twin
				*pNewCol = Ptr->pCol;		// and return the pointer pointing at the original definition
				return(TRUE);
			}

			Ptr = Ptr->pNextDuplicate;
		}
	}	

	// --- And now back to our regularly scheduled programme...



	// First, try to make a new item
	NewColour *pNewItem = new NewColour(*pNewCol, AlreadyExists);
	if (pNewItem == NULL)
		return FALSE;

	// Mark this object as being a duplicate
	Duplicate = TRUE;

	// Ok, link to this object (at end of the one-way list)...
	NewColour *pLast = this;
	while (pLast->pNextDuplicate != NULL)
		pLast = pLast->pNextDuplicate;

	// Found end of list - link it in and mark it as being a duplicate
	pLast->pNextDuplicate = pNewItem;
	pNewItem->Duplicate = TRUE;

	// Worked ok
	return TRUE;
}


/********************************************************************************************

>	NewColour *NewColourList::AddColour(IndexedColour *pCol)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		pCol - the new colour to add to the document.
				AlreadyExists - TRUE if pNewCol points to a colour that already exists in
								the destination document;
								FALSE if pNewCol poinst to a brand new colour.
	Returns:	Pointer to the newly added colour, or 
				NULL if out of memory => ERROR1
	Purpose:	Add a new colour to the list of colours.
	Errors:		Out of memory => ERROR1
	SeeAlso:	NewColour

********************************************************************************************/

NewColour *NewColourList::AddColour(IndexedColour *pCol, BOOL AlreadyExists)
{
	NewColour *pNewCol = new NewColour(pCol, AlreadyExists);

	// Check for out of memory
	if (pNewCol == NULL)
		return NULL;

	// Add to the list and return success
	AddTail(pNewCol);
	return pNewCol;
}


/********************************************************************************************

>	DWORD NewColourList::GetCount()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/94
	Returns:	Number of colours in this list.
	Purpose:	Find out how many colours are held in this list - this includes any
				duplicates.

********************************************************************************************/

DWORD NewColourList::GetCount() const
{
	DWORD Count = 0;
	NewColour *pItem = (NewColour *) GetHead();

	while (pItem != NULL)
	{
		// Count this item
		Count++;

		// Count any duplicates
		NewColour *pDup = pItem->pNextDuplicate;

		while (pDup != NULL)
		{
			// Count this duplicate
			Count++;

			// Try next duplicate
			pDup = pDup->pNextDuplicate;
		}

		// Try next item
		pItem = (NewColour *) GetNext(pItem);
	}

	return Count;
}



/********************************************************************************************

>	ImportedColours::ImportedColours(ColourListComponent *TheColourComponent,
									 BOOL Strict)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		TheColourComponent - the relevant colour component for this import.
				Strict - TRUE => when servicing calls to GetColour(), check the CMYK
								 colour definition for a match in case a colour is being
								 used which wasn't declared in the colour table (this is
								 what we do for ArtWorks EPS).
						FALSE => don't bother checking CMYK - just use the name to look
								 up the colour - we use this for Camelot EPS because we
								 know we don't save out brain damaged EPS like ArtWorks
								 does.
	Purpose:	Create a table of imported colours (initially empty).
	SeeAlso:	ImportedColours::Init

********************************************************************************************/

ImportedColours::ImportedColours(ColourListComponent *TheColourComponent, BOOL Strict)
{
	// Remember this document component
	pColourComponent = TheColourComponent;

	// Do we check CMYK values when matching colours?
	StrictColourMatching = Strict;

	// No hash table yet
	pColourMap = NULL;
}

/********************************************************************************************

>	ImportedColours::~ImportedColours()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroy a table of imported colours.  This should only be done after calling
				AddColoursToDocument() or DestroyColours() - an ERROR3 will occur if this
				is not done.
	Errors:		Colours should be explicitly added to the document or deleted from this
				table before this function is called => ERROR3
	SeeAlso:	ImportedColours::AddColoursToDocument; ImportedColours::DestroyColours

********************************************************************************************/

ImportedColours::~ImportedColours()
{
	// MORE STUFF HERE...

	// Trash the hash table
	delete pColourMap;
	pColourMap = NULL;
}

/********************************************************************************************

>	BOOL ImportedColours::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialise the table of imported colours
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ImportedColours::Init()
{
	// Get a hash table for the colour names...
	try
	{
		pColourMap = new CMapStringToNewColour;
	}
	catch( CMemoryException )
	{
		ERROR(_R(IDS_OUT_OF_MEMORY), FALSE);
	}

	// Check our document's colour list component...
	if (pColourComponent == NULL)
	{
		// Whoops - something's up
		delete pColourMap;
		pColourMap = NULL;
		ERROR2(FALSE, "NULL colour list component!");
	}

	// Ininitalise the colour linking context.
	if (!Context.Init())
		return FALSE;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL ImportedColours::AddColour(const String_64 *pColName, 
									ColourCMYK *pCMYK,  
									NewColourInfo *pColourInfo = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		pColName - the name of the colour to add
				pCMYK - the definition of the colour to add
				pColourInfo - if NULL, then it is a normal colour;
							  if non-NULL then colour is special in some way, i.e. a tint
							    or a linked colour or a spot colour.
	Returns:	TRUE if the colour could be added;
				FALSE if not
	Purpose:	Add the specified colour to the colour table.  If the colour name clashes
				with any already added or already in the destination document, this does not
				matter as the name specified in pColName is what should be used to access
				this colour again.
				If pColourInfo specifies a linked or tinted colour, then this colour
				is also linked to the colour currently specified by the colour link 
				context (see SaveContext and RestoreContextTo).
	Errors:		-
	SeeAlso:	ImportedColours; ImportedColours::GetColour;
				ImportedColours::SaveContext; ImportedColours::RestoreContextTo

********************************************************************************************/

BOOL ImportedColours::AddColour(const String_64 *pColName, ColourCMYK *pCMYK, 
								NewColourInfo *pColourInfo)
{
	IndexedColour *pNewCol = new INDEXEDCOLOUR_CMYK(pCMYK);
	return AddColour(pColName, &pNewCol, pColourInfo);
}

/********************************************************************************************

>	BOOL ImportedColours::AddColour(const String_64 *pColName, 
									ColourRGBT *pRGB,
									NewColourInfo *pColourInfo = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		pColName - the name of the colour to add
				pRGB - the definition of the colour to add
				pColourInfo - if NULL, then it is a normal colour;
							  if non-NULL then colour is special in some way, i.e. a tint
							    or a linked colour or a spot colour.
	Returns:	TRUE if the colour could be added;
				FALSE if not
	Purpose:	Add the specified colour to the colour table.  If the colour name clashes
				with any already added or already in the destination document, this does not
				matter as the name specified in pColName is what should be used to access
				this colour again.
				If pColourInfo specifies a linked or tinted colour, then this colour
				is also linked to the colour currently specified by the colour link 
				context (see SaveContext and RestoreContextTo).
	Errors:		-
	SeeAlso:	ImportedColours; ImportedColours::GetColour;
				ImportedColours::SaveContext; ImportedColours::RestoreContextTo

********************************************************************************************/

BOOL ImportedColours::AddColour(const String_64 *pColName, ColourRGBT *pRGB, 
								NewColourInfo *pColourInfo)
{
	IndexedColour *pNewCol = new INDEXEDCOLOUR_RGBT(pRGB);
	return AddColour(pColName, &pNewCol, pColourInfo);
}

/********************************************************************************************

>	BOOL ImportedColours::AddColour(const String_64 *pColName, 
									ColourHSVT *pHSV,
									NewColourInfo *pColourInfo = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		pColName - the name of the colour to add
				pHSV - the definition of the colour to add
				pColourInfo - if NULL, then it is a normal colour;
							  if non-NULL then colour is special in some way, i.e. a tint
							    or a linked colour or a spot colour.
	Returns:	TRUE if the colour could be added;
				FALSE if not
	Purpose:	Add the specified colour to the colour table.  If the colour name clashes
				with any already added or already in the destination document, this does not
				matter as the name specified in pColName is what should be used to access
				this colour again.
				If pColourInfo specifies a linked or tinted colour, then this colour
				is also linked to the colour currently specified by the colour link 
				context (see SaveContext and RestoreContextTo).
	Errors:		-
	SeeAlso:	ImportedColours; ImportedColours::GetColour;
				ImportedColours::SaveContext; ImportedColours::RestoreContextTo

********************************************************************************************/

BOOL ImportedColours::AddColour(const String_64 *pColName, ColourHSVT *pHSV, 
								NewColourInfo *pColourInfo)
{
	IndexedColour *pNewCol = new INDEXEDCOLOUR_HSVT(pHSV);
	return AddColour(pColName, &pNewCol, pColourInfo);
}

/********************************************************************************************

>	BOOL ImportedColours::AddColour(const String_64 *pColName, 
									ColourGreyT *pGrey,
									NewColourInfo *pColourInfo = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		pColName - the name of the colour to add
				pGrey - the definition of the colour to add
				pColourInfo - if NULL, then it is a normal colour;
							  if non-NULL then colour is special in some way, i.e. a tint
							    or a linked colour or a spot colour.
	Returns:	TRUE if the colour could be added;
				FALSE if not
	Purpose:	Add the specified colour to the colour table.  If the colour name clashes
				with any already added or already in the destination document, this does not
				matter as the name specified in pColName is what should be used to access
				this colour again.
				If pColourInfo specifies a linked or tinted colour, then this colour
				is also linked to the colour currently specified by the colour link 
				context (see SaveContext and RestoreContextTo).
	Errors:		-
	SeeAlso:	ImportedColours; ImportedColours::GetColour;
				ImportedColours::SaveContext; ImportedColours::RestoreContextTo

********************************************************************************************/

BOOL ImportedColours::AddColour(const String_64 *pColName, ColourGreyT *pGrey,
								NewColourInfo *pColourInfo)
{
	IndexedColour *pNewCol = new INDEXEDCOLOUR_GREYT(pGrey);
	return AddColour(pColName, &pNewCol, pColourInfo);
}

/********************************************************************************************

>	BOOL ImportedColours::AddColour(const String_64 *pColName, 
									ImportedNewColour *pCol,
									NewColourInfo *pColourInfo = NULL)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18 03 95
	Inputs:		pColName - the name of the colour to add
				pGrey - the definition of the colour to add
				pColourInfo - if NULL, then it is a normal colour;
							  if non-NULL then colour is special in some way, i.e. a tint
							    or a linked colour or a spot colour.
	Returns:	TRUE if the colour could be added;
				FALSE if not
	Purpose:	Add the specified colour to the colour table.  If the colour name clashes
				with any already added or already in the destination document, this does not
				matter as the name specified in pColName is what should be used to access
				this colour again.
				If pColourInfo specifies a linked or tinted colour, then this colour
				is also linked to the colour currently specified by the colour link 
				context (see SaveContext and RestoreContextTo).
				This version adds a colour of from any colour model (supported by other
				AddColour functions)
	Errors:		-
	SeeAlso:	ImportedColours; ImportedColours::GetColour;
				ImportedColours::SaveContext; ImportedColours::RestoreContextTo

********************************************************************************************/

BOOL ImportedColours::AddColour(const String_64 *pColName, ImportedNewColour *pCol,
								NewColourInfo *pColourInfo)
{
	switch(pCol->Model)
	{
		case COLOURMODEL_RGBT:
			return AddColour(pColName, (ColourRGBT *)&pCol->Colour, pColourInfo);
			break;

		case COLOURMODEL_CMYK:
			return AddColour(pColName, (ColourCMYK *)&pCol->Colour, pColourInfo);
			break;

		case COLOURMODEL_HSVT:
			return AddColour(pColName, (ColourHSVT *)&pCol->Colour, pColourInfo);
			break;

		case COLOURMODEL_GREYT:
			return AddColour(pColName, (ColourGreyT *)&pCol->Colour, pColourInfo);
			break;

		default:
			break;
	}
	
	ERROR3("Colour model presented to ImportedColours::AddColour is not a recognised model");

	return FALSE;
}

/********************************************************************************************

>	BOOL ImportedColours::AddTintOrShade(const String_64 *pColName, NewColourInfo *pLinkInfo)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		pColName - the name of the colour to add
				pLinkInfo - the colour information data
	Returns:	TRUE if the colour could be added;
				FALSE if not
	Purpose:	Add the specified tint or shade to the colour table.
				Assumes the vital tint information has already been filled in.
	Errors:		-
	SeeAlso:	ColourListComponent::ReadTint; ColourListComponent::ReadShade;
				ColourListComponent::ImportColourDefinition;

********************************************************************************************/

BOOL ImportedColours::AddTintOrShade(const String_64 *pColName, NewColourInfo *pColourInfo)
{
	// Get a new colour to add
	IndexedColour *pNewCol = new IndexedColour;

	// Assumes tint and shade information already set up, this includes:-
	// pColourInfo->Type = COLOURTYPE_TINT;
	// pColourInfo->TintValue = 0.0; (FIXED24);
	// If shade then
	// pColourInfo->ShadeValue = 0.0; (FIXED24);

	return AddColour(pColName, &pNewCol, pColourInfo);
}

/********************************************************************************************

>	BOOL ImportedColours::AddTint(const String_64 *pColName, UINT32 Tint)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		pColName - the name of the colour to add
				Tint - the tint value (between 0 and 100)
	Returns:	TRUE if the colour could be added;
				FALSE if not
	Purpose:	Add the specified colour to the colour table.  If the colour name clashes
				with any already added or already in the destination document, this does not
				matter as the name specified in pColName is what should be used to access
				this colour again.
				This colour is a tint of the colour currently specified by the colour link 
				context (see SaveContext and RestoreContextTo).
	Errors:		-
	SeeAlso:	ImportedColours; ImportedColours::GetColour;
				ImportedColours::SaveContext; ImportedColours::RestoreContextTo

********************************************************************************************/

BOOL ImportedColours::AddTint(const String_64 *pColName, UINT32 Tint)
{
	// Get a new colour to add
	IndexedColour *pNewCol = new IndexedColour;

	// Set up the tint information
	NewColourInfo ColourInfo;
	ColourInfo.Type = COLOURTYPE_TINT;

	// Convert 0 <-> 100 to 0.0 <-> 1.0 and pass in as FIXED24.
	ColourInfo.TintValue = FIXED24(((double) Tint) / 100.0);

	return AddColour(pColName, &pNewCol, &ColourInfo);
}



/********************************************************************************************

>	BOOL ImportedColours::AddShade(const String_64 *pColName, INT32 ShadeX, INT32 ShadeY)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copied from code by Tim)
	Created:	7/9/95 (30/11/94)
	Inputs:		pColName - the name of the colour to add
				ShadeX - the shade X value (between -100 and 100)
				ShadeY - the shade Y value (between -100 and 100)

	Returns:	TRUE if the colour could be added;
				FALSE if not
	Purpose:	Add the specified colour to the colour table.  If the colour name clashes
				with any already added or already in the destination document, this does not
				matter as the name specified in pColName is what should be used to access
				this colour again.
				This colour is a shade of the colour currently specified by the colour link 
				context (see SaveContext and RestoreContextTo).
	Errors:		-
	SeeAlso:	ImportedColours; ImportedColours::GetColour;
				ImportedColours::SaveContext; ImportedColours::RestoreContextTo

********************************************************************************************/

BOOL ImportedColours::AddShade(const String_64 *pColName, INT32 ShadeX, INT32 ShadeY)
{
	// Get a new colour to add
	IndexedColour *pNewCol = new IndexedColour;

	// Set up the tint information
	NewColourInfo ColourInfo;
	ColourInfo.Type = COLOURTYPE_TINT;

	// And flag the fact that this "tint" is really a shade!
	ColourInfo.TintIsShade = TRUE;

	// Convert -100 <-> 100 to -1.0 <-> 1.0 and pass in as FIXED24.
	ColourInfo.TintValue = FIXED24(((double) ShadeX) / 100.0);
	ColourInfo.ShadeValue = FIXED24(((double) ShadeY) / 100.0);

	return AddColour(pColName, &pNewCol, &ColourInfo);
}



/********************************************************************************************

>	BOOL ImportedColours::AddColour(const String_64 *pColName, IndexedColour **pNewCol,
									NewColourInfo *pColourInfo)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94

	Modified:	Jason (8/9/95) - Now handles exact duplicates as well as duplicate names
				(This is needed for new shade-colour saving system which does nasty
				things in order to retain backward compatability with old loaders)

	Inputs:		pColName - the name of the colour
				pNewCol - ptr to ptr to the new indexed colour to use (if NULL then just
						  returns FALSE immediately) (see also Outputs)
				pColourInfo - if NULL, then it is a normal colour;
							  if non-NULL then colour is special in some way, i.e. a tint
							    or a linked colour or a spot colour.

	Outputs:	If *pNewCol points to an IndexedColour which already has an *identical*
				twin colour (i.e. exactly the same colour appears twice in the doc) then
				*pNewCol will be modified to point at the first incarnation of the colour,
				and the IndexedColour you passed in will be deleted.
				DO NOT use the previous IndexedColour pointer - only use the pointer as
				passed back from this method.

	Returns:	TRUE if the colour was added ok;	
				FALSE otherwise
	Purpose:	Generic routine for adding a colour to the table of imported colours.
				All the other AddColour() routines map onto this.

	Errors:		Out of memory => ERROR1
				pNewCol is NULL => returns FALSE (no error set)
	SeeAlso:	ImportedColours
	Scope:		Protected

********************************************************************************************/

BOOL ImportedColours::AddColour(const String_64 *pColName, IndexedColour **pNewIndexedCol,
								NewColourInfo *pColourInfo)
{
	// Do we have a valid colour?
	if (pNewIndexedCol == NULL || *pNewIndexedCol == NULL)
		// No!
		return FALSE;

	// First, set the colour name to the one supplied. If it is unnamed, then we want
	// to set its unique-ID string to the value from the file. At the end of the import,
	// we will use SetUnnamed to force it to re-generate a unique name so that it is
	// (a) merged properly during import, and (b) doesn't clash with another colour
	// already in the same document once it is added.
	//String_64 Name = *pColName;
	//(*pNewIndexedCol)->SetName(*pColName, (ColName[0] != '_'));
	TCHAR ch = (*pColName)[0];
	(*pNewIndexedCol)->SetName(*pColName, (ch != '_'));


	if (pColourInfo != NULL)
	{
		// Link the colour if required
		if ((pColourInfo->Type == COLOURTYPE_LINKED) ||
			(pColourInfo->Type == COLOURTYPE_TINT))
		{
			IndexedColour *pParent = NULL;
			if (pColourInfo->WebNativeColour)
			{
				pParent = pColourInfo->pParentCol;
			}
			else
			{
				// Importing a non-native or web file colour and so use the context of the colour
				// to define its parent.
				// Get the current colour context
				pParent = Context.GetContext();
				ERROR2IF(pParent == NULL, FALSE, "NULL parent when creating linked colour!");
			}

			// Link to it - either as a linked colour or a tint
			if (pColourInfo->Type == COLOURTYPE_LINKED)
			{
				// Linked colour
				(*pNewIndexedCol)->SetLinkedParent(pParent, COLOURTYPE_LINKED);

				// Set inheritance flags...
				for (INT32 i = 0; i < 4; i++)
					(*pNewIndexedCol)->SetInheritsComponent(i + 1, pColourInfo->Inherits[i]);
			}
			else
			{
				// Tinted colour
				(*pNewIndexedCol)->SetLinkedParent(pParent, COLOURTYPE_TINT);

				// Set tint/shade value as appropriate. This makes the 'tint' colour a
				// tint or shade. (Shades are special cases of tints for historical reasons)
				if (pColourInfo->TintIsShade)
					(*pNewIndexedCol)->SetShadeValues(pColourInfo->TintValue, pColourInfo->ShadeValue);
				else
					(*pNewIndexedCol)->SetTintValue(pColourInfo->TintValue);
			}
		}
		else if (pColourInfo->Type == COLOURTYPE_SPOT)
		{
			// Mark this as a spot colour (in case the caller hasn't already)
			(*pNewIndexedCol)->SetLinkedParent(NULL, COLOURTYPE_SPOT);
		}
	}

	// Is there a colour the same as this in the destination document already?
	IndexedColour *pIdenticalCol;
	pColourComponent->FindIdenticalColour(*pNewIndexedCol, &pIdenticalCol);

	BOOL AlreadyExists = (pIdenticalCol != NULL);

/*
	Phil, 01/03/2004
	If we remove colours from the document before we have added our list of imported colours
	anything could come along and add colours back in the meantime.
	(e.g. GradFillAttribute::GradFillAttribute)
	So leave colours in the list and only move them later on...
*/
	if (AlreadyExists)
	{
		// Graeme (5/4/00) - This code now changes the colour list so that an
		// existing colour is repositioned where the original definition was.

		// Delete the new one because it duplicates one already in the document.
		delete *pNewIndexedCol;

		// Make pNewIndexedCol point to the existing colour
		*pNewIndexedCol = pIdenticalCol;

		// Set AlreadyExists to be FALSE so that the colour is included.
		AlreadyExists = FALSE;
	}

	NewColour *pNewColour = NULL;

	// If we are in web/native filter mode then we MUST always add a new colour item as this
	// is where we save the record number which will be reused later. So we must stop the 
	// name look up and adding duplicates as this adds the new colour to the a list inside
	// the duplicated name that is in the ColourMap.
	BOOL WebNative = (pColourInfo && pColourInfo->WebNativeColour);

	// First, check to see if we already have a definition of this colour *NAME*
	if( !WebNative && pColourMap->Lookup( *pColName, pNewColour ) )
	{
		// Yes - mark the current definition as a duplicate and add this new one to it.
		if (!pNewColour->AddDuplicateColour(pNewIndexedCol, AlreadyExists))
			// Error encountered.
			return FALSE;
	}
	else
	{
		// No - but is it already in the destination document?
		//
		// (i.e. is there a colour in the document with the same 
		//  name, colour model and definition?)
		//
		// If yes - add a new colour that references the already existing colour, and
		// mark it as being such;
		//
		// If no  - it's a new colour, so add it to the list.

		pNewColour = Colours.AddColour(*pNewIndexedCol, AlreadyExists);

		if (pNewColour == NULL)
			// out of memory
			return FALSE;

		// Either way, add it to the colour name map
		try
		{
			(*pColourMap)[ *pColName ] = pNewColour;
		}
		catch( CMemoryException )
		{
			// Could not do it - report error to caller.
			ERROR(_R(IDS_OUT_OF_MEMORY), FALSE);
		}

		// If we are in web/native filter mode then save the record number that we
		// have been passed in in the NewColourInfo
		// At the same time save out the entry number for where we want this colour in the list
		// of colours
		if (pColourInfo && pColourInfo->WebNativeColour)
		{
			pNewColour->RecordNumber = pColourInfo->RecordNumber;
			pNewColour->EntryNumber = pColourInfo->EntryNumber;
		}
	}

	// We added the colour ok so set it as the current context colour
	// Note that AddDuplicateColour may have changed our pointer to point at a different
	// (identical) colour, but it will always point at the correct colour for us to use here.
	Context.SetContext(*pNewIndexedCol);

	// All ok
	return TRUE;
}


/********************************************************************************************

>	void ImportedColours::SaveContext()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	None
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ImportedColours::SaveContext()
{
	Context.SaveContext();
}

/********************************************************************************************

>	void ImportedColours::RestoreContextTo(UINT32 Level)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	None
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ImportedColours::RestoreContextTo(UINT32 Level)
{
	if (Context.RestoreContextTo(Level) == FALSE)
	{
		ERROR3("Unbalanced colour import context");
	}
}

/********************************************************************************************

>	IndexedColour *ImportedColours::GetColour(const char *ColName)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		ColName - the colour name to look for.
	Returns:	Pointer to the indexed colour if successful;
				NULL if not found.
	Purpose:	Find a named colour in the import list.  This look up is obviously done
				purely on the colour name, and as such is a slightly risky thing to do,
				especially when importing such flaky file formats as EPS - use the
				version of GetColour that takes a PColourCMYK value to provide more
				reliable colour lookup.
	SeeAlso:	ImportedColours

********************************************************************************************/

IndexedColour *ImportedColours::GetColour( PCTSTR ColName )
{
	// Do we have a definition of this colour name?
	NewColour *pNewColour=NULL;
	if( pColourMap->Lookup( ColName, pNewColour ) )
	{
		// Yes - return the first entry
		return pNewColour->pCol;
	}
	else
	{
		// No - return failure.
		return NULL;
	}
}

// This macro takes signed or unsigned values, and returns TRUE if they are equal, or
// different by one.
#define WITHIN_ONE(a,b) (Abs((INT32) (a) - (INT32) (b)) <= 1)

/********************************************************************************************

>	IndexedColour *ImportedColours::GetColour(const char *ColName, const PColourCMYK *Defn)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		ColName - the colour name to look for.
				Defn - pointer to the CMYK values specified in the imported file.
	Returns:	Pointer to the indexed colour if successful;
				NULL if not found.
	Purpose:	Searches for a named colour.  If the colour name has had duplicate
				definitions during this import, then the colour definition is compared
				against the values pointed to by Defn (converting to CMYK first if needed).
	Errors:		-
	SeeAlso:	ImportedColours

********************************************************************************************/

IndexedColour *ImportedColours::GetColour( PCTSTR ColName, const PColourCMYK *Defn )
{
	// Do we have a definition of this colour name?
	NewColour *pNewColour=NULL;
	if( pColourMap->Lookup( ColName, pNewColour ) )
	{
		// Should we check colour values before returning this colour?
		if (StrictColourMatching || (pNewColour->pNextDuplicate != NULL))
		{
			// Yes - find a colour from this entry that matches the colour definition too...
			while (pNewColour != NULL) 
			{
				// Is this a match?
				// NB. We allow them to be out by one to cope with rounding problems.
				//	   This is ok, because it will only happen in ArtWorks EPS files,
				//	   and then only on colours that are 1 out in the colour definition,
				//	   so I don't think anyone's going to notice!  -- Tim
				if (WITHIN_ONE(pNewColour->ColDefn.Cyan, 	Defn->Cyan)    &&
					WITHIN_ONE(pNewColour->ColDefn.Magenta, Defn->Magenta) &&
					WITHIN_ONE(pNewColour->ColDefn.Yellow,  Defn->Yellow)  &&
					WITHIN_ONE(pNewColour->ColDefn.Key,		Defn->Key))
				{
					// Yes - quit loop
					break;
				}

				// Otherwise try next colour
				pNewColour = pNewColour->pNextDuplicate;
			}

			// Did we find one?
			if (pNewColour != NULL)
			{
				// Yes - return it to the caller.
				return pNewColour->pCol;
			}
			else
			{
				// No - return failure
				return NULL;
			}
		}
		else
		{
			// No checking needed -just return this colour
			return pNewColour->pCol;
		}
	}
	else
	{
		// No - return failure.
		return NULL;
	}
}

// Just to be safe...
#undef WITHIN_ONE


/********************************************************************************************

>	BOOL ImportedColours::AddColoursToDocument()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	None
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ImportedColours::AddColoursToDocument()
{
	// Make an array for the new colours
	INT32 NumNewColours = Colours.GetCount();
	INT32 TableSize = sizeof(IndexedColour *) * (NumNewColours + 1);
	IndexedColour **pNewArray = (IndexedColour **) CCMalloc(TableSize);
	if (pNewArray == NULL)
	{
		// Out of memory.
		return FALSE;
	}

	// set up a slow job
	String_64 ImportMessage(_R(IDT_ADDCOLOURSTODOC));
	BeginSlowJob( NumNewColours, TRUE, &ImportMessage );
	INT32 Done = 0;
	
	// Copy the colour pointers into this array
	INT32 i = 0;
	NewColour *pColour = (NewColour *) Colours.GetHead();
	List *pUnnamedColours = pColourComponent->GetColourList()->GetUnnamedColours();

	while (pColour != NULL)
	{
		// Remove this colour from the list, and add it (and all duplicates of it) to the 
		// palette and the array, but only if it is not already in the document,and it
		// is not an unnamed colour (i.e. its name starts with an underscore).
		Colours.RemoveItem(pColour);

		while (pColour != NULL)
		{
			// Is it an unnamed colour?
			if (!pColour->pCol->IsNamed())		// Is it an unnamed colour?
			{
				// Yes - add to the unnamed colour list (no need for undo)
				// We force it unnamed again now to ensure it generates a new unique
				// ID for its destination document, rather than using the
				// unique-within-file id used during import.
				pColour->pCol->SetUnnamed();
				pUnnamedColours->AddTail(pColour->pCol);
			}
			else
			{
				// It's a named colour - may be already in the doc, may be not
				IndexedColour* pIdenticalColour;
				pColourComponent->FindIdenticalColour(pColour->pCol, &pIdenticalColour);
				if (pIdenticalColour)
				{
					// Already in the doc so just move the existing colour
					// into the required position in the list by unlinking and relinking...
					ColourList *pColourList = pColourComponent->GetColourList ();
					if ( pColourList->RemoveItem ( pIdenticalColour ) != NULL )
					{
						pColourComponent->AddNewColour(pIdenticalColour);
						pNewArray[i++] = pIdenticalColour;
					}

				}
				else
				{
					pColourComponent->AddNewColour(pColour->pCol);
					pNewArray[i++] = pColour->pCol;
				}
			}

			// Try next duplicate and delete this item.
			NewColour *pTmp = pColour;
			pColour = pColour->pNextDuplicate;
			delete pTmp;
		}

		// Try the next colour
		pColour = (NewColour *) Colours.GetHead();
	
		// do progess bar stuff
		Done++;
		if(Done % 0xf == 0)
			ContinueSlowJob(Done);
	}

	// Terminate the list of pointers
	pNewArray[i] = NULL;

	// Add these colours|.
	ColourManager::UnHideColours(pColourComponent->GetColourList(), pNewArray);

	// Destroy the list of added colours (UnHideColours copied it, so our copy is useless now)
	CCFree(pNewArray);
	pNewArray = NULL;

	// Destroy the colour map
	delete pColourMap;
	pColourMap = NULL;

	// finish the slow job
	ContinueSlowJob(NumNewColours);
	EndSlowJob();
	
	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL ImportedColours::DestroyColours()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	None
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ImportedColours::DestroyColours()
{
	NewColour *pColour = (NewColour *) Colours.GetHead();

	while (pColour != NULL)
	{
		// Remove this colour from the list, and delete the indexed colour it points to
		// (if it is not already in the document).
		Colours.RemoveItem(pColour);

		while (pColour != NULL)
		{
			if (!pColour->AlreadyExistsInDoc)
			{
				// It's a brand new colour - delete it.
				// Except we don't because it causes an error.

				// NB.  This is a temporary bodge to stop us from getting Indexed colour
				// 		deleted while in use errors - they happen because the undo 
				//		system does not delete the nodes added by InsertNode() yet.
				//		When it does, then we can uncomment this line.  As it stands we
				//		we get a memory leak of new colours when an import fails, but
				//		what the hell - it's only a bodge.

				//delete pColour->pCol;
			}

			// Try next duplicate and delete this item.
			NewColour *pTmp = pColour;
			pColour = pColour->pNextDuplicate;
			delete pTmp;
		}

		// Try the next colour
		pColour = (NewColour *) Colours.GetHead();
	}

	// All done
	return TRUE;
}

/********************************************************************************************

>	IndexedColour *ImportedColours::GetColour(INT32 ReqRecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		ReqRecordNumber	- the record number to search the list for
	Returns:	Pointer to the indexed colour if successful;
				NULL if not found.
	Purpose:	Searches for a previously imported colour definition. Used by the native/web
				filter to convert a colour reference in a record to an IndexedColour. This
				must have already been imported in the file as the rule is that reference to
				colours can only be made if the colour definition has already been output.
	Errors:		-
	SeeAlso:	ImportedColours; ColourListComponent::ImportColourDefinition;

********************************************************************************************/

IndexedColour *ImportedColours::GetColour(INT32 ReqRecordNumber)
{
	// Work our way through the list of imported colours until we find the required record number
	NewColour *pColour = (NewColour *) Colours.GetHead();
	while (pColour != NULL)
	{
		if (pColour->RecordNumber == ReqRecordNumber)
			return pColour->pCol;

		// Try the next colour
		pColour = (NewColour *) Colours.GetNext(pColour);
	}

	// Something went a bit wrong and we didn't find the requested record number
	// return NULL to the caller 
	return NULL;
}

/********************************************************************************************

>	BOOL ImportedColours::SortColoursByEntryNumber()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		-
	Outputs:	-
	Returns:	True if the operation completed successfully
				False if it failed in some way.
	Purpose:	Sorts the list of imported colours by the EntryNumber that is saved in the 
				colour definition in the native file format so that we load the list back in
				the same order that it was in when the document was saved. This because when
				we save the colours we cannot save the list in the order they are defined in
				the colour list as we must save parent colours before children so that the 
				children can reference the parent.
				Borrowed code from SuperGallery::ApplySortNow as we need a much simpler version.
	Errors:		-
	SeeAlso:	ImportedColours; ColourListComponent::ImportColourDefinition;
				ColourListComponent::EndImport; SuperGallery::ApplySortNow

********************************************************************************************/

BOOL ImportedColours::SortColoursByEntryNumber()
{
	INT32 NumItems = 0;

	// Count the number of items we have to sort
	NewColour *pColour = (NewColour *) Colours.GetHead();
	while (pColour != NULL)
	{
		if (pColour->pCol->IsNamed())		// Is it an named colour?
			NumItems++;
		pColour = (NewColour *) Colours.GetNext(pColour);
	}

	// Start progress indicators, with a percentage based upon the number of items.
	// We will update twice for each group (after qsort and shuffle-items stages)
	String_64 Description(_R(IDS_NATIVE_SORTCOLOURS));
	BeginSlowJob(NumItems * 2, FALSE, &Description);
	INT32 NumItemsToSort = 0;

	if (NumItems > 1)
	{
		// Get memory for an array of pointers to these items
		NewColour **SortArray = (NewColour **)CCMalloc(NumItems * sizeof(NewColour *));
		if (SortArray == NULL)
			return FALSE;

		// Fill in the array with pointers to display items to sort
		INT32 i = 0;
		NewColour *pColour = (NewColour *) Colours.GetHead();
		while (pColour != NULL)
		{
			if (pColour->pCol->IsNamed())		// Is it an named colour?
				SortArray[i++] = pColour;
			pColour = (NewColour *) Colours.GetNext(pColour);
		}

		// Sort the array of pointers
		qsort(SortArray, NumItems, sizeof(NewColour *), ImportedColours::SortComparator);

		NumItemsToSort += NumItems;
		// Update percentage complete for the number of items processed
		ContinueSlowJob(NumItemsToSort);

		// Now, take the sorted array, and rearrange the items to be in that order		
		// Special case the first item
		NewColour *pPrevColour = (NewColour *) Colours.GetPrev(SortArray[0]);
		if (pPrevColour != NULL)
		{
			Colours.RemoveItem(SortArray[0]);
			Colours.InsertBefore(SortArray[1], SortArray[0]);
		}

		// Then whip through the rest of the items
		for (i = 1; i < NumItems; i++)
		{
			pPrevColour = (NewColour *) Colours.GetPrev(SortArray[i]);
			if (pPrevColour != SortArray[i-1])
			{
				Colours.RemoveItem(SortArray[i]);
				Colours.InsertAfter(SortArray[i-1], SortArray[i]);
			}
		}

		// Free our temporary workspace
		CCFree(SortArray);
	}
	
	// End the progress bar that we started
	EndSlowJob();

	// We seem to have sorted the items ok
	return TRUE;
}

/********************************************************************************************

>	static INT32 __cdecl ImportedColours::SortComparator(const void *Item1, const void *Item2)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		Item1, Item2 - the display nodes to be compared
	Returns:	negative (I am lesser), 0 (we are equal), or positive (I am greater)
				result of comparing the items with the EntryNumber stored in each item in
				the loaded colour list.
	Purpose:	'qsort' comparator function, used when quicksorting the loaded colour list
	SeeAlso:	ImportedColours::SortColoursByEntryNumber()
				SuperGallery::SortComparator; SuperGallery::ApplySortNow

********************************************************************************************/

INT32 __cdecl ImportedColours::SortComparator(const void *Item1, const void *Item2)
{
	NewColour *pColourItem1 = *((NewColour **)Item1);
	NewColour *pColourItem2 = *((NewColour **)Item2);

	if (pColourItem1 == NULL || pColourItem2 == NULL)
	{
		ERROR3("ImportedColours::SortComparator bad pointers!");	
		return 0;
	}

	if (pColourItem1->EntryNumber < pColourItem2->EntryNumber)
		return(-1);

	return((pColourItem1->EntryNumber == pColourItem2->EntryNumber) ? 0 : 1);
}
