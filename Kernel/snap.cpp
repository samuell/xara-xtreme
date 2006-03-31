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
// Implementation of the Snap class of Camelot - does all snap-type activity

/*
*/

#include "camtypes.h"
#include "snap.h"
#include "app.h"
#include "docview.h"
#include "spread.h"
#include "grid.h"
#include "statline.h"
#include "layer.h"
#include "guides.h"
//#include "scrcamvw.h"
#include "camelot.h"
#include "page.h"
//#include "node.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_MEMDUMP(CSnap, CC_CLASS_MEMDUMP)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

#define DIFF(a, b)	(a > b ? (a - b) : (b - a))

MILLIPOINT 	CSnap::SnapDist = 1;
MILLIPOINT 	CSnap::SnapToCoordDist = 1;
BOOL 		CSnap::TrySnapToPages = TRUE;

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

/********************************************************************************************

>	CSnap::CSnap(DocView* pThisDocView,Spread* pSpread)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		pThisDocView - the view in which this snapping is occuring
				pThisSpread	 - the spread that constains the objects (grids, guides, etc) to snap to
	Purpose:	The constructor.
				At the moment, snapping is controlled on a per-view basis, so a DocView has to 
				be supplied.
				Also, snapping to objects happens on a per-spread basis, hence the need to
				supply a spread.
	SeeAlso:	

********************************************************************************************/

