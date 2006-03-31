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

// The implementation of the TextTool's FontCache

/*
// */

// To resolve: allocation of CamCache

// Set CACHE_STATS to 0 if you don't want loads of TRACE


// Includes
#include "camtypes.h"
#include "fntcache.h"
#include "app.h"
//#include "mario.h"
#include "errors.h"
#include "pathtype.h"
//#include "textfuns.h"
#include "grndrgn.h"
#include "gdraw.h"
#include "fontman.h"
#ifdef RALPH
#include "ralphcri.h"
#endif

#define CACHE_STATS	0
#define new CAM_DEBUG_NEW

#if CACHE_STATS
static UINT32 AveragePathSize = 0;
static UINT32 MinPathSize = 0;
static UINT32 MaxPathSize = 0;
static UINT32 Hits = 0;
static UINT32 Misses = 0;
static UINT32 DisplayRate = 50;
static UINT32 DisplayCnt = 0;
#endif



// statics ...
BOOL    FontCache::InitCalled = FALSE; // Set to TRUE on entry to Init function 
CamCache* FontCache::pPathCache;
INT32     FontCache::BoundsEntries    = 0;		// number of entries in char bounds cache


/********************************************************************************************

>	static BOOL FontCache::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if we run out of memory.

	Purpose:	Initialises the FontCache. This function must be called before the cache can be
				used. 
	
	Scope:		public			 

	Errors:		If we run out of memory during initialisation then FALSE is returned and
				an error is set.
	SeeAlso:	-

********************************************************************************************/



BOOL FontCache::Init()
{
	InitCalled = TRUE; 
	BoundsEntries = 0;	// reset number of char bounds cached

	// Try to allocate our path cache
	pPathCache = new CamCache(); 
	ERROR1IF(pPathCache == NULL, FALSE, _R(IDE_NOMORE_MEMORY));
	return TRUE; 
}


/********************************************************************************************

>	static void FontCache::DeInit()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-

	Purpose:	Deinitialises the font cache
	
	Scope:		public			 

	SeeAlso:	-

********************************************************************************************/

void FontCache::DeInit()
{
	BoundsEntries = 0;	// reset number of char bounds cached

	// Only DeInit if Init was called
	if (InitCalled)
	{
		// Delete the PathCache if one exists
		if (pPathCache)
		{
			delete (pPathCache);
			pPathCache = NULL;
		}
	}
}



/********************************************************************************************

>	static BOOL FontCache::GetPath(CharDescription& ChDesc, INT32**Points, BYTE**Types, 
								   UINT32 *Length)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		ChDesc:	A description of the character you want me to find a path for
				
	Outputs:	Points:	An array of path points
				Types:	An array of path types
				Length: The path length

				After calling GetPath you should create a 
	
	Returns:	TRUE if we find the requested path
				FALSE if we run out of memory. An error is returned and the cache is left in
				a stable state. i.e GetPath can be called again and will succeed if the right
				amount of memory is available. 
	Purpose:	
	Errors:		-
	Scope:		public
	SeeAlso:	-

********************************************************************************************/

BOOL FontCache::GetPath(CharDescription& ChDesc, INT32**Points, BYTE**Types, UINT32 *Length)
{
	#ifdef RALPH
	RalphCriticalSection RalphCS;
	#endif

	// Obtain the path associated with the path handle
	// ChDesc will only be required if the character does not exist in the cache
	// If the function runs out of memory then an error will be set.

	ERROR3IF(pPathCache == NULL, "The FontCache has not been initialised"); 

	if (!pPathCache->FindPath( ChDesc.charHandle, *Points, *Types, *Length ) )
	{
		// We need to generate the path here !
		///*
		if (FONTMANAGER->GetCharPath(ChDesc, (DocCoord**)Points, (PathVerb**)Types, Length))
		{
			#if CACHE_STATS
			Misses++;
			MinPathSize = (*Length < MinPathSize) ? *Length: MinPathSize;
			MaxPathSize = (*Length > MaxPathSize) ? *Length: MaxPathSize;
			AveragePathSize = (AveragePathSize + *Length) / 2;
			#endif
			pPathCache->AddPath( ChDesc.charHandle, *Points, *Types, *Length );
		}
		else
		{
			return FALSE; // Probably ran out of memory
		}
	}
	#if CACHE_STATS
	else
	{
		Hits++;
	}

	DisplayCnt++;

	if (DisplayCnt == DisplayRate)
	{
		DisplayCnt = 0;
		// Display stats
		TRACE( _T("---")); 
		TRACE( _T("Hits = %lu, Misses = %lu\n"), Hits, Misses);
		TRACE( _T("Average Path Size = %lu\n"), AveragePathSize*9); 
		TRACE( _T("Minimum Path Size = %lu\n"), MinPathSize*9);
		TRACE( _T("Maximum Path Size = %lu\n"), MaxPathSize*9);
		TRACE( _T("Hit ratio = %lu\n\n"), Hits/Misses); 
	}


	#endif



#if CACHE_STATS
static INT32 Hitpc;
#endif


	return TRUE; 
}


