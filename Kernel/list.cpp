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
                                         
/**********************************************************************************************
 LIST.CPP - List class definition file.

	This is the Camelot list class. It is an implementation of a doubly linked-list and will be
	used rampantly throughout Camelot.

**********************************************************************************************/


#include "camtypes.h"
													
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]

CC_IMPLEMENT_DYNAMIC( List, CCObject )


#ifdef _DEBUG

/********************************************************************************************

	Preference:		DebugLevel
	Section:		Debug
	Range:			0 to 2
	Purpose:		Controls how much checking is done in the List class.
					0 - No checking
					1 - Normal debug checking
					2 - Extra debug checking
	SeeAlso:		-

********************************************************************************************/

INT32 List::ListDebugLevel = 0;
#endif

/**********************************************************************************************

>	List::List() 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	List class constructor
	Errors:		None

**********************************************************************************************/

List::List() 
{
	HeadItem = NULL;
	TailItem = NULL;
	ItemCount = 0;

#ifdef _DEBUG
	LastItemRemoved = NULL;
#endif
}

/**********************************************************************************************

>	virtual List::~List() 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	List class destructor. NOTE that this does NOT destroy the list items -
				it simply delinks all items in the list and deletes the list header.

				When destructing a list, you should really delete all objects from within
				it (by deriving a class from List with a special destructor, or calling
				List::DeleteAll() on it first, or some other appropriate action) before
				calling delete on the List header.

	Notes:		In DEBUG builds, any non-empty lists are reported to the TRACE output,
				in order to encourage people to discard their used lists thoughtfully.
				To remove this TRACE, you must either delete (DeleteAll()) or delink
				(RemoveItem()) the items from the list before calling the destructor.

				In ALL builds, any items still in the list when it is destructed are delinked
				(RemoveItem()) from the list, in order to ensure their next/previous
				pointers remain valid.

**********************************************************************************************/

List::~List()
{
#ifdef _DEBUG
	if (!IsEmpty())
	{
		TRACE( _T("NON EMPTY LIST DELETED! Its items may appear as memory leaks on exit\n")
			  _T("The %ld items are listed below. I have delinked them from the list\n"),
			  (INT32) GetCount());
	
		// Dump the contents of the list
		ListItem *Ptr = GetHead();
		while (Ptr != NULL)
		{
			TRACE( _T("  '%s' at 0x%x\n"), Ptr->GetRuntimeClass()->GetClassName(), Ptr );
			Ptr = GetNext(Ptr);
		}
		TRACE( _T("\n") );
	}
#endif

	// In all builds, if the list was deleted while non-empty, we delink all items from it
	// to ensure that their next/previous pointers are all NULL, rather than pointing at
	// possibly invalid areas of memory. This will at least mean that the items can be
	// safely added to other lists, without causing complete screwups.
	// These items will show up as memory leaks if they are not properly deleted by anyone
	// so we don't really need to concern ourselves with alerting the programmer too much
	// at this point
	while (!IsEmpty())
		RemoveHead();
}



/**********************************************************************************************

>	virtual ListItem *List::RemoveHead() 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/93
	Inputs:		None
	Outputs:	None
	Returns:	ListItem that was head of list
				NULL if list is empty
	Purpose:
		
		RemoveHead does not as its name may suggest delete the ListItem at the head
		of the list. It instead removes pointers to and from the ListItem at the head
		of the list, in effect removing it from the list. RemoveHead returns a pointer 
		to the	ListItem 'removed'. It is the responsibility of the caller to delete
		the	ListItem from memory. 
 
	Errors:		None.

**********************************************************************************************/

ListItem *List::RemoveHead() 
{
//	CC_ASSERT_VALID(this);

	if (this->IsEmpty())              // if list is empty return a null pointer
		return NULL;
		 				 		
	ListItem *oldHead;

	oldHead = HeadItem;
	HeadItem = HeadItem->Next;		// list head pointed to next item in list

	if (HeadItem != NULL)           // if list is not empty
		HeadItem->Previous = NULL;	//     pointer to old list head must be nullified		
	else
        TailItem = NULL;			//     tail item must be nullified
        
	ItemCount -= 1;					// decrement item counter

#ifdef _DEBUG
	LastItemRemoved = oldHead;
#endif

	return oldHead;
}



