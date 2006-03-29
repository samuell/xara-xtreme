// $Id$
// BitmapCache.cpp: implementation of the CBitmapCache class.
//
//////////////////////////////////////////////////////////////////////
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

#include "bitmapcache.h"

/*********************************************************************************************

	Preference:	Percentage of RAM devoted to cache
	Section:	Cacheing
	Range:		0 - 100 (although greater values are acceptable)
	Purpose:	How much RAM to devote to cacheing bitmaps
	SeeAlso:	-

**********************************************************************************************/ 
INT32 CBitmapCache::CacheRAMPercent = 25;		// 25% of free RAM or 25% of half total RAM


UINT32 AFXAPI HashKey(const CBitmapCacheKey& key)
{
	return key.Hash();
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitmapCache::CBitmapCache()
{
	m_lMaxDataSize = 0;
	m_lCurrentDataSize = 0;
	m_lMaxHashTableLoad = 0;
}




/********************************************************************************************

>	UINT32 CBitmapCache::Initialise()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CBitmapCache::Initialise()
{
	// Declare the section that the magnetic options will live in
	BOOL Worked = Camelot.DeclareSection(TEXT("Cache"), 2);

	// declare the 2 prefs that there are
	if (Worked) Camelot.DeclarePref(TEXT("Cache"), TEXT("CacheRAMPercent"), &CacheRAMPercent, 0, 200);

	// Set up the default maximum cache size
	m_lMaxDataSize = CalcRecommendedMaximumDataSize();
TRACEUSER( "Phil", _T("Recommended Cache Size = %I64d\n"), m_lMaxDataSize);

	// Make the hash table big enough for however many average 80*80*32BPP pixel bitmaps
	// will fit into the configured maximum cache size. Make the hash table twice that size
	// to keep it sparse and thus running more efficiently and ensure that it's prime.
	m_lMaxHashTableLoad = (UINT32) m_lMaxDataSize/(80*80*4);
//	InitHashTable(PrimeAbove(m_lMaxHashTableLoad*2), TRUE);
}




/********************************************************************************************

>	void CBitmapCache::DeInitialise()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CBitmapCache::DeInitialise()
{
	// Get rid of all cached bitmaps...
/*	POSITION pos = GetStartPosition();
	while (pos)
	{
		CBitmapCacheKey key;
		CCachedBitmap bitmap;
		GetNextAssoc(pos, key, bitmap);

		RemoveKey(key);
		m_lCurrentDataSize -= bitmap.GetBitmapSize();
		FreeDIB(bitmap.pbmpInfo, bitmap.pbmpBits);
	}
*/	CCacheKeyMap::iterator pos;
	for ( pos=m_map.begin() ; pos!=m_map.end() ; pos++ )
	{
		CBitmapCacheKey key = pos->first;
		CCachedBitmap bitmap = pos->second;
//		m_map.erase(key);
		m_lCurrentDataSize -= bitmap.GetBitmapSize();
		FreeDIB(bitmap.pbmpInfo, bitmap.pbmpBits);
	}
	m_map.clear();
}




/********************************************************************************************

>	void CBitmapCache::RemoveLowPriorityBitmaps(const INT32 maxpriority = CACHEPRIORITY_TEMPBITMAP_HIGH)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/09/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CBitmapCache::RemoveLowPriorityBitmaps(const INT32 maxpriority)
{
	// Get rid of all cached bitmaps...
/*	POSITION pos = GetStartPosition();
	while (pos)
	{
		CBitmapCacheKey key;
		CCachedBitmap bitmap;
		GetNextAssoc(pos, key, bitmap);

		if (bitmap.nPriority <= maxpriority)
		{
			RemoveKey(key);
			m_lCurrentDataSize -= bitmap.GetBitmapSize();
			FreeDIB(bitmap.pbmpInfo, bitmap.pbmpBits);
		}
	}
*/	CCacheKeyMap::iterator pos;
	for ( pos=m_map.begin() ; pos!=m_map.end() ; )
	{
		CBitmapCacheKey key = pos->first;
		CCachedBitmap bitmap = pos->second;
		pos++;
		if (bitmap.nPriority <= maxpriority)
		{
			m_map.erase(key);
			m_lCurrentDataSize -= bitmap.GetBitmapSize();
			FreeDIB(bitmap.pbmpInfo, bitmap.pbmpBits);
		}
	}
}




