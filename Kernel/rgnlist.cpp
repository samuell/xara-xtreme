// $Id: rgnlist.cpp 662 2006-03-14 21:31:49Z alex $
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

#include "rgnlist.h"
#include "osrndrgn.h"
#include "ensure.h"
#include "document.h"
#include "docview.h"
#include "camview.h"
#include "paper.h"
//#include "menucmds.h"
#include "camelot.h"
#include "errors.h"
#include "spread.h"
//#include "ctrlhelp.h"
#include "app.h"
//#include "statline.h"
//#include "dragmgr.h"
//#include "prncamvw.h"
#include "menuops.h"

DECLARE_SOURCE("$Revision: 662 $");


/********************************************************************************************

>	class ViewItem : public ListItem

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Purpose:	When optimising redraw for a particular View, we try to join all the
				render regions of a View into one if they cover more than 70% of the
				resulting union.  This class is used to remember which DocViews we have
				already tried to optimise as we step through the render region list.
	SeeAlso:	RenderRegionList; ViewList

********************************************************************************************/

class ViewItem : public ListItem
{
	CC_DECLARE_MEMDUMP(ViewItem);

public:
	ViewItem(View *pNewView) : pView(pNewView) {}
	View *pView;
};


/********************************************************************************************

>	class ViewList : public List

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Purpose:	When optimising redraw for a particular View, we try to join all the
				render regions of a DocView into one if they cover more than 70% of the
				resulting union.  This class is used to remember which Views we have
				already tried to optimise as we step through the render region list
	SeeAlso:	ViewItem; RenderRegionList

********************************************************************************************/

class ViewList : public List
{
	CC_DECLARE_MEMDUMP(ViewList)

public:
	~ViewList() { DeleteAll(); }
	BOOL AddView(View *);
	BOOL IsViewOptimised(View*);
};





CC_IMPLEMENT_MEMDUMP(RenderRegionList, List)
CC_IMPLEMENT_MEMDUMP(ViewItem, ListItem)
CC_IMPLEMENT_MEMDUMP(ViewList, List)

#define new CAM_DEBUG_NEW


/********************************************************************************************

>	BOOL ViewList::AddView(View *pView)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Inputs:		pView - pointer to the View to add to the list (only this pointer
						   is stored in the list).
	Returns:	TRUE if View was added ok;
				FALSE if not => ERROR1
	Purpose:	Add a View to a list to indicate that it has been optimised.
	Errors:		Out of memory => ERROR1
	SeeAlso:	ViewList; ViewList::IsViewOptimised

********************************************************************************************/

BOOL ViewList::AddView(View *pView)
{
	// Try to create the new item
	ViewItem *pItem = new ViewItem(pView);
	if (pItem == NULL)
		// Failed - flag error to caller
		return FALSE;

	// Ok - add to list and return success
	AddTail(pItem);
	return TRUE;
}


/********************************************************************************************

>	BOOL ViewList::IsViewOptimised(View *pView)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Inputs:		pView - the View to test for.
	Returns:	TRUE if the View has been optimised;
				FALSE if not
	Purpose:	Determine whether the given View is recorded in this list as having
				been optimised or not.
	SeeAlso:	ViewList; ViewList::AddView

********************************************************************************************/

BOOL ViewList::IsViewOptimised(View *pView)
{
	ViewItem *pItem = (ViewItem *) GetHead();
	while (pItem != NULL)
	{
		if (pItem->pView == pView)
			// It's in the list
			return TRUE;

		// Try next item
		pItem = (ViewItem *) GetNext(pItem);
	}

	// Not found - inform caller
	return FALSE;
}


/********************************************************************************************

>	RenderRegionList::RenderRegionList()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	Initialises the RenderRegionList - in particular, the pointer to the 
				last render region to be rendered.

********************************************************************************************/

RenderRegionList::RenderRegionList()
{
	LastRendered = NULL;
	ListIsOptimal = FALSE;
}

/********************************************************************************************

>	RenderRegionList::~RenderRegionList()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys all render regions in the list, and then destroys the list.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RenderRegionList::~RenderRegionList()
{
	// Call destructors on all objects in the list
	
	RenderRegion *Item = (RenderRegion *) RemoveHead();
	
	while (Item != NULL)
	{
		// Delete the list item
		delete Item;
		
		// Try the next item
		Item = (RenderRegion *) RemoveHead();
	}
}


/********************************************************************************************

>	void RenderRegionList::Merge(RenderRegion *NewRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		NewRegion - the RenderRegion object to add to the list.
	Outputs:	-
	Returns:	-
	Purpose:	Add a render region object to the application's global list of render
				regions.
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*
Technical notes:

	Merging Algorithm
	-----------------

	The New Rect is assumed to be in a Null RenderState (ie. it hasn't rendered yet)

		Clip any Rectangles in List to new Client Area (and Chapter)
	
		For each Rectangle in the List,
			If the Rect is in the same View as the New Rect,
			And it is in the same Chapter as the New Rect,
				If the RenderState is at the start of the Tree,
					Does the New Rectangle contain the Rect in the List ?
					Or, is it Adjacent to the Rect in the List ?
						Yes, then Set Rect in List to Union of the two Rects.
				Ignoring the RenderState do they Intersect at all ?
					Yes, then Split the Rect in the list into sub rects excluding the new
					rect.
		
		Add the New Rect to the List.
    	Attempt to Compact the List.

********************************************************************************************/