/********************************************************************************************
>	static BOOL FontCache::GetBounds(DocRect* pBounds, CharDescription& CharDesc)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		CharDesc - char description (charcode/bold/italic/fonthandle)
	Outputs:	pBounds  - bounds of the char
	Returns:	FALSE if fails
	Purpose:	Get the bounds of a char specified by the char descriptor at default size
********************************************************************************************/

struct AttrdCharBoundsCacheEntry
{
	CharDescription desc;
	DocRect			bounds;
};

BOOL FontCache::GetBounds(DocRect* pBounds, CharDescription& CharDesc)
{
	#ifdef RALPH
	RalphCriticalSection RalphCS;
	#endif

	ERROR2IF(pBounds==NULL,FALSE,"FontCache::GetBounds() - pBounds==NULL");

	// static cache
	const  INT32 MaxBoundsEntries = 128;
	static AttrdCharBoundsCacheEntry entry[MaxBoundsEntries];

	// search cache for desired char
	INT32 i=0;
	while (i<BoundsEntries)
	{
		if (entry[i].desc==CharDesc)
			break;
		i+=1;
	}

	// if not found, either add to end of cache or replace a random entry
	if (i==BoundsEntries)
	{
		if (BoundsEntries<MaxBoundsEntries)
			i=BoundsEntries++;
		else
			i = rand() % MaxBoundsEntries;
		entry[i].desc=CharDesc;
		if (FontCache::CalcDefaultCharBounds(&(entry[i].bounds),CharDesc)==FALSE)
			return FALSE;
	}

	// set output and return
	*pBounds=entry[i].bounds;
	return TRUE;
}


/********************************************************************************************
>	static BOOL FontCache::CalcDefaultCharBounds(DocRect* pRect, CharDescription CharDesc)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Inputs:		CharDesc - description of char (charcode/bold/italic/font handle)
	Outputs:	pRect    - bounding box of char
	Returns:	FALSE if fails
	Purpose:	Calculate the bounding box of the specified char at default size
	Note:		Always uses zero line width (LineCapButt and BevelledJoin)
********************************************************************************************/