/********************************************************************************************

>	UINT32 CBitmapCache::StoreBitmap()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/2004
	Inputs:		key
				Bitmap
	Outputs:	-
	Returns:	-
	Purpose:	Places a bitmap in the cache (making room if necessary by removing others)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CBitmapCache::StoreBitmap(const CBitmapCacheKey& key, const CCachedBitmap& bitmap)
{
/*
#if _DEBUG
	if ((UINT32)m_nCount >= m_lMaxHashTableLoad)
	{
		TRACE( _T("BitmapCache hash table moving into inefficient zone!\n"));
//		ERROR3("BitmapCache hash table moving into inefficient zone\n");
	}
#endif
*/
//	ENSURE(bitmap.rectCachedRect.IsValid(), "Invalid rect in CBitmapCache::StoreBitmap");
	ENSURE(bitmap.pbmpInfo->bmiHeader.biCompression==BI_RGB || bitmap.pbmpInfo->bmiHeader.biCompression==0x80000001,
			"Invalid compression type in StoreBitmap");

	// Remove the previous entry for this key, if there was one...
	CCachedBitmap cbmp = RemoveBitmap(key);									// Watch out - doesn't release bitmap memory!
	if (bitmap.pbmpBits!=cbmp.pbmpBits && bitmap.pbmpInfo!=cbmp.pbmpInfo)	// Only release the bitmap if we're not still using it
		cbmp.Release();														// But this does!

	UINT32 newitemsize = bitmap.GetBitmapSize();
	UINT32 priority = bitmap.nPriority;
	while ((UINT64)(m_lCurrentDataSize+newitemsize) > m_lMaxDataSize)
	{
		CCachedBitmap cbmp = RemoveRandomBitmap(priority);					// Remove random bitmap from cache
		if (cbmp.IsValid())
		{
			ERROR3IF(bitmap.pbmpInfo==cbmp.pbmpInfo || bitmap.pbmpBits==cbmp.pbmpBits,
					 "RemoveRandomBitmap found the same bitmap we're trying to Store! Something's gone horribly wrong!\n");
			TRACE( _T("Removing bitmap to make room in cache!\n") );
			cbmp.Release();
		}
		else
		{
			// We have run out of room in the cache for this priority level
			TRACEUSER( "Phil", _T("Changing priority to make room in cache\n"));
			if (priority < CACHEPRIORITY_TEMPBITMAP_HIGH)
			{
				// We can try to remove higher-priority temp bitmaps...
				// (But not the very highest, permanent bitmaps...)
				priority = CACHEPRIORITY_TEMPBITMAP_HIGH;
			}
			else
			{
				// We have no more room to remove any temporary bitmaps
				// ARGH!!!! Now what?
				ERROR3("Bitmap Cache is full!");
				break;					// Just gobble up the user's memory!
			}
		}
	}

	m_lCurrentDataSize += newitemsize;
//	SetAt(key, (CCachedBitmap)bitmap);
	m_map.insert(pair<CBitmapCacheKey, CCachedBitmap>(key,(CCachedBitmap)bitmap));
}