/**********************************************************************************************

>	virtual ListItem *List::RemoveTail() 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/93
	Inputs:		None
	Outputs:	None
	Returns:	ListItem that was tail of list
				NULL if it is an empty list
	Purpose:	 
		
		RemoveTail does not as its name may suggest delete the ListItem at the tail
		of the list. It instead removes pointers to and from the ListItem at the tail
		of the list, in effect removing it from the list. RemoveTail returns a pointer 
		to the	ListItem 'removed'. It is the responsibility of the caller to delete
		the	ListItem from memory.
 
	Errors:		None.

**********************************************************************************************/
	
ListItem *List::RemoveTail() 
{
//	CC_ASSERT_VALID(this);

	if (this->IsEmpty())              // if list is empty return a null pointer
		return NULL;
		 		
	ListItem *oldTail;

	oldTail = TailItem;
	TailItem = TailItem->Previous;	// list tail pointed to previous item in list

	if (TailItem != NULL)           // if list is not empty
		TailItem->Next = NULL;		// 	   pointer to old list tail must be nullified
	else
        HeadItem = NULL;			//     head item must be nullified
	
	ItemCount -= 1;					// decrement item counter

#ifdef _DEBUG
	LastItemRemoved = oldTail;
#endif
	
	return oldTail;
}



/********************************************************************************************

>	virtual void List::DeleteAll()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deletes the list, calling the destructors of all ListItems
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void List::DeleteAll()
{
	while (!IsEmpty())
	{
		delete (RemoveTail()); 
	}
}



/**********************************************************************************************

>	virtual void List::AddHead( ListItem* )

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/93
	Inputs:		ListItem to be inserted
	Outputs:	None
	Returns:	void
	Purpose:	To insert a ListItem at the head of the list.
	Errors:		None.

**********************************************************************************************/
/*
Technical notes:

	If a new item is added to the list that already exists the list will become corrupted.
	In order to trap this error the FindPosition() function can be used to scan the list, 
	but this will cause a performance overhead. The solution is to perform the scan only in  
	debug mode so that users of the list are warned during development while release versions 
	of the software do not suffer from inadequate performance.

**********************************************************************************************/

void List::AddHead( ListItem* newHead )
{
//	CC_ASSERT_VALID(this);
    
	if (this->IsEmpty())                // if list is empty
	{                       
		newHead->Next = NULL;           // init next prev pointers
		newHead->Previous = NULL;
		HeadItem = newHead;             // head & tail point to first item
		TailItem = newHead;
	}
	else
	{
#ifdef _DEBUG
		if (ListDebugLevel > 0)
		{
			ENSURE(	this->FindPosition(newHead) == NOT_IN_LIST, 
    				"AddHead: New head item is already in the list");
		}
#endif    	
		newHead->Next = HeadItem;   	// new head pointed to old head
		newHead->Previous = NULL;       // new head previous pointer nullified
		HeadItem->Previous = newHead;   // old head pointed back to new head
		HeadItem = newHead;			    // head pointer pointed to new head
	}
		
	ItemCount += 1;						// increment item counter 

#ifdef _DEBUG
	if (newHead == LastItemRemoved)
		LastItemRemoved = NULL;
#endif
}



/**********************************************************************************************

>	virtual void List::AddTail( ListItem* )

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/93
	Inputs:		ListItem to be inserted
	Outputs:	None
	Returns:	void
	Purpose:	To insert a ListItem at the tail of the list.
	Errors:		None.

**********************************************************************************************/
/*
Technical notes:

	If a new item is added to the list that already exists the list will become corrupted.
	In order to trap this error the FindPosition() function can be used to scan the list, 
	but this will cause a performance overhead. The solution is to perform the scan only in  
	debug mode so that users of the list are warned during development while release versions 
	of the software do not suffer from inadequate performance.

**********************************************************************************************/
	
