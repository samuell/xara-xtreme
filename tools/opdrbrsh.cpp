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

// Implementation of OpDrawBrush

#include "camtypes.h"
#include "opdrbrsh.h"
#include "trans2d.h"
#include "pen.h"
#include "docview.h"
#include "spread.h"
#include "app.h"
#include "opfree.h"
//#include "resource.h"
#include "freehand.h"
#include "csrstack.h"
#include "undoop.h"
#include "combshps.h"
//#include "rik.h"
//#include "bubbleid.h"
#include "ndbldpth.h"
//#include "mario.h"
#include "progress.h"
#include "range.h"
//#include "grndbrsh.h"
#include "objchge.h"
#include "nodeattr.h"
#include "attrmap.h"
#include "nodershp.h"
#include "opbevel.h"
#include "nodebev.h"
//#include "markn.h"
#include "blndtool.h"
#include "brshattr.h"
#include "brshcomp.h"
#include "pathproc.h"
#include "lineattr.h"
#include "freehand.h"
//#include "freehres.h"
#include "fixmem.h"
#include "ccobject.h"
#include "rsmooth.h"
#include "camelot.h"
#include "nodeshad.h"
#include "nodecntr.h"
#include "lineattr.h"
#include "samplist.h"
#include "linwthop.h"
//#include "scrcamvw.h"
#include "ophist.h"

#include "qualattr.h"

// Declare the class
CC_IMPLEMENT_DYNCREATE( OpDrawBrush, SelOperation );
CC_IMPLEMENT_DYNCREATE( OpChangeBrush, SelOperation );
CC_IMPLEMENT_DYNAMIC(ChangeBrushOpParam, OpParam);
CC_IMPLEMENT_DYNCREATE(ChangeBrushAction, Action);
CC_IMPLEMENT_DYNCREATE(RemoveTimeStampPointsAction, Action);
CC_IMPLEMENT_DYNCREATE(AddTimeStampPointsAction, Action);
CC_IMPLEMENT_DYNCREATE(UpdateBrushAction, Action);
CC_IMPLEMENT_DYNCREATE(SetTimeStampUpdateTypeAction, Action);
CC_IMPLEMENT_DYNCREATE(RemovePressurePointsAction, Action);
CC_IMPLEMENT_DYNCREATE(AddPressurePointsAction, Action);

#define DELPTR(p) if (p != NULL) { delete p; p = NULL; }
#define SWAP(type,a,b) { type x=a; a=b; b=x; }

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

const UINT32 PressureValsThreshold = 3; //basically the first 3 or so values coming out of the 

									  // pressure pen are always zero, so we must ignore them



/********************************************************************************************

>	OpDrawBrush::OpDrawBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Purpose:	Constructor.	
********************************************************************************************/

OpDrawBrush::OpDrawBrush()
{
	ResetMembers();
}

/********************************************************************************************

>	OpDrawBrush::OpDrawBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Purpose:	Constructor. This simply sets a few of the operation flags.

********************************************************************************************/

OpDrawBrush::OpDrawBrush(FreeHandTool *pTool)
{
	ResetMembers();
	if (pTool != NULL)
		m_pTool = pTool;
}


/********************************************************************************************

>	OpDrawBrush::~OpDrawBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Purpose:	destructor.

********************************************************************************************/

OpDrawBrush::~OpDrawBrush()
{
//	if (m_pGRenderBrush != NULL)
//		delete m_pGRenderBrush;
	if (m_pProcPathDistance != NULL)
		delete m_pProcPathDistance;


	if (m_pPathProcBrush != NULL)
		delete m_pPathProcBrush;

	if (m_pPressureList != NULL)
		delete m_pPressureList;

	if (m_pPointsCache != NULL)
		delete m_pPointsCache;

	if (m_pPressureSampler != NULL)
		delete m_pPressureSampler;
}


/********************************************************************************************

>	void OpDrawBrush::ResetMembers()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Purpose:	Initialises all our member variables to something sensible

********************************************************************************************/

void OpDrawBrush::ResetMembers()
{
	// Set all our cursors to NULL to start with
	pFreeHandCursor = NULL;
	pJoinCursor = NULL;
	pStraightCursor = NULL;
	pRubOutCursor = NULL;
	pModifyCursor = NULL;
	
	// Set other default values
	TrackData = NULL;
	StartSpread = NULL;
	PreviousSpread = NULL;
	Smoothness = 512;
	LineSegmentCount = 0;
	CanLineJoin = FALSE;
	IsStraightLineMode = FALSE;
	AddPressureToPath = FALSE;
	FreeHandPressure = 0;

	// The paths that we are joined to, or NULL if we are joined to none
	pJoinInfo = NULL;
	StartPath = NULL;
	EndPath = NULL;
	CloseTo = 0;
	Mu = 0.0;
	IsEndNearEndpoint = FALSE;
	CurrentCursorID = 0;

	m_pGRenderBrush = NULL;
	m_pPathProcBrush = NULL;
//#ifdef NEWFASTBRUSHES
	m_pBrushDefinition = NULL;
	mustClearStatusBar = TRUE;
	pApp = GetApplication ();
	ERROR3IF (!pApp, "Could not find the application!");
//#endif
	m_pProcPathDistance = NULL;
	m_BrushScaling = 1.0;

#ifdef OPBRUSHPOINTSCACHE
	m_pPointsCache = NULL;
#endif
	
	m_pTimeStampList = NULL;
	m_TimeStamp      = 0;
	m_LastTimeStamp  = -1;

	m_pPressureList   = NULL;

	m_LastPathIndexRendered = 1;

	m_FirstSpacing = 25;

	m_pPressureSampler = NULL;
	m_NumPressureVals = 0;


}

/********************************************************************************************

>	void OpDrawBrush::DoDrag( DocCoord Anchor, Spread *pSpread, INT32 Smooth, BrushHandle Handle, 
							NodePath* pPath, Path* ToolPath, GRenderBitmap* pGRndBMP )

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		Anchor 	- The position of the mouse at the start of the Drag
				pSpread - The spread that the drag started on
				Smooth	- Closeness of fit value
				Handle - the handle of the brush we want to use
				pFreeHandInfo - Pointer to the info about the Joining set up by the
				freehand tool
				ToolPath - The path to store the mouse tracking info in
				pGRndBMP - pointer to the big bitmap used to store the view
				
	Purpose:	Starts up the drag operation by storing all start positions and setting
				up a Path to store the mouse movement in

********************************************************************************************/

void OpDrawBrush::DoDrag(DocCoord Anchor, Spread *pSpread, INT32 Smooth, BrushHandle Handle,
						FreeHandJoinInfo* pFreeHandInfo, Path* ToolPath,
						GRenderBrush* pGRndRgn, ClickModifiers ClickMods)
{
	TRACEUSER( "Diccon", _T("Drag started\n"));
	if (pSpread == NULL)
	{
		ERROR3("No spread");
		FailAndExecute();
		End();
		return;
	}
	if (ToolPath == NULL)
	{
		ERROR3("Tool path is NULL");
		FailAndExecute();
		End();
		return;
	}
// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	// Tell the pressure pen that we're starting a new stroke
	CCPen *pPen = pApp->GetPressurePen();
	if (pPen != NULL)
		pPen->StartStroke();
#endif // VECTOR_STROKING

	// Snap the starting coord if we are not joining to something else
	if ((pFreeHandInfo==NULL) || (pFreeHandInfo->pJoinPath==NULL))
		DocView::SnapCurrent(pSpread, &Anchor, FALSE, TRUE);

	// Make a note of various starting conditions
	TrackData 	= ToolPath;
	Smoothness 	= Smooth;
	pJoinInfo	= pFreeHandInfo;
	StartPath	= pJoinInfo->pJoinPath;
	EndPath		= NULL;

	// Make a mental note of the start point
	StartPoint    = Anchor;
	StartSpread   = pSpread;
	PreviousSpread= pSpread;
	PreviousPoint = Anchor;
	LineSegmentCount = 0;
	IsStraightLineMode = FALSE;
	CanLineJoin = FALSE;

	// Prepare the Track data path and add in the initial click point to the path
	if (!PrepareTrackDataPath())
	{
		// We failed to get the memory to initialise the track data
		InformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
		FailAndExecute();
		End();
		return;
	}
	
	// Create some cursors that I might need
	if (!LoadCursors())
	{
		// The cursors did not load, so fail?
		FailAndExecute();
		End();
		return;
	}


	// Push my new cursor onto the stack
	CurrentCursorID = CursorStack::GPush(pFreeHandCursor, TRUE);
	MyCurrentCursor = pFreeHandCursor;

	// Diccon new brush stuff
	TRACEUSER( "Diccon", _T("DRAG STARTED\n"));
	if (pGRndRgn == NULL)
	{
		ERROR3("No GRenderBrush");
		FailAndExecute();
		End();
		return;
	}
	else
		m_pGRenderBrush = pGRndRgn;

	// if we are joining a brush then make sure we know that handle
	if (pFreeHandInfo->m_BrushHandle != BrushHandle_NoBrush)
		Handle = pFreeHandInfo->m_BrushHandle;

	if (!InitialisePathProcessorBrush(Handle, pFreeHandInfo->BrushDistance))
	{
		FailAndExecute();
		End();
		return;
	}	

	// if out freehand info indicates that we are joining to an existing brush then 
	// copy out the data from that brush
	if (pFreeHandInfo->m_BrushHandle != BrushHandle_NoBrush && pFreeHandInfo->pAttrBrush != NULL)
	{	
//		MILLIPOINT Spacing = 0;
		m_pPathProcBrush->GetSpacingAndScalingAtDistance(pFreeHandInfo->BrushDistance, &m_FirstSpacing, &m_BrushScaling);
	
		m_pPathProcBrush->CopyDataFromObject(&(pFreeHandInfo->m_BrushData));
		m_LastSpacing = m_pPathProcBrush->GetSpacing();
	
		// set the scaling, this can vary as we may be adding to a line
		// of any width
		m_BrushScaling = pFreeHandInfo->m_BrushData.m_BrushScaling;	
		
		m_pPathProcBrush->AdvanceBrushToDistance(pFreeHandInfo->BrushDistance);

		m_LastInkObjectRendered = m_pPathProcBrush->GetLastObject();

		// if we're overriding colours then tell the ppb what colour to use
		m_pPathProcBrush->SetUseLocalFillColour(pFreeHandInfo->UseLocalColour);
		m_pPathProcBrush->SetUseNamedColours(pFreeHandInfo->UseNamedColour);
		if (pFreeHandInfo->UseLocalColour || !pFreeHandInfo->UseNamedColour)
			m_pPathProcBrush->SetStrokeColour(pFreeHandInfo->StrokeColour);

	//	m_FirstSpacing = m_pPathProcBrush->GetLastSpacing() - pFreeHandInfo->FirstBrushSpacing;

	//	TRACEUSER( "Diccon", _T("JOINING EXISTING BRUSH\n"));
	}

	if (!InitialiseProcessPathDistance())
	{
		FailAndExecute();
		End();
		return;
	}

	if (!InitialisePressureSampler())
	{
		FailAndExecute();
		End();
		return;
	}
	if (AddPressureToPath == TRUE)
	{
	//	TRACEUSER( "Diccon", _T("Pressure = %d\n"), ClickMods.Pressure);
	//	m_pPressureSampler->CollectData(Anchor, ClickMods.Pressure);  //?? not sure this is right
	}

	if (m_pPathProcBrush != NULL)
	{
		m_NumInkObjects = m_pPathProcBrush->GetNumBrushObjects();
		m_BrushSpacing = m_pPathProcBrush->GetSpacing();
		m_pPathProcBrush->SetParentAttribute(pFreeHandInfo->pAttrBrush);
	}
	else
	{
		FailAndExecute();
		End();
		return;
	}
	if (m_NumInkObjects < 1)
	{
		ERROR3("No brush ink objects");
		FailAndExecute();
		End();
		return;
	}

	// if we are not timestamping then we want a regular point-at-distance map to
	// use as a cache, so long as we are not joining an existing brush
#ifdef OPBRUSHPOINTSCACHE	
	if (m_TimeStamp <= 0 && pFreeHandInfo->m_BrushHandle == BrushHandle_NoBrush)
	{
		// allocate the map for caching
		m_pPointsCache = new PointsMap;

		//m_pPointsList = new List;
		// not a disaster if we don't get it
//		if (m_pPointsCache != NULL)
//			m_pPointsCache->InitHashTable(1277);	
//		else
//			ERROR3("Failed to allocate cache");
	}
#endif
	// if we are timestamping then we want a list
	if (m_TimeStamp > 0)
	{
		if (!InitialiseTimeStampList())
		{
			// can't do anything if we don't get our cache
			FailAndExecute();
			End();
			return;
		}
	}

	// initialise needs to have correct start point
	m_StartPoint = Anchor;
	
	// set up the member variables
	m_LastPoint = Anchor;
	m_LastPointRendered = Anchor;
	m_LastDistanceRendered = 0;
	m_DistanceSinceLastObject = 0;
	m_LastSpacing = m_BrushSpacing;
	m_NumBrushObjects = 0;

	AddPointsToPath(Anchor, pSpread);
	m_bFirstPointRendered = FALSE;

	StartDrag( DRAGTYPE_NOSCROLL );

//	TRACEUSER( "Diccon", _T("Drag initialised"));

PORTNOTE("other", "Removed m_pRenderGBrush");
#ifndef EXCLUDE_FROM_XARALX
//#ifdef NEWFASTBRUSHES
	View*	pView = View::GetCurrent();
	CCamView* pCCamView = pView->GetConnectionToOilView();
	CDC* pDevContext = pCCamView->GetRenderDC();
	HDC DeviceHdc = pDevContext->GetSafeHdc();

	m_pGRenderBrush->SetView (pView);
	m_pGRenderBrush->SetCamView (pView->GetConnectionToOilView());
	m_pGRenderBrush->SetCDC (pCCamView->GetRenderDC());
	m_pGRenderBrush->SetHDC (pDevContext->GetSafeHdc());
//#endif
#endif
}



/********************************************************************************************

>	void OpDrawBrush::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								  Spread *pSpread, BOOL bSolidDrag)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
				pSpread - The spread that the mouse is moving over
	Purpose:	Handles the event of the mouse moving during a drag
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpDrawBrush::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								  Spread *pSpread, BOOL bSolidDrag)
{
	// Lets whip out the current pressure value
	if (AddPressureToPath)
		FreeHandPressure = ClickMods.Pressure;

	// If drag has moved onto a different spread, convert the coord to be relative to the
	// original spread.
	if (pSpread != StartSpread)
#ifdef NEWFASTBRUSHES
		PointerPos = MakeRelativeToSpreadNoOverhead(StartSpread, pSpread, PointerPos);	// min variable allocation overhead version
#else
		PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);	// min variable allocation overhead version
#endif

	// I'm not allowing rubbing out or straightlines whilst drawing a brush
	ClickMods.Alternative1 = FALSE;
	ClickMods.Adjust = FALSE;

	// Change the Cursor to display the appropriate thing.
	SetCursorOnMove(ClickMods, StartSpread, &PointerPos);

	AddPointsToPath(PointerPos, StartSpread);
	// Set the last spread to something appropriate
	PreviousSpread = pSpread;

	//  brush specific code
	double Distance = (PointerPos.Distance(m_LastPoint));
	m_DistanceSinceLastObject += (MILLIPOINT)Distance;
		// cache the point for this brush object
	m_LastPoint = PointerPos;
	// pass the data to our pressure sampler
	if (m_pPressureSampler != NULL)
	{
		m_NumPressureVals++;
		if (m_NumPressureVals > PressureValsThreshold)
		{
			TRACEUSER( "Diccon", _T("Pressure = %d\n"), ClickMods.Pressure);
			
			if (!m_pPressureSampler->CollectData(PointerPos, FreeHandPressure))
			{
				ERROR3("Error in Pressure sampler, aborting..");
				FailAndExecute();
				End();
				return;
			}
		}
		else
			return;
		
	}
		// --- If the quality is set low enough, strokes are just rendered as centrelines
		// "low enough" is defined as the same point that Blends show their middle parts
		// BLOCK
		{
PORTNOTE("other", "Removed m_pRenderGBrush");
#ifndef EXCLUDE_FROM_XARALX
			if(m_pGRenderBrush != NULL)
			{
				QualityAttribute *pQuality = (QualityAttribute *) m_pGRenderBrush->GetCurrentAttribute(ATTR_QUALITY);
				if (pQuality != NULL && pQuality->QualityValue.GetBlendQuality() != Quality::FullBlend)
				{
					RenderRegion* pRegion = DocView::RenderOnTop(NULL, pSpread, UnclippedEOR );
					while ( pRegion )
					{
						RenderLine(pRegion, TrackData, TrackData->GetNumCoords()-1, FALSE);
						// get the next region to draw in
						pRegion = DocView::GetNextOnTop(NULL);
					}	
					return;
				}
			}
#endif
		}

//	if (pJoinInfo->BrushHandle != BrushHandle_NoBrush)
//		RenderLine(m_pGRenderBrush, TrackData, TrackData->GetNumCoords()-1);
	// Render brush objects for the specified period
	RenderStepsForTime(50, pSpread);

	// if we are editing then render the path too
/*	if (pJoinInfo->BrushHandle != BrushHandle_NoBrush)
	{
		RenderLine(NULL, pSpread, TrackData, TrackData->GetNumCoords() - 1);
		RenderLine(m_pGRenderBrush, TrackData, TrackData->GetNumCoords()-1);
	}*/
}	



/********************************************************************************************

>	void OpDrawBrush::DragPointerIdle( DocCoord PointerPos, ClickModifiers ClickMods, 
								  Spread *pSpread, BOOL bSolidDrag)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
				pSpread - The spread that the mouse is moving over
	Purpose:	Handles the mouse idle events
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpDrawBrush::DragPointerIdle(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag)
{
	BOOL bAllowRender = TRUE;
	
	// --- If the quality is set low enough, strokes are just rendered as centrelines
	// "low enough" is defined as the same point that Blends show their middle parts
	// BLOCK
	{
PORTNOTE("other", "Removed m_pRenderGBrush");
#ifndef EXCLUDE_FROM_XARALX
		if(m_pGRenderBrush != NULL)
		{
			QualityAttribute *pQuality = (QualityAttribute *) m_pGRenderBrush->GetCurrentAttribute(ATTR_QUALITY);
			if (pQuality != NULL && pQuality->QualityValue.GetBlendQuality() != Quality::FullBlend)
			{
				bAllowRender = FALSE;
				// return;
			}
		}
#endif
	}

	// because we are throwing away the first few pressure values we want to make sure we have 
	// got enough before we render here 
	if (AddPressureToPath)
	{
		if (m_NumPressureVals > PressureValsThreshold && bAllowRender)
			RenderStepsForTime(75, pSpread); // 75 is the time to render for in ms, as determined by trial and error
	}
	else
	{
		if(bAllowRender)
			RenderStepsForTime(75, pSpread);
	}
	
	//TRACEUSER( "Diccon", _T("Drag pointer idle\n"));	
}

/********************************************************************************************

>	void OpDrawBrush::RenderStepsForTime(double TimeSlice, Spread* pSpread)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/99
	Inputs:		TimeSlice - the length of time to render for
				pSpread - the spread to render on to
	Returns:	-
	Purpose:	Renders as many steps in the brush as possible in the time available.
				The reason for having the code here rather than in the processor is twofold:
				1) I feel the property of rendering for a certain timeslice is a property of
				the Op rather than the PPB;
				2) In order to make use of the caching ProcessPathDistance, which belongs
				to the Op rather than the PPB.
********************************************************************************************/

/* Notes: In case its not clear from the code what is happening here we go: the member variable
m_DistanceSinceLastObject records how far the mouse has travelled since we last rendered an object.
So long as this distance is greater than our spacing value then we wish to keep rendering points, and
reducing m_DistanceSinceLastObject until it becomes less than our spacing value.
*/

