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

// collist.cpp - Implementation fo the ColourList class

/*
*/

// If you get any unexplainable problems with this file - Try changing the order of the includes
// It's due to a curse put on Jason's code.

#include "camtypes.h"

#include "basedoc.h"
//#include "colcontx.h"
#include "collist.h"
//#include "colormgr.h"
#include "colourix.h"
#include "fixmem.h"
//#include "jason.h"


CC_IMPLEMENT_MEMDUMP(ColourList, List)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/**********************************************************************************************

>	ColourList::ColourList()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-

	Purpose:	ColourList constructor

**********************************************************************************************/

ColourList::ColourList()
{
	ParentDoc = NULL;				// Document containing this list
	ArrayParent = NULL;				// Current parent of the cached array
	InheritanceArray = NULL;		// Cached array of all offspring of ArrayParent
	LastUnnamedColour = 2;			// Memory to optimise unnamed-colour name generation
}



/**********************************************************************************************

>	ColourList::~ColourList()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-

	Purpose:	ColourList destructor

**********************************************************************************************/

ColourList::~ColourList()
{
	InvalidateInheritanceCache();	// Release our memory claims

	UnnamedColours.DeleteAll();		// Delete any remaining unnamed colours
									// This will probably cause IxCol in use errors
									// as by now all colours should have been deleted
									// automatically when their usage reached zero.
}

/**********************************************************************************************

>	void ColourList::Init(BaseDocument *ParentDocument)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		ParentDocument - the parent document of this colour list
	
	Purpose:	ColourList initialiser. MUST be called after constructing a ColourList
				in order to correctly initialise it. Note that if ParentDocument is not
				set to a useful value, some operations on colour lists (eg. the colour
				editor) will fail.

**********************************************************************************************/

void ColourList::Init(BaseDocument *ParentDocument)
{
	ERROR3IF(ParentDocument == NULL, "NULL ParentDoc in ColourList::Init()");
	ParentDoc = ParentDocument;
}

/********************************************************************************************

>	void ColourList::InvalidateInheritanceCache(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94

	Purpose:	Deallocates any memory used for the array of IndexedColours compiled by
				ColourList::CompileInheritanceArray. The next request for such an array will
				be regenerated from the ColourList, rather than from cache.

				This is automatically called by the ColourManager immediately prior to
				a ColourChangingMsg COLOURUPDATED message broadcast.

	SeeAlso:	ColourList::CompileInheritanceArray

********************************************************************************************/

void ColourList::InvalidateInheritanceCache(void)
{
	if (InheritanceArray != NULL)
		CCFree(InheritanceArray);

	InheritanceArray = NULL;
	ArrayParent = NULL;
}

/**********************************************************************************************

>	void ColourList::AddHead(ListItem *ItemToAdd)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/94

	Purpose:	Overrides the List::AddHead method to ensure that any added item has a name
				which is unique within this colour list

				ERROR3's if the item is not a NAMED IndexedColour

	SeeAlso:	ColourList::AddItem

**********************************************************************************************/

void ColourList::AddHead(ListItem *ItemToAdd)
{
	ERROR3IF(!IS_A(ItemToAdd, IndexedColour), "You can only insert IndexedColours into ColourLists!");

	IndexedColour *NewColour = (IndexedColour *) ItemToAdd;

	// Ensure the colour has a name which is unique within this colour list
	ERROR3IF(!NewColour->IsNamed(), "You can't Insert an unnamed colour before a named one!");

	// It's a named colour, so make sure its name is unique
	String_64 NewName;
	if (GenerateUniqueColourName(NewColour->GetName(), &NewName))
		NewColour->SetName(NewName);

	// And call the base class to add the item
	List::AddHead(ItemToAdd);
}



/**********************************************************************************************

>	void ColourList::AddTail(ListItem *ItemToAdd)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/94

	Purpose:	Overrides the List::AddTail method to ensure that any added item has a name
				which is unique within this colour list

				ERROR3's if the item is not a NAMED IndexedColour

	SeeAlso:	ColourList::AddItem

**********************************************************************************************/

