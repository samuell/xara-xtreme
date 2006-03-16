// $Id: defalloc.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef DEFALLOC_H
#define DEFALLOC_H

#include <limits.h>
#include "fixmem.h"

#include "errors.h"


/*
 * the following template function is replaced by the following two functions
 * due to the fact that the Borland compiler doesn't change prediff_t type
 * to type INT32 when compile with -ml or -mh.


template <class T>
inline T* allocate(ptrdiff_t size, T*) {
    set_new_handler(0);
    T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
    if (tmp == 0) {
	cerr << "out of memory" << endl; 
	exit(1);
    }
    return tmp;
}
*/
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

template <class T>
inline T* Allocate(INT32 size, T*)
{
    T* tmp = (T*)CCMalloc((UINT32)(size * sizeof(T)));
	ERROR2IF(tmp == 0, NULL, "Out of Memory");
    return tmp;
}

/*template <class T>
inline T* Allocate(INT32 size, T*)
{
    T* tmp = (T*)(::operator new((UINT32)(size * sizeof(T))));
	ERROR2IF(tmp == 0, NULL, "Out of Memory");
    return tmp;
} */

template <class T>
inline void Deallocate(T* buffer)
{
    ::operator delete(buffer);
}

template <class T>
inline void Destroy(T* pointer)
{
    pointer->~T();
}

inline void Destroy(char*) {}
inline void Destroy(unsigned char*) {}
inline void Destroy(short*) {}
inline void Destroy(unsigned short*) {}
inline void Destroy(INT32*) {}
inline void Destroy(UINT32*) {}
inline void Destroy(float*) {}
inline void Destroy(double*) {}
inline void Destroy(char**) {}
inline void Destroy(unsigned char**) {}
inline void Destroy(short**) {}
inline void Destroy(unsigned short**) {}
inline void Destroy(INT32**) {}
inline void Destroy(UINT32**) {}
inline void Destroy(float**) {}
inline void Destroy(double**) {}

inline void Destroy(char*, char*) {}
inline void Destroy(unsigned char*, unsigned char*) {}
inline void Destroy(short*, short*) {}
inline void Destroy(unsigned short*, unsigned short*) {}
inline void Destroy(INT32*, INT32*) {}
inline void Destroy(UINT32*, UINT32*) {}
inline void Destroy(float*, float*) {}
inline void Destroy(double*, double*) {}
inline void Destroy(char**, char**) {}
inline void Destroy(unsigned char**, unsigned char**) {}
inline void Destroy(short**, short**) {}
inline void Destroy(unsigned short**, unsigned short**) {}
inline void Destroy(INT32**, INT32**) {}
inline void Destroy(UINT32**, UINT32**) {}
inline void Destroy(float**, float**) {}
inline void Destroy(double**, double**) {}

template <class T1, class T2>
inline void Construct(T1* p, const T2& value)
{
    *p = T1(value);
}

template <class T>
class Allocator
{
public:
    typedef T			ValueType;

    typedef T*			Pointer;
    typedef const T*	ConstPointer;

    typedef T&			Reference;
    typedef const T&	ConstReference;
    
    typedef INT32		SizeType;
    typedef ptrdiff_t	DifferenceType;

    Pointer Allocate(SizeType n)
	{ 
		return ::Allocate((DifferenceType)n, (Pointer)0);
    }

    void Deallocate(Pointer p)
	{
		::Deallocate(p);
	}

    Pointer Address(Reference x)
	{
		return (Pointer)&x;
	}

    ConstPointer ConstAddress(ConstReference x)
	{ 
		return (ConstPointer)&x; 
    }

    SizeType InitPageSize()
	{ 
		return max(SizeType(1), SizeType(4096/sizeof(T))); 
    }

    SizeType MaxSize() const
	{ 
		return max(SizeType(1), SizeType(UINT_MAX/sizeof(T))); 
    }
};

template <>
class Allocator<void>
{
public:
    typedef void* Pointer;
};





#endif		// DEFALLOC_H
