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
//*/

#include "camtypes.h"
#include "saveeps.h"
#include "ccdc.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "osrndrgn.h"
//#include "simon.h"
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"
//#include "opscale.h"
//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "objchge.h"
#include "blobs.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfrech.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "rechgrp.h"
#include "cmxrendr.h"
//#include "cmxexdc.h"
#include "extender.h"
#include "nodeblnd.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodebldr.h"
//#include "capturemanager.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "phil.h"				// for _R(IDB_CACHE_MARK)
#include "lineattr.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "dibutil.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodeliveeffect.h"
#include "scanrr.h"
#include "pmaskrgn.h"
#include "blndhelp.h"
//#include "mrhbits.h"
#include "transop.h"
#include "opscale.h"

CC_IMPLEMENT_DYNAMIC(NodeGroup, NodeCompound)
CC_IMPLEMENT_DYNAMIC(NodeListItemWithDocPtr, NodeListItem)
CC_IMPLEMENT_DYNAMIC(GroupRecordHandler, CamelotRecordHandler)

// Give this file in memory dumps
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/*********************************************************************************************

>    NodeGroup::NodeGroup() 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    -
     Outputs:   
     Returns:   -
			  
     Purpose: This constructor creates a NodeGroup linked to no other, with all status
		  	  flags false and an uninitialised bounding rectangle.           
			
     Errors:    

**********************************************************************************************/
 
NodeGroup::NodeGroup(): NodeCompound()
{
	blenderNode = NULL;
}
	
/***********************************************************************************************

>   void NodeGroup::NodeGroup
	(
		Node* ContextNode,  
		AttachNodeDirection Direction,  
		BOOL Locked = FALSE, 
		BOOL Mangled = FALSE,  
		BOOL Marked = FALSE, 
		BOOL Selected = FALSE, 
	)

    Author:  Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created: 19/10/93             
    
    Inputs: ContextNode: Pointer to a node which this node is to be attached to.     
    
			Direction: 
			
				Specifies the direction in which this node is to be attached to the 
				ContextNode. The values this variable can take are as follows: 
								  
				PREV      : Attach node as a previous sibling of the context node
				NEXT      : Attach node as a next sibling of the context node
				FIRSTCHILD: Attach node as the first child of the context node
				LASTCHILD : Attach node as a last child of the context node                               
					      
			The remaining inputs specify the status of the node: 
			
			Locked:     Is node locked ?
			Mangled:    Is node mangled ?
			Marked:     Is node marked ?
			Selected:   Is node selected ?
			
    Outputs:   -
    Returns:   - 
    Purpose: This method initialises the node and links it to ContextNode in the
			 direction specified by Direction. All necessary tree links are
			 updated.     
			 
    Errors:  An assertion error will occur if ContextNode is NULL


***********************************************************************************************/

NodeGroup::NodeGroup(Node* ContextNode,  
					 AttachNodeDirection Direction,  
					 BOOL Locked, 
				 	 BOOL Mangled,  
					 BOOL Marked, 
					 BOOL Selected   
			   ):NodeCompound(ContextNode, Direction, Locked, Mangled, Marked, 
			 	Selected) 
{
	blenderNode = NULL;
} 



void NodeGroup::PreExportRender(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
	{
		// Output "start group" token
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
		pDC->OutputToken(_T("u"));
		pDC->OutputNewLine();
	}
PORTNOTE("cmx", "Removed use of CMXRenderRegion")
#ifndef EXCLUDE_FROM_XARALX
	else if(pRegion->IsKindOf(CC_RUNTIME_CLASS(CMXRenderRegion)))
	{
		// mark start of a group...
		CMXExportDC *pDC = (CMXExportDC *) pRegion->GetRenderDC();
		DocRect BBox = GetBoundingRect();
		pDC->StartGroup(&BBox);
	}
#endif
#endif
}

BOOL NodeGroup::ExportRender(RenderRegion* pRegion) 
{
#ifdef DO_EXPORT
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
	{
		// Output "end group" token
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
		pDC->OutputToken(_T("U"));
		pDC->OutputNewLine();
		
		// Tell caller we rendered ourselves ok
		return TRUE;
	}
PORTNOTE("cmx", "Removed use of CMXRenderRegion")
#ifndef EXCLUDE_FROM_XARALX
	else if(pRegion->IsKindOf(CC_RUNTIME_CLASS(CMXRenderRegion)))
	{
		// mark start of a group...
		CMXExportDC *pDC = (CMXExportDC *) pRegion->GetRenderDC();
		pDC->EndGroup();

		return TRUE;
	}
#endif
#endif
	// Render thid node in the normal way
	return FALSE;
}

/*********************************************************************************************

>    virtual BOOL NodeGroup::IsCompound(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a compound object, will return TRUE
     Purpose:   For determining quickly if the node is a compound object
     Errors:    

**********************************************************************************************/

// Phil, 22/10/2004. Implemented in Group.h where it will be automatically inlined
//BOOL NodeGroup::IsCompound() const
//{
//	return TRUE; // cos it is
//}
   


/********************************************************************************************

>	void NodeGroup::RenderAfterSubtree(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/2004
	Inputs:		pRender - The region to render into
	Purpose:	Capture the group as a cached bitmap

********************************************************************************************/

void NodeGroup::RenderAfterSubtree(RenderRegion* pRegion)
{
	// Deal with group transparency capture
	// Call Helper function to run all my cacheing functionality for me...
	// If we have Effect attrs applied then the user wants "tight group transparency"
	if (HasEffectAttrs())
		CaptureTight(pRegion);
	else
		CaptureCached(pRegion);
}



/********************************************************************************************

>	SubtreeRenderState NodeGroup::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/06/2004
	Inputs:		pRender - The region render into (or use as context for decisions about subtree)
				ppNextNode - Address of node pointer for next node to render or run to after this
				bClip - flag indicating whether to clip or not
	Purpose:	Do clever stuff on the way into a subtree, possibly modifying rendering
				behaviour.

********************************************************************************************/

SubtreeRenderState NodeGroup::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	if (pRender == NULL)							// If no render region supplied, assume we need to be rendered
		return SUBTREE_ROOTANDCHILDREN;

	// Go find out about my bounding rectangle
	DocRect BoundingRect = GetBoundingRect();
	DocRect ClipRect = pRender->GetClipRect();

	if (bClip && !ClipRect.IsIntersectedWith(BoundingRect))	// If not within the clipping rect then
		return SUBTREE_NORENDER;					// Don't render us or our children

	// Ask Helper function to set up cacheing for me...
	// If we have Effect attrs applied then the user wants "tight group transparency"
	if (HasEffectAttrs())
	{
		if (RenderTight(pRender))
			return SUBTREE_NORENDER;
	}
	else
	{
		if (RenderCached(pRender))	// If we can find a cached bitmap for this node and render it
		{
//TRACEUSER("Phil", _T("Skipping cached group %x %s\n"), this, GetRuntimeClass()->m_lpszClassName);
			return SUBTREE_NORENDER;					// Then tell the renderer to move on without doing any more...
		}
//TRACEUSER("Phil", _T("Re-rendering cached group %x %s\n"), this, GetRuntimeClass()->m_lpszClassName);
	}

	return SUBTREE_ROOTANDCHILDREN;					// Else we must render ourselves and our children
}



