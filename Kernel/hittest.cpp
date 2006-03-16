// $Id: hittest.cpp 662 2006-03-14 21:31:49Z alex $
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
// The hit-testing functions of Node, NodeRenderableInk etc.

#include "camtypes.h"

#include "ccobject.h"
#include "node.h"
#include "ink.h"
#include "nodeattr.h"
#include "spread.h"
#include "layer.h"
#include "document.h"
#include "docview.h"
#include "rndrgn.h"
#include "osrndrgn.h"
#include "rendbits.h"
#include "objreg.h"
#include "ensure.h"
#include "fillattr.h"
#include "qualattr.h"
#include "attrmap.h"
#include "nodeshad.h"
#include "nodecont.h"
//#include "ndcchbmp.h"
//#include "grndclik.h"
#include "nodeclip.h"	// for NodeClipView
#include "ndclpcnt.h"	// for NodeClipViewController
#include "brshattr.h"
#include "strkattr.h"
#include "lineattr.h"


#pragma warning(disable: 4786)
#include <list>
#pragma warning(default: 4786)

DECLARE_SOURCE("$Revision: 662 $");

// This definition allows sensible debug trace output of memory leaks...
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

//	Karim 12/04/2000 -	uncomment this #define to enable rendering
//						of the hit-test bitmap on-screen.
//#define DEBUG_CLICK_DETECT_BITMAP

static AttrFlatColourFill DummyFill;	// Dummy fill to use in the place of fancy
										// fills for hit testing

/*********************************************************************************************
	Preference:	SmartClicks
	Section:	DebugFlags
	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Range:		TRUE or FALSE
	Purpose:	If TRUE then click detection is done with rendering, else if FALSE it is
				done with rectangle intersection. Will be made obsolete once rendering
				methods are reliable. Defaults to TRUE.
**********************************************************************************************/

BOOL NodeRenderableInk::bUseSmartClicks = TRUE;

CCAttrMap* NodeRenderableInk::pAttribMap;
INT32 NodeRenderableInk::nFoundAttributes;
INT32 NodeRenderableInk::nMaxAttributes;


/*********************************************************************************************
	Preference:	AverageHitColour
	Section:	?
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Range:		TRUE or FALSE
	Purpose:	If TRUE then click detection is done with rendering, else if FALSE it is
				done with rectangle intersection. Will be made obsolete once rendering
				methods are reliable. Defaults to TRUE.
**********************************************************************************************/

BOOL NodeRenderableInk::AverageHitColour = FALSE;


///////////////////////////////////////////////////////////////////////////// TEMP /////////
///////////////////////////////////////////////////////////////////////////// TEMP /////////
//
// These are only going here until I can get a lock on noderend.cpp and put them in there.
// 11 years later and it's still not in noderend.cpp (why break with tradition)
//

/********************************************************************************************
>	virtual BOOL NodeRenderableInk::OnMouseMove(const DocCoord& dcPos, Spread* pSpread,
												ClickModifiers mods)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		dcPos			the current mouse position, in spread coordinates
				pSpread			the spread containing the mouse position
				mods			which keys/buttons are down, eg. Adjust (shift/right button)
	Outputs:	-
	Returns:	TRUE if the node did something with this call, FALSE if it didn't/won't
	Purpose:	Called by the selector tool for the selected object(s) when the mouse is
				moved.
	Errors:		-
	SeeAlso:	NodeRenderableInk::OnClick; NodeRenderableInk::GetStatusInfo
********************************************************************************************/

BOOL NodeRenderableInk::OnMouseMove(const DocCoord& dcPos, Spread* pSpread, ClickModifiers mods)
{
#ifdef _DEBUG
//	if (IsUserName("JustinF"))
//	{
//		TRACE( _T("Call to base class NodeRenderableInk::OnMouseMove for a %s\n"), (LPCTSTR) Name());
//	}
#endif
	return FALSE;
}



/********************************************************************************************
>	virtual BOOL NodeRenderableInk::GetStatusInfo(String_256** ppStatusText,
												  Cursor** ppStatusCursor)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		-
	Outputs:	ppStatusText		pointer to a string pointer to set to an approrpiate
									status bar text message, or 0 if not relevant
				ppStatusCursor		pointer to a cursor pointer to set to an apppropriate
									Cursor object, or 0 if not relevant
	Returns:	TRUE if the routine used any of the output parameters and thus has some
				text/cursor to show, FALSE if it doesn't.
	Purpose:	Called by the selector tool if either a previous OnMouseMove or OnClick
				member function call returns TRUE, indicating that the node has some
				feedback to display when the mous is at the previously passed point.
	Errors:		-
	SeeAlso:	NodeRenderableInk::OnClick; NodeRenderableInk::OnMouseMove
********************************************************************************************/

