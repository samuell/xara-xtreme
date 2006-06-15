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
// */

// PathProcessor Brush implementation

#include "camtypes.h"
#include "ppbrush.h"
#include "brshcomp.h"
#include "pathproc.h"
//#include "doccoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docrect.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "attrmap.h"
#include "brushref.h"
#include "ndbldpth.h"
//#include "fixed16.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "matrix.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillval.h"  // for my traces, may remove  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "qualattr.h"
// Native file load/save includes
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfdefs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"
//#include "grndbrsh.h" // for real time brush drawing
//#include "rik.h"      // for the strings
#include "progress.h"
#include "mtrand.h"
#include "pen.h"
#include "brshbeca.h"
#include "lineattr.h"
//#include "colourix.h"
#include "cnamecol.h"
//#include "swfrndr.h"
#include "samplist.h"
#include "scanrr.h"
#include "valfunc.h"
#include "strkattr.h"
//#include "offscrn.h"
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "ophist.h"
#include "vectrndr.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(PathProcessorBrush, PathProcessor);
PORTNOTE("other","Removed BrushSubRenderContext - derived from SubRenderContext")
#ifndef EXCLUDE_FROM_XARALX
CC_IMPLEMENT_DYNCREATE(BrushSubRenderContext, SubRenderContext);
#endif

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

const double MAX_RANDOM_FACTOR = 2.0;
const double MIN_RANDOM_FACTOR = 1.0;
const double MIN_ROTATION_RANDOM_FACTOR = 1.0;
const double MAX_ROTATION_RANDOM_FACTOR = 1.0;
const UINT32 MAX_RANDOM_AMOUNT = 100;
const UINT32 MIN_RANDOM_AMOUNT = 0;

// Define this if you want to use Phil's new RenderBrushAtPoint function
// to fix accuracy/repeatability problems with brush rendering
#define NEW_RENDERBRUSHATPOINT

/********************************************************************************************

>	PathProcessorBrush::PathProcessorBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Purpose:	Constructor

********************************************************************************************/

PathProcessorBrush::PathProcessorBrush()
{
	m_AltValue = 0;
	m_BrushSpacing = 10000;
	m_bScaleToLineWidth = TRUE;
	m_LineWidth = -1;
	m_pParentAttr = NULL;

	m_BrushScaling			= 1.0;
	m_BrushScalingIncr		= 1.0;
	m_BrushScalingIncrConst = 0.0;
	m_BrushScalingMaxRand   = 0;
	m_BrushScalingRandSeed  = 0;
	m_ScalingMaxPressure	= 0;

	m_BrushSpacingIncrProp  = 1.0;
	m_BrushSpacingIncrConst = 0;
	m_BrushSpacingMaxRand   = 0;
	m_BrushSpacingRandSeed  = 5;
	m_SpacingMaxPressure    = 0;

	m_RotationMaxRand       = 0;
	m_RotationRandSeed      = 0;
#ifdef USE_MTRNG
	m_pSpacingRNG           = NULL;
	m_pScalingRNG			= NULL;
	m_pOffsetTypeRNG		= NULL;
	m_pOffsetValueRNG       = NULL;
	m_pRotationRNG          = NULL;
	m_pSaturationRNG        = NULL;
	m_pHueRNG				= NULL;
	m_pSequenceRNG			= NULL;
#endif
	m_PathOffsetIncrProp = 1.0;
	m_PathOffsetIncrConst = 0;

	m_SequenceType			  = SEQ_FORWARD;
	m_SequenceRandSeed        = 0;

	m_OffsetTypeRandSeed      = 0;
	m_bUseLocalFillColour = FALSE;
	m_bUseLocalTransp	  = FALSE;
	m_bUseNamedColours	  = TRUE;
	m_CurrentPressure.m_Pressure = MAXPRESSURE / 2;
	m_bCalculateRect  = FALSE;

	m_RotateAngle = 0.0;
	m_RotAngleIncrProp = 1.0;
	m_RotAngleIncrConst = 0;
	
	m_BrushSatMaxRand = 0;
	m_BrushSatRandSeed = 0;
	m_BrushHueMaxRand = 0;
	m_BrushHueRandSeed = 0;
	m_BrushHueIncrement = 1.0;
	m_BrushSatIncrement = 1.0;

	m_LastOffset = 0;	
	m_LastScaling = 10000;
	m_LastObject = 0;

	m_pObjectRendered = NULL;
	m_pLastScalingRendered = NULL;
	m_pLastTangent	= NULL;
	m_pLastRotation	= NULL;

	m_BrushTransparency = 100;
	m_TranspMaxPressure = 0;

	// we have to allocate our 'dummy' transparency attribute, which we must use in order to 
	// prevent transparency from one blendpath being 'carried over' to other blendpaths.
	m_pDefaultTransp = NULL;
	
	m_pDefaultTransp = new AttrFlatTranspFill;
	if (m_pDefaultTransp != NULL)
	{
		TranspFillAttribute* pVal = (TranspFillAttribute*)m_pDefaultTransp->GetAttributeValue();
		if (pVal != NULL)
			pVal->Transp = 0;
	}
}


/********************************************************************************************

>	PathProcessorBrush::~PathProcessorBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Purpose:	Destructor - basically this just wipes the caches. 
				

********************************************************************************************/

PathProcessorBrush::~PathProcessorBrush()
{

#ifdef USE_MTRNG
	if (m_pSpacingRNG != NULL)
		delete m_pSpacingRNG;
	if (m_pScalingRNG != NULL)
		delete m_pScalingRNG;
	if (m_pOffsetTypeRNG != NULL)
		delete m_pOffsetTypeRNG;
	if (m_pOffsetValueRNG != NULL)
		delete m_pOffsetValueRNG;
	if (m_pRotationRNG != NULL)
		delete m_pRotationRNG;
	if (m_pSaturationRNG != NULL)
		delete m_pSaturationRNG;
	if (m_pHueRNG != NULL)
		delete m_pHueRNG;
	if (m_pSequenceRNG != NULL)
		delete m_pSequenceRNG;
#endif

	if (m_pDefaultTransp != NULL)
		delete m_pDefaultTransp;
}

/********************************************************************************************

>	virtual BOOL PathProcessorBrush::WillChangeFillAndBrushSeparately(void)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	???? haven't decided yet

	Purpose:	Called by the RenderRegion to determine if this PathProcessorBrush affects
				the "fill" and "Brush" portions of the path separately. (Generally
				speaking, only fill/Brush providers will cause these two different
				"bits" of a path to be rendered separately). This call is made BEFORE
				this Processor's ProcessPath function will be called.

				If the caller gets a TRUE back, the Brush and fill paths will be
				rendered separately.

	Notes:		Actually this call serves no purpose in this derived class.

********************************************************************************************/

BOOL PathProcessorBrush::WillChangeFillAndBrushSeparately(void)
{
	return(TRUE);
}


/********************************************************************************************

>	virtual BOOL PathProcessorBrush::IsAPathProcessorBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	TRUE!

	Purpose:	Virtual identifier
********************************************************************************************/

BOOL PathProcessorBrush::IsAPathProcessorBrush()
{
	return TRUE;
}

/********************************************************************************************

>	virtual void PathProcessorBrush::ProcessPath(Path *pPath, RenderRegion *pRender);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Purpose:	Called by the RenderRegion to apply the path processing operation to 
				the given path.

				The PathProcessorBrush class changes the stroking (only) of paths
				passed into it.

	Notes:		* When rendering a path, always pass in your 'this' pointer to
				  RenderRegion::DrawPath, so that you don't End an infinite
				  recursion!

				* To stop rendering of the path, simply return without calling the RR

				* To render this path unchanged, simply call directly back to the RR:
					pRender->DrawPath(pPath, this);
				
********************************************************************************************/

void PathProcessorBrush::ProcessPath(Path *pPath, RenderRegion *pRender,
									PathShape ShapePath)
{
	ERROR3IF(pPath == NULL || pRender == NULL, "Illegal NULL Params");

	// We don't touch it if we're doing EOR rendering
	if (pRender->DrawingMode != DM_COPYPEN)
	{
		pRender->DrawPath(pPath, this);
		return;
	}


	// --- If the quality is set low enough, strokes are just rendered as centrelines
	// "low enough" is defined as the same point that Blends show their middle parts
	// BLOCK
	{
		QualityAttribute *pQuality = (QualityAttribute *) pRender->GetCurrentAttribute(ATTR_QUALITY);
		if (pQuality != NULL && pQuality->QualityValue.GetBlendQuality() != Quality::FullBlend)
		{
			pRender->DrawPath(pPath, this, ShapePath);
			return;
		}
	}


#ifdef NEWFASTBRUSHES
	GRenderRegionWrapper* pRendWrap = NULL;
	RenderRegion *pOrig = NULL;
	NodePath* pParentPath = NULL;

	if (pLinkedAttr)
	{
TRACEUSER( "Phil", _T("PathProcessorBrush::ProcessPath:  entered with pNewAttr"));
		if (pLinkedAttr->GetCachedBitmap ())
		{
			// we have a bitmap, then render that ....
			pLinkedAttr->GetCachedBitmap ()->Render(pRender);
TRACEUSER( "Phil", _T("PathProcessorBrush::ProcessPath:  rendered with bitmap\n"));
			return;
		}
		else
		{
			// we need to change this to be a parent of the correct type!
			pParentPath = (NodePath*) pLinkedAttr->GetBoundsParent ();

			if (pParentPath)
			{
				// we will try and generate a bitmap (cached render) of this path ....
				pRendWrap = GRenderRegionWrapper::GetAppropriateRenderWrapper ((GRenderRegion*)pRender, 1.0, pParentPath->GetBoundingRect (), 32, TRUE);
TRACEUSER( "Phil", _T("PathProcessorBrush::ProcessPath:  rendered with path\n"));
			}
			else
			{
				// since were here - somebody is probably drawing to the render region directly, so we need to
				// fake that the fact that we don't have a parent
				// we do this using the current clip rect of the supplied render region ...
				
				pRendWrap = GRenderRegionWrapper::GetAppropriateRenderWrapper ((GRenderRegion*)pRender, 1.0, pRender->GetClipRect (), 32, TRUE);
TRACEUSER( "Phil", _T("PathProcessorBrush::ProcessPath:  rendered with path\n"));
			}
			// else will allow processing as normal ....
		}
	}

	if (pRendWrap)
	{
		pOrig = pRender;
		pRender = (RenderRegion*) pRendWrap->GetRenderRegion();
	}
#endif

	// if we don't have a brush definition then just draw the path (our brush may have been deleted)
	BrushDefinition* pBrushDef = GetOurBrushDefinition();
	if (pBrushDef == NULL)
	{
		pRender->DrawPath(pPath, this);
		return;
	}

	Reset(); // sets all our members to the values they should hold at the start of the path

	// if we have no stroke colour then we will want to just draw the line as transparent and then leave.
	// If we don't then filled shapes lose their fill.  Note that we must get the strokecolour attribute
	// before we SaveContext.
	StrokeColourAttribute* pStroke = (StrokeColourAttribute*)pRender->GetCurrentAttribute(ATTR_STROKECOLOUR);
	BOOL NoStroke = FALSE;
	
	if (pStroke != NULL)
	{
		m_CurrentStrokeCol = pStroke->Colour;
		NoStroke = pStroke->Colour.IsTransparent();
	}
	else
	{
		NoStroke = TRUE;
	}

	// get the line width , if we don't exit due to no stroke colour then we will scale to this value
	LineWidthAttribute* pLine = (LineWidthAttribute*)pRender->GetCurrentAttribute(ATTR_LINEWIDTH);
	
	// first things: we need to draw the path in order to get any fill effect we might have, however
	// we don't actually want to see the path so set a transparent line colour
//	ColourFillAttribute* pFill = (ColourFillAttribute*)pRender->GetCurrentAttribute(ATTR_FILLGEOMETRY);

	pRender->SaveContext();

	pRender->SetLineColour(COLOUR_TRANS);	
//	pRender->SetLineWidth(500);
	pRender->DrawPath(pPath, this);	

	if (NoStroke)
	{
		pRender->RestoreContext();
		return;
	}
	
	// scale to our line width
	UINT32 LineWidth = pLine->LineWidth;

	// get the brush definition to make copies of its data for rendering
	if (!pBrushDef->StartRender())
	{
		TRACEUSER( "Diccon", _T("Unable to render brush%d\n"));
		pRender->DrawPath(pPath, this);
		pRender->RestoreContext();
		return;
	}

	UINT32 NumBrushObjects = pBrushDef->GetNumBrushObjects();

	// prepares caches to store transformation data
	if (!PrepareForRenderingLoop(NumBrushObjects))
	{
		pRender->RestoreContext();
		return;
	}

	// get some variables to keep track of where we are on the path
	double dPathLength = pPath->GetPathLength(750/2);
	double dOrigPathLength = dPathLength;
	MILLIPOINT PathLength = (MILLIPOINT)dPathLength;
	double PathProportion = 0.0;

	// this records the distance along the path at which we want to find a point
	MILLIPOINT DistanceToPoint = 10;
	
	// declare some locals that we need in the loop
	MILLIPOINT Spacing = m_BrushSpacing;
	
	CPathPointInfo PathPoint;
	double Angle;
	DocCoord Point; 
	BOOL ok = FALSE;
	BOOL Found = TRUE;
	UINT32 Count = 0;

//TRACEUSER( "Phil", _T("Start Process Path\n"));

	UINT32 BrushObjectToRender = m_LastObject;

	
	// we will need to work out if each object needs to be rendered by testing its bounds
	// against the bounds of our render region.  This information will be used when calling
	// OnjectIsWithinRect
	DocRect RenderRect = pRender->GetClipRect();
	DocRect InkRect = pBrushDef->GetLargestBoundingBox();
	
	MILLIPOINT InkWidth = InkRect.Width() / 2;
	MILLIPOINT InkHeight = InkRect.Height() / 2;
	
	// To get our pressure data we will use either a cache that is held by our parent attribute
	// OR use a value function if a VariableWidthAttribute has been rendered into the render region 

	// get our pressure data
	ValueFunction* pValFunc = NULL;
	CDistanceSampler* pPressure = GetPressureCacheAndInitialise(pRender, &pValFunc);
	CSampleItem TheItem; // we need this in the loop
	
	// scale to the right line width
	if (LineWidth < 500)
		LineWidth = 500;

	ScaleToValue(LineWidth, !m_bValidPressure);

	// render a dummy variable width attribute
	VariableWidthAttrValue* pVarWidth = new VariableWidthAttrValue(NULL);
	if (pVarWidth)
		pRender->SetVariableWidth(pVarWidth, TRUE);

	// start a tiny ways in so that our first point doesn't necessarily have
	// a tangent of zero
	DistanceToPoint = 10;
	
	m_bRenderAllAttributes = TRUE;

	DocCoord TestPoint;
	
	if (pPath->IsClosed())
	{
		// if the path is closed then hopefully we will have adjusted the spacing so that the
		// path length fits an integer number of objects.  However we do not want to render
		// two objects on the start point so reduce the path length we actually use
		double ActualSpacing = m_BrushScaling * (double)m_BrushSpacing;

		PathLength -= (MILLIPOINT)(ActualSpacing * 0.2); // we need some margin for error
		dPathLength = (double)PathLength;

		AdjustSpacingForClosedPath(pPath, dOrigPathLength);
	}


	m_LastScalingRendered = m_BrushScaling;
	m_CurrentScaling = GetNextScaling(Count);
	// bear in mind we've now got two scaling variables at work here.  m_CurrentScaling records the actual scaling
	// that will be performed on our ink objects.  m_LastScaling is the scaling value compared to the starting size
	// of the objects.  e.g. if m_CurrentScaling = 1.5 then that means the objects will be scaled by 1.5 prior to
	// being rendered.  If m_LastScaling = 1.5 it means that the objects are currently 1.5 times as big as they
	// originally were.  I should probably rename these variables at some point.

	Spacing = GetNextSpacing();
	double TestAngle = 0;
	m_LastScalingRendered = m_CurrentScaling;

	// If we are printing or exporting then don't do the clip test
	BOOL bScan = pRender->IsKindOf(CC_RUNTIME_CLASS(ScanningRenderRegion))
				|| pRender->IsKindOf(CC_RUNTIME_CLASS(VectorFileRenderRegion));

	BOOL bContinue = TRUE;

//::timeBeginPeriod(0);
//DWORD timeStart = ::timeGetTime();
	while ((DistanceToPoint < PathLength) && bContinue)
	{
	//	TRACEUSER( "Diccon", _T("Count %d\n"), Count);
		// try and get the point 
		ok = GetPointAtDistance(pPath, DistanceToPoint, &Point, &Angle);

		// get the offset type (only important for random offset types)
		m_LastOffsetType = GetNextOffsetType();
	
		if (ok)  
		{
		
		//	TRACEUSER( "Diccon", _T("Scaling = %f\n"), m_CurrentScaling);
			TestPoint = AdjustPositionForOffset(Point, Angle, m_BrushScaling, GetNextOffset(), m_LastOffsetType);
			GetNextAngle();
			GetNextHueMultiplier();
			GetNextSaturationMultiplier();

			// if we're rotated then rotate the rect
			TestAngle = m_LastAngle;
			if (m_bRotate)
				TestAngle += (Angle / PI * 180);  //the angle we get from GetPoint is in radians but we want degrees

			// is it in our render region?
//TRACEUSER( "Phil", _T("Test ObjectIsWithinRect %d at %d, %d. Angle = %f\n"), Count, Point.x, Point.y, Angle);
			if (bScan || ObjectIsWithinRect(RenderRect, TestPoint, InkWidth, InkHeight,  m_LastScaling, TestAngle))
			{
//TRACEUSER( "Phil", _T("Rendering Object %d at %d, %d. Angle = %f\n"), Count, Point.x, Point.y, Angle);
				if (m_CurrentScaling > 0)
					RenderBrushAtPoint(Point, Angle, BrushObjectToRender, Count,  pRender);

				// if we've only got one object then don't keep rendering attributes that we don't have to 
				if (NumBrushObjects == 1)
					m_bRenderAllAttributes = FALSE;

			}
		//	else
			//	TRACEUSER( "Diccon", _T("NOT Rendering Object %d at %d, %d.  Angle = %f\n"), Count, Point.x, Point.y, Angle);

			Count++;
			// get the index of the next object.
			if (NumBrushObjects > 1)
			{
				BrushObjectToRender = GetNextInkObject(BrushObjectToRender, NumBrushObjects);
			}
			// if the flag for this object is false then we have not rendered it yet so we need to 
			// get the absolute scaling value.  If we have already rendered it then we want a scaling
			// value relative to its last scaling.
			DecideWhetherToUseActualScaling(BrushObjectToRender);
			
		}
	
		// do we really want to automatically scale the spacing??? keep it for now

#ifdef SCALESPACING		
		DistanceToPoint += (MILLIPOINT)(Spacing * m_LastScaling);
		
		if (pPressure)
			pPressure->SetSampleRateFromSpacing((INT32)(Spacing * m_CurrentScaling));
#else
			DistanceToPoint += (MILLIPOINT)(Spacing * m_BrushScaling);
#endif
		// get the next distance
		Spacing = GetNextSpacing();

		if (pValFunc != NULL)
		{
			PathProportion = (double)DistanceToPoint / dPathLength;
			m_CurrentPressure.m_Pressure = GetNextSampleItem(NULL, pValFunc, PathProportion).m_Pressure;
		
			// our pressure values can sometimes be erroneous
			if (m_CurrentPressure.m_Pressure > MAXPRESSURE)
				m_CurrentPressure.m_Pressure = MAXPRESSURE;
		}

		if (pPressure != NULL)
		{
			double pLength = dOrigPathLength;		// Don't keep callnig pPath->GetPathLength(); It's complex!
			double pDataLength = pPressure->GetDistanceRep (), count = pPressure->GetCount ();
			double dataRatio = 1.0;

			if (pDataLength < pLength)
			{
				dataRatio = pDataLength / pLength;
			}
			else
			{
				dataRatio = pLength / pDataLength;
			}

			double offset = 1.0;
				
			if (DistanceToPoint < pLength)
			{
				offset = (DistanceToPoint / pLength) * dataRatio * count;
			}
			else
			{
				offset = (pLength / DistanceToPoint) * dataRatio * count;
			}
			
			if (pPressure->GetAtPos (offset, &TheItem))//GetNext(&TheItem))
			{
				if (TheItem.m_Pressure < MAXPRESSURE)
					m_CurrentPressure.m_Pressure = TheItem.m_Pressure;
				else
					m_CurrentPressure.m_Pressure = MAXPRESSURE;
			//	TRACEUSER( "Diccon", _T("Pressure = %d\n"), TheItem.m_Pressure);
			}
		}

		// to get the next scaling we need to set the last scaling member to the value of the
		// object we are about to render next
		m_LastScalingRendered = m_pLastScalingRendered[BrushObjectToRender];
		m_CurrentScaling = GetNextScaling(Count);

		//TRACEUSER( "Diccon", _T("Spacing = %d, Scaling = %f\n"), Spacing, m_CurrentScaling);
		// this member keeps track of which side we should offset on if we have
		// an alternating offset
		if (m_AltValue == 0)
			m_AltValue = 1;
		else
			m_AltValue = 0;
		if (Found == FALSE)
			TRACEUSER( "Diccon", _T("Failed to find point at %d\n"), DistanceToPoint);

		// see if we get more rendering time
		bContinue = TRUE; //pView->CanContinue(pRender); 
	}
//TRACEUSER( "Phil", _T("ProcessPath Duration = %d\n"), ::timeGetTime()-timeStart);
//::timeEndPeriod(0);
	
	CleanUpAfterRender();

	TRACEUSER( "Diccon", _T("Finished Process Path, rendered %d objects\n"), Count);
	
	pRender->RestoreContext();

	pBrushDef->StopRender();

#ifdef NEWFASTBRUSHES
	if (pRendWrap)
	{
		pRender = pOrig;

		KernelBitmap* kBmp = pRendWrap->GetKernelBitmap();
		pRendWrap->RestorePreviousRendererState();

		NodePath* pParentPath = (NodePath*) pLinkedAttr->GetBoundsParent ();

		NodeBitmap* DummyBmp = new NodeBitmap();

		if (DummyBmp)
		{		
			DummyBmp->SetUpPath();

			if (pParentPath)
			{
				DummyBmp->CreateShape(pParentPath->GetBoundingRect ());
			}
			else
			{
				// CGS:  since were here - somebody is probably drawing to the render region directly, so we need to
				// fake that the fact that we don't have a parent
				// we do this using the current clip rect of the supplied render region ...
				// NOTE:  

				DummyBmp->CreateShape(pRender->GetClipRect ());
			}

			DummyBmp->GetBitmapRef()->SetBitmap(kBmp);
			pLinkedAttr->SetCachedBitmap (DummyBmp);
			DummyBmp->Render (pRender);
		}
	}
#endif
}


