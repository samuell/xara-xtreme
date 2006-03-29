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

// An EPS import/export filter system for Camelot.


#include "camtypes.h"
#include "epsfiltr.h"

#include <ctype.h>
#include <strstrea.h>

#include <afxdlgs.h>


#include "attrmgr.h"
#include "bitfilt.h"
#include "ccdc.h"
#include "colcomp.h"
#include "csrstack.h"
#include "doccomp.h"
#include "document.h"
#include "docview.h"
#include "errors.h"
#include "ensure.h"
#include "prvwflt.h"
#ifndef WEBSTER
#include "extfilts.h"
#endif //WEBSTER
#include "fixmem.h"
#include "group.h"
#include "layer.h"
#include "lineattr.h"
#include "nativeps.h"	// old native filter based on eps, used for version 1.1  
#include "nodeelip.h"
#include "nodepath.h"
#include "noderect.h"
#include "oilfltrs.h"
#include "page.h"
#include "paper.h"
#include "pathname.h"
#include "paths.h"
#include "progress.h"        
//#include "resource.h"
//#include "rik.h"
#include "saveeps.h"
#include "sglayer.h"
#include "sprdmsg.h"
#include "spread.h"
//#include "tim.h"
#include "trans2d.h"
#include "gclips.h"
#include "nodetxts.h"
#include "nodetxtl.h"
#include "nodershp.h"
#include "app.h"
#include "insertnd.h"
#include "fontman.h"
#include "guides.h"
//#include "nev.h"		// _R(IDN_USER_CANCELLED)
//#include "will2.h"
#include "moldshap.h"
#include "ai_bmp.h"
#include "epsclist.h"
#include "epscdef.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC(EPSFilter, VectorFilter);
CC_IMPLEMENT_MEMDUMP(RangeList, List);
CC_IMPLEMENT_MEMDUMP(EPSClipContext, List);
CC_IMPLEMENT_MEMDUMP(EPSClipContextItem, ListItem);

#define new CAM_DEBUG_NEW

// Job 10463: remove PS Level bits - default to Level 2
INT32 EPSFilter::XSEPSExportPSType = 2;
INT32 EPSFilter::XSEPSExportDPI	 = 200;
BOOL EPSFilter::XSEPSExportTextAsCurves	= FALSE;

/********************************************************************************************

>	class RangeListItem : public ListItem

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/94
	Purpose:	Hold a range that
	SeeAlso:	

********************************************************************************************/

class RangeListItem : public ListItem
{
	CC_DECLARE_MEMDUMP(RangeListItem)

public:
	RangeListItem(Node *pFirst, Node *pLast);
	BOOL IsLayer;
	Range TheRange;
	Layer *TheLayer;
};

CC_IMPLEMENT_MEMDUMP(RangeListItem, ListItem)

/********************************************************************************************

>	RangeListItem::RangeListItem(Node *pFirst, Node *pLast)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/09/94
	Inputs:		pFirst, pLast - the first and last nodes in the range.
	Purpose:	Create a range list item using the specified node range.
	SeeAlso:	RangeListItem; RangeList

********************************************************************************************/

RangeListItem::RangeListItem(Node *pFirst, Node *pLast)
{
	// range or layer?
	if(pFirst == pLast && IS_A(pFirst, Layer))
	{
		// just a layer, so make a layer variant
		TheLayer = (Layer *)pFirst;
		IsLayer = TRUE;
	}
	else
	{
		// it's more than one, or not a layer
		
		// We want all nodes to be included in the range
		RangeControl Flags;
		Flags.Selected = TRUE;
		Flags.Unselected = TRUE;

		// Make the range and copy to where we want it (spot slight omission from Range class!)
		Range Tmp(pFirst, pLast, Flags);
		TheRange = Tmp;
		IsLayer = FALSE;
	}
}

/********************************************************************************************

>	BOOL RangeList::AddRange(Node *pFirst, Node *pLast)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/94
	Inputs:		pFirst, pLast - the first and last nodes in the range.
	Returns:	TRUE if the range was added ok;
				FALSE if not.
	Purpose:	Adds another range to the range list.  All nodes from pFirst to pLast
				are inclusively added to the range.
	SeeAlso:	Range

********************************************************************************************/

BOOL RangeList::AddRange(Node *pFirst, Node *pLast)
{
	// Get a new range
	RangeListItem *pItem = new RangeListItem(pFirst, pLast);
	if (pItem == NULL)
		// Out of memory
		return FALSE;

	// Add it to the list and return success
	AddTail(pItem);
	return TRUE;
}

/********************************************************************************************

>	DocRect RangeList::GetBoundingRect()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/09/94
	Returns:	The union of the bounding boxes of all nodes in this range list.
	Purpose:	Goes through all nodes in all the ranges in this RangeList, and unions
				all the bounding boxes together.  It uses the simple algorithm, which does
				not take account of the attributes applied to the nodes - i.e. it only
				gives an approximation (hence this function can result in bounding boxes
				being in the tree which are not entirely accurate).
	SeeAlso:	RangeList

********************************************************************************************/

DocRect RangeList::GetBoundingRect()
{
	// Start off with empty rectangle
	DocRect RangeBBox;

	// Do each range in turn
	RangeListItem *pItem = (RangeListItem *) GetHead();

	while (pItem != NULL)
	{
		// Do each node in turn
		if(pItem->IsLayer)
		{
			// layer based calcuations

			// see long note below
			RangeBBox = RangeBBox.Union(pItem->TheLayer->GetBoundingRect(TRUE));
		}
		else
		{
			// range based calculations
			Node *pNode = pItem->TheRange.FindFirst();

			while (pNode != NULL)
			{
				// Does this node have a bounding rectangle?
				if (pNode->IsBounded())
				{
					// Yes - add it to our union
					NodeRenderableBounded *pNodeBounded = (NodeRenderableBounded *) pNode;

					// Pass in TRUE so that we don't do a 'proper' attribute scan for the
					// bounding box - we only need an approximate bounding rectangle
					// for centering it anyway.
					// NB. this results in inaccurate bounding boxes in the tree, but when
					//     we call this we're about to transform them anyway, so they'll
					//     be updated correctly afterwards.
					RangeBBox = RangeBBox.Union(pNodeBounded->GetBoundingRect(TRUE));
				}
				else
				{
					ENSURE(FALSE, "Found a non-renderable node in the import range");
				}

				// Try the next node.
				pNode = pItem->TheRange.FindNext(pNode);
			}
		}

		// Try the next range
		pItem = (RangeListItem *) GetNext(pItem);
	}

	// Return the accumulated bounds
	return RangeBBox;
}

/********************************************************************************************

>	void RangeList::Transform(TransformBase& Trans)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/09/94
	Inputs:		Trans - the transform object to transform all the nodes by.
	Purpose:	Transforms all the nodes in all the ranges of this range list using the
				transform object given.
	SeeAlso:	Range::Transform; TransformBase

********************************************************************************************/

void RangeList::Transform(TransformBase& Trans)
{
	// Do each range in turn
	RangeListItem *pItem = (RangeListItem *) GetHead();

	while (pItem != NULL)
	{
		if(pItem->IsLayer)
		{
			pItem->TheLayer->Transform(Trans);
		}
		else
		{
			// Transform all nodes in this range
			Node *pNode = pItem->TheRange.FindFirst();

			while (pNode != NULL)
			{
				// Is this node renderable?
				if (pNode->IS_KIND_OF(NodeRenderable))
				{
					// Yes - transform it.
					NodeRenderable *pNodeRenderable = (NodeRenderable *) pNode;
					pNodeRenderable->Transform(Trans);
				}
				else
				{
					ENSURE(FALSE, "Found a non-renderable node in the import range");
				}

				// Try the next node.
				pNode = pItem->TheRange.FindNext(pNode);
			}
		}

		// Try the next range
		pItem = (RangeListItem *) GetNext(pItem);
	}
}

/********************************************************************************************

>	EPSClipContextItem::EPSClipContextItem(INT32 TheContextLevel, InkPath *pNewClipPath)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/22/95
	Inputs:		TheContextLevel - the context level for this clip region.
	Purpose:	Initialise the clip region item to sensible values.
	SeeAlso:	EPSClipContext

********************************************************************************************/

EPSClipContextItem::EPSClipContextItem(INT32 TheContextLevel, Path *pNewClipPath)
{
	ContextLevel = TheContextLevel;
	pClipPath = pNewClipPath;

	// Defaults to not a complex region.
	ComplexRegionLevel = 0;
}

/********************************************************************************************

>	EPSClipContextItem::~EPSClipContextItem()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/22/95
	Purpose:	Clean up the context item - this includes deleting the clipping path held
				in the region item.
	SeeAlso:	EPSClipContext

********************************************************************************************/

EPSClipContextItem::~EPSClipContextItem()
{
	delete pClipPath;
}

/********************************************************************************************

>	EPSClipContext::EPSClipContext()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/22/95
	Purpose:	Initialise the context to a sensible state, i.e. a zero context level.

********************************************************************************************/

EPSClipContext::EPSClipContext()
{
	ContextLevel = 0;
	ComplexRegionLevel = 0;

	// Initially there is no clip region.
	pCachedClipPath = NULL;
	CacheIsValid = TRUE;

	// set default clipping flags
	ClippingFlags = 2 | CLIPPING_CLIP_WINDING;
}

/********************************************************************************************

>	void EPSClipContext::SaveContext()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/22/95
	Purpose:	Save the current clipping state for future use - no matter how many
				clip regions are added to the current context, the exact state of the
				current clipping context can be restore with a (balanced) call to
				EPSClipContext::RestoreContext().
	SeeAlso:	EPSClipContext::RestoreContext

********************************************************************************************/

void EPSClipContext::SaveContext()
{
	ContextLevel++;
}

/********************************************************************************************

>	BOOL EPSClipContext::RestoreContext()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/22/95
	Purpose:	Restores the clipping context to the state it was in the last time
				SaveContext() was called on this clip context.
	Errors:		No call to SaveContext() has been made => ERROR2
	SeeAlso:	EPSClipContext::SaveContext

********************************************************************************************/

BOOL EPSClipContext::RestoreContext()
{
	// Invalidate cache
	CacheIsValid = FALSE;

	// Sanity check
	ERROR2IF(ContextLevel == 0, FALSE, "Unbalanced call to EPSClipContext::RestoreContext()");

	// Decrement context level
	ContextLevel--;

	// Delete all items added since save context was last called...
	EPSClipContextItem *pItem = (EPSClipContextItem *) GetTail();

	while ((pItem != NULL) && (pItem->ContextLevel > ContextLevel))
	{
		delete RemoveTail();
		
		// Get the next item
		pItem = (EPSClipContextItem *) GetTail();
	}

	// If we get here then it's ok
	return TRUE;
}


/********************************************************************************************

>	BOOL EPSClipContext::StartComplexClipRegion()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/22/95
	Returns:	TRUE if ok;
				FALSE if out of memory.
	Purpose:	Indicates that we are about to start constructing a complex clipping region.
				All future calls to AddNewClippingPath() will be joined into the current
				clipping region until EndComplexRegion is called.
				Note that you cannot interleave these calls with Save/Restore context calls;
				complex regions cannot be unbalanced across context levels.
	Errors:		Out of memory => ERROR1
	SeeAlso:	EPSClipContext::EndComplexClipRegion()

********************************************************************************************/