BOOL FontCache::CalcDefaultCharBounds(DocRect* pRect, CharDescription& CharDesc)
{
	#ifdef RALPH
	RalphCriticalSection RalphCS;
	#endif

	ERROR2IF(pRect==NULL,FALSE,"FontCache::CalcCharBounds() - pRect==NULL");

	// create the char's default path
	Path* pCharPath=FontCache::CreateDefaultCharPath(CharDesc);
	if (pCharPath==NULL)
		return FALSE;

	// calculate the path's bounds
	BOOL    ok=TRUE;
    RECT    DrawRect = {0, 0, 0, 0};
	DocRect TempRect(0,0,0,0);
	INT32    points=pCharPath->GetNumCoords();
	if (points!=0)
	{
 		DocCoord* pCoords=pCharPath->GetCoordArray();
		PathVerb* pVerbs =pCharPath->GetVerbArray();

		if (ok) ok=(pCoords!=NULL && pVerbs!=NULL);
		if (ok)
		{
			// Get hold of the default GDraw context.
			GDrawContext* pGDC = GRenderRegion::GetStaticDrawContext();

			if (pGDC == NULL)
				ok = FALSE;
			else
				ok = !pGDC->CalcStrokeBBox((POINT*)pCoords, pVerbs, points, &DrawRect, pCharPath->IsFilled, 0, CAPS_BUTT, JOIN_BEVEL, NULL);
			// TRACEUSER("wuerthne", _T("CDraw returned %d bbox from CDraw is %d, %d, %d, %d"), ok, DrawRect.left, DrawRect.top, DrawRect.right, DrawRect.bottom);

			// if the call to GDraw failed then we fall back to calculating the bounds from the points bounding box.
			if (ok)
			{
				// GDraw did the job, so copy over the result from its RECT structure
				// NB: The following assignment *looks* wrong because top and bottom seem to be swapped over,
				//     but it is correct! The fields in RECT are just named in a strange way - GDraw certainly
				//     has the same view of the structure as DocRect, so we only need to copy the fields over
				//     in the order as they are defined and things are OK. Alternatively, we could pass a pointer
				//     to a DocRect to GDraw but this would break things if ever someone changed the representation
				//     of DocRect. I hope noone changes the field names in RECT - the ENSURE protects us:
				ENSURE(&DrawRect.bottom > &DrawRect.right && &DrawRect.right > &DrawRect.top && &DrawRect.top > &DrawRect.left,
					   "Incompatible change of RECT structure");
				TempRect = DocRect(DrawRect.left, DrawRect.top, DrawRect.right, DrawRect.bottom);
			}
			else
			{
				// GDraw failed, so scan the coordinates
				// pCharPath->DumpPath();
				TempRect = pCharPath->GetBoundingRect();
				ok = TRUE; // Well not really !
			}
		}
	}

	// tidy up, set output and return
	delete pCharPath;
	if (ok)	*pRect=TempRect;
	return ok;
}


/********************************************************************************************
>	static Path* FontCache::CreateDefaultCharPath(CharDescription CharDesc)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Inputs:		CharDesc - description of char (charcode/bold/italic/font handle)
	Returns:	pointer to new path (else NULL if fails)
	Purpose:	Create a path of the specified char at the default height
	Note:		The caller is resposible for initialising the path's flags if required
********************************************************************************************/

Path* FontCache::CreateDefaultCharPath(CharDescription& CharDesc)
{
	#ifdef RALPH
	RalphCriticalSection RalphCS;
	#endif

	// get pointers to the char's path data
	DocCoord* pCoords=NULL;
	PathVerb* pVerbs=NULL;
	UINT32 points=0;
	if (FontCache::GetPath(CharDesc, (INT32**)&pCoords, &pVerbs, &points)==FALSE)
		return FALSE;

	// put path data into a Path structure
	Path* pPath=new Path(); 
	ERROR2IF(pPath==NULL,NULL,"FontCache::CreateDefaultCharPath() - failed to create path");
	BOOL ok=pPath->Initialise(points,12);
	if (ok) ok=pPath->CopyPathDataFrom(pCoords,pVerbs,points,TRUE);

	// if not OK, delete any path that exists and set returned path pointer to NULL
	if (!ok)
	{
		delete pPath;
		pPath=NULL;
	}

	return pPath;
}


// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

#if 0


//
// WE ARE NOT USING THE HI-TECH FONT CACHE AT THE MOMENT
//
//



// static data
PathHandleItem* FontCache::CacheA [CACHE_LIST_SIZE]; 
PathHandleItem* FontCache::CacheB [CACHE_LIST_SIZE];
PathHandleItem** FontCache::PrimaryC; 
PathHandleItem** FontCache::SecondaryC;
CamCache* FontCache::pPathCache;
UINT32 FontCache::NextUniquePathHandle;
UINT32 FontCache::NumItemsInPrimaryCache;

// For test purposes
#ifdef _DEBUG
UINT32 FontCache::PHits = 0;
UINT32 FontCache::SHits = 0;
UINT32 FontCache::Misses = 0;
#endif


BOOL FontCache::InitCalled = FALSE; // Set to TRUE on entry to Init function 

DECLARE_SOURCE("$Revision$")

// Declare nodes dynamic
CC_IMPLEMENT_DYNAMIC(PathHandleItem, CCObject)
CC_IMPLEMENT_DYNAMIC(CharDescription, CCObject)


// Riks magic
#define new CAM_DEBUG_NEW

// ------------------------------------------------------------------------------------------
// FontCache methods


/********************************************************************************************

>	static BOOL FontCache::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if we run out of memory.

	Purpose:	Initialises the FontCache. This function must be called before the cache can be
				used. 
	
	Scope:		public			 

	Errors:		If we run out of memory during initialisation then FALSE is returned and
				an error is set.
	SeeAlso:	-

********************************************************************************************/



