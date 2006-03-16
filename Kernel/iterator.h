// $Id: iterator.h 662 2006-03-14 21:31:49Z alex $
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
// Some template classes for the STL iterators
// (actually there's only the bidrectional one because that's all I want)

#ifndef INC_ITERATOR
#define INC_ITERATOR

// We want better memory tracking
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

//#include "function.h"

//struct input_iterator_tag {};
//struct output_iterator_tag {};
//struct forward_iterator_tag {};
struct BidirectionalIteratorTag {};
//struct random_access_iterator_tag {};


//template <class T, class Distance> struct input_iterator {};
//struct output_iterator {};
//template <class T, class Distance> struct forward_iterator {};
template <class T, class Distance> struct BidirectionalIterator {};
//template <class T, class Distance> struct random_access_iterator {};

/*
template <class T, class Distance> 
inline input_iterator_tag 
iterator_category(const input_iterator<T, Distance>&) {
    return input_iterator_tag();
}

inline output_iterator_tag iterator_category(const output_iterator&) {
    return output_iterator_tag();
}

template <class T, class Distance> 
inline forward_iterator_tag
iterator_category(const forward_iterator<T, Distance>&) {
    return forward_iterator_tag();
}
*/
template <class T, class Distance> 
inline BidirectionalIteratorTag IteratorCategory(const BidirectionalIterator<T, Distance>&)
{
    return BidirectionalIteratorTag();
}
/*
template <class T, class Distance> 
inline random_access_iterator_tag
iterator_category(const random_access_iterator<T, Distance>&) {
    return random_access_iterator_tag();
}

template <class T>
inline random_access_iterator_tag iterator_category(const T*) {
    return random_access_iterator_tag();
}

template <class T, class Distance> 
inline T* value_type(const input_iterator<T, Distance>&) {
    return (T*)(0); 
}

template <class T, class Distance> 
inline T* value_type(const forward_iterator<T, Distance>&) {
    return (T*)(0);
}

template <class T, class Distance> 
inline T* value_type(const bidirectional_iterator<T, Distance>&) {
    return (T*)(0);
}

template <class T, class Distance> 
inline T* value_type(const random_access_iterator<T, Distance>&) {
    return (T*)(0);
}

template <class T>
inline T* value_type(const T*) { return (T*)(0); }

template <class T, class Distance> 
inline Distance* distance_type(const input_iterator<T, Distance>&) {
    return (Distance*)(0);
}

template <class T, class Distance> 
inline Distance* distance_type(const forward_iterator<T, Distance>&) {
    return (Distance*)(0);
}

template <class T, class Distance> 
inline Distance* 
distance_type(const bidirectional_iterator<T, Distance>&) {
    return (Distance*)(0);
}

template <class T, class Distance> 
inline Distance* 
distance_type(const random_access_iterator<T, Distance>&) {
    return (Distance*)(0);
}

template <class T>
inline ptrdiff_t* distance_type(const T*) { return (ptrdiff_t*)(0); }

template <class Container>
class back_insert_iterator : public output_iterator {
protected:
    Container& container;
public:
    back_insert_iterator(Container& x) : container(x) {}
    back_insert_iterator<Container>&
    operator=(const Container::value_type& value) { 
	container.push_back(value);
	return *this;
    }
    back_insert_iterator<Container>& operator*() { return *this; }
    back_insert_iterator<Container>& operator++() { return *this; }
    back_insert_iterator<Container>& operator++(INT32) { return *this; }
};

template <class Container>
back_insert_iterator<Container> back_inserter(Container& x) {
    return back_insert_iterator<Container>(x);
}

template <class Container>
class front_insert_iterator : public output_iterator {
protected:
    Container& container;
public:
    front_insert_iterator(Container& x) : container(x) {}
    front_insert_iterator<Container>&
    operator=(const Container::value_type& value) { 
	container.push_front(value);
	return *this;
    }
    front_insert_iterator<Container>& operator*() { return *this; }
    front_insert_iterator<Container>& operator++() { return *this; }
    front_insert_iterator<Container>& operator++(INT32) { return *this; }
};

template <class Container>
front_insert_iterator<Container> front_inserter(Container& x) {
    return front_insert_iterator<Container>(x);
}

template <class Container>
class insert_iterator : public output_iterator {
protected:
    Container& container;
    Container::iterator iter;
public:
    insert_iterator(Container& x, Container::iterator i) 
	: container(x), iter(i) {}
    insert_iterator<Container>&
    operator=(const Container::value_type& value) { 
	iter = container.insert(iter, value);
	++iter;
	return *this;
    }
    insert_iterator<Container>& operator*() { return *this; }
    insert_iterator<Container>& operator++() { return *this; }
    insert_iterator<Container>& operator++(INT32) { return *this; }
};

template <class Container, class Iterator>
insert_iterator<Container> inserter(Container& x, Iterator i) {
    return insert_iterator<Container>(x, Container::iterator(i));
}

template <class BidirectionalIterator, class T, class Reference, 
          class Distance> 
// Reference = T& 
// Distance = ptrdiff_t
class reverse_bidirectional_iterator 
    : public bidirectional_iterator<T, Distance> {
    typedef reverse_bidirectional_iterator<BidirectionalIterator, T, Reference,
                                           Distance> self;
    friend bool operator==(const self& x, const self& y);
protected:
    BidirectionalIterator current;
public:
    reverse_bidirectional_iterator() {}
    reverse_bidirectional_iterator(BidirectionalIterator x) : current(x) {}
    BidirectionalIterator base() { return current; }
    Reference operator*() const {
	BidirectionalIterator tmp = current;
	return *--tmp;
    }
    self& operator++() {
	--current;
	return *this;
    }
    self operator++(INT32) {
	self tmp = *this;
	--current;
	return tmp;
    }
    self& operator--() {
	++current;
	return *this;
    }
    self operator--(INT32) {
	self tmp = *this;
	++current;
	return tmp;
    }
};

template <class BidirectionalIterator, class T, class Reference,
          class Distance>
inline bool operator==(
    const reverse_bidirectional_iterator<BidirectionalIterator, T, Reference,
		                         Distance>& x, 
    const reverse_bidirectional_iterator<BidirectionalIterator, T, Reference,
		                         Distance>& y) {
    return x.current == y.current;
}

template <class RandomAccessIterator, class T, class Reference,
          class Distance> 
// Reference = T&
// Distance = ptrdiff_t
class reverse_iterator : public random_access_iterator<T, Distance> {
    typedef reverse_iterator<RandomAccessIterator, T, Reference, Distance>
	self;
    friend bool operator==(const self& x, const self& y);
    friend bool operator<(const self& x, const self& y);
    friend Distance operator-(const self& x, const self& y);
    friend self operator+(Distance n, const self& x);
protected:
    RandomAccessIterator current;
public:
    reverse_iterator() {}
    reverse_iterator(RandomAccessIterator x) : current(x) {}
    RandomAccessIterator base() { return current; }
    Reference operator*() const { return *(current - 1); }
    self& operator++() {
	--current;
	return *this;
    }
    self operator++(INT32) {
	self tmp = *this;
	--current;
	return tmp;
    }
    self& operator--() {
	++current;
	return *this;
    }
    self operator--(INT32) {
	self tmp = *this;
	++current;
	return tmp;
    }
    self operator+(Distance n) const {
	return self(current - n);
    }
    self& operator+=(Distance n) {
	current -= n;
	return *this;
    }
    self operator-(Distance n) const {
	return self(current + n);
    }
    self& operator-=(Distance n) {
	current += n;
	return *this;
    }
    Reference operator[](Distance n) { return *(*this + n); }
};

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline bool operator==(const reverse_iterator<RandomAccessIterator, T,
		                              Reference, Distance>& x, 
		       const reverse_iterator<RandomAccessIterator, T,
		                              Reference, Distance>& y) {
    return x.current == y.current;
}

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline bool operator<(const reverse_iterator<RandomAccessIterator, T,
		                             Reference, Distance>& x, 
		      const reverse_iterator<RandomAccessIterator, T,
		                             Reference, Distance>& y) {
    return y.current < x.current;
}

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline Distance operator-(const reverse_iterator<RandomAccessIterator, T,
			                         Reference, Distance>& x, 
			  const reverse_iterator<RandomAccessIterator, T,
			                         Reference, Distance>& y) {
    return y.current - x.current;
}

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline reverse_iterator<RandomAccessIterator, T, Reference, Distance> 
operator+(Distance n,
	  const reverse_iterator<RandomAccessIterator, T, Reference,
	                         Distance>& x) {
    return reverse_iterator<RandomAccessIterator, T, Reference, Distance>
	(x.current - n);
}


template <class OutputIterator, class T>
class raw_storage_iterator : public output_iterator {
protected:
    OutputIterator iter;
public:
    raw_storage_iterator(OutputIterator x) : iter(x) {}
    raw_storage_iterator<OutputIterator, T>& operator*() { return *this; }
    raw_storage_iterator<OutputIterator, T>& operator=(const T& element) {
	construct(iter, element);
	return *this;
    }        
    raw_storage_iterator<OutputIterator, T>& operator++() {
	++iter;
	return *this;
    }
    raw_storage_iterator<OutputIterator, T> operator++(INT32) {
	raw_storage_iterator<OutputIterator, T> tmp = *this;
	++iter;
	return tmp;
    }
};


template <class T, class Distance> // Distance == ptrdiff_t
class istream_iterator : public input_iterator<T, Distance> {
friend bool operator==(const istream_iterator<T, Distance>& x,
		       const istream_iterator<T, Distance>& y);
protected:
    istream* stream;
    T value;
    bool end_marker;
    void read() {
	end_marker = (*stream) ? true : false;
	if (end_marker) *stream >> value;
	end_marker = (*stream) ? true : false;
    }
public:
    istream_iterator() : stream(&cin), end_marker(false) {}
    istream_iterator(istream& s) : stream(&s) { read(); }
    const T& operator*() const { return value; }
    istream_iterator<T, Distance>& operator++() { 
	read(); 
	return *this;
    }
    istream_iterator<T, Distance> operator++(INT32)  {
	istream_iterator<T, Distance> tmp = *this;
	read();
	return tmp;
    }
};

template <class T, class Distance>
bool operator==(const istream_iterator<T, Distance>& x,
		const istream_iterator<T, Distance>& y) {
    return x.stream == y.stream && x.end_marker == y.end_marker ||
	x.end_marker == false && y.end_marker == false;
}

template <class T>
class ostream_iterator : public output_iterator {
protected:
    ostream* stream;
    char* string;
public:
    ostream_iterator(ostream& s) : stream(&s), string(0) {}
    ostream_iterator(ostream& s, char* c) : stream(&s), string(c)  {}
    ostream_iterator<T>& operator=(const T& value) { 
	*stream << value;
	if (string) *stream << string;
	return *this;
    }
    ostream_iterator<T>& operator*() { return *this; }
    ostream_iterator<T>& operator++() { return *this; } 
    ostream_iterator<T>& operator++(INT32) { return *this; } 
};
*/

#endif	// INC_ITERATOR
