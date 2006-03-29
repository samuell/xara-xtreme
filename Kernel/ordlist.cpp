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

// ordlist.cpp -- the OrderedList class

/*
*/


#include "camtypes.h"

#include "ensure.h"
//#include "galstr.h"			// For string resources
#include "ordlist.h"
#include "progress.h"


#if TRUE
CC_IMPLEMENT_DYNAMIC(OrderedList, CCObject)
CC_IMPLEMENT_MEMDUMP(ComparatorInfo, ListItem)
CC_IMPLEMENT_MEMDUMP(SequenceItem, ListItem)
CC_IMPLEMENT_MEMDUMP(OrderSequence, ListItem)


#define new CAM_DEBUG_NEW


/**********************************************************************************************

>	ComparatorInfo::ComparatorInfo()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a ComparatorInfo object
	Notes:		This constructor should not really be used; if you do use it, then make
				sure that you set at least the SortFn and ResourceID fields of the new
				object to legal values, else OrderedList clients may explode.
	SeeAlso:	OrderedList

**********************************************************************************************/

ComparatorInfo::ComparatorInfo()
{
	SortFn		= NULL;
	CanReverse	= TRUE;
}


/**********************************************************************************************

>	ComparatorInfo::ComparatorInfo(const ComparatorInfo &other)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Copy constructor for a ComparatorInfo object
	Notes:		This constructor should not really be used; if you do use it, then make
				sure that you set at least the SortFn and ResourceID fields of the new
				object to legal values, else OrderedList clients may explode.
	SeeAlso:	OrderedList

**********************************************************************************************/

ComparatorInfo::ComparatorInfo(const ComparatorInfo &other)
{
	SortFn		= other.SortFn;
	SortName	= other.SortName;
	CanReverse	= other.CanReverse;
}



/**********************************************************************************************

>	ComparatorInfo::ComparatorInfo(ListComparator Compare, UINT32 SortName, BOOL Reverse = TRUE)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/94
	Inputs:		Compare -  The Comparator function used for the sort
				SortName - a resource ID for a string describing the sort
				(e.g. "Sort by Name", "Sort by Date", etc)
				Reverse - TRUE (the default) if the sort can be applied in reverse
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a ComparatorInfo
	SeeAlso:	OrderedList

**********************************************************************************************/

ComparatorInfo::ComparatorInfo(ListComparator Compare, UINT32 SortNameID, BOOL Reverse)
{
	SortFn		= Compare;
	SortName.MakeMsg(SortNameID);
	CanReverse	= Reverse;
}



/**********************************************************************************************

>	ComparatorInfo::ComparatorInfo(ListComparator Compare, const String_32 &Name,
										BOOL Reverse = TRUE)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/94
	Inputs:		Compare -  The Comparator function used for the sort
				Name - a string_32 describing the sort
				(e.g. "Sort by Name", "Sort by Date", etc)
				Reverse - TRUE (the default) if the sort can be applied in reverse
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a ComparatorInfo
				This allows you to set the name given a String_32, which means that
				'external' entities such as tools can provide sort modes.
	SeeAlso:	OrderedList

**********************************************************************************************/

ComparatorInfo::ComparatorInfo(ListComparator Compare, const String_32 &Name, BOOL Reverse)
{
	SortFn		= Compare;
	SortName	= Name;
	CanReverse	= Reverse;
}













/**********************************************************************************************

>	SequenceItem::SequenceItem()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a SequenceItem
	SeeAlso:	OrderSequence; OrderedList

**********************************************************************************************/

SequenceItem::SequenceItem()
{
	MasterItem = NULL;
}














/**********************************************************************************************

>	OrderSequence::OrderSequence()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a OrderSequence
	SeeAlso:	SequenceItem; OrderedList

**********************************************************************************************/

OrderSequence::OrderSequence()
{
	Comparator	= NULL;
	Cached		= FALSE;
	Reversed	= FALSE;
	UsageCount	= 0;
}