BOOL FontCache::Init()
{
	InitCalled = TRUE; 

	// Nullify CacheA and CacheB arrays;
	for (INT32 i=0; (i < CACHE_LIST_SIZE); i++)
	{
		CacheA[i] = CacheB[i] = NULL; 
	}

	// initially CacheA will be used as the primary cache
	PrimaryC = CacheA; 
	// and CacheB will be our secondary cache
	SecondaryC = CacheB; 
	// Next PathHandle = 1. We use NULL as a failure return code 
	NextUniquePathHandle = 1;
	// The primary cache is empty
	NumItemsInPrimaryCache = 0;

	// Try to allocate our path cache
	pPathCache = new CamCache(); 
	ERROR1IF(pPathCache == NULL, FALSE, _R(IDE_NOMORE_MEMORY));
	return TRUE; 
}


/********************************************************************************************

>	static void FontCache::DeInit()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-

	Purpose:	Deinitialises the font cache
	
	Scope:		public			 

	SeeAlso:	-

********************************************************************************************/

void FontCache::DeInit()
{
	// Only DeInit if Init was called
	if (InitCalled)
	{
		// Delete all lists

		DeleteCacheLists(PrimaryC);
		DeleteCacheLists(SecondaryC);
		
		// Delete the PathCache if one exists
		if (pPathCache)
		{
			delete (pPathCache);
		}
	}
}



/********************************************************************************************

>	static BOOL FontCache::GetPath(CharDescription& ChDesc, INT32**Points, BYTE**Types, UINT32 *Length)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		ChDesc:	A description of the character you want me to find a path for
				
	Outputs:	Points:	An array of path points
				Types:	An array of path types
				Length: The path length
	
	Returns:	TRUE if we find the requested path
				FALSE if we run out of memory. An error is returned and the cache is left in
				a stable state. i.e GetPath can be called again and will succeed if the right
				amount of memory is available. 
	Purpose:	
	Errors:		-
	Scope:		public
	SeeAlso:	-

********************************************************************************************/

BOOL FontCache::GetPath(CharDescription& ChDesc, INT32**Points, BYTE**Types, UINT32 *Length)
{
	// Obtain a path handle
	UINT32 PathHandle = GetPathHandle(ChDesc); 
	if (PathHandle == NULL)
	{
		// This situation can only arise if a new path handle had to be allocated
		// and we ran out of memory
		return FALSE; // Out of memory, the error will have been set
	}

	// Obtain the path associated with the path handle
	// ChDesc will only be required if the character does not exist in the cache
	// If the function runs out of memory then an error will be set.
	BOOL ok = GetPathFromPathCache(PathHandle, ChDesc, Points, Types, Length);
	
	return ok; 
}


