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

// ordlist.h -- the OrderedList class

#if TRUE
// OrderedList is no longer used by anything
// If you think you might want to use an OrderedList, ask Jason about them first
// and he'll try to make you see the error of your ways...

#ifndef INC_ORDLIST
#define INC_ORDLIST


//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]



/***********************************************************************************************

<	ListComparator

	Comment:	typedef INT32 (*ListComparator)(OrderedList *ParentList,
												ListItem *A, ListItem *B);

				This defines a function type which is passed to and used
				by OrderedList objects to sort OrderedList sequences for you.

				The function should return negative, 0, or positive values to
				indicate that A is considered less-than, equal-to, or greater-than B.

				'ParentList' is a pointer to the OrderedList object which contains
				the list on whose items you are operating. This is useful in some cases
				(e.g. the ColourList can provide colour contexts for its sort functions
				to use; these are used on a per-document basis and may change the
				resulting sort order).

				Note that you should be very careful only to use a ListComparator
				function on OrderedLists made up of appropriate ListItem objects.
				It would be best, if possible, to use runtime classing to check
				this in an ENSURE for the debug build, to avoid accidental misuse.

	SeeAlso:	OrderedList::SelectOrder; OrderedList::Unsorted
	Documentation:	HowToUse\ordlist.doc

***********************************************************************************************/

class OrderedList;
typedef INT32 (*ListComparator)(OrderedList *, ListItem *, ListItem *);


/***********************************************************************************************

>	class ComparatorInfo : public ListItem

	Purpose:	This defines an object which is used to describe a sorting order.

				A list of these objects is provided by all OrderdList objects, to
				detail all of the sort orders which may be used on that type of
				OrderedList. (The OrderedList base class provides only one sort order,
				"Unsorted")

				The idea behind this is to derive classes from the OrderedList
				(see the ColourList class, collist.h, for an example that does this)
				which provide a list of available sort modes that can be used on that
				type of OrderedList. Thus, new sort modes can be added to the list class
				and they can automatically appear as new sort-by modes in related dialogue
				boxes etc.

				Each ComparatorInfo contains a resource ID which references a name for
				the sort type, e.g."Sort by Name", "Sort by Date", etc, plus a function
				pointer to a ListComparator function which defines the sort order.

				External entities (tool dlls etc) can call OrderedList::GetSortOrders
				and List::AddTail to add their own sort orders to the available ones.

	SeeAlso:	OrderedList::GetSortOrders; keyword ListComparator; ColourList
	Documentation:	HowToUse\ordlist.doc

***********************************************************************************************/

class CCAPI ComparatorInfo : public ListItem
{

CC_DECLARE_MEMDUMP(ComparatorInfo)

public:
	ComparatorInfo();
	ComparatorInfo(const ComparatorInfo &other);
	ComparatorInfo(ListComparator Compare, UINT32 SortName, BOOL Reverse = TRUE);
	ComparatorInfo(ListComparator Compare, const String_32 &Name, BOOL Reverse = TRUE);

	inline const String_32 &GetSortName(void) const	{ return SortName; };
	inline ListComparator GetSortFn(void) const		{ return SortFn; };
	inline BOOL CanBeReversed(void) const	   		{ return CanReverse; };

protected:
	String_32 SortName;	
	ListComparator SortFn;		// Comparator function that defines the sort order
	BOOL CanReverse;			// TRUE if this sort can be applied in reverse
};




/***********************************************************************************************

<	SortKey; MaxColourSortKeys

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Purpose:	Holds information describing a sort key - the ComparatorInfo
				describing the sort, and a boolean determining if the sort is 
				in reverse order.

				For examples of use, see the ColourList, ColourBar, and ColourSortDlg,
				which provide a 2-key sort mechanism using arrays of SortKeys.
				These arrays are MaxColourSortKeys in length
	Documentation:	HowToUse\ordlist.doc

***********************************************************************************************/

typedef struct
{
	ComparatorInfo	*Info;
	BOOL			Reversed;
} SortKey;


const INT32 MaxSortKeys = 2;


/***********************************************************************************************

>	class SequenceItem : public ListItem

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/94
	Friends:	OrderedList
	Purpose:	A list of SequenceItems is used to point to an OrderedList's
				Master List Items in an arbitrary order. Each SequenceItem is 
				thus made up solely of a Master-Item pointer.
	Scope:		Private (to OrderedList)
	SeeAlso:	OrderedList
	Documentation:	HowToUse\ordlist.doc

***********************************************************************************************/