void RenderRegionList::Merge(RenderRegion *NewRenderRegion)
{
	// If it's a printer region, just render it to completion and return, because
	// we don't do background printing yet.
	if (NewRenderRegion->IsPrinting())
	{
		NewRenderRegion->DefaultRender();
		return;
	}

	// Adjust any render regions that are being scrolled away
	ClipRectsToClient();

//	TRACE( _T("RenderRegionList::Merge GetCount == %d\n"), GetCount());

	if (GetCount() == 0)
	{
		// Couldn't merge with any others, so just add it to the list.
		AddTail((ListItem *) NewRenderRegion);
// IMMEDIATE REDRAW
//#if _DEBUG
//		ImmediateRender(true);		// Added by GAT
//#endif
		return;
	}

	// Find out how big a pixel is
	PixelWidth = NewRenderRegion->GetScaledPixelWidth();

	// Attempt to compact the list
	MergeRenderRegions(NewRenderRegion);

	// Make sure that there are no render regions that overlap - ever!
	RemoveRegionOverlap(NewRenderRegion);

	// add the New RenderRegion on the end of the List
	AddTail((ListItem *) NewRenderRegion);

	// Go though all the regions in the list and try to strictly
	// merge them with all the other in the list
	TidyUpAdjacentRegions(NewRenderRegion);

	// Set flag to indicate that one or more new regions have been added, and so we
	// should check for the possibility of merging all regions the next time we try a
	// background render.
	ListIsOptimal = FALSE;
// IMMEDIATE REDRAW
//	ImmediateRender();
}



/********************************************************************************************

>	void RenderRegionList::TidyUpAdjacentRegions(RenderRegion* pBrandNewRegion)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/94
	Inputs:		pBrandNewRegion - The region that has just been added to the list by OnDraw.
				We should not delete this region during the Tidy up or OnDraw gets upset
	Purpose:	Goes through all the regions in the list and tests to see if any of them line
				up exactly. If they do, they are joined into a single region

********************************************************************************************/

void RenderRegionList::TidyUpAdjacentRegions(RenderRegion* pBrandNewRegion)
{
	// Need to perform an Nsquared loop through the regions
	RenderRegion* pRegion  = (RenderRegion*) GetHead();

	while(pRegion!=NULL)
	{
		// Get info about the region that we will need to test
		DocRect  MergeClipRect = pRegion->GetRegionRect();
		View* 	 MergeView = pRegion->GetRenderView();
		Node*    NewContext = pRegion->GetRenderState();

		// Find the region after pRegion to start searching from
		RenderRegion* pCompare = (RenderRegion*) GetNext(pRegion);

		// loop through all the regions that come after pRegion
		// see if any are suitbale for merging
		while(pCompare!=NULL)
		{
			// Find the next one now, as we may delete pCompare on the way through
			RenderRegion* pNextOne = (RenderRegion*) GetNext(pCompare);

			// We do not want to delete the brand new region, we should delete the others first
			if (pCompare!=pBrandNewRegion)
			{
				// Find the equivalent information in this region ready for testing
				View* OldView 	 = pCompare->GetRenderView();
				Node* OldContext = pCompare->GetRenderState();

				// See if they can be merged
				if ( (MergeView == OldView) && 
					 (pRegion->GetRenderSpread()==pCompare->GetRenderSpread()) &&
					 ((OldContext==NewContext) || 
					 ((pRegion->CanBeMerged()) && (pCompare->CanBeMerged()))))
				{
					// So far so good. Just check that they are adjacent
					// Find this regions clip rect
					DocRect OldClipRect = pCompare->GetRegionRect();

					// see if one of the render regions is EXACTLY adjacent to this one
					if (OldClipRect.IsAdjacent(MergeClipRect, 0))
					{
						DocRect ClipRect = OldClipRect.Union(MergeClipRect);

						// Set the clip rect of one of the regions to be the union of both regions
//						pRegion->SetClipRect(ClipRect, TRUE);
						pRegion->SetBackmostChangedNode(pCompare->GetBackmostChangedNode());
						pRegion->ResetRegion(ClipRect);

						// Get rid of the other region
						Remove(pCompare);
						delete pCompare;
					}
				}
			}

			// Get the next region
			pCompare = pNextOne;
		}

		// Get the next region in the outer loop
		pRegion = (RenderRegion*) GetNext(pRegion);
	}
}




/********************************************************************************************

>	void RenderRegionList::StrictMerge(RenderRegion* pRegionToMerge)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/94
	Inputs:		pRegionToMerge - The region that we are planning to add to the list
	Purpose:	Tries to merge the render region supplied with the rest of the list.
				They must be exactly adjacent ie. They don't intersect, and the union
				contains no area outside the two rectangles.


********************************************************************************************/

