// $Id: uielem.cpp 662 2006-03-14 21:31:49Z alex $
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
// 


#include "camtypes.h"
//#include "dialogop.h"
#include "gadget.h"
#include "uielem.h"
//#include "tmpltdlg.h"


/**************************************************************************************

>	class VisibleListItemPtr : public ListItem

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Provides a container element for a visible list

**************************************************************************************/
class VisibleListItemPtr : public ListItem
{
public:
	VisibleListItemPtr(VisibleListItem& Item) : m_Item(Item)	{}
	virtual ~VisibleListItemPtr()								{	delete &m_Item;	}

	VisibleListItem& GetEntry() const							{	return m_Item;	}
private:
	VisibleListItem&		m_Item;
};


// Place any IMPLEMENT type statements here
CC_IMPLEMENT_MEMDUMP(UserInterface, CC_CLASS_MEMDUMP);
CC_IMPLEMENT_MEMDUMP(VisibleListItem, UserInterface);
CC_IMPLEMENT_MEMDUMP(VisibleList, UserInterface);


// We want better memory tracking
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

#define ENSURE_KIND(pInstance, Class)	ERROR3IF(!pInstance->IS_KIND_OF(Class), #pInstance" is not kind of "#Class);

// Functions follow


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//				V  I  S  I  B  L  E  L  I  S  T  I  T  E  R  A  T  O  R
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //





/*******************************************************************************************

>	VisibleListIterator::VisibleListIterator(	List& Container, 
												VisibleListItemPtr* const pPointer, 
												INT32* IndexesOfItemsToReturn = NULL)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	This constructor provides two variants of a VisibleListIterator:
				1.	Returns all entries in the given list.
				2.	Returns only those items whose indexes are contained in the given INT32
					array.
				(These should probably be two separate classes)

	Inputs:		Container:	The container of VisibleListItemPtr's
				pPointer:	The item in the container with which the iteration will begin
				IndexesOfItemsToReturn:	An array of INT32's, representing indexes of items in 
							the Container (starting from 0, in ascending order and 
							terminated by -1)

*******************************************************************************************/
VisibleListIterator::VisibleListIterator(List& Container, VisibleListItemPtr* const pPointer, 
										INT32* IndexesOfItemsToReturn) :
	m_VisibleListItemPtr(pPointer), 
	m_Container(Container), 
	m_IndexesOfItemsToReturn(IndexesOfItemsToReturn), 
	m_IndexInIndex(0)
{
	if (IndexesOfItemsToReturn != NULL)
	{
		m_VisibleListItemPtr = (VisibleListItemPtr*)m_Container.FindItem(LISTPOS(m_IndexesOfItemsToReturn[0]));
	}
}


/*******************************************************************************************

>	VisibleListIterator::~VisibleListIterator()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Provides a destructor to delete any array given in the constructor.

*******************************************************************************************/
VisibleListIterator::~VisibleListIterator()
{
	if (m_IndexesOfItemsToReturn != NULL)
	{
		delete [] m_IndexesOfItemsToReturn;
	}
}



/*******************************************************************************************

>	VisibleListIterator::VisibleListIterator(const VisibleListIterator& Other)
	const VisibleListIterator& VisibleListIterator::operator=(const VisibleListIterator& Other)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Warns that these have not been implemented (but a destructor has)

*******************************************************************************************/
VisibleListIterator::VisibleListIterator(const VisibleListIterator& Other) : 
	m_Container(Other.m_Container)
{	
	ERROR3("VisibleListIterator - copy constructor unimplemented\n");	
}


const VisibleListIterator& VisibleListIterator::operator=(const VisibleListIterator& Other)
{	
	ERROR3("VisibleListIterator - assignment operator not implemented\n");	

	return *this;
}


/*******************************************************************************************

>	VisibleListIterator& VisibleListIterator::operator++()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Moves on the iterator to the next entry in the VisibleList

	Returns:	An iterator pointing at the next entry.

*******************************************************************************************/
VisibleListIterator& VisibleListIterator::operator++()
{
	if (m_IndexesOfItemsToReturn == NULL)
	{
		m_VisibleListItemPtr = (VisibleListItemPtr*)m_Container.GetNext(m_VisibleListItemPtr);
	}
	else
	{
		++m_IndexInIndex;
		if (m_IndexesOfItemsToReturn[m_IndexInIndex] == -1)
		{
			m_VisibleListItemPtr = NULL;
		}
		else
		{
			m_VisibleListItemPtr = (VisibleListItemPtr*)m_Container.FindItem(LISTPOS(m_IndexesOfItemsToReturn[m_IndexInIndex]));
		}
	}

	return *this;
}