void OpDrawBrush::RenderStepsForTime(double TimeSlice, Spread* pSpread)
{
	if ( (m_pProcPathDistance == NULL) || (m_pPathProcBrush == NULL)
		|| (m_pGRenderBrush == NULL) || (TrackData == NULL))
	{
		ERROR3("One or more members are NULL");
		return;
	}

	MonotonicTime timer;
	
	// declare locals outside the loop
	DocCoord Point;
	double Angle;
	UINT32 Pressure = 0;
	CSampleItem TheItem;
	BrushPointInfo PathPoint;

	// records how far along the path we are
	MILLIPOINT DistanceToGetPoint = m_LastDistanceRendered;

	// spacing records the spacing between objects (duh)
	// this little hack is here to make sure our first object is a few millipoints in from the 
	// start of the path, otherwise we will always have a tangetial angle of zero

	MILLIPOINT Spacing = m_LastSpacing;
	if (!m_bFirstPointRendered)
	{
		Spacing = m_FirstSpacing;
		if (m_pPressureSampler != NULL)
		{
			if (m_pPressureSampler->GetAt(0, &TheItem))
				Pressure = TheItem.m_Pressure;
			else
				Pressure =127;
			m_pPathProcBrush->SetCurrentPressure(Pressure);
		}
	}


	BOOL Found;
	//m_BrushScaling = m_pPathProcBrush->GetLastScaling();
#ifdef SCALESPACING
	double ActualSpacing = (double)Spacing * m_BrushScaling;
#else
	double ActualSpacing = (double)Spacing * m_pPathProcBrush->GetBrushScaling();
#endif
//	if (TrackData->GetNumCoords() > 2)
//		RenderLine(m_pGRenderBrush, TrackData, TrackData->GetNumCoords() - 1);
	UINT32 Dummy = 0;
	// render objects at m_BrushSpacing intervals starting at the last point rendered
	while (m_DistanceSinceLastObject >= (MILLIPOINT)ActualSpacing)
	{
		// the path processor needs to set up some variables to deal with offsets
		// before we come to actually render
		m_pPathProcBrush->SetNextOffsetType();

		//TRACEUSER( "Diccon", _T("Scaled Spacing = %f\n"), double(Spacing * m_BrushScaling));
		DistanceToGetPoint += (MILLIPOINT)ActualSpacing;
		
		// get the point from the nodebrushpath
		m_pProcPathDistance->GetCoordAndTangentWithCache(&Point, &Angle, &Found,
														DistanceToGetPoint, TrackData, &Dummy);
		if (Found)
		{
			
			//TRACEUSER( "Diccon", _T("Point found at: %d, %d\n"), Point.x, Point.y);
			PathPoint.m_Point = Point;
			PathPoint.m_Tangent = Angle;
			PathPoint.m_Pressure = Dummy;

			// pass to the pathprocessor which does the work of rendering, but not if our object is zero-sized
PORTNOTE("other", "Removed m_pRenderGBrush");
#ifndef EXCLUDE_FROM_XARALX
			if (m_BrushScaling > 0)
			{
				m_pPathProcBrush->RenderBrushAtPoint(Point, Angle, m_LastInkObjectRendered, m_NumBrushObjects,
													m_pGRenderBrush, NULL, TRUE, m_pBrushDefinition);
			}
#endif			
			// get the next pressure value
			if (m_pPressureSampler != NULL && m_bFirstPointRendered)
			{
				m_pPressureSampler->SetSampleRateFromSpacing((MILLIPOINT)ActualSpacing);
				
				if (m_pPressureSampler->GetNext(&TheItem))
					Pressure = TheItem.m_Pressure;
				else
					Pressure = 127;
				m_pPathProcBrush->SetCurrentPressure(Pressure);
				//TRACEUSER( "Diccon", _T("SETTING PRESSURE %d = %d\n"), m_NumBrushObjects, Pressure);
			}

PORTNOTE("other", "Removed m_pRenderGBrush");
#ifndef EXCLUDE_FROM_XARALX
			// get it to the screen ASAP
			m_pGRenderBrush->DrawToScreenNow();
#endif

			m_DistanceSinceLastObject -= (MILLIPOINT)(ActualSpacing); //m_BrushSpacing;
			m_LastDistanceRendered = DistanceToGetPoint;

			// get the next spacing (spacing only changes once we have drawn an object)
			Spacing = m_pPathProcBrush->GetNextSpacing();
			m_BrushScaling = m_pPathProcBrush->GetLastScaling();
#ifdef SCALESPACING
			ActualSpacing = Spacing * m_BrushScaling;
#else
			ActualSpacing = (double)Spacing * m_pPathProcBrush->GetBrushScaling(); //m_BrushSpacing; //Spacing * m_BrushScaling;
#endif
			// switch our alternating offset 
			m_pPathProcBrush->SwitchAlternateValue();

#ifdef OPBRUSHPOINTSCACHE
			// stash this point in the cache
			if (m_pPointsCache != NULL)
				(*m_pPointsCache)[DistanceToGetPoint]=PathPoint;
#endif
		 
			// get the next object in the sequence
			m_LastInkObjectRendered = m_pPathProcBrush->GetNextInkObject(m_LastInkObjectRendered,
																	 m_NumInkObjects);

			m_pPathProcBrush->DecideWhetherToUseActualScaling(m_LastInkObjectRendered);
			// increment our counter
			m_NumBrushObjects++;
			m_bFirstPointRendered = TRUE;
		//	TRACEUSER( "Diccon", _T("Pressure at end of loop = %d\n"), Pressure);
		}

		// if we hit the time constraint then just get out now
		if (timer.Elapsed(UINT32(TimeSlice * 1000.0)))
			break;
	}
//	TRACEUSER( "Diccon", _T("Exit RenderStepsForTime\n"));
	// remember the spacing for next time
	m_LastSpacing = Spacing;
}


/********************************************************************************************

>	void OpDrawBrush::RenderTimeStamp()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/99
	Inputs:		-
	Returns:	-
	Purpose:	To be used in timestamp mode, obviously.  This fn. determines whether or not 
				enough time has elapsed for us to render a point.  If so we render a point
				and cache it, updating the necessary members.
********************************************************************************************/

void OpDrawBrush::RenderTimeStamp()
{
	if (m_TimeStamp <=0)
	{
		ERROR3("Invalid timestamp value");
		return;
	}

	if (m_Timer.Elapsed((UINT32)(m_TimeStamp * 1000.0)))
	{	
		m_Timer.Sample();
		
		// find out how far along the path we are
		MILLIPOINT DistanceAlongPath = m_LastDistanceRendered + m_DistanceSinceLastObject;
		
		DocCoord Point;
		double Angle = 0;
		BOOL Found = FALSE;
		// get the point from the path
		m_pProcPathDistance->GetCoordAndTangentWithCache(&Point, &Angle, &Found,
														DistanceAlongPath, TrackData);
	
		if (Found)
		{	
			TimeStampBrushPoint TSP(Point, Angle, DistanceAlongPath);
			
				// get the next object in the sequence
		m_LastInkObjectRendered = m_pPathProcBrush->GetNextInkObject(m_LastInkObjectRendered,
																	 m_NumInkObjects);

PORTNOTE("other", "Removed m_pRenderGBrush");
#ifndef EXCLUDE_FROM_XARALX
			// pass to the pathprocessor which does the work of rendering
			if (m_pGRenderBrush != NULL)
			{
				m_pPathProcBrush->RenderBrushAtPoint(Point, Angle, m_LastInkObjectRendered++, 1,
												m_pGRenderBrush, NULL, TRUE);
				// get it to the screen ASAP
				m_pGRenderBrush->DrawToScreenNow();
				TRACEUSER( "Diccon", _T("OpDrawBrush Rendered TimeStamp"));
			}
#endif
			// update our member variables
			m_DistanceSinceLastObject = 0;
			m_LastDistanceRendered = DistanceAlongPath;

			// cache the point
			if (m_pTimeStampList != NULL)
				m_pTimeStampList->push_back(TSP);
		}
	}
}



/********************************************************************************************

>	void OpDrawBrush::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
							   Spread *pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				pSpread - The spread that the drag finished on
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	Handles the drag finishing by rubbing out an EOR on the screen and
				adding the path to the tree and building all the undo that will be
				needed
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpDrawBrush::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
							   Spread *pSpread, BOOL Success, BOOL bSolidDrag)
{	
	TRACEUSER( "Diccon", _T("Drag finished\n"));
	// Erase the whole EORed line
	DocRect ClipRect(0,0,0,0);
	RenderDragBlobs(ClipRect, StartSpread, bSolidDrag);
	
	// Get rid of all the cursors
	RemoveCursors();

	// Put the hour glass up as we have to and end the drag
	//BeginSlowJob();
	String_32 ProgString = _T("Calculating brush, please wait..");
	Progress Hourglass(&ProgString, -1, FALSE);

	EndDrag();
	
	
	// Add the new path to the tree if it was valid
	BOOL Worked = FALSE;
	if (Success)
	{
		// were we in the middle of drawing a straight line
		if (IsStraightLineMode)
		{
			// we were so add it in to the track data
			AddStraightLine();
			PreviousPoint = StraightLinePos;
			IsStraightLineMode = FALSE;
		}
		BeginSlowJob(-1, TRUE, NULL);
		
		// invalidate the rect that we calculated (this may be different to the bounding 
		// rect of the node due to smoothing)
		DocRect BRect = m_pPathProcBrush->GetCachedRect();
// Get rid of cached bitmaps here!
Node* pANode = NULL;
if (m_pPathProcBrush->GetParentAttribute())
{
	pANode = m_pPathProcBrush->GetParentAttribute()->FindParent();
	if (pANode->IsBounded())
	{
		((NodeRenderableBounded*)pANode)->ReleaseCached();
	}
}
		DoInvalidateRegion(pSpread, BRect);
		
		// clean up some memory we used
		m_pPathProcBrush->CleanUpAfterRender();

		BrushDefinition* pDef = BrushComponent::FindBrushDefinition(m_pPathProcBrush->GetBrushDefinitionHandle());
		if (pDef)
			pDef->StopRender();
		else
			ERROR3("Unable to get brush definition in OpDrawBrush::DragFinished");

		// try and smooth the path and insert it into the tree
		Worked = CompleteOperation();
		EndSlowJob();	
	}
// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	// Inform the pressure pen that we've finished the stroke
	CCPen *pPen = pApp->GetPressurePen();
	if (pPen != NULL)
		pPen->EndStroke();
#endif // VECTOR_STROKING
	

	if (m_pTool != NULL)
		m_pTool->BrushFinished();

	// point the pointer away from the render region (the tool still needs it so we don't delete)
	m_pGRenderBrush = NULL;

	// If the operation failed, then tidy up
	if (Worked==FALSE)
		FailAndExecute();
	else
	{
		// Update all the parents of the effected paths.
		ObjChangeFlags cFlags;
		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
		ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
		UpdateChangedNodes(&ObjChange);
	}
	// End the operation properly
	End();
}


/********************************************************************************************

>	BOOL OpDrawBrush::ApplyAttributes(NodePath* NewPath, Document *pDocument)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/2000
	Inputs:		NewPath - The NodePath to apply the current attributes to
				pDocument - the document this path will be placed into.
	Returns:	TRUE if it worked, FALSE if it did not
	Purpose:	Applies the current attributes to the Node passed in. It has to find the
				current document to do this and will ENSURE if it fails to find one.
				This overridden version also finds the newly applied brush attribute and 
				gives it the cache of points that we have been recording

				This fn. has changed slightly as we now have to comply with the "most recently
				applied attributes" option.  In order to do this both the line width and the
				brush attribute have to be applied via the attribute manager.

				Andy Hills, 18-09-00
				Note - I have temporarily reverted this function to the 24-08-00 version.
				This fixes the problem where, if the above-mentioned option is disabled,
				an access violation occurs.
********************************************************************************************/

BOOL OpDrawBrush::ApplyAttributes(NodePath* pNewPath, Document *pDocument)
{
	ERROR2IF(pNewPath == NULL, FALSE, "pNewNodePath is NULL in OpDrawBrush::ApplyAttributes");
	ERROR2IF(pDocument == NULL, FALSE, "pDocument is NULL in OpDrawBrush::ApplyAttributes");
	
	//ApplyRetroSmoother(pNewPath, 50);
	
	// Find the current document to get the current attributes from
	ENSURE(pDocument!=NULL, "Null Document while setting attributes for new NodePath");
	if (pDocument!=NULL)
	{	
		// we make out own attributes here
		AttrBrushType* pNewAttr = CreateNewAttribute();
		if (pNewAttr == NULL)
			return FALSE;
#ifdef NEWFASTBRUSHES
		BrushAttrValue* pAttrVal = (BrushAttrValue*) pNewAttr->GetAttributeValue();
		pAttrVal->SetBoundsParent (pNewPath);
#endif
		// now make the line the correct width, if someone has selected a line width then we will make it
		// that size, otherwise we will use the default provided by the brush
		MILLIPOINT LineWidth = GetCurrentLineWidthIfNotDefault();
		if (LineWidth == -1)
			LineWidth = pNewAttr->GetDefaultLineWidth(!AddPressureToPath);
	
	
		// Apply the current attributes to the path, changing the line width if necessary
		if (!pDocument->GetAttributeMgr().ApplyCurrentAttribsToNode((NodeRenderableInk*)pNewPath))
			ERROR3("Failed to apply current attributes in OpDrawBrush::ApplyAttributes");
		
		// change the line width value
		AttrLineWidth* pLineWidth = (AttrLineWidth*)pNewPath->FindFirstChild(CC_RUNTIME_CLASS(AttrLineWidth));
		if (pLineWidth != NULL)
			pLineWidth->Value.LineWidth = LineWidth;
		else
			ERROR3("Unable to find line width in OpDrawBrush::ApplyAttributes");
		
		// if the document inserted a brush then get rid of it
		Node* pDocBrush = pNewPath->FindFirstChild(CC_RUNTIME_CLASS(AttrBrushType));
		if (pDocBrush)
		{
			pDocBrush->CascadeDelete();
			delete pDocBrush;
			pDocBrush = NULL;
		}


		// apply our brush attribute here, in case the document already applied a default
		pNewPath->ApplyAttributeToObject(pNewAttr, FALSE);
		
		// tell it about the freehand tool
		pNewAttr->SetFreeHandTool(m_pTool);

		if (pNewAttr->GetBrushHandle() != BrushHandle_NoBrush)		
		{
					
			#ifdef OPBRUSHPOINTSCACHE
			if (m_TimeStamp <= 0)
				pNewAttr->SetCache(m_pPointsCache);
			
			// make sure we don't try to use the cache, the attribute is now responsible for
			// deleting it
			m_pPointsCache = NULL;
			#endif
			BrushAttrValue* pVal = (BrushAttrValue*)pNewAttr->GetAttributeValue();
			// if we're timestamping then set the timestamp cache instead
			if (m_TimeStamp > 0)
			{	
				if (pVal != NULL)
				{
					pVal->SetTimeStampList(m_pTimeStampList);
					pVal->CalculateProportionalDistances((MILLIPOINT)pNewPath->InkPath.GetPathLength());
				}
			}
			// add the pressure cache, if we need to
			if (AddPressureToPath)
			{
				//m_pPressureVals->SetSize(m_NumPressureVals, 1);
				//m_pPressureVals->FreeExtra();
				m_pPressureSampler->FinishSampling();
				m_pPressureSampler->FreeExtra();

				pNewAttr->SetPressureCache(m_pPressureSampler);
				// set our pointer to null so we don't end up deleting it
				m_pPressureSampler = NULL;
				
				// ask the attribute to calculate the proportional distances
				//if (pVal != NULL)
				//	pVal->CalculatePressureArrayProportions((MILLIPOINT)pNewPath->InkPath.GetPathLength());
			}
		}

		return TRUE;
	}

	// We failed to apply the attributes, so fail
	return FALSE;
}


/********************************************************************************************

>	BOOL OpDrawBrush::ApplyRetroSmoother(NodePath* NewPath, Spread* pSpread, double SmoothAmount)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/2000
	Inputs:		NewPath - The NodePath that we have created
				SmoothAmount - the amount that we wish to smooth (0 -100)
	Returns:	TRUE if it worked, FALSE if it did not
	Purpose:	Initialises the RetroSmoother object and uses it to smooth the nodepath we have
				created.  The reason for using this smoother rather than the FreeHand smoother
				is that it does not seem to change the shape of the path in a visible way.
				It is useful to apply this because otherwise the paths that we draw have too
				many control points which can obscure the brush objects.

********************************************************************************************/

BOOL OpDrawBrush::ApplyRetroSmoother(NodePath* pNodePath, double SmoothAmount)
{
	ERROR2IF(pNodePath == NULL, FALSE, "NodePath is NULL in OpDrawBrush::ApplyRetroSmoother");
	
	// make a smoothing object
	RetroSmooth Smoother;

	if (!Smoother.Initialise())
		return FALSE;

	// tell the smoother we don't want EOR rendering
	Smoother.SetRenderFlag(FALSE);

	// we need to set all our path elements selected for the smoother
	pNodePath->InkPath.SetAllSubSelection();

	// we need the spread for the smoothing operation
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread == NULL)
	{
		ERROR3("Spread is NULL in OpDrawBrush::ApplyRetroSmoother");
		return FALSE;
	}

	Smoother.Changing(pNodePath, pSpread, SmoothAmount);
	Smoother.FinishedNoUndo(pNodePath);

	// unselect our path elements
	pNodePath->InkPath.ClearSubSelection();

	return TRUE;
}



/********************************************************************************************

>	AttrBrushType* OpDrawBrush::CreateNewAttribute()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

AttrBrushType* OpDrawBrush::CreateNewAttribute()
{
		
	AttrBrushType* pNewAttr = new AttrBrushType;
	if (pNewAttr == NULL)
	{
		ERROR3("Couldn't make attribute node");
		return NULL;
	}
	
	BrushAttrValue* pVal = (BrushAttrValue*)pNewAttr->GetAttributeValue();
	if (pVal == NULL)
	{
		ERROR3("No attribute value");
		return NULL;
	}

	if (m_pPathProcBrush == NULL)
	{
		ERROR3("Path Processor Brush is NULL already");
		return NULL;
	}
	pVal->SetPathProcessor(m_pPathProcBrush);

	m_pPathProcBrush->SetParentAttribute(pNewAttr);

	// make sure our processor has all the correct data
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, pNewAttr, "No document in OpDrawBrush::CreateNewAttribute");
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	ERROR2IF(pBrushComp == NULL, pNewAttr, "No brush component in OpDrawBrush::CreateNewAttribute");
	BrushDefinition* pBrushDef = pBrushComp->FindBrushDefinition(m_pPathProcBrush->GetBrushDefinitionHandle());
	ERROR2IF(pBrushDef == NULL, pNewAttr, "Unable to retrieve brush definition, invalid handle");
	
	// copy the data from the definition
	pBrushDef->CopyDataToProcessor(m_pPathProcBrush);

	// scale to the appropriate line width (or not)
	
	MILLIPOINT LineWidth = GetCurrentLineWidthIfNotDefault();
	if (LineWidth != -1)
	{
		m_pPathProcBrush->ScaleToValue(LineWidth, !AddPressureToPath);
	}
	else
	{
		MILLIPOINT Size = pBrushDef->GetDefaultLineWidth(!AddPressureToPath);
		m_pPathProcBrush->ScaleToValue(Size, !AddPressureToPath);
	}

	// make sure we don't delete it now
	m_pPathProcBrush = NULL;
	return pNewAttr;
}


/********************************************************************************************

>	BOOL OpDrawBrush::EditBrushLists(NodePath* pNewPath, INT32 FirstChangedIndex,
										INT32 NumElements)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/2000
	Inputs:		pNewPath - The New curve that has just been draw
				FirstChangedIndex - The first coord in the path that is to be replaced
				NumElements - The number of elements in the path that need replacing
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	When we are editing a brush that contains either timestamping or pressure data 
				and we are editing using a brush we will have generated our own list of TS or 
				pressure info that we need to insert into the existing list.  This fn. manages
				the insertion of these lists and any others that might arise.

********************************************************************************************/

BOOL OpDrawBrush::EditBrushLists(NodePath* pNewPath, INT32 FirstChangedIndex, INT32 NumElements)
{
	ERROR2IF(pNewPath == NULL, FALSE, "pNewPath is NULL in OpDrawBrush::EditBrushLists");

	BOOL ok = TRUE;
	// if we have a timestamp list then attempt to insert it
	if (m_pTimeStampList != NULL)
		ok = EditTimeStampList(pNewPath, FirstChangedIndex, NumElements);
	// edit the pressure list (if there is one)
	if (ok)
		ok = EditPressureList(pNewPath, FirstChangedIndex, NumElements);

	return ok;
}

/********************************************************************************************

>	BOOL OpDrawBrush::EditTimeStampList(NodePath* pNewPath, INT32 FirstChangedIndex,
										INT32 NumElements)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/2000
	Inputs:		pNewPath - The New curve that has just been draw
				FirstChangedIndex - The first coord in the path that is to be replaced
				NumElements - The number of elements in the path that need replacing
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	When a path is edited that has a timestamping brush applied to it we those 
				points are stored in a list that is generated when the path is drawn.  If we 
				subsequently want to edit that path we have to insert or remove points from 
				that list.  This function works out what points should be inserted or removed,
				and performs that action on the applied brush.
				This is made slightly easier

********************************************************************************************/