/**********************************************************************************************

>	OrderSequence::OrderSequence(ListComparator Compare, BOOL Reverse)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		Compare - A comparator function which determines the sort order
				Reverse - TRUE if you want the list sorted in reverse order				
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a OrderSequence
	SeeAlso:	SequenceItem; OrderedList; keyword ListComparator

**********************************************************************************************/

OrderSequence::OrderSequence(ListComparator Compare, BOOL Reverse)
{
	Comparator	= Compare;
	Cached		= FALSE;
	Reversed	= Reverse;
	UsageCount	= 0;
}


















/**********************************************************************************************

>	OrderedList::OrderedList() 

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OrderedList class constructor
	Notes:		Remember to call OrderedList::Init() after creating

**********************************************************************************************/

OrderedList::OrderedList()
{
	SortOrders = NULL;		// This is properly initialised in the Init() function


	for (INT32 i = 0; i < MaxSortKeys; i++)
	{
		SortMode[i].Info	 = NULL;
		SortMode[i].Reversed = FALSE;
	}
}



/**********************************************************************************************

>	OrderedList::~OrderedList() 

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OrderedList class destructor
	Errors:		None

**********************************************************************************************/

OrderedList::~OrderedList()
{
	// Clear all encapsulated master and sequence items
	DeleteAll();

	// Clear away our list of provided sort modes
	if (SortOrders != NULL)
	{
		SortOrders->DeleteAll();
		delete SortOrders;
	}

	if (!Sequences.IsEmpty())
	{
#ifdef _DEBUG
/*
		// Warn Jason that someone's being rampant
		JCWTRACE( _T("  ** Warning: OrderedList deleted with Sequences non-empty! Cleaning up...\n"));

		ListItem *Ptr = Sequences.GetHead();
		while (Ptr != NULL)
		{
			// Check - if in use, somebody forgot to deselect an order.
			if ( ((OrderSequence *) Ptr)->UsageCount > 0 )
				JCWTRACE( _T("    I was forced to delete an in-use sequence!\n"));
			Ptr = Sequences.GetNext(Ptr);
		}
*/
#endif

		Sequences.DeleteAll();
	}
}



/**********************************************************************************************

>	BOOL OrderedList::Init(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/94
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if initialisation failed (lack of memory)

	Purpose:	This initialises the OrderedList.

	Notes:		This includes creation of a List of avaliable Sorts.

				NO LONGER AVAILABLE:
				The OrderedList base class provides only one sort method - "Unsorted"
				Derived classes may override this function if they don't want 'Unsorted'
				to be an available method.
				If you want this, then all you need to do is uncomment this stuff and
				add a string resource baclk in for its name.

**********************************************************************************************/

BOOL OrderedList::Init(void)
{
	if (SortOrders == NULL)		// Only init if we have not already done so
	{
		SortOrders = new List;
		if (SortOrders == NULL)
			return(FALSE);

//		ComparatorInfo *SortOrder;
//
//		SortOrder = new ComparatorInfo(Unsorted, _R(IDS_SORTBY_UNSORTED), FALSE);
//		if (SortOrder == NULL)
//			return(FALSE);
//
//		SortOrders->AddTail(SortOrder);
	}

	return(TRUE);
}



/**********************************************************************************************

>	BOOL OrderedList::AddItem(ListItem *TheItem)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		ListItem - The item to add to the master list
	Outputs:	-
	Returns:	TRUE if it succeeds, FALSE if there isn't enough memory
	Purpose:	Adds the given ListItem to the OrderedList.
	Notes		This adds entries to all sequences in this OrderedList which refer
				to the new item in the master list. All lists will be resorted to
				place this item correctly the next time they are SelectOrder'd

**********************************************************************************************/

BOOL OrderedList::AddItem(ListItem *TheItem)
{
	MasterList.AddTail(TheItem);

	OrderSequence *TheSeq = (OrderSequence *) Sequences.GetHead();
	SequenceItem *NewItem;

	while (TheSeq != NULL)
	{
		TheSeq->Cached = FALSE;						// This sequence needs re-sorting

		NewItem = new SequenceItem;
		if (NewItem == NULL)
		{
			ENSURE(FALSE, "OrderedList::AddItem failed to allocate new SequenceItem");
			return(FALSE);
		}

		NewItem->MasterItem = TheItem;				// Set up the new SeqItem
		TheSeq->Sequence.AddTail(NewItem);	 		// Add it to this Sequence

		TheSeq = (OrderSequence *) Sequences.GetNext(TheSeq);	// Move on to the next sequence
	}	

	return(TRUE);
}