/********************************************************************************************

>	virtual BOOL NodeGroup::RenderTight(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/01/2005
	Returns:	TRUE if the node managed to render a cached version of itself
					then pNextNode contains the pointer to the node to continue rendering from
				FALSE if the node did not render a cached version of itself and thus
					normal rendering is required
	Purpose:	Protected Helper function
				Allow this node to render itself using cached data if it has it and for that
				cached data to represent an arbitrary section of the document (usually a subtree
				but can be more)

				For this implementation we assume that our cached info will represent just 
				this subtree.

				Also start to capture the rendered info into a cached bitmap if possible
	SeeAlso:	NodeRenderableBounded::CaptureCached

********************************************************************************************/

BOOL NodeGroup::RenderTight(RenderRegion* pRender)
{
	// If the caller wants his bitmap to respond to Effect Attributes
	// Then we have to behave quite differently than a normal cache/capture sequence
	// We must grab the bitmap regardless of the cache-enabling flag
	// We must grab at a controlled resolution
	// We must grab the whole thing
	// We must grab at 32BPP RGBT
	//
	// If the rendering quality is outline - no point showing cached bitmaps...
	if (pRender->RRQuality.GetFillQuality() <= Quality::Solid)
		return FALSE;

	// If the rendering system is doing black and white only then we can't sensibly
	// produce a monochrome version of the cached bitmap...
	if (pRender->IsVeryMono() || pRender->IsHitDetect())
		return FALSE;

	// Tight groups need to be rendered in the complex bitmap stages of printing
	if (pRender->IsKindOf(CC_RUNTIME_CLASS(ScanningRenderRegion)))
	{
		ScanningRenderRegion* pScanner = (ScanningRenderRegion*)pRender;
		pScanner->ComplexShapeFoundWrapper();

		return TRUE;	// Return TRUE so that rendering will NOT continue into the group normally
	}

	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache==NULL)
		return FALSE;

	// If this node is definitely not cached, skip the cache rendering stuff...
	DocRect cliprect = pRender->GetClipRect();
	DocRect bounds = GetBoundingRect();										// Assure new bounds are recalced if they were invalid
	BOOL bRendered = FALSE;

	CBitmapCacheKey inky(this, GetTightGroupPixelWidth(pRender));

	// No need to check BackmostChanged node here because we never capture
	// our background in a 24BPP bitmap we always capture at 32BPP using cfLOCKEDTRANSPARENT

	// Look for a cached bitmap at the appropriate pixel size...
	CCachedBitmap cbmp;
	BOOL bFound = pBitmapCache->Lookup(inky, cbmp);
	if (bFound)
	{
		// If cached bitmap covers the current clipped object rect, we can use it...
		DocRect clippedboundrect = cliprect.Intersection(bounds);
		if (cbmp.GetCachedRect().ContainsRect(clippedboundrect))
		{
//TRACEUSER("Phil", _T("Rendering cached bitmap for %x %s\n"), this, GetRuntimeClass()->GetClassName() );
			bRendered = pRender->RenderBits(cbmp.pbmpInfo, cbmp.pbmpBits, &cbmp.coord0, 3, TRUE, this);
		}
	}

	if (bRendered)
		return bRendered;

	// If we couldn't find or render a cached bitmap then try to cache a new one
//	double ScaledPixelWidth = pRender->GetScaledPixelWidth();

	// Work out how much of the object we propose to capture
	DocRect viewrect = cliprect;
	DocRect CaptureRect = bounds;

	if (!pRender->IsPrinting())
	{
		View* pView = pRender->GetRenderView();
		Spread* pSpread = pRender->GetRenderSpread();
		if (pView && pSpread)
		{
			viewrect = pView->GetDocViewRect(pSpread);
			pSpread->DocCoordToSpreadCoord(&viewrect);
			if (!viewrect.IsValid() || !viewrect.ContainsRect(cliprect))
				viewrect = cliprect;
		}
	}

	CaptureRect = CaptureRect.Intersection(viewrect);				// Proposed capture area is obj x viewport

	// If our capture rect is invalid (no intersection) then don't do any rendering
	if (!CaptureRect.IsValid())
		return(TRUE);

	if (!pRender->IsPrinting())
	{
		// If we're nearly going to capture the whole object
		// (Compare area of bounding rect with area of view rect so that bitmap sizes don't get out of control)
		XLONG areaBounds = (XLONG)bounds.Width()*(XLONG)bounds.Height();
		XLONG area2View = (XLONG)2*(XLONG)viewrect.Width()*(XLONG)viewrect.Height();
		if (areaBounds < area2View)
		{
			// Then grab the whole thing
			// (Bounding Rect is small enough for us to capture the whole object ignoring clipping rect)
			CaptureRect = bounds;
		}
	}

	// Inflate by one pixel all round to allow for anti-aliasing effects at the edges
	CaptureRect.Inflate((INT32)GetTightGroupPixelWidth(pRender));

	BOOL bFullCoverage = CaptureRect.ContainsRect(bounds);

	// Finally start the Capture @ 32BPP
	CaptureFlags caFlags = CaptureFlags(cfLOCKEDTRANSPARENT | cfUNCLIP | (bFullCoverage ? cfFULLCOVERAGE : cfNONE));
//TRACEUSER("Phil", _T("Starting TightGroup Capture %x, %f\n"), this, GetTightGroupPixelsPerInch(pRender));
	pRender->StartCapture(this, CaptureRect, CAPTUREINFO(ctNESTABLE, caFlags), TRUE, FALSE, GetTightGroupPixelsPerInch(pRender));

	return bRendered;
}




/********************************************************************************************

>	virtual BOOL NodeGroup::CaptureTight(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/01/2005
	Returns:	TRUE if captured data was cached
	Purpose:	Protected Helper function
				Use the CaptureManager to capture the results of rendering, cache them
				and associate them with this Ink node
	SeeAlso:	NodeRenderableBounded::RenderCached

********************************************************************************************/

BOOL NodeGroup::CaptureTight(RenderRegion* pRender)
{
	// If the caller wants his bitmap to respond to Effect Attributes
	// Then we have to behave quite differently than a normal cache/capture sequence
	// We must grab the bitmap regardless of the cache-enabling flag
	// We must grab at a controlled resolution
	// We must grab the whole thing
	// We must grab at 32BPP RGBT
	Capture* pCapture = pRender->GetTopCapture();
	if (pCapture==NULL)											// If nothing was captured
		return FALSE;											// Then do nothing

	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache==NULL)
	{
		return FALSE;
	}

