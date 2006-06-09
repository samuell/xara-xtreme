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

// CSampleList and CSampleItem implementation

#include "camtypes.h"
#include "samplist.h"
#include "brshcomp.h"
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "pen.h"

CC_IMPLEMENT_DYNAMIC(CSampleItem, CCObject);
CC_IMPLEMENT_DYNAMIC(CSampleData, CCObject);
CC_IMPLEMENT_DYNAMIC(CDistanceSampler, CSampleData);

const double MIN_COLLECTION_SAMPLE_RATE = 1.0;
const double MAX_COLLECTION_SAMPLE_RATE = 1.0;
const double MIN_RETRIEVAL_SAMPLE_RATE = 1.0;
const double MAX_RETRIEVAL_SAMPLE_RATE = 10000.0;

/*-------------------------------------------------------------------------------------------
------------------------------CSampleItem implementation-------------------------------------
---------------------------------------------------------------------------------------------
*/

/********************************************************************************************

>	CSampleItem::CSampleItem()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	this object
	Purpose:	default constructor
				
	Notes:		
********************************************************************************************/

CSampleItem::CSampleItem()
{
	m_Distance = 0;
	m_Coord.x = m_Coord.y = 0;
	m_Pressure = 0;
}


/********************************************************************************************

>	CSampleItem::CSampleItem()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Coord - the position of this sample
				Distance - the distance along the path of this sample
	Outputs:	-
	Returns:	this object
	Purpose:	constructor with initialisation
				
	Notes:		
********************************************************************************************/

CSampleItem::CSampleItem(DocCoord Coord, MILLIPOINT Distance, UINT32 Pressure)
{
	m_Coord = Coord;
	m_Distance = Distance;
	m_Pressure = Pressure;

}


/********************************************************************************************

>	CSampleItem::CSampleItem()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Other - another CSampleItem
	Outputs:	-
	Returns:	this object
	Purpose:	copy constructor
				
	Notes:		
********************************************************************************************/

CSampleItem::CSampleItem(const CSampleItem& Other)
{
	m_Distance = Other.m_Distance;
	m_Coord	   = Other.m_Coord;
	m_Pressure = Other.m_Pressure;

}



/********************************************************************************************

>	CSampleItem CSampleItem::operator =(CSampleItem& Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Other - another CSampleItem
	Outputs:	-
	Returns:	this object
	Purpose:	assignment operator
				
	Notes:		
********************************************************************************************/

CSampleItem CSampleItem::operator =(const CSampleItem& Other)
{
	m_Coord	   = Other.m_Coord;
	m_Distance = Other.m_Distance;
	m_Pressure = Other.m_Pressure;
	return *this;
}



/********************************************************************************************

>	BOOL CSampleItem::operator ==(CSampleItem& Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Other - another CSampleItem
	Outputs:	-
	Returns:	TRUE if the objects are identical, FALSE if they are not
	Purpose:	equality operator
				
	Notes:		
********************************************************************************************/

BOOL CSampleItem::operator ==(const CSampleItem& Other)
{
	BOOL bDiff =  (m_Coord != Other.m_Coord || 
				   m_Distance != Other.m_Distance ||
				   m_Pressure != Other.m_Pressure);
	return bDiff;
}

/********************************************************************************************

>	BOOL CSampleItem::operator ==(CSampleItem& Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Other - another CSampleItem
	Outputs:	-
	Returns:	TRUE if the objects are identical, FALSE if they are not
	Purpose:	equality operator
				
	Notes:		
********************************************************************************************/

CSampleItem CSampleItem::InterpolateItems(CSampleItem& Other, double Proportion)
{
	CSampleItem NewItem;

	if (Proportion < 0.0 || Proportion > 1.0)
	{
		ERROR3("Invalid proportion value in CSampleItem::InterpolateItems");
		return NewItem;
	}

	NewItem.m_Distance = (INT32)(((double)m_Distance * Proportion) + ((double)Other.m_Distance * (1-Proportion)));
	MILLIPOINT XVal = (INT32)((m_Coord.x * Proportion) + (Other.m_Coord.x * (1 - Proportion)));
	MILLIPOINT YVal = (INT32)((m_Coord.y * Proportion) + (Other.m_Coord.y * (1 - Proportion)));
	
	NewItem.m_Coord.x = XVal;
	NewItem.m_Coord.y = YVal;
	
	NewItem.m_Pressure = (UINT32)((m_Pressure * Proportion) + (Other.m_Pressure * (1-Proportion)));

	return NewItem;
}

/********************************************************************************************


>	BOOL CSampleItem::WriteNative(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		pRecord - the record to write to
	Returns:	TRUE if the Node has written out a record to the filter
	Purpose:	Writes this sampledata item to a record

	SeeAlso:	CSampleData::WriteNative

********************************************************************************************/

BOOL CSampleItem::WriteNative(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL in CSampleItem::WriteNative");
	
	BOOL ok = pRecord->WriteINT32((INT32)m_Pressure);
	if (ok) ok = pRecord->WriteCoord(m_Coord);
	if (ok) ok = pRecord->WriteINT32(m_Distance);

	return ok;
}

/*-------------------------------------------------------------------------------------------
------------------------------CSampleData implementation-------------------------------------
---------------------------------------------------------------------------------------------
*/

/********************************************************************************************

>	CSampleData::CSampleData()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	this object
	Purpose:	default constructor
				
	Notes:		
********************************************************************************************/

CSampleData::CSampleData()
{
	m_pSampleData = NULL;
	m_CollectionSampleRate = 1.0;
	m_RetrievalSampleRate  = 1.0;
	m_CollectionInternalIndex = 0;
	m_RetrievalInternalIndex  = 0;
	m_NumItems				  = 0;
	m_MaxPressure			  = 0;
	m_ElementSize = (UINT32)sizeof(CSampleItem);
}


/********************************************************************************************

>	CSampleData::~CSampleData()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor, cleans out the list
				
	Notes:		
********************************************************************************************/

CSampleData::~CSampleData()
{
	if (m_pSampleData != NULL)
	{
		m_pSampleData->clear();
		delete m_pSampleData;
		m_pSampleData = NULL;
	}
}


/********************************************************************************************

>	CSampleData* CSampleData::MakeCopy(CSampleData* pOther)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	a copy of this object, unless something goes wrong (i.e memory) in which case NULL
	Purpose:	Makes a copy of this sample data.
				
	Notes:		
********************************************************************************************/

CSampleData* CSampleData::MakeCopy()
{
	CSampleData* pNewData = new CSampleData;

	if (pNewData == NULL)
		return NULL;

	if (!pNewData->InitialiseData(m_NumItems))
	{
		delete pNewData;
		return NULL;
	}

	m_RetrievalSampleRate = 1; // make sure we get all the items

	CSampleItem TheItem;

	// unfortunately my implementation is a bit clumsy so we have to do the first item
	// by hand
	BOOL	Getok = GetAt(0, &TheItem);
	BOOL	Setok = pNewData->SetAt(0, TheItem);
	//TRACEUSER( "Diccon", _T("Copying pressure %d\n"), TheItem.m_Pressure);
	if (Getok == FALSE || Setok ==  FALSE)
		ERROR3("Failed to get or set first item in CSampleData::MakeCopy");

	UINT32 Counter = 1;
	// now we can loop through the rest
	while (Getok && Setok)
	{
		Getok = GetNext(&TheItem);
		Setok = pNewData->SetNext(TheItem);
		//TRACEUSER( "Diccon", _T("Copying pressure %d\n"), TheItem.m_Pressure);
		Counter++;
	}

	if (Counter != m_NumItems)
		TRACEUSER( "Diccon", _T("CSampleData::MakeCopy - Num items copied = %d, Actual num items = %d\n"), Counter, m_NumItems);

	return pNewData;
}

/********************************************************************************************

>	BOOL CSampleData::InitialiseData(UINT32 Size, UINT32 GrowBy)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if we initialised ok, FALSE if we didn't get the memory
	Purpose:	creates the list member variable
				
	Notes:		
********************************************************************************************/