CSnap::CSnap(DocView* pThisDocView,Spread* pThisSpread)
{
	pDocView = pThisDocView;
	pSpread  = pThisSpread;
	pLastSnappingObject = NULL;

	CalcSnapDist();
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

/********************************************************************************************

>	BOOL CSnap::Snap(DocCoord* pDocCoord,
								BOOL TryMagSnap = TRUE,
								BOOL TryGridSnap = TRUE)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		pDocCoord 	= a coord relative to the given spread
	Returns:	TRUE	- The coord has been snapped to something
				FALSE	- The coord is untouched by man and beast
	Purpose:	Allows you to easily snap a coord to something relevant in the given spread
				(e.g. to a grid, magnetic object, etc).
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL CSnap::Snap(DocCoord* pDocCoord,
								BOOL TryMagSnap,
								BOOL TryGridSnap)
{
	ERROR2IF(pDocView==NULL,FALSE,"CSnap::Snap() - pDocView==NULL");
	ERROR2IF( pSpread==NULL,FALSE,"CSnap::Snap() - pSpread==NULL");
	// This has to be called here so that the static SnapDist member var is
	// correct for this CSnap's DocView
	CalcSnapDist();

	BOOL Snapped=FALSE;

	// Try snapping to the guidelines on the guide layers
	if (!Snapped && pDocView->GetSnapToGuidesState())
		Snapped = SnapToGuidelines(pDocCoord);

	// Try snapping to the objects on the guide layers
	if (!Snapped && pDocView->GetSnapToGuidesState())
		Snapped = SnapToGuideObjects(pDocCoord);

	// Find the state of the snapping flags
	BOOL SnapToAllObjs = TryMagSnap && pDocView->GetSnapToObjectsState();		// "Snap to all objects" state
	BOOL SnapToMagObjs = TryMagSnap && pDocView->GetSnapToMagObjectsState();	// "Snap to magnetic objects" state

	// Only try and snap to objects if either the flags are set
	if (!Snapped && (SnapToAllObjs || SnapToMagObjs))
	{
		// try and snap to the last object we snapped to
		if ((pLastSnappingObject!=NULL) 						&&	// Do we have a last snapped object?
			(pLastSnappingObject->FindParent() != NULL) 		&&	// Does it have a parent (i.e. not been hidden)?
			(TryToSnapToObject(pLastSnappingObject, pDocCoord))
		   )	
		{
			Snapped=TRUE;
		}
		else
		{
			// We failed to snap to the last object we snapped to, so set the pointer to NULL
			pLastSnappingObject = NULL;

			// Only check magnetic objects if the "snap to magnetic objects" flag is TRUE and the 
			// "Snap to all objects" flag is FALSE
			BOOL Magnetic = (SnapToMagObjs && !SnapToAllObjs);
			if (SnapToObjects(pSpread->FindLastChild(), pDocCoord, Magnetic, FALSE))
				Snapped=TRUE;
		}
	}

	// Try snapping to the page boundaries
	if (!Snapped && TrySnapToPages && SnapToAllObjs)
		Snapped = SnapToPages(pDocCoord);

	// set a flag to indicate if snapped to an object but not the grid
	// for update of magnetic snap indicator
	BOOL NonGridSnapped=Snapped;
	
	// if not already snapped to an object, try to snap to grid
	if (!Snapped)
		if (TryGridSnap && (pDocView->GetSnapToGridState()) && (SnapToGrids(pDocCoord)))
			Snapped=TRUE;

#if !defined(EXCLUDE_FROM_RALPH)
	// update mouse position in StatusLine (with snapped mouse position)
	StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
	ERROR3IF(pStatusLine==NULL,"CSnap::Snap() - GetpStatusLine() returned NULL!");
	if (pStatusLine)
		pStatusLine->UpdateMousePosAndSnap(pDocCoord,pSpread,pDocView,NonGridSnapped);
#endif

	return Snapped;
}
 

/********************************************************************************************

>	BOOL CSnap::Snap(DocRect* pDocRect, const DocCoord& PrevCoord, const DocCoord& CurCoord);
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		pDocRect 	= a rectangle is spread coords
				PrevCoord	= Used to determine how rect is snapped 
				CurCoord	= As above
	Returns:	TRUE	- The rect has been snapped to something
				FALSE	- The rect is untouched by man and beast
	Purpose:	Allows you to easily snap a rectangle to objects
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL CSnap::Snap(DocRect* pDocRect, const DocCoord& PrevCoord, const DocCoord& CurCoord)
{
	ERROR3IF(pDocView == NULL || pSpread == NULL,"Invalid DocView and/or spread");
	if (pDocView == NULL || pSpread == NULL) return FALSE;

	// This has to be called here so that the static SnapDist member var is
	// correct for this CSnap's DocView
	CalcSnapDist();

	BOOL Snapped = FALSE;

	if (!Snapped && pDocView->GetSnapToGuidesState())
		Snapped = SnapToGuidelines(pDocRect,PrevCoord,CurCoord);

	if (!Snapped && pDocView->GetSnapToGuidesState())
		Snapped = SnapToGuideObjects(pDocRect,PrevCoord,CurCoord);

	BOOL SnapToAllObjs = pDocView->GetSnapToObjectsState();		// "Snap to all objects" state
	BOOL SnapToMagObjs = pDocView->GetSnapToMagObjectsState();	// "Snap to magnetic objects" state

	if (!Snapped)
	{
		// Only try and snap to objects if either the "snap to all objects" OR "snap to magnetic objects"
		// flags are set

		if (SnapToAllObjs || SnapToMagObjs)
		{
			// Only check magnetic objects if the "snap to magnetic objects" flag is TRUE and the 
			// "Snap to all objects" flag is FALSE

			BOOL Magnetic = (SnapToMagObjs && !SnapToAllObjs);

			Snapped = SnapToObjects(pDocRect,PrevCoord,CurCoord,Magnetic,FALSE);
		}
	}

	// Try snapping to the page boundaries
	if (!Snapped && TrySnapToPages && SnapToAllObjs)
		Snapped = SnapToPages(pDocRect,PrevCoord,CurCoord);

	// Try and snap to the grids
	if (!Snapped && pDocView->GetSnapToGridState())
		Snapped = SnapToGrids(pDocRect,PrevCoord,CurCoord);

	return (Snapped);
}
 
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

/********************************************************************************************

>	BOOL CSnap::SnapToPages(DocCoord* pDocCoord)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Inputs:		pDocCoord = a spread coord 
	Returns:	TRUE	- The coord has been snapped to a page
				FALSE	- The coord is untouched by man and beast
	Purpose:	Allows you to easily snap a coord to a page in a spread

	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL CSnap::SnapToPages(DocCoord* pDocCoord)
{
	ERROR3IF(pDocView == NULL || pSpread == NULL,"Invalid DocView and/or spread");
	if (pDocView == NULL || pSpread == NULL) return FALSE;

	BOOL Snapped = FALSE;

	Page* pPage = pSpread->FindFirstPageInSpread();
	while (pPage != NULL && !Snapped)
	{
		Snapped = pPage->Snap(pDocCoord);
		pPage = pPage->FindNextPage();
	}

	return Snapped;
}

/********************************************************************************************

>	BOOL CSnap::SnapToPages(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Inputs:		pDocRect  = a spread rect
				PrevCoord = Prev coord of mouse pointer
				CurCoord  = Current coord of mouse pointer
	Returns:	TRUE	- The rect has been snapped to a page
				FALSE	- The coord is untouched by man and beast
	Purpose:	Allows you to easily snap a coord to a page in a spread

	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL CSnap::SnapToPages(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
	ERROR3IF(pDocView == NULL || pSpread == NULL,"Invalid DocView and/or spread");
	if (pDocView == NULL || pSpread == NULL) return FALSE;

	BOOL Snapped = FALSE;

	Page* pPage = pSpread->FindFirstPageInSpread();
	while (pPage != NULL && !Snapped)
	{
		Snapped = pPage->Snap(pDocRect,PrevCoord,CurCoord);
		pPage = pPage->FindNextPage();
	}

	return Snapped;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

/********************************************************************************************

>	BOOL CSnap::SnapToGuidelines(DocCoord* pDocCoord,GuidelineType Type = GUIDELINE_NOTYPE)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		pDocCoord = a spread coord 
				Type	  = type of guideline to snap to (GUIDELINE_NOTYPE means snap to all types)
	Returns:	TRUE	- The coord has been snapped to a guideline
				FALSE	- The coord is untouched by man and beast
	Purpose:	Allows you to easily snap a coord to a guideline in a spread

				When Type == GUIDELINE_NOTYPE:
					It snaps the X ordinate to the vertical rulers, and the Y ordinate to horizontal rulers.
					This also creates the desired effect of making H & V guideline intersection points hot.

				When Type != GUIDELINE_NOTYPE:
					It snaps the given coord to this type of guideline only.
					(Used by the DocRect snapping version of this func)

	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL CSnap::SnapToGuidelines(DocCoord* pDocCoord,GuidelineType Type)
{
	ERROR3IF(pDocView == NULL || pSpread == NULL,"Invalid DocView and/or spread");
	if (pDocView == NULL || pSpread == NULL) return FALSE;

	// We haven't snapped yet
	BOOL Snapped = FALSE;

	// Scan all layers in the spread
	Layer* pLayer = pSpread->FindFirstLayer();
	while (pLayer != NULL && !Snapped)
	{
		if (pLayer->IsGuide() && pLayer->IsVisible())
		{
			// Only guide layers can contain guidelines

			if (Type == GUIDELINE_NOTYPE)
			{
				// OK, try to snap to both H & V guidelines
				Snapped  = SnapToGuidesInLayer(pLayer,pDocCoord,GUIDELINE_HORZ);
				Snapped |= SnapToGuidesInLayer(pLayer,pDocCoord,GUIDELINE_VERT);
			}
			else
				// Only Snap to guidelines of the given type.
				Snapped  = SnapToGuidesInLayer(pLayer,pDocCoord,Type);
		}

		pLayer = pLayer->FindNextLayer();
	}

	return Snapped;
}

/********************************************************************************************

>	BOOL CSnap::SnapToGuidesInLayer(Layer* pLayer,DocCoord* pDocCoord,GuidelineType Type)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		pLayer = ptr to layer
				pDocCoord = ptr to DocCoord to snap
				Type	  = the type of guide to snap to (HORZ or VERT)
	Returns:	TRUE	- The coord has been snapped to a guideline
				FALSE	- The coord is untouched by man and beast
	Purpose:	Helper function that will snap one component of the DocCoord to one type of
				guideline.

				Only guidelines that are top-level children of pLayer are used

				The following statements describe how the snapping is performed (though the code
				implements a more general method)

					if Type == HORZ then the Y coord is snapped to the closest HORZ guideline
					if Type == VERT then the X coord is snapped to the closest VERT guideline

	SeeAlso:	CSnap::SnapToGuidelines(DocCoord* pDocCoord)

********************************************************************************************/

BOOL CSnap::SnapToGuidesInLayer(Layer* pLayer,DocCoord* pDocCoord,GuidelineType Type)
{
	ERROR2IF(pLayer    == NULL,FALSE,"pLayer is NULL");
	ERROR2IF(pDocCoord == NULL,FALSE,"pDocCoord is NULL");

	BOOL Snapped      = FALSE;
	BOOL FirstSnapped = TRUE;

	DocCoord SnappedCoord;			// Holds the snapped version of pDocCoord
	DocCoord ClosestSnappedCoord;	// The closest snapped coord to the original coord

	// Scan all top-level objects in the layer for guidelines
	Node* pNode = pLayer->FindFirstChild(CC_RUNTIME_CLASS(NodeGuideline));
	while (pNode != NULL && !Snapped)
	{
		NodeGuideline* pGuide = ((NodeGuideline*)pNode);

		if (pGuide->GetType() == Type)
		{
			// We've found a guideline of the correct type

			// Snap the coord
		 	SnappedCoord = *pDocCoord;
			Snapped = pGuide->Snap(&SnappedCoord);

			if (Snapped)
			{
				// If it has snapped, make sure our idea of the closest snapped coord is correct
				if (FirstSnapped)
				{
					// If it's the first time we have received a snapped coord, it must be the closest
					ClosestSnappedCoord = SnappedCoord;
					FirstSnapped = FALSE;
				}
				else
				{
					// Is this snapped coord closer to the original coord than the previous closest snapped coord?
					if (pDocCoord->Distance(SnappedCoord) < pDocCoord->Distance(ClosestSnappedCoord))
						ClosestSnappedCoord = SnappedCoord;
				}
			}
		}

		pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeGuideline));
	}

	// Update the supplied coord if we have a snapped version.
	if (Snapped)
		*pDocCoord = ClosestSnappedCoord;

	return Snapped;
}


