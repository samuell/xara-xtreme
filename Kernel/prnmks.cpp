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
// Author  : Mike
// Created : 23/07/96
// Purpose : The PrintMarks Manager and all its helper classes.
//			 This manager handles the caching and rendering of print marks
//			 within a document. PrintMarks are generally only used for typesetting

/*
*/

#include "camtypes.h"
//#include "node.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "prnmks.h"
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "fileutil.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "opdesc.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nativeop.h"
#include "layer.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "page.h"
//#include "nodeattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "colormgr.h"
#include "prnmkcom.h"
#include "qualattr.h"
//#include "matrix.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "dbugtree.h"
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cctime.h"
#include "nodetxts.h"
#include "nodetxtl.h"
#include "colplate.h"
#include "printctl.h"
#include "fontman.h"
#include "psrndrgn.h"
#include "psdc.h"
//#include "printstr.h"

CC_IMPLEMENT_MEMDUMP(LoadPrintMarks, LoadDirect);
CC_IMPLEMENT_MEMDUMP(PrintMarksMan, CC_CLASS_MEMDUMP);
CC_IMPLEMENT_MEMDUMP(PrintMarksCache, CC_CLASS_MEMDUMP);
CC_IMPLEMENT_MEMDUMP(PrintMark, CC_CLASS_MEMDUMP);
CC_IMPLEMENT_DYNCREATE(MarkPosition, ListItem);
CC_IMPLEMENT_DYNCREATE(PrintMarkItem, ListItem);
CC_IMPLEMENT_MEMDUMP(MarkFormatRegion, CC_CLASS_MEMDUMP);
CC_IMPLEMENT_DYNCREATE(PageRect, ListItem);
CC_IMPLEMENT_DYNAMIC(PageRectList, List);

DECLARE_SOURCE("$Revision$");

#define new CAM_DEBUG_NEW


/*****************************************************************************************

>	MarkFormat::MarkFormat()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Construct a Mark format object

*****************************************************************************************/

MarkFormat::MarkFormat()
{
	Top = Middle = Bottom = FALSE;
	Left = Centre = Right = FALSE;
}

BOOL MarkFormat::IsValid() const
{
	return (Top || Middle || Bottom || Left || Centre || Right);
}

void MarkFormat::MakeValid()
{
	Centre = Middle = TRUE;
	Top = Bottom = Left = Right = FALSE;
}

INT32 operator==(const MarkFormat& x, const MarkFormat& y)
{
	return ( (x.Centre == y.Centre) &&
			 (x.Middle == y.Middle) &&
			 (x.Left   == y.Left)   &&
			 (x.Right  == y.Right)  &&
			 (x.Top    == y.Top)    &&
			 (x.Bottom == y.Bottom)
			);
}



/*****************************************************************************************

>	BYTE MarkFormat::GetAsFlagByte(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/96

	Returns:	A Byte with bits set for each of the mark formats available

	Purpose:	Fill in a mark format object from a "flags byte"
				Used for native file load/save

	SeeAlso:	MarkFormat::SetFromFlagByte

*****************************************************************************************/

BYTE MarkFormat::GetAsFlagByte(void)
{
	BYTE Flags = 0;

	if (Left)	Flags |= 0x01;
	if (Centre)	Flags |= 0x02;
	if (Right)	Flags |= 0x04;
	if (Top)	Flags |= 0x08;
	if (Middle)	Flags |= 0x10;
	if (Bottom)	Flags |= 0x20;

	return(Flags);
}



/*****************************************************************************************

>	void MarkFormat::SetFromFlagByte(BYTE Flags)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/96

	Inputs:		Flags - A Byte with bits set for each of the mark formats available

	Purpose:	Fill in a mark format object from a "flags byte"
				Used for native file load/save

	SeeAlso:	MarkFormat::GetAsFlagByte

*****************************************************************************************/

void MarkFormat::SetFromFlagByte(BYTE Flags)
{
	Left	= (Flags & 0x01) ? TRUE : FALSE;
	Centre	= (Flags & 0x02) ? TRUE : FALSE;
	Right	= (Flags & 0x04) ? TRUE : FALSE;
	Top		= (Flags & 0x08) ? TRUE : FALSE;
	Middle	= (Flags & 0x10) ? TRUE : FALSE;
	Bottom	= (Flags & 0x20) ? TRUE : FALSE;
}





/*****************************************************************************************

>	MarkPosition::MarkPosition()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Construct a mark position object

*****************************************************************************************/

MarkPosition::MarkPosition()
{
	Initialise();
}


/*****************************************************************************************

>	void MarkPosition::Initialise()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Initialise all the local variables in this mark position

*****************************************************************************************/

void MarkPosition::Initialise()
{
	Region = MarkRegion_TopLeft;
}


/*****************************************************************************************

>	BOOL MarkPosition::IsEqualTo(const MarkPosition* pOther) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pOther = a pointer to a mark position to match
	Returns:	TRUE if pOther is identical to this mark
				FALSE if not
	Purpose:	Check whether another mark position is identical to this one

*****************************************************************************************/

BOOL MarkPosition::IsEqualTo(const MarkPosition* pOther) const
{
	ERROR2IF(pOther==NULL, FALSE, "NULL pointer passed to MarkPosition::IsEqualTo");
	return ( (Region == pOther->Region) &&
			 (Format == pOther->Format)
		   );
}


/*****************************************************************************************

>	PrintMark::PrintMark

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Construct a new print mark

*****************************************************************************************/

PrintMark::PrintMark()
{
	MarkIsCustom	= FALSE;
	IDByte			= 0;
	Type			= MarkType_Unknown;
	Orientation		= MO_None;
	OnAsDefault		= FALSE;
}


/*****************************************************************************************

>	virtual PrintMark::~PrintMark()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	virtual destructor, destroys all instances of the 'MarkPositions' list

*****************************************************************************************/

PrintMark::~PrintMark()
{
	MarkPosition* pMarkPos;
	while (pMarkPos=(MarkPosition*)MarkPositions.RemoveTail())
		delete pMarkPos;
}



/*****************************************************************************************

>	void PrintMark::SetMarkMenuText(const String_256* pMenuText)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pMenuText = a pointer to a string containing the textual description of 
							the print mark, (may be used in menus).
	Purpose:	Set this print mars menu text

*****************************************************************************************/

void PrintMark::SetMarkMenuText(const String_256* pMenuText)
{
	ERROR3IF(pMenuText==NULL,"ERROR ERROR, NULL pointer passed to SetMarkMenuText");
	if (pMenuText!=NULL)
		MenuText = (*pMenuText);
}


/*****************************************************************************************

>	String_256 PrintMark::GetMarkMenuText() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		-
	Returns:	This print marks textual description
	Purpose:	Return the menu text string saved in this print mark

*****************************************************************************************/

String_256 PrintMark::GetMarkMenuText() const
{
	return MenuText;
}


/*****************************************************************************************

>	void PrintMark::AddNewPosition(MarkPosition* pNewMarkPos)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pNewMarkPos, a pointer to a new mark position
	Returns:	This print marks textual description
	Purpose:	Return the menu text string saved in this print mark

*****************************************************************************************/

void PrintMark::AddNewPosition(MarkPosition* pNewMarkPos)
{
	ERROR3IF(pNewMarkPos==NULL,"NULL MarkPosition pointer passed to AddNewPosition");
	if (pNewMarkPos)
	{
		if (!FindMarkPosition(pNewMarkPos))
			MarkPositions.AddTail(pNewMarkPos);
	}
}

/*****************************************************************************************

>	BOOL PrintMark::FindMarkPosition(const MarkPosition* pSearchMark, MarkPosition** pFoundMark=NULL) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pSearchMark = a pointer to a print mark to search for
				pFoundMark, a pointer to a pointer to be set on exit. This defaults to null
				when we're only interested in whether the mark exists or not.

	Outputs:	pFoundMark set to a pointer to the print mark in the cache if we find it
				pFoundMark set to NULL if no match found
							 		
	Returns:	TRUE if there is a mark in the cache identical to this search mark
				FALSE if there isn't
	Purpose:	Check the cache to see if there's a mark already in it which matches
				pSearchMark. You can get the pointer to this cached mark by using pFoundMark

*****************************************************************************************/

BOOL PrintMark::FindMarkPosition(const MarkPosition* pSearchMark, MarkPosition** pFoundMark) const
{
	ERROR3IF(pSearchMark==NULL,"PrintMark::FindMarkPosition passed a NULL search mark pointer");
	if (pSearchMark)
	{
		MarkPosition* pMarkPos = GetFirstPosition();
		while (pMarkPos)
		{
			// Check to see if this exists
			if (pSearchMark->IsEqualTo(pMarkPos))
			{
				if (pFoundMark!=NULL)
					(*pFoundMark=pMarkPos);
				return TRUE;
			}
			pMarkPos = GetNextPosition(pMarkPos);
		}
	}
	if (pFoundMark!=NULL)
		(*pFoundMark=NULL);
	return FALSE;
}

/*****************************************************************************************

>	BOOL PrintMark::IsSimilarTo(const PrintMark* pOther) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pOther, a pointer to another print mark
	Returns:	TRUE if this print mark is similar to pOther.
				FALSE if not
	Purpose:	Checks whether this print mark is similar to pOther. The current checks
				for similarity are...
				(1) the mark is of the same type
				(2) the mark has the same menu text

*****************************************************************************************/

BOOL PrintMark::IsSimilarTo(const PrintMark* pOther) const
{
	ERROR2IF(pOther==NULL, FALSE, "NULL PrintMark pointer passed to IsSimilar()");

	if (Type == pOther->GetType())
	{
		if (MenuText == pOther->GetMarkMenuText())
			return TRUE;
	}
	return FALSE;
}



/*****************************************************************************************

>	PrintMarkItem::PrintMarkItem

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Construct a new print mark item

*****************************************************************************************/

PrintMarkItem::PrintMarkItem()
{
	pMarkGlyph	= NULL;
	Handle		= 0;
	Saveable	= TRUE;
	Renderable	= TRUE;
}


/*****************************************************************************************

>	PrintMarkItem::~PrintMarkItem

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	destruct a print mark item

*****************************************************************************************/

PrintMarkItem::~PrintMarkItem()
{
	RemoveMark();
}




/*****************************************************************************************

	BOOL PrintMarkItem::SetMarkGlyph(NodeGroup* pNewMark, BOOL MakeCopy=FALSE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pNewMark = A pointer to a group node containing the mark. 
						   The node should still be in the tree.
				MakeCopy = TRUE, the mark will be left where it is and an
							     attribute complete copy will be made.
						   FALSE, the mark will be extracted from the document as is. 
	Purpose:	Cache a print mark in this print mark item. The mark pointer should be
				a group held in the document tree. As we attempt to make attributes
				complete on this group, it has to be in the tree. MakeCopy which defaults
				to FALSE will, when TRUE, copy the entire group and used the copy, hence
				it will leave the document unaffected. When FALSE the group will be
				extracted from the tree. This is only acceptable in a few circumstances
				when undo is not being generated.

*****************************************************************************************/

BOOL PrintMarkItem::SetMarkGlyph(NodeGroup* pNewMark, BOOL MakeCopy)
{
	ERROR2IF(pNewMark==NULL,FALSE, "NULL mark pointer passed to SetMark");

	if (MakeCopy)
	{
		NodeGroup* pNewGroup = new NodeGroup;
		if (pNewGroup==NULL)
			return FALSE;

		NodeGroup* pNewGroup1 = new NodeGroup;
		if (pNewGroup1==NULL)
		{
			delete pNewGroup;
			return FALSE;
		}
		
		// attach this new group to the tree
		pNewGroup->AttachNode(pNewMark,NEXT);
		pNewGroup1->AttachNode(pNewGroup,FIRSTCHILD);

		// copy the children across to this new group
		if (!pNewMark->CopyChildrenTo(pNewGroup1))
		{
			// CascadeDelete unlinks the node from the tree so don't panic.
			pNewGroup->CascadeDelete();
			delete pNewGroup;
			return FALSE;
		}

		// make sure we have all the attributes we're gonna need
		if (!pNewGroup->MakeAttributeComplete(NULL,TRUE,NULL,TRUE))
		{
			pNewGroup->CascadeDelete();
			delete pNewGroup;
			return FALSE;
		}

		// dump the tree out
		//TRACEUSER( "Mike", _T("Dumping subtree contents\n\n"));
		//DebugTreeDlg::DumpSubTree(pNewGroup);
		
		// rip the node out of the tree and stick it in our cache.
		pNewGroup->UnlinkNodeFromTree();
				
		// Make sure we get rid of any previously cached mark
		if (pMarkGlyph!=NULL)
			RemoveMark();

		pMarkGlyph = pNewGroup;
	}
	else
	{
		if (!pNewMark->MakeAttributeComplete(NULL,TRUE,NULL,TRUE))
			return FALSE;

		pNewMark->UnlinkNodeFromTree();
				
		// Make sure we get rid of any previously cached mark
		if (pMarkGlyph!=NULL)
			RemoveMark();

		pMarkGlyph = pNewMark;
	}

	return TRUE;
}