/**********************************************************************************************

>	ListItem *OrderedList::RemoveMasterItem(ListItem *TheItem)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		TheItem - A pointer to the item which you want to remove
	Outputs:	-
	Returns:	The item that was removed (returns the pointer as passed in)
	Purpose:	Delinks the given item from the OrderedList. TheItem is NOT 
				deleted - it is up to you to do this when you're finished with it

	Notes:		(Removes the item from the Master List, and deletes all
				reference to it from all sequence lists. TheItem is NOT deleted)

**********************************************************************************************/

ListItem *OrderedList::RemoveMasterItem(ListItem *TheItem)
{
	MasterList.RemoveItem(TheItem);

	OrderSequence *TheSeq = (OrderSequence *) Sequences.GetHead();
	SequenceItem *SeqItem;

	while (TheSeq != NULL)
	{
		SeqItem = (SequenceItem *) TheSeq->Sequence.GetHead();
		while (SeqItem != NULL)
		{
			if (TheItem == SeqItem->GetItem())
				break;

			SeqItem = (SequenceItem *) TheSeq->Sequence.GetNext(SeqItem);
		}


		if (SeqItem == NULL)
		{
			ENSURE(FALSE, "OrderedList::RemoveItem- Expected SequenceItem was not found!");
			return(TheItem);
		}

		TheSeq->Sequence.RemoveItem(SeqItem);
		delete SeqItem;

		TheSeq = (OrderSequence *) Sequences.GetNext(TheSeq); // Move on to the next sequence
	}	

	return(TheItem);
}



/**********************************************************************************************

>	void OrderedList::DeleteAll(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Removes all items in the OrderedList, calling all their destructors
	Notes:		This includes ALL items in all sequences AND THE MASTER LIST held
				by the OrderedList (i.e. deletes all enclosed objects)

**********************************************************************************************/

void OrderedList::DeleteAll(void)
{
	ListItem *Ptr = MasterList.GetHead();
	ListItem *Next;

	while (Ptr != NULL)
	{
		Next = MasterList.GetNext(Ptr);
		RemoveMasterItem(Ptr);
		delete Ptr;

		Ptr = Next;
	}
}



/**********************************************************************************************

>	void OrderedList::InvalidateCaches(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Informs the OrderedList that something you've done to the list or a
				ListItem in it could possibly change the sort order.

	Notes:		This causes all sequences to be resorted (reasonably quick if they're
				already almost in the correct order) the next time they are SelectOrder'd.
				Note that this is done automatically if you add items through the 
				OrderedList interface, so you should only need to call this if you change
				data in a listitem which is (or potentially may be) used as a sort key.
				Note that OrderedList::InvalidateSequenceCache can be used on a single
				Sequence, but this should only be used if you can absolutely positively
				guarantee (beyond a shadow of a doubt and more certainly than a really 
				certain certainty) that your change will only ever affect the sort order
				of that one sequence.

	SeeAlso:	OrderedList::InvalidateSequenceCache

**********************************************************************************************/

void OrderedList::InvalidateCaches(void)
{
	OrderSequence *TheSeq = (OrderSequence *) Sequences.GetHead();

	while (TheSeq != NULL)
	{
		TheSeq->Cached = FALSE;
		TheSeq = (OrderSequence *) Sequences.GetNext(TheSeq);
	}
}



/**********************************************************************************************

>	OrderSequence *OrderedList::FindSequence(ListComparator Compare, BOOL Reverse)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		Compare - A ListComparator function defining the sort order
				Reverse - TRUE if you want a reverse-order sort
	Outputs:	-
	Returns:	-
	Purpose:	Internal list search method. This searches the list of available
				sequences, and returns a pointer to the first one that matches
				the given comparator function, or NULL if there is no such sequence
				available.
	Scope:		private

**********************************************************************************************/

