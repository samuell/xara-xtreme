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

/*
*/

#include "camtypes.h"
#include "objcache.h"
#include "cache.h"
#include <time.h>
#include <stdlib.h>


DECLARE_SOURCE( "$Revision$" );


CC_IMPLEMENT_DYNCREATE(ObjectCache, CCObject)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


#define MYTRACE if(IsUserName("Olivier")) TRACE

/*********************************************************************************************

>	ObjectCache::ObjectCache()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/97
	Purpose:	default constructor for ObjectCache
	Note:		
	SeeAlso:	
**********************************************************************************************/
ObjectCache::ObjectCache()
{
	// Compute the available amount of memory
	/*TunedMemory* pTuneMem = GetTunedMemManager();
	if (pTuneMem != NULL)
		Ceiling = pTuneMem->GetAvailableTunedMem();*/

	m_NumObjects	= 0;
	m_HandleFactory = 1;
	m_CurrentSize	= 0;
}

/*********************************************************************************************

>	ObjectCache::ObjectCache(UINT32 ceiling)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/97
	Purpose:	constructor for ObjectCache, set the cache size.
	Inputs:		ceiling: the amount of memory available for the cache
	Note:		
	SeeAlso:	
**********************************************************************************************/
ObjectCache::ObjectCache(UINT32 ceiling)
{
	m_Ceiling		= ceiling;
	m_NumObjects	= 0;
	m_HandleFactory = 1;	// we don't start from 0 
	m_CurrentSize	= 0;
}

/*********************************************************************************************

>	ObjectCache::~ObjectCache()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/97
	Purpose:	destructor for ObjectCache
	Note:		
	SeeAlso:	
**********************************************************************************************/
ObjectCache::~ObjectCache()
{}


/*********************************************************************************************

>	void ObjectCache::SetMaxCacheSize(UINT32 NewCeiling)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/97
	Inputs:		NewCeiling: The size of the new ceiling
	Purpose:	dynamically change the size (in bytes) of the Cache.
	Note:		
	SeeAlso:	
**********************************************************************************************/

BOOL ObjectCache::SetMaxCacheSize(UINT32 NewCeiling)
{
//	MYTRACE( _T("ObjectCache::SetMaxCacheSize()\n"));
	while (m_CurrentSize > NewCeiling)
	{
		if(!DeleteObject())
			break;
	}
	m_Ceiling = NewCeiling;
	return TRUE;
}


/*********************************************************************************************

>	CACHE_HANDLE ObjectCache::Add(CachedObject *pObj)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		pObj: the Object to cache
	Purpose:	Add an object in the Hash table of the cache
	Note:		
	SeeAlso:	
**********************************************************************************************/

CACHE_HANDLE ObjectCache::Add(CachedObject *pObj)
{
	ERROR3IF(pObj==NULL, "Error: the object to cache can't be NULL");

	if (pObj == NULL)
		return 0;

//	MYTRACE( _T("ObjectCache::Add()\n"));

	// if we can make room for the object then assign the object a unique handle and cache it
	if (MakeRoomFor(pObj))
	{
		CACHE_HANDLE hObj = m_HandleFactory++;

		pObj->StoreHandle(hObj);		// store the CachedObject Handle
		m_NumObjects++;					// Increase the number of objects
		m_CurrentSize += pObj->GetSize();	// Increase the space used
		m_ObjectMap[hObj] = pObj;	// set the object in the hash table
		return hObj;
	}

	return 0;
}


/*********************************************************************************************

>	BOOL ObjectCache::MakeRoomFor(CachedObject* pObj)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		pObj: The object to add in the cache
	Returns:	TRUE if succesful, FALSE otherwise
	Purpose:	make room for another object by deleting old objects if the cache is full
	Note:		
	SeeAlso:	
**********************************************************************************************/

BOOL ObjectCache::MakeRoomFor(CachedObject* pObj)
{
	ERROR3IF(pObj==NULL, "Error: the object to cache can't be NULL");

	if (pObj == NULL)
		return FALSE;

	// is there room in the cache or do we have to free some memory ?
	if (pObj->GetSize() + m_CurrentSize <= m_Ceiling)
		return TRUE;	// enough room

	// is the cache big enough ? 
	if (pObj->GetSize() > m_Ceiling)
		return FALSE;	// the object is bigger than the cache size

	// Scan the cache deleting objects until there is room
	BOOL EnoughRoom = (pObj->GetSize() + m_CurrentSize <= m_Ceiling);
	do
	{
		BOOL ObjectDeleted = DeleteObject();

		EnoughRoom = (pObj->GetSize() + m_CurrentSize <= m_Ceiling);

		if(!ObjectDeleted && !EnoughRoom)
			return FALSE;		// no room and no deleted objects 
	}
	while (!EnoughRoom);

	return TRUE;
}


/*********************************************************************************************

>	CachedObject *ObjectCache::LookUp(CACHE_HANDLE hObj)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Purpose:	LookUp in the hash table
	Returns:	return a pointer to the object found, NULL otherwise
	Note:		
	SeeAlso:	
**********************************************************************************************/

CachedObject *ObjectCache::LookUp(CACHE_HANDLE hObj)
{
//	void* pObjMap = NULL;

	std::map<WORD, PVOID>::iterator iter = m_ObjectMap.find( hObj );
	if( m_ObjectMap.end() != iter  )
		return (CachedObject *)iter->second;

	return NULL;
}


/*********************************************************************************************

>	BOOL ObjectCache::Remove(CACHE_HANDLE hObj)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		hObj: the object handle
	Returns:	TRUE if success, FALSE otherwise
	Purpose:	Remove a key from the hash table
	Note:		
	SeeAlso:	
**********************************************************************************************/

BOOL ObjectCache::Remove(CACHE_HANDLE hObj)
{
	if( m_ObjectMap.erase( hObj ) )
		return TRUE;

	return FALSE;
}


/*********************************************************************************************

>	BOOL ObjectCache::DeleteObject()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Purpose:	virtual function. This is the base class delete function. All algorithms
				are implemented in the derived classes such as ObjectCacheRandom...etc.
	Note:		
	SeeAlso:	
**********************************************************************************************/

BOOL ObjectCache::DeleteObject()
{
	return TRUE;
}