/*****************************************************************************************

>	void PrintMarkItem::RemoveMark()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Remove the cached mark from our list structure. This is generally
				called when the mark item is destroyed.

*****************************************************************************************/

void PrintMarkItem::RemoveMark()
{
	// check to see if we have a sub tree here
	if (pMarkGlyph!=NULL)
	{
		Node* pNode = pMarkGlyph->FindFirstChild();
		if (pNode!=NULL)
			pMarkGlyph->DeleteChildren(pNode);
	}
	delete pMarkGlyph;
	pMarkGlyph = NULL;
}


INT32 PrintMarkItem::GetWidth() const
{
	if (pMarkGlyph!=NULL)
	{
		DocRect bounds = pMarkGlyph->GetBoundingRect();
		return bounds.Width();
	}
	return 0;
}

INT32 PrintMarkItem::GetHeight() const
{
	if (pMarkGlyph!=NULL)
	{
		DocRect bounds = pMarkGlyph->GetBoundingRect();
		return bounds.Height();
	}
	return 0;
}

DocCoord PrintMarkItem::GetOrigin() const
{
	if (pMarkGlyph!=NULL)
	{
		DocRect bounds = pMarkGlyph->GetBoundingRect();
		return bounds.lo;
	}
	return DocCoord(0,0);
}


MarkOrient PrintMarkItem::GetOrient() const
{
	return Mark.GetOrientation();
}


BOOL PrintMarkItem::DefaultsToOn() const
{
	return Mark.DefaultsToOn();
}


BOOL PrintMarkItem::NeedsSaving() const
{
	return Saveable;
}


void PrintMarkItem::SetSaveable(BOOL savestate)
{
	Saveable = savestate;
}

BOOL PrintMarkItem::NeedsToRender() const
{
	return Renderable;
}

void PrintMarkItem::SetRenderable(BOOL renderstate)
{
	Renderable = renderstate;
}



/*****************************************************************************************

>	PrintMarksCache::PrintMarksCache()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Construct the PrintMarksCache object. This used to be part of the print
				marks manager but I've now split it into its own class to prevent
				confusion with the handling of cache marks and document marks. Various
				functions do similar things so it could become confusing without a
				scope resolution operator PMMCache::FindMark as opposed to Doc::FindMark

*****************************************************************************************/

PrintMarksCache::PrintMarksCache()
{
	NextHandle=0;
	TextInfoHandle=0;
	CropMarkHandle=0;
}


/*****************************************************************************************

>	PrintMarksCache::~PrintMarksCache()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Destroys the cached marks completely

*****************************************************************************************/

PrintMarksCache::~PrintMarksCache()
{
	Invalidate();
}


/*****************************************************************************************

>	PrintMarksCache::~PrintMarksCache()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Invalidate the cache list. If we do this, all the mark references
				in existing document will reference nothing. As we create new marks
				new handles will be built, so it might be necessary to delete references
				made in all doc components.
				Use with care.

*****************************************************************************************/

void PrintMarksCache::Invalidate()
{
	// destroy all marks in the cache.
	PrintMarkItem* pItem;
	while (pItem=(PrintMarkItem*)PrintMarkCache.RemoveTail())
		delete pItem;
	// Make sure the crop and text info handles don't reference anything
	CropMarkHandle=0;
	TextInfoHandle=0;
}


/*****************************************************************************************

>	UINT32 PrintMarksCache::GetUniqueHandle()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		-
	Returns:	A handle used to reference a unique print mark
	Purpose:	Generate a unique print mark handle. All handles will be >=1 and hence
				an illegal handle is given the value 0.

*****************************************************************************************/

UINT32 PrintMarksCache::GetUniqueHandle()
{
	NextHandle++;
	return (NextHandle);
}



/*****************************************************************************************

>	PrintMarkItem* PrintMarksCache::GetFirstMark() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		-
	Returns:	A pointer to the first cached mark
	Purpose:	Return a pointer to the first cached mark

*****************************************************************************************/

PrintMarkItem* PrintMarksCache::GetFirstMark() const
{
	return ((PrintMarkItem*)PrintMarkCache.GetHead()); 
}


/*****************************************************************************************

>	PrintMarkItem* PrintMarksCache::GetNextMark(PrintMarkItem* pCurrMark) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		A pointer to a print mark item
	Returns:	A pointer to the next print mark item after this
	Purpose:	Returns the next print mark item beyond pCurrMark

*****************************************************************************************/

PrintMarkItem* PrintMarksCache::GetNextMark(PrintMarkItem* pCurrMark) const
{
	return ((PrintMarkItem*)PrintMarkCache.GetNext(pCurrMark));
}


/*****************************************************************************************

>	BOOL PrintMarksCache::DoesMarkExist(PrintMark* pCheckMark) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		A pointer to a print mark
	Returns:	TRUE if this mark already exists in the cache
				FALSE if not
	Purpose:	Try to find a print mark in the cache

*****************************************************************************************/

BOOL PrintMarksCache::DoesMarkExist(PrintMark* pCheckMark) const
{
	PrintMarkItem* pItem = GetFirstMark();
	while (pItem)
	{
		PrintMark* pCurrMark = pItem->GetPrintMark();
		if (pCheckMark->IsSimilarTo(pCurrMark))
			return TRUE;
		pItem = GetNextMark(pItem);
	}
	return FALSE;
}


/*****************************************************************************************

>	PrintMarkItem* PrintMarksCache::FindMark(UINT32 SearchHandle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		SearchHandle = the handle of the mark to search for
	Returns:	A pointer to the print mark described by this handle (NULL if none)
	Purpose:	Try to find a print mark in the cache, using its handle

*****************************************************************************************/

PrintMarkItem* PrintMarksCache::FindMark(UINT32 SearchHandle)
{
	PrintMarkItem* pItem = GetFirstMark();
	while (pItem)
	{
		if (pItem->GetHandle() == SearchHandle)
			return pItem;
		pItem = GetNextMark(pItem);
	}
	return NULL;
}




/*****************************************************************************************

>	BOOL PrintMarksCache::DecodeCachedLayers(Document* pDocument)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pDocument = a pointer to a document
	Returns:	TRUE if the document has been processed
				
	Purpose:	Given a document, we scan each layer in the document. For each layer
				we check the loaded layer name. If the layer name contains a % at the
				begining (meaning its a format string) and the format is PRINTMARK, we
				take the first group in the layer (+ all its contents) and assign a
				new print mark item to this group. This group object will (if successfully)
				loaded, become a new print mark.
	
*****************************************************************************************/

BOOL PrintMarksCache::DecodeCachedLayers(Document* pDocument)
{
	ERROR2IF(pDocument==NULL,FALSE,"NULL Layer passed to DecodeCachedLayer");
	
	Spread* pSpread = pDocument->FindFirstSpread();
	Layer* pLayer;
	while (pSpread!=NULL)
	{
		pLayer = pSpread->FindFirstLayer();
		while (pLayer!=NULL)
		{
			DecodeCachedLayer(pLayer);
			pLayer = pLayer->FindNextLayer(); // Find next layer
		}
		pSpread = pSpread->FindNextSpread();
	}
	return TRUE;
}


/*****************************************************************************************

>	BOOL PrintMarksMan::DecodeCachedLayer(Layer* pLayer)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pLayer = a pointer to a layer
	Returns:	TRUE if the layer has been processed
	Purpose:	Given a layer, if the layer contains a print mark, strip the print mark
				out and try to add it to the PrintMarksMan cache.
	
*****************************************************************************************/

BOOL PrintMarksCache::DecodeCachedLayer(Layer* pLayer)
{
	ERROR2IF(pLayer==NULL,FALSE,"NULL Layer passed to DecodeCachedLayer");

	// Read in the layer name
	String_256 Name;
	String_16 Token;

	Name = pLayer->GetLayerID();
	Name.Left(&Token, 10);

	BOOL IsValid = FALSE;
	BOOL IsCustomMark = FALSE;
	if (Token == String_16("%PRINTMARK"))				// This text must not be internationalised
		IsValid = TRUE;

	if (!IsValid && Token == String_16("%CUSTOMMAR"))	// This text must not be internationalised
	{
		IsValid = TRUE;
		IsCustomMark = TRUE;
	}

	if (!IsValid)
		return FALSE;

	// find the first renderable group in this layer.
	NodeGroup* pGroup = (NodeGroup*)pLayer->FindFirstChild(CC_RUNTIME_CLASS(NodeGroup));
	if (pGroup==NULL)
		return FALSE;

	// Create a new mark structure to put the decoded data into
	PrintMarkItem* pNewMark = new PrintMarkItem;
	if (!pNewMark)
		return FALSE;

	PrintMark* pMark = pNewMark->GetPrintMark();
	pMark->SetCustomOrDefault(IsCustomMark);		// Remember if it's a custom or default mark
	if (!DecodeMarkFormat(&Name, pMark))
	{
		delete pNewMark;
		return FALSE;
	}

	// Right we have a new print mark descriptor, 
	// we simply merge the mark into the
	// cache. It would be faster, before copying the subtree, to check whether we
	// dont already have this mark cached in the print mark manager.

	if (DoesMarkExist(pMark))
	{
		delete pNewMark;
		return TRUE;
	}

	// Add a handle to this mark
	pNewMark->SetHandle(GetUniqueHandle());

	// ok the mark does not exist, so now look for that first group
	if (!pNewMark->SetMarkGlyph(pGroup, TRUE))
	{
		delete pNewMark;
		return FALSE;
	}

	// Hey hey, we've finally done it! cool!
	PrintMarkCache.AddTail(pNewMark);

	return TRUE;
}


/*****************************************************************************************

>	UINT32 PrintMarksCache::AddNewMark(PrintMarkItem* pNewMark, NodeGroup* pGroup)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pNewMark = a pointer to the new mark to add
				pGroup = a pointer to the glyph to use. 
	Returns:	The handle of the newly added mark
	Purpose:	A simple way of adding a prebuilt mark to the list. The mark must be in
				a renderable format, in that it should be a group with all needed attrs
				as children, followed by a child group containing all the ink objects
	
*****************************************************************************************/

UINT32 PrintMarksCache::AddNewMark(PrintMarkItem* pNewMark, NodeGroup* pGroup)
{
	// Add a handle to this mark
	UINT32 hndle = GetUniqueHandle();
	pNewMark->SetHandle(hndle);
	pNewMark->pMarkGlyph = pGroup;
	// Hey hey, we've finally done it! cool!
	PrintMarkCache.AddTail(pNewMark);
	return hndle;
}


/*****************************************************************************************

>	void PrintMarksCache::UpdateMarkGlyph(PrintMarkItem *pMarkItem, NodeGroup *pGroup)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pMarkItem = a pointer to an existing cached mark
				pGroup    = a pointer to a new glyph to use.
	Returns:	-
	Purpose:	Change the mark glyph to this new mark.

*****************************************************************************************/

void PrintMarksCache::UpdateMarkGlyph(PrintMarkItem *pMarkItem, NodeGroup *pGroup)
{
	if (pMarkItem!=NULL)
	{
		// Make sure there's nothing in there at the mo.
		pMarkItem->RemoveMark();
		// now set the new mark glyph
		pMarkItem->pMarkGlyph = pGroup;
	}
}



/*****************************************************************************************

>	BOOL PrintMarksCache::DecodeMarkFormat(const String_256 *pFormatString, PrintMark* pMark)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		pFormatString
	Outputs:	pMark
	Returns:	TRUE if the format string was valid.
	Purpose:	Decode the string pointed to by pFormatString, placing each format
				identifier into the output mark.

				Special tag			Mark name			ID On/off Type		Position(s)
				%PRINTMARK  = "Registration mark",      3,  1,	_R(ID_STAR), 3MCS + 7MC + 11MCS + 15MC
				%CUSTOMMARK = "User registration mark", 0,  1,	_R(ID_STAR), 3MCS + 7MC + 11MCS + 15MC

*****************************************************************************************/