BOOL CSampleData::InitialiseData( size_t Size, UINT32 GrowBy )
{
	// if we've already got a list then delete it
	if (m_pSampleData != NULL)
	{
		m_pSampleData->clear();
		delete m_pSampleData;
		m_pSampleData = NULL;
	}
	m_pSampleData = new SampleArray;

	if (m_pSampleData == NULL)
		return FALSE;

	if( Size < m_pSampleData->size() + GrowBy )
		Size = m_pSampleData->size() + GrowBy;

	m_pSampleData->resize( Size );

	return TRUE;
}


/********************************************************************************************

>	BOOL CSampleData::CollectData(DocCoord Coord, UINT32 Pressure)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Coord - the coordinate data
				Pressure - the pressure data
	Outputs:	-
	Returns:	always TRUE in the base class
	Purpose:	This is the function you should override in you derived classes to do the sampling
				with.  For an example see CDistanceSampler::CollectData
				
	Notes:		
********************************************************************************************/

BOOL CSampleData::CollectData(DocCoord Coord, UINT32 Pressure)
{
	return TRUE;
}

/*-------------------------------------------------------------------------------------------
-------------------------Array access functions-----------------------------------------------
--------------------------------------------------------------------------------------------*/


/********************************************************************************************

>	void CSampleData::Add(CSampleItem& Item)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Item - the item to insert
	Outputs:	-
	Returns:	-
	Purpose:	adds a new item to the end of the array
				
	Notes:		
********************************************************************************************/

void CSampleData::Add(CSampleItem& Item)
{
	if (m_pSampleData != NULL)
		m_pSampleData->push_back(Item);
	m_NumItems++;

}

/********************************************************************************************

>	BOOL CSampleData::SetAt(UINT32 Index CSampleItem& Item)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Index - the position to set Item at
				Item - the item to insert
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	inserts Item at Index
				
	Notes:		Currently it is not permitted to Set, Remove or Insert items if the sample rate is not 1.
				This is because a) I really don't want people changing data in the array once
				it has been collected (we're sampling remember) and b) Its a pain in the ass
********************************************************************************************/

BOOL CSampleData::SetAt(UINT32 Index, CSampleItem& Item)
{
	BOOL Retval = FALSE;
	if (m_pSampleData != NULL)
	{
		// we are not allowed to be setting data when the sample rate is not 1.0
		if (m_CollectionSampleRate < MIN_COLLECTION_SAMPLE_RATE || 
			m_CollectionSampleRate > MAX_COLLECTION_SAMPLE_RATE)
		{
			ERROR3("Attempting to set data when SampleRate != 1");
			return FALSE;
		}
		if (SetInternalCollectionIndex(Index))
		{
			(*m_pSampleData)[size_t(m_CollectionInternalIndex)] = Item;	
			Retval = TRUE;
			if ((UINT32)m_CollectionInternalIndex > m_NumItems)
				m_NumItems = (UINT32)m_CollectionInternalIndex;
		}
		else
			ERROR3("Attempting to set over the end of the array in CSampleData::SetAt");
	}
	return Retval;
}

/********************************************************************************************

>	BOOL CSampleData::SetNext(CSampleItem& Item, BOOL Grow)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Item - the item to insert
				Grow - do we wish to grow the array if we hit the end (defaults to FALSE)
	Outputs:	-
	Returns:	TRUE, unless we hit the end of the array and do not widh to grow
	Purpose:	Increments our internal index and then inserts a new item at that position.
				If we hit the end of the array then we will add an element, so long as our flag
				is set
	Notes:		
********************************************************************************************/

BOOL CSampleData::SetNext(CSampleItem& Item, BOOL Grow)
{
	BOOL Retval = FALSE;
	if (m_pSampleData != NULL)
	{
		// we are not allowed to be setting data when the sample rate is not 1.0
		if (m_CollectionSampleRate < MIN_COLLECTION_SAMPLE_RATE || 
			m_CollectionSampleRate > MAX_COLLECTION_SAMPLE_RATE)
		{
			ERROR3("Attempting to set data when SampleRate != 1");
			return FALSE;
		}
		if (IncrementInternalIndex(&m_CollectionInternalIndex))
		{
			// if internal index == -1 after incrementing it means we hit the end of the array
			if (m_CollectionInternalIndex != -1.0)
			{
				//TRACEUSER( "Diccon", _T("Setting at %f\n"), m_CollectionInternalIndex);
				(*m_pSampleData)[size_t(m_CollectionInternalIndex)] = Item;
				Retval = TRUE;
				if ((UINT32)m_CollectionInternalIndex > m_NumItems)
					m_NumItems = (UINT32)m_CollectionInternalIndex;
			}
			else
			{
				// we must have hit the end of the array, so we need to add
				if (Grow == TRUE)
				{
					m_pSampleData->push_back( Item );
					Retval = TRUE;
					m_NumItems++;
					m_CollectionInternalIndex = m_NumItems;
				}
			}
		}
		else
			ERROR3("Failed to increment index in CSampleData::SetNext");
	}	

	return Retval;
}

/********************************************************************************************

>	BOOL CSampleData::SetPrev(CSampleItem& Item)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Item - the item to insert
	Outputs:	-
	Returns:	TRUE if successful, FALSE if we're already at the head of the array
	Purpose:	Decrements our internal index and then inserts Item into the array.
				
	Notes:		
********************************************************************************************/

BOOL CSampleData::SetPrev(CSampleItem& Item)
{
	BOOL Retval = FALSE;
	if (m_pSampleData != NULL)
	{
		// we are not allowed to be setting data when the sample rate is not 1.0
		if (m_CollectionSampleRate < MIN_COLLECTION_SAMPLE_RATE || 
			m_CollectionSampleRate > MAX_COLLECTION_SAMPLE_RATE)
		{
			ERROR3("Attempting to set data when SampleRate != 1");
			return Retval;
		}
		if (DecrementInternalIndex(&m_CollectionInternalIndex))
		{
			if (m_CollectionInternalIndex != -1.0)
			{
				(*m_pSampleData)[size_t(m_CollectionInternalIndex)] = Item;
				Retval = TRUE;
			}
			else
				ERROR3("Attempting to SetPrev over the start of the array in CSampleData::SetPrev");
		}
			ERROR3("Unable to decrement index in CSampleData::SetPrev");
	}	
	return Retval;
}

/********************************************************************************************

>	BOOL CSampleData::RemoveAt(UINT32 Index)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Index - the position to remove from
	Outputs:	-
	Returns:	TRUE, unless we go over the end of the array
	Purpose:	Removes item at Index
				
	Notes:		
********************************************************************************************/

BOOL CSampleData::RemoveAt(UINT32 Index)
{
	BOOL Retval = FALSE;
	if (m_pSampleData != NULL)
	{
		// we are not allowed to be setting data when the sample rate is not 1.0
		if (m_CollectionSampleRate < MIN_COLLECTION_SAMPLE_RATE || 
			m_CollectionSampleRate > MAX_COLLECTION_SAMPLE_RATE)
		{
			ERROR3("Attempting to remove data when SampleRate != 1");
			return FALSE;
		}
		if (SetInternalRetrievalIndex(Index))
		{
			m_pSampleData->erase( m_pSampleData->begin() + size_t(m_RetrievalInternalIndex) );
			Retval = TRUE;
			m_NumItems--;
		}
		else
			ERROR3("Attempting to remove beyond the end of the array in CSampleData::RemoveAt");
	}
	return Retval;
}




/********************************************************************************************

>	BOOL CSampleData::GetAt(UINT32 Index, CSampleItem* pItem)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Index - the index to retrieve from
	Outputs:	pItem - the item at index
	Returns:	TRUE if successful, FALSE if Index is invalid or there is no array
	Purpose:	Retrieves the item at Index
				
	Notes:		All the access functions set the m_ListPosition member variable which records
				the last list positon accessed.  This means that you can the use GetNext or
				GetPrev to retrieve subsequent elements, which is much quicker than using
				GetAt all the time.
********************************************************************************************/

BOOL CSampleData::GetAt(UINT32 Index, CSampleItem* pItem)
{
	ERROR2IF(pItem == NULL, FALSE, "Item pointer is NULL in CSampleData::GetAt");

	if (m_pSampleData != NULL)
	{
		if (SetInternalRetrievalIndex(Index))
		{
			//TRACEUSER( "Diccon", _T("Get At: "));
			CSampleItem TheItem;
			if (GetItemAtInternalIndex(m_RetrievalInternalIndex, &TheItem))
				*pItem = TheItem;
			return TRUE;
		}
		else
			ERROR3("Trying to access element over the end of the array in CSampleData::GetAt"); 
	}
	return FALSE;
}