BOOL OpDrawBrush::EditTimeStampList(NodePath* pNewPath, INT32 FirstChangedIndex, INT32 NumElements)
{
	ERROR2IF(pNewPath == NULL, FALSE, "pNewPath is NULL in OpDrawBrush::EditTimeStampingBrush");

	// if we're not timestamping then just return now
	if (m_pTimeStampList == NULL)
		return TRUE;

	/* First up, find the nearest applied brush attribute. If there isn't one or
	it is not timestamping then we don't have to bother with all this */
	AttrBrushType* pAttrBrush = NULL;

	pAttrBrush = EndPath->GetAppliedBrushAttribute();
	if (pAttrBrush == NULL)
		return TRUE;

	// if we are attempting to edit an existing brush that does not use the timestamping 
	// system then also give up
	if (!pAttrBrush->IsTimeStamping())
		return TRUE;
	
	// most of the action takes place in the attribute value so get that also
	BrushAttrValue* pVal = (BrushAttrValue*)pAttrBrush->GetAttributeValue();
	if (pVal == NULL)
		return TRUE;

	/* next, record the following distances:
	- original path length;
	- distance to first changed point;
	- distance to last changed point;
	*/

//	double OriginalLength = EndPath->InkPath.GetPathLength();
	MILLIPOINT DistToFirstPoint = -1;
	MILLIPOINT DistToLastPoint = -1;
	DocCoord* pCoords = EndPath->InkPath.GetCoordArray();

	DocCoord FirstChangedPoint;
	DocCoord LastChangedPoint;

	if (pCoords == NULL)
		return FALSE;
	if (FirstChangedIndex + NumElements > EndPath->InkPath.GetNumCoords())
	{
		ERROR3("Illegal number of coordinates");
		return FALSE;
	}

	// get the coordinates from the array
	FirstChangedPoint = pCoords[FirstChangedIndex];
	LastChangedPoint  =  pCoords[FirstChangedIndex + NumElements];

	EndPath->InkPath.GetDistanceToPoint(FirstChangedPoint, &DistToFirstPoint);
	EndPath->InkPath.GetDistanceToPoint(LastChangedPoint, &DistToLastPoint);


	/* now get the timestamping points list and ask the attribute to clear all the points
	between the edited distances*/
	
	// first make a removepoints action
	RemoveTimeStampPointsAction* pAction; 
	
	if (RemoveTimeStampPointsAction::Init(this, &UndoActions, pAttrBrush, DistToFirstPoint, DistToLastPoint, NULL, &pAction) == AC_FAIL)
		return FALSE;

	// now insert our new points into the attribute
	AddTimeStampPointsAction* pAddAction;
	
	// find out the distance of the new points to be inserted
	TimeStampBrushPoint StartPoint = m_pTimeStampList->front();
	TimeStampBrushPoint EndPoint   = m_pTimeStampList->back();

	INT32 NewPointsDistance = EndPoint.m_Distance - StartPoint.m_Distance;

	if (AddTimeStampPointsAction::Init(this, &UndoActions, pAttrBrush, m_pTimeStampList, DistToFirstPoint, DistToFirstPoint + NewPointsDistance, 0, &pAddAction) == AC_FAIL)
		return FALSE;

	// invalidate the whole thing3
	if (!DoInvalidateNodeRegion(EndPath, TRUE))
		return FALSE;		

	return TRUE;
}


/********************************************************************************************

>	BOOL OpDrawBrush::EditPressureList(NodePath* pNewPath, INT32 FirstChangedIndex, INT32 NumElements)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/2000
	Inputs:		pNewPath - The New curve that has just been draw
				FirstChangedIndex - The first coord in the path that is to be replaced
				NumElements - The number of elements in the path that need replacing
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	

********************************************************************************************/

BOOL OpDrawBrush::EditPressureList(NodePath* pNewPath, INT32 FirstChangedIndex, INT32 NumElements)
{
	ERROR2IF(pNewPath == NULL, FALSE, "pNewPath is NULL in OpDrawBrush::EditPressureList");

		/* First up, find the nearest applied brush attribute. If there isn't one or
	it has no pressure list then we don't have to bother with all this */
	AttrBrushType* pAttrBrush = NULL;

	pAttrBrush = EndPath->GetAppliedBrushAttribute();
	if (pAttrBrush == NULL)
		return TRUE;

	// if we are attempting to edit an existing brush that does not have an existing pressure
	// cache then leave now
	if (!pAttrBrush->ContainsPressureCache())
		return TRUE;
	
	// most of the action takes place in the attribute value so get that also
	BrushAttrValue* pVal = (BrushAttrValue*)pAttrBrush->GetAttributeValue();
	if (pVal == NULL)
		return TRUE;

	/* next, record the following distances:
	- original path length;
	- distance to first changed point;
	- distance to last changed point;
	*/

//	double OriginalLength = EndPath->InkPath.GetPathLength();
	MILLIPOINT DistToFirstPoint = -1;
	MILLIPOINT DistToLastPoint = -1;
	DocCoord* pCoords = EndPath->InkPath.GetCoordArray();

	DocCoord FirstChangedPoint;
	DocCoord LastChangedPoint;

	if (pCoords == NULL)
		return FALSE;
	if (FirstChangedIndex + NumElements > EndPath->InkPath.GetNumCoords())
	{
		ERROR3("Illegal number of coordinates");
		return FALSE;
	}

	// get the coordinates from the array
	FirstChangedPoint = pCoords[FirstChangedIndex];
	LastChangedPoint  =  pCoords[FirstChangedIndex + NumElements];

	EndPath->InkPath.GetDistanceToPoint(FirstChangedPoint, &DistToFirstPoint);
	EndPath->InkPath.GetDistanceToPoint(LastChangedPoint, &DistToLastPoint);

	// Now find the indices into the pressure list that correspond to those distances
	INT32 StartPressureIndex = pVal->GetPressureListIndexAtDistance(DistToFirstPoint);
	if (StartPressureIndex == -1)
	{
		ERROR3("StartPressureIndex is -1 in OpDrawBrush::EditPressureList");
		return FALSE;
	}
	
	INT32 EndPressureIndex = pVal->GetPressureListIndexAtDistance(DistToLastPoint);
	if (EndPressureIndex == -1 || EndPressureIndex <= StartPressureIndex)
	{
		ERROR3("EndPressureIndex is invalid in OpDrawBrush::EditPressureList");
		return FALSE;
	}
	UINT32 NumObjects = EndPressureIndex - StartPressureIndex;
//	MILLIPOINT Spacing = pAttrBrush->GetSpacing();
	UINT32 NumIndexes = 0; //Spacing / MIN_BRUSH_SPACING;
	StartPressureIndex -= NumIndexes;
	if (StartPressureIndex < 0)
		StartPressureIndex = 0;
	
//	if (m_pPressureList == NULL)
//		m_pPressureList = GeneratePressureList(pAttrBrush, StartPressureIndex, EndPressureIndex, 
//												(MILLIPOINT)pNewPath->InkPath.GetPathLength());	

//	if (m_pPressureList == NULL)  // did it fail, of so we must quit
//		return FALSE;
	/* now get the pressure points list and ask the attribute to clear all the points
	between the edited distances*/
	pAttrBrush->ClearCachedRect();
	DocRect Rect = pAttrBrush->GetAttrBoundingRect(EndPath);
	Spread* pSpread = Document::GetSelectedSpread();

	if (!DoInvalidateRegion(pSpread, Rect))
		return FALSE;
	// Invalidate it as it was
	if (!DoInvalidateNodeRegion(EndPath, TRUE))
		return FALSE;		

	// now make a removepoints action
	RemovePressurePointsAction* pAction;

	if (m_pPressureSampler)
	{
		// only remove items if m_pPressureSampler is non-NULL!
		if (RemovePressurePointsAction::Init(this, &UndoActions, pAttrBrush, (UINT32)StartPressureIndex, NumObjects, NULL, &pAction) == AC_FAIL)
			return FALSE;
	}

	// now insert our new points into the attribute
	AddPressurePointsAction* pAddAction;

	if (m_pPressureSampler)
	{
		// find out how many points we're inserting
		UINT32 NumAddPoints = m_pPressureSampler->GetNumItems();
		
		if (AddPressurePointsAction::Init(this, &UndoActions, pAttrBrush, m_pPressureSampler, (UINT32)StartPressureIndex, NumAddPoints,  &pAddAction) == AC_FAIL)
			return FALSE;
	}
	else
	{
		// YUCK, this means that somebodies trying to 'glue' a non-pressure sensititive path in with the pressure
		// sensitive one ....

		// I think the most sensible thing to do is just to leave current pressure data intact.  Otherwise we get
		// bogged down in bodging default pressure values (which might not be what the user wants/go wrong), and trouble
		// with undo/redo by replacing these values on the fly.
	}

	m_pPressureSampler = NULL; // otherwise we end up deleting it whilst we still need it
	// invalidate the whole thing
	pAttrBrush->ClearCachedRect();
	Rect = pAttrBrush->GetAttrBoundingRect(EndPath);
	EndPath->ReleaseCached();
	
	if (!DoInvalidateRegion(pSpread, Rect))
		return FALSE;
	
	return TRUE;
}

/********************************************************************************************

>	BOOL OpDrawBrush::SimpleJoin(NodePath* pInsertedNode, Path* pNewPath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/2000
	Inputs:		pInsertedNode - the node that we have just inserted
				pPath - the new section of path that we have just drawn
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Joins a Simple path with the new freehand path and builds all the undo that
				is needed in the process.

				This version also incorporates code to insert timestamping or pressure data if
				we are editing such a brush

********************************************************************************************/

BOOL OpDrawBrush::SimpleJoinBrush(NodePath* pInsertedNode, Path* pNewPath)
{
	if (pInsertedNode == NULL || pNewPath == NULL)
	{
		ERROR3("Null inputs to OpDrawBrush::SimpleJoinBrush");
		return FALSE;
	}
	
	// first check to see if the edited path has a brush applied to it,
	// if it doesn't then we'll leave
	NodeAttribute* pAttr;
	AttrBrushType* pAttrBrush;
	pInsertedNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), &pAttr);
	if (pAttr == NULL)
		return TRUE;

	pAttrBrush = (AttrBrushType*)pAttr;
	// if our brush has a 'no brush' handle then likewise
	if (pAttrBrush->GetBrushHandle() == BrushHandle_NoBrush)
		return TRUE;

	// if our brush is not either timestamping or has a pressure list then we will leave aswell
	CDistanceSampler* pPressData = pAttrBrush->GetPressureCache();
	if (pPressData == NULL)
		return TRUE;

	// Do we have a pressure cache??
	if (m_pPressureSampler == NULL)
		return TRUE;

	// find out what type of join we have 
	SimpleJoinType JoinType = GetSimpleJoinType(pNewPath, &(pInsertedNode->InkPath));
	if (JoinType == JOINTYPE_NONE)
	{
		ERROR3("No join type in OpDrawBrush::SimpleJoinBrush");
		return TRUE;
	}

	// we need to know the distance along the original path where we want to insert the
	// new points.  As this is a simple join it will either be at the beginning or the end.
	// we also need to know the distance to the end of the new points, as this is required
	// for the undo.
	MILLIPOINT NewPathLength = (MILLIPOINT)pNewPath->GetPathLength();
	MILLIPOINT OldPathLength = -1;
	if (StartPath != NULL)
	{
		OldPathLength = (MILLIPOINT)StartPath->InkPath.GetPathLength();	
	}
	else 
	{
		if (EndPath != NULL)
			OldPathLength = (MILLIPOINT)EndPath->InkPath.GetPathLength();
	}

	MILLIPOINT InsertDistance = -1;
	MILLIPOINT EndInsertDistance = -1;
	INT32 StartIndex = 0; //where we will begin our insert
	
	// according to the type of join we will want to insert our new data at the beginning or the
	// end, we may also want to reverse it
	switch (JoinType)
	{
		case JOINTYPE_NEWSTART_TO_OLDSTART:
			InsertDistance = 0;
			EndInsertDistance = NewPathLength;
			StartIndex = 0; 
			// in this instance we reversed the new path section so also reverse the pressure data
			m_pPressureSampler->ReverseData();
			break;
		case JOINTYPE_NEWSTART_TO_OLDEND:
			InsertDistance = OldPathLength;
			EndInsertDistance = OldPathLength + NewPathLength;
			StartIndex = pPressData->GetInternalIndexFromDistance(OldPathLength);
			if (StartIndex==-1) StartIndex = pPressData->GetNumItems();
			break;
		case JOINTYPE_NEWEND_TO_OLDSTART:  
			InsertDistance = 0;
			EndInsertDistance = NewPathLength;
			StartIndex = 0; 
			break;
		case JOINTYPE_NEWEND_TO_OLDEND:
			InsertDistance = OldPathLength;
			EndInsertDistance = OldPathLength + NewPathLength;
			StartIndex = pPressData->GetInternalIndexFromDistance(OldPathLength);	
			if (StartIndex==-1) StartIndex = pPressData->GetNumItems();
			m_pPressureSampler->ReverseData();
			break;
		default:
			ERROR3("Unknown join type in OpDrawBrush::SimpleJoinBrush");
			return FALSE;
	}

	// make the action to insert the pressure list
	if (m_pPressureSampler != NULL)
	{
		AddPressurePointsAction* pAction;
		UINT32 NumPoints = m_pPressureSampler->GetNumItems();

		if (AddPressurePointsAction::Init(this, &UndoActions, pAttrBrush, m_pPressureSampler, 
										StartIndex, NumPoints, &pAction) == AC_FAIL)
			return FALSE;
		m_pPressureSampler = NULL;  // so we don't delete it 
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL OpDrawBrush::RetroSmoothBrush(NodePath* pNewNodePath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/2000
	Inputs:		pNewPath - The New curve that has just been drawn
			
	Returns:	TRUE 
	Purpose:	As above, the base class version does nothing

********************************************************************************************/

BOOL OpDrawBrush::RetroSmoothBrush(NodePath* pNewNodePath)
{
	return ApplyRetroSmoother(pNewNodePath, 85);
}


/********************************************************************************************

>	BOOL OpDrawBrush::ReverseBrushPressure()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/2000
	Inputs:	
	Returns:	TRUE if it worked, FALSE if it failed, or if we aren't using pressure
	Purpose:	Calls the attribute function to reverse our pressure list

********************************************************************************************/

BOOL OpDrawBrush::ReverseBrushPressure()
{
	if (m_pPressureSampler == NULL)
		return FALSE;

	return m_pPressureSampler->ReverseData();
}


/********************************************************************************************

>	CDistanceSampler* OpDrawBrush::GeneratePressureList(AttrBrushType* pAttrBrush, MILLIPOINT Start,
													MILLIPOINT End, MILLIPOINT NewPathLength)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/2000
	Inputs:		pAttrBrush - the brush that we are joining to 
				Start	   - the distance along the original path where we want to start the insert, 
							 set this to -1 to indicate we are inserting at the beginning of the path

				End		   - the distance along the original path where we wish to end the insert, 
							set this to -1 to indicate that we are inserting at the end of the path

				NewPathLength - the length of the path that we wish to insert
			
	Returns:	a newly generated pressure sampler, complete with values (or NULL if we fail)
	Purpose:	This function deals with the case when we wish to edit a brush with pressure data
				but we do not generate a pressure list during the edit for some reason.
				In this case we must create a new pressure list and fill it with values based upon
				the values in the existing brush pressure list

********************************************************************************************/

CDistanceSampler* OpDrawBrush::GeneratePressureData(AttrBrushType* pAttrBrush, MILLIPOINT Start,
													 MILLIPOINT End, MILLIPOINT NewPathLength)
{
	if (pAttrBrush == NULL)
	{
		ERROR3("Entry pointer is NULL in OpDrawBrush::GeneratePressureList");
		return NULL;
	}
	if (NewPathLength < 0 )
	{
		ERROR3("Negative path length in OpDrawBrush::GeneratePressureList");
		return NULL;
	}
	
	// just make sure we actually have a pressure list
	CDistanceSampler* pSampler = pAttrBrush->GetPressureCache();
	if (pSampler == NULL)
	{
		ERROR3("Why are you trying to do a pressure edit when this brush doesn't use pressure!??");
		return NULL;
	}

	/* there are two possibilities here: 
	1) we are inserting in the middle, in which case get the two insert values and interpolate, or
	2) we are inserting at the beginning or end, in which case just repeat the first/last value
	*/
	// get the pressure values at StartIndex and EndIndex
	INT32 StartIndex = -1;
	INT32 EndIndex   = -1;
	
	// case 1
	if (Start != -1 && End != -1)
	{
		StartIndex = pSampler->GetInternalIndexFromDistance(Start);
		EndIndex = pSampler->GetInternalIndexFromDistance(End);
	}

	// case 2
	else if (Start == -1)
			StartIndex = EndIndex = 0;
	else if (End == -1)
			StartIndex = EndIndex = pSampler->GetInternalIndexFromDistance(Start);

	// just make sure we got valid indexes
	if (StartIndex == -1 || EndIndex == -1)
	{
		ERROR3("Start or end index off the end of the array in OpDrawBrush::GeneratePressureData");
		return NULL;
	}
		
	// we are using internal indexes so set the sample rate to 1.0
	pSampler->SetRetrievalSampleRate(1.0);

	// get the starting pressure, make it a double as we're going to be doing lots of fp ops.
	CSampleItem StartItem;
	BOOL ok = pSampler->GetAt(StartIndex, &StartItem);
	double StartNewValue = (double)StartItem.m_Pressure; 

	// same for the end 
	CSampleItem EndItem; 
	if (ok) ok = pSampler->GetAt(EndIndex, &EndItem);
	double EndNewValue	 = (double)EndItem.m_Pressure;
	
	// now we have our start and end values, we also need to know how many items to make
	// Given that we sample at the rate of MIN_BRUSH_SPACING this is fairly straightforward
	UINT32 NumObjects = NewPathLength / MIN_BRUSH_SPACING;

	// find out by how much we must increase/decrease by each time
	double Incr = (EndNewValue - StartNewValue)/NumObjects;

	// lets make our new sampler here
	CDistanceSampler* pNewSampler = new CDistanceSampler;
	if (pNewSampler == NULL)
		return NULL;

	if (!pNewSampler->InitialiseData(NumObjects))
	{
		delete pNewSampler;
		return NULL;
	}

	// now just loop through, adding items to the sampler
	CSampleItem NewItem;
	NewItem.m_Pressure = (UINT32)StartNewValue;
	ok = pNewSampler->SetAt(0, NewItem);
	while (NumObjects > 0 && ok)
	{
		NewItem.m_Pressure += (UINT32)Incr;
		ok = pNewSampler->SetNext(NewItem);
		NumObjects--;
	}
	return pNewSampler;
}

/********************************************************************************************

>	BOOL OpDrawBrush::Declare()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpDrawBrush::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_FREE_HAND_TOOL),
								CC_RUNTIME_CLASS(OpDrawBrush), 
								OPTOKEN_DRAWBRUSH,
								OpDrawBrush::GetState,
								0,	/* help ID */
								_R(IDBBL_FREEHANDTOOLOP),
								0	/* bitmap ID */));
}


/********************************************************************************************

>	OpState OpDrawBrush::GetState(String_256* Description, OpDescriptor*)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpDrawBrush::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}



/********************************************************************************************

>	void OpDrawBrush::AddPointsToPath(DocCoord Pos, Spread* pSpread)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		Pos - The position of the mouse
				pSpread - Pointer to the spread that we are drawing on
	Purpose:	Adds a new point to the path of mouse moves. When the drag
				finishes the mouse moves will be smoothed

********************************************************************************************/

void OpDrawBrush::AddPointsToPath(DocCoord PointerPos, Spread* pSpread)
{
	// If this coord is the same as the last one, then do not bother adding to the track data
	if (PreviousPoint == PointerPos)
		return;
 
	// Insert the move
	if (TrackData->InsertLineTo(PointerPos))
	{
		// and add pressure info if needed
		if (AddPressureToPath)
		{
	 		TrackData->AddExtraInfo(CI_PRESSURE, FreeHandPressure);
		}
	}
	else
	{
		// Oh no, we ran out of mem in the middle of making a new path
		// Tidy up and report back to HQ
		EndDrag();

		// Tell the World that it all went horribly wrong
		InformError();

		// Remove it from the tree and delete it
		TrackData->ClearPath();
		
		// End this operation and return
		FailAndExecute();
		End();
		return;
	}
	
//	RenderLine(m_pGRenderBrush, TrackData, TrackData->GetNumCoords()-1);

	// Make a note of the coord, inc the line count and mark joining to the start as active.
	PreviousPoint = PointerPos;
	LineSegmentCount++;
	CanLineJoin = TRUE;
}



/********************************************************************************************

>	void OpDrawBrush::SetCursorAndStatus(CursorType CurType)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/7/2000
	Inputs:		CurType - The type of cursor to change to
	Purpose:	Overridden version for the brush, basically we do not offer any of the keyboard
				options so we wish to keep the status line blank during brush operations
********************************************************************************************/

void OpDrawBrush::SetCursorAndStatus(CursorType CurType)
{
// Now, if FlipCursor = TRUE, we flip it!
	static Cursor* WhichCursor;
	WhichCursor = NULL;

	switch (CurType)
	{
		case NORMAL_CURSOR:
			WhichCursor = pFreeHandCursor;
		break;

		case JOIN_CURSOR:
			WhichCursor = pJoinCursor;
		break;

		case MODIFY_CURSOR:
			WhichCursor = pModifyCursor;
		break;

		default:
			break;
	}

	if (mustClearStatusBar)	// got to execute the following at least once so that we clear the status bar ....
	{
		String_256 StatusMsg("");
		// Change Status bar message and the cursor
		pApp->UpdateStatusBarText(&StatusMsg);
		mustClearStatusBar = FALSE;
	}

	if (WhichCursor != MyCurrentCursor)
	{
		String_256 StatusMsg(_T(""));

		switch (CurType)
		{
			case NORMAL_CURSOR:
			break;

			case JOIN_CURSOR:
				StatusMsg.Load(_R(IDS_FREEHANDDRAGJOIN));
			break;

			case MODIFY_CURSOR:
				StatusMsg.Load(_R(IDS_FREEHANDMODIFY));
			break;

			default:
				break;

		}
		
		// Change Status bar message and the cursor
		pApp->UpdateStatusBarText(&StatusMsg);
		
		// set this cursor as the current cursor and immediately display it
		CursorStack::GSetTop(WhichCursor, CurrentCursorID);

		// remember this is our current cursor
		MyCurrentCursor = WhichCursor;
	}
}