OrderSequence *OrderedList::FindSequence(ListComparator Compare, BOOL Reverse)
{
	if (Compare == NULL)
		return(NULL);

	OrderSequence *TheSeq = (OrderSequence *) Sequences.GetHead();

	while (TheSeq != NULL)
	{
		if (TheSeq->Comparator == Compare && TheSeq->Reversed == Reverse)
			return((OrderSequence *) TheSeq);

		TheSeq = (OrderSequence *) Sequences.GetNext(TheSeq); // Move on to the next sequence
	}

	return(NULL);
}


/**********************************************************************************************

>	OrderSequence *OrderedList::CreateSequence(ListComparator Compare, BOOL Reverse)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		Compare - A ListComparator function defining the sort order
				Reverse - TRUE if you want a reverse-order sort
	Outputs:	-
	Returns:	-
	Purpose:	Internal list maintenance method. If, after calling
				OrderedList::FindSequence, you can't find a given sequence, then
				you can create a new instance of that sequence by calling this function.
				It is added to the end of the list, and no checks for duplicates are made
				(if a duplicate exists, the first one created will always be used)

**********************************************************************************************/

OrderSequence *OrderedList::CreateSequence(ListComparator Compare, BOOL Reverse)
{
	if (Compare == NULL)
		return(NULL);

	OrderSequence *TheSeq;

	TheSeq = new OrderSequence(Compare, Reverse);
	if (TheSeq == NULL)
		return(NULL);

	Sequences.AddTail(TheSeq);

	ListItem *Ptr = MasterList.GetHead();
	SequenceItem *NewItem;

	while (Ptr != NULL)
	{
		NewItem = new SequenceItem;
		if (NewItem == NULL)
		{
			ENSURE(FALSE, "OrderedList::CreateSequence failed to allocate new SequenceItem");
			return(NULL);
		}

		NewItem->MasterItem = Ptr;					// Set up the new SeqItem
		TheSeq->Sequence.AddTail(NewItem);	 		// Add it to this Sequence

		Ptr = MasterList.GetNext(Ptr);
	}

	return(TheSeq);
}



/**********************************************************************************************

>	BOOL OrderedList::SortSequence(OrderSequence *TheSeq)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		TheSeq - The OrderSequence to be sorted
				Reverse - TRUE if you want it sorted into reverse order
	Outputs:	-
	Returns:	-
	Purpose:	Internal list maintenance method. Sorts the given sequence into
				order using a sort and the user-supplied comparator function.
				This sounds slow, but (a) we're only intending to sort short lists, so
				should be fast enough, (b) It's not a completely silly implementation, and
				(c) the sort is only used when the sequence is created, and when it gets
				out of order.  In the latter case, it usually means that the list is very
				close to correct sorted order, and only 1 or 2 passes will be needed to
				bring the list back to fully sorted order.
				(Probably if this sort turns out to be slow, it will be best to change
				to a faster sort for the original (creation) sort, and leave this in place
				for resorting the near-correct list)
				Just in case, SlowJob progress indicators are displayed while sorting

**********************************************************************************************/