void ColourList::AddTail(ListItem *ItemToAdd)
{
	ERROR3IF(!IS_A(ItemToAdd, IndexedColour), "You can only insert IndexedColours into ColourLists!");

	IndexedColour *NewColour = (IndexedColour *) ItemToAdd;

	// Ensure the colour has a name which is unique within this colour list
	ERROR3IF(!NewColour->IsNamed(), "You can't Insert an unnamed colour before a named one!");

	// It's a named colour, so make sure its name is unique
	String_64 NewName;
	if (GenerateUniqueColourName(NewColour->GetName(), &NewName))
		NewColour->SetName(NewName);

	// And call the base class to add the item
	List::AddTail(ItemToAdd);
}



/**********************************************************************************************

>	BOOL ColourList::AddItem(IndexedColour *NewColour)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/94
	Returns:	TRUE

	Purpose:	Adds an IndexedColour item to the tail of the ColourList
				The name of the colour is checked, and may be changed to ensure that it
				is a unique identifier within the scope of this colour list.

				This can also be used to add unnamed colours - in this case, they are just
				added to the list of unnamed colours held by this object.

	SeeAlso:	ColourList::GenerateUniqueColourName

**********************************************************************************************/

BOOL ColourList::AddItem(IndexedColour *NewColour)
{
	// Ensure the colour has a name which is unique within this colour list
	if (!NewColour->IsNamed())
	{
		UnnamedColours.AddTail(NewColour);
		return(TRUE);
	}

	// It's a named colour, then...
	String_64 NewName;

	if (GenerateUniqueColourName(NewColour->GetName(), &NewName))
		NewColour->SetName(NewName);

	// And add it to the tail of the list - using the base class function, not our overrides!
	List::AddTail(NewColour);
	return(TRUE);
}



/**********************************************************************************************

>	LISTPOS ColourList::InsertBefore(LISTPOS here, ListItem* item)
>	ListItem *ColourList::InsertBefore(ListItem* here, ListItem* item)
>	LISTPOS ColourList::InsertAfter(LISTPOS here, ListItem* item)
>	ListItem *ColourList::InsertAfter(ListItem* here, ListItem* item)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/95

	Inputs:		here - the list item to insert before. NULL is not allowed (List is unhelpful)
				item - the item to insert before/after the 'here' item

	Returns:	The position/item which was inserted

	Purpose:	Inserts a NAMED IndexedColour at a particular list position
				(See List base class for a further description of the actual insertion process)

				These methods are a veneer onto the normal list calls, which ensure that
				all inserted colours are given unique names within this list.

	Note:		IMPORTANT: NOTE that at present,
				these are NOT VIRTUAL, so you MUST call them using a ColourList pointer rather
				than a generic List pointer!

	SeeAlso:	List; List::InsertBefore; ColourList::GenerateUniqueColourName

**********************************************************************************************/

LISTPOS ColourList::InsertBefore(LISTPOS here, ListItem* item)
{
	ERROR3IF(!IS_A(item, IndexedColour), "You can only insert IndexedColours into ColourLists!");

	IndexedColour *NewColour = (IndexedColour *) item;

	// Ensure the colour has a name which is unique within this colour list
	ERROR3IF(!NewColour->IsNamed(), "You can't Insert an unnamed colour before a named one!");

	// It's a named colour, so make sure its name is unique
	String_64 NewName;
	if (GenerateUniqueColourName(NewColour->GetName(), &NewName))
		NewColour->SetName(NewName);

	// And add it to the tail of the list - using the base class function, not our overrides!
	return(List::InsertBefore(here, item));
}

ListItem *ColourList::InsertBefore(ListItem* here, ListItem* item)
{
	ERROR3IF(!IS_A(item, IndexedColour), "You can only insert IndexedColours into ColourLists!");

	IndexedColour *NewColour = (IndexedColour *) item;

	// Ensure the colour has a name which is unique within this colour list
	ERROR3IF(!NewColour->IsNamed(), "You can't Insert an unnamed colour before a named one!");

	// It's a named colour, so make sure its name is unique
	String_64 NewName;
	if (GenerateUniqueColourName(NewColour->GetName(), &NewName))
		NewColour->SetName(NewName);

	// And add it to the tail of the list - using the base class function, not our overrides!
	return(List::InsertBefore(here, item));
}