BOOL EPSClipContext::StartComplexClipRegion()
{
	// Update complex region count
	ComplexRegionLevel++;

	// We need to create a record to hold this complex region
	// (it has a NULL clipping region initially)
	EPSClipContextItem *pItem = new EPSClipContextItem(ContextLevel, NULL);
	if (pItem == NULL)
		// Out of memory
		return FALSE;

	// It's a complex region so save the level
	pItem->ComplexRegionLevel = ComplexRegionLevel;

	// Add to end of list
	AddTail(pItem);

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSClipContext::EndComplexClipRegion()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/22/95
	Returns:	TRUE if ok;
				FALSE if no corresponding call to StartComplexClipRegion() has been 
				encountered => ERROR2
	Purpose:	See StartComplexClipRegion().
	Errors:		If unbalanced call is made => ERROR2
	SeeAlso:	EPSClipContext::StartComplexClipRegion

********************************************************************************************/

BOOL EPSClipContext::EndComplexClipRegion()
{
	ERROR2IF(ComplexRegionLevel == 0, FALSE, 
			 "Unbalanced call to EPSClipContext::EndComplexClipRegion()");

	// Adjust complex region level.
	ComplexRegionLevel--;

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL EPSClipContext::AddNewClippingPath(Path *pNewClipPath)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/22/95
	Inputs:		The path to augment the current clipping region with.
	Returns:	TRUE if clipping region updated ok;
				FALSE if not, i.e. out of memory => ERROR1
	Purpose:	Takes a copy of the path passed in and merges it with the current clipping
				context.
	Errors:		Out of memory => ERROR1

********************************************************************************************/

BOOL EPSClipContext::AddNewClippingPath(Path *pNewClipPath)
{
	// Invalidate cache
	CacheIsValid = FALSE;

	// Flag to indicate if we just do a straightforward copy
	BOOL SimpleCopy = FALSE;

	EPSClipContextItem *pItem = NULL;

	if (ComplexRegionLevel > 0)
	{
		// We are building a complex region, so join the new path with the existing one.

		// First, make sure we have a clip path already?
		pItem = (EPSClipContextItem *) GetTail();
		if (pItem->pClipPath == NULL)
		{
			// No clip path - just copy the new path and use that.
			SimpleCopy = TRUE;
		}
		else
		{
			// Already have a clip path - join with new one to form a complex path.
			if (!pItem->pClipPath->MergeTwoPaths(*pNewClipPath))
				// Error occured (i.e. out of memory)
				return FALSE;
		}
	}
	else
	{
		// Just a simple clipping region
		pItem = new EPSClipContextItem(ContextLevel, NULL);

		if (pItem == NULL)
			// Out of memory
			return FALSE;

		// Add to end of list
		AddTail(pItem);

		// Just copy the path passed in
		SimpleCopy = TRUE;
	}

	// Should we just copy the path into the list item?
	if (SimpleCopy)
	{
		// Sanity check
		ERROR2IF(pItem == NULL, FALSE, "No clipping item found!");

		// (yes, this code is pretty tedious!)
		pItem->pClipPath = new Path;

		if (pItem->pClipPath == NULL)
			// Out of memory
			return FALSE;

		// Graeme (26/7/00) - Added a test for a NULL pointer to prevent access violations.
		if ( pNewClipPath != NULL &&
			 !pItem->pClipPath->Initialise ( pNewClipPath->GetNumCoords () + 3, 12 ) )
		{
			// Failed to initialise - out of memory
			return FALSE;
		}

		if (!pItem->pClipPath->CopyPathDataFrom(pNewClipPath))
			// Failed to copy - out of memory - but this shouldn't happen as
			// we made sure we had enough space with the initialise call!
			ERROR2(FALSE, "Unexpected out-of-space error when copying clipping path");
	}

	// If we've got here, it must have worked...
	return TRUE;
}


/********************************************************************************************

>	BOOL EPSClipContext::ClipPathToRegion(Path *pPathToClip)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/22/95
	Inputs:		pPathToClip - the path object that we want to clip
	Outputs:	pPathToClip - the clipped version of the path
	Returns:	TRUE if clipped ok;
				FALSE if not.
	Purpose:	Given a path, clip it to the current clipping region specified by this
				object.
	SeeAlso:	EPSClipContext::GetClipRegion

********************************************************************************************/

BOOL EPSClipContext::ClipPathToRegion(Path *pPathToClip)
{
	// Cope with NULL clip region first
	if (GetClipRegion() == NULL)
		// Path is already 'clipped'
		return TRUE;

	// Ensure GetClipRegion validated the cache
	ERROR2IF(!CacheIsValid, FALSE, 
			 "Could not get valid clipping path in EPSClipContext::ClipPathToRegion()");

	// We now have a valid region, so use it to clip the given path
	// NB. we do an intersection using the cliping path's winding, with a tolerance of 0,
	//	   and flatness of 128 millipoints (arbitrary).
	if (pCachedClipPath->ClipPathToPath(*pPathToClip, pPathToClip, ClippingFlags, 
										128, 256, 256) == -1)
	{
		// An error occured...
		return FALSE;
	}

	// Initialise flags so the bezier tool is happy 
	// [see comment for Path::ClipPathToPath()].
	pPathToClip->InitialiseFlags();

	// If we get this far, it worked!
	return TRUE;
}

/********************************************************************************************

>	Path *EPSClipContext::GetClipRegion()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/22/95
	Returns:	Pointer to the clipping path, or
				NULL if no clipping region is specified by this object.
	Purpose:	Get the path which defines the current clipping region ofheld by this
				object.
	SeeAlso:	EPSClipContext::ClipPathToRegion

********************************************************************************************/

Path *EPSClipContext::GetClipRegion()
{
	if (!CacheIsValid)
	{
		// Cache is invalid - if there is no path in the cache, get a new one, otherwise
		// empty the one in the cache and use that.
		if (pCachedClipPath == NULL)
		{
			pCachedClipPath = new Path;

			if (pCachedClipPath == NULL)
				// Out of memory
				return NULL;

			if (!pCachedClipPath->Initialise(12, 12))
				// Failed to initialise - out of memory
				return NULL;
		}
		else
		{
			// Just clear this path out so we can use it to build a clipping region.
			pCachedClipPath->ClearPath();
		}

		// Calculate the current clipping path - we do this by going along the region
		// list and combining the clip regions.
		EPSClipContextItem *pItem = (EPSClipContextItem *) GetHead();

		while (pItem != NULL)
		{
			if (pItem->pClipPath != NULL)
			{
				// Do we already have a clipping region?
				if (pCachedClipPath->GetNumCoords() > 0)
				{
					// Combine this clipping region with the region being constructed...
					if (pItem->pClipPath->ClipPathToPath(*pCachedClipPath, pCachedClipPath, 
														 2 | CLIPPING_CLIP_WINDING, 
														 128, 256, 256) == -1)
					{
						// An error occured...
						return FALSE;
					}
				}
				else
				{
					// No clipping region yet - just copy the data from this path
					if (!pCachedClipPath->MakeSpaceInPath(pItem->pClipPath->GetNumCoords() + 3))
						// Failed to initialise - out of memory
						return FALSE;

					if (!pCachedClipPath->CopyPathDataFrom(pItem->pClipPath))
						// Failed to copy - out of memory - but this shouldn't happen as
						// we made sure we had enough space with the initialise call!
						ERROR2(FALSE, "Unexpected out-of-space error when copying clipping path");
				}
			}

			// Get the next item
			pItem = (EPSClipContextItem *) GetNext(pItem);
		}

		// Did we get any clipping at all?
		if (pCachedClipPath->GetNumCoords() == 0)
		{
			// No clip region - delete and use NULL to indicate this.
			delete pCachedClipPath;
			pCachedClipPath = NULL;
		}

		// Validate cache
		CacheIsValid = TRUE;
	}

	// Cache must be valid, so return the contents of the cache.
	return pCachedClipPath;
}


// This is the array of ArtWorks EPS command/keyword names.
CommandMap EPSFilter::Commands[] =
{
	// Path Construction operators
	EPSC_l,			"l",
	EPSC_c,			"c",
	EPSC_m,			"m",
	EPSC_L,			"L",
	EPSC_C,			"C",
	EPSC_v,			"v",
	EPSC_V,			"V",
	EPSC_y,			"y",
	EPSC_Y,			"Y",

	// Colour operators
	EPSC_g,			"g",
	EPSC_G,			"G",
	EPSC_k,			"k",
	EPSC_K,			"K",
	EPSC_x,			"x",
	EPSC_X,			"X",
	EPSC_p,			"p",
	EPSC_P,			"P",
	
	// Path Painting operators
	EPSC_N,			"N",
	EPSC_n,			"n",
	EPSC_F,			"F",
	EPSC_f,			"f",
	EPSC_S,			"S",
	EPSC_s,			"s",
	EPSC_B,			"B",
	EPSC_b,			"b",
	EPSC__u,		"*u",
	EPSC__U,		"*U",

	// Group operators
	EPSC_u,			"u",
	EPSC_U,			"U",
	
	// Graphics state operators
	EPSC_A,			"A",
	EPSC_d,			"d",
	EPSC_i,			"i",
	EPSC_D,			"D",
	EPSC_j,			"j",
	EPSC_J,			"J",
	EPSC_M,			"M",
	EPSC_w,			"w",

	// Special operators
	EPSC_ArrayStart,"[",
	EPSC_ArrayEnd,	"]",
	EPSC_Slash,		"/",

	// Overprint operators
	EPSC_O,			"O",
	EPSC_R,			"R",

	// Clipping operators
	EPSC_q,			"q",
	EPSC_Q,			"Q",
	EPSC_H,			"H",
	EPSC_h,			"h",
	EPSC_W,			"W",
	
	// Text operators
	EPSC_To,		"To",
	EPSC_TO,		"TO",
	EPSC_Tp,		"Tp",
	EPSC_TP,		"TP",

	// Matrix operators
	EPSC_Tm,		"Tm",
	EPSC_Td,		"Td",
	EPSC_T_,		"T*",
	EPSC_TR,		"TR",

	// Text Attribute operators
	EPSC_Tr,		"Tr",
	EPSC_Tf,		"Tf",
	EPSC_Ta,		"Ta",
	EPSC_Tl,		"Tl",
	EPSC_Tt,		"Tt",
	EPSC_TW,		"TW",
	EPSC_Tw,		"Tw",
	EPSC_TC,		"TC",
	EPSC_Tc,		"Tc",
	EPSC_Ts,		"Ts",
	EPSC_Ti,		"Ti",
	EPSC_Tz,		"Tz",
	EPSC_TA,		"TA",
	EPSC_Tq,		"Tq",

	// Text Body operators
	EPSC_Tx,		"Tx",
	EPSC_Tj,		"Tj",
	EPSC_TX,		"TX",
	EPSC_Tk,		"Tk",
	EPSC_TK,		"TK",
	EPSC_Tplus,		"T+",
	EPSC_Tminus,	"T-",

	// Misc
	EPSC_showpage,	"showpage",
	EPSC_end,		"end",
	EPSC_ctx,		"ctx",
	EPSC_ctex,		"ctex",

	EPSC_Invalid,	"Invalid",

	EPSC_Name,		"Name",
	EPSC_Integer,	"Integer",
	EPSC_Double,	"Double",
	EPSC_FixedPoint,"FixedPt",
	EPSC_String,	"String",
	EPSC_Comment,	"Comment",
	EPSC_EOL,		"EOL",
	EPSC_EOF,		"EOF"
};


RangeList EPSFilter::ImportRange;

/********************************************************************************************

>	EPSFilter::EPSFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Purpose:	Constructor for an EPSFilter object.  The object should be initialised
				before use.
	SeeAlso:	EPSFilter::Init

********************************************************************************************/

EPSFilter::EPSFilter()
{
	ImportMsgID = _R(IDT_IMPORTMSG_AI);
	Flags.CanImport = TRUE;
	//WEBSTER-Martin-27/01/97
#ifdef WEBSTER
	Flags.CanExport = FALSE;
#else
	Flags.CanExport = TRUE;
#endif //WEBSTER
	FilterID = FILTERID_AIEPS;

#ifndef DO_EXPORT
	Flags.CanExport = FALSE;
#endif

	EPSFile = NULL;
	TokenBuf = NULL;
	pColours = NULL;

	ExportDCPtr = NULL;
	ExportRegion = NULL;
	ExportMsgID = _R(IDT_EXPORTMSG_AI);

	AdjustOrigin = TRUE;
	SupportsLayers = TRUE;

	// The offsets into the file for the Preview thing
	EPSStart = 0;
	EPSEnd = 0;
	PreviewStart = 0;
	PreviewEnd = 0;

	TextComment[0] = 0;
	TextComment[1] = 0;
	TextComment[2] = 1;

	// set the font extras block vars. These indicate to the
	// filter to do something special with the next set of font attributes
	FontFlags.Bold = FALSE;
	FontFlags.Italic = FALSE;

	// Set the font type to assume we are loading true type fonts unless
	// told otherwise.
	ClassOfFont = FC_TRUETYPE;
	
	// Define the mould threshold default value.
	NewMouldThreshold = MOULD_V1THRESHOLD;

	//	We're not in a group at first.
	EPS_Group	=	FALSE;

	// Clear the length of the file to the end of the EPS code.
	EndOfEPS	= 0;

	// Set up the comment list. These are the known DSC comments, and how to handle them.
	EPSComments.Add ( "%%BeginProlog", 			"%%EndProlog",			TRUE,	0 );
	EPSComments.Add ( "%%BeginSetup",			"%%EndSetup",			TRUE,	0 );
	EPSComments.Add ( "%%BeginDocument",		"%%EndDocument",		FALSE,	0 );
	EPSComments.Add ( "%%BeginPageSetup",		"%%EndPageSetup",		FALSE,	0 );
	EPSComments.Add ( "%%BeginDefaults",		"%%EndDefaults",		FALSE,	0 );
	EPSComments.Add ( "%%BeginEmulation",		"%%EndEmulation",		FALSE,	0 );
	EPSComments.Add ( "%%BeginPreview",			"%%EndPreview",			FALSE,	0 );
	EPSComments.Add ( "%%BeginFeature",			"%%EndFeature",			FALSE,	0 );
	EPSComments.Add ( "%%BeginFile",			"%%EndFile",			FALSE,	0 );
	EPSComments.Add ( "%%BeginFont",			"%%EndFont",			FALSE,	0 );
	EPSComments.Add ( "%%BeginProcSet",			"%%EndProcSet",			FALSE,	0 );
	EPSComments.Add ( "%%BeginResource",		"%%EndResource",		FALSE,	0 );
	EPSComments.Add ( "%%BeginCustomColor",		"%%EndCustomColor",		FALSE,	0 );
	EPSComments.Add ( "%%BeginProcessColor",	"%%EndProcessColor",	FALSE,	0 );
}

/********************************************************************************************

>	virtual BOOL EPSFilter::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Returns:	FALSE => don't use this class for real filters!
	Purpose:	Initialise an EPSFilter object.  This is a base class and so should
				never actually be initialised so this function will always return FALSE,
				and also ENSURE in debug builds.

********************************************************************************************/

BOOL EPSFilter::Init()
{
	// This class is now used only as basis for deriving EPS filters - it should not
	// actually be used as a real filter.
	ENSURE(FALSE, "Base EPSFilter class called! Do not use this!");
	return FALSE;
}

/********************************************************************************************

>	BOOL EPSFilter::InitPrefs()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/01/95
	Purpose:	Initialise preferences for EPS formats.  This is used by the Camelot
				EPS export filter, and includes font mappings, DPI, PostScript language
				level control, and exporting text as curves.

********************************************************************************************/

BOOL EPSFilter::InitPrefs()
{
#ifndef STANDALONE

	// Register preferences for EPS output.
	Camelot.DeclareSection("EPS", 5);
	Camelot.DeclarePref(NULL, "ExportPSType", &XSEPSExportPSType, 0, 2); 
	Camelot.DeclarePref(NULL, "ExportDPI", &XSEPSExportDPI, 10, 600); 
	Camelot.DeclarePref(NULL, "ExportTextAsCurves", &XSEPSExportTextAsCurves); 

	// Register preferences for font mappings from TrueType to PostScript names.
	// This is used by Camelot EPS (or any other filter or class that wants to use it).
	Camelot.DeclareSection("EPSFontMapping", 50);

	static const char FontMappings[][2][30] = 
	{
		// Standard PostScript font mappings

		//
		// TrueType name          =>	PostScript name
		//
		{ "Times-New-Roman",			"Times-Roman" 					},
		{ "Times-New-Roman-Bold",		"Times-Bold" 					},
		{ "Times-New-Roman-Italic",		"Times-Italic" 					},
		{ "Times-New-Roman-BoldItalic",	"Times-BoldItalic" 				},
		{ "Arial",						"Helvetica"			 			},
		{ "Arial-Bold",					"Helvetica-Bold"				},
		{ "Arial-Italic",				"Helvetica-Oblique"				},
		{ "Arial-BoldItalic",			"Helvetica-BoldOblique"			},
		{ "Courier-New",				"Courier"			 			},
		{ "Courier-New-Bold",			"Courier-Bold"			 		},
		{ "Courier-New-Italic",			"Courier-Oblique"				},
		{ "Courier-New-BoldItalic",		"Courier-BoldOblique"			},
		{ "Michael",					"Palatino-Roman"				},
		{ "Michael-Bold",				"Palatino-Bold"					},
		{ "Michael-Italic",				"Palatino-Italic"				},
		{ "Michael-BoldItalic",			"Palatino-BoldItalic"			},
		{ "NewSchbook",					"NewCenturySchlbk-Roman" 		},
		{ "NewSchbook-Bold",			"NewCenturySchlbk-Bold" 		},
		{ "NewSchbook-Italic",			"NewCenturySchlbk-Italic" 		},
		{ "NewSchbook-BoldItalic",		"NewCenturySchlbk-BoldItalic"	},
		{ "ZapfDingbatsBT",				"ZapfDingbats"					},
		{ "Symbol",						"Symbol"						},

#if 0
		// Currently we have no TrueType equivalents for these fonts.
		{ "",							"AvantGarde-Book"				},
		{ "",							"AvantGarde-Demi"				},
		{ "",							"AvantGarde-BookOblique"		},
		{ "",							"AvantGarde-DemiOblique"		},
		{ "",							"Bookman-Light"					},
		{ "",							"Bookman-Demi"					},
		{ "",							"Bookman-LightItalic"			},
		{ "",							"Bookman-DemiItalic"			},
		{ "",							"ZapfChancery-MediumItalic"		},
#endif

		// Terminator
		{ "",							""								}
	};

	// Loop to declare all of these font mappings...
	INT32 i = 0;
	while (cc_strlenCharacters(FontMappings[i][0]) > 0)
	{
		Camelot.SetPrefDirect("EPSFontMapping", 
							  (char *) &FontMappings[i][0][0], 
							  &FontMappings[i][1][0]); 
		i++;
	}

#endif

	// All ok
	return TRUE;
}

// The size of the buffer used to hold tokens
const INT32 TokenBufSize = 256;

/********************************************************************************************

>	BOOL EPSFilter::PrepareToImport()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Returns:	TRUE if succeeded, FALSE if not (e.g. no memory for EPS stack)
	Purpose:	Prepare to import EPS data using this filter.  This sets up the filter
				to a sensible state for reading.
	Errors:		Out of memory.
	SeeAlso:	EPSFilter::DoImport; EPSFilter::CleanUpAfterImport
	Scope: 		Private

********************************************************************************************/

BOOL EPSFilter::PrepareToImport()
{
	if (!SetUpCurrentAttrs())
		return FALSE;

	// Start the selection operation.
	if (!ImportInfo.pOp->DoStartSelOp(FALSE))
		return FALSE;

	// Error if no file object.
	if (EPSFile == NULL)
		return FALSE;

	// Get ready to parse EPS tokens.
	EPSFile->InitLexer();
	EPSFile->SetDelimiters("()<>[]{}/%");
	EPSFile->SetCommentMarker('%');
	EPSFile->SetStringDelimiters("()");

	TokenBuf = EPSFile->GetTokenBuf();
	Token = EPSC_Invalid;

	EPSFlags.ComplexPath = 0;//FALSE;
	EPSFlags.StrokeComplexPath = FALSE;
	EPSFlags.NoAttributes = FALSE;
	EPSFlags.EPSErrorEncountered = FALSE;
	EPSFlags.AddToNewLayer = FALSE;
	EPSFlags.PathIsHidden = FALSE;

	ThePathType = PATH_NORMAL;


	// Decide whether we need to use layers or not
	// Neville 7/8/97 - Layers are bad in Webster as they conflict with frames
	// Now the same is true in Camelot
#ifndef WEBSTER
	// If the format supports layers and we are importing then we need to make a decision
	// based on whether the present document has frames in or not
	// Otherwise, just use the old code
	if (SupportsLayers && TheDocument->IsImporting())
	{
		// Otherwise if we are in:-
		// - a framed document then remove all layers from importing
		// - a layered document then import with layers
		ERROR2IF(ImportInfo.pSpread == NULL,FALSE,"EPSFilter::PrepareToImport No spread");
		Layer * pFrame = ImportInfo.pSpread->FindFirstFrameLayer();
		if (pFrame == NULL)
			UseLayers = TRUE;	// Layered = allow layers
		else
			UseLayers = FALSE;	// Framed = disallow layers
	}
	else
		UseLayers = SupportsLayers && Filter::ImportWithLayers;
#else
	// Do we want to import with layers? Use the preference flags to determine the actions
	if ( SupportsLayers &&
		(!TheDocument->IsImporting() && Filter::OpenWithLayers) ||
		(TheDocument->IsImporting() && Filter::ImportWithLayers)
		)
		UseLayers = TRUE;
	else
		UseLayers = FALSE;
#endif // WEBSTER

	// No path is being constructed
	pPath = NULL;
	pInkPath = NULL;

	// No nodes imported yet.
	ImportRange.DeleteAll();
	pFirstNodeInRange = NULL;
	pLastNodeInRange  = NULL;

	// No groups found yet.
	GroupNesting = 0;
	MaskedGroupNesting = 0;

	// No invalid paths found yet
	InvalidPathFound = FALSE;

	// Inform all the document components that we are about to import
	TheDocument->EPSStartImport(this);
	DocComponent *pComponent = TheDocument->EnumerateDocComponents(NULL);

	while (pComponent != NULL)
	{
		// Inform this document component that we are about to start an EPS import.
		pComponent->EPSStartImport(this);

		// If this is the colour component, remember it
		if (pComponent->GetRuntimeClass() == CC_RUNTIME_CLASS(ColourListComponent))
			pColours = (ColourListComponent *) pComponent;

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}
		
	ENSURE(pColours != NULL, "Didn't find a colour list component!");
	if (pColours == NULL)
		// Tricky to import if we can't find the colours!
		return FALSE;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	void EPSFilter::CleanUpAfterImport(BOOL Successful)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/02/94
	Inputs:		Successful: TRUE => the import succeeded
						   FALSE => something went wrong with the import, so abandon all
									changes made.
	Purpose:	Cleans up the memory allocated by EPSFilter::PrepareToImport() - used
				when the import process ends, either normally or abnormally.
	SeeAlso:	EPSFilter::PrepareToImport; EPSFilter::DoImport
	Scope: 		Private

********************************************************************************************/

void EPSFilter::CleanUpAfterImport(BOOL Successful)
{
	DeleteCurrentAttrs();

	// Get rid of file object
	EPSFile->DeinitLexer();
	EPSFile = NULL;
	TokenBuf = NULL;

	// get rid of anything on the graphic state stack
	TRACEUSER( "Ben", _T("Entries on GraphicStateStack = %d\n"), GraphicStateStack.GetCount());
	GraphicStateStack.DeleteAll();

	// Throw away handle to colour list component.
	pColours = NULL;

	// Inform all the document components that we have finished importing
	TheDocument->EPSEndImport(this, Successful);
	DocComponent *pComponent = TheDocument->EnumerateDocComponents(NULL);

	while (pComponent != NULL)
	{
		// Inform this document component that we have finished importing
		pComponent->EPSEndImport(this, Successful);

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}

	// If imported with layers, make sure the top layer becomes the active one.
	if (Successful) // && UseLayers)
	{
		LayerSGallery::MakeTopLayerActive(ImportInfo.pSpread);
		BROADCAST_TO_ALL(SpreadMsg(ImportInfo.pSpread,SpreadMsg::LAYERCHANGES));
	}

	// Inform the user if we found any bad paths
	// BODGE: This is disabled while we are doing the clipart so the batch converter
	// 		  can run unattended.
	if (0 && InvalidPathFound)
		InformWarning(_R(IDT_IMPORT_BAD_PATH_FOUND));

	// Clean up import range items.
	ImportRange.DeleteAll();

	TheDocument->PostImport();
}



/********************************************************************************************

>	INT32 EPSFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, 
							 UINT32 HeaderSize, UINT32 FileSize)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		Filename - The name of the file to get bits of
				HeaderStart - The block of mem that holds the first part of the file to be
				read in
				HeaderSize - The size of the bit of memory
				FileSize - The size of the file
	Returns:	0 to 10. 0 means I know nothing about this file. 10 means its definatly mine
	Purpose:	Looks at the start of the file to see if it recognises the file as belonging
				to it.

********************************************************************************************/

INT32 EPSFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, 
							 UINT32 HeaderSize, UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	return EPSHeaderIsOk(HeaderStart, HeaderSize);
}



/********************************************************************************************

>	virtual BOOL EPSFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		pcszPathName	pointer to the pathname to check
	Returns:	TRUE if the filter requires a default document, FALSE if not.
	Purpose:	Works out if opening a file of this type requires a default document to be
				loaded. If the file format supplies the document then return FALSE otherwise
				return TRUE. An example would be opening a bitmap file. This has no document
				defined in the file format and so we need to laod the default document before
				importing the bitmap into this file.
				In this baseclass version return FALSE and hence assume that the filters that
				need to will override this function to return TRUE.
	SeeAlso:	Filter; Filter::IsDefaultDocRequired; CCamDoc::OnOpenDocument;
	SeeAlso:	FilterFamily::DoImport; Filter::DoImport;

********************************************************************************************/

BOOL EPSFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)
{
	return TRUE;
}	


/********************************************************************************************

>	INT32 EPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Inputs:		pFileHeader - pointer to the first few bytes of the file.
				HeaderSize - the number of bytes pointed to by pFileHeader.
	Returns:	A number between 0 and 10 indicating how much it likes this file - see
				Filter::HowCompatible for details.
	Purpose:	Checks to see if the EPS comment headers specify that this is an Illustrator
				generated EPS file, as required.
	SeeAlso:	Filter::HowCompatible

********************************************************************************************/

INT32 EPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)
{
	// Check the first line in EPS file
	if (_tcsncmp((char *) pFileHeader, "%!PS-Adobe", 10) != 0)
	{
		// Incorrect version of EPS header line - we don't want this
		return 0;
	}

	// !PS-Adobe line is ok - check creator line...
	istrstream HeaderFile((char *) pFileHeader, HeaderSize);
	char Buffer[200];

	UINT32 Lines = 0;
	while ((Lines < 20) && !HeaderFile.eof())
	{
		HeaderFile.getline(Buffer, 200);
		Lines++;

		// Return TRUE if this file was created by Illustrator, or has been exported in 
		// Illustrator format.
		if (_tcsncmp(Buffer, "%%Creator: Adobe Illustrator", 28) == 0)
			// We definitely want this.
			return 8;

		if (_tcsncmp(Buffer, "%%Creator:", 10) == 0)
		{
			// Found the creator line - does it contain the word Illustrator?
			if (_tcsstr(Buffer, "Illustrator") != NULL)
				return 9;
			else
				break;
		}

		// If we find the compression token then stop the search as we don't want to start
		// looking in the compressed data!
		if (_tcsncmp(Buffer, "%%Compression:", 14)==0)
			break;
	}

	// Didn't find a suitable Creator line, but the EPS line was ok, so return
	// that we're interested, but not sure.
	return 5;
}

/********************************************************************************************

>	BOOL EPSFilter::DoImport(SelOperation *Op, CCLexFile* pFile, 
						 Document* DestDoc, BOOL AutoChosen, ImportPosition* Pos,
						 KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Inputs:		Op - pointer to the operation that this input process is associated with.
				pFile - the EPS file to be loaded.
				DestDoc - The Document object which should hold the data read in from
						  the EPS file.
				Pos -
				ppImportedBitmap - this is used mainly in the bitfltr.cpp for the HTML
				import filter. HTMLFilter::DoImport() needs a pointer to a kernel bitmap
				to set the background bitmap up into Camelot.
				pPosTranslate - This is used too by the HTMLFilter in order to do a formatting.
				URL - URL of the original imported file
	Returns:	TRUE if the input operation worked ok, FALSE if not.
	Purpose:	Read EPS data from an EPS file, and convert it into Camelot tree data
				structures.  When the file reading is complete, a forced redraw is issued
				for the area affected by the new data.
				At the moment, the data is just put on the first layer of the first spread
				of the first chapter in the document, and this is a bodge.
				The operation is terminated (i.e. its End() function is called) if the
				operation completed succesfully.
	Errors:		Fails (returns FALSE) if the document structure is incorrect, if the
				EPS file contains a syntax error, if the EPS file cannot be found/opened, 
				or if the EPS file is not actually an EPS file.
	SeeAlso:	EPSFilter::PrepareToImport; EPSFilter::CleanUpAfterImport


********************************************************************************************/