/********************************************************************************************

>	BOOL OpDrawBrush::InitialiseProcessPathDistance()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/99
	Inputs:		-
	Purpose:	To create a new path processor object
	SeeAlso:	-

********************************************************************************************/

BOOL OpDrawBrush::InitialiseProcessPathDistance()
{
	if (m_pProcPathDistance != NULL)
	{
		delete m_pProcPathDistance;
		m_pProcPathDistance = NULL;
	}

	m_pProcPathDistance = new ProcessPathDistance(750/2);
	if (m_pProcPathDistance == NULL)
	{
		InformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
		return FALSE;
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL OpDrawBrush::InitialisePathProcessorBrush(BrushHandle Index, MILLIPOINT Distance = 0)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/99
	Inputs:		BrushIndex - index into the array of brush definitions stored in the brush component
				Distance - the distance along the path that we are starting at
	Retruns:    TRUE if successful, FALSE otherwise
	Purpose:	To create a new pathprocessorbrush and copy all the data from the brush definitino
				found using BrushIndex
	SeeAlso:	-

********************************************************************************************/

BOOL OpDrawBrush::InitialisePathProcessorBrush(BrushHandle Handle, MILLIPOINT Distance)
{
	// find the brush definition corresponding to the Handle
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, FALSE, "er, wheres the document gone");
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent((CC_RUNTIME_CLASS(BrushComponent)));
	ERROR2IF(pBrushComp == NULL, FALSE, "No brush component");
	BrushDefinition* pBrushDef = pBrushComp->FindBrushDefinition(Handle);
	ERROR2IF(pBrushDef == NULL, FALSE, "Couldn't find brush definition");

	// ask the definition to prepare for rendering
	if (!pBrushDef->StartRender())
		return FALSE;
	
	// check we haven't already allocated this member
	if (m_pPathProcBrush != NULL)
	{
		delete m_pPathProcBrush;
		m_pPathProcBrush = NULL;
	}
	// allocate the new path processor
	m_pPathProcBrush = new PathProcessorBrush;
	if (m_pPathProcBrush == NULL)
	{
		InformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
		return FALSE;
	}

	// everything is ok so far, so transfer the data
	m_pPathProcBrush->SetBrushDefinition(Handle);
	pBrushDef->CopyDataToProcessor(m_pPathProcBrush);
	
PORTNOTE("other", "Removed m_pGRenderBrush support")
#ifndef EXCLUDE_FROM_XARALX
	// if our screen depth is < 24 BPP then we need to ensure that the scaling and offset
	// variations are turned off.
	if (m_pGRenderBrush->GetScreenDepth() < 24)
	{
		m_pPathProcBrush->SetBrushScalingIncr(1);
		m_pPathProcBrush->SetBrushScalingIncrConst(0);
	//	m_pPathProcBrush->SetScalingMaxPressure(0);
		m_pPathProcBrush->SetPathOffsetIncrConst(0);
		m_pPathProcBrush->SetPathOffsetIncrProp(1);
	//	m_pPathProcBrush->SetScalingMaxRand(0);
	}
#endif

  // if we're not using pressure then turn it off
	if (!AddPressureToPath)
	{
		m_pPathProcBrush->SetScalingMaxPressure(0);
		m_pPathProcBrush->SetTransparencyPressure(0);
	}

	// do we need to scale to line width
	MILLIPOINT LineWidth = GetCurrentLineWidthIfNotDefault();
	if (LineWidth != -1)
	{
		if (LineWidth < 501)  // make sure we don't go too small
			LineWidth = 501;
		m_pPathProcBrush->ScaleToValue(LineWidth, !AddPressureToPath);
	}
	
	m_BrushScaling = m_pPathProcBrush->GetBrushScaling();
	
	m_pPathProcBrush->Reset();
	m_pPathProcBrush->SetCalculateBoundingBox(TRUE); // we want the bounding box

	// set up the arrays to hold cached transformation data
	if (!m_pPathProcBrush->PrepareForRenderingLoop(m_pPathProcBrush->GetNumBrushObjects()))
	{
		delete m_pPathProcBrush;
		return FALSE;
	}


	// if we are starting at some point along the path then advance our data to what it should be
	if (Distance)
		m_pPathProcBrush->AdvanceBrushToDistance(Distance);

	m_LastInkObjectRendered = m_pPathProcBrush->GetLastObject();

	// finally if this brush does not require pressure then turn it off
	if (!pBrushDef->UsesPressure())
		AddPressureToPath = FALSE;

//#ifdef NEWFASTBRUSHES	
	m_pBrushDefinition = m_pPathProcBrush->GetOurBrushDefinition ();	// store this off here
	// old RenderBrushAtPoint would grab this for each iteration!
//#endif

	return TRUE;
}


/********************************************************************************************

>	BOOL OpDrawBrush::InitialisePressureCache()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/99
	Inputs:		-
	Purpose:	To allocate the pressure array, if we need it
	SeeAlso:	-

********************************************************************************************/

BOOL OpDrawBrush::InitialisePressureCache()
{
	// do we actually need to do this?
	if (!AddPressureToPath)
		return TRUE; 
	/*
	m_pPressureVals = new PressureArray;
	if (m_pPressureVals == NULL)
		return FALSE;

	// make it pretty big so we don't have to keep reallocating
	m_pPressureVals->SetSize(200, 10);
	*/

	m_pPressureList = new PressureList;
	if (m_pPressureList == NULL)
		return FALSE;

	return TRUE;
}

/********************************************************************************************

>	BOOL OpDrawBrush::InitialiseTimeStampList()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/99
	Inputs:		-
	Purpose:	to create a new timestamp object list and assign it to our member
	SeeAlso:	-

********************************************************************************************/

BOOL OpDrawBrush::InitialiseTimeStampList()
{
	// if we've already got one then delete it
	if (m_pTimeStampList != NULL)
	{
		delete m_pTimeStampList;
		m_pTimeStampList = NULL;
	}
	m_pTimeStampList = new TimeStampList;
	if (m_pTimeStampList == NULL)
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL OpDrawBrush::SetTimeStamp(INT32 TimeStamp)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/99
	Inputs:		Value of timestamp to use in ms.
	Returns:    TRUE unless Timestamp is outside the specified bounds
	Purpose:	to set the timestamp member
	SeeAlso:	-

********************************************************************************************/

BOOL OpDrawBrush::SetTimeStamp(double TimeStamp)
{
	if (TimeStamp < MIN_TIMESTAMP || TimeStamp > MAX_TIMESTAMP)
		return FALSE;

	m_TimeStamp = TimeStamp;
	return TRUE;
}


/********************************************************************************************

>	BOOL OpDrawBrush::InitialisePressureSampler()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/99
	Inputs:		-
	Returns:	TRUE if successful, FALSE if our allocation failed.
	Purpose:	to create a new CSampleData object and assign it to our pointer for sampling pressure
	SeeAlso:	-

********************************************************************************************/

BOOL OpDrawBrush::InitialisePressureSampler()
{
	if (m_pPressureSampler != NULL)
	{
		delete m_pPressureSampler;
		m_pPressureSampler = NULL;
	}
	if (!AddPressureToPath)
		return TRUE;

	m_pPressureSampler = new CDistanceSampler;

	if (m_pPressureSampler == NULL)
		return FALSE;
	
	// set the max pressure value from our device
	CCPen *pPen = pApp->GetPressurePen();
	UINT32 MaxPressure = MAXPRESSURE;
	if (pPen != NULL)
		MaxPressure = pPen->GetPressureMax();
	
	m_pPressureSampler->SetMaxPressure(MaxPressure);

	return m_pPressureSampler->InitialiseData(5000);

}

/********************************************************************************************

>	MILLIPOINT OpDrawBrush::GetCurrentLineWidthIfNotDefault()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/2000
	Inputs:		-
	Outputs:	-
	Returns:	The current line width, UNLESS the current line width is the Default attribute,
				in which case it returns -1
	Purpose:	In order to determine whether we have to tell the brush to scale to a new line
				width or not.

********************************************************************************************/

MILLIPOINT OpDrawBrush::GetCurrentLineWidthIfNotDefault()
{
	MILLIPOINT Retval = -1;

	Document* pDoc = Document::GetCurrent();
	if (pDoc == NULL)
	{
		ERROR3("No document");
		return Retval;
	}

	AttributeManager* pAttrMgr = &(pDoc->GetAttributeMgr());
	if (pAttrMgr == NULL)
	{
		ERROR3("No attribute manager");
		return Retval;
	}

	AttrLineWidth* pAttr = (AttrLineWidth*)(pAttrMgr->GetCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), CC_RUNTIME_CLASS(AttrLineWidth)));

	if (pAttr != NULL)
	{
		AttrLineWidth* pDefault = (AttrLineWidth*)(AttributeManager::GetDefaultAttribute(ATTR_LINEWIDTH));
		if (pDefault == NULL)
		{
			ERROR3("Unable to get default line width, theres no way this can happen Jim!");
			return Retval;
		}
		MILLIPOINT CurrentLineWidth = pAttr->Value.LineWidth;
		MILLIPOINT DefaultLineWidth = pDefault->Value.LineWidth;

		if (CurrentLineWidth != DefaultLineWidth -1) // for some reason the default is 501
			Retval = CurrentLineWidth;
		
		// it turns out that GetDefault returns a copy of the default, so we must delete it here
		delete pDefault;
	}
	return Retval;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//  OpChangeBrush  - to change the features of the brush
/////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************

>	OpChangeBrush::OpChangeBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	-
	Purpose:	Constructor

********************************************************************************************/

OpChangeBrush::OpChangeBrush()
{
	 ChangeType = CHANGEBRUSH_NONE; 
}



/********************************************************************************************

>	BOOL OpChangeBrush::Declare()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpChangeBrush::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								0,
								CC_RUNTIME_CLASS(OpChangeBrush), 
								OPTOKEN_CHANGEBRUSH,
								OpChangeBrush::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}



/********************************************************************************************

>	static OpState OpChangeBrush::GetState(String_256* Description, OpDescriptor*)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpChangeBrush::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE); // It's not ticked, but it is greyed by default
	
	/*
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeBrush));

	if (!NodeList.IsEmpty())
	{
		State.Greyed = FALSE;
	}

	NodeList.DeleteAll();	
	*/
	// find out if there are any brushed nodes in the selection
	SelRange *pSel = GetApplication ()->FindSelection();
	if (pSel != NULL)
	{
		Node *pNode = pSel->FindFirst();
		while (pNode != NULL)
		{
			if (pNode->IsBrushed())
				State.Greyed = FALSE;
			pNode = pSel->FindNext(pNode);
		}
	}
	return State;

}

/********************************************************************************************

>	virtual void OpChangeBrush::GetOpName(String_256* OpName) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpChangeBrush::GetOpName(String_256* OpName) 
{ 
	UINT32 IDS = _R(IDS_MARKN_EMPTY);

	switch (ChangeType)
	{
		case CHANGEBRUSH_NONE:			  	break;
		case CHANGEBRUSH_SPACING:			IDS = _R(IDS_BRUSHSPACING_UNDO); 	break;
		case CHANGEBRUSH_SPACING_INCRPROP:  IDS = _R(IDS_BRUSHSPACING_INCR);	break;
		case CHANGEBRUSH_SPACING_INCRCONST:	IDS = _R(IDS_BRUSHSPACING_INCR);	break;
		case CHANGEBRUSH_SPACING_MAXRAND:	IDS = _R(IDS_BRUSHSPACING_MAXRAND); break;
		case CHANGEBRUSH_SPACING_RANDSEED:	IDS = _R(IDS_BRUSHSPACING_RANDSEED); break;
		case CHANGEBRUSH_SCALING:			IDS = _R(IDS_BRUSHSCALING_UNDO);	break;
		case CHANGEBRUSH_SCALING_INCR:		IDS = _R(IDS_BRUSHSCALING_INCR);	break;
		case CHANGEBRUSH_SCALING_INCRCONST:	IDS = _R(IDS_BRUSHSCALING_INCR);	break;
		case CHANGEBRUSH_SCALING_MAXRAND:	IDS = _R(IDS_BRUSHSCALING_MAXRAND); break;
		case CHANGEBRUSH_SCALING_RANDSEED:	IDS = _R(IDS_BRUSHSCALING_RANDSEED); break;
		case CHANGEBRUSH_SCALING_PRESSURE:	IDS = _R(IDS_BRUSHSCALING_PRESSURE); break;
		case CHANGEBRUSH_OFFSET_TYPE:		IDS = _R(IDS_BRUSHOFFSET_TYPE_UNDO);break;
		case CHANGEBRUSH_OFFSET_TYPE_SEED:	IDS = _R(IDS_OFFSETTYPE_RANDSEED);	break;
		case CHANGEBRUSH_OFFSET_VAL:		IDS = _R(IDS_BRUSHOFFSET_VAL_UNDO);	break;
		case CHANGEBRUSH_OFFSET_VALSEED:	IDS = _R(IDS_OFFSETVAL_RANDSEED);	break;
		case CHANGEBRUSH_OFFSET_SEEDS:		IDS = _R(IDS_OFFSETVAL_RANDSEED);	break;
		case CHANGEBRUSH_OFFSET_INCRPROP:	IDS = _R(IDS_BRUSHOFFSET_INCR);		break;
		case CHANGEBRUSH_OFFSET_INCRCONST:	IDS = _R(IDS_BRUSHOFFSET_INCR);		break;
		case CHANGEBRUSH_OFFSET_MAXRAND:	IDS = _R(IDS_OFFSETVAL_MAXRAND);	break;
		case CHANGEBRUSH_TILED:				IDS = _R(IDS_BRUSHTILE_UNDO);		break;
		case CHANGEBRUSH_TANGENTIAL:		IDS = _R(IDS_BRUSHROTATE_UNDO);		break;
		case CHANGEBRUSH_ROTATE_ANGLE:		IDS = _R(IDS_BRUSHROTATE_ANGLE_UNDO); break;
		case CHANGEBRUSH_ROTATE_INCRPROP:	IDS = _R(IDS_BRUSHROTATE_ANGLE_UNDO); break;
		case CHANGEBRUSH_ROTATE_INCRCONST:	IDS = _R(IDS_BRUSHROTATE_INCR);		 break;
		case CHANGEBRUSH_ROTATE_PRESSURE:	IDS = _R(IDS_BRUSHROTATE_PRESSURE);  break;
		case CHANGEBRUSH_NAME:				IDS = _R(IDS_BRUSHNAME_UNDO);		break;
		case CHANGEBRUSH_SEQUENCE:			IDS = _R(IDS_BRUSHNAME_UNDO);		break;
		case CHANGEBRUSH_SEQUENCE_RANDSEED:	IDS = _R(IDS_SEQUENCE_RANDSEED);	break;
		case CHANGEBRUSH_USELOCALFILLCOL:	IDS = _R(IDS_BRUSH_USELOCAL_FILLCOL);	break;
		case CHANGEBRUSH_USELOCALTRANSP:	IDS = _R(IDS_BRUSH_USELOCAL_TRANSP);	break;
		case CHANGEBRUSH_USENAMEDCOL:		IDS = _R(IDS_BRUSH_USELOCAL_FILLCOL);	break;
		case CHANGEBRUSH_ALL:				IDS = _R(IDS_BRUSH_CHANGEALL);		break;
		case CHANGEBRUSH_ROTATE_MAXRAND:	IDS = _R(IDS_ROTATE_MAXRAND);		break;
		case CHANGEBRUSH_ROTATE_RANDSEED:	IDS = _R(IDS_ROTATE_RANDSEED);		break;
		case CHANGEBRUSH_HUE_INCR:			IDS = _R(IDS_ROTATE_RANDSEED);		break;
		case CHANGEBRUSH_HUE_MAXRAND:		IDS = _R(IDS_BRUSH_HUE_RANDOM);		break;
		case CHANGEBRUSH_FILL_SEEDS:		IDS = _R(IDS_BRUSH_HUE_RANDSEED);	break;
		case CHANGEBRUSH_SAT_MAXRAND:		IDS = _R(IDS_BRUSH_SAT_RANDOM);		break;
		case CHANGEBRUSH_SAT_RANDSEED:		IDS = _R(IDS_BRUSH_SAT_RANDSEED);	break;
		case CHANGEBRUSH_TRANSP:			IDS = _R(IDS_BRUSHTRANSP);			break;
		case CHANGEBRUSH_TRANSP_PRESSURE:	IDS = _R(IDS_BRUSHTRANSP_PRESSURE); break;
		case CHANGEBRUSH_REGEN:				break;
		
		default: ERROR3_PF(("Unknown flag type (%d)",ChangeType));  break;
	}

	*OpName = String_256(IDS);
}  


/********************************************************************************************

>	void OpChangeBrush::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Returns:	-
	Purpose:	This changes the flag specified in FlagType (given in pOpParam->Param1) in all the 
				selected brush objects to have the same as state of pOpParam->Param2.

********************************************************************************************/

void OpChangeBrush::DoWithParam(OpDescriptor*,OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL,"NULL OpParam ptr");
	if (pOpParam == NULL) return;

	ChangeBrushOpParam* pChangeParam = (ChangeBrushOpParam*)pOpParam;

	// find all the brushes in the current selection
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeRenderableInk));

	BOOL ok = !NodeList.IsEmpty();
	if (ok) ok = DoStartSelOp(FALSE,FALSE);

	NodeListItem * pItem = NULL;
	
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread == NULL)
	{
		ERROR3("Wheres the spread!?");
		return;
	}
	
	if (ok)
	{
		
		pItem = (NodeListItem *)NodeList.GetHead();

		Node* pSelNode = NULL;

		if (pItem)
		{
			pSelNode = pItem->pNode;
		}
		
		while (pSelNode != NULL && ok)
		{
			Node* pNode = pSelNode;

			pItem = (NodeListItem *)NodeList.GetNext(pItem);

			if (pItem)
			{
				pSelNode = pItem->pNode;
			}
			else
			{
				pSelNode = NULL;
			}
			
			AttrBrushType* pAttrBrush = ((NodeRenderableInk*)pNode)->GetAppliedBrushAttribute();
			if (pAttrBrush != NULL && pAttrBrush->GetBrushHandle() != BrushHandle_NoBrush)
			{
				// We now have a selected node with a brush attribute so:
				//	Invalidate the node's region
				//	Store the current state of brush flag in an undo actiom
				// 	Change the flag to the setting in Param2

				ChangeBrushAction* pAction;
	
				
				// Ask the node if it's ok to do the op
				ObjChangeFlags cFlags;
				ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
				ok = pNode->AllowOp(&ObjChange, TRUE);
			//	TRACEUSER( "Diccon", _T("CHANGE BRUSH ACTION\n"));
				DocRect PreActRect =((NodeRenderableBounded*)pNode)->GetBoundingRect();
				// invalidate the brush's region
				if (ok) ok = DoInvalidateNodeRegion((NodeRenderableBounded*)pNode,TRUE,FALSE);
			//	if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,(NodeRenderableBounded*)pNode,TRUE) != AC_FAIL);

				// invalidate it before
				if (ok) ok = DoInvalidateRegion(pSpread, PreActRect);
				
				INT32 OldSize = pAttrBrush->GetDefaultLineWidth();

				// change the brush in an undoable way
				if (ok) ok = ChangeBrushAction::Init(this,&UndoActions,(NodeRenderableInk*)pNode,
													pChangeParam,&pAction) != AC_FAIL;
				
				// have we changed the size of the brush? Depends on what our param says
				INT32 NewSize = pAttrBrush->GetDefaultLineWidth();
				
				if (pChangeParam->ChangeLineWidth())
				{
					double SizeChange = (double)NewSize / (double)OldSize;
					ok = ChangeLineWidth(SizeChange, (NodeRenderableInk*)pNode);
				}
				
				DocRect PostActRect = ((NodeRenderableBounded*)pNode)->GetBoundingRect();
			
				if (ok) ok = DoInvalidateNodeRegion((NodeRenderableBounded*)pNode,TRUE,FALSE);
			
				// invalidate it after
				if (ok) ok = DoInvalidateRegion(pSpread, PostActRect);
				
			}
			
		}
	}
	NodeList.DeleteAll();
	

	if (ok) 
	{
		// Inform the effected parents of the change
		ObjChangeFlags cFlags;
		ObjChangeParam ObjChange(OBJCHANGE_FINISHED,cFlags,NULL,this);
		UpdateChangedNodes(&ObjChange);

		// Note the way the selected brush were changed
		ChangeType = pChangeParam->ChangeType;
	}
	else
		FailAndExecute();

	End();
}



/********************************************************************************************

>	BOOL OpChangeBrush::ChangeLineWidth(const double &PropChange, NodeRenderableInk* pInk)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		PropChange - the proportion to change the line width by (must be > 0)
				pInk - the node whose line width we want to change
	Returns:	-
	Purpose:	To change the linewidth applied to pInk by the specified proportion

	Notes:		What we must do here is get in a situation where we have a line width attribut
				that applies only to this ink node,as we don't want to be changing other peoples
				line widths.  Therefore if pInk doesn't have a line width child we will make one for it

********************************************************************************************/

