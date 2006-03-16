// $Id: gcache.cpp 662 2006-03-14 21:31:49Z alex $
/////////////////////////////////////////////////////////////////////////////////////////////////
//
// GCache.cpp
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

#include "camtypes.h"
#include <afxwin.h>
#include <iostream.h>
#include "gcmaths.h"
#include "gcache.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

GCache::GCache( size_t Size, UINT32 Log2MaxEntries )
{
	HashTableSize = 1<<Log2MaxEntries ;
	HashTableMask = HashTableSize-1 ;
	HashTable	  = NULL ;
	CacheStart	  = NULL ;
	TRY {
		HashTable	= new ( CacheBlock* [HashTableSize] ) ;
		CacheStart	= (CacheBlock*) new BYTE[Size+2*FreeCacheBlockSize] ;
	} CATCH_ALL (e) {
		delete [] HashTable  ; HashTable  = NULL ;
		delete [] CacheStart ; CacheStart = NULL ;
		return ;
	} END_CATCH_ALL ;
	for ( size_t i=0 ; i<HashTableSize ; i++ )
		HashTable[i] = NULL ;
	CacheBlock *FreeBlock ;
	FreeBlock	= (CacheBlock*) ((BYTE*)CacheStart+FreeCacheBlockSize) ;
	CacheEnd	= (CacheBlock*) ((BYTE*)CacheStart+Size-FreeCacheBlockSize) ;
	CacheStart->Prev		= CacheStart ;
							  CacheEnd->Next		= CacheEnd ;
	CacheStart->Next		= CacheEnd->Prev		=
	CacheStart->NextFree 	= CacheEnd->PrevFree	= FreeBlock ;
	CacheStart->PrevFree 	= CacheEnd->NextFree	= NULL ;
	CacheStart->State		= CacheEnd->State		= NULL ;
	FreeBlock->Prev			= FreeBlock->PrevFree	= CacheStart ;
	FreeBlock->Next			= FreeBlock->NextFree	= CacheEnd ;
	FreeBlock->State		= FREE ;
	LeastUsed = MostUsed = NULL ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// Verify should be used after creation of a GCache object to ensure that it was created OK.
//
BOOL GCache::Verify()
{
	return HashTable && CacheStart ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

GCache::~GCache()
{
	delete [] HashTable  ; HashTable  = NULL ;
	delete [] CacheStart ; CacheStart = NULL ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

BOOL GCache::FindPath( UINT32 Handle, INT32* &Points, BYTE* &Types, UINT32 &Length )
{
	INT32 *Object = (INT32*) FindEntry( Handle ) ;
	if ( Object )
	{
		Length = *Object ;
		Points = ++Object ;
		Types = (BYTE*) (Object+2*Length) ;
	}
	return Object!=NULL ;
}

void GCache::AddPath( UINT32 Handle, INT32* Points, BYTE* Types, UINT32 Length )
{
	INT32 *DPtr = (INT32*) AddEntry( Handle,Length*9+4 ) ;
	*DPtr++ = Length ;
	INT32 *SPtr = Points ;
	for ( size_t i=0 ; i<2*Length ; i++ )
		*DPtr++ = *SPtr++ ;
	SPtr = (INT32*) Types ;
	Length = (Length+3)>>2 ;					/* Word length */
	for ( i=0 ; i<Length ; i++ )
		*DPtr++ = *SPtr++ ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

char* GCache::FindString( UINT32 Handle )
{
	return (char*) FindEntry( Handle ) ;
}

void GCache::AddString( UINT32 Handle, char* String )
{
	size_t StringLength = cc_strlenBytes(String)+1 ;	/* Byte length */
	INT32 *SPtr = (INT32*) String ;
	INT32 *DPtr = (INT32*) AddEntry( Handle,StringLength ) ;
	StringLength = (StringLength+3)>>2 ;		/* Word length */
	for ( size_t i=0 ; i<StringLength ; i++ )
		*DPtr++ = *SPtr++ ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void* GCache::FindEntry( UINT32 Handle )
{
	UINT32 HashIndex = Hash( Handle ) & HashTableMask ;
	CacheBlock *Ptr = HashTable[HashIndex] ;
	while ( Ptr )
		if ( Ptr->Handle == Handle )
		{
			if ( Ptr != MostUsed )
			{
				/* Make this the most recently used object */
				if ( Ptr==LeastUsed )
					LeastUsed = Ptr->NextUsed ;
				else
					Ptr->PrevUsed->NextUsed = Ptr->NextUsed ;
				Ptr->NextUsed->PrevUsed = Ptr->PrevUsed ;
				Ptr->NextUsed = NULL ;
				Ptr->PrevUsed = MostUsed ;
				MostUsed->NextUsed = Ptr ;
				MostUsed = Ptr ;
			}
			return Ptr->Object ;
		}
		else
			Ptr = Ptr->Link ;

	return NULL ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void* GCache::AddEntry( UINT32 Handle, size_t ObjectSize )
{
	UINT32 HashIndex = Hash( Handle ) & HashTableMask ;
	CacheBlock *Ptr = HashTable[HashIndex] ;
	while ( Ptr )
		if ( Ptr->Handle == Handle )
			return Ptr->Object ;					/* Entry already exists */
		else
			Ptr = Ptr->Link ;
	CacheBlock *FreeBlock = CacheStart->NextFree ;
	size_t ObjectBlockSize = (ObjectSize+UsedCacheBlockSize+3) & ~3 ;
	while ( ObjectBlockSize > BlockSize(FreeBlock) )
		if ( !(FreeBlock=FreeBlock->NextFree) )
		{
			/* Remove least recently used object from hash table */
			UINT32 HashIndex = Hash( LeastUsed->Handle ) & HashTableMask ;
			CacheBlock *Ptr = HashTable[HashIndex] ;
			if ( Ptr == LeastUsed )
				HashTable[HashIndex] = LeastUsed->Link ;
			else
			{
				while ( Ptr->Link != LeastUsed )
					Ptr = Ptr->Link ;
				Ptr->Link = LeastUsed->Link ;
			}				
			/* Merge least recently used object with adjacent free blocks and try again */
			FreeBlock = LeastUsed ;
			LeastUsed = LeastUsed->NextUsed ;
			LeastUsed->PrevUsed = NULL ;
			FreeBlock->State = FREE ;
			CacheStart->NextFree->PrevFree = FreeBlock ;
			FreeBlock->NextFree = CacheStart->NextFree ;
			FreeBlock->PrevFree = CacheStart ;
			CacheStart->NextFree = FreeBlock ;
			CacheBlock *NextBlock = FreeBlock->Next ;
			CacheBlock *PrevBlock = FreeBlock->Prev ;
			if ( NextBlock->State == FREE )
			{
				NextBlock->PrevFree->NextFree = NextBlock->NextFree ;
				NextBlock->NextFree->PrevFree = NextBlock->PrevFree ;
				FreeBlock->Next = NextBlock->Next ;
				NextBlock->Next->Prev = FreeBlock ;
			}
			if ( PrevBlock->State == FREE )
			{
				FreeBlock->PrevFree->NextFree = FreeBlock->NextFree ;
				FreeBlock->NextFree->PrevFree = FreeBlock->PrevFree ;
				PrevBlock->Next = FreeBlock->Next ;
				FreeBlock->Next->Prev = PrevBlock ;
				FreeBlock = PrevBlock ;
			}
		}
	/* Add object */
	if ( ObjectBlockSize+FreeCacheBlockSize > BlockSize(FreeBlock) )
	{
		FreeBlock->PrevFree->NextFree = FreeBlock->NextFree ;
		FreeBlock->NextFree->PrevFree = FreeBlock->PrevFree ;
	}
	else
	{
		CacheBlock *NewBlock = (CacheBlock*) ((BYTE*)FreeBlock+ObjectBlockSize) ;
		NewBlock->Prev		= FreeBlock ;
		NewBlock->Next		= FreeBlock->Next ;
		NewBlock->State		= FREE ;
		NewBlock->NextFree	= FreeBlock->NextFree ;
		NewBlock->PrevFree	= FreeBlock->PrevFree ;
		NewBlock->NextFree->PrevFree = NewBlock->PrevFree->NextFree = NewBlock ;
		FreeBlock->Next		= NewBlock->Next->Prev = NewBlock ;
	}
	FreeBlock->Link		= HashTable[HashIndex] ;
	FreeBlock->PrevUsed	= MostUsed ;
	FreeBlock->NextUsed	= NULL ;
	FreeBlock->Handle	= Handle ;
	HashTable[HashIndex] = FreeBlock ;
	if ( MostUsed )
		MostUsed->NextUsed = FreeBlock ;
	MostUsed = FreeBlock ;
	if ( !LeastUsed )
		LeastUsed = FreeBlock ;
	return FreeBlock->Object ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

ostream& operator << ( ostream& os, GCache& Cache )
{
	for ( size_t i=0 ; i<Cache.HashTableSize ; i++ )
	{
		os << Cache.HashTable[i] << " " ;
		if ( (i & 7) == 7 )
			os << endl ;
	}
	os << "Start : " << Cache.CacheStart
					 << ", " << Cache.CacheStart->Prev	   << "," << Cache.CacheStart->Next
					 << ", " << Cache.CacheStart->PrevFree << "," << Cache.CacheStart->NextFree << endl ;
	CacheBlock *Ptr = Cache.CacheStart->Next ;
	while ( Ptr != Cache.CacheEnd )
	{
		if ( Ptr->State == FREE )
			cout << "FREE  : " << Ptr << ", " << Ptr->Prev	   << "," << Ptr->Next
									  << ", " << Ptr->PrevFree << "," << Ptr->NextFree
									  << " (" << hex
									  << Cache.BlockSize(Ptr)-FreeCacheBlockSize
									  << dec << ")" << endl ;
		else
			cout << "USED  : " << Ptr << ", " << Ptr->Prev	   << "," << Ptr->Next
									  << ", " << Ptr->PrevUsed << "," << Ptr->NextUsed
									  << ", " << Ptr->Link	   << "," << Ptr->Handle
									  << ", " << (char*) Ptr->Object << " (" << hex
									  << Cache.BlockSize(Ptr)-UsedCacheBlockSize-cc_strlenBytes((char*)Ptr->Object)-1
									  << dec << ")" << endl ;
		Ptr = Ptr->Next ;
	}
	os << "End   : " << Cache.CacheEnd
				     << ", " << Cache.CacheEnd->Prev	 << "," << Cache.CacheEnd->Next
				     << ", " << Cache.CacheEnd->PrevFree << "," << Cache.CacheEnd->NextFree << endl ;
	os << "LeastUsed : " << Cache.LeastUsed << endl ;
	os << " MostUsed : " << Cache. MostUsed << endl ;
	return os ;
}

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