void List::AddTail( ListItem* newTail ) 
{                                     
//	CC_ASSERT_VALID(this);

	if (this->IsEmpty())                // if list is empty
	{                       
		newTail->Next = NULL;           // init next prev pointers
		newTail->Previous = NULL;
		HeadItem = newTail;             // head & tail point to first item
		TailItem = newTail;
	}
	else
	{
#ifdef _DEBUG
		if (ListDebugLevel > 0)
		{
    		ENSURE(this->FindPosition(newTail) == NOT_IN_LIST, 
    				"AddTail: New tail item is already in the list");
		}
#endif

		newTail->Previous = TailItem; 	// previous points old tail
		newTail->Next = NULL;           // next is nullified  
		TailItem->Next = newTail;       // old tail next points to new tail
		TailItem = newTail;             // update tail pointer
    }
    
	ItemCount += 1;						// increment item counter

#ifdef _DEBUG
	if (newTail == LastItemRemoved)
		LastItemRemoved = NULL;
#endif
}



/**********************************************************************************************

>	ListItem *List::GetHead() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/93
	Inputs:		None
	Outputs:	None
	Returns:	ListItem if not empty
				NULL if empty list
	Purpose:	To allow access to the ListItem at the head of the list.
	Errors:		None.

**********************************************************************************************/
	
ListItem *List::GetHead() const
{
//	CC_ASSERT_VALID(this);
	
	if (this->IsEmpty())
		return NULL;
	else
		return HeadItem;	
}



/**********************************************************************************************

>	ListItem *List::GetTail() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/93
	Inputs:		None
	Outputs:	None
	Returns:	ListItem if not empty
				NULL if list empty
	Purpose:	To allow access to the ListItem at the tail of the list.
	Errors:		None.

**********************************************************************************************/
	
ListItem *List::GetTail( ) const
{
//	CC_ASSERT_VALID(this);
	
	if (this->IsEmpty())
		return NULL;
	else
		return TailItem;
}



/**********************************************************************************************

>	LISTPOS List::GetHeadPosition() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/93
	Inputs:		None
	Outputs:	None
	Returns:	ListPosition of head
				EMPTY_LIST error flag 
	Purpose:	Gives the user the relative position of the head of the list
	Errors:		None.

**********************************************************************************************/
/*
Technical notes: 

	The first position in the list is always zero.

**********************************************************************************************/

LISTPOS List::GetHeadPosition() const
{
//	CC_ASSERT_VALID(this);
	
	if (this->IsEmpty())
		return EMPTY_LIST;
	else
		return 0;
}



/**********************************************************************************************

>	LISTPOS List::GetTailPosition() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/93
	Inputs:		None
	Outputs:	None
	Returns:	ListPosition of tail item
				EMPTY_LIST error flag 
	Purpose:	Gives the user the relative position of the tail of the list
	Errors:		None.


**********************************************************************************************/
/*
Technical notes: 

	The first position in the list is zero therefore the last is:  
			
			( Number of Items - 1 )

**********************************************************************************************/

LISTPOS List::GetTailPosition() const
{
//	CC_ASSERT_VALID(this);
	                      
	if (this->IsEmpty())
		return EMPTY_LIST;
	else                      
		return (ItemCount - 1);
}



/**********************************************************************************************

>	LISTPOS List::FindPosition(ListItem * here) const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/93
	Inputs:		ListItem
	Outputs:	None
	Returns:	ListPosition if found
				NOT_IN_LIST if not found or input is NULL
				EMPTY_LIST if list is empty
	Purpose:	Gives the user the relative position of the ListItem passed in.
	Errors:		None.


**********************************************************************************************/

LISTPOS List::FindPosition(ListItem * here) const
{
//	CC_ASSERT_VALID(this);


	if (this->IsEmpty())          					// if list is empty return a null pointer
		return EMPTY_LIST;                                
   
    if (here == NULL)                               // if input list item is NULL
    	return NOT_IN_LIST;

    ListItem *listIter;								// list iterator 
	LISTPOS listCount = 0;							// list iterator count
                 
    listIter = HeadItem;
    
	while (listIter != here && listIter != NULL)	// iterate until input ListItem found
	{
		listIter = listIter->Next;
		listCount += 1;
	}
        
    if (listIter == NULL)
    	return NOT_IN_LIST;
    else
		return listCount;
}