BOOL EPSFilter::DoImport(SelOperation *Op, CCLexFile* pFile, 
						 Document* DestDoc, BOOL AutoChosen, ImportPosition* Pos,
						 KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)
{
	// Set up document pointer
	TheDocument = DestDoc;

	// Remember operation and position so we can use them to add nodes in other functions.
	ImportInfo.pOp = Op;
	ImportInfo.Pos = Pos;
	ImportInfo.pSpread = NULL;

	// Get ready to read some EPS data
	EPSFile = pFile;

	// See if we are capable of having a Preview bitmap attached.
	if ( pFile->IS_KIND_OF ( CCStreamFile ) )
	{
		// OK, we will have to check for a preview bmp and skip to the Postscript if there is one
		// read the first 4 bytes and see if they match the magic PostScript word
		char Buf[4];
		
		// clear the buffer
		memset(Buf, 0, 4);

		// read the bytes in
		pFile->read(Buf, 4);

		// see if its a match
		if ((Buf[0]=='\xC5') && (Buf[1]=='\xD0') && (Buf[2]=='\xD3') && (Buf[3]=='\xC6'))
		{
			// Yes, this is a binary EPS file that has a TIFF attached, so find the EPS
			FilePos	StartOfEPS	= 0;
			EndOfEPS			= 0;

			pFile->read(&StartOfEPS, 4);
			TRACEUSER( "Rik", _T("EPS Starts at %ld\n"), StartOfEPS);

			// Now try to find a pointer to the end of the EPS stuff.
			pFile->read ( &EndOfEPS, 4 );

			EndOfEPS += StartOfEPS;

			// Seek to the start of the EPS ready for the importer to read it.
			pFile->seekIn(StartOfEPS, ios::beg);
		}
		else
		{
			// Nope, this must be any ordinary EPS file, so go back to the start of the file
			pFile->seekIn(0, ios::beg);

			EndOfEPS = pFile->Size ();
		}
	}

	// Get the right spread...
	Spread *pSpread;

	if (Pos == NULL)
	{
		// Find the layer on the first page of this document...
		pSpread = GetFirstSpread(DestDoc);
	}
	else
	{
		// Use the spread provided
		pSpread = Pos->pSpread;
	}

	// Remember it
	ImportInfo.pSpread = pSpread;

	// Let the filter set up itself ready to import
	if (!PrepareToImport())
	{
		// Didn't work (no memory)
		CleanUpAfterImport(FALSE);
		return FALSE;
	}

	pLayer = pSpread->FindActiveLayer();

	// Get the spread's bounding rectangle and convert it to spread coords.
	SpreadRect = pSpread->GetPasteboardRect();
	pSpread->DocCoordToSpreadCoord(&SpreadRect);

	// pNode is where new objects will go - they all go into a group so the user
	// can easily drag the imported EPS file around.  They can then ungroup it afterwards
	// if they want.

	// If we're ignoring layers, load it all in as a group on the active layer;
	// otherwise default to adding to the active layer.
	NodeGroup *pEPSGroup = NULL;
	if (UseLayers)
	{
		EPSFlags.AddToNewLayer = FALSE;
		pNode = (Node *) pLayer;
	}
	else
	{
		pEPSGroup = new NodeGroup;
		ERRORIF(pEPSGroup == NULL, _R(IDT_EPS_NOMEMORY), FALSE);
		pNode = pEPSGroup;
	}
		
	// Scaling factor is 1000 for ArtWorks EPS files
	Stack.SetCoordScaleFactor(EPSScaleFactor);

	if (Pos == NULL)
	{	
		// For now, position EPS objects on 1st page of spread 1
		Page *pPage = pSpread->FindFirstPageInSpread(); 
		ERROR3IF(pPage == NULL, "Spread has no pages"); 

		// Use bottom left of page as origin
		if (pPage)
		{
			DocRect PageRect = pPage->GetPageRect();
			Stack.SetCoordOrigin(PageRect.lo);
		}
	}
	else
	{
		// Use position provided
		Stack.SetCoordOrigin(Pos->Position);
	}
	
	// No complex path initially
	EPSFlags.ComplexPath = 0;//FALSE;

	// Try to open the file
	// The file is already open when it is passed in

	// Find out the size of the file, in bytes.
	INT32 filesize = EPSFile->Size();
	if (filesize == -1)
	{
		if (IsUserName("JustinF"))
			TRACE( _T("Couldn't take size of the EPS file\n"));
		return FALSE;
	}

	// Initialise
	LastProgressUpdate = 0;

	// Set the progress indicator, this next bit might take a while.
	//String_64 ImportMessage(ImportMsgID);
	String_64 ImportMessage = GetImportProgressString(EPSFile, GetImportMsgID());
	BeginSlowJob(filesize, TRUE, &ImportMessage);

	// Process each token in the file
	do
	{
		GetToken();
	}
	while (HandleToken() && (!EPSFlags.EPSErrorEncountered) && (!EPSFile->eof()));

	// File loaded ok if we reached eof (and no outstanding groups left)
	BOOL Ok = (GroupNesting == 0) && 
			  (MaskedGroupNesting == 0) && 
			  (EPSFile->eof() && !EPSFile->fail());

	#if _DEBUG
	Stack.Dump(this);
	#endif
	
	// If the load failed for any reason, delete the subtree we have created; otherwise
	// graft it into the tree (currently bodged to be on the first layer of the second page.
	if (!Ok)
	{
		// Failed - delete the sub-tree we just created, and report error.
		if (UseLayers)
		{
			// Clean up if we need to
			if (pNode!=NULL)
			{
				// If UseLayers is set, the pNode usually refers to a node in the tree
				// So the last thing we want to do is start
				// deleting it. The operation unwinding will do that for us.

				//pNode->CascadeDelete();
				//delete pNode;
				pNode = NULL;
			}
		}
		else
		{
			// Clean up the group if we need to
			if (pEPSGroup!=NULL)
			{
				pEPSGroup->CascadeDelete();
				//delete pEPSGroup;
				pEPSGroup = NULL;
			}
		}

		// Tidy up after ourselves.
		// NB. we must delete the imported group before telling the document components,
		//     otherwise, eg. the colour list component will try to delete indexed colours
		//     when they are still referenced by nodes in the group.
		CleanUpAfterImport(FALSE);

		// Detach document.
		TheDocument = NULL;

		Op->FailAndExecute();
				
		// All work has been completed.
		EndSlowJob();
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
		DialogBarOp::UpdateStateOfAllBars();
#endif
		return FALSE;
	}

	// Try to attach any outstanding nodes
	BOOL Success = AttachAllNodes();

	// (Chris G 17/11/00) - the following fix was removed, as it was causing a crash when
	//	new clipart indices were created from old .xar files (by selecting a new folder with
	//	at least one old (CorelXara 2) .xar file in the file dialog which appears after the
	//	"DiscClipart" button is pressed (in the Clipart gallery)). Since the old bug report
	//	numbers are no longer valid (this was added to fix bug nos. 1469 and 3262 in 1995 - 
	//	checked into sourcesafe database "VSS", epsfiltr.cpp, checkin 82/83 by Ben Summers 
	//	on 8/7/95 and 12/7/95) it was impossible to find out why this fix was added, but it 
	//	appears to attempt to avoid name-clashing of the layers.

	//	This fix: Sourcesafe DB: "Camelot2000", epsfiltr.cpp, ChrisG, no. 28, 17/11/00

		// nasty thing:
		// if importing with layers, and there's only one layer, make it pretend it was
		// importing without layers. ie, completely ignore the user's preferences if we
		// just happen to feel like it.


	// Tidy up after ourselves
	CleanUpAfterImport(TRUE);

	// Detach document.
	TheDocument = NULL;

	// End job and inform caller of the success or failure of the venture.
	EndSlowJob();
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	DialogBarOp::UpdateStateOfAllBars();
#endif
	return Success;
}

/********************************************************************************************

>	BOOL EPSFilter::HandleToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the token was processed ok, FALSE if not.
	Purpose:	'Handles' the current EPS token - if it's an EOL or EOF token, it updates
				the progress display, otherwise it just calls ProcessToken();
	Errors:		Syntax error in EPS file, User interrupted with Ctrl-C, Out of memory.
	SeeAlso:	EPSFilter::GetToken; EPSFilter::ProcessToken

********************************************************************************************/

BOOL EPSFilter::HandleToken()
{
	BOOL Error = FALSE;	

	// At the end of a line or the file?
	if ((Token != EPSC_EOL) && (Token != EPSC_EOF))
	{
		// No, so try to process this token - if it fails, we have encountered an
		// error, so quit the loop.
		if (!ProcessToken())
			return FALSE;
	}
	else
	{
		// Update the progress counter at the end of every 2kbytes, and break out of
		// the loop if the user has hit Crtl-C etc.
		INT32 CharsRead = EPSFile->GetCharsRead();

		if (CharsRead > (LastProgressUpdate + 2048))
		{
			if (!ContinueSlowJob(CharsRead))
			{
				// Abort operation - make sure nodes are deleted and not added to the tree.
				ERROR(_R(IDT_IMPORT_USERABORT), FALSE);
			}

			// Update the progress counter
			LastProgressUpdate = CharsRead;
		}
	}

	// All ok
	return TRUE;
}


/********************************************************************************************

>	void EPSFilter::DecodeToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Purpose:	Decode the PostScript token that is contained in the 'TokenBuf' buffer.
				This is not used for strings, comments, or array brackets, but is used to
				detect tokens which are floating point numbers, integers, EPS keywords, or
				PostScript 'names' (e.g. font names).  In other words, any token which
				cannot be identified solely by its first character.
	SeeAlso:	EPSFilter::GetToken
	Scope:		Private

********************************************************************************************/

void EPSFilter::DecodeToken()
{
	//
	// Determine what we just read into TokenBuf.
	//
	ENSURE(_tcscmp(TokenBuf,"}bd") != 0, "Found a }bd token!");
		
	// Not interested in comments, unless it's actually a strange token
	if (Token == EPSC_Comment)
	{
		LookUpToken();
		return;
	}

	// i is the index if the character we are currently examining.
	INT32 i = 0;

	// FoundDot is TRUE if we have already encountered a '.' character in this token.
	BOOL FoundDot = FALSE;

	// Number is TRUE if we haven't found any characters (or combination of characters,
	// eg. a token with two '.' characters in it) which mean that this token cannot be a 
	// number (either floating point ot integer).
	BOOL Number = TRUE;

	// FoundFloatingPoint is TRUE if we have found a number that cannot be represented
	// by an integer or fixed point, i.e. it is in scientific notation (with exponent), or
	// the number has more than 3dp of precision.
	BOOL FoundFloatingPoint = FALSE;
	INT32 Precision = 0;

	// Try a floating point number first...

	while (TokenBuf[i] != 0)
	{
		char c = TokenBuf[i];
		
		if (c == '.')
		{
			if (FoundDot)
			{
				// Found more than one dot - not a number
				Number = FALSE;
				break;
			}
			else
			{
				// Found the first '.' in this token
				FoundDot = TRUE;
			}
		}
		else
		{
			if (FoundDot)
			{
				// If it's a number, check the precision needed to represent it
				if (isdigit(c))
				{
					Precision++;
					if (Precision > 3)
						FoundFloatingPoint = TRUE;
				}
				else
				{
					// Check for illegal characters
					if ((c == 'e') || (c == 'E'))
					{
						// Found an exponent
						FoundFloatingPoint = TRUE;
					}
					else if ((c != '-') && (c != '+'))
					{
						// Illegal character - e.g. alphanumeric, etc
						Number = FALSE;
						break;
					}
				}
			}
			else if (!isdigit(c) && (c != '-') && (c != '+'))
			{
				// Illegal character - e.g. alphanumeric, etc
				Number = FALSE;
				break;
			}
		}
		
		i++;
	}
	
	if (Number)
	{
		// It's a number - is it integer, fixed point, or floating point?
		if (FoundFloatingPoint)
		{
			// Floating point
			Token = EPSC_Double;
			TokenData.Double = atof(TokenBuf);
		}
		else if (FoundDot)
		{
			// Fixed point
			Token = EPSC_FixedPoint;
			TokenData.FixedPoint.FromAscii(TokenBuf);
		}
		else
		{
			// Integer
			Token = EPSC_Integer;
			TokenData.Long = _ttol(TokenBuf);
		}

		return; // All done - found a number
	}
	
	// Try to find this token in the set(s) of tokens understood by this filter.
	LookUpToken();
}

/********************************************************************************************

>	void EPSFilter::LookUpToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/05/94
	Purpose:	Determines if the current token is one of the standard Illustrator tokens.
				The 'Token' variable is set accordingly.  If the token is not recognised,
				it is assumed to be a PostScript 'name'.
	SeeAlso:	EPSFilter::DecodeToken; EPSFilter::ProcessToken

********************************************************************************************/

void EPSFilter::LookUpToken()
{
	// Not interested in comments
	if (Token == EPSC_Comment)
		return;

	// Check to see if it is a keyword - cycle through the array of keyword names and
	// compare against our token (could use a hash table?)
	INT32 i = 0;
	while (Commands[i].Cmd != EPSC_Invalid)
	{
		if (_tcscmp(TokenBuf, Commands[i].CmdStr) == 0)
		{
			// Found the token - set the token variable and exit.
			Token = Commands[i].Cmd;
			return;
		}
		// Try next command
		i++;
	}

	// Keyword not found - must be a PostScript 'name'
	Token = EPSC_Name;
}

/********************************************************************************************

>	BOOL EPSFilter::GetToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Returns:	TRUE if a token was obtained ok, FALSE if not.
	Purpose:	Extract a token from the input stream.  The Token variable holds the type
				of the token, and TokenBuf holds the characters that make up the token (be
				it a command, a string. a number, or whatever).
	Errors:		Fails if EOF detected while parsing a token (e.g. in the middle of a 
				PostScript string token).
	Scope:		Private

********************************************************************************************/