//	ERROR3IF(IsDragged() || m_bCachedBitmapShared, "CaptureTight asked to capture when its dragged");
//	ERROR3IF(IsDragged(), "CaptureTight asked to capture when its dragged");

	// Only stop the capture if we started it
	// (means we only capture whole subtrees at the mo.)
	BOOL bCached = FALSE;
	if (pCapture->GetOwner()==this)
	{
		// End this capture:
		// Blit capture to screen
		// Retain bitmap because we will release it ourselves onyl if we fail to cache it
		LPBITMAPINFO lpInfo = NULL;
		LPBYTE lpBits = NULL;
		DocRect CaptureRect = GetBoundingRect();				// Set maximum size we allow
		BOOL bFullCoverage = pCapture->HasFullCoverage();
		pRender->StopCapture(this, FALSE, FALSE, &lpInfo, &lpBits, &CaptureRect);

		// If the capture gave us back a bitmap, try to cache it
		if (lpInfo && lpBits && CaptureRect.IsValid())
		{
//TRACEUSER("Phil", _T("Ending TightGroup Capture SUCCEEDED %x, %f\n"), this, GetTightGroupPixelWidth(pRender));
			// Cache the ORIGINAL bitmap as Option 0
			// See also, SetOriginalBitmap
			double PixelWidth = GetTightGroupPixelWidth(pRender);
			CBitmapCacheKey inky(this, PixelWidth, 0);

			CCachedBitmap cbmp;

			cbmp.pbmpBits = lpBits;
			cbmp.pbmpInfo = lpInfo;
			cbmp.SetCachedRect(CaptureRect);
			cbmp.nPriority = CACHEPRIORITY_TEMPBITMAP_HIGH;
			cbmp.bFullCoverage = bFullCoverage;

			if ( cbmp.IsValid() 
				 && !pRender->IsKindOf(CC_RUNTIME_CLASS(PrintingMaskedRenderRegion))
				)
			{
				pBitmapCache->StoreBitmap(inky, cbmp);
				MayBeCached = TRUE;
				bCached = TRUE;
			}

			// Render the bitmap taking effect attrs into account...
			if (lpInfo && lpBits)
				pRender->RenderBits(lpInfo, lpBits, CaptureRect, TRUE, this);
		}

		// If we failed to cache the captured bitmap then release it
		if (lpInfo!=NULL && lpBits!=NULL && !bCached)
		{
//TRACEUSER("Phil", _T("Ending TightGroup Capture FAILED %x, %f\n"), this, GetTightGroupPixelWidth(pRender));
			FreeDIB(lpInfo, lpBits, NULL, FALSE);
		}
	}

	return bCached;
}

/********************************************************************************************

>	virtual double NodeGroup::GetTightGroupPixelsPerInch(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/2005
	Inputs:		-
	Returns:	pixel width for capturing
	Purpose:	Get width of pixels for use in capturing this group as a tight group bitmap

********************************************************************************************/

double NodeGroup::GetTightGroupPixelsPerInch(RenderRegion* pRender) const
{
	double dPPI = 0.0;

	if (NodeBitmapEffect::GroupPixelsPerInch==0)
	{
		// 0 means automatic
		// Automatic means get resolution in context
		// Context means either respond to a fixed resolution above us in the tree
		// or get resolution from current view
		double dViewPPI = 96.0;
		double dViewScaledPPI = 96.0;
		if (pRender==NULL)
		{
			View* pView = View::GetCurrent();
			if (pView)
				dViewPPI = 72000.0 / pView->GetPixelWidth().MakeDouble();
				dViewScaledPPI = 72000.0 / pView->GetScaledPixelWidth().MakeDouble();
		}
		else
		{
			dViewPPI = pRender->GetPixelsPerInch();
			dViewScaledPPI = 72000.0 / pRender->GetScaledPixelWidth();
		}

		Node* pParent = this->FindParent();
		while (pParent && !pParent->IsLayer())
		{
			if (pParent->IsBitmapEffect())
			{
				dPPI = ((NodeBitmapEffect*)pParent)->GetPixelsPerInchValue();
				if (dPPI!=0)
					break;

				// If we found an effect at all then ensure we use default effect PPI
				dPPI = dViewPPI;
			}

			pParent = pParent->FindParent();
		}

		// If we found no effects above us then try to operate at zoomed view res
		// for best results on screen.
		if (dPPI==0)
		{
			// See also OpEffectLock::DoEffectNodeOp
			// We should capture at at least 200dpi for printing
			// We should capture at a size that's at least as good as the current zoomed pixel size
			// (We rely on the capture being clipped to keep bitmap sizes under control at high zoom factors)
			// (We should reduce the resolution if the bitmap will be huge...)
			dPPI = dViewScaledPPI;
		}
	}

	// Try to limit bitmap size if dpi is getting large inside a wrapped
	// RenderRegion.
	// Without this restriction, tight groups inside shadows become a problem
	// because this function does not pick up the reduced resolution of the
	// offscreen render region used by the shadow to create its silhouette.
	if (pRender && pRender->IsWrappedRender() && dPPI>480)
	{
		dPPI = 480;
	}

	if (dPPI==0)
		dPPI = NodeBitmapEffect::GroupPixelsPerInch;

	if (dPPI==0)
	{
		// Shouldn't ever reach this clause but just in case...
		ERROR3("GetPixelsPerInch can't get sensible PPI so defaulting to 96");
		dPPI = 96.0;
	}

TRACEUSER("Phil", _T("GTGPPI %f\n"), dPPI);
	return dPPI;
}

/********************************************************************************************

>	double NodeGroup::GetTightGroupPixelWidth(RenderRegion* pRender) const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/2005
	Inputs:		-
	Returns:	pixel width for capturing
	Purpose:	Get width of pixels for use in capturing this group as a tight group bitmap

********************************************************************************************/

/*double NodeGroup::GetTightGroupPixelWidth(RenderRegion* pRender) const
{
	return 72000.0/GetTightGroupPixelsPerInch(pRender);
}*/


/*double NodeGroup::GetTightGroupPixelWidth(RenderRegion* pRender) const
{
	// We should capture at at least 200dpi for printing
	// We should capture at a size that's at least as good as the current zoomed pixel size
	// (We rely on the capture being clipped to keep bitmap sizes under control at high zoom factors)
	// (We should reduce the resolution if the bitmap will be huge...)
	double pixwidth = 72000.0/200.0;
	double scaledwidth = 72000.0/200.0;

	View* pView = View::GetCurrent();
	if (pView!=NULL)
		scaledwidth = pView->GetScaledPixelWidth().MakeDouble();

	// If our proposed pixels are bigger than screen pixels we'll use screen pixel width instead
	if (pixwidth > scaledwidth)
		pixwidth = scaledwidth;

	return pixwidth;
}*/	
	
/********************************************************************************************

>	void NodeGroup::IsValidEffectAttr(CCRuntimeClass* pAttrClass) const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/2005
	Inputs:		Attribute class
	Returns:	TRUE if the attr class is a valid effect attr
	Purpose:	Determine whether this type of attribute can be an effect attribute
				On this node at this time.

********************************************************************************************/

/*BOOL NodeGroup::IsValidEffectAttr(CCRuntimeClass* pAttrClass) const
{
	return (AttrFillGeometry::s_bGroupTransparency &&
			(pAttr->IsATranspFill() ||
			 pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrTranspChange)) ||
			 pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeTranspChange)
			 )
			);
}*/

	
	
	
/********************************************************************************************

>	void NodeGroup::IsValidEffectAttr(NodeAttribute* pAttr) const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/2005
	Inputs:		Attribute
	Returns:	TRUE if the attr is a valid effect attr
	Purpose:	Determine whether this attribute instance can be an effect attribute
				On this node at this time.

********************************************************************************************/