/********************************************************************************************

>	CCachedBitmap CBitmapCache::RemoveBitmap(const CBitmapCacheKey& key)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/2004
	Inputs:		key
	Outputs:	-
	Returns:	Pointer to bitmap or NULL if not found
	Purpose:	Removes either the specified bitmap from the cache or a selected bitmap
				according to the removal policy
				NOTE! It's up to the caller to delete the bitmap returned!!!
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
CCachedBitmap CBitmapCache::RemoveBitmap(const CBitmapCacheKey& key)
{
	CCachedBitmap bitmap;

	if (m_map.size()==0)
			return bitmap;

	if (key.IsValid())
	{
		// Then we must remove the specified bitmap from the cache
		if (Lookup(key, bitmap))
		{
			m_lCurrentDataSize -= bitmap.GetBitmapSize();
			m_map.erase(key);
//			RemoveKey(key);
		}
	}

	return bitmap;
}




/********************************************************************************************

>	CCachedBitmap CBitmapCache::RemoveRandomBitmap(const INT32 maxpriority = CACHEPRIORITY_TEMPBITMAP_HIGH)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/2004
	Inputs:		key
	Outputs:	-
	Returns:	Pointer to bitmap or NULL if not found
	Purpose:	Removes either the specified bitmap from the cache or a selected bitmap
				according to the removal policy
				NOTE! It's up to the caller to delete the bitmap returned!!!
				NOTE! This routine only uses the primary association array of the hash table
						The chains of values stored in the same slot are ignored at first
						(but obviously will be considered later on if their head item
						has been removed)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
CCachedBitmap CBitmapCache::RemoveRandomBitmap(const INT32 maxpriority)
{
//	ENSURE(m_nHashTableSize!=0, "Zero-sized hash table found in RemoveRandomBitmap\n");
/*
	// Implement cache removal policy - remove a random bitmap
	//
	// Select a bitmap of our choosing for removal
	// The removal policy is random (ish)
	// Must be Order(0) complexity
	//
	CCachedBitmap bitmap;


	UINT32 startitem = rand() % m_nHashTableSize;
	UINT32 item = startitem;
	CAssoc* pAssoc = NULL;
	do
	{
		pAssoc = m_pHashTable[item];
		item++;
		if (item==m_nHashTableSize)
			item = 0;

		// If we have found a value, check whether it's of equal or lesser "value"
		// than the one we're trying to store before throwing it out...
		if (pAssoc)
		{
//			bitmap = (CCachedBitmap)pAssoc->value;		// Get value in a form we can use
			if (((CCachedBitmap)pAssoc->value).nPriority <= maxpriority)		// Check whether value's priority is <= priority limit
				break;									// If so, we'll get rid of this one
			else
				pAssoc = NULL;
		}
	}
	while (item != startitem);

	if (pAssoc)
	{
		// Remove the bitmap that we found from the cache...
		bitmap = (CCachedBitmap)pAssoc->value;
		m_lCurrentDataSize -= bitmap.GetBitmapSize();
		RemoveKey(pAssoc->key);
	}

	return bitmap;
*/
	// Implement cache removal policy - remove a random bitmap
	//
	// Select a bitmap of our choosing for removal
	// The removal policy is random (ish)
	// Must be Order(0) complexity
	//
	// Pick a random start point iterate
	// through the map from there...
	//
	srand((unsigned)time(NULL));
	UINT32 item = rand()%(UINT32)m_map.size();
	CCacheKeyMap::iterator pos;
	for ( pos=m_map.begin() ; item-- ; pos++ )
		;
	CCacheKeyMap::iterator spos = pos;
	do
	{
		if ( pos==m_map.end() )
			pos = m_map.begin() ;
		CBitmapCacheKey key = pos->first;
		CCachedBitmap bitmap = pos->second;
		if ( bitmap.nPriority<=maxpriority )		// Check whether value's priority is <= priority limit
		{
			m_lCurrentDataSize -= bitmap.GetBitmapSize();
			m_map.erase(key);
			return bitmap;
		}
		pos++;
	} while ( pos!=spos );
	return m_map.begin()->second;
}

	
	