/********************************************************************************************

>	void PathProcessorBrush::RenderBrushAtPoint(DocCoord Point, double Tangent, UINT32 BrushIndex, UINT32 Counter
												RenderRegion* pRender, BOOL RealtimeBrush) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Rewritten:	Phil 13/01/2004
	Inputs:     Point - co-ordinate where we wish to render the brush object
				Angle - the tangent at that point
				BrushIndex - index into the brushref array of the brush definition
				Counter - the number of objects drawn so far
				pBecomeA - deals with things if we are doing a DoBecomeA, can be null
				RealtimeBrush - are we drawing a brush in real time (must supply a GRenderBrush region
				if so) defaults to FALSE.
				pRender - the render region to render on to
	Outputs:    -
	Returns:	- 
	Purpose:	Multipurpose !
				Renders a brush object at the point specified if we supply a render region, or
				calculates the bounding box of the brush at this point and adds it to our
				cached rect, or makes calls PassBack for our BecomeA object.

				This function is called from:
				ProcessPath - for normal rendering
				CalculateBoundingBox - 
				DoBecomeA
				OpDrawBrush::RenderStepsForTime 

				Essentially what this does is:

				For each Brushref
					For Each Path and AttrMap belonging to the brushref
						Work out render point, based on Coord provided plus offset
						Scale 
						Rotate
						Rotate for tangent
						either render, or do passback, depending on who's calling
						
						Reverse the transformations, however now that we are using 
						copies rather than the original paths etc. we can probably
						ditch these.
	Rewrite Notes:
				The matrix handling in the original version of this routine was horribly
				inefficient and resulted in compounded maths errors that caused brush facets
				to "wobble" unpredictably.


********************************************************************************************/
#ifdef NEW_RENDERBRUSHATPOINT
void PathProcessorBrush::RenderBrushAtPoint(DocCoord Point,
											double Tangent,
											UINT32 BrushIndex,
											UINT32 Counter,
											RenderRegion* pRender,
											HandleBrushBecomeA* pBecomeA, 
											BOOL RealtimeBrush,
											BrushDefinition* pBrushDefinition)
{
	BrushDefinition* pOurBrushDef = NULL;
	if (!pBrushDefinition)
	{
		pOurBrushDef = GetOurBrushDefinition();
	}
	else
	{
		pOurBrushDef = pBrushDefinition;
	}

	if (pOurBrushDef == NULL)
		return;
	
	// lets declare some locals here that we need in the loop later
	Trans2DMatrix	Trans;
	BrushRef*		pBrushRef; // the brushref for this brush object
	CCAttrMap*		pAttrMap = NULL;  // the attrmap that holds the attributes for each brush
	Path*			pRenderPath = NULL;	// the path that we will render
	BlendPath*		pBlendPath = NULL;	// the source path that we have copied the render path from
	DocCoord*		pRenderCoords = NULL; // pointer to the coords of pRenderPath
	INT32			NumRenderCoords = 0; // num coords in pRenderPath
	DocCoord		RenderPoint; // the coord to render at
	MILLIPOINT		OffsetVal = 0;
	DocCoord		TransformCentre;

	BrushRef::iterator ListPosition;
	BlendPathOffset* pOffset = NULL; // pointer to the x,y offset of each individual object within
									 // a brush made from a group
	
	// quickly ensure that we will draw a brush object
	if (BrushIndex >= pOurBrushDef->GetNumBrushObjects())
		BrushIndex = 0;
	
	// get the scaling here, this differs slightly according to whether this is a realtime
	// or a normal render (in the latter GetNextScaling was called in ProcessPath)
	double Scaling = 1.0;
	double ExtraAngle = 0;
	if (!RealtimeBrush)
	{
		Scaling = m_CurrentScaling;
		ExtraAngle = m_LastAngle;

		OffsetVal = m_LastOffset;
	}
	else
	{
		ExtraAngle = GetNextAngle();
		Scaling = GetNextScaling(Counter);
		if (Scaling <= 0)
			return;
		
		GetNextHueMultiplier();
		GetNextSaturationMultiplier();

		OffsetVal = GetNextOffset();
	}
	
	double TranspAdjust = GetNextTransparencyAdjust();
	double PressureTranspAdjust = GetNextPressureTranspAdjust();

	pBrushRef = pOurBrushDef->GetBrushRef(BrushIndex);
	
	if (pBrushRef == NULL)
	{
		ERROR3("No brushref in RenderBrushAtPoint");
		return;
	}

	UINT32 NumBlendPaths = pBrushRef->GetNumBlendPaths();
	
	// if we are dealing with multiple blendpaths then
	// a) we want to render all attributes always and
	// b) we want to do our tangential rotation around the point where the centre 
	// of the group would be
//	BOOL SpecifyRotateCoord = FALSE;
	
	if (NumBlendPaths > 1)
	{
		m_bRenderAllAttributes = TRUE;
	}

	if (pBrushRef != NULL)
	{
		pOffset = pBrushRef->GetFirstOffset(&ListPosition);
		pBlendPath = pBrushRef->GetFirstBlendPath();
		pRenderPath = pBrushRef->GetFirstCopyPath();

		DocRect ChangedRect = DocRect(0,0,0,0);
	
		// if we're doing a passback/convert to shapes then tell it how many blendpaths we have
		if (pBecomeA != NULL)
			pBecomeA->SetNumBlendPaths(NumBlendPaths);

		UINT32 ObjectCounter = 0;
		while (pRenderPath != NULL)
		{
			DocRect BRect;
			Matrix transform;		// Identity

			// get data that we need for transformations
			pRenderCoords = pRenderPath->GetCoordArray();
			NumRenderCoords = pRenderPath->GetNumCoords();

			// Get the attribute map associated with the current brush path (RenderPath)
			pAttrMap = pBlendPath->GetCopyAttributes();
			if (pAttrMap == NULL)
			{
				ERROR3("No applied attributes");
				return;
			}

			ObjectCounter++;

			//--------------------------------------------------------------
			// first work out where we are going to translate to
			RenderPoint = AdjustPositionForOffset(Point, Tangent, m_BrushScaling, OffsetVal, m_LastOffsetType);
			
			// find out where the blendpath is at the moment
			BRect = pRenderPath->GetBoundingRect(); //pBlendPath->m_pPath->GetBoundingRect();
			DocCoord CurrentPoint = BRect.Centre();
		
			// Take relative position amongst other paths in this brush definition into account
			if (pOffset != NULL)
				CurrentPoint = AdjustPositionForBlendPathOffset(CurrentPoint, pOffset, -1.0, Tangent);	
			
			//--------------------------------------------------------------
			// translate brush down to origin for scale and rotate transforms
			transform.translate(-CurrentPoint.x, -CurrentPoint.y );
	
			//--------------------------------------------------------------
			// work out the scaling matrix
			if (Scaling != 1.0)
			{
				// get the scaling that we are going to use
				FIXED16 NewScale(Scaling);
				transform *= Matrix(NewScale, NewScale);
			
				Trans.SetTransform(transform);
				Trans.TransLines = TRUE;
				
				// if we're going to end up scaling the line width to zero then set a transparent stroke colour
				if (pAttrMap->WillScaleLineWidthToZero(Trans))
				{
					AttrStrokeColour* pOldStrokeCol = NULL;
					pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStrokeColour), (void*&)pOldStrokeCol );
					if (pOldStrokeCol)
					{
						// set the transparent colour
						pOldStrokeCol->Value.Colour = DocColour(COLOUR_TRANS);
					}
					else
					{
						AttrStrokeColour* pNewStrokeCol = new AttrStrokeColour;
						pNewStrokeCol->Value.Colour = DocColour(COLOUR_TRANS);
					}
				}
			}

			//--------------------------------------------------------------
			// do tangential rotation 
			if (m_bRotate)
			{
				// note that if we have a multiple blendpath situation then we want to rotate
				// around what would be the centre point of the group.
				double Tang = Tangent / PI * 180;
				transform *= Matrix(Tang);
			}
		
			//--------------------------------------------------------------
			// rotate if we have to
			if (m_LastAngle != 0 || m_RotationMaxRand != 0)
			{
				double RotAngle = 0;
				RotAngle += m_LastAngle;
				transform *= Matrix(RotAngle);
			}
		
			//--------------------------------------------------------------
			// Move into correct position on line...
			transform.translate(RenderPoint.x, RenderPoint.y);


			//--------------------------------------------------------------
			//--------------------------------------------------------------
			// Transform the path and attributes
			Trans.SetTransform(transform);
			if (Scaling != 1.0)
				Trans.TransLines = TRUE;

			Trans.Transform(pRenderCoords, NumRenderCoords);
		
			if (m_bTile)
				pAttrMap->TransformForBrush(Trans);
			else
				pAttrMap->Transform(Trans);

			// scale the bounding box
			transform.TransformBounds(&BRect);
			//--------------------------------------------------------------
			//--------------------------------------------------------------


			//--------------------------------------------------------------
			// we now need a colour replacer, in case we are overriding colours
			CHSVChanger ColChanger;
			ColChanger.SetHueMultiplier(m_LastHueMultiplier);
			ColChanger.SetSaturationMultiplier(m_LastSatMultiplier);
			
			// we also want a transparency replacer
			CTransparencyReplacer TranspRep;
			TranspRep.SetAdjustmentValue(TranspAdjust);
			TranspRep.SetInvertedAdjuster(PressureTranspAdjust);
		
			//--------------------------------------------------------------
			// Finally lets draw it
			if (pRender != NULL)
			{
				RenderAttributes(pRender, pAttrMap, &ColChanger, &TranspRep);
				pRender->DrawPath(pRenderPath, this);
			}

			//--------------------------------------------------------------
			// if we're doing a passback/convert to shapes then do it here
			if (pBecomeA != NULL)
			{
				AdjustFillAttributes(pAttrMap, &ColChanger, &TranspRep);
				pBecomeA->Passback(pRenderPath, pAttrMap, ObjectCounter);
			}
		
			// we must restore any colours we replaced after drawing the path
			if (ColChanger.IsInitialised())
				ColChanger.RestoreColours();
			if (TranspRep.IsInitialised())
				TranspRep.RestoreTransparency();
			
			// get true bounding rect here, including the linewidth	
			AttrLineWidth* pLineWidth = NULL;
			pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrLineWidth), (void*&)pLineWidth );
			INT32 LineWidth = 1500;
			if (pLineWidth != NULL)
			{
				LineWidth = pLineWidth->Value.LineWidth;
				if (LineWidth <= 500)
					LineWidth = 1500;
			}

			pRenderPath->GetTrueBoundingRect(&BRect, LineWidth, NULL);
			
			//--------------------------------------------------------------
			// if we have not already retrieved the last item then get it here
			if (ListPosition!=pBrushRef->GetEndPosition())
				pOffset = pBrushRef->GetNextOffset(&ListPosition);
		
			//--------------------------------------------------------------
			// we need to know the on screen area that has changed
			if (ChangedRect.IsEmpty())
				ChangedRect = BRect;
			else
				ChangedRect = ChangedRect.Union(BRect);
			
			// if we are calculating the bounding rect then do it here using the previous calculation
			if (m_bCalculateRect)
			{
				if (m_CachedRect.IsEmpty())
					m_CachedRect = ChangedRect;
				else
					m_CachedRect = m_CachedRect.Union(ChangedRect);
			}
			
			//--------------------------------------------------------------
			// reverse the transformation of the paths and the attributes
			// in the brush copy by recpoying from the original BlendPath
			//
			// Reset this BrushRef Copy back to its original state before we transformed it all
			// Copy recopying the original data - NOT by transforming back to some equivalent state!
			pBlendPath->UpdateCopyPathAndAttributes();

			//--------------------------------------------------------------
			// Get next path and go round again
			pRenderPath = pBrushRef->GetNextCopyPath();
			pBlendPath = pBrushRef->GetNextBlendPath(pBlendPath);

		} // end while pBlendPath

		// if we are drawing the brush in realtime then set the changed bounding box
		if (RealtimeBrush == TRUE)
		{
			// oh no, another awful hack
//			INT32 Height = ChangedRect.Height();
//			INT32 Width = ChangedRect.Width();
	
PORTNOTE("other","Removed GRenderBrush usage")
// NB nested
#ifndef EXCLUDE_FROM_XARALX
			((GRenderBrush*)pRender)->SetChangedBBox(ChangedRect);
#endif
		}

	} // end if pBrushRef
	
	m_pObjectRendered[BrushIndex] = TRUE;  // Set the flag to say we have rendered
	m_pLastScalingRendered[BrushIndex] = m_LastScaling;  // record the scaling we rendered at
}
#endif

/********************************************************************************************

>	void PathProcessorBrush::RenderBrushAtPoint(DocCoord Point, double Tangent, UINT32 BrushIndex, UINT32 Counter
												RenderRegion* pRender, BOOL RealtimeBrush) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Point - co-ordinate where we wish to render the brush object
				Angle - the tangent at that point
				BrushIndex - index into the brushref array of the brush definition
				Counter - the number of objects drawn so far
				pBecomeA - deals with things if we are doing a DoBecomeA, can be null
				RealtimeBrush - are we drawing a brush in real time (must supply a GRenderBrush region
				if so) defaults to FALSE.
				pRender - the render region to render on to
	Outputs:    -
	Returns:	- 
	Purpose:	Multipurpose !
				Renders a brush object at the point specified if we supply a render region, or
				calculates the bounding box of the brush at this point and adds it to our
				cached rect, or makes calls PassBack for our BecomeA object.

				This function is called from:
				ProcessPath - for normal rendering
				CalculateBoundingBox - 
				DoBecomeA
				OpDrawBrush::RenderStepsForTime 

				Essentially what this does is:

				For each Brushref
					For Each Path and AttrMap belonging to the brushref
						Work out render point, based on Coord provided plus offset
						Scale 
						Rotate
						Rotate for tangent
						either render, or do passback, depending on who's calling
						
						Reverse the transformations, however now that we are using 
						copies rather than the original paths etc. we can probably
						ditch these.


********************************************************************************************/

#ifndef NEW_RENDERBRUSHATPOINT

void PathProcessorBrush::RenderBrushAtPoint(DocCoord Point, double Tangent, UINT32 BrushIndex, UINT32 Counter,
											 RenderRegion* pRender, HandleBrushBecomeA* pBecomeA, 
											 BOOL RealtimeBrush, BrushDefinition* pBrushDefinition)
{
	BrushDefinition* pOurBrushDef = NULL;
	if (!pBrushDefinition)
	{
		pOurBrushDef = GetOurBrushDefinition();
	}
	else
	{
		pOurBrushDef = pBrushDefinition;
	}

	if (pOurBrushDef == NULL)
		return;
	
//	TRACEUSER( "Diccon", _T("Brush Object = %d\n"), BrushIndex);

	// lets declare some locals here that we need in the loop later
	Trans2DMatrix Trans;
	Trans2DMatrix Rotate;
	Trans2DMatrix RevRotate;
	Trans2DMatrix TangMat;
	Trans2DMatrix RevTangent;
	Trans2DMatrix Scale;
//	Trans2DMatrix RevScale;

	// allocate some locals outside  the loop
	BrushRef* pBrushRef; // the brushref for this brush object
	CCAttrMap* pAttrMap = NULL;  // the attrmap that holds the attributes for each brush
	Path*	   pRenderPath = NULL;	// the path that we will render
	DocCoord*  pRenderCoords = NULL; // pointer to the coords of pRenderPath
	INT32	   NumRenderCoords = 0; // num coords in pRenderPath
	DocCoord RenderPoint; // the coord to render at

	POSITION ListPosition;
	BlendPathOffset* pOffset = NULL; // pointer to the x,y offset of each individual object within
									 // a brush made from a group
	
	// quickly ensure that we will draw a brush object
	if (BrushIndex >= pOurBrushDef->GetNumBrushObjects())
		BrushIndex = 0;
	
	// get the scaling here, this differs slightly according to whether this is a realtime
	// or a normal render (in the latter GetNextScaling was called in ProcessPath)
	double Scaling = 1.0;
	double ExtraAngle = 0;
	if (!RealtimeBrush)
	{
		Scaling = m_CurrentScaling;
		ExtraAngle = m_LastAngle;
	}
	else
	{
		ExtraAngle = GetNextAngle();
		Scaling = GetNextScaling(Counter);
		if (Scaling <= 0)
			return;
		
		GetNextHueMultiplier();
		GetNextSaturationMultiplier();
		
	}
	
	// get the offset value from the path
	MILLIPOINT OffsetVal = 0;
	if (RealtimeBrush) // || pRender == NULL)
		OffsetVal = GetNextOffset();
	else
		OffsetVal = m_LastOffset;

	double TranspAdjust = GetNextTransparencyAdjust();
	double PressureTranspAdjust = GetNextPressureTranspAdjust();

	pBrushRef = pOurBrushDef->GetBrushRef(BrushIndex);
	
	if (pBrushRef == NULL)
	{
		ERROR3("No brushref in RenderBrushAtPoint");
		return;
	}
	DocCoord RotateCoord;
	UINT32 NumBlendPaths = pBrushRef->GetNumBlendPaths();
	
	// if we are dealing with multiple blendpaths then
	// a) we want to render all attributes always and
	// b) we want to do our tangential rotation around the point where the centre 
	// of the group would be
	BOOL SpecifyRotateCoord = FALSE;
	
	if (NumBlendPaths > 1)
	{
		m_bRenderAllAttributes = TRUE;
		SpecifyRotateCoord = TRUE;
	}

	if (pBrushRef!= NULL)
	{
		pOffset = pBrushRef->GetFirstOffset(&ListPosition);
		//pBlendPath = pBrushRef->GetFirstBlendPath();
		pRenderPath = pBrushRef->GetFirstCopyPath();

		DocRect ChangedRect = DocRect(0,0,0,0);
		DocRect BRect;
	
		// if we're doing a passback/convert to shapes then tell it how many blendpaths we have
		if (pBecomeA != NULL)
			pBecomeA->SetNumBlendPaths(NumBlendPaths);
		UINT32 ObjectCounter = 0;
		while (pRenderPath != NULL)
		{	
			// get data that we need for transformations
			pRenderCoords = pRenderPath->GetCoordArray();
			NumRenderCoords = pRenderPath->GetNumCoords();

			ObjectCounter++;
			// first work out where we are going to translate to
			RenderPoint = AdjustPositionForOffset(Point, Tangent, m_BrushScaling, OffsetVal, m_LastOffsetType);
			
			// if we have a group that we want to rotate tangentially, we want the centre 
			// of rotation to be the original center of the group
			if (SpecifyRotateCoord)
				RotateCoord = RenderPoint;

			if (pOffset != NULL)
				RenderPoint = AdjustPositionForBlendPathOffset(RenderPoint, pOffset, m_LastScaling, Tangent);	
			
			// find out where the blendpath is at the moment, and transate it
			// to where we are now
			BRect = pRenderPath->GetBoundingRect(); //pBlendPath->m_pPath->GetBoundingRect();
			DocCoord CurrentPoint = BRect.Centre();
		
			Trans.SetTransform(RenderPoint.x  - CurrentPoint.x, RenderPoint.y - CurrentPoint.y );
			BRect.Translate(RenderPoint.x  - CurrentPoint.x, RenderPoint.y - CurrentPoint.y );
		//	TRACEUSER( "Diccon", _T("Post translate BRect hiY = %d\n"), BRect.hiy);
		
			
			// get the attribute map, note that attributes applying to this blendpath
			// are transformed by the blendpath::transform fn.
			pAttrMap = pBrushRef->GetCurrentAttributeCopy();            //FindAppliedAttributes(pBlendPath); // pBlendPath->FindAppliedAttributes();
			if (pAttrMap == NULL)
			{
				ERROR3("No applied attributes");
				return;
			}

			// Transform the path and attributes
			Trans.Transform(pRenderCoords, NumRenderCoords);
		
			if (m_bTile)
				pAttrMap->TransformForBrush(Trans);
			else
				pAttrMap->Transform(Trans);


			// get the scaling matrices
			Matrix ScaleMat;  
			Matrix RevScaleMat;
			
			// work out the scaling matrix
			if (Scaling != 1.0)
			{
				// get the scaling that we are going to use
				FIXED16 NewScale(Scaling);
				FIXED16 OldScale = 1 / NewScale; 
			
				// translate to origin, scale, translate back
				ScaleMat*=Matrix(-BRect.Centre().x, -BRect.Centre().y);
				ScaleMat*=Matrix(NewScale, NewScale);
				ScaleMat*=Matrix(BRect.Centre().x, BRect.Centre().y);
				
				// set our 2D matrix
				Scale.SetTransform(ScaleMat);
				Scale.TransLines = TRUE;
				
				// if we're going to end up scaling the line width to zero then set a transparent stroke colour
				if (pAttrMap->WillScaleLineWidthToZero(Scale))
				{
					AttrStrokeColour* pOldStrokeCol = NULL;
					pAttrMap->Lookup((void*)CC_RUNTIME_CLASS(AttrStrokeColour), (void*&)pOldStrokeCol);
					if (pOldStrokeCol)
					{
						// set the transparent colour
						pOldStrokeCol->Value.Colour = DocColour(COLOUR_TRANS);
					}
					else
					{
						AttrStrokeColour* pNewStrokeCol = new AttrStrokeColour;
						pNewStrokeCol->Value.Colour = DocColour(COLOUR_TRANS);
					}
			
				}

				
				// scale 'em Danno
				Scale.Transform(pRenderCoords, NumRenderCoords);

				// if its our first time then scale the fills to the value specified by our member
				if (m_UseActualScaling)
				{
					if (m_BrushScaling != 0)
					{
						FIXED16 FillScale(m_BrushScaling);
						Trans2DMatrix ScaleFills;
						Matrix FillScaleMat;
						FillScaleMat*=Matrix(-BRect.Centre().x, -BRect.Centre().y);
						FillScaleMat*=Matrix(FillScale, FillScale);
						FillScaleMat*=Matrix(BRect.Centre().x, BRect.Centre().y);
						ScaleFills.SetTransform(FillScaleMat);
					
						pAttrMap->TransformBrushFills(ScaleFills);
					}

				}


				if (m_bTile)
					pAttrMap->TransformForBrush(Scale);
				else
					pAttrMap->Transform(Scale);

				// scale the bounding box
				ScaleMat.TransformBounds(&BRect);		
			}

			// do tangential rotation 
			if (m_bRotate)
			{
				// note that if we have a multiple blendpath situation then we want to rotate
				// around what would be the centre point of the group.
				double Tang = Tangent / PI * 180;
				if (SpecifyRotateCoord)
				{
					TangMat.SetTransform(RotateCoord, Tang);
					RevTangent.SetTransform(RotateCoord, -Tang);
					//BrushAttrValue::RotateBounds(Tang, &BRect, &RotateCoord);
				}
				else
				{
					TangMat.SetTransform(RenderPoint, Tang);
					RevTangent.SetTransform(RenderPoint, -Tang);
					// rotate and expand the bounding box
					
				}
				BrushAttrValue::RotateBounds(Tang, &BRect);	
			
				//pBlendPath->Transform(TangMat.GetpMatrix(), FALSE, m_bTile);		
				TangMat.Transform(pRenderCoords, NumRenderCoords);
				if (m_bTile)
					pAttrMap->TransformForBrush(TangMat);
				else
					pAttrMap->Transform(TangMat);
			}
		
			
			//TRACEUSER( "Diccon", _T("Count = %d, Angle = %f\n"), Counter, m_LastAngle);
			// rotate if we have to
			if (m_LastAngle != 0 || m_RotationMaxRand != 0)
			{
				double RotAngle = 0;
				RotAngle += m_LastAngle;
				if (SpecifyRotateCoord)
					Rotate.SetTransform(RotateCoord, RotAngle);
				else
					Rotate.SetTransform(RenderPoint, RotAngle);
			
				//pBlendPath->Transform(&(Rotate.GetMatrix()),FALSE, m_bTile);
				Rotate.Transform(pRenderCoords, NumRenderCoords);
				
				if (m_bTile)
					pAttrMap->TransformForBrush(Rotate);
				else
					pAttrMap->Transform(Rotate);
			
				// set the reverse
				RevRotate.SetTransform(RenderPoint, -RotAngle);
				// rotate the bounding rect
				BrushAttrValue::RotateBounds(RotAngle, &BRect);
			}
		
			// we now need a colour replacer, in case we are overriding colours
			CHSVChanger ColChanger;
			ColChanger.SetHueMultiplier(m_LastHueMultiplier);
			ColChanger.SetSaturationMultiplier(m_LastSatMultiplier);
			
			// we also want a transparency replacer
			CTransparencyReplacer TranspRep;
		
			TranspRep.SetAdjustmentValue(TranspAdjust);
			TranspRep.SetInvertedAdjuster(PressureTranspAdjust);
		
			// finally lets draw it
			if (pRender != NULL)
			{
				RenderAttributes(pRender, pAttrMap, &ColChanger, &TranspRep);
		//		if (!RealtimeBrush)
					pRender->DrawPath(pRenderPath, this);
		
			}
			// if we're doing a passback/convert to shapes then do it here
			if (pBecomeA != NULL)
			{
				AdjustFillAttributes(pAttrMap, &ColChanger, &TranspRep);
				pBecomeA->Passback(pRenderPath, pAttrMap, ObjectCounter);
			}
		
			// we must restore any colours we replaced after drawing the path
			if (ColChanger.IsInitialised())
				ColChanger.RestoreColours();
			if (TranspRep.IsInitialised())
				TranspRep.RestoreTransparency();
			
			// get true bounding rect here, including the linewidth	
			AttrLineWidth* pLineWidth = NULL;
			pAttrMap->Lookup((void*)CC_RUNTIME_CLASS(AttrLineWidth), (void*&)pLineWidth);
			INT32 LineWidth = 1500;
			if (pLineWidth != NULL)
			{
				LineWidth = pLineWidth->Value.LineWidth;
				if (LineWidth <= 500)
					LineWidth = 1500;
			}

			//pBlendPath->m_pPath->GetTrueBoundingRect(&BRect, LineWidth, pAttrMap);
			pRenderPath->GetTrueBoundingRect(&BRect, LineWidth, NULL);
			
			// reverse the rotation
			if (m_LastAngle != 0 )
			{
			//	pBlendPath->Transform(RevRotate.GetpMatrix(), FALSE, m_bTile);
				RevRotate.Transform(pRenderCoords, NumRenderCoords);
				if (m_bTile)
					pAttrMap->TransformForBrush(RevRotate);
				else
					pAttrMap->Transform(RevRotate);
				DocRect NewRect = BRect;
				BrushAttrValue::RotateBounds(m_LastAngle, &NewRect);
				BRect = BRect.Union(NewRect);
			}
			// reverse the tangential rotation
			if (m_bRotate )
			{
				//pBlendPath->Transform(RevTangent.GetpMatrix(), FALSE, m_bTile);
				RevTangent.Transform(pRenderCoords, NumRenderCoords);
				if (m_bTile)
					pAttrMap->TransformForBrush(RevTangent);
				else
					pAttrMap->Transform(RevTangent);

				DocRect NewRect = BRect;
				BrushAttrValue::RotateBounds(Tangent, &NewRect);
				BRect = BRect.Union(NewRect);
			}
			//TRACEUSER( "Diccon", _T("Rendering Object %d at %d, %d\n"), m_BrushStep, Point.x, Point.y);
				
			// if we have not already retrieved the last item then get it here
			if (ListPosition != NULL)
				pOffset = pBrushRef->GetNextOffset(&ListPosition);
		
			pRenderPath = pBrushRef->GetNextCopyPath();

			//pBlendPath = pBrushRef->GetNextBlendPath(pBlendPath);
					
			// we need to know the on screen area that has changed
			if (ChangedRect.IsEmpty())
				ChangedRect = BRect;
			else
				ChangedRect = ChangedRect.Union(BRect);
			
			// if we are calculating the bounding rect then do it here using the previous calculation
			if (m_bCalculateRect)
			{
				if (m_CachedRect.IsEmpty())
					m_CachedRect = ChangedRect;
				else
					m_CachedRect = m_CachedRect.Union(ChangedRect);
			}
			
		} // end while pBlendPath
		
		// if we are drawing the brush in realtime then set the changed bounding box
		if (RealtimeBrush == TRUE)
		{
			// oh no, another awful hack
			INT32 Height = ChangedRect.Height();
			INT32 Width = ChangedRect.Width();
		//	if (NumBlendPaths > 1)
		//	ChangedRect.Inflate((INT32)(Height * 1.25), (INT32)(Width *1.25));
	
			((GRenderBrush*)pRender)->SetChangedBBox(ChangedRect);
		}

	} // end if pBrushRef
	
	m_pObjectRendered[BrushIndex] = TRUE;  // Set the flag to say we have rendered
	m_pLastScalingRendered[BrushIndex] = m_LastScaling;  // record the scaling we rendered at

}
#endif