BOOL OrderedList::SortSequence(OrderSequence *TheSeq)
{
	if (TheSeq == NULL || TheSeq->Comparator == NULL)	// Error!
		return(FALSE);

	if (TheSeq->Cached)			// Well, I reckon it's already sorted. Hah
		return(TRUE);

	SequenceItem *ItemA;
	SequenceItem *ItemB;
	SequenceItem *ItemC;

	BOOL Sorted;
	INT32 CompareResult;

// Commented out 11/1/95 by Markn because Jason told me to. And it fixes one of my bugs.
// Sometimes this gets called when there's already a slow job going (e.g. import with layers with layer gal open).
//
//	BeginSlowJob(-1);			// Show hourglass, with delay, no percentage

	do
	{
		Sorted = TRUE;			// Assume in sorted order until we have to move an item
		ItemA = (SequenceItem *) TheSeq->Sequence.GetHead();
	
		while (ItemA != NULL)
		{
			ContinueSlowJob();

			ItemB = (SequenceItem *) TheSeq->Sequence.GetNext(ItemA);
 			if (ItemB == NULL)		// At end of list, so have finished this pass
				break;
 
			if (TheSeq->Reversed)
				CompareResult = TheSeq->Comparator(this, ItemB->GetItem(), ItemA->GetItem());
			else
				CompareResult = TheSeq->Comparator(this, ItemA->GetItem(), ItemB->GetItem());

			if (CompareResult > 0)
			{
				Sorted = FALSE;		// Have to move an item - can't be in order yet

				// Delink ItemA from the list - it has to move up
				TheSeq->Sequence.RemoveItem(ItemA);

				// Search for proper position for A...
				ItemC = (SequenceItem *) TheSeq->Sequence.GetNext(ItemB);
				while (ItemC != NULL)
				{
					if (TheSeq->Reversed)
						CompareResult = TheSeq->Comparator(this, ItemC->GetItem(), ItemA->GetItem());
					else
						CompareResult = TheSeq->Comparator(this, ItemA->GetItem(), ItemC->GetItem());

					if (CompareResult <= 0)	// Have found place to drop ItemA...
						break;

					ItemC = (SequenceItem *) TheSeq->Sequence.GetNext(ItemC);
				}

				if (ItemC == NULL)
					TheSeq->Sequence.AddTail(ItemA);			 // Belongs at end of list
				else
					TheSeq->Sequence.InsertBefore(ItemC, ItemA); // Belongs just before ItemC
			}

 			ItemA = ItemB;				// Loop, from the next unsorted item
		}
	} while (!Sorted);

//	EndSlowJob();		BeginSlowJob() statement see above

	TheSeq->Cached = TRUE;
	return(TRUE);
}



/**********************************************************************************************

>	List *OrderedList::SelectOrder(ListComparator Compare, BOOL Reversed = FALSE)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		Compare - a list comparator function which determines how the list
				will be sorted.
				Reversed - TRUE if you wish the list to be sorted using the comparator into
				reverse order.
	Outputs:	-
	Returns:	A pointer to a List of SequenceItems, sorted as desired, or
				NULL if it failed.

	Purpose:	Selects a given order for use.

				Once selected, you can traverse the OrderedList much like an ordinary
				list (see the SeeAlso comments) only you'll find that the items are 
				returned by GetNext/Prev in an order dicatated by the ListComparator
				function that you supplied on selection.

	Notes:		Selecting has the following effects:
				* Increments a UsageCount (which is decremented by DeselectOrder)
				- While selected, a sequence cannot be DestroyOrder'd
				* Creates (if necessary) a new sequence within the OrderedList
				* Sorts (if necessary) the list using the given comparator function

				Sorting occurs when the sequence is first created, and thereafter when
				SelectOrder() is called AND it is determined that the list order may be
				incorrect (when item(s) have been added, or InvalidataeCache menthods
				have been invoked)

				If an 'Unsorted' order is requested, this actually returns the Master
				List; this can be traversed in exactly the same manner as an OrderedList
				(as normal ListItems also have a GetItem() method).

				Passing 'Compare' == NULL will ignore the 'Reversed' parameter and 
				simply return the master list.

	SeeAlso:	OrderedList::InvalidateCaches; OrderedList::DeselectOrder;
				OrderedList::DestroyOrder; Keyword ListComparator

**********************************************************************************************/

List *OrderedList::SelectOrder(ListComparator Compare, BOOL Reverse)
{
	if (Compare == NULL || Compare == Unsorted)
		return(&MasterList);

	OrderSequence *TheSeq = FindSequence(Compare, Reverse);	// Find existing Seq?

	if (TheSeq == NULL)										// No - create new one
		TheSeq = CreateSequence(Compare, Reverse);

	if (TheSeq == NULL)										// Failed - argh!
		return(NULL);

	if (!TheSeq->Cached)									// Needs sorting?
		SortSequence(TheSeq);

	TheSeq->UsageCount++;									// Increment the usage count

	return(&TheSeq->Sequence);
}


