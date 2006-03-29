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
//

#ifndef INC_LIST_TEMPLATE
#define INC_LIST_TEMPLATE

// We want better memory tracking
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/**************************************************************************************

>	template <class T>
>	class ListT

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from STL)
	Created:	20/12/96
	Purpose:	Generic List

**************************************************************************************/

//#include "function.h"
//#include <algobase.h>
#include "iterator.h"
#include "defalloc.h"


template <typename T>
class ListT
{
protected:
    typedef Allocator<void>::Pointer VoidPointer;

    struct ListNode;
    friend struct ListNode;

    struct ListNode
	{
		VoidPointer next;
		VoidPointer prev;
		T data;
    };

public:      
    typedef T	ValueType;

    typedef Allocator<T>							ValueAllocatorType;
    typedef typename Allocator<T>::Pointer			Pointer;
    typedef typename Allocator<T>::Reference		Reference;
    typedef typename Allocator<T>::ConstReference	ConstReference;

    typedef Allocator<ListNode>								ListNodeAllocatorType;
    typedef typename Allocator<ListNode>::Pointer			LinkType;
    typedef typename Allocator<ListNode>::SizeType			SizeType;
    typedef typename Allocator<ListNode>::DifferenceType	DifferenceType;

protected:
    static ListNodeAllocatorType	g_ListNodeAllocator;
    static ValueAllocatorType		g_ValueAllocator;

protected:
//    SizeType BufferSize()
//	{
//		return ListNodeAllocator.InitPageSize();
//    }

//    struct list_node_buffer;
//    friend list_node_buffer;
//    struct ListNodeBuffer
//	{
//		void_pointer next_buffer;
//		link_type buffer;
//  };
public:
//    typedef Allocator<list_node_buffer> buffer_allocator_type;
//    typedef Allocator<list_node_buffer>::pointer buffer_pointer;     
protected:
//    static Allocator<list_node_buffer> buffer_allocator;

//    /*static*/ buffer_pointer buffer_list;
//    /*static*/ link_type free_list;
//    /*static*/ link_type next_avail;
//    /*static*/ link_type last;
/*
    void add_new_buffer() {
	buffer_pointer tmp = buffer_allocator.allocate((size_type)1);
	tmp->buffer = list_node_allocator.allocate(buffer_size());
	tmp->next_buffer = buffer_list;
	buffer_list = tmp;
	next_avail = buffer_list->buffer;
	last = next_avail + buffer_size();
    }
*/
    SizeType m_NumberOfLists;
//    void deallocate_buffers();

    LinkType GetNode()
	{
		return new ListNode;
/*	Extended Mix
		LinkType NodeToReturn;
		if (m_FreeList != NULL)
		{
			LinkType tmp = m_FreeList;
			m_FreeList = (LinkType)m_FreeList->next;
			NodeToReturn = tmp;
		}
		else
		{
			if (m_NextAvail == m_Last)
			{
				AddNewBuffer();
			}
			NodeToReturn = m_NextAvail++;
		}
		return NodeToReturn;
	}
*/
//		return free_list ? (free_list = (link_type)(free_list->next), tmp) 
//					: (next_avail == last ? (add_new_buffer(), next_avail++) : next_avail++);
	// ugly code for inlining - avoids multiple returns
    }
    void PutNode(LinkType p)
	{
		delete p;
//		p->next = free_list;
//		free_list = p;
    }

protected:
    LinkType	m_Node;
    SizeType	m_Length;
	BOOL		m_bIsValid;

public:
    class ConstIterator;
    class Iterator : public BidirectionalIterator<T, DifferenceType>
	{
		friend class ListT<T>;
	    friend class ConstIterator;
//  friend bool operator==(const iterator& x, const iterator& y);
    protected:
		LinkType	m_Node;
		Iterator(LinkType x) : m_Node(x) {}

    public:
		Iterator() {}