BOOL EPSFilter::GetToken()
{
	// Initialise the token type.
	Token = EPSC_Invalid;

	// Ensure that the file doesn't run on past the end of the EPS segment.
	if ( !EPSFile->IsCompressionSet () && EPSFile->tell () > EndOfEPS )
	{
		// Skip to the end of the file.
		EPSFile->seekIn ( 0, ios::end );
	}

	if (!EPSFile->GetToken())
		return FALSE;

	// No effect if at EOF
	switch (EPSFile->GetTokenType())
	{
		case TOKEN_EOF:
			return TRUE; // EOF does not count as an error - TOKEN_EOF is a valid token

		case TOKEN_EOL:
			Token = EPSC_EOL;
			return TRUE;

		case TOKEN_STRING:
			Token = EPSC_String;
			return TRUE;

		case TOKEN_COMMENT:
			Token = EPSC_Comment;
			break; // Comments may be tokens in some EPS formats.
	}

	ENSURE((EPSFile->GetTokenType() == TOKEN_NORMAL) || (EPSFile->GetTokenType() == TOKEN_COMMENT), 
		   "Bad token type in EPSFilter::GetToken()");
	
	// Decode this token		
	DecodeToken();

	// Token extracted ok
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSFilter::GetLineToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Returns:	TRUE if a token was obtained ok, FALSE if not.
	Purpose:	Extract a token from the input stream in a line-based manner.  
				The Token variable holds the type of the token, and TokenBuf holds the 
				characters that make up the token (be it a command, a string. a number, or 
				whatever).
	Errors:		Fails if EOF detected while parsing a token (e.g. in the middle of a 
				PostScript string token).
	Scope:		Private
	SeeAlso:	EPSFilter::GetToken; CCLexFile::GetLineToken

********************************************************************************************/

BOOL EPSFilter::GetLineToken()
{
	Token = EPSC_Invalid;

	// Ensure that the file doesn't run on past the end of the EPS segment.
	if ( !EPSFile->IsCompressionSet () && EPSFile->tell () > EndOfEPS )
	{
		// Skip to the end of the file.
		EPSFile->seekIn ( 0, ios::end );
	}

	if (!EPSFile->GetLineToken())
		return FALSE;

	// No effect if at EOF
	switch (EPSFile->GetTokenType())
	{
		case TOKEN_EOF:
			return TRUE; // EOF does not count as an error - TOKEN_EOF is a valid token

		case TOKEN_EOL:
			Token = EPSC_EOL;
			return TRUE;

		case TOKEN_STRING:
			Token = EPSC_String;
			return TRUE;

		case TOKEN_COMMENT:
			// Give filters a chance to claim comments as tokens for some strange EPS
			// formats (e.g. Illustrator 5)
			Token = EPSC_Comment;
			DecodeToken();
			return TRUE;
	}

	ENSURE((EPSFile->GetTokenType() == TOKEN_NORMAL) || 
		   (EPSFile->GetTokenType() == TOKEN_LINE), 
		   "Bad token type in EPSFilter::GetLineToken()");
	
	// Decode this token		
	DecodeToken();

	// Token extracted ok
	return TRUE;
}


/********************************************************************************************

>	BOOL EPSFilter::AddNewNode(Node *pNewNode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/08/94
	Inputs:		pNewNode - the new node to add
	Returns:	TRUE if it worked, 
				FALSE if not.
	Purpose:	When a new node is created, this function puts it into the tree in the
				correct place.  It may add it into a group, or a new layer which is currently
				being constructed, or onto an existing layer.
	Errors: 	Out of memory
	SeeAlso:	EPSFilter::UseLayer

********************************************************************************************/

BOOL EPSFilter::AddNewNode(Node *pNewNode)
{
	if ((EPSFlags.AddToNewLayer) || (!UseLayers))
	{
		// Simple case - just add to the new layer node
		pNewNode->AttachNode(pNode, LASTCHILD);

		if (pNewNode->IsBounded())
		{
			// Update the bounds for this object
			NodeRenderableBounded *pBounded = (NodeRenderableBounded *) pNewNode;
			pBounded->InvalidateBoundingRect();
		}
	}
	else
	{
		// Tricky case - we have to add the node to the existing layer in the 
		// document in an undoable way.
		return InsertNewNode(pNewNode);
	}

	// All went ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSFilter::InsertNewNode(Node *pNewNode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/08/94
	Inputs:		pNewNode - the node to add.
	Returns:	TRUE if the node is added to the tree successfully;
				FALSE if not
	Purpose:	This function adds a node to the tree in an undoable way.  It uses 
				DoInsertNewNode() to do this. If it fails then 'pNewNode' and all its
				children are deleted.
				It adds the node as a child of the node pointed to by 'pNode'.
	Errors:		Out of memory
	SeeAlso:	UndoableOperation::DoInsertNewNode

********************************************************************************************/

BOOL EPSFilter::InsertNewNode(Node *pNewNode)
{
	// We have to add a NodeRenderableBounded.
	ENSURE(pNewNode->IS_KIND_OF(NodeRenderableBounded), 
		   "Trying to add a non-bounded node while importing!");
	NodeRenderableBounded *pNodeBounded = (NodeRenderableBounded *) pNewNode;
	
	// Add the node and invalidate the region...
	if (pNodeBounded == (NodeRenderableBounded *) pLayer)
	{
		// Adding a new layer - sanity check
		ENSURE(pNode == ImportInfo.pSpread, "Not adding layer to correct spread!");
	}

	// Some flags to see if things worked
	BOOL Success = FALSE;
	BOOL ReadyToInsert = TRUE;

	// We will see if the new node is a layer.
	// if not we will have to check to see if there is a layer in the document
	if (!pNewNode->IS_KIND_OF(Layer))
	{
		// Its not a layer, so check that there is a layer (and try and make one if not)
		if (!MakeSureLayerExists(TheDocument))
		{
			// There is no layer and one could not be made, so we will have to fail
			ReadyToInsert = FALSE;
		}
		else
		{
			// There is a layer in the document, so change the insertion node for the
			// filter if we need to.
			if (pNode==NULL)
			{
				// Find the spread and the active layer on it
				Spread* pSpread = ImportInfo.pSpread;
				if (pSpread!=NULL)
					pNode = (Node*) pSpread->FindActiveLayer();
			}
		}
	}

	// If we passed the layer test, then we can carry on and try and insert the new nodes
	if (ReadyToInsert)
	{
		// We don't invalidate the region because we do that at the end when the import
		// has finished.
		if (UseLayers)
		{
			// Importing with layers - don't select the imported objects.
			Success = ImportInfo.pOp->DoInsertNewNode(pNodeBounded, pNode, LASTCHILD,
												  	  FALSE, FALSE, FALSE);

			// Update the range pointers
			if (pFirstNodeInRange == NULL)
				pFirstNodeInRange = pNodeBounded;

			// Only update if the node is a direct child of a layer, because node ranges
			// should be held in terms of pointers to children of layers, otherwise the
			// end of the range will not be found and will spill over.
			if (pNodeBounded->FindParent()->IS_KIND_OF(Layer))
				pLastNodeInRange = pNodeBounded;
		}
		else
		{
			// This is without layers, i.e. as a group, so we want to add it to the correct
			// place on the active layer, so we use this form of DoInsertNewNode which uses
			// the 'InsertionNode' thingy to put it in the right place.
			Success = ImportInfo.pOp->DoInsertNewNode(pNodeBounded, ImportInfo.pSpread,
												  	  FALSE);
		}
	}

	// If it worked, try to optimise attributes - this currently just factors out attributes 
	// common to compound objects.
	if ((!Success) || (!pNewNode->OptimiseAttributes()))
	{
		// It didn't work - delete the sub-tree we've created
		pNewNode->CascadeDelete();
		delete pNewNode;

		// Return failure
		ImportInfo.pOp->FailAndExecute(); 
		return FALSE; 
	}

	// It worked!
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSFilter::AttachAllNodes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/08/94
	Returns:	TRUE if it worked ok;
				FALSE if not.
	Purpose:	Called at the end of an import - attaches any nodes to the tree that have
				not yet been attached. e.g. a new layer node, or a group node if layers are
				being ignored.
	Errors:		Out of memory
	SeeAlso:	EPSFilter::AddNewNode

********************************************************************************************/

BOOL EPSFilter::AttachAllNodes()
{
	BOOL Success = TRUE;
	NodeGroup *pGroup;

	if (UseLayers)
	{
		// Check to see if there is an outstanding layer node to add...
		if (EPSFlags.AddToNewLayer)
		{
			// Yes - add it...

			// We're adding the layer to the spread now
			EPSFlags.AddToNewLayer = FALSE;
			Node *pLayer = pNode;
			pNode = ImportInfo.pSpread;

			// Do it.
			Success = AddNewNode(pLayer);

			// Remember it
			if (Success)
				ImportRange.AddRange(pLayer, pLayer);
		}
		else if (pFirstNodeInRange != NULL)
		{
			// We have just finished adding a range of nodes to an existing layer so let's
			// remember the nodes.
			ImportRange.AddRange(pFirstNodeInRange, pLastNodeInRange);
			pFirstNodeInRange = NULL;
			pLastNodeInRange  = NULL;
		}
	}
	else
	{
		// No layers - always imported as a group, so add it into the tree.
		pGroup = (NodeGroup *) pNode;
		pNode = (Node *) pLayer;

		Success = InsertNewNode(pGroup);
	}

	if (!Success)
		return FALSE;

	// we need the bounding rect to do this (not the one with attributes)
	DocRect BoundsRect;

	if (UseLayers)
		BoundsRect = ImportRange.GetBoundingRect();
	else
		BoundsRect = pGroup->GetBoundingRect(TRUE);

	// Translate objects to the drag and drop position if necessary...
	Coord Offset;

	if (GetDragAndDropTranslation(ImportInfo.Pos, BoundsRect, &Offset))
	{
		Trans2DMatrix Xlate(Offset.x, Offset.y);

		if (UseLayers)
		{
			// Transform the range of nodes and then delete the range info.
			ImportRange.Transform(Xlate);

			// Transform the bounding rect too because we use it to invalidate the region.
			BoundsRect.Translate(Offset.x, Offset.y);
		}
		else
		{
			// Just transform the group node
			pGroup->Transform(Xlate);
		}
	}

	// Invalidate the region

	if (UseLayers)
	{
		// Importing with layers - invalidate the bounds of all the inserted nodes
		Success = ImportInfo.pOp->DoInvalidateRegion(ImportInfo.pSpread, BoundsRect);
	}
	else
	{
		// When importing as a group, we select the group node at the end, so we need to 
		// invalidate the blob bounding rect to get the redraw correct.
		Success = ImportInfo.pOp->DoInvalidateNodeRegion(pGroup, TRUE, FALSE);
	}

	if (!Success)
	{
		// Need to add tidy up code here.
		return FALSE;
	}

	// Make sure the node insertion position is in the correct place
	TheDocument->ResetInsertionPosition();

	return Success;
}

/********************************************************************************************

>	BOOL EPSFilter::UseLayer ( String_256&	LayerName,
							   BOOL			GuideLayer	= FALSE )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/00
	Inputs:		LayerName	- The name of the new layer.
				GuideLayer	- TRUE to create a guide layer, FALSE to create a standard layer.
	Returns:	TRUE		- Success.
				FALSE		- Failure - the layer wasn't created.
	Purpose:	This is a wrapper around the new AddLayer method, which now returns a pointer
				to the newly created layer, which is where all the work is done. I've done
				this to preserve the original interface.
	SeeAlso:	EPSFilter::AddLayer ()

********************************************************************************************/

BOOL EPSFilter::UseLayer ( String_256& LayerName,
						   BOOL GuideLayer )
{
	// Create a new layer using the layer name information.
	if ( AddLayer ( LayerName, GuideLayer ) != NULL )
	{
		// Things went OK.
		return TRUE;
	}
	else
	{
		// A problem occurred.
		return FALSE;
	}
}

/********************************************************************************************

>	Layer* EPSFilter::AddLayer ( String_256&	LayerName,
								 BOOL			GuideLayer )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>	(based on Tim's code)
	Created:	17/4/00	(originally 16/08/94)
	Inputs:		LayerName	- The name of the new layer.
				GuideLayer	- TRUE to create a guide layer, FALSE to create a standard layer.
	Returns:	pLayer		- A pointer to the newly creaded layer.
	Purpose:	Given a layer name, construct a new layer, or find an existing layer with
				the same name, and cause new nodes to be added to the resulting layer.
	Errors:		An error is thrown if the layer is not created.
	SeeAlso:	EPSFilter::AddNewNode

********************************************************************************************/

Layer* EPSFilter::AddLayer ( String_256& LayerName,
							 BOOL GuideLayer )
{
	// Have we currently got a pending new layer to add to the tree?
	if (EPSFlags.AddToNewLayer)
	{
		// Yes - add the new layer to the tree.
		EPSFlags.AddToNewLayer = FALSE;
		pNode = ImportInfo.pSpread;
		if (!InsertNewNode((Node *) pLayer))
			return FALSE;

		// Remember it
		ImportRange.AddRange(pLayer, pLayer);
	}
	else if (pFirstNodeInRange != NULL)
	{
		// We have just finished adding a range of nodes to an existing layer so let's
		// remember the nodes.
		ImportRange.AddRange(pFirstNodeInRange, pLastNodeInRange);
		pFirstNodeInRange = NULL;
		pLastNodeInRange  = NULL;
	}

	// Find out if we need a new layer name, because there is already one with this name.
	String_256 NewLayerName = LayerName;
	MakeLayerNameUnique(NewLayerName, ImportInfo.pSpread);

	// Make a new layer using this name.
	if (GuideLayer)
	{
#if !defined(EXCLUDE_FROM_RALPH)
// WEBSTER - markn 15/1/97
// CreateGuideLayer() used to be part of OpGuideline class, now part of Layer class
		pLayer = Layer::CreateGuideLayer();
#else
		ERROR3("EPSFilter::UseLayer GuideLayer == TRUE - whoop whoop ! BAD !");
#endif
	}
	else
		pLayer = new Layer;

	ERRORIF(pLayer == NULL, _R(IDT_EPS_NOMEMORY), FALSE);
	pLayer->SetLayerID(NewLayerName);
	EPSFlags.AddToNewLayer = TRUE;

	// Make it the new destination node for all new nodes.
	pNode = (Node *) pLayer;

	// Pass the layer pointer out.
	return pLayer;
}


/********************************************************************************************

>	void EPSFilter::MakeLayerNameUnique(String_256& LayerName, Spread *pSpread)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Given a layer name, ensure it is unique within the spread. If a clash occurs
				then the following layer names are tried until a unique one is found.
				(Assuming layer name is "Foreground" in this example)
					
					Imported Foreground
					Foreground 2
					Foreground 3
					Foreground 4
					 	.
						.
						.
	Errors:		-
	SeeAlso:	EPSFilter::UseLayer

********************************************************************************************/

void EPSFilter::MakeLayerNameUnique(String_256& LayerName, Spread *pSpread)
{
	BOOL TriedImported = FALSE;
	UINT32 Number = 2;
	String_256 NewName = LayerName;
	
	Layer *pFirstLayer = pSpread->FindFirstLayer();
	Layer *pLayer = pFirstLayer;

	while (pLayer != NULL)
	{
		// Get info on this layer.
		LayerStatus Status = pLayer->GetLayerStatus();

		// Does this layer have the same name as the proposed new one?
		if (Status.StringLayerID == NewName)
		{
			// Yes - try a new name
			if (TriedImported)
			{
				// Try adding a number instead of "Imported"
				TCHAR NumBuf[10];
				wsprintf(NumBuf, TEXT(" %d"), Number);
				NewName = LayerName;
				NewName += NumBuf;

				// Make sure we try a different number next time
				Number++;
			}
			else
			{
				// Try prefixing the name with "Imported"
				NewName.Load(_R(IDS_K_EPSFILTER_IMPORTED));
				NewName += LayerName;
				TriedImported = TRUE;
			}

			// Start from first layer again
			pLayer = pFirstLayer;
		}
		else
		{
			// Try next layer
			pLayer = pLayer->FindNextLayer();
		}
	}

	// Found an acceptable name - use it
	LayerName = NewName;
}



/********************************************************************************************

>	BOOL EPSFilter::StartGroup()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/08/94
	Returns:	TRUE if the new group was created ok;
				FALSE if not.
	Purpose:	Used when a group structure needs to be created - after this is called,
				all new nodes added with AddNewNode() will be added as children of this
				new group, until EndGroup is called.
	Errors:		Out of memory
	SeeAlso:	EPSFilter::EndGroup

********************************************************************************************/

BOOL EPSFilter::StartGroup()
{
	// Make a new group node for this group
	NodeGroup *pGroup = new NodeGroup;
	ERRORIF(pGroup == NULL, _R(IDT_EPS_NOMEMORY), FALSE);

	// Add it into the tree
	if (!AddNewNode(pGroup))
		return FALSE;

	// Make sure new objects are added as children of the node
	pNode = pGroup;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSFilter::EndGroup(BOOL Masked = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/08/94
	Inputs:		Masked - TRUE if this was a masked group;
						FALSE if it was a normal one.
	Returns:	TRUE if the group was ended ok;
				FALSE if not.
	Purpose:	Used when a group has finished being constructed and we want to return to
				normal node positioning.  The group is added to the document when this
				function is called (although that depending on the current filter mode, 
				i.e. whether this is a new or existing layer, it may not be added
				directly to the documnent tree straight away - it may be deferred until
				the next layer is found, or the import has ended).
	Errors:		Out of memory
	SeeAlso:	EPSFilter::StartGroup

********************************************************************************************/

BOOL EPSFilter::EndGroup(BOOL Masked)
{
	// Sanity check
	ENSURE(pNode->IsKindOf(CC_RUNTIME_CLASS(NodeGroup)), "No group in EPSFilter::EndGroup");
	
	//	Finishing the handling of the group, next we're not in a group anymore.
//	EPS_Group	=	FALSE;
	
	if(Masked)
	{
		// call the derived class
		if(!MaskedGroupEnding())
			return FALSE;			// error...
	}

	// Remember the group node
	Node *pGroup = pNode;

	// Get the parent of the group node, and use that to add new objects to
	pNode = pNode->FindParent();
	ENSURE(pNode != NULL, "Group has no parent in ProcessGroup()");

	// Check for bad groups, i.e. groups with no children
	Node *pChild = pGroup->FindFirstChild();
	if (pChild == NULL)
	{
		// if pFirstNodeInRange is this node, set it to zero
		if(pFirstNodeInRange == pGroup)
			pFirstNodeInRange = 0;

		// No children! Remove this group from the tree - we must hide it like this
		// and not delete it because it may already have been put in the undo buffer
		// via DoInsertNewNode(), and so we will end up with a pointer to a deleted item.
		// Hence we simply Hide it now and it all comes out in the wash if the user does
		// undo/redo.
		if (!ImportInfo.pOp->DoHideNode(pGroup, TRUE))
			return FALSE;
	} else {
		// it was OK - validate it...
		if(!ValidateGroup(pGroup))
			return FALSE;
	}

	// Keep group nesting up to date, and restore clipping region if necessary
	if (Masked)
	{
		MaskedGroupNesting--;

		if (!ClipRegion.RestoreContext())
			// Error in EPS
			return FALSE;
	}
	else
		GroupNesting--;

	return TRUE;
}


/********************************************************************************************

>	BOOL EPSFilter::ValidateGroup(Node *pGroup)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Inputs:		pGroup - the group to validate
	Returns:	error flag
	Purpose:	Validate a group. Can be overridden to, for example, ensure that groups have
				at least two children.
	SeeAlso:	FreeHandEPSFilter::ValidateGroup

********************************************************************************************/

BOOL EPSFilter::ValidateGroup(Node *pGroup)
{
	return TRUE;
}


/********************************************************************************************

>	BOOL EPSFilter::MaskedGroupEnding()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Inputs:		-
	Returns:	error in EPS flag
	Purpose:	Function to call when a masked group ends. Gives a chance to restore graphic
				states and the like.
	SeeAlso:	FreeHandEPSFilter::MaskedGroupEnding

********************************************************************************************/

BOOL EPSFilter::MaskedGroupEnding()
{
	EPS_Group = FALSE;
	return TRUE;
}


/********************************************************************************************

>	BOOL EPSFilter::AddAttributes(NodeRenderableBounded *pNode, BOOL Stroked, BOOL Filled)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Inputs:		pNode - the bounded renderable node to add atributes to.
				Stroked - TRUE if the object to add attributes to should be stroked, 
						 FALSE if not.
				Stroked - TRUE if the object to add attributes to should be filled, 
						 FALSE if not.
	Returns:	TRUE if the attributes were added ok, FALSE if not.
	Purpose:	Add attributes to an object being constructed.  The attributes 
				are optimised so that if they are the same as the document's default
				attributes, they are not applied.
	Errors:		Fails (returns FALSE) if not enough memory to add attributes.
	Scope:		Private

********************************************************************************************/

BOOL EPSFilter::AddAttributes(NodeRenderableBounded *pNode, BOOL Stroked, BOOL Filled)
{
	DocColour OldCol;

	// If not stroked, temporarily set the ignore bit on the line colour attribute.
	if (!Stroked)
	{
		StrokeColourAttribute *pAttr = 
			(StrokeColourAttribute *) CurrentAttrs[ATTR_STROKECOLOUR].pAttr;

		OldCol = pAttr->Colour;
		if (!SetLineColour(DocColour(COLOUR_TRANS)))
			return FALSE;
	}

	// If not filled, then set the ignore bit on the fill attribute.
	if (!Filled) //pPath->InkPath.IsFilled)
	{
		CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = TRUE;
	}

	// Add attributes to the path, if they are different from the default...
	BOOL Result = AttributeManager::ApplyBasedOnDefaults(pNode, CurrentAttrs);

	// If not stroked, reinstall old line colour.
	if (!Stroked)
		// This can't fail. (tempting fate or wot!)
		SetLineColour(OldCol);

	// Enable the fill attribute again
	CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = FALSE;

	// Re-enable path-filling, in case it was disabled for this object
	SetPathFilled(TRUE);

	// Return success or failure
	return Result;
}


/********************************************************************************************

>	void EPSFilter::GetEPSColour(DocColour *DestCol, 
								 PColourCMYK *pCMYK, 
								 TintType Tint, FIXEDPOINT TintVal, 
								 String_64 *ColName)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/94
	Inputs:		pCMYK - the colour components read from the EPS file.
				Tint - the type of tint used for this colour (i.e. specifies whether
						or not this is a named/tinted colour, and so on).
				TintVal - the percentage tint for a tinted colour. (ignored if no tint)
				ColName - the name of the colour. (may be NULL if no tint)
	Outputs:	DestCol - the resulting DocColour, which may be a reference to an indexed
						  colour.
	Purpose:	Used when an EPS colour definition has just been read off the stack.
				Provides a centralised way of interpreting the usual colour definitions
				and getting a valid DocColour out of it.  This copes with all sorts of
				things such as creating an indexed colour for named colours, and even
				handles creation of special colours for tinted named colours.
				Basically, it's a fire-and-forget function :-).
				If all else fails, it falls back to an immediate DocColour.
	SeeAlso:	EPSFilter::GetEPSColourRGB, ColourComponentList

********************************************************************************************/

void EPSFilter::GetEPSColour(DocColour *DestCol, 
							 PColourCMYK *pCMYK, 
							 TintType Tint, FIXEDPOINT TintVal, 
							 String_64 *ColName)
{
	IndexedColour *pIndexed = NULL;

	// Set up the colour with what we have.
	DestCol->SetCMYKValue(pCMYK);

	// Nasty hack time:
	// When saving immediate colours in a context where only named colours are expected,
	// e.g. when saving a grad fill that has an immediate colour, we use the name defined
	// by the constant 'ImmediateColourFudgeyBodgeName' (which is defined in epsfiltr.h)
	// to indicate this, so when we encounter this name, we don't treat it as an indexed
	// (named) colour - we treat it as an immediate colour). -- Tim
	BOOL IsNamed = ColName ? (ColName->CompareTo(ImmediateColourFudgeyBodgeName) != 0) : FALSE;

	if (((Tint == TINT_ILLUSTRATOR) || (Tint == TINT_COREL)) && IsNamed)
	{
		if (ColName->Length() > 0)
		{
			// Named colour - try to find it.
			pIndexed = pColours->FindNamedColour((TCHAR *) (*ColName), DestCol, 
												 (TintVal * 100) / FixedPointScale);

			if (IsUserName("Tim") && (pIndexed == NULL))
			{
				TRACE( _T("Unable to find named colour: %s\n"), (TCHAR *) (*ColName));
			}
		}
		else
		{
			// A zero-length colour name is how Camelot currently represents 
			// 'no colour' in grad fill colours, e.g. fade from red to no colour, i.e.
			// fade to 100% transparent.
			// In future we should have a proper named colour in Camelot that is 100%
			// transparent and we can use that.  Until then...
			DocColour TransCol(COLOUR_TRANS);
			(*DestCol) = TransCol;
		}
	}
	else
	{
		// Unnamed colour - make a new indexed colour for it, or look up a previous
		// definition if it exists.  We don't bother if the user has disabled loading
		// of unnamed colours.
		//
		// NB. The TRUE value for the 'Strict' parameter - we may well have defined
		// a colour called this already but with different values, so we always want
		// to check the colour definition as well as the name.
		if (Filter::AddUnnamedColours)
		{
			pIndexed = pColours->FindNamedColour(String_64(_R(IDS_K_EPSFILTER_UNNAMED)), DestCol, 100, TRUE);

			if (IsUserName("Tim") && (pIndexed == NULL))
			{
				TRACE( _T("Unable to construct an unnamed colour\n"));
			}
		}
	}

	if (pIndexed != NULL)
	{
		// Found the indexed colour, so use it.
		DestCol->MakeRefToIndexedColour(pIndexed);
	}

	// If no indexed colour made, we're stuck with an immediate colour...
}


/********************************************************************************************

>	void EPSFilter::GetEPSColourRGB (DocColour *DestCol,
									INT32 red, INT32 green, INT32 blue,
									TintType Tint, FIXEDPOINT TintVal,
									String_64 *ColName)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/00
	Inputs:		red, green, blue - the RGB colour components read from the EPS file.

				NOTE: Tints aren't currently supported by the RGB import - all they 
				really do is determine whether the colour is named.
				Tint - the type of tint used for this colour (i.e. specifies whether
						or not this is a named/tinted colour, and so on).
				TintVal - the percentage tint for a tinted colour. (ignored if no tint)
				ColName - the name of the colour. (may be NULL if no tint)
	Outputs:	DestCol - the resulting DocColour, which may be a reference to an indexed
						  colour.
	Purpose:	Identical to EPSFilter::GetEPSColour, except that it deals with RGB colours
				instead of CMYK ones.
	SeeAlso:	EPSFilter::GetEPSColour, ColourComponentList

********************************************************************************************/

void EPSFilter::GetEPSColourRGB (DocColour *DestCol, 
								INT32 red, INT32 green, INT32 blue, 
								 TintType Tint, FIXEDPOINT TintVal, 
								 String_64 *ColName)
{
	IndexedColour *pIndexed = NULL;

	// Set up the colour with what we have.
	DestCol->SetRGBValue(red, green, blue);

	// Nasty hack time:
	// When saving immediate colours in a context where only named colours are expected,
	// e.g. when saving a grad fill that has an immediate colour, we use the name defined
	// by the constant 'ImmediateColourFudgeyBodgeName' (which is defined in epsfiltr.h)
	// to indicate this, so when we encounter this name, we don't treat it as an indexed
	// (named) colour - we treat it as an immediate colour). -- Tim
	BOOL IsNamed = ColName ? (ColName->CompareTo(ImmediateColourFudgeyBodgeName) != 0) : FALSE;

	if (((Tint == TINT_ILLUSTRATOR) || (Tint == TINT_COREL)) && IsNamed)
	{
		if (ColName->Length() > 0)
		{
			// Named colour - try to find it.
			pIndexed = pColours->FindNamedColour((TCHAR *) (*ColName), DestCol, 
												 (TintVal * 100) / FixedPointScale);
		}
		else
		{
			// A zero-length colour name is how Camelot currently represents 
			// 'no colour' in grad fill colours, e.g. fade from red to no colour, i.e.
			// fade to 100% transparent.
			// In future we should have a proper named colour in Camelot that is 100%
			// transparent and we can use that.  Until then...
			DocColour TransCol(COLOUR_TRANS);
			(*DestCol) = TransCol;
		}
	}
	else
	{
		// Unnamed colour - make a new indexed colour for it, or look up a previous
		// definition if it exists.  We don't bother if the user has disabled loading
		// of unnamed colours.
		//
		// NB. The TRUE value for the 'Strict' parameter - we may well have defined
		// a colour called this already but with different values, so we always want
		// to check the colour definition as well as the name.
		if (Filter::AddUnnamedColours)
		{
			pIndexed = pColours->FindNamedColour(String_64(_R(IDS_K_EPSFILTER_UNNAMED)), DestCol, 100, TRUE);
		}
	}

	if (pIndexed != NULL)
	{
		// Found the indexed colour, so use it.
		DestCol->MakeRefToIndexedColour(pIndexed);
	}

	// If no indexed colour made, we're stuck with an immediate colour...
}

/********************************************************************************************

>	BOOL EPSFilter::ProcessToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Returns:	TRUE if the token was processed ok, FALSE if an error occured.
	Purpose:	Examines the current token and performs whatever actions are
				necessary to process its meaning.  Operands are simply pushed onto
				the EPS stack, and keywords are either acted upon, or ignored if
				they describe something that Camelot does not yet support (in the
				latter case, the operands of the keyword, if any, are simply discarded
				from the stack).
	Errors:		Fails (returns FALSE) if a syntax/error is detected in the EPS file, or if
				there is insufficient memory to complete the operation.
	SeeAlso:    EPSFilter::GetToken
	Scope:		Private

********************************************************************************************/
	
BOOL EPSFilter::ProcessToken()
{
	// Variables used to extract operands from the stack
	DocCoord 	Coords[3];
	PColourCMYK	Col;
	DocColour	DocCol;
	double 		Double;
	DocRect 	BoundingRect;
	PathFlags 	Flags;
	TintType	Tint = TINT_NONE;
	FIXEDPOINT	TintVal;
	LineCapType	LineCap;
	JointType	JoinType;
	INT32 		ArraySize;
	INT32 		Array[8];
	DashRec		Dash;
	INT32		Long;
	String_64	ColName;

	// Decode the command, and execute it...
	switch (Token)
	{
		case EPSC_showpage:
			// No action required
			break;

		case EPSC_end:
			// No action required
			break;

		// Graphics state operators
		case EPSC_A:
			TRACE(_T("Ignoring graphics state token\n"));
			if (!Stack.Discard())
				goto EPSError;
			break;

		case EPSC_i:
			TRACE(_T("Ignoring token i\n"));
			if (!Stack.Discard())
				goto EPSError;
			break;

		case EPSC_D:
			TRACE(_T("Ignoring token D\n"));
			if (!Stack.Discard())
				goto EPSError;
			break;

		case EPSC_M:
			// Set current mitre limit (defined in points)
			if (Stack.Pop(&Double))
			{
				// Scale mitre limit from points to millipoints, and remember for future
				// objects.
				if (!SetMitreLimit((MILLIPOINT) (Double * EPSScaleFactor)))
					goto NoMemory;
			}
		    else	
				goto EPSError;
			break;


		// Group related procedures
		
		// Start of group
		case EPSC_u:
			GroupNesting++;
			return ProcessGroup();
			
		// End of group
		case EPSC_U:
			// We're not in a group at the moment - syntax error in EPS
			goto EPSError;
			
		
		// Colour operators
		case EPSC_g:
			if (Stack.PopGrayScale(&Col))
			{
				// Remember this fill colour for future objects
				DocCol.SetCMYKValue(&Col);
				if (!SetFillColour(DocCol))
					goto NoMemory;
			}
		    else
		    	// Invalid colour operands
				goto EPSError;
			break;

		case EPSC_G:
			if (Stack.PopGrayScale(&Col))
			{
				// Remember this line colour for future objects
				DocCol.SetCMYKValue(&Col);
				if (!SetLineColour(DocCol))
					goto NoMemory;
			}
		    else
		    	// Invalid colour operands
				goto EPSError;
			break;

		case EPSC_p:
		case EPSC_P:
			// pattern
			TRACE(_T("Ignoring token p(P)\n"));
			if ( !Stack.DiscardArray() || !Stack.Discard(10) )
				goto EPSError;
			break;


		case EPSC_x:
		case EPSC_X:
			Tint = TINT_ILLUSTRATOR;
		case EPSC_k:
		case EPSC_K:
			// Set current line/fill colour (CMYK)
			if (Stack.PopColour(&Col, Tint, &TintVal, &ColName))
			{
				GetEPSColour(&DocCol, &Col, Tint, TintVal, &ColName);

				// Remember this colour for future objects
				if ((Token == EPSC_x) || (Token == EPSC_k))
				{
			    	if (!SetFillColour(DocCol))
						goto NoMemory;
				}
				else
				{
					if (!SetLineColour(DocCol))
						goto NoMemory;
				}
			}
		    else	
		    	// Invalid colour operands
				goto EPSError;
			break;

		
		// Overprint related procedures
		case EPSC_O:

			TRACE(_T("Ignoring token O\n"));
			if (!Stack.Discard())
				goto EPSError;
			break;
			
		case EPSC_R:

			TRACE(_T("Ignoring token R\n"));
			if (!Stack.Discard())
				goto EPSError;
			break;
			
		case EPSC_w:
			// Set current line width (defined in points)
			if (Stack.Pop(&Double))
			{
				// Scale line width from points to millipoints, and remember for future
				// objects.
				if (!SetLineWidth((MILLIPOINT) (Double * EPSScaleFactor)))
					goto NoMemory;
			}
		    else	
				goto EPSError;
			break;
			
		case EPSC_j:
			// Line join style
			if (!Stack.Pop(&Long))
				goto EPSError;

			// Decode line join style
			switch (Long)
			{
				case 0: 	JoinType = MitreJoin; 		break;
				case 1: 	JoinType = RoundJoin;		break;
				case 2: 	JoinType = BevelledJoin; 	break;
				// Unknown join style.
				default: 	goto EPSError;
			}

			// Try to set this line join style
			if (!SetJoinType(JoinType))
				goto NoMemory;
			break;
			
		case EPSC_J:
			// Line cap style
			if (!Stack.Pop(&Long))
				goto EPSError;

			// Decode line cap style
			switch (Long)
			{
				case 0: 	LineCap = LineCapButt; 		break;
				case 1: 	LineCap = LineCapRound;		break;
				case 2: 	LineCap = LineCapSquare; 	break;
				// Unknown cap style.
				default: 	goto EPSError;
			}

			// Try to set this line cap style
			if (!SetLineCap(LineCap))
				goto NoMemory;
			break;
			
		case EPSC_d:
			// Read a DashPattern
			if (!Stack.Pop(&Double))
				goto EPSError;

			TRACEUSER( "Will", _T("Importing dash pattern\n"));
			
			// Start offset into the Dash array
			Dash.DashStart = (MILLIPOINT) (Double * EPSScaleFactor);

			// Now get the actual dash elements
			ArraySize = 8;	// Max number of elements
			if (!Stack.PopArray(Array, &ArraySize))
				goto EPSError;

			Dash.Elements = ArraySize;
			Dash.ElementData = Array;

			// We've lost the scaling information, so we won't try to
			// scale this Dash Pattern.
			Dash.ScaleWithLineWidth = FALSE;

			// Now set the Dash Pattern
			if (!SetDashPattern(Dash))
				goto NoMemory;
			break;
			
		//
		// Procedures that define a path object
		//
		
		// MoveTo
		case EPSC_m:
			// Get the co-ordinate from the stack
			if (Stack.PopCoordPair(&Coords[0]))
			{
				if ((ThePathType != PATH_DISCARD) && (ThePathType != PATH_DISCARD_STICKY))
				{
					if ((pPath == NULL) && (pInkPath == NULL))
					{
						// We need a new NodePath because there isn't an InkPath around to use.

						// Create a new NodePath object
						switch (ThePathType)
						{
							case PATH_NORMAL:
								pPath = new NodePath;
								break;
					
							case PATH_RECT:
								pPath = (NodePath *) new NodeRect;
								break;
					
							case PATH_ELLIPSE:
								pPath = (NodePath *) new NodeEllipse;
								break;

							case PATH_MANGLED:
								pPath = new NodePath;
								if (pPath)
									pPath->SetMangled(TRUE);
								break;
							
							default:
								ENSURE(FALSE, "Unknown path type in EPS filter!");
								goto EPSError;
						}

						// No attributes on the path yet
						EPSFlags.NoAttributes = TRUE;
					
						if ((pPath == NULL) || (!pPath->SetUpPath()))
							// Not enough memory to initialise path
							goto NoMemory;
					
						// Position tag at start of path.
						pPath->InkPath.FindStartOfPath();

						// Point inkpath pointer at the nodepath's path object.
						pInkPath = &pPath->InkPath;
					}

					else if (EPSFlags.PathIsHidden)
					{
						// The last path was handled by an 'H' or 'h' operator, so
						// it was not deleted, so we delete it now.
						// (This is so we cope with incorrect ArtWorks EPS!)
						pInkPath->ClearPath();
					}

					// Error handling for "else if (EPSFlags.ComplexPath == 0)" to
					// go in here if necessary. The program seems to work better
					// without it.
					
					// Insert a moveto into the path
					if (!pInkPath->InsertMoveTo(Coords[0]))
					{
						// Not enough dynamic heap to insert the moveto command
						goto NoMemory;
					}
				}
			}
			else
				// Invalid number/type of coordinate operands
				goto EPSError;
			break;

		// LineTo			
		case EPSC_l:
		case EPSC_L:
			// Get the co-ordinate from the stack
			if (Stack.PopCoordPair(&Coords[0]))
			{
				if ((ThePathType != PATH_DISCARD) && (ThePathType != PATH_DISCARD_STICKY))
				{
					if (pInkPath == NULL)
						// Paths must start with a moveto
						goto EPSError;
				
					if (Token == EPSC_l)
						Flags.IsSmooth = TRUE;
					if (!pInkPath->InsertLineTo(Coords[0], &Flags))
						// Not enough dynamic heap to insert the lineto command
						goto NoMemory;
				}
			}
			else
				// Invalid number/type of coordinate operands
				goto EPSError;
			break;

		// Curveto			
		case EPSC_c:
		case EPSC_C:
			// Get the co-ordinate from the stack
			if (Stack.PopCoordPair(&Coords[2]) &&
			    Stack.PopCoordPair(&Coords[1]) &&
			    Stack.PopCoordPair(&Coords[0]))
			{
				if ((ThePathType != PATH_DISCARD) && (ThePathType != PATH_DISCARD_STICKY))
				{
					if (pInkPath == NULL)
						// Paths must start with a moveto
						goto EPSError;
				
					if (Token == EPSC_c)
						Flags.IsSmooth = TRUE;
					if (!pInkPath->InsertCurveTo(Coords[0], Coords[1], Coords[2], &Flags))
						// Not enough dynamic heap to insert the curveto command
						goto NoMemory;
				}
			}
			else
				// Invalid number/type of coordinate operands
				goto EPSError;
			break;
			
		// This curve token only has two coordinate pairs - the first
		// is implicitly the previous coordinate
		case EPSC_v:
		case EPSC_V:
			// Get the co-ordinate from the stack
			if (Stack.PopCoordPair(&Coords[2]) &&
			    Stack.PopCoordPair(&Coords[1]))
			{
				if ((ThePathType != PATH_DISCARD) && (ThePathType != PATH_DISCARD_STICKY))
				{
					if (pInkPath == NULL)
						// Paths must start with a moveto
						goto EPSError;
				
					if (Token == EPSC_v)
						Flags.IsSmooth = TRUE;
					
					// Find out what the previous coordinate was:
					INT32 index = pInkPath->GetNumCoords();
					DocCoord* pCoord = pInkPath->GetCoordArray();
					Coords[0] = pCoord[index-1];
					if (!pInkPath->InsertCurveTo(Coords[0], Coords[1], Coords[2], &Flags))
						// Not enough dynamic heap to insert the curveto command
						goto NoMemory;
				}
			}
			else
				// Invalid number/type of coordinate operands
				goto EPSError;
			break;
			
		case EPSC_y:
		case EPSC_Y:
			// Get the co-ordinate from the stack
			if (Stack.PopCoordPair(&Coords[2]) &&
			    Stack.PopCoordPair(&Coords[0]))
			{
				if ((ThePathType != PATH_DISCARD) && (ThePathType != PATH_DISCARD_STICKY))
				{
					if (pInkPath == NULL)
						// Paths must start with a moveto
						goto EPSError;
				
					if (Token == EPSC_y)
						Flags.IsSmooth = TRUE;
					if (!pInkPath->InsertCurveTo(Coords[0], Coords[2], Coords[2], &Flags))
						// Not enough dynamic heap to insert the curveto command
						goto NoMemory;
				}
			}
			else
				// Invalid number/type of coordinate operands
				goto EPSError;
			break;

		case EPSC_S:
		case EPSC_b:
		case EPSC_B:
		case EPSC_f:
		case EPSC_F:
		case EPSC_s:
			// Process the end of path directive.
			if ( !ProcessEndOfPath () )
				goto NoMemory;
			break;
			
		// Hidden path - not stroked or filled, or used up.
		case EPSC_h:
		case EPSC_H:
			// Mark as a hidden flag to get around ArtWorks EPS bug.
			EPSFlags.PathIsHidden = TRUE;

			// Terminate path
			if (Token == EPSC_h)
			{
				if (!pPath->InkPath.CloseSubPath())
					// Not enough dynamic heap to insert the final element of the path
					goto NoMemory;

				// This path is closed - set the filled flag.
				pPath->InkPath.IsFilled = TRUE;
			}
			break;

		// Path is not stroked or filled, but it is used up.
		case EPSC_n:
		case EPSC_N:
			// Process the end of path directive.
			if ( !ProcessUnfilledPath () )
				goto NoMemory;
			break;
			
		case EPSC__u:  // *u
			if ((EPSFlags.ComplexPath != 0) && !EPS_Group)
				// Can't start a complex path if one is still being constructed.
				//	Apart in a group where we're actually dealing with a clipping!
				goto EPSError;

			if(EPS_Group && (EPSFlags.ComplexPath == 0))
				ClipRegion.StartComplexClipRegion();
	
			// Set the flag so the moveto routine knows that a complex path is being
			// created.				

			EPSFlags.ComplexPath++;
			

			// Default to not stroking paths.
			EPSFlags.StrokeComplexPath = FALSE;
			break;
			
		case EPSC__U:  // *U
			if (EPSFlags.ComplexPath == 0)
				// Can't end a complex path if we're not currently constructing one
				goto EPSError;
			
			//	EPSFlags.ComplexPath = FALSE;
			EPSFlags.ComplexPath--;
			if(EPSFlags.ComplexPath <= 0)
			{
				EPSFlags.ComplexPath = 0;

				if(EPS_Group)
					ClipRegion.EndComplexClipRegion();

				if ((ThePathType != PATH_DISCARD) && (ThePathType != PATH_DISCARD_STICKY))
				{
					if (pPath == NULL)
						// Paths must start with a moveto
						goto EPSError;
	
					// Make sure this path is valid
					if (!pPath->InkPath.EnsureValid())
					{
						// Invalid path - throw it away
						delete pPath;
						InvalidPathFound = TRUE;
					}
					else
					{
						// First, we clip if necessary
						if (!ClipRegion.ClipPathToRegion(&pPath->InkPath))
							// Error!
							goto NoMemory;
	
						// Make sure bounding box is up to date.	
						pPath->InvalidateBoundingRect();
		
						// Add the attributes, if we haven't done so for this path
						if (EPSFlags.NoAttributes)
						{
							if (!AddAttributes(pPath, EPSFlags.StrokeComplexPath, pPath->InkPath.IsFilled))
								goto NoMemory;
							EPSFlags.NoAttributes = FALSE;
						}
		
						// Add this path into the tree
						if (!AddNewNode(pPath))
								goto NoMemory;
					}
			
					// We've finished building this path - set to NULL so the other routines
					// know that we're not building a path any more.
					pPath = NULL;
					pInkPath = NULL;
				}
			}

			break;


		// Clipping operators

		case EPSC_q:
			// Starting a clipped (masked) group - save the clip context before we start.
			ClipRegion.SaveContext();

			//	And keep in mind we're clipping!
			EPS_Group	=	TRUE;

			// Start decoding the masked group...
			MaskedGroupNesting++;
			return (ProcessGroup());
			
		case EPSC_Q:
			// We're not in a masked group at the moment - syntax error in EPS
			goto EPSError;

		case EPSC_W:
			// Use this path to augment the clipping region...
			if (pInkPath == NULL)
				// No path to use!
				goto EPSError;

			if (!ClipRegion.AddNewClippingPath(pInkPath))
				// Error!
				return FALSE;

			break;
			

		// Text operators
		case EPSC_To:
			return ProcessText();
			break;
			
		case EPSC_TO:
			TRACE(_T("Ignoring token TO\n"));
			break;
			
		case EPSC_Tp:
			TRACE(_T("Ignoring token Tp\n"));
			if (!Stack.Discard(7))
				goto EPSError;
			break;
			
		case EPSC_TP:
			TRACE(_T("Ignoring token TP\n"));
			break;
			
		case EPSC_Tm:
			TRACE(_T("Ignoring token Tm\n"));
			if (!Stack.Discard(6))
				goto EPSError;
			break;
			
		case EPSC_Td:
			TRACE(_T("Ignoring token Td\n"));
			if (!Stack.Discard(2))
				goto EPSError;
			break;
			
		case EPSC_T_:
			TRACE(_T("Ignoring token T\n"));
			break;
			
		case EPSC_TR:
			TRACE(_T("Ignoring token TR\n"));
			if (!Stack.Discard(8))
				goto EPSError;
			break;

		// Text Attribute operators
		case EPSC_Tr:
		case EPSC_Ta:
		case EPSC_Tt:
		case EPSC_Tw:
		case EPSC_Tc:
		case EPSC_Ts:
		case EPSC_Tz:
		case EPSC_TA:
		case EPSC_Tq:
			TRACE(_T("Ignoring text attribute token\n"));
			if (!Stack.Discard())
				goto EPSError;
			break;

		case EPSC_Tf:
		case EPSC_Tl:
			TRACE(_T("Ignoring token Tf/Tl\n"));
			if (!Stack.Discard(2))
				goto EPSError;
			break;

		case EPSC_TW:
		case EPSC_TC:
		case EPSC_Ti:
			TRACE(_T("Ignoring text token\n"));
			if (!Stack.Discard(3))
				goto EPSError;
			break;


		// Text Body operators
		case EPSC_Tx:
		case EPSC_Tj:
		case EPSC_TX:
			TRACE(_T("Ignoring text body tokens\n"));
			if (!Stack.Discard())
				goto EPSError;
			break;
			
		case EPSC_Tk:
		case EPSC_TK:
			TRACE(_T("Ignoring token Tk(K)\n"));
			if (!Stack.Discard(2))
				goto EPSError;
			break;
			
		case EPSC_Tplus:
		case EPSC_Tminus:
			TRACE(_T("Ignoring token T+/T-\n"));
			break;
			
		
		// Special tokens
		case EPSC_Name:
			return Stack.Push(TokenBuf, TRUE);
			
		case EPSC_ArrayStart:
		case EPSC_ArrayEnd:
			return Stack.Push(Token);
		
		case EPSC_Slash:
			// Don't care about literal names
			break;
				
		case EPSC_Integer:
			return Stack.Push(TokenData.Long);
			
		case EPSC_Double:
			return Stack.Push(TokenData.Double);
			
		case EPSC_FixedPoint:
			return Stack.Push(TokenData.FixedPoint);
			
		case EPSC_String:
			return Stack.Push(TokenBuf);
			
		case EPSC_Comment:
			return ProcessComment();
			
		default:
			// Token not understood
			goto EPSError;
	}


	// No errors encountered while parsing this token and its operands.
	return TRUE;
	
	
	// Error handlers:
	
EPSError:
	HandleEPSError();
	return FALSE;
	
NoMemory:
	HandleNoMemory();
	return FALSE;
	// Error - Out of memory while reading EPS file
	
}

/********************************************************************************************

>	BOOL EPSFilter::ProcessEndOfPath ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com> (From Tim's code.)
	Created:	08/04/94
	Inputs:		-
	Returns:	TRUE	- The path was terminated correctly.
				FALSE	- Something went wrong.
	Purpose:	Processes the end-of-path directive.

********************************************************************************************/

BOOL EPSFilter::ProcessEndOfPath ( void )
{
	// Not a 'hidden' path, so mark it as such.
	EPSFlags.PathIsHidden = FALSE;

	if (ThePathType == PATH_DISCARD)
	{
		// Discard this path, but try to use the next one.
		ThePathType = PATH_NORMAL;
		return TRUE;
	}
	
	if (ThePathType == PATH_DISCARD_STICKY)
		// Keep discarding paths until explicitly told not to.
		return TRUE;

	ENSURE((ThePathType == PATH_NORMAL) || 
		   (ThePathType == PATH_RECT) || 
		   (ThePathType == PATH_ELLIPSE) ||
		   (ThePathType == PATH_MANGLED), "No path in stroke!");
				
	// If no path being constructed, probably just some strange masking going on
	// that we can't cope with yet, so ignore it.
	if (pPath == NULL)
		return TRUE;

	// Terminate path
	if ((Token == EPSC_s) || (Token == EPSC_b) || (Token == EPSC_f))
	{
		if (!pPath->InkPath.CloseSubPath())
			// Not enough dynamic heap to insert the final element of the path
			return FALSE;

		// This path is closed - set the filled flag.
		pPath->InkPath.IsFilled = TRUE;
	}
					
	// Make the path filled or not, depending on the EPS token
	if ((Token == EPSC_b) || 
		(Token == EPSC_B) || 
		(Token == EPSC_F) || 
		(Token == EPSC_f))
	{
		// Path is filled.
		pPath->InkPath.IsFilled = TRUE;
		SetPathFilled(TRUE);
	}
	else
	{
		SetPathFilled(FALSE);
	}

	// Do any extra things we need to complete the construction of
	// the pPath object

	// Do any extra things we need to complete the construction of
	// the pPath object
	BOOL InsertQuickShape = FALSE;
	NodeRegularShape* pQuickShape = NULL;

	switch (ThePathType)
	{
		case PATH_RECT:
			pQuickShape = new NodeRegularShape;
			if (pQuickShape != NULL)
			{
				if (ProcessRectangle((NodeRect*)pPath, pQuickShape))
					InsertQuickShape = TRUE;
				else
				{
					// It's not really a rectangle after all - insert it as a path
					delete pQuickShape;
					NodeRect* OldRect = (NodeRect*)pPath;
					pPath = new NodePath();
					if ((pPath == NULL) || !pPath->InkPath.Initialise(OldRect->InkPath.GetNumCoords()))
						return TRUE;
					pPath->InkPath.CopyPathDataFrom(&OldRect->InkPath);
					delete OldRect;
				}
			}
			else
			{
				return FALSE;
			}
			break;

		case PATH_ELLIPSE:
			pQuickShape = new NodeRegularShape;
			if (pQuickShape != NULL)
			{
				if (ProcessEllipse((NodeEllipse*)pPath, pQuickShape))
					InsertQuickShape = TRUE;
				else
				{
					// It's not really an ellipse after all - insert it as a path
					delete pQuickShape;
					NodeEllipse* OldElip = (NodeEllipse*)pPath;
					pPath = new NodePath();
					if ((pPath == NULL) || !pPath->InkPath.Initialise(OldElip->InkPath.GetNumCoords()))
						return FALSE;
					pPath->InkPath.CopyPathDataFrom(&OldElip->InkPath);
					delete OldElip;
				}
			}
			else
			{
				return FALSE;
			}
			break;
	}

	if (EPSFlags.ComplexPath == 0)
	{
		// Not a complex path, so we terminate properly...
		if (InsertQuickShape)
		{
			// Make sure the bbox is updated...
			pQuickShape->InvalidateBoundingRect();
			pQuickShape->InvalidateCache();
					
			// Add the attributes, if we haven't done so for this path
			if (EPSFlags.NoAttributes)
			{
				if (!AddAttributes(pQuickShape, (Token != EPSC_f) && (Token != EPSC_F), pPath->InkPath.IsFilled))
					return FALSE;

				EPSFlags.NoAttributes = FALSE;
			}

			delete pPath;	// Delete the old rect/ellipse;
		
			// Add this path into the tree
			if (!AddNewNode(pQuickShape))
				return FALSE;
		}
		else
		{
			// Make sure this path is valid
			if (!pPath->InkPath.EnsureValid())
			{
				// No - throw it away.
				delete pPath;
				InvalidPathFound = TRUE;
			}
			else
			{
				// It's valid so we finish normally.

				// First, we clip if necessary
				if (!ClipRegion.ClipPathToRegion(&pPath->InkPath))
					// Error!
					return FALSE;

				// (Chris G 16/11/00) - Fix to make sure we don't attempt to write a path 
				//	that's been clipped into oblivion. i.e. has no points left.
				if (pPath->InkPath.EnsureValid ())
				{
					// Make sure the bbox is updated...
					pPath->InvalidateBoundingRect();
						
					// Add the attributes, if we haven't done so for this path
					if (EPSFlags.NoAttributes)
					{
						if (!AddAttributes(pPath, (Token != EPSC_f) && (Token != EPSC_F), pPath->InkPath.IsFilled))
							return FALSE;
						EPSFlags.NoAttributes = FALSE;
					}
			
					// Add this path into the tree
					if (!AddNewNode(pPath))
						return FALSE;
				}
				else
				{
					delete pPath;
					InvalidPathFound = TRUE;
				}
			}
		}

		// We've finished building this path - set to NULL so the other routines
		// know that we're not building a path any more.
		pPath = NULL;
		pInkPath = NULL;
	}
	else
		EPSFlags.StrokeComplexPath = (Token != EPSC_f) && (Token != EPSC_F);

	// Handle the next path normally
	ThePathType = PATH_NORMAL;

	// It worked!
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSFilter::ProcessUnfilledPath ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com> (From Tim's code.)
	Created:	08/04/94
	Inputs:		-
	Returns:	TRUE	- The path was terminated correctly.
				FALSE	- Something went wrong.
	Purpose:	Processes the end-of-path directive of an unfilled or unstroked path.

********************************************************************************************/

BOOL EPSFilter::ProcessUnfilledPath ( void )
{
	// Not a 'hidden' path, so mark it as such.
	EPSFlags.PathIsHidden = FALSE;

	if (ThePathType == PATH_DISCARD)
	{
		// Discard this path, but try to use the next one.
		ThePathType = PATH_NORMAL;
		return TRUE;
	}
	
	if (ThePathType == PATH_DISCARD_STICKY)
		// Keep discarding paths until explicitly told not to.
		return TRUE;

	ENSURE((ThePathType == PATH_NORMAL) || 
		   (ThePathType == PATH_RECT) || 
		   (ThePathType == PATH_ELLIPSE) ||
		   (ThePathType == PATH_MANGLED), "No path in hide!");
	// If no path being constructed, probably just some strange masking going on
	// that we can't cope with yet, so ignore it.
	if (pPath == NULL)
		return TRUE;

	// Terminate path
	if ((Token == EPSC_h) || (Token == EPSC_n))
	{
		if (!pPath->InkPath.CloseSubPath())
			// Not enough dynamic heap to insert the final element of the path
			return FALSE;

		// This path is closed - set the filled flag.
		pPath->InkPath.IsFilled = TRUE;
	}
					
	// Make the path not filled.
	SetPathFilled(FALSE);
		
	// Add the attributes, if we haven't done so for this path
	if (EPSFlags.NoAttributes)
	{
		// Add attributes (FALSE => not stroked)
		if (!AddAttributes(pPath, FALSE, pPath->InkPath.IsFilled))
			return FALSE;
		EPSFlags.NoAttributes = FALSE;
	}
		
	// Do the wierdness required for NodeRectangles
	if (ThePathType == PATH_RECT)
	{
		// Make sure we have a semi-valid path for a rectangle
		NodeRect *pRect = (NodeRect *) pPath;
		ENSURE(pRect->InkPath.GetNumCoords() == 5, 
			   "Bad rectangle path in EPS import!");

		// Copy the path coordinates into the parallelogram
		DocCoord *Points = pRect->InkPath.GetCoordArray();
		for (INT32 i = 0; i <= 3; i++)
			pRect->Parallel[i] = Points[i];
	}
	else if (ThePathType == PATH_ELLIPSE)
	{
		// Make sure we have a semi-valid path for an ellipse
		NodeEllipse *pEllipse = (NodeEllipse *) pPath;
		ENSURE(pEllipse->InkPath.GetNumCoords() == 13, 
			   "Bad ellipse path in EPS import!");

		// Copy the path coordinates into the parallelogram
		for (INT32 i = 0; i <= 3; i++)
			pEllipse->Parallel[i] = ShapeBBox[i];
	}

	if (EPSFlags.ComplexPath == 0)
	{
		// Not a complex path, so we terminate properly.

		// Make sure this path is valid
		if (!pPath->InkPath.EnsureValid())
		{
			// Invalid path - discard it
			delete pPath;
			InvalidPathFound = TRUE;
		}
		else
		{
			// Path is ok so carry on.
			pPath->InvalidateBoundingRect();
		
			// Add this path into the tree
			if (!AddNewNode(pPath))
				return FALSE;
		}

		// We've finished building this path - set to NULL so the other routines
		// know that we're not building a path any more.
		pPath = NULL;
		pInkPath = NULL;
	}
	else
		EPSFlags.StrokeComplexPath = FALSE;

	// Handle the next path normally
	ThePathType = PATH_NORMAL;

	// Success!
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSFilter::ProcessComment ( BOOL BypassDocComponents = FALSE )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/94
	Inputs:		BypassDocComponents - TRUE => don't pass this component to all the
											  document components for processing.
									 FALSE => pass this comment around to all the
											  doc components to see if they handle it.
	Returns:	TRUE if the comment was processed ok, FALSE if not.
	Purpose:	Decodes an EPS comment - usually starting with "%%", e.g. %%BeginProlog etc.

********************************************************************************************/

BOOL EPSFilter::ProcessComment(BOOL BypassDocComponents)
{
	if ((_tcsncmp(TokenBuf, "%%Trailer", 9) == 0) ||
		(_tcsncmp(TokenBuf, "%%PageTrailer", 13) == 0))
	{
		ProcessEPSTrailer();
	}
	else if (_tcsncmp(TokenBuf, "%%BoundingBox:", 14) == 0)
	{
		ProcessBoundingBox();
	}
	else
	{
		// Look for our known EPS/DSC comments
		INT32				Index			= 0;
		BOOL			FoundComment	= FALSE;
		EPSCommentDef	Comments;

		for(;;)
		{
			Comments = EPSComments.ReturnElement ( Index );
			INT32 StartLen = cc_strlenCharacters ( Comments.StartMarker );

			if (StartLen == 0)
			{
				// No more comments to try
				break;
			}

			if (_tcsncmp(TokenBuf, Comments.StartMarker, StartLen) == 0)
			{
				FoundComment = TRUE;
				break;
			}

			// Try the next comment
			Index++;
		}

		if (FoundComment)
		{
			// Look for the end of this comment block.
			INT32 EndLen = cc_strlenCharacters(Comments.EndMarker);

			do
			{
				GetLineToken();

				if (Token == EPSC_Comment)
				{
					if (_tcsncmp(TokenBuf, Comments.EndMarker, EndLen) == 0)
					{
						// Found the end of the comment block
						break;
					}

					// Special case for brain-damaged formats that don't bother to
					// bracket prolog segments correctly
					if ((_tcsncmp(TokenBuf, "%%EndProlog", 11) == 0) ||
						(_tcsncmp(TokenBuf, "%%EndSetup", 10) == 0))
					{
						// Found the end of the prolog/setup - this means we're not in the
						// prolog or setup processing loop (otherwise the above test
						// would have caught it) - it must be the next loop out.
						// So we put the token back onto the stream for the outer loop to
						// find and recognise, and quit our loop.
						EPSFile->UngetToken();
						break;
					}


					// This is a comment - should we decode it?
					if (Comments.ProcessContents)
					{
						// Yes - first try the filter itself, and if that doesn't
						// need it, then try the document components.
						ProcessEPSComment();
					}
					else
					{
						// No - recurse to handle nested comment blocks, and make sure it
						// isn't passed to the doc components.
						ProcessComment(TRUE);
					}
				}

				if (EPSFlags.EPSErrorEncountered)
					return FALSE;
			}
			while (!EPSFile->eof());
		}
		else if (!BypassDocComponents)
		{
			// We're all out of ideas, so let the document components have a bash
			ProcessEPSComment();
		}
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL EPSFilter::ProcessFilterComment()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/94
	Returns:	TRUE if the comment is used by the filter;
				FALSE if not.
	Purpose:	Override this function to claim EPS comments for the filter to decode.
				This function is called after the usual comments are checked for such as
				%%BeginProlog etc, but *before* the doc components are called.
				If you want to claim the current token (i.e. as held in TokenBuf), return
				TRUE.
	SeeAlso:	EPSFilter::ProcessEPSComment

********************************************************************************************/

BOOL EPSFilter::ProcessFilterComment()
{
	// This is the base class - there are no EPS comments specific to this filter.
	if (_tcsncmp(TokenBuf, "%%XSScript", 10) == 0)
	{
		TextComment[0]=2;
		return TRUE;
	}
	return FALSE;
}


/********************************************************************************************

>	BOOL EPSFilter::ProcessEPSComment()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com> & Rik
	Created:	9/2/95
	Returns:	TRUE if the comment was recognised
	Purpose:	Tries to process the contents of a comment. First it asks the filter and
				then it tries the document and all the doc components to see if it is
				one of thiers.

********************************************************************************************/

BOOL EPSFilter::ProcessEPSComment()
{
	// First try the filter to see if it understands this comment...
	if (ProcessFilterComment())
		// Yes - we're done.
		return TRUE;

	// We will start off by assuming that there are no doc components
	DocComponent *pComponent = NULL;
	ProcessEPSResult Result = EPSCommentUnknown;

	// ask the document what it thinks about the comment
	Result = TheDocument->ProcessEPSComment(this, TokenBuf);

	// If the document did not like the comment, let the doc components have a go
	if (Result == EPSCommentUnknown)
		pComponent = TheDocument->EnumerateDocComponents(NULL);

	while (pComponent != NULL)
	{
		// Does this document component want this comment?
		Result = pComponent->ProcessEPSComment(this, TokenBuf);
		
		if (Result != EPSCommentUnknown)
			// The component recognised the EPS comment
			break;

		// Otherwise, not recognised - try the next document component.
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}

	// Work out if the comment was known to the doc-component - if the comment is
	// not known to any of them, we return FALSE to let the filter proces it.
	BOOL Recognised = Result != EPSCommentUnknown;

	// There may be continuation comments, so check for them.
	do
	{
		// Get the next token
		GetToken();

		if ((Token != EPSC_Comment) || (_tcsncmp(TokenBuf, "%%+", 3) != 0))
		{
			// Not a continuation comment - so we're finished
			if (Result == EPSCommentOK)
			{
				// Tell the document component that this comment has finished.
				if (pComponent==NULL)
					TheDocument->EndOfEPSComment(this);
				else
					pComponent->EndOfEPSComment(this);
			}

			// Put this token back onto the stream as we don't want to use it
			EPSFile->UngetToken();

			break;
		}

		// Found a continuation comment - work out what to do with it
		if (Result == EPSCommentOK)
		{
			// The component understands this comment, so pass it on
			if (pComponent==NULL)
				Result = TheDocument->ProcessEPSComment(this, TokenBuf);
			else
				Result = pComponent->ProcessEPSComment(this, TokenBuf);
		}
		else
		{
			// Either the comment was not understood by any of the document components,
			// or it contained an error - either way, discard the rest of this comment.
			// i.e. ignore it
		}
	} while (!EPSFile->eof());

	// Tell the caller whether or not we understood this comment.
	return (Recognised);
}


/********************************************************************************************

>	BOOL EPSFilter::ProcessEPSTrailer()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95
	Returns:	TRUE	- Success!
				FALSE	- An error occured.
	Purpose:	Handles the end of the page (I think)

********************************************************************************************/

BOOL EPSFilter::ProcessEPSTrailer()
{
	BOOL Result = FALSE;

	// Found the end of a page, skip lines until another DSC comment is found.
	// This is so we skip the optional procset tidyup code that Illustrator
	// puts in.
	do
	{
		GetToken();

		// Is it a DSC comment?
		if ((Token == EPSC_Comment) && (TokenBuf[1] == '%'))
		{	// Yes - quit the loop - this is the end of the trailer.
			Result = TRUE;
			break;
		}

	} while (!EPSFile->eof());

	// Put this comment back onto the input stream so it is processed properly.
	EPSFile->UngetToken();

	return Result;
}


/********************************************************************************************

>	BOOL EPSFilter::ProcessBoundingBox()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95
	Returns:	TRUE	- Success!
				FALSE	- An error occured whilst processing the file.
	Purpose:	Tries to make use of the BoundingBox info in the EPS file

********************************************************************************************/

BOOL EPSFilter::ProcessBoundingBox()
{
	// Does this filter care about the bounding box?
	if (!AdjustOrigin)
		// No - discard it
		return TRUE;

	// Found the image's bounding box - set the origin accordingly.
	DocRect BBox;

	// Take a copy of the bounding box line so we can decompose it.
	char BBoxStr[50];
	_tcsncpy(BBoxStr, TokenBuf + 14, 50);

	// Extract the minimum values (converting to millipoints)

	char *Num = _tcstok(BBoxStr, " ");
	if (Num == NULL)
		// Error encountered in bounding box data - ignore it.
		return TRUE;
	BBox.lo.x = _ttol(Num) * 1000;

	Num = _tcstok(NULL, " ");
	if (Num == NULL)
		// Error encountered in bounding box data - ignore it.
		return TRUE;
	BBox.lo.y = _ttol(Num) * 1000;

	Num = _tcstok(NULL, " ");
	if (Num == NULL)
		// Error encountered in bounding box data - ignore it.
		return TRUE;
	BBox.hi.x = _ttol(Num) * 1000;

	Num = _tcstok(NULL, " ");
	if (Num == NULL)
		// Error encountered in bounding box data - ignore it.
		return TRUE;
	BBox.hi.y = _ttol(Num) * 1000;

	// Is this bounding box within the spread area?
	if (!SpreadRect.ContainsRect(BBox))
		// No - adjust the origin
		Stack.TranslateCoordOrigin(-BBox.lo.x, -BBox.lo.y);

	// It worked!
	return TRUE;
}


/********************************************************************************************

>	void EPSFilter::HandleEPSError()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Purpose:	Cleans up when a syntax error is encountered in the EPS file.
				Any objects constructed so far are deleted, and the error message ID is
				set.

********************************************************************************************/

void EPSFilter::HandleEPSError()
{
	// Error - EPS contains a syntax error.
	TRACE( _T("EPS syntax error at token <%s> (line %ld, char %d)\n"), 
		  TokenBuf, EPSFile->GetLineNumber(), EPSFile->GetCharOffset());

	// Destroy any half-created paths
	if (pPath != NULL)
	{
		pPath->CascadeDelete();
		pPath = NULL;
	}
	else
	{
		delete pInkPath;
	}

	pInkPath = NULL;
	
	// We know we've found an error, so clear the stack before we delete it so we don't
	// set off the ENSURE in EPSStack::~EPSStack.
	Stack.DeleteAll ();

	// Set global flag - this is a bodge until I set up a proper C++ exception to do this.
	EPSFlags.EPSErrorEncountered = TRUE;

	// Set the error for the caller to report
	String_256 ErrMsg;
	ErrMsg.MakeMsg(_R(IDT_EPS_BADSYNTAX), EPSFile->GetLineNumber(), EPSFile->GetCharOffset());
	Error::SetError(_R(IDT_EPS_BADSYNTAX), (TCHAR *) ErrMsg, 0);
}

/********************************************************************************************

>	void EPSFilter::HandleNoMemory()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Purpose:	Cleans up when the filter runs out of memory when importing a file.
				Any objects constructed so far are deleted, and the error message ID is
				set.

********************************************************************************************/

void EPSFilter::HandleNoMemory()
{
	// Destroy any half-created paths
	if (pPath != NULL)
	{
		pPath->CascadeDelete();
		pPath = NULL;
	}
	else
	{
		delete pInkPath;
	}

	pInkPath = NULL;
		
	// Set the error for the caller to report (if it was a memory problem)
	if (Error::GetErrorNumber() == _R(IDS_OUT_OF_MEMORY))
		Error::SetError(_R(IDT_EPS_NOMEMORY));
	else
		// Must be an error in the file - e.g. bad path data
		HandleEPSError();
}

/********************************************************************************************

>	BOOL EPSFilter::ProcessGroup()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Returns:	TRUE if the group was processed correctly, FALSE if not.
	Purpose:	Reads in all the elements of a group structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL EPSFilter::ProcessGroup()
{
	if (!StartGroup())
		return FALSE;

	// Keep processing tokens until we find the end of the group
	do
	{
		GetToken();

		// Look for the end of the group token(s)...
		if ((Token == EPSC_U) || (Token == EPSC_Q))
		{
			return EndGroup(Token == EPSC_Q);
		}
	}
	// Otherwise keep going until an error or eof is encountered
	while (HandleToken() && (!EPSFile->eof()));

	if (EPSFile->eof())
	{
		// Didn't find end of group - syntax error; deal with it
		HandleEPSError();
	}

	// If we're here, something went wrong
	return FALSE;
}

/********************************************************************************************

>	char *EPSFilter::GetEPSCommand(EPSCommand Cmd)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/02/94
	Inputs:		Cmd - the EPS token, e.g. EPSC_showpage
	Returns:	Pointer to the string representation of the token, e.g. "showpage"
	Purpose:	Given an EPS token, return the string representation of it; mainly for
				debugging purposes.

********************************************************************************************/

char *EPSFilter::GetEPSCommand(EPSCommand Cmd)
{
	INT32 i = 0;
	while (Commands[i].Cmd != EPSC_Invalid)
	{
		if (Commands[i].Cmd == Cmd)
			return Commands[i].CmdStr;

		// Try next command
		i++;
	}

	// Couldn't find it - just return "Invalid"
	return Commands[i].CmdStr;
}



/********************************************************************************************

>	BOOL EPSFilter::ExportBinaryHeader(CCLexFile* pFile)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		pFile - The file we are saving to
	Returns:	TRUE if it worked, FALSE if there was an error
	Purpose:	Outputs the initial binary Header for EPS files that contain a TIFF preview
				of the EPS. This format is described on page 729 of the Big Red Postscript
				book (PostScript Language Reference Manual Second Edition). The basic form
				is as follows :-
				MonoOn:
				Bytes 0-3	C5D0D3C6
				Bytes 4-7	Byte pos of start of PostScript in file
				Bytes 8-11	Length of PostScript section in Bytes
				Bytes 12-15	Start of MetaFile (always 0 from Camelot)
				Bytes 16-19	Length of MetaFile (always 0 from Camelot)
				Bytes 20-23	Start of TIFF file
				Bytes 24-27	Length of TIFF file
				Bytes 28-29 Checksum or FFFF
	SeeAlso:	EPSFilter::CorrectBinaryHeader

********************************************************************************************/

BOOL EPSFilter::ExportBinaryHeader(CCLexFile* pFile)
{
#ifdef DO_EXPORT
	// Output the header here
	char Buf[30];

	// Clear it to zero
	memset(Buf, 0, 30);

	// Fill the buffer with the Magic number that must appear at the start of the file
	// acording to the PostScript Manual
	Buf[0] = '\xC5';
	Buf[1] = '\xD0';
	Buf[2] = '\xD3';
	Buf[3] = '\xC6';

	// Fill in the Checksum to FFFF (meaning ignore checksum)
	Buf[28] = '\xFF';
	Buf[29] = '\xFF';
	
	// Write out the Magic number (0-3)
	if (pFile->write(Buf, 30).fail())
		return FALSE;

#endif
	// all worked
	return TRUE;
}


/********************************************************************************************

>	BOOL EPSFilter::CorrectBinaryHeader(CCLexFile* pFile)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		pFile - The file we are saving to
	Returns:	TRUE if it worked, FALSE if there was an error
	Purpose:	Goes back to the binary header at the start of the file and fills in the
				correct values to all the fields.
	SeeAlso:	EPSFilter::ExportBinaryHeader

********************************************************************************************/

BOOL EPSFilter::CorrectBinaryHeader(CCLexFile* pFile)
{
#ifdef DO_EXPORT
	INT32 Length = 0;
	INT32 Zero = 0;

	// Seek back to the first thing that needs changing
	// The Start of PostScript
	pFile->seek(4);

	// Write out the start of postscript (4-7)
	if (pFile->write(&EPSStart, 4).fail())
		return FALSE;

	// Write out the length of postscript (8-11)
	Length = EPSEnd - EPSStart;
	if (pFile->write(&Length, 4).fail())
		return FALSE;

	// Write out the Start of metafile (12-15)
	if (pFile->write(&Zero, 4).fail())
		return FALSE;

	// Write out the length of metafile (16-19)
	if (pFile->write(&Zero, 4).fail())
		return FALSE;

	// Write out the start of TIFF
	if (pFile->write(&PreviewStart, 4).fail())
		return FALSE;

	// Write out the legth of TIFF
	Length = PreviewEnd - PreviewStart;
	if (pFile->write(&Length, 4).fail())
		return FALSE;
#endif
	// All seemed to work
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL EPSFilter::WriteEPSProlog()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Returns:	TRUE if it worked (even if it did nothing), FALSE if there was an error
	Purpose:	Allows the filter a chance to write out something in the Prolog section

********************************************************************************************/

BOOL EPSFilter::WriteEPSProlog()
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL EPSFilter::WriteEPSSetup()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Returns:	TRUE if it worked (even if it did nothing), FALSE if there was an error
	Purpose:	Allows the filter a chance to write out something in the Prolog section

********************************************************************************************/

BOOL EPSFilter::WriteEPSSetup()
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL EPSFilter::WriteEPSComments()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Returns:	TRUE if it worked (even if it did nothing), FALSE if there was an error
	Purpose:	Allows the filter a chance to write out something in the Prolog section

********************************************************************************************/

BOOL EPSFilter::WriteEPSComments()
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL EPSFilter::WriteScript()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Returns:	TRUE if it worked (even if it did nothing), FALSE if there was an error
	Purpose:	Allows the filter a chance to write out something in the Prolog section

********************************************************************************************/

BOOL EPSFilter::WriteScript()
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL EPSFilter::NeedsPrintComponents ()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/00
	Returns:	TRUE - Print components are required for this EPS type.
	Purpose:	Allows the program to determine whether print components are needed for this
				flavour of EPS.

********************************************************************************************/

BOOL EPSFilter::NeedsPrintComponents ()
{
	// Default behaviour - assume that they are needed.
	return TRUE;
}

/********************************************************************************************

>	EPSExportDC* EPSFilter::CreateExportDC()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Returns:	pointer to EPSExportDC or NULL
	Purpose:	This allows the EPS filter to choose which sort of EPSExportDC to use.
				Most filters will use this default one, but some (Native Save/Load eps) will
				make use of a different ExportDC derived from EPSExportDC. This function
				simply returns the result of a 'new' and does no checking on the returned
				parameter. It is perfectly valid for this function to return NULL (meaning
				no ExportDC could be created) and the calling function should handle this
				appropriatly.

********************************************************************************************/

EPSExportDC* EPSFilter::CreateExportDC()
{
#ifdef DO_EXPORT
	// Create and return a DC
	return new EPSExportDC(this);
#else
	return NULL;
#endif
}


/********************************************************************************************

>	virtual BOOL EPSFilter::GetExportOptions( )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> HUmphrys
	Created:	22/12/95
	Inputs:		-
	Returns:	TRUE if OK, FALSE if user pressed Cancel.
	Purpose:	Allows the user to be prompted to get information for export.
				Base class returns True so that nothing happens by default.
	Scope: 		Protected.

********************************************************************************************/

BOOL EPSFilter::GetExportOptions( )
{
	// baseclass version does nothing, so just return TRUE
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSFilter::PrepareToExport(CCLexFile* pFile, Spread *pSpread)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Inputs:		pFile - The file to export to
				pSpread - The spread to export
	Returns:	TRUE if succeeded, FALSE if not (e.g. no memory for EPS stack)
	Purpose:	Prepare to import EPS data using this filter.  This sets up the filter
				to a sensible state for reading.
	Errors:		Out of memory.
	SeeAlso:	EPSFilter::DoImport; EPSFilter::CleanUpAfterImport
	Scope: 		Private

********************************************************************************************/

BOOL EPSFilter::PrepareToExport(CCLexFile* pFile, Spread* pSpread)
{
#ifdef DO_EXPORT
	// Created the 'file' DC for rendering and try to open the specified file.
	ExportDCPtr = CreateExportDC();

	if (ExportDCPtr == NULL)
		return FALSE;

	TRY
	{
		ExportDCPtr->Init(pFile);
	}
	CATCH(CFileException, e)
	{
		// Could not open the export file - return error
		return FALSE;
	}
	END_CATCH

	// Get the position of the first page, and use this to set the origin.
	Page *pPage = pSpread->FindFirstPageInSpread(); 
	ENSURE(pPage != NULL, "Spread has no pages");
	ERRORIF(pPage == NULL, _R(IDT_DOC_BADSTRUCTURE), FALSE);

	// Use bottom left of page as origin
	DocRect PageRect = pPage->GetPageRect();
	ExportDCPtr->SetOrigin(PageRect.lo);

	// Create a new render region to export to:

	// Don't care about clip regions when exporting - create a null region.
	DocRect NullClipRect;
	NullClipRect.MakeEmpty();

	if (IS_A(this, EPSFilter))
	{
		// Don't use rendering matrix when exporting EPS as it uses fractional coordinates.
		Matrix Identity;

		// Don't use view scale; set to 1
		FIXED16 Scale(1);

		// Create the region
		ExportRegion = new EPSRenderRegion(NullClipRect, Identity, Scale);
		if (ExportRegion == NULL)
			return FALSE;

		// Attach to the right device.
		ExportRegion->AttachDevice(DocView::GetSelected(), ExportDCPtr, pSpread);
	}

	// Inform all the document components that we are about to export
	DocComponent *pComponent = TheDocument->EnumerateDocComponents(NULL);

	while (pComponent != NULL)
	{
		// Inform this document component that we are about to start an EPS export.
		pComponent->EPSStartExport(this);

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}
#endif		
	// All ok
	return TRUE;
};


/********************************************************************************************

>	void EPSFilter::CleanUpAfterExport()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/02/94
	Purpose:	Cleans up the memory allocated by EPSFilter::PrepareToImport() - used
				when the import process ends, either normally or abnormally.
	SeeAlso:	EPSFilter::PrepareToImport; EPSFilter::DoImport
	Scope: 		Private

********************************************************************************************/

void EPSFilter::CleanUpAfterExport()
{
#ifdef DO_EXPORT
	// Get rid of our dynamic objects
	if (ExportDCPtr!=NULL)
		delete ExportDCPtr;
	ExportDCPtr = NULL;

	if (ExportRegion!=NULL)
		delete ExportRegion;
	ExportRegion = NULL;

	// Inform all the document components that we have finished exporting
	DocComponent *pComponent = TheDocument->EnumerateDocComponents(NULL);

	while (pComponent != NULL)
	{
		// Inform this document component that we have finished exporting
		pComponent->EPSEndExport(this);

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}
#endif
}


/********************************************************************************************

>	BOOL EPSFilter::DoExport(Operation *pOp, CCLexFile* pFile, PathName * pPath,
							 Document *pDoc, BOOL ShowOptions)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		pOp - The calling operation
				pFile - The file to export to
				pPath - the pathname of the file to be exported to
				pDoc - The doc to export
				ShowOptions - Show the export options dialogue?
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Exports the File as EPS. The various forms of EPS are to be found in the 
				classes derived from this one.

********************************************************************************************/

BOOL EPSFilter::DoExport(Operation* pOp, CCLexFile* pFile, PathName* pPath, Document* pDoc,
						 BOOL ShowOptions)
{
#ifdef DO_EXPORT
	// See if the EPS filters need some exporting options or not
	BOOL ok = GetExportOptions();
	if (!ok)
	{
		// User has cancelled the operation so set our error whcih will be suppressed
		// and return FALSE
		Error::SetError(_R(IDN_USER_CANCELLED),0);
		return FALSE;
	}

	// Used to open the file up before starting DoExport. But this meant a cancel on the export
	// options dialog had filled the file, if it was already present. So now up up here if
	// not open already. In the PreviewBitmap case the file will already be open.
	if (!pFile->isOpen())
	{
		if (pFile->IsKindOf(CC_RUNTIME_CLASS(CCDiskFile)))
		{
			ok = OpenExportFile((CCDiskFile*) pFile, pPath);
			if (!ok)
				return FALSE;
		}
		else
		{
			TRACEUSER( "JustinF", _T("Tried to open non-CCDiskFile in EPSFilter::DoExport\n"));
			return FALSE;
		}
	}
	
	// Do we want a Preview Bitmap while we are here?
	Filter* pFilter = NULL;

	if (WantPreviewBmp)
	{
		// Graeme (23/5/00) - The preview bitmap was originally a TIFF, but for
		// Xara X I've changed it to be a PNG. The preview filter code in the
		// older versions of Camelot (CorelXARA 1 and 2) *SHOULD* work OK with
		// this.
		pFilter = Filter::GetFirst();

		// (ChrisG 28/11/00) - The preview bitmap should be in TIFF format, 
		//	otherwise import into Quark Express, etc... doesn't show anything
		//	but a block stating that this is Postscript data;
		while ((pFilter != NULL) && (pFilter->FilterID != FILTERID_PREVIEW_TIFF))
		{
			// Try the next filter
			pFilter = Filter::GetNext(pFilter);
		}

		// If we've found a filter, set up the preview export.
		if ( pFilter!=NULL )
		{
			// Get ready for the binary header
			EPSStart = 0;
			EPSEnd = 0;
			PreviewStart = 0;
			PreviewEnd = 0;

			// Save out the Binary header
			if (!ExportBinaryHeader(pFile))
			{
				CleanUpAfterExport();
				return FALSE;
			}

			// if this is the Native Exporter, then we want to force the Preview to a fixed size
			//WEBSTER-Martin-03/01/97
			INT32 OldPreviewSize = PreviewFilter::PreviewBitmapSize;

			if (IS_KIND_OF(CamelotNativeEPSFilter))
			{
				// Set the Preview to be just over an inch accross
				PreviewFilter::PreviewBitmapSize = 96000;
			}

			// Get as Bitmap Filter and set it up
			BaseBitmapFilter* pBitmapFilter = static_cast<BaseBitmapFilter*> ( pFilter );
			pBitmapFilter->SetPreviewBitmap(TRUE);
			PreviewStart = pFile->tell();

			// Export the Preview to the file
			if (!pBitmapFilter->DoExport(pOp, pFile, pPath, pDoc, TRUE))
			{
				// The Preview bitmap failed to export
				// Go back to where the bitmap started
				pFile->seek(PreviewStart);

				// mark the fact that no bitmap was saved
				PreviewStart = 0;
				PreviewEnd = 0;
			}
			else
			{
				// Mark where the tiff ends
				PreviewEnd = pFile->tell();
			}

			// Find out where the EPS Starts
			EPSStart = pFile->tell();

			// Set it back
			pBitmapFilter->SetPreviewBitmap(FALSE);
			//WEBSTER-Martin-03/01/97
			if (IS_KIND_OF(CamelotNativeEPSFilter))
			{
				// Set the Preview back to its default setting
				PreviewFilter::PreviewBitmapSize = OldPreviewSize;
			}
		}
	}

	// Set up document pointer
	TheDocument = pDoc;

	// Get pointer to the spread to export.
	Spread *pSpread = GetFirstSpread(pDoc);

	// Set up device context and render region for this export.
	if (!PrepareToExport(pFile, pSpread))
	{
		CleanUpAfterExport();
		return FALSE;
	}

	// Export the data to the file
	if (!ExportRender(ExportRegion))
	{
		CleanUpAfterExport();
		return FALSE;
	}

	// All done - deallocate dynamic objects and return success.
	CleanUpAfterExport();

	// Detach document
	TheDocument = NULL;

	// Do we want a Preview Bitmap while we are here (and were we able to make one)
	if ((WantPreviewBmp) && (pFilter!=NULL))
	{
		// Find out where the PostScript ended
		EPSEnd = pFile->tell();

		// BODGE to make the viewer work.
		// The viewer will get a read error if the EPS data i.e. not including the EPS
		// header and preview bitmap, is less than 1k as this is the section loaded in
		// by Filters::LoadInitialSegment and it will error if less than 1k long. 
		INT32 Length = EPSEnd - EPSStart;
		if (Length < 1024)
		{
			const char PadChar = '\n';
			// Pad the file up the required size
			FilePos	CurrentPosition = pFile->tell();
			while ((CurrentPosition - EPSEnd) < 1024)
			{
				if (pFile->put(PadChar).fail())
					break;
				CurrentPosition = pFile->tell();	
			}
			
		}

		// Correct the binary header, now we know where everything is
		if (!CorrectBinaryHeader(pFile))
		{
			ERROR1(FALSE, _R(IDT_EXPORT_INTERNAL_ERR));
		}

		// Go back and rub out the copyright in the middle of the file
		RemoveMessage(pFile);
	}
#endif
	return TRUE;
}



/********************************************************************************************

>	void EPSFilter::RemoveMessage(CCLexFile* pFile)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/95
	Purpose:	Removes the Copyright message that appears in the middle of our native file
				format.

********************************************************************************************/

void EPSFilter::RemoveMessage(CCLexFile* pFile)
{
#ifdef DO_EXPORT
	// What we are looking for
	char Message[] =  "1992 ACCUSOFT INC, ALL RIGHTS RESERVED";
	String_64 Replace("Xara Studio, Xara Studio, Xara Studio.");
	INT32 Length = cc_strlenBytes(Message);
	char Ch = 0;
	FilePos StartOfStr = 0;

	TRY
	{
		// Go to the start of the Preview bitmap
		if (pFile->seekIn(PreviewStart, ios::beg).fail())
			return;

		// Start looking for the string
		INT32 CharsToSearch = PreviewEnd - PreviewStart;
		INT32 MatchedChars = 0;
		
		// Get the first char and start looking
		pFile->get(Ch);
		while ((!pFile->eof()) && (!pFile->fail()) && (MatchedChars<Length) && (CharsToSearch>0))
		{
			// Count this char
			CharsToSearch--;

			// see if matches our string
			if (Ch==Message[MatchedChars])
			{
				// see if it is the first char in the string
				if (MatchedChars==0)
				{
					// We have just read the first char so the start is
					// where we are now, minus 1
					StartOfStr = pFile->tell() - 1;
				}

				// Count the char
				MatchedChars++;
			}
			else
			{
				// Mismatch, so start again
				MatchedChars=0;
			}

			// Get the next char in the file and return if there is a problem
			pFile->get(Ch);
		}

		// If we found the whole string, replace it
		if (MatchedChars==Length)
		{
			// Go back to the start of the string
			pFile->seek(StartOfStr);
			pFile->write(Replace);
			TRACEUSER( "Rik", _T("Replaced copyright\n"));
		}
	}

	CATCH(CFileException, e)
	{
		// There was a problem, so don't change the message
		return;
	}
	END_CATCH
#endif
}



/********************************************************************************************

>	BOOL EPSFilter::ImportBinary(ADDR pData, INT32 Length)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		Length - the number of bytes to read, in terms of binary data imported, as
						 opposed to how many bytes the file format takes to represent this
						 binary data.
	Outputs:	pData - the buffer to put the data into.
	Returns:	TRUE if it imported the specified number of bytes correctly;
				FALSE if not.
	Purpose:	Converts EPS hex strings into binary data.  Attempts to read the number
				of bytes specified from the file.
	Errors:		File input errors; EOF
	SeeAlso:	Filter::ImportBinary

********************************************************************************************/

INT32 EPSFilter::ImportBinary(ADDR pData, INT32 Length)
{
	do
	{
		// Get the next token from the file
		if (!EPSFile->GetHexToken())
			return FALSE;

		INT32 CharsRead = EPSFile->GetCharsRead();

		if (CharsRead > (LastProgressUpdate + 2048))
		{
			if (!ContinueSlowJob(CharsRead))
			{
				// Abort operation - make sure nodes are deleted and not added to the tree.
				ERROR(_R(IDT_IMPORT_USERABORT), FALSE);
			}
			else
			{
				LastProgressUpdate = CharsRead;
			}
		}

		// Ignore whitespace
		if (EPSFile->GetTokenType() == TOKEN_STRING)
			// Error
			break;

		// Decode the token into hex data.
		INT32 nBytes = DecodeHexString(pData, Length);

		if (nBytes == -1)
		{
			// Error
			ENSURE(FALSE, "Error in EPS hex data");
			break;
		}

		pData += nBytes;
		Length -= nBytes;
	}
	while ((Length > 0) && !EPSFile->eof());

	if (Length == 0)
		// All ok
		return TRUE;
	
	// Error encountered
	ENSURE(FALSE, "Error importing binary data from EPS file");
	return FALSE;
}


/********************************************************************************************

>	INT32 EPSFilter::DecodeHexString(ADDR pData, INT32 Length, INT32 nStart)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		Length - the maximum number of bytes to read from the hex string.
				nStart -	where in TokenBuf (the member that contains the semantic value of
							the token) to start the decoding
	Outputs:	pData - the buffer to place the binary data into.
	Returns:	The number of bytes read from the current token.
				If an error occurs (i.e. invalid hex data) then -1 is returned.
	Purpose:	Treat the current token as a hex string, and decode it into binary data
				and place it in the buffer provided by the caller.  No more than Length
				bytes should be placed in the buffer.
	SeeAlso:	EPSFilter::ImportBinary

********************************************************************************************/
INT32 EPSFilter::DecodeHexString(ADDR pData, INT32 Length, INT32 nStart)
{
	INT32 TokenLen = cc_strlenBytes(TokenBuf + nStart);
	INT32 Ofs = 0;

	// Assume hex strings are even-numbered in length for the moment
	ENSURE((TokenLen & 1) == 0, "Bad hex string length in DecodeHexString");

	// Work out how much of the string we should decode
	INT32 MaxOfs = Length * 2;

	// Decode the string two characters at a time
	for (INT32 i = nStart; (i < TokenLen) && (i < MaxOfs); i += 2)
	{
		char Ch = _totupper(TokenBuf[i]);
		BYTE Byte;

		// Decode first digit.
		if ((Ch >= '0') && (Ch <= '9'))
		{
			Byte = (BYTE) (Ch - '0');
		}
		else if ((Ch >= 'A') && (Ch <= 'F'))
		{
			Byte = (BYTE) (Ch - 'A') + 10;
		}
		else
		{
			// Error in hex data
			return -1;
		}

		Ch = _totupper(TokenBuf[i+1]);
		Byte <<= 4;
			
		// Decode swcond digit.
		if ((Ch >= '0') && (Ch <= '9'))
		{
			Byte |= (BYTE) (Ch - '0');
		}
		else if ((Ch >= 'A') && (Ch <= 'F'))
		{
			Byte |= (BYTE) (Ch - 'A') + 10;
		}
		else
		{
			// Error in hex data
			return -1;
		}

		pData[Ofs++] = Byte;
	}

	// How much data did we read?
	return Ofs;
}



/********************************************************************************************

>	BOOL EPSFilter::ResetImportOrigin()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/2/95
	Returns:	TRUE if it worked, FLASE if not
	Purpose:	If the importer changes the pages in the document when loading, this allows
				it to reset the origin.

********************************************************************************************/

BOOL EPSFilter::ResetImportOrigin()
{
	// For now, position EPS objects on 1st page of spread 1
	Page *pPage = ImportInfo.pSpread->FindFirstPageInSpread(); 
	ERROR2IF(pPage==NULL, FALSE, "Spread has no pages");

	// Use bottom left of page as origin
	DocRect PageRect = pPage->GetPageRect();
	Stack.SetCoordOrigin(PageRect.lo);

	// worked
	return TRUE;
}


/********************************************************************************************

>	BOOL EPSFilter::ResetImportOrigin(DocCoord NewOrigin)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/95
	Returns:	TRUE if it worked, FLASE if not
	Purpose:	Instructs the filter to use the new origin, because we are importing and
				otherwise QuickShapes and Text Stories will be incorrectly positioned.

********************************************************************************************/

BOOL EPSFilter::ResetImportOrigin(DocCoord NewOrigin)
{
	Stack.SetCoordOrigin(NewOrigin);

	// worked
	return TRUE;
}



/********************************************************************************************

>	BOOL EPSFilter::ProcessText()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>/Mike
	Created:	29/11/94
	Returns:	TRUE if the Text was processed correctly, FALSE if not.
	Purpose:	Reads in all the elements of a Text structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL EPSFilter::ProcessText()
{
	INT32 Type;
	Stack.Pop(&Type);
	Node *OldPos = pNode;
	BOOL PastAnEOL = FALSE;

	// we don't support area text 
	if ((Type!=0) && (Type!=2))
		return TRUE;

	// Create a matrix to use
	Matrix StoryMat;
	if (!GetStoryMatrix(&StoryMat))
	{
		HandleEPSError();
		return FALSE;
	}	

	// create the text story
	TextStory *TheStory = new TextStory();  
	AddNewNode(TheStory);

	// set the story matrix
	TheStory->SetStoryMatrix(StoryMat);

	// change insert position
	pNode = TheStory;

	while (!EPSFile->eof())
	{
		GetToken();

		switch (Token)
		{
			// Start of a new text object 
			case EPSC_To:	
			{
				ERROR2RAW("Bad Error in EPS .. TextObject inside TextObject !");
				goto error;
			}

			// Found the end of text so return done.
			case EPSC_TO:	
			{
				if (!TheStory->AttachCaretAttributes())
					goto error;

				// (ChrisG 29/3/2001) - Make sure that the last line has an EOL node at the end.
				TextLine * pLine = TheStory->FindLastLine ();
				if (!pLine->FindEOLNode ())
				{
					EOLNode* pEOL = new EOLNode(); 
					if (!AddNewNode(pEOL))
						goto error;
				}

				pNode = OldPos;
				return TRUE;
			}

			// text render type
			case EPSC_Tr:
			{
				INT32 Style;
				if (!Stack.Pop(&Style))
					goto error;
				
				switch (Style)
				{
					case 0: // Fill text (create a transparent line colour)
							SetStrokeColourNone();
							break;
					case 1:	// stroke text (create a transparent fill colour)
							SetFillColourNone();
							break;
					case 2:	// fill and stroke text
							break;
					case 3:	// text with no fill or stroke (create transparent fill and line)
							SetStrokeColourNone();
							SetFillColourNone();
							break;
				}
				break;
			}

			case EPSC_Tx:
			case EPSC_Tj:
			case EPSC_TX:	// found some text	   
			{	  				
				// we want to add text
				if (PastAnEOL)
				{
					MakeNewTextLine(TheStory);
				 	PastAnEOL = FALSE;
				}

				String_256 Text;

				if (!Stack.Pop(&Text))
					goto error;

				char * buf = (TCHAR*)Text;
				INT32 len = Text.Length();
			
				TextChar* pCh = NULL;
				// BODGE
				for ( INT32 i = 0; i < len; i++)
				{
					// check for a carriage return
					if (buf[i]=='\r')
					{
						PastAnEOL = TRUE;

						EOLNode* pEOL = new EOLNode(); 
						if (!AddNewNode(pEOL))
							goto error;

						if (!AttributeManager::ApplyBasedOnDefaults(pEOL,CurrentAttrs))
							goto error;
					}
					// (ChrisG 2/4/2001) Don't import overflow (invisible) text, unless it's a 
					//	newline. Technically speaking this should still be imported, but it is
					//	redundant information that Xara X won't ever use, so why import it?
					else if (Token != EPSC_TX)
					{
						// create a TextChar node 
						pCh = new TextChar();
						if (pCh == NULL)
							goto error;

						pCh->SetUnicodeValue((WCHAR)buf[i]);

						// Make sure we actually fill our paths matey
						if (!SetPathFilled(TRUE))
							goto error;

						if(!AddNewNode(pCh))
							goto error;

						if (!AttributeManager::ApplyBasedOnDefaults(pCh,CurrentAttrs))
							goto error;
					}
				}

				// now turn off single character attributes
				break;
			}


			case EPSC_T_:
			{	  				
				// ok we've received a T* token so translate the text matrix by
				// (-lineleading, 0) according to the documentation. Is this correct?
				// What I should do for now is to check wether we have a return code
				// in the last line generated and if not create one

				break;
			}

			// end of text path - not sure about this !!
			case EPSC_TP:	// create a text line
			{
				if (!MakeNewTextLine(TheStory))
					goto error;

				CaretNode* pCaret = new CaretNode(); 
				if (!AddNewNode(pCaret))
					goto error;

				break;
			}
			
			// Aspect Ratio %
			case EPSC_Tz:	
			{
				double Aspect;
				if (!Stack.Pop(&Aspect))
					goto error;
				if (!SetTextAspectRatio(FIXED16(Aspect/100.0)))	// convert back from % to ratio!
					goto error;
				break;
			}
			
			// Tracking
			case EPSC_Tt:	
			{
				INT32 Tracking;
				if (!Stack.Pop(&Tracking))
					goto error;
				if (!SetTextTracking(Tracking))
					goto error;
				break;
			}

			case EPSC_Ta:	// Justification
			{
				Justification JustVal;
				INT32 Justify;
				if (!Stack.Pop(&Justify))
					goto error;
				switch (Justify)
	   			{
	   			case 0:
	   				JustVal = JLEFT; 
	   				break;
				case 2:
					JustVal = JRIGHT; 
	   				break;
				case 1:
					JustVal = JCENTRE; 
	   				break;
				case 3:
					JustVal = JFULL; 
	   				break;
				}
				if (!SetTextJustification(JustVal))
					goto error;
				break;
			}

			// line spacing, format=   linespace paraspace Tl
			case EPSC_Tl:
			{
				double lineLead, paraLead;
					
				if (!Stack.Pop(&paraLead))
					goto error;
				
				if (!Stack.Pop(&lineLead))
					goto error;
				
				// line lead measure in absolute pointsize
				INT32 lead = (INT32)(lineLead*1000);

				if (!SetTextLineSpacing(1,0,lead,0))
					goto error;
				
				break;
			}
			
			
			// create a Kern Code.
			case EPSC_Tk:	
			{

				// we want to add text
				if (PastAnEOL)
				{
					MakeNewTextLine(TheStory);
				 	PastAnEOL = FALSE;
				}

				INT32 AutoKern;
				DocCoord KernValue;

				KernCode* pKernCode = new KernCode(); 
				if (!pKernCode)
					goto error;

				if (!Stack.Pop(&KernValue.x))
					goto error;

				if (!Stack.Pop(&AutoKern))
					goto error;

				pKernCode->SetValue(KernValue); 

				if (!AddNewNode(pKernCode))
					goto error;

				if (!AttributeManager::ApplyBasedOnDefaults(pKernCode,CurrentAttrs))
					goto error;

				break;
			}

			// Baseline shift
			case EPSC_Ts:	
			{
				double BaseShift;

				if (!Stack.Pop(&BaseShift))
					goto error;

				INT32 RealShift = (INT32)(BaseShift*1000+0.5);

				if (TextComment[0]>0)
				{
					TextComment[1]=RealShift;
					TextComment[0]--;
					if (TextComment[0]==0)
					{
						if (!SetTextScript(TextComment[1], TextComment[2]))
							goto error;
						TextComment[1]=0;
						TextComment[2]=0;
					}
				}
				else
				{
					if (!SetTextBaseLine(RealShift))
						goto error;
				}
				break;
			}
			
			// Font name, pointsize
			case EPSC_Tf:	
			{
				String_64 EncodedName;
				double FSize;

				// Pop the font size.
				if (!Stack.Pop(&FSize))
					goto error;

 				// Graeme (12/6/00) - The font format has changed with AI7, which adds two
				// more operators to the definition. The AI5 operator expects a string here -
				// if it's not present (i.e. the Pop fails), then we guess that it's an AI7
				// file, and make a second attempt to pull the font size and name from the
				// stack.
				if ( !Stack.Pop ( &EncodedName ) )
				{
					// We're dealing with an AI7 font, which contains descent and spacing
					// information. Ignore the last item, and use the next two as the
					// size and font name fields.

					// Pull out the font size.
					if ( !Stack.Pop ( &FSize ) )
						goto error;

					// And take a second stab at getting the font name.
					if ( !Stack.Pop ( &EncodedName ) )
						goto error;
				}

				// Calculate the millipoint fontsize.
				INT32 MillFSize = (INT32)(FSize*1000+0.5);

				// check whether we're inside a sub/super script comment
				if (TextComment[0]>0)
				{
					TextComment[2]=MillFSize;
					TextComment[0]--;	
					if (TextComment[0]==0)
					{
						if (!SetTextScript(TextComment[1], TextComment[2]))
							goto error;
						TextComment[1]=0;
						TextComment[2]=0;
					}
				}
				else
				{
					if (!SetTextFont(&EncodedName, MillFSize))
						goto error;
				}
				break;
			}

			// Unicode character handling (only output in native documents)
			// This is inserted here rather than in the native filter due
			// to EOL characters and the way the old code generates new lines.
			// We keep a LOCAL PastAnEOL flag to let us know whether we need
			// to generate a new line for the next character or not. This avoids
			// dangling lines with no chars in. Unfortunately all vars are local
			// to this routine and hence the native class cant get at them. So for
			// this single token, the native class will pass it up here where we
			// can get at it.

			case EPSC_ctx:
			{
				INT32 numc=0, ch, i, buff[256];
				TextChar* pCh = NULL;

				// we want to add text
				if (PastAnEOL)
				{
					MakeNewTextLine(TheStory);
				 	PastAnEOL = FALSE;
				}

				// get the number of unicode characters in this string
				if (!Stack.Pop(&numc))
					goto error;

				if (numc>255)
					goto error;

				for (i=0; i<numc; i++)
				{
					if (!Stack.Pop(&ch))
						goto error;

					buff[i]=ch;
				}

				for (i=(numc-1); i>=0; i--)
				{
					// create a TextChar node 
					pCh = new TextChar();
					if (pCh == NULL)
						goto error;

					pCh->SetUnicodeValue((WCHAR)buff[i]);

					// Make sure we actually fill our paths matey
					if (!SetPathFilled(TRUE))
						goto error;

					if (!AddNewNode(pCh))
						goto error;

					if (!AttributeManager::ApplyBasedOnDefaults(pCh,CurrentAttrs))
						goto error;
				}
			
				break;
			}

			case EPSC_ctex:
			{
				// the text extras token
				INT32 version;

				if (!Stack.Pop(&version))
					goto error;

				switch (version)
				{
					case 0:
						double Scale,Aspect,Rotation,Shear;
						INT32 AsShapes, WordWrapping;
						MILLIPOINT StoryWidth;
						
						if (!Stack.Pop(&AsShapes))
							goto error;
						if (!Stack.Pop(&Shear))
							goto error;
						if (!Stack.Pop(&Rotation))
							goto error;
						if (!Stack.Pop(&Aspect))
							goto error;
						if (!Stack.Pop(&Scale))
							goto error;
						if (!Stack.Pop(&StoryWidth))
							StoryWidth = 0;
						if (!Stack.Pop(&WordWrapping))
							WordWrapping = FALSE;

						// Set values in story here....
						TheStory->SetWordWrapping(WordWrapping);
						TheStory->SetStoryWidth(StoryWidth);
						TheStory->SetCharsScale(Scale);
						TheStory->SetCharsAspect(Aspect);
						TheStory->SetCharsRotation(Rotation);
						TheStory->SetCharsShear(Shear);
						TheStory->SetPrintingAsShapes(AsShapes);

						break;

					default:
						// don't understand this version so chuck it!
						if (!Stack.Discard())
							goto error;
				}

				break;
			}

			default:
				if (!HandleToken())
					goto error;
				break;
		}
	}

error:
	pNode = OldPos;	

	HandleEPSError();
	return FALSE;
}



/********************************************************************************************

>	BOOL EPSFilter::GetStoryMatrix(Matrix* pMatrix)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Outputs:	pMatrix - holds the story matrix red
	Returns:	TRUE if the matirx was read correctly, FALSE if not.
	Purpose:	Reads in all the elements of a Text Story Matrix.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL EPSFilter::GetStoryMatrix(Matrix* pMatrix)
{
	while (!EPSFile->eof())
	{
		GetToken();

		// Look for the end of text token...
		switch (Token)
		{
			case EPSC_Tp:	// found the start of a text path.
			{
				INT32 StartDist;
				if (!Stack.Pop(&StartDist))
					return FALSE;

				if (!Stack.Pop(pMatrix, FALSE))
					return FALSE;

				// if we've loaded an illustrator file we need to fix up
				// the text matrix position
				ProcessTextMatrix(pMatrix);

				return TRUE;						
			}
			default:
				if(!HandleToken())
					return FALSE;
				break;
		}
	}

	HandleEPSError();
	return FALSE;
}


/********************************************************************************************

>	virtual void EPSFilter::ProcessTextMatrix(Matrix* pMatrix)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/95
	Returns:	-
	Purpose:	Having read a text object position matrix from the file, we may (dependent
				on the filter type), need to process it in some way, ie add in the spread
				origin.

********************************************************************************************/

void EPSFilter::ProcessTextMatrix(Matrix* pMatrix)
{
	// Do nothing in this base class
}

/********************************************************************************************

>	BOOL EPSFilter::MakeNewTextLine()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Returns:	TRUE if the Text was processed correctly, FALSE if not.
	Purpose:	Reads in all the elements of a Text structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL EPSFilter::MakeNewTextLine(Node * pParent)
{
	pNode = pParent;
	TextLine* pTextLine = new TextLine(); 
	if(!AddNewNode(pTextLine))
		return FALSE;
	// make all future chars etc.. children of this line
	pNode = pTextLine;
	return TRUE;
}



/********************************************************************************************

>	BOOL EPSFilter::SetStrokeColourNone()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		-
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for StrokeColour with the value of COLOUR_NONE
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL EPSFilter::SetStrokeColourNone()
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_STROKECOLOUR].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_STROKECOLOUR].pAttr = new StrokeColourAttribute();
		if (CurrentAttrs[ATTR_STROKECOLOUR].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_STROKECOLOUR].Temp = TRUE;
	}

	// Line Colour must be NONE.
	StrokeColourAttribute* pLineCol = (StrokeColourAttribute*)CurrentAttrs[ATTR_STROKECOLOUR].pAttr;
	pLineCol->SetStartColour(&DocColour(COLOUR_NONE));
	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSFilter::SetFillColourNone()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		-
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for FillColour with the value of COLOUR_NONE
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL EPSFilter::SetFillColourNone()
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_FILLGEOMETRY].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_FILLGEOMETRY].pAttr = new FlatFillAttribute();
		if (CurrentAttrs[ATTR_FILLGEOMETRY].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_FILLGEOMETRY].Temp = TRUE;
	}

	// Fill Colour must be NONE.
	FlatFillAttribute* pFillCol = (FlatFillAttribute*)CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;
	pFillCol->SetStartColour(&DocColour(COLOUR_NONE));
	// All worked ok.
	return TRUE;
}



/********************************************************************************************

>	BOOL EPSFilter::ProcessRectangle(NodeRect* pRect, NodeRegularShape* pQuickShape)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/95
	Inputs:		pPath - the old style rectangle that has been read
				pQuickShape - points to an unitilialied QuickShape	
	Outputs:	pQuickShape will be a rectangle QuickShape
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Having read in a rectangle path, this function builds a QuickShape rectangle
				as defined by the path

********************************************************************************************/
BOOL EPSFilter::ProcessRectangle(NodeRect* pRect, NodeRegularShape* pQuickShape)
{
	BOOL ok = TRUE;

	if ((pRect != NULL) && (pQuickShape != NULL) && (pRect->InkPath.GetNumCoords() == 5))
	{
		if (ok)
			ok = pQuickShape->SetUpShape();
		DocCoord *Points = pRect->InkPath.GetCoordArray();
		if (ok && pQuickShape->MakeRectangle((INT32)(Points[0].Distance(Points[1])),
										(INT32)(Points[1].Distance(Points[2])),0))
		{
			// Now skew, rotate and translate the rectangle as required
			// Get rotation
			DocCoord Centre = DocCoord::OneHalf(Points[0], Points[2]);
			DocCoord RenderMajor = DocCoord::OneHalf(Points[0], Points[1]);

			double RotAngle = atan2((double)RenderMajor.y-Centre.y,(double)RenderMajor.x-Centre.x) - (PI/2);
			//double SkewAngle= atan2(pQuickShape->GetUTMinorAxes().y,pQuickShape->GetUTMinorAxes().x) - (PI/2);

			Trans2DMatrix TransMat(Matrix(ANGLE(RotAngle*(180/PI))));
			TransMat *= Trans2DMatrix(Centre.x,Centre.y);
			pQuickShape->Transform(TransMat);
		}
		else
		{
			ERROR3("MakeRectangle failed");
			ok = FALSE;
		}
	}
	else
	{
		ERROR3("Bad rectangle path in EPS import!");
		ok = FALSE;
	}

	return ok;
}



/********************************************************************************************

>	BOOL EPSFilter::ProcessEllipse(NodeEllipse* pEllipse, NodeRegularShape* pQuickShape)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/95
	Inputs:		pEllipse - the old style ellipse that has been read
				pQuickShape - points to an unitilialied QuickShape	
	Outputs:	pQuickShape will be a ellipse QuickShape
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Having read in a ellipse path, this function builds a QuickShape ellipse
				as defined by the path

********************************************************************************************/
BOOL EPSFilter::ProcessEllipse(NodeEllipse* pEllipse, NodeRegularShape* pQuickShape)
{
	BOOL ok = TRUE;

	if ((pEllipse != NULL) && (pQuickShape != NULL) && (pEllipse->InkPath.GetNumCoords() == 13))
	{
		if (ok)
			ok = pQuickShape->SetUpShape();
		const DocCoord Centre = DocCoord::OneHalf(ShapeBBox[0], ShapeBBox[2]);
		const double Height = DocCoord::DistanceFromLine(ShapeBBox[0], ShapeBBox[1], Centre);
		const double Width = DocCoord::DistanceFromLine(ShapeBBox[1], ShapeBBox[2], Centre);

		if (ok && pQuickShape->MakeEllipse((INT32)Width, (INT32)Height))
		{
			// Now skew, rotate and translate the ellipse as required
			// Get rotation
			DocCoord RenderMajor = DocCoord::OneHalf(ShapeBBox[0], ShapeBBox[1]);

			double RotAngle = atan2((double)RenderMajor.y-Centre.y,(double)RenderMajor.x-Centre.x) - (PI/2);
			//double SkewAngle= atan2(pQuickShape->GetUTMinorAxes().y,pQuickShape->GetUTMinorAxes().x) - (PI/2);

			Trans2DMatrix TransMat(Matrix(ANGLE(RotAngle*(180/PI))));
			TransMat *= Trans2DMatrix(Centre.x,Centre.y);
			pQuickShape->Transform(TransMat);
		}
		else
		{
			ERROR3("MakeEllipse failed");
			ok = FALSE;
		}
	}
	else
	{
		ERROR3("Bad ellipse path in EPS import!");
		ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	class EPSSavedGraphicState : class ListItem

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Purpose:	to store a graphic state for the EPS filter
	SeeAlso:	EPSFilter

********************************************************************************************/

class EPSSavedGraphicState : public ListItem
{
	CC_DECLARE_MEMDUMP(EPSSavedGraphicState);

public:
	EPSSavedGraphicState();
	~EPSSavedGraphicState();
	BOOL Setup();

public:
	NodePath *pPath;
	INT32 ThePathType;
	AttributeEntry *Attrs;
	EPSFlagsDefn EPSFlags;
};

CC_IMPLEMENT_MEMDUMP(EPSSavedGraphicState, ListItem)

/********************************************************************************************

>	EPSSavedGraphicState::EPSSavedGraphicState()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Inputs:		-
	Returns:	-
	Purpose:	constructor
	SeeAlso:	EPSFilter

********************************************************************************************/

EPSSavedGraphicState::EPSSavedGraphicState()
{
	pPath = 0;
	Attrs = 0;
}


/********************************************************************************************

>	EPSSavedGraphicState::Setup()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Inputs:		-
	Returns:	-
	Purpose:	set up the object
	SeeAlso:	EPSFilter

********************************************************************************************/

BOOL EPSSavedGraphicState::Setup()
{
	// number of attributes?
	INT32 NumAttrs = AttributeManager::GetNumAttributes();

	// get some memory for the nice attribute values
	Attrs = new AttributeEntry[NumAttrs];

	// set up the values
	for (INT32 i = 0; i < NumAttrs; i++)
	{
		Attrs[i].pAttr = 0;
		Attrs[i].Temp = FALSE;
		Attrs[i].Ignore = FALSE;
	}

	// All done
	return TRUE;
}


/********************************************************************************************

>	EPSSavedGraphicState::~EPSSavedGraphicState()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Inputs:		-
	Returns:	-
	Purpose:	destructor
	SeeAlso:	EPSFilter

********************************************************************************************/

EPSSavedGraphicState::~EPSSavedGraphicState()
{
	if(pPath != 0)
	{
		pPath->CascadeDelete();
		delete pPath;
	}

	if(Attrs != 0)
	{
		// how many?
		INT32 NumAttrs = AttributeManager::GetNumAttributes();

		// delete them...
		for (INT32 i = 0; i < NumAttrs; i++)
		{
			if((Attrs[i].pAttr != 0) && (Attrs[i].Temp == TRUE))
				delete Attrs[i].pAttr;
		}

		// delete the attay
		delete [] Attrs;
	}
}

/********************************************************************************************

>	BOOL EPSFilter::Import_gsave()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Inputs:		-
	Returns:	error flag - can be a memory error
	Purpose:	stores the current path and attributes to emulate a gsave in the imported
				EPS file. All things are copied. States can be nested.
	SeeAlso:	EPSFilter::Import_grestore

********************************************************************************************/

BOOL EPSFilter::Import_gsave()
{
	// get a new item to put everything in
	EPSSavedGraphicState *State = new EPSSavedGraphicState;

	if(State == 0 || !State->Setup())
	{
		delete State;
		return FALSE;
	}

	// copy the path into it
	if(pPath != 0)
	{
		if(!pPath->NodeCopy((Node **)&State->pPath))
		{
			delete State;
			return FALSE;
		}
	}
	else
	{
		State->pPath = 0;
	}


	State->ThePathType = ThePathType;

	// save the flags
	State->EPSFlags = EPSFlags;

	// copy all the attributes into it

	// number of attributes?
	INT32 NumAttrs = AttributeManager::GetNumAttributes();

	// copy the values in
	for (INT32 i = 0; i < NumAttrs; i++)
	{
		if(CurrentAttrs[i].pAttr != 0)
		{
			if(CurrentAttrs[i].Temp)
			{
				// copy this attribute
				CCRuntimeClass* ObjectType = CurrentAttrs[i].pAttr->GetRuntimeClass();
				AttributeValue* AttrClone = (AttributeValue*)ObjectType->CreateObject();

				if(AttrClone == 0)
				{
					delete State;
					return FALSE;
				}

				AttrClone->SimpleCopy(CurrentAttrs[i].pAttr);

				// pop a pointer to it in the state
				State->Attrs[i].pAttr = AttrClone;
			}
			else
			{
				State->Attrs[i].pAttr = CurrentAttrs[i].pAttr;
			}
		}
		else
		{
			State->Attrs[i].pAttr = 0;
		}
		State->Attrs[i].Temp = CurrentAttrs[i].Temp;
		State->Attrs[i].Ignore = CurrentAttrs[i].Ignore;
	}

	// add the state to the list
	GraphicStateStack.AddTail(State);

	return TRUE;
}


/********************************************************************************************

>	BOOL EPSFilter::Import_grestore()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Inputs:		-
	Returns:	error flag - but only if there isn't an entry on the stack to restore
	Purpose:	restores path and attributes saved by the matching Import_gsave.
	SeeAlso:	EPSFilter::Import_gsave

********************************************************************************************/

BOOL EPSFilter::Import_grestore()
{
	// find the graphics state to restore...
	EPSSavedGraphicState *State = (EPSSavedGraphicState *)GraphicStateStack.GetTail();

	if(State == 0)
	{
		// no entry on stack - there's an error in the EPS
		return FALSE;
	}

	// swap the path over
	if(pPath != 0)
	{
		pPath->CascadeDelete();
		delete pPath;
		pPath = 0;
		pInkPath = 0;
	}
	pPath = State->pPath;
	State->pPath = 0;
	ThePathType = (EPSFilter::PathType)State->ThePathType;
	if(pPath != 0)
	{
		pInkPath = &pPath->InkPath;
	}
	else
	{
		pInkPath = 0;
	}

	// restore the flags
	EPSFlags = State->EPSFlags;

	// move the attributes over
	// number of attributes?
	INT32 NumAttrs = AttributeManager::GetNumAttributes();

	// move the attributes
	for (INT32 i = 0; i < NumAttrs; i++)
	{
		// if there's a temporary one there, delete it
		if(CurrentAttrs[i].Temp && CurrentAttrs[i].pAttr != 0)
		{
			delete CurrentAttrs[i].pAttr;
			CurrentAttrs[i].pAttr = 0;
		}
			
		// move this one over
		CurrentAttrs[i].pAttr = State->Attrs[i].pAttr;
		State->Attrs[i].pAttr = 0;
		
		CurrentAttrs[i].Temp = State->Attrs[i].Temp;	
		CurrentAttrs[i].Ignore = State->Attrs[i].Ignore;
	}

	// delete this item
	delete GraphicStateStack.RemoveItem(State);

	return TRUE;
}