/**********************************************************************************************

>	ListItem *List::FindItem(LISTPOS here) const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/93
	Inputs:		ListItem
	Outputs:	None
	Returns:	ListItem if found
				NULL if not found or input is less than zero or list is empty
	Purpose:	Gives the user the ListItem at list position passed in.
	Errors:		None.


**********************************************************************************************/

ListItem *List::FindItem(LISTPOS  here) const
{
//	CC_ASSERT_VALID(this);


	if ((this->IsEmpty()) || (here < 0))          	// if list is empty return a null pointer
    	return NULL;

    ListItem *listIter;								// list iterator 
	LISTPOS listCount = 0;							// list iterator count
                 
    listIter = HeadItem;
    
	while (listCount != here && listIter != NULL)	// iterate until input List position found
	{
		listIter = listIter->Next;
		listCount += 1;
	}
        
	return listIter;
}



/**********************************************************************************************

>	virtual ListItem *List::RemoveItem( ListItem* )

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/93
	Inputs:		ListItem
	Outputs:	None
	Returns:	ListItem removed
				NULL if not found or if list empty or input is NULL
	Purpose:	            
	
		Removes the ListItem passed in as a parameter. It is the responsibility of the
		caller to delete the removed ListItem from memory.  
		
	Errors:		In debug builds, an ENSURE is triggered if the item to be removed is not
				actually in the list.


**********************************************************************************************/
/*
Technical notes: 

	Assert is used to trap the error caused by trying to remove an item not in the list. It
    does this by scanning the entire list with the function FindPosition() and if item is not 
	found assert will stop the program. This will only occur in the debug version of the list
	class, the release copy will not include this overhead. 

**********************************************************************************************/

ListItem *List::RemoveItem( ListItem* oldItem )
{
//	CC_ASSERT_VALID(this);

#ifdef _DEBUG
	if (ListDebugLevel > 0)
	{
		ENSURE(	this->FindPosition(oldItem) != NOT_IN_LIST,
				"RemoveItem: List item to be removed is not in the list");
	}
#endif

    if ((this->IsEmpty()) ||                            // if list is empty or
      	(oldItem == NULL))	                            //    NULL input or
    	return NULL;

	ListItem *adjacentItem;
                                                   
	if ((oldItem->Previous) != NULL) 
	{
		adjacentItem = oldItem->Previous;				//  point to previous item in list
		adjacentItem->Next = oldItem->Next;		        //  by pass old item forwards
		if ((oldItem->Next) != NULL)                    // Remove from middle of ist
		{                                               //  there are items both sides
			adjacentItem = oldItem->Next;               //  point to next item in list
			adjacentItem->Previous = oldItem->Previous;	//  by pass old item backwards
		}
		else
		{   											// Removal of tail of list 
			TailItem = adjacentItem;					//  new tail item
		} 
	}	
	else
	{                                                   // Removal of head of list
		if ((oldItem->Next) != NULL)
		{
			adjacentItem = oldItem->Next;				//  next item in list
			adjacentItem->Previous = NULL;              //  nullify prev pointer to old item
			HeadItem = adjacentItem;                    //  new head item		
		}
		else
		{   
			HeadItem = NULL;    		           		// nullify head and tail pointers
			TailItem = NULL;				        
		}
	}
	
	ItemCount -= 1;                                     // decrement item counter

#ifdef _DEBUG
	LastItemRemoved = oldItem;							// Check for stupidity - see GetNext/Prev
#endif

	oldItem->Previous = NULL;							// Er, we should vape the disused links
	oldItem->Next = NULL;								// shouldn't we?

	return oldItem;
}                   



/**********************************************************************************************

>	virtual LISTPOS List::InsertBefore(LISTPOS here, ListItem* newItem)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/93
	Inputs:		ListItem to be inserted
				List position 
	Outputs:	None
	Returns:	List position new item is inserted at.
				INVALID_LISTPOS when the list position goes beyond the list size 
				INVALID_NEWITEM when the new item is NULL
	Purpose:	Inserts a ListItem in the list position before the one that is passed in.
	Notes:		This function involves scanning the entire list until 'here' is found,
				so is much slower on average than the other flavour of InsertBefore
	Errors:		This ALWAYS checks that the item you wish to insert
				before is actually in the list, and that the item you are inserting
				is not already in the list - an ENSURE failure results if not.
	
**********************************************************************************************/
/*
Technical notes:   

	If a new item is added to the list that already exists the list will become corrupted.
	In order to trap this error the FindPosition() function can be used to scan the list, 
	but this will cause a performance overhead. The solution is to perform the scan only in  
	debug mode so that users of the list are warned during development while release versions 
	of the software do not suffer from inadequate performance.
   
**********************************************************************************************/