/********************************************************************************************

>void PathProcessorBrush::RenderAttributes(RenderRegion* pRender, CCAttrMap* pAttrMap, 
										  CHSVChanger* pReplacer, CTransparencyReplacer* pTranspRep)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		pRender = ptr to render region
				pAttrMap - the attribute map to render
				pColChange - object that changes the colours in our fills
				pTranspRep - object to change transparencies
	Outputs:	-
	Returns:	-
	Purpose:	Renders the attributes to the given render region.  This function has ended up being
				really messy because people have insisted on being able to override various of the
				cached attributes.  Heres a brief rundown:
				- m_bUseLocalFillColour means that we use to fill colour closest to our attribute
				node instead of the cached fill colour. Note that if m_bUseLocalFillColour is FALSE
				we may still use the local fill colour instead of our Named colours.
				- Instead of our cached transparency we may choose not to render transparency, thereby
				letting the previously rendered transparency apply to our brush objects.

				Prior to rendering our attribute map we must render a blank transparency attribute.
				This is because we are unable to call RestoreContext from a path processor and if
				a previous blendpath rendered a transparency we do not want it to remain.  This
				also applies to fill colour.

	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::RenderAttributes(RenderRegion* pRender, CCAttrMap* pAttrMap, 
										  CHSVChanger* pColChange, CTransparencyReplacer* pTranspRep)
{
	// Check entry params
	BOOL ok = (pRender != NULL && pAttrMap != NULL);
	ERROR3IF(!ok,"One or more NULL entry params");
	if (!ok) return;

	BOOL bRenderedTransp = FALSE;
	BOOL bRenderedColourFill = FALSE;

	CCRuntimeClass	   *pType;
	void			   *pVal;
	NodeAttribute	   *pNodeAttr;
	
	m_bRenderAllAttributes = TRUE; // now that we always need to have rendered transparencies we must force this on

	pRender->SetFillColour(COLOUR_NONE);// clear any previous fill colours

	// iterating all (key, value) pairs
	for( CCAttrMap::iterator Pos = pAttrMap->GetStartPosition(); Pos != pAttrMap->GetEndPosition(); )
	{
		// Get attr at position Pos 
		pAttrMap->GetNextAssoc( Pos, pType, pVal );

		// pVal is actually an attribute, so get a ptr to it and render it
		pNodeAttr = (NodeAttribute *)pVal;

		// if its a fill we might prefer to use the local stroke colour instead
		if (pNodeAttr->IsAColourFill() && !pNodeAttr->IsATranspFill() && !pNodeAttr->IsABitmapFill())	 
		{
			
			// as we now have to deal with named colours this is a bit complex but basically:
			// - if m_bUseLocalFillColour == FALSE, && m_bUseNamedColours == FALSE && this fill has
			// a named colour, then we will end up rendering the local colour anyway.  Otherwise we
			// will render this colour normally.
			// Another case to cater for, if we have hue and saturation variations then we need to do those also
			
			bRenderedColourFill = TRUE;
			if (!m_bUseLocalFillColour)
			{
				
				//first find out if we have a named colour
				ColourFillAttribute* pVal = (ColourFillAttribute*)pNodeAttr->GetAttributeValue();
				if (pVal->ContainsNamedColour())
				{
					if (!m_bUseNamedColours)
					{
						//TRACEUSER( "Diccon", _T("Overriding Named Colours\n"));
						// we need to initialise the Replacer with our attribute and the local
						// stroke colour
				
						DocColour Col = m_CurrentStrokeCol;
						pColChange->Initialise(pVal, &Col); 
						pColChange->ReplaceColours();

						// note that CNamedColorReplacer::RestoreColours() MUST be called AFTER 
						// DrawPaths in the render routine, if it is not the the colours are permanently replaced.
					}
				}

				// change HSV values if we need to
				if (pColChange->ShouldChangeHSV())
				{
					pColChange->SetColourFill(pVal);
					pColChange->ChangeHSVValues();
				}
			
				// check to see if we actually need to render it
				if (m_bRenderAllAttributes || pNodeAttr->NeedsToRenderAtEachBrushStroke())
				{
					pNodeAttr->Render(pRender);
				}

			}
			else
			{
				// we want the local colour
				if (m_pParentAttr != NULL)
				{
					DocColour LocalColour = m_CurrentStrokeCol;
					if (pColChange != NULL)
					{
						pColChange->ChangeColour(&LocalColour);
					}
					pRender->SetFillColour(LocalColour);
				}
			}
		}
		else if (pNodeAttr->IsATranspFill())
		{
			AdjustAndRenderTransparency(pRender, (AttrFillGeometry*)pNodeAttr, pTranspRep);
			bRenderedTransp = TRUE;
		}
		else
		{
		
			if (m_bRenderAllAttributes || pNodeAttr->NeedsToRenderAtEachBrushStroke())
			{
				pNodeAttr->Render(pRender);
			}
		}
	
	}
	

	if (m_bRenderAllAttributes)
	{
		// as we can't restore contexts and we have not already rendered a transparency then 
		//we must render a blank transparency, in case we have a transpareny left over from the last object
		if (m_pDefaultTransp != NULL && !bRenderedTransp)
		{
			AdjustAndRenderTransparency(pRender, m_pDefaultTransp, pTranspRep);
			m_pDefaultTransp->Render(pRender);
		}
	}

	if (bRenderedColourFill == FALSE)
		pRender->SetFillColour(COLOUR_NONE);

	// ensure that if we are tiling then our fill mapping type is not single
	if (m_bTile)
	{
		FillMappingAttribute* pVal = (FillMappingAttribute*)pRender->GetCurrentAttribute(ATTR_FILLMAPPING);
		if (pVal)
		{
			if (pVal->Repeat == RT_Simple)
			{
				// Whose dozy idea was this?  This directly alters the FillMapping attribute
				// applied to the object with the brush-stroked outline.
				// pVal->Repeat = RT_Repeating;

				// Instead we will create a copy of the current attribute, set it's Repeat
				// value to "repeating" and render it.
				FillMappingAttribute* pNewVal = (FillMappingAttribute*)(pVal->GetRuntimeClass()->CreateObject());
				if (pNewVal)
				{
					pNewVal->SimpleCopy(pVal);
					pNewVal->Repeat = RT_Repeating;
					pRender->SetFillMapping(pNewVal, TRUE);
				}
			}
		}
	}
}


/********************************************************************************************

>void PathProcessorBrush::AdjustAndRenderTransparency(RenderRegion* pRender, AttrFillGeometry* pAttrTransp, 
													 CTransparencyReplacer* pRep)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		pRender = ptr to render region
				pAttrTransp - the attribute to render, this must be a transparency attribute
				pRep - the replacer which changes transparency values
	Outputs:	pAttrTransp gets its values changed
	Returns:	-
	Purpose:	To adjust the transparency attribute with the replacer and then render it into
				the render region.  Note that you must call CTransparencyReplacer::Restore() after
				you have rendered your objects with this transparency, otherwise the change will
				be permanent
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::AdjustAndRenderTransparency(RenderRegion* pRender, AttrFillGeometry* pAttrTransp, 
													 CTransparencyReplacer* pRep)
{
	if (pRender == NULL || pAttrTransp == NULL || pRep == NULL)
		return;

	if (!pAttrTransp->IsATranspFill())
	{		
		ERROR3("Attribute is not transparent in PathProcessorBrush::AdjustAndRenderTransparency");
		return;
	}

	TranspFillAttribute* pTranspVal = (TranspFillAttribute*)pAttrTransp->GetAttributeValue();
	if (pTranspVal != NULL)
	{
		// see if our parent attr has a local transparency value, if so we will use that
		if (m_pParentAttr != NULL)
		{
			UINT32 ReplaceVal = m_pParentAttr->GetLocalTranspValue();
			if (ReplaceVal > 0)
				pRep->SetReplacementValue((INT32)ReplaceVal);
			UINT32 Type = m_pParentAttr->GetLocalTranspType();
			pTranspVal->SetTranspType(Type);
		}
		pRep->SetTransparencyAttr(pTranspVal);
		pRep->ReplaceTransparency();
	}

	pAttrTransp->Render(pRender);
}


/********************************************************************************************

>void PathProcessorBrush::AdjustFillAttributes(CCAttrMap* pAttrMap, CHSVChanger* pReplacer, 
											  CTransparencyReplacer* pTransRep)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		pAttrMap - the map containing attributes for this brush
				pColChange - object to replace HSV values for colour fills
				pTranspRep - object to replace transparency values
	Outputs:	
	Returns:	-
	Purpose:	To adjust the colour fill and transparency values in the attribute map
				Note that you must call the Restore() method for the two replacer objects
				otherwise the attributes will be permanently changed
				
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::AdjustFillAttributes(CCAttrMap* pAttrMap, CHSVChanger* pColChange, 
											  CTransparencyReplacer* pTranspRep)
{
	if (pAttrMap == NULL || pColChange == NULL || pTranspRep == NULL)
	{
		ERROR3("Invalid inputs to PathProcessorBrush::AdjustFillAttributes");
		return;
	}


	// lets find our attributes
	CCRuntimeClass	   *pType;
	void			   *pVal;
	NodeAttribute	   *pNodeAttr;
	
	BOOL FoundTransp = FALSE;

	// iterating all (key, value) pairs
	for( CCAttrMap::iterator Pos = pAttrMap->GetStartPosition(); Pos != pAttrMap->GetEndPosition(); )
	{
		// Get attr at position Pos
		pAttrMap->GetNextAssoc(Pos,pType,pVal);

		// pVal is actually an attribute, so get a ptr to it and render it
		pNodeAttr = (NodeAttribute *)pVal;

		if (pNodeAttr->IsAColourFill())
		{
			if (!m_bUseLocalFillColour)
			{
				
				//first find out if we have a named colour
				ColourFillAttribute* pVal = (ColourFillAttribute*)pNodeAttr->GetAttributeValue();
				if (pVal->ContainsNamedColour())
				{
					if (!m_bUseNamedColours)
					{
						// we need to initialise the Replacer with our attribute and the local
						// stroke colour
						if (m_pParentAttr != NULL && pColChange != NULL)
						{
							DocColour Col = m_CurrentStrokeCol;
							pColChange->Initialise(pVal, &Col);
							pColChange->ReplaceColours();

						}
						// note that CNamedColorReplacer::RestoreColours() MUST be called AFTER 
						// DrawPaths in the render routine, if it is not the the colours are permanently replaced.
					}
				}

				// change HSV values if we need to
				if (pColChange->ShouldChangeHSV())
				{
					pColChange->SetColourFill(pVal);
					pColChange->ChangeHSVValues();
				}

			}
		
		}
		if (pNodeAttr->IsATranspFill())
		{
			TranspFillAttribute* pTranspVal = (TranspFillAttribute*)pNodeAttr->GetAttributeValue();
			if (pTranspVal != NULL)
			{
				// see if our parent attr has a local transparency value, if so we will use that
				if (m_pParentAttr != NULL)
				{
					UINT32 ReplaceVal = m_pParentAttr->GetLocalTranspValue();		
					if (ReplaceVal > 0)
					{
						pTranspRep->SetReplacementValue((INT32)ReplaceVal);
						UINT32 Type = m_pParentAttr->GetLocalTranspType();
						pTranspVal->SetTranspType(Type);
					}

				}
				
				pTranspRep->SetTransparencyAttr(pTranspVal);
				pTranspRep->ReplaceTransparency();
				FoundTransp = TRUE;
			}
		}
		
	}
	
	// if we didn't find a transparency attribute then we might have to make one
	if (FoundTransp == FALSE)
	{
		AttrFlatTranspFill* pTransp = new AttrFlatTranspFill;
		if (m_pParentAttr != NULL && pTransp != NULL)
		{
			// get the transparency value of our parent
			UINT32 TranspVal = m_pParentAttr->GetLocalTranspValue();		
			TranspFillAttribute* pTranspVal = (TranspFillAttribute*)pTransp->GetAttributeValue();
			pTranspVal->Transp = TranspVal;
			
			UINT32 Type = m_pParentAttr->GetLocalTranspType();
			pTranspVal->SetTranspType(Type);

			// add it to the map
			pAttrMap->SetAt( CC_RUNTIME_CLASS(AttrFlatTranspFill), (void*)pTransp );

			// now do the replacing
			pTranspRep->SetTransparencyAttr(pTranspVal);
			pTranspRep->ReplaceTransparency();

		}

	}
}


/********************************************************************************************

>	DocCoord PathProcessorBrush::AdjustPositionForOffset(DocCoord OriginalPoint, BlendPathOffset Offset) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     OriginalPoint - the point on the line at the correct distance for us to draw an object
				Angle - the tangent at this point on the path
				Scaling - the scaling to use
				Offset - the blendpathoffset for this blendpath
				OffsetType - used only for random offset types, this can be left right or centre and
				is determined by GetNextOffsetType
	Outputs:    -
	Returns:	the new position where we wish the object to be drawn 
	Purpose:	To determine where the next brush object should be drawn.  The position from the
				line can be altered by the following factors;
				
			

				- m_PathOffsetType and m_PathOffsetValue:  These translations map the point on one side of
				the path or the other, the value determines exactly how far from the path they will be.

				- m_PositionRandomness:  introduces a random element to the position of the object
	SeeAlso:	-

*********************************************************************************************************/

DocCoord PathProcessorBrush::AdjustPositionForOffset(DocCoord OriginalPoint, double Angle, double Scaling, 
													 MILLIPOINT OffsetValue, UINT32 OffsetType)
											 //BlendPathOffset* pOffset)
{	
	// if its zero or less we don't want to know
	if (OffsetValue <= 0)
		return OriginalPoint;

	// scale it to our current scaling
	OffsetValue = (MILLIPOINT)((double) OffsetValue * Scaling);
//	TRACEUSER( "Diccon", _T("OffsetVal = %d\n"), OffsetValue);
	double theta;
	double NewX;
	double NewY;

	
	switch (m_PathOffsetType)
	{
		case OFFSET_NONE:
			break;
		case OFFSET_ALTERNATE:
			if (m_AltValue == 0)
			{
				theta = Angle - (PI / 2);				
			}
			else
			{
				theta = Angle + (PI / 2);
			}
			
			NewX = OffsetValue * cos (theta);
			NewY = OffsetValue * sin (theta);
		//	TRACEUSER( "Diccon", _T("Y Offset = %d\n"), (INT32)NewY);
			OriginalPoint.translate((INT32)NewX, (INT32)NewY);
			break;
		case OFFSET_RIGHT:
			theta = Angle - (PI / 2);
			NewX = OffsetValue * cos (theta);
			NewY = OffsetValue * sin (theta);
			OriginalPoint.translate((INT32)NewX, (INT32)NewY);
			break;
		case OFFSET_LEFT:
			theta = Angle + (PI / 2);
			NewX = OffsetValue * cos (theta);
			NewY = OffsetValue * sin (theta);
			OriginalPoint.translate((INT32)NewX, (INT32)NewY);
			break;
		case OFFSET_RANDOM: 
		{
		//	TRACEUSER( "Diccon", _T("Offset type = %d\n"), OffsetType);
			switch (OffsetType)
			{
				case OFFSETTYPE_LEFT:
					theta = Angle + (PI / 2);
					NewX = OffsetValue * cos (theta);
					NewY = OffsetValue * sin (theta);
					OriginalPoint.translate((INT32)NewX, (INT32)NewY);
					break;
				case OFFSETTYPE_RIGHT:
					theta = Angle - (PI / 2);
					NewX = OffsetValue * cos (theta);
					NewY = OffsetValue * sin (theta);
					OriginalPoint.translate((INT32)NewX, (INT32)NewY);
					break;
				case OFFSETTYPE_CENTRE:
					break;
				default:
					ERROR3("Invalid offset type returned");
					break;
			}
			break;
		}
			
		default:
			ERROR3("Unknown offset type");
			break;
	}

	return OriginalPoint;
}

/********************************************************************************************

>	DocCoord PathProcessorBrush::AdjustPositionForBlendPathOffset(DocCoord OriginalPoint, BlendPathOffset* pOffset) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     OriginalPoint - the point on the line at the correct distance for us to draw an object
				Offset - the blendpathoffset for this blendpath
				Scaling
				Angle
	Outputs:    -
	Returns:	the new position where we wish the object to be drawn 
	Purpose:	To determine where the next brush object should be drawn.  The position from the
				line can be altered by the following factors;
				
	SeeAlso:	-

*********************************************************************************************************/

DocCoord PathProcessorBrush::AdjustPositionForBlendPathOffset(DocCoord OriginalPoint, BlendPathOffset* pOffset, 
															double Scaling, double Angle)
{
	if (pOffset->m_XOffset == 0 && pOffset->m_YOffset == 0)
		return OriginalPoint;

	MILLIPOINT XOffset;
	MILLIPOINT YOffset;

	DocCoord RotateCentre = OriginalPoint;

	XOffset = (MILLIPOINT)(pOffset->m_XOffset *Scaling);
	YOffset = (MILLIPOINT)(pOffset->m_YOffset *Scaling);

	// not much to it really
	OriginalPoint.translate(XOffset, YOffset);

	return OriginalPoint;
}


/********************************************************************************************
>	void PathProcessorBrush::RenderTimeStampPoints(RenderRegion* pRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/99
	Inputs:		pRender - the region to render into
				
	Outputs:	
			
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Accesses the timestamp cache of the parent attribute and tests its points
				against the bounds of the render region, rendering them if they are inside.
			
********************************************************************************************/

void PathProcessorBrush::RenderTimeStampPoints(RenderRegion* pRender)
{
	if (m_pParentAttr == NULL)
	{
		//ERROR3("Parent attribute is null");
		return;
	}

	BrushDefinition* pBrushDef = GetOurBrushDefinition();
	if (pBrushDef == NULL)
	{
		ERROR3("No brush definition");
		return;
	}

	UINT32 NumBrushObjects = pBrushDef->GetNumBrushObjects();
	
	Reset();
		
	// declare some locals that we need in the loop
//	BOOL ok = FALSE;
//	BOOL Found = TRUE;
	UINT32 Count = 0;
	UINT32 BrushObjectToRender = 0;
	
	DocRect InkRect = pBrushDef->GetLargestBoundingBox();
	DocRect RenderRect = pRender->GetClipRect();

	// if we're rotated then rotate the rect, don't have time to work out
	// the exact angle so use 90 deg, which gives the largest increase in size
	if ((m_bRotate) || (m_RotateAngle != 0.0) || m_RotationMaxRand != 0)
		m_pParentAttr->RotateBounds(90, &InkRect);

	MILLIPOINT InkWidth = InkRect.Width() / 2;
	MILLIPOINT InkHeight = InkRect.Height() / 2;
	
	// get the list from our attribvute
	TimeStampList* pTimeStampList = m_pParentAttr->GetTimeStampList();
	if( pTimeStampList == NULL || pTimeStampList->empty() )
	{
		ERROR3("No Timestamp list");
		return;
	}
	TimeStampList::iterator ListPos = pTimeStampList->begin();
	TimeStampBrushPoint TSBP;
//	PressureList* pPressure = m_pParentAttr->GetPressureCache();
//	POSITION PressurePos;
//	if (pPressure)
//		PressurePos = pPressure->GetHeadPosition();
	m_bRenderAllAttributes = TRUE;
	DocCoord TestPoint;
	m_bRenderAllAttributes = TRUE;
	while( ListPos != pTimeStampList->end() )
	{
		TSBP = *ListPos++;
		
		m_CurrentScaling = GetNextScaling(Count);
//		if (pPressure != NULL)
//		{
//			if (PressurePos != NULL)
//				m_CurrentPressure = 127; //pPressure->GetNext(PressurePos);
//		}
		// get the offset type (only important for random offset types)
		SetNextOffsetType();
		GetNextAngle();
		TestPoint = AdjustPositionForOffset(TSBP.m_Point, TSBP.m_Tangent,  m_CurrentScaling, GetNextOffset(), m_LastOffsetType);
		// is it in our render region?
		if (ObjectIsWithinRect(RenderRect, TestPoint, InkWidth, InkHeight, m_CurrentScaling))
		{
	//		TRACEUSER( "Diccon", _T("Rendering Object\n"));
			RenderBrushAtPoint(TSBP.m_Point, TSBP.m_Tangent,
					   BrushObjectToRender, Count,  pRender);
			
		}
		if (NumBrushObjects > 1)
				BrushObjectToRender = GetNextInkObject(BrushObjectToRender, NumBrushObjects);
		Count++;
		GetNextAngle();
		// this member keeps track of which side we should offset on if we have
		// an alternating offset
		if (m_AltValue == 0)
			m_AltValue = 1;
		else
			m_AltValue = 0;

	}
	pRender->RestoreContext();
//	TRACEUSER( "Diccon", _T("Rendered %d Objects\n"), Count);
}




/********************************************************************************************

>	BOOL PathProcessorBrush::ObjectIsWithinRect(DocRect Rect, DocCoord Centre, 
												MILLIPOINT HalfWidth, MILLIPOINT HalfHeight,
												double Scaling)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		Rect -  rect to test against
				Centre - the centre of the object to test
				HalfWidth - half the width of the object bounding rect
				HalfHeight - half the height;
				Scaling - the scaling
				Angle - the angle at which this object is rotated IN DEGREES
	Returns:	TRUE, if any part of this object intersects the rect.

	Purpose:	as above
********************************************************************************************/

BOOL PathProcessorBrush::ObjectIsWithinRect(DocRect Rect, DocCoord Centre, MILLIPOINT HalfWidth,
											MILLIPOINT HalfHeight, double Scaling, double Angle)
{
	if (Rect.IsEmpty())
		return FALSE;

	// if we're scaling then adjust the object widths
	if (Scaling != 1.0)
	{
		if (Scaling <=0)
			return FALSE;
		HalfHeight = (MILLIPOINT)((double)HalfHeight * Scaling);
		HalfWidth = (MILLIPOINT)((double)HalfWidth * Scaling);
	}
	
	
	DocRect TestRect;
	TestRect.hi.x = Centre.x + HalfWidth;
	TestRect.lo.x = Centre.x - HalfWidth;
	TestRect.hi.y = Centre.y + HalfHeight;
	TestRect.lo.y = Centre.y - HalfHeight;

	BrushAttrValue::RotateBounds(Angle, &TestRect, &Centre);

	
	if (Rect.IsIntersectedWith(TestRect))
	{
		//TRACEUSER( "Diccon", _T("Rects intersect\n"));
		return TRUE;
	}

	return FALSE;
}


/********************************************************************************************
>	BOOL PathProcessorBrush::GetPointAtDistance(Path* pPath, MILLIPOINT Distance, DocCoord* pPoint, 
												double* pTangent)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/99
	Inputs:		pPath - the path along which is the poiht we want
				Distance = a length along the path
	Outputs:	pPoint = the coordinate of the point that distance along the path
				pTangent = tangent at this point (can be NULL)
			
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Gets the coordinate of a point a certain distance along this path, either by looking
				in its cache pr by calling the Path class function
			
********************************************************************************************/

BOOL PathProcessorBrush::GetPointAtDistance(Path* pPath, MILLIPOINT Distance, DocCoord* pPoint, 
												double* pTangent)									   
{
	ERROR2IF(pPoint == NULL,FALSE,"NULL pPoint param");
	ERROR2IF(pPath == NULL, FALSE, "Path is NULL");
//	ERROR2IF(m_pParentAttr == NULL, FALSE, "parent attribute is NULL");
	BOOL Found = FALSE;
	BrushPointInfo PointInfo;


#ifdef BRUSHPOINTSCACHE
	// retrieve pointer to the cache from parent attribute
	PointsMap* pCache = NULL;
	if (m_pParentAttr != NULL && m_pParentAttr->CanUsePointsCache())
	{
		pCache = m_pParentAttr->GetCache();
		if (pCache != NULL)
		{
			// look for point in the cache
			PointsMap::iterator iter = pCache->find( Distance );
			Found = ( pCache->end() != iter );
			if( Found )
			{
				PointInfo = iter->second;
			}

		//	if (Found)
		//		TRACEUSER( "Diccon", _T("Point found in cache %d, %d\n"), PointInfo.m_Point.x, PointInfo.m_Point.y);
		}
	}
	
#endif
	if (!Found)
	{	
		// get the point from the path
		if (pPath->GetPointAtDistance(Distance,&PointInfo.m_Point,&PointInfo.m_Tangent, &PointInfo.m_Pressure))
		{
#ifdef BRUSHPOINTSCACHE
			// insert it into the cache
			if (pCache != NULL)
				(*pCache)[Distance] = PointInfo;
#endif
			//TRACEUSER( "Diccon", _T("Point %d, %d found from line\n"), PointInfo.m_Point.x, PointInfo.m_Point.y);
			Found = TRUE;
		}
		
	}

	if (Found)
	{
		*pPoint = PointInfo.m_Point;
		if (pTangent)
			*pTangent = PointInfo.m_Tangent;
		
	}


	if (Found == FALSE)
		TRACEUSER( "Diccon", _T("Not found at distance %d\n"), Distance);
	return Found;
}


/********************************************************************************************
>	void PathProcessorBrush::Reset

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/99
	Inputs:		-			
	Outputs:	-
	Returns:	-
	Purpose:	Sets the various members etc. to the values that they must hold at the beginning
				of any of the following:
				- Rendering;
				- Bounding box calculation;
				- Convert to shapes;
				- GetObjectCount;
********************************************************************************************/

void PathProcessorBrush::Reset()
{
	// set members to record variations for each step
	m_LastScaling = m_BrushScaling;
	m_LastScalingRendered = m_BrushScaling;
	m_LastSpacing = m_BrushSpacing;
	m_LastOffset  = m_PathOffsetValue;
	m_LastAngle   = m_RotateAngle;
	m_AltValue    = 0;
	m_LastObject = 0; 
	m_LastHueMultiplier = m_BrushHueIncrement;
	m_LastSatMultiplier = m_BrushSatIncrement;

	m_LastSpacingNoRandom = m_BrushSpacing;
	if (m_PathOffsetType != OFFSET_RANDOM)
		m_LastOffsetNoRandom  = m_PathOffsetValue;
	else
		m_LastOffsetNoRandom = 0;

	m_LastScalingNoRandom = m_BrushScaling;
	m_LastRotationNoRandom = m_RotateAngle;
	
	m_CurrentStrokeCol = DocColour(COLOUR_BLACK);

	// reset our random number generators
	ResetRandomNumberGenerators();


	// to find out which object comes first in the sequence we may need
	// to know how many objects we're going to have
	
	UINT32 NumObjects = 1;
	BrushDefinition* pDef = GetOurBrushDefinition();
	ERROR3IF(pDef == NULL, "No brush definition in PathProcessorBrush::Reset");

	if (pDef)
	{
		NumObjects = pDef->GetNumBrushObjects();

		switch (m_SequenceType)
		{
			case SEQ_FORWARD:
			case SEQ_MIRROR:
				m_LastObject = 0;
				m_MirrorSeqAscend = TRUE; //we always start ascending
				break;
			case SEQ_BACKWARD:
				m_LastObject = NumObjects -1;
				break;
			case SEQ_RANDOM:
				if (NumObjects > 1)
					m_LastObject = GetNextInkObject(m_LastObject, NumObjects);
				break;
			default:
				ERROR3("Invalid sequence type in PathProcessorBrush::Reset");
		}
	}
}

/********************************************************************************************

>	DocRect PathProcessorBrush::CalculateBoundingRect(Path* pPath, NodeRenderableInk* pParent) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		Path along which this brush will be applied
				pParent - the ink node that generated pPath.
	Returns:	the bounding rect of the given path with the brush applied
	Purpose:	Calculates the bounding rect, unfortunately the only way to do this is to
				do a 'fake' render.  So this function is essentially the same as processpath
				but it doesn't require a render region

********************************************************************************************/