LISTPOS ColourList::InsertAfter(LISTPOS here, ListItem* item)
{
	ERROR3IF(!IS_A(item, IndexedColour), "You can only insert IndexedColours into ColourLists!");

	IndexedColour *NewColour = (IndexedColour *) item;

	// Ensure the colour has a name which is unique within this colour list
	ERROR3IF(!NewColour->IsNamed(), "You can't Insert an unnamed colour before a named one!");

	// It's a named colour, so make sure its name is unique
	String_64 NewName;
	if (GenerateUniqueColourName(NewColour->GetName(), &NewName))
		NewColour->SetName(NewName);

	// And add it to the tail of the list - using the base class function, not our overrides!
	return(List::InsertAfter(here, item));
}

ListItem *ColourList::InsertAfter(ListItem* here, ListItem* item)
{
	ERROR3IF(!IS_A(item, IndexedColour), "You can only insert IndexedColours into ColourLists!");

	IndexedColour *NewColour = (IndexedColour *) item;

	// Ensure the colour has a name which is unique within this colour list
	ERROR3IF(!NewColour->IsNamed(), "You can't Insert an unnamed colour before a named one!");

	// It's a named colour, so make sure its name is unique
	String_64 NewName;
	if (GenerateUniqueColourName(NewColour->GetName(), &NewName))
		NewColour->SetName(NewName);

	// And add it to the tail of the list - using the base class function, not our overrides!
	return(List::InsertAfter(here, item));
}



/********************************************************************************************

>	virtual void ColourList::DeleteAll()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/96

	Purpose:	Deletes the list, calling the destructors of all ListItems

	Notes:		Colours are deleted carefully to be tidy with child links
				(As we're deleting them all this shouldn't matter, but when children
				of a deleted colour are deleted, they poke its ChildUsage, and Smart
				Heap chokes itself in dismay. The old smartheap didn't care)

				We thus vape all colour linking before deleting the colours. We
				don't need to be particularly gentle about this because we are
				deleting them all anyway.

	SeeAlso:	List::DeleteAll

********************************************************************************************/

void ColourList::DeleteAll()
{
	// Vape all colour linkages in the named colour list
	IndexedColour *Ptr = (IndexedColour *) GetHead();
	while (Ptr != NULL)
	{
		Ptr->SetLinkedParent(NULL, COLOURTYPE_NORMAL);
		Ptr = (IndexedColour *) GetNext(Ptr);
	}

	// And vape all colour linkages in the UNnamed colour list
	Ptr = (IndexedColour *) UnnamedColours.GetHead();
	while (Ptr != NULL)
	{
		Ptr->SetLinkedParent(NULL, COLOURTYPE_NORMAL);
		Ptr = (IndexedColour *) UnnamedColours.GetNext(Ptr);
	}

	// Delete all the unnamed colours now too, just to be tidy
	UnnamedColours.DeleteAll();

	// Finally, call the base class to do the actual deletions
	List::DeleteAll();
}



/**********************************************************************************************

>	UINT32 ColourList::GetUndeletedCount(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/94
	Inputs:		-
	Outputs:	-
	Returns:	The number of IndexedColours in this ColourList which are not
				'deleted' so as to be hidden from view by the undo system.

	Purpose:	Count the number of non-'deleted' items in a ColourList

**********************************************************************************************/

UINT32 ColourList::GetUndeletedCount(void)
{
	ListItem *Ptr = GetHead();
	UINT32 Count = 0;

	while (Ptr != NULL)
	{
		if ( !((IndexedColour *)Ptr)->IsDeleted() )
			Count++;

		Ptr = GetNext(Ptr);
	}

	return(Count);
}



/********************************************************************************************

>	IndexedColour *ColourList::FindUndeletedItem(INT32 Index)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/94
	Inputs:		ColourIndex - essentially a LISTPOS of the item in the Palette list

	Returns:	NULL if the index is past the end of the list
				else a pointer to the referenced IndexedColour ListItem

	Purpose:	The same as the conventional List FindItem() function, except
				that it treats 'deleted' colours as if they are not in the list.

********************************************************************************************/

IndexedColour *ColourList::FindUndeletedItem(INT32 Index)
{
	IndexedColour *Ptr = (IndexedColour *) GetHead();

	while (Ptr != NULL)
	{
		if (!Ptr->IsDeleted() && --Index < 0)
			return(Ptr);

		Ptr = (IndexedColour *) GetNext(Ptr);
	}

	return(NULL);
}



