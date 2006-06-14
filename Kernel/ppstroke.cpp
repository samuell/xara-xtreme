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
// pathpcs - Definition of rendering PathProcessor class

#include "camtypes.h"

#include "ppstroke.h"

//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "pathstrk.h"
#include "pathtrap.h"
#include "qualattr.h"
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "strkattr.h"
#include "valfunc.h"
#include "nodetxtl.h" // for the format region
//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "attrmap.h"
#include "lineattr.h"
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "brshattr.h"
#include "ai_epsrr.h"	// for definition of AIEPSRenderRegion. Only used in ProcessStroke()

#include "ophist.h"
#include "combshps.h"

CC_IMPLEMENT_DYNAMIC(PathProcessorStroke, PathProcessor);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



/********************************************************************************************

>	PathProcessorStroke::PathProcessorStroke()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/12/96

	Purpose:	Constructor

********************************************************************************************/

PathProcessorStroke::PathProcessorStroke()
{
}



/********************************************************************************************

>	virtual BOOL PathProcessorStroke::WillChangeFillAndStrokeSeparately(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/12/96

	Returns:	TRUE

	Purpose:	Called by the RenderRegion to determine if this PathProcessorStroke affects
				the "fill" and "stroke" portions of the path separately. (Generally
				speaking, only fill/stroke providers will cause these two different
				"bits" of a path to be rendered separately). This call is made BEFORE
				this Processor's ProcessPath function will be called.

				If the caller gets a TRUE back, the stroke and fill paths will be
				rendered separately.

	Notes:		Base class implementation returns FALSE. Derived Stroke and Fill
				processors (such as this one) override this method to return TRUE.

********************************************************************************************/

BOOL PathProcessorStroke::WillChangeFillAndStrokeSeparately(void)
{
	return(TRUE);
}



/********************************************************************************************

>	virtual void PathProcessorStroke::ProcessPath(Path *pPath,
												  RenderRegion *pRender,
												  PathShape ShapePath = PATHSHAPE_PATH);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/12/96

	Purpose:	Called by the RenderRegion to apply the path processing operation to 
				the given path.

				The PathProcessorStroke class changes the stroking (only) of paths
				passed into it.

	Notes:		* When rendering a path, always pass in your 'this' pointer to
				  RenderRegion::DrawPath, so that you don't start an infinite
				  recursion!

				* To stop rendering of the path, simply return without calling the RR

				* To render this path unchanged, simply call directly back to the RR:
					pRender->DrawPath(pPath, this);
				
				* Only affect the fill of this path if pPath->IsFilled

				* Only affect the stroke of this path if pPath->IsStroked

				* If converting a path into a "filled path" for stroking, the output
				  path should be rendered with IsStroked=FALSE or it'll get a line
				  around the outside!				

********************************************************************************************/

void PathProcessorStroke::ProcessPath(Path *pPath,
									  RenderRegion *pRender,
									  PathShape ShapePath)
{
	TRACEUSER( "Diccon", _T("Rendering stroke\n"));
	ERROR3IF(pPath == NULL || pRender == NULL, "Illegal NULL Params");

	// --- If the provided path is not stroked, then we'll just pass it straight through
	// We also don't touch it if we're doing EOR rendering
	if (!pPath->IsStroked || pRender->DrawingMode != DM_COPYPEN)
	{
		pRender->DrawPath(pPath, this, ShapePath);
		return;
	}

	// --- If the quality is set low, strokes are just rendered as centrelines
	// BLOCK
	{
		QualityAttribute *pQuality = (QualityAttribute *) pRender->GetCurrentAttribute(ATTR_QUALITY);
		if (pQuality != NULL && pQuality->QualityValue.GetLineQuality() != Quality::FullLine)
		{
			pRender->DrawPath(pPath, this, ShapePath);
			return;
		}
	}

	// --- If the attribute which created us is not the current StrokeType attribute, then
	// we have been overridden by a different stroke type, so we do nothing.
	// BLOCK
	{
		StrokeTypeAttrValue *pTypeAttr = (StrokeTypeAttrValue *) pRender->GetCurrentAttribute(ATTR_STROKETYPE);
		if (pTypeAttr != NULL && pTypeAttr != GetParentAttr())
		{
			pRender->DrawPath(pPath, this);
			return;
		}
	}

	// --- We don't expect the input path to be stroked AND filled on entry
	ERROR3IF(pPath->IsFilled, "PathProcessor expected RenderRegion to handle IsFilled case");

	// --- Get the current line width, cap style, and join style from the render region
	// In case of failure, we initialise with suitable defaults
	INT32 LineWidth = 5000;

	// BLOCK
	{
		LineWidthAttribute *pWidthAttr = (LineWidthAttribute *) pRender->GetCurrentAttribute(ATTR_LINEWIDTH);
		if (pWidthAttr != NULL)
			LineWidth = pWidthAttr->LineWidth;
	}

	// --- Create a stroke outline by calling our helper function

	// (ChrisG 4/11/00) - Use smoothed paths with AIExport. This is only done for the export, as the smooth
	//	path creation function is about five times slower than the non-smoothed one.
	Path * pOutput = NULL;
	if (pRender->IsKindOf (CC_RUNTIME_CLASS (AIEPSRenderRegion)))
	{
		pOutput = CreateSmoothVarWidthStroke(pPath, pRender, LineWidth);
	}
	else
	{
		pOutput = CreateVarWidthStroke(pPath, pRender, LineWidth);
	}
	
	if (pOutput == NULL)
	{
		pRender->DrawPath(pPath, this);
		return;
	}

	// --- Finally, render the new stroke outline path in place of the one we were given
	pRender->SaveContext();

		// !!!!****ToDo - for now, strokes always render flat-filled with the stroke colour
		StrokeColourAttribute *pStrokeColour = (StrokeColourAttribute *) pRender->GetCurrentAttribute(ATTR_STROKECOLOUR);
		if (pStrokeColour != NULL)
			pRender->SetFillColour(pStrokeColour->Colour);

		pRender->SetWindingRule(NonZeroWinding);
		pRender->DrawPath(pOutput, this);

	pRender->RestoreContext();

	// --- And clean up
	delete pOutput;
}