BOOL NodeGroup::IsValidEffectAttr(NodeAttribute* pAttr) const
{
//	return (AttrFillGeometry::s_bGroupTransparency &&					// If pref allows group transparency
//			(pAttr->IsATranspFill() ||									// And the attribute is a transparency type
//			 pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrTranspChange)) ||		// Return TRUE to apply the attr as an effect
//			 pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeTranspChange))
//			 )
//			);
	CCRuntimeClass* pAttrType = pAttr->GetAttributeType();

	return (AttrFillGeometry::s_bGroupTransparency &&					// If pref allows group transparency
			(pAttr->IsATranspFill() ||
			pAttrType == CC_RUNTIME_CLASS(AttrTranspFillGeometry) || //->IsKindOf(CC_RUNTIME_CLASS(AttrTranspChange)) ||
			pAttrType == CC_RUNTIME_CLASS(AttrStrokeTransp) || //pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeTranspChange)) ||
			pAttrType == CC_RUNTIME_CLASS(AttrTranspFillMapping)
			)
		   );
}




/***********************************************************************************************

>	virtual void NodeGroup::Render(RenderRegion* pRRegion)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/2005
	Inputs:		Pointer to a render region
	Purpose:	Will render the group iff we have it cached - for hit detection reasons

***********************************************************************************************/

// NOTE: No need for a render function because this effect does not add any pixel data
// to those rendered by its children - just let the children render themselves into
// the hit-detection bitmap
/*
void NodeGroup::Render(RenderRegion* pRender)
{
	// Tight groups need to be rendered in the complex bitmap stages of printing
	if (HasEffectAttrs() && pRender->IsKindOf(CC_RUNTIME_CLASS(ScanningRenderRegion)))
	{
		ScanningRenderRegion* pScanner = (ScanningRenderRegion*)pRender;
		pScanner->ComplexShapeFoundWrapper();
	}
}*/
/*void NodeGroup::Render(RenderRegion* pRender)
{
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache!=NULL && HasEffectAttrs() && pRender->IsHitDetect())
	{
		CBitmapCacheKey inky(this, GetTightGroupPixelWidth(pRender));
		CCachedBitmap cbmp;
		BOOL bFound = pBitmapCache->Lookup(inky, cbmp);
		if (bFound)
			pRender->RenderBits(cbmp.pbmpInfo, cbmp.pbmpBits, &cbmp.coord0, 3, TRUE, this);
	}
}*/




/********************************************************************************************

>	void NodeGroup::RenderObjectBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		pRender - The region to draw the blobs in
	Purpose:	Renders the object blobs

********************************************************************************************/

void NodeGroup::RenderObjectBlobs(RenderRegion* pRegion)
{
	NodeCompound::RenderObjectBlobs(pRegion);

#if !defined(EXCLUDE_FROM_RALPH)
	// Find out about the groups bounding rect
	DocRect BoundingRect = GetBoundingRect();

	// Find out where to draw the blobs
	DocCoord Low  = BoundingRect.LowCorner();
	DocCoord High = BoundingRect.HighCorner();

	// Set the colours of the blobs
	pRegion->SetFillColour(COLOUR_UNSELECTEDBLOB);
	pRegion->SetLineColour(COLOUR_NONE);

	// Draw all the blobs
	pRegion->DrawBlob(Low, BT_UNSELECTED);  
	pRegion->DrawBlob(High, BT_UNSELECTED); 
	pRegion->DrawBlob(DocCoord(Low.x, High.y), BT_UNSELECTED); 
	pRegion->DrawBlob(DocCoord(High.x, Low.y), BT_UNSELECTED); 

#endif
}



/********************************************************************************************

>	void NodeGroup::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		pRender - The region to draw the blobs in
	Purpose:	Renders the tiny blobs for a group (A Single blob on the topmost child object
				in the group)

********************************************************************************************/

void NodeGroup::RenderTinyBlobs(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (NodeRenderableBounded::bShowCacheBlobs)
	{
		// To help see what the cacheing system is doing
		// If this node has a cached bitmap associated with it
		// Render a special blob to show that...
		CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
		if (pBitmapCache && MayBeCached && !HasEffectAttrs())
		{
			CBitmapCacheKey inky(this, pRegion->GetScaledPixelWidth());			//GetScaledPixelWidthDouble?
			CCachedBitmap cbmp;
			BOOL bFound = pBitmapCache->Lookup(inky, cbmp);
			if (bFound)
			{
				// Find out about the groups bounding rect
				DocRect BoundingRect = GetBoundingRect();

				// Draw a blob
				if (cbmp.IsTransparent())
					pRegion->DrawBitmapBlob(DocCoord(BoundingRect.hi.x - pRegion->GetScaledPixelWidth(), BoundingRect.lo.y - pRegion->GetScaledPixelWidth()), _R(IDB_CACHE_MARK_32));
				else
					pRegion->DrawBitmapBlob(DocCoord(BoundingRect.hi.x - pRegion->GetScaledPixelWidth(), BoundingRect.lo.y - pRegion->GetScaledPixelWidth()), _R(IDB_CACHE_MARK_24));
			}
		}
	}

	// get the topmost object in this group and tell it to render its tiny blobs.
	Node* pNode = FindLastChild();
	while (pNode != NULL && !pNode->IsAnObject())
		pNode = pNode->FindPrevious();

	// empty groups are not allowed!
	if (pNode == NULL)
	{
		ERROR3("NodeGroup::RenderTinyBlobs; This group is empty! Shouldn't be!");
		return;
	}
	else
	{
		((NodeRenderableInk*)pNode)->RenderTinyBlobs(pRegion);
	}

#endif
}


/********************************************************************************************

>	void NodeGroup::Transform( TransformBase& Trans )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		Trans - A transformation object
	Purpose:	Transforms the group object and all its children.

********************************************************************************************/

