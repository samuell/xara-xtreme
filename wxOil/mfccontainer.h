// $Id: mfccontainer.h 662 2006-03-14 21:31:49Z alex $
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

// Classes which emulate MFC containers using STL

#ifndef INC_MFC_CONTAINER
#define INC_MFC_CONTAINER

template< class KEY, class ARG_KEY, class VALUE, class ARG_VALUE, typename BaseClass = std::map<KEY, VALUE> >
class CMap
{
protected:
	BaseClass			m_TheMap;

public:
	typedef typename BaseClass::iterator		iterator;
	typedef typename BaseClass::const_iterator	const_iterator;

	CMap()
	{
	}

	CMap( INT32 )
	{
	}

// Attributes
	// number of elements
	PINT32 GetCount() const	{ return m_TheMap.size(); }
	PINT32 GetSize() const		{ return m_TheMap.size(); }
	BOOL IsEmpty() const		{ return m_TheMap.empty(); }

// Lookup
	BOOL Lookup( ARG_KEY key, VALUE& rValue ) const
	{
		const_iterator	iter = m_TheMap.find( key );
		if( m_TheMap.end() == iter )
			return FALSE;

		rValue = iter->second;
		return TRUE;
	}

// Operations
	// Lookup and add if not there
	VALUE &operator[]( ARG_KEY key )
	{
		return m_TheMap[key];
	}

	// add a new (key, value) pair
	void SetAt(ARG_KEY key, ARG_VALUE newValue)
	{
		m_TheMap[key] = newValue;
	}

	// removing existing (key, ?) pair
	BOOL RemoveKey( ARG_KEY key )
	{
		
		const_iterator	iter = m_TheMap.find( key );
		if( m_TheMap.end() == iter )
			return FALSE;
		m_TheMap.erase( key );
		return TRUE;
	}
	void RemoveAll()
	{
		m_TheMap.clear();
	}

	// iterating all (key, value) pairs
	const_iterator GetStartPosition() const
	{
		return m_TheMap.begin();
	}

	void GetNextAssoc( const_iterator &rNextPosition, KEY &rKey, VALUE &rValue ) const
	{
		if( m_TheMap.end() == rNextPosition || NULL == rNextPosition )
		{
			rNextPosition = NULL;
			return;
		}

		rKey	= rNextPosition->first;
		rValue	= rNextPosition->second;
		++rNextPosition;
	}
};

#endif//INC_MFC_CONTAINER
