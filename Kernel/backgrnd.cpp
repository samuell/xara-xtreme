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
// OpBackground implementation

/*
*/

#include "camtypes.h"

#include "backgrnd.h"	// OpBackground

#include "document.h"
#include "spread.h"
#include "page.h"
#include "layer.h"
#include "sprdmsg.h"	// SpreadMsg
#include "nodershp.h"	// NodeRegularShape
#include "lineattr.h"	// AttrStrokeColourChange
#include "attrmgr.h"	// AttributeManager
#include "qualattr.h"	// QualityAttribute

//#include "galstr.h"		// _R(IDS_BACKGROUNDLAYERNAME)

CC_IMPLEMENT_DYNCREATE(OpBackground, UndoableOperation)
CC_IMPLEMENT_MEMDUMP(OpBackgroundParam, OpParam)

#define new CAM_DEBUG_NEW

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

#if !defined(EXCLUDE_FROM_RALPH)

/***********************************************************************************************

>	OpBackground::OpBackground()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:    Default constructor
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

OpBackground::OpBackground()
{
//	m_pDoc = NULL;
//	m_pSpread = NULL;
//	m_pDocColour = NULL;
//	m_pBitmap = NULL;

	// Versions of the parameters used in the drag and drop process
//	m_pDroppedBitmap = NULL;
//	m_pDroppedDocColour = NULL;
}


/***********************************************************************************************

>	OpBackground::~OpBackground()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:    Default destructor
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

OpBackground::~OpBackground()
{
}

/***********************************************************************************************

>	void OpBackground::DoWithParam(OpDescriptor* pOpDesc,OpParam* pParam)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/97
	Inputs:		pOpDesc = ptr op desc
				pOpParam = param block to use
	Outputs:	-
	Returns:	-
	Purpose:    This function is used to create a guideline in the selected spread.
				It calls DoAddNewGuideline() with pParam->Param1 specifying the guideline type.
	SeeAlso:	DoAddNewGuideline(), OilRuler::OnLButtonDown()
		
***********************************************************************************************/

void OpBackground::DoWithParam(OpDescriptor* pOpDesc,OpParam* pParam)
{
	ERROR3IF(pParam == NULL,"pParam is NULL");

	if (pParam != NULL)
	{
		BOOL ok = TRUE;

		OpBackgroundParam* pOpBackgroundParam = (OpBackgroundParam*)pParam;
		Document *		pDoc		= pOpBackgroundParam->pDoc;
		Spread *		pSpread		= pOpBackgroundParam->pSpread;
		DocColour *		pDocColour	= pOpBackgroundParam->pDocColour;
		KernelBitmap *	pBitmap		= pOpBackgroundParam->pBitmap;

		// If no spread is given then use the selected one
		if (pSpread == NULL)
		{
			// We will work off the currently selected spread
			pSpread = Document::GetSelectedSpread();
		}

		// Go and do the job of making the page background
		ok = DoMakePageBackground(this, pSpread, pBitmap, pDocColour);
	}

	End();
}

/***********************************************************************************************

>	static BOOL OpBackground::MakePageBackground(UndoableOperation * pOp, Spread * pSpread, DocColour * pDocColour)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/97
	Inputs:		pOp			- the operation to use
				pSpread		- the spread to apply the background to
				pDocColour	- the doc colour to apply
	Outputs:	-
	Returns:	True if worked ok, False otherwise.
	Purpose:    Tries to apply the DocColour or bitmap as a new page background.
				
				It is up to the caller to call End() on the operation. This is so
				that an outside op should be able to call this and everything should
				work ok.
	SeeAlso:	-
		
***********************************************************************************************/

BOOL OpBackground::MakePageBackground(UndoableOperation * pOp, Spread * pSpread, DocColour * pDocColour)
{
	ERROR2IF(pOp == NULL || pSpread == NULL || pDocColour == NULL,FALSE,"MakePageBackground Bad params!");

	// Get the op to do its stuff
	BOOL ok = DoMakePageBackground(pOp, pSpread, NULL, pDocColour);

	return ok;
}