LISTPOS List::InsertBefore(LISTPOS here, ListItem* newItem)
{           
//	CC_ASSERT_VALID(this);

#ifdef _DEBUG
	if (ListDebugLevel > 0)
	{
 		if (!this->IsEmpty())
   			ENSURE(	this->FindPosition(newItem) == NOT_IN_LIST, 
   					"InsertBefore: Item to be inserted is already in the list");
	}
#endif

	if ((this->IsEmpty()) && (here > 0))
		return INVALID_LISTPOS;
	
	if (newItem == NULL)       						// if input parameter is NULL
		return INVALID_NEWITEM;	
	
	if (here == this->GetHeadPosition())			// if head of list 
	{
		this->AddHead(newItem);                     // insert before head
		return this->GetHeadPosition();
	}

    ListItem *listIter;								// list iterator 
	LISTPOS listCount = this->GetHeadPosition();	// list iterator count
                 
    listIter = HeadItem;
    
	while (listCount != here && listIter != NULL)	// iterate until list position found
	{
		listIter = listIter->Next;
		listCount += 1;
	}

    if (listIter == NULL)                           // list position does not exist in list
		return INVALID_LISTPOS;

	newItem->Next = listIter;						// point new item to before one
	newItem->Previous = listIter->Previous;			// point new item to previous one 
	listIter->Previous = newItem;					// point before item to new one
	newItem->Previous->Next = newItem;				// point previous item back new one
	ItemCount += 1;									// increment item counter

#ifdef _DEBUG
	if (newItem == LastItemRemoved)
		LastItemRemoved = NULL;
#endif

	return listCount;
}



/**********************************************************************************************

>	virtual ListItem *List::InsertBefore(ListItem* here, ListItem* newItem)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/93
	Inputs:		ListItem to be inserted
				ListItem before insertion 
	Outputs:	None
	Returns:	ListItem inserted
				INVALID_NEWITEM when the new item is NULL
				NULL if list empty or ListItem not found or parameters are NULL
	Purpose:	Inserts the 'newItem' ListItem before 'here' ListItem in the list.
	Errors:		In the debug build, this checks that the item you wish to insert
				before is actually in the list, and that the item you are inserting
				is not already in the list - an ENSURE failure results if not.
	
**********************************************************************************************/
/*
Technical notes:   

	If a new item is added to the list that already exists the list will become corrupted.
	In order to trap this error the FindPosition() function can be used to scan the list, 
	but this will cause a performance overhead. The solution is to perform the scan only in  
	debug mode so that users of the list are warned during development while release versions 
	of the software do not suffer from inadequate performance.
   
**********************************************************************************************/

ListItem *List::InsertBefore(ListItem* here, ListItem* newItem)
{
//	CC_ASSERT_VALID(this);

#ifdef _DEBUG
	if (ListDebugLevel > 0)
	{
 		if (!this->IsEmpty())
   			ENSURE(	this->FindPosition(newItem) == NOT_IN_LIST, 
   					"InsertBefore: Item to be inserted is already in the list");
	}
#endif

	if (this->IsEmpty() || (here == NULL) || (newItem == NULL))
		return NULL;
	
	if (here == HeadItem)							// if head of list 
	{
		this->AddHead(newItem);                     // insert before head
		return newItem;
	}

#ifdef _DEBUG
	if (ListDebugLevel > 0)
	{
		ListItem *listIter;								// list iterator
                 
		listIter = HeadItem;
    
		while (listIter != here && listIter != NULL)	// iterate until list position found
			listIter = listIter->Next;

		ENSURE (listIter != NULL, "List::InsertBefore(Here) - 'Here' does not exist!");
	}
#endif

	newItem->Next = here;                   		// point new item to before one
	newItem->Previous = here->Previous;				// point new item to previous one 
	here->Previous = newItem;               		// point before item to new one
	newItem->Previous->Next = newItem;      		// point previous item back new one

#ifdef _DEBUG
	if (newItem == LastItemRemoved)
		LastItemRemoved = NULL;
#endif

	ItemCount += 1;									// increment item counter
	return newItem;
}