void NodeGroup::Transform( TransformBase& Trans )
{
	View* pView = View::GetCurrent();
	if (GroupCanTransformCached(Trans) && pView)
//	if (GroupCanTransformCached() && pView)
	{
		// Note! View::GetScaledPixelWidth returns an unrounded fractional value where
		// RenderRegion::GetScaledPixelWidth return a rounded integer!!!
		//
		// Note! It would be better not to call HasEffectAttrs() here because it can be slow in large groups
		if (HasEffectAttrs())
			TransformTight(Trans, GetTightGroupPixelsPerInch());
		else
			TransformCached(Trans, (MILLIPOINT)(pView->GetScaledPixelWidth().MakeDouble()+0.5));
	}
	else
	{
		// Our cached data can't be transformed so we must remove it because
		// it will now be out of date
		// We must do it directly because ReleaseCached does conditional
		// stuff based on IsDragged
//TRACEUSER("Phil", _T("NodeGroup::Transform releasing cache\n"));
		CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
		if (pBitmapCache!=NULL)
		{
			CBitmapCacheKey inky(this, 42);
			pBitmapCache->RemoveAllOwnedBitmaps(inky);
			MayBeCached = FALSE;
			Trans.bHaveTransformedCached = FALSE;
			Trans.bTransformYourChildren = TRUE;
		}
	}

	// Optimisation:
	// If we have transformed our cached data 
	//	And we're dragging 
	//	And we were holding cached data (sanity check)
	//  (And /assuming/ that the cached data fully represents the child objects!!!)
	//
	// Then we can avoid transforming our children to save time
	//
	// This is safe because we know that the dragged objects will not remain permanently in the
	// document so it doesn't matter if they're not up to date...
	//
	BOOL bTransCache = Trans.bHaveTransformedCached;
	if (Trans.bTransformYourChildren)
	{
		// Transform all the children
		NodeRenderableInk::TransformChildren(Trans);
	}
	else
	{
		// We can get away with just transforming our effect attrs
		TransformEffectAttrs(Trans);
		Trans.bHaveTransformedChildren = FALSE;
	}
	Trans.bHaveTransformedCached = bTransCache;

	// If we have successfully transformed the cached data for this
	// contour then we can simply transform the bounds to suit...
	// (Some derived classes might not be able to compute bounds
	//  if their children have not been transformed.)
	// (Bounds have to be correct for RenderCached to allow the cached
	//  bitmap to be used during dragging.)
	//
	// We can only do this for translations at the moment
	//
	if (Trans.bHaveTransformedCached && Trans.IsTranslation())
	{
		Trans.Transform(&BoundingRectangle.lo, 1);
		Trans.Transform(&BoundingRectangle.hi, 1);
/*		The following code doesn't work because it keeps on transforming
		the results of previous transformed bounding rects, iteratively getting
		bigger and bigger.
		It needs to be able to work on the original bounding rect all the time
		DocCoord temp[4];
		temp[0] = BoundingRectangle.lo;
		temp[1] = BoundingRectangle.hi;
		temp[2] = DocCoord(BoundingRectangle.lox, BoundingRectangle.hiy);
		temp[3] = DocCoord(BoundingRectangle.hix, BoundingRectangle.loy);

		Trans.Transform(temp, 4);

		BoundingRectangle.lo = temp[0];
		BoundingRectangle.hi = temp[0];
		BoundingRectangle.IncludePoint(temp[1]);
		BoundingRectangle.IncludePoint(temp[2]);
		BoundingRectangle.IncludePoint(temp[3]);
*/
	}
	else
	{
		// The groups bounding rect is no longer valid
//		ERROR3IF(!IS_A(this, NodeGroup) && !Trans.bHaveTransformedChildren, "Compound children not transformed but compound bounding rect depends on them\n");
		InvalidateBoundingRect();
	}

	// --------------------------------------------------------------------
	// CGS:  We need to check for a blend on a path here ....
	if (IS_A (this, NodeBlend))
	{
		NodeBlend* ptrBlend = (NodeBlend*) this;
		NodeBlender* ptrBlender = (NodeBlender*) ptrBlend->FindFirstBlender ();

		if (ptrBlender)
		{
			ptrBlender->UpdateBlendStartAngles ((Trans2DMatrix&) Trans);

			BOOL done = FALSE;
			
			while (!done)
			{
				ptrBlender = ptrBlend->FindNextBlender (ptrBlender);
				
				if (!ptrBlender)
				{
					done = TRUE;
				}
				else
				{
					ptrBlender->UpdateBlendStartAngles ((Trans2DMatrix&) Trans);
				}
			}
		}
	}
}




/********************************************************************************************

>	virtual void NodeGroup::TransformTight(TransformBase& Trans, double dTestPixelWidth)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/2004
	Inputs:		Trans - Transformation
				dPixelWidth - PixelWidth to be tested specifically to control Trans.bHaveTransformedCached
	Returns:	-
	Purpose:	Transform all the cached bitmaps associated with this node
	SeeAlso:	NodeRenderableInk::RenderCached, CaptureCached

********************************************************************************************/

void NodeGroup::TransformTight(TransformBase& Trans, double dTestPixelWidth)
{
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();

	CBitmapCacheKey inky(this, 42);
	CCacheKeyMap::iterator pos = pBitmapCache->GetStartPosition();
	CCachedBitmap abitmap = pBitmapCache->FindNextOwnedBitmap(pos, inky);
	MayBeCached = abitmap.IsValid();						// Update MayBeCached here because we can
	BOOL bTransformedTested = FALSE;
	while (abitmap.IsValid())
	{
		abitmap.Transform(Trans);
		pBitmapCache->StoreBitmap(inky, abitmap);

		if (inky.GetPixelWidth() == dTestPixelWidth && abitmap.IsTransparent())
			bTransformedTested = TRUE;

		abitmap = pBitmapCache->FindNextOwnedBitmap(pos, inky);
	}

	// We can only continue to transform cached things if all our bitmaps are transparent (32BPP)
	// And if we actually had some cached data to transform
	if (!bTransformedTested)
	{
		Trans.bHaveTransformedCached = FALSE;
		Trans.bTransformYourChildren = TRUE;
	}
}




/********************************************************************************************

>	virtual DocRect NodeGroup::GetEorDragBoundingRect()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/2000
	Inputs:		None
	Returns:	The eor drag bounding rect for all of my children
	Purpose:	Gets the eor drag bounding rect for all my children
	SeeAlso:	-

********************************************************************************************/
DocRect NodeGroup::GetEorDragBoundingRect()
{
	Node * pChild = FindFirstChild();

	DocRect dr(0,0,0,0);

	while (pChild)
	{
		if (pChild->IsBounded())
		{
			dr = dr.Union(((NodeRenderableBounded *)pChild)->GetEorDragBoundingRect());
		}

		pChild = pChild->FindNext();
	}

	return dr;
}

/********************************************************************************************

>	DocRect NodeGroup::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		DontUseAttrs - TRUE if the attrs associated with the node should be
				ignored. defaults to FALSE
				HitTest      - TRUE if being called during HitTest
	Returns:	The Groups bounding rect
	Purpose:	If the groups bounding rect is known then it is returned. If not, it is
				re-calculated.

********************************************************************************************/