BOOL NodeRenderableInk::GetStatusInfo(String_256** ppStatusText, Cursor** ppStatusCursor)
{
#ifdef _DEBUG
//	if (IsUserName("JustinF"))
//	{
//		TRACE( _T("Call to base class NodeRenderableInk::GetStatusInfo for a %s\n"), (LPCTSTR) Name());
//	}
#endif
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////// TEMP /////////
///////////////////////////////////////////////////////////////////////////// TEMP /////////



/********************************************************************************************
>	static NodeRenderableInk* NodeRenderableInk::FindSimpleAtPoint(Spread* pSpread,
																   DocCoord dcPoint,
																   Node* pHighNode = 0,
																   Node** ppInterruptNode = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		pSpread		---		pointer to the spread containing the given point
				dcPoint		---		the point the object to be searched for occupies
				pHighNode			Node below which search should be carried out.
				ppInterruptNode		0 if hit-detection shouldn't be interrupted
									Non-null otherwsie, then contains pointer to node
									at which hit-detection was interrupted on exit.
	Outputs:	-
	Returns:	A pointer to an ink-able object that lies at or is closest to the
				given point, or 0 if there is none.  This object will be "simple",
				ie. it will be a single "atomic" object such as a path, rectangle etc.
				and not a "compound" object like a group.
	Purpose:	Searches the ink-able objects within the given spread for one that lies
				at or near the given point.  The method used to render each candidate
				object into a small monochrome bitmap, in the reverse order to normal
				(so that the object normally rendered last is the first to be rendered
				into the bitmap).  The bitmap is examined to see if the object affected
				the centre or surrounding pixels.

				One of the biggest time users in this routine is the scanning of the tree
				to find the attributes applied to an object. Attempts are made to cut down
				this scanning. A hash table of pointers to attributes is maintained.
				Assume that the hash table is full of attributes used to render an object.
				Then, as we scan backwards along a sibling list we simply remove attributes
				from the hash table as we encounter them because they have moved out of scope.
				When we go down a child link we scan along the sibling child list adding all
				those attributes encountered to the hash table because they have just moved
				into scope. Thus, at any time the hash table contains pointers to all the
				attributes that apply to the node we are about to render WITH ONE EXCEPTION!
				The reverse render-order tree-scan means that objects are encountered before
				their child attributes. The FindAppliedAttribute routine overcomes this by
				forcibly adding all child attributes to the map when it scans for applied
				attributes.

	Errors:		-
	SeeAlso:	NodeRenderableInk::FindCompoundAtPoint;
				NodeRenderableInk::OldFindFirstAtPoint;
				Node::FindFirstHitTest;
				Node::FindNextHitTest
********************************************************************************************/

NodeRenderableInk* NodeRenderableInk::FindSimpleAtPoint(Spread* pSpread, 
														DocCoord dcPoint, 
														Node* pHighNode,
														Node** ppInterruptNode
														)
{
//	TRACE(_T("FindSimpleAtPoint(%d, %d)\n"), dcPoint.x, dcPoint.y);

	// Find the currently active DocView.
	DocView* pDocView = DocView::GetSelected();
	ENSURE(pDocView != 0, "Null current DocView in NodeRenderableInk::FindFirstAtPoint");

	// Initialise the count and limit on the number of attributes encountered.
	List* pTypeList = ObjectRegistry::GetRequiredAttribs(0);	 // <---
	ENSURE(pTypeList != 0 && !pTypeList->IsEmpty(),
			"Object Registry returned a dodgy list in NodeRenderableInk::FindSimpleAtPoint");
	nFoundAttributes = 0;
	nMaxAttributes = pTypeList->GetCount();

	// Clear interrupt node pointer
	if (ppInterruptNode)
		*ppInterruptNode = 0;

	// Initialise the hash-table used to look-up attributes encountered.
	// Make the hash table twice as big as the number of att types for efficient look-up.
	pAttribMap = new CCAttrMap(nMaxAttributes*2);
	ERROR2IF(pAttribMap==0,0,"FindSimpleAtPoint: couldn't create a CCAttrMap!");

	// Build a rectangle around the given point, based on the size of a blob.
	DocRect drClickRect;
	OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), dcPoint,
								BT_CLICKME, &drClickRect);

	// Get the extent of the current view and convert to spread coordinates.
	DocRect drViewRect = pDocView->GetDocViewRect(pSpread);
	pSpread->DocCoordToSpreadCoord(&drViewRect);
	
	// Create a special hit-detection render region to draw into, if possible.
	RenderRegion* pRegion = pDocView->RenderOnTop(&drClickRect, pSpread, CLICKBITMAP);
	if (pRegion==NULL)
	{
		delete pAttribMap;
		ERROR2(NULL,"FindSimpleAtPoint: couldn't create a RenderRegion!");
	}

	// Start with a (very) clean sheet.
	pRegion->SetClean(TRUE, TRUE);

	// These point to ink-nodes in the tree that have been rendered into our hit-test
	// render-region.
	NodeRenderableInk* pHitNode  = NULL;			// node that occupies dcPoint
	NodeRenderableInk* pNearNode = NULL;			// node that came closest to dcPoint
	NodeRenderableInk* pCompoundHitNode = NULL;		// compound node that was hit before trying children
	NodeRenderableInk* pCompoundNearNode = NULL;	// compound node that was near before trying children

	MonotonicTime Slice;							// Sample time now

	BOOL bSkipChildren = FALSE;

	// Perform hit-testing for each render-region provided by the DocView.
	while (pRegion != NULL)
	{
		// Render the first and subsequent ink-nodes into the render-region, checking if any
		// draw into it.
		for (Node* pNode = FindFirstHitTest(pSpread, drClickRect, pAttribMap, TRUE, pHighNode);
			 pNode != NULL;
			 pNode = FindNextHitTest(pNode, drClickRect, pAttribMap, TRUE, bSkipChildren))
		{
			bSkipChildren = FALSE;

			if (pNode)
			{
				if (pNode->IsAnObject())
				{
					// Does this renderable node intersect the click rectangle?  If not, then
					// ignore it.
					DocRect drBounds = ((NodeRenderableInk*) pNode)->GetBoundingRect(FALSE, TRUE);
					if (!drBounds.IsIntersectedWith(drClickRect)) continue;
					
					// This node can be rendered.  Find all the outstanding attributes in the
					// tree that affect the rendering of this node, and render them.
					//
					// Jason 15/1/97 - Save the current RR context so that we can restore the render 
					// stack to a sensible state after rendering this object. This is safe because we
					// always render all relevant attributes for every object we hit test.
					// We restore immediately after rendering (the 3 RestoreContext() calls, below)
					pRegion->SaveContext();
					((NodeRenderableInk*) pNode)->RenderAppliedAttributes(pRegion);
					
					// This ensures that the object is rendered according to the view Quality setting.
					// When Quality attributes appear in the tree this may not be a good thing!!!
					// It should really be called before RenderAppliedAttributes but at the moment that seems
					// to always set Quality to Normal for some reason!!!
					// Extract Quality from DocView object and set it up in the renderregion/monobitmap
					
					// Markn 26/9/95: Added the following 'if' statement to prevent the setting of the view
					// quality overriding the quality set by the AttrQuality attribute in the guides layer.
					if (pRegion->RRQuality.GetQuality() != Quality::QualityGuideLayer)
					{
						QualityAttribute *pQualAttr = new QualityAttribute(pDocView->RenderQuality);
						pRegion->SetQuality(pQualAttr, TRUE);
					}
					
					// Phil 18/03/2004
					// Must render clip region attribute AFTER Quality attribute has been set how 
					// the region will be rendered otherwise GDraw will fail in FillPath!
					//
					// Karim 13/4/2000 (lucky for some...)
					// ok, BODGE CITY here. we're going to do a RenderAppliedClipAttributes(), which
					// renders all the ClipRegionAttributes which would normally apply to this node.
					// see the method header for more info.
					((NodeRenderableInk*) pNode)->RenderAppliedClipAttributes(pRegion);

					do
					{
						// This 'do' loop will render the same node until the sub render state is 0
						// This allows heavy-duty nodes (e.g. blends) to get the hit test bmp to be checked
						// at any point during the rendering of the node.
						
						// Now the attributes are done, render the object itself.
						pRegion->SetClean(FALSE, FALSE);	// Reset just the clean flag so we can tell whether
															// compound nodes render anything
						// ------------------------------
						pNode->Render(pRegion);
						// ------------------------------
						
#ifdef DEBUG_CLICK_DETECT_BITMAP
						OSRenderBitmap::DebugMe(pRegion, 8);
#endif
						// Check if any of the pixels in the render-region were affected by the
						// last rendering.
						BitmapContents bcDrawCode = OSRenderBitmap::GetContents(pRegion, FALSE);

						if (pNode->IsCompoundClass() && !pRegion->IsClean())
						{
#ifdef DEBUG_CLICK_DETECT_BITMAP
						OSRenderBitmap::DebugMe(pRegion, 108);
#endif
							// Compound has rendered something...
							//
							// Clean out the bitmap since we know we are going to use it some more
							pRegion->SetClean(FALSE, TRUE);

							// Compound nodes may render themselves if they have effect
							// attributes applied (so that effect transparency is
							// properly taken into account during hit testing)
							// If the compound node rendered the centre pixel then we must
							// allow the children of the node to render themselves to get
							// a true indication of the clicked on node
							// If not, then we should not allow our children to render
							// themselves at all
							if (bcDrawCode != BITMAPCONTENTS_CENTRE && bcDrawCode != BITMAPCONTENTS_ANY)
							{
								// Skip out of this node's subtree...
								bSkipChildren = TRUE;
							}
							else
							// else just keep going (treat this as a "near" node)
							{
//								if (pNearNode == NULL) pNearNode = (NodeRenderableInk*) pNode;
								if (bcDrawCode == BITMAPCONTENTS_CENTRE)
								{
									if (pCompoundHitNode == NULL) pCompoundHitNode = (NodeRenderableInk*)pNode;
								}
								else
								{
									if (pCompoundNearNode == NULL) pCompoundNearNode = (NodeRenderableInk*)pNode;
								}
							}
						}
						else
						{
							if (bcDrawCode == BITMAPCONTENTS_CENTRE)
							{
								// The centre pixel was affected.  Remember this node and prepare to
								// return result to caller.
								pHitNode = (NodeRenderableInk*) pNode;
								
								// And exit both loops. We have to restore the previous RR state first, though
								pRegion->RestoreContext();
								goto AfterSearch;
							}
							else if (bcDrawCode == BITMAPCONTENTS_ANY)
							{
								// Some outer pixels were affected.  If we don't already have a
								// "closest" node then remember this one.  If we do already have a
								// "closest" node then the previous "closest" is "closer", because
								// it is on-top!
								if (pNearNode == NULL) pNearNode = (NodeRenderableInk*) pNode;

								// Don't break because although this stage (typically a stage of a Blend) has returned
								// "Near" the next one might be a direct hit!
								//break;	// break out of the do-while sub render state loop
							}
						}
					
					
						// If the caller has allowed us to inetrrupt the hit-testing process
						// then we will do so if the mouse has moved...
						// Only do these tests every twentieth of a second because they could take some time
						if (ppInterruptNode && Slice.Elapsed(50))
						{
							Slice.Sample();								// Reset the slice timer
							DocView* pDocView = DocView::GetCurrent();
							if (pDocView->IsMouseMoved())				// Has the mouse moved?
							{
								TRACEUSER( "Phil", _T("FindSimpleAtPoint Interrupt! %lx\n"), pNode);
								if (*ppInterruptNode == NULL)			// If so set the interrupt
									*ppInterruptNode = pNode;			// node pointer
								pNode = 0;							// Clear pointer to "hit" node
								
								// And exit both loops. We have to restore the previous RR state first, though
								pRegion->RestoreContext();
								goto AfterSearch;						// and stop the search
							}
						}
						
						// Render the same node until the sub render state is NULL
					} while (pRegion->GetSubRenderState() != NULL);

					// Restore the RenderRegion attr state to how it was before we rendered this node
					pRegion->RestoreContext();
				}
			}

		}
		// Advance onto the next render-region.
		pRegion = pDocView->GetNextOnTop(&drClickRect);
	}