/********************************************************************************************

>	BOOL PathProcessorStroke::DoBecomeA(BecomeA* pBecomeA, Path* pPath, Node* pParent)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/2000
	Inputs:		pBecomeA - the object that tells us what to become, and recieves the results
				pPath - the path that we are processing
				pParent- the node that this brush is applied to
	Outputs:	
	Returns:	TRUE if everything went ok,
	Purpose:	To return the processed path to a BecomeA object.  Does pretty much the same
				as process path but withot rendering anything.
		
	Note:		Because of the problems with bevelling and contouring this function will never
				be called as part of the normal DoBecomeA procedure.  You have to specifically
				locate the AttrStrokeType and call its DoBecomeA directly if you wish to 
				extract the paths from a stroke.  This should be resolved somehow after
				the release.

				25/10/2000 Extra note:  We are now working out a mechanism which uses
				GetProcessedPath and GetOutlinePath, so this function may become obsolete
********************************************************************************************/

BOOL PathProcessorStroke::DoBecomeA(BecomeA* pBecomeA, Path* pPath, Node* pParent)
{
	ERROR2IF(pBecomeA == NULL, FALSE, "BecomeA pointer is NULL in PathProcessorStroke::DoBecomeA");
	ERROR2IF(pParent == NULL, FALSE, "Parent is NULL in PathProcessorStroke::DoBecomeA");
	ERROR2IF(pPath == NULL, FALSE, "Path is NULL in PathProcessorStroke::DoBecomeA");
	

	// we need to make a new nodepath
	NodePath* pNewNode = GetSmoothProcessedPath(pPath, pParent);

	if (pNewNode == FALSE)
		return FALSE;

	BOOL Success = FALSE;
	// now what we do depends on what type of BecomeA we've got
	if (pBecomeA->BAPath())
	{
		switch (pBecomeA->GetReason())
		{
			case BECOMEA_REPLACE:
			{
				UndoableOperation* pUndoOp = pBecomeA->GetUndoOp();
				ERROR2IF(pUndoOp == NULL, FALSE, "No op pointer in PathProcessorStroke::DoBecomeA");
				//  apply attributes to our new node
				// Get the attributes
				CCAttrMap AttribMap(30);
				if (!((NodeRenderableInk*)pParent)->FindAppliedAttributes(&AttribMap))
					break;
				
				// another bodge: this time if we have a stroked path that is not closed and 
				// we have given it a stroke colour then we want this stroke colour to become
				// the fill colour of the new shape
				AttrStrokeColour * pColour = NULL;
			//	if (!pParent->IsNodePath() && ((NodePath*)pParent)->InkPath.IsClosed())
				{
					// look up the stroke colour attribute
					
					//AttribMap.Lookup((void *)CC_RUNTIME_CLASS(AttrStrokeColour),
					//	(void *&)pColour);
					((NodeRenderableInk*)pParent)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), (NodeAttribute**)&pColour);
					// make a new flat fill attribute and apply this to the node
					if (pColour)
					{
						StrokeColourAttribute * pAttrVal = (StrokeColourAttribute *)pColour->GetAttributeValue();

						if (pAttrVal)
						{
							AttrFlatColourFill *pFill = NULL;
							// create and insert at first child
							ALLOC_WITH_FAIL(pFill, new AttrFlatColourFill(pNewNode, FIRSTCHILD), pUndoOp);

							pFill->SetStartColour(&(pAttrVal->Colour));
							// remove from the map so we don't duplicate
							AttribMap.RemoveKey(pFill->GetAttributeType());
						}
					}

				}

				if (!pNewNode->ApplyAttributes(&AttribMap))
					break;
			
				// insert the new node and delete the original
				BOOL ok = TRUE;
				// If we don't own the parent node, we should insert after (on top of) it
				if (pBecomeA->IsSecondary())
					ok = pUndoOp->DoInsertNewNode(pNewNode,pParent,NEXT,FALSE,FALSE,FALSE,FALSE);
				else
				{
					ok = pUndoOp->DoInsertNewNode(pNewNode,pParent,PREV,FALSE,FALSE,FALSE,FALSE);
					// If this BecomeA is a primary, then delete the source node
					if (ok)
						ok = pUndoOp->DoHideNode((NodeRenderableInk*)pParent, TRUE);
				}

				if (!ok)
					break;


				if (!pBecomeA->IsCombineBecomeA())
				{
					// pass it back then
					pBecomeA->PassBack(pNewNode,(NodeRenderableInk*)pParent);
				}
				else
				{
					CombineBecomeA* pPassback = (CombineBecomeA*) pBecomeA;
				
					pPassback->SetStrokeCreatedPassbackPath(TRUE);
					pBecomeA->PassBack(pNewNode,(NodeRenderableInk*)pParent);
					pPassback->SetStrokeCreatedPassbackPath(FALSE);
				}
				Success = TRUE;
			}
			break;

			case BECOMEA_PASSBACK :
			{
				if (!pBecomeA->IsCombineBecomeA ())
				{
					// pass it back then
					Success = pBecomeA->PassBack(pNewNode,(NodeRenderableInk*)pParent);
				}
				else
				{
					CombineBecomeA* pPassback = (CombineBecomeA*) pBecomeA;

					pPassback->SetStrokeCreatedPassbackPath(TRUE);
					Success = pBecomeA->PassBack(pNewNode,(NodeRenderableInk*)pParent);
					pPassback->SetStrokeCreatedPassbackPath(FALSE);
				}
			}
			break;

			default:
				break;
		}
	}
	
	
	if (!Success)
		delete pNewNode;

	return Success;
}


