// $Id: uielem.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_UIELEM
#define INC_UIELEM


/**************************************************************************************

>	class UserInterface : public CC_CLASS_MEMDUMP

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Allows a user to edit a list of items

**************************************************************************************/
class UserInterface : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(UserInterface);
public:
	virtual ~UserInterface() { };
	virtual BOOL Display(DialogOp& Dialog) = 0;
	virtual BOOL Interact(DialogOp& Dialog) = 0;
	virtual void Hide() = 0;
};


class ListControl;
class TextControl;
/**************************************************************************************

>	class VisibleListItem : public UserInterface

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Allows a user to edit a list of items

**************************************************************************************/
class VisibleListItem : public UserInterface
{
	CC_DECLARE_MEMDUMP(VisibleListItem);

public:
	virtual ~VisibleListItem()	{}

// Overridden implementation does nothing
	virtual BOOL Display(DialogOp& Dialog)		{	return TRUE;	}
	virtual BOOL Interact(DialogOp& Dialog)		{	return TRUE;	}
	virtual void Hide()							{}

	virtual StringBase& GetText(StringBase& Description) const = 0;
	virtual BOOL operator > (const VisibleListItem& OtherItem) const;
};

inline BOOL VisibleListItem::operator > (const VisibleListItem& OtherItem) const
{
	String_64 ThisItemText;
	String_64 OtherItemText;

	return GetText(ThisItemText) > OtherItem.GetText(OtherItemText);
}



class VisibleListItemPtr;
/**************************************************************************************

>	class VisibleListIterator

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Returns entries in a visible list

	See Also:	VisibleList::Begin(), End()

**************************************************************************************/
class VisibleListIterator
{
public:
	VisibleListIterator& operator ++();
	VisibleListIterator& operator --();
	VisibleListItem& operator*();
	BOOL operator!=(const VisibleListIterator& Other);


	VisibleListIterator(List& Container, VisibleListItemPtr* const pPointer, 
						INT32* IndexesOfItemsToReturn = NULL);
	~VisibleListIterator();

	VisibleListIterator(const VisibleListIterator& Other);
	const VisibleListIterator& operator=(const VisibleListIterator& Other);
private:
	VisibleListItemPtr*	m_VisibleListItemPtr;
	List&				m_Container;
	INT32*				m_IndexesOfItemsToReturn;
	UINT32				m_IndexInIndex;
};

/**************************************************************************************

>	class VisibleList : public UserInterface

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Allows a user to edit a list of items

**************************************************************************************/
class VisibleList : public UserInterface, public Notifier
{
	CC_DECLARE_MEMDUMP(VisibleList);

public:
	VisibleList() : m_pUIElement(NULL)	{}
	virtual ~VisibleList();

// Overridden implementations...do nothing for now (use non-virtuals)
	virtual BOOL Display(DialogOp& Dialog);
	virtual BOOL Interact(DialogOp& Dialog);
	virtual void Hide();

	virtual BOOL Display(ListControl& Control);
	virtual BOOL Interact(ListControl& Control);

// Visible List specific
	virtual BOOL InsertEntry(VisibleListItem& EntryToInsert);
	virtual BOOL DeleteSelectedEntries();
	virtual BOOL DeleteAllEntries();

	virtual UINT32 GetNumberOfEntries() const;
	virtual UINT32 GetNumberOfSelectedEntries() const;
	virtual VisibleListItem* GetFirstSelectedEntry() const;
	virtual void RemoveSelection();

	virtual VisibleListIterator Begin();
	virtual const VisibleListIterator& End() const;
	virtual VisibleListIterator BeginOfEnd();
	
	virtual VisibleListIterator SelectionBegin();

protected:
// Implementation
	virtual VisibleListItem& GetEntryAt(UINT32 Index);
	virtual BOOL DeleteEntryAtIndex(UINT32 Index);

	List&			GetContainer();

private:
	ListControl*	m_pUIElement;
	List			m_Container;
};


/**************************************************************************************

>	class EditableText : public UserInterface

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Allows a user to edit a list of items

**************************************************************************************/
class EditableText : public UserInterface, public Notifier
{
public: // overrides of User_interface functions
	EditableText(const StringBase& TextString = NullString);

	virtual ~EditableText();
	EditableText(const EditableText& Other);
	virtual	const EditableText& operator=(const EditableText& Other);

	virtual	const EditableText& operator=(const StringBase& NewText );
	operator const StringBase&();

// Overridden implementation does nothing...slightly bodgy this
	virtual BOOL Display(DialogOp& Dialog)		{	return TRUE;	}
	virtual BOOL Interact(DialogOp& Dialog)		{	return TRUE;	}
	virtual BOOL Display(TextControl& Control);
	virtual void Hide();

private:
	// Implementation
	class Observer : public Notifiable
	{
		EditableText* m_pTheTextObject;
	public:
		Observer( EditableText* const pContainer ): m_pTheTextObject(pContainer){}
		void Notify( Notifier* const pSender );
	}
	m_Watcher;

	friend void Observer::Notify( Notifier* const pSender );

	TextControl*	m_pUIElement;
	String_256		m_String;


};



/**************************************************************************************

>	class VisibleLabel : public UserInterface, public Notifier

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Allows a user to edit a list of items

**************************************************************************************/
class VisibleLabel : public UserInterface, public Notifier
{
public: // overrides of User_interface functions
	VisibleLabel(const StringBase& TextString = NullString);

	virtual ~VisibleLabel();

	virtual	const VisibleLabel& operator=(const StringBase& NewText );
	operator const StringBase&() const;

// Overridden implementation does nothing...slightly bodgy this
	virtual BOOL Display(DialogOp& Dialog);
	virtual BOOL Interact(DialogOp& Dialog);
	virtual void Hide();

private:
	// Implementation
	class Observer : public Notifiable
	{
		VisibleLabel* m_pTheTextObject;
	public:
		Observer( VisibleLabel* const pContainer ): m_pTheTextObject(pContainer){}
		void Notify( Notifier* const pSender );
	}
	m_Watcher;

	friend void Observer::Notify( Notifier* const pSender );

	StaticTextControl*	m_pUIElement;
	String_256			m_String;
};


#endif	// INC_UIELEM