		BOOL operator==(const Iterator& x) const	{ return m_Node == x.m_Node; }
		Reference operator*() const					{ return (*m_Node).data; }
		Iterator& operator++()
		{ 
			m_Node = (LinkType)((*m_Node).next);
			return *this;
		}

		Iterator operator++(INT32)
		{ 
		    Iterator tmp = *this;
			++*this;
			return tmp;
		}
		Iterator& operator--()
		{ 
			m_Node = (LinkType)((*m_Node).prev);
			return *this;
		}
		Iterator operator--(INT32)
		{ 
			Iterator tmp = *this;
			--*this;
			return tmp;
		}
    };

    class ConstIterator : public BidirectionalIterator<T, DifferenceType>
	{
		friend class ListT<T>;
    protected:
		LinkType	m_Node;
		ConstIterator(LinkType x) : m_Node(x) {}
    public:     
		ConstIterator() {}
		ConstIterator(const Iterator& x) : m_Node(x.m_Node) {}
		BOOL operator==(const ConstIterator& x) const	{ return m_Node == x.m_Node; } 

		ConstReference operator*() const				{ return (*m_Node).data; }


		ConstIterator& operator++()
		{ 
			m_Node = (LinkType)((*m_Node).next);
			return *this;
		}

		ConstIterator operator++(INT32)
		{ 
			ConstIterator tmp = *this;
			++*this;
			return tmp;
		}

		ConstIterator& operator--()
		{ 
			m_Node = (LinkType)((*m_Node).prev);
		    return *this;
		}

		ConstIterator operator--(INT32)
		{ 
			ConstIterator tmp = *this;
			--*this;
			return tmp;
		}
    };
/*
    typedef reverse_bidirectional_iterator<const_iterator, value_type,
                                           const_reference, difference_type>
	const_reverse_iterator;
    typedef reverse_bidirectional_iterator<iterator, value_type, reference,
                                           difference_type>
        reverse_iterator; 
 */
    ListT() : m_NumberOfLists(0), m_Length(0) //,free_list(0), buffer_list(0), next_avail(0), last(0), 
	{ 
		++m_NumberOfLists;
		m_Node = GetNode();
		if (m_Node != NULL)
		{
			(*m_Node).next = m_Node;
			(*m_Node).prev = m_Node;
			m_bIsValid = TRUE;
		}
		else
		{
			m_bIsValid = FALSE;
		}
    }

	BOOL		IsValid() const		{	return m_bIsValid;	}

    Iterator		Begin()			{ return (LinkType)((*m_Node).next); }
	ConstIterator	Begin() const	{ return (LinkType)((*m_Node).next); }
    Iterator		End()			{ return m_Node; }
    ConstIterator	End() const		{ return m_Node; }
/*
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { 
        return const_reverse_iterator(end()); 
    }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { 
        return const_reverse_iterator(begin());
    }
*/
    BOOL Empty() const		{ return m_Length == 0; }
    SizeType Size() const	{ return m_Length; }
/*
    size_type max_size() const { return list_node_allocator.max_size(); }
*/
	Reference Front()				{ return *(Begin()); }		// ++?
	ConstReference Front() const	{ return *(Begin()); }		// ++?
    Reference Back()				{ return *(--End()); }
    ConstReference Back() const		{ return *(--End()); }
/*
    void swap(list<T>& x) {
	std::swap(node, x.node);
	std::swap(length, x.length);
    }
*/
    Iterator Insert(Iterator Position, const T& X)
	{
		LinkType tmp = GetNode();
		if (tmp != NULL)
		{
			Construct(g_ValueAllocator.Address((*tmp).data), X);
			(*tmp).next = Position.m_Node;
			(*tmp).prev = (*Position.m_Node).prev;
			(*(LinkType((*Position.m_Node).prev))).next = tmp;
			(*Position.m_Node).prev = tmp;
			++m_Length;
		}
		return tmp;
    }
/*
    void insert(iterator position, const T* first, const T* last);
    void insert(iterator position, const_iterator first,
		const_iterator last);
    void insert(iterator position, size_type n, const T& x);
	void push_front(const T& x) { insert(begin(), x); }
*/
    BOOL PushBack(const T& X)
	{
		return (Insert(End(), X).m_Node != NULL);
	}


