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

// Kernel level parts of bitmap lists

/*
*/

#include "camtypes.h"
#include "bmplist.h"
#include "bitmap.h"
#include "bitmpinf.h"
#include "list.h"
#include "errors.h"
#include "app.h"
#include "fillattr.h"
//#include "will.h"
//#include "native.h"			// The new designed native filter, used for v2
//#include "nativeps.h"		// The old style EPS native filter, used in v1.1

#include "clipint.h"
#include "nodebmp.h"
#include "progress.h"

#include "toollist.h"
#include "objchge.h"
#include "csrstack.h"

//#include "clipint.h"
#include "ophist.h"

//#include "will2.h"

#include "nodetxtl.h"

#include "bmpcomp.h"

//#include "nodecach.h"		// for CacheCompound specific tests in cleaning up GlobalList

CC_IMPLEMENT_MEMDUMP( GlobalBitmapList, List )
//CC_IMPLEMENT_MEMDUMP( GreyscaleBitmapList, List )

CC_IMPLEMENT_DYNAMIC(BitmapListChangedMsg, Msg)
#if !defined(EXCLUDE_FROM_RALPH)
CC_IMPLEMENT_DYNCREATE(OpDeleteBitmap,Operation);
#endif

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// Preference for the maximum size of the greyscale bitmap list
//INT32 GreyscaleBitmapList::m_lMaxSize = 2 * 1024 * 1024;		// 2MB default max size


/**********************************************************************************************

>	GlobalBitmapList::GlobalBitmapList()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GlobalBitmapList constructor

**********************************************************************************************/

GlobalBitmapList::GlobalBitmapList()
{
}

/**********************************************************************************************

>	GlobalBitmapList::~GlobalBitmapList()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GlobalBitmapList destructor

**********************************************************************************************/

GlobalBitmapList::~GlobalBitmapList()
{
}

/**********************************************************************************************

>	BOOL GlobalBitmapList::Init()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/95
	Returns:	FALSE if failed.
	Purpose:	Initialise the GlobalBitmapList

**********************************************************************************************/

BOOL GlobalBitmapList::Init()
{
//	GreyscaleBitmapList::Init();

#if !defined(EXCLUDE_FROM_RALPH)
	return OpDeleteBitmap::Init();
#else
	return TRUE;
#endif
}

/**********************************************************************************************

>	BOOL GlobalBitmapList::DeInit()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/95
	Returns:	FALSE if failed.
	Purpose:	DeInitialise the GlobalBitmapList

**********************************************************************************************/

BOOL GlobalBitmapList::DeInit()
{
	// First deinit the global bitmap list
	GlobalBitmapList* pBmpList = GetApplication()->GetGlobalBitmapList();

	ListItem* pItem = pBmpList->GetHead();

	while (pItem != NULL)
	{
		ListItem* pNext = pBmpList->GetNext(pItem);

		OILBitmap* pBitmap = (OILBitmap*)pItem;

		ERROR3IF(pBitmap->IsTemp(), "Warning.  Found temp bitmap in global bitmap list");

		if (pBitmap == OILBitmap::Default)
		{
			// Don't delete the default bitmap, just remove it from the list
			pBitmap->RemoveFromGlobalList();
		}
		else
		{
			delete pBitmap;		// This will remove it from the list before deleting it
		}

		pItem = pNext;
	}

	// Now deinit the greyscale bitmap list
//	GreyscaleBitmapList::DeInit();

	return TRUE;
}

/**********************************************************************************************

>	BOOL GlobalBitmapList::AddItem(OILBitmap *Item)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Returns:	TRUE
	Purpose:	Adds an item to the tail of the GlobalBitmaplist

**********************************************************************************************/

BOOL GlobalBitmapList::AddItem(OILBitmap *Item)
{
	ERROR3IF(Item == NULL, "Trying to add NULL item to Global Bitmap List");
	
	if (Item == NULL)
		return FALSE;

	// Is this Bitmap already in the list ?
	LISTPOS pos = FindPosition((ListItem*)Item);

	if (pos == NOT_IN_LIST || pos == EMPTY_LIST)
	{
		// It's not in the list yet, so add it on the end
		AddTail((ListItem*)Item);
		return TRUE;
	}
	else
	{
		// It's already in the list, so just return
		return FALSE;
	}
}

/**********************************************************************************************

>	ListItem* GlobalBitmapList::GetFirstBitmapName(String_256* BitmapName)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Returns:	The bitmap found at the head of the List, or NULL if none found.
	Purpose:	Find the names of the bitmaps in the list

**********************************************************************************************/