DocRect PathProcessorBrush::CalculateBoundingBox(Path* pPath, NodeRenderableInk* pParent)
{
	DocRect ReturnRect(0,0,0,0);

	ERROR2IF(pPath == NULL, ReturnRect, "Path is null in PathProcessorBrush::CalculateBoundingBox");
	ERROR2IF(pParent == NULL, ReturnRect, "Parent is null in PathProcessorBrush::CalculateBoundingBox");

	// if we are timestamping then use a different method and return
	if (m_pParentAttr != NULL)
	{
		if (m_pParentAttr->IsTimeStamping())
		{
			return TimeStampBoundingBox();
		}
	}
	BrushDefinition* pBrushDef = GetOurBrushDefinition();
	if (pBrushDef == NULL)
	{
		return ReturnRect;
	}
	
	// get the definition to generate its data copies for us
	if (!pBrushDef->StartRender())
	{
		TRACEUSER( "Diccon", _T("Unable to start render brush for bounding box\n"));
		return ReturnRect;
	}

	Reset();

	if (m_BrushScaling <= 0)
		return ReturnRect;

	UINT32 NumBrushObjects = pBrushDef->GetNumBrushObjects();
	
	if (!PrepareForRenderingLoop(NumBrushObjects))
		return ReturnRect;

	// some variables to keep track of where we are on the path
	double dPathLength = pPath->GetPathLength(750/2);
//	double dOrigPathLength = dPathLength;
	MILLIPOINT PathLength = (MILLIPOINT)dPathLength;
	double PathProportion = 0.0;

	// this records the distance along the path at which we want to find a point
	MILLIPOINT DistanceToPoint = 10;
	
	// declare some locals that we need in the loop
	MILLIPOINT Spacing = m_BrushSpacing;
	CPathPointInfo PathPoint;
	double Angle;
	DocCoord Point; 
	BOOL ok = FALSE;
//	BOOL Found = TRUE;
	UINT32 Count = 0;
	UINT32 BrushObjectToRender = m_LastObject;

	
	/* To get our pressure data we will use either a cache that is held by our parent attribute
	OR use a value function if a VariableWidthAttribute has been rendered into the render region*/ 

	// get our pressure data
	ValueFunction* pValFunc = NULL;
	CDistanceSampler* pPressure = GetPressureCacheAndInitialise(pParent, &pValFunc);
	CSampleItem TheItem; // we need this in the loop

	// find our line width
/*	AttrLineWidth* pLineWidth = NULL;
	pParent->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrLineWidth), (NodeAttribute**)pLineWidth);
	if (pLineWidth)
	{
		INT32 Width = pLineWidth->Value.LineWidth;
		ScaleToValue(Width, !m_bValidPressure);
	}
*/
	m_bRenderAllAttributes = TRUE;

	m_CachedRect = DocRect(0,0,0,0);
	m_bCalculateRect = TRUE;

	TRACEUSER( "Diccon", _T("About to calculate bounding box\n"));
	
	// get the first spacing and scaling values
	m_UseActualScaling = TRUE;
	m_CurrentScaling = GetNextScaling(Count);
	m_LastScalingRendered = m_CurrentScaling;
	
	Spacing = GetNextSpacing();
	
	
	while (DistanceToPoint < PathLength)
	{
	//	TRACEUSER( "Diccon", _T("Count %d\n"), Count);
		// try and get the point	
		ok = GetPointAtDistance(pPath, DistanceToPoint, &Point, &Angle);
		
		if (ok)  
		{
			GetNextAngle();
			SetNextOffsetType();
			GetNextOffset();
				
			if (m_CurrentScaling > 0)
				RenderBrushAtPoint(Point, Angle,
						 BrushObjectToRender, Count,  NULL);
			Count++;

			// get the index of the next object.
			if (NumBrushObjects > 1)
				BrushObjectToRender = GetNextInkObject(BrushObjectToRender, NumBrushObjects);
			
			DecideWhetherToUseActualScaling(BrushObjectToRender);
		}
	
		if (pPressure != NULL)
		{
			if (pPressure->GetNext(&TheItem))
			{	
				if (TheItem.m_Pressure < 0 || TheItem.m_Pressure > MAXPRESSURE)
				{
					//ERROR3("Invalid pressure in PathProcessorBrush::CalculateBoundingBox");
					//TRACEUSER( "Diccon", _T("Invalid pressure %d at count %d\n"), TheItem.m_Pressure, Count);
				}
				else
				{
					m_CurrentPressure.m_Pressure = TheItem.m_Pressure;
					//RACEUSER("Diccon", "BBox pressure %d, = %d\n", Count, m_CurrentPressure.m_Pressure);
				}
			}
		//	else
		//		ERROR3("Unable to get pressure item in PathProcessorBrush::CalculateBoundingBox");
		}
#ifdef SCALESPACING		
		DistanceToPoint += (MILLIPOINT)(Spacing * m_LastScaling);
		
		if (pPressure)
			pPressure->SetSampleRateFromSpacing((INT32)(Spacing * m_CurrentScaling));
#else
		DistanceToPoint += (MILLIPOINT)(Spacing * m_BrushScaling);
		
		if (pPressure)
			pPressure->SetSampleRateFromSpacing((INT32)(Spacing * m_CurrentScaling));
#endif	
		
		// get the next distance
		Spacing = GetNextSpacing();
		m_LastScalingRendered = m_pLastScalingRendered[BrushObjectToRender];

		if (pValFunc != NULL)
		{
			PathProportion = (double)DistanceToPoint / dPathLength;
			m_CurrentPressure.m_Pressure = GetNextSampleItem(NULL, pValFunc, PathProportion).m_Pressure;
			if (m_CurrentPressure.m_Pressure > MAXPRESSURE)
				m_CurrentPressure.m_Pressure = MAXPRESSURE;
		}

		m_CurrentScaling = GetNextScaling(Count);

		// we're now allowed to go to zero scaling, but if we do so we must stop rendering
		if (m_CurrentScaling <= 0)
			break;

		// this member keeps track of which side we should offset on if we have
		// an alternating offset
		if (m_AltValue == 0)
			m_AltValue = 1;
		else
			m_AltValue = 0;
	
	}

	/*some more awful hacks
	if (!m_CachedRect.IsEmpty() && (!pBrushDef->BrushContainsGroup()))
	{
		DocRect InkRect = pBrushDef->GetLargestBoundingBox();
		MILLIPOINT Height = (MILLIPOINT)(InkRect.Height() * m_BrushScaling);
		MILLIPOINT Width = (MILLIPOINT)(InkRect.Width() * m_BrushScaling);
	}
	*/
//	TRACEUSER( "Diccon", _T("Bounding rect: Height = %d, Width = %d\n"), m_CachedRect.Height(), m_CachedRect.Width());
	m_bCalculateRect = FALSE;	
	TRACEUSER( "Diccon", _T("Finished bounding box\n"));
	CleanUpAfterRender();
	pBrushDef->StopRender();

	return m_CachedRect;
}	
	

/********************************************************************************************

>	DocRect PathProcessorBrush::TimeStampBoundingBox() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		Path along which this brush will be applied
	Returns:	the bounding rect of the given path with the brush applied
	Purpose:	Calculates the bounding rect, unfortunately the only way to do this is to
				do a 'fake' render.  So this function is essentially the same as processpath
				but it doesn't require a render region

********************************************************************************************/

DocRect PathProcessorBrush::TimeStampBoundingBox()
{
	if (m_pParentAttr == NULL)
	{
		//ERROR3("Parent attribute is null");
		return DocRect(0,0,0,0);
	}

	BrushDefinition* pBrushDef = GetOurBrushDefinition();
	if (pBrushDef == NULL)
	{
		ERROR3("No brush definition");
		return DocRect(0,0,0,0);
	}

	UINT32 NumBrushObjects = pBrushDef->GetNumBrushObjects();
	
	// seed the sequence random number generator
	if (m_SequenceType == SEQ_RANDOM)
		srand(m_SequenceRandSeed);

	// reset our random number generators
	ResetRandomNumberGenerators();
		
	// declare some locals that we need in the loop
//	BOOL ok = FALSE;
//	BOOL Found = TRUE;
	UINT32 Count = 0;
	UINT32 BrushObjectToRender = 0;
	

	// get the list from our attribvute
	TimeStampList* pTimeStampList = m_pParentAttr->GetTimeStampList();
	if (pTimeStampList == NULL || pTimeStampList->empty())
	{
		ERROR3("No Timestamp list");
		return DocRect(0,0,0,0);
	}
	TimeStampList::iterator ListPos = pTimeStampList->begin();
	TimeStampBrushPoint TSBP;
//	PressureList* pPressure = m_pParentAttr->GetPressureCache();
//	POSITION PressurePos;
//	if (pPressure != NULL)
//		PressurePos = pPressure->GetHeadPosition();

	m_bCalculateRect = TRUE;
	m_CachedRect = DocRect(0,0,0,0);
	while (ListPos != pTimeStampList->end())
	{
		TSBP = *ListPos++;
		
		m_CurrentScaling = GetNextScaling(Count);
	//	if (pPressure != NULL)
	//	{
//			if (PressurePos != NULL)
	//			m_CurrentPressure = 127; //pPressure->GetNext(PressurePos);
	//	}

	
		RenderBrushAtPoint(TSBP.m_Point, TSBP.m_Tangent,
				   BrushObjectToRender, Count,  NULL);
			
		
		if (NumBrushObjects > 1)
				BrushObjectToRender = GetNextInkObject(BrushObjectToRender, NumBrushObjects);
		Count++;
			
		// this member keeps track of which side we should offset on if we have
		// an alternating offset
		if (m_AltValue == 0)
			m_AltValue = 1;
		else
			m_AltValue = 0;
	}

	return m_CachedRect;

}


/********************************************************************************************

>	void PathProcessorBrush::SetCalculateBoundingBox(BOOL Set) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		value to set
	Returns:	-
	Purpose:	Sets the flag indicating whether or not to calculate the bounding rect on the
				next pass through RenderBrushAtPoint

********************************************************************************************/

void PathProcessorBrush::SetCalculateBoundingBox(BOOL Set)
{
	m_bCalculateRect = Set;
}


/********************************************************************************************

>	DocRect PathProcessorBrush::GetCachedRect()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		-
	Returns:	the cached bounding rect
	Purpose:	access fn.
********************************************************************************************/

DocRect PathProcessorBrush::GetCachedRect()
{
	return m_CachedRect;
}

/********************************************************************************************

>	BOOL PathProcessorBrush::DoBecomeA(BecomeA* pBecomeA, Path* pPath, Node* pParent)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/2000
	Inputs:		pBecomeA - the object that tells us what to become, and recieves the results
				pPath - the path that we are processing
				pParent- the node that this brush is applied to
	Outputs:	
	Returns:	TRUE if everything went ok,
	Purpose:	To convert our brush into its individual paths and attributes, very similar
				to the bounding box routine in that we extract the path from our parent node
				and use it to calculate our blenpaths etc.
		
********************************************************************************************/

BOOL PathProcessorBrush::DoBecomeA(BecomeA* pBecomeA, Path* pPath, Node* pParent)
{
	ERROR2IF(pBecomeA == NULL, FALSE, "BecomeA pointer is NULL in PathProcessorBrush::DoBecomeA");
	ERROR2IF(pParent == NULL, FALSE, "Parent is NULL in PathProcessorBrush::DoBecomeA");
	ERROR2IF(pPath == NULL, FALSE, "Path is NULL in PathProcessorBrush::DoBecomeA");

	BeginSlowJob();

	TRACEUSER( "Diccon", _T("ABOUT TO BECOME A\n"));

	BrushDefinition* pBrushDef = GetOurBrushDefinition();
	if (pBrushDef == NULL)
	{
		return FALSE;
	}
	if (!pBrushDef->StartRender())
		return FALSE;

	UINT32 NumBrushObjects = pBrushDef->GetNumBrushObjects();
	if (!PrepareForRenderingLoop(NumBrushObjects))
		return FALSE;
	Reset();

	if (m_BrushScaling <= 0)
		return TRUE;

	// some variables to keep track of where we are on the path
	double dPathLength = pPath->GetPathLength(750/2);
	double dOrigPathLength = dPathLength;
	MILLIPOINT PathLength = (MILLIPOINT)dPathLength;
	double PathProportion = 0.0;
	
	// this records the distance along the path at which we want to find a point
	MILLIPOINT DistanceToPoint = 0;
	
	// declare some locals that we need in the loop
	MILLIPOINT Spacing = m_BrushSpacing;
	CPathPointInfo PathPoint;
	double Angle;
	DocCoord Point; 
	BOOL ok = FALSE;
//	BOOL Found = TRUE;
	UINT32 Count = 0;
	UINT32 BrushObjectToRender = m_LastObject;
	
	m_bRenderAllAttributes = TRUE;
	
	if (pPath->IsClosed())
	{
		// if the path is closed then hopefully we will have adjusted the spacing so that the
		// path length fits an integer number of objects.  However we do not want to render
		// two objects on the start point so reduce the path length we actually use
		double ActualSpacing = m_BrushScaling * (double)m_BrushSpacing;

		PathLength -= (MILLIPOINT)(ActualSpacing * 0.2); // we need some margin for error
		dPathLength = (double)PathLength;
		AdjustSpacingForClosedPath(pPath, dOrigPathLength);

		if (pParent->IsARegularShape())
			pPath->Reverse();

	}

	// get our pressure data
	ValueFunction* pValFunc = NULL;
	CDistanceSampler* pPressure = GetPressureCacheAndInitialise((NodeRenderableInk*)pParent, &pValFunc);
	CSampleItem TheItem; // we need this in the loop

	// make an object to deal with the blendpaths etc
	HandleBrushBecomeA HandleBecomeA;
	HandleBecomeA.SetSecondary(pBecomeA->IsSecondary());

	// Allocate a new group to hold all our objects
	UndoableOperation* pUndoOp = pBecomeA->GetUndoOp();
	NodeRenderableInk* pContext = (NodeRenderableInk*)pParent;
	NodeGroup* pNodeGroup = NULL;
	
	// we need a new group node if we are replacing the brush or passing back to BrushBecomeAGroup
	if (pBecomeA->GetReason() == BECOMEA_REPLACE || pBecomeA->IsBrushBecomeAGroup())
	{	
		ALLOC_WITH_FAIL(pNodeGroup, (new NodeGroup), pUndoOp); 
		if (pNodeGroup == NULL)
			return FALSE;
	
		pContext = pNodeGroup;
	}
	
	
	if (pBecomeA->GetReason() == BECOMEA_REPLACE)
	{
		// insert the new group next to our parent
		if (pUndoOp)
		{
			BOOL ok = TRUE;
			// If we don't own the parent node, we should insert after (on top of) it
			if (pBecomeA->IsSecondary())
				ok = pUndoOp->DoInsertNewNode(pContext,pParent,NEXT,FALSE,FALSE,FALSE,FALSE);
			else
				ok = pUndoOp->DoInsertNewNode(pContext,pParent,PREV,FALSE,FALSE,FALSE,FALSE);

			if (!ok)
			{
				delete pNodeGroup;
				return FALSE;
			}
		}
		else
		{
			pContext->AttachNode(pParent, PREV);
		}

		HandleBecomeA.Initialise(pBecomeA, pNodeGroup, pParent);
	}
	else
		HandleBecomeA.Initialise(pBecomeA, pContext, pParent);

	// we need the local stroke colour
	NodeAttribute* pStrokeColour = NULL;
	
	// get the attribute
	((NodeRenderableInk*)pParent)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), &pStrokeColour);

	if (pStrokeColour != NULL)
	{

		// get the stroke colour
		StrokeColourAttribute* pVal = (StrokeColourAttribute*)pStrokeColour->GetAttributeValue();
		if (pVal != NULL)
			m_CurrentStrokeCol = pVal->Colour;
	}
	
	// get the first spacing and scaling values
	m_UseActualScaling = TRUE;
	m_CurrentScaling = GetNextScaling(Count);
	m_LastScalingRendered = m_CurrentScaling;

	Spacing = GetNextSpacing();
	while (DistanceToPoint < PathLength)
	{
	//	TRACEUSER( "Diccon", _T("Count %d\n"), Count);
		// try and get the point 
		ok = GetPointAtDistance(pPath, DistanceToPoint, &Point, &Angle);
		GetNextAngle();
		GetNextOffset();
		GetNextHueMultiplier();
		GetNextSaturationMultiplier();
		if (ok)  
		{
			SetNextOffsetType();

			if (m_CurrentScaling > 0)
				RenderBrushAtPoint(Point, Angle,
						 BrushObjectToRender, Count,  NULL, &HandleBecomeA);

			// get the next pressure value
			if (pPressure != NULL)
			{
				double pLength = dOrigPathLength;		// Don't keep calling pPath->GetPathLength(); It's complex!
				double pDataLength = pPressure->GetDistanceRep (), count = pPressure->GetCount ();
				double dataRatio = 1.0;

				if (pDataLength < pLength)
				{
					dataRatio = pDataLength / pLength;
				}
				else
				{
					dataRatio = pLength / pDataLength;
				}

				double offset = 1.0;
					
				if (DistanceToPoint < pLength)
				{
					offset = (DistanceToPoint / pLength) * dataRatio * count;
				}
				else
				{
					offset = (pLength / DistanceToPoint) * dataRatio * count;
				}
				
				if (pPressure->GetAtPos (offset, &TheItem))//GetNext(&TheItem))
				{
					if (TheItem.m_Pressure < MAXPRESSURE)
						m_CurrentPressure.m_Pressure = TheItem.m_Pressure;
					else
						m_CurrentPressure.m_Pressure = MAXPRESSURE;
				//	TRACEUSER( "Diccon", _T("Pressure = %d\n"), TheItem.m_Pressure);
				}
			}
	
			Count++;
		
			// get the index of the next object.
			if (NumBrushObjects > 1)
				BrushObjectToRender = GetNextInkObject(BrushObjectToRender, NumBrushObjects);

			DecideWhetherToUseActualScaling(BrushObjectToRender);
		}
#ifdef SCALESPACING		
		DistanceToPoint += (MILLIPOINT)(Spacing * m_LastScaling);
		
		if (pPressure)
			pPressure->SetSampleRateFromSpacing((INT32)(Spacing * m_CurrentScaling));
#else
			DistanceToPoint += (MILLIPOINT)(Spacing * m_BrushScaling);
		
		if (pPressure)
			pPressure->SetSampleRateFromSpacing(Spacing);
#endif
		
		if (pValFunc != NULL)
		{
			PathProportion = (double)DistanceToPoint / dPathLength;
			m_CurrentPressure.m_Pressure = GetNextSampleItem(NULL, pValFunc, PathProportion).m_Pressure;
			if (m_CurrentPressure.m_Pressure > MAXPRESSURE)
				m_CurrentPressure.m_Pressure = MAXPRESSURE;
		}

		// get the next distance
		Spacing = GetNextSpacing();
		m_LastScalingRendered = m_pLastScalingRendered[BrushObjectToRender];
		m_CurrentScaling = GetNextScaling(Count);
		
		// this member keeps track of which side we should offset on if we have
		// an alternating offset
		if (m_AltValue == 0)
			m_AltValue = 1;
		else
			m_AltValue = 0;
	
	}
	
	pBrushDef->StopRender();

	CleanUpAfterRender();
	TRACEUSER( "Diccon", _T("FINISHED BRUSH DOBECOMEA\n"));

	if (pBecomeA->GetReason() == BECOMEA_REPLACE)
	{
		HandleBecomeA.FinishPassback();				// Call passback even in Replace mode to inform handler of
													// location of new object(s) in document
		return HandleBecomeA.HideCreatedByNode();	// hide the original node 
	}
	else
		return HandleBecomeA.FinishPassback();

	return FALSE;
}


/********************************************************************************************

>	virtual BOOL PathProcessor::DoesActuallyDoAnything(RenderRegion* pRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/10/2000
	Inputs:		pRender - pointer to the render region that we are about to render our path into
	Returns:	TRUE if it is anticipated that this path processor will alter the path in any
				way, FALSE if not
	Purpose:	To determine whether or not our path processor is 'default' i.e. if does not 
				change the path at all.  If so then the render region will be free to use
				DrawPathToOutputDevice which will let us use dash patterns, arrowheads etc.
	
	Errors:		

********************************************************************************************/

BOOL PathProcessorBrush::DoesActuallyDoAnything(RenderRegion* pRender)
{
	if (pRender == NULL)
	{
		ERROR3("No render region supplied to PathProcessorBrush::DoesActuallyDoAnything");
		return FALSE;
	}

	// get the line width , if its too small we don't want to draw anything
	LineWidthAttribute* pLine = (LineWidthAttribute*)pRender->GetCurrentAttribute(ATTR_LINEWIDTH);
	
	if (pLine && pLine->LineWidth < 500)
		return FALSE;

	// if we have a transparent stroke colour then don't bother either
	StrokeColourAttribute* pStroke = (StrokeColourAttribute*)pRender->GetCurrentAttribute(ATTR_STROKECOLOUR);
	
	if (pStroke && pStroke != NULL)
	{
		if (pStroke->Colour.IsTransparent())
			return FALSE;
	}
	return TRUE;
}

/********************************************************************************************

>	virtual BrushDefinition* PathProcessorBrush::GetOurBrushDefinition() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the brush definition corresponding to our brush handle
	Purpose:	As above

********************************************************************************************/

BrushDefinition* PathProcessorBrush::GetOurBrushDefinition()
{
	// first check to see that we have been assigned a brush
	if (m_BrushHandle == BrushHandle_NoBrush)
	{
		//ERROR3("This processor has not been assigned a brush");
		return NULL;
	}

	return BrushComponent::FindBrushDefinition(m_BrushHandle);

}

/********************************************************************************************

>	virtual BOOL PathProcessorBrush::IsDifferent(PathProcessorBrush *pOther);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pOther - The other PathProcessorBrush

	Returns:	TRUE if they're considered different, FALSE if they are equivalent

	Purpose:	Equality operator

	Notes:		The base class implementation compares the runtime classes of the
				2 objects to see if they are different classes. If they are the same
				class, it assumes they're cnsidered equal - derived classes should
				override this behaviour if further comparisons are necessary.

********************************************************************************************/

BOOL PathProcessorBrush::IsDifferent(PathProcessorBrush *pOther)
{
	ERROR3IF(pOther == NULL, "Illegal NULL param");

	if (GetRuntimeClass() != pOther->GetRuntimeClass())
		return(TRUE);

	// compare all the members
	if ( 
		   (m_BrushHandle			!= pOther->GetBrushDefinitionHandle())
		|| (m_BrushSpacing			!= pOther->GetSpacing())
		|| (m_BrushSpacingIncrConst	!= pOther->GetSpacingIncrConst())
		|| (m_BrushSpacingIncrProp	!= pOther->GetSpacingIncrProp())
		|| (m_BrushSpacingMaxRand	!= pOther->GetSpacingMaxRand())
		|| (m_BrushSpacingRandSeed	!= pOther->GetSpacingRandSeed())
		|| (m_BrushScaling			!= pOther->GetBrushScaling())
		|| (m_BrushScalingIncr		!= pOther->GetBrushScalingIncr())
		|| (m_BrushScalingIncrConst != pOther->GetBrushScalingIncrConst())
		|| (m_BrushScalingMaxRand	!= pOther->GetScalingMaxRand())
		|| (m_BrushScalingRandSeed	!= pOther->GetScalingRandSeed())
		|| (m_RotateAngle			!= pOther->GetRotationAngle())
		|| (m_RotAngleIncrConst		!= pOther->GetRotationIncrConst())
		|| (m_RotationMaxRand		!= pOther->GetRotationMaxRand())
		|| (m_RotationRandSeed		!= pOther->GetRotationRandSeed())
		|| (m_bTile					!= pOther->IsTiled())
		|| (m_bRotate				!= pOther->IsRotated())
		|| (m_PathOffsetType		!= pOther->GetPathOffsetType())
		|| (m_PathOffsetValue		!= pOther->GetPathOffsetValue())
		|| (m_PathOffsetIncrConst	!= pOther->GetPathOffsetIncrConst())
		|| (m_PathOffsetIncrProp	!= pOther->GetPathOffsetIncrProp())
		|| (m_OffsetValueMaxRand	!= pOther->GetOffsetValueMaxRand())
		|| (m_OffsetValueRandSeed	!= pOther->GetOffsetValueRandSeed())
		|| (m_OffsetTypeRandSeed	!= pOther->GetOffsetTypeRandSeed())
		|| (m_SequenceType			!= pOther->GetSequenceType())
		|| (m_SequenceRandSeed		!= pOther->GetSequenceSeed())
		|| (m_bUseLocalFillColour	!= pOther->GetUseLocalFillColour())
		|| (m_bUseLocalTransp		!= pOther->GetUseLocalTransp())
		|| (m_bUseNamedColours		!= pOther->GetUseNamedColours())
		)
		return TRUE;
	
	// must be the same
	return(FALSE);
}



/********************************************************************************************

>	virtual PathProcessorBrush *PathProcessorBrush::Clone(void)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	NULL if we're out of memory, else
				a pointer to a clone (exact copy) of this object

	Purpose:	To copy PathProcessorBrush or derived-class object

********************************************************************************************/

PathProcessorBrush *PathProcessorBrush::Clone(void)
{

	// Clone this object 
	PathProcessorBrush *pClone = new PathProcessorBrush;

	if (pClone == NULL)
		return NULL;

	// copy the data
	pClone->SetBrushDefinition(m_BrushHandle);
	pClone->SetPathOffsetType(m_PathOffsetType);
	pClone->SetPathOffsetValue(m_PathOffsetValue);
	pClone->SetRotated(m_bRotate);
	pClone->SetRotationAngle(m_RotateAngle);
	pClone->SetRotationIncrConst(m_RotAngleIncrConst);
	pClone->SetRotationIncrProp(m_RotAngleIncrProp);
	pClone->SetRotationMaxPressure(m_RotationMaxPressure);

	pClone->SetSpacing(m_BrushSpacing);
	pClone->SetTiling(m_bTile);
	pClone->SetBrushScaling(m_BrushScaling);
	pClone->SetBrushScalingIncr(m_BrushScalingIncr);
	pClone->SetBrushScalingIncrConst(m_BrushScalingIncrConst);
	pClone->SetMaxScaling(m_MaxScaling);
	pClone->SetScalingMaxPressure(m_ScalingMaxPressure);
	pClone->SetSpacingIncrProp(m_BrushSpacingIncrProp);
	pClone->SetSpacingIncrConst(m_BrushSpacingIncrConst);
	pClone->SetSpacingMaxPressure(m_SpacingMaxPressure);
	pClone->SetPathOffsetIncrConst(m_PathOffsetIncrConst);
	pClone->SetPathOffsetIncrProp(m_PathOffsetIncrProp);
	pClone->SetSpacingMaxRand(m_BrushSpacingMaxRand);
	pClone->SetSpacingRandSeed(m_BrushSpacingRandSeed);
	pClone->SetScalingMaxRand(m_BrushScalingMaxRand);
	pClone->SetScalingRandSeed(m_BrushScalingRandSeed);
	pClone->SetSequenceType(m_SequenceType);
	pClone->SetSequenceSeed(m_SequenceRandSeed);
	pClone->SetOffsetValueMaxRand(m_OffsetValueMaxRand);
	pClone->SetOffsetValueRandSeed(m_OffsetValueRandSeed);
	pClone->SetOffsetTypeRandSeed(m_OffsetTypeRandSeed);

	pClone->SetRotationMaxRand(m_RotationMaxRand);
	pClone->SetRotationRandSeed(m_RotationRandSeed);

	pClone->SetUseLocalFillColour(m_bUseLocalFillColour);
	pClone->SetUseLocalTransp(m_bUseLocalTransp);
	pClone->SetUseNamedColours(m_bUseNamedColours);
	
	pClone->SetHueIncrement(m_BrushHueIncrement);
	pClone->SetHueMaxRand(m_BrushHueMaxRand);
	pClone->SetHueRandSeed(m_BrushHueRandSeed);
	pClone->SetSatIncrement(m_BrushSatIncrement);
	pClone->SetSatMaxRand(m_BrushSatMaxRand);
	pClone->SetSatRandSeed(m_BrushSatRandSeed);	

	pClone->SetBrushTransparency(m_BrushTransparency);
	pClone->SetTransparencyPressure(m_TranspMaxPressure);

	pClone->SetParentAttribute(m_pParentAttr);
	
	return(pClone);
}