void RenderRegionList::StrictMerge(RenderRegion* pRegionToMerge)
{
	// set a flag to indicate if none of the render regions could be merged
	BOOL Merged;

	// loop round until we can find no more render regions to merge together
	do
	{
		// We have not merged anything yet, so set the flag
		Merged = FALSE;

		// Find the render region to merges clipping rectangle and context node and
		// find which view the render region is to be rendering to
		DocRect  MergeClipRect = pRegionToMerge->GetRegionRect();
		View* MergeView = pRegionToMerge->GetRenderView();
		Node*    NewContext = pRegionToMerge->GetRenderState();

		// Get the first render region in the list
		RenderRegion* pOldRegion = (RenderRegion*) GetHead();
		RenderRegion* pNextRegion;

		while (pOldRegion != NULL)
		{
			pNextRegion = (RenderRegion *) GetNext(pOldRegion);

			// Find this old render regions view
			View* OldView = pOldRegion->GetRenderView();
			Node* OldContext = pOldRegion->GetRenderState();
		
			// see if the two render regions can be merged
			if ( (MergeView == OldView) && 
				 (pOldRegion->GetRenderSpread()==pRegionToMerge->GetRenderSpread()) &&
				 ((OldContext==NewContext) || 
				 ((pOldRegion->CanBeMerged()) && (pRegionToMerge->CanBeMerged()))))
			{
				// Find this regions clip rect
				DocRect  OldClipRect = pOldRegion->GetRegionRect();

				// see if one of the render regions is EXACTLY adjacent to this one
				if (OldClipRect.IsAdjacent(MergeClipRect, 0))
				{
					DocRect ClipRect = OldClipRect.Union(MergeClipRect);

					// Set the clip rect and get the render region to find its starting context node again
					// as things could have changed.
//					pRegionToMerge->SetClipRect(ClipRect, TRUE);
					pRegionToMerge->ResetRegion(ClipRect);
					Remove(pOldRegion);
					delete pOldRegion;
					Merged = TRUE;
					break;
				}
			}

			// Get the next render region in the list
//			pOldRegion = (RenderRegion *) GetNext(pOldRegion);
			pOldRegion = pNextRegion;
		}
	} while( Merged );

	AddTail( (ListItem *) pRegionToMerge);
}



/********************************************************************************************

>	void RenderRegionList::MergeRenderRegions()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Attempts to find regions which can be merged together to make redrawing
				more efficient.  At the moment, the two regions to be merged must satisfy
				the following conditions:
				
				* They must be have got to the same node in the tree when they last stopped
				  rendering.
				
				* They must be rendering into the same View and Chapter.
				
				* One of the regions must completely enclose or be adjacent to the other.
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void RenderRegionList::MergeRenderRegions(RenderRegion* pRegionToMerge)
{
	// set a flag to indicate if none of the render regions could be merged
	BOOL Merged;

	// loop round until we can find no more render regions to merge together
	do
	{
		// We have not merged anything yet, so set the flag
		Merged = FALSE;

		// Find the render region to merges clipping rectangle and context node
		DocRect  MergeClipRect    = pRegionToMerge->GetRegionRect();

		// Find which view the render region is to be rendering to
		View* MergeView = pRegionToMerge->GetRenderView();

		// Get the first render region in the list
		RenderRegion* pOldRegion = (RenderRegion*) GetHead();
		RenderRegion* pNextRegion;

		while (pOldRegion != NULL)
		{
			pNextRegion = (RenderRegion *) GetNext(pOldRegion);

			if (pOldRegion != pRegionToMerge)
			{
				// Find this old render regions view
				View* OldView = pOldRegion->GetRenderView();
			
				// see if the two render regions can be merged
				if ( (MergeView == OldView) && 
					 (pOldRegion->GetRenderSpread()==pRegionToMerge->GetRenderSpread()) &&
					 (pOldRegion->CanBeMerged()) && (pRegionToMerge->CanBeMerged()))
				{
					// Find this regions clip rect
					DocRect  OldClipRect = pOldRegion->GetRegionRect();

					// see if one of the render regions contains the other
					if (MergeClipRect.ContainsRect(OldClipRect))
					{
						// Get rid of the region that is inside the other
						pRegionToMerge->SetBackmostChangedNode(pOldRegion->GetBackmostChangedNode());
						pRegionToMerge->NeedsOSPaper = pRegionToMerge->NeedsOSPaper && pOldRegion->NeedsOSPaper;
						Remove(pOldRegion);
						delete pOldRegion;
						Merged = TRUE;
						break;
					}
					else if (OldClipRect.ContainsRect(MergeClipRect))
					{
						// Get rid of the region that is inside the other
//						pRegionToMerge->SetClipRect(OldClipRect, TRUE);
						pRegionToMerge->SetBackmostChangedNode(pOldRegion->GetBackmostChangedNode());
						pRegionToMerge->ResetRegion(OldClipRect);
						pRegionToMerge->NeedsOSPaper = pRegionToMerge->NeedsOSPaper && pOldRegion->NeedsOSPaper;
						Remove(pOldRegion);
						delete pOldRegion;
						Merged = TRUE;
						break;					
					}
					else
					{
						// Find the smallest dimension of the new region
						INT32 ShortSide = MergeClipRect.Height();
						if (MergeClipRect.Width() < ShortSide)
							ShortSide = MergeClipRect.Width();

						// Make sure that this figure is smaller than the sides
						ShortSide /= 3;

						// Want a fuzzyness of at least 64 pixels
						INT32 MinFuzzy = PixelWidth * 64;
						if (ShortSide<MinFuzzy)
							ShortSide = MinFuzzy;

						// If there is an overlap, then do some merging
						if (OldClipRect.IsAdjacent(MergeClipRect, ShortSide))
						{
							DocRect ClipRect = OldClipRect.Union(MergeClipRect);
//							pRegionToMerge->SetClipRect(ClipRect, TRUE);
							pRegionToMerge->SetBackmostChangedNode(pOldRegion->GetBackmostChangedNode());
							pRegionToMerge->ResetRegion(ClipRect);
							pRegionToMerge->NeedsOSPaper = pRegionToMerge->NeedsOSPaper && pOldRegion->NeedsOSPaper;
							Remove(pOldRegion);
							delete pOldRegion;
							Merged = TRUE;
							break;
						}
					}
				}
			}

			// Get the next render region in the list
//			pOldRegion = (RenderRegion *) GetNext(pOldRegion);
			pOldRegion = pNextRegion;
		}
	} while( Merged );
}




/********************************************************************************************

>	void RenderRegionList::RemoveRegionOverlap(RenderRegion* NewRenderRegion)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/01/94
	Inputs:		NewRenderRegion - pointer to the render region being added
	Purpose:	Called when adding a new render region to the list. It checks the new
				render region for overlap with all the old render regions and if it 
				overlapps any of them, the old region is slit up. This function should
				guarantee that you never get render regions that overlap, so that the
				EORed stuff does not get screwed up by getting rendered more than once.

********************************************************************************************/