/********************************************************************************************

>	BOOL CSnap::SnapToGuidelines(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Inputs:		pDocRect  = a spread rect
				PrevCoord = Prev coord of mouse pointer
				CurCoord  = Current coord of mouse pointer
	Returns:	TRUE	- The rect has been snapped to something
				FALSE	- The rect is untouched by man and beast
	Purpose:	Allows you to easily snap a rect to a guideline in a spread

				This will snap the sides of the rect closest to the guidelines on the page.

				It snaps the H & V sides of the rect independantly, so that the corner of the rect
				will snap into H & V guideline intersection points correctly/

	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL CSnap::SnapToGuidelines(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
	ERROR2IF(pDocRect == NULL,FALSE,"pDocRect is NULL");

	// This is the amount we will be translating pDocRect by in order to get it to snap
	INT32 DeltaX = 0, DeltaY = 0;

	// Copy of the original pDocRect to preserve the original coords
	DocRect SnappedRect = *pDocRect;

	//----
	// Snap the left or right hand side of the rect
	//----

	BOOL SnappedLoX = SnapToGuidelines(&SnappedRect.lo,GUIDELINE_VERT);		// Snap the left hand side
	BOOL SnappedHiX = SnapToGuidelines(&SnappedRect.hi,GUIDELINE_VERT);		// Snap the right hand side
	
	if (SnappedLoX && SnappedHiX)
	{
		// Did both sides snap? I.e. are both sides very close to a vertical guideline?
		// If so, choose to snap the side that's closest to a guideline

		BOOL DeltaLoX = SnappedRect.lo.x - pDocRect->lo.x;
		BOOL DeltaHiX = SnappedRect.hi.x - pDocRect->hi.x;

		if (abs(DeltaLoX) < abs(DeltaHiX))
			DeltaX = DeltaLoX;
		else
			DeltaX = DeltaHiX;
	}
	else if (SnappedLoX)
		DeltaX = SnappedRect.lo.x - pDocRect->lo.x;		// Only the left hand side snapped
	else if (SnappedHiX)
		DeltaX = SnappedRect.hi.x - pDocRect->hi.x;		// Only the right hand side snapped

	//----
	// Snap the top or bottom of the rect
	//----

	BOOL SnappedLoY = SnapToGuidelines(&SnappedRect.lo,GUIDELINE_HORZ);		// Snap the bottom
	BOOL SnappedHiY = SnapToGuidelines(&SnappedRect.hi,GUIDELINE_HORZ);		// Snap the top
	
	if (SnappedLoY && SnappedHiY)
	{
		// Did both sides snap? I.e. are both sides very close to a horizontal guideline?
		// If so, choose to snap the side that's closest to a guideline

		BOOL DeltaLoY = SnappedRect.lo.y - pDocRect->lo.y;
		BOOL DeltaHiY = SnappedRect.hi.y - pDocRect->hi.y;

		if (abs(DeltaLoY) < abs(DeltaHiY))
			DeltaY = DeltaLoY;
		else
			DeltaY = DeltaHiY;
	}
	else if (SnappedLoY)
		DeltaY = SnappedRect.lo.y - pDocRect->lo.y;		// Only the bottom snapped
	else if (SnappedHiY)
		DeltaY = SnappedRect.hi.y - pDocRect->hi.y;		// Only the top snapped


	// Did any snapping occur?
	BOOL Snapped = (SnappedLoX || SnappedHiX || SnappedLoY || SnappedHiY);

	// If it did, translate pDocRect by the required amount
	if (Snapped)
		pDocRect->Translate(DeltaX,DeltaY);

	return (Snapped);
}

//------------------------------------------------------------
//------------------------------------------------------------

/********************************************************************************************

>	BOOL CSnap::SnapToGuideObjects(DocCoord* pDocCoord)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/95
	Inputs:		pDocCoord = a spread coord 
	Returns:	TRUE	- The coord has been snapped to a guideline
				FALSE	- The coord is untouched by man and beast
	Purpose:	Allows you to easily snap a coord to objects in guide layers

	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL CSnap::SnapToGuideObjects(DocCoord* pDocCoord)
{
	ERROR3IF(pDocView == NULL || pSpread == NULL,"Invalid DocView and/or spread");
	if (pDocView == NULL || pSpread == NULL) return FALSE;

	// We haven't snapped yet
	BOOL Snapped = FALSE;

	// Scan all layers in the spread
	Layer* pLayer = pSpread->FindFirstLayer();
	while (pLayer != NULL && !Snapped)
	{
		// Only snap to objects in visible guide layers
		if (pLayer->IsGuide() && pLayer->IsVisible())
			Snapped = SnapToObjects(pLayer,pDocCoord,FALSE,TRUE);

		pLayer = pLayer->FindNextLayer();
	}

	return Snapped;
}

/********************************************************************************************

>	BOOL CSnap::SnapToGuideObjects(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/95
	Inputs:		pDocRect  = a spread rect
				PrevCoord = Prev coord of mouse pointer
				CurCoord  = Current coord of mouse pointer
	Returns:	TRUE	- The rect has been snapped to a guideline
				FALSE	- The rect  is untouched by man and beast
	Purpose:	Allows you to easily snap a rect to objects in guide layers

	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL CSnap::SnapToGuideObjects(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
	ERROR3IF(pDocView == NULL || pSpread == NULL,"Invalid DocView and/or spread");
	if (pDocView == NULL || pSpread == NULL) return FALSE;

	// We haven't snapped yet
	BOOL Snapped = FALSE;

	// Scan all layers in the spread
	Layer* pLayer = pSpread->FindFirstLayer();
	while (pLayer != NULL && !Snapped)
	{
		// Only snap to objects in visible guide layers
		if (pLayer->IsGuide() && pLayer->IsVisible())
			Snapped = SnapToObjects(pDocRect,PrevCoord,CurCoord,FALSE,TRUE);

		pLayer = pLayer->FindNextLayer();
	}

	return Snapped;
}

//------------------------------------------------------------
//------------------------------------------------------------

/********************************************************************************************

>	BOOL CSnap::SnapToGrids(DocCoord* pDocCoord)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		pDocCoord = a spread coord 
	Returns:	TRUE	- The coord has been snapped to a grid
				FALSE	- The coord is untouched by man and beast
	Purpose:	Allows you to easily snap a coord to a grid in a spread
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL CSnap::SnapToGrids(DocCoord* pDocCoord)
{
	// Make sure that we can and should be snapping to grids
	ERROR3IF(pDocView == NULL || pSpread == NULL,"Invalid DocView and/or spread");
	if (pDocView == NULL || pSpread == NULL) return FALSE;
// >>>> Phil says: This isn't needed because the caller always checks this
// >>>> if necessary.
//	if (!pDocView->GetSnapToGridState()) return (FALSE);

	// We haven't snapped yet
	BOOL Snapped = FALSE;

	// find the first grid in this layer
	Node* pNode = pSpread->FindFirstChild();
	if (!pNode->IS_KIND_OF(NodeGrid))
		pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeGrid));

	// Get some DocCoords ready
	DocCoord SnappedCoord,ClosestSnappedCoord;

	while (pNode != NULL)
	{
		// We've got a kind of grid
		NodeGrid* pNodeGrid = (NodeGrid *)pNode;

		SnappedCoord = *pDocCoord;
		if (pNodeGrid->Snap(&SnappedCoord))
		{
			// We've now got a snapped version of the coord
			// Keep the snapped coord that is closest to the original point

			if (Snapped)
			{
				if (pDocCoord->Distance(SnappedCoord) < pDocCoord->Distance(ClosestSnappedCoord))
					ClosestSnappedCoord = SnappedCoord;
			}
			else
			{
				ClosestSnappedCoord = SnappedCoord;
				Snapped = TRUE;
			}
		}
		// Get the next node, it could be a grid!
		pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeGrid));
	}

	if (Snapped)
		*pDocCoord = ClosestSnappedCoord;

	return (Snapped);
}



/********************************************************************************************

>	BOOL CSnap::SnapToGrids(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		pDocRect 	= a rectangle is spread coords
				PrevCoord	= Used to determine how rect is snapped 
				CurCoord	= As above
	Returns:	TRUE	- The rect has been snapped to something
				FALSE	- The rect is untouched by man and beast
	Purpose:	Allows you to easily snap a coord to a grid in a spread
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL CSnap::SnapToGrids(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
	// Make sure that we can and should be snapping to grids
	ERROR3IF(pDocView == NULL || pSpread == NULL,"Invalid DocView and/or spread");
	if (pDocView == NULL || pSpread == NULL) return FALSE;
	if (!pDocView->GetSnapToGridState()) return (FALSE);

	BOOL Snapped = FALSE;

	// find the first grid in this layer
	Node* pNode = pSpread->FindFirstChild();
	if (!pNode->IS_KIND_OF(NodeGrid))
		pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeGrid));

	DocRect SnappedRect,ClosestSnappedRect;

	while (pNode != NULL)
	{
		NodeGrid* pNodeGrid = (NodeGrid *)pNode;

		SnappedRect = *pDocRect;
		if (pNodeGrid->Snap(&SnappedRect,PrevCoord,CurCoord))
		{
			if (Snapped)
			{
				if (pDocRect->lo.Distance(SnappedRect.lo) < pDocRect->lo.Distance(ClosestSnappedRect.lo))
					ClosestSnappedRect = SnappedRect;
			}
			else
			{
				ClosestSnappedRect = SnappedRect;
				Snapped = TRUE;
			}
		}
		pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeGrid));
	}

	if (Snapped)
		*pDocRect = ClosestSnappedRect;

	return (Snapped);
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

/********************************************************************************************

>	BOOL CSnap::SnapToObjects(Node* pRootNode,DocCoord* pDocCoord,BOOL Magnetic,BOOL GuideLayersOnly)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		pRootNode = ptr to root node of sub tree of objects to snap to
				pDocCoord = a spread coord 
				Magnetic  = TRUE  means only snap to magnetic objects
							FALSE means snap to all objects
				GuideLayersOnly = TRUE  means objects on visible guide layers only
								  FALSE means objects on visible layers EXCEPT visible guide layers
	Returns:	TRUE	- The coord has been snapped to an object
				FALSE	- The coord is untouched by man and beast
	Purpose:	Allows you to snap a coord to an object in a spread.
				This will try and snap the coord to NodeRenderableBounded nodes in the sub-tree
				in the reverse order in which they are rendered.
				Each eligable node is firstly checked to see if the bounds contain the coord.
				If the Magnetic param is TRUE, only NodeRend nodes with their Man
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL CSnap::SnapToObjects(Node* pRootNode, DocCoord* pDocCoord, BOOL Magnetic, BOOL GuideLayersOnly)
{
	// were we passed rubbish
	if (pRootNode==NULL)
		return FALSE;

	// Got a real node here. It should be the last child node of the spread
	Node* pNode = pRootNode;
	BOOL  IsRising = FALSE;
//	BOOL  Snapped = FALSE;

	// Find a DocView
	DocView* pDocView = DocView::GetCurrent();
	if (pDocView==NULL)
		return FALSE;

	// set up an interval timer so we can regularly check if we need to abort
	const UINT32 AbortDelay = 100;	// milliseconds
	MonotonicTime AbortTimer;
	AbortTimer.Sample();

	while (!pNode->IsSpread())
	{
		// at regular intervals, check to see if we should abort the snap test
		if (AbortTimer.Elapsed(AbortDelay))
		{
			// if mouse moved, abort
			if (pDocView->IsMouseMoved())
				return FALSE;

			// also abort if the mouse is no longer of the docview
PORTNOTE("Snap", "Removed use of GetMousePosAndWindowID to abort snap processing")
#if !defined(EXCLUDE_FROM_XARALX)
			CWindowID WinID;
			WinCoord  WndPos;
			BOOL      MouseOK = CCamApp::GetMousePosAndWindowID( &WinID, &WndPos );
			if( MouseOK && WinID != NULL && 
				pDocView != CCamView::GetDocViewFromWindowID( WinID ) )
				return FALSE;
#endif

			// resample timer
			AbortTimer.Sample();
		}


//			while (pNode->IsLayer() &&
//				   (!(((Layer *)pNode)->IsVisible()) || (GuideLayersOnly && ((Layer*)pNode)->IsGuide()))
//				  )

		// We need to skip ALL unsuitable layers, so repeat until we find a layer we can snap to
		// A suitable layer has to be visible.
		// If 'GuideLayersOnly' flag is set, it also has to be a guide layer

		// Assume the layer is not suitable
		BOOL SuitableLayer = FALSE;

		// If we have found a layer, and it's not suitable, keep searching until we find one we like
		while (pNode->IsLayer() && !SuitableLayer)
		{
			Layer* pLayer = (Layer*)pNode;

			BOOL Visible = pLayer->IsVisible();					// Is layer visible?

			if (GuideLayersOnly)								// Guide layers only?
				SuitableLayer = Visible && pLayer->IsGuide();	// Suitable if visible & guide layer
			else
				SuitableLayer = Visible && !pLayer->IsGuide();	// Else, suitable if visible & NOT guide layer

			if (!SuitableLayer)
			{
				// This layer is not suitable, so skip it
				if (pNode->FindPrevious() != NULL)
					pNode = pNode->FindPrevious();
				else
				{
					pNode = pNode->FindParent();
					IsRising = TRUE;	// If we haven't hit a spread, then we'd better make sure
										// that the next if statement does the right thing
				}

				// See if we have come back to the spread
				if (pNode == NULL || pNode->IsSpread())
					return FALSE;
			}
		}

		if (!IsRising)
		{
			// Get the last most child from this node
			if (pNode->ShouldBeRendered())
			{
				while (pNode->FindFirstChild()!=NULL)
				{
					// Get the last node in the sibling list
					pNode = pNode->FindLastChild();

					// If it was NULL then something odd has happened, so fail
					if (pNode==NULL)
						return FALSE;
				}
			}
		}

		// Test this node for magnetic snapping
		if (pNode->ShouldBeRendered())
		{		
			if (TryToSnapToObject(pNode, pDocCoord))
			{
				// This node snapped. Note the node so we can try again next time
				pLastSnappingObject = pNode;
				return TRUE;
			}
		}

		// It did not snap to that node, so find the previous one
		if (pNode->FindPrevious()!=NULL)
		{
			// find the node before this one
			pNode = pNode->FindPrevious();
			IsRising = FALSE;
		}
		else
		{
			// run out of nodes, so find the parent and flag that
			// we are going up in the tree
			pNode = pNode->FindParent();
			IsRising = TRUE;
		}
	} 

	// No nodes snapped to the coord
	return FALSE;
}


/********************************************************************************************

>	BOOL CSnap::TryToSnapToObject(Node* pNode, DocCoord* pDocCoord)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		pNode - The node to try and snap to
				pDocCoord - The coord to snap
	Returns:	TRUE if it snapped the object to the coord, FALSE if it failed to snap
	Purpose:	Allows a test to see if a particular node snaps to a particaular coord.

********************************************************************************************/