/********************************************************************************************

>	BOOL CBitmapCache::RemoveAllOwnedBitmaps(const CBitmapCacheKey& key,
											const BOOL bOpaqueOnly,
											const INT32 maxpriority = CACHEPRIORITY_TEMPBITMAP_HIGH)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/2004
	Inputs:		key
	Outputs:	-
	Returns:	TRUE if removed one or more bitmap associated with the owner of this key
				FALSE otherwise
	Purpose:	Removes all bitmaps from the cache that have the owner specified in the
				key...
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL CBitmapCache::RemoveAllOwnedBitmaps(const CBitmapCacheKey& key,
										 const BOOL bOpaqueOnly,
										 const INT32 maxpriority)
{
	BOOL bRemovedSome = FALSE;
	CCachedBitmap abitmap;

	if (!key.IsValid())
		return FALSE;

	if (m_map.size()==0)
		return FALSE;

	// Use the owner information stored in the keys to find all bitmap entries
	// in the hash table with the same owner...
/*	POSITION pos = GetStartPosition();
	while (pos)
	{
		CBitmapCacheKey akey;
		GetNextAssoc(pos, akey, abitmap);

		if (akey.pOwner == key.pOwner && abitmap.nPriority <= maxpriority)
		{
			if (bOpaqueOnly==FALSE || !abitmap.IsTransparent())
			{
				RemoveKey(akey);
				m_lCurrentDataSize -= abitmap.GetBitmapSize();
				FreeDIB(abitmap.pbmpInfo, abitmap.pbmpBits);
				bRemovedSome = TRUE;
			}
		}
	}
*/	CCacheKeyMap::iterator pos;
	for ( pos=m_map.begin() ; pos!=m_map.end() ; )
	{
		CBitmapCacheKey akey = pos->first;
		CCachedBitmap abitmap = pos->second;
		pos++;
		if (akey.pOwner == key.pOwner && abitmap.nPriority <= maxpriority)
		{
			if (bOpaqueOnly==FALSE || !abitmap.IsTransparent())
			{
				m_map.erase(akey);
				m_lCurrentDataSize -= abitmap.GetBitmapSize();
				FreeDIB(abitmap.pbmpInfo, abitmap.pbmpBits);
				bRemovedSome = TRUE;
			}
		}
	}

	return bRemovedSome;
}




/********************************************************************************************

>	CCachedBitmap CBitmapCache::FindNextOwnedBitmap(POSITION& pos,
											CBitmapCacheKey& key,
											const BOOL bOpaqueOnly,
											const INT32 maxpriority = CACHEPRIORITY_TEMPBITMAP_HIGH)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/2004
	Inputs:		key
	Outputs:	-
	Returns:	TRUE is removed one or more bitmap associated with the owner of this key
				FALSE otherwise
	Purpose:	Removes all bitmaps from the cache that have the owner specified in the
				key...
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
CCachedBitmap CBitmapCache::FindNextOwnedBitmap(CCacheKeyMap::iterator &pos,
										CBitmapCacheKey& key,
										const BOOL bOpaqueOnly,
										const INT32 maxpriority)
{
//	BOOL bRemovedSome = FALSE;
	CCachedBitmap abitmap;

	if (!key.IsValid())
		return abitmap;

	if (m_map.size()==0)
		return abitmap;

	// Use the owner information stored in the keys to find all bitmap entries
	// in the hash table with the same owner...
/*	while (pos)
	{
		CBitmapCacheKey akey;
		GetNextAssoc(pos, akey, abitmap);

		if (akey.pOwner == key.pOwner && abitmap.nPriority <= maxpriority)
		{
			if (bOpaqueOnly==FALSE || !abitmap.IsTransparent())
			{
				key = akey;			// Return the key we found
				return abitmap;		// along with the bitmap we found
			}
		}
	}
*/
	while (pos!=m_map.end())
	{
		CBitmapCacheKey akey = pos->first;
		CCachedBitmap abitmap = pos->second;
		pos++;
		if (akey.pOwner == key.pOwner && abitmap.nPriority <= maxpriority)
		{
			if (bOpaqueOnly==FALSE || !abitmap.IsTransparent())
			{
				key = akey;			// Return the key we found
				return abitmap;		// along with the bitmap we found
			}
		}
	}

	return CCachedBitmap();
}