AfterSearch:

	// Discard remaining render regions (although there shouldn't be any).
	while (pRegion != NULL)
		pRegion = pDocView->GetNextOnTop(&drClickRect);

	// Deallocate the hash table.
	delete pAttribMap;

	// If we found a "hit" then return that node, otherwise return the closest
	// "near miss" (which will be null if no node was close).
	if (pHitNode == NULL)
	{
		if (pNearNode != NULL)
		{
			if (!pNearNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadow)) &&
				!pNearNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadowController)))
					pHitNode = pNearNode;
		}
		else
			if (pCompoundHitNode)
				pHitNode = pCompoundHitNode;
			else if (pCompoundNearNode)
				pHitNode = pCompoundNearNode;

	}
	else
	{
		// We might have set pNearNode from a compound render
		// but then allowed processing to continue
		// So if pNearNode is set but is not the parent of the hit node then
		// we should use pNearNode instead
		//
		// NOTE! We could avoid extra work by making FinNextHitTest stop as soon
		// as it leaves the subtree whose root is pNearNode.
		if (pCompoundHitNode && !pCompoundHitNode->IsNodeInSubtree(pHitNode))
			pHitNode = pCompoundHitNode;
	}

	// Check if this node wishes to be selected
	// and if not, find it's parent instead...
	while (pHitNode != NULL && !pHitNode->CanSelectAsSimple())	// While we have a node but it doesn't
	{														// want to be selected...
		Node* ptemp = pHitNode->FindParent();				// Find parent of current node
		if (ptemp != NULL && ptemp->IsAnObject())			// If parent is ink
		{
			pHitNode = (NodeRenderableInk*) ptemp;			// Then reset current pointer
			if (!pHitNode->CanSelectAsCompoundParent())		// See if it is prepared to be selected
				pHitNode = NULL;							// If not then can't select anything!
		}
		else												// Else (parent is NOT ink)
			pHitNode = NULL;								// Then can't select anything!
	}

	// Give the node a chance to alter it's "hit" status
	if (pHitNode)
		pHitNode = pHitNode->FindNodeAtPointHelper(pSpread, dcPoint);

	return pHitNode;
}



/********************************************************************************************
>	static NodeRenderableInk* NodeRenderableInk::FindSimpleAtPointForColourPicker(
																	   Spread* pSpread, 
																	   DocCoord dcPoint, 
																	   Pixel32bpp& Pix,
																	   Node* pHighNode,
																	   Node** ppInterruptNode)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/99
	Inputs:		pSpread		---		pointer to the spread containing the given point
				dcPoint		---		the point the object to be searched for occupies
				Pix			---		reference to the structure that will receive pixel data
									(created internally by custom colour picker control)
				pHighNode			Node below which search should be carried out.
				ppInterruptNode		0 if hit-detection shouldn't be interrupted
									Non-null otherwsie, then contains pointer to node
									at which hit-detection was interrupted on exit.

				NOTE:  uses same inputs as NodeRenderableInk::FindSimpleAtPoint so that we
					   may call NodeRenderableInk::FindSimpleAtPoint internally.
	Outputs:	-
	Returns:	A pointer to an ink-able object that lies at or is closest to the
				given point, or 0 if there is none.  This object will be "simple",
				ie. it will be a single "atomic" object such as a path, rectangle etc.
				and not a "compound" object like a group.
				Also returns (by reference) details concerning the pixel that the colour
				picker control is over.
	Purpose:	Firstly, this function serves to find the 'ID' of the item that our colour
				picker is over - which is used to update our mouse cursor.  Secondly, it
				obtains the TRUE 32-BIT RGB index for the pixel that our mouse cursor is
				over - enabling us to do colour manager/dialog type things.  The first part
				of this routine simply calls FindSimpleAtPointFor - and the second part
				actually does colour specific work.

	Errors:		-
	SeeAlso:	NodeRenderableInk::FindSimpleAtPoint

********************************************************************************************/

NodeRenderableInk* NodeRenderableInk::FindSimpleAtPointForColourPicker(Spread* pSpread, 
																	   DocCoord dcPoint, 
																	   Pixel32bpp& Pix,
																	   Node* pHighNode,
																	   Node** ppInterruptNode)
{
	PORTNOTETRACE("other","NodeRenderableInk::FindSimpleAtPointForColourPicker - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Find the currently active DocView.
	DocView* pDocView = DocView::GetSelected();
	ENSURE(pDocView != 0, "Null current DocView in NodeRenderableInk::FindFirstAtPoint");

	// Build a rectangle around the given point, based on the size of a blob.
	DocRect drClickRect;
	OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), dcPoint,
								BT_CLICKME, &drClickRect);

	// Get the extent of the current view and convert to spread coordinates.
	DocRect drViewRect = pDocView->GetDocViewRect(pSpread);
	pSpread->DocCoordToSpreadCoord(&drViewRect);

	// find 'ID' of item that mouse cursor is over (or not)
	NodeRenderableInk* pHitNode = FindSimpleAtPoint (pSpread, dcPoint, pHighNode, ppInterruptNode);

	BOOL bOldForce = pDocView->SetForceDefaultColourContexts();

	// Create a colour render region to draw into, if possible.
	RenderRegion* pRegion = pDocView->RenderOnTop(&drClickRect, pSpread, COLOURBITMAP);
	if (pRegion == 0)
	{
		pDocView->SetForceDefaultColourContexts(bOldForce);
		delete pAttribMap;
		ERROR2(0,"FindSimpleAtPoint: couldn't create a RenderRegion!");
	}

	// Start with a (very) clean sheet.
	pRegion->SetClean(TRUE, TRUE);
	pRegion->StopRender();

	while (pRegion != 0)
	{
		// we want to render everything using the quality the document is using ....
		
		if (pRegion->RRQuality.GetQuality() != Quality::QualityGuideLayer)
		{
			QualityAttribute *pQualAttr = new QualityAttribute(pDocView->RenderQuality);
			pRegion->SetQuality(pQualAttr, TRUE);
		}

		// render everything over pSpread into our renderregion ....
		pDocView->ContinueRenderView(pRegion, pSpread, TRUE, FALSE);
		
		// uncommenting the following line allows us to see what is going on
//		OSRenderBitmap::DebugMe(pRegion,8);
		
		// need explicit cast to enable us to call ReadCentrePixel32bpp
		if (pRegion->IsKindOf(CC_RUNTIME_CLASS(GRenderClickColour)))
		{
			GRenderClickColour* actualRegion = (GRenderClickColour*) pRegion;
			if (NodeRenderableInk::AverageHitColour)
				Pix = actualRegion->GetAveragePixel32BPP();
			else
				Pix = actualRegion->GetCentrePixel32BPP();
		}

		pRegion->StartRender();
		pRegion = pDocView->GetNextOnTop(&drClickRect);
	}

	pDocView->SetForceDefaultColourContexts(bOldForce);
	return pHitNode;