/********************************************************************************************

>	IndexedColour *ColourList::GetUndeletedHead(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/95

	Returns:	NULL if the list contains no 'live' (non-deleted) colours
				else a pointer to the first IndexedColour ListItem in the list

	Purpose:	The same as the conventional List GetHead() function, except
				that it treats 'deleted' colours as if they are not in the list.

********************************************************************************************/

IndexedColour *ColourList::GetUndeletedHead(void)
{
	IndexedColour *Ptr = (IndexedColour *) GetHead();

	while (Ptr != NULL && Ptr->IsDeleted())
		Ptr = (IndexedColour *) GetNext(Ptr);

	return(Ptr);
}



/********************************************************************************************

>	IndexedColour *ColourList::GetUndeletedNext(IndexedColour *ThisItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/95

	Returns:	NULL if the list contains no more 'live' (non-deleted) colours
				else a pointer to the next 'live' IndexedColour in the list

	Purpose:	The same as the conventional List GetNext() function, except
				that it treats 'deleted' colours as if they are not in the list.

********************************************************************************************/

IndexedColour *ColourList::GetUndeletedNext(IndexedColour *ThisItem)
{
	ERROR2IF(ThisItem == NULL, NULL,
				"ColourList::GetUndeletedNext - NULL parameter is illegal");

	IndexedColour *Ptr = (IndexedColour *) GetNext(ThisItem);

	while (Ptr != NULL && Ptr->IsDeleted())
		Ptr = (IndexedColour *) GetNext(Ptr);

	return(Ptr);
}



/********************************************************************************************

>	IndexedColour **ColourList::CompileInheritanceArray(IndexedColour *Parent)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		Parent - The colour for which you want an inheritance array

	Returns:	If NO error occurred, this returns a pointer to a NULL terminated array of
				IndexedColours, with Array[0] == Parent.
				If an error did occur, the error is reported, and NULL is returned

	Purpose:	Compiles a NULL-terminated array of pointers to ALL colours which are linked
				to this, both directly and indirectly (i.e. an array listing the entire
				inheritance 'subtree' from 'Parent', *including* 'Parent', and including
				named, unnamed, and 'deleted' colours).

				This array is cached so that the cost of generating it several times in a
				row (as may be necessary for different clients of the ColourChangingMsg)
				is minimised.

	Notes:		The array is NULL terminated

				The array INCLUDES Parent as its first member (Array[0])

				The array is owned (allocated and deallocated) by the ColourList.
				It MUST be treated as a read-only shared array.

				The array includes ALL affected indexed colours. Use IsDeleted() and
				IsNamed() to sort out named/unnamed colours, and deleted (hidden for
				undo) colours.

				This is expected to be called mainly as a result of COLOURUPDATED
				ColourChangingMsg broadcasts. The cache is flushed automatically
				by the colour manager just prior to these broadcasts. If you want to
				use this function outside these message broadcasts, then you must
				invalidate the cache at an appropriate time.

	SeeAlso:	ColourList::InvalidateInheritanceCache;
				IndexedColour::IsADescendantOf;
				ColourManager::ColourHasChanged

********************************************************************************************/

