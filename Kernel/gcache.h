// $Id: gcache.h 662 2006-03-14 21:31:49Z alex $
/////////////////////////////////////////////////////////////////////////////////////////////////
//
// GCache.h
//
/////////////////////////////////////////////////////////////////////////////////////////////////
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

CONST UINT32 FREE = 0xffffffff ;

struct CacheBlock {
	CacheBlock	*Prev ;
	CacheBlock	*Next ;
	union {
		struct {
			CacheBlock	*PrevFree ;
			CacheBlock	*NextFree ;
			UINT32		State ;
		} ;
		struct {
			CacheBlock	*PrevUsed ;
			CacheBlock	*NextUsed ;
			CacheBlock	*Link ;
			UINT32		Handle ;
			INT32		Object[1] ;
		} ;
	} ;
} ;

const size_t FreeCacheBlockSize = 20 ;
const size_t UsedCacheBlockSize = 24 ;

/////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************************************
// Gavin's original defaults
//GCache( size_t Size = 0x40000, UINT32 Log2MaxEntries = 12 ) ;

// The size of the cache is 128k, average character size = 1k, We allow a maximum of 1K entries
//GCache( size_t Size = 0x20000, UINT32 Log2MaxEntries = 10 ) ;


	Size specifies the byte size of the cache. Log2MaxEntries specifes the number of entries in
	the hash table. Note that the cache will still work even if there are more characters than
	the size of the hash table.

BOOL Verify() ;
	This will return TRUE if the cache is in use. Use after construction to ensure that
	construction succeeded.

BOOL FindPath( UINT32 Handle, INT32* &Points, BYTE* &Types, UINT32 &Length ) ;

	Searches for the given handle. If found it returns TRUE and returns the associated path in
	Points, Types and Length, else it returns FALSE.

void AddPath( UINT32 Handle, INT32*  Points, BYTE*  Types, UINT32  Length ) ;

	Stores the given path in the cache using the specified handle.

char* FindString( UINT32 Handle ) ;

	Searches for the given handle. If found it returns the string associated with the handle,
	else it returns FALSE.

void  AddString( UINT32 Handle, char* String ) ;

	Stores the given string in the cache using the specified handle.

************************************************************************************************/

class GCache
{
#ifdef _DEBUG
	friend ostream& operator << ( ostream& os, GCache& Cache ) ;
#endif

public:
	GCache( size_t Size = 0x40000, UINT32 Log2MaxEntries = 12 ) ;
	~GCache() ;
	BOOL Verify() ;

	BOOL FindPath( UINT32 Handle, INT32* &Points, BYTE* &Types, UINT32 &Length ) ;
	void  AddPath( UINT32 Handle, INT32*  Points, BYTE*  Types, UINT32  Length ) ;

	char* FindString( UINT32 Handle ) ;
	void   AddString( UINT32 Handle, char* String ) ;

private:
	size_t		HashTableSize ;
	UINT32		HashTableMask ;
	CacheBlock	**HashTable ;

	CacheBlock	*CacheStart ;
	CacheBlock	*CacheEnd ;

	CacheBlock	*LeastUsed ;
	CacheBlock	*MostUsed ;

	void* FindEntry( UINT32 Handle ) ;
	void*  AddEntry( UINT32 Handle, size_t ObjectSize ) ;

	inline size_t BlockSize( CacheBlock* Block )
	{
		return (size_t)((BYTE*)Block->Next-(BYTE*)Block) ;
	} ;
} ;

/////////////////////////////////////////////////////////////////////////////////////////////////
