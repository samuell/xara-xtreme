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
#include "bmpcomp.h"
//#include "bmplist.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "bitmap.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bitmpinf.h"
#include "bmpsrc.h"
#include "ccbuffil.h"
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "will.h"
#include "nativeps.h"		// The old style EPS native filter, used in v1.1

#include "clipint.h"
#include "nodebmp.h"
//#include "progress.h"

#include "toollist.h"
#include "objchge.h"
#include "csrstack.h"

#include "clipint.h"
#include "ophist.h"
#include "sgliboil.h"
//#include "will2.h"

#include "expbmp.h"		// ExportedBitmaps handling class
#include "impbmp.h"		// ImportedBitmaps handling class
//#include "camfiltr.h"	// BaseCamelotFilter - version 2 native filter - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"	// TAG_DEFINEBITMAP_****
#include "cxfile.h"		// CXF_UNKNOWN_SIZE
//#include "cxfrech.h"	// CXaraFileRecordHandler - in camtypes.h [AUTOMATICALLY REMOVED]
#include "expcol.h"		// ExportedColours handling class
//#include "cxfrec.h"		// CXaraFileRecord handler - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tim.h"		// _R(IDT_CANT_FIND_FILTER)
//#include "filtrres.h"	// _R(IDS_DONT_SUPPORT_BITMAP_TYPE) _R(IDS_NATIVE_BITMAPWARNING)
#include "bitfilt.h"	// BaseBitmapFlter
//#include "fixmem.h"		// CCMalloc, CCFree - in camtypes.h [AUTOMATICALLY REMOVED]

CC_IMPLEMENT_MEMDUMP( BitmapList, List )
CC_IMPLEMENT_DYNAMIC( BitmapListComponentClass, DocComponentClass )
CC_IMPLEMENT_DYNAMIC( BitmapListComponent, DocComponent )

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/**********************************************************************************************

>	BitmapList::BitmapList()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	BitmapList constructor

**********************************************************************************************/

BitmapList::BitmapList()
{
	ParentDoc = NULL;				// Document containing this list
}

/**********************************************************************************************

>	BitmapList::~BitmapList()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	BitmapList destructor

**********************************************************************************************/

BitmapList::~BitmapList()
{
	ListItem* pItem = GetHead();
	ListItem* pNextItem;

	while (pItem)
	{
		pNextItem = GetNext(pItem);

		KernelBitmap* pBmp = (KernelBitmap*)pItem;

		ERROR3IF(pBmp->GetParentBitmapList() != this, "Bad ParentList ptr found in bitmap (during BitmapList destructor)");
		
		// if this bitmap is used by a brush we don't want to delete it here.
		if (!pBmp->IsUsedByBrush())
			delete pBmp;	// This should remove it from the list first
		else
		{
			pBmp->Detach();
			RemoveItem(pBmp);
		}

		pItem = pNextItem;
	}

	// Tidy up the global bitmap list
	Camelot.GetGlobalBitmapList()->DeleteAllUnusedBitmaps();
}

/**********************************************************************************************

>	void BitmapList::Init(BaseDocument *ParentDocument)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		ParentDocument - the parent document of this Bitmap list
	Purpose:	BitmapList initialiser. MUST be called after constructing a BitmapList
				in order to correctly initialise it. Note that if ParentDocument is not
				set to a useful value, some operations on Bitmap lists will fail.

**********************************************************************************************/

void BitmapList::Init(BaseDocument *ParentDocument)
{
	ERROR3IF(ParentDocument == NULL, "NULL ParentDoc in BitmapList::Init!\n");

	// Ensure the default bitmap is always in the list.
	KernelBitmap* Default = new KernelBitmap();
	Default->Attach(this);

	ParentDoc = ParentDocument;
}

/**********************************************************************************************

>	BOOL BitmapList::AddItem(KernelBitmap *Item)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Returns:	TRUE
	Purpose:	Adds an item to the tail of the Bitmaplist, and incremeants it's usage count.
				(Unless it's already in the list, in which case it's usage count is just
				bumped up).

**********************************************************************************************/

BOOL BitmapList::AddItem(KernelBitmap *Item)
{
	ERROR3IF(Item == NULL, "Trying to add NULL item to Bitmap List");
	
	if (Item == NULL)
		return FALSE;

	// Is this Bitmap already in the list ?
	LISTPOS pos = FindPosition((ListItem*)Item);

	if (pos == NOT_IN_LIST || pos == EMPTY_LIST)
	{
		// It's not in the list yet, so add it on the end
		AddTail((ListItem*)Item);

		if (ParentDoc != NULL)
		{
			BROADCAST_TO_ALL(BitmapListChangedMsg(this)); 
		}

		return TRUE;
	}
	else
	{
		// It's already in the list, so just return
		// (It's usage count has already been incremeanted)
		return FALSE;
	}
}

/**********************************************************************************************

>	BOOL BitmapList::RemoveItem(KernelBitmap *Item)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Returns:	TRUE
	Purpose:	Removes an item from the list.

**********************************************************************************************/

BOOL BitmapList::RemoveItem(KernelBitmap *Item)
{
	ERROR3IF(Item == NULL, "Trying to remove NULL item from Bitmap List");
	
	if (Item == NULL)
		return FALSE;

	// Is this Bitmap in the list ?
	LISTPOS pos = FindPosition((ListItem*)Item);

	if (pos != NOT_IN_LIST || pos == EMPTY_LIST)
	{
		// It's not in the list yet, so add it on the end
		List::RemoveItem((ListItem*)Item);
		Item->m_pParentList = NULL;

		if (ParentDoc != NULL)
		{
			BROADCAST_TO_ALL(BitmapListChangedMsg(this)); 
		}

		return TRUE;
	}
	else
	{
		// It's not in the list, so just return
		return FALSE;
	}
}

/**********************************************************************************************

>	BOOL BitmapList::MoveBefore(KernelBitmap *InsertPoint, KernelBitmap *ItemToMove)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Returns:	TRUE
	Purpose:	Re-arranges the bitmaps in the list.

**********************************************************************************************/

BOOL BitmapList::MoveBefore(KernelBitmap *InsertPoint, KernelBitmap *ItemToMove)
{
	List::RemoveItem((ListItem*)ItemToMove);
	List::InsertBefore((ListItem*)InsertPoint, (ListItem*)ItemToMove);

	if (ParentDoc != NULL)
	{
		BROADCAST_TO_ALL(BitmapListChangedMsg(this)); 
	}

	return TRUE;
}

/**********************************************************************************************

>	BOOL BitmapList::MoveAfter(KernelBitmap *InsertPoint, KernelBitmap *ItemToMove)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Returns:	TRUE
	Purpose:	Re-arranges the bitmaps in the list.

**********************************************************************************************/

BOOL BitmapList::MoveAfter(KernelBitmap *InsertPoint, KernelBitmap *ItemToMove)
{
	List::RemoveItem((ListItem*)ItemToMove);
	List::InsertAfter((ListItem*)InsertPoint, (ListItem*)ItemToMove);

	if (ParentDoc != NULL)
	{
		BROADCAST_TO_ALL(BitmapListChangedMsg(this)); 
	}

	return TRUE;
}

/**********************************************************************************************

>	BaseDocument *BitmapList::GetParentDocument(void)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Returns:	The document in which this Bitmap list resides
	Purpose:	To find the parent document of this Bitmap list

**********************************************************************************************/

BaseDocument *BitmapList::GetParentDocument(void)
{
	ERROR3IF(ParentDoc == NULL, "Uninitialised BitmapList detected!");
	return(ParentDoc);
}

/**********************************************************************************************

>	KernelBitmap* BitmapList::FindDuplicateBitmap(OILBitmap* pBitmap)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Purpose:	Looks to see if an identical bitmap is already in the bitmap list

**********************************************************************************************/

KernelBitmap* BitmapList::FindDuplicateBitmap(OILBitmap* pOILBitmap)
{
	ListItem* pItem = GetHead();
	while (pItem != NULL)
	{
		KernelBitmap* pBmp = (KernelBitmap*)pItem;

		if (pBmp->ActualBitmap &&
			pBmp->ActualBitmap != pOILBitmap &&
			*pBmp->ActualBitmap == *pOILBitmap)
		{
			return pBmp;
		}

		pItem = GetNext(pItem);
	}

	return NULL;
}