/********************************************************************************************

>	void PathProcessorBrush::SetBrushDefinition(BrushHandle Handle)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/2/97

	Purpose:	Sets this path processor up to use the given vector Brush definition
				in all future rendering. 

	Notes:		If not set, or if set to BrushHandle_NoBrush, the Brush processor
				will render its Brushs as simple (flat filled) Brushs by calling the
				base class ProcessPath method.

********************************************************************************************/

void PathProcessorBrush::SetBrushDefinition(BrushHandle Handle)
{
	m_BrushHandle = Handle;	
}



/********************************************************************************************

>	BrushHandle PathProcessorBrush::GetBrushDefinitionHandle()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/99
	Inputs:		-
	Returns:	the handle currently used by this pathprocessor
	Purpose:	As above

********************************************************************************************/

BrushHandle PathProcessorBrush::GetBrushDefinitionHandle()
{
	return m_BrushHandle;
}


/********************************************************************************************

>	void PathProcessorBrush::SwitchAlternateValue()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/99
	Inputs:		-
	Returns:	switches the alternating flag used to determine which side of the path the 
				offset is on.  If you are using this PPB by calling RenderBrushAtPoint directly
				rather than passing in a path to ProcessPath and you want to have an alternating
				offset then you need to call this prior to RenderBrushAtPoint.
	Purpose:	

********************************************************************************************/

void PathProcessorBrush::SwitchAlternateValue()
{
	if (m_AltValue == 0)
		m_AltValue = 1;
	else
		m_AltValue = 0;
}


/********************************************************************************************

>	INT32 PathProcessorBrush::GetObjectCountToDistance(MILLIPOINT Distance)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/2000
	Inputs:		Distance along the path to find the object count for
	Returns:	the number of brush objects in the distance provided.
	Purpose:	To find out how many brush objects we will have in the distance given.  

********************************************************************************************/

INT32 PathProcessorBrush::GetObjectCountToDistance(MILLIPOINT Distance)
{
	if (Distance < 0)
		return -1;
	Reset();  // set our RNG back to the start
	MILLIPOINT DistanceSoFar = 0;
	INT32 Count = 0;
	while (DistanceSoFar < Distance)
	{
		DistanceSoFar += GetNextSpacing();
		Count++;
	}
	return Count;
}


/********************************************************************************************

>	MILLIPOINT PathProcessorBrush::GetDistanceToObject(UINT32 ObjectNum)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/2000
	Inputs:		ObjectNum - the object number that we want to get to
	Returns:	the distance along a path that we have to go before we reach ObjectNum
	Purpose:	Finds out at what we distance we place the ObjectNum'th object

********************************************************************************************/

MILLIPOINT PathProcessorBrush::GetDistanceToObject(UINT32 ObjectNum)
{
	MILLIPOINT Distance = 0;
	UINT32 ObjectCounter = 0;

	Reset(); // reset our RNG

	while (ObjectCounter < ObjectNum)
	{
		Distance += GetNextSpacing();
		ObjectCounter++;
	}
	return Distance;
}

/********************************************************************************************

>	MILLIPOINT PathProcessorBrush::GetSpacingAtDistance(MILLIPOINT Distance)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/2000
	Inputs:		Distance along the path 
	Returns:	the spacing to the next object, or -1 if Distance is invalid
	Purpose:	If we are Distance along our path then thif function finds out how far it
				would be to the next brush object.

********************************************************************************************/

MILLIPOINT PathProcessorBrush::GetSpacingAtDistance(MILLIPOINT Distance)
{
	if (Distance < 0)
		return -1;

	Reset(); // set our RNG

	MILLIPOINT DistanceSoFar = 0;
	
	while (DistanceSoFar <= Distance)
	{
		DistanceSoFar+= GetNextSpacing();
	}

	MILLIPOINT SpacingGap = DistanceSoFar - Distance;
	TRACEUSER( "Diccon", _T("Spacing at distance = %d\n"), SpacingGap);
	return SpacingGap;
}


/********************************************************************************************

>	BOOL GetSpacingAndScalingAtDistance(MILLIPOINT Distance, MILLIPOINT* pSpacing, double* pScaling,
										UINT32* pPressure)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/2000
	Inputs:		Distance along the path 
	Outputs:	pSpacing - the spacing at that distance
				pScaling - the scaling at that distance
				pPressure (bonus) - the pressure, defaults to NULL
	Returns:	the spacing to the next object, or -1 if Distance is invalid
	Purpose:	Retrieves the values for spacing, scaling and pressure, that are in effect at 
				Distance along the path.
				
	Notes:		This is a replacement for GetSpacingAtDistance which is necessary now that spacing
				is dependant on the scaling of each brush object.  This function duplicates the 
				loop found in ProcessPath, iterating through brush objects replicating the spacing and
				scaling values that would be found in the render loop.

	2nd Note:   The spacing value is a bit of a misnomer, what we really retrieve is the distance
				to the next object after Distance.  
********************************************************************************************/

BOOL PathProcessorBrush::GetSpacingAndScalingAtDistance(MILLIPOINT Distance, MILLIPOINT* pSpacing, 
														double* pScaling, UINT32* pPressure)

{
	// some checks
	ERROR2IF(pSpacing == NULL, FALSE, "Invalid input to PathProcessorBrush::GetSpacingAndScalingAtDistance");
	ERROR2IF(pScaling == NULL, FALSE, "Invalid input to PathProcessorBrush::GetSpacingAndScalingAtDistance");
	ERROR2IF(Distance < 0, FALSE, "Invalid input to PathProcessorBrush::GetSpacingAndScalingAtDistance");

	// first reset our members so we start at the beginning
	Reset();

	// get the pressure cache from our attribute, if there is one
	CDistanceSampler* pSampler = GetPressureCacheAndInitialise();
	CSampleItem TheItem;

	UINT32 Count = 0;
	m_CurrentScaling = GetNextScaling(Count);
	MILLIPOINT Spacing = GetNextSpacing();
	MILLIPOINT DistanceSoFar = 0;  // we start at 10 MP in from the start of the path, for consistenc

	while (DistanceSoFar <= Distance)
	{
		// increment our distance
#ifdef SCALESPACING
		DistanceSoFar += (MILLIPOINT)(Spacing * m_LastScaling);
#else
		DistanceSoFar += (MILLIPOINT)Spacing;
#endif
		
		// get the next pressure value, if there is one
		if (pSampler != NULL)
		{
			if (pSampler->GetNext(&TheItem))
			{
				if (TheItem.m_Pressure < MAXPRESSURE)
					m_CurrentPressure.m_Pressure = TheItem.m_Pressure;
			}
		}	
#ifdef SCALESPACING	
		if (pPressure)
			pSampler->SetSampleRateFromSpacing((INT32)(Spacing * m_LastScaling));
#else
		if (pPressure)
			pSampler->SetSampleRateFromSpacing(Spacing);
#endif
		
		// get the next spacing and scaling values
		Spacing = GetNextSpacing();
		m_CurrentScaling = GetNextScaling(Count);
	}

	// we want the distance to the next brush object
	MILLIPOINT DistanceGap = DistanceSoFar - Distance;
	if (Spacing < DistanceGap)
		DistanceGap = Spacing;

	// hopefully this little hack will deal with the case when we are adding to the start of the path
	if (DistanceGap == 0)
		DistanceGap = Spacing;

	// assign the data we've found
	*pSpacing = DistanceGap;
	*pScaling = m_CurrentScaling;
	if (pPressure != NULL)
		*pPressure = m_CurrentPressure.m_Pressure;

	return TRUE;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::AdvanceBrushToDistance(MILLIPOINT Distance)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/2000
	Inputs:		Distance along the path 
	
	Returns:	TRUE, unless something goes wrong
	Purpose:	Advances the member variables in this PPB to the values they would acquire if 
				the given distance were to be rendered.  Basically another variant on GetSpacing
				AtDistance
********************************************************************************************/

BOOL PathProcessorBrush::AdvanceBrushToDistance(MILLIPOINT Distance)
{
	ERROR2IF(Distance < 0, FALSE, "Invalid input to PathProcessorBrush::GetSpacingAndScalingAtDistance");
	BrushDefinition* pDef = BrushComponent::FindBrushDefinition(m_BrushHandle);
	ERROR2IF(pDef == NULL, FALSE,"No brush definition in PathProcessorBrush::AdvanceBrushToDistance");

	// first reset our members so we start at the beginning
	Reset();

	// get the pressure cache from our attribute, if there is one
	CDistanceSampler* pSampler = GetPressureCacheAndInitialise();
	CSampleItem TheItem;

	UINT32 Count = 0;
	
	UINT32 NumBrushObjects = pDef->GetNumBrushObjects();
	UINT32 BrushObject = m_LastObject;
	if (NumBrushObjects > 1 && m_SequenceType == SEQ_RANDOM)
		BrushObject = GetNextInkObject(m_LastObject, NumBrushObjects);

	m_CurrentScaling = GetNextScaling(Count++);
	MILLIPOINT Spacing = GetNextSpacing();
	MILLIPOINT DistanceSoFar = 0;  // we start at 10 MP in from the start of the path, for consistenc

	while (DistanceSoFar <= Distance)
	{
		// increment our distance
#ifdef SCALESPACING
		DistanceSoFar += (MILLIPOINT)(Spacing * m_CurrentScaling);
#else
		DistanceSoFar += (MILLIPOINT)Spacing;
#endif
		GetNextOffsetType();
		GetNextOffset();
		GetNextAngle();
		GetNextHueMultiplier();
		GetNextSaturationMultiplier();
		// get the next pressure value, if there is one
		if (pSampler != NULL)
		{
			if (pSampler->GetNext(&TheItem))
			{
				if (TheItem.m_Pressure < MAXPRESSURE)
					m_CurrentPressure.m_Pressure = TheItem.m_Pressure;
			}
#ifdef SCALESPACING
			pSampler->SetSampleRateFromSpacing((INT32)(Spacing * m_CurrentScaling));
#else
			pSampler->SetSampleRateFromSpacing(Spacing);
#endif
		}	
		
		
		// get the next spacing and scaling values
		Spacing = GetNextSpacing();
		m_CurrentScaling = GetNextScaling(Count++);

		if (m_AltValue == 0)
			m_AltValue = 1;
		else
			m_AltValue = 0;

		if (NumBrushObjects > 1)
			BrushObject = GetNextInkObject(BrushObject, NumBrushObjects);
	}
	m_LastObject = BrushObject;
	return TRUE;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetLastObject()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/2000
	Inputs:		-
	
	Returns:	the last object member
	Purpose:	The last object member keeps track of which object to render next as we proceed
				along the path.
********************************************************************************************/

UINT32 PathProcessorBrush::GetLastObject()
{
	return m_LastObject;
}



/********************************************************************************************

>	void PathProcessorBrush::AdjustSpacingForClosedPath(NodeRenderableInk* pInk)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/2000
	Inputs:		pInk - the ink node that this path processor will process
	
	Returns:	-
	Purpose:	When we apply a brush to a closed path we want to adjust the spacing so that 
				the distance between the start object and the end object is the same as the 
				distance between the start object and the second object.
				Here we check to see if the ink node is a single shape that produces a closed
				path (no groups etc.).  We will then get the length of that path and try 
				to get the best spacing value.
********************************************************************************************/

void PathProcessorBrush::AdjustSpacingForClosedPath(NodeRenderableInk* pInk)
{
	if (pInk == NULL)
	{
		ERROR3("Ink node is NULL in PathProcessorBrush::AdjustSpacingForClosedPath");
		return;
	}
	if (pInk->IsCompound())
		return;

	// Use a SimpleBecomeA to get the path from the parent object
	SimpleBecomeA BecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), NULL);
		
	if (pInk->DoBecomeA(&BecomeA))
	{
		NodePath* pNodePath = BecomeA.GetNodePath();
		if (pNodePath) 
		{
			if (pNodePath->InkPath.IsClosed())
			{
				AdjustSpacingForClosedPath(&(pNodePath->InkPath), pNodePath->InkPath.GetPathLength(750/2));
			}

			// delete the nodepath we got
			delete pNodePath;
		}
	}
		
}


/********************************************************************************************

>	void PathProcessorBrush::AdjustSpacingForClosedPath(Path* pPath, double dPathLength)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/2000
	Inputs:		pInk - the ink node that this path processor will process
	
	Returns:	-
	Purpose:	When we apply a brush to a closed path we want to adjust the spacing so that 
				the distance between the start object and the end object is the same as the 
				distance between the start object and the second object.
				Here we check to see if the ink node is a single shape that produces a closed
				path (no groups etc.).  We will then get the length of that path and try 
				to get the best spacing value.
********************************************************************************************/

void PathProcessorBrush::AdjustSpacingForClosedPath(Path* pPath, double dPathLength)
{
	if (pPath == NULL || !pPath->IsClosed())
		return;
	
///	double Length = pPath->GetPathLength();		// Don't keep calling this - It's complex!
			
	// what we want to do is set the spacing so that it places the first
	// and last object on the same spot

	// get the number of objects with the current spacing
	double ActualSpacing = m_BrushScaling * (double)m_BrushSpacing;
	INT32 NumObjects = (INT32)(dPathLength / ActualSpacing);

	// Ensure that the NumObjects value isn't equal to zero. This prevents a divide by
	// zero error from being thrown.
	if ( NumObjects == 0 )
	{
		NumObjects = 1;
	}

	// work out what the spacing would be if we used the whole of the path exactly
	double NewActualSpacing = dPathLength / (double)NumObjects;
	
	double NewSpacing = NewActualSpacing / m_BrushScaling;

	// set the new value
	SetSpacing((MILLIPOINT)NewSpacing);
}

/********************************************************************************************

>	CDistanceSampler* PathProcessorBrush::GetPressureCacheAndInitialise()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/2000
	Inputs:		-
	Returns:	pointer to the pressure data held by our attribute, or NULL if we fail
	Purpose:	This should be called to get the pressure cache from the attribute and to 
				set up the relevant members when we wish to start retrieving pressure data
				from the beginning.

********************************************************************************************/

CDistanceSampler* PathProcessorBrush::GetPressureCacheAndInitialise()
{
	// initialise to NULL
	CDistanceSampler* pPressure = NULL;
	
	// get the cache from our parent attribute
	if (m_pParentAttr != NULL)
		pPressure = m_pParentAttr->GetPressureCache();
	
	
	// initialise members to their starting values
	CSampleItem TheItem;
	
	// our first sample is a short distance in
	MILLIPOINT FirstDistance = 10;

	// set the default pressure to normal value
	m_CurrentPressure.m_Pressure = MAXPRESSURE / 2;
	if (pPressure == NULL)
		m_bValidPressure = FALSE;
	else
	{	
		if (pPressure->GetAt(0, &TheItem))
		{	
			if (TheItem.m_Pressure < 0 || TheItem.m_Pressure > MAXPRESSURE)
			{
				ERROR3("Invalid pressure here ");
			}
			else
			{
				m_CurrentPressure.m_Pressure = TheItem.m_Pressure;
				pPressure->SetSampleRateFromSpacing(FirstDistance);
			}
		}
		else
			ERROR3("Unable to retrieve first pressure value in PathProcessorBrush::ProcessPath");
		
		m_bValidPressure = TRUE;
	}
	return pPressure;
}


/********************************************************************************************

>	CDistanceSampler* PathProcessorBrush::GetPressureCacheAndInitialise(RenderRegion* pRegion, 
											ValueFunction** ppValFunc)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/2000
	Inputs:		pRegion - the render region we are about to render into
	Outputs:	ppValFunc - if a non-constant variable width attribute has been rendered into
				pRegion then we return a pointer to it
	Returns:	pointer to the pressure data held by our attribute, or NULL if either
				we don't have a pressure cache OR the render region contains a variable width
				attribute with a non-constant value function.

	Purpose:	This should be called to get the pressure cache from the attribute and to 
				set up the relevant members when we wish to start retrieving pressure data
				from the beginning.

				This overridden version allows us to use applied variable width strokes to 
				provide pressure information instead of the input pressure data.  If we find
				one of these has been rendered into the render region we will use this in preferenve
				to the cache found in the AttrBrushType.

********************************************************************************************/

CDistanceSampler* PathProcessorBrush::GetPressureCacheAndInitialise(RenderRegion* pRegion, ValueFunction** ppValFunc)
{
	ERROR2IF(pRegion == NULL, NULL, "Render region is NULL in PathProcessorBrush::GetPressureCacheAndInitialise");

	ValueFunction* pValFunc = NULL;

	// first look for the variable width function
	VariableWidthAttrValue *pVarWidthAttr = (VariableWidthAttrValue *) pRegion->GetCurrentAttribute(ATTR_VARWIDTH);
	if (pVarWidthAttr != NULL)
		pValFunc = pVarWidthAttr->GetWidthFunction();

	
	// If it is a constant width stroke, then we ignore it, try to get the cache from the attribute instead
	if (pValFunc == NULL || IS_A(pValFunc, ValueFunctionConstant))
		return GetPressureCacheAndInitialise();
	else
	{
		// set our starting pressure value

		// set the default pressure to normal value
		m_CurrentPressure.m_Pressure = MAXPRESSURE / 2;
		
		// get the first value out of the value function
		double Val = pValFunc->GetValue(0);
		
		m_CurrentPressure.m_Pressure = (UINT32)((double)MAXPRESSURE * Val);

		// assign it to the output pointer
		*ppValFunc = pValFunc;

		m_bValidPressure = TRUE;
	}
	return NULL;
}


/********************************************************************************************

>	CDistanceSampler* PathProcessorBrush::GetPressureCacheAndInitialise(RenderRegion* pRegion, 
											ValueFunction** ppValFunc)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/2000
	Inputs:		pInkNode - the inknode that hsa this brush applied to it
	Outputs:	ppValFunc - if a non-constant variable width attribute is applied to pInkNode then
				we will return a pointer to it

	Returns:	pointer to the pressure data held by our attribute, or NULL if either
				we don't have a pressure cache OR a non-constant VWF is applied to our inknoded

	Purpose:	This should be called to get the pressure cache from the attribute and to 
				set up the relevant members when we wish to start retrieving pressure data
				from the beginning.

				This overridden version is designed to be use when no render region is available
				but we still need pressure info, for example when calculating bounding boxes
				or DoBecomeA's.   It is a little slower as we have to search for the applied
				attribute.

********************************************************************************************/

CDistanceSampler* PathProcessorBrush::GetPressureCacheAndInitialise(NodeRenderableInk* pInkNode, 
																	ValueFunction** ppValFunc)
{
	ERROR2IF(pInkNode == NULL, NULL, "Ink node is NULL in PathProcessorBrush::GetPressureCacheAndInitialise");

	ValueFunction* pValFunc = NULL;
	AttrVariableWidth* pVarWidth = NULL;
	
	if (pInkNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth), (NodeAttribute**)&pVarWidth))
	{
		VariableWidthAttrValue* pVal = (VariableWidthAttrValue*)pVarWidth->GetAttributeValue();
		pValFunc = pVal->GetWidthFunction();
	}
	
		// If it is a constant width stroke, then we ignore it, try to get the cache from the attribute instead
	if (pValFunc == NULL || IS_A(pValFunc, ValueFunctionConstant))
		return GetPressureCacheAndInitialise();
	else
	{
		// set our starting pressure value

		// set the default pressure to normal value
		m_CurrentPressure.m_Pressure = MAXPRESSURE / 2;
		
		// get the first value out of the value function
		double Val = pValFunc->GetValue(0);
		
		m_CurrentPressure.m_Pressure = (UINT32)((double)MAXPRESSURE * Val);

		// assign it to the output pointer
		*ppValFunc = pValFunc;

		m_bValidPressure = TRUE;
	}


	return NULL;
}


/********************************************************************************************

>	CSampleItem PathProcessorBrush::GetNextSampleItem(CDistanceSampler* pSampler, ValueFunction* pValFunc, 
												double PathProportion = 0)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/2000
	Inputs:		pSampler - the pressure cache that we may be using
				pValFunc - the value function that we may be using
				PathProportion - if you are using a value function then you need to provide this
	Returns:	The next item to use from either the value function or the cache.  Note that we
				will use the value function in preference to the cache.

	Purpose:	Your one stop shop for getting the next pressure value to use.  Note that we may not
				be using pressure at all in which case all of the inputs will be NULL 

********************************************************************************************/

CSampleItem PathProcessorBrush::GetNextSampleItem(CDistanceSampler* pSampler, ValueFunction* pValFunc, 
												double PathProportion)
{
	if (PathProportion > 1.0)
		PathProportion = 0.98;
	if (PathProportion < 0.0)
		PathProportion = 0.0;

	// make a default item 
	CSampleItem TheItem;
	TheItem.m_Pressure = MAXPRESSURE / 2;

	// if we have a value function then use that
	if (pValFunc != NULL)
	{
		double Val = pValFunc->GetValue(PathProportion);
		TheItem.m_Pressure = (UINT32)((double)MAXPRESSURE * Val);
	//	TRACEUSER( "Diccon", _T("Sample pressure = %d\n"), TheItem.m_Pressure);
	}
	else if (pSampler != NULL) // otherwise use the sampler
	{
		pSampler->GetNext(&TheItem);
	}

	return TheItem;
}


/*------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------
------------------THE DATA ACCESS FUNCTIONS-------------------------------------------------
-------------------------------------------------------------------------------------------*/

/********************************************************************************************

>	BOOL PathProcessorBrush::SetBrushSpacing(double Spacing) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the spacing value to set
	Returns:	TRUE if successful, FALSE if the spacing is invalid 
	Purpose:	To set the spacing between brush objects
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetSpacing(MILLIPOINT Spacing)
{
	if (Spacing < MIN_BRUSH_SPACING || Spacing > MAX_BRUSH_SPACING)
		return FALSE;

	m_BrushSpacing = Spacing;
	m_LastSpacing = Spacing;
	m_LastSpacingNoRandom = Spacing;
//	TRACEUSER( "Diccon", _T("Spacing Set at %d\n"), m_BrushSpacing);

	return TRUE;

}



/********************************************************************************************

>	MILLIPOINT PathProcessorBrush::GetBrushSpacing() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the spacing between the brush objects 
	Purpose:	As above

********************************************************************************************/