/********************************************************************************************

>	static UINT32 FontCache::GetPathHandle(CharDescription& ChDesc)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		ChDesc:	Specifies the character to obtain the path handle for.
	Outputs:	-
	Returns:	A path handle, or NULL if we are out of memory. Error is set in this situation
				and the cache is left in a stable state.

	Purpose:	Obtains a unique path handle for ChDesc
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/


UINT32 FontCache::GetPathHandle(CharDescription& ChDesc)
{
	// Get our hash key
	UINT32 Key = GetHashKey(ChDesc);

	// First let’s see if the the path handle for the char can be found in the Primary cache
	PathHandleItem* pPathHItem; 
	pPathHItem = GetPathHandleItem(PrimaryC[Key], ChDesc); 
	if (pPathHItem == NULL)
	{
		// OK the path handle is not in the primary cache
		// let’s try the secondary
		pPathHItem = GetPathHandleItem(SecondaryC[Key], ChDesc);
		if (pPathHItem == NULL)
		{
			//TRACE( _T("Miss\n"));
			//Misses++;
			// It’s not here either, try to create a new handle
			// error is set if we fail.
			pPathHItem = AllocatePathHandle(Key, ChDesc);
			// Have we run out of memory ?
			if (pPathHItem == NULL)
			{
				return NULL;
			}
		}
		else
		{
			//TRACE( _T("Hit - secondary\n"));
			//SHits++; 

			// We move the PathHandleItem to the primary cache, where
			// it will have a longer life.

			// Unlink pPathHItem from secondary cache, otherwise TidyCache may delete it
			pPathHItem->RemoveItemFromList(&(SecondaryC[Key])); 

			// Tidy cache detects if the primary cache is full, if it is then we swap the primary and
			// secondary caches. Then clear the primary cache (the old secondary).
			TidyCache();
			
			pPathHItem->Next = PrimaryC[Key];		   // NULL or old head
			PrimaryC[Key] = pPathHItem; 			   // Move Item to head of list
			
			NumItemsInPrimaryCache++; 
		}
	}
//	else
//	{
		//TRACE( _T("Hit - primary\n"));
		//PHits++; 
//	}
	return pPathHItem->PathHandle; 
}

/********************************************************************************************

>	static PathHandleItem* FontCache::GetPathHandleItem(PathHandleItem* ListHead, 
														CharDescription&  ChDesc)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		ListHead: A pointer to the head of the list  
				ChDesc:	  The ChDesc to find
	Outputs:	-
	Returns:	The PathHandleItem if found, otherwise NULL
	Purpose:	Searches cache list for a PathHandleItem associated with ChDesc
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/


PathHandleItem* FontCache::GetPathHandleItem(PathHandleItem* ListHead, 
													CharDescription&  ChDesc)
{
	PathHandleItem* pCurrent = 	ListHead;
	while (pCurrent != NULL)
	{
		if (pCurrent->CharDesc == ChDesc)
		{
			// found
			break;
		}
		pCurrent = pCurrent->Next;
	}
	return pCurrent; 
} 

/********************************************************************************************

>	static PathHandleItem* FontCache::AllocatePathHandle(UINT32 Key, CharDescription& ChDesc)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		Key: The position in the PrimaryC where we should insert the new PathHandleItem
				ChDesc: The character description we should generate the path handle for
	Outputs:	-
	Returns:	The new PathHandleItem or NULL, if we run out of memory. An error is set in this
				situation.
	Purpose:	Allocates a new PathHandleItem for ChDesc and adds this to the primary cache.
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/



PathHandleItem* FontCache::AllocatePathHandle(UINT32 Key, CharDescription& ChDesc)
{
	// Have we run out of path handles, We have 4 Billion to play with ie. 
	// if we allocate 1 every sec we will run out in about 126 years

	if (NextUniquePathHandle == MAX_PATH_HANDLE)
	{
		// We've run out of handles, this calls for drastic action. 
		// First let’s clear our cache lists.

		DeleteCacheLists(PrimaryC);
		DeleteCacheLists(SecondaryC);
		
		// We must also clear the CamCache, there is no function to do this so we will
		// use the destructor.
		
		delete (pPathCache);

		if (!Init())
		{
			// We are left with no path cache. The GetPathFromPathCache fn should cope with this
			// Init will have set the error
			return FALSE; 
		}
	}

	// Try to create the path handle 
	PathHandleItem* pPathHItem = new PathHandleItem(ChDesc, NextUniquePathHandle);
	ERROR1IF(pPathHItem == NULL, NULL, _R(IDE_NOMORE_MEMORY))

	NextUniquePathHandle++;

	// Tidy cache detects if the primary cache is full, if it is then we swap the primary and
	// secondary caches. Then clear the primary cache (the old secondary).

	TidyCache(); 

	// Add our new path handle item to the PrimaryC
	pPathHItem->Next = PrimaryC[Key];		   // NULL or old head
	PrimaryC[Key] = pPathHItem; 			   // Move Item to head of list
	NumItemsInPrimaryCache++;

	return pPathHItem; 
} 

/********************************************************************************************

>	static void FontCache::TidyCache()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The TidyCache routine is called before a new item is added to the primary 
				cache.

				It checks if the PrimaryC is full and if it is performs special processing 
				to keep cache access efficient.	As a result of calling this function the
				contents of the secondary cache may be deleted.


	Errors:		-
	scope:		private
	SeeAlso:	-

********************************************************************************************/