/**********************************************************************************************

>	KernelBitmap* BitmapList::FindDefaultBitmap()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/96
	Purpose:	Finds the default bitmap in this bitmap list

**********************************************************************************************/

KernelBitmap* BitmapList::FindDefaultBitmap()
{
	ListItem* pItem = GetHead();
	while (pItem != NULL)
	{
		KernelBitmap* pBmp = (KernelBitmap*)pItem;

		if (pBmp->ActualBitmap &&
			pBmp->ActualBitmap == OILBitmap::Default)
		{
			return pBmp;
		}

		pItem = GetNext(pItem);
	}

	return NULL;
}


/********************************************************************************************

>	BOOL BitmapListComponentClass::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Returns:	TRUE if all went well;
				FALSE if not.
	Purpose:	Register the Bitmap list document component with the main application.
	Errors:		Out of memory.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL BitmapListComponentClass::Init()
{
	// Instantiate a component class to register with the application.
	BitmapListComponentClass *pClass = new BitmapListComponentClass;
	if (pClass == NULL)
		return FALSE;

	// Register it
	GetApplication()->RegisterDocComponent(pClass);

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL BitmapListComponentClass::AddComponent(BaseDocument *pDocument)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pDocument - the document to add the Bitmap list to.
	Returns:	TRUE if the Bitmap list was added ok;
				FALSE if not.
	Purpose:	Add a Bitmap list component to the specified document.
	Errors:		Out of memory
	SeeAlso:	BitmapListComponentClass

********************************************************************************************/

BOOL BitmapListComponentClass::AddComponent(BaseDocument *pDocument)
{
	// Check to see if this document already has a Bitmap list; if so, leave it alone.
	if (pDocument->GetDocComponent(CC_RUNTIME_CLASS(BitmapListComponent)) != NULL)
		return TRUE;

	// No Bitmap list - try to create a new one for this document.
	BitmapList *pList = new BitmapList;
	if(pList == NULL)
		return FALSE;

	pList->Init(pDocument);		// Let the BitmapList know what its parent document is

	// Ok - create the Bitmap list component using this list.
	BitmapListComponent *pComponent = new BitmapListComponent(pList);
	if (pComponent == NULL)
	{
		// Out of memory...
		delete pList;
		return FALSE;
	}

	// All ok - add the component to the document.
	pDocument->AddDocComponent(pComponent);
	return TRUE;
}



/********************************************************************************************

>	BitmapListComponent::BitmapListComponent()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	Construct a Bitmap list component. DO NOT use this constructor. It gives
				and ERROR3 (and sets up this in a semi-safe default state)
	SeeAlso:	BitmapListComponent

********************************************************************************************/

BitmapListComponent::BitmapListComponent()
{
	ERROR3("BitmapListComponent constructed with default constructor!?\n");
}



/********************************************************************************************

>	BitmapListComponent::BitmapListComponent(BitmapList *pBmpList)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pColList - the Bitmap list to use for this Bitmap list componet.
	Purpose:	Construct a Bitmap list component using the given Bitmap list.
	Errors:		ENSURE if pColList is NULL.
	SeeAlso:	BitmapListComponent

********************************************************************************************/

BitmapListComponent::BitmapListComponent(BitmapList *pBmpList)
{
	ENSURE(pBmpList != NULL, "NULL Bitmap list in Bitmap list component constructor!");

	// Install this Bitmap list.
	pBitmaps = pBmpList;

	// Initialise Exporter bitmap list
	pExportedBitmaps = NULL;

	// Initialise Importer bitmap list
	pImportedBitmaps = NULL;

	// Our link to the BaseCamelotFilter
	pCamFilter = NULL;
	// Set our variable so that we warn about a possible replacement only once
	WarnedBefore = FALSE;

	// Initialise Exporter bitmap list (used for EPS only)
	pBitmapsToExport = NULL;
}

/********************************************************************************************

>	BitmapListComponent::~BitmapListComponent()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	Clean up a Bitmap list component's data structures - deletes the Bitmap
				list.
	Errors:		-
	SeeAlso:	BitmapListComponent

********************************************************************************************/

BitmapListComponent::~BitmapListComponent()
{
	// Delete our Bitmap list.
	delete pBitmaps;

	// As we have finished with it now, destroy the list of imported bitmaps
	if (pImportedBitmaps)
	{
		delete pImportedBitmaps;
		pImportedBitmaps = NULL;
	}

	// As we have finished with it now, destroy the list of exported bitmaps
	if (pExportedBitmaps)
	{
		delete pExportedBitmaps;
		pExportedBitmaps = NULL;
	}

	if (pBitmapsToExport)
	{
		delete pBitmapsToExport;
		pBitmapsToExport = NULL;
	}
}



/********************************************************************************************

>	BOOL BitmapListComponent::EPSStartImport(EPSFilter *pFilter)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
	Purpose:	Inform the Bitmap list document component that an EPS import is about to 
				start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL BitmapListComponent::EPSStartImport(EPSFilter *pFilter)
{
	return FALSE;
}

/********************************************************************************************

>	void BitmapListComponent::EPSEndImport(EPSFilter *pFilter, BOOL Success)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.
	Purpose:	Inform the Bitmap list document component that an EPS import has just 
				finished.
	SeeAlso:	DocComponent

********************************************************************************************/

void BitmapListComponent::EPSEndImport(EPSFilter *pFilter, BOOL Success)
{
}

/********************************************************************************************

>	BOOL BitmapListComponent::EPSStartExport(EPSFilter *pFilter)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
	Purpose:	Inform the Bitmap list document component that an EPS export is about to 
				start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL BitmapListComponent::EPSStartExport(EPSFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR3IF(pBitmapsToExport != NULL, "Why is there an export bitmap list already ?");
	
	// Build the list of bitmaps to export here ...
	pBitmapsToExport = new ExportedBitmaps();

	ERROR3IF(pBitmapsToExport == NULL, "Couldn't create bitmap export list !!");
	if (pBitmapsToExport == NULL)
		return FALSE;

	Document* ParentDoc = (Document*)pBitmaps->GetParentDocument();
	ERROR3IF(ParentDoc == NULL, "Bitmap list has no parent document !!");
	if (ParentDoc == NULL)
	{
		delete pBitmapsToExport;
		pBitmapsToExport = NULL;
		return FALSE;
	}

	UINT32 BitmapNumber = 0;

	// Scan the document's tree for bitmap references
	Node* pNode = Node::DocFindFirstDepthFirst(ParentDoc);
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
				// Check for a deleted bitmap
				if (pBitmap->HasBeenDeleted())
				{
					ERROR2IF(pBitmap->GetParentBitmapList() == NULL, 0L, "Deleted bitmap has no parent list");
					
					// Use the default bitmap instead
					pBitmap = pBitmap->GetParentBitmapList()->FindDefaultBitmap();

					// There should always be a default bitmap at the start of the list
					ERROR2IF(pBitmap == NULL, 0L, "Couldn't find the default bitmap");
				}

				// Found a bitmap reference, so ...
				// .. if this bitmap is not in the export list, add it
				if (pBitmapsToExport->GetBitmap(pBitmap) == NULL)
					pBitmapsToExport->AddBitmap(pBitmap, BitmapNumber++);
				
				pBitmap = pNode->EnumerateBitmaps(Count++);				
			}
		}

		// Move onto the next node in the tree
		pNode = pNode->DocFindNextDepthFirst(); 
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	void BitmapListComponent::EPSEndExport(EPSFilter *pFilter, BOOL Success)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.
	Purpose:	Inform the Bitmap list document component that an EPS export has just 
				finished.
	SeeAlso:	DocComponent

********************************************************************************************/

void BitmapListComponent::EPSEndExport(EPSFilter *pFilter)
{
#ifdef DO_EXPORT
	if (pBitmapsToExport)
	{
		delete pBitmapsToExport;
		pBitmapsToExport = NULL;
	}
#endif
}