IndexedColour **ColourList::CompileInheritanceArray(IndexedColour *Parent)
{
	if (Parent != ArrayParent)
	{
		InvalidateInheritanceCache();		// Throw away any existing array

		// Generate the array
		INT32 ColourArraySize = 32;
		InheritanceArray = (IndexedColour**) CCMalloc(ColourArraySize * sizeof(IndexedColour *));

		if (InheritanceArray == NULL)
		{
			InformError();					// Out of memory - inform the user
			return(NULL);
		}

		INT32 ListEnd = 1;					// Initialise array to just the Parent
		InheritanceArray[0] = Parent;
		InheritanceArray[1] = NULL;

		// Do all the Named colours
		IndexedColour *Ptr = (IndexedColour *) GetHead();
		while (Ptr != NULL)
		{
			if (Ptr != Parent && Ptr->IsADescendantOf(Parent))
			{
				// Is a descendant: add this item to the end of the list
				if (ListEnd >= ColourArraySize - 1)
				{
					// We've filled the array - must increase the size
					ColourArraySize += 32;
					IndexedColour **TempArray = (IndexedColour **) CCRealloc(InheritanceArray,
												ColourArraySize * sizeof(IndexedColour *));
					if (TempArray == NULL)
					{
						InvalidateInheritanceCache();	// Free our existing memory claim
						InformError();
						return(NULL);
					}

					InheritanceArray = TempArray;		// It worked- set our ptr to the new block
				}
				
				InheritanceArray[ListEnd++] = Ptr;		// Add the item and a new NULL terminator
				InheritanceArray[ListEnd] = 0;			// to the end of the list
			}

			Ptr = (IndexedColour *) GetNext(Ptr);
		}


		// And likewise with all unnamed colours
		IndexedColour *Next;
		Ptr = (IndexedColour *) UnnamedColours.GetHead();
		while (Ptr != NULL)
		{
			// Allow for deleting this colour from underneath ourselves
			Next = (IndexedColour *) UnnamedColours.GetNext(Ptr);

			if (Ptr->IsInUse())
			{
				if (Ptr->IsADescendantOf(Parent))
				{
					// Is a descendant: add this item to the end of the list
					if (ListEnd >= ColourArraySize - 1)
					{
						// We've filled the array - must increase the size
						ColourArraySize += 32;
						IndexedColour **TempArray = (IndexedColour **) CCRealloc(InheritanceArray,
													ColourArraySize * sizeof(IndexedColour *));
						if (TempArray == NULL)
						{
							InvalidateInheritanceCache();	// Free our existing memory claim
							InformError();
							return(NULL);
						}

						InheritanceArray = TempArray;		// It worked- set our ptr to the new block
					}
				
					InheritanceArray[ListEnd++] = Ptr;		// Add the item and a new NULL terminator
					InheritanceArray[ListEnd] = 0;			// to the end of the list
				}
			}
			else
			{
				// We've found an unnamed IndexedColour which is not in use, so we can vape it
TRACEUSER( "Jason", _T("CIA: Deleting unnamed colour which is not in use [%p]\n"), Ptr );
//				UnnamedColours.RemoveItem(Ptr);
//				delete Ptr;
			}

			Ptr = Next;
		}


		ArrayParent = Parent;
	}

TRACEUSER( "Jason", _T("\n"));

	return(InheritanceArray);
}



/********************************************************************************************

>	void ColourList::RemoveLinksToColour(IndexedColour *DeadColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94

	Inputs:		DeadColour - The colour for which all ParentColour references are to be
				removed.

	Purpose:	Scans the entire ColourList for any colours which reference the given
				IndexedColour. All these colours are converted into standalone
				representation of their former definition, so that all child-links of
				this colour are removed. It is then safe to delete the colour.

				This is done automatically for colours deleted by the HideColours
				method of the colour manager; however, deletion only occurs for real
				when the undo record containing the deleted colours is deleted.

				NOTE that this does NOT remove the colour from the ColourList
				It also does NOT delete the colour.

	SeeAlso:	ColourManager::HideColours

********************************************************************************************/

void ColourList::RemoveLinksToColour(IndexedColour *DeadColour)
{
	// Search through the named colours
	IndexedColour *Ptr = (IndexedColour *) GetHead();

	while (Ptr != NULL)										// For all colours in the list...
	{
		if (Ptr->FindLastLinkedParent() == DeadColour)		// If they reference DeadColour...
		{				
			IndexedColourType TheType = COLOURTYPE_NORMAL;

			if (Ptr->GetType() == COLOURTYPE_SPOT)
				TheType = COLOURTYPE_SPOT;

			Ptr->SetLinkedParent(NULL, TheType);			// Convert to unlinked (normal/spot) colour
		}

		Ptr = (IndexedColour *) GetNext(Ptr);
	}


	// And drag through the unnamed colours as well
	Ptr = (IndexedColour *) UnnamedColours.GetHead();
	IndexedColour *Next;

	while (Ptr != NULL)										// For all colours in the list...
	{
		Next = (IndexedColour *) UnnamedColours.GetNext(Ptr);
		if (Ptr->FindLastLinkedParent() == DeadColour)		// If they reference DeadColour...
		{				
			IndexedColourType TheType = COLOURTYPE_NORMAL;

			if (Ptr->GetType() == COLOURTYPE_SPOT)
				TheType = COLOURTYPE_SPOT;

			Ptr->SetLinkedParent(NULL, TheType);			// Convert to unlinked (normal/spot) colour
		}
// This could be dangerous - what if the deleted colour is in the inheritance array?
//		if (!Ptr->IsInUse())
//		{
//			// We've found an unnamed colour which is not in use - we can vape it
//TRACEUSER( "Jason", _T("RLTC: Deleting unnamed colour which is not in use [%x]\n"), (INT32)Ptr);
//			UnnamedColours.RemoveItem(Ptr);
//			delete Ptr;
//		}

		Ptr = Next;
	}
}