ListItem* GlobalBitmapList::GetFirstBitmapName(String_256* BitmapName)
{
	ListItem* First = GetHead();
	
	while (First != NULL)
	{
		if (!((OILBitmap*)First)->HasBeenDeleted())
		{
			*BitmapName = ((OILBitmap*)First)->GetName();
			return First;
		}

		First = GetNext(First);
	}

	return NULL;
}

/**********************************************************************************************

>	ListItem* GlobalBitmapList::GetNextBitmapName(ListItem* Prev, String_256* BitmapName)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Returns:	The next bitmap in the List, or NULL if none found.
	Purpose:	Find the names of the bitmaps in the list

**********************************************************************************************/

ListItem* GlobalBitmapList::GetNextBitmapName(ListItem* Prev, String_256* BitmapName)
{
	ListItem* Next = GetNext(Prev);
	
	while (Next != NULL)
	{
		if (!((OILBitmap*)Next)->HasBeenDeleted())
		{
			*BitmapName = ((OILBitmap*)Next)->GetName();
			return Next;
		}

		Next = GetNext(Next);
	}

	return NULL;
}

/**********************************************************************************************

>	OILBitmap* GlobalBitmapList::GetBitmapFromName(String_256* BitmapName)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Returns:	Ptr to Bitmap of the name given, or NULL if not found.
	Purpose:	Finds a bitmap with a particular name.

**********************************************************************************************/

OILBitmap* GlobalBitmapList::GetBitmapFromName(String_256* BitmapName)
{
	ListItem* pItem = GetHead();
	
	while (pItem != NULL)
	{
		OILBitmap* pBmp = (OILBitmap*)pItem;
		
		if (!pBmp->HasBeenDeleted())
		{
			if (*BitmapName == pBmp->GetName())
				return pBmp;
		}

		pItem = GetNext(pItem);
	}

	return NULL;
}

/**********************************************************************************************

>	void GlobalBitmapList::MakeNameUnique(String_256* BitmapName)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	Ensures a bitmap name is unique.

**********************************************************************************************/

void GlobalBitmapList::MakeNameUnique(String_256* BitmapName)
{
	ListItem* Bitmap = GetHead();

	INT32 Number = 1;
	String_256 Name = *BitmapName;

	while (Bitmap != NULL)
	{
		if ( Name == ((OILBitmap*)Bitmap)->GetName() )
		{
			// This name already exists, so lets make it unique
			String_256	Str;
			tsprintf( Str, 256, TEXT("%d"), Number++ );
			Name = (*BitmapName);
			Name += Str;

			Bitmap = GetHead();	// Start at the beginning again
		}
		else
		{
			Bitmap = GetNext(Bitmap);
		}
	}	

	*BitmapName = Name;
}


/**********************************************************************************************

>	OILBitmap* GlobalBitmapList::FindDuplicateBitmap(OILBitmap* pBitmap)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Purpose:	Looks to see if an identical bitmap is already in the bitmap list

**********************************************************************************************/

OILBitmap* GlobalBitmapList::FindDuplicateBitmap(OILBitmap* pOILBitmap)
{
	ListItem* pItem = GetHead();

	while (pItem != NULL)
	{
		OILBitmap* pBmp = (OILBitmap*)pItem;

		ERROR3IF(pBmp == pOILBitmap, "Why is this bitmap already in the list ?");

		if (pBmp != pOILBitmap &&
			*pBmp == *pOILBitmap)
		{
			return pBmp;
		}

		pItem = GetNext(pItem);
	}

	return NULL;
}


/**********************************************************************************************

>	INT32 GlobalBitmapList::GetDocumentBitmapCount(Document* pDoc)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Purpose:	Find how many bitmaps are being used by this document

**********************************************************************************************/

INT32 GlobalBitmapList::GetDocumentBitmapCount(Document* pDoc)
{
	INT32 Count = 0;
	INT32 DefaultIsUsed = FALSE;

	if (pDoc != NULL)
	{
		// Now get the actual bitmap list
		BitmapList* Bitmaps = pDoc->GetBitmapList();

		if (Bitmaps)
		{
			// Go through all the bitmaps in the list 
			ListItem *Ptr = Bitmaps->GetHead();
			while (Ptr != NULL)
			{
				KernelBitmap* Bmp = (KernelBitmap*)Ptr;

				if (Bmp->IsDefaultBitmap() && !DefaultIsUsed)
				{
					if (Bmp->IsUsedInDocument(pDoc))
						DefaultIsUsed = TRUE;
				}
				else if (Bmp->IsUsedInDocument(pDoc))
				{
					Count++;
				}
				
				Ptr = Bitmaps->GetNext(Ptr);
			}
		}
	}

	if (DefaultIsUsed) Count++;

	return Count;
}

