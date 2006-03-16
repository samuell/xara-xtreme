// $Id: blobs.cpp 662 2006-03-14 21:31:49Z alex $
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
// Provides a class to help manage the various types of blobs available in Camelot

/*
*/

#include "camtypes.h"
#include "blobs.h"
#include "rndrgn.h"
#include "docview.h"
#include "range.h"
#include "app.h"
#include "ink.h"
#include "osrndrgn.h"
#include "docvmsg.h"
#include "fillattr.h"
#include "tool.h"

// Declare the source version number
DECLARE_SOURCE("$Revision: 662 $");

//Give my name in memory dumps
CC_IMPLEMENT_MEMDUMP(BlobManager, MessageHandler)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

//static BOOL CurrentCaretState = FALSE;


/********************************************************************************************

>	BlobStyle::BlobStyle()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Purpose:	Default constructor for the blob style which sets all the types of blob to
				FALSE.

********************************************************************************************/

BlobStyle::BlobStyle()
{
	// Set all the blobs to false
	Object = Artistic = Fill = Tiny = Pen = ToolObject = FALSE;
}



/********************************************************************************************

>	BlobStyle::BlobStyle(	BOOL BObject, BOOL BArtistic, BOOL BFill,
							BOOL BTiny, BOOL BPen, BOOL BToolObject, BOOL BEffect )
	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		BObject - TRUE if you want Object blobs in the Blob Style
				BArtistic - TRUE if you want Artistic blobs in the Blob Style
				BFill - TRUE if you want Fill blobs in the Blob Style
				BTiny - TRUE if you want Tiny blobs in the Blob Style
				BPen - TRUE if Pen blobs are wanted
				BToolObject - TRUE if tool-specific object blobs are wanted.
				BEffect - TRUE if effect attributes blobs are wanted
				All these params default to FALSE.
	Purpose:	Constructor for the blob style flags that lets you specify which of the blob
				types you want to use.

********************************************************************************************/

BlobStyle::BlobStyle(	BOOL BObject, BOOL BArtistic, BOOL BFill,
						BOOL BTiny, BOOL BPen, BOOL BToolObject, BOOL BEffect )
{
	// Set the blobs that are required
	Object = BObject;
	Artistic = BArtistic;
	Fill = BFill;
	Tiny = BTiny;
	Pen = BPen;
	ToolObject = BToolObject;
	Effect = BEffect;
}





/********************************************************************************************

>	BlobManager::BlobManager()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Purpose:	Default constructor for the blob manager, marks all the blobs as off

********************************************************************************************/

BlobManager::BlobManager() : MessageHandler(CC_RUNTIME_CLASS(MessageHandler), TRUE)
{
	// There are no blobs visible when we start
	IsFillBlob = FALSE;
	IsObjectBlob = FALSE;
	IsArtisticBlob = FALSE;
	IsTinyBlob = FALSE;
	IsPenBlob = FALSE;
	IsToolObjectBlob = FALSE;
	IsEffectBlob = FALSE;

	// Blob rendering is switched on by default
	DrawBlobsAtAll = TRUE;

	RemovingBlobs = FALSE;

	bToolBlobsAreOff = FALSE;
}

/********************************************************************************************

>	void BlobManager::ToolInterest(BlobStyle Blobs)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	BlobStyle

********************************************************************************************/

void BlobManager::ToolInterest(BlobStyle Blobs)
{
	BlobStyle BlobsToRemove;

	// Find all the blobs that are NOT selected
	BlobsToRemove.Object 	= !Blobs.Object;
	BlobsToRemove.Artistic 	= !Blobs.Artistic;
	BlobsToRemove.Fill 		= !Blobs.Fill;
	BlobsToRemove.Tiny 		= !Blobs.Tiny;
	BlobsToRemove.Pen	 	= !Blobs.Pen;
	BlobsToRemove.ToolObject= !Blobs.ToolObject;
	BlobsToRemove.Effect	= !Blobs.Effect;

	// and remove them
	RemoveInterest(BlobsToRemove);

	// Add the selected blobs
	AddInterest(Blobs);

/*
	// Do we need to render Object Blobs
	BOOL DrawObject = FALSE;
	if ((Blobs.Object && !IsObjectBlob) || (!Blobs.Object && IsObjectBlob))
		DrawObject = TRUE;

	// Do we need to render Artistic Blobs
	BOOL DrawArtistic = FALSE;
	if ((Blobs.Artistic && !IsArtisticBlob) || (!Blobs.Artistic && IsArtisticBlob))
		DrawArtistic = TRUE;

	// Do we need to render Fill Blobs
	BOOL DrawFill = FALSE;
	if ((Blobs.Fill && !IsFillBlob) || (!Blobs.Fill && IsFillBlob))
		DrawFill = TRUE;

	// Do we need to render Tiny Blobs
	BOOL DrawTiny = FALSE;
	if ((Blobs.Tiny && !IsTinyBlob) || (!Blobs.Tiny && IsTinyBlob))
		DrawTiny = TRUE;

	// Do we need to render Tiny Blobs
	BOOL DrawPen = FALSE;
	if ((Blobs.Pen && !IsPenBlob) || (!Blobs.Pen && IsPenBlob))
		DrawPen = TRUE;


	// Find out about the selection
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	// was there a selection to render?
	if (pNode!=NULL && DrawBlobsAtAll)
	{
		// Find the parent spread
		Spread* pSpread = pNode->FindParentSpread();

		// Render what we need to
		RenderRegion* pRegion = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
		while (pRegion)
		{
			// Find the first selected object in the tree;
			Node* pNode = Selected->FindFirst();
			while(pNode)
			{
				// Render only those blobs that are changing state (ie those appearing or disappearing)
				RenderSpecificBlobs((NodeRenderable*)pNode, pRegion, DrawObject, DrawArtistic,
									DrawFill, DrawTiny, DrawPen);

				// Find the next selected node to render
				pNode = Selected->FindNext(pNode);
			}

			// Go find the next region
			pRegion = DocView::GetNextOnTop(NULL);
		}
	}

	// Now we have to set the flags to the appropraite value
	IsFillBlob 		= Blobs.Fill;
	IsArtisticBlob	= Blobs.Artistic;
	IsObjectBlob 	= Blobs.Object;
	IsTinyBlob 		= Blobs.Tiny;
	IsPenBlob		= Blobs.Pen;

	// Bodge to stop fill meshes EOR each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;
*/
}