    void Erase(Iterator Position)
	{
		if (m_Length != 0)
		{
			(*(LinkType((*Position.m_Node).prev))).next = (*Position.m_Node).next;
			(*(LinkType((*Position.m_Node).next))).prev = (*Position.m_Node).prev;
			Destroy(g_ValueAllocator.Address((*Position.m_Node).data));
			PutNode(Position.m_Node);
			--m_Length;
		}
		else
		{
			ERROR3("Erase from Empty ListT");
		}
    }
	void Erase(Iterator First, Iterator Last);
//	void pop_front() { erase(begin()); }
    void PopBack()
	{ 
		Iterator tmp = End();
		Erase(--tmp);
    }
/*
    List(size_type n, const T& value = T()) : length(0), free_list(0), buffer_list(0), next_avail(0), last(0), number_of_lists(0) { 
	++number_of_lists;
	node = get_node();
	(*node).next = node;
	(*node).prev = node;
	insert(begin(), n, value);
    }
    list(const T* first, const T* last) : length(0), free_list(0), buffer_list(0), next_avail(0), last(0), number_of_lists(0) {
	++number_of_lists;
	node = get_node();
	(*node).next = node;
	(*node).prev = node;
	insert(begin(), first, last);
    }
    list(const list<T>& x) : length(0), free_list(0), buffer_list(0), next_avail(0), last(0), number_of_lists(0) {
	++number_of_lists;
	node = get_node();
	(*node).next = node;
	(*node).prev = node;
	insert(begin(), x.begin(), x.end());
    }
*/
    ~ListT()
	{
		Erase(Begin(), End());
		PutNode(m_Node);
//		if (--m_NumberOfLists == 0)
//		{
//			deallocate_buffers();
//		}
    }

//    list<T>& operator=(const list<T>& x);
/*
protected:
    void transfer(iterator position, iterator first, iterator last) {
	(*(link_type((*last.node).prev))).next = position.node;
	(*(link_type((*first.node).prev))).next = last.node;
	(*(link_type((*position.node).prev))).next = first.node;  
	link_type tmp = link_type((*position.node).prev);
	(*position.node).prev = (*last.node).prev;
	(*last.node).prev = (*first.node).prev; 
	(*first.node).prev = tmp;
    }
public:
    void splice(iterator position, list<T>& x) {
	if (!x.empty()) {
	    transfer(position, x.begin(), x.end());
	    length += x.length;
	    x.length = 0;
	}
    }
    void splice(iterator position, list<T>& x, iterator i) {
	iterator j = i;
	if (position == i || position == ++j) return;
	transfer(position, i, j);
	++length;
	--x.length;
    }
    void splice(iterator position, list<T>& x, iterator first, iterator last) {
	if (first != last) {
	    if (&x != this) {
		difference_type n = 0;
	    	distance(first, last, n);
	    	x.length -= n;
	    	length += n;
	    }
	    transfer(position, first, last);
	}
    }
    void remove(const T& value);
    void unique();
    void merge(list<T>& x);
    void reverse();
    void sort();
*/
};

/*
template <class T>
list<T>::buffer_pointer list<T>::buffer_list = 0;

template <class T>
list<T>::link_type list<T>::free_list = 0;

template <class T>
list<T>::link_type list<T>::next_avail = 0;

template <class T>
list<T>::link_type list<T>::last = 0;

template <class T>
list<T>::size_type list<T>::number_of_lists = 0;
*/

template< typename T >
typename ListT<T>::ListNodeAllocatorType	ListT<T>::g_ListNodeAllocator;