/********************************************************************************************

>	BOOL BitmapListComponent::WriteEPSComments(EPSFilter *pFilter)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pFilter - the filter being used to import the EPS file.
	Returns:	TRUE if the Bitmap table was written successfully;
				FALSE if not.
	Purpose:	Writes out the Bitmap table in the form of EPS comments in the 'header'
				comments of the EPS file.
	Errors:		Out of disk space, and similar disk errors.
	SeeAlso:	DocComponent::WriteEPSComments;
				DocComponent

********************************************************************************************/

BOOL BitmapListComponent::WriteEPSComments(EPSFilter *pFilter)
{
	return TRUE;
}


/********************************************************************************************

>	BOOL BitmapListComponent::WriteScript(EPSFilter* pFilter)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Returns:	TRUE if the Script for this component was written out successfully;
				FALSE if not (e.g. out of disk space etc)
	Purpose:	This is called after all the Comments, Prolog etc have been output so that
				DocComponents get a chance to do something right at the start of the real
				EPS data, but before the tree starts to be exported.
				(The base class does nothing)
	SeeAlso:	DocComponent::WriteEPSSetup; 
				DocComponent::WriteEPSProlog;
				DocComponent::ProcessEPSComment; 
				DocComponent; 
				EPSFIlter

********************************************************************************************/

BOOL BitmapListComponent::WriteScript(EPSFilter* pFilter)
{
#ifdef DO_EXPORT
	// This only happens in Native EPS
	if (pFilter->IS_KIND_OF(CamelotNativeEPSFilter))
	{
		// Find out the context to save to
		EPSExportDC *pDC = pFilter->GetExportDC();

		// Make sure that there's a bitmap list to export.
		if (pBitmapsToExport == NULL)
		{
			ERROR3 ( "Why isn't there an export bitmap list (in WriteScript) ?" );
			return FALSE;
		}

		// Something to mark the start of the bitmap pool
		pDC->OutputToken(_T("%%BeginCamelotBitmapPool"));
		pDC->OutputNewLine();

		// Tell the filter we are starting to save the bitmap pool
		((CamelotNativeEPSFilter*)pFilter)->IsSavingBitmapPool = TRUE;

		// Find the number of Bitmaps
		INT32 TotalBitmaps = pBitmapsToExport->GetCount();

		// Output a comment saying how many bitmaps we have
		pDC->OutputToken(_T("%%BitmapPoolCount"));
		pDC->OutputValue(TotalBitmaps);
		pDC->OutputNewLine();

		INT32 BitmapNum = 0;

		// Find the first bitmap in the list again
		ExportBitmap* pExportBitmap = pBitmapsToExport->EnumerateBitmaps();

		while (pExportBitmap != NULL)
		{
			ERROR3IF(pExportBitmap->RecordNumber != BitmapNum, "Bad bitmap number found when saving bitmap pool");
				
			// Save the bitmap out
			ExportBitmapPoolItem(BitmapNum, pFilter, pDC, pExportBitmap->pBmp);

			BitmapNum++;

			// Find the next one
			pExportBitmap = pBitmapsToExport->EnumerateBitmaps(pExportBitmap);
		}

		// Tell the filter we are starting to save the bitmap pool
		((CamelotNativeEPSFilter*)pFilter)->IsSavingBitmapPool = FALSE;

		// Something to mark the start of the bitmap pool
		pDC->OutputToken(_T("%%EndCamelotBitmapPool"));
		pDC->OutputNewLine();
	}

#endif
	return TRUE;
}



/********************************************************************************************

>	BOOL BitmapListComponent::ExportBitmapPoolItem(INT32 BitmapNum, EPSFilter* pFilter,
												   EPSExportDC* pDC, KernelBitmap* pBitmap)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95
	Inputs:		BitmapNum - The number of the Kernel bitmap in the pool
				pFilter - The filter that things are being saved in
				pDC - The DC that the saving is happening in
				pBitmap - The Kernel Bitmap to be saved
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Does the actual saving for a bitmap pool item. This uses the cbmp token to
				represent the Camelot BitMap Pool object.

********************************************************************************************/

BOOL BitmapListComponent::ExportBitmapPoolItem(INT32 BitmapNum, EPSFilter* pFilter, EPSExportDC* pDC, KernelBitmap* pBitmap)
{
#ifdef DO_EXPORT
	ERROR2IF(pBitmap->HasBeenDeleted(), FALSE, "Trying to export a deleted bitmap");
	
	if (pFilter->IS_KIND_OF(CamelotNativeEPSFilter))
	{
		// ...then the bitmap object token.
		pDC->OutputValue(BitmapNum);			// Output the number of the bitmap
		pDC->OutputValue(INT32(0));				// Bitmap pool type (ready for expansion)
		pDC->OutputToken(_T("cbmp"));				// Bitmap pool token
		pDC->OutputNewLine();
		
		// ...and then the bitmap data itself.
		pFilter->ExportBitmap(*pBitmap);

		// Tell caller we rendered ourselves ok
		return TRUE;
	}
#endif
	return FALSE;
}


/********************************************************************************************

>	ProcessEPSResult BitmapListComponent::ProcessEPSComment(EPSFilter *pFilter, 
														    const char *pComment)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
				pComment - pointer to the comment to process.
	Returns:	EPSCommentUnknown	 - This EPS comment is not recognised by the document
									   component.
				EPSCommentError		 - This EPS comment was recognised by this document
									   component, but it contained an error.
				EPSCommentOK		 - This EPS comment was recognised as a legal comment by 
									   this document component, and was processed
									   successfully.
	Purpose:	Process an EPS comment - if this is a Bitmap table comment, this component
				will claim it and try to decode it.
	Errors:		Badly formed EPS comment.
	SeeAlso:	DocComponent::ProcessEPSComment

********************************************************************************************/

ProcessEPSResult BitmapListComponent::ProcessEPSComment(EPSFilter *pFilter, 
														const char *pComment)
{
	// Ignore all comments
	return EPSCommentUnknown;
}




/********************************************************************************************

>	void BitmapListComponent::EndOfEPSComment(EPSFilter *pFilter)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
	Purpose:	Informs the document component that the comment it has been decoding has
				now finished, and it should take whatever action required to act on
				the comment.
	SeeAlso:	DocComponent::EndOfPESComment

********************************************************************************************/

void BitmapListComponent::EndOfEPSComment(EPSFilter *pFilter)
{
	// Do any cleanup we need to here after loading in a Bitmap table...
}



/********************************************************************************************

>	INT32 BitmapListComponent::GetSizeOfExport(Filter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		pFilter - the export filter that will be used.
	Returns:	An approximation of the size of data that will be exported by this
				bitmap list, in terms of nodes. (see NodeRenderable::GetSizeOfExport).
	Purpose:	Find out how many nodes will be exported when this bitmap list is exported.
				The node can use the filter to find out how it will be exported, e.g.
				bitmap lists should return a value that reflects whether or not bitmap
				indirection is possible with this filter and so on.
				NB. This is virtual - the default implementation just returns 1 - only
					override if this is not accurate.
	SeeAlso:	Node::NeedsToExport; Node::ExportRender; NodeRenderable::GetSizeOfExport

********************************************************************************************/

INT32 BitmapListComponent::GetSizeOfExport(Filter *pFilter)
{
#ifdef DO_EXPORT
	// Check if this filter supports bitmap pools
	if (pFilter->GetBitmapSupportLevel() != IndirectedBitmapSupport)
		// No - just return 1.
		return 1;

	ERROR3IF(pBitmapsToExport == NULL, "Why isn't there an export bitmap list (in GetSizeOfExport) ?");
	if (pBitmapsToExport == NULL)
		return 1;

	INT32 TotalScanlines = 0;

	// Find the first bitmap in the list again
	ExportBitmap* pExportBitmap = pBitmapsToExport->EnumerateBitmaps();

	while (pExportBitmap != NULL)
	{
		KernelBitmap* pBitmap = pExportBitmap->pBmp;

		// Get information on this bitmap.
		BitmapInfo Info;
		if (!pBitmap->ActualBitmap->GetInfo(&Info))
			// Failed for some reason - bodge the answer.
			return 1;

		// Add the number of scanlines to our total
		TotalScanlines += Info.PixelHeight;

		// Find the next one
		pExportBitmap = pBitmapsToExport->EnumerateBitmaps(pExportBitmap);
	}
	
	// Return size to caller.
	if (TotalScanlines == 0)
		return 1;
	else
		return TotalScanlines;
#else
	return 0;
#endif
}