BOOL CSnap::TryToSnapToObject(Node* pNode, DocCoord* pDocCoord)
{
	// Test this node for magnetic snapping
	if (pNode->IsBounded())
	{
		// Convert it to a RenderableBounded
		NodeRenderableBounded* pNodeRndBnd = (NodeRenderableBounded*) pNode;

		// Don't allow snapping to dragged nodes...
		if (pNodeRndBnd->IsDragged())
			return FALSE;

		// Firstly, inflate the bounds by the size of the snapping distance
		DocRect Bounds = pNodeRndBnd->GetBoundingRect();
		if (CSnap::SnapDist > CSnap::SnapToCoordDist)
			Bounds.Inflate(CSnap::SnapDist);
		else
			Bounds.Inflate(CSnap::SnapToCoordDist);

		// If the coord lies inside the inflated bounds of the node, try and snap to the object
		if (Bounds.ContainsCoord(*pDocCoord))
		{
			// Try to snap to the more significant points on the node
			if (pNodeRndBnd->SnapToCoords(pDocCoord))
				return TRUE;

			// It failed, so try and snap to the outline of the shape
			if (pNodeRndBnd->Snap(pDocCoord))
				return TRUE;
		}
	}

	// It did not snap to anything
	return FALSE;
}

/********************************************************************************************

>	BOOL CSnap::SnapToObjects(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord,BOOL Magnetic,BOOL GuideLayersOnly)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		pDocRect 	= a rectangle is spread coords
				PrevCoord	= Used to determine how rect is snapped 
				CurCoord	= As above
				Magnetic  	= TRUE  means only snap to magnetic objects
							  FALSE means snap to all objects
				GuideLayersOnly = TRUE  means ignore objects on guide layers
								  FALSE means treat guide layers as standard layers
	Returns:	TRUE	- The rect has been snapped to an object
				FALSE	- The rect is untouched by man and beast
	Purpose:	Allows you to easily snap a coord to an object in a spread
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL CSnap::SnapToObjects(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord,BOOL Magnetic,BOOL GuideLayersOnly)
{
	return FALSE;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

/********************************************************************************************

>	void CSnap::CalcSnapDist()
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/94
	Purpose:	Calculates the snapping distance to objects for this CSnap object's DocView.
				Uses the DocView's scale factor as a parameter, so this should be called
				every time the DocView's scale factor changes

********************************************************************************************/