/********************************************************************************************

>	BOOL ColourList::IsColourInUseInDoc(IndexedColour *TheColour, BOOL IgnoreColourGallery = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/95
	Inputs:		TheColour - Points to the colour to check
				IgnoreColourGallery - TRUE to ignore usage in the colour gallery. DO NOT USE this
				parameter. It is a special-case for the colour gallery to use to ignore the
				usage that that gallery has of the colour.

	Returns:	TRUE if the colour is in use in the document

	Purpose:	Determine if a colour is used in the parent document. This is similar to
				IndexedColour::IsInUse(), but it also traverses the linking hierarchy
				to see if a colour is in use indirectly (e.g. A parent of a colour that
				is in use is needed for the document even if it is not used directly in\the
				document.

	SeeAlso:	IndexedColour::IsInUse

********************************************************************************************/

BOOL ColourList::IsColourInUseInDoc(IndexedColour *TheColour, BOOL IgnoreColourGallery)
{
	ERROR3IF(TheColour == NULL, "Illegal NULL param");

	// First, is it directly in-use?
	if (TheColour->IsInUse(IgnoreColourGallery))
		return(TRUE);

	// Is it a parent of other colours? If not, then it is not in use
	if (!TheColour->IsNamed() || !TheColour->HasLinkedChildren())
		return(FALSE);

	// It has descendants, so scan the inheritance tree to determine if any of them are in use
	// Scan through the named colours (NOTE: We also scan 'deleted' colours, as a parent colour
	// may be indirectly in use in the undo record)
	IndexedColour *Ptr = (IndexedColour *) GetHead();
	while (Ptr != NULL)												// For all colours in the list...
	{
		if (Ptr != TheColour && Ptr->IsADescendantOf(TheColour))	// If this one is a descendant...
		{				
			if (Ptr->IsInUse(IgnoreColourGallery))					// and it's in use, then we are
				return(TRUE);										// in use, and can return now.
		}

		Ptr = (IndexedColour *) GetNext(Ptr);
	}

	// And scan through the unnamed colours as well
	Ptr = (IndexedColour *) UnnamedColours.GetHead();
	while (Ptr != NULL)												// For all colours in the list...
	{
		if (Ptr != TheColour && Ptr->IsADescendantOf(TheColour))	// If this one is a descendant...
		{				
			if (Ptr->IsInUse())										// and it's in use, then we are
				return(TRUE);										// in use, and can return now.
		}

		Ptr = (IndexedColour *) UnnamedColours.GetNext(Ptr);
	}

	// OK, we're obviously not in use then
	return(FALSE);	
}



/********************************************************************************************

>	BOOL ColourList::NamedColourExists(const char *pName)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com> (Jason copied this code here from colcomp.cpp)
	Created:	08/08/94 (19/12/94)
	Inputs:		pName - the name to look for.
	Returns:	TRUE if the name is already in use;
				FALSE otherwise.
	Purpose:	Determine if a colour name is already in use by any colours stored
				in this colour list
	SeeAlso:	ColourList::GenerateUniqueColourName

********************************************************************************************/

BOOL ColourList::NamedColourExists( PCTSTR pName )
{
	IndexedColour *pItem = (IndexedColour *) GetHead();

	while (pItem != NULL)
	{
		if (!pItem->IsDeleted() && pItem->IsNamed() &&
			(pItem->GetName()->IsIdentical(pName)) )
		{
			// The name is the same - it's a match
			return(TRUE);
		}

		// Try the next colour
		pItem = (IndexedColour *) GetNext(pItem);
	}

	// No match for this name
	return(FALSE);
}



/********************************************************************************************

>	BOOL ColourList::NamedColourExists(const StringBase *pName)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pName - the name to look for.
	Returns:	TRUE if the name is already in use;
				FALSE otherwise.
	Purpose:	Determine if a colour name is already in use by any colours stored
				in this colour list
	SeeAlso:	ColourList::GenerateUniqueColourName

********************************************************************************************/