#else
	return NULL;
#endif
}



/********************************************************************************************
>	static NodeRenderableInk* NodeRenderableInk::FindColourForNodeRenderableAtPoint (Spread* pSpread,
																		  DocCoord dcPoint,
																		  Pixel32bpp& Pix,
																		  Node* pThisNode,
																		  AttrFillGeometry* pThisFill)
{
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/2000
	Inputs:		pSpread		---		pointer to the spread containing the given point
				dcPoint		---		the point the object to be searched for occupies
				Pix			---		reference to the structure that will receive pixel data
									(created internally by custom colour picker control)
				pThisNode			The node that this function is concerned with
				pThisFill	---		The fill that this function is concerned with
									( parent (pThisFill) == pThisNode )

	Outputs:	-
	Returns:	0 (at present)
	Purpose:	The function obtains the TRUE 32-BIT RGB index for the pixel that our mouse
				cursor is over for the supplied node.

	Errors:		-
	SeeAlso:	NodeRenderableInk::FindSimpleAtPointForColourPicker

********************************************************************************************/

NodeRenderableInk* NodeRenderableInk::FindColourForNodeRenderableAtPoint (Spread* pSpread,
																		  DocCoord dcPoint,
																		  Pixel32bpp& Pix,
																		  Node* pThisNode,
																		  AttrFillGeometry* pThisFill)
{
	PORTNOTETRACE("other","NodeRenderableInk::FindColourForNodeRenderableAtPoint - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Find the currently active DocView.
	DocView* pDocView = DocView::GetSelected();
	ENSURE(pDocView != 0, "Null current DocView in NodeRenderableInk::FindFirstAtPoint");

	// Build a rectangle around the given point, based on the size of a blob.
	DocRect drClickRect;
	OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), dcPoint,
								BT_CLICKME, &drClickRect);

	// Get the extent of the current view and convert to spread coordinates.
	DocRect drViewRect = pDocView->GetDocViewRect(pSpread);
	pSpread->DocCoordToSpreadCoord(&drViewRect);
	
	// Create a colour render region to draw into, if possible.
	RenderRegion* pRegion = pDocView->RenderOnTop(&drClickRect, pSpread, COLOURBITMAP);

	if (pRegion==0)
	{
		delete pAttribMap;
		ERROR2(0,"FindSimpleAtPoint: couldn't create a RenderRegion!");
	}

	// Start with a (very) clean sheet.
	pRegion->SetClean(TRUE, TRUE);

	pRegion->StopRender ();

	while (pRegion != 0)
	{
		// we want to render everything using the quality the document is using ....
		
		if (pRegion->RRQuality.GetQuality() != Quality::QualityGuideLayer)
		{
			QualityAttribute* pQualAttr = new QualityAttribute(pDocView->RenderQuality);
			pRegion->SetQuality(pQualAttr, TRUE);
		}

		pRegion->SetFillGeometry ((ColourFillAttribute*) (pThisFill->GetAttributeValue ()), FALSE);
		pThisNode->Render (pRegion);
		
		// uncommenting the following line allows us to see what is going on
//		OSRenderBitmap::DebugMe(pRegion,8);
		
		// need explicit cast to enable us to call ReadCentrePixel32bpp
		if (pRegion->IsKindOf(CC_RUNTIME_CLASS(GRenderClickColour)))
		{
			GRenderClickColour* actualRegion = (GRenderClickColour*) pRegion;
			Pix = actualRegion->GetCentrePixel32BPP();
		}

		pRegion->StartRender ();
		pRegion = pDocView->GetNextOnTop(&drClickRect);
	}
#endif
	return 0;	// I suppose I should return something else here? //pThisNode);
}

/********************************************************************************************
>	static NodeRenderableInk* NodeRenderableInk::FindCompoundFromSimple(NodeRenderableInk* pSimpleNode,
																		Node* pSiblingNode = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		pSimpleNode			a simple node, eg. a renderable node such as a path,
									possibly returned by a previous call to FindSimpleAtPoint.
	Outputs:	-
	Returns:	A pointer to the "compound" node, eg. a group, that contains the given
				simple node, or (pSimpleNode) if there isn't one.
	Purpose:	This function will search up the parent links from the given "simple" node
				looking for a node whose parent is a layer.  This guarantees that the
				returned node will be a genuinely selectable object, for example the
				routine will return the group node if the given point is within a
				grouped object.
	Errors:		-
	SeeAlso:	NodeRenderableInk::FindSimpleAtPoint; NodeRenderableInk::FindCompoundAtPoint
********************************************************************************************/

NodeRenderableInk* NodeRenderableInk::FindCompoundFromSimple(NodeRenderableInk* pSimpleNode,
															 Node* pSiblingNode)
{
	// If necessary, search for a selectable parent node.
	if (pSimpleNode != 0)
	{
		for (;;)
		{
			// Find the node's parent, and if it is a layer then we have a node that can
			// be selected.
			Node* pParentNode = pSimpleNode->FindParent();
			Node* pSaveParentNode = 0;

			// DMc - new test to see which parent node wishes to be selected
			while (pParentNode != 0)
			{
				if (pParentNode->IsRenderable()		&& 
					!pParentNode->IsNodeDocument()	&&
					!pParentNode->IsLayer()			&& 
					!pParentNode->IsPaper()			&& 
					!pParentNode->IsSpread()		&&
					!pParentNode->IsChapter())
				{
					if (pParentNode->PromoteHitTestOnChildrenToMe() ||
						pParentNode->IsSelected())
							pSaveParentNode = pParentNode;
				}

				pParentNode = pParentNode->FindParent();
			}

			pParentNode = pSaveParentNode;
			if (pParentNode == 0 || pParentNode->IsLayer()) break;

			// If the caller has specified that the node should be in the same level
			// as the specified sibling then check to see if the node we've got to
			// exists in that sibling list or not...
			if (pSiblingNode != 0)
			{
				// Check all siblings. Start from first sibling in this list.
				Node* pSibling = pSimpleNode->FindParent();
				if (pSibling != 0) pSibling = pSibling->FindFirstChild();

				// Loop around comparing members of the sibling list with the specified sibling...
				while (pSibling != 0 && pSibling != pSiblingNode)
					pSibling = pSibling->FindNext();

				// If we found the specified sibling then stop the CompoundFromSimple search.
				if (pSibling == pSiblingNode) break;
			}

			// If the parent doesn't want to be selected due to one of it's children being
			// clicked on then stop NOW!
			if (pParentNode->IsAnObject())
			{
				if (((NodeRenderableInk*) pParentNode)->CanSelectAsCompoundParent())
					pSimpleNode = (NodeRenderableInk*) pParentNode;		// Parent allowed to be selected
				else
					break;												// Parent not allowed to be selected
																		// So return pSimpleNode now!
			}
			else
			{
				// Hit a non-ink node so we must have emerged above the selection surface
				// This shouldn't happen, so raise an error!
				ERROR2(0, "Couldn't find a selectable compound node in FindCompoundFromSimple");
			}
		}
	}

	// Return the node found, or 0 if we couldn't find any.
	return pSimpleNode;
}