/********************************************************************************************

>	void CBitmapCache::SetMaximumDataSize(UINT64 maxsize)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CBitmapCache::SetMaximumDataSize(UINT64 maxsize)
{
	m_lMaxDataSize = maxsize;
}




/********************************************************************************************

>	UINT64 CBitmapCache::GetMaximumDataSize()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
UINT64 CBitmapCache::GetMaximumDataSize()
{
	return m_lMaxDataSize;
}




/********************************************************************************************

>	UINT32 CBitmapCache::GetCurrentDataSize()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
UINT32 CBitmapCache::GetCurrentDataSize()
{
	return m_lCurrentDataSize;
}




/********************************************************************************************

>	UINT64 CBitmapCache::CalcRecommendedMaximumDataSize()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	Recommended max cache size in current memory environment
	Purpose:	Work out how much RAM this machine can afford to devote
				to the bitmap cache.
				Never use more than 50% of physical RAM
				Never use more RAM than is free
				Use CachRAMPercent preference to decide percentage of free ram to grab
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
UINT64 CBitmapCache::CalcRecommendedMaximumDataSize()
{
	UINT64		ullPhysRAM = 0;
	UINT32		LoadPercentage = 0;

	GetMemoryStatus(&ullPhysRAM, &LoadPercentage);

	// Don't use more than 50% of physical ram in the Bitmap Cache
	if (LoadPercentage<50) LoadPercentage = 50;		// If <50% in use pretend that 50% is used (never use more than 50%)
	return ullPhysRAM * (100-LoadPercentage) * CacheRAMPercent / 10000;
}




/********************************************************************************************

>	UINT32 CBitmapCache::PrimeAbove(UINT32 number)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> (from the Internet)
	Created:	11/06/2004
	Inputs:		number to start from
	Outputs:	-
	Returns:	prime number above the starting value
	Purpose:	Computes the first prime number equal to or greater than a specified number.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*
UINT32 CBitmapCache::PrimeAbove(UINT32 number)
{
    if (number < 4)  return (number) ;		// 0, 1, 2, and 3 are prime.
    if ((number % 2) == 0)  number++ ;		// Prime must be odd.

    for ( ; ; )
	{
//    Check for possible divisors.  The "divisor > dividend" test is similar
//    to checking 2 .. sqrt(N) as possible divisors, but avoids the need for
//    linking to the math library.

        for ( UINT32 divisor = 3 ; ; divisor+=2 )
		{
            if ((number % divisor) == 0)
                break ;				// Not prime - divisor found.
            if (divisor > (number / divisor))
                return number ;		// Prime - no divisors found.
        }

        number += 2 ;				// Check next odd number.
    }
}
*/



#ifdef _DEBUG
/********************************************************************************************

>	void CBitmapCache::DebugDump()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/11/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CBitmapCache::DebugDump()
{
	TRACE( _T("Contents of Bitmap Cache:\n"));
/*	POSITION pos = GetStartPosition();
	while (pos)
	{
		CBitmapCacheKey key;
		CCachedBitmap bitmap;
		GetNextAssoc(pos, key, bitmap);

		TRACE( _T("Bitmap: %x (%d*%d, %d) Priority: %d Owner: %x %d %s\n"),
														bitmap.pbmpInfo,
														bitmap.pbmpInfo->bmiHeader.biWidth,
														bitmap.pbmpInfo->bmiHeader.biHeight,
														bitmap.GetBitmapSize(),
														bitmap.nPriority,
														key.pOwner,
														key.nOption,
														((CCObject*)key.pOwner)->GetRuntimeClass()->m_lpszClassName
														);
	}
*/	CCacheKeyMap::iterator pos;
	for ( pos=m_map.begin() ; pos!=m_map.end() ; pos++ )
	{
		CBitmapCacheKey key = pos->first;
		CCachedBitmap bitmap = pos->second;
		TRACE( _T("Bitmap: %x (%d*%d, %d) Priority: %d Owner: %x %d %s\n"),
														bitmap.pbmpInfo,
														bitmap.pbmpInfo->bmiHeader.biWidth,
														bitmap.pbmpInfo->bmiHeader.biHeight,
														bitmap.GetBitmapSize(),
														bitmap.nPriority,
														key.pOwner,
														key.nOption,
														((CCObject*)key.pOwner)->GetRuntimeClass()->m_lpszClassName
														);
	}
}
#endif




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCachedBitmap::CCachedBitmap()
{
	pbmpInfo = NULL;
	pbmpBits = NULL;
//	rectCachedRect = DocRect(0,0,0,0);
	coord0 = DocCoord(0,0);
	coord1 = DocCoord(0,0);
	coord2 = DocCoord(0,0);
	nPriority = 0;
}

