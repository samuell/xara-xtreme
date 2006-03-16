// $Id: ppvecstr.cpp 662 2006-03-14 21:31:49Z alex $
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
// ppvecstr - Implemenbtation of rendering PathProcessor classes for Vector Path Stroking

#include "camtypes.h"

#include "ppvecstr.h"

#include "attrmgr.h"
#include "docview.h"
#include "document.h"
#include "fillattr.h"
#include "fixmem.h"
//#include "grndbmp.h"
#include "nodepath.h"
#include "paths.h"
#include "pathstrk.h"
#include "pathtrap.h"
#include "qualattr.h"
#include "rndrgn.h"
#include "spread.h"
#include "strkattr.h"
#include "valfunc.h"


/***********************************************************************************************

>	class VectorStrokeSubRenderContext : public SubRenderContext

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/97
	Purpose:	Used during rendering to store the render context of a repeating vector stroke
				if it has taken too long to render so far.

	Notes:		pOutputTraps will be auto-deleted on destruct. pValFunc will NOT be deleted.

***********************************************************************************************/

PORTNOTE("other","Removed VectorStrokeSubRenderContext - derived from SubRenderContext")
#ifndef EXCLUDE_FROM_XARALX
class VectorStrokeSubRenderContext : public SubRenderContext
{
CC_DECLARE_DYNCREATE(VectorStrokeSubRenderContext);
public:
	VectorStrokeSubRenderContext();
	~VectorStrokeSubRenderContext();

	TrapsList		*pOutputTraps;			// The output trapezoids we've generated for stroking
	ValueFunction	*pValFunc;				// The width function for the stroke
	JointType		JoinStyle;				// The stroke's join style
	INT32			LineWidth;				// The stroke's line width
	INT32			RepeatDist;				// The distance for each repeat (or 0 if non-repeating)
	UINT32			Index;					// The index of the next TrapEdgeList to be stroked (in pOutputTraps)
};

VectorStrokeSubRenderContext::VectorStrokeSubRenderContext()
{
	LineWidth	 = 5000;
	JoinStyle	 = RoundJoin;
	pValFunc	 = NULL;
	RepeatDist	 = 0;
	pOutputTraps = NULL;
	Index		 = 0;
}

VectorStrokeSubRenderContext::~VectorStrokeSubRenderContext()
{
	pValFunc = NULL;		// Set it to NULL because it's a public var

	if (pOutputTraps != NULL)
		delete pOutputTraps;
	pOutputTraps = NULL;	// Set it to NULL because it's a public var
}

CC_IMPLEMENT_DYNCREATE(VectorStrokeSubRenderContext, SubRenderContext);
#endif

CC_IMPLEMENT_DYNAMIC(PathProcessorStrokeVector, PathProcessorStroke);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW




/********************************************************************************************

>	virtual BOOL PathProcessorStrokeVector::WillChangeFillAndStrokeSeparately(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/2/97

	Returns:	TRUE

	Purpose:	Called by the RenderRegion to determine if this PathProcessorStrokeVector affects
				the "fill" and "stroke" portions of the path separately. (Generally
				speaking, only fill/stroke providers will cause these two different
				"bits" of a path to be rendered separately). This call is made BEFORE
				this Processor's ProcessPath function will be called.

				If the caller gets a TRUE back, the stroke and fill paths will be
				rendered separately.

	Notes:		Base class implementation returns FALSE. Derived Stroke and Fill
				processors (such as this one) override this method to return TRUE.

********************************************************************************************/