BOOL CSampleData::GetAtPos(double Index, CSampleItem* pItem)
{
	ERROR2IF(pItem == NULL, FALSE, "Item pointer is NULL in CSampleData::GetAt");

	if (m_pSampleData != NULL)
	{
		//if (SetInternalRetrievalIndex(Index))
		{
			//TRACEUSER( "Diccon", _T("Get At: "));

			UINT32 rounded = (UINT32) Index;

			CSampleItem TheItem;
			if (GetItemAtInternalIndex((double) rounded, &TheItem))
				*pItem = TheItem;
			return TRUE;
		}
		//else
		//	ERROR3("Trying to access element over the end of the array in CSampleData::GetAt"); 
	}
	return FALSE;
}




/********************************************************************************************

>	BOOL CSampleData::GetNext(CSampleItem* pItem)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	pItem - the tail item
	Returns:	TRUE if successful, FALSE if we are at the end of the array or it doesn't exist
	Purpose:	Increments the internal index, then retrieves the item at that index
				
	Notes:		It is recommended that you do a GetAt before you start doing GetNexts, otherwise
				I won't vouch for the position of the value of the internal index
********************************************************************************************/

BOOL CSampleData::GetNext(CSampleItem* pItem)
{
	BOOL Retval = FALSE;
	if (m_pSampleData != NULL)
	{
		//TRACEUSER( "Diccon", _T("Get Next: "));
		if (IncrementInternalIndex(&m_RetrievalInternalIndex))
		{
			// BODGE ALERT! if the retrieval index is -1 then this indicates that we
			// have reached the end of the array! However I am letting it pass on, 
			// and have inserted more bodge code at GetItemAtInternalIndex to return
			// the last item.
			
		//	if (m_RetrievalInternalIndex == -1.0)
		//		ERROR3("Invalid index in CSampleData::GetNext");

			{
				if (GetItemAtInternalIndex(m_RetrievalInternalIndex, pItem))
					Retval = TRUE;
			}
		}
		else
			ERROR3("Unable to increment index in CSampleData::GetNext");
	}

	return Retval;		
}

/********************************************************************************************

>	BOOL CSampleData::GetPrev(CSampleItem* pItem)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	pItem - the previous item in the array
	Returns:	TRUE if successful, FALSE if we are at the start of the array or it doesn't exist
	Purpose:	Decrements our internal index and retrieves the item at that position
				
	Notes:		Really only works if the internal index has been set by a previous function such
				as GetAt, SetAt, etc.
********************************************************************************************/

BOOL CSampleData::GetPrev(CSampleItem* pItem)
{
	BOOL Retval = FALSE;
	if (m_pSampleData != NULL)
	{
		if (DecrementInternalIndex(&m_RetrievalInternalIndex))
		{
			if (m_RetrievalInternalIndex != -1.0)
			{
				if (GetItemAtInternalIndex(m_RetrievalInternalIndex, pItem))
					Retval = TRUE;
			}	
		}
		else
			ERROR3("Unable to decrement index in CSampleData::GetPrev");
	}
	return Retval;
}

/********************************************************************************************

>	INT32 CSampleData::GetCount()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	the number of items in the array, or -1 if the array does not exist
	Purpose:	To find the number of items in the array
				
********************************************************************************************/

INT32 CSampleData::GetCount()
{
	if (m_pSampleData != NULL)
		return (INT32)m_pSampleData->size();

	return -1;
}

/********************************************************************************************

>	INT32 CSampleData::FreeExtra()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Asks the array to free any unused memory above its current upper bound.  Recommneded
				that you call this if you do a big SetSize and don't use it all
				
********************************************************************************************/

void CSampleData::FreeExtra()
{
	if (m_pSampleData != NULL)
		m_pSampleData->resize( (INT32)m_NumItems );
}

/********************************************************************************************

>	BOOL CSampleData::InsertNewData(UINT32 Index, CSampleData* pData)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Index - the index to insert at
				pData - array of items to insert
	Outputs:	-
	Returns:	TRUE if successful
	Purpose:	To insert pData into our data array at Index.  This function does the shuffling
				of existing array elements in order to insert the new data
				
********************************************************************************************/

BOOL CSampleData::InsertNewData(UINT32 Index, CSampleData* pData)
{
	ERROR2IF(pData == NULL, FALSE, "Attempting to insert null array in CSampleData::InsertNewData");
	ERROR2IF(m_pSampleData == NULL, FALSE, "Member data array is NULL in CSampleData::InsertNewData");

	if (Index > m_pSampleData->size() - 1)
	{
		ERROR3("Attempting to insert beyond end of array in CSampleData::InsertNewData");
		return FALSE;
	}

	// find out how many elements we are inserting
	INT32 NumNewItems = (INT32)pData->GetNumItems();
	if (NumNewItems <= 0)
	{
		ERROR3("Attempting to insert zero or fewer elements in BOOL CSampleData::InsertNewData");
		return FALSE;
	}

	// Set our sample rates to 1.0 so that we don't skip any elements
	m_CollectionSampleRate = 1.0;
	m_RetrievalSampleRate  = 1.0;
	pData->SetRetrievalSampleRate(1.0);
	// work out how many elements we have to move
	size_t NumMoveItems = m_pSampleData->size() - 1 - Index;

		// this keeps track of where we're moving from
	UINT32 MoveIndex = (UINT32)m_pSampleData->size() - 1;
	m_NumItems = m_pSampleData->size() + NumNewItems;
	// extend our array so we have room for the new data
	m_pSampleData->resize((INT32)m_NumItems);

	// start at the tail and work down, shifting existing elements up to make room
	CSampleItem TheItem;
	BOOL KeepGoing = GetAt(MoveIndex, &TheItem);
//	TRACEUSER( "Diccon", _T("SHIFTING %d ITEMS UP FROM %d TO %d\n"), NumMoveItems, MoveIndex-NumMoveItems, MoveIndex-NumMoveItems + NumNewItems);
	while (NumMoveItems && KeepGoing)
	{
		// copy the item to its place further up in the array
		(*m_pSampleData)[MoveIndex + NumNewItems] = TheItem;
		// get the next item
		KeepGoing = GetPrev(&TheItem);
		MoveIndex--;
		NumMoveItems--;
	}
//	TRACEUSER( "Diccon", _T("FINISHED SHIFTING UP\n"));
	// now we can insert our new data at Index

	// get the first item and set it, so we can use GetNext for each array
	KeepGoing = pData->GetAt(0, &TheItem);
	if (KeepGoing)
		SetAt(Index, TheItem);
	UINT32 Counter = 0;
//	TRACEUSER( "Diccon", _T("INSERTING NEW DATA\n"));
	while (NumNewItems && KeepGoing)
	{
		if (KeepGoing) KeepGoing = pData->GetNext(&TheItem);
		if (KeepGoing)  SetNext(TheItem);
		//TRACEUSER( "Diccon", _T("Inserting item %d\n"), TheItem.m_Pressure);
		NumNewItems--;
		Counter++;
	}
	
	//TRACEUSER( "Diccon", _T("Added %d items at index %d\n"), Counter, Index);
	// hopefully that should have done the trick!

	return TRUE;
}

/********************************************************************************************

>	BOOL CSampleData::RemoveData(UINT32 StartIndex, UINT32 EndIndex, SampleArray* pArray)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		StartIndex - the index to start removing from
				EndIndex - the index to stop removing at
	Outputs:	pArray (optional) - an array to store the removed elements in
	Returns:	TRUE if successful
	Purpose:	To remove all the items between StartIndex and EndIndex, and optionally record
				them in pArray
				
********************************************************************************************/