/*******************************************************************************************

>	VisibleListIterator& VisibleListIterator::operator--()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Moves the iterator to the previous entry in the VisibleList

	Returns:	An iterator pointing at the next entry.

*******************************************************************************************/
VisibleListIterator& VisibleListIterator::operator --()
{
	if (m_IndexesOfItemsToReturn == NULL)
	{
		m_VisibleListItemPtr = (VisibleListItemPtr*)m_Container.GetPrev(m_VisibleListItemPtr);
	}
	else
	{
		if (m_IndexInIndex == 0)
		{
			m_VisibleListItemPtr = NULL;
		}
		else
		{
			--m_IndexInIndex;
			m_VisibleListItemPtr = (VisibleListItemPtr*)m_Container.FindItem(LISTPOS(m_IndexesOfItemsToReturn[m_IndexInIndex]));
		}
	}

	return *this;
}


/*******************************************************************************************

>	BOOL VisibleListIterator::operator!=(const VisibleListIterator& Other)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Enables us to determine when an iteration has finished, a la
					while (Iterator != Container.End())

	Returns:	TRUE if the iterator's are pointing at different VisibleListEntry's
				FALSE if they're the same

*******************************************************************************************/
BOOL VisibleListIterator::operator!=(const VisibleListIterator& Other)
{
	return m_VisibleListItemPtr != Other.m_VisibleListItemPtr;
}