void FontCache::TidyCache()
{
	// Is the primary cache full
	ERROR3IF(NumItemsInPrimaryCache >  MAX_ITEMS_IN_CACHE_LIST, "Too many items in primary cache"); 
	if (NumItemsInPrimaryCache == MAX_ITEMS_IN_CACHE_LIST)
	{
		//TRACE( _T("Cache full\n"));
		// yup ok let’s do the clever bit
		// The secondary cache contains the least frequently used path handles
		// so we delete these
		DeleteCacheLists(SecondaryC); 
			
		// Swap the primary and secondary caches
		PathHandleItem** Temp = PrimaryC;
		PrimaryC = SecondaryC;      // The primary cache is now empty
		SecondaryC = Temp;

	   	// The PrimaryC contains no items
		NumItemsInPrimaryCache = 0;

		
	}
	return;
}
   

/********************************************************************************************

>	BOOL FontCache::GetPathFromPathCache(UINT32 PathHandle,
										 CharDescription& ChDesc, 
									 	 INT32** Points, 
									 	 BYTE** Types, 
									 	 UINT32* Length)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		PathHandle:
	 
	Outputs:	Points:	An array of path points
				Types:	An array of path types
				Length: The path length

	Returns:	FALSE if we run out of memory, in this situation an error is returned.

	Purpose:	Returns back a path for the character described by ChDesc. If the path is
				not found in the cache then the routine tries to generate it.
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

BOOL FontCache::GetPathFromPathCache(UINT32 PathHandle,
								 CharDescription& ChDesc, 
								 INT32** Points, 
								 BYTE** Types, 
								 UINT32* Length 
								 )

{  
	// The first thing to check is that a PathCache exists
	if (pPathCache == NULL)
	{
		// There is no path cache. This is possible, it may have been deleted when we ran out
		// of memory before.

		// try to allocate one
		pPathCache = new CamCache(); 
		ERROR1IF(pPathCache == NULL, FALSE, _R(IDE_NOMORE_MEMORY));

	}
	if ( !pPathCache->FindPath( PathHandle, *Points, *Types, *Length ))
	{
		// Call the GetPath routine which will generate a path from the 
		// ChDesc. 
		/*
		// Waiting for GeneratePath ?

		if (GeneratePath( ChDesc, Points,Types,Length ))
		{
			PathCache->AddPath( PathHandle, *Points, *Types, *Length );
		}
			
		else return FALSE; 	
		*/
		//TRACE( _T("Path not found")); 
	}
	return TRUE; 
}


/********************************************************************************************

>	static void FontCache::DeleteCacheLists(PathHandleItem** pCacheArray)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		pCacheArray: The array to delete lists for
	Outputs:	-
	Returns:	-
	Purpose:	Deletes all lists for the specified cache array. All Items in the array
				are left nullified.
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/



void FontCache::DeleteCacheLists(PathHandleItem** pCacheList)
{
	PathHandleItem* p;
	PathHandleItem* pNxt; 

	for (INT32 i = 0; i< CACHE_LIST_SIZE; i++)
	{
		p = pCacheList[i];	   // Get head of list
		while ( p != NULL) 	   // while list still contains item
		{
			pNxt = p->Next;	   // hand over hand
			delete p;
			p = pNxt; 	
		}
		pCacheList[i] = NULL;
	}
}


/********************************************************************************************

>	UINT32 FontCache::GetHashKey(CharDescription& ChDesc)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		ChDesc: Used to generate hash key
	Outputs:	-
	Returns:	-
	Purpose:	A hashing function which generates a key from ChDesc. The key is in the range 
			    0..CACHE_LIST_SIZE-1. Taken from compiler dragon book.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


UINT32 FontCache::GetHashKey(CharDescription& ChDesc)
{
	BYTE* p;

	// Obtain the size of the CharDescription structure
	INT32 ChDescSize = sizeof(CharDescription); 

	// perform some magic
	// for each byte, shift the bits of h left 4 positions and add in the byte.If any of the 
	// four high-order bits of h is 1, shift the four bits right 24 positions, exclusive-or them
	// into h, and reset to 0 any of the four bits right 24 positions, exclusive or them into h, and
	// reset to 0 any of the four high-order bits that was 1.

	unsigned h = 0; 
	unsigned g;

	INT32 i = 0; 

	for (p =(BYTE*)(&ChDesc); i < ChDescSize; p++)
	{
		h = (h << 4) + (*p);
		if (g = h & 0xf0000000)
		{
			h = h^ (g >> 24);
			h = h ^ g;
		}
		i++;
	}
	 

	return (h % CACHE_LIST_SIZE); 
}

// ------------------------------------------------------------------------------------------
// CharDescription methods

/********************************************************************************************

>	INT32 CharDescription::operator==(const CharDescription& ChDesc) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95
	Inputs:		ChDesc: The CharDescription to compare with
	Outputs:	-
	Returns:	TRUE if ChDesc == this
	Purpose:	CharDescription == operator
	Errors:		-
	SeeAlso:	-

********************************************************************************************/