class CCAPI SequenceItem : public ListItem
{
friend class OrderedList;

CC_DECLARE_MEMDUMP(SequenceItem)

public:
	SequenceItem();

	inline ListItem *GetItem(void);

protected:
	ListItem *MasterItem;			// Ptr to the actual Master List item
};



/***********************************************************************************************

>	inline ListItem *SequenceItem::GetItem(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/94
	Friends:	
	Purpose:	Returns the actual ListItem referred to by a SequenceItem.
	SeeAlso:	OrderedList

***********************************************************************************************/

ListItem *SequenceItem::GetItem(void)
{
	return(MasterItem);
}




/***********************************************************************************************

>	class OrderSequence : public ListItem

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/94
	Friends:	OrderedList
	Purpose:	An OrderedList owns a set of OrderSequence objects. Each OrderSequence
				is a list of SequenceItems which describe the Master List in a given
				order.
	SeeAlso:	OrderedList; SequenceItem
	Documentation:	HowToUse\ordlist.doc

***********************************************************************************************/

class CCAPI OrderSequence : public ListItem
{
friend class OrderedList;

CC_DECLARE_MEMDUMP(OrderSequence)

public:
	OrderSequence();
	OrderSequence(ListComparator Compare, BOOL Reverse = FALSE);

protected:
	INT32				UsageCount;		// Number of clients who currently have this 'selected'
	BOOL			Cached;			// TRUE if this list has been sorted into correct order
	BOOL			Reversed;		// TRUE if this list is in reversed order
	ListComparator	Comparator;		// Sort-comparator which give the sort order; also used as ID
	List			Sequence;		// List of SequenceItems
};



/***********************************************************************************************

>	class OrderedList : public CCObject

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/94
	Purpose:	See docs... how\ordlist.doc
			 	This class represents a linked list of ListItems, which can have more than
				one oredering. (i.e. a set of items which are retained in several different
				sequences).

				This is achieved using a master list of all the list items stored in the list,
				which is stored in order of creation. For each desired sort-order, an
				OrderSequence is held, which is a list of SequenceItems - each SequenceItem is
				a pointer to one item in the master list (there is a 1:1 relationship between
				master ListItems and SequenceItems is a given Sequence).

				Note that after Creating an OrderedList, you should call its Init() function.

	Notes:		You MUST remember to call OrderedList::Init() before using the list

				OrderedLists are used in the following manner:

					OrderSequence *Seq = MyOrderedList->SelectOrder(SortByName);
					if (Seq != NULL)
					{
						SequenceItem *Item = Seq->GetFirst();

						while(Item != NULL)
						{
							Item->GetItem()->DoSomething();
							Item = Seq->GetNext(Item);
						}
					}
					MyOrderedList->DeselectOrder(SortByName);					

				Special points of interest are:
				Selection/Deselection.
					These fuctions find the sequence of your choice, using the sort
					comparator function (In this example 'SortByName') as a key.
					When Selected for the first time, a new sequence will be created
					and sorted using this function; this list is then cached for later
					use. A count of the number of selections is kept in each list, so
					that they can be deleted again when they are not in use.

				GetItem()
					As a SequenceItem is a list item containing a pointer to the master
					list item, an extra level of indirection is needed to find the actual
					list item.

				Other than this, an OrderSequence is a normal list for traversal purposes.

				It is MOST IMPORTANT that these lists be treated as read-only, and that all
				write-operations on them are done through the OrderedList interface.
				Although some functions seem to be missing (e.g. InsertAfter), these functions
				are pointless on an OrderedList which will be sorted anyway!

				You can, however, apply any operation to Master/Sequence list items, so long as
				the result leaves all master items at the same memory addresses, and leaves all
				lists containing the same set of items. (i.e. you can move an item to a new
				position in the master list, but you cannot replace an item with a new item
				unless you explicitly overwrite the data at that memory address)

	Documentation:	HowToUse\ordlist.doc
				
***********************************************************************************************/

class CCAPI OrderedList : public CCObject
{
	CC_DECLARE_DYNAMIC( OrderedList )

public:
	OrderedList();					 				// List constructor
	~OrderedList();			 						// List destructor

	// Initialisation
	virtual BOOL Init(void);

	// List manipulation functions
	BOOL AddItem(ListItem *Item);

	ListItem *RemoveMasterItem(ListItem *TheItem);
	inline ListItem *RemoveItem(SequenceItem *TheItem);

	void DeleteAll(void);

	inline DWORD GetCount() const;

	void InvalidateCaches(void);
	inline void InvalidateSequenceCache(OrderSequence *TheSequence);	

	inline List *GetMasterList(void);