BOOL OpChangeBrush::ChangeLineWidth(const double &PropChange, NodeRenderableInk* pInk)
{
	ERROR2IF(PropChange <= 0, FALSE, "Negative value passed to OpChangeBrush::ChangeLineWidth");
	ERROR2IF(pInk == NULL, FALSE, "Null input pointer to OpChangeBrush::ChangeLineWidth");

	// first get our current line width
	AttrLineWidth* pLineWidth = NULL;
	
	// search for it underneath us, if we can't find it we'll make a new one
	Node* pNode = pInk->FindFirstChild();
	while (pNode != NULL)
	{
		if (pNode->IsAnAttribute() && ((NodeAttribute*)pNode)->IsALineWidthAttr())
		{
			pLineWidth = (AttrLineWidth*)pNode;
			break;
		}
		pNode = pNode->FindNext();
	}
	if (pLineWidth != NULL)
	{
		// Try to hide the Node    
		NodeHidden* Hidden; 
		ALLOC_WITH_FAIL(Hidden,(new NodeHidden(pLineWidth)),this);
		if (Hidden == NULL)
		{          
			return (FALSE); 
		}

		// Try to create a ShowNodeAction which will show the node that we have just hidden. 
		
		ShowNodeAction* UndoShowNodeAction; 

		if (ShowNodeAction::Init(this,  
								 &UndoActions, 
								 Hidden, 
								 TRUE, 
								 (Action**)(&UndoShowNodeAction),
								 TRUE) == AC_FAIL)
		{   
			// We must unhide the NodeToHide manually, cos no action will do it for us 
			Hidden->ShowNode(); // Hidden is deleted don't worry 
			return FALSE; 
		}
	}
	else
	{
		// there isn't a line width child, so we must be in a compound node or something.
		pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrLineWidth), (NodeAttribute**)&pLineWidth);

		if (pInk == NULL)
		{
			ERROR3("Unable to find line width node");
			return FALSE;
		}
	}

	// Make a new line width to insert
	AttrLineWidth* pNewLineWidth = new AttrLineWidth;
	if (pNewLineWidth == NULL)
	{
		ERROR1RAW(_R(IDE_NOMORE_MEMORY));
		return FALSE;
	}

	// insert the new attribute
	pNewLineWidth->AttachNode(pInk, FIRSTCHILD);

	// Create a hide node action to hide the node when we undo 
	HideNodeAction* UndoHideNodeAction;     
	if (HideNodeAction::Init(this, &UndoActions, pNewLineWidth,  TRUE, 	( Action**)(&UndoHideNodeAction))      
					  		  == AC_FAIL)
	{
		delete pNewLineWidth;
		return FALSE;
	}

	

	double OldWidth = (double)pLineWidth->Value.LineWidth;
	double NewWidth = OldWidth * PropChange;
	
	pNewLineWidth->Value.LineWidth = (MILLIPOINT)NewWidth;

	// change the control by abusing an op descriptor
	OpChangeLineWidthOpDesc* pDesc = (OpChangeLineWidthOpDesc*)OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGELINEWIDTH);
	if (pDesc)
	{
		String_256 StrWidth = TEXT("");
		Convert::DoubleToString(NewWidth*0.001, &StrWidth, 2);
		StrWidth += TEXT("pt");
		pDesc->SetLineWidthGadgets(StrWidth);
	}


	return TRUE;

}



//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeBrushAction class

/********************************************************************************************

>	ChangeBrushAction::ChangeBrushAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeBrushAction::ChangeBrushAction()
{
	m_pBrushedNode 	= NULL;
}


/********************************************************************************************

>	ChangeBrushAction::~ChangeBrushAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


ChangeBrushAction::~ChangeBrushAction()
{
}

/********************************************************************************************

>	ActionCode ChangeBrushAction::Init( 	Operation* 		pOp,
											ActionList* 	pActionList,
											NodeRenderableInk * pNodeBrush,
											ChangeBrushOpParam	*pChangeParam,
											ChangeBrushAction** ppNewAction);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pBrushedNode 	= ptr to node with brush attribute to change 
				pChangeParam	= ptr to class that details how the blend should be changed.
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

				This function actually changes the blend node in a way specified in pChangeParam
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode ChangeBrushAction::Init(	Operation* 			pOp,
									ActionList* 		pActionList,
									NodeRenderableInk* 	pBrushedNode,
									ChangeBrushOpParam*	pChangeParam,
									ChangeBrushAction** ppNewAction)
{
	ERROR2IF(pBrushedNode == NULL,AC_FAIL,"pNodeBrush is NULL");
	ERROR2IF(pChangeParam == NULL,AC_FAIL,"pChangeParam is NULL");

	// just check that we do actually have a brush attribute
	
/*	NodeAttribute* pAttr = NULL;
	pBrushedNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), &pAttr);
	ERROR2IF(pAttr == NULL, AC_FAIL, "No applied brush attribute");*/

	AttrBrushType* pAttrBrush = GetAttrBrush(pBrushedNode);
	if (pAttrBrush == NULL)
	{
		ERROR3("AttrBrushType is NULL in ChangeBrushAction::Init");
		return AC_FAIL;
	}
	if (pAttrBrush->GetBrushHandle() == BrushHandle_NoBrush)
	{
	//	ERROR3("Brush handle = BrushHandle_NoBrush in ChangeBrushAction::Init");
		return AC_OK;
	}

	// make the new action
	UINT32 ActSize = sizeof(ChangeBrushAction);
	ChangeBrushAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ChangeBrushAction),(Action**)&pNewAction);
	*ppNewAction = pNewAction;

	if (Ac != AC_FAIL && pNewAction != NULL)
	{
		ChangeBrushType ChangeType = pChangeParam->ChangeType;

		// set data in the new action
		pNewAction->m_pBrushedNode  		 = pBrushedNode;
		pNewAction->m_ChangeParam.ChangeType = ChangeType;
		pNewAction->m_ChangeParam.SetOldValues(pAttrBrush);
		
		if (!ChangeBrush(pChangeParam, pAttrBrush))
			Ac = AC_FAIL;

		pBrushedNode->InvalidateBoundingRect();
		pNewAction->m_ChangeParam.SetNewValues(pAttrBrush);
	} // end if (AC != ..

	return Ac;
}



/********************************************************************************************

>	ActionCode ChangeBrushAction::Execute();

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the brush variables to their old values
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeBrushAction::Execute()
{
	m_ChangeParam.SwapOldAndNew();

	ActionCode Act;
	ChangeBrushAction* pAction;
	TRACEUSER( "Diccon", _T("Undoing Change Brush Action\n"));
	Act = ChangeBrushAction::Init(pOperation,pOppositeActLst,m_pBrushedNode,&m_ChangeParam,&pAction);

	return Act;
}


/********************************************************************************************

>	ActionCode ChangeBrushAction::Execute();

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		pInk - the ink node that has the applied brush
	Outputs:	-
	Returns:	the applied brush attribute
	Purpose:	Returns the brush attribute that applies to this node.  However there is a little
				trickery needed to cope with the likes of shadow nodes etc. as the brushed node
				will be a sibling of that node.

	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

AttrBrushType* ChangeBrushAction::GetAttrBrush(NodeRenderableInk* pInk)
{
	NodeAttribute* pAttr = NULL;
	
	if (pInk->IS_KIND_OF(NodeShadow) || pInk->IS_KIND_OF(NodeBevel) || pInk->IS_KIND_OF(NodeContour))
	{
		// cycle through the siblings and see if one has an AttrBrushType as a child
		BOOL Found = FALSE;
		Node* pSibling = pInk->FindNext();
		while (pSibling != NULL && Found == FALSE)
		{
			Node* pChild = pSibling->FindFirstChild();
			while (pChild != NULL && !Found)
			{
				if (pChild->IS_KIND_OF(AttrBrushType))
				{
					pAttr = (NodeAttribute*)pChild;
					Found = TRUE;
					break;
				}
				pChild = pChild->FindNext();
			}
			pSibling = pSibling->FindNext();
		}
	}
	else
		pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), &pAttr);

	return (AttrBrushType*)pAttr;
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeBrush(ChangeBrushOpParam* pParam, AttrBrushType* pBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/2000
	Inputs:		pParam - object containing the data about what to change
				pBrush - the brush attribute to be changed
	Outputs:	-
	Returns:	TRUE if successful, ,FALSE otherwise
	Purpose:	Changes pBrush according to the data provided in pParam

	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

BOOL ChangeBrushAction::ChangeBrush(ChangeBrushOpParam* pChangeParam, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pChangeParam == NULL || pAttrBrush == NULL, FALSE, "Invalid inputs to ChangeBrushAction::ChangeBrush");

	ChangeBrushType ChangeType = pChangeParam->ChangeType;
	
	BOOL Retval = FALSE;

	switch (ChangeType)
	{
		case CHANGEBRUSH_SPACING:			
			Retval = ChangeBrushSpacing(pChangeParam->m_NewSpacing, 
									pChangeParam->m_pPressure,  pAttrBrush);
			
			break;
		case CHANGEBRUSH_SPACING_INCRPROP:
			Retval = ChangeSpacingIncrProp(pChangeParam->m_NewSpacingIncrProp, pChangeParam->m_pPressure, pAttrBrush);
			break;
		case CHANGEBRUSH_SPACING_INCRCONST:
			Retval = ChangeSpacingIncrConst(pChangeParam->m_NewSpacingIncrConst, pChangeParam->m_pPressure, pAttrBrush);
			break;
		case CHANGEBRUSH_SCALING_INCR:
			Retval = ChangeScalingIncr(pChangeParam->m_NewScalingIncr, pAttrBrush);
			break;
		case  CHANGEBRUSH_SCALING_INCRCONST:
			Retval = ChangeScalingIncrConst(pChangeParam->m_NewScalingIncrConst, pAttrBrush);
			break;
		case CHANGEBRUSH_SCALING:
			Retval = ChangeBrushScaling(pChangeParam->m_NewScaling, pAttrBrush);
			break;
		case CHANGEBRUSH_SCALING_MAXRAND:
			Retval = ChangeScalingMaxRand(pChangeParam->m_NewScalingMaxRand, pAttrBrush);
			break;
		case CHANGEBRUSH_SCALING_RANDSEED:
			Retval = ChangeScalingRandSeed(pChangeParam->m_NewScalingRandSeed, pAttrBrush);
			break;
		case CHANGEBRUSH_SCALING_PRESSURE:
			Retval = ChangeScalingPressure(pChangeParam->m_NewScalingMaxPressure, pAttrBrush);
			break;
		case CHANGEBRUSH_TILED:
			Retval = ChangeBrushTile(pChangeParam->m_bNewTiled, pAttrBrush);
			break;
		case CHANGEBRUSH_TANGENTIAL:
			Retval = ChangeTangential(pChangeParam->m_bNewRotated, pAttrBrush);
			break;
		case CHANGEBRUSH_OFFSET_TYPE:
			Retval = ChangeOffsetType(pChangeParam->m_NewPathOffsetType, pAttrBrush);
			break;
		case CHANGEBRUSH_OFFSET_TYPE_SEED:
			Retval = ChangeOffsetTypeSeed(pChangeParam->m_NewOffsetTypeRandSeed, pAttrBrush);
			break;
		case CHANGEBRUSH_OFFSET_SEEDS:
			Retval = ChangeOffsetSeeds(pChangeParam->m_NewOffsetTypeRandSeed, pChangeParam->m_NewOffsetValRandSeed,
									pAttrBrush);
			break;
		case CHANGEBRUSH_OFFSET_VAL:
			Retval = ChangeOffsetValue(pChangeParam->m_NewPathOffsetVal, pAttrBrush);
			break;
		case CHANGEBRUSH_OFFSET_INCRPROP:
			Retval = ChangeOffsetIncrProp(pChangeParam->m_NewOffsetIncrProp, pAttrBrush);
			break;
		case CHANGEBRUSH_OFFSET_INCRCONST:
			Retval = ChangeOffsetIncrConst(pChangeParam->m_NewOffsetIncrConst, pAttrBrush);
			break;
		case CHANGEBRUSH_OFFSET_MAXRAND:
			Retval = ChangeOffsetValMaxRand(pChangeParam->m_NewOffsetValMaxRand, pAttrBrush);
			break;
		case CHANGEBRUSH_OFFSET_VALSEED:
			Retval = ChangeOffsetValRandSeed(pChangeParam->m_NewOffsetValRandSeed, pAttrBrush);
			break;
		case CHANGEBRUSH_ROTATE_ANGLE:
			Retval = ChangeRotation(pChangeParam->m_NewRotateAngle, pAttrBrush);
			break;
		case CHANGEBRUSH_ROTATE_INCRPROP:
			Retval = ChangeRotationIncrProp(pChangeParam->m_NewRotationIncrProp, pAttrBrush);
			break;
		case CHANGEBRUSH_ROTATE_INCRCONST:
			Retval = ChangeRotationIncrConst(pChangeParam->m_NewRotationIncrConst, pAttrBrush);
			break;
		case CHANGEBRUSH_ROTATE_MAXRAND:
			Retval = ChangeRotationMaxRand(pChangeParam->m_NewRotateMaxRand, pAttrBrush);
			break;
		case CHANGEBRUSH_ROTATE_RANDSEED:
			Retval = ChangeRotationRandSeed(pChangeParam->m_NewRotateRandSeed, pAttrBrush);
			break;
		case CHANGEBRUSH_ROTATE_PRESSURE:
			Retval = ChangeRotationMaxPressure(pChangeParam->m_NewRotationMaxPressure, pAttrBrush);
			break;
		case CHANGEBRUSH_SPACING_MAXRAND:
			Retval = ChangeSpacingMaxRand(pChangeParam->m_NewSpacingMaxRand, pAttrBrush);
			break;
		case CHANGEBRUSH_SPACING_RANDSEED:
			Retval = ChangeSpacingRandSeed(pChangeParam->m_NewSpacingRandSeed, pAttrBrush);
			break;
		case CHANGEBRUSH_USELOCALFILLCOL:
			Retval = ChangeUseLocalFillColour(pChangeParam->m_bNewUseLocalFillColour, pAttrBrush);
			break;
		case CHANGEBRUSH_USELOCALTRANSP:
			Retval = ChangeUseLocalTransp(pChangeParam->m_bNewUseLocalTransp, pAttrBrush);
			break;
		case CHANGEBRUSH_USENAMEDCOL:
			Retval = ChangeUseNamedColour(pChangeParam->m_bNewUseNamed, pAttrBrush);
			break;
		case CHANGEBRUSH_SEQUENCE:
			Retval = ChangeSequenceType(pChangeParam->m_NewSequenceType, pAttrBrush);
			break;
		case CHANGEBRUSH_SEQUENCE_RANDSEED:
			Retval = ChangeSequenceSeed(pChangeParam->m_NewSequenceRandSeed, pAttrBrush);
			break;
		case CHANGEBRUSH_ALL:
			Retval = ChangeAll(pChangeParam, pAttrBrush);
			break;
		case CHANGEBRUSH_TIMESTAMP:
			break;
		case CHANGEBRUSH_HUE_INCR:
			Retval = ChangeHueIncrement(pChangeParam->m_NewHueIncrement, pAttrBrush);
			break;
		case CHANGEBRUSH_HUE_MAXRAND:
			Retval = ChangeHueMaxRand(pChangeParam->m_NewHueMaxRand, pAttrBrush);
			break;
		case CHANGEBRUSH_FILL_SEEDS:
			Retval = ChangeFillSeeds(pChangeParam->m_NewHueRandSeed, pChangeParam->m_NewSatRandSeed,
								pAttrBrush);
			break;
		case CHANGEBRUSH_SAT_MAXRAND:
			Retval = ChangeSatMaxRand(pChangeParam->m_NewSatMaxRand, pAttrBrush);
			break;
		case CHANGEBRUSH_SAT_RANDSEED:
			Retval = ChangeSatRandSeed(pChangeParam->m_NewSatRandSeed, pAttrBrush);
			break;
		case CHANGEBRUSH_TRANSP:
			Retval = ChangeTransparency(pChangeParam->m_NewTransp, pAttrBrush);
			break;
		case CHANGEBRUSH_TRANSP_PRESSURE:
			Retval = ChangeTranspPressure(pChangeParam->m_NewTranspPressure, pAttrBrush);
			break;

		default : ERROR2(FALSE,"Unknown change brush type"); break;
		
	} // end switch
	return Retval;
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeSelectionNoUndo(ChangeBrushOpParam* pParam)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/2000
	Inputs:		pParam - object containing the data about what to change
	Outputs:	-
	Returns:	TRUE if successful, ,FALSE otherwise
	Purpose:	In order to avoid generating undo information for each single slider movement
				in the brush edit dialog I am introducing this system where we only generate
				an undoable action on a slider_pos_set message.  For slider moving messages we
				call this method, which will any brushes in the selection but not generate any undo
				information.

				This is potentially a bit naughty as it means we bypass the AllowOp mechanism
				but its been given to me as a showstopper...

	Errors:		-
	SeeAlso:	Action::ChangeBrush()

********************************************************************************************/