/********************************************************************************************

>	NodePath* PathProcessorStroke::GetProcessedPath(Path* pPath, Node* pParent, BOOL OutlineOnly = FALSE)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/2000
	Inputs:		pPath - the path that we are processing
				pParent- the node that this brush is applied to
	Outputs:	
	Returns:	A NodePath, containing the path generated by our path stroker
	Purpose:	To return the stroked path
		
	Notes:	
********************************************************************************************/

NodePath* PathProcessorStroke::GetProcessedPath(Path* pPath, Node* pParent)
{
	PORTNOTETRACE("other","PathProcessorStroke::GetProcessedPath - do nothing");
	return NULL;
#ifndef EXCLUDE_FROM_XARALX
	// we need to fake a renderregion as our helper functions need one,
	// luckily FormatRegion is on hand from the text stuff
	FormatRegion FakeRender;

	if (!FakeRender.Init((NodeRenderableInk*)pParent)) // init renders all applied attributes into the region
		return FALSE;

	
	// From here copied from ProcessPath:
	// --- Get the current line width, cap style, and join style from the render region
	// In case of failure, we initialise with suitable defaults
	INT32 LineWidth = 5000;

	// BLOCK
	{
		LineWidthAttribute *pWidthAttr = (LineWidthAttribute *) FakeRender.GetCurrentAttribute(ATTR_LINEWIDTH);
		if (pWidthAttr != NULL)
			LineWidth = pWidthAttr->LineWidth;
	}

	// --- Create a stroke outline by calling our helper function
	Path *pOutput = CreateVarWidthStroke(pPath, &FakeRender, LineWidth);
	if (pOutput == NULL)
		return FALSE;

	if (!pOutput->IsClosed())
		pOutput->TryToClose();

	// we need to make a new nodepath
	NodePath* pNewNode = new NodePath; 
		
	if (pNewNode == NULL)
	{
		delete pOutput;
		return FALSE;
	}
	
	// initialise the path
	if (!pNewNode->InkPath.Initialise(pOutput->GetNumCoords(), 128))
	{
		delete pOutput;
		delete pNewNode;
		return FALSE;
	}

	// Copy our path data into it
	//CALL_WITH_FAIL(!pNewNode->InkPath.CopyPathDataFrom(pOutput), pUndoOp, Success)
	if (!pNewNode->InkPath.CopyPathDataFrom(pOutput))
	{
		delete pOutput;
		delete pNewNode;
		return FALSE;
	}
	// don't need that path anymore
	delete pOutput;
	pOutput = NULL;

	return pNewNode;
#endif
}