/********************************************************************************************
>	static NodeRenderableInk* NodeRenderableInk::FindInnerCompound(Node* pLowNode, Node* pHighNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pLowNode		the node to start the search from (deep in the tree)
				pHighNode		the node to finish searching at
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

NodeRenderableInk* NodeRenderableInk::FindInnerCompound(Node* pLowNode, Node* pHighNode)
{
	if (pLowNode==pHighNode)
		return 0;
	
	if (!pHighNode->AllowSelectInside())
		return 0;

	Node* pLastCompoundNode = pLowNode;
	if (pLowNode != 0)
	{
		for (;;)
		{
			pLowNode = pLowNode->FindParent();
			if (pLowNode == 0 || pLowNode == pHighNode || IS_A(pLowNode, Layer)) break;
			if (pLowNode->IsCompound()) pLastCompoundNode = pLowNode;
//			if (IsUserName("JustinF")) TRACE( _T("\t\tLooking at a %s\n"), pLowNode->Name());
		}
	}

	return (NodeRenderableInk*) pLastCompoundNode;
}




/********************************************************************************************
>	static NodeRenderableInk* NodeRenderableInk::FindCompoundAtPoint(Spread* pSpread,
																	 DocCoord dcPoint,
																	 Node* pHighNode = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		pSpread		---		pointer to the spread containing the given point
				dcPoint		---		the point the object to be searched for occupies
				pHighNode		Node below which search should be carried out.
	Outputs:	-
	Returns:	A pointer to a node at the given point, which may be a "compound" object,
				eg. a group.
	Purpose:	This function is similar to NodeRenderableInk::FindSimpleAtPoint, except
				that 
	Errors:		-
	SeeAlso:	NodeRenderableInk::FindSimpleAtPoint
********************************************************************************************/

NodeRenderableInk* NodeRenderableInk::FindCompoundAtPoint(Spread* pSpread, DocCoord dcPoint, Node* pHighNode)
{
	return FindCompoundFromSimple(FindSimpleAtPoint(pSpread, dcPoint, pHighNode));
}




/********************************************************************************************
>	virtual BOOL NodeRenderableInk::CanSelectAsCompoundParent()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the node will allow itself to be selected when one of it's children
				has been "hit"
				FALSE otherwise
	Purpose:	Ask a node whether or not it's prepared to become selected when one of it's
				children is clicked on.
				This function is called in the FindSimple/Compound routines when they are
				following parent links up the tree from a node which has been "hit".
				This virtual function should be overridden in derived classes to alter its
				behaviour.
	Errors:		-
	SeeAlso:	NodeRenderableInk::FindSimpleAtPoint; NodeRenderableInk::FindCompoundAtPoint
********************************************************************************************/

BOOL NodeRenderableInk::CanSelectAsCompoundParent()
{
	// Virtual base-class function returns TRUE always.
	// Override this function in your class.
	return	TRUE;
}




/********************************************************************************************
>	virtual BOOL NodeRenderableInk::CanSelectAsSimple()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the node will allow itself to be selected when it has been "hit"
				FALSE otherwise
	Purpose:	Ask a node whether or not it's prepared to become selected when one of it's
				clicked on.
				This function is called in the FindSimple routines when they have just detected
				a "hit" on a node.
				This virtual function should be overridden in derived classes to alter its
				behaviour.
	Errors:		-
	SeeAlso:	NodeRenderableInk::FindSimpleAtPoint; NodeRenderableInk::FindCompoundAtPoint
********************************************************************************************/

BOOL NodeRenderableInk::CanSelectAsSimple()
{
	// Virtual base-class function returns TRUE always.
	// Override this function in your class.
	return	TRUE;
}











/********************************************************************************************
>	static Node* NodeRenderableInk::FindFirstHitTest(Spread* pStartSpread,
													 const DocRect& drClickRect,
													 CCAttrMap* pAttribMap,
													 BOOL bExcludeLayers,
													 Node* pHighNode = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		pStartSpread	--- the spread node to begin the hit-testing search
									from.  Only nodes contained within this spread will
									be returned.
				drClickRect		--- the rectangle centred on the mouse click position
				pAttribMap		--- Pointer to the map of attributes current at this point
				bExcludeLayers	--- TRUE if the routine should not return objects
									contained within locked or invisible layers.
				pHighNode		Node below which search should be carried out.
	Outputs:	-
	Returns:	A pointer to the next NodeRenderableInk to consider for hit-testing.  The
				returned node intersects the given click rectangle.
	Purpose:	Walks the node tree in the reverse order to rendering, which is the order
				that nodes should be considered for hit-testing.
	Errors:		ENSURE failure if the routine runs out of nodes to consider - this should
				never happen - if it does then there is something wrong with the node tree,
				for example it contains no spread nodes.
	SeeAlso:	Node::FindNextHitTest; NodeRenderableInk::FindSimpleAtPoint
********************************************************************************************/

Node* NodeRenderableInk::FindFirstHitTest(Spread* pStartSpread,
										  const DocRect& drClickRect,
										  CCAttrMap* pAttribMap,
										  BOOL bExcludeLayers,
										  Node* pHighNode)
{
	// Make sure that the node we start at is really a spread, as all NodeRenderableInks
	// are children of spread nodes.
	ENSURE(pStartSpread != 0,
			"Node::FindFirstHitTest: pStartSpread parameter is 0!");
	ENSURE(pStartSpread->IsSpread(),
			"Node::FindFirstHitTest: pStartSpread parameter is not a spread!");

	// Get the bounding box of the objects within the specified spread, converted to
	// spread coordinates.
	DocRect drSpreadBounds = pStartSpread->GetBoundingRect();
	pStartSpread->DocCoordToSpreadCoord(&drSpreadBounds);

	// If the bounding box doesn't intersect the click rectangle then the given spread
	// cannot hold any object that was clicked on.
	if (!drSpreadBounds.IsIntersectedWith(drClickRect)) return 0;

	// As the spread does contain the click rectangle, begin the traversal...
	// Call FindNextHitTest directly on the Spread so that the last item of the
	// Spread's child list can be considered correctly.
	Node* pFirstNode = FindNextHitTest((Node*)pStartSpread, drClickRect, pAttribMap, bExcludeLayers);
	if (pFirstNode == 0) return 0;

	// If there is an upper limit node then go through the tree until we get down to that
	// without returning the results for hit-testing.
	// (Or until there are no more nodes to consider!)
	if (pHighNode != 0)
	{
		while (pFirstNode != 0 && pFirstNode != pHighNode)
		{
 			pFirstNode = FindNextHitTest(pFirstNode, drClickRect, pAttribMap, bExcludeLayers);
		}

		// If we have stopped at the high node then return the next node below that in HitTest order
		// as the "first" node...  Note that we do not want to examine child nodes of the high node.
		if (pFirstNode != 0)
		{
			pFirstNode = FindNextHitTest(pFirstNode, drClickRect, pAttribMap, bExcludeLayers, TRUE);
		}
	}

 	return pFirstNode;
}