void RenderRegionList::RemoveRegionOverlap(RenderRegion* NewRenderRegion)
{
	// Find the clipping rect of the new render region
	DocRect NewClipRect = NewRenderRegion->GetRegionRect();
	DocRect OldClipRect;

	// Find which view and spread the render region is to be rendering to
	View* NewView = NewRenderRegion->GetRenderView();
	Spread*  NewSpread  = NewRenderRegion->GetRenderSpread();

	// Get the first render region in the list
	RenderRegion* pOldRegion = (RenderRegion*) GetHead();

	while (pOldRegion != NULL)
	{
		// Get the old render regions clipping rectangle
		OldClipRect = pOldRegion->GetRegionRect();

		// Find which view and spread the render region is to be rendering to
		View* OldView = pOldRegion->GetRenderView();
		Spread*  OldSpread  = pOldRegion->GetRenderSpread();
	
		// If they intersect, split the old rectangle up into bits
		if ((OldView==NewView) && (OldSpread==NewSpread) && 
		    (OldClipRect.IsIntersectedWith(NewClipRect)))
		{
			if (NewClipRect.ContainsRect(OldClipRect))
			{
				// The 2 regions were the same, so get rid of it, and go back around
				// the loop with the next region in the list.
				NewRenderRegion->SetBackmostChangedNode(pOldRegion->GetBackmostChangedNode());
				NewRenderRegion->NeedsOSPaper = NewRenderRegion->NeedsOSPaper && pOldRegion->NeedsOSPaper;
				RenderRegion *Tmp = pOldRegion;
				pOldRegion = (RenderRegion *) GetNext(pOldRegion);
				Remove(Tmp);
				delete Tmp;
				continue;
			}
			else
			{
				// Split the old rect into sub-rects and add the new one
	         	DocRect SubRects[4];
	         	INT32 RectCount = OldClipRect.SplitRect(NewClipRect, SubRects);
				ENSURE( RectCount!=0, "No rectangles found in SplitRect" );

				// Change the clip rect of the region already in the list
//				pOldRegion->ResizeRegion(SubRects[0]);
//				pOldRegion->SetClipRect(SubRects[0]);
				pOldRegion->ResetRegion(SubRects[0]);

				// and create some new render regions if needed
				for (INT32 i=1; i<RectCount; i++)
				{
					// Create a new render region.
					// NB. important to pass in View pointer so that we get correct bitmap depth.
					RenderRegion* SubRenderRegion = OSRenderRegion::Create(SubRects[i], 
																		   pOldRegion->GetMatrix(), 
																		   OldView->GetViewScale(), 
																		   RENDERTYPE_SCREEN, 
																		   NewView);

					//OSRenderRegion* SubRenderRegion = new OSRenderRegion(*(OSRenderRegion*)pOldRegion);

					ENSURE(	SubRenderRegion != NULL, "RenderRegionList::Merge: SubRenderRegion == 0");
					if (SubRenderRegion == NULL)
						return;

					// Set the new clip region and see if it joins exactly
					if (!SubRenderRegion->CopyRenderInfo(*pOldRegion))
					{
						// copying failed, so tidy up
						delete SubRenderRegion;
						TRACE( _T("CopyRenderInfo %d failed\n"), i);
						continue;								// carry on round loop
					}
	
					// make sure the region is re-rendered
//					SubRenderRegion->IsPaperRendered = FALSE;
//					SubRenderRegion->IsInkRenderStarted = FALSE;

//					SubRenderRegion->SetClipRect(SubRects[i]);
					SubRenderRegion->ResetRegion(SubRects[i]);
//					SubRenderRegion->ResizeRegion(SubRects[i]);
					StrictMerge(SubRenderRegion);
	            }
			}
		}

		// Get the next region in the list
		pOldRegion = (RenderRegion*) GetNext(pOldRegion);
	}
}