/********************************************************************************************

>	virtual BOOL PathProcessorStroke::IsDifferent(PathProcessorStroke *pOther);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Inputs:		pOther - The other PathProcessorStroke

	Returns:	TRUE if they're considered different, FALSE if they are equivalent

	Purpose:	Equality operator

	Notes:		The base class implementation compares the runtime classes of the
				2 objects to see if they are different classes. If they are the same
				class, it assumes they're cnsidered equal - derived classes should
				override this behaviour if further comparisons are necessary.

********************************************************************************************/

BOOL PathProcessorStroke::IsDifferent(PathProcessorStroke *pOther)
{
	ERROR3IF(pOther == NULL, "Illegal NULL param");

	if (GetRuntimeClass() != pOther->GetRuntimeClass())
		return(TRUE);

	// Assume that we're the same, as the base class is very simple
	return(FALSE);
}



/********************************************************************************************

>	virtual PathProcessorStroke *PathProcessorStroke::Clone(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Returns:	NULL if we're out of memory, else
				a pointer to a clone (exact copy) of this object

	Purpose:	To copy PathProcessorStroke or derived-class object

********************************************************************************************/

PathProcessorStroke *PathProcessorStroke::Clone(void)
{
	// Clone this object - this can be done by just creating a new one
	PathProcessorStroke *pClone = new PathProcessorStroke;

	// And copy the (base class) parent-attribute pointer so we know when our
	// parent attribute is "current" in the render region.
	if (pClone != NULL)
		pClone->SetParentAttr(GetParentAttr());

	return(pClone);
}


/********************************************************************************************

>	virtual BOOL PathProcessorStroke::DoesActuallyDoAnything(RenderRegion* pRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/2000
	Inputs:		pRender - the render region we are about to render into.  Note that it is 
				important that the region has had all its attributes rendered into it
	Returns:	TRUE if this PPS is going to change the path, FALSE if it will not.
				
	Purpose:	Allows the render region to know whether this PPS will do anything, if not then
				it can DrawPath normally allowing Dash patterns to be applied to nodes with
				constant VarWidth attributes applied to them

********************************************************************************************/

BOOL PathProcessorStroke::DoesActuallyDoAnything(RenderRegion* pRender)
{
	ERROR2IF(pRender == NULL, FALSE, "RenderRegion is NULL in PathProcessorStroke::DoesActuallyDoAnything");

	if (IsDisabled())
		return FALSE;

	// Get the value function from the render region and have a look at it
	ValueFunction *pValFunc = NULL;

	VariableWidthAttrValue *pVarWidthAttr = (VariableWidthAttrValue *) pRender->GetCurrentAttribute(ATTR_VARWIDTH);
	if (pVarWidthAttr != NULL)
		pValFunc = pVarWidthAttr->GetWidthFunction();

	// If it is a constant width stroke, then we don't really do anything
	if (pValFunc == NULL || IS_A(pValFunc, ValueFunctionConstant))
		return FALSE;

	// likewise if we have a brush don't do anything either, because we will use the stroke to 
	// generate a pressure curve for the brush
	BrushAttrValue* pBrush  = (BrushAttrValue*) pRender->GetCurrentAttribute(ATTR_BRUSHTYPE);
	if (pBrush != NULL && pBrush->GetBrushHandle() != BrushHandle_NoBrush)
		return FALSE;

	return TRUE;
}

/********************************************************************************************

>	TrapsList *PathProcessorStroke::PrepareTrapsForStroke(Path *pPath, RenderRegion *pRender,
															INT32 LineWidth)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/12/96 (split into new function, 6/2/97)

	Inputs:		pPath		- The source path to be stroked
				pRender		- The render region it will be drawn to
				LineWidth	- The maximum width of all strokes you intend to render with the
							  resulting TrapsList. Normally this is the current line width from
							  the output RenderRegion.

	Returns:	NULL if it failed, else
				a pointer to a new TrapsList object (which the caller must delete).

	Purpose:	Precalculation function to be used in tandem with CreateVarWidthStroke().
				If you want to create several strokes along the same path, then precalculating
				the trapezoids will give a large performance improvement.

				Call this with the width of the largest stroke you intend to render, and
				then render all strokes using the provided trapezoids.

	SeeAlso:	PathProcessorStroke::CreateVarWidthStroke
		
********************************************************************************************/