/**********************************************************************************************

>	INT32 GlobalBitmapList::GetDocumentBitmapSize(Document* pDoc)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Purpose:	Find the total size of the bitmaps being used by this document

**********************************************************************************************/

INT32 GlobalBitmapList::GetDocumentBitmapSize(Document* pDoc)
{
	INT32 Mem = 0;
	INT32 DefaultIsUsed = FALSE;

	if (pDoc != NULL)
	{
		// Now get the actual bitmap list
		BitmapList* Bitmaps = pDoc->GetBitmapList();

		if (Bitmaps)
		{
			// Go through all the bitmaps in the list 
			ListItem *Ptr = Bitmaps->GetHead();
			while (Ptr != NULL)
			{
				KernelBitmap* Bmp = (KernelBitmap*)Ptr;

				if (Bmp->IsDefaultBitmap() && !DefaultIsUsed)
				{
					if (Bmp->IsUsedInDocument(pDoc))
						DefaultIsUsed = TRUE;
				}
				else if (Bmp->IsUsedInDocument(pDoc))
				{
					Mem += Bmp->ActualBitmap->GetBitmapSize();
				}
				
				Ptr = Bitmaps->GetNext(Ptr);
			}
		}
	}

	if (DefaultIsUsed) 
	{
		Mem += OILBitmap::Default->GetBitmapSize();
	}

	return Mem;
}

/**********************************************************************************************

>	BOOL GlobalBitmapList::IsUsedInDocument(Document * pDoc, OILBitmap* pOILBmp)

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/97
	Inputs:		pDoc	Document to check bitmap list of
				pOILBmp	the OIL bitmap to find in the list
	Returns:	True if the bitmap is found, False otherwise
	Purpose:	Checks if the specified OIL bitmap is in the bitmap list of the specified
				document.
	SeeAlso:	DeleteAllUnusedBitmaps;

**********************************************************************************************/