/***********************************************************************************************

>	static BOOL OpBackground::MakePageBackground(UndoableOperation * pOp, Spread * pSpread, KernelBitmap * pBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/97
	Inputs:		pOp			- the operation to use
				pSpread		- the spread to apply the background to
				pBitmap		- the bitmap to apply
	Outputs:	-
	Returns:	True if worked ok, False otherwise.
	Purpose:    Tries to apply the DocColour or bitmap as a new page background.
				
				It is up to the caller to call End() on the operation. This is so
				that an outside op should be able to call this and everything should
				work ok.
	SeeAlso:	-
		
***********************************************************************************************/

BOOL OpBackground::MakePageBackground(UndoableOperation * pOp, Spread * pSpread, KernelBitmap * pBitmap)
{
	ERROR2IF(pOp == NULL || pSpread == NULL || pBitmap == NULL,FALSE,"MakePageBackground Bad params!");

	// Get the op to do its stuff
	BOOL ok = DoMakePageBackground(pOp, pSpread, pBitmap);

	return ok;
}

/***********************************************************************************************

>	static BOOL OpBackground::DoMakePageBackground(UndoableOperation * pOp, Spread * pSpread, KernelBitmap * pBitmap,
												   DocColour * pDocColour = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/97
	Inputs:		pOp			- the operation to use
				pSpread		- the spread to apply the background to
				pBitmap		- the bitmap to apply
				pDocColour	- the doc colour to apply
	Outputs:	-
	Returns:	True if worked ok, False otherwise.
	Purpose:    Tries to apply the DocColour or bitmap as a new page background.
				
				It is up to the caller to call End() on the operation. This is so
				that an outside op should be able to call this and everything should
				work ok.
	SeeAlso:	-
		
***********************************************************************************************/

BOOL OpBackground::DoMakePageBackground(UndoableOperation * pOp, Spread * pSpread, KernelBitmap * pBitmap,
										DocColour * pDocColour)
{
	// Our inputs are actually set up in the class variables, so check them
	ERROR2IF(pSpread == NULL || pOp == NULL,FALSE,"DoMakePageBackground Bad params!");
	ERROR2IF(pDocColour == NULL && pBitmap == NULL,FALSE,"DoMakePageBackground Bad params!");

	BOOL	ok		= TRUE;
	Page	*pPage	= pSpread->FindFirstPageInSpread ();
	DocRect	Bounds	= pPage->GetPageRect ();

	// If there's a transparent background colour, just hide the existing node.
	if ( pDocColour != NULL && pDocColour->IsTransparent () )
	{
		Layer*	pOldLayer	= pSpread->FindFirstPageBackgroundLayer ();

		// Attempt to remove the old background layer.
		if ( pOldLayer != NULL )
		{
			if ( !pOp->DoHideNode(pOldLayer, TRUE ) )
			{
				// Something went wrong, so handle the error.
				ERROR3( "OpBackground failed to hide the old page background" );
				pOp->FailAndExecute ();
				ok = FALSE;
			}
		}
	}

	// Otherwise create a new background layer.
	else
	{
		Layer* pBackgroundLayer = DoCreateBackgroundLayer(pOp, pSpread);

		if (pBackgroundLayer == NULL)
		{
			ERROR3("Something went wrong with the layer creation");
			pOp->FailAndExecute();
			ok = FALSE;
		}
		else
		{
			// Try and find an existing page filling rectangle on this layer
			NodeRegularShape * pShape = DoFindPageRectangle(pSpread, pBackgroundLayer);

			// If not found then create a new one
			if (pShape != NULL)
			{
				// Hide the old version of the rectangle
				if (!pOp->DoHideNode(pShape, TRUE))
				{
					ERROR3("OpBackground failed to hide the old node");
					pOp->FailAndExecute();
					ok = FALSE;
				}
			}

			if (ok)
			{
				// Make a new rectangle of the correct size
				pShape = DoCreateRectangle(pSpread);

				// Apply the bitmap or the colour to it before we insert it into the tree.
				if (pDocColour)
				{
					// Set the colour to be the background.
					ok = ApplyDocColour(pDocColour, pShape);
				}
				else if (pBitmap)
				{
					ok = ApplyBitmap(pBitmap, pShape);
				}
				else
				{
					ERROR3("OpBackground Haven't got anything to apply");
					pOp->FailAndExecute();
					ok = FALSE;
				}

				// Apply a useful set of default attibutes on top of the fill colour
				// or bitmap fill.
				if ( pShape != NULL )
				{
					ApplyDefaultAttributes(pShape);
				}
			}

			// We must make the layer unlocked so that we don't get the stupid
			// inserting on locked layer message
			pBackgroundLayer->SetLocked(FALSE);

			// And now insert it into the background layer
			if (ok && !pOp->DoInsertNewNode(pShape, pBackgroundLayer, FIRSTCHILD,FALSE,
											FALSE,FALSE,FALSE))
			{
				delete pShape;
				pShape = NULL;
				pOp->FailAndExecute();
				ok = FALSE;
			}

			// Set the layer back to locked as we have finished inserting onto the layer
			pBackgroundLayer->SetLocked(TRUE);

			// Since we prevented DoInsertnewNode from invalidating, it won't have cleared
			// the cache appropriately so we will have to do that ourselves...
			if (ok)
				pBackgroundLayer->ReleaseCached();
		}
	}

	// Force a redraw of the new background layer.
	if ( ok && !pOp->DoInvalidateRegion ( pSpread, Bounds ) )
	{
		pOp->FailAndExecute();
		ok = FALSE;
	}

	return ok;
}