INT32 CharDescription::operator==(const CharDescription& ChDesc) 
{
	return ((Bold == ChDesc.Bold) &&
	(Italic == ChDesc.Italic) &&
	(HTypeface == ChDesc.HTypeface)	&&
	(CharCode == ChDesc.CharCode));
}

/********************************************************************************************

>	CharDescription& CharDescription::operator=(CharDescription& ChDesc)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95
	Inputs:		ChDesc:	The CharDescription to assign to this
	Outputs:	-
	Returns:	*this
	Purpose:	CharDescription = operator
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CharDescription& CharDescription::operator=(CharDescription& ChDesc)
{
	Bold = ChDesc.Bold;
	Italic = ChDesc.Italic;
	HTypeface = ChDesc.HTypeface;
	CharCode = ChDesc.CharCode;
	return *this;	
}


// ----------------------------------------------------------------------------------------
// PathHandleItem methods

/********************************************************************************************

>	PathHandleItem::PathHandleItem(CharDescription& ChDesc, UINT32 PathHandle)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		ChDesc: The ChDesc for the PathHandleItem to be generated
				PathHandle: The path handle to associate
	Outputs:	-
	Returns:	-
	Purpose:	Creates a new PathHandleItem with a NULL Next pointer
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


PathHandleItem::PathHandleItem(CharDescription& ChDesc, UINT32 PathHandle)
{
	CharDesc = ChDesc;
	PathHandleItem::PathHandle = PathHandle;
	Next = NULL;		
}

/********************************************************************************************

>	static void PathHandleItem::RemoveItemFromList(PathHandleItem** pHead)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		A pointer to the head of the list on which this item lives
	Outputs:	-
	Returns:	-
	Purpose:	Removes this item from the specified list. An ERROR3 will occur	if the item
				does not live on this list.
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

void PathHandleItem::RemoveItemFromList(PathHandleItem** pHead)
{
	// The list should not be empty
	ERROR3IF(*pHead == NULL, "Trying to remove an item from an empty list");

	// Are we at the head of the list ?
	if (*pHead == this)
	{
		*pHead = Next; // The head of the list points to our next sibling, which may be NULL 
	}
	else
	{
		// Search for the item
		PathHandleItem* Prev = NULL;  // Need to keep track of our previous node
		PathHandleItem* p = *pHead;
		while ( p != NULL)
		{
			if (p == this)
			{
				// weve found ourselves
				ERROR3IF(Prev == NULL, "We should not live at the head of the list"); 
				Prev->Next = Next;
				break;
			}
			Prev = p;
			p = p->Next;
		}
		ERROR3IF(p == NULL, "Cannot remove an item from a list it doesn't live in"); 
	}

	Next = NULL; // We are no longer in the list
}

// Test routines

#ifdef _DEBUG

// Test 1 generates random Character descriptions and tries to add them to the cache
// Used to test all paths through the code.
// It assumes that Init has been called on the cache

void FontCache::Test1()
{
	INT32* p;
	BYTE* t;
	UINT32 l;

	// We will only vary the ChDesc's CharCode. It is important that we hash well on this
	// anyway. 
	CharDescription	ChDesc;
	ChDesc.Height = 10;
	ChDesc.Width = 10;
	ChDesc.Italic = FALSE;

	INT32 n;
	for (n=0;n<10000;n++)
	{
		if (n==255)
		{
			PHits = 0; SHits =0; Misses = 0;
		}
		TRACE( _T("n = %d "),n); 
		// generate a random CharacterDescription to find 0..5
		ChDesc.CharCode = (rand() % 255) + 1;
		// And try to obtain path for Ch
		if (!FontCache::GetPath(ChDesc, &p, &t, &l))
		{
			InformError();
		}
	}
	TRACE( _T("------\n"));
	TRACE( _T("pHits = %lu, SHits = %lu, Miss = %lu\n"), PHits, SHits, Misses); 		
}



#endif
#endif