BOOL GlobalBitmapList::IsUsedInDocument(Document * pDoc, OILBitmap* pOILBmp)
{
	PORTNOTETRACE("other","GlobalBitmapList::IsUsedInDocument - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(pDoc == NULL || pOILBmp == NULL,FALSE,"GlobalBitmapList::IsUsedInDocument Bad params!");
	
	// Get the bitmap list for the specified document
	BitmapList* pBitmaps = pDoc->GetBitmapList();
	if (pBitmaps)
	{
		ListItem* pKBmp = pBitmaps->GetHead();

		while (pKBmp)
		{
			KernelBitmap* pBmp = (KernelBitmap*)pKBmp;

			if (pBmp->ActualBitmap == pOILBmp)
			{
				// Found a copy of the bitmap and so return TRUE
				return TRUE;
			}

			// Check whether this bitmap is an XPE master
			if (pBmp->ActualBitmap)
			{
				OILBitmap* pMaster = NULL;
				IXMLDOMDocumentPtr pInfo = NULL;
				pBmp->ActualBitmap->GetXPEInfo(pMaster, pInfo);
				if (pMaster == pOILBmp)
					return TRUE;
			}

			pKBmp = pBitmaps->GetNext(pKBmp);
		}
	}
#endif

	// Bitmap not found and so return FALSE
	return FALSE;
}

/**********************************************************************************************

>	void GlobalBitmapList::DeleteAllUnusedBitmaps()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Purpose:	Called when a document is deleted, to delete all the unused bitmaps

**********************************************************************************************/

void GlobalBitmapList::DeleteAllUnusedBitmaps()
{
	InternalClipboard::AttrsHaveChanged();
	// As we now have two clipboards, we need to sort out both
	InternalClipboard::Swap(); // Swap to alternative clipboard
	InternalClipboard::AttrsHaveChanged();
	InternalClipboard::Swap(); // Swap back to orignal clipboard

	ListItem* pItem = GetHead();
	
	while (pItem != NULL)
	{
		OILBitmap* pOILBmp = (OILBitmap*)pItem;

		ERROR3IF(pOILBmp->IsAFractal(), "There's a fractal in the global list !  How'd that happen ?");

		BOOL IsUsed = FALSE;

		if (pOILBmp == OILBitmap::Default)
		{
			IsUsed = TRUE;
		}
		// DY 16/3/2000, we don't want to delete bitmaps that are used by brushes when the
		// document they are in is deleted. 
		if (pOILBmp->IsUsedByBrush())
		{
			IsUsed = TRUE;
		}
		else
		{
			// Go though all the documents
			Document* pDoc = (Document*)GetApplication()->Documents.GetHead();
			while (pDoc != NULL)
			{
				if (IsUsedInDocument(pDoc, pOILBmp))
				{
					IsUsed = TRUE;
					break;
				}

				// Move onto the next document
				pDoc = (Document*)GetApplication()->Documents.GetNext(pDoc);
			}

#if !defined(EXCLUDE_FROM_RALPH)
			// Now check the ClipBoard to see if the Bitmap is in there
			if (!IsUsed && IsBitmapOnClipBoard(pOILBmp))
			{
				IsUsed = TRUE;
				//break;
			}
			
			// Should do this but will ERROR2 on exit as the clipboard has no doc components
			// Should be ok as we now clear out the bitmap lists when the clipboard is cleared
			/*if (!IsUsed)
			{
				Document * pClipDoc = InternalClipboard::Instance();
				if (pClipDoc && IsUsedInDocument(pClipDoc, pOILBmp))
				{
					IsUsed = TRUE;
				}
			} */

			// As we now have two clipboards, we need to check both
			// Now check the ClipBoard to see if the Bitmap is in there
			if (!IsUsed)
			{
				InternalClipboard::Swap(); // Swap to alternative clipboard
				if (IsBitmapOnClipBoard(pOILBmp))
				{
					IsUsed = TRUE;
					// break;
				}
				InternalClipboard::Swap(); // Swap back to orignal clipboard
			}

			// Should do this but will ERROR2 on exit as the clipboard has no doc components
			// Should be ok as we now clear out the bitmap lists when the clipboard is cleared
			/*if (!IsUsed)
			{
				Document * pClipDoc = InternalClipboard::Other();
				if (pClipDoc && IsUsedInDocument(pClipDoc, pOILBmp))
	.			{
					IsUsed = TRUE;
				}
			} */
#endif

		}

		// Get next item before we delete this item
		pItem = GetNext(pItem);

		if (!IsUsed)
		{
			delete pOILBmp;	// Delete if not used
			pOILBmp = NULL;
		}
	}
}

/**********************************************************************************************

>	BOOL GlobalBitmapList::IsBitmapOnClipBoard(OILBitmap* pOILBmp)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Purpose:	Check to see if a bitmap is used on the clipboard

**********************************************************************************************/

BOOL GlobalBitmapList::IsBitmapOnClipBoard(OILBitmap* pOILBmp)
{
#if !defined(EXCLUDE_FROM_RALPH)

	// Clear out any Cached attrs on the Clipboard
	Node* Subtree = (Node*) InternalClipboard::GetInsertionLayer();
	if (!Subtree) return FALSE;

	Range ClipRange(Subtree->FindFirstChild(),
					Subtree->FindLastChild(),
					RangeControl(TRUE,TRUE,TRUE,TRUE));

	// Go though all the nodes on the clipboard
	Node* Current = ClipRange.FindFirst();
	while (Current != NULL)
	{
		// And search right down each subtree
		Node* pNodeToCheck = Current->FindFirstDepthFirst();
	
		while (pNodeToCheck != NULL)
		{
			// See if this node uses the bitmap
			INT32 Count = 0;

			// Does this node have any bitmaps in it ?
			KernelBitmap* pKernelBmp = pNodeToCheck->EnumerateBitmaps(Count++);

			while (pKernelBmp != NULL)
			{
				// Found a bitmap reference, but ... is it the one we want ?
				if (pKernelBmp->GetActualBitmap() == pOILBmp)
				{
					return TRUE;
				}
				
				// Check whether this bitmap is an XPE master
				if (pKernelBmp->GetActualBitmap())
				{
PORTNOTE("other","Removed IXMLDOMDocumentPtr usage")
#ifndef EXCLUDE_FROM_XARALX
					OILBitmap* pMaster = NULL;
					IXMLDOMDocumentPtr pInfo = NULL;
					pKernelBmp->GetActualBitmap()->GetXPEInfo(pMaster, pInfo);
					if (pMaster == pOILBmp)
						return TRUE;
#endif
				}

				pKernelBmp = pNodeToCheck->EnumerateBitmaps(Count++);				
			}
			
			pNodeToCheck = pNodeToCheck->FindNextDepthFirst(Current);
		}

		Current = ClipRange.FindNext(Current);
	}

#endif

	return FALSE;
}

/**********************************************************************************************

>	GreyscaleBitmapList* GlobalBitmapList::GetGreyscaleBitmapList()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Purpose:	Get a pointer to the greyscale bitmap list

**********************************************************************************************/

//GreyscaleBitmapList* GlobalBitmapList::GetGreyscaleBitmapList()
//{
//	return &GreyscaleBitmaps;
//}


#ifdef _DEBUG

void GlobalBitmapList::DumpGlobalBitmapListToTrace()
{
	TRACEALL( _T("\nDumping Global bitmap list ...\n\n") );

	INT32 TotalSize = 0;
	INT32 Count = 0;
	
	ListItem* pItem = GetHead();
	
	while (pItem != NULL)
	{
		OILBitmap* pBmp = (OILBitmap*)pItem;

		if (pBmp->HasBeenDeleted())
		{
			TRACEALL( _T("OILBitmap @ %x, (Deleted)\n"), pBmp);
		}
		else
		{
			TRACEALL( _T("OILBitmap @ %x, '%s', Size=%d bytes"), pBmp, (TCHAR*)pBmp->GetName(), pBmp->GetBitmapSize());

			if (pBmp->IsGreyscale())
				TRACEALL( _T(", (Greyscale)") );

			TRACEALL( _T("\n") );
		
			TotalSize += pBmp->GetBitmapSize();
		}

		Count++;

		pItem = GetNext(pItem);
	}

	TRACEALL( _T("\n") );
	TRACEALL( _T("Total bitmaps = %d\n"), Count );
	TRACEALL( _T("Total size    = %d bytes\n\n"), TotalSize );
}

#endif



#if 0
/**********************************************************************************************

>	GreyscaleBitmapList::GreyscaleBitmapList()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GreyscaleBitmapList constructor

**********************************************************************************************/

GreyscaleBitmapList::GreyscaleBitmapList()
{
	m_CurrentSize = 0;
}

/**********************************************************************************************

>	GreyscaleBitmapList::~GreyscaleBitmapList()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GreyscaleBitmapList destructor

**********************************************************************************************/

GreyscaleBitmapList::~GreyscaleBitmapList()
{
}

/**********************************************************************************************

>	BOOL GreyscaleBitmapList::Init()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/95
	Returns:	FALSE if failed.
	Purpose:	Initialise the GreyscaleBitmapList

**********************************************************************************************/

BOOL GreyscaleBitmapList::Init()
{
	if (Camelot.DeclareSection("Screen", 1))
	{
		Camelot.DeclarePref( NULL, "MaxGreyscaleCacheSize", (UINT32*)&m_lMaxSize);
	}

	GreyscaleBitmapList* pGreyList = 
			GetApplication()->GetGlobalBitmapList()->GetGreyscaleBitmapList();

	pGreyList->SetMaxGreyscaleSize(m_lMaxSize);

	return TRUE;
}

/**********************************************************************************************

>	BOOL GreyscaleBitmapList::DeInit()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/95
	Returns:	FALSE if failed.
	Purpose:	DeInitialise the GreyscaleBitmapList

**********************************************************************************************/

BOOL GreyscaleBitmapList::DeInit()
{
	GreyscaleBitmapList* pGreyList = 
			GetApplication()->GetGlobalBitmapList()->GetGreyscaleBitmapList();

	ListItem* pItem = pGreyList->GetHead();

	while (pItem != NULL)
	{
		ListItem* pNext = pGreyList->GetNext(pItem);

		OILBitmap* pBitmap = (OILBitmap*)pItem;
		delete pBitmap;		// This will remove it from the list before deleting it

		pItem = pNext;
	}

	return TRUE;
}

/**********************************************************************************************

>	BOOL GreyscaleBitmapList::AddItem(OILBitmap *Item)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Returns:	TRUE
	Purpose:	Adds an item to the tail of the GreyscaleBitmaplist

**********************************************************************************************/

BOOL GreyscaleBitmapList::AddItem(OILBitmap *Item)
{
	ERROR3IF(Item == NULL, "Trying to add NULL item to Greyscale Bitmap List");
	if (Item == NULL)
		return FALSE;

	ERROR2IF(!Item->IsGreyscale(), FALSE, "Trying to add a non-grey bitmap to Greyscale Bitmap List");

	// Is this Bitmap already in the list ?
	LISTPOS pos = FindPosition((ListItem*)Item);

	if (pos == NOT_IN_LIST || pos == EMPTY_LIST)
	{
		// It's not in the list yet, so add it on the end
		AddTail((ListItem*)Item);

		TRACEUSER( "Will", _T("Added new Greyscale bitmap to list\n") );

		// Check we haven't gone over the maximum size for the list
		CheckSizeIsOk();

		return TRUE;
	}
	else
	{
		// It's already in the list, so just return
		return FALSE;
	}
}

/**********************************************************************************************

>	ListItem* GreyscaleBitmapList::RemoveItem(ListItem* Item)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/03/2004
	Returns:	TRUE
	Purpose:	Adds an item to the tail of the GreyscaleBitmaplist

**********************************************************************************************/

ListItem* GreyscaleBitmapList::RemoveItem(ListItem* Item)
{
	m_CurrentSize -= ((OILBitmap*)Item)->GetBitmapSize();
	return List::RemoveItem(Item);
}

void GreyscaleBitmapList::AddHead(ListItem* Item)
{
	List::AddHead(Item);
	m_CurrentSize += ((OILBitmap*)Item)->GetBitmapSize();
}

void GreyscaleBitmapList::AddTail(ListItem* Item)
{
	List::AddTail(Item);
	m_CurrentSize += ((OILBitmap*)Item)->GetBitmapSize();
}

ListItem* GreyscaleBitmapList::RemoveHead()
{
	ListItem* pItem = List::RemoveHead();

	if (pItem)
		m_CurrentSize -= ((OILBitmap*)pItem)->GetBitmapSize();

	return pItem;
}

ListItem* GreyscaleBitmapList::RemoveTail()
{
	ListItem* pItem = List::RemoveTail();

	if (pItem)
		m_CurrentSize -= ((OILBitmap*)pItem)->GetBitmapSize();

	return pItem;
}

LISTPOS GreyscaleBitmapList::InsertBefore(LISTPOS here, ListItem* item)
{
	ERROR3("Not implemented");
	return 0;
}

ListItem* GreyscaleBitmapList::InsertBefore(ListItem* here, ListItem* item)
{
	ERROR3("Not implemented");
	return NULL;
}

LISTPOS GreyscaleBitmapList::InsertAfter(LISTPOS here, ListItem* item)
{
	ERROR3("Not implemented");
	return 0;
}

ListItem* GreyscaleBitmapList::InsertAfter(ListItem* here, ListItem* item)
{
	ERROR3("Not implemented");
	return NULL;
}

/**********************************************************************************************

>	INT32 GreyscaleBitmapList::GetCurrentGreyscaleSize()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Purpose:	Gets the current total size of the list

**********************************************************************************************/

INT32 GreyscaleBitmapList::GetCurrentGreyscaleSize()
{	
/*	INT32 CurrentSize = 0;

	OILBitmap* pItem = (OILBitmap*)GetHead();

	while (pItem)
	{
		INT32 ItemSize = pItem->GetBitmapSize();
		CurrentSize += ItemSize;

		pItem = (OILBitmap*)GetNext(pItem);
	}

	return CurrentSize;
*/
	return m_CurrentSize;
}

/**********************************************************************************************

>	void GreyscaleBitmapList::SetMaxGreyscaleSize(INT32 NewMaxSize)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Purpose:	Sets the maximum size that the list can grow to

**********************************************************************************************/

void GreyscaleBitmapList::SetMaxGreyscaleSize(INT32 NewMaxSize)
{
	if (NewMaxSize < 0)
		NewMaxSize = 0;

	if (NewMaxSize > 16*1024*1024)		// 16MB should be plenty for a few years
		NewMaxSize = 16*1024*1024;

	m_lMaxSize = NewMaxSize;

	TRACEUSER( "Will", _T("Setting MaxGreyscaleSize to %d bytes\n"), m_lMaxSize );

	// And check the list is not bigger than the new size
	CheckSizeIsOk();
}

/**********************************************************************************************

>	INT32 GreyscaleBitmapList::GetMaxGreyscaleSize()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Purpose:	Gets the current maximum size that the list can grow to

**********************************************************************************************/

INT32 GreyscaleBitmapList::GetMaxGreyscaleSize()
{
	return m_lMaxSize;
}

/**********************************************************************************************

>	void GreyscaleBitmapList::CheckSizeIsOk()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Purpose:	Ensures the list is smaller than the max list size, and throws away
				bitmaps from the start of the list if not

**********************************************************************************************/

void GreyscaleBitmapList::CheckSizeIsOk()
{
	INT32 CurrentSize = GetCurrentGreyscaleSize();
	INT32 MaxSize = GetMaxGreyscaleSize();

	TRACEUSER( "Will", _T("CheckSizeIsOk, current size is %d bytes, max is %d bytes\n"), CurrentSize, MaxSize );

	if (CurrentSize > MaxSize)
	{
		// How many bytes do we need to remove ?
		INT32 SizeToRemove = CurrentSize - MaxSize;
		INT32 SizeRemoved = 0;

		TRACEUSER( "Will", _T("Trying to remove %d bytes from greyscale list\n"), SizeToRemove );

		OILBitmap* pItem = (OILBitmap*)GetHead();
		if (pItem == NULL)
			return;

		while (pItem && (SizeRemoved < SizeToRemove))
		{
			OILBitmap* pNextItem = (OILBitmap*)GetNext(pItem);

			INT32 ItemSize = pItem->GetBitmapSize();

			// Remove it from this list
			RemoveItem(pItem);

			// and delete it
			delete pItem;
			
			SizeRemoved += ItemSize;
			pItem = pNextItem;
		}

		TRACEUSER( "Will", _T("New size is %d bytes\n"), CurrentSize - SizeRemoved );
		ERROR3IF(SizeRemoved < SizeToRemove, "Couldn't free enough memory in Greyscale list");
	}
}



#ifdef _DEBUG

void GreyscaleBitmapList::DumpGreyscaleBitmapListToTrace()
{
	TRACEALL( _T("\nDumping Greyscale bitmap list ...\n\n") );

	INT32 TotalSize = 0;
	INT32 Count = 0;
	
	ListItem* pItem = GetHead();
	
	while (pItem != NULL)
	{
		OILBitmap* pBmp = (OILBitmap*)pItem;

		TRACEALL( _T("Grey bitmap @ %x, '%s', Size=%d bytes\n"), pBmp, (TCHAR*)pBmp->GetName(), pBmp->GetBitmapSize());
	
		TotalSize += pBmp->GetBitmapSize();
		Count++;

		pItem = GetNext(pItem);
	}

	TRACEALL( _T("\n"));
	TRACEALL( _T("Total bitmaps = %d\n"), Count);
	TRACEALL( _T("Total size    = %d bytes\n"), TotalSize);
	TRACEALL( _T("Max size      = %d bytes\n\n"), GetMaxGreyscaleSize());
}

#endif

#endif // 0




#if !defined(EXCLUDE_FROM_RALPH)

//-----------------------------------------------------------------------------------------------
// OpDeleteBitmap - Delete all references to a bitmap, and remove it from memory

/********************************************************************************************

>	BOOL OpDeleteBitmap::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpDeleteBitmap initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDeleteBitmap::Init()
{
	return (RegisterOpDescriptor(
	 							0,
	 							0,
								CC_RUNTIME_CLASS(OpDeleteBitmap),
	 							OPTOKEN_DELETEBITMAP,
	 							OpDeleteBitmap::GetState,
	 							0,	/* help ID */
	 							0,
	 							0	/* bitmap ID */));
}               
    