/***********************************************************************************************

>	static NodeRegularShape * OpBackground::DoFindPageRectangle(Spread * pSpread, Layer* pBackgroundLayer)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		pSpread				- the spread to apply the background to
				pBackgroundLayer	- the layer to search for the shape on
	Outputs:	-
	Returns:	The regular shape, if found
	Purpose:    Tries to find the page filling rectangle on the specified layer.
	SeeAlso:	-
		
***********************************************************************************************/

NodeRegularShape * OpBackground::DoFindPageRectangle(Spread * pSpread, Layer* pBackgroundLayer)
{
	if (pSpread == NULL || pBackgroundLayer == NULL)
		return NULL;

	// We want to find the rectangle which should be the same size as the current union of
	// all the pages on the current spread
	DocRect PagesRect;
	pSpread->GetPagesRect(&PagesRect);

	return FindPageRectangle(pBackgroundLayer, PagesRect);
}

/***********************************************************************************************

>	static NodeRegularShape * OpBackground::FindPageRectangle(Layer* pBackgroundLayer, const DocRect& PagesRect)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/97
	Inputs:		The layer to search for the shape on
				The encompassing rectangle of the current pages
	Outputs:	-
	Returns:	The regular shape, if found
	Purpose:    Tries to find the page filling rectangle on the specified layer.
	SeeAlso:	-
		
***********************************************************************************************/

NodeRegularShape * OpBackground::FindPageRectangle(Layer* pBackgroundLayer, const DocRect& PagesRect)
{
	ERROR2IF(pBackgroundLayer == NULL,FALSE,"OpBackground::FindPageRectangle Bad params error!");

	NodeRegularShape *pShape = NULL;
	NodeRegularShape *pRegNode = NULL;
	Node * pNode = pBackgroundLayer->FindFirstChild();
	DocRect rect;
	const MILLIPOINT delta = 1000;
	while (pNode != NULL)
	{
		// Only check regular shape nodes 
		if (pNode->IS_KIND_OF(NodeRegularShape))
		{
			// Only check rectangles
			pRegNode = (NodeRegularShape *)pNode;
			if (pRegNode->IsARectangle())
			{
				//DocCoord MinorAxis NodeRegularShape::GetMinorAxes();
				// The bounding rect should be roughly what we want
				rect = pRegNode->GetBoundingRect(TRUE, FALSE);
				if (
					(PagesRect.lox > rect.lox - delta) && (PagesRect.lox < rect.lox + delta) &&
					(PagesRect.hix > rect.hix - delta) && (PagesRect.hix < rect.hix + delta) &&
					(PagesRect.loy > rect.loy - delta) && (PagesRect.loy < rect.loy + delta) &&
					(PagesRect.hiy > rect.hiy - delta) && (PagesRect.hiy < rect.hiy + delta)
				   )
				{
					pShape = pRegNode;
				}
			}
		}

		pNode = pNode->FindNext();
	}

	return pShape;
}