/********************************************************************************************

>	void RenderRegionList::ClipRectsToClient()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Clips the RenderRegions in the list to the client area, incase it has been
				scrolled.
	Errors:		-
	Scope:		Private.
	SeeAlso:	-

********************************************************************************************/

void RenderRegionList::ClipRectsToClient()
{
	RenderRegion *pRegion = (RenderRegion*) GetHead();
	while (pRegion != NULL)
	{
		// Find the view associated with the window and skip if null.
		View* pView = pRegion->GetRenderView();
		if (pView != NULL && DocView::GetCurrent() == pView)
		{
			// Get the views rectangle, and convert it into spread coords
			DocRect ClientRect = pView->GetDocViewRect(pRegion->GetRenderSpread());
			pRegion->GetRenderSpread()->DocCoordToSpreadCoord(&ClientRect);

			// Get the clipping rectangle of this render region
			DocRect ClipRect = pRegion->GetRegionRect();

			// see if we need to lose any parts of the render region
			if (!ClientRect.ContainsRect(ClipRect))
			{
		    	if (ClipRect.IsIntersectedWith(ClientRect))
				{
					ClipRect = ClipRect.Intersection(ClientRect);
					pRegion->ResizeRegion(ClipRect);
//					pRegion->SetClipRect(ClipRect);
//					pRegion->ResetRegion(ClipRect);
				}
				else
				{
					// This render region is now off screen, so we can throw it away
					RenderRegion* pPrev = (RenderRegion *) GetPrev(pRegion);
		    		Remove(pRegion);
					delete pRegion;
					pRegion = pPrev;
				}
			}
		}

		// Try the next item.
		pRegion = (RenderRegion*) ((pRegion != NULL) ? GetNext(pRegion) : GetHead());
	}	
}

/********************************************************************************************

>	void RenderRegionList::Remove(RenderRegion *RegionToRemove)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		RegionToRemove - the render region object to remove from the list.
	Outputs:	-
	Returns:	-
	Purpose:	Removes a render region from the application's list of render regions,
				It DOES NOT delete the RenderRegion object itself.
				This is used when the region has been completely rendered and is
				no longer needed.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void RenderRegionList::Remove(RenderRegion *RegionToRemove)
{
	ENSURE( FindPosition(RegionToRemove) != NOT_IN_LIST, 
			"Could not find RenderRegion to Remove" );

	// Is it the LastRendered one?
	if (RegionToRemove == LastRendered)
	{
		// Yes - so make LastRendered point to the previous render region
		// which means if Item is first in the list, LastRendered is set to
		// NULL, which is what we want - see BackgroundRender().
		LastRendered = (RenderRegion *) GetPrev(RegionToRemove);
	}
	
	// Remove the item from the list
	RemoveItem((ListItem *) RegionToRemove);

}
	
/********************************************************************************************

>	void RenderRegionList::Remove(Document *pParentDoc)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		pParentDoc - the parent document that is being removed.
	Purpose:	Removes all render regions from the application's list of render regions
				that are associated with a particular document.
				Usually called because the document is being closed (deleted).
				It deletes all RenderRegion objects it removes.

				HOWEVER:
				When a Document is being closed, all of its DocViews should already have
				been closed, so there should be NO render regions still in the list that
				are associated with this document, so this function will ENSURE whenever
				it finds a render region belonging to this document.
				NB. This is a debugging aid, and hence it only exists in debug builds.


********************************************************************************************/

#if _DEBUG