DocRect NodeGroup::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	if (!IsBoundingRectValid || DontUseAttrs)
	{
		// Optimisation:
		// If we're holding cached data that is a full representation of our children
		//	And we're being dragged
		//	And the bounding box should include attrs (the cached data includes attrs)
		//
		// Then we can avoid calling our children recursively to get their bounding
		// rectangles because the info is all contained in our cached data's bounding
		// rectangle.
		//
		if (MayBeCached && IsDragged() && !DontUseAttrs)
		{
			CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
			CCachedBitmap cbmp;
			BOOL bFound = FALSE;
			if (HasEffectAttrs())
			{
				CBitmapCacheKey inky(this, GetTightGroupPixelWidth());
				bFound = pBitmapCache->Lookup(inky, cbmp);
				bFound = bFound && cbmp.bFullCoverage;
			}
			else
			{
				View* pCurrentView = View::GetCurrent();
				if (pCurrentView)
				{
					// Note! View::GetScaledPixelWidth returns an unrounded fractional value where
					// RenderRegion::GetScaledPixelWidth return a rounded integer!!!
					CBitmapCacheKey inky(this, (MILLIPOINT)(pCurrentView->GetScaledPixelWidth().MakeDouble()+0.5));
					bFound = pBitmapCache->Lookup(inky, cbmp);
					bFound = bFound && cbmp.bFullCoverage;
				}
			}
			if (bFound)
			{
				BoundingRectangle = cbmp.GetCachedRect();
				IsBoundingRectValid = TRUE;
				return BoundingRectangle;
			}
		}

		// Find the groups first child
		DocRect NewRect(0,0,0,0);
		Node* pNode = FindFirstChild();
		
		while (pNode!=NULL)
		{
			// if this node is bounded, union its bounds with the others
			if (pNode->IsBounded())
				NewRect = NewRect.Union(((NodeRenderableBounded*)pNode)->GetBoundingRect(DontUseAttrs));

			// Get the next node in the list
			pNode = pNode->FindNext();
		}

		if (DontUseAttrs)
		{
			// just return this rectangle as it is not the nodes true bounding rect
			return NewRect;
		}

		// copy the new docrect into the node bounding rect
		BoundingRectangle = NewRect;

		// Mark the bounding rect as valid
		IsBoundingRectValid = TRUE;
	}

	// Return the resulting bounding rect
	return BoundingRectangle;
}


/********************************************************************************************
>	DocRect NodeGroup::GetBlobBoundingRect()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com> (re-written)
	Created:	29/6/95
	Returns:	DocRect - The bounding rect of the group and its blobs
	Purpose:	Finds the bounding rect of the group along with its blobs. Its main use is
				to determine which areas of the document to invalidate
********************************************************************************************/

DocRect NodeGroup::GetBlobBoundingRect()
{
	DocRect NewRect = DocRect(0,0,0,0);

#if !defined(EXCLUDE_FROM_RALPH)
	// Find the Blob manager
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr!= NULL)
	{
		BlobStyle VisibleBlobs = pBlobMgr->GetCurrentInterest(TRUE);
		if (VisibleBlobs.Object)
		{
			DocRect BoundingRect = GetBoundingRect();

			// Object blobs are visible, so include blob in each
			// corner of the group bounds
			DocRect BlobSize;

			// Find out where the blobs	will be drawn
			DocCoord Low  = BoundingRect.LowCorner();
			DocCoord High = BoundingRect.HighCorner();

			// Include the object blob in each corner
			pBlobMgr->GetBlobRect(Low, &BlobSize);
			NewRect = NewRect.Union(BlobSize);
			pBlobMgr->GetBlobRect(High, &BlobSize);
			NewRect = NewRect.Union(BlobSize);
			pBlobMgr->GetBlobRect(DocCoord(Low.x, High.y), &BlobSize);
			NewRect = NewRect.Union(BlobSize);
			pBlobMgr->GetBlobRect(DocCoord(High.x, Low.y), &BlobSize);
			NewRect = NewRect.Union(BlobSize);
		}
	}

	Node* pNode=FindFirstChild();
	while (pNode!=NULL)
	{
		if (pNode->IsBounded())
			NewRect = NewRect.Union(((NodeRenderableBounded*)pNode)->GetBlobBoundingRect());
		pNode = pNode->FindNext();
	}

	IncludeChildrensBoundingRects(&NewRect);
#endif
	return NewRect;
}


/********************************************************************************************

>	virtual String NodeGroup::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/93
	Inputs:		Plural: Flag indicating if the string description should be plural or
						singular. 
	Outputs:	-
	Retuns:		Description of the group node 
	Purpose:	To return a description of the Group object in either the singular or the 
				plural. This method is called by the DescribeRange method.
				
				The description will always begin with a lower case letter.   
				
	Errors:		(Need to do this)
	SeeAlso:	-

********************************************************************************************/

String NodeGroup::Describe(BOOL Plural, BOOL Verbose) 
{     
	if (Plural)
		return(String(_R(IDS_GROUP_DESCRP)));  
	else
		return(String(_R(IDS_GROUP_DESCRS))); 
}; 


/***********************************************************************************************
> Node* NodeGroup::SimpleCopy()  

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	Inputs:     -  
    Outputs:    
    Returns:    A copy of the node, or NULL if memory has run out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	            
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.                                                                      
                                                                                 
**********************************************************************************************/

Node* NodeGroup::SimpleCopy()
{
	NodeGroup* NodeCopy; 
	NodeCopy = new NodeGroup();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return (NodeCopy);
}   

   
/***********************************************************************************************

>	void NodeGroup::CopyNodeContents(NodeGroup* pCopyOfNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/2004
	Inputs:		pCopyOfNode - The node to copy data to
	Outputs:	-
	Returns:	-
	Purpose:	Copies the data from this node to pCopyOfNode by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeGroup::CopyNodeContents

***********************************************************************************************/

void NodeGroup::CopyNodeContents(NodeGroup* pCopyOfNode)
{
	NodeRenderableInk::CopyNodeContents(pCopyOfNode);
	pCopyOfNode->m_bBlendStartNode = m_bBlendStartNode;			// NodeController?
	pCopyOfNode->m_bBlendEndNode   = m_bBlendEndNode;				// NodeController?
	pCopyOfNode->m_pBlendCreatedByNode = m_pBlendCreatedByNode;	// NodeController?
	CopyCached(pCopyOfNode);							// Copy cached bitmaps
}


/***********************************************************************************************
>   void NodeGroup::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    22/10/2004
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeGroup::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeGroup), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeGroup))
		CopyNodeContents((NodeGroup*)pNodeCopy);
}


/********************************************************************************************

>	virtual BOOL NodeGroup::CloneNode(Node** ppNodeCopy, BOOL bLightweight)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/2005
	Inputs:		bLightweight - Only copy what you really need,
							   Make references to original large data items instead of copying them
							   if possible
	Outputs:	NodeCopy: A deep copy of the node if Successful
	Returns:	- 
    Purpose:    This method outputs a 'deep' copy of the node. It is the same as CopyNode 
				except that the copy is not linked into the tree. 
				Override if your class can interpret the lightweight flag

	Errors:		-
	SeeAlso:	Node::CopyNode

********************************************************************************************/

/*BOOL NodeGroup::CloneNode(Node** ppNodeCopy, BOOL bLightweight)
{
	// Don't let derived classes be copied as NodeGroups
	// (There must be a better way to do this!)
	if (!IS_A(this, NodeGroup))
		return NodeCompound::CloneNode(ppNodeCopy, bLightweight);

	// TODO: Do clever stuff with bLightWeight!!!!
	NodeGroup* pNewGroup = new NodeGroup();
	ERRORIF(pNewGroup == NULL, _R(IDE_NOMORE_MEMORY), FALSE);

	NodeRenderableInk::CopyNodeContents(pNewGroup);
//	CopyCached(pNewGroup, bLightweight);					// Copy cached bitmaps
	CopyCached(pNewGroup);									// Copy cached bitmaps

	// Possible optimisation:
	// If a lightweight copy is requested and GroupCanTransformCached==TRUE
	// and we find we have a cached bitmap that will be used during dragging
	// then we could possibly avoid copying the children!
	//
	if (!CopyChildren(Child, pNewGroup, ccALL))				// Out of memory 
	{
		DeleteChildren(pNewGroup->FindFirstChild());
		delete pNewGroup;
		return FALSE;
	}

	*ppNodeCopy = pNewGroup;
	return TRUE;
}
*/