/***********************************************************************************************

>	static NodeRegularShape * OpBackground::DoCreateRectangle(Spread * pSpread)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		pSpread				- the spread to apply the background to
	Outputs:	-
	Returns:	The regular shape
	Purpose:    Returns a new rectangle the size of the current spread.
	SeeAlso:	-
		
***********************************************************************************************/

NodeRegularShape * OpBackground::DoCreateRectangle(Spread * pSpread)
{
	ERROR2IF(pSpread == NULL,FALSE,"OpBackground::DoCreateRectangle Bad params error!");

	if (pSpread == NULL)
		return NULL;

	// This assumes that we have already checked to see if there is a page covering rectangle
	// already present on this layer.

	// Create a new rectangle node on the layer
	NodeRegularShape *pShape = new NodeRegularShape;
	if (pShape != NULL)
	{
		BOOL ok = pShape->SetUpShape();

		// We want to make the rectangle the same size as the current union of
		// all the pages on the current spread
		DocRect Rect;
		pSpread->GetPagesRect(&Rect);

		/* // We must expand the rectangle by a single pixel as the page redraw
		// draws a single pixel line around the outside.
		const MILLIPOINT pixel = (MILLIPOINT)(72000.0/96.0);
		Rect.Inflate(pixel); */

		const INT32 CornerRadius = 0; // No curvature
		
		if (pShape->MakeRectangle(Rect.Width(), Rect.Height(), CornerRadius))
		{
			// Translate centre from 0,0 to required position relative to page
			INT32 XTrans = Rect.lox + (Rect.Width()/2);
			INT32 YTrans = Rect.loy + (Rect.Height()/2);
			Trans2DMatrix Trans(XTrans, YTrans);

			pShape->Transform(Trans);

			// finish off the shape
			pShape->InvalidateBoundingRect();
		}
	}

	return pShape;
}

/***********************************************************************************************

>	static BOOL OpBackground::ApplyDocColour(DocColour * pColourToApply, NodeRegularShape * pShape)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		The colour to apply
				The regular shape to apply the colour to
	Outputs:	-
	Returns:	
	Purpose:    Applies the specified colour to the specified NodeRegularShape.
	SeeAlso:	-
		
***********************************************************************************************/

BOOL OpBackground::ApplyDocColour(DocColour * pColourToApply, NodeRegularShape * pShape)
{
	ERROR2IF(pColourToApply == NULL || pShape == NULL,FALSE,"OpBackground::ApplyDocColour Bar params!")

	// Set up a fill colour attribute
	AttrFlatColourFill *pFillColAttr = new AttrFlatColourFill();
	if (pFillColAttr == NULL)
	{
		InformError();
		return(FALSE);
	}

	((AttrFillGeometry*)pFillColAttr)->SetStartColour(pColourToApply);

	// And now insert it as a child of the regular shape
	// Factoring should be ok as the Regular Shape is assumed to be the child of
	// layer
	pFillColAttr->AttachNode(pShape, FIRSTCHILD);

	return TRUE;
}

/***********************************************************************************************

>	static BOOL OpBackground::ApplyBitmap(KernelBitmap * pBitmapToApply, NodeRegularShape * pShape)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		The bitmap to apply
				The regular shape to apply the bitmap to
	Outputs:	-
	Returns:	
	Purpose:    Applies the specified bitmap to the specified NodeRegularShape.
	SeeAlso:	-
		
***********************************************************************************************/