BOOL PrintMarksCache::DecodeMarkFormat(const String_256 *pFormatString, PrintMark* pMark)
{
	ERROR2IF(pFormatString == NULL, FALSE, "NULL pointer passed to DecodeMarkFormat");
	ERROR2IF(pMark == NULL, FALSE, "NULL mark pointer passed to DecodeMarkFormat");

	String_256 Name(*pFormatString);
	String_256 MenuText;
	String_256 MenuID;
	MarkFormat NewFormat;
	MarkRegion NewRegion;
	MarkOrient NewOrient;

	const TCHAR COMMA	= TEXT(',');		// This text must not be internationalised
	const TCHAR EQUALS	= TEXT('=');
	const TCHAR QUOTES	= TEXT('"');
	const TCHAR PLUS	= TEXT('+');
	const TCHAR SPACE	= TEXT(' ');

	INT32 pos, pos1;

	// Everything is hunky dory at the moment! ER NOT!
	BOOL AllIsWell = FALSE;

	// Turn on the exceptions function in the string reading class
	StringBase::EnableExceptionHandler();

	TRY 
	{
		// toast everything up to the first quote
		pos = Name.FindNextChar(QUOTES);
		Name.Remove(0,pos+1);

		// now find the last quote of the first string
		pos = Name.FindNextChar(QUOTES);
		Name.Left(&MenuText,pos);
		Name.Remove(0,pos+1);

		// skip over the first comma onto the ID field
		pos  = Name.SkipComma();
		Name.Remove(0,pos);
		pos = 0;
		INT32 ID = Name.ConvertToInteger(pos);
		Name.Remove(0,pos);
		if (ID < 0 || ID > 255)
			ID = 0;
		pMark->SetIDByte((BYTE)ID);

		// skip over the next comma onto the enable/disable state
		pos  = Name.SkipComma();
		Name.Remove(0,pos);
		BOOL Enabled = (Name[0] == TEXT('1'));

		// skip comma to next field
		pos  = Name.SkipComma();
		pos1 = Name.FindNextChar(COMMA, pos+1);
		Name.Mid(&MenuID, pos, (pos1-pos));
		Name.Remove(0,pos1+1);

		// turn the menu id into a type of mark if we recognise it
		MarkType actid = (MarkType)DecodeToMarkType(&MenuID);

		// skip the orientation field
		pos=Name.FindNextChar();
		NewOrient=MO_None;
		if (Name[pos] == TEXT('H'))
			NewOrient=MO_Horizontal;
		else if (Name[pos] == TEXT('V'))
			NewOrient=MO_Vertical;

		// remove the orientation bit
		pos = Name.SkipComma();

		// Add what we have so far
		pMark->SetMarkMenuText(&MenuText);
		pMark->SetOrientation(NewOrient);
		pMark->SetType(actid);
		pMark->SetDefaultState(Enabled);

		// The remainder of this string is a format list
		BOOL format=FALSE;
		BOOL eos=FALSE;

		// Turn our exception handler off but don't pop it.
		StringBase::DisableExceptionHandler(FALSE);

		INT32 region;

		while (pos>0)
		{
			// Try to create a new mark to add
			MarkPosition* pNewMark = new MarkPosition;

			if (pNewMark!=NULL)
			{
				region = Name.ConvertToInteger(pos);

				if (region<1 || region>MAXPAGEREGIONS)
				{
					// Ok, the region number is out of range so lets
					// skip to the next one (if there is one!)
					// we need to find the next +
					delete pNewMark;
					pos = Name.FindNextChar(PLUS, pos);
				}
				else
				{
					// store this type of region
					NewRegion = (MarkRegion)(region-1);
					pNewMark->SetRegion(NewRegion);
								
					// if we haven't run off the end of the string
					while ( Name.IsLegal(pos) && (Name[pos]!=PLUS) )
					{
						TCHAR c = Name[pos];
						
						switch (c)
						{
							case TEXT('T'): NewFormat.Top=TRUE;		break;
							case TEXT('M'): NewFormat.Middle=TRUE;	break;
							case TEXT('B'): NewFormat.Bottom=TRUE;	break;
							case TEXT('L'): NewFormat.Left=TRUE;	break;
							case TEXT('C'): NewFormat.Centre=TRUE;	break;
							case TEXT('R'): NewFormat.Right=TRUE;	break;
							default:
								// unknown token, just ignore it!
								if (c != SPACE)
									TRACEUSER( "Mike", _T("Unknown format character during PrintMarksMan::DecodeMarkFormat"));
								break;
						}
						
						pos = Name.FindNextChar(pos+1);
					}

					// make sure the format is ok
					if (!NewFormat.IsValid())
						NewFormat.MakeValid();
					
					// and set it in the structure.
					pNewMark->SetFormat(NewFormat);

					// Add the new mark position to the list.
					pMark->AddNewPosition(pNewMark);

					AllIsWell=TRUE;

					if (Name.IsLegal(pos))
						pos = Name.FindNextChar(pos+1);
				}
			}
			else
			{
				pos = -1;
			}
		}
	}
	CATCH (CUserException, e)
	{
		// An exception has been thrown which means
		// we failed to parse this string, so simply report failure
		AllIsWell = FALSE;
	}
	END_CATCH

	// Switch off the exception state finally, (and of course pop it)!
	StringBase::DisableExceptionHandler();

	return AllIsWell;
}



/*****************************************************************************************

>	UINT32 PrintMarksCache::DecodeToMarkType(String_256* Name) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		Name = a pointer to a string describing the mark type
	Returns:	A unsigned integer describing the mark type (see MarkType for instances)
	Purpose:	Have a look at the string passed in and check to see it matches any of
				the known mark type names. If so return the proper mark type enum, otherwise
				'Unkown' is returned.
	
*****************************************************************************************/

UINT32 PrintMarksCache::DecodeToMarkType(const String_256* Name) const
{
	ERROR2IF(Name == NULL, MarkType_Unknown, "NULL pointer passed to DecodeToMarkType");
	
	if ((*Name)==String_16("_R(ID_NONE)")) 	return MarkType_Unknown;
	if ((*Name)==String_16("_R(ID_STAR)")) 	return MarkType_Star;
	if ((*Name)==String_16("_R(ID_REGM)")) 	return MarkType_Registration;
	if ((*Name)==String_16("_R(ID_CBAR)")) 	return MarkType_ColourBar;
	if ((*Name)==String_16("_R(ID_GBAR)")) 	return MarkType_GreyBar;
	if ((*Name)==String_16("_R(ID_INFO)")) 	return MarkType_Information;
	
	return MarkType_Unknown;
}


/*****************************************************************************************

>	BOOL PrintMarksCache::CreateCropMark()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:
	Returns:	-
	Purpose:	
	
*****************************************************************************************/

BOOL PrintMarksCache::CreateCropMark()
{
	if (CropMarkHandle>0)
		return TRUE;

	// Create a particular mark structure for this crop mark
	PrintMarkItem* pMarkItem = new PrintMarkItem;
	if (pMarkItem==NULL)
		return FALSE;

	MarkPosition* pPosition = new MarkPosition;
	if (pPosition==NULL)
	{
		delete pMarkItem;
		return FALSE;
	}

	MarkFormat format;
	pPosition->SetRegion(MarkRegion_Top1);
	pPosition->SetFormat(format);
	String_256 MenuText(_R(IDS_PRINTMARK_CROPMARKS));

	PrintMark* pMark = pMarkItem->GetPrintMark();
	pMark->SetCustomOrDefault(FALSE);
	pMark->SetIDByte(PRINTMARK_CROPMARKID);
	pMark->SetType(MarkType_Crop);
	pMark->SetMarkMenuText(&MenuText);
	pMark->SetOrientation(MO_Horizontal);
	pMark->SetDefaultState(TRUE);
	pMark->AddNewPosition(pPosition);

	// we don't want to save this mark
	pMarkItem->SetSaveable(FALSE);
	// we render this without formatting in a different way
	pMarkItem->SetRenderable(FALSE);
	// finally stick the thing in our cache!
	CropMarkHandle = AddNewMark(pMarkItem, NULL);
	
	return TRUE;
}



/*****************************************************************************************

>	BOOL PrintMarksCache::CreateTextInfoMark()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		pComp	   = the document component containing the print marks
				CurrRegion = the current region we're formatting marks in
	Returns:	-
	Purpose:	This function creates a text information mark which will describe the
				current print job. As this will be created and destroyed on each print
				it should be reasonably fast to execute. We include the following
				information in the text....

					Document title
					Date
					Time
					Page number?
					Plate name
	
*****************************************************************************************/

BOOL PrintMarksCache::CreateTextInfoMark()
{
	// is it already created?
	if (TextInfoHandle>0)
		return TRUE;

	// Create a particular mark structure for this info
	PrintMarkItem* pMarkItem = new PrintMarkItem;
	if (pMarkItem==NULL)
		return FALSE;

	MarkPosition* pPosition = new MarkPosition;
	if (pPosition==NULL)
	{
		delete pMarkItem;
		return FALSE;
	}

	MarkFormat format;
	format.Middle = TRUE;
	format.Left = TRUE;
	pPosition->SetRegion(MarkRegion_Top2);
	pPosition->SetFormat(format);

	String_256 MenuText(_R(IDS_PRINTMARK_PLATEINFO));
	PrintMark* pMark = pMarkItem->GetPrintMark();
	pMark->SetCustomOrDefault(FALSE);
	pMark->SetIDByte(PRINTMARK_INFOMARKID);
	pMark->SetType(MarkType_Information);
	pMark->SetMarkMenuText(&MenuText);
	pMark->SetOrientation(MO_Horizontal);
	pMark->SetDefaultState(TRUE);
	pMark->AddNewPosition(pPosition);

	// we don't want to save this mark
	pMarkItem->SetSaveable(FALSE);
	// finally stick the thing in our cache!
	TextInfoHandle = AddNewMark(pMarkItem, NULL);
	
	return TRUE;
}



/*****************************************************************************************

>	BOOL PrintMarksCache::UpdateTextInfoMark(Document* pDocument, RenderRegion* pRRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		pComp	   = the document component containing the print marks
				CurrRegion = the current region we're formatting marks in
	Returns:	-
	Purpose:	This function creates a text information mark which will describe the
				current print job. As this will be created and destroyed on each print
				it should be reasonably fast to execute. We include the following
				information in the text....

					Document title
					Date
					Time
					Page number?
					Plate name
	
*****************************************************************************************/