BOOL CSampleData::RemoveData(UINT32 StartIndex, UINT32 EndIndex, SampleArray* pArray)
{
	// do some checks
	ERROR2IF(m_pSampleData == NULL, FALSE, "Attempting to remove from an array that doesn't exist in CSampleData::RemoveData");
	ERROR2IF(EndIndex <= StartIndex, FALSE, "Invalid indexes in CSampleData::RemoveData");

	UINT32 UpperBound = (UINT32)m_pSampleData->size() - 1;
	if (StartIndex > UpperBound || EndIndex > UpperBound)
	{
		ERROR3("Attempting to remove items beyond the bounds of the array in CSampleData::RemoveData");
		return FALSE;
	}
	// Set our samples rate to 1.0 so that we don't skip any elements
	m_CollectionSampleRate = 1.0;
	m_RetrievalSampleRate  = 1.0;
	
	// find out how many we're dealing with
	UINT32 NumRemoveItems = EndIndex - StartIndex;
	
//	TRACEUSER( "Diccon", _T("COPYING ITEMS TO BE REMOVED\n"));
	// first copy the removed items to the new array (if relevant)
	if (pArray != NULL)
	{
		pArray->resize( NumRemoveItems );
		CSampleItem TheItem;
		for (UINT32 i = 0; i < NumRemoveItems; i++)
		{
			TheItem = (*m_pSampleData)[i + StartIndex];
			(*pArray)[i] = TheItem;
		//	TRACEUSER( "Diccon", _T("Removing data %d\n"), TheItem.m_Pressure);
		}
	}
	
	// now copy all elements beyond EndIndex back to StartIndex
	UINT32 NumMoveItems = UpperBound - EndIndex;
	UINT32 NumItemsMoved = 0;
	CSampleItem TheItem;
	// EndIndex +1 ??
//	UINT32 Index = StartIndex;
//	TRACEUSER( "Diccon", _T("SHIFTING ITEMS DOWN FROM %d TO %d\n"), EndIndex, Index);
//	TRACEUSER( "Diccon", _T("Upper bound = %d\n"), UpperBound);
	BOOL ok = GetAt(EndIndex, &TheItem);
	INT32 RemoveCount = NumRemoveItems;
	while ((NumItemsMoved <= NumMoveItems) && ok)
	{
		(*m_pSampleData)[StartIndex++] = TheItem;
		ok = GetNext(&TheItem);
		NumItemsMoved++;
		if (RemoveCount > 0)
		{
			RemoveCount--;
			m_NumItems--; // decrement the number of items
		}
	}
	
//	TRACEUSER( "Diccon", _T("Removed %d items\n"), EndIndex - Index);
//	TRACEUSER( "Diccon", _T("Moved %d items from index %d to index %d\n"), NumItemsMoved, EndIndex, Index); 
	// shrink the array to the new size
	m_pSampleData->resize( m_NumItems );

	return TRUE;
}


/********************************************************************************************

>	BOOL CSampleData::SetInternalCollectionIndex(UINT32 IndexRequested)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		IndexRequested - the requested index to set the array pointer to
	Outputs:	-
	Returns:	TRUE, unless index converts to an internal index that is out of bounds
	Purpose:	Sets our collection internal array index to the position corresponding to the 
				requested index
				
********************************************************************************************/

BOOL CSampleData::SetInternalCollectionIndex(UINT32 Index)
{
	ERROR2IF(m_pSampleData == NULL, FALSE, "Sample data is null in CSampleData::SetInternalIndex");

	BOOL Retval = FALSE;

	double TempIndex = m_CollectionSampleRate * Index;
	if (TempIndex <= (double)m_pSampleData->size() - 1 && TempIndex >= 0)
	{
		m_CollectionInternalIndex = TempIndex;
		Retval = TRUE;
	}
	else
		ERROR3("Attempting to set internal index beyond the end of the array in CSampleData::SetInternalIndex"); 

	return Retval;
}


/********************************************************************************************

>	BOOL CSampleData::SetInternalRetrievalIndex(UINT32 IndexRequested)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		IndexRequested - the requested index to set the array pointer to
	Outputs:	-
	Returns:	TRUE, unless index converts to an internal index that is out of bounds
	Purpose:	Sets our Retrieval internal array index to the position corresponding to the 
				requested index
				
********************************************************************************************/

BOOL CSampleData::SetInternalRetrievalIndex(UINT32 Index)
{
	ERROR2IF(m_pSampleData == NULL, FALSE, "Sample data is null in CSampleData::SetInternalIndex");

	BOOL Retval = FALSE;

	double TempIndex = m_RetrievalSampleRate * Index;
	if (TempIndex <= (double)m_pSampleData->size() - 1)
	{
		m_RetrievalInternalIndex = TempIndex;
		Retval = TRUE;
	}
	else
		ERROR3("Attempting to set internal index beyond the end of the array in CSampleData::SetInternalIndex"); 

	return Retval;
}


/********************************************************************************************

>	BOOL CSampleData::IncrementInternalIndex(double* pCurrentIndex)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		pCurrentIndex - the index to be incremented, this should really only be either
				the collection index or the retrieval index
	Outputs:	-
	Returns:	TRUE, unless something goes wrong
	Purpose:	Increments our internal index counter, unless we have reached
				the end of the array in which case we set the counter to -1
				
********************************************************************************************/

BOOL CSampleData::IncrementInternalIndex(double* pCurrentIndex)
{
	// if we've already overrun then don't go any further
	if (*pCurrentIndex == -1)
		return TRUE;

	// not allowed to use indexes that aren't one of our members, also 
	// find out which sample rate to use here
	double SampleRate = 1.0;
	if (pCurrentIndex == &m_CollectionInternalIndex)
	{
		SampleRate = m_CollectionSampleRate;
	}
	else if (pCurrentIndex == &m_RetrievalInternalIndex)
	{
		SampleRate = m_RetrievalSampleRate;
	}
	else
	{
		ERROR3("Attempting to use non-member index in CSampleData::IncrementInternalIndex");
		return FALSE;
	}

	// we need to have valid sample data
	ERROR2IF(m_pSampleData == NULL, FALSE, "Sample data is null in CSampleData::IncrementInternalIndex");

	double TestIndex = *pCurrentIndex + SampleRate;
	//	TRACEUSER( "Diccon", _T("  Internal index = %f"), TestIndex);
	//	TRACEUSER( "Diccon", _T(", Upper bound = %f\n"), (double)(m_pSampleData->size() - 1));
	if (TestIndex <= (double)(m_pSampleData->size() - 1))
	{
		*pCurrentIndex += SampleRate;
	}
	else
	{
		
	//	TRACEUSER( "Diccon", _T("END OF ARRAY, Test index = %f, Num items = %d\n"), TestIndex, m_NumItems);
		*pCurrentIndex = -1;
		
	}
	
	
	return TRUE;
}

/********************************************************************************************

>	BOOL CSampleData::DecrementInternalIndex(double* pCurrentIndex)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		pCurrentIndex - the index to be decremented, this should really only be either
				the collection index or the retrieval index
	Outputs:	-
	Returns:	TRUE, unless something went wrong
	Purpose:	Decrements our internal array pointer and index counter, unless we have reached
				the start of the array in which case we set it to -1.
********************************************************************************************/