BOOL OpBackground::ApplyBitmap(KernelBitmap * pBitmapToApply, NodeRegularShape * pShape)
{
	ERROR2IF(pBitmapToApply == NULL || pShape == NULL,FALSE,"OpBackground::ApplyBitmap Bar params!")

	// Set up a fill colour attribute
	AttrBitmapColourFill *pBitmapFillAttr = new AttrBitmapColourFill();
	if (pBitmapFillAttr == NULL)
	{
		InformError();
		return(FALSE);
	}

	// Set the coordinates of the fill end points
	// We want to start the fill at the top left of the regular shape
	// The bounding rect should be roughly what we want
	DocRect Rect = pShape->GetBoundingRect(TRUE, FALSE);
				
	// Get the size of the bitmap taking into account its DPI
	INT32 Width = pBitmapToApply->GetRecommendedWidth();
	INT32 Height = pBitmapToApply->GetRecommendedHeight();
	// You would think that the handles on the fill correspond to where you place them
	// but oh now the displayed handles are shown at different points. So instead of
	// positioning the start in the centre of the bitmap, you need to position it bottom left
	// The End should be centre and right but needs to be bottom right.
	// The End2 should be centre and top but needs to be top left.

	// We want the start point to be half the height of the bitmap below this
	//DocCoord Start(Rect.lox + Width/2, Rect.hiy - Height/2);
	DocCoord Start(Rect.lox, Rect.hiy - Height);
	// We want the other end point to be same height as the centre point
	// but on the far right of the rectangle i.e the full width across
	//DocCoord End(Rect.lox + Width, Rect.hiy - Height/2);
	DocCoord End(Rect.lox + Width, Rect.hiy - Height);
	// We want the end point to be middle and top of the rectangle
	//DocCoord End2(Rect.lox + Width/2, Rect.hiy);
	DocCoord End2(Rect.lox, Rect.hiy);

	((AttrFillGeometry*)pBitmapFillAttr)->SetStartPoint(&Start);
	((AttrFillGeometry*)pBitmapFillAttr)->SetEndPoint(&End);
	((AttrFillGeometry*)pBitmapFillAttr)->SetEndPoint2(&End2);

	// Now attach the correct bitmap to the fill
	((AttrFillGeometry*)pBitmapFillAttr)->AttachBitmap(pBitmapToApply);
	// It would seem that this is necessary to stop kernel bitmap leaks
	// It only seems to call the delete in the case where we drag a bitmap
	// from say the bitmap gallery to the page
	if (((AttrFillGeometry*)pBitmapFillAttr)->GetBitmap() != pBitmapToApply)
	{
		// It didn't use the bitmap we gave it, so we can delete it
		delete pBitmapToApply;
	}

	// And now insert it as a child of the regular shape
	// Factoring should be ok as the Regular Shape is assumed to be the child of
	// layer
	pBitmapFillAttr->AttachNode(pShape, FIRSTCHILD);

	return TRUE;
}

/***********************************************************************************************

>	static BOOL OpBackground::ApplyDefaultAttributes(NodeRegularShape * pShape)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/97
	Inputs:		The regular shape to apply the attributes to
	Outputs:	-
	Returns:	
	Purpose:    Applies some useful default attributes to the specified NodeRegularShape.
	SeeAlso:	-
		
***********************************************************************************************/

BOOL OpBackground::ApplyDefaultAttributes(NodeRegularShape * pShape)
{
	ERROR2IF(pShape == NULL,FALSE,"OpBackground::ApplyDefaultAttributes Bar params!")

	// *****
	// Set up a no colour line colour
	AttrStrokeColour * pLineColAttr = new AttrStrokeColour();
	if (pLineColAttr == NULL)
	{
		InformError();
		return(FALSE);
	}

	((AttrFillGeometry*)pLineColAttr)->SetStartColour(&DocColour(COLOUR_NONE));
	// And now insert it as a child of the regular shape
	// Factoring should be ok as the Regular Shape is assumed to be the child of
	// layer
	pLineColAttr->AttachNode(pShape, LASTCHILD);



	// *****
	// Set up a line width attribute
	AttrLineWidth *pLineWidthAttr = new AttrLineWidth();
	if (pLineWidthAttr == NULL)
	{
		InformError();
		return(FALSE);
	}

	((AttrLineWidth*)pLineWidthAttr)->Value.LineWidth = 0;

	// And now insert it as a child of the regular shape
	// Factoring should be ok as the Regular Shape is assumed to be the child of
	// layer
	pLineWidthAttr->AttachNode(pShape, LASTCHILD);


	// *****
	// Set up a join type attribute
	AttrJoinType  * pJoinTypeAttr = new AttrJoinType();
	if (pLineWidthAttr == NULL)
	{
		InformError();
		return(FALSE);
	}

	// We will use a default constructed one as this should be vaguely correct
	// ((AttrLineWidth*)pJoinTypeAttr)->Value.LineWidth = 0;

	// And now insert it as a child of the regular shape
	// Factoring should be ok as the Regular Shape is assumed to be the child of
	// layer
	pJoinTypeAttr->AttachNode(pShape, LASTCHILD);

	return TRUE;
}