/**********************************************************************************************

>	void OrderedList::ReSortOrder(ListComparator Compare, BOOL Reversed = FALSE)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		Compare - a list comparator function which determines how the list
				will be sorted.
				Reversed - TRUE if you wish the list to be sorted using the comparator into
				reverse order.
	Outputs:	-
	Returns:	-
	Purpose:	Immediately (re-)sorts the given order. If no such order exists, no
				action is taken (a new order sequence will NOT be created, as this will
				happen anyway if you subsequently try to Select it)
				Normally, if you add an item to the order sequence, it is not sorted into its
				correct position until the next time the order is selected; The item will be
				added at the tail of the list. This allows you to continue scanning the list
				knowing that it has not shifted about under you. However, if adding items to
				the list you may want to immediately force a resort to ensure it is in order
				(if you are not going to SelectOrder on the list before using it again)

				Passing 'Compare' == NULL will ignore the 'Reversed' parameter and 
				simply do nothing (as this asks you to resort the master list).


	SeeAlso:	OrderedList::InvalidateCaches; OrderedList::SelectOrder;
				Keyword ListComparator

**********************************************************************************************/

void OrderedList::ReSortOrder(ListComparator Compare, BOOL Reverse)
{
	OrderSequence *TheSeq = FindSequence(Compare, Reverse);	// Find existing Seq?

	if (TheSeq == NULL)										// No such seq - exit
		return;

	SortSequence(TheSeq);									// Force a sort
}



/**********************************************************************************************

>	void OrderedList::DeselectOrder(ListComparator Compare, BOOL Reversed = FALSE)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		Compare - The list comparator function which defined the sort order
				Reversed -  TRUE if you wish to deselect the Reverse-sorted order
	Outputs:	-
	Returns:	-
	Purpose:	This indicates to the OrderedList that you have finished using the given
				sequence (list sort-order). This unlocks the List so that the sequence may
				be deleted using the DestroyOrder function.
	Notes:	   	Passing 'Compare' == NULL will ignore the 'Reversed' parameter and 
				simply 'deselect' the master list (this in fact returns, doing nothing).

	SeeAlso:	OrderedList; OrderedList::SelectOrder; OrderedList::DestroyOrder
	Errors:		This ENSUREs that the sequence exists (has been Selected and not yet Destroy'd)
				and that the UsageCount is legal (that Deselect has not been called more often
				than Select)

**********************************************************************************************/

void OrderedList::DeselectOrder(ListComparator Compare, BOOL Reversed)
{
	if (Compare == NULL || Compare == Unsorted)
		return;

	OrderSequence *TheSeq = FindSequence(Compare, Reversed);	// Find existing Seq?	

	ENSURE(TheSeq != NULL, "OrderedList::DeselectOrder called for nonexistent sequence!");
	ENSURE(TheSeq->UsageCount > 0, "OrderedList::DeselectOrder called more often than SelectOrder!");

	TheSeq->UsageCount--;							// Decrement the usage count
}



/**********************************************************************************************

>	void OrderedList::DestroyOrder(ListComparator Compare, BOOL Reversed = FALSE)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		Comparator - The comparator function which determines the sort order
				Reversed - TRUE if you wish to destroy the reverse-sorted order.
	Outputs:	-
	Returns:	-
	Purpose:	Destroys an order sequence. This is not strictly necessary, but allows
				you to make the OrderedList release all memory used by the sequence if
				you know that you no longer need to acces that sort order. (If you may
				want to look at the list again, it is probably more efficient to never
				call Destroy, so that the cached sort order is available to you in the
				future, without having to re-sort the items)

	Notes:		The sequence will ONLY be destroyed if its usage count is zero (if the
				order has been deselected exactly the same number of times it has been
				selected. This means you can call Destroy after every use of the order
				if you wish, and it will be deleted only if nobody else is sharing it.

				Passing 'Compare' == NULL will ignore the 'Reversed' parameter and 
				simply do nothing (You can't destroy the master list).

	SeeAlso:	OrderedList::SelectOrder; OrderedList::DeselectOrder
	Errors:		In the debug build, this ENSURES that you don't try to destroy an order
				which does not exist, and that the order is not in use (selected) when you
				try to destroy it. In the release build, this will just delete the sequence
				if it can be found

**********************************************************************************************/