BOOL CSampleData::DecrementInternalIndex(double* pCurrentIndex)
{
	// not allowed to use indexes that aren't one of our members, also 
	// find out which sample rate to use here
	double SampleRate = 1.0;
	if (pCurrentIndex == &m_CollectionInternalIndex)
	{
		SampleRate = m_CollectionSampleRate;
	}
	else if (pCurrentIndex == &m_RetrievalInternalIndex)
	{
		SampleRate = m_RetrievalSampleRate;
	}
	else
	{
		ERROR3("Attempting to use non-member index in CSampleData::DecrementInternalIndex");
		return FALSE;
	}

	// we need to have valid sample data
	ERROR2IF(m_pSampleData == NULL, FALSE, "Sample data is null in CSampleData::DecrementInternalIndex");

	double TestIndex = *pCurrentIndex - SampleRate;
	if (TestIndex > 0)
	{
		*pCurrentIndex -= SampleRate;
	}
	else
	{
		*pCurrentIndex = -1;
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL CSampleData::GetItemAtInternalIndex(double Index, CSampleItem* pItem)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Index - the index to get the value from.  Note that this does not have to be
				an integer, should really be the collection or retrieval index only
	Outputs:	pointer to item retrieved
	Returns:	TRUE if all went well, FALSE if we don't have an array or the index is invalid
	Purpose:	Retrieves the item in the array indicated by the current internal index.  Note
				that it is possible that this index may not be an integer in which case this 
				function will generate a new CSampleItem by interpolation
				
********************************************************************************************/

BOOL CSampleData::GetItemAtInternalIndex(double Index, CSampleItem* pItem)
{
	// no reason why you shouldn't be using this function on values other than our member
	// indexes but that is not the intended purpose of this fn. hence the ERROR3
	//if (Index != m_CollectionInternalIndex && Index != m_RetrievalInternalIndex)
//		ERROR3("Attempting to use non-member index in CSampleData::GetItemAtInternalIndex");

	ERROR2IF(m_pSampleData == NULL, FALSE, "Sample data is null in CSampleData::GetItemAtInternalIndex");

	BOOL Retval = FALSE;
	//TRACEUSER( "Diccon", _T("Retrieving item at %f\n"), Index);

	double UpperBound = (double)m_pSampleData->size() - 1;
	
	// If our index is -1 then we have run off the end of the array, so return the last item
	if (Index == -1)
		Index = UpperBound;

	if (Index <= UpperBound)
	{
		size_t LowIndex = (UINT32)Index;
		if (LowIndex > m_NumItems)
			//ERROR3("Attempting to access data that has not been collected in CSampleData::GetItemAtInternalIndex");
			LowIndex = m_NumItems; //bodge alert!

		UINT32 HighIndex = (UINT32)Index+1;
		if (HighIndex < m_NumItems)
		{
			double Proportion = Index - (double)LowIndex;
			CSampleItem LowItem = (*m_pSampleData)[LowIndex];
			CSampleItem HighItem = (*m_pSampleData)[HighIndex];
			CSampleItem NewItem = LowItem.InterpolateItems(HighItem, (1-Proportion));
			//TRACEUSER( "Diccon", _T("Lo = %d, Hi = %d, Prop  =%f, Pressure = %d\n"), LowIndex, HighIndex, Proportion, NewItem.m_Pressure);
			*pItem = NewItem;
		}
		else
			*pItem = (*m_pSampleData)[LowIndex];
		
		Retval = TRUE;
	}
	else
		ERROR3("Internal index is over the end of the array in CSampleData::GetItemAtInternalIndex");
	
	return Retval;
}

/*------------------------------------------------------------------------------------------
-----------------------------------Other functions------------------------------------------
-------------------------------------------------------------------------------------------*/

/********************************************************************************************

>	BOOL CSampleData::SetCollectionSampleRate(UINT32 Rate)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Rate - the sample rate to use
	Outputs:	-
	Returns:	TRUE if Rate is within the set bounds
	Purpose:	Sets the sample rate to use for collecting data.  Note that for current purposes
				this is only allowed to be 1.  So you probably won't be using this function 
				very often.
********************************************************************************************/

BOOL CSampleData::SetCollectionSampleRate(double Rate)
{
	if (Rate < MIN_COLLECTION_SAMPLE_RATE || Rate > MAX_COLLECTION_SAMPLE_RATE)
	{
		ERROR3("Attempting to set invalid sample rate in CSampleData::SetCollectionSampleRate");
		return FALSE;
	}

	m_CollectionSampleRate = Rate;

	return TRUE;
}

/********************************************************************************************

>	double CSampleData::GetCollectionSampleRate()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	the current collection sample rate
	Purpose:	as above
				
********************************************************************************************/

double CSampleData::GetCollectionSampleRate()
{
	return m_CollectionSampleRate;
}

/********************************************************************************************

>	BOOL CSampleData::SetRetrievalSampleRate(UINT32 Rate)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Rate - the sample rate to use
	Outputs:	-
	Returns:	TRUE if Rate is within the set bounds
	Purpose:	Sets the sample rate to use for retrieving data, 
********************************************************************************************/

BOOL CSampleData::SetRetrievalSampleRate(double Rate)
{
	// CGS:  because of the way that I now use this function, its NOT fair to do the following ....
	
	if (Rate < MIN_RETRIEVAL_SAMPLE_RATE || Rate > MAX_RETRIEVAL_SAMPLE_RATE)
	{
		TRACE( _T("Sample rate passed to CSampleData::SetRetrievalSampleRate was oustide of range!") );
		//ERROR3("Attempting to set invalid sample rate in CSampleData::SetRetrievalSampleRate");
		//return FALSE;
	}

	if (Rate < MIN_RETRIEVAL_SAMPLE_RATE) Rate = MIN_RETRIEVAL_SAMPLE_RATE;
	if (Rate > MAX_RETRIEVAL_SAMPLE_RATE) Rate = MAX_RETRIEVAL_SAMPLE_RATE;

	m_RetrievalSampleRate = Rate;

	return TRUE;
}

/********************************************************************************************

>	double CSampleData::GetRetrievalSampleRate()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	the current Retrieval sample rate
	Purpose:	as above
				
********************************************************************************************/

double CSampleData::GetRetrievalSampleRate()
{
	return m_RetrievalSampleRate;
}

/********************************************************************************************

>	void CSampleData::SetMaxPressure(UINT32 Max)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Max - the maximum pressure value allowed by the current pressure device
	Outputs:	-
	Returns:	-
	Purpose:	To set our max pressure member.  Do this by querying the WinTab device BEFORE
				you start collecting data.
				
********************************************************************************************/

void CSampleData::SetMaxPressure(UINT32 Max)
{
	m_MaxPressure = Max;
	
	// +1 because values go from 0 -> MaxVal whereas we really want the number of values
	m_ScalePressure = (double)(MAXPRESSURE+1) / (Max+1); 
	
}

/********************************************************************************************

>	UINT32 CSampleData::GetMaxPressure()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	the maximum pressure value allowed by the device sending the pressure data
	Purpose:	as above
				
********************************************************************************************/

UINT32 CSampleData::GetMaxPressure()
{
	return m_MaxPressure;
}

/********************************************************************************************

>	void CSampleData::SetSampleArray(SampleArray* pArray)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		pArray - the array to set as a data member
	Outputs:	-
	Returns:	-
	Purpose:	sets the given array to use as our sample data
				
********************************************************************************************/

void CSampleData::SetSampleArray(SampleArray* pArray)
{
	// if we've already got one then delete it
	if (m_pSampleData != NULL)
	{
		m_pSampleData->clear();
		delete m_pSampleData;
		m_pSampleData = NULL;
	}
	m_pSampleData = pArray;
}

/********************************************************************************************


>	BOOL CSampleData::ReverseData()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		-
	Returns:	TRUE if successful, FALSE if we didn't get the memory
	Purpose:	Reverses the items in the data array, or rather just copies them into a new one
********************************************************************************************/

BOOL CSampleData::ReverseData()
{
	if (m_pSampleData == NULL)
		return TRUE;  //EASY

	size_t ArraySize = m_NumItems;  

	if (ArraySize == 0)
		return TRUE;  // also easy

	// make a new array
	SampleArray* pNewArray = new SampleArray;
	if (pNewArray == NULL)
		return FALSE;

	pNewArray->resize( ArraySize );

	size_t UpperBound = ArraySize-1;
	CSampleItem TheItem;
//	BOOL ok = TRUE;
	// take the item at the start of our member array and set it at the end of the new array
	for (UINT32 Counter = 0; Counter < (UINT32)ArraySize ; Counter++)
	{
		TheItem = (*m_pSampleData)[Counter];
		(*pNewArray)[UpperBound - Counter] = TheItem;
	}
	// swap the pointers
	SampleArray* pTemp = m_pSampleData;
	m_pSampleData = pNewArray;
	
	// clear out the original
	pTemp->clear();
	delete pTemp;

	return TRUE;
}

/********************************************************************************************


>	void CSampleData::FinishSampling()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		-
	Returns:	-
	Purpose:	Tells the object that sampling has finished.  As a security precaution we will 
				add an extra 10 * m_RetrievalSampleRates worth of entries.
				The real reason for this hack is that once the path is entered it is then smoothed,
				which causes the path length to increase.  Since we never sampled over this increased
				length we need to kind of pretend that we did.
********************************************************************************************/

void CSampleData::FinishSampling()
{
	if (m_pSampleData != NULL)
	{
		// find out how many items to insert
		UINT32 NumExtraItems = (UINT32)(m_RetrievalSampleRate * 10);
		
		// get the last item we inserted
		UINT32 InsertIndex = (UINT32)m_CollectionInternalIndex;
		CSampleItem TheItem = (*m_pSampleData)[InsertIndex];
		
		// make sure the array is big enough
		UINT32 CurrentSize = (INT32)m_pSampleData->size();
		if (CurrentSize < InsertIndex+NumExtraItems)
		{
			m_pSampleData->resize( CurrentSize + NumExtraItems );
		}

	

		for (UINT32 i = 1; i <= NumExtraItems; i++)
		{
			(*m_pSampleData)[InsertIndex + i] = TheItem;
			m_NumItems++;
		}
	}
}
/********************************************************************************************


>	void CSampleData::SetNumItemsFromArraySize()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		-
	Returns:	-
	Purpose:	Sets the num items member from the size of the array.  Note that you should
				never really use this unless you are certain that a) your current m_NumItems
				is incorrect and b)You are certain that your array contains nothing but valid
				data.  
				An example of when you should use this is if you decide to set the array from
				out side, rather than collect it through sampling.  And you really shouldn't
				be doing that either.
********************************************************************************************/

void CSampleData::SetNumItemsFromArraySize()
{
	if (m_pSampleData != NULL)
		m_NumItems = (UINT32)m_pSampleData->size();
	else
		m_NumItems = 0;
}

/********************************************************************************************


>	BOOL CSampleData::ClipArray(UINT32 Size)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		Size - the desired array size
	Returns:	TRUE unless Size is bigger than the array we have
	Purpose:	Clips the array to be the desired size, be careful with this as you will lose
				all data beyond the Size'th index.
********************************************************************************************/

BOOL CSampleData::ClipArray(size_t Size)
{
	ERROR2IF(m_pSampleData == NULL, FALSE, "Sample data is NULL in CSampleData::ClipArray");
	ERROR2IF(Size > (UINT32)m_pSampleData->size(), FALSE, "Attempting to clip over the end of the array in CSampleData::ClipArray");

	m_pSampleData->resize( Size );
	if (m_NumItems > Size)
		m_NumItems = Size;

	return TRUE;
}


/********************************************************************************************

>	BOOL CSampleData::ReSampleArray(SampleArray** ppArray, UINT32 NewNumItems)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/2000
	Inputs:		pArray      - the array to be resampled
				NewNumItems - the number of items we want to end up with in the array
	Returns:	TRUE if successful, FALSE if something went wrong
	Purpose:	Resamples the data in the sample array so that we end up with NewNumItems in
				the section required.  We will do this by selecting some resampling points in
				the original array, maintain these points at the same proportional position 
				and interpolate to get the new points.
	
	 Notes:		Because, in reality most of our 'sampled' points are actually interpolated
				from a smaller number of data points collected I feel that we don't actually
				lose much accuracy if we just take one in every four points as a control point.

********************************************************************************************/

BOOL CSampleData::ReSampleArray(SampleArray** ppArray, UINT32 NewNumItems)
{
	SampleArray* pArray = *ppArray;
	// check our input conditions
	ERROR2IF(pArray == NULL, FALSE, "Sample array is NULL in CSampleData::ReSampleData");

	
	if (NewNumItems == 0)  // well this is easy
	{
		pArray->clear();
		return TRUE;
	}
	
	// allocate a new array to put the resampled data in
	SampleArray* pNewArray = new SampleArray;
	if (pNewArray == NULL)
		return FALSE;
	pNewArray->resize( NewNumItems );

	// how many items per control item
	UINT32 ItemsPerControl = 4;  // magic number alert!!

	// Insert the control points into the new array
	if (!InsertControlPoints(ItemsPerControl, pArray, pNewArray))
	{
		pNewArray->clear();
		delete pNewArray;
		return FALSE;
	}

	// proceed through the new array interpolating between the control points.
	// We will be able to tell the control points because their coordinates will be non-zero

	UINT32 ControlIndex1 = 0;  // first point is a control point
	UINT32 ControlIndex2 = 0;
	CSampleItem TheItem;

	UINT32 Index = 0;

	while (Index < NewNumItems)
	{
		TheItem = (*pNewArray)[Index];
		if (TheItem.m_Coord.x != 0 && TheItem.m_Coord.y != 0)  // got another control
		{
			ControlIndex2 = Index;
			if (Index != 0)
				InterpolateItems(ControlIndex1, ControlIndex2, pNewArray);
			ControlIndex1 = ControlIndex2;  // current control becomes first control
		}
		Index++;
	}
	
	// now we have to destroy the original array and swap the pointers
	delete pArray;
	*ppArray = pNewArray;

	return TRUE;
}


/********************************************************************************************

>	BOOL CSampleData::InsertControlPoints(UINT32 Frequency, SampleArray* pOriginal, 
										SampleArray* pNew)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/2000
	Inputs:		Frequency - the number of indexes per control point. i.e. 4 = a control point
				every 4th index.
				pOriginal - the original array
				pNew	  - the new array, this MUST be allocated AND sized
	Returns:	TRUE if successful, FALSE if something went wrong
	Purpose:	As part of the ReSampleArray process this function takes the case where we need
				to increase the number of control points.  It therefore proceeds through the original
				array copying control points over at the same proportional position as they were in 
				the original array.  
	

********************************************************************************************/

BOOL CSampleData::InsertControlPoints(UINT32 Frequency, SampleArray* pOriginal, SampleArray* pNew)
{
	ERROR2IF(pOriginal == NULL, FALSE, "Original array is NULL in CSampleData::InsertControlPoints");
	ERROR2IF(pNew      == NULL, FALSE, "New array is NULL in CSampleData::InsertControlPoints");

	double NumOrigItems = (double)pOriginal->size();
	double NumNewItems  = (double)pNew->size();

	INT32    OrigIndex = 0;
	double dOrigIndex = 0;
	INT32 NewIndex  = 0;

	CSampleItem TheItem;
	double OrigProportion;
	while (OrigIndex < NumOrigItems)
	{
		TheItem = (*pOriginal)[OrigIndex];
		OrigProportion = dOrigIndex / NumOrigItems;
		NewIndex = (INT32)(OrigProportion * NumNewItems);
		if (NewIndex < NumNewItems)
			(*pNew)[NewIndex] = TheItem;
		else
			ERROR3("Attempting to set over the end of the new array in CSampleData::InsertControlPoints");
		OrigIndex += Frequency;
		dOrigIndex += Frequency;
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL CSampleData::GetArraySection(UINT32 StartIndex, UINT32 EndIndex, SampleArray* pSection)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/2000
	Inputs:		StartIndex - the internal index at which to start the resampling
				EndIndex   - the internal index to finish the resampling
				pSection   - a to an allocated sample array
	Outputs:	Allocatest the array and fills it with data from the section
	Returns:	TRUE if successful, FALSE if something went wrong
	Purpose:	To copy the data from the section specified into a new array that we will 
				create.

	Errors:		if StartIndex or EndIndex are invalid, or if we don't have a sample array
	
********************************************************************************************/

BOOL CSampleData::GetArraySection(UINT32 StartIndex, UINT32 EndIndex, SampleArray* pSection)
{
	// check our input conditions
	ERROR2IF(pSection == NULL, FALSE, "Sample array is NULL in CSampleData::GetArraySection");
	if (EndIndex <= StartIndex || EndIndex > m_NumItems)
	{
		ERROR3("Invalid indexes in CSampleData::GetArraySection");
		return FALSE;
	}
	
	UINT32 NumItems = EndIndex - StartIndex;
	pSection->resize(NumItems);

	// we want to make sure we retrieve all the items
	m_RetrievalSampleRate = 1.0;

	// We have to do the first one manually, due to my rather clumsy implementation
	CSampleItem TheItem;
	BOOL ok = GetAt(StartIndex, &TheItem);
	if (ok) 
		(*pSection)[0] =  TheItem;
	else
		ERROR3("Failed to retrieve first item in CSampleData::GetArraySection");
	
	UINT32 Counter = 1;
	while (ok && (Counter < NumItems))
	{
		ok = GetNext(&TheItem);
		(*pSection)[Counter] = TheItem;
		Counter++;
	}
	
	// Did we copy the right number of items?
	if (Counter != NumItems)
	{
		ERROR3("Error copying data in CSampleData::GetArraySection");
		ok = FALSE;
	}
	else
		ok = TRUE;
	return ok;
}


/********************************************************************************************


>	BOOL CSampleData::InterpolateItems(UINT32 StartIndex, UINT32 EndIndex, SampleArray* pArray)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/2000
	Inputs:		StartIndex, EndIndex - the indexes to interpolate between
				pArray - the array to do the interpolatin' on
	Outputs:	the interpolated array
	Returns:	TRUE unless the indexes are wack
	Purpose:	Interpolates between the two indexes in the array, creating intermediate items
				and inserting them

********************************************************************************************/

BOOL CSampleData::InterpolateItems(UINT32 StartIndex, UINT32 EndIndex, SampleArray* pArray)
{
	ERROR2IF(pArray == NULL, FALSE, "Sample array is NULL in CSampleData::InterpolateItems");
	size_t				MaxIndex = pArray->size() - 1;

	if (EndIndex <= StartIndex  || EndIndex > MaxIndex)
	{
		ERROR3("Invalid indexes in CSampleData::InterpolateItems");
		return FALSE;
	}
	// get the two items to interpolate between
	CSampleItem FirstItem = (*pArray)[StartIndex];
	CSampleItem LastItem  = (*pArray)[EndIndex];

	// get the proportional changes for each step
	UINT32 NumItems = EndIndex - StartIndex;
	double ItemProportion = 1 / (double)NumItems;

	MILLIPOINT XVal = (INT32)((double)(LastItem.m_Coord.x - FirstItem.m_Coord.x) * ItemProportion);
	MILLIPOINT YVal = (INT32)((double)(LastItem.m_Coord.y - FirstItem.m_Coord.y) * ItemProportion);
	INT32 PressVal	= (INT32) (((double)(LastItem.m_Pressure - FirstItem.m_Pressure) * ItemProportion));

	UINT32 Index = StartIndex + 1;
	CSampleItem NewItem = FirstItem;
	while (Index < EndIndex)
	{
		NewItem.m_Coord.x += XVal;
		NewItem.m_Coord.y += YVal;
		NewItem.m_Pressure += PressVal;
		(*pArray)[Index] = NewItem;
		Index++;
	}
	return TRUE;
}

/********************************************************************************************


>	BOOL CSampleData::WriteNative(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		pRecord - the record to write to
	Returns:	TRUE if the Node has written out a record to the filter
	Purpose:	Writes out the data contained in this SampleData to a record

********************************************************************************************/

BOOL CSampleData::WriteNative(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL in CSampleData::WriteNative");

	BOOL ok = TRUE;
	ok = pRecord->WriteINT32((INT32)m_NumItems);

	if (m_pSampleData != NULL)
	{
		CSampleItem TheItem;
		
		SetRetrievalSampleRate(1.0); // as we want 
		BOOL KeepWriting  = GetAt(0, &TheItem);  // get the first itemto save out every item
		

		UINT32 Counter = 0;
		// note that GetNext returns FALSE it just means we've reached the end, not an error
		while (ok && Counter <= m_NumItems && KeepWriting)
		{
			if (ok) ok = TheItem.WriteNative(pRecord);
			KeepWriting = GetNext(&TheItem);
			Counter++;
		}
	}
	return ok;
}

/*-------------------------------------------------------------------------------------------
------------------------------CPressureSampler implementation-------------------------------------
---------------------------------------------------------------------------------------------
*/

/********************************************************************************************

>	CDistanceSampler::CDistanceSampler()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	this object
	Purpose:	default constructor
				
	Notes:		
********************************************************************************************/

CDistanceSampler::CDistanceSampler()
{
	m_SampleDistance = MIN_BRUSH_SPACING;
	m_DistanceSinceLastSample = -1;
	m_DistanceSoFar = 0;
	m_TotalDistance = 0;
	m_ItemsInserted = 0;
	m_DistanceSinceLastData = 0;
	m_ItemRemainder = 0.0;
}

/********************************************************************************************

>	CDistanceSampler::~CDistanceSampler()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	this object
	Purpose:	destructor
				
	Notes:		
********************************************************************************************/

CDistanceSampler::~CDistanceSampler()
{

}

/********************************************************************************************

>	BOOL CDistanceSampler::SetSampleDistance(MILLIPOINT Distance)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Distance - the distance to sample at
	Outputs:	-
	Returns:	TRUE, unless Distance is out of bounds
	Purpose:	Sets the sample distance, I can't see why you would wish to sample at anything
				other than MIN_BRUSH_SPACING, but each to their own
				
	Notes:		
********************************************************************************************/

BOOL CDistanceSampler::SetSampleDistance(MILLIPOINT Distance)
{
	if (Distance < MIN_BRUSH_SPACING || Distance > MAX_BRUSH_SPACING)
	{
		ERROR3("Invalid distaance in CDistanceSampler::SetSampleDistance");
		return FALSE;
	}
	m_SampleDistance = Distance;

	return TRUE;
}

/********************************************************************************************

>	BOOL CDistanceSampler::SetSampleDistance(MILLIPOINT Distance)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	our sample distance
	Purpose:	access fn.
				
	Notes:		
********************************************************************************************/

MILLIPOINT CDistanceSampler::GetSampleDistance()
{
	return m_SampleDistance;
}

/********************************************************************************************

>	BOOL CDistanceSampler::CollectData(DocCoord Coord,  UINT32 Pressure)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Coord - the current coordinate
				Pressure - the current pressure
	Outputs:	-
	Returns:	TRUE if successful, FALSE if something went wrong
	Purpose:	This function is in charge of doing the actual sampling of data
				
	Notes:		
********************************************************************************************/

BOOL CDistanceSampler::CollectData(DocCoord Coord, UINT32 Pressure)
{
	// first up check that we have set the max pressure value
	ERROR2IF(m_MaxPressure == 0, FALSE, "You're supposed to set the Max Pressure value BEFORE you collect data");
	ERROR2IF(Pressure > m_MaxPressure, FALSE, "Pressure is greater than maximum value in CDistanceSampler::CollectData");

	BOOL ok = FALSE;
	// first up, if we are initialised to -1 then this is the first point, which we want
	if (m_DistanceSinceLastSample == -1)
	{
		if (m_ScalePressure != 1.0)
			Pressure = (UINT32)(m_ScalePressure * Pressure);
		CSampleItem TheItem(Coord, 0, Pressure);
		ok = SetAt(0, TheItem);
		m_DistanceSinceLastSample = 0;
//		TRACEUSER( "Diccon", _T("Recording pressure %d at 0\n"), Pressure);
		m_LastItemStored = TheItem;
		m_LastItem = TheItem;
		m_LastCoord = Coord;

	}
	else
	{
		// find out how far we've gone
		MILLIPOINT Dist = (MILLIPOINT)Coord.Distance(m_LastItem.m_Coord);
		m_DistanceSinceLastData		 = Dist;
		m_DistanceSinceLastSample	+= Dist;
		m_TotalDistance				+= Dist;
	//	TRACEUSER( "Diccon", _T("Received Pressure %d at Distance %d\n"), Pressure ,m_TotalDistance);
		// work out how many new items we need to make, and the proportional values to give to each
		double NumInsertItems = (double)m_DistanceSinceLastSample / (double)m_SampleDistance;
		if (NumInsertItems > 1)
		{
			if (m_ItemRemainder > 1) // if our remainder total adds up to >1 then add one more
			{
				NumInsertItems++;
				m_ItemRemainder--;
			}
			double ItemProportion = 1 / NumInsertItems;
		
			// work out the values that we need to add on for each step
			MILLIPOINT XVal = (INT32)((double)(Coord.x - m_LastItem.m_Coord.x) * ItemProportion);
			MILLIPOINT YVal = (INT32)((double)(Coord.y - m_LastItem.m_Coord.y) * ItemProportion);
			double dPressure = (double)Pressure * m_ScalePressure;
			INT32 PressVal	= (INT32)((dPressure - (double)m_LastItemStored.m_Pressure) * ItemProportion);
			m_ItemsInserted += (UINT32)NumInsertItems;
			//TRACEUSER( "Diccon", _T("INSERTED %d ITEMS\n"), m_ItemsInserted); 
			CSampleItem TheItem = m_LastItemStored;
			while (NumInsertItems > 1)
			{
				m_DistanceSoFar += m_SampleDistance;
				TheItem.m_Pressure += PressVal;
				TheItem.m_Coord.x += XVal;
				TheItem.m_Coord.y += YVal;
				TheItem.m_Distance = m_DistanceSoFar;
				ok = SetNext(TheItem, TRUE);
				NumInsertItems--;
				m_DistanceSinceLastSample -= m_SampleDistance;
				//TRACEUSER( "Diccon", _T("Recording pressure %d at %d\n"), TheItem.m_Pressure, m_DistanceSoFar);
			}
			// add the remainder to our remainder counter
			m_ItemRemainder += NumInsertItems;

			m_LastItemStored = TheItem;

		}
		else
			ok = TRUE;
	}
	m_LastItem.m_Pressure = Pressure;
	m_LastItem.m_Coord	  = Coord;
	return ok;
}


/********************************************************************************************

>	BOOL CDistanceSampler::SetSampleRateFromSpacing(MILLIPOINT Spacing)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Spacing - the spacing between brush objects
	Outputs:	-
	Returns:	TRUE so long as spacing is between bounds
	Purpose:	Given the spacing between objects this works out the sample rate at which
				we want to retrieve objects
	Notes:		
********************************************************************************************/

BOOL CDistanceSampler::SetSampleRateFromSpacing(MILLIPOINT Spacing)
{
	if (/*Spacing < MIN_BRUSH_SPACING ||*/ Spacing > MAX_BRUSH_SPACING)
	{
		ERROR3("Attempting to set invalid spacing value");
		return FALSE;
	}

	/* recall that sample rate 1 : Spacing = MIN_BRUSH_SPACING
				   sample rate 2 : Spacing = 2 * MIN_BRUSH_SPACING
				   sample rate 3 : Spacing = 3 * .. etc. */

	m_RetrievalSampleRate = ((double)Spacing / (double)MIN_BRUSH_SPACING) ;

	return TRUE;
}


/********************************************************************************************

>	UINT32 CDistanceSampler::GetInternalIndexFromDistance(MILLIPOINT Distance)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		Distance - the distance along the path
	Outputs:	-
	Returns:	the internal index into the distance array at that distance, or -1 if something goes wrong
	Purpose:	If you want to find out what index corresponds to Distance then call this,
				actually all it does is divide by the minimum spacing. Note that it will always
				round down.
	ERRORS:		Negative distance being passed in, Distance resulting in index which is off the
				end of the array.
	Notes:		
********************************************************************************************/

INT32 CDistanceSampler::GetInternalIndexFromDistance(MILLIPOINT Distance)
{
	if (Distance < 0)
	{
		ERROR3("Negative distance in CDistanceSampler::GetInternalIndexFromDistance");
		return -1;
	}

	INT32 Index = Distance / MIN_BRUSH_SPACING;

	if (Index > (INT32)m_NumItems)
		Index = -1;
	return Index;
}


/********************************************************************************************

>	CDistanceSampler* CDistanceSampler::MakeCopy()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	a copy of this object, unless something goes wrong (i.e memory) in which case NULL
	Purpose:	Makes a copy of this sample data. Most of the work is done in the base class
				but we need to also copy a few members over
				
	Notes:		
********************************************************************************************/

CDistanceSampler* CDistanceSampler::MakeCopy()
{
	CDistanceSampler* pNewData = new CDistanceSampler;

	if (pNewData == NULL)
		return NULL;

	if (!pNewData->InitialiseData(m_NumItems))
	{
		delete pNewData;
		return NULL;
	}

	m_RetrievalSampleRate = 1; // make sure we get all the items

	CSampleItem TheItem;

	// unfortunately my implementation is a bit clumsy so we have to do the first item
	// by hand
	BOOL	Getok = GetAt(0, &TheItem);
	BOOL	Setok = pNewData->SetAt(0, TheItem);
	//TRACEUSER( "Diccon", _T("Copying pressure %d\n"), TheItem.m_Pressure);
	if (Getok == FALSE || Setok ==  FALSE)
		ERROR3("Failed to get or set first item in CSampleData::MakeCopy");

	UINT32 Counter = 1;
	// now we can loop through the rest
	while (Getok && Setok)
	{
		Getok = GetNext(&TheItem);
		Setok = pNewData->SetNext(TheItem);
		//TRACEUSER( "Diccon", _T("Copying pressure %d\n"), TheItem.m_Pressure);
		Counter++;
	}

	pNewData->m_DistanceSoFar = TheItem.m_Distance;

	if (Counter != m_NumItems)
		TRACEUSER( "Diccon", _T("CSampleData::MakeCopy - Num items copied = %d, Actual num items = %d\n"), Counter, m_NumItems);

//	pNewData->SetSampleDistance(m_SampleDistance);
	return pNewData;
}


/********************************************************************************************

>	BOOL CDistanceSampler::GetDataSection(MILLIPOINT Start, MILLIPOINT End, CDistanceSampler* pSection)
	
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/2000
	Inputs:		Start, End - the distances between which we want to get the data from
				pSection   - pointer to an allocated CDistanceSampler
	Outputs:	Initialises pSection with data between the two distances
	Returns:	TRUE if successful, FALSE if something went wrong
	Purpose:	Retrieves the data between Start and End and puts it into the sampler provided.

	Errors:		if Start or End are invalid, or if we don't have a sample array
	See Also:	CSampleData::GetArraySection

	Notes:		If this fn. goes wrong you are responsible for deleting the CDistanceSampler
********************************************************************************************/

BOOL CDistanceSampler::GetDataSection(MILLIPOINT Start, MILLIPOINT End, CDistanceSampler* pSection)
{	
	ERROR2IF(pSection == NULL, FALSE, "Sampler is NULL in CDistanceSampler::GetDataSection");

	if (Start < 0 || End < 0 || (End <= Start))
	{
		ERROR3("Invalid distances in CSampleData::ReSample");
		return FALSE;
	}
	// I guess if the caller hasn't allocated the sampler then we can do it, under duress..
	if (pSection == NULL)
	{
		ERROR3("You haven't initialised the distance sampler.  Well I suppose I can do it for you...");
		pSection = new CDistanceSampler;
		if (pSection == NULL)
		{
			ERROR3("Well actually I can't, sorry must bail");
			return FALSE;
		}
	}
	// get the indexes of the start and end points
	INT32 StartIndex = GetInternalIndexFromDistance(Start);
	INT32 EndIndex   = GetInternalIndexFromDistance(End);

	if (StartIndex == -1 || EndIndex == -1)
	{
		ERROR3("Error getting indexes in CDistanceSampler::ReSample");
		return FALSE;
	}

	// figure out how many items in the new length
	INT32 NewNumItems = (EndIndex - StartIndex) / MIN_BRUSH_SPACING;

	// initialise the data
	if (!pSection->InitialiseData(NewNumItems))
		return FALSE;

	// call the helper function to do the work
	BOOL ok = GetArraySection((UINT32)StartIndex, (UINT32)EndIndex, pSection->GetSampleArray());
	if (ok)
		pSection->SetNumItemsFromArraySize();

	return ok;
}



/********************************************************************************************

>	BOOL CDistanceSampler::ReSample(MILLIPOINT NewLength)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/2000
	Inputs:		NewLength - the new length to sample over
	Outputs:	-
	Returns:	TRUE if all went well, FALSE if not
	Purpose:	If we want to change the distance that we sampled over then we must resample
				the data to make it fit.  
				
	Notes:		In truth most of the work is done in the base class. Here we simply calculate the
				new number of items that we need and pass it along
********************************************************************************************/

BOOL CDistanceSampler::ReSample(MILLIPOINT NewLength)
{
	//checks
	if (NewLength <= 0 || m_pSampleData == NULL)
	{
		ERROR3("Invalid entry conditions in CDistanceSampler::ReSample");
		return FALSE;
	}

	// find out the new number of items
	INT32 NewNumItems = NewLength / MIN_BRUSH_SPACING;

	m_DistanceSoFar = NewLength;

	BOOL ok = ReSampleArray(&m_pSampleData, NewNumItems);
	if (ok)
		SetNumItemsFromArraySize();  // make sure our number of items is up to date

	return ok;

}
