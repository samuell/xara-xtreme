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


#include "camtypes.h"
#include "extender.h"

#include "ncntrcnt.h"	// for NodeContourController.
#include "nodecont.h"	// for NodeShadowController.
#include "nbevcont.h"	// for NodeBevelController.
#include "layer.h"		// for Layer.
#include "becomea.h"	// for BecomeA.
#include "range.h"		// for SelRange.
#include "undoop.h"		// for UndoableOperation.
#include "nodershp.h"	// for NodeRegularShape.
#include "objchge.h"	// for ObjChange stuff.
#include <list>			// for std::list.
#include "nodeblnd.h"	// for NodeBlend
#include "nodebldr.h"	// for NodeBlender
#include "nodepath.h"

#if 0
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#endif

DECLARE_SOURCE("$Revision$");

// Place any IMPLEMENT type statements here
CC_IMPLEMENT_MEMDUMP(Extender, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(ExtendParams, CC_CLASS_MEMDUMP)

// We want better memory tracking
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


// Functions follow



/********************************************************************************************

>	void Extender::Extend(	NodeRenderableInk* pNode,
							DocRect* pStartRect,
							DocRect* pEndRect,
							BYTE fExtendFlags,
							DocRect* pOldStartRect = NULL)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/1999
	Inputs:		
	Outputs:	
	Purpose:	** DEPRECATED - DO NOT CALL **
	Errors:		ERROR2 always.
	See also:	

********************************************************************************************/
void Extender::Extend(	NodeRenderableInk* pNode,
						DocRect* pStartRect,
						DocRect* pEndRect,
						BYTE fExtendFlags,
						DocRect* pOldStartRect)
{
	ERROR2RAW("This implementation of Extender::Extend is no longer used or valid");
}



/********************************************************************************************

>	BOOL Extender::CheckValidExtend(NodeRenderableInk *pNode,
									DocRect *pStartRect,
									DocRect *pEndRect,
									BYTE fExtendFlags,
									DocRect* pOldStartRect = NULL)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/1999
	Inputs:		
	Outputs:	
	Purpose:	** DEPRECATED - DO NOT CALL **
	Returns:	FALSE
	Errors:		ERROR2 always.
	See also:	

********************************************************************************************/
BOOL Extender::CheckValidExtend(NodeRenderableInk* pNode,
								DocRect* pStartRect,
								DocRect* pEndRect,
								BYTE fExtendFlags,
								DocRect* pOldStartRect)
{
	ERROR2(	FALSE,
			"This implementation of Extender::CheckValidExtend is no longer used or valid" );
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Extender helper function for make-shapes capability.
//


/********************************************************************************************

>	BOOL Extender::ConvertQuickShapesInSelRangeToPaths(	UndoableOperation* pUndoOp,
														SelRange* pSel )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 March 2000
	Inputs:		pUndoOp		we need an UndoOp to make this undoable.
				pSel		the SelRange we'll iterate over.
	Outputs:	
	Returns:	TRUE if any quickshapes were successfully turned,
				FALSE if not.

	Purpose:	Iterates over the given SelRange looking for any NodeRegularShapes,
				ie QuickShapes. If we find any then they get zapped, and all that is left
				is a smoking pair of shoes! ... erm, sorry - a NodePath.

	Notes:		This is all done undoably. There is however a problem in that we do not
				necessarily want to completely unwind the UndoOp we were given if anything
				goes wrong. But if we don't do that then what _can_ we do???
				Currently, if things go wrong while converting to shapes, we just try to
				convert as many shapes to paths as possible and return FALSE at the end.

				We don't mind if pSel is empty or has no QuickShapes.

				Our search of the SelRange is deep - we look at children of nodes in the
				range too.

	Errors:		ERROR2 with FALSE if we get invalid parameters.
	See also:	

********************************************************************************************/
BOOL Extender::ConvertQuickShapesInSelRangeToPaths(	UndoableOperation* pUndoOp,
													Range* pRange )
{
	// validate params.
	ERROR2IF(pUndoOp == NULL || pRange == NULL, FALSE, "Invalid parameter(s)");

//	TODO: May need to set the range's RangeControl to avoid promoting to parents.

	// iterate over the range and build a list of QuickShapes.
	// we do a depth-first search from every node in the range,
	// looking everywhere for those those pesky QuickShapes :)
	BOOL bQuickShapesConverted = FALSE;
	std::list<NodeRegularShape*> lpShapes;
	Node* pCurrentRangeNode = pRange->FindFirst();
	while (pCurrentRangeNode != NULL)
	{
		Node* pNextNode = NULL;
		Node* pCurrentNode = pCurrentRangeNode->FindFirstDepthFirst();
		while (pCurrentNode != NULL)
		{
			pNextNode = pCurrentNode->FindNextDepthFirst(pCurrentRangeNode);
			if (pCurrentNode->IsARegularShape())
			{
				if (!(pCurrentNode->FindParent (CC_RUNTIME_CLASS (NodeBlend))))
				{
					// note thate we're *saying* we're going to replace with only one node.
					// this isn't necessarily true - DoBecomeA on a brushed path results in
					// many ungrouped nodes. it's just the best we can do, considering blends.
					ObjChangeFlags ocf;
					ocf.ReplaceNode	= TRUE;
					ObjChangeParam ObjChange(OBJCHANGE_STARTING, ocf, pCurrentNode, pUndoOp);
					if (pCurrentNode->AllowOp(&ObjChange, FALSE, FALSE))
						lpShapes.push_front((NodeRegularShape*)pCurrentNode);
				}
				/*else
				{
					// note thate we're *saying* we're going to replace with only one node.
					// this isn't necessarily true - DoBecomeA on a brushed path results in
					// many ungrouped nodes. it's just the best we can do, considering blends.
					ObjChangeFlags ocf;
					ocf.ReplaceNode	= TRUE;
					ObjChangeParam ObjChange(OBJCHANGE_STARTING, ocf, pCurrentNode, pUndoOp);
					if (pCurrentNode->AllowOp(&ObjChange, TRUE, FALSE))
						lpShapes.push_front((NodeRegularShape*)pCurrentNode);
				}*/
			}
			pCurrentNode = pNextNode;
		}
		pCurrentRangeNode = pRange->FindNext(pCurrentRangeNode, TRUE);
	}

	// okay, if our list isn't empty then we have work to do.
	if (!lpShapes.empty())
	{
		// invalidate the region of the selection.
		if (pUndoOp->DoInvalidateNodesRegions(*pRange, TRUE))
		{
			// change all the QuickShapes into NodePaths.
			NodeRegularShape* pCurrentShape = NULL;
			while (!lpShapes.empty())
			{
				pCurrentShape = lpShapes.front();
				lpShapes.pop_front();
				BecomeA BecomeAPath( BECOMEA_REPLACE,
									 CC_RUNTIME_CLASS(NodePath),
									 pUndoOp,
									 pCurrentShape->IsSelected() );

				if (pCurrentShape->CanBecomeA(&BecomeAPath))
				{
					if (!(pCurrentShape->FindParent (CC_RUNTIME_CLASS (NodeBlend))))
					{
						if (pCurrentShape->DoBecomeA(&BecomeAPath))
						{
							pCurrentShape->DeSelect(FALSE);
							bQuickShapesConverted = TRUE;
						}
					}
					/*else
					{
						NodeBlend* ptrBlend = (NodeBlend*) pCurrentShape->FindParent (CC_RUNTIME_CLASS (NodeBlend));
						BecomeA BecomeAPath( BECOMEA_REPLACE,
											 CC_RUNTIME_CLASS(NodePath),
											 pUndoOp,
											 pCurrentShape->IsSelected() );
						if (pCurrentShape->DoBecomeA(&BecomeAPath))
						{
							pCurrentShape->DeSelect(FALSE);
							bQuickShapesConverted = TRUE;
						}												
						
						// nastiness - to reinitialise the blend, I need a ptr to the converted path
						// BUT there is no easy way of doing this!  This is a hack - which seems to
						// hold up well ....  NOTE:  assumes that the last action executed was a
						// HideNodeAction (i.e.  hide pCurrentShape)

						ActionList* actList = pUndoOp->GetUndoActionList ();	// get undo history
						ListItem* pItem = actList->GetTail ();					// get last op
						ERROR3IF (!IS_A (pItem, HideNodeAction), "Assumed HideNodeAction is not a HideNodeAction!");
						HideNodeAction* hnAct = (HideNodeAction*) pItem;
						NodeRenderableInk* newNode = (NodeRenderableInk*) hnAct->GetNode ();
						ERROR3IF (!IS_A (ptrBlend, NodeBlend), "NodeBlend is not a NodeBlend!");

						BOOL done = FALSE;
						NodeBlender* ptrNode = ptrBlend->FindFirstBlender ();

						while (!done)
						{
							ptrNode->Deinit ();
							
							if (ptrNode->GetNodeStart () == pCurrentShape)
							{
								ptrNode->Reinit(newNode, NULL, FALSE);
							}
							if (ptrNode->GetNodeEnd () == pCurrentShape)
							{
								ptrNode->Reinit(NULL, newNode, FALSE);
							}

							ptrNode = ptrBlend->FindNextBlender (ptrNode);

							if (!ptrNode)
							{
								done = TRUE;
							}
						}
					}*/
				}
			}
		}
		if (bQuickShapesConverted)
			pRange->Update();
	}

	return bQuickShapesConverted;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//	New Extender functions, for virtual-fn extend procedure.
//


/********************************************************************************************

>	DocRect Extender::CheckValidExtend( NodeRenderableInk* pNode, BYTE fExtendFlags,
						const DocRect& drTriggerRelation, const DocRect& drTargetRelation,
						const DocRect& drNewTrigger, const DocRect& drLastTarget )

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		pNode				pointer to a NodeRenderableInk which will be extended.
				fExtendFlags		bitwise flags, describing any combination of x- or y-
									stretching or extending, except that you may not extend
									and stretch along an axis at the same time. Values:

									X_EXTEND	the node should be extended along the x-axis
									Y_EXTEND	the node should be extended along the y-axis
									X_STRETCH	the node should be stretched along the x-axis
									Y_STRETCH	the node should be stretched along the y-axis

				drTriggerRelation	relationship rectangle - the bounding box of the trigger
									sets when the relationship is defined.
				drTargetRelation	relationship rectangle - the bounding box of the target
									sets when the relationship is defined.
				drNewTrigger		bounding box of sets of triggers which started the call
									to this function.
				drLastTrigger		the bounding box of the trigger set(s) before it changed.
				drLastTarget		the bounding box of the target set before it is extended.

	Purpose:	This function tests whether or not a call to the extend function will extend
				an object in a valid way. For example, a path cannot be shrunken so that its
				control points cross its centre, as this operation is irreversible - if the
				path was extended back again, these points would extend the wrong direction.
				Similarly, an invalid extend occurs when a trigger set is shrunk so that its
				centre happens to move over the centre of a target node which is not moving
				in this particular extension; undoing this operation would end up dragging
				the target node back with the trigger.

				This function should always be called on all objects sharing the same name,
				*before* Extender::Extend is called to actually perform the extension on
				those objects, to make sure that no irreversible operation is performed.

	Returns:	A DocRect, whose values will all be set to INT32_MAX if the test was passed.
				For each side on which the test failed, the corresponding value will be set
				to how far in it is safe to extend the node, eg if the node could only be
				shrunk inwards by 10 millipoints on the right-hand side, then the hix value
				of the DocRect will contain 10, while all other values will be INT32_MAX.

	Errors:		ERROR2 with FALSE	if fExtendFlags holds contradictory flags
									or any parameters are NULL.

	See also:	Extender::CheckValidExtend() and for more information, see:
				\\Earth\Develop\CAMELOT\DOCS\Specs\Camelot V3\Extending Objects - Mechanism.doc

	NOTE:		A call to Extender::CheckValidExtend must *always* be made before performing
				a corresponding Extender::Extend process.

********************************************************************************************/
DocRect Extender::CheckValidExtend( NodeRenderableInk* pNode, BYTE fExtendFlags,
						const DocRect& drTriggerRelation, const DocRect& drTargetRelation,
						const DocRect& drNewTrigger, const DocRect& drLastTrigger,
						const DocRect& drLastTarget, BOOL * pOK, BOOL ExtendAroundTarget)
{
	// validate parameters.
	ERROR2IF(	pNode == NULL, DocRect(0, 0, 0, 0),
				"NULL parameters passed to Extender::CheckValidExtend" );

	ERROR2IF(	((fExtendFlags & X_EXTEND) && (fExtendFlags & X_STRETCH)) ||
				((fExtendFlags & Y_EXTEND) && (fExtendFlags & Y_STRETCH)),
				DocRect(0, 0, 0, 0),
				"Extender::CheckValidExtend called with contradictory flags in fExtendFlags" );

	if (pOK)
		*pOK = TRUE;
	// quit immediately if any of the bounding rectangles is invalid,
	// or is of zero width or height.
	if(			!drTriggerRelation.IsValid() ||
				!drTargetRelation.IsValid() ||
				!drNewTrigger.IsValid() ||
				!drLastTrigger.IsValid() ||
				!drLastTarget.IsValid() ||
				drTriggerRelation.Height() == 0 ||
				drTriggerRelation.Width() == 0 ||
				drTargetRelation.Height() == 0 ||
				drTargetRelation.Width() == 0 ||
				drNewTrigger.Height() == 0 ||
				drNewTrigger.Width() == 0 ||
				drLastTrigger.Height() == 0 ||
				drLastTrigger.Width() == 0 ||
				drLastTarget.Height() == 0 ||
				drLastTarget.Width() == 0)
	{
		if (pOK)
			*pOK = FALSE;

		return DocRect(0, 0, 0, 0);
	}



	// determine extension working values.
	ExtendParams eps;
	CalculateExtendParams(	&eps, fExtendFlags, drTriggerRelation, drTargetRelation,
												drNewTrigger, drLastTrigger, drLastTarget, NULL, ExtendAroundTarget );

	// check whether pNode, or any child of pNode, is extendible.
	// also, check that extending all of these Nodes is a valid operation.
	// if this is the case, recursively extend pNode and its children.
	DocRect drMinExtend = pNode->ValidateExtend(eps);
	BOOL bValidExtend = (	drMinExtend.lo.x == INT32_MAX &&
							drMinExtend.lo.y == INT32_MAX &&
							drMinExtend.hi.x == INT32_MAX &&
							drMinExtend.hi.y == INT32_MAX );

// DEBUG:
#ifdef _DEBUG
	TRACEUSER( "Karim",	_T("%5s ValidateExtend() on %s\n"),	bValidExtend ? _T("TRUE") : _T("FALSE"),
														pNode->Name());
#endif

	BOOL bExtendible = TRUE;

	if (!bExtendible || !bValidExtend)
	{
//#ifdef _DEBUG
//		TRACEUSER( "Karim", _T("Invalid Extend on %s; Ext: %d, Valid: %d\n"),
//							pNode->Name(), bExtendible, bValidExtend);
//#endif
		return drMinExtend;
	}

	// return the outcome of the test, in the form of drMinExtend.
	return drMinExtend;
}



/********************************************************************************************

>	void Extender::Extend(	NodeRenderableInk* pNode, BYTE fExtendFlags,
							const DocRect& drTriggerRelation, const DocRect& drTargetRelation,
							const DocRect& drNewTrigger, const DocRect& drLastTarget,
							const DocRect* pdrDifference = NULL)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/12/1999
	Inputs:		pNode				pointer to a NodeRenderableInk which will be extended.
				fExtendFlags		bitwise flags, describing any combination of x- or y-
									stretching or extending, except that you may not extend
									and stretch along an axis at the same time. Values:

									X_EXTEND	the node should be extended along the x-axis
									Y_EXTEND	the node should be extended along the y-axis
									X_STRETCH	the node should be stretched along the x-axis
									Y_STRETCH	the node should be stretched along the y-axis

				drTriggerRelation	relationship rectangle - the bounding box of the trigger
									sets when the relationship is defined.
				drTargetRelation	relationship rectangle - the bounding box of the target
									sets when the relationship is defined.
				drNewTrigger		bounding box of sets of triggers which started the call
									to this function.
				drLastTrigger		the bounding box of the trigger set(s) before it changed.
				drLastTarget		the bounding box of the target set before it is extended.
				pdrDifference		a rectangle containing four difference parameters which
									may be used to alter the extend. ok to omit or be NULL.

	Outputs:	pNode may be extended or stretched horizontally
				or vertically in any combination.

	Purpose:	Perform an extend operation on the given Node and its children, using the
				given rectangles and flags to determine how the Node should transform.

	Errors:		ERROR2 if fExtendFlags holds contradictory flags or any parameters are NULL.

	See also:	Extender::CheckValidExtend() and for more information, see:
				\\Earth\Develop\CAMELOT\DOCS\Specs\Camelot V3\Extending Objects - Mechanism.doc

	NOTE:		A call to Extender::CheckValidExtend must always be made before performing
				a corresponding Extender::Extend process.

********************************************************************************************/
void Extender::Extend(	NodeRenderableInk* pNode, BYTE fExtendFlags,
						const DocRect& drTriggerRelation, const DocRect& drTargetRelation,
						const DocRect& drNewTrigger, const DocRect& drLastTrigger,
						const DocRect& drLastTarget, const DocRect* pdrDifference, const BOOL ExtendAroundTarget,
						UndoableOperation * pOp)
{
	// validate parameters.
	if (pNode == NULL)
	{
		ERROR2RAW("NULL parameters passed to Extender::Extend.");
		return;
	}

	if (((fExtendFlags & X_EXTEND) && (fExtendFlags & X_STRETCH)) ||
		((fExtendFlags & Y_EXTEND) && (fExtendFlags & Y_STRETCH)))
	{
		ERROR2RAW("Extender::Extend called with contradictory flags in fExtendFlags");
		return;
	}

	// quit immediately if any of the bounding rectangles is invalid,
	// or is of zero width or height.
	if (!drTriggerRelation.IsValid() ||
		!drTargetRelation.IsValid() ||
		!drNewTrigger.IsValid() ||
		!drLastTrigger.IsValid() ||
		!drLastTarget.IsValid() ||
		drTriggerRelation.Height() == 0 ||
		drTriggerRelation.Width() == 0 ||
		drTargetRelation.Height() == 0 ||
		drTargetRelation.Width() == 0 ||
		drNewTrigger.Height() == 0 ||
		drNewTrigger.Width() == 0 ||
		drLastTrigger.Height() == 0 ||
		drLastTrigger.Width() == 0 ||
		drLastTarget.Height() == 0 ||
		drLastTarget.Width() == 0)
	{
		ERROR2RAW("Extender::Extend; Invalid or zero-width rectangle");
		return;
	}

	// determine extension working values.
	ExtendParams eps;
	eps.pOp = pOp;
	CalculateExtendParams(	&eps, fExtendFlags, drTriggerRelation, drTargetRelation,
												drNewTrigger, drLastTrigger,
												drLastTarget, pdrDifference, ExtendAroundTarget );

	// this hasn't really changed size so do nothing
	if (!eps.xdec && !eps.xinc && !eps.ydec && !eps.yinc && eps.xscale == 1.0 && eps.yscale == 1.0
		&& eps.doccOffset.x == 0 && eps.doccOffset.y == 0)
		return;

	// invalidate the old image of the object, perform the extension
	// and flag that the object should be redrawn.
	// we must look above this node, to detect whether we are the child
	// of a node whose on-screen bounds are greater than ours.
	// if this is the case, we need to invalidate *that* node and
	// get it to redraw.
	// TODO: there may be a better way of doing this (is it necessary at all with the AllowOp mechanism?)
	BOOL bFoundBigParent = FALSE;
	Node* pInvalidateNode = pNode->FindParent();
	while (pInvalidateNode != NULL && !IS_A(pInvalidateNode, Layer) && !bFoundBigParent)
	{
		if (IS_A(pInvalidateNode, NodeBevelController) ||
			IS_A(pInvalidateNode, NodeShadowController) ||
			IS_A(pInvalidateNode, NodeContourController))
			bFoundBigParent = TRUE;
		else
			pInvalidateNode = pInvalidateNode->FindParent();
	}

	if (!bFoundBigParent)
		pInvalidateNode = pNode;

	((NodeRenderableInk*)pInvalidateNode)->RedrawObject();
	((NodeRenderableInk*)pInvalidateNode)->InvalidateBoundingRect();
	pNode->Extend(eps);

// DEBUG:
//#ifdef _DEBUG
//	TRACEUSER( "Karim", _T("Extend() on %s\n"), pNode->Name());
//#endif

	((NodeRenderableInk*)pInvalidateNode)->RedrawObject();
}



/********************************************************************************************

>	DocRect Extender::ValidateControlPoints(INT32 numPoints, const DocCoord* doccArray, const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/1999
	Inputs:		numPoints	number of coordinates to validate.
				doccArray	the array of coordinates.
				ExtParams	extend parameters.

	Returns:	A DocRect containing maximum safe shrink distances for the points:

					lox		maximum shrinkage in from the left.
					hix		maximum shrinkage in from the right.
					loy		maximum shrinkage in from the bottom.
					hiy		maximum shrinkage in from the top.

				If shrinking in a particular direction is ok, the
				corresponding DocRect value will be set to INT32_MAX.

	Purpose:	Validate the given points, using the provided extend parameters.
				If extending the points is invalid, then return a rectangle containing the
				largest distances which the points can be shrunk in by.
	Errors:		ERROR3 if doccArray is NULL.
	See also:	

********************************************************************************************/
DocRect Extender::ValidateControlPoints(INT32 numPoints, const DocCoord* doccArray, const ExtendParams& ExtParams)
{
	// validate data
	ERROR3IF(doccArray == NULL, "Extender::ValidateControlPoints- NULL doccArray passed!");

	DocRect drMaxSafeShrink(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX), drCheckExtend;

	// this check is only necessary if extension will be occurring along an
	// axis; if the extension type is stretch, or no action, no validation
	// is required.

	// for each of the four directions of displacement, and only if we shrank, check that
	// none of the points lie in a position to which they would not be returned under the
	// inverse extend operation. if they do, we record the largest safe amount we could
	// shrink them in by.

	// providing the extend operation involves displacing all points by the difference
	// between start- and end- centres, followed by the extension, these invalid points
	// will be only those which would be dragged inwards, to cross over the end-centre.

	if (ExtParams.fExtendFlags & X_EXTEND)
	{
		if (ExtParams.xinc < 0)
		{
			INT32 minXshrink = CheckInvalidShrinkingPoints(	ExtParams.doccEndCentre.x,
															ExtParams.xinc,
															ExtParams.doccOffset.x,
															numPoints, doccArray, TRUE );
			if (minXshrink >= 0)	// invalid, so record the return value.
				drMaxSafeShrink.hi.x = minXshrink;
		}

		// we do a different test if we're expanding.
		else if (ExtParams.xinc > 0)
		{
			INT32 minXshrink = CheckInvalidExpandingPoints(	ExtParams.doccEndCentre.x,
															ExtParams.xinc,
															ExtParams.doccOffset.x,
															numPoints, doccArray, TRUE );
			if (minXshrink >= 0)	// invalid, so record the return value.
				drMaxSafeShrink.hi.x = minXshrink;
		}

		if (ExtParams.xdec < 0)
		{
			INT32 minXshrink = CheckInvalidShrinkingPoints(	ExtParams.doccEndCentre.x,
															-ExtParams.xdec,
															ExtParams.doccOffset.x,
															numPoints, doccArray, TRUE );
			if (minXshrink >= 0)	// invalid, so record the return value.
				drMaxSafeShrink.lo.x = minXshrink;
		}

		// we do a different test if we're expanding.
		else if (ExtParams.xdec > 0)
		{
			INT32 minXshrink = CheckInvalidExpandingPoints(	ExtParams.doccEndCentre.x,
															-ExtParams.xdec,
															ExtParams.doccOffset.x,
															numPoints, doccArray, TRUE );
			if (minXshrink >= 0)	// invalid, so record the return value.
				drMaxSafeShrink.lo.x = minXshrink;
		}
	}

	if (ExtParams.fExtendFlags & Y_EXTEND)
	{
		if (ExtParams.yinc < 0)
		{
			INT32 minYshrink = CheckInvalidShrinkingPoints(	ExtParams.doccEndCentre.y,
															ExtParams.yinc,
															ExtParams.doccOffset.y,
															numPoints, doccArray, FALSE );
			if (minYshrink >= 0)	// invalid, so record the return value.
				drMaxSafeShrink.hi.y = minYshrink;
		}

		// we do a different test if we're expanding.
		else if (ExtParams.yinc > 0)
		{
			INT32 minYshrink = CheckInvalidExpandingPoints(	ExtParams.doccEndCentre.y,
															ExtParams.yinc,
															ExtParams.doccOffset.y,
															numPoints, doccArray, FALSE );
			if (minYshrink >= 0)	// invalid, so record the return value.
				drMaxSafeShrink.hi.y = minYshrink;
		}

		if (ExtParams.ydec < 0)
		{
			INT32 minYshrink = CheckInvalidShrinkingPoints(	ExtParams.doccEndCentre.y,
															-ExtParams.ydec,
															ExtParams.doccOffset.y,
															numPoints, doccArray, FALSE );
			if (minYshrink >= 0)	// invalid, so record the return value.
				drMaxSafeShrink.lo.y = minYshrink;
		}

		// we do a different test if we're expanding.
		else if (ExtParams.ydec > 0)
		{
			INT32 minYshrink = CheckInvalidExpandingPoints(	ExtParams.doccEndCentre.y,
															-ExtParams.ydec,
															ExtParams.doccOffset.y,
															numPoints, doccArray, FALSE );
			if (minYshrink >= 0)	// invalid, so record the return value.
				drMaxSafeShrink.lo.y = minYshrink;
		}
	}

	return drMaxSafeShrink;
}



/********************************************************************************************

>	INT32 Extender::CheckInvalidShrinkingPoints(	INT32 nCentre, INT32 nDelta, INT32 nOffset,
												INT32 nNumPoints, const DocCoord* doccArray,
												BOOL bExamineX )

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/1999
	Inputs:		nCentre		the limiting x or y centre-value, to which points are shrinking.
				nDelta		the distance by which the points are being shrunk.
				nOffset		an offset which should be applied to the points before testing.
				nNumPoints	the number of points to test.
				doccArray	the array of points to test.
				bExamineX	whether to examine x-coords (TRUE) or y-coords (FALSE).
	Outputs:	
	Returns:	The maximum safe distance that points can be extended towards nCentre by.
				This value will be non-negative if any invalid points were found, or
				negative if all points passed the test.
	Purpose:	Check whether any of the points in the given array will be shrunk past the
				given limiting value. If all the points check out, we return an all-clear
				value, which is negative. Otherwise, the maximum safe shrinkage is given.

				Karim 11/05/2000
				With the addition of invariant points, this method only checks those points
				which would move under extension. If a point is invariant, ie it is so close
				to nCentre that it would not be moved, it is disregarded. It is up to the
				various extend methods to ensure that these invariant points actually *stay*
				invariant!
	Errors:		
	See also:	

********************************************************************************************/
INT32 Extender::CheckInvalidShrinkingPoints(	INT32 nCentre, INT32 nDelta, INT32 nOffset,
											INT32 nNumPoints, const DocCoord* doccArray,
											BOOL bExamineX )
{
	// return with an all-clear if delta is zero.
	if (nDelta == 0)
		return -1;

	// test the points we were given, and determine the furthest we can move them without
	// dragging any into the dead zone, defined as the limit value +/- our buffer size.
	// this value will be the least of all the maximum distances that each point can
	// shrink by.
	INT32 maxShrink, smallest_maxShrink = INT32_MAX;

	// each point should be offset by nOffset so for testing, we 'un-offset' the centre.
	INT32 myCentre = nCentre - nOffset;

	// nDelta < 0 means we are being dragged down or left.
	// we're checking that each point can stretch by nDelta down towards nCentre.
	// if it can't, we see how far it _can_ go.
	if (nDelta < 0)
	{
		// validate the x-component of the coordinates.
		if (bExamineX)
		{
			for (INT32 i = 0; i < nNumPoints; i ++)
			{
				maxShrink = CheckLimits(myCentre, nDelta, doccArray[i].x);
				if (maxShrink >= 0 && smallest_maxShrink > maxShrink)
					smallest_maxShrink = maxShrink;
			}
		}

		// validate the y-component of the coordinates.
		else
		{
			for (INT32 i = 0; i < nNumPoints; i ++)
			{
				maxShrink = CheckLimits(myCentre, nDelta, doccArray[i].y);
				if (maxShrink >= 0 && smallest_maxShrink > maxShrink)
					smallest_maxShrink = maxShrink;
			}
		}
	}

	// nDelta > 0 means we are being dragged up or right.
	// we're checking that each point can stretch by nDelta up towards nCentre.
	// if it can't, we see how far it _can_ go.
	else
	{
		// validate the x-component of the coordinates.
		if (bExamineX)
		{
			for (INT32 i = 0; i < nNumPoints; i ++)
			{
				maxShrink = CheckLimits(myCentre, -nDelta, myCentre - doccArray[i].x + myCentre);
				if (maxShrink >= 0 && smallest_maxShrink > maxShrink)
					smallest_maxShrink = maxShrink;
			}
		}

		// validate the y-component of the coordinates.
		else
		{
			for (INT32 i = 0; i < nNumPoints; i ++)
			{
				maxShrink = CheckLimits(myCentre, -nDelta, myCentre - doccArray[i].y + myCentre);
				if (maxShrink >= 0 && smallest_maxShrink > maxShrink)
					smallest_maxShrink = maxShrink;
			}
		}
	}

	return smallest_maxShrink == INT32_MAX ? -1 : smallest_maxShrink;
}



/********************************************************************************************

>	inline INT32 Extender::CheckLimits(const INT32 limit, const INT32 delta, const INT32 value)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/1999
	Inputs:		limit		the limiting value to use.
				delta		the amount to attempt to shrink by - should always be negative.
				value		the value to validate.
	Outputs:	
	Returns:	-1	if the validation was successful,
				non-negative integer value otherwise.

	Purpose:	Checks that the given value may be reduced by the amount delta without
				going below the given limit value. If it cannot, we return the most which
				the given value can be safely reduced by.

				A preset buffer value is applied, defining a 'dead zone' around the limit,
				into which the value may not be reduced. All calculations performed within
				this method take account of this dead zone. Also note that if a value begins
				within the dead zone, then an all-clear value of -1 will be returned, as we
				don't want this invariant point to interfere with shrinking of other values.
				The actual extension code should always check for and not alter these points.

	Errors:		ERROR3 if delta is non-negative (ie >= 0).
	See also:	

********************************************************************************************/
inline INT32 Extender::CheckLimits(const INT32 limit, const INT32 delta, const INT32 value)
{
	// data validation.
	ERROR3IF(delta >= 0, "Extender::CheckLimits called with delta non-negative!");

	// we return an all-clear if the test value starts in the buffer zone.
	if (value >= limit - EXT_SHRINK_BUFFER && value <= limit + EXT_SHRINK_BUFFER)
		return -1;

	// give the all-clear if the test value lies below the limit value already.
	// also give the all-clear if the test value will not be drawn down into the dead-zone.
	if (value < limit - EXT_SHRINK_BUFFER || value + delta > limit + EXT_SHRINK_BUFFER)
		return -1;

	// ok, we're invalid - return the safe shrinkage distance for the test value.
	return value - (limit + EXT_SHRINK_BUFFER + 1);
}



/********************************************************************************************

>	void Extender::CalculateExtendParams(ExtendParams* pEPS, BYTE fExtendFlags,
						const DocRect& drTriggerRelation, const DocRect& drTargetRelation,
						const DocRect& drNewTrigger, const DocRect& drLastTarget )

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		pEPS				pointer to an ExtendParams object to fill.
				fExtendFlags		bitwise flags, describing any combination of x- or y-
									stretching or extending, except that you may not extend
									and stretch along an axis at the same time. Values:

									X_EXTEND	the node should be extended along the x-axis
									Y_EXTEND	the node should be extended along the y-axis
									X_STRETCH	the node should be stretched along the x-axis
									Y_STRETCH	the node should be stretched along the y-axis

				drTriggerRelation	relationship rectangle - the bounding box of the trigger
									sets when the relationship is defined.
				drTargetRelation	relationship rectangle - the bounding box of the target
									sets when the relationship is defined.
				drNewTrigger		bounding box of sets of triggers which started the call
									to this function.
				drLastTrigger		the bounding box of the trigger set(s) before it changed.
				drLastTarget		the bounding box of the target set before it is extended.
				pdrDifference		a rectangle containing four difference parameters which
									may be used to alter the extend. ok to omit or be NULL.

	Outputs:	*pEPS will be set up to contain extend information derived from the inputs.
	Purpose:	Given the rectangles and flags which define an extension, this function
				derives the bare information which a Node requires to perform the extension
				on itself.
	Errors:		ERROR3 in debug builds if pEPS is NULL.
	See also:	Extender::Extend().

********************************************************************************************/
void Extender::CalculateExtendParams(ExtendParams* pEPS, BYTE fExtendFlags,
						const DocRect& drTriggerRelation, const DocRect& drTargetRelation,
						const DocRect& drNewTrigger, const DocRect& drLastTrigger,
						const DocRect& drLastTarget, const DocRect* pdrDifference,
						const BOOL ExtendAroundTarget)
{
	// validate input.
	ERROR3IF(pEPS == NULL, "Extender::CalculateExtendParams- pEPS is NULL!");

	// use the relationship rects and the new trigger rect to calculate the new target rect.
	DocRect drNewTarget(drNewTrigger);

	// x-extend - the new target rect must be x-offset from new-trigger on each
	// side, by the same amount as target-relation from trigger-relation.
	if (fExtendFlags & X_EXTEND)
	{
		// if the trigger and target-relation rects do not overlap in the x-direction,
		// then the new target rect must be x-offset from new-trigger on its near-side,
		// by the same amount as target-relation from trigger-relation.
		if (drTargetRelation.lo.x > drTriggerRelation.hi.x)
		{
			drNewTarget.lo.x  = drNewTrigger.hi.x + (drTargetRelation.lo.x - drTriggerRelation.hi.x);
			drNewTarget.hi.x  = drNewTrigger.hi.x + (drTargetRelation.hi.x - drTriggerRelation.hi.x);
		}
		else if (drTargetRelation.hi.x < drTriggerRelation.lo.x)
		{
			drNewTarget.lo.x = drNewTrigger.lo.x - (drTriggerRelation.lo.x - drTargetRelation.lo.x);
			drNewTarget.hi.x = drNewTrigger.lo.x - (drTriggerRelation.lo.x - drTargetRelation.hi.x);
		}

		// normal extend behaviour - distances between corresponding sides are kept constant.
		else
		{
			drNewTarget.lo.x += drTargetRelation.lo.x - drTriggerRelation.lo.x;
			drNewTarget.hi.x += drTargetRelation.hi.x - drTriggerRelation.hi.x;

			// if the target's new sides are coincident or invalid, then
			// they need to be moved back into valid positions.
			if (drNewTarget.lo.x >= drNewTarget.hi.x)
			{
				drNewTarget.hi.x = drNewTarget.lo.x + 2;
			}
		}
	}
	else if (fExtendFlags & X_STRETCH)
	{
		double a = drNewTrigger.Width() / (double)drTriggerRelation.Width();
		INT32 b = drNewTrigger.hi.x - (INT32)(a * drTriggerRelation.hi.x);

		drNewTarget.lo.x = b + (INT32)(a * drTargetRelation.lo.x);
		drNewTarget.hi.x = b + (INT32)(a * drTargetRelation.hi.x);
	}

	// y-extend - the new target rect must be y-offset from new-trigger on top and
	// bottom, by the same amount as target-relation from trigger-relation.
	if (fExtendFlags & Y_EXTEND)
	{
		// if the trigger and target-relation rects do not overlap in the y-direction,
		// then the new target rect must be y-offset from new-trigger on its near-side,
		// by the same amount as target-relation from trigger-relation.
		if (drTargetRelation.lo.y > drTriggerRelation.hi.y)
		{
			drNewTarget.lo.y  = drNewTrigger.hi.y + (drTargetRelation.lo.y - drTriggerRelation.hi.y);
			drNewTarget.hi.y  = drNewTrigger.hi.y + (drTargetRelation.hi.y - drTriggerRelation.hi.y);
		}
		else if (drTargetRelation.hi.y < drTriggerRelation.lo.y)
		{
			drNewTarget.lo.y = drNewTrigger.lo.y - (drTriggerRelation.lo.y - drTargetRelation.lo.y);
			drNewTarget.hi.y = drNewTrigger.lo.y - (drTriggerRelation.lo.y - drTargetRelation.hi.y);
		}

		// normal extend behaviour - distances between corresponding sides are kept constant.
		else
		{
			drNewTarget.lo.y += drTargetRelation.lo.y - drTriggerRelation.lo.y;
			drNewTarget.hi.y += drTargetRelation.hi.y - drTriggerRelation.hi.y;

			// if the target's new top and bottom are coincident or invalid, then
			// they need to be moved back into valid positions.
			if (drNewTarget.lo.y >= drNewTarget.hi.y)
			{
				drNewTarget.hi.y = drNewTarget.lo.y + 2;
			}
		}
	}
	else if (fExtendFlags & Y_STRETCH)
	{
		double a = drNewTrigger.Height() / (double)drTriggerRelation.Height();
		INT32 b = drNewTrigger.hi.y - (INT32)(a * drTriggerRelation.hi.y);

		drNewTarget.lo.y = b + (INT32)(a * drTargetRelation.lo.y);
		drNewTarget.hi.y = b + (INT32)(a * drTargetRelation.hi.y);
	}

	// use the old and new target rectangles to calculate our extension working values.

	// extend flags.
	pEPS->fExtendFlags = fExtendFlags;

	// the start- and end- centres of extension + the offset between them.
	// Karim 14/03/2000 - changed to use the trigger set bounds if their old centre
	// lies within the bounds of the old target set.
	if (!ExtendAroundTarget && drLastTarget.ContainsCoord(drLastTrigger.Centre()))
	{
		pEPS->doccStartCentre = drLastTrigger.Centre();
		pEPS->doccEndCentre = drNewTrigger.Centre();
		pEPS->doccOffset = pEPS->doccEndCentre - pEPS->doccStartCentre;
	}
	else
	{
		pEPS->doccStartCentre = drLastTarget.Centre();
		pEPS->doccEndCentre = drNewTarget.Centre();
		pEPS->doccOffset = pEPS->doccEndCentre - pEPS->doccStartCentre;
	}

	// magnitudes of extension. note that under extension, objects are first offset
	// from start- to end- centre, then extended. this makes extension mirror-symmetric
	// so that xinc == xdec (+/- 1).
	pEPS->xinc = drNewTarget.hi.x - (drLastTarget.hi.x + pEPS->doccOffset.x);
	pEPS->xdec = (drLastTarget.lo.x + pEPS->doccOffset.x) - drNewTarget.lo.x;
	pEPS->yinc = drNewTarget.hi.y - (drLastTarget.hi.y + pEPS->doccOffset.y);
	pEPS->ydec = (drLastTarget.lo.y + pEPS->doccOffset.y) - drNewTarget.lo.y;

	// scale factors, if automatic _stretching_ is required.
	pEPS->xscale = drNewTarget.Width() / (double)drLastTarget.Width();
	pEPS->yscale = drNewTarget.Height() / (double)drLastTarget.Height();

	// if our difference rect is non-NULL, then we may need to substitute
	// xinc, xdec, yinc, ydec with negative copies of some of its values.
	if (pdrDifference != NULL)
	{
		if (pdrDifference->lo.x != INT32_MAX) pEPS->xdec = -pdrDifference->lo.x;
		if (pdrDifference->lo.y != INT32_MAX) pEPS->ydec = -pdrDifference->lo.y;
		if (pdrDifference->hi.x != INT32_MAX) pEPS->xinc = -pdrDifference->hi.x;
		if (pdrDifference->hi.y != INT32_MAX) pEPS->yinc = -pdrDifference->hi.y;
	}

	// TODO: should probably not use these, but the equivalent of b (used above) instead.
	pEPS->doccScaleStart = drLastTarget.lo;
	pEPS->doccScaleEnd = drNewTarget.lo;

	// set the various dead-zones; these depend on the signs of the expansions in each dirn.
	pEPS->xincExtendBuffer = (pEPS->xinc >= 0) ? EXT_EXPAND_BUFFER : EXT_SHRINK_BUFFER;
	pEPS->xdecExtendBuffer = (pEPS->xdec >= 0) ? EXT_EXPAND_BUFFER : EXT_SHRINK_BUFFER;
	pEPS->yincExtendBuffer = (pEPS->yinc >= 0) ? EXT_EXPAND_BUFFER : EXT_SHRINK_BUFFER;
	pEPS->ydecExtendBuffer = (pEPS->ydec >= 0) ? EXT_EXPAND_BUFFER : EXT_SHRINK_BUFFER;
}



/********************************************************************************************

>	INT32 Extender::CheckInvalidExpandingPoints(	INT32 nCentre, INT32 nDelta, INT32 nOffset,
												INT32 nNumPoints, const DocCoord* doccArray,
												BOOL bExamineX )

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/2000
	Inputs:		nCentre		the centre x- or y- value from which point are expanding.
				nDelta		(currently unused)	the distance by which the points are moving.
				nOffset		an offset which should be applied to the points before testing.
				nNumPoints	the number of points to test.
				doccArray	the array of points to test.
				bExamineX	whether to examine x-coords (TRUE) or y-coords (FALSE).
	Outputs:	
	Returns:	Negative value if all points can expand safely away from nCentre, or
				Zero if any of the points cannot be safely moved away.

	Purpose:	Two checks:

				No points may lie inside the dead-zone.

				Due to rounding errors, the centre-point of an object depends on whether its
				width is even or odd. There is a potential for error here, if a point lying
				one millipoint outside the dead zone is moved out from the centre, and the
				operation then reversed. If the centre-point changes position due to a new
				width then the point may be moving back *inside* the dead zone, which will
				be disallowed -> irreversible operation.

				This method checks for these points. (boy, what a mouthful!)

	Assumptions:	That the extension being checked is symmetric - ie all points are moving
					away from the centre point, regardless of their position relative to it.
	Errors:		
	See also:	

********************************************************************************************/
INT32 Extender::CheckInvalidExpandingPoints(	INT32 nCentre, INT32 nDelta, INT32 nOffset,
											INT32 nNumPoints, const DocCoord* doccArray,
											BOOL bExamineX )
{
	// return with an all-clear if delta is zero.
	if (nDelta == 0)
		return -1;

	// each point should be offset by nOffset, so instead we 'un-offset' the centre.
//	INT32 myCentre = nCentre - nOffset;

/*
 *	Karim 11/05/2000
 *	Commented out, as this test is being moved into the actual extension code - rather than
 *	have invariant points limit the extension, they will have no effect, and will instead
 *	just not themselves extend.
 *
	// The test we do is simple - ensure that no points lie within one millipoint of the
	// dead zone. We only need one invalid point for the test to fail.
	BOOL bPassedTest = TRUE;
	if (bExamineX)
	{
		for (INT32 i = 0; i < nNumPoints && bPassedTest; i ++)
		{
			bPassedTest =	(doccArray[i].x > (myCentre + EXT_EXPAND_BUFFER)) ||
							(doccArray[i].x < (myCentre - EXT_EXPAND_BUFFER));
		}
	}
	else
	{
		for (INT32 i = 0; i < nNumPoints && bPassedTest; i ++)
		{
			bPassedTest =	(doccArray[i].y > (myCentre + EXT_EXPAND_BUFFER)) ||
							(doccArray[i].y < (myCentre - EXT_EXPAND_BUFFER));
		}
	}

	return bPassedTest ? -1 : 0;
 */

	return -1;
}