CCachedBitmap::CCachedBitmap(LPBITMAPINFO pNewInfo, LPBYTE pNewBits, INT32 nNewPriority)
{
	pbmpInfo = pNewInfo;
	pbmpBits = pNewBits;
//	rectCachedRect = DocRect(0,0,0,0);
	coord0 = DocCoord(0,0);
	coord1 = DocCoord(0,0);
	coord2 = DocCoord(0,0);
	nPriority = nNewPriority;
}

CCachedBitmap::CCachedBitmap(const CCachedBitmap& cbmp)
{
	pbmpInfo = cbmp.pbmpInfo;
	pbmpBits = cbmp.pbmpBits;
//	rectCachedRect = cbmp.rectCachedRect;
	coord0 = cbmp.coord0;
	coord1 = cbmp.coord1;
	coord2 = cbmp.coord2;
	nPriority = cbmp.nPriority;
}


CCachedBitmap::CCachedBitmap(UINT32 Width, UINT32 Height, UINT32 Depth, DocRect rect, INT32 nNewPriority)
{
	ENSURE(rect.IsValid(), "Attempt to create a cached bitmap with an invalid rectangle!");
	pbmpInfo = AllocDIB(Width, Height, Depth, &pbmpBits);
//	rectCachedRect = rect;
	coord0 = DocCoord(rect.lo.x, rect.lo.y);
	coord1 = DocCoord(rect.hi.x, rect.lo.y);
	coord2 = DocCoord(rect.lo.x, rect.hi.y);
	nPriority = nNewPriority;
}


CCachedBitmap::~CCachedBitmap()
{
//	if (pbmpInfo && pbmpBits)					DON'T DO THIS!!!
//		DIBUtil::FreeDIB(pbmpInfo, pbmpBits);
	pbmpInfo = NULL;
	pbmpBits = NULL;
}