/********************************************************************************************

>	virtual UINT32 NodeGroup::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeGroup::GetNodeSize() const 
{     
	return (sizeof(NodeGroup)); 
}  


/********************************************************************************************

>	BOOL NodeGroup::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						     Spread* pSpread )

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/09/94
	Inputs:		PointerPos - The Location of the mouse pointer at the time of the click
				Click - The type of click received (single, double, drag etc)
				ClickMods - The modifiers to the click (eg shift, control etc )
	Returns:	TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Determines if the user has started a drag on one of the groups blobs.
				If they have then it starts the groups resize operation
		
********************************************************************************************/

BOOL NodeGroup::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find a view and if we failed to find a view, then just don't claim the click
	DocView *pDocView = DocView::GetSelected();
	if (pDocView==NULL)
		return FALSE;

	// we only handle the click if we can confirm that object blobs are being displayed.
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr == NULL)
		return FALSE;
	if (!pBlobMgr->GetCurrentInterest().Object)
		return FALSE;

	// Find the DocRect of blob size around the point that was clicked on 
	DocRect MouseBlob;
	OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), PointerPos, BT_SELECTEDLARGEST, &MouseBlob);

	INT32 StartBlob = 0; 

	// If the user has clicked on a blob we will need to set the centre of rotation to the 
	// oposite corner
	DocCoord OpCorner;
	 
	// Has the user clicked on one of the group's blobs ?
	if	(MouseBlob.ContainsCoord(BoundingRectangle.lo)) 	 // Bottom left
	{
		StartBlob = 6; 
		OpCorner = BoundingRectangle.hi; 
	}
	else if	(MouseBlob.ContainsCoord(BoundingRectangle.hi)) 	 // Top right
	{
		StartBlob = 3; 
		OpCorner = BoundingRectangle.lo; 

	}
	else if	(MouseBlob.ContainsCoord(DocCoord(BoundingRectangle.lo.x,BoundingRectangle.hi.y)))  // Top left
	{
		StartBlob = 1; 
		OpCorner = DocCoord(BoundingRectangle.hi.x,BoundingRectangle.lo.y);
	}
	else if	(MouseBlob.ContainsCoord(DocCoord(BoundingRectangle.hi.x,BoundingRectangle.lo.y))) // Bottom right
	{
		StartBlob = 8; 
		OpCorner = DocCoord(BoundingRectangle.lo.x,BoundingRectangle.hi.y);

	}
	if (StartBlob != 0)
	{
		// User has clicked on a blob 
		if (Click==CLICKTYPE_DRAG)
		{	
			// Try to create the operation to scale the group
			OpScaleTrans* pOp = new OpScaleTrans(); 
			if (pOp == NULL)
			{
				InformError(); 
				return TRUE; // Claim the click though
			}

			// Create a range containing only this object
//			RangeControl RngCtl = { TRUE, FALSE, FALSE }; 
			
			static TransformData TransData; 

			// Setup the transform info 
			TransData.CentreOfTrans = OpCorner;
			TransData.StartBlob = StartBlob;
			TransData.LockAspect = FALSE; 
			TransData.LeaveCopy = FALSE; 
			TransData.ScaleLines = TRUE; 
			TransData.TransFills = TRUE;
			TransData.pRange = 0; 

			TransformBoundingData BoundingData;
			BoundingData.x 		  = BoundingRectangle.lo.x;
			BoundingData.y 		  = BoundingRectangle.lo.y;
			BoundingData.Width    = BoundingRectangle.Width();
			BoundingData.Height   = BoundingRectangle.Height();
			BoundingData.XScale   = (FIXED16) 1;
			BoundingData.YScale   = (FIXED16) 1;
			BoundingData.Rotation = (ANGLE) 0;
			BoundingData.Shear 	  = (ANGLE) 0;


			// Start the drag 
			pOp->DragStarted(&TransData, 
							 NULL, // Don't report to any tool
							 &BoundingData, // Must give bounds data or call fails
							 PointerPos, 
							 pSpread,
							 ClickMods,
							 DocCoord(0,0),
							 this
							 ); 


		}		
		// we have used that click up
		return TRUE;
	}
#endif
	// we did not use the click, so let someone else try
	return FALSE;
}

/********************************************************************************************

>	virtual ChangeCode NodeGroup::OnChildChange(ObjChangeParam* pParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/95
	Inputs:		pParam	= pointer to a object change parameter class
	Returns:	CC_OK		if we have successfully processed the change.
				CC_FAIL		if we cannot handle this particular change and must prevent the
							child from continuing
	Purpose:	This function should be overridden in derived object classes.
				Composite objects can use this function to respond to one of their children
				undergoing a change. They should return CC_FAIL whenever they are unable to
				cope with the change.

				Groups will hide themselves if the op has left it in a state where it has no bounds,
				i.e. all it's node renderable bounded children are gone.

	SeeAlso:	WarnParentOfChange(),AllowOp();

********************************************************************************************/