void OrderedList::DestroyOrder(ListComparator Compare, BOOL Reversed)
{
	if (Compare == NULL || Compare == Unsorted)
		return;

	OrderSequence *TheSeq = FindSequence(Compare, Reversed);	// Find existing Seq?

	ENSURE(TheSeq != NULL, "OrderedList::DestroyOrder called for nonexistent sequence!");

	if (TheSeq != NULL && TheSeq->UsageCount == 0)
	{
		Sequences.RemoveItem(TheSeq);
		TheSeq->Sequence.DeleteAll();
		delete TheSeq;
	}
}




/**********************************************************************************************

>	INT32 OrderedList::Unsorted(OrderedList *ParentList, ListItem *Item1, ListItem *Item2)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		ParentList - the OrderedList in which these items are linked
				Item1, Item2 - the two items to be compared
	Outputs:	-
	Returns:	A negative value if Item1 is considered "less than" Item2

				A positive value if Item1 is considered "greater than" Item2
	
				Zero if Item1 is considered "equal to" Item2

	Purpose:	"Unsorted" List sort ComparatorFunction, and an example of writing
				ListComparators.

				Determines if two items in an OrderedList need to be swapped during
				a sort operation. The function returns a value indicating if the
				items were considered equal, or if one item was "greater than" or
				"less than" the other.

	Notes:		This particular function always returns zero, so that it imposes no order
				at all upon the list, thus giving an "Unsorted" List order.
				
				Normally, we would code the function to return the 3 possible results
				e.g. If we wish to compare two integers to sort them into ascending
				order, we should return(Item1->IntegerValue - Item2->IntegerValue)
				NOTE, however, that care must be taken to always return an 'INT32' 
				compatible value (i.e. if comparing INT32s, you should code to 
				return -1, 0, or +1, using explicit < and == comparisons)

				In your own ListComparator functions, you should attempt to check the
				runtime type of the provided ListItems to ensure they are of an
				appropriate type to be sorted using your function.

	SeeAlso:	ListComparator

**********************************************************************************************/

INT32 OrderedList::Unsorted(OrderedList *ParentList, ListItem *Item1, ListItem *Item2)
{
	return(0);
}



/**********************************************************************************************

>	List *OrderedList::GetSortOrders(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to a list of ComparatorInfo objects enumerating the
				sort orders available from this OrderedList object.
	Purpose:	Find a list of available sorting orders.
	SeeAlso:	ComparatorInfo; ListComparator

**********************************************************************************************/

List *OrderedList::GetSortOrders(void)
{
	return(SortOrders);
}



/**********************************************************************************************

>	void OrderedList::AddSortOrder(const ComparatorInfo &CompInfo)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/94
	Inputs:		CompInfo - a ComparatorInfo object describing the new sort method.
				Note that this will be duplicated and added into the list if such
				a sort is not already in the list.
	Outputs:	-
	Returns:	-
	Purpose:	Add a sort order to the list of those available from this
				OrderedList object.
	SeeAlso:	ComparatorInfo; ListComparator

**********************************************************************************************/

void OrderedList::AddSortOrder(const ComparatorInfo &CompInfo)
{
	ComparatorInfo *Ptr;

	ENSURE(SortOrders != NULL,
			"Someone forgot to call OrderedList::Init before calling AddSortOrder");

	if (SortOrders == NULL)
		return;

	Ptr = (ComparatorInfo *) SortOrders->GetHead();
	while (Ptr != NULL)
	{
		if (Ptr->GetSortFn() == CompInfo.GetSortFn() &&
			Ptr->GetSortName() == CompInfo.GetSortName())
			return;			// Already have this sort mode in the list

		Ptr = (ComparatorInfo *) SortOrders->GetNext(Ptr);
	}

	// Have failed to find this order in the list, so add it
	Ptr = new ComparatorInfo(CompInfo);
	if (Ptr != NULL)
		SortOrders->AddTail(Ptr);
}


#endif
