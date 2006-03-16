// $Id: bitmapcache.h 662 2006-03-14 21:31:49Z alex $
// BitmapCache.h: interface for the CBitmapCache class.
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

#if !defined(AFX_BITMAPCACHE_H__41BFBD9B_A02E_4205_AE0A_4F6A5E0DC7D0__INCLUDED_)
#define AFX_BITMAPCACHE_H__41BFBD9B_A02E_4205_AE0A_4F6A5E0DC7D0__INCLUDED_

//#include <afxtempl.h>
#include "bitmapcachekey.h"
#include "bitmap.h"

#define CACHEPRIORITY_NORMAL 0
#define CACHEPRIORITY_TEMPBITMAP_LOW 0
#define CACHEPRIORITY_TEMPBITMAP_HIGH 1000
#define CACHEPRIORITY_PERMANENT 8000


class CCachedBitmap: public CCObject
{
public:
	CCachedBitmap();
	CCachedBitmap(LPBITMAPINFO pNewInfo, LPBYTE pNewBits, INT32 nNewPriority = CACHEPRIORITY_NORMAL);
	CCachedBitmap(const CCachedBitmap& cbmp);
	CCachedBitmap(UINT32 Width, UINT32 Height, UINT32 Depth, DocRect rect, INT32 nNewPriority = CACHEPRIORITY_NORMAL);
	~CCachedBitmap();

	bool operator==(const CCachedBitmap& cbmp) const;
	const CCachedBitmap& operator=( const CCachedBitmap& cbmp );

	void			Release();
	CCachedBitmap	Copy();
	void			Transform(TransformBase& Trans);
	DocRect			GetCachedRect();
	void			SetCachedRect(DocRect r);
	void			SetCachedParallelogram(DocCoord* pCoords, UINT32 numCoords);

	BOOL			IsValid() const;
	UINT32			GetBitmapSize() const;
	BOOL			IsTransparent() const;
	UINT32			GetBPP() const;

public:
	LPBITMAPINFO	pbmpInfo;
	LPBYTE			pbmpBits;
//	DocRect			rectCachedRect;
	DocCoord		coord0;				// Three coords representing a parallelogram alow the bitmap to be transformed
	DocCoord		coord1;				// (mainly for use with destructive liveffects but also good for quick blits
	DocCoord		coord2;				//  of cached info during dragging)
	INT32			nPriority;
	BOOL			bFullCoverage;		// TRUE when this bitmap fully covers the relevant object
};


// Declare global template-specific override for HashKey function...
struct LessBitmapCacheKey
{
  bool operator()(const CBitmapCacheKey &s1, const CBitmapCacheKey &s2) const
  {
	  return s1.Hash() < s2.Hash();
  }
};

typedef std::map<CBitmapCacheKey, CCachedBitmap, LessBitmapCacheKey> CCacheKeyMap;

class CBitmapCache
{
public:
	CBitmapCache();
//	virtual ~CBitmapCache();

// Methods
public:
	void			Initialise();
	void			DeInitialise();

	void			StoreBitmap(const CBitmapCacheKey& key, const CCachedBitmap& bitmap);
//	CCachedBitmap	FindBitmap(const CBitmapCacheKey& key);
	CCachedBitmap	RemoveBitmap(const CBitmapCacheKey& key);
	BOOL			RemoveAllOwnedBitmaps(	const CBitmapCacheKey& key,
											const BOOL bOpaqueOnly = FALSE,
											const INT32 maxpriority = CACHEPRIORITY_TEMPBITMAP_HIGH);
	void			RemoveLowPriorityBitmaps(const INT32 maxpriority = CACHEPRIORITY_TEMPBITMAP_HIGH);
	CCachedBitmap	FindNextOwnedBitmap(CCacheKeyMap::iterator &pos,
										CBitmapCacheKey &key,
										const BOOL bOpaqueOnly = FALSE,
										const INT32 maxpriority = CACHEPRIORITY_TEMPBITMAP_HIGH);
	void			SetMaximumDataSize(UINT64 maxsize);
	UINT64			GetMaximumDataSize();
	UINT32			GetCurrentDataSize();

#ifdef _DEBUG
	void			DebugDump();
#endif

	// Some CMap emulation functions
	BOOL Lookup( const CBitmapCacheKey &Key, CCachedBitmap &CachedBitmap )
	{
		CCacheKeyMap::iterator iter = m_map.find( Key );
		if( m_map.end() == iter )
			return false;
		
		CachedBitmap = iter->second;
		return true;
	}

	CCacheKeyMap::iterator GetStartPosition() {return m_map.begin();}

// Public helper functions
public:
	static UINT64 CalcRecommendedMaximumDataSize();

// Private helper functions
private:
//	UINT32			PrimeAbove(UINT32 number);
	CCachedBitmap	RemoveRandomBitmap(const INT32 maxpriority = CACHEPRIORITY_TEMPBITMAP_HIGH);			// Cache removal policy helper

// Internal members
private:
	CCacheKeyMap	m_map;

	UINT64		m_lMaxDataSize;
	UINT32			m_lCurrentDataSize;
	UINT32			m_lMaxHashTableLoad;

// Preferences
	static INT32 CacheRAMPercent;			// Amount of free RAM to grab for cacheing

};

#endif // !defined(AFX_BITMAPCACHE_H__41BFBD9B_A02E_4205_AE0A_4F6A5E0DC7D0__INCLUDED_)