TrapsList *PathProcessorStroke::PrepareTrapsForStroke(Path *pPath, RenderRegion *pRender, INT32 LineWidth)
{
	LineCapType LineCap = LineCapButt;
	JointType JoinStyle = RoundJoin;

	// BLOCK
	{
		// --- Get the current line cap style from the render region
		StartCapAttribute *pCapAttr = (StartCapAttribute *) pRender->GetCurrentAttribute(ATTR_STARTCAP);
		if (pCapAttr != NULL)
			LineCap = pCapAttr->StartCap;

		// --- Get the current line join style from the render region
		JoinTypeAttribute *pJoinAttr = (JoinTypeAttribute *) pRender->GetCurrentAttribute(ATTR_JOINTYPE);
		if (pJoinAttr != NULL)
			JoinStyle = pJoinAttr->JoinType;
	}

	ProcessPathToTrapezoids GenerateTraps(64);
	TrapsList *pTrapezoids = new TrapsList;

	if (pTrapezoids != NULL && GenerateTraps.Init(pPath, pTrapezoids))
	{
		// Flags are: Flatten, QuantiseLines, QuantiseAll
		ProcessFlags PFlags(TRUE, FALSE, FALSE);
		if (!GenerateTraps.Process(PFlags, TrapTravel_Parametric, JoinStyle))
		{
			delete pTrapezoids;
			pTrapezoids = NULL;
		}
	}

	return pTrapezoids;
}



/********************************************************************************************

>	Path *PathProcessorStroke::CreateVarWidthStroke(Path *pPath, RenderRegion *pRender,
													INT32 LineWidth, TrapsList *pTrapezoids = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/12/96 (split into new function, 6/2/97)

	Inputs:		pPath		- The source path to be stroked
				pRender		- The render region it will be drawn to

				LineWidth	- The maximum width of the stroke. Normally this is the current
							  line width from the RndRgn, but derived classes like airbrushes
							  like to vary this value, hence it is a parameter.

				pTrapezoids	- NULL (in which case this function will generate the TrapList for itself),
							  or a pointer to a precalculated trapezoid list (this should be generated
							  by calling PathProcessorStroke::PrepareTrapsForStroke, and should be
							  used if you are generating multiple strokes from one source (e.g.
							  see the Airbrush stroker - ppairbsh.cpp)).
							  (NOTE that using one TrapsList for multiple strokes will also give more
							  consistent positioning of width/pattern elements along the path length)

	Returns:	NULL if it failed, else
				a pointer to a new Path object (which the caller must delete) to be
				drawn instead of the provided pPath parameter.

	Purpose:	Strokes the given path according to variable width parameters in the
				given render region, producing a new path representing the outline of
				a variable-width stroke.

				Internal helper funciton for this class and derived classes.

	SeeAlso:	PathProcessorStroke::ProcessPath;
				PathProcessorStroke::PrepareTrapsForStroke

********************************************************************************************/