BOOL PrintMarksCache::UpdateTextInfoMark(Document* pDocument, RenderRegion* pRRegion)
{
	// We should at least have one of these.
	if (TextInfoHandle==0)
		return FALSE;

	// find it in the cache
	PrintMarkItem *pMarkItem = FindMark(TextInfoHandle);
	if (!pMarkItem)
		return FALSE;

	// first put all the information we need to display in a string.
	String_256 TextInfo;
	BuildTextInfoString(pDocument, pRRegion, &TextInfo);

	// create a proper mark structure to fit this mark in
	NodeGroup* pNewGroup = new NodeGroup;
	if (pNewGroup==NULL)
		return FALSE;

	NodeGroup* pNewGroup1 = new NodeGroup;
	if (pNewGroup1==NULL)
	{
		delete pNewGroup;
		return FALSE;
	}
/*
	// Make a text style LOGFONT to describe the text information
	LOGFONT Style;
	Style.lfHeight			=0x08;		// 8pt text
	Style.lfWidth			=0x05;
	Style.lfEscapement		=0x00;
	Style.lfOrientation		=0x00;
	Style.lfWeight			=0x190;
	Style.lfItalic			=0x00;
	Style.lfUnderline		=0x00;
	Style.lfStrikeOut		=0x00;
	Style.lfCharSet			=0x00;
	Style.lfOutPrecision	=0x03;
	Style.lfClipPrecision	=0x02;
	Style.lfQuality			=0x01;
	Style.lfPitchAndFamily	=0x12;
	camStrncpy(Style.lfFaceName, TEXT("Times New Roman\0"), 16);
*/
	String_64 FontName("Courier New");
	MILLIPOINT FontSize = 9*1000;

	// now create the text story to put in the mark.
	TextStory *pStory;
	DocColour TextCol(COLOUR_BLACK);
	pStory = TextStory::CreateFromChars(DocCoord(0,0), (TCHAR *)TextInfo, NULL, pDocument, NULL, FALSE, &TextCol);
	if (pStory==NULL)
	{
		// deleting this will delete the mark position
		delete pNewGroup1;
		delete pNewGroup;
		return FALSE;
	}

	// ok make sure the attributes are set nicely
	if (!SetInfoMarkAttributes(pStory,&FontName,FontSize))
	{
		delete pStory;
		delete pNewGroup1;
		delete pNewGroup;
		return FALSE;
	}

	// Turn the print as shapes on. This means we will render it rather
	// than GDI rendering it. It also means the text goes out after our header
	// which sets things like Negative transfer. If we don't render the
	// text we wont have put out our header yet..... Hence the text has a habit
	// of vanishing on 'Negative' prints, black on black n all that. We could
	// set the colour of the text of course, hmm maybe? This is quicker for now.
	pStory->SetPrintingAsShapes(TRUE);

	// attach this new group to the tree
	pNewGroup1->AttachNode(pNewGroup,FIRSTCHILD);
	// attach the story to the group
	pStory->AttachNode(pNewGroup1,FIRSTCHILD);

	// We need to attach this structure to the tree, at the insertion node and makeattributes complete on it.
	pDocument->InsertNewNode(pNewGroup,NULL);

/*
	// make sure we have all the attributes we're gonna need
	if (!pNewGroup->MakeAttributeComplete(NULL,TRUE,NULL,TRUE))
	{
		pNewGroup->CascadeDelete();
		delete pNewGroup;
		return FALSE;
	}
*/
	// time to format the story into shape.
	if (!pStory->FormatAndChildren())
	{
		pNewGroup->CascadeDelete();
		delete pNewGroup;
		return FALSE;
	}

	// ok this is silly, but here we go, make the text into shapes so
	// it bloomin well prints properly.
	NodeGroup* pNGroup = new NodeGroup;
	if (pNGroup!=NULL)
	{
		// stuff it in the tree
		pNGroup->AttachNode(pNewGroup1,NEXT);
		// copy all children as shapes
		if (pNewGroup1->CopyChildrenAsShapes(pNGroup))
		{
			// rip the node out of the tree and stick it in our cache.
			pNGroup->UnlinkNodeFromTree();
			// destroy the first group we created
			pNewGroup->CascadeDelete();
			delete pNewGroup;
			// finally stick the thing in our cache!
			// rip the node out of the tree and stick it in our cache.
			// finally stick the thing in our cache!
			UpdateMarkGlyph(pMarkItem, pNGroup);
			return TRUE;
		}
		pNGroup->CascadeDelete();
		delete pNGroup;
	}

	// rip the node out of the tree and stick it in our cache.
	pNewGroup->UnlinkNodeFromTree();
	// finally stick the thing in our cache!
	UpdateMarkGlyph(pMarkItem, pNewGroup);

	return TRUE;
}


/*****************************************************************************************

>	BOOL PrintMarksCache::SetInfoMarkAttributes(TextStory *pStory, String_64 *pFontName, MILLIPOINT FontSize)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		pComp	   = the document component containing the print marks
				CurrRegion = the current region we're formatting marks in
	Returns:	-
	Purpose:	This function creates a text information mark which will describe the
				current print job. As this will be created and destroyed on each print
				it should be reasonably fast to execute. We include the following
				information in the text....

					Document title
					Date
					Time
					Page number?
					Plate name
	
*****************************************************************************************/

BOOL PrintMarksCache::SetInfoMarkAttributes(TextStory *pStory, String_64 *pFontName, MILLIPOINT FontSize)
{
	BOOL ok;

	// Ensure the font is cached. The fontmangler will return a replace font if
	// it can't find the required one.
	WORD Handle = FONTMANAGER->CacheNamedFont(pFontName);
	if (Handle==ILLEGALFHANDLE)
		return FALSE;

	// find the first line in this story
	TextLine* pTextLine = pStory->FindFirstLine();
	if (pTextLine==NULL)
		return FALSE;

	// Delete the current instances of the attribute first
	ok = DeleteChildAttribute(pTextLine, CC_RUNTIME_CLASS(AttrTxtFontTypeface));
	if (!ok) ok = DeleteChildAttribute(pStory, CC_RUNTIME_CLASS(AttrTxtFontTypeface));

	ok = DeleteChildAttribute(pTextLine, CC_RUNTIME_CLASS(AttrTxtFontSize));
	if (!ok) ok = DeleteChildAttribute(pStory, CC_RUNTIME_CLASS(AttrTxtFontSize));
	
	// now create new ones
	TxtFontTypefaceAttribute FontAttr(Handle);
	TxtFontSizeAttribute     SizeAttr(FontSize);

			ok = ((AttributeValue*)&FontAttr)->MakeNode(pTextLine,PREV) != NULL;
 	if (ok) ok = ((AttributeValue*)&SizeAttr)->MakeNode(pTextLine,PREV) != NULL;

	return ok;
}

/***********************************************************************************************

>	BOOL PrintMarksCache::DeleteChildAttribute(NodeRenderableInk* pParent, CCRuntimeClass* pReqdAttrib)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		-
	Purpose:	Delete a child attribute of a specified type from this parent.

***********************************************************************************************/

// Should this be in Node.h	?

BOOL PrintMarksCache::DeleteChildAttribute(NodeRenderableInk* pParent, CCRuntimeClass* pReqdAttrib)
{
	ERROR2IF(pParent==NULL || pReqdAttrib==NULL, FALSE, "NULL parameter");
	NodeAttribute* pAppliedAttr = pParent->GetChildAttrOfType(pReqdAttrib);
	if (!pAppliedAttr)
		return FALSE;

	pAppliedAttr->CascadeDelete();
	delete pAppliedAttr;
	return TRUE;
}




/***********************************************************************************************

>	void PrintMarksCache::DestroyTextInfoMark()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		-
	Purpose:	Throw away any cached print information mark

***********************************************************************************************/

void PrintMarksCache::DestroyTextInfoMark()
{
	if (TextInfoHandle>0)
	{
		PrintMarkItem *pItem = FindMark(TextInfoHandle);
		if (pItem!=NULL)
		{
			pItem = (PrintMarkItem*)PrintMarkCache.RemoveItem(pItem);
			if (pItem) delete pItem;
		}
	}
	TextInfoHandle=0;
}


/***********************************************************************************************

>	void PrintMarksCache::BuildTextInfoString(Document *pDocument, RenderRegion* pRRegion, String_256 *pString)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		pMark = a print mark item pointer
				pForm = a mark format object pointer
	Purpose:	

***********************************************************************************************/

void PrintMarksCache::BuildTextInfoString(Document *pDocument, RenderRegion* pRRegion, String_256 *pString)
{
	// First add the doc title to the string
	(*pString) = pDocument->GetTitle();
	(*pString) += TEXT("  ");

	// now add the date and time (set up by StartPrinting above)
	(*pString) += CachedTime;

	// ok add the plate name of the plate we're rendering
	ColourPlate *pColPlate = pRRegion->GetRenderView()->GetColourPlate();
	if (pColPlate!=NULL)
	{
		String_64 PlateName;
		
		pColPlate->GetDescription(&PlateName);
		(*pString) += TEXT("  (");
		(*pString) += PlateName;
		(*pString) += TEXT(")");
	}
}



/********************************************************************************************

>	PageRectList::PageRectList()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:	The page rect list and all its helper functions provide for the upkeep of
				a list of rectangles. These rectangles generally specify the positions of
				printing pages on a single sheet of paper. For instance, you might be printing
				a single page as 4x4 multiple fit to one sheet of paper. You would then have
				16 rectangle entries in this list. For each entry, the print marks manager
				will render crop marks around the outside of the paper to match up with each
				rectangle. Cool hu?
	SeeAlso:

********************************************************************************************/

PageRect::PageRect(const DocRect &rect)
{
	Rect = rect;
}

PageRectList::~PageRectList()
{
	DeleteAll();
}

void PageRectList::DeleteAll()
{
	PageRect* pItem;
	while (pItem=((PageRect*)RemoveTail()))
		delete pItem;
}

inline PageRect* PageRectList::GetFirstPageRect() const
{
	return ((PageRect*)GetHead());
}
		
inline PageRect* PageRectList::GetNextPageRect(PageRect* pItem) const
{
	return ((PageRect*)GetNext(pItem));
}