/********************************************************************************************

>	void BlobManager::AddInterest(BlobStyle Blobs)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/94
	Inputs:		Blobs - The blobs that you which to add to the ones currently displayed
	Purpose:	If the blobs specified are not already displayed, then they are rendered
				onto the screen. The Flags that the blob manager maintains about which
				blobs are on screen and which are not are also updated
	SeeAlso:	BlobManager::RemoveInterest; BlobStyle

********************************************************************************************/

void BlobManager::AddInterest(BlobStyle Blobs)
{
	RemovingBlobs = FALSE;

	// First we have to work out which bits of the we have to add.
	// Do we need Object blobs to be rendered
	BOOL DrawObject = FALSE;
	if (!IsObjectBlob && Blobs.Object)
	{
		DrawObject = TRUE;
		IsObjectBlob = TRUE;
	}

	// Do we need Artistic blobs to be rendered
	BOOL DrawArtistic = FALSE;
	if (!IsArtisticBlob && Blobs.Artistic)
	{
		DrawArtistic = TRUE;
		IsArtisticBlob = TRUE;
	}

	// Do we need Fill blobs to be rendered
	BOOL DrawFill = FALSE;
	if (!IsFillBlob && Blobs.Fill)
	{
		DrawFill = TRUE;
		IsFillBlob = TRUE;
	}

	// Do we need Effect blobs to be rendered
	BOOL DrawEffect = FALSE;
	if (!IsEffectBlob && Blobs.Effect)
	{
		DrawEffect = TRUE;
		IsEffectBlob = TRUE;
	}

	// Do we need Tiny blobs to be rendered
	BOOL DrawTiny = FALSE;
	if (!IsTinyBlob && Blobs.Tiny)
	{
		DrawTiny = TRUE;
		IsTinyBlob = TRUE;
	}

	// Do we need Pen blobs to be rendered
	BOOL DrawPen = FALSE;
	if (!IsPenBlob && Blobs.Pen)
	{
		DrawPen = TRUE;
		IsPenBlob = TRUE;
	}

	// Do we need ToolObject blobs to be rendered
	BOOL DrawToolObject = FALSE;
	if (!IsToolObjectBlob && Blobs.ToolObject)
	{
		DrawToolObject = TRUE;
		IsToolObjectBlob = TRUE;
	}

	if (!NeedToRenderSelectionBlobs(NULL))
		return;

	RenderSpecificBlobsOnSelection( NULL, DrawObject, DrawArtistic, 
									DrawFill, DrawTiny, DrawPen, DrawToolObject, DrawEffect);

	// Now EOR off in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will put them on
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();

	if (!pRegionList->IsEmpty())
	{
		// Find out which spread the selection is on
		SelRange* Selected = GetApplication()->FindSelection();
		Node* pNode = Selected->FindFirst();
		
		if (pNode)	// Is there a Selection ?
		{
			Spread* pSpread = pNode->FindParentSpread();

			RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

			while (pRegion)
			{
				// Check the RenderRegion is for the same spread.
				if (pRegion->GetRenderSpread() == pSpread &&
					pRegion->GetRenderView()==DocView::GetSelected() &&
					(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
				{
					// Render the blobs 'clipped' to this Render Region.
					DocRect ClipRect = pRegion->GetRegionRect();
					RenderSpecificBlobsOnSelection( &ClipRect,
													DrawObject, DrawArtistic, DrawFill,
													DrawTiny, DrawPen, DrawToolObject, DrawEffect );
				}

				// Get the Next render region
				pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
			}
		}
	}
}




/********************************************************************************************

>	void BlobManager::RemoveInterest(BlobStyle Blobs)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/94
	Inputs:		Blobs - The blobs that you which to remove from the ones currently displayed
	Purpose:	If the blobs specified are already displayed, then they are rendered
				onto the screen to get rid of them (EORing!). The Flags that the blob manager
				maintains about which blobs are on screen and which are not are also updated
	SeeAlso:	BlobManager::AddInterest; BlobStyle

********************************************************************************************/

void BlobManager::RemoveInterest(BlobStyle Blobs)
{
	RemovingBlobs = TRUE;

	// First we have to work out which bits of the we have to add.
	// Do we need Object blobs to be rendered
	BOOL DrawObject = FALSE;
	if (IsObjectBlob && Blobs.Object)
	{
		DrawObject = TRUE;
		IsObjectBlob = FALSE;
	}

	// Do we need Artistic blobs to be rendered
	BOOL DrawArtistic = FALSE;
	if (IsArtisticBlob && Blobs.Artistic)
	{
		DrawArtistic = TRUE;
		IsArtisticBlob = FALSE;
	}

	// Do we need Fill blobs to be rendered
	BOOL DrawFill = FALSE;
	if (IsFillBlob && Blobs.Fill)
	{
		DrawFill = TRUE;
		IsFillBlob = FALSE;
	}

	// Do we need Fill blobs to be rendered
	BOOL DrawEffect = FALSE;
	if (IsEffectBlob && Blobs.Effect)
	{
		DrawEffect = TRUE;
		IsEffectBlob = FALSE;
	}

	// Do we need Tiny blobs to be rendered
	BOOL DrawTiny = FALSE;
	if (IsTinyBlob && Blobs.Tiny)
	{
		DrawTiny = TRUE;
		IsTinyBlob = FALSE;
	}

	// Do we need Pen blobs to be rendered
	BOOL DrawPen = FALSE;
	if (IsPenBlob && Blobs.Pen)
	{
		DrawPen = TRUE;
		IsPenBlob = FALSE;
	}

	// Do we need ToolObject blobs to be rendered
	BOOL DrawToolObject = FALSE;
	if (IsToolObjectBlob && Blobs.ToolObject)
	{
		DrawToolObject = TRUE;
		IsToolObjectBlob = FALSE;
	}

	if (!NeedToRenderSelectionBlobs(NULL))
		return;

	// EOR on in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will take them off
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();

	if (!pRegionList->IsEmpty())
	{
		// Find out which spread the selection is on
		SelRange* Selected = GetApplication()->FindSelection();
		Node* pNode = Selected->FindFirst();

		if (pNode)	// Is there a Selection ?
		{
			Spread* pSpread = pNode->FindParentSpread();

			RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

			while (pRegion)
			{
				// Check the RenderRegion is for the same spread.
				if (pRegion->GetRenderSpread() == pSpread &&
					pRegion->GetRenderView()==DocView::GetSelected() &&
					(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
				{
					// Render the blobs 'clipped' to this Render Region.
					DocRect ClipRect = pRegion->GetRegionRect();
					RenderSpecificBlobsOnSelection( &ClipRect,
													DrawObject, DrawArtistic, DrawFill,
													DrawTiny, DrawPen, DrawToolObject, DrawEffect );
				}

				// Get the Next render region
				pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
			}
		}
	}

	RenderSpecificBlobsOnSelection( NULL, DrawObject, DrawArtistic, 
									DrawFill, DrawTiny, DrawPen, DrawToolObject, DrawEffect);
}

/********************************************************************************************

>	BlobStyle BlobManager::GetCurrentInterest(BOOL bIgnoreOffState = FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94
	Inputs:		bIgnoreOffState - Flag for use by GetBlobBoundingRect functions so that
				they can return correct bounds even when blob rendering is turned off
	Returns:	A 'BlobStyle' refecting the current blob interest
	Purpose:	Allows people other than the blobmanager to know what blob are currently
				displayed.
	SeeAlso:	BlobManager::AddInterest; BlobStyle

********************************************************************************************/

BlobStyle BlobManager::GetCurrentInterest(BOOL bIgnoreOffState)
{
	// Make a default blob style with all interest off.
	BlobStyle CurrentBlobs;

	if (DrawBlobsAtAll || bIgnoreOffState)
	{
		// Update it with the current Interest
		CurrentBlobs.Fill		= IsFillBlob;
		CurrentBlobs.Effect		= IsEffectBlob;
		CurrentBlobs.Object		= IsObjectBlob;
		CurrentBlobs.Artistic	= IsArtisticBlob;
		CurrentBlobs.Tiny		= IsTinyBlob;
		CurrentBlobs.Pen		= IsPenBlob;
		CurrentBlobs.ToolObject	= IsToolObjectBlob;
	}

	return CurrentBlobs;
}

/********************************************************************************************

>	void BlobManager::BlobRenderingOff(BOOL Redraw)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Inputs:		Redraw - TRUE if you want the blob to be rendered before blob rendering is
				turned off.
	Purpose:	After calling this function, any attempts to render blobs from anywhere in
				Camelot will result in nothing appearing on screen. This is a dangerous
				function to call if you are not sure what is going on.
				Normally you should use this function if you are about to do something
				that will involve the selection state becoming inconsistent with the
				blobs on screen and you specifically do not want lots of blob rendering
				going on until you have finished.
				Here is an example way of using it
				MonoOn
					// Get rid of the blobs from the screen
					BlobMgr->BlobRenderingOff(TRUE);

					// Party on the selection and change lots of things
					...

					// Invalidate the region of the blobs
					...

					// Switch the blob manager back on again
					// but leave the rendering of the blobs to the normal OnDraw loop
					BlobMgr->BlobRenderingOn(FALSE)
				MonoOff
	SeeAlso:	BlobManager::BlobRenderingOn()

********************************************************************************************/

void BlobManager::BlobRenderingOff(BOOL Redraw)
{
	RemovingBlobs = TRUE;

	if (Redraw && NeedToRenderSelectionBlobs(NULL))
	{
		// EOR on in all the render regions that are still rendering,
		// so that the Blob rendering when the region is finished, 
		// will take them off
		RenderRegionList* pRegionList = GetApplication()->GetRegionList();

		if (!pRegionList->IsEmpty())
		{
			// Find out which spread the selection is on
			SelRange* Selected = GetApplication()->FindSelection();
			Node* pNode = Selected->FindFirst();

			if (pNode)	// Is there a Selection ?
			{
				Spread* pSpread = pNode->FindParentSpread();

				RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

				while (pRegion)
				{
					// Check the RenderRegion is for the same spread.
					if (pRegion->GetRenderSpread() == pSpread &&
						pRegion->GetRenderView()==DocView::GetSelected() &&
						(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
					{
						// Render the blobs 'clipped' to this Render Region.
						DocRect ClipRect = pRegion->GetRegionRect();
						RenderRequiredBlobsOnSelection(&ClipRect);
					}

					// Get the Next render region
					pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
				}
			}
		}

		RenderRequiredBlobsOnSelection(NULL);
	}

	// Mark blob rendering to be off
	DrawBlobsAtAll = FALSE;

	// Bodge to stop fill meshes EOR each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;
}


/********************************************************************************************

>	void BlobManager::BlobRenderingOn(BOOL Redraw)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Inputs:		Redraw - TRUE if you want the blobs to be rendered before blob rendering is
				turned on.
	Purpose:	If blob rendering has been turned off ( by calling BlobRenderinOff() )
				then this function will re-enable it so that rendering can carry on
				like normal. Ideally this should be called not very long after calling
				BlobRenderingOff().
	SeeAlso:	BlobManager::BlobRenderingOn()

********************************************************************************************/

void BlobManager::BlobRenderingOn(BOOL Redraw)
{
	RemovingBlobs = FALSE;

	if (Redraw && NeedToRenderSelectionBlobs(NULL))
	{
		RenderRequiredBlobsOnSelection(NULL);

		// Now EOR off in all the render regions that are still rendering,
		// so that the Blob rendering when the region is finished, 
		// will put them on
		RenderRegionList* pRegionList = GetApplication()->GetRegionList();

		if (!pRegionList->IsEmpty())
		{
			// Find out which spread the selection is on
			SelRange* Selected = GetApplication()->FindSelection();
			Node* pNode = Selected->FindFirst();

			if (pNode)	// Is there a Selection ?
			{
				Spread* pSpread = pNode->FindParentSpread();

				RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

				while (pRegion)
				{
					// Check the RenderRegion is for the same spread.
					if (pRegion->GetRenderSpread() == pSpread &&
						pRegion->GetRenderView()==DocView::GetSelected() &&
						(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
					{
						// Render the blobs 'clipped' to this Render Region.
						DocRect ClipRect = pRegion->GetRegionRect();
						RenderRequiredBlobsOnSelection(&ClipRect);
					}

					// Get the Next render region
					pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
				}
			}
		}
	}

	// Mark blob rendering to be on
	DrawBlobsAtAll = TRUE;

	// Bodge to stop fill meshes EOR each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;
}




/********************************************************************************************

>	INT32 BlobManager::GetBlobSize()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Returns:	INT32 - The size (in DocCoords) of a blob.
	Purpose:	This function allows you to find out how wide a blob should be.

********************************************************************************************/

INT32 BlobManager::GetBlobSize()
{
	// To do this, we need to find out how big a blobs rect is
	DocRect Rect;
	GetBlobRect(DocCoord(0,0), &Rect);

	// return the width of the resulting rect
	return Rect.Width();
}



/********************************************************************************************

>	void BlobManager::GetBlobRect(DocCoord& Centre, DocRect* Rect)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Inputs:		Centre - The coordinate to centre the blob on
				MFill = Whether we should force this routine to use multi-stage fill routine
	Outputs:	Rect - The rect in DocCoords of the blobs
	Purpose:	Finds the rectangle of a blob that surrounds a given coord. This is to
				replace the rather long winded method that had to be employed before. If
				there is no selected document it will leave the rect unchanged.

				Karim 22/05/2000 - modified this method to take a BlobType param. Note that
				you *cannot* make use of this new param until the MFill param is removed and
				all current instances of GetBlobRect() updated. The new param is currently
				only checked against my ClipView blob type.

********************************************************************************************/

void BlobManager::GetBlobRect( const DocCoord &Centre, DocRect *Rect, BOOL MFill /*= FALSE*/,
																	BlobType eBlobType)
{
	// We will be needing a view to do this
	DocView* pDocView = DocView::GetSelected();

	// Find out how big a rect is these days, if we have a view
	if (pDocView!=NULL)
	{
		switch (eBlobType)
		{
		case BT_CLIPVIEW:
			OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), Centre, BT_CLIPVIEW, Rect);
			break;

		default:
			if (!MFill)
			{
				OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), Centre, BT_SELECTEDLARGEST, Rect);
			}
			else
			{
				OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), Centre, BT_MSTAGESELECTEDLARGEST, Rect);
			}
			break;
		}
	}
}


/********************************************************************************************

>	void BlobManager::Render(DocRect Rect, Spread *pSpread)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/94
	Purpose:	This function will render all the blobs that are currently being displayed
				for the given clipping rectangle.

********************************************************************************************/

void BlobManager::Render(DocRect* Rect, Spread *pSpread)
{
	// If we are not drawing blobs today, then return straight away
	if (!DrawBlobsAtAll)
		return;

	// Now render the sections that need to be rendered to get the corrent blobs on screen
	RenderRegion* pRegion = DocView::RenderOnTop(Rect, pSpread, ClippedEOR);
	while (pRegion)
	{
		// Find out the clipping rect of this region
		DocRect TestRect = pRegion->GetClipRect();

		// Find the first selected object in the tree;
		SelRange* Selected = GetApplication()->FindSelection();

		// Karim 29/06/2000
		//	PromoteToParent should never be set TRUE on the selection range, outside
		//	of code in which its modifications are required.
		//	I have included a TRACE statement here, as a 'quiet' note to programmers,
		//	should this occur.
		RangeControl rc = Selected->GetRangeControlFlags();
		if (rc.PromoteToParent)
		{
			TRACE( _T("BlobManager::Render; PromoteToParent is TRUE! Read inline comment for details.\n"));
			rc.PromoteToParent = FALSE;
			Selected->Range::SetRangeControl(rc);
		}

		Node* pNode = Selected->FindFirst();
		while(pNode)
		{
			// Render only the required blobs
			if (pNode->IsBounded())
			{
				// This node is bounded, so see if we really need to render it
				if (TestRect.IsIntersectedWith(((NodeRenderableBounded*)pNode)->GetBlobBoundingRect()))
					RenderRequiredBlobs((NodeRenderable*)pNode, pRegion);
			}
			else
			{
				// Always render it if it is not bounded (probably an Attribute)
				RenderRequiredBlobs((NodeRenderable*)pNode, pRegion);
			}

			// Find the next selected node to render
			pNode = Selected->FindNext(pNode);
		}

		// Go find the next region
		pRegion = DocView::GetNextOnTop(Rect);
	}

	// Bodge to stop fill meshes EOR each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;
}

/********************************************************************************************

>	void BlobManager::RenderOn(DocRect Rect, Spread *pSpread)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Purpose:	This function will render on all the blobs that are currently being displayed
				for the given clipping rectangle.

********************************************************************************************/

void BlobManager::RenderOn(DocRect* Rect, Spread *pSpread)
{
	RemovingBlobs = FALSE;

	// If we are not drawing blobs today, then return straight away
	if (!DrawBlobsAtAll)
		return;

	// Are we in the middle of a Background redraw ?
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	if (pRegionList->IsEmpty())
	{
		// Nope, so do stuff as normal

		// Draw the blobs on
		Render(Rect, pSpread);

		// Stop fill meshes EORing each other out.
		AttrFillGeometry::LastRenderedMesh = NULL;
		return;
	}

	// We must be background rendering ...
	// So lets try a bit of an optimisation

	// If the Blob we are about to render, is completely contained
	// within a pending Render Region, then we don't need to
	// render anything

	if (!NeedToRenderSelectionBlobs(Rect))
		return;

	// Oh well, lets render some blobs then

	// Draw the blobs on
	Render(Rect, pSpread);

	// Stop fill meshes EORing each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;
	
	// Now EOR off in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will put them on
	RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

	while (pRegion)
	{
		// Check the RenderRegion is for the same spread.
		if (pRegion->GetRenderSpread() == pSpread &&
			pRegion->GetRenderView()==DocView::GetSelected() &&
			(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
		{
			// Render the blobs 'clipped' to this Render Region.
			DocRect ClipRect = pRegion->GetRegionRect();
			Render(&ClipRect, pSpread);
		}

		// Get the Next render region
		pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
	}

	// Stop fill meshes EORing each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;
}

/********************************************************************************************

>	void BlobManager::RenderOff(DocRect Rect, Spread *pSpread)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Purpose:	This function will render off all the blobs that are currently being displayed
				for the given clipping rectangle.

********************************************************************************************/

void BlobManager::RenderOff(DocRect* Rect, Spread *pSpread)
{
	RemovingBlobs = TRUE;

	// If we are not drawing blobs today, then return straight away
	if (!DrawBlobsAtAll)
		return;

	// Are we in the middle of a Background redraw ?
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	if (pRegionList->IsEmpty())
	{
		// Nope, so do stuff as normal

		// Draw the blobs on
		Render(Rect, pSpread);

		// Stop fill meshes EORing each other out.
		AttrFillGeometry::LastRenderedMesh = NULL;
		return;
	}

	// We must be background rendering ...
	// So lets try a bit of an optimisation

	// If the Blob we are about to render, is completely contained
	// within a pending Render Region, then we don't need to
	// render anything

	if (!NeedToRenderSelectionBlobs(Rect))
		return;

	// Oh well, lets render some blobs then

	// EOR on in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will take them off
	RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

	while (pRegion)
	{
		// Check the RenderRegion is for the same spread.
		if (pRegion->GetRenderSpread() == pSpread &&
			pRegion->GetRenderView()==DocView::GetSelected() &&
			(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
		{
			// Render the blobs 'clipped' to this Render Region.
			DocRect ClipRect = pRegion->GetRegionRect();
			Render(&ClipRect, pSpread);
		}

		// Get the Next render region
		pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
	}

	// Stop fill meshes EORing each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;

	// Draw the blobs off
	Render(Rect, pSpread);

	// Stop fill meshes EORing each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;
}

/********************************************************************************************

>	void BlobManager::RenderMyBlobs(NodeRenderable* pNode)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Inputs:		pNode - The node to render the blobs of
	Purpose:	Renders the blobs for the given node. Only the required blobs are rendered
				and since the blobs are EOR rendered, this function can be used to put
				the blobs onto the screen or take them off the screen

********************************************************************************************/

void BlobManager::RenderMyBlobs(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode)
{
	// If we are not drawing blobs today, then return straight away
	if (!DrawBlobsAtAll)
		return;

	// Now render the sections that need to be rendered to get the corrent blobs on screen
	RenderRegion* pRegion = DocView::RenderOnTop(Rect, pSpread, ClippedEOR);
	while (pRegion)
	{
		// Find out the clipping rect of this region
		DocRect TestRect = pRegion->GetClipRect();

		// Render only the blobs that need to be drawn
		if (pNode->IsBounded())
		{
			// This node is bounded, so see if we really need to render it
			if (TestRect.IsIntersectedWith(((NodeRenderableBounded*)pNode)->GetBlobBoundingRect()))
				RenderRequiredBlobs(pNode, pRegion);
		}
		else
		{
			// Always render it if it is not bounded (probably an Attribute)
			RenderRequiredBlobs(pNode, pRegion);
		}

		// Go find the next region
		pRegion = DocView::GetNextOnTop(Rect);
	}
}

/********************************************************************************************

>	void BlobManager::RenderMyBlobsOn(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Purpose:	This function will render on all the blobs that are currently being displayed
				for the given clipping rectangle.

********************************************************************************************/

void BlobManager::RenderMyBlobsOn(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode)
{
	RemovingBlobs = FALSE;

	// If we are not drawing blobs today, then return straight away
	if (!DrawBlobsAtAll)
		return;

	// Are we in the middle of a Background redraw ?
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	if (pRegionList->IsEmpty())
	{
		// Nope, so do stuff as normal

		// Draw the blobs on
		RenderMyBlobs(Rect, pSpread, pNode);
		return;
	}

	// We must be background rendering ...

	// So lets try a bit of an optimisation
	if (!NeedToRenderNodeBlobs(Rect, pSpread, pNode))
		return;

	// Oh well, lets render some blobs then
	
	// Draw the blobs on
	RenderMyBlobs(Rect, pSpread, pNode);

	// Stop fill meshes EORing each other out.
	FillGeometryAttribute* pLastMesh = AttrFillGeometry::LastRenderedMesh;
	AttrFillGeometry::LastRenderedMesh = NULL;
	
	// Now EOR off in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will put them on

	RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

	while (pRegion)
	{
		// Check the RenderRegion is for the same spread.
		if (pRegion->GetRenderSpread() == pSpread &&
			pRegion->GetRenderView()==DocView::GetSelected() &&
			(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
		{
			// Render the blobs 'clipped' to this Render Region.
			DocRect ClipRect = pRegion->GetRegionRect();
			RenderMyBlobs(&ClipRect, pSpread, pNode);
		}

		// Get the Next render region
		pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
	}

	// Stop fill meshes EORing each other out.
	AttrFillGeometry::LastRenderedMesh = pLastMesh;
}

/********************************************************************************************

>	void BlobManager::RenderMyBlobsOff(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Purpose:	This function will render off all the blobs that are currently being displayed
				for the given clipping rectangle.

********************************************************************************************/

void BlobManager::RenderMyBlobsOff(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode)
{
	RemovingBlobs = TRUE;

	// If we are not drawing blobs today, then return straight away
	if (!DrawBlobsAtAll)
		return;

	// Are we in the middle of a Background redraw ?
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	if (pRegionList->IsEmpty())
	{
		// Nope, so do stuff as normal

		// Draw the blobs off
		RenderMyBlobs(Rect, pSpread, pNode);
		return;
	}

	// We must be background rendering ...

	// So lets try a bit of an optimisation
	if (!NeedToRenderNodeBlobs(Rect, pSpread, pNode))
		return;

	// Oh well, lets render some blobs then

	// EOR on in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will take them off
	RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

	while (pRegion)
	{
		// Check the RenderRegion is for the same spread.
		if (pRegion->GetRenderSpread() == pSpread &&
			pRegion->GetRenderView()==DocView::GetSelected() &&
			(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
		{
			// Render the blobs 'clipped' to this Render Region.
			DocRect ClipRect = pRegion->GetRegionRect();
			RenderMyBlobs(&ClipRect, pSpread, pNode);
		}

		// Get the Next render region
		pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
	}

	// Stop fill meshes EORing each other out.
	FillGeometryAttribute* pLastMesh = AttrFillGeometry::LastRenderedMesh;
	AttrFillGeometry::LastRenderedMesh = NULL;

	// Draw the blobs off
	RenderMyBlobs(Rect, pSpread, pNode);

	// Stop fill meshes EORing each other out.
	AttrFillGeometry::LastRenderedMesh = pLastMesh;
}

/********************************************************************************************

>	void BlobManager::RenderObjectBlobsOn(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Purpose:	This function will render on all the blobs that are currently being displayed
				for the given clipping rectangle.

********************************************************************************************/

void BlobManager::RenderObjectBlobsOn(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode)
{
	RemovingBlobs = FALSE;

	// Are we in the middle of a Background redraw ?
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	if (pRegionList->IsEmpty())
	{
		// Nope, so do stuff as normal

		// Draw the blobs on
		RenderObjectBlobs(Rect, pSpread, pNode);
		return;
	}

	// We must be background rendering ...

	// So lets try a bit of an optimisation
	if (!NeedToRenderNodeBlobs(Rect, pSpread, pNode))
		return;

	// Oh well, lets render some blobs then

	// Draw the blobs on
	RenderObjectBlobs(Rect, pSpread, pNode);

	// EOR off in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will put them on
	RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

	while (pRegion)
	{
		// Check the RenderRegion is for the same spread.
		if (pRegion->GetRenderSpread() == pSpread &&
			pRegion->GetRenderView()==DocView::GetSelected() &&
			(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
		{
			// Render the blobs 'clipped' to this Render Region.
			DocRect ClipRect = pRegion->GetRegionRect();

			// Render what we need to
			RenderObjectBlobs(&ClipRect, pSpread, pNode);
		}

		// Get the Next render region
		pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
	}
}

/********************************************************************************************

>	void BlobManager::RenderObjectBlobsOff(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Purpose:	This function will render off all the blobs that are currently being displayed
				for the given clipping rectangle.

********************************************************************************************/

void BlobManager::RenderObjectBlobsOff(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode)
{
	RemovingBlobs = TRUE;

	// Are we in the middle of a Background redraw ?
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	if (pRegionList->IsEmpty())
	{
		// Nope, so do stuff as normal

		// Draw the blobs off
		RenderObjectBlobs(Rect, pSpread, pNode);
		return;
	}

	// We must be background rendering ...

	// So lets try a bit of an optimisation
	if (!NeedToRenderNodeBlobs(Rect, pSpread, pNode))
		return;

	// Oh well, lets render some blobs then

	// EOR on in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will take them off
	RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

	while (pRegion)
	{
		// Check the RenderRegion is for the same spread.
		if (pRegion->GetRenderSpread() == pSpread &&
			pRegion->GetRenderView()==DocView::GetSelected() &&
			(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
		{
			// Render the blobs 'clipped' to this Render Region.
			DocRect ClipRect = pRegion->GetRegionRect();

			// Render what we need to
			RenderObjectBlobs(&ClipRect, pSpread, pNode);
		}

		// Get the Next render region
		pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
	}

	// Draw the blobs off
	RenderObjectBlobs(Rect, pSpread, pNode);
}

/********************************************************************************************

>	void BlobManager::RenderObjectBlobs(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Purpose:	This function will render all the blobs that are currently being displayed
				for the given clipping rectangle.

********************************************************************************************/

void BlobManager::RenderObjectBlobs(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode)
{
	RenderRegion* pOnTopRegion = DocView::RenderOnTop(Rect, pSpread, ClippedEOR);
	while (pOnTopRegion)
	{
		pNode->RenderObjectBlobs(pOnTopRegion);

		// Go find the next region
		pOnTopRegion = DocView::GetNextOnTop(Rect);
	}
}

/********************************************************************************************

>	void BlobManager::RenderRequiredBlobs(NodeRenderable* pNode, RenderRegion* pRegion)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		pNode - The Node whos blobs need rendering
				pRegion - The render region to draw into
	Purpose:	Renders the required blobs for the given node into the given render region.
				The required blobs are specified by the tool when it is activated.

********************************************************************************************/

void BlobManager::RenderRequiredBlobs(NodeRenderable* pNode, RenderRegion* pRegion)
{
	// We know that the node is a renderable ink as it is selected
	ENSURE( pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)), "Selected object was not Renderable!");

	// Render the Object Blobs of this node
	if (IsObjectBlob)
		pNode->RenderObjectBlobs(pRegion);

	// Render the Artistic blobs of this node
	if (IsArtisticBlob)
		pNode->RenderArtisticBlobs(pRegion);

	// Render the Fill blobs of this node
	if (IsFillBlob)
		pNode->RenderAppliedFillBlobs(pRegion);

	// Render the Fill blobs of this node
	if (IsEffectBlob)
		pNode->RenderEffectBlobs(pRegion);

	// Render the tiny blobs of this node
	if (IsTinyBlob)
		pNode->RenderTinyBlobs(pRegion);

	// Render the pen blobs of this node
	if (IsPenBlob)
		pNode->RenderPenBlobs(pRegion);

	// Render the ToolObject blobs of this node
	if (IsToolObjectBlob)
		pNode->RenderToolObjectBlobs(pRegion);
}

/********************************************************************************************

>	void BlobManager::RenderRequiredBlobsOnSelection(DocRect* Rect)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/95
	Inputs:		pNode - The Node whos blobs need rendering
				pRegion - The render region to draw into
	Purpose:	Renders the required blobs for the given node into the given render region.
				The required blobs are specified by the tool when it is activated.

********************************************************************************************/

void BlobManager::RenderRequiredBlobsOnSelection(DocRect* Rect)
{
	// Find out which spread the selection is on
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	// go and render the blobs if there are any selected nodes
	if (pNode!=NULL && DrawBlobsAtAll)
	{
		Spread* pSpread = pNode->FindParentSpread();

		// Render what we need to
		RenderRegion* pRegion = DocView::RenderOnTop(Rect, pSpread, ClippedEOR);
		while (pRegion)
		{
			// Find the first selected object in the tree;
			Node* pNode = Selected->FindFirst();
			while(pNode)
			{
				// Render only those blobs that are changing state (ie those appearing or disappearing)
				RenderRequiredBlobs((NodeRenderable*) pNode, pRegion);

				// Find the next selected node to render
				pNode = Selected->FindNext(pNode);
			}

			// Go find the next region
			pRegion = DocView::GetNextOnTop(Rect);
		}
	}

	// Bodge to stop fill meshes EOR each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;
}

/********************************************************************************************

>	void BlobManager::RenderSpecificBlobs(NodeRenderable* pNode, RenderRegion* pRegion, 
				  BOOL DrawObject, BOOL DrawArtistic, BOOL DrawFill, BOOL DrawTiny,
				  BOOL DrawPen, BOOL DrawToolObject, BOOL DrawEffect)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		pNode - The node to draw the blobs for
				pRegion - The region to render them into
				DrawObject - Do we want to draw the Object Blobs
				DrawArtistic - Do we want to draw the Artistic Blobs
				DrawFill - Do we want to draw the Fill Blobs
				DrawTiny - Do we want to draw the Tiny Blobs
				DrawPen -  Do we want to draw the Pen Blobs
				DrawToolObject - Do we want to draw the ToolObject Blobs
	Purpose:	Renders only the blobs specified. This is called from the Add and Remove
				Interest Functions to add new types of blob to the display or take types of
				blob that are no longer needed off the display.

********************************************************************************************/

void BlobManager::RenderSpecificBlobs(NodeRenderable* pNode, RenderRegion* pRegion, 
				  BOOL DrawObject, BOOL DrawArtistic, BOOL DrawFill, BOOL DrawTiny,
				  BOOL DrawPen, BOOL DrawToolObject, BOOL DrawEffect)
{
	// We know that the node is a renderable ink as it is selected
	ENSURE( pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)), "Selected object was not Renderable!");

	// Render the Object Blobs of this node
	if (DrawObject)
		pNode->RenderObjectBlobs(pRegion);

	// Render the Artistic blobs of this node
	if (DrawArtistic)
		pNode->RenderArtisticBlobs(pRegion);

	// Render the Fill blobs of this node
	if (DrawFill)
		pNode->RenderAppliedFillBlobs(pRegion);

	// Render the tiny blobs of this node
	if (DrawTiny)
		pNode->RenderTinyBlobs(pRegion);

	// Render the Pen blobs of this node
	if (DrawPen)
		pNode->RenderPenBlobs(pRegion);

	// Render the ToolObject blobs of this node
	if (DrawToolObject)
		pNode->RenderToolObjectBlobs(pRegion);

	// Render the Effect blobs of this node
	if (DrawEffect)
		pNode->RenderEffectBlobs(pRegion);
}

/********************************************************************************************

>	void BlobManager::RenderSpecificBlobsOnSelection(BOOL DrawObject, BOOL DrawArtistic, 
													 BOOL DrawFill, BOOL DrawTiny,
													 BOOL DrawPen, BOOL DrawToolObject, BOOL DrawEffect)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Inputs:		pNode - The node to draw the blobs for
				pRegion - The region to render them into
				DrawObject - Do we want to draw the Object Blobs
				DrawArtistic - Do we want to draw the Artistic Blobs
				DrawFill - Do we want to draw the Fill Blobs
				DrawTiny - Do we want to draw the Tiny Blobs
				DrawPen -  Do we want to draw the Pen Blobs
				DrawToolObject - Do we want to draw the ToolObject Blobs
	Purpose:	Renders only the blobs specified. This is called from the Add and Remove
				Interest Functions to add new types of blob to the display or take types of
				blob that are no longer needed off the display.

********************************************************************************************/

void BlobManager::RenderSpecificBlobsOnSelection(	DocRect* Rect, BOOL DrawObject,
													BOOL DrawArtistic, BOOL DrawFill,
													BOOL DrawTiny, BOOL DrawPen,
													BOOL DrawToolObject, BOOL DrawEffect )
{
	// Find out which spread the selection is on
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	// go and render the blobs if there are any selected nodes
	if (pNode!=NULL && DrawBlobsAtAll)
	{
		Spread* pSpread = pNode->FindParentSpread();

		// Render what we need to
		RenderRegion* pRegion = DocView::RenderOnTop(Rect, pSpread, ClippedEOR);
		while (pRegion)
		{
			// Find the first selected object in the tree;
			Node* pNode = Selected->FindFirst();
			while(pNode)
			{
				// Render only those blobs that are changing state (ie those appearing or disappearing)
				RenderSpecificBlobs((NodeRenderable*)pNode, pRegion, DrawObject, DrawArtistic,
									DrawFill, DrawTiny, DrawPen, DrawToolObject, DrawEffect);

				// Find the next selected node to render
				pNode = Selected->FindNext(pNode);
			}

			// Go find the next region
			pRegion = DocView::GetNextOnTop(Rect);
		}
	}

	// Bodge to stop fill meshes EOR each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;
}

/********************************************************************************************

>	MsgResult BlobManager::Message(Msg* Message)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Inputs:		Message - the message to examine
	Returns:	-
	Purpose:	Handles the View Changing message, and makes sure that the blobs are
				removed from the old view and drawn onto the new view

********************************************************************************************/

MsgResult BlobManager::Message(Msg* Message)
{
	if (MESSAGE_IS_A(Message, DocViewMsg))
	{
		// Get a version of the message in the right type
		DocViewMsg* pDocViewMsg = (DocViewMsg*) Message;

		switch (pDocViewMsg->State)
		{
			// respond to messages that arrive just before and just after the view has changed.
			case DocViewMsg::SELABOUTTOCHANGE:
			case DocViewMsg::SELCHANGED:
			{
				// rub blobs off old view or draw them in on the new one
				if (DocView::GetSelected()!=NULL)
				{
					// go find the selection
					SelRange* Selected = GetApplication()->FindSelection();
					
					// Have to be sure that the selection is correct for the view
					Selected->Update();
					Node* pNode = Selected->FindFirst();
			
					// If there is something selected
					if (pNode!=NULL)
					{
						// Find its spread
						Spread* pSpread = pNode->FindParentSpread();
						if (pSpread!=NULL)
						{
							// and render the blobs
							Render(NULL, pSpread);
						}
					}
				}
				break;
			}


			default:
				break;
		}
	}

	return OK; 
}      

/********************************************************************************************

>	void BlobManager::RenderToolBlobsOn(Tool_v1* pTool, Spread* pSpread, DocRect* pClipRect)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Purpose:	This function will render on the specified tools blobs.

********************************************************************************************/

void BlobManager::RenderToolBlobsOn(Tool_v1* pTool, Spread* pSpread, DocRect* pClipRect)
{
	// If we are not drawing blobs today, then return straight away
	if (!DrawBlobsAtAll)
		return;

	// Put the tool blobs on
	pTool->RenderToolBlobs(pSpread, pClipRect);

	// Now EOR off in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will put them on
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();

	if (!pRegionList->IsEmpty())
	{
		RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

		while (pRegion)
		{
			// Check the RenderRegion is for the same spread.
			if (pRegion->GetRenderSpread() == pSpread && pRegion->GetRenderView()==DocView::GetSelected())
			{
				// Render the blobs 'clipped' to this Render Region.
				DocRect ClipRect = pRegion->GetRegionRect();
				pTool->RenderToolBlobs(pSpread, &ClipRect);
			}

			// Get the Next render region
			pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
		}
	}
	bToolBlobsAreOff = FALSE;
}

/********************************************************************************************

>	void BlobManager::RenderToolBlobsOff(Tool_v1* pTool, Spread* pSpread, DocRect* pClipRect)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Purpose:	This function will render off the specified tools blobs.

********************************************************************************************/

void BlobManager::RenderToolBlobsOff(Tool_v1* pTool, Spread* pSpread, DocRect* pClipRect)
{
	// If we are not drawing blobs today, then return straight away
	if (!DrawBlobsAtAll)
		return;

//	if (bToolBlobsAreOff)
//		return;

	// EOR on in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will take them off
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();

	if (!pRegionList->IsEmpty())
	{
		RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

		while (pRegion)
		{
			// Check the RenderRegion is for the same spread.
			if (pRegion->GetRenderSpread() == pSpread && pRegion->GetRenderView()==DocView::GetSelected())
			{
				// Render the blobs 'clipped' to this Render Region.
				DocRect ClipRect = pRegion->GetRegionRect();
				pTool->RenderToolBlobs(pSpread, &ClipRect);
			}

			// Get the Next render region
			pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
		}
	}

	// Take the tool blobs off
	pTool->RenderToolBlobs(pSpread, pClipRect);
	bToolBlobsAreOff = TRUE;
}

/********************************************************************************************

>	BOOL BlobManager::NeedToRenderSelectionBlobs(DocRect* Rect)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/95
	Purpose:	Check to make sure we need to render some blobs.

********************************************************************************************/

BOOL BlobManager::NeedToRenderSelectionBlobs(DocRect* Rect)
{
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	if (pRegionList->IsEmpty())
		return TRUE;

	// Find the first selected object in the tree;
	SelRange* Selected = GetApplication()->FindSelection();

	if (Selected->Count() == 0)
		return FALSE;		// Nothing to render

	Node* pNode = Selected->FindFirst();
	Spread* pSpread = pNode->FindParentSpread();

	// Find out the blob rect of the selection
	DocRect SelRect = Selected->GetBlobBoundingRect();
	
	// If a clip rect was given, and it doesn't completely contain the sel bounds,
	// then we'll have to use that
   	if (Rect && !Rect->ContainsRect(SelRect))
		SelRect = *Rect;

	if (!SelRect.IsEmpty())
	{
		// So lets check the pending regions
		RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

		while (pRegion)
		{
			// Check the RenderRegion is for the same spread.
			if (pRegion->GetRenderSpread() == pSpread &&
				pRegion->GetRenderView()==DocView::GetSelected() &&
				(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
			{
				// Get the 'clip' rect of this Render Region.
				DocRect ClipRect = pRegion->GetRegionRect();

				// If any of the RenderRegions completely contain the rect
				// then there is no point in rendering anything

				if (ClipRect.ContainsRect(SelRect))
					return FALSE;	 	// We don't need to render any blobs
			}

			// Get the Next render region
			pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
		}
	} 
	return TRUE;
}

/********************************************************************************************

>	BOOL BlobManager::NeedToRenderNodeBlobs(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/95
	Purpose:	Check to make sure we need to render some blobs.

********************************************************************************************/

BOOL BlobManager::NeedToRenderNodeBlobs(DocRect* Rect, Spread *pSpread, NodeRenderable* pNode)
{
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	if (pRegionList->IsEmpty())
		return TRUE;

	if (Rect != NULL || pNode->IsBounded())
	{
		// If the Blob we are about to render, is completely contained
		// within a pending Render Region, then we don't need to
		// render anything

		// Find out the blob rect of this node, if we can
		DocRect NodeRect = DocRect(0,0,0,0);
		
		if (pNode->IsBounded())
		{
			// Use the node's blob bounds
			NodeRect = ((NodeRenderableBounded*)pNode)->GetBlobBoundingRect();
		}

		// If a clip rect was given, and it doesn't completely contain the node rect,
		// then we'll have to use that
	   	if (Rect && !Rect->ContainsRect(NodeRect))
			NodeRect = *Rect;

		if (!NodeRect.IsEmpty())
		{
			// So lets check the pending regions
			RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

			while (pRegion)
			{
				// Check the RenderRegion is for the same spread.
				if (pRegion->GetRenderSpread() == pSpread &&
					pRegion->GetRenderView()==DocView::GetSelected() &&
					(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
				{
					// Get the 'clip' rect of this Render Region.
					DocRect ClipRect = pRegion->GetRegionRect();

					// If any of the RenderRegions completely contain the rect
					// then there is no point in rendering anything

					if (ClipRect.ContainsRect(NodeRect))
						return FALSE;	 	// We don't need to render this node's blobs
				}

				// Get the Next render region
				pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
			}
		}
	}
	return TRUE;
}