/********************************************************************************************
>	Node* NodeRenderableInk::FindNextHitTest(Node* pNode,
											 const DocRect& drClickRect,
											 CCAttrMap* pAttribMap,
											 BOOL bExcludeLayers,
											 BOOL bSkipChildren = FALSE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		pNode			--- Pointer to previous node in reverse rendering order
									(usually a result of calling FindFirstHitText or 
									 a previous call to FindNextHitTest)
				drClickRect		--- the rectangle centred on the mouse click position
				pAttribMap		--- Pointer to the map of attributes current at this point
				bExcludeLayers	--- TRUE if the routine should not return objects
									contained within locked or invisible layers.
				bSkipChildren	--- if TRUE then the routine will not examine the children
									of this Node.  By default this flag is FALSE.
	Outputs:	-
	Returns:	A pointer to the next node to consider for hit-testing.
	Purpose:	Walks the node tree in the reverse order to rendering, which is the order
				that nodes should be considered for hit-testing.
	Errors:		ENSURE failure if the routine runs out of nodes to consider - this should
				never happen - if it does then there is something wrong with the node tree,
				for example it contains no spread nodes.
	SeeAlso:	Node::FindFirstHitTest; NodeRenderableInk::FindSimpleAtPoint;
				Node::IsUnhittableLayer
********************************************************************************************/

Node* NodeRenderableInk::FindNextHitTest(Node* pNode,
										 const DocRect& drClickRect,
										 CCAttrMap* pAttribMap,
										 BOOL bExcludeLayers,
										 BOOL bSkipChildren)
{
	if(pNode == 0)
	{
		return 0;
	}

	// Storage for pointer to attribute types...
	CCRuntimeClass* pTypeInfo;
	void* pDummy;

	// Check if we should go down the child link of this node.
	// (If node is a spread then allow it's children to be scanned because it's bounds
	// are in a different coordinate system. This is OK because we know that the
	// only time this routine is called on a Spread is the first call made in FindFirstHitTest
	// which has already done a bounds test.)
	if (!bSkipChildren &&
		(HitTestChildren(pNode, drClickRect, bExcludeLayers) || 
		 pNode->IsSpread()))
	{
		// Find its right-most sibling and continue walking from there.
		pNode = pNode->FindFirstChild();
		while (pNode->FindNext() != 0)
		{
			// If this node is an attribute then it's more local than anything that might
			// be in the Attribute map (because we've just gone DOWN in the tree).
			// So, we'll bung it directly into the attribute map!
			if (pNode->IsAnAttribute() && !pNode->IsAClipViewAttr())
			{
				pTypeInfo = ((NodeAttribute*)pNode)->GetAttributeType();

				// Check if this type is already in the hash table.  If it isn't then increment
				// the number of attribute types found
				if (!pAttribMap->Lookup(pTypeInfo, pDummy)) nFoundAttributes++;

				// Insert this local attribute into the map
				pAttribMap->SetAt(pTypeInfo, pNode);
			}

			pNode = pNode->FindNext();
		}
	}
	else if (pNode->FindPrevious() != 0)
	{											  
		// Otherwise, just backtrack to the immediately-previous sibling, if possible.
		pNode = pNode->FindPrevious();
	}
	else
	{	
		// We have hit a node with no previous sibling or children.  Search up through
		// its parents until we find a parent with a previous sibling.  This previous
		// sibling is our next node.  However, if while searching up the parent links
		// we hit a spread then we know we have passed all NodeRenderableInks, and the
		// search can be terminated.
		do
		{
			if (pNode->IsSpread()) return 0;
			pNode = pNode->FindParent();
		}
		while (pNode != 0 && pNode->FindPrevious() == 0);

		// If we broke out of the above loop because we found a parent with a previous
		// sibling then advance to that sibling.
		if (pNode != 0 && pNode->FindPrevious() != 0) pNode = pNode->FindPrevious();
	}

	// If the found node is an attribute then remove it from the attribute map because it
	// has just gone out of scope!
	if (pNode->IsAnAttribute() && !pNode->IsAClipViewAttr())
	{
		// The node is an attribute.  Make sure that it no longer occurs in the
		// attribute hash table, so that the next time RenderAppliedAttributes is
		// called it will look for this kind.
		if (nFoundAttributes > 0 &&
			pAttribMap->RemoveKey(((NodeAttribute*) pNode)->GetAttributeType()))
				nFoundAttributes--;
	}

	
	// Return the node we've found.
	return pNode;
}



/********************************************************************************************
>	BOOL NodeRenderableInk::HitTestChildren(Node* pNode, const DocRect& drClickRect, BOOL bExcludeLayers)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Inputs:		pNode			---		Pointer to node to test child link of
				drClickRect		---		the rectangle enclosing a mouse-click, in spread
										coordinates.
				bExcludeLayers	---		whether locked or invisible layers should be
										excluded from hit-testing.
	Outputs:	-
	Returns:	TRUE if the caller should go down the child link of "this" node.
	Purpose:	Private helper function for Node::FindNextHitTest.  This function decides
				whether FindNextHitTest should go down a child link, which it should if
				(i) there is a child link; AND (ii) this node isn't a locked or invisible
				layer if such things should be excluded; (iii) if this node has a bounding
				rectangle it intersects the click rectangle.
	Errors:		-
	SeeAlso:	Node::FindNextHitTest
********************************************************************************************/

BOOL NodeRenderableInk::HitTestChildren(Node* pNode, const DocRect& drClickRect, BOOL bExcludeLayers)
{
	// If there's no child link to follow the decision is easy . . .
	if (pNode->FindFirstChild() == 0) return FALSE;

	// If the node is a locked or invisible layer and the caller wants such things
	// ignored then don't follow link down . . .
	if (bExcludeLayers && pNode->IsLayer())
	{
		Layer* pLayer = (Layer*) pNode;
		if (pLayer->IsLocked() || !pLayer->IsVisible()) return FALSE;
	}

	// If this node has a bounding rectangle then don't go down the link if the given
	// mouse-click rectangle doesn't intersect with the bounds . . .
	if (pNode->IsBounded())
	{
		DocRect drBounds = ((NodeRenderableBounded*) pNode)->GetBoundingRect();
		if (!drBounds.IsIntersectedWith(drClickRect)) return FALSE;
	}
	
	// If we pass all the above tests then by all means go down the child link.
	return TRUE;
}



/********************************************************************************************
>	void NodeRenderableInk::ClearAttribMap()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Clears out all entries in the attribute map that is being accumulated as 
				click-detection occurs. This has to be done when going down a child-link
				because then you are entering a deeper scope where the attributes in the map
				may not apply.
				It is called from Node::FindNextHitTest
	Errors:		-
	SeeAlso:	Node::FindNextHitTest
********************************************************************************************/

void NodeRenderableInk::ClearAttribMap()
{
	pAttribMap->RemoveAll();
	nFoundAttributes = 0;
}



/********************************************************************************************
>	void NodeRenderableInk::RenderAppliedAttributes(RenderRegion* pRegion) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/94
	Inputs:		pRegion		---		pointer to the render-reigion the function should
									render attributes into.
	Outputs:	-
	Returns:	-
	Purpose:	Searches for all the attributes which apply to "this" ink-node, rendering
				them into the given render-region.  Private helper function for
				NodeRenderableInk::FindSimpleAtPoint.
	Errors:		-
	SeeAlso:	NodeRenderableInk::FindSimpleAtPoint
********************************************************************************************/