Path *PathProcessorStroke::CreateVarWidthStroke(Path *pPath, RenderRegion *pRender, INT32 LineWidth,
												TrapsList *pTrapezoids)
{
	ERROR3IF(pPath == NULL || pRender == NULL, "Illegal NULL Params");
	ERROR3IF(!pPath->IsStroked, "PathProcessor expects to be given a stroked path");

	// --- Ignore infinitely thin strokes
	if (LineWidth < 1)
		return(NULL);

	// --- Create a new path to be rendered in place of the provided path
	// Note that I use a large allocation size so that reallocation need not be done
	// frequently, which also helps reduce memory fragmentation.
	Path *pOutput = new Path;
	if (pOutput == NULL)
		return(NULL);

	pOutput->Initialise(128, 128);

	// --- Get the current cap style from the render region
	// In case of failure, we initialise with suitable defaults
	LineCapType LineCap = LineCapButt;
	// BLOCK
	{
		StartCapAttribute *pCapAttr = (StartCapAttribute *) pRender->GetCurrentAttribute(ATTR_STARTCAP);
		if (pCapAttr != NULL)
			LineCap = pCapAttr->StartCap;
	}

	// --- Get the variable line width descriptor from the render region
	ValueFunction *pValFunc = NULL;
	// BLOCK
	{
		VariableWidthAttrValue *pVarWidthAttr = (VariableWidthAttrValue *) pRender->GetCurrentAttribute(ATTR_VARWIDTH);
		if (pVarWidthAttr != NULL)
			pValFunc = pVarWidthAttr->GetWidthFunction();

		// If it is a constant width stroke, we'll just get GDraw to stroke it for us,
		// as that code is optimised assembler, and need not worry about variable width,
		// and as a result is about 4 times faster!
		if (pValFunc == NULL || IS_A(pValFunc, ValueFunctionConstant))
		{
			// Get the current line join style from the render region
			JointType JoinStyle = RoundJoin;
			JoinTypeAttribute *pJoinAttr = (JoinTypeAttribute *) pRender->GetCurrentAttribute(ATTR_JOINTYPE);
			if (pJoinAttr != NULL)
				JoinStyle = pJoinAttr->JoinType;

			pPath->StrokePathToPath(LineWidth, LineCap, JoinStyle, NULL, pOutput);

			pOutput->IsFilled  = TRUE;
			pOutput->IsStroked = FALSE;
			return(pOutput);
		}
	}

	// --- If none were passed in, generate the set of trapezoids for the dest path
	BOOL MustDeleteTraps = FALSE;
	if (pTrapezoids == NULL)
	{
		MustDeleteTraps = TRUE;
		pTrapezoids = PrepareTrapsForStroke(pPath, pRender, LineWidth);
		if (pTrapezoids == NULL)
			return(NULL);
	}

	// --- Stroke the trapezoids into a variable-width path
	PathStroker Stroker(pValFunc, LineWidth, LineCap);
	
	// our new option allows us toonly get the forward mapped path

	if (!Stroker.StrokePath(pTrapezoids, pOutput))
	{
		if (MustDeleteTraps)
			delete pTrapezoids;
		return(NULL);
	}

	if (MustDeleteTraps)
		delete pTrapezoids;

	return(pOutput);
}



/********************************************************************************************

>	Path* PathProcessorStroke::GetProcessedPath(Path* pPath, CCAttrMap* pAttrMap)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/2000
	Inputs:		pPath		the path to stroke
				pAttrMap	the attr-map to get things like line-width and cap styles from.

	Returns:	ptr to a new path which is a stroked version of the supplied source path, or
				NULL if unsuccessful.

	Purpose:	This is a rehash of Diccon's GetProcessedPath method, except you needn't
				worry about any nodes but the four NodeAttributes below, which must live
				in the supplied attr-map.

				We also don't use render-regions, as their not actually necessary
				(the other version of this method does).

	Notes:		The following valid NodeAttributes must live in the attr-map:
					AttrLineWidth
					AttrStartCap
					AttrVariableWidth
					AttrJoinType

	Errors:		ERROR2 with FALSE if anything goes wrong, or our inputs are NULL or invalid.

	See also:	Diccon's GetProcessedPath() higher up this file.

********************************************************************************************/
Path* PathProcessorStroke::GetProcessedPath(Path* pPath, CCAttrMap* pAttrMap)
{
	ERROR2IF(pPath == NULL || pAttrMap == NULL, NULL,
			"PathProcessorStroke::GetProcessedPath; NULL input parameters!");

	INT32			LineWidth;
	LineCapType		LineCap;
	ValueFunction*	pValFunc;
	JointType		JoinStyle;

	if (!GetRequiredAttributes(pAttrMap, &LineWidth, &LineCap, &pValFunc, &JoinStyle))
		return NULL;

	if (LineWidth < 1)
		return NULL;

	// Create a path to hold our stroking exploits!
	// We'll prob'ly be generating loads of data, so be generous with our path's
	// initialisation, so it's not continually reallocating memory as it runs out.
	Path* pStroke = new Path;
	if (pStroke == NULL)
		return NULL;
	if (!pStroke->Initialise(128, 128))
	{
		delete pStroke;
		return NULL;
	}

	// if it's a constant-width stroke, ask GDraw to stroke it - it's
	// quicker and if it goes wrong, it's Gavin's fault (only joking ;o)
	if (pValFunc == NULL || IS_A(pValFunc, ValueFunctionConstant))
	{
		pPath->StrokePathToPath(LineWidth, LineCap, JoinStyle, NULL, pStroke);
		pStroke->IsFilled	= TRUE;
		pStroke->IsStroked	= TRUE;
		return pStroke;
	}

	// ok, GDraw didn't deal with it, so let's get down to business.

	// Generate the set of trapezoids for the dest path.
	ProcessPathToTrapezoids GenerateTraps(64);
	TrapsList *pTrapezoids = new TrapsList;
	BOOL	ok = (pTrapezoids != NULL);
	if (ok)	ok = GenerateTraps.Init(pPath, pTrapezoids);
	if (ok)
	{
		// Flags are: Flatten, QuantiseLines, QuantiseAll
		ProcessFlags PFlags(TRUE, FALSE, FALSE);
		ok = GenerateTraps.Process(PFlags, TrapTravel_Parametric, JoinStyle);
	}

	// Stroke the trapezoids into a variable-width path.
	if (ok)
	{
		PathStroker Stroker(pValFunc, LineWidth, LineCap);
		ok = Stroker.StrokeSmoothPath(pTrapezoids, pStroke);
	}

	// tidy up temporary info.
	if (pTrapezoids != NULL)
	{
		delete pTrapezoids;
		pTrapezoids = NULL;
	}

	// tidy up the path and pass it out.
	if (pStroke != NULL)
	{
		if (ok)
		{
			if (!pStroke->IsClosed())
				pStroke->TryToClose();
		}
		else
		{
			delete pStroke;
			pStroke = NULL;
		}
	}

	return pStroke;
}