BOOL ColourList::NamedColourExists(const StringBase *pName)
{
	return(NamedColourExists((const TCHAR *)(*pName)));
}



/********************************************************************************************

>	BOOL ColourList::GenerateUniqueColourName(const StringBase *pPrefix, String_64 *pResult)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (From code by Tim)
	Created:	16/12/94 (8/8/94)

	Inputs:		pPrefix - Points to the suggested name for this colour. This may be NULL,
				in which case a name based on the string "Unnamed" will be generated

	Outputs:	pResult - is filled in with a unique name
				NOTE that this is a String_64, the maximum length for a colour name. If
				the input pPrefix string is longer than will fit, it will be truncated.
				If truncation was necessary, this will have no effect on the return value.
				(IndexedColour::SetName will truncate strings longer than 64 chars as well)

	Returns:	TRUE if the pResult contains a different string from that supplied in
				pPrefix. (pResult will always contain a valid result, but checking this flag
				may save you the bother of copying the string over)

	Purpose:	Scans the entire ColourList to determine if the suggested name is unique
				within this document. If it is not, the name is altered (currently by
				truncating to 50 characters and appending a decimal number) to provide
				a unique name, which is returned in the supplied string.

				This method can be called whenever you wish to generate a unique name for
				a colour. However, note that it is called automatically for every colour
				which is added to the colour list with the AddItem() method

	Notes:		Name matching is done using StringBase::IsIdentical, so see that method
				to determine the rules by which strings are matched

				If the colour is unnamed (pPrefix == NULL, *pPrefix is a NULL string, 
				or the name is "Unnamed" (_R(IDS_UNNAMEDCOLOUR)), then the code is optimised
				to provide a new name for it extra fast.

	SeeAlso:	ColourList::AddItem; IndexedColour::SetName; StringBase::IsIdentical

********************************************************************************************/

BOOL ColourList::GenerateUniqueColourName(const StringBase *pPrefix, String_64 *pResult)
{
	if (pResult == NULL)
	{
		ERROR3("ColourList::GenerateUniqueColourName: pResult is NULL!");
		return(FALSE);
	}

	if (pPrefix != NULL && !NamedColourExists(pPrefix))
	{
		// Either the List is empty, or this name is unique, so return it (ensuring < 64 chars)
		pPrefix->Left(pResult, 63);
		return(FALSE);
	}

	// Copy the prefix into this string.
	String_64 NewName;
	BOOL Unnamed = FALSE;								// Remember if it is 'unnamed'

	if (pPrefix != NULL)
		pPrefix->Left(&NewName, 50);					// Copy into NewName, truncating

	String_64 NoName(_R(IDS_UNNAMEDCOLOUR));
	if ((pPrefix == NULL) || (NewName.Length() == 0))	// Has no name, so call it "Unnamed"
	{
		Unnamed = TRUE;
		NewName = NoName;
	}
	else if (NewName.IsIdentical(NoName))				// Is called 'Unnamed', so remember that
	{
		Unnamed = TRUE;
	}

	INT32 PrefixLen = NewName.Length();
	if (PrefixLen > 50)
	{
		// Limit prefix length to 50 chars - this means we can add numbers up to
		// 10-odd digits long in order to make it unique - should be enough!
		PrefixLen = 50;
	}


	TCHAR *pPrefixEnd = (TCHAR *) NewName;
	pPrefixEnd += PrefixLen;

	// Is this name ok?
	if (!NamedColourExists(NewName))
	{
		(*pResult) = NewName;				// Yes - return it.
		return(TRUE);
	}

	// Start adding numbers to make it unique
	INT32 i = (Unnamed) ? LastUnnamedColour : 2;

	while (TRUE)
	{
		// If this doesn't terminate [Tim] will eat [his] Archimedes!
		tsprintf( pPrefixEnd, 64, _T(" %ld"), i++ );

		// Is this name ok?
		if (!NamedColourExists(NewName))
		{
			(*pResult) = NewName;			// Yes - return it

			if (Unnamed)					// Update unnamed colour generation number
				LastUnnamedColour = i;

			return(TRUE);
		}
	}

	*pResult = NewName;		// Ensure stable return, though this should never happen
	return(TRUE);
}