/***********************************************************************************************

>	static Layer* OpBackground::DoCreateBackgroundLayer(UndoableOperation * pOp, Spread * pSpread)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/97
	Inputs:		pOp					- The operation to use
				pSpread				- the spread to apply the background to
	Outputs:	-
	Returns:	Ptr to the background layer, or NULL one couldn't be created.
	Purpose:    This returns a ptr to the background layer.

				If the background layer already exists, a ptr to it is returned.

				If there isn't a background layer, it is created and inserted into the doc tree.
				If this fails the func returns NULL

	SeeAlso:	OpBackground::DoNewGuideline()
		
***********************************************************************************************/

Layer* OpBackground::DoCreateBackgroundLayer(UndoableOperation * pOp, Spread * pSpread)
{
	ERROR2IF(pOp == NULL || pSpread == NULL,FALSE,"OpBackground::DoCreateBackgroundLayer Bad params error!");

	// Search for our special page background layer
	Layer* pFoundLayer = pSpread->FindFirstPageBackgroundLayer();
	// If we have found it then return this to the caller as we don't want
	// more than one present!
	if (pFoundLayer != NULL)
		return pFoundLayer;

	// We didn't find a page background layer so go and create a new one
	Layer* pNewLayer = CreateBackgroundLayer();
	if (pNewLayer != NULL)
	{
		// If we insert the new layer as the FIRSTCHILD then this will be like the guidelayer
		// and behind the page. If we insert as the last child then we will be at the front of
		// the stacking order. We need to insert ourselves as the node after the last page node
		Page *pLastPage = pSpread->FindLastPageInSpread();

		// Insert the new layer as the next node after the last page.
		if (pLastPage && pOp->DoInsertNewNode(pNewLayer, pLastPage, NEXT, FALSE,FALSE,FALSE,FALSE))
		{
			pNewLayer->EnsureUniqueLayerID();
			BROADCAST_TO_ALL(SpreadMsg(pSpread, SpreadMsg::SpreadReason::LAYERCHANGES));
		}
		else
		{
			delete pNewLayer;
			pNewLayer = NULL;
		}
	}

	return pNewLayer;
}



/***********************************************************************************************

>	static Layer* OpBackground::CreateBackgroundLayer()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/97
	Inputs:		-
	Outputs:	-
	Returns:	Ptr to a background layer that's ready to be inserted into the tree
	Purpose:    This does everything required in creating a layer fit to be called the background layer
	SeeAlso:	-
		
***********************************************************************************************/

Layer* OpBackground::CreateBackgroundLayer()
{
	Layer* pNewLayer = new Layer;
	if (pNewLayer != NULL)
	{
		String_256 LayerName(_R(IDS_BACKGROUNDLAYERNAME));

		pNewLayer->SetVisible(TRUE); 
		pNewLayer->SetLocked(TRUE); 
		pNewLayer->SetPrintable(FALSE); 
		pNewLayer->SetBackground(TRUE); 
		pNewLayer->SetOutline(FALSE);
		pNewLayer->SetGuide(FALSE);
		pNewLayer->SetPageBackground(TRUE);		// This is the special page background layer flag
		pNewLayer->SetLayerID(LayerName); 

/*		// Ensure we are always rendered at maximum quality so that turning WYSIWYG
		// down does not remove the page background!
		Quality Qual;
		Qual.SetQuality(QUALITY_MAX);
		QualityAttribute QualAttr(Qual);
		NodeAttribute* pNodeAttr = QualAttr.MakeNode();
		pNodeAttr->AttachNode(pNewLayer, FIRSTCHILD); */
	}

	return pNewLayer;
}

/***********************************************************************************************

>	static BOOL OpBackground::FixBackgroundLayer(Layer * pBackgroundLayer, const DocRect& NewPagesRect,
												 const DocRect& OldPagesRect)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/97
	Inputs:		The background layer to fix
				The encompassing size of the new pages in the spread
				The encompassing size of the old pages in the spread
	Outputs:	-
	Returns:	Ptr to a background layer that's ready to be inserted into the tree
	Purpose:    This tries to fix up the special page background layer so that the rectangle
				on it is resized to the cover the newly resized pages on the spread.
	SeeAlso:	Spread::SetPageSize;
		
***********************************************************************************************/