/**********************************************************************************************

>	virtual LISTPOS List::InsertAfter(LISTPOS here, ListItem* newItem)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/93
	Inputs:		ListItem to be inserted
				List position 
	Outputs:	None
	Returns:	List position new item is inserted at.
				INVALID_LISTPOS when the list position goes beyond the list size
				INVALID_NEWITEM when new list item is NULL
	Purpose:	Inserts a ListItem in the list position after the one that is passed in.
	Notes:		To find 'here', the list must be scanned, so this is much slower on
				average than the other flavour of InsertAfter
	Errors:		This ALWAYS checks that the item you wish to insert
				after is actually in the list, and that the item you are inserting
				is not already in the list - an ENSURE failure results if not.

**********************************************************************************************/
/*
Technical notes:   

	If a new item is added to the list that already exists the list will become corrupted.
	In order to trap this error the FindPosition() function can be used to scan the list, 
	but this will cause a performance overhead. The solution is to perform the scan only in  
	debug mode so that users of the list are warned during development while release versions 
	of the software do not suffer from inadequate performance.
   
**********************************************************************************************/

LISTPOS List::InsertAfter(LISTPOS here, ListItem* newItem)
{
//	CC_ASSERT_VALID(this);

#ifdef _DEBUG
	if (ListDebugLevel > 0)
	{
 		if (!this->IsEmpty())
   			ENSURE(	this->FindPosition(newItem) == NOT_IN_LIST, 
   					"InsertAfter: Item to be inserted is already in the list");
	}
#endif

	if ((this->IsEmpty()) && (here > 0))
		return INVALID_LISTPOS;
		
	if (newItem == NULL)       						
		return INVALID_NEWITEM;
	
	if (here == this->GetTailPosition())			// if tail of list 
	{
		this->AddTail(newItem);                     // insert after tail
		return this->GetTailPosition();
	}

    ListItem *listIter;								// list iterator 
	LISTPOS listCount = GetHeadPosition();			// list iterator count
                 
    listIter = HeadItem;
    
	while (listCount != here && listIter != NULL)	// iterate until list position found
	{
		listIter = listIter->Next;
		listCount += 1;
	}

    if (listIter == NULL)                           // list position does not exist in list
		return INVALID_LISTPOS;


	newItem->Previous = listIter;					// point new item to previous one in list
	newItem->Next = listIter->Next;             	// point new item to next one
	listIter->Next = newItem;		            	// point after item to new one
	newItem->Next->Previous = newItem;				// point next item back to new one
	ItemCount += 1;									// increment item counter

#ifdef _DEBUG
	if (newItem == LastItemRemoved)
		LastItemRemoved = NULL;
#endif

	return (listCount + 1);
}



/**********************************************************************************************

>	virtual ListItem *List::InsertAfter(ListItem* here, ListItem* newItem)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/93
	Inputs:		ListItem to be inserted
				ListItem after insertion 
	Outputs:	None
	Returns:	ListItem inserted
				NULL if list empty or ListItem not found or parameters are NULL
	Purpose:	Inserts the 'newItem' ListItem after 'here' ListItem in the list.
	Errors:		In the debug build, this checks that the item you wish to insert
				after is actually in the list, and that the item you are inserting
				is not already in the list - an ENSURE failure results if not.

**********************************************************************************************/
/*
Technical notes:   

	If a new item is added to the list that already exists the list will become corrupted.
	In order to trap this error the FindPosition() function can be used to scan the list, 
	but this will cause a performance overhead. The solution is to perform the scan only in  
	debug mode so that users of the list are warned during development while release versions 
	of the software do not suffer from inadequate performance.
   
**********************************************************************************************/