void NodeRenderableInk::RenderAppliedAttributes(RenderRegion* pRegion) const
{
	// If we are missing some attributes then search until we find all of them.
	FindAppliedAttributes(pAttribMap, nMaxAttributes, &nFoundAttributes);

	// Make sure our dummy flat fill, that will be used in the
	// place of any 'fancy' fills, is set up.
	*(DummyFill.GetStartColour()) = DocColour(COLOUR_BLACK);

	// OK, we have found the full quota of attributes.  Now render them all.
	CCAttrMap::iterator pos = pAttribMap->GetStartPosition();
	while( pos != pAttribMap->GetEndPosition() )
	{
		CCRuntimeClass* pKey;
		void* pVal;
		pAttribMap->GetNextAssoc(pos, pKey, pVal);

		// Get the 'Family' of Attributes the fill belongs to
		CCRuntimeClass* AttrFamily = ((NodeAttribute*) pVal)->GetAttributeType();
		// Get the actual fill type
		CCRuntimeClass* AttrClass = ((NodeAttribute*) pVal)->GetRuntimeClass();

		// Check for a Transparency fill.
		// We can ignore them, as they do not effect hit testing.
		if ( AttrFamily != CC_RUNTIME_CLASS(AttrTranspFillGeometry) )
		{
			// Is this a fancy fill type ?
			if ( AttrFamily == CC_RUNTIME_CLASS(AttrFillGeometry) &&
				 	AttrClass != CC_RUNTIME_CLASS(AttrFlatColourFill) )
			{
				if (AttrClass == CC_RUNTIME_CLASS(AttrBitmapColourFill) &&
					((NodeAttribute*) pVal)->NeedsTransparency())
				{
					// It must be a masked bitmap, so we'll need to render it.
					// The render code will check the 'VeryMono' flag, and render
					// the bitmap non-masked pixels as black
					((NodeAttribute*) pVal)->Render(pRegion);
				}
				else
				{
					// Yes, so just render a black flat fill
				   	DummyFill.Render(pRegion);
				}
			}	
			else
			{
				// It's only a flat fill, so we might as well render it.
				// This will allow 'no colour' fills to work too.
				((NodeAttribute*) pVal)->Render(pRegion);
			}
		}
	}
}



/********************************************************************************************

>	void NodeRenderableInk::RenderAppliedClipAttributes(RenderRegion* pRender)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/2000
	Inputs:		pRender		the render region to render into.
	Outputs:	The given render region will have its clipping region reduced to the clip
				region which would apply to this node in the normal course of rendering.

	Purpose:	Helper method for RenderAppliedAttributes().

				Render all ClipRegionAttributes which would be applied to this node in the
				normal render-loop. This actually gets a little complicated, as these attrs
				are rendered by NodeClipView nodes, so we need to go up the tree looking for
				NodeClipViewController nodes, then getting their NodeClipView nodes to render
				the clip-attribute.Things are complicated more by the fact that a ClipView
				clips all nodes below a NCVC *except* the keyhole node of that NCVC, which
				is *not* clipped.

	Notes:		This is a bit of a BODGE, but is required because ClipRegionAttributes are
				not stored in the tree inside a NodeAttribute, and are therefore not picked
				up on by the normal RenderAppliedAttributes() process.

	See also:	RenderAppliedAttributes().

********************************************************************************************/
void NodeRenderableInk::RenderAppliedClipAttributes(RenderRegion* pRender) const
{
	// 1.	pRefNode starts on this node. We look for a NCVC which clips pRefNode, add it to
	//		our list, and then that NCVC becomes the new pRefNode to work from.
	//
	// 2.	To find a NCVC which clips us, we go straight up the tree. If we find a NCVC,
	//		then we check whether its child which we came from is also one of its keyhole
	//		nodes. If so, we ignore it and continue. If not, then it must clip us, and
	//		therefore the NCVC becomes the new pRefNode.
	//
	// 3.	We repeat the process until we hit a layer node, which marks the top of this
	//		branch of the tree.
	//
	Node* pRefNode = (Node*) this;
	std::list<NodeClipViewController*> lNCVC;
	BOOL bFinished = FALSE;
	while (!bFinished)
	{
		// go up the tree until we reach the top or we reach a NCVC
		// for which we are not a keyhole node.
		Node* pChild = pRefNode;
		Node* pParent = pRefNode->FindParent();
		BOOL bFoundApplicableNCVC = pParent->IsANodeClipViewController() &&
								!((NodeClipViewController*)pParent)->OwnsAsKeyholeNode(pChild);

		while (pParent != NULL && !pParent->IsLayer() && !bFoundApplicableNCVC)
		{
			pChild	= pParent;
			pParent	= pParent->FindParent();
			bFoundApplicableNCVC = pParent->IsANodeClipViewController() &&
								!((NodeClipViewController*)pParent)->OwnsAsKeyholeNode(pChild);
		}

		if (bFoundApplicableNCVC)
		{
			lNCVC.push_front((NodeClipViewController*) pParent);
			pRefNode = pParent;
		}
		else
		{
			bFinished = TRUE;
		}
	}

	// ok, now go through the list in reverse order, telling the NCV belonging to each NCVC
	// to render is ClipRegionAttribute.
	NodeClipView* pClipView			= 0;
	NodeClipViewController* pNCVC	= 0;
	while (!lNCVC.empty())
	{
		pNCVC = lNCVC.back();
		lNCVC.pop_back();
		pClipView = pNCVC->GetClipView();
		if (pClipView != 0)
		{
// DEBUG:
//			TRACEUSER( "Karim", _T("NRInk::RenderAppliedClipAttrs; NCVC: 0x%x\n"), (DWORD)pNCVC);
			pClipView->RenderClipAttr(pRender);
		}
	}
// DEBUG:
//	TRACEUSER( "Karim", _T("NRInk::RenderAppliedClipAttrs; this: 0x%x; %s\n"),
//												(DWORD)this, this->Name());
}



/********************************************************************************************
>	BOOL NodeRenderableInk::FindAppliedAttributes(CCAttrMap* pAttribMap,
												  INT32 nMax = 5000,
												  INT32* pnFound = 0,
												  BOOL ExcludeIndirectlyAppliedGLAs = FALSE,
												  BOOL bStrictEffectStatus = TRUE) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		pAttribMap	Map table to fill in with attribute details. This table stores 
				Key,Value tuples of the form (CCRuntimeClass*, NodeAttribute*). You can use
				the normal CMap Lookup function to extract the pointer to an attribute from
				the map, eg.
					Lookup(CC_RUNTIME_CLASS(NodeAttrFill),pAttrib);
				nMax		The maximum number of attribute types that can be found
				pnFound		Pointer to the number already in the Map. This param can be used
							in conjunction with nMax to make the scan stop as soon as the 
							required number of attributes have been found. (Defaults cause 
							whole tree scan.)
				ExcludeIndirectlyAppliedGLAs -	ensures that this map won't include a pointer
												to a geometry linked attribute which is not
												directly linked to this nodes geometry (ie
												exclude parent GLA's)
				bStrictEffectStatus - TRUE when non-effect attrs are ignored in compound that
										allow effect attrs
									- FALSE when non-effect attrs are allowed throughout
	Outputs:	-
	Returns:	TRUE if attributes were successfully found
				FALSE otherwise (if no attributes were found)
	Purpose:	Searches for all the attributes which apply to "this" ink-node. This routine
				does NOT render them! Use RenderAppliedAttributes if you want that functionality.
	Errors:		-
	SeeAlso:	NodeRenderableInk::RenderAppliedAttributes
				NodeAttribute::FindFirstAppliedAtt
				NodeAttribute::FindPrevAppliedAtt
********************************************************************************************/