BOOL OpBackground::FixBackgroundLayer(Layer * pBackgroundLayer, const DocRect& NewPagesRect,
									  const DocRect& OldPagesRect)
{
	ERROR2IF(pBackgroundLayer == NULL,FALSE,"OpBackground::FixBackgroundLayer Bad params!");
	
	//BOOL ok = TRUE;

	// There should be a rectangle on the page encompassing the old position of the pages
	// so go and try and find it
	NodeRegularShape * pShape = FindPageRectangle(pBackgroundLayer, OldPagesRect);
	// If no rectangle is found then something might be slightly wrong. Should we error?
	if (pShape)
	{
		const INT32 CornerRadius = 0; // No curvature
		if (pShape->MakeRectangle(NewPagesRect.Width(), NewPagesRect.Height(), CornerRadius))
		{
			// Translate centre from 0,0 to required position relative to page
			INT32 XTrans = NewPagesRect.lox + (NewPagesRect.Width()/2);
			INT32 YTrans = NewPagesRect.loy + (NewPagesRect.Height()/2);
			Trans2DMatrix Trans(XTrans, YTrans);

			pShape->Transform(Trans);

			// finish off the shape
			pShape->InvalidateBoundingRect();
		}
	
		// Find any bitmap fills and fix up the fill origin etc.
		Node * pNode = pShape->FindFirstChild();
		while (pNode != NULL)
		{
			// Only check and fix bitmap colour fill attributes of the regular shape nodes 
			if (pNode->IS_KIND_OF(AttrBitmapColourFill))
			{
				// Convert the pointer to the correct type
				AttrBitmapColourFill * pBitmapFillAttr = (AttrBitmapColourFill*)pNode;

				// Set the coordinates of the fill end points
				// We want to start the fill at the top left of the regular shape
				// The bounding rect should be roughly what we want
				DocRect Rect = pShape->GetBoundingRect(TRUE, FALSE);
							
				KernelBitmap * pBitmap = ((AttrFillGeometry*)pBitmapFillAttr)->GetBitmap();				
				// Get the size of the bitmap taking into account its DPI
				INT32 Width = pBitmap->GetRecommendedWidth();
				INT32 Height = pBitmap->GetRecommendedHeight();
				// You would think that the handles on the fill correspond to where you place them
				// but oh now the displayed handles are shown at different points. So instead of
				// positioning the start in the centre of the bitmap, you need to position it bottom left
				// The End should be centre and right but needs to be bottom right.
				// The End2 should be centre and top but needs to be top left.

				// We want the start point to be half the height of the bitmap below this
				//DocCoord Start(Rect.lox + Width/2, Rect.hiy - Height/2);
				DocCoord Start(Rect.lox, Rect.hiy - Height);
				// We want the other end point to be same height as the centre point
				// but on the far right of the rectangle i.e the full width across
				//DocCoord End(Rect.lox + Width, Rect.hiy - Height/2);
				DocCoord End(Rect.lox + Width, Rect.hiy - Height);
				// We want the end point to be middle and top of the rectangle
				//DocCoord End2(Rect.lox + Width/2, Rect.hiy);
				DocCoord End2(Rect.lox, Rect.hiy);

				((AttrFillGeometry*)pBitmapFillAttr)->SetStartPoint(&Start);
				((AttrFillGeometry*)pBitmapFillAttr)->SetEndPoint(&End);
				((AttrFillGeometry*)pBitmapFillAttr)->SetEndPoint2(&End2);
			}
		
			pNode = pNode->FindNext();
		}		
	}

	return TRUE;
}


/***********************************************************************************************

>	static BOOL OpBackground::GetPageColour(Spread *pSpread, KernelBitmap **ppOutBitmap, 
																DocColour **ppOutColour)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/97
	Inputs:		pSpread - a spread
	Outputs:	ppOutBitmap - receives the page bitmap, if a bitmap colour fill is applied to it
				ppOutColour - receives the page colour, if a flat colour fill is applied to it
	Returns:	TRUE, if succesful, FALSE otherwise
	Purpose:    Gets the page's fill attribute, which could only be either a colour, or a bitmap
		
***********************************************************************************************/