ListItem *List::InsertAfter(ListItem* here, ListItem* newItem)
{
//	CC_ASSERT_VALID(this);

#ifdef _DEBUG
	if (ListDebugLevel > 0)
	{
 		if (!this->IsEmpty())
   			ENSURE(	this->FindPosition(newItem) == NOT_IN_LIST, 
   					"InsertAfter: Item to be inserted is already in the list");
	}
#endif

	if ((this->IsEmpty()) || (here == NULL) || (newItem == NULL))
		return NULL;
		
	
	if (here == TailItem)							// if tail of list 
	{
		this->AddTail(newItem);                     // insert after tail
		return newItem;
	}

#ifdef _DEBUG
	if (ListDebugLevel > 0)
	{
		ListItem *listIter = HeadItem;

		while (listIter != here && listIter != NULL)	// iterate until list position found
			listIter = listIter->Next;

		if (listIter == NULL)                           // list position does not exist in list
		{
			return NULL;
		}
	}
#endif

	newItem->Previous = here;						// point new item to previous one in list
	newItem->Next = here->Next;             		// point new item to next one
	here->Next = newItem;		            		// point after item to new one
	newItem->Next->Previous = newItem;      		// point next item back to new one

#ifdef _DEBUG
	if (newItem == LastItemRemoved)
		LastItemRemoved = NULL;
#endif

	ItemCount += 1;									// increment item counter
	return newItem;
}   



/**********************************************************************************************

>	UINT32 List::GetCount() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/93
	Inputs:		None
	Outputs:	None
	Returns:	Number of items in the list
	Purpose:	To give user of list an indication of its size in terms of number of items. 
	Errors:		None

**********************************************************************************************/

UINT32 List::GetCount() const
{
//	CC_ASSERT_VALID(this);

	return ItemCount;
}



/**********************************************************************************************

>	BOOL List::IsEmpty() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/93
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if list is empty
				FALSE if list is not empty
	Purpose:	Allows user to see if list is empty or not
	Errors:		None

**********************************************************************************************/

BOOL List::IsEmpty() const
{
//	CC_ASSERT_VALID(this);

	if (ItemCount == 0)
		return TRUE;
	else
		return FALSE;
}

/********************************************************************************************

>	BOOL List::CreateIndex(List* ListIndex)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Outputs:	A List of in-order pointers into 'this' list. Each item on the list is a
				ListItemIdx.
	Returns:	FALSE if we run out of memory. All items on the output list are deleted
				if this occurs.
	Purpose:	Creates a list of in-order pointers into this list. 
	
				This has many uses eg.

				You wish to scan a list repeatedly, not wasting time considering items
				you are not interested in. This method avoids changing the order of the
				items within the list.

				Could be used for list sorting etc.
				 
	SeeAlso:	ListItemIdx

********************************************************************************************/

BOOL List::CreateIndex(List* ListIndex)
{
	
	ListItem* pi = GetHead();	  // pointer to this lists item
	ListItemIdx* ppi;			  // pointer to pi
	while (pi)
	{
		ppi = new ListItemIdx();
		if (!ppi)
		{
			ListIndex->DeleteAll(); // Tidyup
			return FALSE;	
		}
		ppi->pItem = pi;
		ListIndex->AddTail(ppi);
		pi = GetNext(pi);  
	}
	return TRUE;
} 



/**********************************************************************************************

>	void List::AssertValid() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/93
	Inputs:		None
	Outputs:	None
	Returns:	void
	Purpose:	              
	
		Validates the internal state of the object. If an inconsistency is detected
		an error message is indicated. This function will not be included in the release 
		version.
	
	Errors:		ENSURE failures will occur if the list is found to be inconsistent

**********************************************************************************************/
/*
Technical notes: 

		Use of AfxIsValidAddress will cause portability problems if it is not implemented as 
		part of the memory management functions of the OIL. It is currently being used on a
		temporary basis.

**********************************************************************************************/

#ifdef _DEBUG
void List::AssertValid() const
{
	CCObject::AssertValid();

	if (ItemCount == 0)
	{
		// empty list
		ENSURE(HeadItem == NULL, "Head item of a non-empty list should never be null");
		ENSURE(TailItem == NULL, "Tail item of a non-empty list should never be null");
	}
}
#endif   

// End of List class