void CSnap::CalcSnapDist()
{
	// Find out the Magnetic Line Radius preference from its owner
	INT32 MagneticRad = NodeRenderableBounded::MagneticLineRadius;
	INT32 PointRad = NodeRenderableBounded::MagneticPointRadius;

	// Scale it according to the ScaleFactor.
	if (pDocView!=NULL)
	{
		// Get the Scale factor and modify the magnetic radius
		double Scale = pDocView->GetViewScale().MakeDouble();

		MagneticRad = (INT32) ((double)MagneticRad / Scale);
		PointRad = (INT32) ((double)PointRad / Scale);
	}

	// Set the snapping distance to the (hopefully) modified magnetic radius
	SnapDist = MagneticRad;
	SnapToCoordDist = PointRad;
}



/********************************************************************************************
>	static BOOL CSnap::SnapCoordToPath(DocCoord* pDocCoord, Path* pPath)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/95
	Inputs:     pDocCoord	= a coord in Spread coords
				pPath - the path it should be snapped to
    Outputs:    pDocCoord will hiold the snapped coodinate, if snapped
    Returns:    TRUE 	- the DocCoord has been snapped to the path.
    			FALSE	- the DocCoord has not been snapped
	Purpose:	Common code called by a number of Nodes Snap(DocCoord*) functions.  Attempts
				to snap a coord onto a path
********************************************************************************************/
BOOL CSnap::SnapCoordToPath(DocCoord* pDocCoord, Path* pPath)
{
	BOOL Snapped = FALSE;

	MILLIPOINT SnapDist    = CSnap::GetSnapDist();
	MILLIPOINT SqrSnapDist = SnapDist*SnapDist;

	INT32 NearEl = 0;
	double mu = 0;
	double SqrDist = pPath->SqrDistanceToPoint(*pDocCoord, &NearEl, &mu);

	if (SqrDist <= SqrSnapDist)
	{
		*pDocCoord = pPath->ClosestPointTo(mu, NearEl);
		Snapped = TRUE;
	}

	return (Snapped);
}