/********************************************************************************************

>	BOOL PathProcessorStroke::GetRequiredAttributes(CCAttrMap* pAttrMap,
													INT32* pLineWidth,
													LineCapType* pLineCap,
													ValueFunction** ppValFunc,
													JointType* pJoinStyle)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/2000

	Inputs:		pAttrMap	ptr to the attr-map to search.
				pLineWidth	line width ptr to fill.
				pLineCap	end-cap type ptr to fill.
				ppValFunc	variable line-width function ptr to fill.
				pJoinStyle	join style ptr to fill.

	Outputs:	If successful, the supplied pointers are filled with the appropriate values
				from the attribute map, otherwise they are filled with default values.

	Returns:	TRUE if successful,
				FALSE otherwise.

	Purpose:	Extracts the following values, required for path stroking,
				from the supplied attribute map:
					Line width.
					Line start-cap - eg square or round end.
					Pointer to any applied width function - may be NULL.
					Join style - eg bevelled, mitred, round.

	Notes:		If this function fails, *DO NOT* use the output values! They are only there
				in case you screw up and *do* decide to use them, in which case they should
				hopefully not bring Camelot down around your ears!

	Errors:		An error message is output to TRACE if we were unsuccessful.
				I'm not putting in an ERROR2 or 3, as I think from a user view-point,
				a quiet failure is more graceful.

********************************************************************************************/
BOOL PathProcessorStroke::GetRequiredAttributes(CCAttrMap* pAttrMap,
												INT32* pLineWidth,
												LineCapType* pLineCap,
												ValueFunction** ppValFunc,
												JointType* pJoinStyle)
{
	AttrLineWidth*		pAttrWidth		= NULL;
	AttrStartCap*		pAttrCap		= NULL;
	AttrVariableWidth*	pAttrVarWidth	= NULL;
	AttrJoinType*		pAttrJoinType	= NULL;

	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrLineWidth),	(void*&)pAttrWidth);
	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStartCap),		(void*&)pAttrCap);
	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrVariableWidth),(void*&)pAttrVarWidth);
	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrJoinType),		(void*&)pAttrJoinType);

	if (pAttrWidth		== NULL || pAttrCap		== NULL ||
		pAttrVarWidth	== NULL || pAttrJoinType	== NULL)
	{
		TRACEALL( _T("PathProcessorStroke::GetProcessedPath; AttrMap doesn't contain the required attrs!") );
		*pLineWidth	= 0;
		*pLineCap	= LineCapButt;
		*ppValFunc	= NULL;
		*pJoinStyle = RoundJoin;
		return FALSE;
	}

	*pLineWidth	= pAttrWidth->Value.LineWidth;
	*pLineCap	= pAttrCap->Value.StartCap;
	*ppValFunc	= pAttrVarWidth->Value.GetWidthFunction();
	*pJoinStyle	= pAttrJoinType->Value.JoinType;

	return TRUE;
}