/********************************************************************************************

>	virtual BOOL BitmapListComponent::StartComponentCopy();

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if it succeeded. FALSE if the copy must be aborted
	Purpose:	This function gets called to prepare for a copy of Bitmaps into
				the document. It sets up the various arrays and other thangs which the
				Bitmap copy will need.
	Errors:		ENSURE failure if called when a component copy is already under way.
				Errors will be reported if memory is unavailable
	SeeAlso:	BitmapListComponent::EndComponentCopy;
				BitmapListComponent::AbortComponentCopy;
				BitmapListComponent::CopyBitmapAcross

********************************************************************************************/

BOOL BitmapListComponent::StartComponentCopy()
{
	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL BitmapListComponent::EndComponentCopy();

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if it succeeded
	Purpose:	Will commit all changes made by doing the component copy, returning TRUE.
	Errors:		-
	Notes:		May be called multiple times - subsequent calls will be ignored

	SeeAlso:	BitmapListComponent::StartComponentCopy;
				BitmapListComponent::AbortComponentCopy;
				BitmapListComponent::CopyBitmapAcross

********************************************************************************************/

BOOL BitmapListComponent::EndComponentCopy()
{
	return(TRUE);
}


/********************************************************************************************

>	virtual void BitmapListComponent::AbortComponentCopy();

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Will abort all changes made for component copies.
				This means that things such as the Bitmaps referenced by the nodes
				you have just copied do not exist, so you must clean up to the state
				the document was in before you strated copying.
	Errors:		-
	Notes:		May be called even if StartComponentCopy has not been called.
				May be called multiple times
	SeeAlso:	BitmapListComponent::EndComponentCopy;
				BitmapListComponent::AbortComponentCopy;
				BitmapListComponent::CopyBitmapAcross

********************************************************************************************/

void BitmapListComponent::AbortComponentCopy()
{
}


/********************************************************************************************
********************************************************************************************/

// Remove the following code on all builds but those who want it

/********************************************************************************************
********************************************************************************************/

/********************************************************************************************

>	BOOL BitmapListComponent::StartImport(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
	Returns:	TRUE if the component was able to prepare for importing;
				FALSE if not (e.g. out of memory)
	Purpose:	Inform the bitmap list document component that a Native or Web import is
				about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL BitmapListComponent::StartImport(BaseCamelotFilter *pFilter)
{
TRACEUSER( "Neville", _T("BitmapListComponent::StartImport\n") );
	if (pFilter == NULL)
	{
		ERROR3("BitmapListComponent::StartExport filter is null!");
		return TRUE;
	}

	// Save this in our link to the BaseCamelotFilter
	pCamFilter = pFilter;
	// Set our variable so that we warn about a possible replacement only once
	WarnedBefore = FALSE;

	// Set up and start the imported bitmap list
	pImportedBitmaps = new ImportedBitmaps();
	if (pImportedBitmaps == NULL)
		return FALSE;

	return TRUE;
}

/********************************************************************************************

>	BOOL BitmapListComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.
	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)
	Purpose:	Inform the bitmap list document component that a Native or Web import has
				just finished.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL BitmapListComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)
{
TRACEUSER( "Neville", _T("BitmapListComponent::EndImport\n") );
	if (pFilter == NULL)
	{
		ERROR3("BitmapListComponent::EndImport filter is null!");
		return TRUE;
	}

	// As we have finished with it now, destroy the list of imported bitmaps
	if (pImportedBitmaps)
	{
		delete pImportedBitmaps;
		pImportedBitmaps = NULL;
	}

	// Null our link to the BaseCamelotFilter
	pCamFilter = NULL;
	// Set our variable so that we warn about a possible replacement only once
	WarnedBefore = FALSE;

	return TRUE;
}

/********************************************************************************************

>	KernelBitmap* BitmapListComponent::GetReadBitmapReference(INT32 BitmapRef)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		BitmapRef	- The bitmap reference or record number that the record handler has loaded
							from the record.
	Outputs:	-
	Returns:	pBitmap	- A pointer to return the required kernel bitmap
	Purpose:	Try and convert a bitmap reference i.e. record number into a previously
				imported bitmap definition which should now be an kernel bitmap in the document.
	SeeAlso:	- 

********************************************************************************************/

KernelBitmap* BitmapListComponent::GetReadBitmapReference(INT32 BitmapRef)
{
TRACEUSER( "Neville", _T("GetReadBitmapReference for ref %d\n"), BitmapRef );

	// First check to see if the bitmap refers to any of the built in bitmaps.
	// These will have negative record numbers
	KernelBitmap* pBitmap = NULL;
	if (BitmapRef <= 0)
	{
		// The only one that exists at present is the default bitmap
		if (REF_DEFAULTBITMAP_BUILTIN)
		{
			// Its the default one so just construct a new kernel bitmap which
			// refers to this and return this to the caller
			pBitmap = KernelBitmap::MakeKernelBitmap(NULL, NULL);
		}
		else
		{
			// If we have a pFilter then ask the default warning handle to append our message
			if (pCamFilter)
			{
				if (!WarnedBefore)
				{
					pCamFilter->AppendWarning(_R(IDS_NATIVE_BITMAPWARNING));
					WarnedBefore = TRUE;
				}

				// We have warned the user about problems so just construct a new kernel bitmap which
				// is the default one and return this to the caller
				pBitmap = KernelBitmap::MakeKernelBitmap(NULL, NULL);
			}
			else
				ERROR2(NULL,"BitmapListComponent::GetReadBitmapReference negative BitmapRef is unknown");
		}
	}
	else
	{
		// Try and find the specified record number in our colour list
		pBitmap = pImportedBitmaps->GetBitmap(BitmapRef);
		// If not found then this is a problem as a bitmap must have been defined before its being
		// referenced 
		if (pBitmap == NULL)
		{
			// If we have a pFilter then ask the default warning handle to append our message
			if (pCamFilter)
			{
				if (!WarnedBefore)
				{
					pCamFilter->AppendWarning(_R(IDS_NATIVE_BITMAPWARNING));
					WarnedBefore = TRUE;
				}

				// We have warned the user about problems so just construct a new kernel bitmap which
				// is the default one and return this to the caller
				pBitmap = KernelBitmap::MakeKernelBitmap(NULL, NULL);
			}
			else
				ERROR2(NULL,"BitmapListComponent::GetReadBitmapReference BitmapRef cannot be found");
		}
	}

	return pBitmap;
}

/********************************************************************************************

>	BOOL BitmapListComponent::LoadBitmapDefinition(CXaraFileRecordHandler *pXFileRecHandler, CXaraFile * pCXFile,
												   INT32 Tag, UINT32 Size, UINT32 RecordNumber)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pXFileRecHandler - The CXaraFileRecordHandler that is being used to provide read functions etc.
				pCXFile	- The CXaraFile that is being used to provide read functions etc.
				Tag				- The tag of the record and hence the type of bitmap being imported
				Size			- The size of this bitmap record
				RecordNumber	- The record number for this bitmap record
	Returns:	TRUE if the record was succesfully imported;
				FALSE if not (e.g. out of memory)
	Purpose:	Try and import a bitmap definition record into the document.
	SeeAlso:	- 

********************************************************************************************/

BOOL BitmapListComponent::LoadBitmapDefinition(CXaraFileRecordHandler *pXFileRecHandler,
											   CXaraFile * pCXFile,
											   INT32 Tag, UINT32 Size, UINT32 RecordNumber)
{
	ERROR2IF(pXFileRecHandler == NULL,FALSE,"BitmapListComponent::LoadBitmapDefinition NULL pXFileRecHandler");
	ERROR2IF(pCXFile == NULL,FALSE,"BitmapListComponent::LoadBitmapDefinition NULL pCXFile");
	ERROR2IF(pImportedBitmaps == NULL,FALSE,"BitmapListComponent::LoadBitmapDefinition NULL pImportedBitmaps");

	BOOL ok = TRUE;

	// Get the underlying file stream class, CCFile, that we are using to load the file
	CCLexFile* pFile = pCXFile->GetCCFile();
	ERROR2IF(pFile == NULL,FALSE,"BitmapListComponent::LoadBitmapDefinition CCFile pointer is NULL");
	// Note present position in file
	FilePos pos = pFile->tellIn();

	// Get the underlying BaseCamelotFilter, which we will use to handle the progress bar updates
	// and other useful things
	CamelotRecordHandler * pCamRecHandler = (CamelotRecordHandler *)pXFileRecHandler;
	BaseCamelotFilter *pFilter = pCamRecHandler->GetBaseCamelotFilter();

	// We use this for file for the import, but we might need to change it
	// if we use a BitmapSource
	CCLexFile* pPseudoFile = pFile;

	// First get the name of the bitmap and other details stored in the record
	String_256 BitmapName;
	if (ok) ok = pCXFile->Read(BitmapName, BitmapName.MaxLength());

	// Some bitmaps might have a compressed and uncompressed format
	// At present, this only happens for bitmaps
	BOOL Compressed = FALSE;
	// Work out which filter is required from the tag on the record
	UINT32 SearchFilter = 0;
	// Assume we won't need a BitmapSource
	BitmapSource* pSource = NULL;
	BOOL ReadPalette = FALSE;
	switch (Tag)
	{
		case TAG_DEFINEBITMAP_BMP:
			SearchFilter = FILTERID_BMP;
			break;
		case TAG_DEFINEBITMAP_GIF:
			SearchFilter = FILTERID_TI_GIF;
			break;
		case TAG_DEFINEBITMAP_JPEG:
			SearchFilter = FILTERID_IMPORT_JPEG;
			break;
		case TAG_DEFINEBITMAP_PNG:
			SearchFilter = FILTERID_PNG;
			break;
		case TAG_DEFINEBITMAP_BMPZIP:
			SearchFilter = FILTERID_BMP;
			Compressed = TRUE;
			break;
		case TAG_DEFINEBITMAP_JPEG8BPP:
			SearchFilter = FILTERID_IMPORT_JPEG;
			Compressed = TRUE;
			ReadPalette = TRUE;
			break;

		default:
			// Not a known tag, so fall through below and use the default bitmap
			// warn the user and skip the definition
			//ERROR1(FALSE, _R(IDS_DONT_SUPPORT_BITMAP_TYPE));
			break;
	}

	// If we have a filter to search for then find it.
	Filter *pBmpFilter = NULL;
	if (SearchFilter != 0)
	{
		// Try and find the bitmap filter to use to load that bitmap in
		// Go get the first filter in the list
		pBmpFilter = Filter::GetFirst();
		while ((pBmpFilter != NULL) && (pBmpFilter->FilterID != SearchFilter))
		{
			// Try the next filter
			pBmpFilter = Filter::GetNext(pBmpFilter);
		}
	}
	 
	// Check that the Filter existed
	KernelBitmap *pBitmap = NULL;
	if (pBmpFilter == NULL)
	{
		// We have no filter present to load it. In the case of RALPH and even in Camelot,
		// the best course of action will be to use a default bitmap instead.
		// In the case of Camelot, warn the user that this has happened.
		pBitmap = KernelBitmap::MakeKernelBitmap(NULL, NULL);

		// Code below will clean this up so that:-
		//	- we skip the bitmap definition
		//  - we add the record number to the loaded list so that records which access this definition
		//	  get something rather than an error.

		// If we have a pFilter then ask the default warning handle to append our message
		if (pFilter)
		{
			if (!WarnedBefore)
			{
				pFilter->AppendWarning(_R(IDS_NATIVE_BITMAPWARNING));
				WarnedBefore = TRUE;
			}
		}
		else
		{
			// Shouldn't happen, as pFilter should never be null
			// But Warn the user about the problem, just in case it does happen
			InformWarning(_R(IDW_CANNOTLOADBITMAPDEF));
		}
		//ERROR1(FALSE, _R(IDT_CANT_FIND_FILTER));
	}
	else
	{
		// Load the bitmap definition in using that filter
		TRACEUSER( "Neville", _T("LoadBitmapDefinition Name %s RecordNumber %d\n"),(TCHAR*)BitmapName, RecordNumber);

		// Now use that filter to load in the bitmap
		//INT32 BitmapType = 0;
		//BitmapInfo Info;
		BOOL IsNew = FALSE;
		BaseBitmapFilter *pBitmapFilter = (BaseBitmapFilter *)pBmpFilter;
		UINT32 NumberOfPaletteEntries = 0;
		RGBTRIPLE * pPalette = NULL;

		if (ReadPalette)
		{
			// If we are an 8bpp JPEG then we must have saved out the palette
			// So load that in before anybody else gets a look in
			// First, read the number of entries (this is number of entries - 1)
			BYTE Entries = 0;
			if (ok) ok = pCXFile->Read(&Entries);
			NumberOfPaletteEntries = Entries + 1;
			if (ok && NumberOfPaletteEntries > 0)
			{
				pPalette = (RGBTRIPLE*)CCMalloc(NumberOfPaletteEntries * sizeof(RGBTRIPLE));
				RGBTRIPLE * pPal = pPalette;
				// Now read the palette entries in
				for (UINT32 i = 0; i < NumberOfPaletteEntries; i++)
				{
					if (ok) ok = pCXFile->Read(&pPal->rgbtRed);
					if (ok) ok = pCXFile->Read(&pPal->rgbtGreen);
					if (ok) ok = pCXFile->Read(&pPal->rgbtBlue);
					pPal++;
				}
			}
		}

		// WEBSTER - markn 10/2/97
		// New flag so that pPseudoFile only gets deleted when it should
		BOOL DeletePseudoFile = FALSE;

		// Do we need to create a BitmapSource from this import?
		if (pBitmapFilter->IsFormatLossy())
		{
			// Create a new bitmap source the size of the current record minus a bit
			OFFSET SourceSize = Size - (pFile->tellIn() - pos);
			pSource = pBitmapFilter->CreateBitmapSource(SourceSize);
			if (pSource == NULL || !pSource->IsOK())
			{
				ok = FALSE;
			}
			// Now create a file that can fill it
			if (ok)
			{
				pPseudoFile = (CCLexFile*)new CCBufferFile(pFile);
				if (pPseudoFile == NULL)
				{
					ok = FALSE;
				}
				else
					DeletePseudoFile = TRUE; // WEBSTER - markn 10/2/97 - New flag so that pPseudoFile only gets deleted when it should
			}
			// and provide the filter with something to work with
			if (ok)	pSource->AttachToBufferFile((CCBufferFile*)pPseudoFile);
		}

		// Import that bitmap 
		// If we have a palette then we need to convert the 24bpp image into an 8bpp image
		// using the supplied palette
		if (ok) ok = KernelBitmap::ImportBitmap(pPseudoFile, pBitmapFilter, pFilter, Compressed, 
												pPalette, NumberOfPaletteEntries,
												&pBitmap, &IsNew);
		
		if (ok && IsNew)
		{
			// Set the name in it
			pBitmap->ActualBitmap->SetName(BitmapName);
			// Associate the new bitmap with its source...if there is one
			if (pSource != NULL)
			{
				pBitmap->SetOriginalSource(pSource, pBitmapFilter);
				pBitmap->SetAsLossy();
			}
		}
		else
		{
			// Throw away the source as we already have it or don't need it
			if (pSource != NULL)
			{
				delete pSource;
			}
		}
		
		if (pPalette != NULL)
		{
			CCFree(pPalette);
			pPalette = NULL;
		}

		// If we have a BitmapSource we should have had a file to use it with
		// Delete it
		if (pSource != NULL)
		{
			ERROR3IF(pPseudoFile == NULL, "pPseudoFile == NULL");
			// WEBSTER - markn 10/2/97
			// New flag so that pPseudoFile only gets deleted when it should
			if (DeletePseudoFile && pPseudoFile != NULL) 
			{
				delete pPseudoFile;
				pPseudoFile = NULL;
			}
		}
	}
	
	if (!ok)
	{
		TRACEALL( _T("==============Failed to load bitmap in, try and recover situation\n") );
		// Try some limited error recovery
		// First set the flag to true so it thinks everything went ok
		ok = TRUE;
		// If it has thrown a file exception error then we will need to reset the good flag
		// so file access still works ok
		pFile->SetGoodState();
		// We should now fall through and skip to the end of this record/start of next
		// ready to read in the next bit of data.
		// REM Any errors will be set, we could inform error at this point but the user should get
		// a more friendly, a bitmap failed to load properly, using defaults.
	}
	
	// Only do this if everything went ok
	if (ok)
	{
		// Note the current position in the file
		FilePos newpos = pFile->tellIn(); 
		TRACEUSER( "Neville", _T("newpos %d, pos %d, newpos-pos %d should = Size %d\n"),newpos,pos,newpos-pos,Size);
		if (newpos - pos != (INT32)Size)
		{
			// The bitmap filter has been naughty and left the file
			// pointer sitting somewhere different to where it should.
			// We need to now fix this to where it should be
			pFile->seekIn(pos + Size);
		}
	}

	// If have a genuine bitmap then add it to the loaded list
	// If the bitmap is null then the reference will use the default instead.
	if (pBitmap != NULL)
		pImportedBitmaps->AddBitmap(pBitmap, RecordNumber);

	return ok;
}

/********************************************************************************************
********************************************************************************************/


/********************************************************************************************

>	BOOL BitmapListComponent::StartExport(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Returns:	TRUE if the component was able to prepare for exporting;
				FALSE if not (e.g. out of memory)
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
	Purpose:	Inform the bitmap list document component that a Web or Native export is
				about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL BitmapListComponent::StartExport(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	TRACEUSER( "Neville", _T("BitmapListComponent::StartExport\n") );
	// Mark all bitmaps as unsaved 
	if (pFilter == NULL)
	{
		ERROR3("BitmapListComponent::StartExport filter is null!");
		return TRUE;
	}

	// Set up and start the exported bitmap list
	pExportedBitmaps = new ExportedBitmaps();
	if (pExportedBitmaps == NULL)
		return FALSE;
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL BitmapListComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - True if everything went swimmingly, False if just a clean up is required.
	Purpose:	Inform the bitmap list document component that a Web or Native export has
				just finished.
	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL BitmapListComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)
{
#ifdef DO_EXPORT
//	ERROR2IF(pExportedBitmaps == NULL, FALSE, "BitmapListComponent::EndExport no pExportedBitmaps!");
	// If we error about the pExportedBitmaps then we assume that the EndImport has been called.
	// This may not be the case if we are in clean up mode. So just handle it!

TRACEUSER( "Neville", _T("BitmapListComponent::EndExport\n") );
	if (pFilter == NULL)
	{
		ERROR3("BitmapListComponent::EndExport filter is null!");
		return TRUE;
	}
		
	// Check if Web or Native, if Web do nothing
	// If Native and we are not just in clean up mode, Save out any unsaved bitmaps
	//if (!pFilter->IsWebFilter() && Success)
	//{
//TRACEUSER( "Neville", _T("BitmapListComponent::EndExport exporting unsaved bitmaps\n"));
/*		// Count the number of Bitmaps
		INT32 TotalBitmaps = 0;
		KernelBitmap* pBitmap = (KernelBitmap*) pBitmaps->GetHead();

		while (pBitmap != NULL)
		{
			// Find the next one
			if (pBitmap->GetUsageCount() > 0)
				TotalBitmaps++;		// Only count this bitmap if it really used

			pBitmap = (KernelBitmap*) pBitmaps->GetNext(pBitmap);
		}
	//}
*/
	// As we have finished with it now, destroy the list of exported bitmaps
	if (pExportedBitmaps)
	{
		delete pExportedBitmaps;
		pExportedBitmaps = NULL;
	}
	
#if 0
	if(Success)
		SGLibOil::ExportWav(pFilter);
#endif

#endif
	return TRUE;
}