BOOL NodeRenderableInk::FindAppliedAttributes(CCAttrMap* pAttribMap,
											  INT32 nMax,
											  INT32* pnFound, 
											  BOOL ExcludeIndirectlyAppliedGLAs,
											  BOOL bStrictEffectStatus) const
{
	// Precondition checks
	ERROR2IF(pAttribMap == 0, FALSE, "FindAppliedAttributes passed null map");

	// Find the closest attribute applied to "this" node...
	NodeAttribute* pAttrib = NodeAttribute::FindFirstAppliedAttr((Node*)this);
	if (pAttrib == 0) return FALSE;

	// Get variables ready for the loop, OUTSIDE the loop!!!
	CCRuntimeClass* pTypeInfo;
	void* pDummy;
	INT32 nFound;
	if (pnFound)
		nFound = *pnFound;
	else
		nFound = 0;

//	BOOL FoundClipAttr = FALSE;

	// If we are finding a completely new set of attrs then we should not overwrite any entries
	// into the map that have been made within this routine
	// (So that normal attrs don't overwrite effect attrs)
	BOOL bOverwriteExisting = (nFound!=0);

	// Repeat the following loop for all attributes we encounter in the tree.
	while (pAttrib)
	{
		// Get the run-time type of the attribute, which serves as a hash "key".
		pTypeInfo = pAttrib->GetAttributeType();

		// Check if this type is already in the hash table.  If it isn't then insert
		// its address with its run-time type as the key, and check if we need to look
		// for any more.
		BOOL bEffectStatusOK = FALSE;
		if (bStrictEffectStatus)
			bEffectStatusOK = (pAttrib->IsEffectAttribute() == this->IsValidEffectAttr(pAttrib));
		else
			bEffectStatusOK = !pAttrib->IsEffectAttribute();

		if (!pAttrib->IsAClipViewAttr() && (pAttrib->FindParent() != (Node*)this || bEffectStatusOK))
		{
			if (!pAttribMap->Lookup(pTypeInfo, pDummy))
			{
				// This is a new attribute for the map
				BOOL AddAttr = TRUE;
				// NB keep default attrs in map so we don't have incomplete maps
				if (ExcludeIndirectlyAppliedGLAs && !pAttrib->IsADefaultAttr() && pAttrib->IsLinkedToNodeGeometry())
				{
					if(!pAttrib->IsLinkedToThisNode((Node*)this))
					{
						AddAttr = FALSE;
					}
				}
				
				if(AddAttr)
				{
					pAttribMap->SetAt(pTypeInfo, pAttrib);
					nFound++;
					ERROR3IF(nFound>nMax,"Found more attribute types than are registered!");
				}
			}
			else if (bOverwriteExisting)
			{
				// An attribute of that type is already in the map
				// But if this attribute is a child of the scanned node
				// we should force it into the map because it's
				// still in scope!
				if (pAttrib->FindParent() == (Node*)this)
				{
					pAttribMap->SetAt(pTypeInfo,pAttrib);
					// (Attrib type was already in map so no need to increment counter.)
				}
			}
		}

		// After dealing with that attribute can we stop the scan?
		// We want to stop as soon as we can, namely when we have all the attribute
		// types in the map and when we're no longer scanning children of "this".
		if ( nFound >= nMax && (pAttrib->FindParent()!=(Node*)this) )
			break;

		// Go on to find the next attribute.
		pAttrib = NodeAttribute::FindPrevAppliedAttr(pAttrib);
	}

#ifdef _DEBUG
PORTNOTE("other","Removed some IsUserName protected debug")
#if 0
	if (IsUserName("Phil") && nMax<5000 && nFound<nMax)
	{
		TRACE( _T("Not found all reqd atts: (found %d attributes, should be %d)\n"),
				nFoundAttributes, nMaxAttributes);
		
		for (POSITION pos = pAttribMap->GetStartPosition(); pos != 0; )
		{
			void* pKey;
			void* pValue;
			pAttribMap->GetNextAssoc(pos, pKey, pValue);
			TRACE( _T("\tFound a %s at 0x%lX\n"),
					((CCRuntimeClass*) pKey)->m_lpszClassName,
					(UINT32) pValue);
		}

		List* pTypeList = ObjectRegistry::GetRequiredAttribs(GetRuntimeClass());
		for (ListItem* pItem = pTypeList->GetHead();
			 pItem != 0;
			 pItem = pTypeList->GetNext(pItem))
		{
			CCRuntimeClass* pClass = ((NodeAttributeClassItem*) pItem)->AttributeClass;
			void* pDummy;
			if (!pAttribMap->Lookup(pClass, pDummy))
			{
				TRACE( _T("\t\tCouldn't find a %s\n"), pClass->m_lpszClassName);
			}
		}
	}
#endif
#endif
	ERROR2IF(nMax<5000 && nFound<nMax,FALSE,"Failed to find all required attributes.");

	if (pnFound)
		*pnFound = nFound;

	return TRUE;
}



/***********************************************************************************************
>  	AttrBrushType* NodeRenderableInk::GetAppliedBrushAttribute()
    
	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    15-5-2000
	Inputs:           
    Outputs:    -
    Returns:    the attrbrushtype that applies to this inknode	 
    Purpose:    as above
	
***********************************************************************************************/

AttrBrushType* NodeRenderableInk::GetAppliedBrushAttribute()
{
	AttrBrushType* pAttr = 0;
	
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), (NodeAttribute**)&pAttr);
	
	if (pAttr == NULL)
		return NULL;

	BrushHandle Handle = pAttr->GetBrushHandle();
	
	if (Handle == BrushHandle_NoBrush)
		return NULL;

	// double check to make sure it isn't referring to a deleted brush
	BrushDefinition* pDef = BrushComponent::FindBrushDefinition(Handle);
	if (pDef == NULL)
		return NULL;

	return pAttr;
}


/***********************************************************************************************
>  	AttrStrokeType* NodeRenderableInk::GetActiveStroke()
    
	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    15-5-2000
	Inputs:           
    Outputs:    -
    Returns:    pointer to the stroke attribute applied to us, so long as it is active
    Purpose:    We may have a stroke attribute applied to us that does not do anything under
				certain circumstances (zero line width, no accompanying variable width attribute).
	
***********************************************************************************************/

AttrStrokeType* NodeRenderableInk::GetActiveStroke()
{
	// first up, if we have no stroke colour then any strokes we have aren't really active
	AttrStrokeColour* pStrokeColour = NULL;
	
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), (NodeAttribute**)&pStrokeColour);

	if (pStrokeColour != NULL)
	{
		// get the stroke colour
		StrokeColourAttribute* pVal = (StrokeColourAttribute*)pStrokeColour->GetAttributeValue();
		if (pVal != NULL)
			if (pVal->Colour.IsTransparent())
				return NULL;
	}
	else
		return NULL;
	
	// now find out if we have both a stroke and variable width attribute
	AttrStrokeType* pStroke = NULL;
	AttrVariableWidth* pVarWidth = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeType), (NodeAttribute**)&pStroke);
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth), (NodeAttribute**)&pVarWidth);
					
	if (pVarWidth != NULL && ((VariableWidthAttrValue*)pVarWidth->GetAttributeValue())->GetWidthFunction() != NULL)
	{
		if (pStroke != NULL && pStroke->HasPathProcessor())
			return pStroke;
	}
	return NULL;

}

/***********************************************************************************************
>  	virtual NodePath* NodeRenderableInk::GetVariableWidthStrokePath()
    
	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    15-5-2000
	Inputs:     -  
    Outputs:    -
    Returns:    a nodepath containing the path that is generated by any variable width strokes that
				are applied to us, or NULL if there aren't any
    Purpose:    If we have a variable width stroke applied to us then this will get the path generated
				by that stroke.  This base class version returns NULL, overridden versions must supply
				their own outline path.

	See also:   NodePath::GetVariableWidthStrokePath, NodeRegularShape::GetVariableWidthStrokePath
				
	
***********************************************************************************************/

NodePath* NodeRenderableInk::GetVariableWidthStrokePath()
{
	return NULL;
}



NodePath* NodeRenderableInk::GetSmoothVariableWidthStrokePath()
{
	return NULL;
}