BOOL PageRectList::AddPageRect(const DocRect &rect)
{
	if (FindPageRect(rect)==NULL)
	{
		PageRect *pRect = new PageRect(rect);
		if (pRect)
		{
			AddTail(pRect);
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

PageRect* PageRectList::FindPageRect(const DocRect &rect) const
{
	PageRect* pItem = GetFirstPageRect();
	while (pItem)
	{
		if (pItem->Rect == rect)
			return pItem;
		pItem = GetNextPageRect(pItem);
	}
	return NULL;
}






/*****************************************************************************************

>	PrintMarksMan::PrintMarksMan()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Constructor for the PrintMarks Manager

*****************************************************************************************/

PrintMarksMan::PrintMarksMan()
{
	Bleed = 0;
	TemplateCached=FALSE;
	EmulsionDown=FALSE;
}

/*****************************************************************************************

>	PrintMarksMan::PrintMarksMan()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Destruct the PrintMarks manager. We delete all remaining cached items
				here.

*****************************************************************************************/

PrintMarksMan::~PrintMarksMan()
{
}


/*****************************************************************************************

>	BOOL PrintMarksMan::IsTemplateCached() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Returns:	TRUE if we've cached the print marks template once during the life
				     of the program
				FALSE if not.
	Purpose:	Return whether the disc based template file of marks has been cached once
				already.

*****************************************************************************************/

BOOL PrintMarksMan::IsTemplateCached() const
{
	return TemplateCached;
}



/*****************************************************************************************

>	BOOL PrintMarksMan::ConstructCache()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Here we construct the cache afresh. Well almost afresh, we need to 
				do the following
				
				(1) find the disc file containing our marks
				(2) load the file as a document
				(3) convert all index colours used in objects to lookalike doc colours
				(4) enumerate each layer and decode the layer name
				(5) find the group inside the layer which contains the mark
				(6) make a copy of this information as a PrintMarkItem
				(7) merge the PrintMarkItem with the current contents of the cache.
				    This is v.important as there will be valuable information already
					in the cache we don't want to lose.
				(8) destroy the document on completion (or partial completion==failure)

				easy really. We use various lower level functions to perform all of course.

*****************************************************************************************/

BOOL PrintMarksMan::ConstructCache()
{
	// If its already cached then ignore this call.
	if (TemplateCached)
		return TRUE;

	BOOL ok=FALSE;
	// otherwise do some serious stuff.
	TCHAR SearchFile[] = "mrktmpl.xar";
	TCHAR *OutputBuff = new TCHAR[_MAX_PATH];
	if (OutputBuff)
	{
		LoadPrintMarks LdPrintMark;
		// try to find the search file...
		ok = FileUtil::FindResourceFile(SearchFile, OutputBuff);
		// Add a crop mark bits.
		ok = ok && PMMCache.CreateCropMark();
		// Add a text info mark too.
		ok = ok && PMMCache.CreateTextInfoMark();
		// Get the winoil level direct loader to do its stuff
		ok = ok && LdPrintMark.Execute(OutputBuff);
		// delete the allocated buffer
		delete OutputBuff;
	}
	// return all is well/not well
	return (TemplateCached=ok);
}



/*****************************************************************************************

>	virtual BOOL LoadPrintMarks::OnLoadDocument(Document* pKernelDoc)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/08/96
	Inputs:		pKernelDoc = a pointer to the loaded kernel document
	Returns		TRUE if the document was processed correctly
				FALSE if not.
	Purpose:	Decode all print marks in this loaded document.

*****************************************************************************************/

BOOL LoadPrintMarks::OnLoadDocument(Document* pKernelDoc)
{
	// Get a pointer to the marks manager
	PrintMarksMan* pPrintMarksMan = GetApplication()->GetMarksManager();
	if (pPrintMarksMan==NULL)
		return FALSE;
	
	// ok do the conversion of all colours.
	pPrintMarksMan->ConvertAllDocColours(pKernelDoc);
	
	// ok we now have a loaded document, lets scan of the all the layers we need.
	// should we respond to an error from here?
	pPrintMarksMan->PMMCache.DecodeCachedLayers(pKernelDoc);
	
	// All is well we hope.
	return TRUE;
}




/*****************************************************************************************

>	void PrintMarksMan::ConvertAllDocColours(Document* pDoc)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> / Mike
	Created:	31/07/96
	Purpose:	Convert all the index colours in the document to local doc colours. This
				avoids problems with copying data out of the tree and deleting the index
				colours. What does this mean. Ok, if you make a copy of the print mark,
				put the copy in the PMM and delete the original document, you will
				delete the doc component colour list which contains all used index colour
				definitions. These index col defs contain usage counts which will be
				incremented during the copy. An attempt to delete all these index colours
				will cause an error (index colour still in use) even though its not in
				use in the document. Hence we need to remove all index colour references
				from the document before we make a copy. This is what the function does
				by making local doc colours.

*****************************************************************************************/

void PrintMarksMan::ConvertAllDocColours(Document* pDoc)
{
	ERROR3IF(pDoc==NULL,"No doc pointer during PrintMarksMan::ConvertAllDocColours!");

	Node *CurNode = Node::DocFindFirstDepthFirst(pDoc);
	Node *NextNode;

	while (CurNode !=NULL)
	{
		// We may be about to chop this node out of the tree, so get the next node now
		NextNode = CurNode->DocFindNextDepthFirst();

		// Use to scan the colour fields of the attribute.
		UINT32 Context = 0;
		DocColour *pColour;

		if (CurNode->IsAnAttribute())
		{
			NodeAttribute *pNodeAttr = (NodeAttribute *) CurNode;

			// Get the next colour field from the attribute
			pColour = pNodeAttr->EnumerateColourFields(Context++);

			while (pColour != NULL)
			{
				// For each colour field, make sure the colour is a local DocColour so that
				// the sub-tree is entirely stand-alone
				if (pColour->FindParentIndexedColour() != NULL)
				{
					ColourGeneric ColDef;
					ColourContext *cc = ColourManager::GetColourContext(pColour->GetColourModel());
					ERROR3IF(cc == NULL, "Can't find colour context?!");

					// Get the IndexedColour definition as a standalone colour definition
					cc->ConvertColour(pColour->FindParentIndexedColour(), &ColDef);

					// Make the DocColour into a simple standalone "lookalike" of the parent colour
					*pColour = DocColour(pColour->GetColourModel(), &ColDef);
				}

				pColour = pNodeAttr->EnumerateColourFields(Context++);
			}
		}
		CurNode = NextNode;
	}
}


/*****************************************************************************************

>	void PrintMarksMan::InvalidateCache()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		-
	Returns:	-
	Purpose:	Throws away the cache entirely. We unset the 'Template has been cached'
				variable which will result in a proper refiltering of the mark template
				file the next time we initialise.
	
*****************************************************************************************/

void PrintMarksMan::InvalidateCache()
{
	// Force a proper recache next time the marks are accessed
	TemplateCached=FALSE;
	// Destroy everything in the current marks list
	PMMCache.Invalidate();
}



/*****************************************************************************************

>	BOOL PrintMarksMan::AddMarkToDoc(UINT32 MarkHandle, Document* pDocument)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		MarkHandle = the handle of the mark to add
				pDocument = a pointer to a document to add the mark to
	Returns:	TRUE if the mark has been added,
				FALSE if not.
	Purpose:	Add a mark reference to the documents displayable printer marks
	
*****************************************************************************************/

BOOL PrintMarksMan::AddMarkToDoc(UINT32 MarkHandle, Document* pDocument)
{
	ERROR2IF(pDocument==NULL,FALSE,"NULL document pointer passed to AddMarkToDoc!");
	// Sanity check, make sure this mark handle actually exists
	if (!PMMCache.FindMark(MarkHandle))
	{
		ERROR3("Illegal handle given to PrintMarksMan::AddMarkToDoc");
		return FALSE;
	}

	// ok, try to find the print marks component in this document.
	PrintMarksComponent* pComponent = 
	(PrintMarksComponent*)pDocument->GetDocComponent(CC_RUNTIME_CLASS(PrintMarksComponent));
	if (!pComponent)
	{
		ERROR3("We're stimmied, there's no PrintMarksComponent in this document!");
		return FALSE;
	}

	// super duper, we've got the doc component, so lets add our mark reference to
	// its list.
	return pComponent->AddMark(MarkHandle);
}




/*****************************************************************************************

>	BOOL PrintMarksMan::RemoveMarkFromDoc(UINT32 MarkHandle, Document* pDocument)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		MarkHandle = the handle of the mark to remove
				pDocument = a pointer to a document to remove the mark from
	Returns:	TRUE if the mark has been removed
				FALSE if not. This result will occur only when the document does not
				contain a print marks component or I cannot find the specified mark.
				If the mark exists it will never fail to be removed.
	Purpose:	Removes the mark reference held in the print marks doc component inside
				this document.
	
*****************************************************************************************/

BOOL PrintMarksMan::RemoveMarkFromDoc(UINT32 MarkHandle, Document* pDocument)
{
	ERROR2IF(pDocument==NULL,FALSE,"NULL document pointer passed to AddMarkToDoc!");
	// ok, try to find the print marks component in this document.
	PrintMarksComponent* pComponent = 
	(PrintMarksComponent*)pDocument->GetDocComponent(CC_RUNTIME_CLASS(PrintMarksComponent));
	if (!pComponent)
	{
		ERROR3("We're stimmied, there's no PrintMarksComponent in this document!");
		return FALSE;
	}

	// super duper, we've got the doc component, so lets add our mark reference to
	// its list.
	pComponent->RemoveMark(MarkHandle);
	return TRUE;
}




/*****************************************************************************************

>	BOOL PrintMarksMan::AddDefaultMarksToDoc(Document* pDocument)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		pDocument = a pointer to a document to add the default set to
	Returns:	TRUE if all 'defaults to on' marks have been added,
				FALSE if none have.
	Purpose:	Add all 'defaults to on' mark references to the documents displayable 
				printer marks.
	
*****************************************************************************************/

BOOL PrintMarksMan::AddDefaultMarksToDoc(Document* pDocument)
{
	ERROR3IF(pDocument==NULL,"NULL document pointer passed to AddDefaultMarks()");
	BOOL Added=FALSE;
	if (pDocument!=NULL)
	{
		// Get that print marks doc component
		PrintMarksComponent* pMarksComp = 
		(PrintMarksComponent*)pDocument->GetDocComponent(CC_RUNTIME_CLASS(PrintMarksComponent));
		if (pMarksComp && pMarksComp->IsVirgin())
		{
			// now go through each print mark and add it if necessary
			PrintMarkItem* pMarkItem = PMMCache.GetFirstMark();
			while (pMarkItem)
			{
				// check to see if this mark is on by default
				if (pMarkItem->DefaultsToOn())
				{
					// Add this item to the document
					UINT32 handle = pMarkItem->GetHandle();
					pMarksComp->AddMark(handle);
				}
				// find the next print mark item
				pMarkItem = PMMCache.GetNextMark(pMarkItem);
			}
			// ok we've tried to add em.
			Added=TRUE;
		}
	}
	return Added;
}







/*****************************************************************************************

>	void PrintMarksMan::SetImagesettingRect()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		-
	Returns:	-
	Purpose:	Define the print marks rectangle. During a print this rectangle will
				determin where print marks will be placed. For instance if you are
				printing a 2x2 multiple fit page, the print marks rectangle will be the
				union of all these page tiles. Hence the print marks will be places around
				the entire print area. Crop marks will be placed in this area too but
				will be positioned to align with each page.
	Notes:		The union of the PageRects, added while printing the current paper will
				give the imagesetting rectangle. Each one of these PageRects needs a
				set of crop marks.
	
*****************************************************************************************/

void PrintMarksMan::SetImagesettingRect()
{
	ImagesettingRect.MakeEmpty();
	PageRect* pRect = PageRects.GetFirstPageRect();
	while (pRect)
	{
		ImagesettingRect = ImagesettingRect.Union(pRect->Rect);
		pRect = PageRects.GetNextPageRect(pRect);
	}
}




/*****************************************************************************************

>	void PrintMarksMan::StartPrinting()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		-
	Returns:	-
	Purpose:	Called from the print control system to tell the print mark manager that
				printing has started. Here we make sure all our print time local variables
				are set to default values.
	
*****************************************************************************************/

void PrintMarksMan::StartPrinting()
{
	// destroy the current set of rectangles
	PageRects.DeleteAll();
	ImagesettingRect.MakeEmpty();
	EmulsionDown=FALSE;
	Bleed=0;
	// now cache the print time
	CCTime MyTime;
	PMMCache.CachedTime.Empty();
	String_64 TimeFormat("%dy/%mt/%yr  %th.%mn %pm");
	MyTime.ConvertStandardDateAndTime(&TimeFormat, &PMMCache.CachedTime);
}


/*****************************************************************************************

>	void PrintMarksMan::EndPrinting()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		-
	Returns:	-
	Purpose:	Called from the print control system to tell the print mark manager that
				printing has ended. Here we delete all the page rectangle positions added
				to our cached list.
	
*****************************************************************************************/

void PrintMarksMan::EndPrinting()
{
	PageRects.DeleteAll();
	ImagesettingRect.MakeEmpty();
}



/*****************************************************************************************

>	void PrintMarksMan::SetBleed(MILLIPOINT bleed)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		bleed = the bleed used during this print
	Returns:	-
	Purpose:	Tells the print marks manager what bleed to use during this print pass.
				This is usually a globally set value and is unlikely to change across
				plates or pages. We use the value to inflate the typesetting rect by.
	
*****************************************************************************************/

void PrintMarksMan::SetBleed(MILLIPOINT bleed)
{
	Bleed=bleed;
}


/*****************************************************************************************

>	BOOL PrintMarksMan::AddPageRect(const DocRect& rect)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		rect = a document rectangle specifying the a pages position in the
					   print paper coordinate system
	Returns:	-
	Purpose:	This function provides the ability to specify n page rectangles. Each of
				these rectangles will have crop marks positioned around the paper edge to
				line up with this rectangle. Hence the print system uses this function
				to specify the final positions of all the multi fit page rectangles during
				a print. We make sure we then create and render the right number of crop
				marks to fit this rectangle list.
	SeeAlso:	ScreenView::EnumeratePagePatches().
	
*****************************************************************************************/

BOOL PrintMarksMan::AddPageRect(const DocRect& rect)
{
	// add this rectangle
	return PageRects.AddPageRect(rect);
}



/*****************************************************************************************

>	void PrintMarksMan::SetEmulsionDown(BOOL state)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		state = the new state of the emulsion down flag the print marks manager
						will use during this print
	Returns:	-
	Purpose:	Tells the print marks manager which way up the emulsion is. This determins
				whether the marks manager will flip its marks during rendering or not.
	
*****************************************************************************************/

void PrintMarksMan::SetEmulsionDown(BOOL state)
{
	EmulsionDown=state;
}	


/*****************************************************************************************

>	void PrintMarksMan::ResetPageRects()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		-
	Returns:	-
	Purpose:	Destroys the list of page rectangles the print marks manager caches.
	
*****************************************************************************************/

void PrintMarksMan::ResetPageRects()
{
	PageRects.DeleteAll();
}



/*****************************************************************************************

>	MILLIPOINT PrintMarksMan::AdjustedBleed() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/96
	Inputs:		-
	Returns:	the size of the bleed as we know it
	Purpose:	Internal function to get the adjusted bleed value
	
*****************************************************************************************/

MILLIPOINT PrintMarksMan::AdjustedBleed() const
{
	return (Bleed+OUTSIDEBLEEDWIDTH);
}

/*****************************************************************************************

>	void PrintMarksMan::RenderPrintMarks(Document *ScopeDoc, BOOL DisplayEm=TRUE, BOOL LoadTemplateMarks=TRUE);

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>/Jason
	Created:	... / 28/8/96
	Inputs:		ScopeDoc - points to the document for which you want to find out.
				Pass in NULL to use the selected doc.

				DisplayEm - TRUE to enable, FALSE to disable, printing of printers marks

				LoadTemplateMarks - TRUE to ensure that the printers marks are cached in the
				PMM, or FALSE to merely enable/disable printing thereof. Note that if the PMM
				cache is not initialised, then no print marks are available, and hence this
				variable defaults to TRUE. If they're already cached, nothing is done.

	Purpose:	Enable/Disable output of printers marks for the given document

	SeeAlso:	PrinterMarksMan::ShowPrintMarks;
				TypesetInfo::SetOutputPrintersMarks

*****************************************************************************************/

void PrintMarksMan::RenderPrintMarks(Document *ScopeDoc, BOOL DisplayEm, BOOL UpdateCache)
{
	TypesetInfo *pInfo = TypesetInfo::FindTypesetInfoForDoc(ScopeDoc);
	if (pInfo != NULL)
		pInfo->SetOutputPrintersMarks(DisplayEm);

	if (DisplayEm && UpdateCache)
		ConstructCache();
}



/*****************************************************************************************

>	BOOL PrintMarksMan::ShowPrintMarks(Document *ScopeDoc) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/8/96
	Inputs:		ScopeDoc - points to the document for which you want to find out.
				Pass in NULL to use the selected doc.

	Returns:	TRUE if printers marks will be output when printing this document

	Purpose:	Determine if a docuemnt will output printers marks when printing

	SeeAlso:	PrinterMarksMan::RenderPrintMarks;
				TypesetInfo::SetOutputPrintersMarks

*****************************************************************************************/

BOOL PrintMarksMan::ShowPrintMarks(Document *ScopeDoc) const
{
	TypesetInfo *pInfo = TypesetInfo::FindTypesetInfoForDoc(ScopeDoc);
	if (pInfo != NULL)
		return(pInfo->OutputPrintersMarks());

	return(FALSE);
}


MILLIPOINT PrintMarksMan::GetCropMarkWidth(Document *ScopeDoc) const
{
	if (ShowPrintMarks(ScopeDoc))
		return (CROPMARK_LENGTH + OUTSIDEBLEEDWIDTH);
	else
		return 0;
}



/*****************************************************************************************

>	void PrintMarksMan::RenderPrintMarks(PrintMarksComponent *pComp, 
										 RenderRegion* pRRegion, 
										 const Matrix &ViewTrans,
										 const DocRect &ClipRect,
										 Spread* pSpread)
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		pComp	= the document component containing the print marks
				pRRegion= the render region to draw in
				pSpread = the spread to draw
	Returns:
	Purpose:	Render all print marks contained in this doc component and the crop
				marks if they are enabled.
	
*****************************************************************************************/

void PrintMarksMan::RenderPrintMarks(PrintMarksComponent *pComp, 
									 RenderRegion* pRRegion,
									 const Matrix &ViewTrans,
									 const DocRect &ClipRect,
									 Spread* pSpread)
{
	// if any of these are null we can't do the job
	if (pComp==NULL || pRRegion==NULL || pSpread==NULL)
		return;

	// do we need to display our marks?
	if ( !ShowPrintMarks( (Document *) (pSpread->FindOwnerDoc()) ) )
		return;

	// if there's no imagesetting rect to render around we can't position our marks
	if (ImagesettingRect.IsEmpty())
		return;

	// Go get the adjusted bleed
	MILLIPOINT adjbleed = AdjustedBleed();

	// is this region a postscript render region?
	BOOL IsPostScript = pRRegion->IS_KIND_OF(PrintPSRenderRegion);
	BOOL OldClipState = FALSE;
	PSPrintDC *pPrintDC = NULL;

	// configure the postscript clipping region
	if (IsPostScript)
	{
		// Get the render DC
		pPrintDC = (PSPrintDC*)pRRegion->GetRenderDC();
		if (pPrintDC==NULL)
			return;

		// Disable the print clipping for the moment
		OldClipState = pPrintDC->SetClipping(FALSE);
	}
		
	// check for any user doc marks
	DocPrintMark* pDocMark = pComp->GetFirstMark();
	if (pDocMark!=NULL)
	{
		// Update the text info mark now. This builds the time/date etc.
		Document* pDoc = Document::GetSelected();
		PMMCache.UpdateTextInfoMark(pDoc, pRRegion);

		// get the bleed size used in this spread
		RenderPrintMarksAroundRect(pComp, pRRegion, NULL, ImagesettingRect, adjbleed);

		// ok are crop marks created?
		if (PMMCache.CropMarkHandle>0)
		{
			// and are they in this document component?
			if (pComp->FindMark(PMMCache.CropMarkHandle)!=NULL)
				RenderCropMarksAroundRect(pRRegion, NULL, ImagesettingRect, adjbleed);
		}
	}

	// finalise any bleed area clipping in PostScript
	if (IsPostScript)
	{
		if (pPrintDC)
		{
			// Reset the clipping state of affairs.
			pPrintDC->SetClipping(TRUE); //OldClipState);
			// Export the cliptobleed stuff
			pPrintDC->OutputPSClipping();
			// ClipToBleed(pRRegion,pSpread);
		}
	}
}




/*****************************************************************************************

>	woid PrintMarksMan::ClipToBleed(RenderRegion *pRRegion,Spread *pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/8/96
	Inputs:		pRRegion = a pointer to a render region
				Pass in NULL to use the selected doc.
	Returns:	-
	Purpose:	Export the bleed rectangle as the current clipping region. All graphics
				will be clipped to this region.

*****************************************************************************************/

void PrintMarksMan::ClipToBleed(RenderRegion* pRRegion,Spread *pSpread)
{
	// if any of these are null we can't do the job
	if (pRRegion==NULL)
		return;

	// if there's no imagesetting rect to render around we can't position our marks
	if (ImagesettingRect.IsEmpty())
		return;

	// do we need to display our marks?
	if ( !ShowPrintMarks( (Document *) (pSpread->FindOwnerDoc()) ) )
		return;

	// dont render to a none postscript region
	if ( !IS_A(pRRegion, PrintPSRenderRegion) )
		return;

	// BODGE ALERT...
	// Clip everything else to this imagesetting rectangle

	// Go get the adjusted bleed
	MILLIPOINT adjbleed = AdjustedBleed();
	DocRect Rect = ImagesettingRect;
	Rect.Inflate(adjbleed);
	PSPrintDC *pPrintDC = (PSPrintDC*)pRRegion->GetRenderDC();
	((PrintPSRenderRegion*)pRRegion)->WriteClipRegion(pPrintDC, Rect);

	// BODGE ALERT
	// Mike 02/09/96 - We should be performing stacked clipping. This is not the place
	//                 to alter the postscript clip region but if will suffice for now.
}



/*****************************************************************************************

>	void PrintMarksMan::RenderCropMarksAroundRect( RenderRegion* pRRegion,
												   const Matrix &trans
												   const DocRect& Rect, 
												   MILLIPOINT bleed)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		pRRegion= the render region to draw in
				pSpread = the spread to draw
	Returns:
	Purpose:	Render all print marks contained in this doc component and the crop
				marks if they are enabled around the given rectangle.
	
*****************************************************************************************/

void PrintMarksMan::RenderCropMarksAroundRect(RenderRegion* pRRegion,
											  const Matrix *pTrans,
											  const DocRect &Rect,
											  MILLIPOINT bleed)
{
	// create a cropline path and set it up
	Path CropLine;
	if (!CropLine.Initialise())
		return;

	// an extra bit we pust the marks out by if the bleed is zero		
	INT32 ExtraBit = 0;
	if (bleed==0)
		ExtraBit = 2384;

	// transform the rectangle if necessary
	DocRect TransformedRect(Rect);
	TransformedRect.Inflate(bleed+ExtraBit);
	if (pTrans)
	{
		pTrans->transform(&TransformedRect.lo);
		pTrans->transform(&TransformedRect.hi);
	}

	MILLIPOINT Tmp;

	if (TransformedRect.lo.x > TransformedRect.hi.x)
	{
		Tmp = TransformedRect.lo.x;
		TransformedRect.lo.x = TransformedRect.hi.x;
		TransformedRect.hi.x = Tmp;
	}

	if (TransformedRect.lo.y > TransformedRect.hi.y)
	{
		Tmp = TransformedRect.lo.y;
		TransformedRect.lo.y = TransformedRect.hi.y;
		TransformedRect.hi.y = Tmp;
	}

	// Calculate the inverse of the render matrix, this allows us to render
	// through a scaling render region and prevent our marks being scaled.
	Matrix ViewTrans = pRRegion->GetMatrix();
	ViewTrans = ViewTrans.Inverse();

	// get the cropmark length
	INT32 Length = CROPMARK_LENGTH;
	double Thickness = CROPMARK_THICKNESS;

	// now adjust it a little as we may be pushing the cropmarks out
	// by 1mm and taking 1mm off the length. This is only done when there
	// is no bleed set and means the cropmarks don't touch each other.
	Length -= ExtraBit;
	if (Length<1) Length=1;

	// ok we need to scale the thickness dependent on the current scale
	// facter within this render region otherwise, the region will scale it
	// for us which would be real bad stuff indeed.
	FIXED16 CurrScale;
	ViewTrans.Decompose(&CurrScale);
	double scale = CurrScale.MakeDouble();
	// Emulsion down does its stuff by getting the render matrix to flip about
	// the Y axis. This shows up in the scale as a negative value returned. We
	// need the absoulte scale.
	if (scale<0) scale = -scale;
	Thickness = Thickness*scale;
	INT32 Thick = (INT32)(Thickness+0.5);
	if (Thick<1) Thick=1;

	// start setting attributes in this render context
	pRRegion->SaveContext();
	pRRegion->SetLineColour(COLOUR_BLACK);
	pRRegion->SetLineWidth(Thick);

	// Set up some dummy data
	CropLine.AddMoveTo(DocCoord(0,0));
	CropLine.AddLineTo(DocCoord(72000,0));
	// Get the coords array in our cropline path
	DocCoord *Coords = CropLine.GetCoordArray();

	// go through all our registered page rectangles and generate
	DocCoord iLo = TransformedRect.lo;
	DocCoord iHi = TransformedRect.hi;

	// process all our page rectangles
	PageRect *pRect = PageRects.GetFirstPageRect();
	DocCoord p0,p1,rLo,rHi;

	// ok finally render them all
#define LINEDRAW(p,q) \
		ViewTrans.transform(&p); \
		ViewTrans.transform(&q); \
		Coords[0]=p;  \
		Coords[1]=q;  \
		pRRegion->DrawPath(&CropLine);
	
	while (pRect)
	{
		rLo = pRect->Rect.lo;
		rHi = pRect->Rect.hi;

		p0.x = iLo.x; 
		p1.x = iLo.x - Length;
		p0.y = p1.y = rLo.y;
		LINEDRAW(p0,p1);
		p0.x = iLo.x; 
		p1.x = iLo.x - Length;
		p0.y = p1.y = rHi.y;
		LINEDRAW(p0,p1);
		
		p0.y = iHi.y;
		p1.y = iHi.y + Length;
		p0.x = p1.x = rLo.x;
		LINEDRAW(p0,p1);
		p0.y = iHi.y;
		p1.y = iHi.y + Length;
		p0.x = p1.x = rHi.x;
		LINEDRAW(p0,p1);
	
		p0.x = iHi.x;
		p1.x = iHi.x + Length;
		p0.y = p1.y = rHi.y;
		LINEDRAW(p0,p1);
		p0.x = iHi.x;
		p1.x = iHi.x + Length;
		p0.y = p1.y = rLo.y;
		LINEDRAW(p0,p1);

		p0.y = iLo.y;
		p1.y = iLo.y - Length;
		p0.x = p1.x = rHi.x;
		LINEDRAW(p0,p1);
		p0.y = iLo.y;
		p1.y = iLo.y - Length;
		p0.x = p1.x = rLo.x;
		LINEDRAW(p0,p1);

		pRect = PageRects.GetNextPageRect(pRect);
	}

#undef LINEDRAW

	// pop the attribute context again
	pRRegion->RestoreContext();
}





/*****************************************************************************************

>	void PrintMarksMan::RenderPrintMarksAroundRect(PrintMarksComponent *pComp,
												   RenderRegion* pRRegion,
												   const Matrix &trans
												   const DocRect& Rect, 
												   MILLIPOINT bleed)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		pComp	= the document component containing the print marks
				pRRegion= the render region to draw in
				pSpread = the spread to draw
	Returns:
	Purpose:	Render all user print marks contained in this doc component 
	
*****************************************************************************************/

void PrintMarksMan::RenderPrintMarksAroundRect(PrintMarksComponent *pComp,
											   RenderRegion* pRRegion,
											   const Matrix *pTrans,
											   const DocRect &Rect,
											   MILLIPOINT bleed)
{
	pRRegion->SaveContext();

	// transform the rectangle if necessary
	DocRect TransformedRect(Rect);
	if (pTrans)
	{
		pTrans->transform(&TransformedRect.lo);
		pTrans->transform(&TransformedRect.hi);
	}

	// construct all 16 regions around this page rect
	PageMarkRegions MyPageRegions;
	MyPageRegions.Initialise(TransformedRect,bleed);
	Matrix OuterTransform;
	if (EmulsionDown)
	{
		DocRect Rect = TransformedRect;
		Rect.Inflate(bleed);
		INT32 w = Rect.Width();
		OuterTransform *= Matrix(-Rect.lo);
		OuterTransform *= Matrix(FIXED16(-1), FIXED16(0), FIXED16(0), FIXED16(1), w, 0);
		OuterTransform *= Matrix(Rect.lo);
	}

	for (INT32 i=0; i<MAXPAGEREGIONS; i++)
	{
		if (MyPageRegions.Region[i].Valid)
		{
			// Make sure the formatting region is empty first
			MFRegion.Empty();
			MFRegion.SetPosition(MyPageRegions.Region[i].Pos);
			MFRegion.SetOrientation(MyPageRegions.Region[i].OrientX);
//			MFRegion.SetFlipRender(EmulsionDown);
			CompileMarkRegion(pComp, (MarkRegion)i );
			MFRegion.Render(pRRegion,OuterTransform);
		}
	}	
	// pop the attribute context again
	pRRegion->RestoreContext();
}




/*

	{
		// there should be at least one page in this spread
		Page *pCurrPage = pSpread->FindFirstPageInSpread();
		if (!pCurrPage)
			return;

		// Create a full quality attr to render
		Quality Qual;
		Qual.SetQuality(QUALITY_MAX);
		QualityAttribute QualAttrVal(Qual);
		AttrQuality* pAttrQuality = (AttrQuality*)QualAttrVal.MakeNode();
		if (pAttrQuality != NULL)
		{
			// render it
			pAttrQuality->Render(pRRegion);
			delete pAttrQuality;
		}

		// We now need to examine the page structure in this spread
		// and also how it will be printed if we are printing. We may be
		// printing a double page spread either as two separate pages or
		// as a single spread. This makes a large difference to where the
		// marks format.

		DocRect CurrPageRect,PrevPageRect,NextPageRect;
		Page *pPrevPage, *pNextPage;
		pPrevPage=NULL;

		BOOL sl,sr,st,sb;
		sl=sr=sb=st=FALSE;

		while (pCurrPage)
		{
			// Get the page rect measured in millipoints
			PrevPageRect = CurrPageRect;
			CurrPageRect = pCurrPage->GetPageRect();
			pNextPage = pCurrPage->FindNextPage();

			if (pPrevPage)
			{
				sl=(PrevPageRect.hix==CurrPageRect.lox);
				sr=(PrevPageRect.lox==CurrPageRect.hix);
				sb=(PrevPageRect.hiy==CurrPageRect.loy);
				st=(PrevPageRect.loy==CurrPageRect.hiy);
			}

			if (pNextPage)
			{
				NextPageRect=pNextPage->GetPageRect();
				sl=sl || (NextPageRect.hix==CurrPageRect.lox);
				sr=sr || (NextPageRect.lox==CurrPageRect.hix);
				sb=sb || (NextPageRect.hiy==CurrPageRect.loy);
				st=st || (NextPageRect.loy==CurrPageRect.hiy);
			}

			
			// construct all 16 regions around this page rect, we will itterate
			// through these later


			if (sl)
				MyPageRegions.HideLeft();
			if (sr)
				MyPageRegions.HideRight();
			if (sb)
				MyPageRegions.HideBottom();
			if (st)
				MyPageRegions.HideTop();
			

			RenderPrintMarksAroundRect(CurrPageRect, bleed, ViewTrans);

			pPrevPage=pCurrPage;
			pCurrPage=pNextPage;
			pNextPage=NULL;
		}
	}
	else


*/



/*****************************************************************************************

>	void PrintMarksMan::CompileMarkRegion(PrintMarksComponent *pComp, MarkRegion CurrRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		pComp	   = the document component containing the print marks
				CurrRegion = the current region we're formatting marks in
	Returns:	-
	Purpose:	Add all the marks which are supposedly in this region into the region
				and format em before rendering. What happens is this. Each mark
				reference in the doc component is found in the marks manager. The record
				in the marks manager describes the regions the mark should be formatted
				into. We add the mark if it happens to be the right region on entry.
				Adding the mark to the formatting region adds a position record which
				is used to render the mark when the region is rendered.
	
*****************************************************************************************/

void PrintMarksMan::CompileMarkRegion(PrintMarksComponent *pComp, MarkRegion CurrRegion)
{
	UINT32 handle;
	// Dig the mark reference out of this doc component.
	DocPrintMark* pDocMark = pComp->GetFirstMark();
	while (pDocMark)
	{
		// retrieve the reference handle of this mark
		handle=pDocMark->GetHandle();

		// find the mark item in the print marks cache
		PrintMarkItem* pMarkItem = PMMCache.FindMark(handle);
		if (pMarkItem && pMarkItem->NeedsToRender())
		{
			// ok we have a pointer to the mark item. Scan through all its format
			// positions and add the mark if necessary
			PrintMark* pPrintMark = pMarkItem->GetPrintMark();
			if (pPrintMark)
			{
				MarkPosition* pMarkPosition = pPrintMark->GetFirstPosition();
				while (pMarkPosition!=NULL)
				{
					if (pMarkPosition->GetRegion()==CurrRegion)
					{
						MarkFormat LocalFormat = pMarkPosition->GetFormat();
						MFRegion.AddMark(pMarkItem, &LocalFormat);
					}

					pMarkPosition = pPrintMark->GetNextPosition(pMarkPosition);
				}
			}
		}

		// go onto format and render the next object
		pDocMark = pComp->GetNextMark(pDocMark);
	}
}





/*****************************************************************************************

>	PageMarkRegions::PageMarkRegions(const DocRect& page, MILLIPOINT bleed)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		page = the docrect containing the page coordinates.
				bleed = the size of the bleed area (in page coordinates, ie millipoints)
	Returns:	-
	Purpose:	Creates 16 mark formatting regions around a given page, with a
				specified bleed
	
*****************************************************************************************/

PageMarkRegion::PageMarkRegion()
{
	OrientX=TRUE;
	Valid=FALSE;
}


PageMarkRegions::PageMarkRegions()
{
}


void PageMarkRegions::Initialise(const DocRect& page, INT32 bleed)
{
	// ok, we've been given a page rectangle, lets build our regions
	INT32 blox,bloy,bhix,bhiy;
	
	// calculate the bleed bbox
	blox = page.lox-bleed;
	bloy = page.loy-bleed;
	bhix = page.hix+bleed;
	bhiy = page.hiy+bleed;

	INT32 hi = CROPAREA_SIZE;
	INT32 spc = CROPAREA_SIZE/4;
	INT32 slenx = (page.hix-page.lox-4*spc)/10;
	INT32 sleny = (page.hiy-page.loy-4*spc)/10;
	INT32 wlenx = 8*slenx;
	INT32 wleny = 8*sleny;
	INT32 cx=(page.lox+page.hix)>>1;
	INT32 cy=(page.loy+page.hiy)>>1;
	
	// top left corner
	Region[0].Pos = DocRect(blox-hi, bhiy, blox, bhiy+hi);

	// 3 along top edge	
	Region[1].Pos = DocRect(page.lox+spc, bhiy, page.lox+spc+slenx, bhiy+hi);
	Region[2].Pos = DocRect(cx-(wlenx>>1), bhiy, cx+(wlenx>>1), bhiy+hi);
	Region[3].Pos = DocRect(page.hix-spc-slenx, bhiy, page.hix-spc, bhiy+hi);

	// top right corner
	Region[4].Pos = DocRect(bhix, bhiy, bhix+hi, bhiy+hi);

	// 3 down right edge
	Region[5].Pos = DocRect(bhix, page.hiy-spc-sleny, bhix+hi, page.hiy-spc);
	Region[6].Pos = DocRect(bhix, cy-(wleny>>1), bhix+hi, cy+(wleny>>1));
	Region[7].Pos = DocRect(bhix, page.loy+spc, bhix+hi, page.loy+spc+sleny);

	// bottom right corner
	Region[8].Pos = DocRect(bhix, bloy-hi, bhix+hi, bloy);

	// 3 along bottom edge
	Region[9].Pos  = DocRect(page.hix-spc-slenx, bloy-hi, page.hix-spc, bloy);
	Region[10].Pos = DocRect(cx-(wlenx>>1), bloy-hi, cx+(wlenx>>1), bloy);
	Region[11].Pos = DocRect(page.lox+spc, bloy-hi, page.lox+spc+slenx, bloy);

	// bottom left corner
	Region[12].Pos = DocRect(blox-hi, bloy-hi, blox, bloy);

	// 3 up left edge
	Region[13].Pos = DocRect(blox-hi, page.loy+spc, blox, page.loy+spc+sleny);
	Region[14].Pos = DocRect(blox-hi, cy-(wleny>>1), blox, cy+(wleny>>1));
	Region[15].Pos = DocRect(blox-hi, page.hiy-spc-sleny, blox, page.hiy-spc);

	for (INT32 i=0; i<16; i++)
		Region[i].Valid=Region[i].OrientX=TRUE;

	Region[5].OrientX = FALSE;
	Region[6].OrientX = FALSE;
	Region[7].OrientX = FALSE;
	Region[13].OrientX = FALSE;
	Region[14].OrientX = FALSE;
	Region[15].OrientX = FALSE;
}



void PageMarkRegions::HideLeft()
{
	Region[0].Valid=FALSE;
	Region[12].Valid=FALSE;
	Region[13].Valid=FALSE;
	Region[14].Valid=FALSE;
	Region[15].Valid=FALSE;
}

void PageMarkRegions::HideRight()
{
	Region[4].Valid=FALSE;
	Region[5].Valid=FALSE;
	Region[6].Valid=FALSE;
	Region[7].Valid=FALSE;
	Region[8].Valid=FALSE;
}

void PageMarkRegions::HideTop()
{
	Region[0].Valid=FALSE;
	Region[1].Valid=FALSE;
	Region[2].Valid=FALSE;
	Region[3].Valid=FALSE;
	Region[4].Valid=FALSE;
}

void PageMarkRegions::HideBottom()
{
	Region[9].Valid=FALSE;
	Region[10].Valid=FALSE;
	Region[11].Valid=FALSE;
	Region[12].Valid=FALSE;
	Region[13].Valid=FALSE;
}

void PageMarkRegions::ShowLeft()
{
	Region[0].Valid=TRUE;
	Region[12].Valid=TRUE;
	Region[13].Valid=TRUE;
	Region[14].Valid=TRUE;
	Region[15].Valid=TRUE;
}

void PageMarkRegions::ShowRight()
{
	Region[4].Valid=TRUE;
	Region[5].Valid=TRUE;
	Region[6].Valid=TRUE;
	Region[7].Valid=TRUE;
	Region[8].Valid=TRUE;
}

void PageMarkRegions::ShowTop()
{
	Region[0].Valid=TRUE;
	Region[1].Valid=TRUE;
	Region[2].Valid=TRUE;
	Region[3].Valid=TRUE;
	Region[4].Valid=TRUE;
}

void PageMarkRegions::ShowBottom()
{
	Region[9].Valid=TRUE;
	Region[10].Valid=TRUE;
	Region[11].Valid=TRUE;
	Region[12].Valid=TRUE;
	Region[13].Valid=TRUE;
}







/*******************************************************************************
>	MarkPosType::MarkPosType()
*******************************************************************************/

MarkPosType::MarkPosType()
{
	pMarkItem=NULL;
	rotate=FALSE;
}


/*******************************************************************************
>	MarkList::MarkList()
*******************************************************************************/

MarkList::MarkList()
{
	count=size.x=size.y=0;
}



/*******************************************************************************
>	MarkFormatRegion::MarkFormatRegion()
*******************************************************************************/

MarkFormatRegion::MarkFormatRegion()
{
	Init();
}

MarkFormatRegion::MarkFormatRegion(const DocRect &Position)
{
	Init();
	SetPosition(Position);
}

MarkFormatRegion::~MarkFormatRegion()
{
	Empty();
}

void MarkFormatRegion::Init()
{
	OrientX=TRUE;
	MkCx = MkCy = 0;
	BLeft = BCentre = BRight = NULL;
	MLeft = MCentre = MRight = NULL;
	TLeft = TCentre = TRight = NULL;
	OnRenderFlipX=FALSE;
}

void MarkFormatRegion::Empty()
{
	if (BLeft)	 	delete BLeft;
	if (BCentre) 	delete BCentre;
	if (BRight)	 	delete BRight;

	if (MLeft)	 	delete MLeft;
	if (MCentre) 	delete MCentre;
	if (MRight)	 	delete MRight;

	if (TLeft)	 	delete TLeft;
	if (TCentre)	delete TCentre;
	if (TRight)	 	delete TRight;

	BLeft = BCentre = BRight = NULL;
	MLeft = MCentre = MRight = NULL;
	TLeft = TCentre = TRight = NULL;
}


void MarkFormatRegion::SetPosition(const DocRect &Position)
{
	TheBounds = Position;
	MkCx = (TheBounds.lox + TheBounds.hix)>>1;
	MkCy = (TheBounds.loy + TheBounds.hiy)>>1;
}


void MarkFormatRegion::SetOrientation(BOOL orientX)
{
	OrientX=orientX;
}

void MarkFormatRegion::SetFlipRender(BOOL flip)
{
	OnRenderFlipX=flip;
}

/***********************************************************************************************

>	virtual BOOL MarkFormatRegion::AddMark(PrintMarkItem *pMark, MarkFormat *pForm)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		pMark = a print mark item pointer
				pForm = a mark format object pointer
	Purpose:	

***********************************************************************************************/

BOOL MarkFormatRegion::AddMark(PrintMarkItem *pMark, MarkFormat *pForm)
{
	if (!pMark || !pForm)
		return FALSE;
	
	BOOL ok=TRUE;

	INT32 w = pMark->GetWidth();
	INT32 h = pMark->GetHeight();
	BOOL rt;

	rt = (( OrientX && pMark->GetOrient()==MO_Vertical) ||
		  (!OrientX && pMark->GetOrient()==MO_Horizontal) );

	if (rt)
	{
		INT32 Tmp=w;	w=h; h=Tmp;
	}

	if (pForm->Left)
	{
		if (pForm->Top)
		{
			DocCoord anchor = DocCoord(TheBounds.lox,TheBounds.hiy); 
			ok = MakeMark(pMark, anchor, FALSE, FALSE, TRUE, FALSE, 0, h, rt, &TLeft);
		}
		
		if (pForm->Middle)
		{
			DocCoord anchor = DocCoord(TheBounds.lox,MkCy);
			ok = MakeMark(pMark, anchor, FALSE, TRUE, TRUE, TRUE, 0, h>>1, rt, &MLeft);		
		}
		
		if (pForm->Bottom)
		{
			DocCoord anchor = DocCoord(TheBounds.lox,TheBounds.loy);
			ok = MakeMark(pMark, anchor, FALSE, FALSE, TRUE, TRUE, 0, 0, rt, &BLeft);
		}
	}

	if (pForm->Centre)
	{
		if (pForm->Top)
		{
			DocCoord anchor = DocCoord(MkCx,TheBounds.hiy); 
			ok = MakeMark(pMark, anchor, TRUE, FALSE, TRUE, FALSE, w>>1, h, rt, &TCentre);
		}
		
		if (pForm->Middle)
		{
			DocCoord anchor = DocCoord(MkCx,MkCy);
			ok = MakeMark(pMark, anchor, TRUE, TRUE, TRUE, TRUE, w>>1, h>>1, rt, &MCentre);		
		}
		
		if (pForm->Bottom)
		{
			DocCoord anchor = DocCoord(MkCx,TheBounds.loy);
			ok = MakeMark(pMark, anchor, TRUE, FALSE, TRUE, TRUE, w>>1, 0, rt, &BCentre);
		}
	}

	if (pForm->Right)
	{
		if (pForm->Top)
		{
			DocCoord anchor = DocCoord(TheBounds.hix,TheBounds.hiy); 
			ok = MakeMark(pMark, anchor, FALSE, FALSE, FALSE, FALSE, w, h, rt, &TRight);
		}
		
		if (pForm->Middle)
		{
			DocCoord anchor = DocCoord(TheBounds.hix,MkCy);
			ok = MakeMark(pMark, anchor, FALSE, TRUE, FALSE, TRUE, w, h>>1, rt, &MRight);		
		}
		
		if (pForm->Bottom)
		{
			DocCoord anchor = DocCoord(TheBounds.hix,TheBounds.loy);
			ok = MakeMark(pMark, anchor, FALSE, FALSE, FALSE, TRUE, w, 0, rt, &BRight);
		}
	}

	return ok;
}


/*******************************************************************************

>	virtual BOOL MarkFormatRegion::MakeMark(PseudoMark *pMark, 
											DocCoord Anchor,
											BOOL cx,
											BOOL cy,
											BOOL posx,
											BOOL posy,
											INT32 shiftx,
											INT32 shifty,
											BOOL rotate,
											MarkList **pMarkList)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		pMark	- a pointer to a mark to add to the formatting region
				Anchor	- An origin point to begin formatting to
				cx		- TRUE then centre all added marks in X
				cy		- TRUE then centre all added marks in Y
				posx	- TRUE then increment anchor x pos by mark width else decrement
				posy	- TRUE then increment anchor y pos by mark height else decrement
				shiftx  - An amount to shift the positioned mark by in x.
				shifty  - An amount to shift the positioned mark by in y.
	Outputs:	pMarkList, a pointer to a new mark list if on entry this pointer
						   points to a NULL value. Otherwise the list pointed to
						   on entry will contain another item and its count will
						   have increased by 1.
	Returns:	TRUE	- if a mark formating object was added to pMarkList.
				FALSE	- then you've attempted to add more than MAXREGIONMARKS
						  to pMarkList.
	Purpose:	Invented to add a mark to a marklist. This function really is
				a helper to AddMark and should not be used by itself unless you
				really want to. Imagine an arbitrarily shaped box with 9 format
				positions ie combinations of permutations of L,C,R,T,M,B. A
				Mark list will keep track of marks added to any of these regions.
				For instance, if you add a mark to TL ie top left, and then add
				another mark to this region, the TLeft array in the constructed
				marks list will contain two entries, holding the x,y positions
				of where the marks should be positioned. Marks will be added ie
				grow from the edges of the format region dependent on its orientation
				ie
				Vertical region orientation
					bottom edge - marks grow bottom to top	  left edge aligned at left
					middle edge - marks grow centrally		  right edge aligned at right
					top edge    - marks grow top to bottom	  central align in centre
				Horizontal region orientation
					left edge   - marks grow left to right
					centre edge - marks grow centrally
					right edge  - marks grow right to left

				Anyway, the whole job is one of positioning child boxes in a parent
				box region, where more than one child can occupy the same space.


*******************************************************************************/

BOOL MarkFormatRegion::MakeMark(PrintMarkItem *pMark, 
								DocCoord Anchor,
								BOOL cx,
								BOOL cy,
								BOOL posx,
								BOOL posy,
								INT32 shiftx,
								INT32 shifty,
								BOOL rotate,
								MarkList **pMarkList)
{
	MarkList *pItem = (*pMarkList);

	if (pItem==NULL)
	{
		pItem = new MarkList;
		if (pItem==NULL)
			return FALSE;
		
		pItem->curpos = Anchor;
	}

	INT32 width = pMark->GetWidth();
	INT32 height = pMark->GetHeight();
	if (rotate)
	{
		INT32 Tmp=width; width=height; height=Tmp;
	}

	if (pItem->count<MAXREGIONMARKS)
	{
		pItem->MarkPos[pItem->count].pMarkItem = pMark;
		pItem->MarkPos[pItem->count].dim = DocCoord(width,height);
		pItem->MarkPos[pItem->count].rotate = rotate;
		pItem->count++;
		
		if (OrientX)
		{
			pItem->MarkPos[pItem->count-1].pos.y = pItem->curpos.y - shifty;
			if (cx)
			{
				pItem->size.x += width;
				INT32 x=0;
				for (INT32 i=0; i<pItem->count; i++)
				{
					pItem->MarkPos[i].pos.x = MkCx - ((pItem->size.x)>>1) + x;
					x+=pItem->MarkPos[i].dim.x;
				}
			}
			else
			{
				pItem->MarkPos[pItem->count-1].pos.x = pItem->curpos.x - shiftx;
				pItem->curpos.x += ((posx) ? (+width) : (-width));
			}	
		}
		else
		{
			pItem->MarkPos[pItem->count-1].pos.x = pItem->curpos.x - shiftx;
			if (cy)
			{
				pItem->size.y += height;
				INT32 y=0;
				for (INT32 i=0; i<pItem->count; i++)
				{
					pItem->MarkPos[i].pos.y = MkCy - ((pItem->size.y)>>1) + y;
					y+=pItem->MarkPos[i].dim.y;
				}
			}
			else
			{
				pItem->MarkPos[pItem->count-1].pos.y = pItem->curpos.y - shifty;
				pItem->curpos.y += ((posy) ? (+height) : (-height));
			}	
		}
	}

	if (*pMarkList==NULL)
		*pMarkList=pItem;

	return TRUE;
}


/*******************************************************************************
void MarkFormatRegion::Render(RenderRegion *pRRegion, const Matrix& Transform)
*******************************************************************************/

void MarkFormatRegion::Render(RenderRegion *pRRegion, const Matrix& Transform)
{
	if (BLeft)	 	Render(pRRegion, BLeft, Transform);
	if (BCentre) 	Render(pRRegion, BCentre, Transform);
	if (BRight)	 	Render(pRRegion, BRight, Transform);
	if (MLeft)	 	Render(pRRegion, MLeft, Transform);
	if (MCentre) 	Render(pRRegion, MCentre, Transform);
	if (MRight)	 	Render(pRRegion, MRight, Transform);
	if (TLeft)	 	Render(pRRegion, TLeft, Transform);
	if (TCentre) 	Render(pRRegion, TCentre, Transform);
	if (TRight)	 	Render(pRRegion, TRight, Transform);
}


/*******************************************************************************
void MarkFormatRegion::Render(RenderRegion *pRRegion, MarkList *pMarkLst)
*******************************************************************************/

void MarkFormatRegion::Render(RenderRegion *pRRegion, MarkList *pMarkLst, const Matrix& OuterTransform)
{
	PrintMarkItem* pPMI;
	DocRect bounds;
	DocCoord pos,pos1;
	Trans2DMatrix Trans2DMat;

	// Calculate the inverse of the render matrix, this allows us to render
	// through a scaling render region and prevent our marks being scaled.
	Matrix ViewTrans = pRRegion->GetMatrix();
	ViewTrans = ViewTrans.Inverse();

	Matrix Transform;
	Matrix ITransform;
	Matrix Identity;

	for (INT32 i=0; i<pMarkLst->count; i++)
	{
		pPMI = pMarkLst->MarkPos[i].pMarkItem;
		pos = pPMI->GetOrigin();
		// Initialise to the identity
		Transform = Identity;
		// translation mark to origin
		Transform.SetTranslation(-pos.x, -pos.y);

		// spin the mark around if necessary

		INT32 w = pPMI->GetWidth();
		INT32 h = pPMI->GetHeight();
		if (pMarkLst->MarkPos[i].rotate)
		{
			Transform *= Matrix((ANGLE)270);
			Transform *= Matrix(0,w);
		}

		// Take into account any flipping
		if (OnRenderFlipX)
		{
			if (pMarkLst->MarkPos[i].rotate)
				Transform *= Matrix(FIXED16(-1), FIXED16(0), FIXED16(0), FIXED16(1), h, 0);
			else
				Transform *= Matrix(FIXED16(-1), FIXED16(0), FIXED16(0), FIXED16(1), w, 0);
		}
		
		// Now translate into the final position
		Transform = Transform * Matrix(pMarkLst->MarkPos[i].pos);
		// Finally apply the outer transform before rendering
		Transform = Transform * OuterTransform;
		// ok now we need to compensate for the render region matrix
		Transform = Transform * ViewTrans;
		// calculate the inverse for transforming back again.
		ITransform = Transform.Inverse();

		// Apply the transform to the mark, we need to make sure line widths get scaled
		Trans2DMat.SetTransform(Transform);
		Trans2DMat.TransLines = TRUE;
		pPMI->GetMarkGlyph()->Transform(Trans2DMat);

		// once transformed lets render it.
		RenderMark(pRRegion, pPMI);

		// now transform back again (without loss of accuracy ha!)
		Trans2DMat.SetTransform(ITransform);
		Trans2DMat.TransLines = TRUE;
		pPMI->GetMarkGlyph()->Transform(Trans2DMat);
	}
}




/***********************************************************************************************

>	void MarkFormatRegion::RenderMark(RenderRegion* pRRegion, PrintMarkItem *pMark)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		pRRegion = pointer to a render region
				pMark    = pointer to a print mark item
	Purpose:	Render the mark if it intersects the clip rect in the render region.

***********************************************************************************************/

void MarkFormatRegion::RenderMark(RenderRegion* pRRegion, PrintMarkItem *pMark)
{
	// Get the root node and render the entire subtree
	Node* pFirstNode = pMark->GetMarkGlyph();
	if (pFirstNode!=NULL)
		pRRegion->RenderTree(pFirstNode, FALSE, TRUE);		
}