ChangeCode NodeGroup::OnChildChange(ObjChangeParam* pParam)
{
	ERROR2IF(pParam == NULL,CC_FAIL,"pParam == NULL");

	ObjChangeType cType = pParam->GetChangeType();

	if (cType == OBJCHANGE_FINISHED)
	{
		InvalidateBoundingRect();
		// The groups 
		DocRect Rect = GetBoundingRect();

		if (Rect.IsEmpty())
		{
			UndoableOperation* pOp = pParam->GetOpPointer();
			if (pOp != NULL)
			{ 
				Node* pParent = FindParent(); 
				if (!pOp->DoHideNode(this,TRUE))
				{
					return CC_FAIL;
				}
				// This bit is a bit scary, but should work !
				// After a deletion we factor out all attributes on the parent of this group
				if (pParent && pParent->IsCompound())
				{
					if (!pOp->DoFactorOutCommonChildAttributes(((NodeRenderableInk*)pParent),TRUE))
					{
						return CC_FAIL;
					}
				}
			}
		}
	}

	return NodeCompound::OnChildChange(pParam);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/********************************************************************************************

  >	virtual BOOL NodeGroup::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the group record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeGroup::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	CXaraFileRecord Rec(TAG_GROUP,0);

	BOOL ok = pFilter->Write(&Rec);

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See NodeGroup::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL NodeGroup::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	virtual BOOL NodeGroup::WriteBoundsRecord(BaseCamelotFilter* pFilter)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/05
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Write out a record containing the bounds of this object

********************************************************************************************/

BOOL NodeGroup::WriteBoundsRecord(BaseCamelotFilter* pFilter)
{
	BOOL ok = TRUE;
	// Write out the record
	DocRect Rect = GetBoundingRect();

	if (HasEffectAttrs())
	{
		CamelotFileRecord Rec(pFilter, TAG_COMPOUNDRENDER, TAG_COMPOUNDRENDER_SIZE);
		ok = Rec.Init();
		if (ok) ok = Rec.WriteUINT32(0);			// Reserved
		if (ok) ok = Rec.WriteCoord(Rect.lo);
		if (ok) ok = Rec.WriteCoord(Rect.hi);
		if (ok) ok = pFilter->Write(&Rec);
	}
	else
	{
		CamelotFileRecord Rec(pFilter, TAG_OBJECTBOUNDS, TAG_OBJECTBOUNDS_SIZE);
		ok = Rec.Init();
		if (ok) ok = Rec.WriteCoord(Rect.lo);
		if (ok) ok = Rec.WriteCoord(Rect.hi);
		if (ok) ok = pFilter->Write(&Rec);
	}

	return(ok);
}


/********************************************************************************************

>	virtual BOOL NodeGroup::WillWriteBounds(BaseCamelotFilter* pFilter)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/05
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if this node will write out a bounds record
	Purpose:	Determines if the down/up pair need to be written

********************************************************************************************/

BOOL NodeGroup::WillWriteBounds(BaseCamelotFilter* pFilter)
{
	return(HasEffectAttrs() || pFilter->GetBoundsWriteLevel() >= BWL_COMPOUND);
}



/********************************************************************************************

>	virtual BOOL NodeGroup::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/09/2005
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Begin to write out your child records, in the native format

				The base class will write out a TAG_DOWN record, but only if it has a child ptr

	SeeAlso:	WritePostChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/
/*
BOOL NodeGroup::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)
{
	BOOL ok = TRUE;
	Node* pChild = FindFirstChild();
	if (pChild != NULL)
	{
		// The group is considered to have children ONLY if at least one child is not a hidden node.
		if (!pChild->IsNodeHidden() || (pChild->FindNextNonHidden() != NULL))
		{
			ok = pFilter->WriteZeroSizedRecord(TAG_DOWN);

			if (ok)
				ok = WriteBoundsRecord(pFilter);
		}
	}

	return ok;
}
*/



/********************************************************************************************

>	virtual BOOL NodeGroup::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/09/2005
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Begin to write out you child records, in the native format

				The base class will write out a TAG_DOWN record, but only if it has a child ptr

	SeeAlso:	WritePostChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/
/*
BOOL NodeGroup::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)
{
	return WriteBeginChildRecordsNative(pFilter);
}
*/



/********************************************************************************************

>	virtual BOOL NodeGroup::AreYouSafeToRender()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/96
	Inputs:		-
	Returns:	TRUE if safe to render, FALSE otherwise
	Purpose:	Group nodes are always safe to render, but derived group nodes (such as blends and
				text, etc) are not safe until their post import functions have been called
	SeeAlso:	-

********************************************************************************************/

BOOL NodeGroup::AreYouSafeToRender()
{
	if (IS_A(this,NodeGroup))
		return TRUE;
	else
		return NodeRenderableInk::AreYouSafeToRender();
}



/********************************************************************************************

>	DocRect NodeGroup::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/25/00
	Inputs:		ExtParams		parameters describing the extension.
	Outputs:	
	Returns:	TRUE if this group can be validly extended,
				FALSE otherwise.
	Purpose:	Tests to see whether this group's extend-centre is positioned so as to make
				an extend operation irreversible.

				Note that we do not pass the ValidateExtend() call on to our kids, as our
				extend behaviour means that our children don't need extending or validating.
	Errors:		
	See also:	

********************************************************************************************/
//DocRect NodeGroup::ValidateExtend(const ExtendParams& ExtParams)
//{
//	DocCoord doccArray[1] = { FindExtendCentre() };
//	DocRect drMinExtend = Extender::ValidateControlPoints(1, doccArray, ExtParams);
//
//	return drMinExtend;
//}



/********************************************************************************************

>	void NodeGroup::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/25/00
	Inputs:		ExtParams		parameters describing the extension.
	Outputs:	this NodeGroup will be extended in accordance with ExtParams.
	Returns:	
	Purpose:	Perform an Extend operation on this group. Behaviour is as follows:

				* the group extends separately along each axis.
				* if the group is asked to stretch, it translates itself so that its centre
													is where it would be if it had stretched.
				* if the group is asked to extend, it is translated as a whole, as described
																				by ExtParams.

				Note that we do not pass the Extend() call on to our children, as our extend
				behaviours result in our children being transformed appropriately anyway.
	Errors:		
	See also:	class Extender

********************************************************************************************/
//void NodeGroup::Extend(const ExtendParams& ExtParams)
//{
//	// do the extension operations on ourself.
//	TransformTranslateNoStretchObject(ExtParams);
//	TransformTranslateObject(ExtParams);
//}




/////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of NodeListItem
/////////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	NodeListItemWithDocPtr::NodeListItemWithDocPtr()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	default constructor. Initialises pNode = NULL
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

NodeListItemWithDocPtr::NodeListItemWithDocPtr()
{
	pNode = NULL;
}

/********************************************************************************************

>	NodeListItemWithDocPtr::NodeListItemWithDocPtr(Node* WhichNode)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for NodeListItem which sets the pNode variable to WhichNode
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

NodeListItemWithDocPtr::NodeListItemWithDocPtr(Node* WhichNode)
{
	pNode = WhichNode;
}



//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/********************************************************************************************

>	virtual UINT32* GroupRecordHandler::GetTagList()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* GroupRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_GROUP, TAG_COMPOUNDRENDER, TAG_OBJECTBOUNDS, CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL GroupRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL GroupRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_GROUP:
			ok = HandleGroupRecord(pCXaraFileRecord);
			break;

		case TAG_COMPOUNDRENDER:
		case TAG_OBJECTBOUNDS:
			// Do nothing
			// We ignore TAG_COMPOUNDRENDER records because they are really just a hint to
			// external XAR stream handlers
			ok = TRUE;
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}


/********************************************************************************************

BOOL GroupRecordHandler::HandleGroupRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a Line colour record
	SeeAlso:	-

********************************************************************************************/

BOOL GroupRecordHandler::HandleGroupRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_GROUP,FALSE,"I don't handle this tag type");

	NodeGroup* pGroup = new NodeGroup;
	BOOL ok = FALSE;

	if (pGroup != NULL)
		ok = InsertNode(pGroup);

	return ok;
}




/********************************************************************************************

>	virtual void GroupRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/09/2005
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	Produce a textual description of the specified record
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if XAR_TREE_DIALOG
void GroupRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	char s[256];

	// Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	switch (pRecord->GetTag())
	{
		case TAG_GROUP:
			break;

		case TAG_COMPOUNDRENDER:
			{
				INT32 reserved = 0;
				pRecord->ReadINT32(&reserved);
				camSprintf(s,_T("Reserved\t\t= %d\r\n"), reserved);
				(*pStr) += s;
			}
			nobreak;			// fall through

		case TAG_OBJECTBOUNDS:
			DocRect rbounds;
			pRecord->ReadCoord(&rbounds.lo);
			pRecord->ReadCoord(&rbounds.hi);
			camSprintf(s, _T("Bounds\t\t= %d, %d, %d, %d\r\n"), rbounds.lox, rbounds.loy, rbounds.hix, rbounds.hiy);
			(*pStr) += s;
			break;
	}
}
#endif