NodePath* PathProcessorStroke::GetSmoothProcessedPath(Path* pPath, Node* pParent)
{
	PORTNOTETRACE("other","PathProcessorStroke::GetSmoothProcessedPath - do nothing");
	return NULL;
#ifndef EXCLUDE_FROM_XARALX
	// we need to fake a renderregion as our helper functions need one,
	// luckily FormatRegion is on hand from the text stuff
	FormatRegion FakeRender;

	if (!FakeRender.Init((NodeRenderableInk*)pParent)) // init renders all applied attributes into the region
		return FALSE;
	
	// From here copied from ProcessPath:
	// --- Get the current line width, cap style, and join style from the render region
	// In case of failure, we initialise with suitable defaults
	INT32 LineWidth = 5000;

	// BLOCK
	{
		LineWidthAttribute *pWidthAttr = (LineWidthAttribute *) FakeRender.GetCurrentAttribute(ATTR_LINEWIDTH);
		if (pWidthAttr != NULL)
			LineWidth = pWidthAttr->LineWidth;
	}

	// --- Create a stroke outline by calling our helper function
	Path *pOutput = CreateSmoothVarWidthStroke(pPath, &FakeRender, LineWidth);
	if (pOutput == NULL)
		return FALSE;

	if (!pOutput->IsClosed())
		pOutput->TryToClose();

	// we need to make a new nodepath
	NodePath* pNewNode = new NodePath; 
		
	if (pNewNode == NULL)
	{
		delete pOutput;
		return FALSE;
	}

	// initialise the path
	if (!pNewNode->InkPath.Initialise(pOutput->GetNumCoords(), 128))
	{
		delete pOutput;
		delete pNewNode;
		return FALSE;
	}

	// Copy our path data into it
	//CALL_WITH_FAIL(!pNewNode->InkPath.CopyPathDataFrom(pOutput), pUndoOp, Success)
	if (!pNewNode->InkPath.CopyPathDataFrom(pOutput))
	{
		delete pOutput;
		delete pNewNode;
		return FALSE;
	}
	// don't need that path anymore
	delete pOutput;
	pOutput = NULL;

	return pNewNode;
#endif
}


Path *PathProcessorStroke::CreateSmoothVarWidthStroke(Path *pPath, RenderRegion *pRender, INT32 LineWidth,
												TrapsList *pTrapezoids)
{
	ERROR3IF(pPath == NULL || pRender == NULL, "Illegal NULL Params");
	ERROR3IF(!pPath->IsStroked, "PathProcessor expects to be given a stroked path");

	// --- Ignore infinitely thin strokes
	if (LineWidth < 1)
		return(NULL);

	// --- Create a new path to be rendered in place of the provided path
	// Note that I use a large allocation size so that reallocation need not be done
	// frequently, which also helps reduce memory fragmentation.
	Path *pOutput = new Path;
	if (pOutput == NULL)
		return(NULL);

	pOutput->Initialise(128, 128);

	// --- Get the current cap style from the render region
	// In case of failure, we initialise with suitable defaults
	LineCapType LineCap = LineCapButt;
	// BLOCK
	{
		StartCapAttribute *pCapAttr = (StartCapAttribute *) pRender->GetCurrentAttribute(ATTR_STARTCAP);
		if (pCapAttr != NULL)
			LineCap = pCapAttr->StartCap;
	}

	// --- Get the variable line width descriptor from the render region
	ValueFunction *pValFunc = NULL;
	// BLOCK
	{
		VariableWidthAttrValue *pVarWidthAttr = (VariableWidthAttrValue *) pRender->GetCurrentAttribute(ATTR_VARWIDTH);
		if (pVarWidthAttr != NULL)
			pValFunc = pVarWidthAttr->GetWidthFunction();

		// If it is a constant width stroke, we'll just get GDraw to stroke it for us,
		// as that code is optimised assembler, and need not worry about variable width,
		// and as a result is about 4 times faster!
		if (pValFunc == NULL || IS_A(pValFunc, ValueFunctionConstant))
		{
			// Get the current line join style from the render region
			JointType JoinStyle = RoundJoin;
			JoinTypeAttribute *pJoinAttr = (JoinTypeAttribute *) pRender->GetCurrentAttribute(ATTR_JOINTYPE);
			if (pJoinAttr != NULL)
				JoinStyle = pJoinAttr->JoinType;

			pPath->StrokePathToPath(LineWidth, LineCap, JoinStyle, NULL, pOutput);

			pOutput->IsFilled  = TRUE;
			pOutput->IsStroked = FALSE;
			return(pOutput);
		}
	}

	// --- If none were passed in, generate the set of trapezoids for the dest path
	BOOL MustDeleteTraps = FALSE;
	if (pTrapezoids == NULL)
	{
		MustDeleteTraps = TRUE;
		pTrapezoids = PrepareTrapsForStroke(pPath, pRender, LineWidth);
		if (pTrapezoids == NULL)
			return(NULL);
	}

	// --- Stroke the trapezoids into a variable-width path
	PathStroker Stroker(pValFunc, LineWidth, LineCap);
	
	// our new option allows us to only get the forward mapped path

	if (!Stroker.StrokeSmoothPath(pTrapezoids, pOutput))
	{
		if (MustDeleteTraps)
			delete pTrapezoids;
		return(NULL);
	}


	if (MustDeleteTraps)
		delete pTrapezoids;

	return(pOutput);
}