BOOL PathProcessorStrokeVector::WillChangeFillAndStrokeSeparately(void)
{
	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL PathProcessorStrokeVector::NeedsTransparency() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97

	Returns:	TRUE if this stroke type needs transparency in order to render

	Purpose:	Determine if this stroke type needs transparency in order to render.
				Vector strokes 

********************************************************************************************/

BOOL PathProcessorStrokeVector::NeedsTransparency() const
{
	StrokeDefinition *pStrokeDef = StrokeComponent::FindStroke(StrokeID);
	if (pStrokeDef == NULL)
	{
		return(FALSE);
	}

	return(pStrokeDef->NeedsTransparency());
}




/********************************************************************************************

>	virtual void PathProcessorStrokeVector::ProcessPath(Path *pPath,
														RenderRegion *pRender,
														PathShape ShapePath = PATHSHAPE_PATH);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/2/97

	Purpose:	Called by the RenderRegion to apply the path processing operation to 
				the given path.

				The PathProcessorStrokeVector class changes the stroking (only) of paths
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

void PathProcessorStrokeVector::ProcessPath(Path *pPath,
											RenderRegion *pRender,
											PathShape ShapePath)
{
	PORTNOTETRACE("other","PathProcessorStrokeVector::ProcessPath - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(pPath == NULL || pRender == NULL, "Illegal NULL Params");

	// Get the RenderRegion SubRenderContext and see if we're returning part-way through a background render
	VectorStrokeSubRenderContext *pSubRenderContext = (VectorStrokeSubRenderContext *)pRender->GetSubRenderState();
	if (pSubRenderContext != NULL && !IS_A(pSubRenderContext, VectorStrokeSubRenderContext))
	{
		// We can't use the sub render context, because it's not ours!
		pSubRenderContext = NULL;
	}

	// --- If we don't have a valid stroke definition, then get the base class to render
	// the stroke as a simple flat-filled stroke.
	StrokeDefinition *pStrokeDef = StrokeComponent::FindStroke(StrokeID);
	if (pStrokeDef == NULL)
	{
		PathProcessorStroke::ProcessPath(pPath, pRender);
		return;
	}

	// --- See if we have to create a new SubRenderContext, or if we can use the one passed in.
	// We always store all relevant variables in a SubRenderContext object so that we can easily
	// return control to the RenderRegion without having to copy lots of local values in/out.
	const BOOL CreateSubRenderContext = (pSubRenderContext == NULL);
	if (CreateSubRenderContext)
	{
		pSubRenderContext = new VectorStrokeSubRenderContext;
		if (pSubRenderContext == NULL)
		{
			pRender->DrawPath(pPath, this, ShapePath);
			return;
		}

		// --- If the provided path is not stroked, then we'll just pass it straight through
		// We also don't touch it if we're doing EOR rendering, or click hit detection
		if (!pPath->IsStroked || pRender->DrawingMode != DM_COPYPEN || pRender->IsHitDetect())
		{
			delete pSubRenderContext;
			pRender->DrawPath(pPath, this, ShapePath);
			return;
		}

		// --- If the quality is set low, or if the current stroke attribute is not our "parent"
		// attribute (so we're not the "current" stroker) then strokes are just rendered as centrelines
		// BLOCK
		{
			QualityAttribute *pQuality = (QualityAttribute *) pRender->GetCurrentAttribute(ATTR_QUALITY);
			StrokeTypeAttrValue *pTypeAttr = (StrokeTypeAttrValue *) pRender->GetCurrentAttribute(ATTR_STROKETYPE);

			if ((pQuality != NULL && pQuality->QualityValue.GetLineQuality() != Quality::FullLine) ||
				(pTypeAttr != NULL && pTypeAttr != GetParentAttr()))
			{
				delete pSubRenderContext;
				pRender->DrawPath(pPath, this, ShapePath);
				return;
			}
		}

		// --- We don't expect the input path to be stroked AND filled on entry
		ERROR3IF(pPath->IsFilled, "PathProcessor expected RenderRegion to handle IsFilled case");

		// --- Get the current line width & Join Style from the render region
		// BLOCK
		{
			LineWidthAttribute *pWidthAttr = (LineWidthAttribute *) pRender->GetCurrentAttribute(ATTR_LINEWIDTH);
			if (pWidthAttr != NULL)
				pSubRenderContext->LineWidth = pWidthAttr->LineWidth;

			JoinTypeAttribute *pJoinAttr = (JoinTypeAttribute *) pRender->GetCurrentAttribute(ATTR_JOINTYPE);
			if (pJoinAttr != NULL)
				pSubRenderContext->JoinStyle = pJoinAttr->JoinType;
		}
	}

	// --- Create a new path to be rendered in place of the provided path
	// Note that I use a large allocation size so that reallocation need not be done
	// frequently, which also helps reduce memory fragmentation.
	Path *pOutput = new Path;
	if (pOutput == NULL)
	{
		if (!pRender->IsSubRenderStateLocked())
			pRender->SetSubRenderState(NULL);
		delete pSubRenderContext;
		pRender->DrawPath(pPath, this, ShapePath);
		return;
	}

	pOutput->Initialise(128, 128);


	// --- Find our Variable Width function
	if (CreateSubRenderContext)
	{
		// --- Get the variable line width descriptor from the render region
		VariableWidthAttrValue *pVarWidthAttr = (VariableWidthAttrValue *) pRender->GetCurrentAttribute(ATTR_VARWIDTH);
		if (pVarWidthAttr != NULL)
			pSubRenderContext->pValFunc = pVarWidthAttr->GetWidthFunction();
	}

	ValueFunction *pValFunc = pSubRenderContext->pValFunc;

	// If we couldn't find a proper value function, then we'll default to constant-width.
	// We keep a static Constant function around always, because it's thread-safe and because
	// that saves us the overhead of creating and deleting it on the stack each time we're called
	static ValueFunctionConstant Constant(1.0);
	if (pValFunc == NULL)
		pValFunc = &Constant;


	// --- Find our brush clipart tree
	Node *pBrush = pStrokeDef->GetStrokeTree();
	ERROR3IF(!IS_A(pBrush, Spread), "Brush does not start with a Spread Node!");
	DocRect BrushBounds = ((Spread *)pBrush)->GetBoundingRect();

	// Get a PathStroker to map paths onto the destination stroke with
	PathStrokerVector Stroker(pValFunc, pSubRenderContext->LineWidth,
								LineCapButt, &BrushBounds);

	// Work out if this is a repeating stroke, and if so, how often it repeats
	if (CreateSubRenderContext)
	{
		if (pStrokeDef->IsRepeating())
		{
			// The repeat distance is calculated as a number of millipoints per repeat of the
			// stroke, such that it retains the correct aspect ratio. That is, the ratio of
			// brush width to height is the same as RepeatDist to LineWidth
			pSubRenderContext->RepeatDist = (INT32) (Stroker.GetScaleFactor() * (double)BrushBounds.Width());
			if (pSubRenderContext->RepeatDist < 1000)		// I absolutely refuse to repeat it more than
				pSubRenderContext->RepeatDist = 1000;		// once every 1pt, as this is plenty small enough

			// Suss the path length out
			ProcessLength GenerateLength(100);
			double Length = 0;
			BOOL ok = GenerateLength.PathLength(pPath, &Length);

			// Ask the stroke def for its number of brush repeats - 0 means work it out
			INT32 NumberOfRepeats = pStrokeDef->NumRepeats();
			if(NumberOfRepeats == 0 && pSubRenderContext->RepeatDist > 0)
			{
				// Work out the optimal number of repeats along the path
				NumberOfRepeats = (INT32)(floor((Length/pSubRenderContext->RepeatDist) + 0.5));
			}

			// Don't got dividing by zero now...			
			if(NumberOfRepeats <= 0)
				NumberOfRepeats = 1;
		
			// Alter the repeat distance to accomodate this number of repeats
			pSubRenderContext->RepeatDist = (INT32)(Length / (double)NumberOfRepeats);
		}

		// Generate the set of trapezoids for the dest path
		ProcessPathToTrapezoids GenerateTraps(100);
		pSubRenderContext->pOutputTraps = new TrapsList(pSubRenderContext->RepeatDist);

		BOOL Failed = (pSubRenderContext->pOutputTraps == NULL);
		if (!Failed && !GenerateTraps.Init(pPath, pSubRenderContext->pOutputTraps))
			Failed = TRUE;

		ProcessFlags PFlags(TRUE, FALSE, FALSE);	// Flags are: Flatten, !QuantiseLines, !QuantiseAll
		if (!Failed && !GenerateTraps.Process(PFlags, TrapTravel_Parametric, pSubRenderContext->JoinStyle))
			Failed = TRUE;

		if (Failed)
		{
			pRender->DrawPath(pPath, this, ShapePath);
			if (!pRender->IsSubRenderStateLocked())
				pRender->SetSubRenderState(NULL);
			delete pSubRenderContext;
			return;
		}
	}

	ERROR3IF(pSubRenderContext->pOutputTraps == NULL || pValFunc == NULL,
				"Vector stroke SubRenderContext was not properly uninitialised!");

	// --- Handle background rendering. We always store all critical information in a SubRenderContext.
	// If we have to break into rendering because of background rendering, we give the context to the RndRgn
	// to keep for next time. However, when we finish rendering everything, we need to clean up - we will
	// assume that we'll make it to the end, and chnage this variable if we get interrupted.
	BOOL DeleteSubRenderContext = TRUE;

	// Lock the sub-render context so that nobody "inside" the brush uses it
	const BOOL SRContextLocked = pRender->IsSubRenderStateLocked();
	if (!SRContextLocked)
		pRender->LockSubRenderState(TRUE);

	// --- Now "render" the brush clipart tree via our Stroker
	for ( ;
			pSubRenderContext->Index < pSubRenderContext->pOutputTraps->GetNumTraps() && DeleteSubRenderContext;
			pSubRenderContext->Index++)
	{
		// Find the trapezoid edge list for this pass
		TrapEdgeList *pEdgeList = pSubRenderContext->pOutputTraps->GetTrapEdgeList(pSubRenderContext->Index);
		if (pEdgeList->GetNumEdges() < 2)
		{
			ERROR3("No map traps when stroking! Subpath stripped\n");
			continue;
		}

		// And render away
		pRender->SaveContext();

		Node* pNode = pBrush->FindFirstForUnclippedInkRender(pRender);
		while (pNode)
		{
			// Prepare the stroker to stroke this sub-stroke
			Stroker.PrepareToStroke(pEdgeList);

			if (pNode->IsAnAttribute())
			{
				// If we are overriding the fill/transparency with the one applied to the stroke,
				// then we simply throw away all fill/transparency attributes as we render
				// (We do this on the fly rather than as we make the brush so that the user can
				// toggle this mode on and off at any time if they change their mind)
				BOOL RenderIt = TRUE;
				if ( (pStrokeDef->OverrideFill()  && ((NodeAttribute *)pNode)->IsAColourFill())	||
					 (pStrokeDef->OverrideFill()  && ((NodeAttribute *)pNode)->IsAStrokeColour()) ||
					 (pStrokeDef->OverrideTrans() && ((NodeAttribute *)pNode)->IsAStrokeTransp()) ||
					 (pStrokeDef->OverrideTrans() && ((NodeAttribute *)pNode)->IsATranspFill()) )
				{
					RenderIt = FALSE;
				}

				if (RenderIt)
				{
					// We must modify all attributes to lie in the destination stroke.
					// This includes fill/trans geometry endpoints, line widths, and
					// also possibly modifying transparency levels to allow a flat transparency
					// to be applied to the whole stroke.
					AttributeValue *pAttrValue = ((NodeAttribute *)pNode)->GetAttributeValue();
					AttributeValue *pNewValue  = pAttrValue->MouldIntoStroke(&Stroker, 1.0);
					//****!!!!TODO - Just above, we have the chance to handle transparency better
					// - we could at least scale all transparencies by passing a flat scale factor into
					// the MouldIntoStroke call.

					if (pNewValue != NULL)
						pNewValue->Render(pRender, TRUE);		// The RndRgn will delete this when it's done with
					else
						pNode->Render(pRender);					// No change, so render the original attribute
				}
			}
			else if (pNode->IsNodePath())
			{
				// Stroke the trapezoids into the output path
				pOutput->ClearPath();
				if (!Stroker.StrokePath(&((NodePath *)pNode)->InkPath, pOutput))
					break;

				pRender->SetWindingRule(NonZeroWinding);
				pRender->DrawPath(pOutput, this, ShapePath);
			}
//				else
//					TRACEUSER( "Jason", _T("\nBrush node %s not rendered\n"), pNode->GetRuntimeClass()->m_lpszClassName);

			pNode = pNode->FindNextForUnclippedInkRender(pRender);
		}

		pRender->RestoreContext();

		// --- Now check if we should break into rendering for background rendering purposes
		// If the Sub-render-context is locked, then we're inside a blend or something, and it's too dangerous
		// for us to store our sub-render state, so we have no choice but to render until we finish.
		// BLOCK
		if (!SRContextLocked && IS_A(pRender, GRenderDIB))
		{
			View *pView = pRender->GetRenderView();
			if (pView != NULL && !pRender->RenderTreeCanContinue())
			{
				// We have been interrupted by the background redraw system.
				// We will immediately exit, storing our SubRenderContext away into the
				// RenderRegion for the next time it calls us (see below).
				DeleteSubRenderContext = FALSE;

				// Drop through and let the loop condition handle exit
			}
		}
	}

	// If we locked the sub-render context, then we must restore it
	if (!SRContextLocked)
		pRender->LockSubRenderState(FALSE);

	// If we have finished rendering everything, then we vape our sub render context.
	// (We even check if we were interrupted just as we finished the final iteration)
	if (DeleteSubRenderContext || pSubRenderContext->Index >= pSubRenderContext->pOutputTraps->GetNumTraps())
	{
		if (!SRContextLocked)
			pRender->SetSubRenderState(NULL);
		delete pSubRenderContext;
		pSubRenderContext = NULL;
	}
	else
	{
		if (!SRContextLocked)
			pRender->SetSubRenderState(pSubRenderContext);
	}

	delete pOutput;
	pOutput = NULL;
#endif
}



/********************************************************************************************

>	virtual BOOL PathProcessorStrokeVector::IsDifferent(PathProcessorStroke *pOther);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/2/97

	Inputs:		pOther - The other PathProcessorStroke

	Returns:	TRUE if they're considered different, FALSE if they are equivalent

	Purpose:	Equality operator

	Notes:		The base class implementation compares the runtime classes of the
				2 objects to see if they are different classes. If they are the same
				class, it assumes they're considered equal - derived classes should
				override this behaviour if further comparisons are necessary.

********************************************************************************************/

BOOL PathProcessorStrokeVector::IsDifferent(PathProcessorStroke *pOther)
{
	ERROR3IF(pOther == NULL, "Illegal NULL param");

	if (GetRuntimeClass() != pOther->GetRuntimeClass())
		return(TRUE);

	// We're different if we use different stroke definitions
	return(StrokeID != ((PathProcessorStrokeVector *)pOther)->StrokeID);
}



/********************************************************************************************

>	virtual PathProcessorStroke *PathProcessorStrokeVector::Clone(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/2/97

	Returns:	NULL if we're out of memory, else
				a pointer to a clone (exact copy) of this object

	Purpose:	To copy PathProcessorStroke or derived-class object

********************************************************************************************/

PathProcessorStroke *PathProcessorStrokeVector::Clone(void)
{
	// Clone this object - this can be done by just creating a new one
	PathProcessorStrokeVector *pClone = new PathProcessorStrokeVector;

	// And copy the (base class) parent-attribute pointer so we know when our
	// parent attribute is "current" in the render region.
	if (pClone != NULL)
	{
		pClone->SetParentAttr(GetParentAttr());
		pClone->StrokeID = StrokeID;
	}

	return(pClone);
}



/********************************************************************************************

>	void PathProcessorStrokeVector::SetStrokeDefinition(StrokeHandle Handle)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/2/97

	Purpose:	Sets this path processor up to use the given vector stroke definition
				in all future rendering. 

	Notes:		If not set, or if set to StrokeHandle_NoStroke, the stroke processor
				will render its strokes as simple (flat filled) strokes by calling the
				base class ProcessPath method.

********************************************************************************************/

void PathProcessorStrokeVector::SetStrokeDefinition(StrokeHandle Handle)
{
	ERROR3IF(Handle != StrokeHandle_NoStroke && StrokeComponent::FindStroke == NULL,
				"Trying to set a Stroke PathProcessor to use a deleted stroke!");

	StrokeID = Handle;
}