/********************************************************************************************

>	INT32 BitmapListComponent::GetWriteBitmapReference(KernelBitmap* pBitmap, BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pBitmap		- The document bitmap to save
				pFilter		- The filter to use for saving
	Outputs:	-
	Returns:	The bitmap reference for the kernel bitmap ready for writing in a record.
				This will be zero if a problem has occurred. May return -1 if it does not
				need to save anything i.e if the bitmap is not in use.

	Purpose:	Return the bitmap reference to the caller, who will then use it to write the
				bitmap in the record that they are preparing to save to a Web or Native file.
				In the process of getting the bitmap reference, the bitmap definition record
				for the bitmap reference will be generated and saved to the file. 
				In the web format, the bitmap may have been reduced in resolution,
				colour resolution or cropped to	try and reduce the size saved out.
				NOTE: As this does save information into the record stream, it *MUST* be called
				before the caller has called StartRecord() as otherwise the records will become
				intertwined!
	SeeAlso:	DocComponent

********************************************************************************************/

INT32 BitmapListComponent::GetWriteBitmapReference(KernelBitmap* pBitmap, BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pBitmap == NULL,0L,"BitmapListComponent::GetWriteBitmapReference null pBitmap");
	ERROR2IF(pFilter == NULL,0L,"BitmapListComponent::GetWriteBitmapReference null pFilter");

	INT32 RecordNumber = 0L;

	// Check for a deleted bitmap
	if (pBitmap->HasBeenDeleted())
	{
		ERROR2IF(pBitmap->GetParentBitmapList() == NULL, 0L, "Deleted bitmap has no parent list");
		
		// Use the default bitmap instead
		pBitmap = pBitmap->GetParentBitmapList()->FindDefaultBitmap();

		// There should always be a default bitmap in the list
		ERROR2IF(pBitmap == NULL, 0L, "Couldn't find the default bitmap");
	}

	// Check if the bitmap is the default one
	// We could save this as a unique -ve reference but this implies we have the bitmap
	// bound into ALL exes i.e. RALPH and the viewer. Which makes them 77k bigger. Not many
	// people will use the default bitmap and if they do will not assume that it is assumed.
	// They will want and expect the document to contain the bitmap so that it is standalone.
	// Also, what happens if we change the default. Then we need the old and the new default 
	// bound in! 
	//if (pBitmap->IsDefaultBitmap())
	//{
		// If so then there is no point in saving out the definition, just save out
		// our unique identifier for this bitmap i.e. pass this back to the caller
	//	return REF_DEFAULTBITMAP_BUILTIN;
	//}

	// Save out that bitmap definition into the file using the bitmap compression value
	// possibly requested by the user
	RecordNumber = SaveBitmapDefinition(pBitmap, pFilter);

	return RecordNumber;