/********************************************************************************************

>	void CCachedBitmap::Release()
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this key is valid
				FALSE otherwise
	Purpose:	Test whether this object has been constructed correctly
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CCachedBitmap::Release()
{
	FreeDIB(pbmpInfo, pbmpBits);
}




/********************************************************************************************

>	CCachedBitmap CCachedBitmap::Copy()
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Copy this cached bitmap (actually copy the bitmap, not just the bitmap pointers)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
CCachedBitmap CCachedBitmap::Copy()
{
	CCachedBitmap copybmp(*this);
	INT32 lHRes = pbmpInfo->bmiHeader.biXPelsPerMeter;
	INT32 lVRes = pbmpInfo->bmiHeader.biYPelsPerMeter;
	DIBUtil::CopyBitmap(pbmpInfo, pbmpBits, &copybmp.pbmpInfo, &copybmp.pbmpBits);
	if (copybmp.pbmpInfo)
	{
		copybmp.pbmpInfo->bmiHeader.biXPelsPerMeter = lHRes;
		copybmp.pbmpInfo->bmiHeader.biYPelsPerMeter = lVRes;
	}
	return copybmp;
}




/********************************************************************************************

>	CCachedBitmap::operator==()
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this key is exactly the same the the specified key
	Purpose:	Test whether this object has been constructed correctly
				Required by CTypedPtrMap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
bool CCachedBitmap::operator==(const CCachedBitmap& cbmp) const
{
	return (pbmpInfo == cbmp.pbmpInfo &&
			pbmpBits == cbmp.pbmpBits &&
//			rectCachedRect == cbmp.rectCachedRect &&
			coord0 == cbmp.coord0 &&
			coord1 == cbmp.coord1 &&
			coord2 == cbmp.coord2 &&
			nPriority == cbmp.nPriority
			);
}




/********************************************************************************************

>	CCachedBitmap::operator=()
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this key is exactly the same the the specified key
	Purpose:	Test whether this object has been constructed correctly
				Required by CTypedPtrMap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
const CCachedBitmap&  CCachedBitmap::operator=(const CCachedBitmap& cbmp)
{
	pbmpInfo = cbmp.pbmpInfo;
	pbmpBits = cbmp.pbmpBits;
//	rectCachedRect = cbmp.rectCachedRect;
	coord0 = cbmp.coord0;
	coord1 = cbmp.coord1;
	coord2 = cbmp.coord2;
	nPriority = cbmp.nPriority;
	return cbmp;
}

/********************************************************************************************

>	BOOL CCachedBitmap::IsValid() const
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this key is valid
				FALSE otherwise
	Purpose:	Test whether this object has been constructed correctly
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL CCachedBitmap::IsValid() const
{
	return (pbmpInfo!=NULL && pbmpBits!=NULL);
}




/********************************************************************************************

>	UINT32 CCachedBitmap::GetBitmapSize() const
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	Size of bitmap in bytes
	Purpose:	Find the size of this bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
UINT32 CCachedBitmap::GetBitmapSize() const
{
	return (pbmpInfo->bmiHeader.biSizeImage);
}




/********************************************************************************************

>	BOOL CCachedBitmap::IsTransparent() const
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if cached bitmap is 32BPP (transparent)
	Purpose:	Find out whether this bitmap has a transparency channel or not
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL CCachedBitmap::IsTransparent() const
{
	return (pbmpInfo->bmiHeader.biCompression!=BI_RGB);
}




/********************************************************************************************

>	UINT32 CCachedBitmap::GetBPP() const
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/2004
	Inputs:		-
	Outputs:	-
	Returns:	Bits Per Pixel
	Purpose:	Get bit depth of cached bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
UINT32 CCachedBitmap::GetBPP() const
{
	return (pbmpInfo->bmiHeader.biBitCount);
}




/********************************************************************************************

>	void CCachedBitmap::Transform(TransformBase& Trans)
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/20/2004
	Inputs:		Transform to apply to the stored coords of this bitmap
	Outputs:	-
	Returns:	-
	Purpose:	Transform the cached bitmap so that it can be distorted/repositioned
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CCachedBitmap::Transform(TransformBase& Trans)
{
//	Trans.Transform((DocCoord*)&rectCachedRect, 2);
	Trans.Transform(&coord0, 3);
}




/********************************************************************************************

>	DocRect CCachedBitmap::GetCachedRect()
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/20/2004
	Inputs:		-
	Outputs:	-
	Returns:	DocRect - upright rectangle containing all points in stored parallelogram
	Purpose:	Transform the cached bitmap so that it can be distorted/repositioned
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
DocRect CCachedBitmap::GetCachedRect()
{
	DocRect r(coord0, coord0);
	r.IncludePoint(coord1);
	r.IncludePoint(coord2);
	r.IncludePoint(DocCoord(coord2.x+coord1.x-coord0.x, coord2.y+coord1.y-coord0.y));

	return r;
}




/********************************************************************************************

>	void CCachedBitmap::SetCachedRect(DocRect r)
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/20/2004
	Inputs:		-
	Outputs:	-
	Returns:	DocRect - upright rectangle containing all points in stored parallelogram
	Purpose:	Transform the cached bitmap so that it can be distorted/repositioned
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CCachedBitmap::SetCachedRect(DocRect r)
{
	coord0 = r.lo;
	coord1 = DocCoord(r.hi.x, r.lo.y);
	coord2 = DocCoord(r.lo.x, r.hi.y);
}




/********************************************************************************************

>	void CCachedBitmap::SetCachedParallelogram(DocCoord* pCoords, UINT32 numCoords)
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Store parallelogram details
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CCachedBitmap::SetCachedParallelogram(DocCoord* pCoords, UINT32 numCoords)
{
	coord0 = pCoords[0];
	coord1 = pCoords[1];
	coord2 = pCoords[2];
}