BOOL OpBackground::GetPageColour(Spread *pSpread, KernelBitmap **ppOutBitmap, 
														DocColour **ppOutColour)
{
	ERROR2IF(pSpread == NULL,FALSE,"OpBackground::GetPageColour Bad params error!");
	ERROR2IF(ppOutBitmap == NULL || ppOutColour == NULL,FALSE,"OpBackground::GetPageColour Bad params error!");

	// Search for our special page background layer
	Layer* pFoundLayer = pSpread->FindFirstPageBackgroundLayer();
	if (pFoundLayer == NULL)
		return FALSE;

	// search for our page node
	NodeRegularShape *pNode = DoFindPageRectangle(pSpread, pFoundLayer);

	if (!pNode)
		return FALSE;

	// find the fill attribute applied to the page
	NodeAttribute *pAppliedAttr = NULL;
	pNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),&pAppliedAttr);

	if (pAppliedAttr != NULL)
	{
		if (IS_A(pAppliedAttr, AttrFlatColourFill)) // flat colour fill?
		{
			// get the colour attribute
			ColourFillAttribute *pColAttr = (ColourFillAttribute *)(pAppliedAttr->GetAttributeValue());

			// set the colour pointer to the doc colour, and the bitmap pointer to NULL
			*ppOutBitmap = NULL;
			*ppOutColour = pColAttr->GetStartColour();

		}
		else if (IS_A(pAppliedAttr, AttrBitmapColourFill)) // bitmap fill
		{

			// set the colour pointer to NULL, and the bitmap pointer to the kernel bitmap
			*ppOutBitmap = ((AttrFillGeometry *)pAppliedAttr)->GetBitmap();
			*ppOutColour = NULL;
		}
		else
			return FALSE;
	}
	else
		return FALSE;

	return TRUE;
}
				


/***********************************************************************************************

>	virtual BOOL OpBackground::Undo()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successfull, FALSE otherwise
	Purpose:    Called when the op is undone.
				This broadcasts a 'Guidelines changed' layer message
	SeeAlso:	-
		
***********************************************************************************************/

/* BOOL OpBackground::Undo()
{
	if (UndoableOperation::Undo())
	{
		BroadcastGuidelineChanges(pBroadcastLayer);
		return TRUE;
	}

	return FALSE;
} */

/***********************************************************************************************

>	virtual BOOL OpBackground::Redo()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successfull, FALSE otherwise
	Purpose:    Called when the op is redone.
				This broadcasts a 'Guidelines changed' layer message
	SeeAlso:	-
		
***********************************************************************************************/

/* BOOL OpBackground::Redo()
{
	if (UndoableOperation::Redo())
	{
		BroadcastGuidelineChanges(pBroadcastLayer);
		return TRUE;
	}

	return FALSE;
} */

/***********************************************************************************************

>	static BOOL OpBackground::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if op successfully declared, FALSE otherwise
	Purpose:    Registers an op desc for this op
	SeeAlso:	-
		
***********************************************************************************************/

BOOL OpBackground::Init()
{
	BOOL ok = RegisterOpDescriptor(
									0, 
									0,
									CC_RUNTIME_CLASS(OpBackground), 
									OPTOKEN_BACKGROUND,
									OpBackground::GetState,
									0,  /* help ID */
									0,	/* bubble ID */
									0	/* bitmap ID */
									);
	return ok;
}

/***********************************************************************************************

>	static OpState OpBackground::GetState(String_256* Description, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/97
	Inputs:		Description = ptr to place description of why this op can't happen
				pOpDesc     = ptr to the Op Desc associated with this op
	Outputs:	-
	Returns:	An OpState object
	Purpose:    Func for determining the usability of this op
	SeeAlso:	-
		
***********************************************************************************************/

OpState OpBackground::GetState(String_256* Description, OpDescriptor*)
{
	OpState State;
	return State;
}

/***********************************************************************************************

>	static void OpBackground::GetOpName(String_256* OpName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/97
	Inputs:		OpName = ptr to string
	Outputs:	OpName contains a description of the op
	Returns:	-
	Purpose:    Returns a desc of the op.  Mainly used to fill in the menu item for the undo/redo ops. 
	SeeAlso:	-
		
***********************************************************************************************/

void OpBackground::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_SETBACKGROUND));
}


#endif // EXCLUDE_FROM_RALPH