#else
	return 0;
#endif
}


/********************************************************************************************

>	BOOL BitmapListComponent::WriteBitmapPropertiesRecord(KernelBitmap* pBitmap, BaseCamelotFilter *pFilter, INT32 BmpRef)

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/00
	Inputs:		pBitmap		- The document bitmap to save
				pFilter		- The filter to use for saving
				BmpRef		- The bitmap reference
	Outputs:	-
	Returns:	TRUE on success.
	Purpose:	Helper function for SaveBitmapDefinition.
				Write out 'bitmap properties' record.
				This contains:
				* A bitmap reference (INT32). This specifies the bitmap to which the
				  properties apply. (This will usually be the bitmap record which
				  immediately preceded this bmp properties record.)
				* A single flag byte. The LSB stores the value of the 'smooth when
				  scaled up' flag - 1 for yes, 0 for no. Other bits could be used to
				  record other boolean bmp properties; for now these are set to 0.
				* 7 more bytes for future use. Some of this space should be used to
				  store GIF animation info (which is presently not saved).
				  The remainder of the space is reserved for possible future additions
				  to the bmp properties dialogue.
	SeeAlso:	SaveBitmapDefinition

********************************************************************************************/

BOOL BitmapListComponent::WriteBitmapPropertiesRecord(KernelBitmap* pBitmap, BaseCamelotFilter *pFilter, INT32 BmpRef)
{
	BOOL ok = TRUE;

	// populate the record
	BYTE Flags = 0x00;
	if (pBitmap->GetInterpolation()) Flags |= 0x01;

	// Find out whether we need to make an XPE record or not...
PORTNOTE("other","KernelBitmap::GetXPEInfo removed")
#ifndef EXCLUDE_FROM_XARALX
	if (pFilter->GetSaveXPEBitmaps() == FALSE)
	{
		KernelBitmap* pMaster = NULL;
		IXMLDOMDocumentPtr pEditList = NULL;
		pBitmap->GetXPEInfo(pMaster, pEditList);
		if (pMaster!=NULL && pEditList!=NULL)
		{
			// Get master bitmap reference number (writing out the bitmap if necessary)
			INT32 MasterRecord = 0;
			MasterRecord = GetWriteBitmapReference(pMaster, pFilter);
	
			// Only write out new record style if we really have to...
			// (Helps forward compatibility)
			if (MasterRecord!=0)
			{
				// Create a new style XPE bitmap properties record
				CXaraFileRecord Rec(TAG_XPE_BITMAP_PROPERTIES);
	
				BSTR bstrValue;
				HRESULT hr;
				hr = pEditList->get_xml(&bstrValue);
				if (SUCCEEDED(hr))
				{
					ok = Rec.Init();
					if (ok) ok = Rec.WriteReference(BmpRef);		// bitmap reference
					if (ok) ok = Rec.WriteBYTE(Flags);				// flags
					
					// TODO: write GIF animation properties here
	
					// reserved bytes
					for( INT32 i=0; i<7; i++ )
					{
						if (ok) ok = Rec.WriteBYTE(0);
					}
	
					// Now write out XPE stuff
					if (ok) ok = Rec.WriteReference(MasterRecord);		// Master bitmap record number
					if (ok) ok = Rec.WriteUnicode(pBitmap->GetName());	// Oil Bitmap name
					if (ok) ok = Rec.WriteBSTR(bstrValue);				// UNICODE xml string edits list
	
					// Write the record
					if (ok) ok = pFilter->Write(&Rec);
	
					return ok;
				}
			}
		}
	}
#endif

	// Fallback to the original BITMAP PROPERTIES record style
	// Create an old style properties record
	CXaraFileRecord Rec(TAG_BITMAP_PROPERTIES,TAG_BITMAP_PROPERTIES_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteReference(BmpRef);	// bitmap reference
	if (ok) ok = Rec.WriteBYTE(Flags);			// flags
	
	// TODO: write GIF animation properties here

	// reserved bytes
	for( INT32 i=0; i<7; i++ )
	{
		if (ok) ok = Rec.WriteBYTE(0);
	}

	// Write the record
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}


/********************************************************************************************

>	INT32 BitmapListComponent::SaveBitmapDefinition(KernelBitmap* pBitmap, BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		pBitmap		- The document bitmap to save
				pFilter		- The filter to use for saving
	Outputs:	-
	Returns:	The bitmap reference for the kernel bitmap ready for writing in a record.
				This will be zero if a problem has occurred. May return -1 if it does not
				need to save anything i.e if the bitmap is not in use.

	Purpose:	This actually saves out the bitmap definition into the file and returns the
				bitmap reference or record number to the caller. They will then be able to 
				use it to write the	bitmap in the record that they are preparing to save to
				a Web or Native file.
	SeeAlso:	DocComponent

********************************************************************************************/

INT32 BitmapListComponent::SaveBitmapDefinition(KernelBitmap* pBitmap, BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pBitmap == NULL,0L,"BitmapListComponent::GetWriteBitmapReference null pBitmap");
	ERROR2IF(pFilter == NULL,0L,"BitmapListComponent::GetWriteBitmapReference null pFilter");

	INT32 RecordNumber = 0L;

	// Check for a deleted bitmap
	if (pBitmap->HasBeenDeleted())
	{
		ERROR2IF(pBitmap->GetParentBitmapList() == NULL, 0L, "Deleted bitmap has no parent list");
		
		// Use the default bitmap instead
		pBitmap = pBitmap->GetParentBitmapList()->FindDefaultBitmap();

		// There should always be a default bitmap in the list
		ERROR2IF(pBitmap == NULL, 0L, "Couldn't find the default bitmap");
	}

	// ****
	// **** Saved this bitmap before?
	// ****

	// Only write this record out if we have not done so already
	// Check by seeing if the bitmap is already in our exported bitmap list
	ExportBitmap* pExportBitmap = NULL;
	pExportBitmap = pExportedBitmaps->GetBitmap(pBitmap);
	// If it is then do not save this bitmap
	if (pExportBitmap)
	{
		// Just return the record number as the bitmap reference to the caller
		return pExportBitmap->RecordNumber;
	}

PORTNOTE("other", "Removed live effects stuff")
#ifndef EXCLUDE_FROM_XARALX
	// Check to see whether we can save a simple placeholder, relying on the 
	// XPE to regenerate the bitmap from master and edits when it's reloaded.
	if (pFilter->GetSaveXPEBitmaps() == FALSE)
	{
		KernelBitmap* pMaster = NULL;
		IXMLDOMDocumentPtr pEdits = NULL;
		pBitmap->GetXPEInfo(pMaster, pEdits);
		if (pMaster != NULL && pEdits != NULL)
		{
			// Yes, we have a valid set of XPE info so we can regenerate on loading
			UINT32 recnum = pFilter->WriteXPEBitmapPlaceHolder(pBitmap->GetName());
			if (recnum!=0)
			{
				BOOL bOK = FALSE;

				// write out a bitmap properties record
				bOK = WriteBitmapPropertiesRecord(pBitmap, pFilter, recnum);
				if (!bOK) return 0L;

				// Now add this exported bitmap definition record to our exported list for later reuse
				pExportedBitmaps->AddBitmap(pBitmap, recnum);

				return recnum;
			}
		}
	}
#endif

	BOOL ok = TRUE;
	// Find out what bitmap compression was requested by the user
	// Could use BMPZIP which could be deemed to mean no compression
	// > 100 means use PNG
	// 0-100 use JPEG with this quality setting
	// Of course, only do this if we are in web mode
	INT32 Compression = pFilter->GetBitmapCompression(); 
	
	// Get details of the bitmap
	BitmapInfo Info;
	String_256 BitmapName;
	pBitmap->ActualBitmap->GetInfo(&Info);
	BitmapName = pBitmap->ActualBitmap->GetName();

//	double Dpi = Info.PixelDepth;
	UINT32 Height = Info.PixelHeight;
	UINT32 NumberOfPaletteEntries = Info.NumPaletteEntries;	// 0 for no palette

	UINT32 SearchFilter = 0;
	INT32 Tag = TAG_DEFINEBITMAP_BMP;

	// See if we have a BitmapSource for the bitmap we're exporting	
	BitmapSource* pSource = NULL;
	BaseBitmapFilter* pDummyFilter;
	BOOL bUseOriginalSource = pBitmap->GetOriginalSource(&pSource, &pDummyFilter);
	BOOL WritePalette = FALSE;

	if (!bUseOriginalSource)
	{
		// We don't already have a compressed source so pick a filter

		UINT32 Bpp = Info.PixelDepth;
		// Check if the bitmap has a transparent colour or masking colour present.
		// If so then we may need to change our saving strategy
		INT32 TransparentColour = -1;
		if (Bpp <= 8)
			pBitmap->ActualBitmap->GetTransparencyIndex(&TransparentColour);

		switch (Bpp)
		{
			case 1:
			case 4:
				// Cannot use JPEG as only for 8,24bpp
				// Both GIF and PNG support transparency
				// So always use the PNG filter. 
				Tag = TAG_DEFINEBITMAP_PNG;
				SearchFilter = FILTERID_PNG;
				break;
			case 8:
				// JPEG does not support transparency, so if this is set then use PNG
				if (TransparentColour == -1)
				{
					// If it is 8bpp and there is a no transparent colour present
					// then we can JPEG the bitmap or PNG it. Using Accusoft this will save
					// out the bitmap as a 24bpp JPEGd bitmap.
					// If compression is between 0 and 100 then use JPEG
					// otherwise use PNG
					if (Compression >= 0 && Compression <= 100)
					{
						// Check if greyscale or not. If greyscale then legal JPEG so save as normal
						// otherwise save as special 8BPP JPEG record 
						if (pBitmap->IsGreyscale())
						{
							Tag = TAG_DEFINEBITMAP_JPEG;
							SearchFilter = FILTERID_EXPORT_JPEG;
						}
						else
						{
							Tag = TAG_DEFINEBITMAP_JPEG8BPP;
							SearchFilter = FILTERID_EXPORT_JPEG;
							WritePalette = TRUE;
						}
					}
					else
					{
						Tag = TAG_DEFINEBITMAP_PNG;
						SearchFilter = FILTERID_PNG;
					}
				}
				else
				{
					// If it is 8bpp and there is a transparent colour present
					// then we must use PNG to save out this colour
					Tag = TAG_DEFINEBITMAP_PNG;
					SearchFilter = FILTERID_PNG;
				}
				break;
			case 24:
				// If compression is between 0 and 100 then use JPEG
				// otherwise use PNG
				if (Compression >= 0 && Compression <= 100)
				{
					Tag = TAG_DEFINEBITMAP_JPEG;
					SearchFilter = FILTERID_EXPORT_JPEG;
				}
				else if (Compression >= 201 && Compression <= 300)
				{
					// If compression is between 201 and 300 then use JPEG
					// where the bitmap came from a lossy source
					if (pBitmap->IsLossy())
					{
						Tag = TAG_DEFINEBITMAP_JPEG;
						SearchFilter = FILTERID_EXPORT_JPEG;
						Compression -= 200;		// Get quality in the correct range
					}
					else
					{
						Tag = TAG_DEFINEBITMAP_PNG;
						SearchFilter = FILTERID_PNG;
					}
				}
				else
				{
					Tag = TAG_DEFINEBITMAP_PNG;
					SearchFilter = FILTERID_PNG;
				}
				break;
			case 32:
				if (Compression >= 0 && Compression <= 200)
				{
					// Definitely use PNG is this case
					Tag = TAG_DEFINEBITMAP_PNG;
					SearchFilter = FILTERID_PNG;
				}
				else
				{
					// If compression indicates JPEG where possible then use 
					// JPEG where the bitmap came from a lossy source and has 
					// no transparency in its alpha channel
					if (pBitmap->IsLossy() && !pBitmap->IsTransparent())
					{
						Tag = TAG_DEFINEBITMAP_JPEG;
						SearchFilter = FILTERID_EXPORT_JPEG;
						Compression -= 200;		// Get quality in the correct range
					}
					else
					{
						Tag = TAG_DEFINEBITMAP_PNG;
						SearchFilter = FILTERID_PNG;
					}
				}
				break;
			default:
				ERROR2(0,"BitmapListComponent::SaveBitmapDefinition bad pixel depth");
		}
	}
	else
	{
		// We have a BitmapSource...
		// but we don't have the right filter...get rid of this hack!
		// FilterManager* pFilterManager = Camelot->GetFilterManager();
		// pFilter = pFilterManager->FindComplementaryFilter(pDummyFilter);
		Tag = TAG_DEFINEBITMAP_JPEG;
		SearchFilter = FILTERID_EXPORT_JPEG;
		// Now check if we have done a 24bpp to 8bpp conversion job on this
		// The way we can tell this is if the bitmap is 8bpp and not greyscale
		UINT32 Bpp = Info.PixelDepth;
		if (Bpp == 8 && !pBitmap->IsGreyscale())
		{
			Tag = TAG_DEFINEBITMAP_JPEG8BPP;
			WritePalette = TRUE;
		}
	}
	
	// Try and find the bitmap filter to use to load that bitmap in
	// Go get the first filter in the list
	Filter *pBmpFilter = Filter::GetFirst();
	while ((pBmpFilter != NULL) && (pBmpFilter->FilterID != SearchFilter))
	{
		// Try the next filter
		pBmpFilter = Filter::GetNext(pBmpFilter);
	}

	// Check that the Filter existed
	ERROR2IF(pBmpFilter == NULL,0,"BitmapListComponent::SaveBitmapDefinition can't find export filter!");

	// Start writing our record data out to file
	UINT32 Size = 0;
	// If there is a string name, then add it to this size
	// REMEMBER: We save out unicode strings and so we need to double the length of the returned string length
	// but only if we are not being compiled as Unicode
	Size += (BitmapName.Length() + 1) * SIZEOF_XAR_UTF16;
	if (WritePalette)
	{
		// If we are an 8bpp JPEG then we must save out the palette so that on loading we can
		// convert the 24bpp JPEG back into 8bpp using this palette
		Size += 1;	// For the number of palette entries
		Size += NumberOfPaletteEntries * sizeof(RGBTRIPLE);
	}
	
	RecordNumber = pFilter->StartStreamedRecord(Tag, Size);
	//RecordNumber = pFilter->StartRecord(Tag, Size); 
	// If we had a problem starting the record up then exit now
	if (RecordNumber <= 0)
		return RecordNumber;

	// Write out the name to disc
	ok = pFilter->Write(BitmapName);

	// Possibly write palette information into the record
	if (WritePalette)
	{
		TRACEUSER( "Neville", _T("8bpp JPEG write palette size %d to file\n"), NumberOfPaletteEntries );
		// If we are an 8bpp JPEG then we must save out the palette so that on loading we can
		// convert the 24bpp JPEG back into 8bpp using this palette
		// Saves number of palette entries followed by the palette in RGB triples
		pBitmap->WritePalette(pFilter);
	}
	
	// Write out the bitmap data to file
	// First get the CCFile that we are using for writing out the data
	CCLexFile* pFile = pFilter->GetCCFile();
	// The WriteBitmapToFile function is only available in bitmap filters so we need
	// to convert our pointer to that type of filter
	BaseBitmapFilter *pBitmapFilter = (BaseBitmapFilter *)pBmpFilter;
	// On the first pass this will be null as we are trying to work out the export size
	// so that we get the progress bar correct.
	if (pFile)
	{
		// Ensure all current data is flushed to file otherwise some filters
		// may screw up the file.
		pFile->flush();

		// Check to see if we haven't already got it compressed
		if (bUseOriginalSource)
		{
			// We have,so write out the source & don't degrade it
			ERROR2IF(pSource == NULL, 0L, "pSource == NULL");
			pFilter->WriteBitmapSource(*pSource, Height);
		}
		else
		{
			// We are on the real pass so go and export that bitmap data to file
			pBitmapFilter->WriteBitmapToFile(pBitmap, pFilter, pFile, Compression);
		}
	}
	else
	{
		if (WritePalette)
		{
			// If we are on the fake pass and saving the palette then include the number of
			// palette entries in the count.
			pFilter->IncProgressBarCount(NumberOfPaletteEntries);
		}

		// We are on the fake pass so add the height of the bitmap onto the progress bar count
		// as this is what we will use the update the progress bar
		pFilter->IncProgressBarCount(Height);
	}

	// Ask for the record to be ended and hence items like the size in the record header
	// to be cleaned up and hence made correct
	if (ok) ok = pFilter->EndStreamedRecord();

	// write out a bitmap properties record
	if (ok) ok = WriteBitmapPropertiesRecord(pBitmap, pFilter, RecordNumber);

	// If we have had a problem at any of the stages then return that to the caller
	if (!ok)
		return 0L;

	// Now add this exported bitmap definition record to our exported list for later reuse
	if (ok) ok = pExportedBitmaps->AddBitmap(pBitmap, RecordNumber);

	return RecordNumber;
#else
	return 0;
#endif
}