void RenderRegionList::Remove(Document *pParentDoc)
{
	#if 0
	// Find the NodeDocument object of the tree associated with this document
	Node *pRoot = pParentDoc->GetFirstNode();
	ENSURE(pRoot != NULL, "Document has no root node");
	pRoot = pRoot->FindNext();
	ENSURE(pRoot != NULL, "Document has no NodeDocument node");

	// Start at the beginning
	RenderRegion *Item = (RenderRegion *) GetHead();

	// Look for this render region
	while (Item != NULL)
	{
		// Find its parent NodeDocument object
		Node *pNode = Item->GetRenderChapter();
		ENSURE(pNode != NULL, "RenderRegion has no Chapter!");
		Node *pParent = pNode->FindParent();
		ENSURE(pParent != NULL, "Chapter has no parent!");

		// Find ultimate parent
		while (pParent != NULL)
		{
			pNode = pParent;
			pParent = pNode->FindParent();
		}

        // Is it the same as the document passed by the caller?
		if (pNode == pRoot)
		{
			// Is it the LastRendered one?
			if (Item == LastRendered)
			{
				// Yes - so make LastRendered point to the previous render region
				// which means if Item is first in the list, LastRendered is set to
				// NULL, which is what we want - see BackgroundRender().
				LastRendered = (RenderRegion *) GetPrev(Item);
			}
			
			// Move to next region and remove this one from the list and delete it.
			RenderRegion *Tmp = Item;
			Item = (RenderRegion *) GetNext(Item);
			RemoveItem((ListItem *) Tmp);
			delete Tmp;

			// Bitch about the fact that this should not have happened
			ENSURE(FALSE, "Found a render region that should have been deleted");
		}
		else
			// Try the next region
			Item = (RenderRegion *) GetNext(Item);
	}
	#endif
}

#endif
	
/********************************************************************************************

>	void RenderRegionList::Remove(View *pView)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		pView - the parent view that is being removed.
	Purpose:	Removes all render regions from the application's list of render regions
				that are associated with a particular view.
				Usually called because the docview is being closed (deleted).
				It deletes all RenderRegion objects it removes.

********************************************************************************************/

void RenderRegionList::Remove(View *pView)
{
	// Start at the beginning and try to find render regions attached to this CWnd object.
	RenderRegion *Item = (RenderRegion *) GetHead();

	while (Item != NULL)
	{
        // Is it the same as the View passed by the caller?
		if (Item->GetRenderView() == pView)
		{
			// Is it the LastRendered one?
			if (Item == LastRendered)
			{
				// Yes - so make LastRendered point to the previous render region
				// which means if Item is first in the list, LastRendered is set to
				// NULL, which is what we want - see BackgroundRender().
				LastRendered = (RenderRegion *) GetPrev(Item);
			}
			
			// Move to next region and remove this one from the list and delete it.
			RenderRegion *Tmp = Item;
			Item = (RenderRegion *) GetNext(Item);
			RemoveItem((ListItem *) Tmp);
			delete Tmp;
		}
		else
			// Try the next region
			Item = (RenderRegion *) GetNext(Item);
	}
}
	
/********************************************************************************************

>	BOOL RenderRegionList::BackgroundRender()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if rendering was done, FALSE if no render regions to render.
	Purpose:	Cycles through the members of the render region list, rendering each
				one in turn.  Only one render region is rendered on each call to 
				BackgroundRender().
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL RenderRegionList::BackgroundRender()
{
	if ( CCamApp::IsDisabled() )
	{
//		TRACE( wxT("BGR - IsDisabled\n") );
		return FALSE;					     	// If he has got the system disabled, ignore
	}

	// print monitor not present on viewer

	// Make sure printing is ok.
PORTNOTE("print","RenderRegionList::BackgroundRender - removed printing code")
#ifndef EXCLUDE_FROM_XARALX
	if (!PrintMonitor::IsPrintStatusOK())
	{
		// No - a print job has gone wrong, so we delete all printing render regions
		// before continuing.
		RenderRegion *pRender = (RenderRegion *) GetHead();
		while (pRender != NULL)
		{
			if (pRender->IsPrinting())
			{
				// Render region is printing (well it's not really as an exception
				// has occured) so delete it.
				Remove(pRender);
				delete pRender;
			}

			// Try next region
			pRender = (RenderRegion *) GetNext(pRender);
		}

		// Reset the last rendered render region
		LastRendered = NULL;
	}
#endif
	if (GetCount() == 0)
	{
//		TRACE( wxT("BGR - No regions\n") );
		return FALSE;
	}

//GAT
//	if (ControlHelper::IsUserInterfaceCaptured())
//		return FALSE; // No bg rendering if UI is captured (e.g. menu is up)
//
//	if (DragManagerOp::IsDragActive())
//		return FALSE; // No bg rendering if a DragManager op is active (NB this is for
					  // colour/bitmap/bar drags only - not doc operation drags).
	if (ViewOps::IsFullScreenPending()) // no bg rendering if we are about to enter full screen mode
	{
//		TRACE( wxT("BGR - About to full screen\n") );
	 	return FALSE;
	}

	// Optimise list based on render region percentage coverage
	TryTotalUnion();

	// Work out which render region to render
	if (LastRendered != NULL)
		LastRendered = (RenderRegion*) GetNext(LastRendered);
	
	if (LastRendered == NULL)
		LastRendered = (RenderRegion*) GetHead();
			
	if (LastRendered == NULL)
	{
//		TRACE( wxT("BGR - No last region\n") );
		return FALSE; // No regions to render
	}

	// Ignore the RenderRegion if we are Printing, because
	// we have probably been called from the Print AbortProc
	// and are already rendering.
	if (LastRendered->IsPrinting())
	{
//		TRACE( wxT("BGR - Is printing\n") );
	    return FALSE;
	}

//	TRACEUSER("Gerry", _T("BackgroundRender\n"));
	// Show the user we are rendering.
//GAT
//	GetApplication()->GetpStatusLine()->SetRenderIndicator(Animate);
	GetApplication()->BgRendering = TRUE;
	
	// Do the render.
	Error::RenderThreadIn();			// Make errors close window
	LastRendered->DefaultRender();
	Error::RenderThreadOut();			// Normal errors

	// Turn off render indicator if needed.
	if (GetCount() ==0)
	{
//GAT	GetApplication()->GetpStatusLine()->SetRenderIndicator(NotRendering);
		GetApplication()->BgRendering = FALSE;
	}

	// We did some rendering so return TRUE
	return TRUE;
}


/********************************************************************************************

>	void RenderRegionList::ImmediateRender(BOOL bForceImmediate = FALSE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/94
	Purpose:	Finds all render regions associated with DocViews that have background
				rendering disabled, and renders them all immediately (i.e. to completion).

********************************************************************************************/