template< typename T >
typename ListT<T>::ValueAllocatorType		ListT<T>::g_ValueAllocator;

//template <class T>
//list<T>::buffer_allocator_type list<T>::buffer_allocator;

/* 
 * currently the following does not work - made into a member function

template <class T>
inline bool operator==(const list<T>::iterator& x, const list<T>::iterator& y) { 
    return x.node == y.node; 
}
*/
/*
template <class T>
inline bool operator==(const list<T>& x, const list<T>& y) {
    return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T>
inline bool operator<(const list<T>& x, const list<T>& y) {
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}
template <class T>
void list<T>::deallocate_buffers() {
    while (buffer_list) {
	buffer_pointer tmp = buffer_list;
	buffer_list = (buffer_pointer)(buffer_list->next_buffer);
	list_node_allocator.deallocate(tmp->buffer);
	buffer_allocator.deallocate(tmp);
    }
    free_list = 0;
    next_avail = 0;
    last = 0;
}

template <class T>
void list<T>::insert(iterator position, const T* first, const T* last) {
    while (first != last) insert(position, *first++);
}
	 
template <class T>
void list<T>::insert(iterator position, const_iterator first,
		     const_iterator last) {
    while (first != last) insert(position, *first++);
}

template <class T>
void list<T>::insert(iterator position, size_type n, const T& x) {
    while (n--) insert(position, x);
}
*/
template <class T>
void ListT<T>::Erase(Iterator First, Iterator Last)
{
    while (!(First == Last))
	{
		Erase(First++);
	}
}

/*
template <class T>
list<T>& list<T>::operator=(const list<T>& x) {
    if (this != &x) {
	iterator first1 = begin();
	iterator last1 = end();
	const_iterator first2 = x.begin();
	const_iterator last2 = x.end();
	while (first1 != last1 && first2 != last2) *first1++ = *first2++;
	if (first2 == last2)
	    erase(first1, last1);
	else
	    insert(last1, first2, last2);
    }
    return *this;
}

template <class T>
void list<T>::remove(const T& value) {
    iterator first = begin();
    iterator last = end();
    while (first != last) {
	iterator next = first;
	++next;
	if (*first == value) erase(first);
	first = next;
    }
}

template <class T>
void list<T>::unique() {
    iterator first = begin();
    iterator last = end();
    if (first == last) return;
    iterator next = first;
    while (++next != last) {
	if (*first == *next)
	    erase(next);
	else
	    first = next;
	next = first;
    }
}

template <class T>
void list<T>::merge(list<T>& x) {
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = x.end();
    while (first1 != last1 && first2 != last2)
	if (*first2 < *first1) {
	    iterator next = first2;
	    transfer(first1, first2, ++next);
	    first2 = next;
	} else
	    ++first1;
    if (first2 != last2) transfer(last1, first2, last2);
    length += x.length;
    x.length= 0;
}

template <class T>
void list<T>::reverse() {
    if (size() < 2) return;
    for (iterator first = ++begin(); first != end();) {
	iterator old = first++;
	transfer(begin(), old, first);
    }
}    

template <class T>
void list<T>::sort() {
    if (size() < 2) return;
    list<T> carry;
    list<T> counter[64];
    INT32 fill = 0;
    while (!empty()) {
	carry.splice(carry.begin(), *this, begin());
	INT32 i = 0;
	while(i < fill && !counter[i].empty()) {
	    counter[i].merge(carry);
	    carry.swap(counter[i++]);
	}
	carry.swap(counter[i]);         
	if (i == fill) ++fill;
    } 
    while(fill--) merge(counter[fill]);
}

#undef Allocator
#undef list
*/

// undefine "new".otherwise it messes up the CC_IMPLEMENT_DYNCREATE
// and CC_IMPLEMENT_DYNAMIC macros!!
#undef new
#endif