	// Functions for "connecting to" a particular ordering of the list
	List *SelectOrder(ListComparator Compare, BOOL Reverse = FALSE);
	void DeselectOrder(ListComparator Compare, BOOL Reverse = FALSE);
	void DestroyOrder(ListComparator Compare, BOOL Reverse = FALSE);

	void ReSortOrder(ListComparator Compare, BOOL Reverse = FALSE);


	// Access to the sort orders available from this class
	List *GetSortOrders(void);
	void AddSortOrder(const ComparatorInfo &CompInfo);

	inline void SetOwner(void *NewOwner);
	inline void *GetOwner(void) const;

public:
	SortKey SortMode[MaxSortKeys];
		// The OrderedList contains a sort mode defined by several sort keys,
		// which can be used by any client if they so desire. For code that
		// does just this, see the kernel files gallery.h and gallery.cpp
		// This data is NOT used by the OrderedList, though it is initialised on
		// construction to contain {SortFn=NULL, Revered=FALSE} items.

protected:
	List MasterList;			// Master list of actual stored ListItems
	List Sequences;				// List of the available ordering sequences

	List *SortOrders;  			// List of available sorting functions

	void *Owner;				// Pointer to the object that 'owns' this list

	// Internal list management routines
	OrderSequence *FindSequence(ListComparator Compare, BOOL Reverse);
	OrderSequence *CreateSequence(ListComparator Compare, BOOL Reverse);
	BOOL SortSequence(OrderSequence *TheSeq);

	static INT32 Unsorted(OrderedList *, ListItem *, ListItem *);
};



/***********************************************************************************************

>	inline void OrderedList::RemoveItem(SequenceItem *TheItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Returns:	-
	Purpose:	This function removes an item from the master list, given a SequenceItem
				which references it. The item is really removed from the master list, and
				ALL sequences are updated to no longer include this item.

***********************************************************************************************/

ListItem *OrderedList::RemoveItem(SequenceItem *TheItem)
{
	return(RemoveMasterItem(TheItem->GetItem()));
}



/***********************************************************************************************

>	inline void OrderedList::InvalidateSequenceCache(Sequence *TheSequence)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Purpose:	Indicates to an OrderedList that a Sequence you are working on
				is no longer guaranteed to be in the correct order (this will cause
				the list to be re-sorted at the next call to SelectOrder). However,
				generally you will probably need to use OrderedList::InvalidateCaches
				if you change a list, in order to ensure that all sequences are
				resorted.
	Scope:		Generally intended to be private
	SeeAlso:	OrderedList::InvalidateCaches

***********************************************************************************************/

void OrderedList::InvalidateSequenceCache(OrderSequence *TheSequence)
{
	TheSequence->Cached = FALSE;
}


/***********************************************************************************************

>	inline List *OrderedList::GetMasterList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Returns:	A pointer to the master list
	Purpose:	To find the Master list of ordinary ListItems on which the sequences of
				this OrderedList are based. This is a normal List of the items, in no
				particular order (in fact, this is guaranteed to be the creation order
				of the OrderedList; Each new item is appended to the tail of the list)

	Notes:		IMPORTANT
	
				This should be treated strictly as a READ ONLY list!

				If you delete/insert items in this list, the OrderedList will fall flat
				on its face, trash memory, and insult your mother recursively as it dies.

				You must only party on items in this list via the main OrderedList interface

***********************************************************************************************/

List *OrderedList::GetMasterList(void)
{
	return(&MasterList);
}



/***********************************************************************************************

>	inline DWORD GetCount() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/94
	Returns:	The number of items in the list
	Purpose:	To find the number of items in the ordered list

***********************************************************************************************/

DWORD OrderedList::GetCount() const
{
	return(MasterList.GetCount());
}



/***********************************************************************************************

>	inline void OrderedList::SetOwner(void *NewOwner)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/94
	Inputs:		NewOwner - A user-defined ID pointer to set as my 'owner'
	Purpose:	Sets the given pointer as the 'owner' of this list
				(Used as an id system for the Gallery Subsystem)
	SeeAlso:	Gallery

***********************************************************************************************/

void OrderedList::SetOwner(void *NewOwner)
{
	Owner = NewOwner;
}



/***********************************************************************************************

>	inline void *OrderedList::GetOwner(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/94
	Returns:	The 'owner' pointer
	Purpose:	Gets the 'owner' pointer of this list
				(Used as an id system for the Gallery Subsystem)
	SeeAlso:	Gallery

***********************************************************************************************/

void *OrderedList::GetOwner(void) const
{
	return(Owner);
}

#endif

#endif