void RenderRegionList::ImmediateRender(BOOL bForceImmediate)
{
	TRACE( wxT("Warning - RenderRegionList::ImmediateRender - removed code\n") );
//	TRACE( _T("Look, I'm in RenderRegionList::ImmediateRender()\n"));

	if ( CCamApp::IsDisabled() )
		return;					     	// If he has got the system disabled, ignore

PORTNOTE("other","RenderRegionList::ImmediateRender - removed printing code")
#ifndef EXCLUDE_FROM_XARALX
#ifndef STANDALONE
	// Make sure printing is ok.
	if (!PrintMonitor::IsPrintStatusOK())
	{
		// No - a print job has gone wrong, so we delete all printing render regions
		// before continuing.
		RenderRegion *pRender = (RenderRegion *) GetHead();
		while (pRender != NULL)
		{
			if (pRender->IsPrinting())
			{
				// Render region is printing (well it's not really as an exception
				// has occured) so delete it.
				Remove(pRender);
				delete pRender;
			}

			// Try next region
			pRender = (RenderRegion *) GetNext(pRender);
		}

		// Reset the last rendered render region
		LastRendered = NULL;
	}
#endif
#endif
	if (GetCount() == 0)
		return;

PORTNOTE("other","RenderRegionList::ImmediateRender - removed code")
#ifndef EXCLUDE_FROM_XARALX
	if (ControlHelper::IsUserInterfaceCaptured())
		return; 	// No bg rendering if UI is captured (e.g. menu is up)
	if (DragManagerOp::IsDragActive())
		return; 	// No bg rendering if a DragManager op is active (NB this is for
					// colour/bitmap/bar drags only - not doc operation drags)
#endif
	if(ViewOps::IsFullScreenPending()) // no bg rendering if we are about to enter full screen mode
	 	return;

	// Optimise list based on render region percentage coverage
	TryTotalUnion();
	RenderRegion *pRegion = (RenderRegion *) GetHead();

	// Start the rendering indicator going.
PORTNOTE(Other, "RenderRegionList::ImmediateRender - removed render indicator code" );
#ifndef EXCLUDE_FROM_XARALX
	GetApplication()->GetpStatusLine()->SetRenderIndicator(Rendering);
#endif

	while (pRegion != NULL)
	{
		// Find the next region before this one is deleted
		RenderRegion* pNext = (RenderRegion *) GetNext(pRegion);

		if (pRegion->IsPrinting() == FALSE)
		{
			// If this render region's DocView does not have background rendering enabled, 
			// then it must be an immediate render region, so render it.
			View *pView = pRegion->GetRenderView();
			ENSURE(pView != NULL, "No DocView in ImmediateRender!");

			if ( !pView->GetForeBackMode() || bForceImmediate )
			{
				// Keep user up to date
//GAT			GetApplication()->GetpStatusLine()->SetRenderIndicator(Animate);
				//TRACE( _T("Immediate Render now\n"));

				// Render it.
				Error::RenderThreadIn();		// Make errors close window
				pRegion->DefaultRender(bForceImmediate);
				Error::RenderThreadOut();		// Normal errors
			}
		}

		// Find the next render region
		pRegion = pNext;
	}

	// Disable rendering indicator.
//GAT
//	GetApplication()->GetpStatusLine()->SetRenderIndicator(NotRendering);
}



/********************************************************************************************

>	void RenderRegionList::HandleNodeDeletion(Node* pNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/2004
	Purpose:	Tells RenderRegions that they should be afraid, very afraid, if they are
				holding a pointer to this node...

********************************************************************************************/