/*******************************************************************************************

>	VisibleListItem& VisibleListIterator::operator*()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Allows us to get at the VisibleListItem pointed to by this iterator.

	Returns:	A reference to the VisibleListItem

	Errors:		ERROR2's if the iterator's gone off the end of the container.

*******************************************************************************************/
VisibleListItem& VisibleListIterator::operator*()
{
	ERROR3IF(m_VisibleListItemPtr == NULL, "NULL Member");

	return m_VisibleListItemPtr->GetEntry();
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//							V  I  S  I  B  L  E  L  I  S  T
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //






/*******************************************************************************************

>	VisibleList::~VisibleList()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Virtual destructor clears its lists

*******************************************************************************************/
VisibleList::~VisibleList()
{
	DeleteAllEntries();
}


/*******************************************************************************************

>	virtual BOOL VisibleList::Display(DialogOp& Dialog)
	virtual BOOL VisibleList::Interact(DialogOp& Dialog)
	virtual void VisibleList::Hide()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Hmm.. haven't quite decided what these should do.
				How about: 
					Display - show disabled
					Interact - show enabled
					Hide - remove from sight

*******************************************************************************************/
BOOL VisibleList::Display(DialogOp& Dialog)
{
	return TRUE;
}


BOOL VisibleList::Interact(DialogOp& Dialog)
{
	return TRUE;
}

void VisibleList::Hide()
{
}


BOOL VisibleList::Display(ListControl& Control)
{
	m_pUIElement = &Control;
	Control.Disable();
	// should display the entries we have and InsertEntry, etc. only update if we've
	// got a UIElement, but for now we'll insist on a display before InsertEntry, etc.
	return TRUE;
}


BOOL VisibleList::Interact(ListControl& Control)
{
	BOOL Ok = Display(Control);
	Control.Enable();
	return Ok;
}


/*******************************************************************************************

>	List& VisibleList::GetContainer()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Support function to exposeget the VisibleList's innards

*******************************************************************************************/
List& VisibleList::GetContainer()
{
	return m_Container;
}
/*******************************************************************************************

>	BOOL VisibleList::InsertEntry(VisibleListItem& EntryToInsert)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Inserts an entry into a visible list

	Returns:	FALSE if it fails

*******************************************************************************************/
BOOL VisibleList::InsertEntry(VisibleListItem& EntryToInsert)
{
	BOOL Success = TRUE;		// return this

	// Find the entry to insert after & its index
	BOOL Found = FALSE;
	INT32 Index = 0;
	VisibleListItemPtr* pItemToInsertBefore = NULL;

	VisibleListItemPtr* pCurrentItem = (VisibleListItemPtr*)m_Container.GetHead();
	while (pCurrentItem != NULL && !Found)
	{
		if (!(EntryToInsert > pCurrentItem->GetEntry()))
		{
			pItemToInsertBefore = pCurrentItem;
			Found = TRUE;
		}
		else
		{
			++Index;
			pCurrentItem = (VisibleListItemPtr*)m_Container.GetNext(pCurrentItem);
		}
	}

	// Create a new pointer to the given entry and put it in the container
	VisibleListItemPtr* pNewItem = NULL;
	if (Success)
	{
		pNewItem = new VisibleListItemPtr(EntryToInsert);
		Success = (pNewItem != NULL);
	}

	if (Success)
	{
		if (!Found)
		{
			m_Container.AddTail(pNewItem);
		}
		else
		{
			Success = (m_Container.InsertBefore(pItemToInsertBefore, pNewItem) != NULL);
		}
	}

	// Add a text entry for the VisibleListItem in the UI Element (if it's there)
	if (Success && m_pUIElement != NULL)
	{
		String_64 ListText;
		EntryToInsert.GetText(ListText);
		if (!Found)
		{
			Success = m_pUIElement->AddItem(ListText);
		}
		else
		{
			Success = m_pUIElement->InsertItem(ListText, Index);
		}
	}

	// tidy up any mess if it goes wrong
	if (!Success)
	{
		delete pNewItem;
		pNewItem = NULL;
	}

	return Success;
}


/*******************************************************************************************

>	virtual VisibleListItem& VisibleList::GetEntryAt(UINT32 Index)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Support function to get the Index'th entry in the list

	Returns:	The VisibleListItem at the given Index

*******************************************************************************************/
VisibleListItem& VisibleList::GetEntryAt(UINT32 Index)
{
	VisibleListItemPtr* const pEntry = (VisibleListItemPtr*)GetContainer().FindItem(LISTPOS(Index));
	if (pEntry == NULL)
	{
		ERROR2RAW("NULL member");
	}

	return pEntry->GetEntry();
}


/*******************************************************************************************

>	virtual BOOL VisibleList::DeleteEntryAtIndex(UINT32 Index)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Deletes the Index'th entry in this list

	Returns:	FALSE if it fails

*******************************************************************************************/
BOOL VisibleList::DeleteEntryAtIndex(UINT32 Index)
{
	BOOL Success = TRUE;		// return this

//#ifdef _DEBUG	
//	String_256 ListText = m_pUIElement->GetText(pText)	_R(IDC_TMLPTDLG_USED), &Success, 
//															Index);
//	VisibleListItem* pItem = pEntryToDelete->GetEntry();

//		ASSERT(ListText == pEntryToDelete->GetText());

	VisibleListItemPtr* pEntryToDelete = (VisibleListItemPtr*)m_Container.FindItem(LISTPOS(Index));
	Success = (pEntryToDelete != NULL);

	if (Success)
	{
		Success = (m_Container.RemoveItem(pEntryToDelete) != NULL);
	}

	if (Success)
	{
		delete pEntryToDelete;
		pEntryToDelete = NULL;
	}

	// Remove the entry from the display if it's there
	if (Success && m_pUIElement != NULL)
	{
		Success = m_pUIElement->DeleteItemAtIndex(Index);
	}


	return Success;
}


/*******************************************************************************************

>	virtual BOOL VisibleList::DeleteAllEntries()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Deletes all the entries in this list

	Returns:	FALSE if it fails

*******************************************************************************************/
BOOL VisibleList::DeleteAllEntries()
{
	BOOL DeletedAllEntries = TRUE;

	m_Container.DeleteAll();

	if (m_pUIElement != NULL)
	{
		DeletedAllEntries = m_pUIElement->DeleteAllItems();
	}

	return DeletedAllEntries;
}


/*******************************************************************************************

>	virtual BOOL VisibleList::DeleteSelectedEntries()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Deletes the selected entries in this list

	Returns:	FALSE if it fails

*******************************************************************************************/
BOOL VisibleList::DeleteSelectedEntries()
{
	ENSURE_NOT_NULL(m_pUIElement);

	// Delete the selected argument from the used list and copy it to the available list
	INT32* pIndexes = NULL;
	BOOL Success = m_pUIElement->GetSelectedIndexes(&pIndexes);

	UINT32 ArrayIndex = 0;
	UINT32 EntriesDeleted = 0;
	while (Success && pIndexes[ArrayIndex] != -1)
	{
		const UINT32 IndexOfEntryToDelete = pIndexes[ArrayIndex] - EntriesDeleted;
		Success = DeleteEntryAtIndex(IndexOfEntryToDelete);

		++EntriesDeleted;
		++ArrayIndex;
	}

	if (pIndexes != NULL)
	{
		delete pIndexes;
		pIndexes = NULL;
	}

	return Success;
}


/*******************************************************************************************

>	UINT32 VisibleList::GetNumberOfEntries() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Determines how much space you would need for the whole list were you 
				building an array.

	Returns:	The Number of Entries

*******************************************************************************************/
UINT32 VisibleList::GetNumberOfEntries() const
{
	return UINT32(m_Container.GetCount());
}


/*******************************************************************************************

>	UINT32 VisibleList::GetNumberOfSelectedEntries() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Determines if there's a selection

	Returns:	The Number of Entries Selected

*******************************************************************************************/
UINT32 VisibleList::GetNumberOfSelectedEntries() const
{
	UINT32 NumberOfSelectedEntries = 0;

	if (m_pUIElement != NULL)
	{
		NumberOfSelectedEntries = m_pUIElement->GetNumberOfSelectedItems();
	}

	return NumberOfSelectedEntries;
}


/*******************************************************************************************

>	VisibleListItem* VisibleList::GetFirstSelectedEntry() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Gets the VisibleListItem that's selected in this list

	Returns:	NULL if there isn't one (or the list isn't visible)

*******************************************************************************************/
VisibleListItem* VisibleList::GetFirstSelectedEntry() const
{
	VisibleListItem* pSelectedEntry = NULL;		// return this

	INT32 SelectedIndex = 0;

	if (m_pUIElement != NULL && m_pUIElement->GetFirstSelectedIndex(&SelectedIndex))
	{
		VisibleListItemPtr* pEntryPtr = (VisibleListItemPtr*)m_Container.FindItem(LISTPOS(SelectedIndex));
		if (pEntryPtr != NULL)
		{
			pSelectedEntry = &pEntryPtr->GetEntry();
		}
	}

	return pSelectedEntry;
}



/*******************************************************************************************

>	void VisibleList::RemoveSelection()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Deselects any entries in this list

*******************************************************************************************/
void VisibleList::RemoveSelection()
{
	VOID_ENSURE_NOT_NULL(m_pUIElement);

	m_pUIElement->DeselectAll();
}


/*******************************************************************************************

>	virtual VisibleListIterator VisibleList::Begin()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Returns an iterator so you can run through all the entries in this list

*******************************************************************************************/
VisibleListIterator VisibleList::Begin()
{
	return VisibleListIterator(m_Container, (VisibleListItemPtr*)m_Container.GetHead());
}


/*******************************************************************************************

>	virtual VisibleListIterator VisibleList::BeginOfEnd()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Returns an iterator so you can back through all the entries in this list

*******************************************************************************************/
VisibleListIterator VisibleList::BeginOfEnd()
{
	return VisibleListIterator(m_Container, (VisibleListItemPtr*)m_Container.GetTail());
}


/*******************************************************************************************

>	virtual const VisibleListIterator& VisibleList::End()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	So you know when to stop iterating use this 'cos it marks the end of this
				list.

*******************************************************************************************/
const VisibleListIterator& VisibleList::End() const
{
	// doesn't matter about which list it's in
	List				list;
	static const VisibleListIterator TheEnd( list, NULL );	

	return TheEnd;
}



/*******************************************************************************************

>	VisibleListIterator VisibleList::SelectionBegin()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Returns an iterator so you can run through the selected entries in this list

*******************************************************************************************/
VisibleListIterator VisibleList::SelectionBegin()
{
	ERROR2IF(m_pUIElement == NULL, End(), "NULL Member");

	INT32* pIndexes = NULL;
	if (m_pUIElement->GetSelectedIndexes(&pIndexes))
	{
		return VisibleListIterator(m_Container, (VisibleListItemPtr*)m_Container.GetHead(), pIndexes);
	}
	else
	{
		return End();
	}
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//						E  D  I  T  A  B  L  E  T  E  X  T
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //






/********************************************************************************************

>	EditableText::EditableText(const StringBase& TextString)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Provides the user with a string that they can edit and we can retrieve

	Notes:		This was taken from MSJ, but is not complete, so be careful. In particular,
				1.	The interface for Display, etc. is not clarified, (what goes in, what comes
				out).
				2.	Notification is not used properly.

				The following code kicks out:

				warning C4355: 'this' : used in base member initializer list.

				Normally, it would be dangerous for the contained-object
				constructor to access the container class because the contained
				objects are initialized before the container is initialized.
				We don't do that here, and it's convenient to pass a pointer
				to the container at construction time, so turn off the warning.

********************************************************************************************/
#pragma warning( disable : 4355 )

EditableText::EditableText(const StringBase& TextString) :
	m_Watcher(this),
	m_pUIElement(NULL),
	m_String(TextString)
{
}

#pragma warning( default : 4355 )


/********************************************************************************************

>	EditableText::~EditableText()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Ensures the text in the control / gadget is empty

********************************************************************************************/
EditableText::~EditableText()
{
	if (m_pUIElement != NULL)
	{
		m_pUIElement->UpdateText(NullString);
	}
}


/********************************************************************************************

>	EditableText::EditableText(const EditableText& Other)
	const EditableText& EditableText::operator=(const EditableText& Other)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Warns that these members are unimplemented (destructor is though)

********************************************************************************************/
#pragma warning( disable : 4355 )

EditableText::EditableText(const EditableText& Other) :
	m_Watcher(this)
{	
	ERROR3("EditableText - copy constructor unimplemented\n");	
}

#pragma warning( default : 4355 )


const EditableText& EditableText::operator=(const EditableText& Other)
{	
	ERROR3("EditableText - assignment operator not implemented\n");	

	return *this;
}


/********************************************************************************************

>	const EditableText& EditableText::operator=(const StringBase& NewString)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Puts the given text into the UIElement

********************************************************************************************/
const EditableText& EditableText::operator=(const StringBase& NewString)
{
	m_String = NewString;

	Notify(&m_Watcher);

	return *this;
}


/********************************************************************************************

>	EditableText::operator const StringBase&()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Pretends this EditableText is a StringBase, so it can be assigned and stuff

********************************************************************************************/
EditableText::operator const StringBase&()
{
	if (m_pUIElement != NULL)
	{
		m_pUIElement->RetrieveText(&m_String);
	}

	return m_String;
}


/********************************************************************************************

>	void EditableText::Observer::Notify(Notifier* const pSender)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Embedded object catches Notifications (doesn't do anything en ce moment, so
				don't use Notify

********************************************************************************************/
void EditableText::Observer::Notify(Notifier* const pSender)
{
	// This function is a friend of class Text, a pointer
	// to which is in the owner field; It's called when the
	// String base class changes. Note that we have to turn
	// notify off in whichever object is being updated. Otherwise,
	// we'll just end up back here.

	EditableText* const pText = (EditableText*)(pSender);
	if (pText != NULL)
	{
		// String base class was changed, transfer the new value
		// to the display.
		TextControl* const pUI = pText->m_pUIElement;
		if (pUI != NULL)
		{
			pUI->NotifyOff();
			pUI->UpdateText(*m_pTheTextObject);	// modify the string
			pUI->NotifyOn();
		}
	}
	else // User changed the TextControl, copy new value to
	{	 // the string base class of the associated text object.

		TextControl* pUI = (TextControl*)(pSender);
		ASSERT(pUI != NULL);
		
		pUI->NotifyOff();			// prevent string from notifying us
		pUI->RetrieveText(&m_pTheTextObject->m_String);
		pUI->NotifyOn();			// re-enable notification
	}
}


/********************************************************************************************

>	BOOL EditableText::Display(TextControl& Control)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Displays this EditableText in the given TextControl.

	Notes:		This interface may very well change. At present there's one of these taking
				a DialogOp (inherited from UserInterface) which does nothing.
				There's also an Interact function missing: should it just enable the 
				control?

********************************************************************************************/
BOOL EditableText::Display(TextControl& Control)
{
	m_pUIElement = &Control;
	Control.UpdateText(m_String);

	return TRUE;
}


/********************************************************************************************

>	void EditableText::Hide()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Ensures the Text in the control is the text inside this EditableText object.

	Notes:		This interface may very well change.
				At present the control is emptied and disabled: should it disappear 
				completely?

********************************************************************************************/
void EditableText::Hide()
{
	if (m_pUIElement != NULL)
	{
		m_pUIElement->RetrieveText(&m_String);
		m_pUIElement->UpdateText(NullString);
		m_pUIElement->Disable();
		m_pUIElement = NULL;
	}
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//								V  I  S  I  B  L  E  L  A  B  E  L
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //






/********************************************************************************************

>	VisibleLabel::VisibleLabel(const StringBase& TextString)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Provides the user with a label so they can see what's what. And it allows us
				to re-label things.

	Notes:		This is very much like EditableText: should it be the same?
				The original MSJ version had a read-only EditableText object.

********************************************************************************************/
#pragma warning( disable : 4355 )
VisibleLabel::VisibleLabel(const StringBase& TextString) :
	m_Watcher(this),
	m_pUIElement(NULL),
	m_String(TextString)
{
}
#pragma warning( default : 4355 )



/********************************************************************************************

>	VisibleLabel::~VisibleLabel()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Provides the user with a label so they can see what's what. And it allows us
				to re-label things.

	Notes:		This is very much like EditableText: should it be the same?
				The original MSJ version had a read-only EditableText object.

********************************************************************************************/
VisibleLabel::~VisibleLabel()
{
	if (m_pUIElement != NULL)
	{
		m_pUIElement->UpdateText(NullString);
	}
}


/********************************************************************************************

>	const VisibleLabel& VisibleLabel::operator=(const StringBase& NewString)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Presents the given text to the user.

********************************************************************************************/
const VisibleLabel& VisibleLabel::operator=(const StringBase& NewString)
{
	m_String = NewString;

	Notify(&m_Watcher);

	return *this;
}


/********************************************************************************************

>	VisibleLabel::operator const StringBase&() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Pretends this VisibleLabel is a StringBase, so it can be assigned and stuff

********************************************************************************************/
VisibleLabel::operator const StringBase&() const
{
	return m_String;
}


/********************************************************************************************

>	void VisibleLabel::Observer::Notify(Notifier* const pSender)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Embedded object catches Notifications (doesn't do anything en ce moment, so
				don't use Notify

********************************************************************************************/
void VisibleLabel::Observer::Notify(Notifier* const pSender)
{
	// This function is a friend of class Text, a pointer
	// to which is in the owner field; It's called when the
	// String base class changes. Note that we have to turn
	// notify off in whichever object is being updated. Otherwise,
	// we'll just end up back here.

//	ENSURE_KIND(pSender, VisibleLabel);
	VisibleLabel* const pText = (VisibleLabel*)(pSender);
	if (pText != NULL)
	{
		// String base class was changed, transfer the new value
		// to the display.
		StaticTextControl* const pUI = pText->m_pUIElement;
		if (pUI != NULL)
		{
			pUI->NotifyOff();
			pUI->RetrieveText(&m_pTheTextObject->m_String);
			pUI->NotifyOn();
		}
	}
	else // User changed the TextControl, copy new value to
	{	 // the string base class of the associated text object.

		StaticTextControl* pUI = (StaticTextControl*)(pSender);
		ASSERT(pUI != NULL);
		
		pUI->NotifyOff();			// prevent string from notifying us
		pUI->UpdateText(*m_pTheTextObject);	// modify the string
		pUI->NotifyOn();			// re-enable notification
	}
}


/********************************************************************************************

>	BOOL VisibleLabel::Display(DialogOp& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Displays this VisibleLabel slightly bodgily in the given (Template)Dialog,
				but doesn't enable it.

	Notes:		This interface may very well change. At present there's one of these taking
				a DialogOp (inherited from UserInterface) which does nothing.
				There's also an Interact function missing: should it just enable the 
				control?

********************************************************************************************/
BOOL VisibleLabel::Display(DialogOp& Dialog)
{
	PORTNOTETRACE("dialog","VisibleLabel::Display - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(!((&Dialog)->IS_KIND_OF(TemplateDialog)), "Dialog isn't kind of TemplateDialog");

	TemplateDialog& BetterBeThisDialog = (TemplateDialog&)Dialog;

	m_pUIElement = &(BetterBeThisDialog.GetParamHintControl());

	m_pUIElement->UpdateText(m_String);
#endif
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL VisibleLabel::Interact(DialogOp& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Displays this VisibleLabel slightly bodgily in the given (Template)Dialog
				and enables it (otherwise it's grey)

	Notes:		This interface may very well change. At present there's one of these taking
				a DialogOp (inherited from UserInterface) which does nothing.
				There's also an Interact function missing: should it just enable the 
				control?

********************************************************************************************/
BOOL VisibleLabel::Interact(DialogOp& Dialog)
{
	BOOL Ok = Display(Dialog);

	if (Ok && m_pUIElement != NULL)
	{
		m_pUIElement->Enable();
	}

	return Ok;
}


/********************************************************************************************

>	void VisibleLabel::Hide()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Removes this VisibleLabel from the Dialog given in Display() or Interact()

	Notes:		This interface may very well change.
				At present the control is emptied and disabled: should it disappear 
				completely?

********************************************************************************************/
void VisibleLabel::Hide()
{
	if (m_pUIElement != NULL)
	{
		m_pUIElement->UpdateText(NullString);
		m_pUIElement->Disable();
		m_pUIElement = NULL;
	}
}