/********************************************************************************************

>	static BOOL CSnap::SnapRectToRect(DocRect* pSnappedRect, const DocRect& RefRect)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/1999
	Inputs:		pSnappedRect	pointer to the rectangle which will be snapped.
				RefRect			the reference rectangle, to snap to.
	Outputs:	pSnappedRect may be snapped to RefRect.
	Returns:	TRUE if pSnappedRect was snapped,
				FALSE otherwise.
	Purpose:	Snap a rectangle so that it lines up with one or more sides of another
				rectangle. This will only happen if the first rectangle is close to the
				second rectangle.
	Errors:		ERROR2 if pSnappedRect is NULL.
	See also:	

********************************************************************************************/
BOOL CSnap::SnapRectToRect(DocRect* pSnappedRect, const DocRect& RefRect)
{
	// validate input.
	ERROR2IF(pSnappedRect == NULL, FALSE, "CSnap::SnapRectToRect- NULL pSnappedRect.");

	// before we start doing loads of snap-testing, we need to check that the two rectangles
	// are close enough together.
	DocRect TestIntersect = *pSnappedRect;
	TestIntersect.Inflate(SnapDist);
	if (!TestIntersect.IsIntersectedWith(RefRect))
		return FALSE;

	// try to snap the y-coordinates of pSnappedRect to those of RefRect.
	// order of snap-attempts goes:
	//
	//		top-to-top, bottom-to-bottom, bottom-to-top, top-to-bottom
	//
	// ,where we are snapping y-values in pSnappedRect-to-RefRect.
	BOOL bSnappedY = FALSE;
	if (!bSnappedY) bSnappedY = SnapToValue(RefRect.hi.y, &(pSnappedRect->hi.y), &(pSnappedRect->lo.y));
	if (!bSnappedY) bSnappedY = SnapToValue(RefRect.lo.y, &(pSnappedRect->lo.y), &(pSnappedRect->hi.y));
	if (!bSnappedY) bSnappedY = SnapToValue(RefRect.hi.y, &(pSnappedRect->lo.y), &(pSnappedRect->hi.y));
	if (!bSnappedY) bSnappedY = SnapToValue(RefRect.lo.y, &(pSnappedRect->hi.y), &(pSnappedRect->lo.y));

	// try to snap the x-coordinates of pSnappedRect to those of RefRect.
	// order of snap-attempts goes:
	//
	//		left-to-left, right-to-right, left-to-right, right-to-left
	//
	// ,where we are snapping x-values in pSnappedRect-to-RefRect.
	BOOL bSnappedX = FALSE;
	if (!bSnappedX) bSnappedX = SnapToValue(RefRect.lo.x, &(pSnappedRect->lo.x), &(pSnappedRect->hi.x));
	if (!bSnappedX) bSnappedX = SnapToValue(RefRect.hi.x, &(pSnappedRect->hi.x), &(pSnappedRect->lo.x));
	if (!bSnappedX) bSnappedX = SnapToValue(RefRect.hi.x, &(pSnappedRect->lo.x), &(pSnappedRect->hi.x));
	if (!bSnappedX) bSnappedX = SnapToValue(RefRect.lo.x, &(pSnappedRect->hi.x), &(pSnappedRect->lo.x));

	// return the result.
	return bSnappedY || bSnappedX;
}