/********************************************************************************************

>	OpState	OpDeleteBitmap::GetState(String_256*, OpDescriptor*)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95
	Returns:	The state of the OpDeleteBitmap operation
	Purpose:	For finding the OpDeleteBitmap's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDeleteBitmap::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
 	return(OpSt);
}

/********************************************************************************************

>	void OpDeleteBitmap::DoDelete(OILBitmap* BmpToDelete)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95
	Inputs:		pOpParam: Param1 contains a pointer to the OILBitmap to delete.
	Purpose:	Frees the memory used by a bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDeleteBitmap::DoDelete(OILBitmap* pBmpToDelete)
{
	// Declared up 'ere, 'cus of the goto thingy
	Document* Doc;
	OILBitmap* poBmp = NULL;

	// Set the busy cursor . . .
	String_64 StatusText = 	String_64(_R(IDS_K_BMPLIST_DELBMPOPEN));
	StatusText +=	pBmpToDelete->GetName();
	StatusText += 	String_64(_R(IDS_K_BMPLIST_DELBMPCLOSE));

    BeginSlowJob(-1, FALSE, &StatusText);

	// Remember the current document
	Document* pCurrent  = Document::GetCurrent();

	ObjChangeFlags cFlags;
	cFlags.TransformNode = TRUE;	// The node is going to be changed to point at a
									// different bitmap, which is a Bit like transforming
									// it I suppose ?

	cFlags.Attribute = TRUE;		// Needed to make Blends re-calc themselves

	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	// Don't do anything if the bitmap is NULL, or if it's the default bitmap
	if (pBmpToDelete == NULL || pBmpToDelete == OILBitmap::Default)
		goto EndOperation;

	ERROR3IF(pBmpToDelete->HasBeenDeleted(), "This bitmap has been deleted already !");

	// -------------------------------------------------------------
	// If this bitmap is an XPE master bitmap then we must unlink
	poBmp = (OILBitmap*) GetApplication()->GetGlobalBitmapList()->GetHead();
	while (poBmp)
	{
		if (poBmp->m_pMasterBitmap == pBmpToDelete)
		{
			poBmp->RebuildXPEBitmap();		// Ensure that we have a generated bitmap
			poBmp->DestroyXPEInfo();		// Unlink this bitmap from the master being deleted
		}

		poBmp = (OILBitmap*) GetApplication()->GetGlobalBitmapList()->GetNext(poBmp);
	}

	// -------------------------------------------------------------
	// Go though all the documents
	Doc = (Document*)GetApplication()->Documents.GetHead();
	while (Doc != NULL)
	{
		// Set the document to be current, because the bitmap system needs this
		// so it knows which bitmap list to look at.
		Doc->SetCurrent();

		// First we look at the doc component bitmap list to see if the bitmap
		// we deleted is in there somewhere
		BOOL BitmapIsInList = FALSE;

		BitmapList* pBitmaps = Doc->GetBitmapList();
		if (pBitmaps != NULL)
		{
			KernelBitmap* pBitmap = (KernelBitmap*)pBitmaps->GetHead();
			while (pBitmap)
			{
				if (pBitmap->GetActualBitmap() == pBmpToDelete)
				{
					// We've found the bitmap, so flag thaat we need to 
					// scan the tree
					BitmapIsInList = TRUE;
					break;
				}

				pBitmap = (KernelBitmap*)pBitmaps->GetNext(pBitmap);
			}
		}

		if (BitmapIsInList)
		{
			// If the bitmap was found in the list somewhere, then we need to scan
			// though the document tree, and make sure we redraw any nodes that
			// use the bitmap

			BOOL DocModified = FALSE;

			// Scan the document's tree for bitmap references
			Node* pNode = Node::DocFindFirstDepthFirst(Doc);
			while (pNode != NULL)
			{
				// Ignore hidden nodes
				if (!pNode->IsNodeHidden())
				{
					INT32 Count = 0;

					// Does this node have any bitmaps in it ?
					KernelBitmap* pBitmap = pNode->EnumerateBitmaps(Count++);

					while (pBitmap != NULL)
					{
						// Found a bitmap reference, but ... is it the one that we deleted ?
						if (pBitmap->GetActualBitmap() == pBmpToDelete)
						{
							// pBmpToDelete is being deleted, so we MUST reset any bitmapref
							// that references it to the default (otherwise we still reference the
							// deleted bitmap) ....
							if (IS_A (pNode, NodeBitmap))
							{
								// i'm only willing to do this for NodeBitmaps!  Not sure about
								// any other type of node that overides the EnumerateBitmaps function
								// (they look scary).
								NodeBitmap* pBitmap = (NodeBitmap*) pNode;
								pBitmap->GetBitmapRef ()->SetBitmap (pBitmaps->FindDefaultBitmap());
							}
							
							// Make sure the node is redrawn
							ForceRedrawOfNode(Doc, pNode, &ObjChange);
							DocModified = TRUE;
						}
						
						pBitmap = pNode->EnumerateBitmaps(Count++);				
					}
				}

				// Move onto the next node in the tree
				pNode = pNode->DocFindNextDepthFirst(); 
			}

			if (DocModified)
			{
				// Flag that the document has changed
				Doc->SetModified(TRUE);
			}
		}

		// Move onto the next document
		Doc = (Document*)GetApplication()->Documents.GetNext(Doc);
	}

	// Now delete the bitmap data
	pBmpToDelete->DeleteData();

	// and make sure all the bitmap list are updated ...
	// (this needs to be done after the bitmap data is deleted)
	Doc = (Document*)GetApplication()->Documents.GetHead();
	while (Doc != NULL)
	{
		BROADCAST_TO_ALL(BitmapListChangedMsg(Doc->GetBitmapList())); 
		Doc = (Document*)GetApplication()->Documents.GetNext(Doc);
	}

	// Restore the current document
	if (pCurrent)
		pCurrent->SetCurrent();

EndOperation:
	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,NULL);
	UpdateAllChangedNodes(&ObjChange);

	EndSlowJob();
	End();
}

/********************************************************************************************

>	void OpDeleteBitmap::ForceRedrawOfNode(Document* Doc, Node* pNodeToRedraw, 
									   ObjChangeParam* pObjChange)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/95
	Inputs:		-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDeleteBitmap::ForceRedrawOfNode(Document* Doc, Node* pNodeToRedraw, 
									   ObjChangeParam* pObjChange)
{

	/*BOOL Allowed =*/ pNodeToRedraw->AllowOp(pObjChange);
	// Ignore the Allowed state, do it regardless ...

	// Force a redraw on the object and it's parent.
	Node* pParent = pNodeToRedraw;
	Spread* pSpread = pNodeToRedraw->FindParentSpread();

	while (pSpread != NULL && pParent != NULL)
	{
		Node* pPrevParent = pParent->FindParent();

		if (pPrevParent && pPrevParent->IsLayer())
		{
			DocRect Bounds = 
				((NodeRenderableBounded*)pParent)->GetBoundingRect();
			Doc->ForceRedraw(pSpread, Bounds, FALSE, pParent);
		}

		pParent = pPrevParent;
	}
}

#endif