MILLIPOINT PathProcessorBrush::GetSpacing()
{
	return m_BrushSpacing;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetSpacingIncrProp(double Incr) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the spacing increment value to set
	Returns:	TRUE if we can draw 10 objects, FALSE if the spacing is invalid 
	Purpose:	To set the spacing increment between brush objects
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetSpacingIncrProp(double Incr)
{
	if (Incr <= 0)
		return FALSE;

	m_BrushSpacingIncrProp = Incr;
	return TRUE;
}



/********************************************************************************************

>	MILLIPOINT PathProcessorBrush::GetSpacingIncrProp() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the spacing between the brush objects 
	Purpose:	As above

********************************************************************************************/

double PathProcessorBrush::GetSpacingIncrProp()
{
	return m_BrushSpacingIncrProp;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetSpacingIncrConst(MILLIPOINT Incr) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the constant spacing increment value to set
	Returns:	TRUE if we can draw 10 objects, FALSE if the spacing is invalid 
	Purpose:	To set the proportional spacing increment between brush objects
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetSpacingIncrConst(MILLIPOINT Incr)
{

	m_BrushSpacingIncrConst = Incr;
	return TRUE;
}


/********************************************************************************************

>	MILLIPOINT PathProcessorBrush::GetSpacingIncrConst() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the constant spacing increment between the brush objects 
	Purpose:	As above

********************************************************************************************/

MILLIPOINT PathProcessorBrush::GetSpacingIncrConst()
{
	return m_BrushSpacingIncrConst;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetSpacingMaxRand() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the maximum percentage randomness to be applied to spacing 
	Purpose:	As above

********************************************************************************************/

UINT32 PathProcessorBrush::GetSpacingMaxRand()
{
	return m_BrushSpacingMaxRand;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetSpacingMaxRand(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs::	the maximum percentage randomness to be applied to spacing 
	Returns:	TRUE if Value is within the legal limits, otherwise FALSE
	Purpose:	As above

********************************************************************************************/

BOOL PathProcessorBrush::SetSpacingMaxRand(UINT32 Value)
{
	if (Value < MIN_BRUSH_RAND || Value > MAX_BRUSH_RAND)
		return FALSE;
	
	m_BrushSpacingMaxRand = Value;
	return TRUE;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetSpacingRandSeed() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the seed used to determine the sequence of random numbers for spacing
	Purpose:	As above

********************************************************************************************/

UINT32 PathProcessorBrush::GetSpacingRandSeed()
{
	return m_BrushSpacingRandSeed;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetSpacingRandSeed(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs	:	the seed used to determine the sequence of random numbers for spacing
	Returns:	TRUE if Value is valid
	Purpose:	As above
				
********************************************************************************************/

BOOL PathProcessorBrush::SetSpacingRandSeed(UINT32 Value)
{
#ifdef USE_MTRNG
	if(m_pSpacingRNG == NULL)
	{
		m_pSpacingRNG = new MTRand((UINT32)Value);
		if (m_pSpacingRNG == NULL)
			return FALSE;
	}
	else
		m_pSpacingRNG->seed((UINT32)Value);
#endif
	m_BrushSpacingRandSeed = Value;
	return TRUE;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetSpacingMaxPressure(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs	:	the maximum % effect on spacing that pressure will have
	Returns:	TRUE if Value is valid
	Purpose:	As above
				
********************************************************************************************/

BOOL PathProcessorBrush::SetSpacingMaxPressure(UINT32 Max)
{
	if (Max < MIN_PRESSURE_EFFECT || Max > MAX_PRESSURE_EFFECT)
		return FALSE;

	m_SpacingMaxPressure = Max;
	return TRUE;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetSpacingMaxPressure() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs	:	-
	Returns:	the maximum effect that pressure will have on spacing
	Purpose:	As above
				
********************************************************************************************/

UINT32 PathProcessorBrush::GetSpacingMaxPressure()
{
	return m_SpacingMaxPressure;
}



/********************************************************************************************

>	MILLIPOINT PathProcessorBrush::GetLastSpacing()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs	:	-
	Returns:	the maximum effect that pressure will have on spacing
	Purpose:	As above
				
********************************************************************************************/

MILLIPOINT PathProcessorBrush::GetLastSpacing()
{
	return m_LastSpacing;
}

/********************************************************************************************

>	PathOffset PathProcessorBrush::GetPathOffsetType() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	The type of pathoffset this brush is using 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

PathOffset PathProcessorBrush::GetPathOffsetType()
{
	return m_PathOffsetType;
}



/********************************************************************************************

>	void PathProcessorBrush::SetPathOffsetType(PathOffset Offset) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs		Offset - the offset type to set
	Returns:	- 
	Purpose:	to set the offset type for this brush
********************************************************************************************/

void PathProcessorBrush::SetPathOffsetType(PathOffset Offset)
{
#ifdef USE_MTRNG
	if (m_pOffsetTypeRNG == NULL)
	{
		m_pOffsetTypeRNG = new MTRand;
		if (m_pOffsetTypeRNG == NULL)
			return;
	}
	m_pOffsetTypeRNG->seed((UINT32)m_OffsetTypeRandSeed);
#endif

	m_PathOffsetType = Offset;
}



/********************************************************************************************

>	MILLIPOINT PathProcessorBrush::GetPathOffsetValue() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	The value of the path offset used by this brush 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

MILLIPOINT PathProcessorBrush::GetPathOffsetValue()
{
	return m_PathOffsetValue;
}



/********************************************************************************************

>	void PathProcessorBrush::SetPathOffsetValue(MILLIPOINT Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs		Value - the offset value to set
	Returns:	- 
	Purpose:	to set the offset distance for this brush
********************************************************************************************/
void PathProcessorBrush::SetPathOffsetValue(MILLIPOINT Value)
{
	if (Value < 0 || Value > MAX_BRUSH_SPACING)
	{
	//	ERROR3("invalid offset value");
		return;
	}

	m_PathOffsetValue = Value;
	m_LastOffset = Value;
	m_LastOffsetNoRandom  = m_PathOffsetValue;
}


/********************************************************************************************

>	MILLIPOINT PathProcessorBrush::GetPathOffsetIncrConst() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	The value of the constant path offset increment used by this brush 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

MILLIPOINT PathProcessorBrush::GetPathOffsetIncrConst()
{
	return m_PathOffsetIncrConst;
}



/********************************************************************************************

>	BOOL PathProcessorBrush::SetPathOffsetIncrConst(MILLIPOINT Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs		Value - the offset increment value to set
	Returns:	- 
	Purpose:	to set the offset distance for this brush
********************************************************************************************/
BOOL PathProcessorBrush::SetPathOffsetIncrConst(MILLIPOINT Value)
{
	m_PathOffsetIncrConst = Value;
	return TRUE;
}


/********************************************************************************************

>	double PathProcessorBrush::GetPathOffsetIncrProp() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	The value of the proportional path offset increment used by this brush 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetPathOffsetIncrProp()
{
	return m_PathOffsetIncrProp;
}



/********************************************************************************************

>	BOOL PathProcessorBrush::SetPathOffsetIncrProp(double Incr) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs		incr - the proportional offset increment value to set
	Returns:	- 
	Purpose:	to set the offset distance for this brush
********************************************************************************************/
BOOL PathProcessorBrush::SetPathOffsetIncrProp(double Incr)
{
	if (Incr <= 0 )
		return FALSE;
	// test to see if 10 objects takes us to the limit
	// Don't do this anymore, as we limit our spacing at render-time
/*	double TenIncr = pow(Incr, 10.0);
	if ((m_PathOffsetValue * TenIncr) >= MAX_BRUSH_SPACING)
		return FALSE;
*/
	m_PathOffsetIncrProp = Incr;
	return TRUE;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetOffsetValueMaxRand() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the maximum percentage randomness to be applied to Offset
	Purpose:	As above

********************************************************************************************/

UINT32 PathProcessorBrush::GetOffsetValueMaxRand()
{
	return m_OffsetValueMaxRand;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetOffsetValueMaxRand(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs::	the maximum percentage randomness to be applied to Offset
	Returns:	TRUE if Value is within the legal limits, otherwise FALSE
	Purpose:	As above

********************************************************************************************/

BOOL PathProcessorBrush::SetOffsetValueMaxRand(UINT32 Value)
{
	if (Value < MIN_BRUSH_RAND || Value > MAX_BRUSH_RAND)
		return FALSE;
	
	m_OffsetValueMaxRand = Value;
	return TRUE;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetOffsetValueRandSeed() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the seed used to determine the sequence of random numbers for offset value
	Purpose:	As above

********************************************************************************************/

UINT32 PathProcessorBrush::GetOffsetValueRandSeed()
{
	return m_OffsetValueRandSeed;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetOffsetValueRandSeed(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/9
	Inputs	:	the seed used to determine the sequence of random numbers for offset value
	Returns:	TRUE if Value is valid, which it always is at the moment
	Purpose:	As above
				
********************************************************************************************/

BOOL PathProcessorBrush::SetOffsetValueRandSeed(UINT32 Value)
{
#ifdef USE_MTRNG
	if(m_pOffsetValueRNG == NULL)
	{
		m_pOffsetValueRNG = new MTRand((UINT32)Value);
		if (m_pOffsetValueRNG == NULL)
			return FALSE;
	}
	else
		m_pOffsetValueRNG->seed((UINT32)Value);
#endif
	m_OffsetValueRandSeed = Value;
	return TRUE;

}





/********************************************************************************************

>	UINT32 PathProcessorBrush::GetOffsetTypeRandSeed() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the seed used to determine the sequence of random numbers for offset value
	Purpose:	As above

********************************************************************************************/

UINT32 PathProcessorBrush::GetOffsetTypeRandSeed()
{
	return m_OffsetTypeRandSeed;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetOffsetTypeRandSeed(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/9
	Inputs	:	the seed used to determine the sequence of random numbers for offset value
	Returns:	TRUE if Value is valid, which it always is at the moment
	Purpose:	As above
				
********************************************************************************************/
BOOL PathProcessorBrush::SetOffsetTypeRandSeed(UINT32 Value)
{
#ifdef USE_MTRNG
	if(m_pOffsetTypeRNG == NULL)
	{
		m_pOffsetTypeRNG = new MTRand((UINT32)Value);
		if (m_pOffsetTypeRNG == NULL)
			return FALSE;
	}
	else
		m_pOffsetTypeRNG->seed((UINT32)Value);
#endif

	m_OffsetTypeRandSeed = Value;
	return TRUE;

}

/********************************************************************************************

>	void PathProcessorBrush::SetTiling(BOOL Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     whether or not the brush tiles
	Returns:	-
	Purpose:	When a brush tiles it means that the attribute types:
				- 3 colour fill
				- 4 colour fill
				- bitmap fill
				are only rendered once per brush, rather than for every brush step
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::SetTiling(BOOL Value)
{
	m_bTile = Value;
}



/********************************************************************************************

>	BOOL PathProcessorBrush::GetTiling() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     
	Returns:	whether or not the brush tiles
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::IsTiled()
{
	return m_bTile;
}



/********************************************************************************************

>	BOOL PathProcessorBrush::SetRotationAngle(double Angle) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the angle of rotation to set
	Returns:	TRUE if the angle is within the stated bounds
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetRotationAngle(double Angle)
{
	/*if (Angle <= MIN_ANGLE || Angle >= MAX_ANGLE)
	{
		ERROR3("Invalid angle");
		return FALSE;
	
	}*/
	m_RotateAngle = Angle;
	m_LastAngle   = m_RotateAngle;
	m_LastRotationNoRandom = m_RotateAngle;
	return TRUE;
}

/********************************************************************************************

>	double PathProcessorBrush::GetRotationAngle() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the angle of rotation that has been set
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetRotationAngle()
{
	return m_RotateAngle;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetRotAngleIncrConst(double Angle) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the incremental angle of rotation to set
	Returns:	TRUE if the angle is within the stated bounds
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetRotationIncrConst(double Angle)
{
	m_RotAngleIncrConst = Angle;

	return TRUE;
}

/********************************************************************************************

>	double PathProcessorBrush::GetRotAngleIncrConst() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the increment to the angle of rotation 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetRotationIncrConst()
{
	return m_RotAngleIncrConst;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetRotAngleIncrProp(double Angle) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the incremental angle of rotation to set
	Returns:	TRUE if the angle is within the stated bounds
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetRotationIncrProp(double Value)
{
	m_RotAngleIncrProp = Value;

	return TRUE;
}

/********************************************************************************************

>	double PathProcessorBrush::GetRotAngleIncrProp() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the increment to the angle of rotation 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetRotationIncrProp()
{
	return m_RotAngleIncrProp;
}

/********************************************************************************************

>	UINT32 PathProcessorBrush::GetRotationMaxRand() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the maximum amount of randomness to be applied to the rotation angle
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 PathProcessorBrush::GetRotationMaxRand()
{
	return m_RotationMaxRand;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetRotationMaxRand(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the maximum amount of randomness (as a %) to be applied to rotation
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetRotationMaxRand(UINT32 Value)
{
	m_RotationMaxRand = Value;
	return TRUE;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetRotationRandSeed() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the  seed to use to generate the  rotation RNG
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 PathProcessorBrush::GetRotationRandSeed()
{
	return m_RotationRandSeed;
}



/********************************************************************************************

>	BOOL PathProcessorBrush::SetRotationRandSeed(UINT32 Seed) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the seed to use
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::SetRotationRandSeed(UINT32 Seed)
{
#ifdef USE_MTRNG
	if (m_pRotationRNG == NULL)
	{
		m_pRotationRNG = new MTRand;
		if (m_pRotationRNG == NULL)
			return;
	}
	m_pRotationRNG->seed((UINT32)Seed);
#endif
	m_RotationRandSeed = Seed;
}



/********************************************************************************************

>	UINT32 PathProcessorBrush::GetRotationMaxPressure() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the  seed to use to generate the  rotation RNG
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 PathProcessorBrush::GetRotationMaxPressure()
{
	return m_RotationMaxPressure;
}



/********************************************************************************************

>	BOOL PathProcessorBrush::SetRotationMaxPressure(UINT32 Seed) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the seed to use
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetRotationMaxPressure(UINT32 Value)
{
	if (Value < MIN_PRESSURE_EFFECT || Value > MAX_PRESSURE_EFFECT)
		return FALSE;
	m_RotationMaxPressure = Value;
	return TRUE;
}

/********************************************************************************************

>	BOOL PathProcessorBrush::IsRotated() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     
	Returns:	whether or not the brush objects are rotated
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::IsRotated()
{
	return m_bRotate;
}


/********************************************************************************************

>	void PathProcessorBrush::SetRotation(BOOL Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     whether or not the brush rotates
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::SetRotated(BOOL Value)
{
	m_bRotate = Value;
}


/********************************************************************************************

>	double PathProcessorBrush::GetBrushScaling() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the scaling factor to use when drawing the objects
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetBrushScaling()
{
	return m_BrushScaling;
}



/********************************************************************************************

>	bool PathProcessorBrush::SetScaling(double Scale) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the scale factor to set
	Returns:	TRUE if Scale is within the permitted bounds
	Purpose:	to set the member that determine to what scale of the original object each 
				brush object is drawn
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetBrushScaling(double Scale)
{
	/*if (Scale <= MIN_BRUSH_SCALE )
	{
		//InformError(_R(IDS_BRUSH_SCALE_TOOSMALL));
		m_BrushScaling = MIN_BRUSH_SCALE;
		return TRUE;
	}
	else*/ if (Scale >= MAX_BRUSH_SCALE)
	{
		//InformError(_R(IDS_BRUSH_SCALE_TOOBIG));
		m_BrushScaling = MAX_BRUSH_SCALE;
		return TRUE;
	}
	
	m_BrushScaling = Scale;
	m_CurrentScaling = Scale;
	m_LastScaling = Scale;
	m_LastScalingNoRandom = Scale;
	return TRUE;
}


/********************************************************************************************

>	double PathProcessorBrush::GetBrushScalingIncr() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the scaling factor increment to use when drawing the objects
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetBrushScalingIncr()
{
	return m_BrushScalingIncr;
}



/********************************************************************************************

>	bool PathProcessorBrush::SetBrushScalingincr(double incr) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the scale factor increment to set
	Returns:	TRUE if the increment allows 10 objects to be drawn 
	Purpose:	to set the member that determine the increment to scaling applied
				to each brush object
				Note that the scaling MUST be set before this is called
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetBrushScalingIncr(double Incr)
{
	if (Incr <= 0)
	{
		//ERROR3("Negative scaling increment");
		return FALSE;
	}

	m_BrushScalingIncr = Incr;
	return TRUE;
}


/********************************************************************************************

>	double PathProcessorBrush::GetBrushScalingIncrConst() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the scaling factor increment to use when drawing the objects
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetBrushScalingIncrConst()
{
	return m_BrushScalingIncrConst;
}

/********************************************************************************************

>	bool PathProcessorBrush::SetBrushScalingincrConst(double Incr) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the scale factor increment to set
	Returns:	TRUE unless Incr >= 100
	Purpose:	to set the member that determine the increment to scaling applied
				to each brush object
				
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetBrushScalingIncrConst(double Incr)
{
	if (Abs(Incr) >= 100)
	{
		return FALSE;
	}
	m_BrushScalingIncrConst = Incr;
	return TRUE;
}

/********************************************************************************************

>	UINT32 PathProcessorBrush::GetScalingMaxRand() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the maximum percentage randomness to be applied to Scaling 
	Purpose:	As above

********************************************************************************************/

UINT32 PathProcessorBrush::GetScalingMaxRand()
{
	return m_BrushScalingMaxRand;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetScalingMaxRand(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs::	the maximum percentage randomness to be applied to Scaling 
	Returns:	TRUE if Value is within the legal limits, otherwise FALSE
	Purpose:	As above

********************************************************************************************/

BOOL PathProcessorBrush::SetScalingMaxRand(UINT32 Value)
{
	if (Value < MIN_BRUSH_RAND || Value > MAX_BRUSH_RAND)
		return FALSE;
	
	m_BrushScalingMaxRand = Value;
	return TRUE;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetScalingRandSeed() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the seed used to determine the sequence of random numbers for Scaling
	Purpose:	As above

********************************************************************************************/

UINT32 PathProcessorBrush::GetScalingRandSeed()
{
	return m_BrushScalingRandSeed;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetScalingRandSeed(UINT32 Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs	:	the seed used to determine the sequence of random numbers for spacing
	Returns:	TRUE if Value is valid, which it always is at the moment
	Purpose:	As above
				
********************************************************************************************/

BOOL PathProcessorBrush::SetScalingRandSeed(UINT32 Value)
{
#ifdef USE_MTRNG
	if(m_pScalingRNG == NULL)
	{
		m_pScalingRNG = new MTRand((UINT32)Value);
		if (m_pScalingRNG == NULL)
			return FALSE;
	}
	else
		m_pScalingRNG->seed((UINT32)Value);
#endif
	m_BrushScalingRandSeed = Value;
	return TRUE;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetScalingMaxPressure() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the maximum percentage effect that pressure will have on Scaling 
	Purpose:	As above

********************************************************************************************/

UINT32 PathProcessorBrush::GetScalingMaxPressure()
{
	return m_ScalingMaxPressure;
}


/********************************************************************************************

>	double PathProcessorBrush::GetLastScaling()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the last scaling value that was generated
	Purpose:	As above

********************************************************************************************/

double PathProcessorBrush::GetLastScaling()
{
	return m_LastScaling;
}

/********************************************************************************************

>	BOOL PathProcessorBrush::SetScalingMaxPressure(UINT32 Pressure) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		the maximum percentage effect that pressure will have on Scaling 
	Returns:	-
	Purpose:	As above

********************************************************************************************/

BOOL PathProcessorBrush::SetScalingMaxPressure(UINT32 Max)
{
	if (Max < MIN_PRESSURE_EFFECT || Max > MAX_PRESSURE_EFFECT)
		return FALSE;
	m_ScalingMaxPressure = Max;
	return TRUE;
}


/********************************************************************************************

>	double PathProcessorBrush::GetHueIncrement()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the hue increment for this brush
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetHueIncrement()
{
	return m_BrushHueIncrement;
}


/********************************************************************************************

>	double PathProcessorBrush::GetHueIncrement()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the hue increment for this brush
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetHueIncrement(double Incr)
{
	m_BrushHueIncrement = Incr;
	return TRUE;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetHueMaxRand()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the hue increment random amount for this brush
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 PathProcessorBrush::GetHueMaxRand()
{
	return m_BrushHueMaxRand;
}



/********************************************************************************************

>	BOOL PathProcessorBrush::SetHueMaxRand(UINT32 Rand)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the hue increment random amount for this brush
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetHueMaxRand(UINT32 Rand)
{
	m_BrushHueMaxRand = Rand;
	return TRUE;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetHueRandSeed()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the hue increment random amount for this brush
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 PathProcessorBrush::GetHueRandSeed()
{
	return m_BrushHueRandSeed;
}



/********************************************************************************************

>	BOOL PathProcessorBrush::SetHueRandSeed(UINT32 Rand)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the hue increment random amount for this brush
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetHueRandSeed(UINT32 Rand)
{
	m_BrushHueRandSeed = Rand;
	
#ifdef USE_MTRNG
	if(m_pHueRNG == NULL)
	{
		m_pHueRNG = new MTRand((UINT32)Rand);
		if (m_pHueRNG == NULL)
			return FALSE;
	}
	else
		m_pHueRNG->seed((UINT32)Rand);
#endif
	return TRUE;
}


/********************************************************************************************

>	double PathProcessorBrush::GetSatIncrement()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the Saturation increment for this brush
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetSatIncrement()
{
	return m_BrushSatIncrement;
}


/********************************************************************************************

>	double PathProcessorBrush::GetSatIncrement()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the Saturation increment for this brush
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetSatIncrement(double Incr)
{
	m_BrushSatIncrement = Incr;
	return TRUE;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetSatMaxRand()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the Saturation increment random amount for this brush
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 PathProcessorBrush::GetSatMaxRand()
{
	return m_BrushSatMaxRand;
}



/********************************************************************************************

>	BOOL PathProcessorBrush::SetSatMaxRand(UINT32 Rand)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the Saturation increment random amount for this brush
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetSatMaxRand(UINT32 Rand)
{
	m_BrushSatMaxRand = Rand;
	return TRUE;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetSatRandSeed()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the Saturation increment random amount for this brush
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 PathProcessorBrush::GetSatRandSeed()
{
	return m_BrushSatRandSeed;
}



/********************************************************************************************

>	BOOL PathProcessorBrush::SetSatRandSeed(UINT32 Rand)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the Saturation increment random amount for this brush
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetSatRandSeed(UINT32 Rand)
{
	m_BrushSatRandSeed = Rand;

#ifdef USE_MTRNG
	if(m_pSaturationRNG == NULL)
	{
		m_pSaturationRNG = new MTRand((UINT32)Rand);
		if (m_pSaturationRNG == NULL)
			return FALSE;
	}
	else
		m_pSaturationRNG->seed((UINT32)Rand);
#endif
	return TRUE;
}

/********************************************************************************************

>	SequenceType PathProcessorBrush::GetSequenceType() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the type of sequence we are using for mutliple ink objects
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

SequenceType PathProcessorBrush::GetSequenceType()
{
	return m_SequenceType;
}


/********************************************************************************************

>	void PathProcessorBrush::SetSequenceType(SequenceType Type) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the type of sequence to use
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::SetSequenceType(SequenceType Type)
{
	m_SequenceType = Type;
	if (Type == SEQ_MIRROR)
		m_MirrorSeqAscend = TRUE;

	// if we're going random then initialise the RNG
	if (m_SequenceType == SEQ_RANDOM)
		SetSequenceSeed(m_SequenceRandSeed);
}


/********************************************************************************************

>	void PathProcessorBrush::SetSequenceSeed(UINT32 Seed) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the random seed to use for sequences
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::SetSequenceSeed(UINT32 Seed)
{
	m_SequenceRandSeed = Seed;
	if (m_SequenceType == SEQ_RANDOM)
	{
#ifdef USE_MTRNG
		if (m_pSequenceRNG == NULL)
		{
			m_pSequenceRNG = new MTRand;
			if (m_pSequenceRNG == NULL)
				return;
		}
		m_pSequenceRNG->seed((UINT32)Seed);
	}
#endif

	
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetSequenceSeed) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the seed we are using to generate random sequences
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 PathProcessorBrush::GetSequenceSeed()
{
	return m_SequenceRandSeed;
}




/********************************************************************************************

>	UINT32 PathProcessorBrush::GetNumBrushObjects() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the number of objects used by our brush definition, or zero if something goes wrong
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

UINT32 PathProcessorBrush::GetNumBrushObjects()
{
	BrushDefinition* pBrushDef = GetOurBrushDefinition();
	if (pBrushDef == NULL)
		return 0;
	return pBrushDef->GetNumBrushObjects();
}


/********************************************************************************************

>	BOOL PathProcessorBrush::GetUseLocalFillColour() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the flag saying whether or not to use our local fill when rendering
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::GetUseLocalFillColour()
{
	return m_bUseLocalFillColour;
}


/********************************************************************************************

>	void PathProcessorBrush::SetUseLocalFillColour(BOOL UseLocal) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the flag saying whether or not to use our local fill
	Returns:	
	Purpose:	if this flag is set to TRUE, then the cached fill will not be used when rendering.
				Instead the nearest stroke colour will be rendered.  This is so that people can 
				drop colours onto the brush.
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::SetUseLocalFillColour(BOOL UseLocal)
{
	m_bUseLocalFillColour = UseLocal;
	
	// we want to switch our named colours option off, the semantics of this seem a little
	// wrong because we are not going to use named colours, perhaps I need a new name
	if (UseLocal)
		m_bUseNamedColours = TRUE;
}


/********************************************************************************************

>	INT32 PathProcessorBrush::GetBrushTransparency()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	our transparency value (-1 indicates unused)
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

INT32 PathProcessorBrush::GetBrushTransparency()
{
	return m_BrushTransparency;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetBrushTransparency(INT32 Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Value - the value to set
	Returns:	our transparency value (-1 indicates unused)
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetBrushTransparency(INT32 Value)
{
	if (Value > MAX_TRANSP_VALUE)
		return FALSE;

	m_BrushTransparency = Value;
	return TRUE;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetTransparencyPressure(UINT32 Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Value - the value to set
	Returns:	
	Purpose:	Sets the extent to which pressure affects transparency
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetTransparencyPressure(UINT32 Value)
{
	m_TranspMaxPressure = Value;
	return TRUE;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetTransparencyPressure(UINT32 Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Value - the value to set
	Returns:	
	Purpose:	Sets the extent to which pressure affects transparency
	SeeAlso:	-

********************************************************************************************/

UINT32 PathProcessorBrush::GetTransparencyPressure()
{
	return m_TranspMaxPressure;
}

/********************************************************************************************

>	BOOL PathProcessorBrush::GetUseLocalFillTransp() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the flag saying whether or not to use our local transparency
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::GetUseLocalTransp()
{
	return m_bUseLocalTransp;
}


/********************************************************************************************

>	void PathProcessorBrush::SetUseLocalTransp(BOOL UseLocal) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the flag saying whether or not to use our local transparency
	Returns:	
	Purpose:	if this flag is set to TRUE then any cached transparencies will not be rendered.
				Instead the transparency applied to our parent ink object will be rendered.
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::SetUseLocalTransp(BOOL UseLocal)
{
	m_bUseLocalTransp = UseLocal;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::GetUseNamedColours() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the flag saying whether or not to use our named colours when rendering
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::GetUseNamedColours()
{
	return m_bUseNamedColours;
}


/********************************************************************************************

>	void PathProcessorBrush::SetUseNamedColours(BOOL Value

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the flag saying whether or not to use our named colours
	Returns:	
	Purpose:	if this flag is set to FALSE then any named colours in our brush will not be 
				rendered, instead we will use the local stroke colour
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::SetUseNamedColours(BOOL Value)
{
	m_bUseNamedColours = Value;
	if (!Value)
		m_bUseLocalFillColour = FALSE;
}


/********************************************************************************************

>	void PathProcessorBrush::CopyBrushData(BrushData* pData) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		-
	Returns:	pData - pointer to the brushdata object
	Purpose:	copies the info from the path processor to the brushdata object
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::CopyBrushData(BrushData* pData)
{
	if (pData == NULL)
	{
		ERROR3("NULL brush data object");
		return;
	}

	pData->m_BrushHandle = m_BrushHandle;
	
	pData->m_BrushScaling = m_BrushScaling;
	pData->m_BrushScalingIncr = m_BrushScalingIncr;
	pData->m_BrushScalingIncrConst = m_BrushScalingIncrConst;
	pData->m_BrushScalingMaxRand = m_BrushScalingMaxRand;
	pData->m_BrushScalingRandSeed = m_BrushScalingRandSeed;
	pData->m_ScalingMaxPressure  = m_ScalingMaxPressure;

	pData->m_bRotate = m_bRotate;
	pData->m_RotateAngle = m_RotateAngle;
	pData->m_RotAngleIncrConst = m_RotAngleIncrConst;
	pData->m_RotAngleIncrProp  = m_RotAngleIncrProp;
	pData->m_RotationMaxRand = m_RotationMaxRand;
	pData->m_RotationRandSeed = m_RotationRandSeed;
	pData->m_RotationMaxPressure = m_RotationMaxPressure;

	pData->m_BrushSpacing  = m_BrushSpacing;
	pData->m_BrushSpacingIncrConst = m_BrushSpacingIncrConst;
	pData->m_BrushSpacingIncrProp = m_BrushSpacingIncrProp;
	pData->m_BrushSpacingMaxRand = m_BrushSpacingMaxRand;
	pData->m_BrushSpacingRandSeed = m_BrushSpacingRandSeed;

	pData->m_PathOffsetType = m_PathOffsetType;
	pData->m_PathOffsetValue = m_PathOffsetValue;
	pData->m_PathOffsetIncrConst = m_PathOffsetIncrConst;
	pData->m_PathOffsetIncrProp = m_PathOffsetIncrProp;
	pData->m_OffsetValueMaxRand = m_OffsetValueMaxRand;
	pData->m_OffsetValueRandSeed = m_OffsetValueRandSeed;
	pData->m_OffsetTypeRandSeed = m_OffsetTypeRandSeed;

	pData->m_bTile = m_bTile;

	pData->m_SequenceType = m_SequenceType;
	pData->m_SequenceRandSeed = m_SequenceRandSeed;

	pData->m_bUseLocalFillColour = m_bUseLocalFillColour;
	pData->m_bUseLocalTransp = m_bUseLocalTransp;
	pData->m_bUseNamedColour = m_bUseNamedColours;
	
	if (m_pParentAttr != NULL)
		pData->m_bTimeStampBrushes = m_pParentAttr->IsTimeStamping();
	else
		pData->m_bTimeStampBrushes = FALSE;

	pData->m_TimeStampPeriod = (UINT32)-1;

		
	pData->m_BrushHueIncrement = m_BrushHueIncrement;
	pData->m_BrushHueMaxRand = m_BrushHueMaxRand;
	pData->m_BrushHueRandSeed = m_BrushHueRandSeed;
	pData->m_BrushSatIncrement = m_BrushSatIncrement;
	pData->m_BrushSatMaxRand = m_BrushSatMaxRand;
	pData->m_BrushSatRandSeed = m_BrushSatRandSeed;	

	pData->m_BrushTransparency = m_BrushTransparency;
	pData->m_TranspMaxPressure = m_TranspMaxPressure;

	// Get the name from the definition
	BrushDefinition* pDef = GetOurBrushDefinition();

	if (pDef)
		pData->m_Name = *pDef->GetLineName();
}


/********************************************************************************************

>	void PathProcessorBrush::CopyDataFromObject(BrushData* pData) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		-
	Returns:	pData - pointer to the brushdata object
	Purpose:	copies the info from the brushdata object to the processor
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::CopyDataFromObject(BrushData* pData)
{
	if (pData == NULL)
	{
		ERROR3("NULL brush data object");
		return;
	}	
	m_BrushHandle = pData->m_BrushHandle;

	// scaling
	SetBrushScaling(pData->m_BrushScaling);
	m_BrushScalingIncr = pData->m_BrushScalingIncr;
	m_BrushScalingIncrConst = pData->m_BrushScalingIncrConst;
	m_BrushScalingMaxRand = pData->m_BrushScalingMaxRand;
	m_BrushScalingRandSeed = pData->m_BrushScalingRandSeed;
	m_ScalingMaxPressure   = pData->m_ScalingMaxPressure;

	// rotation
	m_bRotate = pData->m_bRotate;
	SetRotationAngle(pData->m_RotateAngle);
	m_RotAngleIncrConst = pData->m_RotAngleIncrConst;
	m_RotAngleIncrProp  = pData->m_RotAngleIncrProp;
	m_RotationMaxRand = pData->m_RotationMaxRand;
	m_RotationRandSeed = pData->m_RotationRandSeed;

	//spacing
	SetSpacing(pData->m_BrushSpacing);
	m_BrushSpacingIncrConst = pData->m_BrushSpacingIncrConst;
	m_BrushSpacingIncrProp = pData->m_BrushSpacingIncrProp;
	m_BrushSpacingMaxRand = pData->m_BrushSpacingMaxRand;
	m_BrushSpacingRandSeed = pData->m_BrushSpacingRandSeed;

	//offset
	m_PathOffsetType = pData->m_PathOffsetType;
	SetPathOffsetValue(pData->m_PathOffsetValue);
	m_PathOffsetIncrConst = pData->m_PathOffsetIncrConst;
	m_PathOffsetIncrProp = pData->m_PathOffsetIncrProp;
	m_OffsetValueMaxRand = pData->m_OffsetValueMaxRand;
	m_OffsetValueRandSeed = pData->m_OffsetValueRandSeed;
	m_OffsetTypeRandSeed = pData->m_OffsetTypeRandSeed;

	// colour variation
	m_BrushHueMaxRand    = pData->m_BrushHueMaxRand;
	m_BrushHueRandSeed   = pData->m_BrushHueRandSeed;
	m_BrushSatMaxRand	 = pData->m_BrushSatMaxRand;
	m_BrushSatRandSeed	 = pData->m_BrushSatRandSeed;

	// Transparency
	m_BrushTransparency  = pData->m_BrushTransparency;
	m_TranspMaxPressure  = pData->m_TranspMaxPressure;


	m_bTile = pData->m_bTile;

	m_SequenceType = pData->m_SequenceType;
	m_SequenceRandSeed = pData->m_SequenceRandSeed;

	m_bUseLocalFillColour = pData->m_bUseLocalFillColour;
	m_bUseLocalTransp = pData->m_bUseLocalTransp;
	m_bUseNamedColours = pData->m_bUseNamedColour;
	

}


/********************************************************************************************

>	void PathProcessorBrush::SetParentAttribute(AttrBrushType* pAttrVal) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the attribute for this ppb
	Returns:	-
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::SetParentAttribute(AttrBrushType* pAttr)
{
	m_pParentAttr = pAttr;
}


/********************************************************************************************

>	void PathProcessorBrush::GetParentAttribute() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	-
	Purpose:	returns the attribute associated with this ppb
	SeeAlso:	-

********************************************************************************************/

AttrBrushType* PathProcessorBrush::GetParentAttribute()
{
	return m_pParentAttr;
}



/********************************************************************************************

>	void PathProcessorBrush::SetMaxScaling(double Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the value to set
	Returns:	sets the flag saying whether or not to scale to the line width
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::SetMaxScaling(double Value)
{
	m_MaxScaling = Value;
}


/********************************************************************************************

>	void PathProcessorBrush::SetScaleToLineWidth(BOOL Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the value to set
	Returns:	sets the flag saying whether or not to scale to the line width
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::SetScaleToLineWidth(BOOL Value)
{	
	m_bScaleToLineWidth = Value;
}



/********************************************************************************************

>	void PathProcessorBrush::ScaleToValue(MILLIPOINT Value, BOOL IgnorePressure = FALSE) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the value to scale to
	Returns:	-
	Purpose:	Scales the brush object such that the height of the largest object is equal to Value.
				Note that as of 27/10/2000 this must incorporate offset, scaling and rotation values.	
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::ScaleToValue(MILLIPOINT ScaleValue, BOOL IgnorePressure)
{
//	if (/*(m_bScaleToLineWidth == FALSE) ||*/ (m_LineWidth == ScaleValue))
//		return;

/*	if (!IgnorePressure)
		INT32 i = 1; // what's all this about then? AMB
*/
	MILLIPOINT Height = GetBrushSize(IgnorePressure);
	double ScaleFactor = double((double)ScaleValue / (double)Height);

	
	//TRACEUSER( "Diccon", _T("Scaling to value %f\n"), ScaleFactor);
	if (ScaleFactor <= 0.0)
	{
		//ERROR3("Argh - attempting to scale below zero");
		ScaleFactor = 1.0;
	}

	SetBrushScaling(ScaleFactor);
	m_LineWidth = ScaleValue;
}



/********************************************************************************************

>	double PathProcessorBrush::ScaleToValue(MILLIPOINT Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the value to scale to
	Returns:	the scaling value required to scale to the value given, or -1 if something went wrong
	Purpose:	as above, but simply retuns the value
	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetScaleValue(MILLIPOINT Value)
{
	MILLIPOINT Height = GetBrushSize();
	double ScaleFactor = double((double)Value / (double)Height);

	return ScaleFactor;
}

/********************************************************************************************

>	MILLIPOINT PathProcessorBrush::GetBrushSize()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the height of the largest bounding box, to be used as a default line width
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

MILLIPOINT PathProcessorBrush::GetBrushSize(BOOL IgnorePressure)
{
	BrushDefinition* pBrushDef = GetOurBrushDefinition();
	if (pBrushDef == NULL)
		return -1;
	DocRect BRect = pBrushDef->GetLargestBoundingBox();
	if (BRect.IsEmpty())
		return -1;

	MILLIPOINT Height = BRect.Height();
	MILLIPOINT Width = BRect.Width();
//	double dHeight = (double)Height;
//	double dWidth = (double)Width;

	double ScaleFactor = 1.0;

	// firstly, if we are rotated, or have random rotation, then use the longest side
	if (m_RotationMaxRand > 0 || m_RotateAngle != 0.0 || !m_bRotate)
	{
		if (Width > Height)
			Height = Width;
	}
	
	if (m_BrushScalingMaxRand > 0)
	{
		// find out the random range
		UINT32 Lower = 0;
		UINT32 Upper = 0;
		GetRandomLimits(100, m_BrushScalingMaxRand, &Lower, &Upper);
		
		//Use the upper limit
		ScaleFactor *= ((double)Upper * 0.01);
	}

/*	if (m_ScalingMaxPressure > 0 && !IgnorePressure)
	{
		ScaleFactor *= ((double)m_ScalingMaxPressure * 0.01 + 1.0);
		
	}
*/
	MILLIPOINT NewHeight = (MILLIPOINT)((double)Height * ScaleFactor);

	// if we have random offset then there will be some effect, this is separate from the offset amount
	if (m_OffsetValueMaxRand > 0)
	{
		// rand value is a percentage of object size
		double RandMultiplier = (double)m_OffsetValueMaxRand * 0.01;
		double OffsetAmount = (double)Height * RandMultiplier;
		
		// remember that it goes both sides
		MILLIPOINT mpAmount = (MILLIPOINT)(2*OffsetAmount);
		NewHeight += mpAmount;
	}

	switch (m_PathOffsetType)
	{
		case OFFSET_NONE:
			break;
		case OFFSET_LEFT:
		case OFFSET_RIGHT:
		case OFFSET_ALTERNATE:
		{
			MILLIPOINT OffsetIncr = 2 * m_PathOffsetValue;
			NewHeight += OffsetIncr;
		}
		break;
		case OFFSET_RANDOM:
			break;
		default:
			ERROR3("Invalid offset type");
	}

	return NewHeight;
}




/********************************************************************************************

>	double PathProcessorBrush::GetAngleAtDistance(MILLIPOINT Distance) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     
	Returns:	The rotation angle to be applied to the brush object at this point
	Purpose:	Pretty much as above, but with the following notes:
				There are several different ways of applying a rotation angle to brush objects:
				
				- Rotate along the path(i.e. rotate according to the tangent of the path at this point)
				this is not included here, as the path tangent is known by the time you get to RenderBrushAtPoint
				
				- Constant angle of rotation, as set by the m_RotateAngle member
				
				- Random a random fn. determines a different value for each distance

	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetNextAngle()
{
	double NewAngle = m_LastRotationNoRandom;
	
	NewAngle += m_RotAngleIncrConst;
	m_LastRotationNoRandom = NewAngle;
	double Rand = 0;
	double PressureAngle = 1.0;
#ifdef USE_MTRNG
	if (m_RotationMaxRand != 0)
	{
		if (m_pRotationRNG != NULL)
		{
			// first find out our random range
//			UINT32 UpperLimit = 0;
//			UINT32 LowerLimit = 0;

		//	GetRotationRandomLimits(100, m_RotationMaxRand, &LowerLimit, &UpperLimit);

			Rand = (double)m_pRotationRNG->GetNextRandomNumberScaled(m_RotationMaxRand, 0);

//double R =  (Rand * 0.01) * 360;
//TRACEUSER( "Phil", _T("Random = %f, Random angle = %f\n"), Rand, R);
		}
		else
			ERROR3("Rotation RNG is NULL");
	}
	
#endif
	if (m_RotationMaxPressure > 0 && m_bValidPressure)
	{
		UINT32 PressureVal = GetCurrentPressureEffect(m_RotationMaxPressure);
		//TRACEUSER( "Diccon", _T("Pressure adj = %d\n"), PressureVal);
		PressureAngle = (double)PressureVal / 100; //* 180;
	
	}
	m_LastAngle = (NewAngle * PressureAngle) + Rand;

	while (m_LastAngle > 360)
		m_LastAngle -= 360;
	//TRACEUSER( "Diccon", _T("Next angle = %f\n"), m_LastAngle);
	return m_LastAngle;
}

/********************************************************************************************

>	double PathProcessorBrush::GetNextScaling() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     
	Returns:	The scaling to be applied to the brush object at this point
	Purpose:	Pretty much as above, but with the following notes:
				There are several different ways of applying a rotation angle to brush objects:
				
				- Constant scaling, as set by the m_BrushScaling member
				- Increasing/decreasing as set by m_ScalingIncr and m_LastScaling
				- Random - a random fn. provides a value to be applied

	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetNextScaling(UINT32 Index)
{
	ERROR2IF(m_LastScaling < 0, 1.0, "Invalid scaling value");
	ERROR2IF(m_BrushScalingIncr <= 0, 1.0, "Invalid scaling value");
	
	double NewScaling = m_BrushScaling;
	if (Index > 0)
		NewScaling = m_LastScalingNoRandom; 
	double RandomScale = 1.0;
	double PressureScale = 1.0;

	// record the last value before randomness
	m_LastScalingNoRandom = NewScaling;
	if (m_BrushScalingMaxRand != 0)
	{
#ifdef USE_MTRNG
		if (m_pScalingRNG != NULL)
		{
			// first find out our random range
			UINT32 UpperLimit = 0;
			UINT32 LowerLimit = 0;

			GetRandomLimits(100, m_BrushScalingMaxRand, &LowerLimit, &UpperLimit);
			// we want our random numbers in the range 100% +/- MaxRand
			UINT32 Random = m_pScalingRNG->GetNextRandomNumberScaled(UpperLimit, LowerLimit, 100);
																   
			//TRACEUSER( "Diccon", _T("Random val =  %d\n"), Random);
			RandomScale = (double)Random / 100;	
//TRACEUSER( "Phil", _T("Scaling = %f\n"), m_BrushScaling * RandomScale);
		}
#endif	
	}
	if (m_ScalingMaxPressure > 0 && m_bValidPressure)
	{
		UINT32 PressureVal = GetCurrentPressureEffect(m_ScalingMaxPressure, TRUE);
		
		PressureScale = (double)PressureVal / 100;
	//	TRACEUSER( "Diccon", _T("Pressure Val = %d, Scale = %f\n"), PressureVal, PressureScale);
	}
	
	NewScaling = NewScaling * RandomScale * PressureScale;

	// we're no longer stopping you from going under the minimum scaling, but there is code in place
	// to break out of the rendering loop if you do so.

/*	if (NewScaling <= MIN_BRUSH_SCALE)
		NewScaling = MIN_BRUSH_SCALE;
	else */
	{
		if (NewScaling >= m_MaxScaling)
				NewScaling = m_MaxScaling;
	}

	// if its the first object then we want to return the actual scaling value, 
	// not the change in scaling
	double NextScaling = NewScaling;
	
	if (!m_UseActualScaling)
		NextScaling = NewScaling / m_LastScalingRendered;
	
	m_LastScaling = NewScaling;
	
	//TRACEUSER( "Diccon", _T("Actual Scaling = %f, Next Scaling = %f, Last Rendered = %f\n"), NewScaling, NextScaling, m_LastScalingRendered);

	return NextScaling;
}


/********************************************************************************************

>	MILLIPOINT PathProcessorBrush::GetNextSpacing() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns     the next spacing to use
	Purpose:	Returns the next spacing to use, this can be determined in several ways, as
				with the other object placement attributes.

	SeeAlso:	-

********************************************************************************************/

MILLIPOINT PathProcessorBrush::GetNextSpacing()
{
	ERROR2IF(m_LastSpacing <= 0, 10000, "Invalid last spacing");
	ERROR2IF(m_BrushSpacing <= 0, 10000, "Invalid spacing value");
	
	// start of with the previous spacing value, before it had its random effect added.
	// If we don't do this then we end up with a random walk effect, always getting either
	// bigger or smaller
	MILLIPOINT NewSpacing = m_LastSpacingNoRandom;
	
	// do the proportional incrementfirst
	if (m_BrushSpacingIncrProp != 1.0)
		NewSpacing = (MILLIPOINT)(m_BrushSpacingIncrProp * m_LastSpacing);
	
	// add the constant increment
	if (m_BrushSpacingIncrConst != 0)
		NewSpacing += m_BrushSpacingIncrConst;
	
	// store this value so we can use it next time
	m_LastSpacingNoRandom = NewSpacing;

	if (m_BrushSpacingMaxRand != 0)
	{
#ifdef USE_MTRNG
		if (m_pSpacingRNG != NULL)
		{
			// first find out our random range
			UINT32 UpperLimit = 0;
			UINT32 LowerLimit = 0;

			GetRandomLimits(100, m_BrushSpacingMaxRand, &LowerLimit, &UpperLimit);
			// we want our random numbers in the range 100% +/- MaxRand
			//UINT32 Random = m_pSpacingRNG->GetNextRandomNumberScaled(100 + m_BrushSpacingMaxRand,
			//														100 - m_BrushSpacingMaxRand);
			
			// now ask our RNG to get a number between these limits
			UINT32 Random = m_pSpacingRNG->GetNextRandomNumberScaled(UpperLimit, LowerLimit/2, 100);
		
			// we treat our numbers as percentages
			double Mult = (double)Random / 100;
			//TRACEUSER( "Diccon", _T("Random spacing multiplier = %f\n"), Mult);
			NewSpacing = (MILLIPOINT)(((double)NewSpacing) * Mult);
	
		}
#endif
	}
	if (m_SpacingMaxPressure > 0 && m_bValidPressure)
	{
		UINT32 PressureVal = GetCurrentPressureEffect(m_SpacingMaxPressure);
	//	TRACEUSER( "Diccon", _T("Pressure adj = %d\n"), PressureVal);
		double PressureScale = (double)PressureVal / 100;
		NewSpacing = (MILLIPOINT)((double)NewSpacing * PressureScale);
	}
	
	

	if (NewSpacing < MIN_BRUSH_SPACING)
		NewSpacing = MIN_BRUSH_SPACING;
	else 
	{
		if (NewSpacing > MAX_BRUSH_SPACING)
			NewSpacing = MAX_BRUSH_SPACING;
	}

	m_LastSpacing = NewSpacing;
	///TRACEUSER( "Diccon", _T("Spacing = %d\n"), NewSpacing);
	return m_LastSpacing;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetNextInkObject(UINT32 LastObject, UINT32 NumObjects) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Lastobject - the index of the last object rendered
				Numobjects - the number of ink objects in this brush
	Returns     the index number of the next ink object to use
	Purpose:	as above

	SeeAlso:	-

********************************************************************************************/

UINT32 PathProcessorBrush::GetNextInkObject(UINT32 LastObject, UINT32 NumObjects)
{
	if (LastObject > NumObjects)
	{
		ERROR3("Duh, last object can't be greater than maximum");
		return 0;
	}
	UINT32 RetVal = 0;
	switch (m_SequenceType)
	{
		case SEQ_FORWARD:
			if (LastObject < NumObjects -1)
				RetVal=  ++LastObject;
			else
				RetVal =  0;
			break;
		case SEQ_BACKWARD:
			if (LastObject > 0)
				RetVal = --LastObject;
			else
				RetVal =  NumObjects -1;
			break;
		case SEQ_MIRROR:
			// go forward if we are ascending
			if (m_MirrorSeqAscend)
			{
				if (LastObject < NumObjects -1)
						RetVal = ++LastObject;
				else
				{
					m_MirrorSeqAscend = FALSE;
					RetVal = NumObjects -1;
				}
			}
			else // go backward if we are descending
			{
				if (LastObject > 0)
					RetVal =  --LastObject;
				else
				{
					m_MirrorSeqAscend = TRUE;
					RetVal =  0;
				}
			}
			break;
		case SEQ_RANDOM:
			//RetVal =  GetNextRandomNumberScaled(NumObjects, 0);
			if (m_pSequenceRNG != NULL)
			{
				RetVal = m_pSequenceRNG->GetNextRandomNumberScaled(NumObjects, 0);
			}
			else
			{
				ERROR3("Trying to get a random number with no RNG");
			}

			break;
		default:
			ERROR3("invalid sequence type");
			RetVal = 0;
			break;
	}
	// quick check
	if (RetVal >= NumObjects)
		RetVal--;

//	TRACEUSER( "Diccon", _T("Ink object = %d, "), RetVal);
	return RetVal;


}



/********************************************************************************************

>	MILLIPOINT PathProcessorBrush::GetNextOffset() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns     the next offset to use
	Purpose:	as above

	SeeAlso:	-

********************************************************************************************/

MILLIPOINT PathProcessorBrush::GetNextOffset()
{
	ERROR2IF(m_LastOffset < 0, 0, "Invalid last Offset");
	ERROR2IF(m_PathOffsetValue < 0, 0, "Invalid Offset value");
	
	MILLIPOINT NewOffset = m_LastOffsetNoRandom;

	// do the proportional incrementfirst
	if (m_PathOffsetIncrProp != 1.0)
		NewOffset = (MILLIPOINT)(m_PathOffsetIncrProp * m_LastOffset);
	
	// add the constant increment
	if (m_PathOffsetIncrConst != 0)
		NewOffset += m_PathOffsetIncrConst;
	
	m_LastOffsetNoRandom = NewOffset;
	
#ifdef USE_MTRNG
	if (m_OffsetValueMaxRand > 0)
	{
	//	if (m_PathOffsetValue > 0)
		{
			if (m_pOffsetValueRNG != NULL)
			{
				// first find out our random range
//				UINT32 UpperLimit = 0;
				UINT32 LowerLimit = 0;

			//	GetRandomLimits(100, m_OffsetValueMaxRand, &LowerLimit, &UpperLimit);

				// we want our random numbers in the range 100% +/- MaxRand
				UINT32 Random = m_pOffsetValueRNG->GetNextRandomNumberScaled(m_OffsetValueMaxRand, LowerLimit);
				
				double Mult = (double)Random / 100;

				BrushDefinition* pBrushDef = GetOurBrushDefinition();
				if (pBrushDef != NULL)
				{
					DocRect BRect = pBrushDef->GetLargestBoundingBox();
					if (!BRect.IsEmpty())
					{
						double Height = (double)BRect.Height();
						Height = Height * Mult;		

						NewOffset += (INT32)Height;
					}
				}

				//NewOffset = (MILLIPOINT)(((double)NewOffset) * Mult);
				//TRACEUSER( "Diccon", _T("Spacing = %d\n"), NewSpacing);
			}
			else
				ERROR3("OffsetVal RNG  is NULL");
		}
	}
#endif
	

	if (NewOffset < 0)
		NewOffset = 0;
	else 
	{
		if (NewOffset > MAX_BRUSH_OFFSET)
			NewOffset = MAX_BRUSH_OFFSET;
	}
	m_LastOffset = NewOffset;
//	TRACEUSER( "Diccon", _T("Offset = %d\n"), m_LastOffset);
	return m_LastOffset;
}



/********************************************************************************************

>	double PathProcessorBrush::GetNextSaturationMultiplier()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns     the next saturation multiplier
	Purpose:	this differs slightly from the other GetNext functions in that it does not return
				a saturation value. Instead it returns a multiplier value which should be given
				to an HSVChanger object, the HSVChanger should then apply that multiplier to the
				colours in the brush

	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetNextSaturationMultiplier()
{
	double NextSat = 0;
#ifdef USE_MTRNG
	if (m_BrushSatMaxRand > 0)
	{		
		if (m_pSaturationRNG != NULL)
		{
			// first find out our random range
			UINT32 UpperLimit = 0;
			UINT32 LowerLimit = 0;

			GetRandomLimits(100, m_BrushSatMaxRand, &LowerLimit, &UpperLimit);

			// we want our random numbers in the range 100% +/- MaxRand
			UINT32 Random = m_pSaturationRNG->GetNextRandomNumberScaled(UpperLimit, LowerLimit);

			// do a bit of cheating here as we want to allow negative numbers
			INT32 NewRand = Random - 100;
			double Mult = (double)NewRand / 100;
		//	TRACEUSER( "Diccon", _T("Sat Rand = %d\n"), NewRand);
			NextSat = Mult;
			
		}
		else
			ERROR3("SaturationVal RNG  is NULL");
	}
#endif	
	m_LastSatMultiplier = NextSat;
	return NextSat;
}



/********************************************************************************************

>	double PathProcessorBrush::GetNextHueMultiplier()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns     the next hue multiplier
	Purpose:	this differs slightly from the other GetNext functions in that it does not return
				a hue value. Instead it returns a multiplier value which should be given
				to an HSVChanger object, the HSVChanger should then apply that multiplier to the
				colours in the brush

	SeeAlso:	-

********************************************************************************************/

double PathProcessorBrush::GetNextHueMultiplier()
{
	double NextHue = 0;
#ifdef USE_MTRNG
	if (m_BrushHueMaxRand > 0)
	{		
		if (m_pHueRNG != NULL)
		{
			// first find out our random range
//			UINT32 UpperLimit = 0;
			UINT32 LowerLimit = 0;

		//	GetRandomLimits(100, m_BrushHueMaxRand, &LowerLimit, &UpperLimit);

			// we want our random numbers in the range 100% +/- MaxRand
			UINT32 Random = m_pHueRNG->GetNextRandomNumberScaled(m_BrushHueMaxRand*2, LowerLimit);

		//	INT32 RealRand = Random - 100;
			//double Mult = (double)RealRand / 100;
			NextHue = (double)Random - m_BrushHueMaxRand ;
		}
		else
			ERROR3("HueVal RNG  is NULL");
	}
#endif
	m_LastHueMultiplier = NextHue;
//	TRACEUSER( "Diccon", _T("Next Hue = %f\n"), NextHue);
	return NextHue;
}

/********************************************************************************************

>	void PathProcessorBrush::SetNextOffsetType() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns     -
	Purpose:	Asks the processor to determine its next offset type and record it in a data member.
				This only occurs if the processor has its path offset type set to random.
				If you are calling RenderBrushAtPoint externally rather than from ProcessPath
				then you should call this for each ink object

	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::SetNextOffsetType()
{
	if (m_PathOffsetType == OFFSET_RANDOM)
		m_LastOffsetType = GetNextOffsetType();
	else
		m_LastOffsetType = OFFSETTYPE_CENTRE; // default to centre
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetNextOffsetType() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns     the next offset type
	Purpose:	as above

	SeeAlso:	-

********************************************************************************************/
UINT32 PathProcessorBrush::GetNextOffsetType()


{

	if (m_PathOffsetType != OFFSET_RANDOM )
	{
		//ERROR3("Trying to get random offset type when we shouldn't be");
		return OFFSETTYPE_CENTRE;
	}
	
	UINT32 Offset = OFFSETTYPE_CENTRE;
#ifdef USE_MTRNG
	if (m_pOffsetTypeRNG == NULL)
	{
		ERROR3("Offset Type RNG is NULL");
		return OFFSETTYPE_CENTRE;
	}
	// get a random number, if its even we go left, if its odd we go right
	UINT32 Rand = m_pOffsetTypeRNG->operator ()();
	if (Rand % 2 == 0)
		Offset = OFFSETTYPE_LEFT;
	else
		Offset = OFFSETTYPE_RIGHT;

	//TRACEUSER( "Diccon", _T("Offset Type = %d\n"), Offset);
#endif
	return Offset;
}


/********************************************************************************************

>	UINT32 PathProcessorBrush::GetCurrentPressureEffect(UINT32 MaxEffect) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     MaxEffect - the maximum % effect that pressure can have
				FixedMaximum - flag indicating that the maximum should always be 100%
	Returns     the % effect of the current pressure
	Purpose:	Simply pass in the maximum effect that you allow and this returns
				the effect caused by the current pressure value

	SeeAlso:	-

********************************************************************************************/

UINT32 PathProcessorBrush::GetCurrentPressureEffect(UINT32 MaxEffect, BOOL FixedMaximum)
{
	if (m_CurrentPressure.m_Pressure < 0 || m_CurrentPressure.m_Pressure > MAXPRESSURE)
	{
		ERROR3("Invalid pressure here Jim");
		return 0;
	}
	
	// set the maximum, we have the option to fix this at 100% if we want
	UINT32 MaxPercent = 100;
	if (!FixedMaximum)
		MaxPercent += MaxEffect;

	UINT32 MinPercent = 100 - MaxEffect;
	UINT32 Range = MaxPercent - MinPercent;

	double PressureVal = (double)((double)Range / (double)MAXPRESSURE);
	UINT32 CurrentPressure = (UINT32)(PressureVal * m_CurrentPressure.m_Pressure);

	return MinPercent + CurrentPressure;
}


/********************************************************************************************

>	INT32 PathProcessorBrush::GetNextTransparencyAdjust()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/2000
	Inputs:     -
	Returns     the first transparency adjustment value 
	Purpose:	

	SeeAlso:	GetPressureTranspAdjust()

	Notes:		Transparency variations now work through multipliers, as opposed to the previous
				replacement system.  This value returned from this function is the main transparency
				adjuster value.

********************************************************************************************/

double PathProcessorBrush::GetNextTransparencyAdjust()
{
	return (double)m_BrushTransparency * 0.01;
}


/********************************************************************************************

>	double PathProcessorBrush::GetNextTranspAdjust()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/2000
	Inputs:     -
	Returns     the transparency adjuster value
	Purpose:	as above, currently the only thing that affects transparency is pressure.  Note
				that -1 indicates that we are not adjusting transparency

	SeeAlso:	notes for GetNextTransparency

********************************************************************************************/

double PathProcessorBrush::GetNextPressureTranspAdjust()
{
	double Retval = 1.0;

	if (m_TranspMaxPressure != 0)
	{
		double PressureEffect = (double)GetCurrentPressureEffect(m_TranspMaxPressure);
		if (PressureEffect != 100)
		{
			Retval = PressureEffect *0.01;
		//	TRACEUSER( "Diccon", _T("Transp adjust %f\n"), Retval);		
		}
	}
	return Retval;
}

/********************************************************************************************

>	UINT32 PathProcessorBrush::GetCurrentPressure() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the current pressure value for the point to be rendered
	Purpose:	As above

********************************************************************************************/

UINT32 PathProcessorBrush::GetCurrentPressure()
{
	return m_CurrentPressure.m_Pressure;
}


/********************************************************************************************

>	void PathProcessorBrush::SetCurrentPressure(UINT32 Pressure) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		the current pressure value for the point to be rendered
	Purpose:	As above

********************************************************************************************/

void PathProcessorBrush::SetCurrentPressure(UINT32 Pressure)
{
	if (Pressure < 0 || Pressure > MAXPRESSURE)
		return;

	m_CurrentPressure.m_Pressure = Pressure;
	m_bValidPressure = TRUE;
}


/********************************************************************************************

>	void PathProcessorBrush::GetRandomLimits(UINT32 Base, UINT32 MaxRand, UINT32* pLower, UINT32* pUpper) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		Base - the base number which you will use to derive the limits. e.g. if you are
				working in percentages then you will probably want to use 100 as your base. 
				In fact this has not been tested with a base other than 100 so that is what is advised.
				
				MaxRand the maximum random amount, only really tested in the range 0 - 100 with 0 
				being no random (i.e upperlimit = lowerlimit = base) and 100 being the maximum
				(i.e. lowerlimit = base * 1/MAX_RANDOM_FACTOR and upperlimit = base * MAX_RANDOM_FACTOR
	Outputs:    pLower will point to a value which will be the lower bound of our random range
				pUpper - similar for the upper bound
	Purpose:	To determine the upper and lower bounds of a random range

********************************************************************************************/


void PathProcessorBrush::GetRandomLimits(UINT32 Base, UINT32 MaxRand, UINT32* pLower, UINT32* pUpper)
{
	// quick checks
	if (pLower == NULL || pUpper == NULL) 
	{
		ERROR3("Illegal entry parameters in PathProcessorBrush::GetRandomLimits");
		return;
	}
	
	if (MaxRand > MAX_RANDOM_AMOUNT || MaxRand < MIN_RANDOM_AMOUNT)
	{
		ERROR3("Illegal entry parameters in PathProcessorBrush::GetRandomLimits");
		*pLower = Base;
		*pUpper = Base;
		return;
	}
	
	if (MaxRand == 0)
	{
		*pLower = Base;
		*pUpper = Base;
		return;
	}

	// so we're ok.
	double RandomFactor = (double)MaxRand / 100;
	RandomFactor = (RandomFactor * MAX_RANDOM_FACTOR) + MIN_RANDOM_FACTOR;

	*pLower = (UINT32)((double)Base  / RandomFactor);
	*pUpper  =(UINT32)((double)Base * RandomFactor);

	
	return;
}


/********************************************************************************************

>	void PathProcessorBrush::GetRotationRandomLimits(UINT32 Base, UINT32 MaxRand, UINT32* pLower, UINT32* pUpper) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		Base - the base number which you will use to derive the limits. e.g. if you are
				working in percentages then you will probably want to use 100 as your base. 
				In fact this has not been tested with a base other than 100 so that is what is advised.
				
				MaxRand the maximum random amount, only really tested in the range 0 - 100 with 0 
				being no random (i.e upperlimit = lowerlimit = base) and 100 being the maximum
				(i.e. lowerlimit = base * 1/MAX_RANDOM_FACTOR and upperlimit = base * MAX_RANDOM_FACTOR
	Outputs:    pLower will point to a value which will be the lower bound of our random range
				pUpper - similar for the upper bound
	Purpose:	To determine the upper and lower bounds of a random range, differs from the above
				function in that the maximum rotation random factor is different

********************************************************************************************/

void PathProcessorBrush::GetRotationRandomLimits(UINT32 Base, UINT32 MaxRand, UINT32* pLower, UINT32* pUpper) 
{
	// quick checks
	if (pLower == NULL || pUpper == NULL) 
	{
		ERROR3("Illegal entry parameters in PathProcessorBrush::GetRandomLimits");
		return;
	}
	
	if (MaxRand > MAX_RANDOM_AMOUNT || MaxRand < MIN_RANDOM_AMOUNT)
	{
		ERROR3("Illegal entry parameters in PathProcessorBrush::GetRandomLimits");
		*pLower = Base;
		*pUpper = Base;
		return;
	}
	
	if (MaxRand == 0)
	{
		*pLower = Base;
		*pUpper = Base;
		return;
	}

	// so we're ok.
	double RandomFactor = (double)MaxRand / 100;
	RandomFactor = (RandomFactor * MAX_ROTATION_RANDOM_FACTOR) + MIN_RANDOM_FACTOR;

	*pLower = (UINT32)((double)Base  / RandomFactor);
	*pUpper  =(UINT32)((double)Base * RandomFactor);

	
	return;

}
/********************************************************************************************

>	UINT32 PathProcessorBrush::GetFirstRandomNumber(UINT32 Seed = 0) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     a seed to use to generate the random number sequence
	Returns:	the first number in a random number sequence
	Purpose:	Seeds srand with either the seed supplied or by using time (i.e. a random seed)
	SeeAlso:

    **** SPECIAL NOTE, now that I am making use of the MT random number generators these functinos
	that make use of srand are only used for random sequences (i think) and maybe I should change
	that over too for consistency.
********************************************************************************************/
/*
UINT32 PathProcessorBrush::GetFirstRandomNumber(UINT32 Seed)
{
	// seed the random number generator
	if (Seed == 0)
		srand( (unsigned)time( NULL ) );
	else
		srand((unsigned)Seed);

	return (UINT32)rand();
}
*/

/********************************************************************************************

>	UINT32 PathProcessorBrush::GetNextRandomNumber() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the next number in an already seeded random number sequence
	Purpose:	as above, note that you MUST have already seeded the sequence
	SeeAlso:

********************************************************************************************/
/*
UINT32 PathProcessorBrush::GetNextRandomNumber()
{
	INT32 Retval =  rand();
	TRACEUSER( "Diccon", _T("random number = %d , "), Retval);
	return (UINT32)Retval;
}
*/

/********************************************************************************************

>	UINT32 PathProcessorBrush::GetNextRandomNumberScaled(UINT32 MaxValue = 100, UINT32 MinValue = 0) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	the next number in an already seeded random number sequence, scaled to between
				the two specified values
	Purpose:	as above, note that you MUST have already seeded the sequence
	SeeAlso:

********************************************************************************************/

/*
UINT32 PathProcessorBrush::GetNextRandomNumberScaled(UINT32 MaxValue, UINT32 MinValue)
{
	UINT32 Random = GetNextRandomNumber();
//	return (Random % MaxValue) + MinValue;
	double ScaleFactor = ((double)(MaxValue - MinValue)) / ((double)RAND_MAX);
	return ((UINT32)(Random * ScaleFactor) + MinValue);
	
}
*/
/********************************************************************************************

>	void PathProcessorBrush::ResetRandomNumberGenerators() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns		-
	Purpose:	to reset all the random number generators so that they repeat their sequence
				from the beginning. Call this prior to rendering
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::ResetRandomNumberGenerators()
{
#ifdef USE_MTRNG
	if (m_pSpacingRNG != NULL)
		m_pSpacingRNG->seed((UINT32)m_BrushSpacingRandSeed);
	if (m_pScalingRNG !=  NULL)
		m_pScalingRNG->seed((UINT32)m_BrushScalingRandSeed);
	if (m_pOffsetTypeRNG != NULL)
		m_pOffsetTypeRNG->seed((UINT32)m_OffsetTypeRandSeed);
	if (m_pOffsetValueRNG != NULL)
		m_pOffsetValueRNG->seed((UINT32)m_OffsetValueRandSeed);
	if (m_pSaturationRNG != NULL)
		m_pSaturationRNG->seed((UINT32)m_BrushSatRandSeed);
	if (m_pHueRNG != NULL)
		m_pHueRNG->seed((UINT32)m_BrushHueRandSeed);
	if (m_pRotationRNG != NULL)
		m_pRotationRNG->seed((UINT32) m_RotationRandSeed);
	if (m_pSequenceRNG != NULL)
		m_pSequenceRNG->seed((UINT32) m_SequenceRandSeed);
#endif
	return TRUE;
}

/********************************************************************************************

>	INT32 PathProcessorBrush::HowManyObjectsInDistance(MILLIPOINT Distance) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     a distance
	Returns		the number of brush object that will fit in the given distance
	Purpose:	as above

	SeeAlso:	-

********************************************************************************************/

INT32 PathProcessorBrush::HowManyObjectsInDistance(MILLIPOINT Distance)
{
	INT32 NumObjects = 0;
	MILLIPOINT DistanceSoFar = 0;
	MILLIPOINT Spacing = m_BrushSpacing;

	while (DistanceSoFar <= Distance)
	{
		NumObjects++;
		Spacing = (MILLIPOINT)(Spacing * m_BrushSpacingIncrProp);
		Spacing += m_BrushSpacingIncrConst;
		if (Spacing < MIN_BRUSH_SPACING)
			Spacing = MIN_BRUSH_SPACING;
		else 
		{
			if (Spacing > MAX_BRUSH_SPACING)
					Spacing = MAX_BRUSH_SPACING;
		}
		DistanceSoFar += Spacing;
	}
	return NumObjects;
}


/********************************************************************************************

>	BOOL PathProcessorBrush::IsThisPathProcessorOnTheStack(RenderRegion* pRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/2000
	Inputs:     pRender - the render region containing the path processor stack
	Returns		TRUE, if another path processor referring to the same brush definition as this one
				is present on the stack, FALSE if not
	Purpose:	as above

	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::IsThisPathProcessorOnTheStack(RenderRegion* pRender)
{
	if (pRender == NULL)
	{
		ERROR3("Render Region is NULL in PathProcessorBrush::IsThisPathProcessorOnTheStack");
		return TRUE;  // for safety, as returning TRUE means we will probably not regenerate the brush
	}

	BOOL Retval = FALSE;
	PathProcessor* pPathProc = pRender->GetFirstPathProcessor();

	while (pPathProc != NULL)
	{
		// is it a brush?
		if (pPathProc->IsAPathProcessorBrush())
		{
			// is it our brush?
			if (((PathProcessorBrush*)pPathProc)->GetBrushDefinitionHandle() == m_BrushHandle)
			{
				if (pPathProc != this)
				{
					Retval = TRUE;
					break;
				}
			}
		}
		pPathProc = pPathProc->GetNextProcessor();
	}

	return Retval;
}

/********************************************************************************************

>	BOOL PathProcessorBrush::PrepareForRenderingLoop(UINT32 NumBrushObjects)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/2000
	Inputs:		NumBrushObjects - the number of brushrefs used by this brush
	Returns		TRUE if all went well
	Purpose:	Prepares the caches that we will use to store data about the transformations
				performed during rendering.  Currently only scaling has been implemented but
				we should also get rotation and tangential information too. This should be called 
				sometime after calling Reset and before you start actually rendering
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::PrepareForRenderingLoop(UINT32 NumBrushObjects)
{
	ERROR2IF(NumBrushObjects > MAX_BRUSH_OBJECTS, FALSE, "Too many objects in PathProcessorBrush::PrepareForRenderingLoop");
	ERROR2IF(NumBrushObjects == 0, FALSE, "No objects in PathProcessorBrush::PrepareForRenderingLoop");

	// make an array with a flag for each object which is set the first time it is rendered
	m_pObjectRendered = new BOOL[NumBrushObjects];
	if (m_pObjectRendered == NULL)
		return FALSE;

	// Initialise to false
	for (UINT32 i = 0; i < NumBrushObjects; i++)
		m_pObjectRendered[i] = FALSE;

	// make an array to hold the scaling values
	m_pLastScalingRendered = new double[NumBrushObjects];
	if (m_pLastScalingRendered == NULL)
	{
		delete m_pObjectRendered;
		m_pObjectRendered = NULL;
		return FALSE;
	}

	// initialise to our base scaling value
	for ( UINT32 i = 0; i < NumBrushObjects; i++)
		m_pLastScalingRendered[i] = m_BrushScaling;

	/*Special note:  As of 7/12/2000 only the Scaling cache is actually in use,
	m_pLastTangent = new double[NumBrushObjects];
	if (m_pLastTangent == NULL)
	{
		CleanUpAfterRender();
		return FALSE;
	}
	for (i = 0; i < NumBrushObjects; i++)
		m_pLastTangent[i] = 0.0;

	m_pLastRotation = new double[NumBrushObjects];
	if (m_pLastRotation == NULL)
	{
		CleanUpAfterRender();
		return FALSE;
	}
	for (i = 0; i < NumBrushObjects; i++)
		m_pLastRotation[i] = 0.0;
	*/
	// we always want to use the actual scaling first time out
	m_UseActualScaling = TRUE; 

	return TRUE;
}

/********************************************************************************************

>	void PathProcessorBrush::CleanUpAfterRender()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/2000
	Inputs:		-
	Returns		-
	Purpose:	deletes the caches that we used to store rendering data
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::CleanUpAfterRender()
{
	if (m_pObjectRendered != NULL)
	{
		delete m_pObjectRendered;
		m_pObjectRendered = NULL;
	}
	if (m_pLastScalingRendered != NULL)
	{
		delete m_pLastScalingRendered;
		m_pLastScalingRendered = NULL;
	}
	if (m_pLastTangent != NULL)
	{
		delete m_pLastTangent;
		m_pLastTangent = NULL;
	}
	if (m_pLastRotation != NULL)
	{
		delete m_pLastRotation;
		m_pLastRotation = NULL;
	}
}


/********************************************************************************************

>	BOOL PathProcessorBrush::SetLastRenderedData(UINT32 BrushObject, double Scaling, double Tangent, 
											 double Rotation)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/2000
	Inputs:		NumBrushObjects - the brush object last rendered
				Scaling - the scaling we last used to render
				Tangent - the tangent we last used to render
				Rotation - the last rotation etc.
	Returns		TRUE if all went well
	Purpose:	Should be called to update the caches that store our cached transformation data
				so that we only have to perform changes in transformations.
				Currently only OpDrawBrush need call this as the normal path processor rendering
				loop updates itself, though this may change.
				Note also that the rotation and tangent data are not yet used.
	SeeAlso:	-

********************************************************************************************/

BOOL PathProcessorBrush::SetLastRenderedData(UINT32 BrushObject, double Scaling, double Tangent, 
											 double Rotation)
{
	ERROR2IF(m_pObjectRendered == NULL || m_pLastScalingRendered == NULL ||
			 m_pLastTangent == NULL || m_pLastRotation == NULL, FALSE, "Data caches have not been initialised in PathProcessorBrush::SetLastRenderedData");


#ifdef _DEBUG
	if (BrushObject > GetNumBrushObjects())
	{
		ERROR3("Object index out of bounds in PathProcessorBrush::SetLastRenderedData");
		return FALSE;
	}
	
#endif
	m_pObjectRendered[BrushObject] = TRUE;
	m_pLastScalingRendered[BrushObject] = Scaling;
	m_pLastTangent[BrushObject] = Tangent;
	m_pLastRotation[BrushObject] = Rotation;

	m_LastScalingRendered = Scaling;

	return TRUE;			
}


/********************************************************************************************

>	void PathProcessorBrush::DecideWhetherToUseActualScaling(UINT32 BrushObject)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/2000
	Inputs:		NumBrushObjects - the brush object last rendered
			

	Purpose:	Decides whether or not to set the UseActualScaling flag for the next call of
				GetNextScaling.  Basically if we have not rendered this object before then the
				scaling we wish to use will be the absolute scaling.  However subsequently we only
				wish to know about scaling changes.
				This function is called by OpDrawBrush::RenderStepsForTime, and ProcessPath.
				
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::DecideWhetherToUseActualScaling(UINT32 BrushObject)
{
	if	(m_pObjectRendered == NULL || m_pLastScalingRendered == NULL)
	{
		ERROR3("Data caches have not been initialised in PathProcessorBrush::DecideWhetherToUseActualScaling");
		return;
	}
			 


//#ifdef _DEBUG
	if (BrushObject >= GetNumBrushObjects())
	{
		ERROR3("Object index out of bounds in PathProcessorBrush::SetLastRenderedData");
		return;
	}
	
//#endif

	if (m_pObjectRendered[BrushObject])
	{
		m_UseActualScaling = FALSE;
		m_LastScalingRendered = m_pLastScalingRendered[BrushObject];
	}
	else
	{
		m_UseActualScaling = TRUE;
	}
// Phil, 14/01/2004 this change accompanies the rewritten RenderBrushAtPoint function
// This routine was a bodge because RenderBrushAtPoint used to permanently affect the size (scale)
// of the brush path definitions!!! ARGH!
// The new routine leave path definitions untouched.
#ifdef NEW_RENDERBRUSHATPOINT
	m_UseActualScaling = TRUE;
#endif

}


/********************************************************************************************

>	void PathProcessorBrush::SetStrokeColour(DocColour Col)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/2000
	Inputs:		Col - the stroke colour to use
	Returns		-
	Purpose:	if the brush is overriding colours then this function can be used to set the
				stroke colour to use.  Although this will be set automatically in ProcessPath.
				Mostly for use by OpDrawBrush
	SeeAlso:	-

********************************************************************************************/

void PathProcessorBrush::SetStrokeColour(DocColour Col)
{
	m_CurrentStrokeCol = Col;	
}

/********************************************************************************************


>	BOOL PathProcessorBrush::WriteNative(BaseCamelotFilter *pFilter)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pFilter - filter to write to

	Returns:	TRUE if the Node has written out a record to the filter

	Purpose:	Writes out a record that represents the processor, to either Native or Web
				file format

	Notes:		Brush details are written out in two places, here and in the brush component.
				This records details of an individual brush in a document, details of the actual
				ink nodes used to generate the brush are recorded in the brush component.
				

	SeeAlso:	AttrBrushType::WritePreChildrenNative; BrushAttrRecordHandler::HandleRecord

********************************************************************************************/


BOOL PathProcessorBrush::WriteNative(BaseCamelotFilter *pFilter)
{
	ERROR3IF(pFilter == NULL, "Illegal NULL param");
	
	CXaraFileRecord Record(TAG_BRUSHATTR, TAG_BRUSHATTR_SIZE);
	BOOL ok = Record.Init();
	
	BYTE Flags = 0;
	if (m_bTile) 
		Flags |= TAG_BRUSHTILE_FLAG;
	if (m_bRotate)
		Flags |= TAG_BRUSHROTATE_FLAG;

//	INT32 Offset = (INT32)m_PathOffsetType;

	if (ok) ok = Record.WriteUINT32(m_BrushHandle);
	if (ok) ok = Record.WriteINT32(m_BrushSpacing);
	if (ok) ok = Record.WriteBYTE(Flags);
	if (ok) ok = Record.WriteDOUBLE(m_RotateAngle);
	if (ok) ok = Record.WriteINT32(m_PathOffsetType);
	if (ok) ok = Record.WriteINT32(m_PathOffsetValue);
	if (ok) ok = Record.WriteDOUBLE(m_BrushScaling);
	if (ok) ok = pFilter->Write(&Record);

	CXaraFileRecord NextRec(TAG_MOREBRUSHATTR, TAG_MOREBRUSHATTR_SIZE);
	if (ok) ok = NextRec.Init();
	if (ok) ok = NextRec.WriteDOUBLE(m_BrushSpacingIncrProp);
	if (ok) ok = NextRec.WriteINT32(m_BrushSpacingIncrConst);
	if (ok) ok = NextRec.WriteINT32((INT32)m_BrushSpacingMaxRand);
	if (ok) ok = NextRec.WriteINT32((INT32)m_BrushSpacingRandSeed);

	if (ok) ok = NextRec.WriteDOUBLE(m_BrushScalingIncr);
	if (ok) ok = NextRec.WriteINT32((INT32)m_BrushScalingMaxRand);
	if (ok) ok = NextRec.WriteINT32((INT32)m_BrushScalingRandSeed);

	if (ok) ok = NextRec.WriteINT32((INT32)m_SequenceType);
	if (ok) ok = NextRec.WriteINT32((INT32)m_SequenceRandSeed);

	if (ok) ok = NextRec.WriteDOUBLE(m_PathOffsetIncrProp);
	if (ok) ok = NextRec.WriteINT32(m_PathOffsetIncrConst);
	if (ok) ok = NextRec.WriteINT32((INT32)m_OffsetTypeRandSeed);
	if (ok) ok = NextRec.WriteINT32((INT32)m_OffsetValueMaxRand);
	if (ok) ok = NextRec.WriteINT32((INT32)m_OffsetValueRandSeed);
	INT32 UseFill = m_bUseLocalFillColour ? 1234 : 0;
	if (ok) ok = NextRec.WriteINT32(UseFill);

	if (ok) ok = pFilter->Write(&NextRec);

	CXaraFileRecord AnotherRec(TAG_EVENMOREBRUSHATTR, TAG_EVENMOREBRUSHATTR_SIZE);
	if (ok) ok = AnotherRec.Init();
	if (ok) ok = AnotherRec.WriteINT32((INT32)m_RotationMaxRand);
	if (ok) ok = AnotherRec.WriteINT32((INT32)m_RotationRandSeed);
	BYTE ScaleFlag = 0;
	ScaleFlag |= TAG_SCALETOWIDTH_FLAG;
	if (ok) ok = AnotherRec.WriteBYTE(ScaleFlag);
	if (ok) ok = pFilter->Write(&AnotherRec);

	// write the fill flags out
	CXaraFileRecord FillRec(TAG_BRUSHATTRFILLFLAGS, TAG_BRUSHATTRFILLFLAGS_SIZE);
	if (ok) ok = FillRec.Init();
	BYTE FillFlags = 0;
	if (m_bUseLocalFillColour)
		FillFlags |= BRUSHFILLFLAG_LOCALFILL;

	if (m_bUseLocalTransp)
		FillFlags |= BRUSHFILLFLAG_LOCALTRANSP;

	if (m_bUseNamedColours)
		FillFlags |= BRUSHFILLFLAG_NAMEDCOL;

	if (ok) ok = FillRec.WriteBYTE(FillFlags);
	if (ok) ok = pFilter->Write(&FillRec);
	
	// write the pressure info
	CXaraFileRecord PressureRec(TAG_BRUSHATTRPRESSUREINFO, TAG_BRUSHATTRPRESSUREINFO_SIZE);
	if (ok) ok = PressureRec.Init();
	if (ok) ok = PressureRec.WriteINT32((INT32)m_ScalingMaxPressure);
	if (ok) ok = PressureRec.WriteINT32((INT32)m_SpacingMaxPressure);
	if (ok) ok = PressureRec.WriteINT32((INT32)m_OffsetMaxPressure);
	if (ok) ok = PressureRec.WriteINT32((INT32)m_RotationMaxPressure);
	if (ok) ok = PressureRec.WriteINT32((INT32)m_HueMaxPressure);
	if (ok) ok = PressureRec.WriteINT32((INT32)m_SatMaxPressure);
	if (ok) ok = PressureRec.WriteINT32((INT32)m_TimeStampMaxPressure);
	if (ok) ok = pFilter->Write(&PressureRec);

	// write the transparency data - plus a few other things I forget elsewhere
	CXaraFileRecord TranspRec(TAG_BRUSHATTRTRANSPINFO, TAG_BRUSHATTRTRANSPINFO_SIZE);
	if (ok) ok = TranspRec.Init();
	if (ok) ok = TranspRec.WriteINT32(m_BrushTransparency);
	if (ok) ok = TranspRec.WriteINT32(INT32(m_TranspMaxPressure));
	if (ok) ok = TranspRec.WriteDOUBLE(m_RotAngleIncrConst);
	if (ok) ok = TranspRec.WriteDOUBLE(m_BrushScalingIncrConst);
	if (ok) ok = TranspRec.WriteINT32(INT32(m_BrushHueMaxRand));
	if (ok) ok = TranspRec.WriteINT32(INT32(m_BrushHueRandSeed));
	if (ok) ok = TranspRec.WriteINT32(INT32(m_BrushSatMaxRand));
	if (ok) ok = TranspRec.WriteINT32(INT32(m_BrushSatRandSeed));
	
	if (ok) ok = pFilter->Write(&TranspRec);

	// recall that the timestamping data and the pressure data belong to
	// the attribute so they are written out there

	return ok;
}