BOOL ChangeBrushAction::ChangeSelectionNoUndo(ChangeBrushOpParam* pParam)
{
	PORTNOTETRACE("other","ChangeBrushAction::ChangeSelectionNoUndo - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(pParam == NULL, FALSE, "Invalid inputs to ChangeBrushAction::ChangeSelectionNoUndo");

	// find all the brushes in the current selection
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeRenderableInk));

	BOOL ok = !NodeList.IsEmpty();

	NodeListItem * pItem = NULL;
	
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread == NULL)
	{
		ERROR3("Wheres the spread!?");
		return FALSE;
	}
	DocView* pDocView = DocView::GetCurrent();
	if (pDocView == NULL)
	{
		ERROR3("No DocView!");
		return FALSE;
	}
	
	if (ok)
	{
		
		pItem = (NodeListItem *)NodeList.GetHead();

		Node* pSelNode = NULL;

		if (pItem)
		{
			pSelNode = pItem->pNode;
		}
		
		AttrBrushType* pAttrBrush = NULL;
		
		while (pSelNode != NULL && ok)
		{
			Node* pNode = pSelNode;

			pItem = (NodeListItem *)NodeList.GetNext(pItem);

			if (pItem)
			{
				pSelNode = pItem->pNode;
			}
			else
			{
				pSelNode = NULL;
			}
			
			pAttrBrush = GetAttrBrush((NodeRenderableInk*)pNode);
	
			if (pAttrBrush != NULL && pAttrBrush->GetBrushHandle() != BrushHandle_NoBrush)			
			{
				// invalidate it before
				DocRect PreActRect = ((NodeRenderableBounded*)pNode)->GetBoundingRect();
				if (pNode->IsAnObject()) ((NodeRenderableInk*)pNode)->ReleaseCached();
				if (ok) 
					pDocView->ForceRedraw(pSpread, PreActRect);

				// make sure we recalculate the rect
				((NodeRenderableBounded*)pNode)->InvalidateBoundingRect();

					
				INT32 OldSize = pAttrBrush->GetDefaultLineWidth();

				// change the brush in an non-undoable way
				if (ok) ok = ChangeBrush(pParam, pAttrBrush);
				

				// have we changed the size of the brush?  If so then we will have applied a new line width
				// in the action, so factor out
				INT32 NewSize = pAttrBrush->GetDefaultLineWidth();
				
				if (NewSize != OldSize && ok)
				{
					double SizeChange = (double)NewSize / (double)OldSize;
					ok = ChangeLineWidthChildNoUndo(SizeChange, (NodeRenderableInk*)pNode);
				}
				
			
				DocRect PostActRect = ((NodeRenderableBounded*)pNode)->GetBoundingRect();
			
				if (pNode->IsAnObject()) ((NodeRenderableInk*)pNode)->ReleaseCached();
				
				// invalidate it after
				if (ok)
					pDocView->ForceRedraw(pSpread, PostActRect);
				
			}
			
		}
	}
	NodeList.DeleteAll();
	
	return ok;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeLineWidthNoUndo(double ChangeProp, NodeRenderableInk* pInk)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		ChangeProp - the proportion to change the line width by
				pInk - the ink node to search under
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To change the line width of the child attribute if this ink node.  Note that 
				we only change child attributes because we don't want to mess up any other inknodes
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeLineWidthChildNoUndo(double ChangeProp, NodeRenderableInk* pInk)
{
	PORTNOTETRACE("other","ChangeBrushAction::ChangeLineWidthChildNoUndo - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(ChangeProp <= 0.0, FALSE, "Invalid change proportion in ChangeBrushAction::ChangeLineWidthChildNoUndo");
	ERROR2IF(pInk == NULL, FALSE, "Ink node is NULL in ChangeBrushAction::ChangeLineWidthChildNoUndo");

	// search for the line width attribute
	Node* pNode = pInk->FindFirstChild();
	AttrLineWidth* pLineWidth = NULL;
	
	while (pNode != NULL)
	{
		if (pNode->IsAnAttribute() && ((NodeAttribute*)pNode)->IsALineWidthAttr())
		{
			pLineWidth = (AttrLineWidth*)pNode;
			break;
		}
		pNode = pNode->FindNext();
	}

	if (pLineWidth == NULL)
		return FALSE;

	// get the old value, multiply by our proportion
	double OldWidth = (double)pLineWidth->Value.LineWidth;
	double NewWidth = OldWidth * ChangeProp;
	
	pLineWidth->Value.LineWidth = (MILLIPOINT)NewWidth;

	// change the line width gadgets by abusing an op descriptor
	OpChangeLineWidthOpDesc* pDesc = (OpChangeLineWidthOpDesc*)OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGELINEWIDTH);
	if (pDesc)
	{
		String_256 StrWidth = TEXT("");
		Convert::DoubleToString(NewWidth*0.001, &StrWidth, 2);
		StrWidth += TEXT("pt");
		pDesc->SetLineWidthGadgets(StrWidth);
	}

	return TRUE;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeBrushSpacing(MILLIPOINT NewSpacing, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSpacing - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush spacing of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeBrushSpacing(MILLIPOINT NewSpacing, PressureArray* pArray,
										   AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
//	if (pArray != NULL)
//		pAttrBrush->SetPressureCache(pArray);
	return pAttrBrush->SetSpacing(NewSpacing);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeSpacingIncrProp(double NewIncrProp, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSpacing - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush spacing of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeSpacingIncrProp(double NewIncr, PressureArray* pArray, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
//	if (pArray != NULL)
	//	pAttrBrush->SetPressureCache(pArray);

		// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	return pPPB->SetSpacingIncrProp(NewIncr);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeSpacingIncrConst(MILLIPOINT NewIncr, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSpacing - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush spacing of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeSpacingIncrConst(MILLIPOINT NewIncr, PressureArray* pArray, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	//if (pArray != NULL)
	//	pAttrBrush->SetPressureCache(pArray);

		// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	return pPPB->SetSpacingIncrConst(NewIncr);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeBrushScaling(double NewScaling, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewScaling - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush scaling of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeBrushScaling(double NewScaling, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	// In order to find out whether or not we need to tell the processor whether or not to
	// scale to line width we need to find the line width, get the scaling required to scale
	// to that value and compare it with the scaling that we want to do here.
	MILLIPOINT LineWidth = pAttrBrush->GetAppliedLineWidth();
	double Scaling = pPPB->GetScaleValue(LineWidth);

	if (Scaling != NewScaling)
		pPPB->SetScaleToLineWidth(FALSE);
	else
		pPPB->SetScaleToLineWidth(TRUE);

	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();


	return pAttrBrush->SetScaling(NewScaling);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeScalingIncr(double NewScaling, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewScaling - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush scaling of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeScalingIncr(double NewIncr, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");

	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();


	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	return pPPB->SetBrushScalingIncr(NewIncr);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeScalingIncrConst(double Incr, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		Incr - the increment to set	
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush scaling increment
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeScalingIncrConst(double Incr, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");

	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	return pPPB->SetBrushScalingIncrConst(Incr);
}

/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeScalingMaxRand(UINT32 NewMax, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewMax - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the amount of brush scaling determined by randomness
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeScalingMaxRand(UINT32 NewMax, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");

	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();


	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	return pPPB->SetScalingMaxRand(NewMax);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeScalingMaxRand(UINT32 NewMax, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewMax - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the amount of brush scaling determined by randomness
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeScalingPressure(UINT32 Pressure, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");

	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();


	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	
	
	return pPPB->SetScalingMaxPressure(Pressure);
}
/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeScalingRandSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeed - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets a new seed for the scaling RNG
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeScalingRandSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");

	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();


	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	return pPPB->SetScalingRandSeed(NewSeed);
}

/********************************************************************************************

>	BOOL ChangeBrushAction::Rotation(double NewAngle, AttrBrushType* pAttrBrush)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewAngle - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the brush scaling of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeRotation(double NewAngle, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");

	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();

	return pAttrBrush->SetRotateAngle(NewAngle);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeRotationIncrProp(double Incr, AttrBrushType* pAttrBrush)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		Incr - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the rotation multiplier for this brush
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeRotationIncrProp(double Incr, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();

	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	return pPPB->SetRotationIncrProp(Incr);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeRotationIncrConst(double Incr, AttrBrushType* pAttrBrush)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		Incr - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the rotation increment for this brush
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeRotationIncrConst(double Incr, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();

	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	return pPPB->SetRotationIncrConst(Incr);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeRotationMaxRand(UINT32 NewMax, AttrBrushType* pAttrBrush)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewMax - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the maximum random effect on the rotation of this brush
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeRotationMaxRand(UINT32 NewMax, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();

	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	return pPPB->SetRotationMaxRand(NewMax);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeRotationRandSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeed - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the seed for the rotation random number generator
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeRotationRandSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();

	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	pPPB->SetRotationRandSeed(NewSeed);

	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeRotationMaxPressure(UINT32 Pressure, AttrBrushType* pAttrBrush)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		Pressure - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the sensitivity that rotation will have to pressure
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeRotationMaxPressure(UINT32 Pressure, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();

	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	return pPPB->SetRotationMaxPressure(Pressure);
}

/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeTiled(BOOL bTile, AttrBrushType* pAttrBrush)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		bTile, to tile, or not	
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the tiling of this attribute node
	SeeAlso:	-

********************************************************************************************/
BOOL ChangeBrushAction::ChangeBrushTile(BOOL bTile, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	return pAttrBrush->SetTiled(bTile);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeTangential(BOOL bRotate, AttrBrushType* pAttrBrush)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		bTile, to tile, or not	
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the tiling of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeTangential(BOOL bRotate, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");

	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();

	
	return pAttrBrush->SetRotated(bRotate);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeOffsetValue(MILLIPOINT NewOfdfset, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewOffset - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the offset value of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeOffsetValue(MILLIPOINT NewOffset, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();
	
	return pAttrBrush->SetPathOffsetValue(NewOffset);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeOffsetIncrProp(double NewIncrProp, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewIncrProp - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the offset value of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeOffsetIncrProp(double NewIncrProp, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();

	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	return pPPB->SetPathOffsetIncrProp(NewIncrProp);
}



/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeOffsetIncrConst(MILLIPOINT NewIncr, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewIncrProp - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the offset value of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeOffsetIncrConst(MILLIPOINT NewIncrConst, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();

	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	return pPPB->SetPathOffsetIncrConst(NewIncrConst);
}



/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeOffsetValMaxRand(UINT32 NewMax, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewMax - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set how much the offset is determined randomly
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeOffsetValMaxRand(UINT32 NewMax, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();

	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	return pPPB->SetOffsetValueMaxRand(NewMax);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeOffsetValRandSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeed - the value to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the seed for the offset value RNG
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeOffsetValRandSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();

	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	return pPPB->SetOffsetValueRandSeed(NewSeed);
}

/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeOffsetType(PathOffset NewOffset, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewOffset - the offset type to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the offset value of this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeOffsetType(PathOffset NewOffset, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
		
	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();

	return pAttrBrush->SetPathOffsetType(NewOffset);	
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeOffsetTypeRandSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeed - the offset type to set		
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets the seed for the offset type RNG
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeOffsetTypeSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");

	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;	

	return pPPB->SetOffsetTypeRandSeed(NewSeed);
}



/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeOffsetSeeds(UINT32 NewTypeSeed, UINT32 NewValSeed, 
												AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewTypeSeed - the seed for the offset sequence
				NewValSeed - the seed for the offset value
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This sets both the offset random seeds, this enables us to change both seeds
				with only one button on the infobar.
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeOffsetSeeds(UINT32 NewTypeSeed, UINT32 NewValSeed, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");

	// clear out our cached bounding rect as we are changing size
	pAttrBrush->ClearCachedRect();	

	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	pPPB->SetOffsetTypeRandSeed(NewTypeSeed);
	pPPB->SetOffsetValueRandSeed(NewValSeed);

	return TRUE;
}

/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeSpacingMaxRand(UINT32 MaxRand, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		MaxRand - the new maximum random number
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the maximum random value for spacing for this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeSpacingMaxRand(UINT32 NewMaxRand, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// can't be bothered to write any more natty access fn's in the attribute
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;
	return pPPB->SetSpacingMaxRand(NewMaxRand);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeSpacingRandSeed(UINT32 RandSeed, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		RandSeed - seed to use for random spacing
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To set the random seed for spacing for this attribute node
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeSpacingRandSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	TRACEUSER( "Diccon", _T("Setting Random Seed\n"));

	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	// ok now that the sequence tab is on the same page they have to share a button
	pPPB->SetSequenceSeed(NewSeed);
	return pPPB->SetSpacingRandSeed(NewSeed);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeUseLocalfillColour(BOOL UseLocal, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		UseLocal - the flag to set
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets the flag telling this attribute whether to use local attributes
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeUseLocalFillColour(BOOL bUseLocal, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;
	pPPB->SetUseLocalFillColour(bUseLocal);
	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeUseLocalTransp(BOOL UseLocal, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		UseLocal - the flag to set
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets the flag telling this attribute whether to use local attributes
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeUseLocalTransp(BOOL UseLocal, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;
	pPPB->SetUseLocalTransp(UseLocal);
	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeUseNamedColour(BOOL UseLocal, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		UseLocal - the flag to set
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets the flag telling this attribute whether to use local attributes
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeUseNamedColour(BOOL UseNamed, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;
	pPPB->SetUseNamedColours(UseNamed);

	return TRUE;
}

/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeSequenceType(SequenceType NewSeq, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeq - the sequence type to set
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets the flag telling this attribute what type of object sequence to use
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeSequenceType(SequenceType NewSeq, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	pPPB->SetSequenceType(NewSeq);
	return TRUE;
}



/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeSequenceType(UINT32 NewSeed, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeed - the seed to set
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Seeds the sequence random number generator
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeSequenceSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	pPPB->SetSequenceSeed(NewSeed);
	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeHueIncrement(
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		NewSeed - the seed to set
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Seeds the sequence random number generator
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeHueIncrement(double NewIncr, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	return pPPB->SetHueIncrement(NewIncr);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeHueMaxRand(UINT32 Rand, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		Rand - the maximum random effect on hue
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeHueMaxRand(UINT32 Rand, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	return pPPB->SetHueMaxRand(Rand);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeHueRandSeed(UINT32 Seed, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		Seed - the seed for the hue RNG
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Seeds the sequence random number generator
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeFillSeeds(UINT32 HueSeed, UINT32 SatSeed, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	pPPB->SetHueRandSeed(HueSeed);
	pPPB->SetSatRandSeed(SatSeed);
	return TRUE;
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeSatMaxRand(UINT32 Rand, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		Rand - the maximum random effect on Saturation
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	sa above
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeSatMaxRand(UINT32 Rand, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	return pPPB->SetSatMaxRand(Rand);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeSatRandSeed(UINT32 Seed, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		Seed - the seed for the Saturation RNG
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Seeds the sequence random number generator
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeSatRandSeed(UINT32 Seed, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	return pPPB->SetSatRandSeed(Seed);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeTransparency(INT32 Transp, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		Transp - the value to set (-1 means don't replace transparency)
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	sets the replacement transparency value
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeTransparency(INT32 Transp, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	return pPPB->SetBrushTransparency(Transp);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeTranspPressure(UINT32 Press, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		Press - the extent to which pressure affects brush transparency
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	sets the extent to which pressure affects brush transparency
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeTranspPressure(UINT32 Press, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL");
	
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;

	return pPPB->SetTransparencyPressure(Press);
}


/********************************************************************************************

>	BOOL ChangeBrushAction::ChangeAll(ChangeBrushOpParam* pParam, AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		pParam - the opparam containing the data to change in our brush
	Outputs:	pAttrBrush - the brush attribute to change
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets all the variables in the opparam
	SeeAlso:	-

********************************************************************************************/

BOOL ChangeBrushAction::ChangeAll(ChangeBrushOpParam* pParam, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "brush attribute is NULL in ChangeBrushAction::ChangeAll");
	ERROR2IF(pParam == NULL, FALSE, "OpParam is NULL in ChangeBrushAction::ChangeAll");
	// get the path processor
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return FALSE;
	
	BOOL ok = pPPB->SetBrushScaling(pParam->m_NewScaling);
	if (ok) ok = pPPB->SetBrushScalingIncr(pParam->m_NewScalingIncr);
	if (ok) ok = pPPB->SetBrushScalingIncrConst(pParam->m_NewScalingIncrConst);
	if (ok) ok = pPPB->SetScalingMaxRand(pParam->m_NewScalingMaxRand);
	if (ok) ok = pPPB->SetScalingRandSeed(pParam->m_NewScalingRandSeed);
	if (ok) ok = pPPB->SetSpacing(pParam->m_NewSpacing);
	if (ok) ok = pPPB->SetSpacingIncrConst(pParam->m_NewSpacingIncrConst);
	if (ok) ok = pPPB->SetSpacingIncrProp(pParam->m_NewSpacingIncrProp);
	if (ok) ok = pPPB->SetSpacingMaxRand(pParam->m_NewSpacingMaxRand);
	if (ok) ok = pPPB->SetSpacingRandSeed(pParam->m_NewSpacingRandSeed);
	if (ok) ok = pPPB->SetHueMaxRand(pParam->m_NewHueMaxRand);
	if (ok) ok = pPPB->SetHueRandSeed(pParam->m_NewHueRandSeed);
	if (ok) ok = pPPB->SetSatMaxRand(pParam->m_NewSatMaxRand);
	if (ok) ok = pPPB->SetSatRandSeed(pParam->m_NewSatRandSeed);
	pPPB->SetPathOffsetType(pParam->m_NewPathOffsetType);
	pPPB->SetPathOffsetValue(pParam->m_NewPathOffsetVal);
	if (ok) ok = pPPB->SetOffsetTypeRandSeed(pParam->m_NewOffsetTypeRandSeed);
	if (ok) ok = pPPB->SetOffsetValueMaxRand(pParam->m_NewOffsetValMaxRand);
	if (ok) ok = pPPB->SetOffsetValueRandSeed(pParam->m_NewOffsetValRandSeed);
	pPPB->SetRotated(pParam->m_bNewTiled);
	if (ok) ok = pPPB->SetRotationAngle(pParam->m_NewRotateAngle);
	pPPB->SetSequenceType(pParam->m_NewSequenceType);
	pPPB->SetSequenceSeed(pParam->m_NewSequenceRandSeed);
	pPPB->SetTiling(pParam->m_bNewTiled);
	pPPB->SetUseLocalFillColour(pParam->m_bNewUseLocalFillColour);
	pPPB->SetRotationIncrConst(pParam->m_NewRotationIncrConst);
	pPPB->SetRotationIncrProp(pParam->m_NewRotationIncrProp);
	pPPB->SetRotationMaxPressure(pParam->m_NewRotationMaxPressure);

	return ok;
}



//-------------------------------------------------------
//------------CLASS CHANGEBRUSHOPPARAM-------------------
//-------------------------------------------------------


/********************************************************************************************

>	ChangeBrushOpParam::ChangeBrushOpParam()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	default contructor 
	Errors:		-

********************************************************************************************/

ChangeBrushOpParam::ChangeBrushOpParam()
{
	InitMembers();
}

/********************************************************************************************

>	ChangeBrushOpParam::ChangeBrushOpParam(const ChangeBrushOpParam& Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		Other the ChangeBrushOpParam whose values we wish to assign
	Outputs:	-
	Returns:	-
	Purpose:	copy constructor
	Errors:		-

********************************************************************************************/

ChangeBrushOpParam::ChangeBrushOpParam(const ChangeBrushOpParam& Other)
{

}


/********************************************************************************************

>	void ChangeBrushOpParam::InitMembers()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	initialise our numerous members to something sensible
	Errors:		-

********************************************************************************************/

void ChangeBrushOpParam::InitMembers()
{
	ChangeType = CHANGEBRUSH_NONE;

	m_NewSpacing = m_OldSpacing = 10000;
	m_NewSpacingIncrProp = m_OldSpacingIncrProp = 1.0;
	m_NewSpacingIncrConst = m_OldSpacingIncrConst = 0;
	m_pPressure = NULL;
	m_OldSpacingMaxRand = m_NewSpacingMaxRand = 0;
	m_NewSpacingRandSeed = m_OldSpacingRandSeed = 0;
	m_OldScaling = m_NewScaling = 1.0;
	m_NewScalingIncr = m_OldScalingIncr = 1.0;
	m_NewScalingIncrConst = m_OldScalingIncrConst = 0;
	m_NewScalingMaxRand = m_OldScalingMaxRand = 0;
	m_NewScalingRandSeed = m_OldScalingRandSeed = 0;
	m_OldScalingMaxPressure = m_NewScalingMaxPressure = 0;
	m_OldPathOffsetType = m_NewPathOffsetType = OFFSET_NONE;
	m_NewOffsetTypeRandSeed = m_OldOffsetTypeRandSeed = 0;
	m_OldPathOffsetVal = m_NewPathOffsetVal = 0;
	m_NewOffsetIncrProp = m_OldOffsetIncrProp = 1.0;
	m_NewOffsetIncrConst = m_OldOffsetIncrConst = 0;
	m_NewOffsetValMaxRand = m_OldOffsetValMaxRand = 0;
	m_NewOffsetValRandSeed = m_OldOffsetValRandSeed = 0;
	m_bOldTiled = m_bNewTiled = TRUE;
	m_bOldRotated = m_bNewRotated = FALSE;
	m_OldRotateAngle = m_NewRotateAngle = 0.0;
	m_OldRotationIncrProp = m_NewRotationIncrProp = 1.0;
	m_OldRotationIncrConst = m_NewRotationIncrConst = 0.0;
	m_NewRotateMaxRand = m_OldRotateMaxRand = 0;
	m_NewRotateRandSeed = m_OldRotateRandSeed = 0;
	m_NewRotationMaxPressure = m_OldRotationMaxPressure = 0;
	m_OldBrushName = m_NewBrushName = TEXT("Custom Brush");
	m_bOldUseLocalFillColour = m_bNewUseLocalFillColour = FALSE;
	m_bNewUseLocalTransp = m_bOldUseLocalTransp = FALSE;
	m_bOldUseNamed = m_bNewUseNamed = FALSE;
	m_OldSequenceType = m_NewSequenceType = SEQ_FORWARD;
	m_NewSequenceRandSeed = m_OldSequenceRandSeed = 0;
	m_NewHueIncrement = m_OldHueIncrement = 0;
	m_NewHueMaxRand = m_OldHueMaxRand = 0;
	m_NewHueRandSeed = m_OldHueRandSeed = 0;
	m_NewSatMaxRand = m_OldSatMaxRand = 0;
	m_NewSatRandSeed = m_OldSatRandSeed = 0;
	m_NewTransp = m_OldTransp = 100;
	m_NewTranspPressure = m_OldTranspPressure = 0;

	m_OldTimeStampValue = m_NewTimeStampValue = 50;



}



/********************************************************************************************

>	BOOL ChangeBrushOpParam::ChangeLineWidth()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this op param will cause the line width pf a brush to change, FALSE
				if not
	Purpose:	as above
	Errors:		-

********************************************************************************************/

BOOL ChangeBrushOpParam::ChangeLineWidth()
{
	// see what type we are
	BOOL Retval = FALSE;
	
	switch (ChangeType)
	{
		// these types of changes will cause the line width to change
		case CHANGEBRUSH_SCALING_MAXRAND:
		case CHANGEBRUSH_SCALING_RANDSEED:
		case CHANGEBRUSH_OFFSET_TYPE:
		case CHANGEBRUSH_OFFSET_VAL:
		case CHANGEBRUSH_OFFSET_MAXRAND:
		case CHANGEBRUSH_TANGENTIAL:
		case CHANGEBRUSH_ROTATE_ANGLE:
		case CHANGEBRUSH_ROTATE_INCRCONST:
		case CHANGEBRUSH_ROTATE_MAXRAND:
		case CHANGEBRUSH_ALL:
			Retval = TRUE;
		break;

		default: // do nothing
		break;
	}

	return Retval;

}

/********************************************************************************************

>	ChangeBrushOpParam ChangeBrushOpParam::operator=(const ChangeBrushOpParam& Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		Other the ChangeBrushOpParam whose values we wish to assign
	Outputs:	-
	Returns:	-
	Purpose:	overrides assignment operator
	Errors:		-

********************************************************************************************/

ChangeBrushOpParam ChangeBrushOpParam::operator=(const ChangeBrushOpParam& Other)
{
	m_OldSpacing = Other.m_OldSpacing;
	m_NewSpacing = Other.m_NewSpacing;
	m_NewSpacingIncrProp = Other.m_NewSpacingIncrProp;
	m_OldSpacingIncrProp = Other.m_OldSpacingIncrProp;
	m_NewSpacingIncrConst = Other.m_NewSpacingIncrConst;
	m_OldSpacingIncrConst = Other.m_OldSpacingIncrConst;
	m_OldSpacingMaxRand = Other.m_OldSpacingMaxRand;
	m_NewSpacingMaxRand = Other.m_NewSpacingMaxRand;
	m_NewSpacingRandSeed = Other.m_NewSpacingRandSeed;
	m_OldSpacingRandSeed = Other.m_OldSpacingRandSeed;
	m_OldScaling = Other.m_OldScaling;
	m_NewScaling = Other.m_NewScaling;
	m_NewScalingIncr = Other.m_NewScalingIncr;
	m_OldScalingIncr = Other.m_OldScalingIncr;
	m_NewScalingIncrConst = Other.m_NewScalingIncrConst;
	m_OldScalingIncrConst = Other.m_OldScalingIncrConst;
	m_NewScalingMaxRand = Other.m_NewScalingMaxRand;
	m_OldScalingMaxRand = Other.m_OldScalingMaxRand;
	m_NewScalingRandSeed = Other.m_NewScalingRandSeed;
	m_OldScalingRandSeed = Other.m_OldScalingRandSeed;
	m_NewScalingMaxPressure = Other.m_NewScalingMaxPressure;
	m_OldScalingMaxPressure = Other.m_OldScalingMaxPressure;
	m_OldPathOffsetType = Other.m_OldPathOffsetType;
	m_NewPathOffsetType = Other.m_NewPathOffsetType;
	m_NewOffsetTypeRandSeed = Other.m_NewOffsetTypeRandSeed;
	m_OldOffsetTypeRandSeed = Other.m_OldOffsetTypeRandSeed;
	m_OldPathOffsetVal = Other.m_OldPathOffsetVal;
	m_NewPathOffsetVal = Other.m_NewPathOffsetVal;
	m_NewOffsetIncrProp = Other.m_NewOffsetIncrProp;
	m_OldOffsetIncrProp = Other.m_OldOffsetIncrProp;
	m_NewOffsetIncrConst = Other.m_NewOffsetIncrConst;
	m_OldOffsetIncrConst = Other.m_OldOffsetIncrConst;
	m_NewOffsetValMaxRand = Other.m_NewOffsetValMaxRand;
	m_OldOffsetValMaxRand = Other.m_OldOffsetValMaxRand;
	m_NewOffsetValRandSeed = Other.m_NewOffsetValRandSeed;
	m_OldOffsetValRandSeed = Other.m_OldOffsetValRandSeed;
	m_bOldTiled = Other.m_bOldTiled;
	m_bNewTiled = Other.m_bNewTiled;
	m_bOldRotated = Other.m_bOldRotated;
	m_bNewRotated = Other.m_bNewRotated;
	m_OldRotateAngle = Other.m_OldRotateAngle;
	m_NewRotateAngle = Other.m_NewRotateAngle;
	m_OldRotationIncrProp = Other.m_OldRotationIncrProp;
	m_NewRotationIncrProp = Other.m_NewRotationIncrProp;
	m_OldRotationIncrConst = Other.m_NewRotationIncrConst;
	m_NewRotationIncrConst = Other.m_NewRotationIncrConst;
	m_NewRotateMaxRand = Other.m_NewRotateMaxRand;
	m_OldRotateMaxRand = Other.m_OldRotateMaxRand;
	m_NewRotateRandSeed = Other.m_NewRotateRandSeed;
	m_OldRotateRandSeed = Other.m_OldRotateRandSeed;
	m_NewRotationMaxPressure = Other.m_NewRotationMaxPressure;
	m_OldBrushName = Other.m_OldBrushName;
	m_NewBrushName = Other.m_NewBrushName;
	m_bOldUseLocalFillColour = Other.m_bOldUseLocalFillColour;
	m_bNewUseLocalFillColour = Other.m_bNewUseLocalFillColour;
	m_bOldUseLocalTransp     = Other.m_bOldUseLocalTransp;
	m_bNewUseLocalTransp     = Other.m_bNewUseLocalTransp;
	m_OldSequenceType = Other.m_OldSequenceType;
	m_NewSequenceType = Other.m_NewSequenceType;
	m_NewSequenceRandSeed = Other.m_NewSequenceRandSeed;
	m_OldSequenceRandSeed = Other.m_OldSequenceRandSeed;
	m_NewTimeStampValue = Other.m_NewTimeStampValue;
	m_OldTimeStampValue = Other.m_OldTimeStampValue;
	m_bOldUseNamed = Other.m_bOldUseNamed;
	m_bNewUseNamed = Other.m_bNewUseNamed;
	m_NewHueMaxRand = Other.m_NewHueMaxRand;
	m_OldHueMaxRand = Other.m_OldHueMaxRand;
	m_NewHueRandSeed = Other.m_NewHueRandSeed;
	m_OldHueRandSeed = Other.m_OldHueRandSeed;
	m_NewSatMaxRand = Other.m_NewSatMaxRand;
	m_OldSatMaxRand = Other.m_OldSatMaxRand;
	m_NewSatRandSeed = Other.m_NewSatRandSeed;
	m_OldSatRandSeed = Other.m_OldSatRandSeed;
	m_NewTransp      = Other.m_NewTransp;
	m_OldTransp      = Other.m_OldTransp;
	m_NewTranspPressure = Other.m_NewTranspPressure;
	m_OldTranspPressure = Other.m_OldTranspPressure;
	return *this;
}

/********************************************************************************************

>	void ChangeBrushOpParam::SetOldValues(AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		pAttrBrush - pointer to brush attribute which is being changed
	Outputs:	-
	Returns:	-
	Purpose:	Copies the current state of the brush node into this classes Old member vars
	Errors:		-

********************************************************************************************/

void ChangeBrushOpParam::SetOldValues(AttrBrushType* pAttrBrush)
{
	if (pAttrBrush == NULL)
	{
		ERROR3("Attribute is NULL");
		return;
	}
	MILLIPOINT Value = pAttrBrush->GetSpacing();
	if (Value != -1)
		m_OldSpacing = Value;
	else
		m_OldSpacing = DEFAULT_BRUSH_SPACING;

	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB != NULL)
	{
		m_OldSpacingMaxRand   = pPPB->GetSpacingMaxRand();
		m_OldSpacingRandSeed  = pPPB->GetSpacingRandSeed();
		m_OldSpacingIncrProp  = pPPB->GetSpacingIncrProp();
		m_OldSpacingIncrConst = pPPB->GetSpacingIncrConst();
		m_OldScaling          = pPPB->GetBrushScaling();
		m_OldScalingIncr	  = pPPB->GetBrushScalingIncr();
		m_OldScalingIncrConst = pPPB->GetBrushScalingIncrConst();
		m_OldScalingMaxPressure = pPPB->GetScalingMaxPressure();
		m_OldScalingMaxRand		= pPPB->GetScalingMaxRand();
		m_OldScalingRandSeed	= pPPB->GetScalingRandSeed();
		m_OldPathOffsetVal    = pPPB->GetPathOffsetValue();
		m_OldOffsetIncrProp   = pPPB->GetPathOffsetIncrProp();
		m_OldOffsetIncrConst  = pPPB->GetPathOffsetIncrConst();
		m_OldPathOffsetType   = pPPB->GetPathOffsetType();
		m_OldOffsetValMaxRand = pPPB->GetOffsetValueMaxRand();
		m_bOldTiled           = pPPB->IsTiled();
		m_bOldRotated         = pPPB->IsRotated();
		m_OldRotateAngle      = pPPB->GetRotationAngle();
		m_OldRotationIncrConst = pPPB->GetRotationIncrConst();
		m_OldRotationIncrProp  = pPPB->GetRotationIncrProp();
		m_OldRotationMaxPressure = pPPB->GetRotationMaxPressure();
		m_OldRotateMaxRand	 = pPPB->GetRotationMaxRand();
		m_bOldUseLocalFillColour    = pPPB->GetUseLocalFillColour();
		m_bOldUseLocalTransp = pPPB->GetUseLocalTransp();
		m_OldOffsetTypeRandSeed = pPPB->GetOffsetTypeRandSeed();
		m_OldSequenceType		= pPPB->GetSequenceType();
		m_OldSequenceRandSeed   = pPPB->GetSequenceSeed();
		m_bOldUseNamed          = pPPB->GetUseNamedColours();
		m_OldHueMaxRand			= pPPB->GetHueMaxRand();
		m_OldHueRandSeed		= pPPB->GetHueRandSeed();
		m_OldSatMaxRand			= pPPB->GetSatMaxRand();
		m_OldSatRandSeed		= pPPB->GetSatRandSeed();
		m_OldTransp				= pPPB->GetBrushTransparency();
		m_OldTranspPressure		= pPPB->GetTransparencyPressure();
		
	}
	//m_pPressure = pAttrBrush->GetPressureCache();


}


/********************************************************************************************

>	void ChangeBrushOpParam::SetNewValues(AttrBrushType* pAttrBrush)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		pAttrBrush - the attribute being changed by the operation
	Outputs:	-
	Returns:	-
	Purpose:	Copies the current state of the brush node into this classes New member vars
	Errors:		-

********************************************************************************************/

void ChangeBrushOpParam::SetNewValues(AttrBrushType* pAttrBrush)
{
	if (pAttrBrush == NULL)
	{
		ERROR3("Attribute is NULL");
		return;
	}
	MILLIPOINT Value = pAttrBrush->GetSpacing();
	if (Value != -1)
		m_NewSpacing = Value;
	else
		m_NewSpacing = DEFAULT_BRUSH_SPACING;

	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB != NULL)
	{
		m_NewSpacingMaxRand		= pPPB->GetSpacingMaxRand();
		m_NewSpacingRandSeed	= pPPB->GetSpacingRandSeed();
		m_NewSpacingIncrProp	= pPPB->GetSpacingIncrProp();
		m_NewSpacingIncrConst	= pPPB->GetSpacingIncrConst();
		m_NewScaling			= pPPB->GetBrushScaling();
		m_NewScalingIncr		= pPPB->GetBrushScalingIncr();
		m_NewScalingIncrConst	= pPPB->GetBrushScalingIncrConst();
		m_NewScalingMaxPressure = pPPB->GetScalingMaxPressure();
		m_NewScalingMaxRand		= pPPB->GetScalingMaxRand();
		m_NewScalingRandSeed	= pPPB->GetScalingRandSeed();
		m_NewPathOffsetVal		= pPPB->GetPathOffsetValue();
		m_NewOffsetIncrProp		= pPPB->GetPathOffsetIncrProp();
		m_NewOffsetIncrConst	= pPPB->GetPathOffsetIncrConst();
		m_NewPathOffsetType		= pPPB->GetPathOffsetType();
		m_NewOffsetValMaxRand   = pPPB->GetOffsetValueMaxRand();
		m_bNewTiled				= pPPB->IsTiled();
		m_bNewRotated			= pPPB->IsRotated();
		m_NewRotateAngle		= pPPB->GetRotationAngle();
		m_NewRotationIncrConst	= pPPB->GetRotationIncrConst();
		m_NewRotationIncrProp	= pPPB->GetRotationIncrProp();
		m_NewRotateRandSeed		= pPPB->GetRotationRandSeed();
		m_NewRotateMaxRand		= pPPB->GetRotationMaxRand();
		m_NewRotationMaxPressure	= pPPB->GetRotationMaxPressure();
		m_bNewUseLocalFillColour   = pPPB->GetUseLocalFillColour();
		m_bNewUseLocalTransp	= pPPB->GetUseLocalTransp();
		m_NewOffsetTypeRandSeed = pPPB->GetOffsetTypeRandSeed();
		m_NewSequenceType		= pPPB->GetSequenceType();
		m_NewSequenceRandSeed   = pPPB->GetSequenceSeed();
		m_bNewUseNamed          = pPPB->GetUseNamedColours();
		m_NewHueMaxRand			= pPPB->GetHueMaxRand();
		m_NewHueRandSeed		= pPPB->GetHueRandSeed();
		m_NewSatMaxRand			= pPPB->GetSatMaxRand();
		m_NewSatRandSeed		= pPPB->GetSatRandSeed();
		m_NewTransp				= pPPB->GetBrushTransparency();
		m_NewTranspPressure		= pPPB->GetTransparencyPressure();
	}

}

/********************************************************************************************

>	void ChangeBrushOpParam::SwapOldAndNew()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Swaps the old and new values of this class
	Errors:		-

********************************************************************************************/

void ChangeBrushOpParam::SwapOldAndNew()
{
	SwapINT32(m_OldSpacing, m_NewSpacing);
	SwapINT32( m_OldSpacingIncrConst, m_NewSpacingIncrConst);
	SwapDouble(m_OldSpacingIncrProp, m_NewSpacingIncrProp);
	SwapUINT( m_OldSpacingMaxRand, m_NewSpacingMaxRand);
	SwapUINT( m_OldSpacingRandSeed, m_NewSpacingRandSeed);
	SwapDouble(m_OldScaling, m_NewScaling);
	SwapDouble(m_OldScalingIncr, m_NewScalingIncr);
	SwapDouble(m_OldScalingIncrConst, m_NewScalingIncrConst);
	SwapUINT( m_OldScalingMaxRand, m_NewScalingMaxRand);
	SwapUINT( m_OldScalingRandSeed, m_NewScalingRandSeed);
	SwapUINT( m_OldScalingMaxPressure, m_NewScalingMaxPressure);
	
	// using this typedef in the macro seems to cause floating point errors in release builds
	PathOffset TempOffset = m_OldPathOffsetType;
	m_OldPathOffsetType = m_NewPathOffsetType;
	m_NewPathOffsetType = TempOffset;

	SwapUINT( m_OldOffsetTypeRandSeed, m_NewOffsetTypeRandSeed);
	SwapINT32( m_OldPathOffsetVal, m_NewPathOffsetVal);
	SwapINT32( m_OldOffsetIncrConst, m_NewOffsetIncrConst);
	SwapDouble(m_OldOffsetIncrProp, m_NewOffsetIncrProp);
	SwapUINT( m_OldOffsetValMaxRand, m_NewOffsetValMaxRand);
	SwapUINT( m_OldOffsetValRandSeed, m_NewOffsetValRandSeed);
	SwapBOOL(m_bOldTiled, m_bNewTiled);
	SwapBOOL(m_bOldRotated, m_bNewRotated);
	SwapDouble(m_OldRotateAngle, m_NewRotateAngle);
	SwapDouble(m_OldRotationIncrProp, m_NewRotationIncrProp);
	SwapDouble(m_OldRotationIncrConst, m_NewRotationIncrConst);
	SwapUINT( m_OldRotateMaxRand, m_NewRotateMaxRand);
	SwapUINT( m_OldRotateRandSeed, m_NewRotateRandSeed);
	SwapUINT( m_OldRotationMaxPressure, m_NewRotationMaxPressure);
	SwapBOOL(m_bOldUseLocalFillColour, m_bNewUseLocalFillColour);
	SwapBOOL(m_bOldUseLocalTransp, m_bNewUseLocalTransp);
	SequenceType Temp = m_OldSequenceType;
	m_OldSequenceType = m_NewSequenceType;
	m_NewSequenceType = Temp;
	SwapUINT( m_OldSequenceRandSeed, m_NewSequenceRandSeed);
	SwapBOOL(m_bOldUseNamed, m_bNewUseNamed);
	SwapUINT( m_OldHueMaxRand, m_NewHueMaxRand);
	SwapUINT( m_OldHueRandSeed, m_NewHueRandSeed);
	SwapUINT( m_OldSatMaxRand, m_NewSatMaxRand);
	SwapUINT( m_OldSatRandSeed, m_NewSatRandSeed);
	SwapINT32( m_OldTransp, m_NewTransp);
	SwapUINT( m_OldTranspPressure, m_NewTranspPressure);
}


/********************************************************************************************

>	void ChangeBrushOpParam::SwapDouble(double& First, double& Second)
	
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		First - the first value
				Second - the value to swap with
	Outputs:	-
	Returns:	-
	Purpose:	Swaps the two double values
	Errors:		-

********************************************************************************************/

void ChangeBrushOpParam::SwapDouble(double& First, double& Second)
{
	double Temp = First;
	First = Second;
	Second = Temp;
}


/********************************************************************************************

>	void ChangeBrushOpParam::SwapUINT(UINT32& First, UINT32& Second)
	
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		First - the first value
				Second - the value to swap with
	Outputs:	-
	Returns:	-
	Purpose:	Swaps the two UINT32 values
	Errors:		-

********************************************************************************************/

void ChangeBrushOpParam::SwapUINT(UINT32& First, UINT32& Second)
{
	UINT32 Temp = First;
	First = Second;
	Second = Temp;
}


/********************************************************************************************

>	void ChangeBrushOpParam::SwapINT32(INT32& First, INT32& Second)
	
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		First - the first value
				Second - the value to swap with
	Outputs:	-
	Returns:	-
	Purpose:	Swaps the two INT32 values
	Errors:		-

********************************************************************************************/

void ChangeBrushOpParam::SwapINT32(INT32& First, INT32& Second)
{
	INT32 Temp = First;
	First = Second;
	Second = Temp;
}


/********************************************************************************************

>	void ChangeBrushOpParam::SwapBOOL(BOOL& First, BOOL& Second)
	
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		First - the first value
				Second - the value to swap with
	Outputs:	-
	Returns:	-
	Purpose:	Swaps the two BOOL values
	Errors:		-

********************************************************************************************/

void ChangeBrushOpParam::SwapBOOL(BOOL& First, BOOL& Second)
{
	BOOL Temp = First;
	First = Second;
	Second = Temp;
}

/********************************************************************************************

>	void ChangeBrushOpParam::SetValues(BrushData Data)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		Data - a data object containing the values to set
	Outputs:	-
	Returns:	-
	Purpose:	sets the values of this opparam to those contained in the data object
	Errors:		-

********************************************************************************************/

void ChangeBrushOpParam::SetValues(BrushData Data)
{
	m_NewSpacing			= Data.m_BrushSpacing;
	m_NewSpacingIncrConst	= Data.m_BrushSpacingIncrConst;
	m_NewSpacingIncrProp    = Data.m_BrushSpacingIncrProp;
	m_NewSpacingMaxRand     = Data.m_BrushSpacingMaxRand;
	m_NewSpacingRandSeed    = Data.m_BrushSpacingRandSeed;
	m_NewScaling            = Data.m_BrushScaling;
	m_NewScalingIncr		= Data.m_BrushScalingIncr;
	m_NewScalingIncrConst   = Data.m_BrushScalingIncrConst;
	m_NewScalingMaxRand     = Data.m_BrushScalingMaxRand;
	m_NewScalingRandSeed    = Data.m_BrushScalingRandSeed;
	m_NewScalingMaxPressure = Data.m_ScalingMaxPressure;
	m_NewPathOffsetType     = Data.m_PathOffsetType;
	m_NewOffsetTypeRandSeed = Data.m_OffsetTypeRandSeed;
	m_NewPathOffsetVal		= Data.m_PathOffsetValue;
	m_NewOffsetIncrConst    = Data.m_PathOffsetIncrConst;
	m_NewOffsetIncrProp     = Data.m_PathOffsetIncrProp;
	m_NewOffsetValMaxRand   = Data.m_OffsetValueMaxRand;
	m_NewOffsetValRandSeed  = Data.m_OffsetValueRandSeed;
	m_bNewTiled				= Data.m_bTile;
	m_bNewRotated			= Data.m_bRotate;
	m_NewRotateAngle        = Data.m_RotateAngle;
	m_NewRotationIncrProp	= Data.m_RotAngleIncrProp;
	m_NewRotationIncrConst  = Data.m_RotAngleIncrConst;
	m_NewRotateMaxRand		= Data.m_RotationMaxRand;
	m_NewRotateRandSeed     = Data.m_RotationRandSeed;
	m_NewRotationMaxPressure = Data.m_RotationMaxPressure;
	m_bNewUseLocalFillColour = Data.m_bUseLocalFillColour;
	m_bNewUseNamed			 = Data.m_bUseNamedColour;
	m_NewSequenceType        = Data.m_SequenceType;
	m_NewSequenceRandSeed    = Data.m_SequenceRandSeed;
	m_NewHueMaxRand			 = Data.m_BrushHueMaxRand;
	m_NewHueRandSeed		 = Data.m_BrushHueRandSeed;
	m_NewSatMaxRand		     = Data.m_BrushSatMaxRand;
	m_NewSatRandSeed		 = Data.m_BrushSatRandSeed;
	m_NewTransp				 = Data.m_BrushTransparency;
	m_NewTranspPressure		 = Data.m_TranspMaxPressure;

}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The RemoveTimeStampPointsAction class

/********************************************************************************************

>	RemoveTimeStampPointsAction::RemoveTimeStampPointsAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RemoveTimeStampPointsAction::RemoveTimeStampPointsAction()
{
	m_pTimeStampList = NULL;
}


/********************************************************************************************

>	RemoveTimeStampPointsAction::~RemoveTimeStampPointsAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


RemoveTimeStampPointsAction::~RemoveTimeStampPointsAction()
{
	if (m_pTimeStampList != NULL)
		delete m_pTimeStampList;
}

/********************************************************************************************

>	ActionCode RemoveTimeStampPointsAction::Init( 	Operation* 		pOp,
											ActionList* 	pActionList,
											AttrBrushType * pAttrBrush,
											MILLIPOINT		StartDistance,
											MILLIPOINT		EndDistamce,
											RemoveTimeStampPointsAction** ppNewAction);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pAttrBrush      - the brush attribute to perform the action on
				StartDistance   - the distance to start removing at
				EndDistance     - the distance to stop removing at
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

				This function actually changes the blend node in a way specified in pChangeParam
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode RemoveTimeStampPointsAction::Init(Operation* pOp, ActionList* pActionList, 
											AttrBrushType* pAttrBrush, MILLIPOINT StartDistance,
											MILLIPOINT EndDistance, TimeStampList* pTimeStampList,
											RemoveTimeStampPointsAction** ppNewAction)
{
	ERROR2IF(pAttrBrush == NULL,AC_FAIL,"pAttrBrush is NULL");
	ERROR2IF(StartDistance < 0, AC_FAIL, "Invalid start distance");

	// just check that we do actually have a time stamping brush attribute, if not then just quit
	if (!pAttrBrush->IsTimeStamping())
		return AC_OK;
	// Get the attribute value, just to make sure
	BrushAttrValue* pVal = (BrushAttrValue*)pAttrBrush->GetAttributeValue();
	if (pVal == NULL)
		return AC_FAIL;

	// make the new action
	UINT32 ActSize = sizeof(RemoveTimeStampPointsAction);
	RemoveTimeStampPointsAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(RemoveTimeStampPointsAction),(Action**)&pNewAction);
	*ppNewAction = pNewAction;

	if (Ac != AC_FAIL)
	{
		
		// if the list is NULL then we are removing points
		if (pTimeStampList == NULL)
		{
			//allocate a new list to store the deleted points in
			pTimeStampList = new TimeStampList;
			if (pTimeStampList == NULL)
			{
				delete pNewAction;
				return AC_FAIL;
			}

			pVal->DeleteTimeStampPoints(StartDistance, EndDistance, pTimeStampList);
		//	m_pAttrBrush = pAttrBrush;
		//	m_StartDistance = StartDistance;
		//	m_EndDistance = EndDistance;
		}
		// otherwise we are undoing
		else
		{
			pVal->AddTimeStampPoints(pTimeStampList, StartDistance);
			delete pTimeStampList;
			pTimeStampList = NULL;
		}
		pNewAction->m_pTimeStampList = pTimeStampList;
		pNewAction->m_pAttrBrush = pAttrBrush;
		pNewAction->m_StartDistance = StartDistance;
		pNewAction->m_EndDistance = EndDistance;

		// tell the attr we are changing the list, so don't reposition
		pVal->SetTimeStampUpdateType(UPDATE_LISTANDPOSITION);
	}
	return Ac;
}



/********************************************************************************************

>	ActionCode RemoveTimeStampPointsAction::Execute();

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBrush to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBrush
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode RemoveTimeStampPointsAction::Execute()
{
	
	ActionCode Act;
	RemoveTimeStampPointsAction* pAction;
	Act = RemoveTimeStampPointsAction::Init(pOperation,pOppositeActLst,m_pAttrBrush,m_StartDistance, m_EndDistance, m_pTimeStampList, &pAction);
	// this gets deleted in the Init, but not nulled
	m_pTimeStampList = NULL;
	return Act;
}


//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The AddTimeStampPointsAction class

/********************************************************************************************

>	AddTimeStampPointsAction::AddTimeStampPointsAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AddTimeStampPointsAction::AddTimeStampPointsAction()
{
	m_pTimeStampList = NULL;
}


/********************************************************************************************

>	AddTimeStampPointsAction::~AddTimeStampPointsAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


AddTimeStampPointsAction::~AddTimeStampPointsAction()
{
	if (m_pTimeStampList != NULL)
		delete m_pTimeStampList;
}

/********************************************************************************************

>	ActionCode AddTimeStampPointsAction::Init( 	Operation* 		pOp,
											ActionList* 	pActionList,
											AttrBrushType * pAttrBrush,
											TimeStampList* pNewPoints,
											MILLIPOINT		StartDistamce,
											AddTimeStampPointsAction** ppNewAction);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pAttrBrush      - the brush attribute to perform the action on
				pNewPoints		- the points to add to the list
				StartDistance   - the distance to start adding the points 
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

				This function actually changes the blend node in a way specified in pChangeParam
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode AddTimeStampPointsAction::Init(Operation* pOp, ActionList* pActionList, 
											AttrBrushType* pAttrBrush, TimeStampList* pNewPoints,
											MILLIPOINT StartDistance, MILLIPOINT EndDistance, 
											size_t NumPoints, AddTimeStampPointsAction** ppNewAction)
{
	ERROR2IF(pAttrBrush == NULL,AC_FAIL,"pAttrBrush is NULL");
	ERROR2IF(StartDistance < 0, AC_FAIL, "Invalid start distance");

	// just check that we do actually have a time stamping brush attribute, if not then just quit
	if (!pAttrBrush->IsTimeStamping())
		return AC_OK;
	// Get the attribute value, just to make sure
	BrushAttrValue* pVal = (BrushAttrValue*)pAttrBrush->GetAttributeValue();
	if (pVal == NULL)
		return AC_FAIL;

	// make the new action
	UINT32 ActSize = sizeof(AddTimeStampPointsAction);
	AddTimeStampPointsAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(AddTimeStampPointsAction),(Action**)&pNewAction);
	*ppNewAction = pNewAction;

	if (Ac != AC_FAIL)
	{
		// if the list is NULL then we are removing points
		if (pNewPoints == NULL)
		{
			//allocate a new list to store the deleted points in
			pNewPoints = new TimeStampList;
			if (pNewPoints == NULL)
			{
				delete pNewAction;
				return AC_FAIL;
			}

			pVal->DeleteTimeStampPoints(StartDistance, EndDistance, pNewPoints);
		}
		// otherwise we are adding
		else
		{	
			pVal->AddTimeStampPoints(pNewPoints, StartDistance);
			NumPoints = (UINT32)pNewPoints->size();
		/*	// we want to find out the distance between the start and end points, 
			// as the list may well have been sorted by the time we come to undo
			TimeStampBrushPoint StartPoint  = pNewPoints->GetHead();
			TimeStampBrushPoint EndPoint = pNewPoints->GetTail();
			MILLIPOINT ListDistance = EndPoint.m_Distance - StartPoint.m_Distance;
			pNewAction->m_EndDistance = StartDistance + ListDistance ;*/
			
			delete pNewPoints;
			pNewPoints = NULL;
		}
		pNewAction->m_pAttrBrush = pAttrBrush;
		pNewAction->m_pTimeStampList = pNewPoints;
		pNewAction->m_StartDistance = StartDistance;
		pNewAction->m_EndDistance = EndDistance;
		pNewAction->m_NumPoints = NumPoints;
		
		// tell the attr we are changing the list, so don't reposition
		pVal->SetTimeStampUpdateType(UPDATE_LISTANDPOSITION);
		
		TRACEUSER( "Diccon", _T("Start = %d, End = %d\n"), StartDistance, pNewAction->m_EndDistance);

		
	}
	return Ac;
}



/********************************************************************************************

>	ActionCode AddTimeStampPointsAction::Execute();

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBrush to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBrush
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode AddTimeStampPointsAction::Execute()
{
	ActionCode Act;
	AddTimeStampPointsAction* pAction;
	size_t NumPoints;
	if (m_pTimeStampList != NULL)
		NumPoints = m_pTimeStampList->size();
	else
		NumPoints = m_NumPoints;
	Act = AddTimeStampPointsAction::Init( pOperation, pOppositeActLst, m_pAttrBrush, m_pTimeStampList, 
		m_StartDistance, m_EndDistance, NumPoints, &pAction );

	// the list gets deleted in the Init but not NULLED
	m_pTimeStampList = NULL;

	return Act;
}


//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The UpdateBrushAction class

/********************************************************************************************

>	UpdateBrushAction::UpdateBrushAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UpdateBrushAction::UpdateBrushAction()
{
	m_pNodePath = NULL;
}


/********************************************************************************************

>	UpdateBrushAction::~UpdateBrushAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


UpdateBrushAction::~UpdateBrushAction()
{

}

/********************************************************************************************

>	ActionCode UpdateBrushAction::Init( 	Operation* 		pOp,
											ActionList* 	pActionList,
											AttrBrushType * pAttrBrush,
											BrushList* pNewPoints,
											MILLIPOINT		StartDistamce,
											UpdateBrushAction** ppNewAction);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pNewPoints		- the points to add to the list
				StartDistance   - the distance to start adding the points 
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

				This function actually changes the blend node in a way specified in pChangeParam
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode UpdateBrushAction::Init(Operation* pOp, ActionList* pActionList,  NodePath* pNodePath, 
								   UpdateBrushAction** ppNewAction)
{
	ERROR2IF(pNodePath == NULL, AC_FAIL, "Nodepath is NULL");

	// Change here now that our attributes are being factored again - we can't rely on pAttrBrush
	// not having been deleted so get a new one each time

	AttrBrushType* pBrush = NULL;
	pNodePath->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), (NodeAttribute**)&pBrush);
	if (pBrush == NULL)
		return AC_OK;

	// just check that our brush isn't a default
	if (pBrush->GetBrushHandle() == BrushHandle_NoBrush)
		return AC_OK;
	// Get the attribute value, just to make sure
	BrushAttrValue* pVal = (BrushAttrValue*)pBrush->GetAttributeValue();
	if (pVal == NULL)
		return AC_FAIL;

	// make the new action
	UINT32 ActSize = sizeof(UpdateBrushAction);
	UpdateBrushAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(UpdateBrushAction),(Action**)&pNewAction);
	*ppNewAction = pNewAction;

	// all we really do is ask the brush to update itself
	if (Ac != AC_FAIL)
		pBrush->NodePathUpdated(&(pNodePath->InkPath));

	pNewAction->m_pNodePath = pNodePath;

	return Ac;
}



/********************************************************************************************

>	ActionCode UpdateBrushAction::Execute();

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBrush to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBrush
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode UpdateBrushAction::Execute()
{
	ActionCode Act;
	UpdateBrushAction* pAction;
	Act = UpdateBrushAction::Init(pOperation,pOppositeActLst, m_pNodePath, &pAction);

	return Act;
}



//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The SetTimeStampUpdateTypeAction class

/********************************************************************************************

>	SetTimeStampUpdateTypeAction::SetTimeStampUpdateTypeAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

SetTimeStampUpdateTypeAction::SetTimeStampUpdateTypeAction()
{
	m_pNodePath = NULL;
	m_pAttrBrush = NULL;
	m_TypeToSet = UPDATE_NONE;
}


/********************************************************************************************

>	SetTimeStampUpdateTypeAction::~SetTimeStampUpdateTypeAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


SetTimeStampUpdateTypeAction::~SetTimeStampUpdateTypeAction()
{

}

/********************************************************************************************

>	ActionCode SetTimeStampUpdateTypeAction::Init( 	Operation* 		pOp,
											ActionList* 	pActionList,
											AttrBrushType * pAttrBrush,
											BrushList* pNewPoints,
											MILLIPOINT		StartDistamce,
											SetTimeStampUpdateTypeAction** ppNewAction);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pAttrBrush      - the brush attribute to perform the action on
				pNewPoints		- the points to add to the list
				StartDistance   - the distance to start adding the points 
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

				This function actually changes the blend node in a way specified in pChangeParam
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode SetTimeStampUpdateTypeAction::Init(Operation* pOp, ActionList* pActionList, AttrBrushType* pAttrBrush,
									   NodePath* pNodePath,ListUpdateType TypeToSet, SetTimeStampUpdateTypeAction** ppNewAction)
{
	ERROR2IF(pAttrBrush == NULL,AC_FAIL,"pAttrBrush is NULL");
	ERROR2IF(pNodePath == NULL, AC_FAIL, "Nodepath is NULL");

	// just check that our brush isn't a default
	if (pAttrBrush->GetBrushHandle() == BrushHandle_NoBrush)
		return AC_OK;
	// Get the attribute value, just to make sure
	BrushAttrValue* pVal = (BrushAttrValue*)pAttrBrush->GetAttributeValue();
	if (pVal == NULL)
		return AC_FAIL;

	// make the new action
	UINT32 ActSize = sizeof(SetTimeStampUpdateTypeAction);
	SetTimeStampUpdateTypeAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(SetTimeStampUpdateTypeAction),(Action**)&pNewAction);
	*ppNewAction = pNewAction;

	// all we really do is ask the brush to update itself
	if (Ac != AC_FAIL)
		pVal->SetTimeStampUpdateType(TypeToSet);

	// set the members of the undo action
	pNewAction->m_pAttrBrush = pAttrBrush;
	pNewAction->m_pNodePath = pNodePath;
	pNewAction->m_TypeToSet= TypeToSet;

	return Ac;
}



/********************************************************************************************

>	ActionCode SetTimeStampUpdateTypeAction::Execute();

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBrush to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBrush
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode SetTimeStampUpdateTypeAction::Execute()
{
	ActionCode Act;
	SetTimeStampUpdateTypeAction* pAction;
	Act = SetTimeStampUpdateTypeAction::Init(pOperation,pOppositeActLst,m_pAttrBrush, m_pNodePath, m_TypeToSet, &pAction);

	return Act;
}


//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The RemovePressurePointsAction class

/********************************************************************************************

>	RemovePressurePointsAction::RemovePressurePointsAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RemovePressurePointsAction::RemovePressurePointsAction()
{
	m_pPressureSampler = NULL;
}


/********************************************************************************************

>	RemovePressurePointsAction::~RemovePressurePointsAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


RemovePressurePointsAction::~RemovePressurePointsAction()
{
	if (m_pPressureSampler != NULL)
		delete m_pPressureSampler;
}

/********************************************************************************************

>	ActionCode RemovePressurePointsAction::Init( 	Operation* 		pOp,
											ActionList* 	pActionList,
											AttrBrushType * pAttrBrush,
											MILLIPOINT		StartDistance,
											MILLIPOINT		EndDistamce,
											RemovePressurePointsAction** ppNewAction);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pAttrBrush      - the brush attribute to perform the action on
				StartIndex		- the index in the pressure list to begin removing at
				NumPoints		- the number of points to remove
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.


	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode RemovePressurePointsAction::Init(Operation* pOp, ActionList* pActionList, 
											AttrBrushType* pAttrBrush, UINT32 StartIndex,
											UINT32 NumPoints, CSampleData* pPressureData,
											RemovePressurePointsAction** ppNewAction)
{
	ERROR2IF(pAttrBrush == NULL,AC_FAIL,"pAttrBrush is NULL");
	
	// just check that we do actually have a time stamping brush attribute, if not then just quit
	if (!pAttrBrush->ContainsPressureCache())
		return AC_OK;
	// Get the attribute value, just to make sure
	BrushAttrValue* pVal = (BrushAttrValue*)pAttrBrush->GetAttributeValue();
	if (pVal == NULL)
		return AC_FAIL;
	
	// make the new action
	UINT32 ActSize = sizeof(RemovePressurePointsAction);
	RemovePressurePointsAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(RemovePressurePointsAction),(Action**)&pNewAction);
	*ppNewAction = pNewAction;

	if (Ac != AC_FAIL)
	{
		
		// if the list is NULL then we are removing points
		if (pPressureData == NULL)
		{
			//allocate a new list to store the deleted points in
			pPressureData = new CSampleData;
			if (pPressureData == NULL)
			{
				delete pNewAction;
				return AC_FAIL;
			}
			// initialise the array 
			if (!pPressureData->InitialiseData(NumPoints))
			{
				delete pNewAction;
				delete pPressureData;
				return AC_FAIL;
			}

				pVal->DeletePressurePoints(StartIndex, NumPoints, pPressureData->GetSampleArray());
				pPressureData->SetNumItemsFromArraySize();
		//	m_pAttrBrush = pAttrBrush;
		//	m_StartDistance = StartDistance;
		//	m_EndDistance = EndDistance;
		}
		// otherwise we are undoing
		else
		{
			pVal->AddPressurePoints(pPressureData, StartIndex);
			delete pPressureData;
			pPressureData = NULL;
		}
		pNewAction->m_pPressureSampler = pPressureData;
		pNewAction->m_pAttrBrush = pAttrBrush;
		pNewAction->m_StartIndex = StartIndex;
		pNewAction->m_NumPoints  = NumPoints;

		pAttrBrush->ClearCachedRect();

		// tell the attr we are changing the list, so don't reposition
		pVal->SetPressureUpdateType(UPDATE_LISTANDPOSITION);
	}
	return Ac;
}



/********************************************************************************************

>	ActionCode RemovePressurePointsAction::Execute();

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBrush to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBrush
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode RemovePressurePointsAction::Execute()
{
	TRACEUSER( "Diccon", _T("\nUNDOING REMOVE PRESSURE POINTS\n"));
	ActionCode Act;
	RemovePressurePointsAction* pAction;
	Act = RemovePressurePointsAction::Init(pOperation,pOppositeActLst,m_pAttrBrush,m_StartIndex, m_NumPoints,  m_pPressureSampler, &pAction);
	// this gets deleted in the Init, but not nulled
	m_pPressureSampler = NULL;
	return Act;
}



//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The AddPressurePointsAction class

/********************************************************************************************

>	AddPressurePointsAction::AddPressurePointsAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AddPressurePointsAction::AddPressurePointsAction()
{
	m_pPressureSampler = NULL;
}


/********************************************************************************************

>	AddPressurePointsAction::~AddPressurePointsAction()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


AddPressurePointsAction::~AddPressurePointsAction()
{
	if (m_pPressureSampler != NULL)
		delete m_pPressureSampler;
}

/********************************************************************************************

>	ActionCode AddPressurePointsAction::Init( 	Operation* 		pOp,
											ActionList* 	pActionList,
											AttrBrushType * pAttrBrush,
											PressureList* pNewPoints,
											MILLIPOINT		StartDistamce,
											AddPressurePointsAction** ppNewAction);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pAttrBrush      - the brush attribute to perform the action on
				pNewPoints		- the points to add to the list
				StartDistance   - the distance to start adding the points 
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

				This function actually changes the blend node in a way specified in pChangeParam
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode AddPressurePointsAction::Init(Operation* pOp, ActionList* pActionList, 
											AttrBrushType* pAttrBrush, CSampleData* pNewPoints,
											UINT32 StartIndex, size_t NumPoints, 
											AddPressurePointsAction** ppNewAction)
{
	ERROR2IF(pAttrBrush == NULL,AC_FAIL,"pAttrBrush is NULL");

	// just check that we do actually have a pressure brush attribute, if not then just quit
	if (!pAttrBrush->ContainsPressureCache())
		return AC_OK;
	// Get the attribute value, just to make sure
	BrushAttrValue* pVal = (BrushAttrValue*)pAttrBrush->GetAttributeValue();
	if (pVal == NULL)
		return AC_FAIL;

	// make the new action
	UINT32 ActSize = sizeof(AddPressurePointsAction);
	AddPressurePointsAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(AddPressurePointsAction),(Action**)&pNewAction);
	*ppNewAction = pNewAction;

	if (Ac != AC_FAIL)
	{
		// if the CSampleData is NULL then we are removing points
		if (pNewPoints == NULL)
		{
			//allocate a new list to store the deleted points in
			pNewPoints = new CSampleData;
			if (pNewPoints == NULL)
			{
				delete pNewAction;
				return AC_FAIL;
			}
			// initialise the array
			if (!pNewPoints->InitialiseData(NumPoints))
			{
				delete pNewAction;
				delete pNewPoints;
				return AC_FAIL;
			}

			pVal->DeletePressurePoints(StartIndex, NumPoints, pNewPoints->GetSampleArray());
			// we must calculate num items ourselves
			pNewPoints->SetNumItemsFromArraySize();
		}
		// otherwise we are adding
		else
		{	
			pNewPoints->ClipArray(NumPoints);
			pVal->AddPressurePoints(pNewPoints, StartIndex);
			NumPoints = (UINT32)pNewPoints->GetNumItems();
			
			delete pNewPoints;
			pNewPoints = NULL;
		}
		pNewAction->m_pAttrBrush = pAttrBrush;
		pNewAction->m_pPressureSampler = pNewPoints;
		pNewAction->m_StartIndex = StartIndex;
		pNewAction->m_NumPoints = NumPoints;
		
		pAttrBrush->ClearCachedRect();

		// tell the attr we are changing the list, so don't reposition
		pVal->SetPressureUpdateType(UPDATE_LISTANDPOSITION);
		
	//	TRACEUSER( "Diccon", _T("Start = %d, End = %d\n"), StartDistance, pNewAction->m_EndDistance);

		
	}
	return Ac;
	
}



/********************************************************************************************

>	ActionCode AddPressurePointsAction::Execute();

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBrush to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBrush
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode AddPressurePointsAction::Execute()
{
	TRACEUSER( "Diccon", _T("\nUNDOING ADD PRESSURE POINTS\n"));
	ActionCode Act;
	AddPressurePointsAction* pAction;
	size_t NumPoints;
//	if (m_pPressureSampler != NULL)
//		NumPoints = m_pPressureSampler->GetNumItems();
//	else
		NumPoints = m_NumPoints;
	Act = AddPressurePointsAction::Init(pOperation,pOppositeActLst,m_pAttrBrush, m_pPressureSampler, m_StartIndex, NumPoints, &pAction);

	// the list gets deleted in the Init but not NULLED
	m_pPressureSampler = NULL;

	return Act;
}