/********************************************************************************************

>	static BOOL CSnap::SnapValues(const INT32 RefValue, INT32* pTestValue, INT32* pLockedValue)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/1999
	Inputs:		RefValue		a reference INT32 to snap to.
				pTestValue		pointer to a INT32 to snap to RefValue.
				pLockedValue	pointer to a INT32 whose behaviour is locked with pTestValue's
	Outputs:	The values referred to by pTestValue, pLockedValue may be changed, in which
				case this function will return TRUE.
	Returns:	TRUE if the given variable values were sucessfully snapped to the reference,
				FALSE otherwise.
	Purpose:	Given a reference and pointers to two variable values, this function attempts
				to snap the first variable value to the reference, and if successful adjusts
				the second variable value by the same amount as the first.
	Errors:		ERROR3 if either of the variable-value pointers is NULL.
				In release builds, just returns FALSE if this is the case.
	See also:	

********************************************************************************************/
BOOL CSnap::SnapToValue(const INT32 RefValue, INT32* pTestValue, INT32* pLockedValue)
{
	if (pTestValue == NULL || pLockedValue == NULL)
	{
		ERROR3("CSnap::SnapValues- NULL parameters");
		return FALSE;
	}

	if ((*pTestValue) > RefValue && ((*pTestValue) - RefValue) <= SnapDist)
	{
		(*pLockedValue) -= ((*pTestValue) - RefValue);
		(*pTestValue) = RefValue;
		return TRUE;
	}
	else if ((*pTestValue) < RefValue && (RefValue - (*pTestValue)) <= SnapDist)
	{
		(*pLockedValue) += (RefValue - (*pTestValue));
		(*pTestValue) = RefValue;
		return TRUE;
	}

	return FALSE;
}