void RenderRegionList::HandleNodeDeletion(Node* pNode)
{
	if (GetCount() == 0)
		return;

	RenderRegion *pRegion = (RenderRegion *) GetHead();

	while (pRegion != NULL)
	{
		// Find the next region before this one is deleted
		RenderRegion* pNext = (RenderRegion *) GetNext(pRegion);

		// TODO: Could attack the context pointer here as well!

		// Make sure we're not holding onto a pointer to this node as the "backmost"
		pRegion->ClearBackmostChangedNode(pNode);
		// And hope that this region is about to get merged out of existence!!!

		// Find the next render region
		pRegion = pNext;
	}

}



/********************************************************************************************

>	void RenderRegionList::TryTotalUnion()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Purpose:	For each DocView, try to union all the render regions, and see if the 
				individual areas add up to more than 70% of the union area.  If they do
				then we union all the regions together and just render one big one.
	SeeAlso:	RenderRegionList::DoTotalUnion

********************************************************************************************/

void RenderRegionList::TryTotalUnion()
{
	if (ListIsOptimal)
		// No need to do our checks
		return;

	DocRect TotalUnion;

	// List of DocViews we have optimised so far
	ViewList Views;

	// Go through each render region, and for each View, work out if it's worth
	// merging all the rectangles into one.
	RenderRegion *pRegion = (RenderRegion *) GetHead();

	while (pRegion != NULL)
	{
		View *pView = pRegion->GetRenderView();

		// Have we optimised this region's View already?
		if (!Views.IsViewOptimised(pView))
		{
			// Haven't done this one yet - add it to the list for future reference
			if (!Views.AddView(pView))
			{
				// Error - abort merging
				Views.DeleteAll();
				return;
			}

			// We only do this merging for regions that have not started ink rendering yet.
			if (pRegion->CanBeMerged())
			{
				// Get the union of all regions in this View, and the total area of them
				RenderRegion *pRegion2 = pRegion;

				// Indicates whether to test the percentage coverage - we don't if either:
				// (a) This View has only one region, or
				// (b) This View has regions which have started drawing ink already.
				BOOL TryUnion = FALSE;

				DocRect TotalUnion;
				double CombinedArea = 0.0;

				while (pRegion2 != NULL)
				{
					// Is this the correct View and render context?
					if (pRegion2->GetRenderView() == pView)
					{
						// More than one region - we can try to union them
						TryUnion = TRUE;

						// Have we started rendering the ink into this region yet?
						if (!pRegion2->CanBeMerged())
						{
							// Yes, and we can't union if any have started rendering ink
							TryUnion = FALSE;

							// Quit the while loop
							break;
						}
						else
						{
							// No - so accumulate this region.
							DocRect ClipRect = pRegion2->GetRegionRect();
							TotalUnion = TotalUnion.Union(ClipRect);
							CombinedArea += ((double) ClipRect.Width()) * ((double) ClipRect.Height());
						}
					}

					// Find the next render region
					pRegion2 = (RenderRegion *) GetNext(pRegion2);
				}

				if (TryUnion)
				{
					// Ok - if the combined area is 70% or more of the area of the union, then
					// we might as well union all these regions.
					double UnionArea = (double) TotalUnion.Width() * (double) TotalUnion.Height();
					double Coverage = CombinedArea / UnionArea;

					if (Coverage > 0.60)
					{
						// Yes - let's union all these regions
						DoTotalUnion(pRegion, pView, TotalUnion);
					}
				}
			}
		}

		// Find the next render region
		pRegion = (RenderRegion *) GetNext(pRegion);
	}

	// Don't bother with this again unless a new render region is added.
	ListIsOptimal = TRUE;
}


/********************************************************************************************

>	void RenderRegionList::DoTotalUnion(RenderRegion *pRegion, View *pView, 
										DocRect TotalUnion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Inputs:		pRegion - the region to put the merged area into.
				pView - the View that should have all its regions merged.
				TotalUnion - the union of all the regions for this DocView.
	Purpose:	For the specified DocView, find all the regions that are attached to
				it, and union them all together and use the region 'pRegion' to hold
				this new region.  All the other regions of this DocView are removed from
				the list and deleted.
	SeeAlso:	RenderRegionList::TryTotalUnion

********************************************************************************************/

void RenderRegionList::DoTotalUnion(RenderRegion *pRegion, View *pView, 
									DocRect TotalUnion)
{
	pRegion->ResetRegion(TotalUnion);

	// Delete all the others
	RenderRegion *pRegion2 =(RenderRegion *) GetNext(pRegion);

	while (pRegion2 != NULL)
	{
		// Remember the 'next' region because we might delete this one.
		RenderRegion *pNextRegion = (RenderRegion *) GetNext(pRegion2);

		// Is this the correct DocView and render context?
		if ((pRegion2->GetRenderView() == pView) &&
			(pRegion2->CanBeMerged()))
		{
			// Yes - delete this region
			pRegion->NeedsOSPaper = pRegion->NeedsOSPaper && pRegion2->NeedsOSPaper;
			Remove(pRegion2);
			delete pRegion2;

			// Make sure we don't have a dangling pointer to this region.
			if (pRegion2 == LastRendered)
				// Start from the beginning of the list again.
				LastRendered = NULL;
		}

		// Find the next render region
		pRegion2 = pNextRegion;
	}
}
