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
// 


#include "camtypes.h"
#include "bmpseq.h"
#include "document.h"
#include "bitmap.h"
#include "bmpcomp.h"
#include "app.h"


// Place any IMPLEMENT type statements here
CC_IMPLEMENT_MEMDUMP(BitmapSequenceInfo, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(BitmapSequence, CC_CLASS_MEMDUMP)


// We want better memory tracking
#define new CAM_DEBUG_NEW

//#define DECLARE_PROCEDURE(p)		#define CURRENT_PROCEDURE_NAME	#p
//#define DECLARE_PROCEDURE_END(p)	#undef CURRENT_PROCEDURE_NAME
#define CURRENT_PROCEDURE_NAME			"bmpseq.cpp"

#define ERROR2_CLASS(arg, classname)	ERROR2IF(arg == NULL || !arg->IS_KIND_OF(classname),	\
										FALSE, #arg " isn't");
#define ERROR3IF_NOT_CLASS(arg, classname)		ERROR3IF(arg != NULL && \
												!arg->IS_KIND_OF(classname), \
												CURRENT_PROCEDURE_NAME " - " #arg " isn't");

#define ERROR3IF_NULL_CLASS(arg, classname)		ERROR3IF(arg == NULL || \
												!arg->IS_KIND_OF(classname), \
												CURRENT_PROCEDURE_NAME " - " #arg " isn't");

// Functions follow
/********************************************************************************************

>	void BitmapSequence::DeleteAll()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Purpose:	Deletes all the KernelBitmaps in the sequence

********************************************************************************************/
void BitmapSequence::DeleteAll()
{
	Bitmaps.DeleteAll();
}


/********************************************************************************************

>	BOOL BitmapSequence::CopyFromDocumentList(Document* pDocument, const KernelBitmap* pStartBitmap, const UINT32 uNumberToCopy)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pDocument - the document to copy from
				pStartBitmap - a pointer to a KernelBitmap in the document's bitmap list
				providing a starting point from which to copy. 
				uNumberToCopy - the number of bitmaps to copy
	Purpose:	Copies a sequence of bitmaps from the given document's bitmap list into
				this BitmapSequence.

********************************************************************************************/
BOOL BitmapSequence::CopyFromDocumentList(Document* pDocument, const KernelBitmap* pStartBitmap, const UINT32 uNumberToCopy)
{
	if (pDocument == NULL)
	{
		pDocument = Document::GetSelected();
	}
	ERROR2IF(pDocument == NULL, FALSE, "pDocument == NULL")
	// The bitmap system relies on the Current Doc being correct,
	// so we'll set it to be on the safe side
	pDocument->SetCurrent();

	// Find this document's BitmapListComponent
	DocComponent* pDocBitmapList = pDocument->GetDocComponent(CC_RUNTIME_CLASS(BitmapListComponent));
	ERROR2IF(pDocBitmapList == NULL, FALSE, "Couldn't find document bitmap list");

	// Now get the actual bitmap list
	BitmapList* pBmpList = ((BitmapListComponent*)pDocBitmapList)->GetBitmapList();

	ListItem* pItem = (ListItem*)pStartBitmap;
	UINT32 uCurrentIndex = 1;

	while (uCurrentIndex <= uNumberToCopy)
	{
		KernelBitmap* pBmp = (KernelBitmap*)pItem;
		ERROR3IF_NULL_CLASS(pBmp, KernelBitmap);

		if (!AddBitmap(pBmp, INT32(uCurrentIndex)))
		{
			return FALSE;
		}

		pItem = pBmpList->GetNext(pItem);
		++uCurrentIndex;
	}
	return TRUE;
}


/********************************************************************************************

>	KernelBitmap* FindBitmap(const List* const pBitmapList, const KernelBitmap* const pStart, 
				const KernelBitmap* const pBitmap)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pList - the list in which pListItem is
				pListItem - the list item from which we wish to find the offset
				uOffset - the number of the item further along the list which we wish to
				retrieve
	Returns:	A pointer to the desired list item
				NULL if gone off the end
	Purpose:	Finds the ListItem as the position uOffset items further down the list from
				pListItem.

********************************************************************************************/
KernelBitmap* FindBitmap(const List* const pBitmapList, const KernelBitmap* const pStart, 
				const KernelBitmap* const pBitmap)
{
	ERROR3IF_NULL_CLASS(pBitmap, KernelBitmap);

	if (pBitmapList == NULL)
	{
		return NULL;
	}
	const ListItem* pItem = pStart;
	const OILBitmap* pOILBmp = pBitmap->ActualBitmap;

	while (pItem != NULL)
	{
		KernelBitmap* pBmp = (KernelBitmap*)pItem;
		ERROR3IF_NOT_CLASS(pBmp, KernelBitmap);

		if (pBmp->ActualBitmap == pOILBmp || *pBmp->ActualBitmap == *pOILBmp)
		{
			return pBmp;
		}
		pItem = pBitmapList->GetNext(pItem);
	}
	return NULL;
}


/********************************************************************************************

>	const ListItem* FindOffsetItem(const List* pList, const ListItem* pListItem, UINT32 uOffset)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pList - the list in which pListItem is
				pListItem - the list item from which we wish to find the offset
				uOffset - the number of the item further along the list which we wish to
				retrieve
	Returns:	A pointer to the desired list item
				NULL if gone off the end
	Purpose:	Finds the ListItem as the position uOffset items further down the list from
				pListItem. Used by the search FindBitmapSubstring()

********************************************************************************************/
const ListItem* FindOffsetItem(const List* pList, const ListItem* pListItem, UINT32 uOffset)
{
	while (uOffset > 0 && pListItem != NULL)
	{
		pListItem = pList->GetNext(pListItem);
		--uOffset;
	}
	return pListItem;
}


/********************************************************************************************

>	KernelBitmap* FindBitmapSubstring(List* pMainList, List* pSubList, 
				const ListItem* pFirstMain, const ListItem* const pFirstSub,
				const INT32 nSubLength)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pMainList - the KernelBitmap list in which we wish to find pSubList
				pSubList - the KernelBitmap list we wish to find
				pFirstMain - the KernelBitmap in pMainList from which we wish
				to start searching
				pFirstSub - the KernelBitmap in pSubList from which we wish to start
				matching
	Purpose:	This is used internally by FindBitmapSubstring(...3 arguments...).
				Call that instead.
	Notes:		Implements some well known string search algorithm whose name I can't
				remember. Basically, it looks at the end of the search string for a match
				and moves along in chunks if it can get away with it. Here it is from the STL

				template <class charT>
				size_t
				basic_string<charT>::find (const basic_string<charT>& str, size_t pos) const
																		_THROW_NONE
				{
				    return find_str (str.data(), pos, str.length());
				}
				
				/*template <class charT>
				size_t
				basic_string<charT>::find_str (const charT* s, size_t pos, size_t len) 
                				               const _THROW_NONE
				{
    				size_t  count = pos;
    				size_t  shift;
    				size_t  place;
    				if ((length() == 0) || (len == 0))
        				return NPOS;
    				while (len <= (length()-count))
    				{
        				for (place = 0; place < len; ++place)
        				{
            				if (baggage_type::ne(	*(s+len-1-place),
													*(data()+count+(len-1-place)))
												)
                				break;
        				}
        				if (place == len)
            				return count;
        				shift = find(*(s+len-1-place), count+(len-place));
        				if (shift == NPOS)
            				return NPOS;
        				count = shift-(len-place-1);
    				}
    				return NPOS;

********************************************************************************************/
KernelBitmap* FindBitmapSubstring(List* pMainList, List* pSubList, 
				const ListItem* pFirstMain, const ListItem* const pFirstSub,
				const INT32 nSubLength)
{
	ERROR3IF_NOT_CLASS(pMainList, List);
	ERROR3IF_NOT_CLASS(pSubList, List);

	if (pFirstMain == NULL || pFirstSub == NULL)
		return NULL;

	ERROR3IF_NOT_CLASS(pFirstMain, KernelBitmap);
	ERROR3IF_NOT_CLASS(pFirstSub, ImportBitmap);

    if (nSubLength <= 0)
	{
		ERROR3("FindBitmapSubstring - nSubLength <= 0");
        return NULL;
	}
	const ListItem* pCurrentMain = FindOffsetItem(pMainList, pFirstMain, nSubLength - 1);
	const ListItem* pCurrentSub;
	const ListItem* pLastSub = pSubList->GetTail();
	ERROR3IF(pLastSub == NULL, "FindBitmapSubstring - pLastSub == NULL");

    while (pCurrentMain != NULL)
    {
		pCurrentSub = pLastSub;
		INT32 nCurrentSubLength = 1;		// set as the new length of the substring to check
        while (nCurrentSubLength <= nSubLength)
		{
			KernelBitmap* pMainKernel = (KernelBitmap*)pCurrentMain;
			OILBitmap* pExistingOIL = pMainKernel->ActualBitmap;
			ERROR3IF_NULL_CLASS(pExistingOIL, OILBitmap);
			
			KernelBitmap* pSubKernel = ((ImportBitmap*)pCurrentSub)->pBmp;
			OILBitmap* pSubOIL = pSubKernel->ActualBitmap;
			ERROR3IF_NULL_CLASS(pSubOIL, OILBitmap);

            if (pExistingOIL != pSubOIL && !(*pExistingOIL == *pSubOIL))
                break;

			pCurrentMain = pMainList->GetPrev(pCurrentMain);
			pCurrentSub = pSubList->GetPrev(pCurrentSub);
			++nCurrentSubLength;
        }

        if (pCurrentSub == pSubList->GetPrev(pFirstSub))		// found the whole sub-list
		{
            return (KernelBitmap*)pCurrentMain;
		}

		// failed to find all the items in SubList so look for the sub-sublist where the
		// disparity was, starting at the next item in the main list.
        pCurrentMain = FindBitmapSubstring(	pMainList, pSubList, 
								pMainList->GetNext(pCurrentMain), pCurrentSub, 
								nCurrentSubLength);
		if (pCurrentMain == NULL)
		{
			return NULL;
		}
		pCurrentMain = pMainList->GetPrev(pCurrentMain);
		pLastSub = pSubList->GetPrev(pCurrentSub);
    }
    return NULL;
}


/*
INT32 FindSubString(INT32* pMainList, INT32* pSubList, const INT32 pFirstMain, INT32 pFirstSub, INT32 nSubLength)
{
	if (pMainList[pFirstMain] == NULL)
		return 0;

	INT32 pLastMain = FindOffsetItem(pMainList, pFirstMain, nSubLength - 1);
	
	INT32 pLastSub = sizeof(search) / sizeof(INT32) - 2;
	while (1)
	{
		INT32 pCurrentMain = pLastMain;
		INT32 pCurrentSub = pLastSub;
		INT32 nCurrentSubLength = 1;
		while (nCurrentSubLength <= nSubLength)
		{
			if (pMainList[pCurrentMain] != pSubList[pCurrentSub])
				break;
			--pCurrentMain;
			--pCurrentSub;
			++nCurrentSubLength;
		}
		if (nCurrentSubLength > nSubLength)
		{
			return (pCurrentMain + 1);
		}
		INT32 pFoundMain = FindSubString(pMainList, pSubList, (pCurrentMain + 1), pCurrentSub, nCurrentSubLength);
		if (pFoundMain == NULL)
			return 0;
		pLastMain = pFoundMain;
//		pLastSub = pCurrentSub;
	}
	return NULL;
}

INT32 Find(INT32* pMainList, const INT32 pFirstMain, const INT32 item)
{
	INT32 pCurrent = pFirstMain;
	while (pMainList[pCurrent] != NULL)
	{
		if (item == pMainList[pCurrent])
		{
			return pCurrent;
		}
		++pCurrent;
	}
	return NULL;
}



INT32 FindSubString(INT32* pMainList, INT32* pSubList, const INT32 pFirstMain)
{
	return FindSubString(array, search, 1, 1, 3);
}
*/

/********************************************************************************************

>	KernelBitmap* FindBitmapSubstring(List* pMainList, List* pSubList, 
					ListItem* pFirstMain)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pMainList - the KernelBitmap list in which we wish to find pSubList
				pSubList - the KernelBitmap list we wish to find
				pFirstMain - the KernelBitmap in pMainList from which we wish
				to start searching
	Returns:	A pointer to the first KernelBitmap in pMainList where the matched
				sequence starts.
				NULL if pSubList wasn't found
	Purpose:	Searches for a sequence (string) of bitmaps.

********************************************************************************************/
KernelBitmap* FindBitmapSubstring(List* pMainList, List* pSubList, ListItem* pFirstMain)
{
	return (FindBitmapSubstring(pMainList, pSubList, pFirstMain, pSubList->GetHead(), 
								pSubList->GetCount()));
}


/********************************************************************************************

>	KernelBitmap* FindSimple(List* pMainList, List* pSubList, 
							const ListItem* const pFirstMain)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pMainList - the KernelBitmap list in which we wish to find pSubList
				pSubList - the KernelBitmap list we wish to find
				pFirstMain - the KernelBitmap in pMainList from which we wish
				to start searching
	Returns:	A pointer to the first KernelBitmap in pMainList where the matched
				sequence starts.
				NULL if pSubList wasn't found
	Purpose:	Searches for a sequence (string) of bitmaps. This may be faster than the
				supposedly fast and complicated FindBitmapSubstring(). It was when I
				profiled it.

********************************************************************************************/
KernelBitmap* FindSimple(List* pMainList, List* pSubList, const ListItem* const pFirstMain)
{
	ERROR2IF(pMainList == NULL, NULL, "pMainList == NULL");
	ERROR2IF(pSubList == NULL, NULL, "pSubList == NULL");
	ERROR2IF(pFirstMain == NULL, NULL, "pFirstMain == NULL");

	// Is the list we're looking for empty or longer than the list we're looking in?
	UINT32 uSubListLen = pSubList->GetCount();
	if (uSubListLen == 0 || uSubListLen > pMainList->GetCount())
	{
		return NULL;
	}

	// ...No. So go through the main list looking for the first item in our search list
	// and each time we find it check subsequent items in the main list for a match with
	// those remaining in our search list. If we found them all that's it, otherwise
	// continue looking for the first item in the main list.

	const ListItem* pStartMain = pFirstMain;
	const ListItem* const pFirstSub = pSubList->GetHead();
	for (;;)	// ever
	{
		// Find the first item of the sublist in the main list
		const ListItem* pFound = (ListItem*)FindBitmap(pMainList, (KernelBitmap*)pStartMain, 
													((ImportBitmap*)pFirstSub)->pBmp);
		if (pFound == NULL)		// the first search item isn't in the main list at all
		{
			return NULL;
		}

		const ListItem* pCurrentMain = pMainList->GetNext(pFound);
		const ListItem* pCurrentSub = pSubList->GetNext(pFirstSub);
		while (pCurrentSub != NULL)
		{
			ERROR3IF(pCurrentMain == NULL, "pCurrentMain == NULL");

			KernelBitmap* pMainKernel = (KernelBitmap*)pCurrentMain;
			OILBitmap* pExistingOIL = pMainKernel->ActualBitmap;
			ERROR3IF_NULL_CLASS(pExistingOIL, OILBitmap);
			
			KernelBitmap* pSubKernel = ((ImportBitmap*)pCurrentSub)->pBmp;
			OILBitmap* pSubOIL = pSubKernel->ActualBitmap;
			ERROR3IF_NULL_CLASS(pSubOIL, OILBitmap);

            if (pExistingOIL != pSubOIL && !(*pExistingOIL == *pSubOIL))
                break;

			pCurrentMain = pMainList->GetNext(pCurrentMain);
			pCurrentSub = pSubList->GetNext(pCurrentSub);
		}
		
		if (pCurrentSub == NULL)		// we reached the end of our search list. All done.
		{
			return (KernelBitmap*)pFound;
		}

		// We didn't manage to find the whole search list, so go back looking for the
		// first item in the search list starting at the next item in the main list
		pStartMain = pMainList->GetNext(pStartMain);
	}
}

/********************************************************************************************

>	BitmapSequenceInfo*	BitmapSequence::GetSequenceInfo() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Returns:	The BitmapSequenceInfo for this BitmapSequence
	Purpose:	Er...

********************************************************************************************/
BitmapSequenceInfo*	BitmapSequence::GetSequenceInfo() const
{
	return m_pSequenceInfo;
}


/********************************************************************************************

>	BOOL BitmapSequence::SetSequenceInfo(BitmapSequenceInfo* pNewInfo)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pNewInfo - a pointer
	Returns:	TRUE if set OK
				FALSE otherwise
	Errors:		ERROR3's for invalid args
	Purpose:	To allow for future extensions

********************************************************************************************/
BOOL BitmapSequence::SetSequenceInfo(BitmapSequenceInfo* pNewInfo)
{
	ERROR3IF(pNewInfo != NULL && !pNewInfo->IS_KIND_OF(BitmapSequenceInfo), 
				"BitmapSequence::SetSequenceInfo - pNewInfo isn't");

	m_pSequenceInfo = pNewInfo;
	return TRUE;
}


/********************************************************************************************

>	UINT32 BitmapSequence::GetCount() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Returns:	The number of bitmaps in this BitmapSequence
	Purpose:	You would probably want to know this

********************************************************************************************/
UINT32 BitmapSequence::GetCount() const
{	
	return Bitmaps.GetCount();
}




/********************************************************************************************

>	KernelBitmap* BitmapSequence::FindInDocumentList(Document* pImportDocument)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pImportDocument - the document in which to search for this BitmapSequence
	Returns:	A pointer to the first KernelBitmap in pMainList where the matched
				sequence starts.
				NULL if pSubList wasn't found
	Purpose:	Searches for this BitmapSequence in pImportDocument's bitmap list

********************************************************************************************/
KernelBitmap* BitmapSequence::FindInDocumentList(Document* pImportDocument)
{
	ERROR2IF(pImportDocument == NULL, FALSE, "pImportDocument NULL");
	ERROR3IF(!pImportDocument->IS_KIND_OF(Document), "BitmapSequence::FindInDocumentList - pImportDocument isn't");

	// First we need to look at the Bitmap List in the document we are Importing
	// into, and if we find a KernelBitmap that points at a bitmap the same as the
	// one we have imported, then we'll just use that.

	// The bitmap system relies on the Current Doc being correct,
	// so we'll set it to be on the safe side
	pImportDocument->SetCurrent();

	// Find this document's BitmapListComponent
	DocComponent* pDocBitmapList = 
						pImportDocument->GetDocComponent(CC_RUNTIME_CLASS(BitmapListComponent));
	ERROR2IF(pDocBitmapList == NULL, FALSE, "Couldn't find document bitmap list");
	List* pMainList = (List*)((BitmapListComponent*)pDocBitmapList)->GetBitmapList();

	KernelBitmap* pFound;		// the first item in the document list for the match
	// if there's only one bitmap do a simple search for it...
/*	if (GetCount() == 1)
	{
		KernelBitmap* pStart = (KernelBitmap*)pMainList->GetHead();
		KernelBitmap* pBitmap = ((ImportBitmap*)((List*)&Bitmaps)->GetHead())->pBmp;
		pFound = FindBitmap(pMainList, pStart, pBitmap);
	}
	else		// do a hideous search
	{*/
		ListItem* pFirstMain = (ListItem*)pMainList->GetHead();
		List* pNewBitmaps = (List*)&Bitmaps;
		pFound = FindSimple(pMainList, pNewBitmaps, pFirstMain);
//	}

	return pFound;
}



