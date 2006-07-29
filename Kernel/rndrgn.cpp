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
*/

// RNDRGN.CPP
//
// Created: 10/5/93 by Phil, Will, Simon
//

#include "camtypes.h"
//#include "rndrgn.h"          - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "view.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ccdc.h"
//#include "tim.h"
#include "diagnost.h"
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "qualattr.h"
#include "osrndrgn.h"
#include "fuzzclip.h"
#include "textfuns.h"
#include "lineattr.h"
#include "nodebmp.h"
#include "colormgr.h"
#include "isetattr.h"
#include "printctl.h"
#include "pathpcs.h"
#include "strkattr.h"
#include "offattr.h"
#include "ppbrush.h"
#include "clipattr.h"
#include "gradtbl.h"
//#include "capturemanager.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "oilbitmap.h"	// TEMP BODGE for testing

#include "fthrattr.h" // For a bodge

#include "scanrr.h"

#include "rendwnd.h"	// Temporary until DC creation moved to wxOil

DECLARE_SOURCE("$Revision$");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

CC_IMPLEMENT_DYNAMIC(RenderRegion, ListItem)
CC_IMPLEMENT_DYNAMIC(SubRenderContext, CCObject)
CC_IMPLEMENT_DYNAMIC(SubTreeRenderContext, SubRenderContext)

const INT32 CHUNKSIZE = 256;	// Amount to claim when memory block is resized

// Static vars initialisation
//BOOL RenderRegion::MergeTestWait = FALSE;
//BOOL RenderRegion::MergeShowRects = FALSE;

/********************************************************************************************

	Some comments from Andy regarding the attribute stack, based on looking at the source.

	When something is pushed onto the ContextStack it looks like this:
	[lower memory]
	ContextLevel
	size of this item (negated), zero means bottom of stack
	pointer to restore function
	[possibly extra bytes e.g. sizeof(fn ptr) returns 6 in 16-bit compiler]
	[extra data, always longwords]

	After pushing A then pushing B, the stack would look something like this:

[low memory]<------A-------><-------B------->[high]
							|				 |
	PreviousContext---------/				 |
	ContextStackOffset-----------------------/

	This pushing gets done in SaveObjectState.

	ContextStackOffset is sort of the stack pointer. PreviousContext is used to
	be able to get back. These are offsets because the stack lives in the relocatable
	heap so is likely to move.

	Popping of attributes is done in RestoreAttributes.

********************************************************************************************/


/********************************************************************************************

	Preference:	SelectedBlobSize
	Section:	Selection Blob Sizes
	Range:		0 - 30
	Purpose:	The number represents the size of the blobs on the control points of selected
				objects. The Selected Blob Size is the size of the blobs when they have been
				clicked on.
	SeeAlso:	UnSelectedBlobSize

********************************************************************************************/
UINT32 RenderRegion::SelectedBlobSize   = 3;
UINT32 RenderRegion::MultiStageSelectedBlobSize   = 4;

/********************************************************************************************

	Preference:	UnSelectedBlobSize
	Section:	Selection Blob Sizes
	Range:		0 - 30
	Purpose:	The number represents the size of the blobs on the control points of selected
				objects. The UnSelected Blob Size is the size of the blobs when the object is
				selected, but none of its control points have been clicked on (and are not 
				themselves selected)
	SeeAlso:	SelectedBlobSize

********************************************************************************************/
UINT32 RenderRegion::UnSelectedBlobSize = 2;
UINT32 RenderRegion::MultiStageUnSelectedBlobSize = 3;

/********************************************************************************************

>	UINT32 RenderRegion::ClipViewBlobSize = 8;

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22 May 2000
	Purpose:	This number represents the size of the ClipView's special selection blobs.
				These blobs are only visible on selected ClipView groups when using the
				selector tool, and are used to select different sections of the group.
	SeeAlso:	SelectedBlobSize

********************************************************************************************/
UINT32 RenderRegion::ClipViewBlobSize = 8;


/********************************************************************************************

>	void RenderRegion::InitPrefs()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/93
	Returns:	TRUE if the Preference section was created ok
	Purpose:	Initialises the RenderRegions preferences from the preference file. This is
				called once as Camelot starts up

********************************************************************************************/

BOOL RenderRegion::InitPrefs()
{
	Camelot.DeclareSection( _T("Selection Blob Sizes"), 2 );
	Camelot.DeclarePref( _T("Selection Blob Sizes"), _T("Selected Blobs"), &SelectedBlobSize, 1, 30 );
	Camelot.DeclarePref( _T("Selection Blob Sizes"), _T("UnSelected Blobs"), &UnSelectedBlobSize, 1, 30 );
	Camelot.DeclarePref( _T("Selection Blob Sizes"), _T("Multi Stage Selected Blobs"), &MultiStageSelectedBlobSize, 1, 30 );
	Camelot.DeclarePref( _T("Selection Blob Sizes"), _T("Multi Stage UnSelected Blobs"), &MultiStageUnSelectedBlobSize, 1, 30 );

	return TRUE;
}


/********************************************************************************************

>	void RenderRegion::SetMergeTestWait(BOOL OnOff)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/93
	Inputs:		TRUE to stop backgound redraw from occurring (rectangles are still added to
				the list) until set to FALSE again..
	Outputs:	-
  	Returns:	-
	Purpose:	Used by the RectTool to suspend background redrawing.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*
void CCAPI RenderRegion::SetMergeTestWait(BOOL OnOff)
{ 
	MergeTestWait = OnOff;
}

*/

/********************************************************************************************

>	inline INT32 RenderRegion::GetTimeslice()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/94
	Inputs:		-
	Outputs:	-
  	Returns:	Time (in milliseconds) that should be used as a slice during background
				rendering.
	Purpose:	Used by DocView::CanContinue
	Errors:		-

********************************************************************************************/

/********************************************************************************************

>	RenderRegion::RenderRegion()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default Constructor for RenderRegion Class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RenderRegion::RenderRegion()
{
	NumCurrentAttrs = 0;
	CurrentAttrs = NULL;

	// By default try to render everything
	RenderComplexShapes = TRUE;

	RenderFlags.Printing = FALSE;
	TRACEUSER("Gavin",_T("RenderRegion::RenderRegion - RenderFlags.Rendering = FALSE;\n"));
	RenderFlags.Rendering = FALSE;
	RenderFlags.bImmediateRender = FALSE;
	RenderFlags.ValidDevice = FALSE;
	RenderFlags.VeryMono = FALSE;
	RenderFlags.HitDetect = FALSE;
	RenderFlags.SmoothedBitmaps = FALSE;

	// All fill attributes are invalid to start with.
	FillFlags.ValidGeometry 		= FALSE;
	FillFlags.ValidEffect 			= FALSE;
	FillFlags.ValidMapping 			= FALSE;
	FillFlags.ValidTransGeometry 	= FALSE;
	FillFlags.ValidTransEffect 		= FALSE;
	FillFlags.ValidTransMapping 	= FALSE;

	StrokeFlags.ValidStrokeColour	= FALSE;
	StrokeFlags.ValidStrokeTransp	= FALSE;
	StrokeFlags.ValidLineWidth		= FALSE;
	StrokeFlags.ValidJoinType		= FALSE;
	StrokeFlags.ValidDashPattern	= FALSE;
	StrokeFlags.ValidLineCap		= FALSE;
	StrokeFlags.ValidStartArrow		= FALSE;
	StrokeFlags.ValidEndArrow		= FALSE;
	StrokeFlags.ValidMitreLimit		= FALSE;

    NeedsOSPaper = TRUE;

    DrawingMode = DM_COPYPEN;
	CurrentColContext = NULL;

	// Now setup the render state
	CurrentRenderState = NULL;
	CurrentSubRenderState = NULL;
	SubRenderStateLocked = FALSE;

	// Set the Fuzzy Clipping Rects to something
	InnerRect.MakeEmpty();
	OuterRect.MakeEmpty();

	// Set up vars to do with banded rendering
	RegionRect.MakeEmpty();
	IsRegionBanded = FALSE;
	CanRegionBeMerged = TRUE;
	IsWaitingForRAM = FALSE;
	IsLastBand = TRUE;
	IsBeingReset = FALSE;
	IsInkRenderStarted = FALSE;
	IsPaperRendered = FALSE;

	PathProcessorStack = NULL;

	// MarkH 23/7/99 - New Flag to wether or not to use the GDraw BiCompression Field!
	m_DoCompression = FALSE;

	m_pFilter = NULL;

	m_pBackmostChangedNode = NULL;
	m_timeRenderTreeStartTime.Sample();
	m_bRenderTreeTimeSlice = FALSE;
	m_countTotal = 0;
	m_countStored = 0;

	RenderDC = NULL;
	m_fOwned = FALSE;
	ScaleFactor = FIXED16(1);

	m_bForceMix = FALSE;

//	double m_dboostm = 1.0;
//	double m_dboostc = 0.0;

#ifdef _DEBUG
	m_CurrentContextLevel = 0;
	m_TraceOutContextLevels = FALSE;
//	m_TraceOutContextLevels = TRUE;
#endif
}

/********************************************************************************************

>	RenderRegion::RenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Inputs:		ClipRect is a DocRect defining the invalid rectangle to be rendered.
				ConvertMatrix is a Matrix for converting Doc coords to OS coords.
				ViewScale is the scale factor of the view, used to calculate how much to
				flatten paths.
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a RenderRegion, allowing a invalid rectangle to be specified.
				The matrix passed is used to convert Document coords into device coords,
				taking into account scaling, scroll offsets etc.
	Errors:		-
	SeeAlso:	OSRenderRegion::OSRenderRegion()

********************************************************************************************/

RenderRegion::RenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale)
{
	NumCurrentAttrs = 0;
	CurrentAttrs = NULL;

	// By default try to render everything
	RenderComplexShapes = TRUE;

	CurrentClipRect = ClipRect;

	RenderMatrix = ConvertMatrix;

//	TRACE( _T("RR ClipRect = (%d, %d) - (%d, %d)\n"), ClipRect.lo.x, ClipRect.lo.y, ClipRect.hi.x, ClipRect.hi.y);
//	RenderMatrix.Dump();

	ScaleFactor = ViewScale;

	RenderView = NULL;
	RenderDC = NULL;
	m_fOwned = FALSE;

	RenderFlags.Printing = FALSE;
	TRACEUSER("Gavin",_T("RenderRegion::RenderRegion - RenderFlags.Rendering = FALSE;\n"));
	RenderFlags.Rendering = FALSE;
	RenderFlags.bImmediateRender = FALSE;
	RenderFlags.ValidDevice = FALSE;
	RenderFlags.VeryMono = FALSE;
	RenderFlags.HitDetect = FALSE;
	RenderFlags.SmoothedBitmaps = FALSE;

	DrawingMode = DM_COPYPEN;

	CurrentColContext = NULL;

	PixelWidth = 1;
	ScaledPixelWidth = 1;

	// Now setup the render state
	CurrentRenderState = NULL;
	CurrentSubRenderState = NULL;
	SubRenderStateLocked = FALSE;

    NeedsOSPaper = TRUE;

	// No paper has been rendered yet
	IsPaperRendered = FALSE;
	IsInkRenderStarted = FALSE;

	Timeslice = 100;											// 0.1 of a second

	// Set the Fuzzy Clipping Rects to something
	InnerRect.MakeEmpty();
	OuterRect.MakeEmpty();

	// Set up vars to do with banded rendering
	RegionRect = ClipRect;
	IsRegionBanded = FALSE;
	CanRegionBeMerged = TRUE;
	IsWaitingForRAM = FALSE;
	IsLastBand = TRUE;
	// Ilan
	IsBeingReset = FALSE;

	PathProcessorStack = NULL;

	m_pFilter = NULL;

	m_pBackmostChangedNode = NULL;
	m_timeRenderTreeStartTime.Sample();
	m_bRenderTreeTimeSlice = FALSE;
	m_countTotal = 0;
	m_countStored = 0;

	m_bForceMix = FALSE;

#ifdef _DEBUG
	m_CurrentContextLevel = 0;
	m_TraceOutContextLevels = FALSE;
//	m_TraceOutContextLevels = TRUE;
#endif
}

/********************************************************************************************

>	RenderRegion::RenderRegion(const RenderRegion &other)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/93
	Inputs:		The RenderRegion to copy
	Outputs:	-
	Returns:	-
	Purpose:	Copy Constructor for RenderRegion.  This conctructs a RenderRegion based
				on another.  This involves copying all the current attributes and also
				making a copy of the ContextStack.
	Errors:		-
	SeeAlso:    -

********************************************************************************************/

RenderRegion::RenderRegion(const RenderRegion &other)
{
	PathProcessorStack = NULL;		// We don't copy the PPStack - that's done as attrs are copied

	CurrentClipRect = other.CurrentClipRect;

	RenderComplexShapes = other.RenderComplexShapes;

	RenderMatrix = other.RenderMatrix;
	ScaleFactor = other.ScaleFactor;

	RenderView = other.RenderView;
	RenderDC = other.RenderDC;
	m_fOwned = other.m_fOwned;
//	DCHandle = other.DCHandle;

	RenderFlags = other.RenderFlags;

	RenderSpread = other.RenderSpread;
	CurrentColContext = other.CurrentColContext;

	PixelWidth = other.PixelWidth;
	ScaledPixelWidth = other.ScaledPixelWidth;

	Timeslice = other.Timeslice;
	RRQuality = other.RRQuality;

    NeedsOSPaper = other.NeedsOSPaper;

	// No paper has been rendered yet
	IsPaperRendered = other.IsPaperRendered;
	IsInkRenderStarted = other.IsInkRenderStarted;


	// Now setup the render state 
	CurrentRenderState = other.CurrentRenderState;
//	CurrentSubRenderState = other.CurrentSubRenderState;
	CurrentSubRenderState = NULL;
	SubRenderStateLocked = FALSE;

	// Copy the current attributes
	NumCurrentAttrs = 0;		// We're constructing, so we have no current attrs yet
	CurrentAttrs = NULL;
	if (!CopyCurrentAttributes(other))
	{
		// Unable to copy the current attributes
		ENSURE(FALSE, "Unable to copy current attributes");
		InformError();
		return;
	}
	
	// Now make a copy of the ContextSatck
	if (!TheStack.Copy(&other.TheStack, this))
	{
		// Unable to copy the render stack
		ENSURE(FALSE, "Unable to copy Rendering stack");
		InformError();
		return;
	}

	DrawingMode = other.DrawingMode;

	// Set the Fuzzy Clipping Rects to something
	InnerRect = other.InnerRect;
	OuterRect = other.OuterRect;

	// Set up vars to do with banded rendering
	RegionRect = other.RegionRect;
	IsRegionBanded = other.IsRegionBanded;
	CanRegionBeMerged = other.CanRegionBeMerged;
	IsWaitingForRAM = other.IsWaitingForRAM;
	IsLastBand = other.IsLastBand;
	// Ilan
	IsBeingReset = FALSE;

	m_pFilter = other.m_pFilter;

	m_pBackmostChangedNode = other.m_pBackmostChangedNode;
	m_timeRenderTreeStartTime = other.m_timeRenderTreeStartTime;
	m_bRenderTreeTimeSlice = other.m_bRenderTreeTimeSlice;
	m_countTotal = other.m_countTotal;
	m_countStored = other.m_countStored;

	m_bForceMix = other.m_bForceMix;

#ifdef _DEBUG
	m_CurrentContextLevel = 0;
	m_TraceOutContextLevels = FALSE;
//	m_TraceOutContextLevels = TRUE;
#endif
}


/********************************************************************************************

>	BOOL RenderRegion::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/05/94
	Returns:	TRUE if initialised ok;
				FALSE if not.
	Purpose:	Initialise the render region.

********************************************************************************************/

BOOL RenderRegion::Init()
{
	return TRUE;
}

/********************************************************************************************

>	RenderRegion::~RenderRegion()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default Destructor for RenderRegion Class.
				Releases any memory claimed for the ContextStack.
	Errors:		-
	SeeAlso:	OSRenderRegion::~OSRenderRegion()

********************************************************************************************/

RenderRegion::~RenderRegion()
{
	// Ilan
	IsBeingReset = TRUE;

	// Get rid of any stacked Captures
	Capture* pCapture = GetTopCapture();
	while (pCapture)
	{
		ENSURE(!pCapture->OwnsBitmap(), "Capture unexpectedly owns bitmap in RenderRegion::ResetRegion");

		m_CaptureStack.Pop();
//TRACEUSER( "Phil", _T("~RenderRegion Pop/Delete Capture\n"));
		if (pCapture->IsMaster())
		{
			SetRenderToCapture(pCapture, FALSE, FALSE);
		}
		else
		{
			pCapture->FreeDIB();
		}
		delete pCapture;

		pCapture = GetTopCapture();
	}


	if( m_fOwned )
	{
		delete RenderDC;
		RenderDC = NULL;
	}

// Webster - markn 15/8/97
// This has been taken out of Webster builds as it can cause fatal access violations with Paper render regions.
// It is new path processor code that not's totally bug-free, and as path processors are not used in Webster,
// it can be safely taken out.
// Now taken out via new VECTOR_STROKING flag Neville 2/10/97 
#ifdef VECTOR_STROKING
	// Ask the RenderStack to pop all attributes off itself. This gives them a chance
	// to clean up properly (remove PathProcessors etc).
	TheStack.CleanUpBeforeDestruct(this);
#endif // VECTOR_STROKING

	// Nearly always non-NULL, but e.g. PaperRenderRegions will have deleted
	// their attributes before we get here so we check (and hence don't GPF!)
	if (CurrentAttrs != NULL)
	{
		for (INT32 i = 0; i < NumCurrentAttrs; i++)
		{

// WEBSTER - markn 16/7/97
// This hides a bug in the text tool, which won't go off if you don't use Path processors
// The bug - 
//		create a multi-line paragraph text object
//		Select all and apply -5 base line shift
//		Change to a large font
//		Change the font
//		Change the text size
//		Should go bang
//
// I have no idea what the problem is (I'd fix it if I knew), and don't have time to spend on it.
// Should be looked into though...
// Now taken out via new VECTOR_STROKING flag Neville 2/10/97 
#ifdef VECTOR_STROKING
			// Tell the attribute that it's being chucked out of scope
		//	CurrentAttrs[i].pAttr->GoingOutOfScope(this);
#endif // VECTOR_STROKING

			// And delete it if it's a temporary attr
			if (CurrentAttrs[i].Temp)
			{
				CC_ASSERT_VALID(CurrentAttrs[i].pAttr);
				delete CurrentAttrs[i].pAttr;
			}
		}

		// We've finished with this array of attributes now
		CCFree(CurrentAttrs);
	}

	if (CurrentSubRenderState != NULL)
		delete CurrentSubRenderState;

	if (PathProcessorStack != NULL)
	{
//		ERROR3("RenderRegion destructed while PathProcessors still active");
		while (PathProcessorStack != NULL)
			PopPathProcessor();
	}
}




/********************************************************************************************

>	BOOL CopyRenderInfo( const RenderRegion &Other)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/94
	Inputs:		Other - The render region to get the info out of
	Returns:	FALSE if failed, TRUE if worked.
	Purpose:	Copies all the parameters from the render region 'Other' into this render
				region. This was written for the region merging code. When a new region
				is created, due to existing regions being split up, it should be based on
				the original region, with a different clipping rectangle

********************************************************************************************/

BOOL RenderRegion::CopyRenderInfo( const RenderRegion &Other)
{
	// We're writing over our render info with the other RndRgn's settings, so we must vape
	// all of our PathProcessors (if there are any, which probably there shouldn't be)
	while (PathProcessorStack != NULL)
		PopPathProcessor();

	// Copy params to do with the rendering
	CurrentClipRect = Other.CurrentClipRect;
	RenderMatrix 	= Other.RenderMatrix;
	ScaleFactor 	= Other.ScaleFactor;
	RenderSpread 	= Other.RenderSpread;
	DrawingMode 	= Other.DrawingMode;
	CurrentColContext = Other.CurrentColContext;
	RenderComplexShapes = Other.RenderComplexShapes;

	// Copy params to do with the window we are rendering into
	RenderView		= Other.RenderView;
	RenderDC 		= Other.RenderDC;
//	DCHandle 		= Other.DCHandle;

	// Copy the flags
	RenderFlags 	= Other.RenderFlags;

	// Copy the pixel width info
	PixelWidth 		= Other.PixelWidth;
	ScaledPixelWidth = Other.ScaledPixelWidth;

	// Copy the rendered state
	IsPaperRendered = Other.IsPaperRendered;
	IsInkRenderStarted = Other.IsInkRenderStarted;
    NeedsOSPaper = Other.NeedsOSPaper;
	Timeslice = Other.Timeslice;
	RRQuality = Other.RRQuality;

	// Copy the node to render
	CurrentRenderState = Other.CurrentRenderState;
	CurrentSubRenderState = NULL;
	SubRenderStateLocked = FALSE;

	// Copy the current attributes
	if (!CopyCurrentAttributes(Other))
	{
		// Unable to copy the current attributes
		ENSURE(FALSE, "Unable to copy current attributes");
		InformError();
		return FALSE;
	}
	

	// Now make a copy of the ContextSatck
	// Actually don't, or you get some attributes left from this render region at the bottom
/*	if (!TheStack.Copy(&Other.TheStack, this))
	{
		// Unable to copy the render stack
		ENSURE(FALSE, "Unable to copy Rendering stack");
		InformError();
		return FALSE;
	}
*/
	DrawingMode = Other.DrawingMode;

	// Set the Fuzzy Clipping Rects to something
	InnerRect = Other.InnerRect;
	OuterRect = Other.OuterRect;

	// Set up vars to do with banded rendering
	RegionRect = Other.RegionRect;
	IsRegionBanded = Other.IsRegionBanded;
	CanRegionBeMerged = Other.CanRegionBeMerged;
	IsLastBand = Other.IsLastBand;

	// this is critical to make sure InitDevice gets called on the next StartRender
	RenderFlags.ValidDevice = FALSE;

	return TRUE;
}






/********************************************************************************************

>	BOOL RenderRegion::AttachDevice(View* ViewToAttach, wxDC* DCToAttach,
											Spread* SpreadToAttach)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Inputs:		A View ptr ,wxDC ptr and Spread ptr to attach to the RenderRegion.
				The View pointer must not be NULL!
	Returns:	TRUE if attached successfully;
				FALSE if not.
	Purpose:	Attaches View, wxDC and Spread pointers to the RenderRegion.
				The View is used to for general rendering purposes.
				The wxDC is the device that we are rendering to.  We extract the HDC from this
				and store it so we can recreate another wxDC object for background rendering.
				The Spread is just stored for external referance and is used to continue
				rendering during BackGnd redraw.

				NB. This function should be called before doing anything else in the 
				AttachDevice() function of a derived class.

	Errors:		If ViewToAttach is NULL => ERROR2
	SeeAlso:	RenderRegion::InitDevice

********************************************************************************************/

BOOL RenderRegion::AttachDevice(View* ViewToAttach, CCDC* DCToAttach,
								Spread* SpreadToAttach, bool fOwned /*= false*/)
{
	return AttachDevice(ViewToAttach, DCToAttach->GetDC(), SpreadToAttach, fOwned);
}
#include "prncamvw.h"

BOOL RenderRegion::AttachDevice(View* ViewToAttach, wxDC* DCToAttach,
								Spread* SpreadToAttach, bool fOwned /*= false*/)
{
	RenderSpread = SpreadToAttach;
	RenderView	 = ViewToAttach;
	RenderDC	 = DCToAttach;
	m_fOwned	 = fOwned;

	ERROR2IF(ViewToAttach == NULL, FALSE, "NULL View passed to RenderRegion::AttachDevice()!");

	// Find which view the render region is to be rendering to
	if (RenderView != NULL)
	{
		// We have a render window - calculate the pixel sizes.
		PixelWidth = CalcPixelWidth();
		ScaledPixelWidth = CalcScaledPixelWidth();
	}
	else if (IS_KIND_OF(OSRenderRegion))
	{
		// Only do this for OSRenderRegions because they can be used to
		// render into dialogs (GRenderRegions have their own system
		// of pixel size).
		PixelWidth = CalcPixelWidth();
		ScaledPixelWidth = CalcScaledPixelWidth();
	}

	// No ink rendering yet.
	CurrentRenderState = NULL;
	CurrentSubRenderState = NULL;
	SubRenderStateLocked = FALSE;

	// Initialise drawing mode.
	DrawingMode = DM_COPYPEN;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL RenderRegion::InitDevice()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/05/94
	Returns:	TRUE if the device specific initialisation is completed successfully;
				FALSE if not.
	Purpose:	Initialises the render region's connection to the device information
				passed in to RenderRegion::AttachDevice.
	Errors:		Out of memory, Internal error
	SeeAlso:	RenderRegion::AttachDevice

********************************************************************************************/

BOOL RenderRegion::InitDevice()
{
	// Find this documents default colour context for the screen (RGB)
	if (RenderView != NULL)
	{
		// Find a colour context for the View we're rendering. This will automatically find
		// a suitable local, document, or global context for us to use
		CurrentColContext = RenderView->GetColourContext(COLOURMODEL_RGBT);
		if (CurrentColContext == NULL)	// should never be a problem!
		{
			DiagnosticFn( _T("InitDevice:"), _T("No colour context") );
			ERROR(_R(IDT_INTERNAL_ERROR), FALSE);
		}
	}
	else
	{
		ERROR3("View is NULL in RenderRegion::InitDevice()");
		return FALSE;
	}

	// If this region has been reset, then we may already have
	// a CurrentAttrs array that needs to be de-allocated.
	if (CurrentAttrs != NULL)
	{
		// Release any temporary objects used by the 'current attribute' array.
		for (INT32 i = 0; i < NumCurrentAttrs; i++)
		{
			if (CurrentAttrs[i].Temp)
			{
				CC_ASSERT_VALID(CurrentAttrs[i].pAttr);
				delete CurrentAttrs[i].pAttr;
			}
		}

		// We've finished with this array of attributes now
		CCFree(CurrentAttrs);
		NumCurrentAttrs = 0;
	}

	// Set up the attributes for this device.
	CurrentAttrs = AttributeManager::GetDefaultAttributes();

	if (CurrentAttrs == NULL)
	{
		NumCurrentAttrs = 0;
		return FALSE;
	}

	// Find out how many attributes there are.
	NumCurrentAttrs = AttributeManager::GetNumAttributes();

	// Render all non-NULL attributes.
	for (INT32 i = 0; i < NumCurrentAttrs; i++)
	{
		if (CurrentAttrs[i].pAttr != NULL && CurrentAttrs[i].pAttr->CanBeRenderedDirectly())
			CurrentAttrs[i].pAttr->Render(this);
	}

	// Ilan
	// Initialise the current offscreen pointer with a placeholder
	// this ensures that there is a default value which can be shifted to TheStack
	// so that the Restore() function gets called to to signal when the offscreen
	// bitmap rendering process has completed
	// NB ATTR_FEATHER used, but any offscreen attr with a conventional Restore() function would do
	// NB2 don't need to free mem as it is a pointer to the default val
	CurrentOffscreenAttr = (OffscreenAttrValue*) AttributeManager::GetDefaultAttributeVal(ATTR_FEATHER);

	// Override the quality attribute according to the View's setting.
	// Don't bother if there is no DocView - e.g. it's a render region that is used
	// to export a file.
	if (RenderView != NULL)
	{
		// Create a new quality attribute and install it.
		QualityAttribute *pAttr = new QualityAttribute(RenderView->RenderQuality);
		ENSURE(pAttr != NULL, "No memory for quality attribute to be created");
		if (pAttr == NULL)
			return FALSE;

		// TRUE => Temp; delete this attribute when it is popped off the stack.
		SetQuality(pAttr, TRUE);
	}

	// Everything completed ok
	return TRUE;
}

BOOL RenderRegion::ResetColourContext()
{
	// Find this documents default colour context for the screen (RGB)
	if (RenderView != NULL)
	{
		// Find a colour context for the View we're rendering. This will automatically find
		// a suitable local, document, or global context for us to use
		CurrentColContext = RenderView->GetColourContext(COLOURMODEL_RGBT);
		if (CurrentColContext == NULL)	// should never be a problem!
		{
			DiagnosticFn( _T("InitDevice:"), _T("No colour context") );
			ERROR(_R(IDT_INTERNAL_ERROR), FALSE);
		}
	}
	else
	{
		ERROR3("View is NULL in RenderRegion::InitDevice()");
		return FALSE;
	}

	return(TRUE);
}

ColourContext* RenderRegion::GetColourContext()
{
	return(CurrentColContext);
}


/********************************************************************************************

>	Node* RenderRegion::GetRenderState()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/93
	Inputs:		-
	Outputs:	-
	Returns:	A Ptr to a Node (or NULL if idle).
	Purpose:	Returns the Current Render State of the RenderRegion, which is a pointer
				to the next Node to render. If NULL then it is not currently rendering.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Node* RenderRegion::GetRenderState()
{
	return CurrentRenderState;
}

/********************************************************************************************

>	BOOL RenderRegion::ImmediateRender()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if current clip rect is within view (ie whether to render or not).
	Purpose:	Used to Draw something immediately.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL RenderRegion::ImmediateRender()
{
	RenderFlags.bImmediateRender = TRUE;
	if (!StartRender())
		return FALSE;

	// Immediate render regions default to 0 line-width because they usually want to
	// render drag blobs etc which should be only one pixel wide.
	SetLineWidth(0);

	// Immediate render is usually used for blob rendering
	// normally we use the default quality, but we mustn't do that on hit-detection bitmaps
	// else they render outlined shapes as solid (See GRenderClick classes)
	SetDefaultQuality();

	//SetStartCap(LineCapButt);
	//SetJoinType(RoundJoin);

	return TRUE;
}

void RenderRegion::SaveContext()
{
	TheStack.SaveContext();

#ifdef _DEBUG
	m_CurrentContextLevel++;
#endif
}

void RenderRegion::RestoreContext()
{
	TheStack.RestoreContext(this);

#ifdef _DEBUG
	m_CurrentContextLevel--;
#endif
}

/********************************************************************************************

>	void RenderRegion::DefaultRender(BOOL bForceImmediate = FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Calls the attatched view to render itself.  This is used to continue
				rendering into a suspended RenderRegion.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void RenderRegion::DefaultRender(BOOL bForceImmediate /*= FALSE*/)
{
	ENSURE(RenderView != NULL,"RenderRegion::DefaultRender called with invalid RenderView");

	if (RenderView)
	{
		RenderView->AllocateDC();
		RenderDC = RenderView->GetRenderDC();
		if (RenderDC)
		{
			View * pView = RenderView; // as ContinueRenderView can delete the RenderRegion (i.e. "this").
   			RenderView->SetCurrent();
			(RenderView->GetDoc())->SetCurrent();
			RenderView->ContinueRenderView(this, RenderSpread, TRUE, TRUE, bForceImmediate);

			// This MUST pair with AllocateDC
			pView->DoneWithDC(); // hint that we might like to drop this DC so a fresh one will be created
		}
		else
		{
			// This error causes a recursive error condition so I'll just TRACE for now
//			Error::SetError(_R(IDT_INTERNAL_ERROR), 0);
//			InformError();
			TRACE(_T("Failed to get a DC for rendering in RenderRegion::DefaultRender\n"));
		}
	}
}


/********************************************************************************************

>	BOOL RenderRegion::StartRender()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/05/94
	Purpose:	Initialises the device attachment, if we haven't already done so for this
				render region.  This is so that we can have a two-stage attachment of a
				device to a render region.   This allows us to create render regions
				very quickly, which makes background rendering more efficient as quite
				often we will just throw them away without ever needing a proper connection
				to the device context.
	SeeAlso:	RenderRegion::AttachDevice; RenderRegion::InitDevice

********************************************************************************************/

BOOL RenderRegion::StartRender()
{
	// If the device has not been Inited, then do it now
	if (!RenderFlags.ValidDevice)
	{
		// Device needs to be initialised
		if (!InitDevice())
		{
			// Something's gone wrong - inform the user.
			//InformSeriousError();
			TRACE( _T("InitDevice failed in Render Region (Could be not enough mem for Gdraw bitmap)"));
			return FALSE;
		}

		// Make sure we don't do this again for this render region
		RenderFlags.ValidDevice = TRUE;
	}

	// Set up the Fuzzy Clipping Rects
	// Inner rect is used to clip with, so we need to make it bigger than the region so that
	// things like thick lines do not show up
	InnerRect = CurrentClipRect;
	INT32 ScaleBy = 1000*ScaledPixelWidth;
	InnerRect.Inflate(ScaleBy, ScaleBy);

	// The outer rect is used to determine if clipping is needed, so make it
	// a few times bigger than the inner rect
	OuterRect = InnerRect;
	OuterRect.Inflate(InnerRect.Width()*3, InnerRect.Height()*3);

	// Create the default, master capture...
	Capture* pNewMasterCapture = NULL;

	if (m_CaptureStack.Empty())
	{
		pNewMasterCapture = new Capture(this, CAPTUREINFO(ctNESTABLE, cfMASTER | cfPIXWIDTHSCALE), RenderMatrix, CurrentClipRect, GetPixelsPerInch(), CurrentColContext);	// NOTE! Use of ScaledPixelWidth in GetPixelsPerInch in RenderRegion!
		if (pNewMasterCapture==NULL) return FALSE;

		m_CaptureStack.Push(pNewMasterCapture);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL RenderRegion::CloseDown ()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/02/00
	Inputs:		None.
	Returns:	TRUE.
	Purpose:	Base class declaration of the CloseDown function. It allows the CloseDown
				functions in the EPS and CMX filters to be virtual over-rides, which means
				that I don't need the IsKindOfs cluttering up the rendering loops.
	SeeAlso:	RenderRegion::AttachDevice; RenderRegion::InitDevice

********************************************************************************************/

BOOL RenderRegion::CloseDown ()
{
	return TRUE;
}

/********************************************************************************************

>	virtual void Region::ConditionalSuicide ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/00
	Inputs:		-
	Returns:	-
	Purpose:	Causes the object to commit suicide. This is to get around using a few
				if IS_A calls elsewhere in Camelot.

********************************************************************************************/

void RenderRegion::ConditionalSuicide ( void )
{
	// Yes, this is a legal C++ expression. Just don't attempt to call this object again.
	delete this;
}

/********************************************************************************************

>	virtual void RenderRegion::SetClean(BOOL bResetChangedBounds, BOOL FillWhite)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/06/94
	Inputs:		FillWhite is an optional parameter which lets the region fill itself with
				white. This is used during hit-testing.
	Purpose:	Virtual function used to inform render region that defers update that it
				should set its render state to be 'clean', i.e. if no more rendering occurs
				then it need not update the device.
				This is used for render regions such as GRenderRegions, where we render
				the paper using the host OS graphics primitives, but we have to render
				the paper into the GRenderRegion too, otherwise we will get garbage.
				Equally though, we don't want the GRenderRegion to do a potentially slow
				blit if all it is going to draw is some paper.  It also allows the 'changed
				box' to be reset, so if we only have a small object on the page, then only
				the area containing the object is updated using a blit.
				The default implementation of this function does nothing - most render
				regions won't need to over-ride this null behaviour.
	SeeAlso:	GRenderRegion::SetClean

********************************************************************************************/

void RenderRegion::SetClean(BOOL bResetChangedBounds, BOOL FillWhite)
{
}

/********************************************************************************************

>	void RenderRegion::SetMatrix( Matrix& NewMatrix )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/93
	Inputs:		NewMatrix - The matrix that will replace the old RenderMatrix
	Purpose:	This can be used to set the RenderRegions transform matrix
				to the given matrix. This should really be used with care!
	SeeAlso:	RenderRegion::ConcatenateMatrix

********************************************************************************************/

void RenderRegion::SetMatrix( Matrix& NewMatrix )
{
	// Set the Render Matrix to the one provided
	RenderMatrix = NewMatrix;
	if (GetMasterCapture())
	{
		GetMasterCapture()->CaptureMatrix = NewMatrix;
	}
}

/********************************************************************************************

>	Matrix RenderRegion::ConcatenateMatrix( Matrix& NewMatrix )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/93
	Inputs:		NewMatrix - The Matrix that will be combined with the current
				RenderMatrix in the RenderRegion
	Returns:	The old RenderRegion Matrix
	Purpose:	This can be used to add a transform in to the render regions
				transformation matrix. The function performs the following
				matrix operation :- MonoOn
				RenderMatrix = NewMatrix * RenderMatrix; MonoOff
	SeeAlso:	RenderRegion::SetMatrix

********************************************************************************************/

Matrix RenderRegion::ConcatenateMatrix( Matrix& NewMatrix )
{
	// Need to remember the old matrix so we can return it
	Matrix Temp = RenderMatrix;

	// Add a transform to the matrix we already have
	RenderMatrix = NewMatrix * RenderMatrix;

	// return the old matrix to the caller
	return Temp;
}






/********************************************************************************************

>	void RenderRegion::SetClipRect(DocRect &NewClipRect, BOOL ReCalcContextNode = FALSE);

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/93
	Inputs:		A DocRect describing the tightest bounding box around the clipping region.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the Clipping rectangle of the Render Region.
				Calls InitClipping() to acually set the clip region in an OS specific way.
	Errors:		-
	SeeAlso:	RenderRegion::GetClipRect(); RenderRegion::RenderRegion()

********************************************************************************************/

void RenderRegion::SetClipRect(DocRect &NewClipRect, BOOL ReCalcContextNode)
{
	ENSURE(NewClipRect.IsValid(),"RenderRegion::SetClipRect called with Invalid Rectangle");
	CurrentClipRect = NewClipRect;

ENSURE(GetMasterCapture()==NULL || MasterCaptureIsCurrent(), "SetClipRect being called while child Captures are running\n");
if (GetMasterCapture()!=NULL)
{
	if (MasterCaptureIsCurrent())
	{
		GetMasterCapture()->SetClipRect(NewClipRect);
	}
}

    if (RenderFlags.Rendering)
    {
		// Do some OS specific initialisation of the clipping region
		InitClipping();	// This is implemented in the Derived classes (OSRenderRegion etc)
	}

	// if we need to recalculate the regions context node then go ahead
	if (ReCalcContextNode==TRUE)
	{
//	 	CurrentRenderState = RenderSpread->FindFirstForClippedInkRender(&CurrentClipRect, this);
	 	CurrentRenderState = RenderSpread;	// For use with RenderTree

		// Don't need Node sub context anymore
		delete CurrentSubRenderState;
		CurrentSubRenderState = NULL;
		SubRenderStateLocked = FALSE;
	}
}

/********************************************************************************************

>	DocRect RenderRegion::GetClipRect()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Inputs:		-
	Outputs:	-
	Returns:	A DocRect describing the tightest bounding box around the clipping region
	Purpose:	Returns the clipping rectangle of the Render Region
	Errors:		-
	SeeAlso:	RenderRegion::SetClipRect(); RenderRegion::RenderRegion() 

********************************************************************************************/

DocRect RenderRegion::GetClipRect()
{
	return CurrentClipRect;
}
/********************************************************************************************

>	void RenderRegion::SetFillAttributes(ChangeAttrType Type = CHANGEATTR_ALL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		Type - the type of the fill attribute that has changed, can be:

					CHANGEATTR_ALL
					CHANGEATTR_GEOMETRY
					CHANGEATTR_EFFECT
					CHANGEATTR_MAPPING
					CHANGEATTR_TRANSP_GEOMETRY
					CHANGEATTR_TRANSP_EFFECT
					CHANGEATTR_TRANSP_MAPPING
					CHANGEATTR_WINDING_RULE

	Purpose:	Inform the render region that an aspect of the path filling attributes
				has changed.  The base class version just updates the RenderRegion
				member variable FillFlags to reflect which aspects of the fill attributes
				have become invalid.  Derived render regions can either call the base class
				or just implement whatever methods they need for keeping track of these
				changes - or a combination of the two, of course.
				Some render regions (e.g. GRenderRegions) don't need to do anything about
				this at all as all attributes are invoked whenever anything is rendered.
				If this is the case, then this should be over-ridden to have a NULL 
				implementation because this will improve rendering speed.
	SeeAlso:	ChangeAttrType

********************************************************************************************/

void RenderRegion::SetTextAttributes(ChangeFillAttrType Type)
{
	switch (Type)
	{
		case CHANGEFILLATTR_ALL:
			TextFlags.ValidFont			= FALSE;
			TextFlags.ValidBold			= FALSE;
			TextFlags.ValidItalic		= FALSE;
			TextFlags.ValidUnderline	= FALSE;
			TextFlags.ValidAspect		= FALSE;
			TextFlags.ValidTracking		= FALSE;
			TextFlags.ValidFontSize		= FALSE;
			TextFlags.ValidJustify		= FALSE;
			TextFlags.ValidScript		= FALSE; 
			TextFlags.ValidBaseLine		= FALSE;
			TextFlags.ValidLineSpace	= FALSE;
			TextFlags.ValidLeftMargin   = FALSE;
			TextFlags.ValidRightMargin  = FALSE;
			TextFlags.ValidFirstIndent  = FALSE;
			TextFlags.ValidRuler        = FALSE;
			break;

		case CHANGEATTR_FONT:
			TextFlags.ValidFont			= FALSE;
			break;

		case CHANGEATTR_BOLD:
			TextFlags.ValidBold			= FALSE;
			break;

		case CHANGEATTR_ITALIC:
			TextFlags.ValidItalic		= FALSE;
			break;

		case CHANGEATTR_UNDERLINE:
			TextFlags.ValidUnderline	= FALSE;
			break;

		case CHANGEATTR_ASPECT:
			TextFlags.ValidAspect		= FALSE;
			break;

		case CHANGEATTR_TRACKING:
			TextFlags.ValidTracking		= FALSE;
			break;

		case CHANGEATTR_SIZE:
			TextFlags.ValidFontSize		= FALSE;
			break;
		case CHANGEATTR_JUSTIFY:
			TextFlags.ValidJustify		= FALSE;
			break;
		case CHANGEATTR_SCRIPT:
			TextFlags.ValidScript		= FALSE;
			break;
		case CHANGEATTR_BASELINE:
			TextFlags.ValidBaseLine		= FALSE;
			break;
		case CHANGEATTR_LINESPACE:
			TextFlags.ValidLineSpace	= FALSE;
			break;
		case CHANGEATTR_LEFTMARGIN:
			TextFlags.ValidLeftMargin   = FALSE;
			break;
		case CHANGEATTR_RIGHTMARGIN:
			TextFlags.ValidRightMargin  = FALSE;
			break;
		case CHANGEATTR_FIRSTINDENT:
			TextFlags.ValidFirstIndent  = FALSE;
			break;
		case CHANGEATTR_RULER:
			TextFlags.ValidRuler        = FALSE;
			break;

		default:
			ERROR3_PF(("Invalid attribute type (%d) in SetFillAttributes", Type));
			break;
	}
}

/********************************************************************************************

>	void RenderRegion::SetLineAttributes(ChangeAttrType Type = CHANGEATTR_ALL)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/95
	Inputs:		Type - the type of the fill attribute that has changed, can be:

				CHANGEATTR_ALL
				CHANGEATTR_STROKECOL
				CHANGEATTR_STROKETRANSP
				CHANGEATTR_LINEWIDTH
				CHANGEATTR_JOINTYPE
				CHANGEATTR_DASHPATTERN
				CHANGEATTR_LINECAP
				CHANGEATTR_STARTARROW
				CHANGEATTR_ENDARROW
				CHANGEATTR_MITRELIMIT

	Purpose:	Inform the render region that an aspect of the path stroking attributes
				has changed.  The base class version just updates the RenderRegion
				member variable StrokeFlags to reflect which aspects of the stroke attributes
				have become invalid.  Derived render regions can either call the base class
				or just implement whatever methods they need for keeping track of these
				changes - or a combination of the two, of course.
				Some render regions (e.g. GRenderRegions) don't need to do anything about
				this at all as all attributes are invoked whenever anything is rendered.
				If this is the case, then this should be over-ridden to have a NULL 
				implementation because this will improve rendering speed.
	SeeAlso:	ChangeLineAttrType

********************************************************************************************/

void RenderRegion::SetLineAttributes(ChangeLineAttrType Type)
{
	switch (Type)
	{
		case CHANGELINEATTR_ALL:
			StrokeFlags.ValidStrokeColour	= FALSE;
			StrokeFlags.ValidStrokeTransp	= FALSE;
			StrokeFlags.ValidLineWidth		= FALSE;
			StrokeFlags.ValidJoinType		= FALSE;
			StrokeFlags.ValidDashPattern	= FALSE;
			StrokeFlags.ValidLineCap		= FALSE;
			StrokeFlags.ValidStartArrow		= FALSE;
			StrokeFlags.ValidEndArrow		= FALSE;
			StrokeFlags.ValidMitreLimit		= FALSE;
			break;

		case CHANGEATTR_STROKECOL:
			StrokeFlags.ValidStrokeColour	= FALSE;
			break;

		case CHANGEATTR_STROKETRANSP:
			StrokeFlags.ValidStrokeTransp	= FALSE;
			break;

		case CHANGEATTR_LINEWIDTH:
			StrokeFlags.ValidLineWidth		= FALSE;
			break;

		case CHANGEATTR_JOINTYPE:
			StrokeFlags.ValidJoinType		= FALSE;
			break;

		case CHANGEATTR_DASHPATTERN:
			StrokeFlags.ValidDashPattern	= FALSE;
			break;

		case CHANGEATTR_LINECAP:
			StrokeFlags.ValidLineCap		= FALSE;
			break;

		case CHANGEATTR_STARTARROW:
			StrokeFlags.ValidStartArrow		= FALSE;
			break;

		case CHANGEATTR_ENDARROW:
			StrokeFlags.ValidEndArrow		= FALSE;
			break;

		case CHANGEATTR_MITRELIMIT:
			StrokeFlags.ValidMitreLimit		= FALSE;
			break;

		default:
			ERROR3_PF(("Invalid attribute type (%d) in SetLineAttributes", Type));
			break;
	}
}

/********************************************************************************************

>	void RenderRegion::SetFillAttributes(ChangeAttrType Type = CHANGEATTR_ALL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		Type - the type of the fill attribute that has changed, can be:

					CHANGEATTR_ALL
					CHANGEATTR_GEOMETRY
					CHANGEATTR_EFFECT
					CHANGEATTR_MAPPING

					CHANGEATTR_TRANSP_GEOMETRY
					CHANGEATTR_TRANSP_EFFECT
					CHANGEATTR_TRANSP_MAPPING
					CHANGEATTR_WINDING_RULE

	Purpose:	Inform the render region that an aspect of the path filling attributes
				has changed.  The base class version just updates the RenderRegion
				member variable FillFlags to reflect which aspects of the fill attributes
				have become invalid.  Derived render regions can either call the base class
				or just implement whatever methods they need for keeping track of these
				changes - or a combination of the two, of course.
				Some render regions (e.g. GRenderRegions) don't need to do anything about
				this at all as all attributes are invoked whenever anything is rendered.
				If this is the case, then this should be over-ridden to have a NULL 
				implementation because this will improve rendering speed.
	SeeAlso:	ChangeAttrType

********************************************************************************************/

void RenderRegion::SetFillAttributes(ChangeAttrType Type)
{
	switch (Type)
	{
		case CHANGEATTR_ALL:
			FillFlags.ValidGeometry 		= FALSE;
			FillFlags.ValidEffect 			= FALSE;
			FillFlags.ValidMapping 			= FALSE;
			FillFlags.ValidTransGeometry 	= FALSE;
			FillFlags.ValidTransEffect 		= FALSE;
			FillFlags.ValidTransMapping 	= FALSE;
			FillFlags.ValidWindingRule	 	= FALSE;
			break;

		case CHANGEATTR_GEOMETRY:
			FillFlags.ValidGeometry 		= FALSE;
			break;

		case CHANGEATTR_EFFECT:
			FillFlags.ValidEffect 			= FALSE;
			break;

		case CHANGEATTR_MAPPING:
			FillFlags.ValidMapping 			= FALSE;
			break;

		case CHANGEATTR_TRANSP_GEOMETRY:
			FillFlags.ValidTransGeometry 	= FALSE;
			break;

		case CHANGEATTR_TRANSP_EFFECT:
			FillFlags.ValidTransEffect 		= FALSE;
			break;

		case CHANGEATTR_TRANSP_MAPPING:
			FillFlags.ValidTransMapping 	= FALSE;
			break;

		case CHANGEATTR_WINDING_RULE:
			FillFlags.ValidWindingRule	 	= FALSE;
			break;

		default:
			ERROR3_PF(("Invalid attribute type (%d) in SetFillAttributes", Type));
			break;
	}
}

/********************************************************************************************

>	BOOL RenderRegion::SaveAttribute(UINT32 Index, AttributeValue *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		Index - Unique attribute ID (See AttributeManager::RegisterDefaultAttribute).
				pAttr - the new attribute to use as the current one.
				Temp - TRUE if pAttr points to a temporary object, FALSE if not.
	Returns:	TRUE if the operation completed successfully, FALSE if not.
	Purpose:	Saves the current attribute on the attribute context stack, and installs
				the given attribute as the new attribute.
				If the attribute object is temporary, then it is deleted when the attribute
				is popped off the stack. See RenderRegion::RestoreAttribute
	Errors:		Out of memory.
	SeeAlso:	AttributeManager::RegisterDefaultAttribute

********************************************************************************************/

BOOL RenderRegion::SaveAttribute(UINT32 Index, AttributeValue *pAttr, BOOL Temp)
{
#ifdef _DEBUG
	if(m_TraceOutContextLevels)
	{
		TRACE( _T("...Saving Level %d Attribute %s 0x%x\n"),m_CurrentContextLevel,(LPCTSTR)pAttr->GetRuntimeClass()->GetClassName(), pAttr);
	}

	// Make sure the current Attr is a valid pointer.
	CC_ASSERT_VALID((CurrentAttrs[Index].pAttr));
	CC_ASSERT_VALID((pAttr));

#endif

	// Push the current attribute onto the stack, and install the new attribute as
	// the 'current' one.
	if (TheStack.Push(CurrentAttrs[Index].pAttr, CurrentAttrs[Index].Temp))
	{
		CurrentAttrs[Index].pAttr = pAttr; 
		CurrentAttrs[Index].Temp = Temp;

		// Everything worked ok.
		return TRUE;
	}

	// Failed
	return FALSE;
}

/********************************************************************************************

>	void RenderRegion::RestoreAttribute(UINT32 Index, AttributeValue *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		Index - Unique attribute ID (See AttributeManager::RegisterDefaultAttribute).
				pAttr - the new attribute to use as the current one.
				Temp - TRUE if pAttr points to a temporary object, FALSE if not.
	Purpose:	Restore the given attribute to the current attribute array.  This is called
				when the rendering context stack is popped - the current attribute is
				ousted (and deleted if it is a temporary object), and the given one is
				installed in its place.
	SeeAlso:	RenderRegion::SaveAttribute

********************************************************************************************/

void RenderRegion::RestoreAttribute(UINT32 Index, AttributeValue *pAttr, BOOL Temp)
{
#ifdef DEBUG
	if(m_TraceOutContextLevels)
	{
		TRACE( _T("Am Resetting %d "), IsBeingReset);
		TRACE( _T("...Restoring Level %d Attribute 0x%x with 0x%x"),m_CurrentContextLevel,CurrentAttrs[Index].pAttr, pAttr);
	}

	// Make sure the current Attr is a valid pointer.
	if(!IsBeingReset)
		CC_ASSERT_VALID((CurrentAttrs[Index].pAttr));

	if(m_TraceOutContextLevels)
	{
		TRACE( _T(" Attribute %s\n"), (LPCTSTR)pAttr->GetRuntimeClass()->m_lpszClassName);
	}
#endif

	// Mark Howitt Friday 13th Oct 2000
	// I`ve placed this new check in here in order to stop a problem that causes camelot to crash
	// when calling the GoingOutOfScope function. The problem is that the attribute that the stack
	// is pointing to has been deleted and is pointing at deleted memory!
	// This only happens in the case of when an Op is called when the render loop is deep inside
	// the tree and the Op affects an attribute above the current context which when restored is
	// corrupt!!! The way we are now dealing with this is to basically not call the GoingOutOfScope
	// function but just make a call to the PopPathProcessor function. This makes sure that we
	// relinquish any memory.
	// We`re using the IsBeingReset variable as it only goes TRUE when the region is about to die!
	// It`s also an Ilan variable used for the OffScreenRenderRegions!
	if(!IsBeingReset)
		CurrentAttrs[Index].pAttr->GoingOutOfScope(this);
	else
		PopPathProcessor();

	// If the current attribute is a temporary one, then delete it as we're about to stop
	// using it.
	if (CurrentAttrs[Index].Temp) 
		delete CurrentAttrs[Index].pAttr;

	// Install the new attribute.
	CurrentAttrs[Index].pAttr = pAttr; 
	CurrentAttrs[Index].Temp = Temp;
}


/********************************************************************************************

>	BOOL RenderRegion::CopyCurrentAttributes(const RenderRegion &Other)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		Other - the render region to copy from.
	Returns:	TRUE if the current attributes are copied ok; FALSE if not.
	Purpose:	Copy the current attribute array of another render region into this
				render region.  The copying is intelligent; i.e. if the other array
				contains 'temporary' objects, then it will copy them, so two render
				regions don't try to delete the same object when their destructor is called.
	Errors:		Out of memory.

********************************************************************************************/

BOOL RenderRegion::CopyCurrentAttributes(const RenderRegion &Other)
{
	// If the source render region has no attributes, this is ok, as we may
	// be cloning it before it has started rendering (attributes only get set up when
	// some rendering is done).
	if (Other.CurrentAttrs == NULL)
	{
		// No attributes - nothing to do.
		CurrentAttrs = NULL;
		NumCurrentAttrs = 0;
		return TRUE;
	}

	// Allocate a current attributes block.
	NumCurrentAttrs = Other.NumCurrentAttrs;
	ENSURE(NumCurrentAttrs != 0, "Mismatched attribute stack in CopyCurrentAttributes");
	INT32 AttrBlkSize = sizeof(AttributeEntry) * NumCurrentAttrs;
	CurrentAttrs = (AttributeEntry *) CCMalloc(AttrBlkSize);
	if (CurrentAttrs == NULL)
		return FALSE;

	// Copy the current attribute values across
	memcpy(CurrentAttrs, Other.CurrentAttrs, AttrBlkSize);

	// We must take copies of any temporary objects to avoid multiple deletion problems.
	for (INT32 i = 0; i < NumCurrentAttrs; i++)
	{
		if (CurrentAttrs[i].Temp)
		{
			// Get the runtime class info on this object
			CCRuntimeClass *pCCRuntimeClass = CurrentAttrs[i].pAttr->GetRuntimeClass();
	
			// Create another object of the same type
			AttributeValue *pNewAttr = (AttributeValue *) pCCRuntimeClass->CreateObject();

			if (pNewAttr == NULL)
			{
				// Failed to create object - quit with error, but first ensure that all
				// the rest of the attributes are marked as non-temporary.
				// Otherwise the destructor will attempt to delete objects that belong 
				// to the other render region.
				for (INT32 j = i; j < NumCurrentAttrs; j++)
					CurrentAttrs[j].Temp = FALSE;

				return FALSE;
			}

			// Object created ok - get the object to copy its contents across.
			pNewAttr->SimpleCopy(CurrentAttrs[i].pAttr);

			// Place this attribute in the array
			CurrentAttrs[i].pAttr = pNewAttr;
		}
	}

	// It all worked
	return TRUE;
}



/********************************************************************************************

>	StrokeColourAttribute *RenderRegion::PrepareLineColour(StrokeColourAttribute *SourceAttr)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96

	Inputs:		SourceAttr -	NULL (to copy the current rendering line attribute),
								or the attribute to be "prepared".

	Returns:	NULL if the given attribute needs not be changed before it is stacked, or
				points to a new attribute which should be stacked in place of the given one.
				Note that it is up to the caller to delete the new attribute (so when
				stacking it, you should set the "Temp" flag)

	Purpose:	This function is used to allow 3 special attributes to function correctly.
				=> Overprint line, Overprint fill, and Print on all plates.

				These attributes are specil in that they act as modifiers to the current
				stroke and fill attributes - they convert active colours into either
				no-colour, or a special greyscale value.

				This is achieved by the following 2 steps:
				(1) Whenever a colour attribute is stacked, it is possibly replaced by a
				    copy which has been modified with respect to the current states of the 3
				    Imagesetting attributes.
				(2) Whenever an Imagesetting attribute is stacked, the current line and/or
				    fill attributes are copied (if necessary) in order to "apply" the new
					attribute effects to the current rendering colours.

				This function takes the appropriate attribute and determines if it needs
				to be changed. If it does, a new, replacement, attribute is created and
				returned, which the caller should use in place of the 
	
	SeeAlso:	RenderRegion::SetFillGeometry; RenderRegion::SetLineColour

********************************************************************************************/

StrokeColourAttribute *RenderRegion::PrepareLineColour(StrokeColourAttribute *SourceAttr)
{
	// --- Make sure we've got a source attribute to play with
	if (SourceAttr == NULL)
		SourceAttr = (StrokeColourAttribute *) CurrentAttrs[ATTR_STROKECOLOUR].pAttr;

	if (SourceAttr == NULL)		// No attribute to modify
		return(NULL);

	// --- Find the source line colour. If it's transparent or non-separable, we won't change it
	DocColour *SourceCol = SourceAttr->GetStartColour();

	if (SourceCol == NULL || SourceCol->IsTransparent() || !SourceCol->IsSeparable())
		return(NULL);

	// --- Check for the PrintOnAllPlates attribute
	if ( ((PrintOnAllPlatesAttrValue *) CurrentAttrs[ATTR_PRINTONALLPLATES].pAttr)->IsPrintOnAllPlatesOn() )
	{
		// Get a global default CMYK colour context
		ColourContextGreyT *cc = (ColourContextGreyT *) ColourManager::GetColourContext(COLOURMODEL_GREYT);
		ERROR3IF(cc == NULL, "No default Greyscale colour context?!");

		// The print on all plates attribute is set, so we convert this into a pre-separated
		// Key greyscale DocColour, overriding overprint etc. First get it as a greyscale:
		PColourGreyT GreyDef;
		cc->ConvertColour(SourceCol, (ColourPacked *) &GreyDef);

		// Then create a new CMYK colour with the grey-level in the Key component
		PColourCMYK ColDef;
		ColDef.Key = 255 - GreyDef.Intensity;
		ColDef.Cyan = ColDef.Magenta = ColDef.Yellow = 0;

		DocColour NewCol;					// Place the SourceCol into a DocColour, and make
		NewCol.SetCMYKValue(&ColDef);		// it non-colour-separable, so that it is dumped
		NewCol.SetSeparable(FALSE);			// out to the rendering device as it stands

		// And create and return a new stroke colour attribute
		StrokeColourAttribute *NewAttr = new StrokeColourAttribute(NewCol);
		return(NewAttr);
	}

	// --- Check for a valid ColourPlate - if there is none, then no more effects will apply
	ERROR3IF(CurrentColContext == NULL, "No colour context when rendering?!");
	ColourPlate *ColPlate = CurrentColContext->GetColourPlate();

	// Is there an active colour plate? If not, no overprint effects will occur
	if (ColPlate == NULL || ColPlate->IsDisabled())
		return(NULL);


	// --- Check for overprint
	// Is this entire colour plate set to overprint?
	BOOL Overprint = CurrentColContext->GetColourPlate()->Overprints();

	// If not, then is there a current "overprint line" attribute in effect?
	if (!Overprint)
		Overprint = ((OverprintLineAttrValue *) CurrentAttrs[ATTR_OVERPRINTLINE].pAttr)->IsOverprintOn();

	// If not, then is this a CMYK colour?
	if (!Overprint && SourceCol->GetColourModel() == COLOURMODEL_CMYK)
	{
		// It is CMYK - is the "always overprint black objects" flag enabled?
		if (RenderView->GetPrintControl() != NULL)
		{
			TypesetInfo *TPInfo = RenderView->GetPrintControl()->GetTypesetInfo();
			
			if (TPInfo != NULL && TPInfo->AlwaysOverprintBlack())
			{
				// It is enabled, so check if this is a CMYK colour with more than 95% Key in it.
				ColourCMYK ColDef;
				SourceCol->GetSourceColour((ColourGeneric *) &ColDef);

				if (ColDef.Key.MakeDouble() > 0.95)		// It is "black", so we'll overprint it
					Overprint = TRUE;
			}
		}
	}

	if (Overprint)
	{
		ERROR3IF(CurrentColContext->GetColourModel() != COLOURMODEL_RGBT,
					"Rendering ColourContext is not RGB. Bummer");

		// If overprint is on, then check if the colour is visible on this plate (i.e. is it white?)
		PColourRGBT ColDef;
		CurrentColContext->ConvertColour(SourceCol, (ColourPacked *) &ColDef);

		if (ColDef.Red > 254 && ColDef.Green > 254 && ColDef.Blue > 254)
		{
			DocColour NewCol(COLOUR_TRANS);		// It shouldn't appear on this plate, so return no-colour

			StrokeColourAttribute *NewAttr = new StrokeColourAttribute(NewCol);
			return(NewAttr);
		}
	}

	// No special effects apply - tell the caller to use the original
	return(NULL);
}



/********************************************************************************************

>	ColourFillAttribute *RenderRegion::PrepareFillGeometry(ColourFillAttribute *SourceAttr)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96

	Inputs:		SourceAttr -	NULL (to copy the current rendering fill attribute),
								or the attribute to be "prepared".

	Returns:	NULL if the given attribute needs not be changed before it is stacked, or
				points to a new attribute which should be stacked in place of the given one.
				Note that it is up to the caller to delete the new attribute (so when
				stacking it, you should set the "Temp" flag)

	Purpose:	This function is used to allow 3 special attributes to function correctly.
				=> Overprint line, Overprint fill, and Print on all plates.

				These attributes are specil in that they act as modifiers to the current
				stroke and fill attributes - they convert active colours into either
				no-colour, or a special greyscale value.

				This is achieved by the following 2 steps:
				(1) Whenever a colour attribute is stacked, it is possibly replaced by a
				    copy which has been modified with respect to the current states of the 3
				    Imagesetting attributes.
				(2) Whenever an Imagesetting attribute is stacked, the current line and/or
				    fill attributes are copied (if necessary) in order to "apply" the new
					attribute effects to the current rendering colours.

				This function takes the appropriate attribute and determines if it needs
				to be changed. If it does, a new, replacement, attribute is created and
				returned, which the caller should use in place of the 

	Notes:		This function has one other purpose: It fixes bogus graduated fills.
				A no-colour to no-colour grad fill is (strangely) rendered as a solid
				black fill, so this methid will convert these silly attributes into flat
				no-colour fills.
	
	SeeAlso:	RenderRegion::SetFillGeometry; RenderRegion::SetLineColour

********************************************************************************************/

ColourFillAttribute *RenderRegion::PrepareFillGeometry(ColourFillAttribute *SourceAttr)
{
	// --- Make sure we've got a source attribute to play with
	if (SourceAttr == NULL)		// No provided attribute, so use the current fill attr
		SourceAttr = (ColourFillAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

	if (SourceAttr == NULL)		// No attribute to modify
		return(NULL);

	// --- Find the source colours.
	DocColour *StartCol	= SourceAttr->GetStartColour();
	if (StartCol == NULL)									// Bitmap fills may be NULL - we leave them be
		return(NULL);

	DocColour *EndCol	= SourceAttr->GetEndColour();		// Note: May be NULL if it's a flat fill

	ColourFillAttribute *NewAttr = NULL;


	// --- Check for bogus grad fills
	// A no-colour to no-colour grad fill renders as solid black, so we always convert this
	// into a simple no-colour flat fill
	if (EndCol != NULL && StartCol->IsTransparent() && EndCol->IsTransparent())
	{
		NewAttr = new FlatFillAttribute(*StartCol);
		return(NewAttr);
	}

	// --- Check for bitmap fills
	// We can't change the colours for PrintOnAllPlates because when printing these are rendered to
	// a masked render region, and then get colour-separated (doh!)
	// We can't render bitmaps with overprint, as we can't do a "transparent" colour in an
	// arbitrarily transformed bitmap
//	if (SourceAttr->IsKindOf(CC_RUNTIME_CLASS(BitmapFillAttribute)))
	if (SourceAttr->IsAKindOfBitmapFill())
		return(NULL);

	// --- Check for the PrintOnAllPlates attribute
	if ( ((PrintOnAllPlatesAttrValue *) CurrentAttrs[ATTR_PRINTONALLPLATES].pAttr)->IsPrintOnAllPlatesOn() )
	{
		BOOL HaveNewStartCol = FALSE;
		BOOL HaveNewEndCol	 = FALSE;

		DocColour NewStartCol;
		DocColour NewEndCol;

		// Get a global default CMYK colour context
		ColourContextGreyT *cc = (ColourContextGreyT *) ColourManager::GetColourContext(COLOURMODEL_GREYT);
		ERROR3IF(cc == NULL, "No default Greyscale colour context?!");

		BOOL bNegate = FALSE;
		ColourPlateType Plate = COLOURPLATE_KEY;
		if (RenderView && RenderView->GetColourPlate())
		{
			bNegate = RenderView->GetColourPlate()->IsNegative();
			if (!RenderView->GetColourPlate()->IsMonochrome())
				Plate = RenderView->GetColourPlate()->GetType();
		}

		// Apply POAP to the start colour, if the colour will be affected
		if (!StartCol->IsTransparent() && StartCol->IsSeparable())
		{
			// The print on all plates attribute is set, so we convert this into a pre-separated
			// Key greyscale DocColour, overriding overprint etc. First get it as a greyscale:
			PColourGreyT GreyDef;
			cc->ConvertColour(StartCol, (ColourPacked *) &GreyDef);

			// Then create a new CMYK colour with the grey-level in the required component
			PColourCMYK ColDef = {0, 0, 0, 0};
			BYTE InkVal = bNegate ? GreyDef.Intensity : 255 - GreyDef.Intensity;
			switch (Plate)
			{
			case COLOURPLATE_CYAN:
				ColDef.Cyan = InkVal;
				break;
			case COLOURPLATE_MAGENTA:
				ColDef.Magenta = InkVal;
				break;
			case COLOURPLATE_YELLOW:
				ColDef.Yellow = InkVal;
				break;
			default:
				// This handles normal and composite as well as mono, key and spot plates
				ColDef.Key = InkVal;
				break;
			}

			NewStartCol.SetCMYKValue(&ColDef);	// Set the new start colour to the CMYK value
			NewStartCol.SetSeparable(FALSE);	// And set it non-separable

			HaveNewStartCol = TRUE;				// Flag the fact that it's changed
		}

		// Apply POAP to the end colour (if any), if the colour will be affected
		if (EndCol != NULL && !EndCol->IsTransparent() && EndCol->IsSeparable())
		{
			// The print on all plates attribute is set, so we convert this into a pre-separated
			// Key greyscale DocColour, overriding overprint etc. First get it as a greyscale:
			PColourGreyT GreyDef;
			cc->ConvertColour(EndCol, (ColourPacked *) &GreyDef);

			// Then create a new CMYK colour with the grey-level in the required component
			PColourCMYK ColDef = {0, 0, 0, 0};
			BYTE InkVal = bNegate ? GreyDef.Intensity : 255 - GreyDef.Intensity;
			switch (Plate)
			{
			case COLOURPLATE_CYAN:
				ColDef.Cyan = InkVal;
				break;
			case COLOURPLATE_MAGENTA:
				ColDef.Magenta = InkVal;
				break;
			case COLOURPLATE_YELLOW:
				ColDef.Yellow = InkVal;
				break;
			default:
				// This handles normal and composite as well as mono, key and spot plates
				ColDef.Key = InkVal;
				break;
			}

			NewEndCol.SetCMYKValue(&ColDef);	// Set the new start colour to the CMYK value
			NewEndCol.SetSeparable(FALSE);		// And set it non-separable

			HaveNewEndCol = TRUE;				// Flag the fact that it's changed
		}

		// Have we changed either colour? If so, create a new attribute and return it
		if (HaveNewStartCol || HaveNewEndCol)
		{
			if (EndCol == NULL)
			{
				// It was a flat fill, so create a new flat fill attribute
				NewAttr = new FlatFillAttribute(NewStartCol);
			}
			else
			{
				// Create a similar graduated fill attribute with the new colours
				CCRuntimeClass *pCCRuntimeClass = SourceAttr->GetRuntimeClass();
				NewAttr = (ColourFillAttribute *) pCCRuntimeClass->CreateObject();

				if (NewAttr != NULL)
				{
					// Copy the details of the source attribute
					NewAttr->SimpleCopy(SourceAttr);

					// Set up the attribute, replacing the colours we've decided to change
					if (HaveNewStartCol)
						NewAttr->SetStartColour(&NewStartCol);

					if (HaveNewEndCol)
						NewAttr->SetEndColour(&NewEndCol);
				}
			}

			return(NewAttr);
		}
	}


	// --- Check for a valid ColourPlate - if there is none, then no more effects will apply
	ERROR3IF(CurrentColContext == NULL, "No colour context when rendering?!");
	ColourPlate *ColPlate = CurrentColContext->GetColourPlate();

	if (ColPlate == NULL || ColPlate->IsDisabled())
		return(NULL);


	// --- Check for overprint
	// Is this entire colour plate set to overprint?
	BOOL Overprint = CurrentColContext->GetColourPlate()->Overprints();

	// If not, then is there a current "overprint fill" attribute in effect?
	if (!Overprint)
		Overprint = ((OverprintFillAttrValue *) CurrentAttrs[ATTR_OVERPRINTFILL].pAttr)->IsOverprintOn();

	// If not, then is this a "black" CMYK colour/fill?
	// We check if "Always Overprint Black" is enabled, and if the fill is a flat "black" fill, or
	// a grad fill between two "black" colours - but the checks are in a special order to try to
	// make the code as efficient as possible.
	if (!Overprint && StartCol->GetColourModel() == COLOURMODEL_CMYK && !StartCol->IsTransparent())
	{
		// How about the end colour? If it's a flat fill (no end col), or the end col is also CMYK, we continue
		if ( EndCol == NULL || (EndCol->GetColourModel() == COLOURMODEL_CMYK && !EndCol->IsTransparent()) )
		{
			// Both colours are potentially "black" - is the "always overprint black objects" flag enabled?
			if (RenderView->GetPrintControl() != NULL)
			{
				TypesetInfo *TPInfo = RenderView->GetPrintControl()->GetTypesetInfo();
				
				if (TPInfo != NULL && TPInfo->AlwaysOverprintBlack())
				{
					// It is enabled, so check if this is a CMYK colour with more than 95% Key in it.
					ColourCMYK ColDef;
					StartCol->GetSourceColour((ColourGeneric *) &ColDef);

					if (ColDef.Key.MakeDouble() > 0.95)		// It is "black", so we'll overprint it
					{
						if (EndCol == NULL)
							Overprint = TRUE;				// Flat "black" fill, so overprint it
						else
						{
							// The start colour is "black", but is the End colour "black" too?
							EndCol->GetSourceColour((ColourGeneric *) &ColDef);

							if (ColDef.Key.MakeDouble() > 0.95)		// Both cols are "black" so we'll overprint
								Overprint = TRUE;
						}
					}
				}
			}
		}
	}

	if (Overprint)
	{
		ERROR3IF(CurrentColContext->GetColourModel() != COLOURMODEL_RGBT,
					"Rendering ColourContext is not RGB. Bummer");

		// If overprint is on, then check if the colour is visible on this plate (i.e. is it white?)
		PColourRGBT ColDef;

		// Check the start colour. If it's white, we'll set it to no-colour (note: the default is to be
		// no-colour, so we only have to flag the fact that we want to change it here)
		BOOL UseTransStart = FALSE;
		if (!StartCol->IsTransparent() && StartCol->IsSeparable())
		{
			CurrentColContext->ConvertColour(StartCol, (ColourPacked *) &ColDef);
			UseTransStart = (ColDef.Red > 254 && ColDef.Green > 254 && ColDef.Blue > 254);
		}

		// Check the end colour - same as the start col, except that it may be NULL if it's a flat fill
		BOOL UseTransEnd = FALSE;
		if (EndCol != NULL && !EndCol->IsTransparent() && EndCol->IsSeparable())
		{
			CurrentColContext->ConvertColour(EndCol, (ColourPacked *) &ColDef);
			UseTransEnd = (ColDef.Red > 254 && ColDef.Green > 254 && ColDef.Blue > 254);
		}

		if (UseTransStart || UseTransEnd)		// If we must change either end to transparent...
		{
			if (EndCol == NULL ||										// If it's a flat fill, or
					((UseTransStart	|| StartCol->IsTransparent()) &&	// the start and end are both
					 (UseTransEnd	|| EndCol->IsTransparent()))		// no-colour, make a flat fill
				)
			{
				// Make a flat fill
				// (NOTE: this convert-to-flat-fill is necessary for the no-colour to no-colour grad fills
				// that overprint can create, as they otherwise (helpfully) render as a black solid fill
				// rather than the expected no-fill, while a no-colour flat fill works properly)
				DocColour Trans(COLOUR_TRANS);
				NewAttr = new FlatFillAttribute((UseTransStart) ? Trans : *StartCol);
				return(NewAttr);
			}

			// NOTE:
			// We don't change the fill if it will remain graduated, because a grad fill from a colour
			// to no-colour will not make the fill go transparent for overprint, so we might as well
			// just leave it alone - i.e. the only case we care about is when the grad fill ends
			// up as ultimately a transparent-transparent fill after colour separation.
		}
	}

	// No special effects apply - tell the caller to use the original
	return(NULL);
}



/********************************************************************************************

>	virtual void RenderRegion::SetLineOverprint(OverprintLineAttrValue *NewAttr, BOOL Temp);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96

	Inputs:		NewAttr - The new OverPrintLineAttrValue you want to set current
				Temp -	TRUE if this is a temporary object which the render region should
						delete when finished with it;
						FALSE if someone else will delete it (i.e. it's in the document tree)

	Purpose:	Set the line overprint state, pushing the current "overprint line" attribute
				onto the context stack.

	SeeAlso:	RenderRegion::RestoreOverprintLine

********************************************************************************************/

void RenderRegion::SetLineOverprint(OverprintLineAttrValue *NewAttr, BOOL Temp)
{
	// Set the new Overprint state
	SaveAttribute(ATTR_OVERPRINTLINE, NewAttr, Temp);

	// And make sure the current line colour attribute is affected by the new overprint state
	StrokeColourAttribute *NewLineCol = PrepareLineColour();
	if (NewLineCol != NULL)
		SetLineColour(NewLineCol, TRUE);
}



/********************************************************************************************

>	virtual void RenderRegion::RestoreLineOverprint(OverprintLineAttrValue *NewAttr, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96

	Inputs:		NewAttr - The new OverPrintLineAttrValue you want to set current
				Temp -	TRUE if this is a temporary object which the render region should
						delete when finished with it;
						FALSE if someone else will delete it (i.e. it's in the document tree)

	Purpose:	Restore the previous Line Overprint state, popping the previous state from
				the rendering stack

	SeeAlso:	RenderRegion::SetOverprintLine

********************************************************************************************/

void RenderRegion::RestoreLineOverprint(OverprintLineAttrValue *NewAttr, BOOL Temp)
{
	RestoreAttribute(ATTR_OVERPRINTLINE, NewAttr, Temp);
}



/********************************************************************************************

>	virtual void RenderRegion::SetFillOverprint(OverprintFillAttrValue *NewAttr, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96

	Inputs:		NewAttr - The new OverPrintFillAttrValue you want to set current
				Temp -	TRUE if this is a temporary object which the render region should
						delete when finished with it;
						FALSE if someone else will delete it (i.e. it's in the document tree)

	Purpose:	Set the fill overprint state, pushing the current "overprint fill" attribute
				onto the context stack.

	SeeAlso:	RenderRegion::RestoreOverprintFill

********************************************************************************************/

void RenderRegion::SetFillOverprint(OverprintFillAttrValue *NewAttr, BOOL Temp)
{
	// Set the new Overprint state
	SaveAttribute(ATTR_OVERPRINTFILL, NewAttr, Temp);

	// And make sure the current fill colour attribute is affected by the new overprint state
	ColourFillAttribute *NewFillGeometry = PrepareFillGeometry();
	if (NewFillGeometry != NULL)
		SetFillGeometry(NewFillGeometry, TRUE);
}



/********************************************************************************************

>	virtual void RenderRegion::RestoreFillOverprint(OverprintFillAttrValue *NewAttr, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96

	Inputs:		NewAttr - The new OverPrintLineAttrValue you want to set current
				Temp -	TRUE if this is a temporary object which the render region should
						delete when finished with it;
						FALSE if someone else will delete it (i.e. it's in the document tree)

	Purpose:	Restore the previous Fill Overprint state, popping the previous state from
				the rendering stack

	SeeAlso:	RenderRegion::SetOverprintLine

********************************************************************************************/

void RenderRegion::RestoreFillOverprint(OverprintFillAttrValue *NewAttr, BOOL Temp)
{
	RestoreAttribute(ATTR_OVERPRINTFILL, NewAttr, Temp);
}



/********************************************************************************************

>	virtual void RenderRegion::SetPrintOnAllPlates(PrintOnAllPlatesAttrValue *NewAttr, BOOL Temp);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96

	Inputs:		NewAttr - The new PrintOnAllPlatesAttrValue you want to set current
				Temp -	TRUE if this is a temporary object which the render region should
						delete when finished with it;
						FALSE if someone else will delete it (i.e. it's in the document tree)

	Purpose:	Set the PrintOnAllPlates state, pushing the current "print on all plates" attribute
				onto the context stack.

	SeeAlso:	RenderRegion::RestorePrintOnAllPlates

********************************************************************************************/

void RenderRegion::SetPrintOnAllPlates(PrintOnAllPlatesAttrValue *NewAttr, BOOL Temp)
{
	// Set the new Overprint state
	SaveAttribute(ATTR_PRINTONALLPLATES, NewAttr, Temp);

	// And make sure the current line colour attribute is affected by the new state
	StrokeColourAttribute *NewLineCol = PrepareLineColour();
	if (NewLineCol != NULL)
		SetLineColour(NewLineCol, TRUE);

	// And make sure the current fill colour attribute is affected by the new state
	ColourFillAttribute *NewFillGeometry = PrepareFillGeometry();
	if (NewFillGeometry != NULL)
		SetFillGeometry(NewFillGeometry, TRUE);
}



/********************************************************************************************

>	virtual void RenderRegion::RestorePrintOnAllPlates(PrintOnAllPlatesAttrValue *NewAttr, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96

	Inputs:		NewAttr - The new OverPrintLineAttrValue you want to set current
				Temp -	TRUE if this is a temporary object which the render region should
						delete when finished with it;
						FALSE if someone else will delete it (i.e. it's in the document tree)

	Purpose:	Restore the previous print-on-all-plates state, popping the previous state from
				the rendering stack

	SeeAlso:	RenderRegion::SetOverprintLine

********************************************************************************************/

void RenderRegion::RestorePrintOnAllPlates(PrintOnAllPlatesAttrValue *NewAttr, BOOL Temp)
{
	RestoreAttribute(ATTR_PRINTONALLPLATES, NewAttr, Temp);
}



/********************************************************************************************

>	void RenderRegion::SetLineColour(DocColour &NewLineColour)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		NewLineColour - the colour to set the line colour to.
	Purpose:	Set the line colour to a specific colour, pushing the current line colour 
				onto the context stack.
	SeeAlso:	RenderRegion::RestoreLineColour

********************************************************************************************/

void RenderRegion::SetLineColour(DocColour &NewLineColour)
{
	// Don't bother if it doesn't cause a change
	StrokeColourAttribute *pAttr = (StrokeColourAttribute *) CurrentAttrs[ATTR_STROKECOLOUR].pAttr;
	if ((pAttr != NULL) && (NewLineColour == pAttr->Colour))
		return;

	// Create a temporary attribute and use it to set the line colour.
	pAttr = new StrokeColourAttribute(NewLineColour);
	SetLineColour(pAttr, TRUE);
}



/********************************************************************************************

>	void RenderRegion::SetLineColour(StockColour NewLineColour)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		NewLineColour - the colour to set the line colour to.
	Purpose:	Set the line colour to a specific colour, pushing the current line colour 
				onto the context stack.
	SeeAlso:	RenderRegion::RestoreLineColour

********************************************************************************************/

void RenderRegion::SetLineColour(StockColour New)
{
	DocColour NewLineColour(New);

	// Don't bother if it doesn't cause a change
	StrokeColourAttribute *pAttr = (StrokeColourAttribute *) CurrentAttrs[ATTR_STROKECOLOUR].pAttr;
	if ((pAttr != NULL) && (NewLineColour == pAttr->Colour))
		return;

	// Create a temporary attribute and use it to set the line colour.
	pAttr = new StrokeColourAttribute(NewLineColour);
	SetLineColour(pAttr, TRUE);
}



/********************************************************************************************

>	void RenderRegion::SetLineColour(StrokeColourAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current line colour according to an AttributeValue object, pushing
				the current line colour onto the context stack.
	SeeAlso:	RenderRegion::RestoreLineColour

********************************************************************************************/

void RenderRegion::SetLineColour(StrokeColourAttribute *pAttr, BOOL Temp)
{
	// Before stacking the attribute, check for overprint and print on all plates - these
	// may cause the line colour to change, in which case we replace the given attribute
	// with a new modified one.
	StrokeColourAttribute *NewAttr = PrepareLineColour(pAttr);

	if (NewAttr != NULL)
	{
		if (Temp)
			delete pAttr;

		pAttr = NewAttr;		// Use the modified attribute
		Temp = TRUE;			// ...which is always temporary
	}

	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_STROKECOLOUR, pAttr, Temp))
	{
		// The line attributes need to be reset before drawing anything.
		SetLineAttributes(CHANGEATTR_STROKECOL);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreLineColour(StrokeColourAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current line colour, and restore the specified line
				colour value.
	SeeAlso:	RenderRegion::SetLineColour

********************************************************************************************/

void RenderRegion::RestoreLineColour(StrokeColourAttribute *pAttr, BOOL Temp)
{
	// Restore the line colour attribute
	RestoreAttribute(ATTR_STROKECOLOUR, pAttr, Temp);

	// The line attributes need to be reset before drawing anything.
	SetLineAttributes(CHANGEATTR_STROKECOL);
}

/********************************************************************************************

>	void RenderRegion::SetLineTransp(StrokeTranspAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current line colour according to an AttributeValue object, pushing
				the current line colour onto the context stack.
	SeeAlso:	RenderRegion::RestoreLineColour

********************************************************************************************/

void RenderRegion::SetLineTransp(StrokeTranspAttribute *pAttr, BOOL Temp)
{
	StrokeTranspAttribute* pNewAttr = NULL;
	if (m_bForceMix)
	{
		UINT32 Type = pAttr->GetTranspType();
		if (Type != TT_NoTranspType &&
			Type != TT_Mix &&
			Type != TT_DARKEN &&
			Type != TT_LIGHTEN &&
			Type != TT_BRIGHTNESS &&
			Type != TT_BEVEL)
		{
			if (Temp)
			{
				pNewAttr = pAttr;
			}
			else
			{
				pNewAttr = (StrokeTranspAttribute*)(pAttr->GetRuntimeClass()->CreateObject());
				if (pNewAttr)
				{
					pNewAttr->SimpleCopy(pAttr);
				}
			}

		}
	}

	if (pNewAttr)
	{
		pNewAttr->SetTranspType(TT_Mix);
		Temp = TRUE;
	}
	else
		pNewAttr = pAttr;

	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_STROKETRANSP, pNewAttr, Temp))
	{
		// The line attributes need to be reset before drawing anything.
		SetLineAttributes(CHANGEATTR_STROKETRANSP);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreLineTransp(StrokeTranspAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current line colour, and restore the specified line
				colour value.
	SeeAlso:	RenderRegion::SetLineColour

********************************************************************************************/

void RenderRegion::RestoreLineTransp(StrokeTranspAttribute *pAttr, BOOL Temp)
{
	// Restore the line colour attribute
	RestoreAttribute(ATTR_STROKETRANSP, pAttr, Temp);

	// The line attributes need to be reset before drawing anything.
	SetLineAttributes(CHANGEATTR_STROKETRANSP);
}



/********************************************************************************************

>	virtual void RenderRegion::SetStrokeType(StrokeTypeAttrValue *pAttr, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pAttr	- the object specifying the colour to use.
				Temp	- indicates if pAttr points to a temporary attribute that
						  should be deleted when it's finished with.

	Purpose:	Set the current Stroke Type according to an AttributeValue object, pushing
				the current Stroke Type onto the context stack.

	SeeAlso:	RenderRegion::RestoreStrokeType

********************************************************************************************/

void RenderRegion::SetStrokeType(StrokeTypeAttrValue *pAttr, BOOL Temp)
{
	SaveAttribute(ATTR_STROKETYPE, pAttr, Temp);
}



/********************************************************************************************

>	virtual void RenderRegion::RestoreStrokeType(StrokeTypeAttrValue *pAttr, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pAttr	- the object specifying the new stroke type
				Temp	- indicates if pAttr points to a temporary attribute that
						  should be deleted when it's finished with.

	Purpose:	Get rid of the current stroke type, and restore the specified stroke type

	SeeAlso:	RenderRegion::SetStrokeType

********************************************************************************************/

void RenderRegion::RestoreStrokeType(StrokeTypeAttrValue *pAttr, BOOL Temp)
{
	RestoreAttribute(ATTR_STROKETYPE, pAttr, Temp);
}



/********************************************************************************************

>	virtual void RenderRegion::SetVariableWidth(VariableWidthAttrValue *pAttr, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pAttr	- the object specifying the colour to use.
				Temp	- indicates if pAttr points to a temporary attribute that
						  should be deleted when it's finished with.

	Purpose:	Set the current VariableWidth according to an AttributeValue object, pushing
				the current VariableWidth onto the context stack.

	SeeAlso:	RenderRegion::RestoreVariableWidth

********************************************************************************************/

void RenderRegion::SetVariableWidth(VariableWidthAttrValue *pAttr, BOOL Temp)
{
	SaveAttribute(ATTR_VARWIDTH, pAttr, Temp);
}



/********************************************************************************************

>	virtual void RenderRegion::RestoreVariableWidth(VariableWidthAttrValue *pAttr, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pAttr	- the object specifying the new stroke type
				Temp	- indicates if pAttr points to a temporary attribute that
						  should be deleted when it's finished with.

	Purpose:	Get rid of the current VariableWidth, and restore the specified VariableWidth

	SeeAlso:	RenderRegion::SetVariableWidth

********************************************************************************************/

void RenderRegion::RestoreVariableWidth(VariableWidthAttrValue *pAttr, BOOL Temp)
{
	RestoreAttribute(ATTR_VARWIDTH, pAttr, Temp);
}



/********************************************************************************************

>	void RenderRegion::SetFillColour(DocColour &NewFillColour)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		NewFillColour - the colour to set the fill colour to.
	Purpose:	Set the fill colour to a specific colour, pushing the current fill colour 
				onto the context stack.
	SeeAlso:	RenderRegion::RestoreFillColour

********************************************************************************************/

void RenderRegion::SetFillColour(DocColour &NewFillColour)
{
	// Don't bother if it doesn't cause a change
	ColourFillAttribute *pAttr = 
		(ColourFillAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

	if ((pAttr != NULL) && 
		(pAttr->GetRuntimeClass() == CC_RUNTIME_CLASS(FlatFillAttribute)) &&
		(NewFillColour == ((FlatFillAttribute *) pAttr)->Colour))
		return;

	// Create a temporary attribute and use it to set the line colour.
	pAttr = new FlatFillAttribute(NewFillColour);
	SetFillGeometry(pAttr, TRUE);
}


/********************************************************************************************

>	void RenderRegion::SetFillColour(StockColour NewFillColour)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		NewFillColour - the colour to set the fill colour to.
	Purpose:	Set the fill colour to a specific colour, pushing the current fill colour 
				onto the context stack.
	SeeAlso:	RenderRegion::RestoreFillColour

********************************************************************************************/

void RenderRegion::SetFillColour(StockColour New)
{
	DocColour NewFillColour(New);

	// Don't bother if it doesn't cause a change
	ColourFillAttribute *pAttr = 
		(ColourFillAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

	if ((pAttr != NULL) && 
		(pAttr->GetRuntimeClass() == CC_RUNTIME_CLASS(FlatFillAttribute)) &&
		(NewFillColour == ((FlatFillAttribute *) pAttr)->Colour))
		return;

	// Create a temporary attribute and use it to set the line colour.
	pAttr = new FlatFillAttribute(NewFillColour);
	SetFillGeometry(pAttr, TRUE);
}



/********************************************************************************************

>	void RenderRegion::SetFillGeometry(FillGeometryAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the fill type to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current fill type according to an AttributeValue object, pushing
				the current fill type onto the context stack.


	SeeAlso:	RenderRegion::RestoreFillType; RenderRegion::SetFillColour

********************************************************************************************/

void RenderRegion::SetFillGeometry(ColourFillAttribute *pAttr, BOOL Temp)
{
	// Before stacking the attribute, check for overprint and print on all plates - these
	// may cause the fill colours to change, in which case we replace the given attribute
	// with a new modified one.
	ColourFillAttribute *NewAttr = PrepareFillGeometry(pAttr);

	if (NewAttr != NULL)
	{
		if (Temp)
			delete pAttr;

		pAttr = NewAttr;		// Use the modified attribute
		Temp = TRUE;			// ...which is always temporary
	}

	// Save the current attribute
	if (SaveAttribute(ATTR_FILLGEOMETRY, pAttr, Temp))
	{
		// The fill attributes need to be reset before drawing anything.
		SetFillAttributes(CHANGEATTR_GEOMETRY);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreFillGeometry(FillAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the fill type to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current fill type, and restore the specified fill type.
	SeeAlso:	RenderRegion::SetFillType; RenderRegion::SetFillColour

********************************************************************************************/

void RenderRegion::RestoreFillGeometry(ColourFillAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_FILLGEOMETRY, pAttr, Temp);

	// The fill attributes need to be reset before drawing anything.
	SetFillAttributes(CHANGEATTR_GEOMETRY);
}

/********************************************************************************************

>	void RenderRegion::SetFillMapping(FillMappingAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the fill type to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current fill type according to an AttributeValue object, pushing
				the current fill type onto the context stack.
	SeeAlso:	RenderRegion::RestoreFillType; RenderRegion::SetFillColour

********************************************************************************************/

void RenderRegion::SetFillMapping(FillMappingAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute
	if (SaveAttribute(ATTR_FILLMAPPING, pAttr, Temp))
	{
		// The fill attributes need to be reset before drawing anything.
		SetFillAttributes(CHANGEATTR_MAPPING);
	}
}



/********************************************************************************************

>	void RenderRegion::RestoreFillMapping(FillMappingAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the fill type to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current fill type, and restore the specified fill type.
	SeeAlso:	RenderRegion::SetFillType; RenderRegion::SetFillColour

********************************************************************************************/

void RenderRegion::RestoreFillMapping(FillMappingAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_FILLMAPPING, pAttr, Temp);

	// The fill attributes need to be reset before drawing anything.
	SetFillAttributes(CHANGEATTR_MAPPING);
}



/********************************************************************************************

>	void RenderRegion::SetFillEffect(FillEffectAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the fill type to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current fill type according to an AttributeValue object, pushing
				the current fill type onto the context stack.
	SeeAlso:	RenderRegion::RestoreFillType; RenderRegion::SetFillColour

********************************************************************************************/

void RenderRegion::SetFillEffect(FillEffectAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute
	if (SaveAttribute(ATTR_FILLEFFECT, pAttr, Temp))
	{
		// The fill attributes need to be reset before drawing anything.
		SetFillAttributes(CHANGEATTR_EFFECT);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreFillEffect(FillEffectAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the fill type to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current fill type, and restore the specified fill type.
	SeeAlso:	RenderRegion::SetFillType; RenderRegion::SetFillColour

********************************************************************************************/

void RenderRegion::RestoreFillEffect(FillEffectAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_FILLEFFECT, pAttr, Temp);

	// The fill attributes need to be reset before drawing anything.
	SetFillAttributes(CHANGEATTR_EFFECT);
}

/********************************************************************************************

>	void RenderRegion::SetTranspFillGeometry(TranspFillAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pAttr - the object specifying the fill type to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current fill type according to an AttributeValue object, pushing
				the current fill type onto the context stack.
	SeeAlso:	RenderRegion::RestoreFillType; RenderRegion::SetFillColour

********************************************************************************************/

void RenderRegion::SetTranspFillGeometry(TranspFillAttribute *pAttr, BOOL Temp)
{
	TranspFillAttribute* pNewAttr = NULL;
	if (m_bForceMix)
	{
		UINT32 Type = pAttr->GetTranspType();
		if (Type != TT_NoTranspType &&
			Type != TT_Mix &&
			Type != TT_DARKEN &&
			Type != TT_LIGHTEN &&
			Type != TT_BRIGHTNESS &&
			Type != TT_BEVEL)
		{
			if (Temp)
			{
				pNewAttr = pAttr;
			}
			else
			{
				pNewAttr = (TranspFillAttribute*)(pAttr->GetRuntimeClass()->CreateObject());
				if (pNewAttr)
				{
					pNewAttr->SimpleCopy(pAttr);
				}
			}

		}
	}

	if (pNewAttr)
	{
		pNewAttr->SetTranspType(TT_Mix);
		Temp = TRUE;
	}
	else
		pNewAttr = pAttr;

	// Save the current attribute
	if (SaveAttribute(ATTR_TRANSPFILLGEOMETRY, pNewAttr, Temp))
	{
		// The fill attributes need to be reset before drawing anything.
		SetFillAttributes(CHANGEATTR_TRANSP_GEOMETRY);

		// The capture system needs to know about non-MIX transparencies being set
		// because it can't capture them in RGBT bitmaps...
		UINT32 ttype = pNewAttr->GetTranspType();
		ERROR3IF(ttype<TT_NoTranspType || ttype>TT_MAX, "Someone's trying to set an unknown transp type!");

//		if (!(ttype==TT_NoTranspType || ttype==TT_Mix))
		if (!TranspTypeIsRGBTCompatible(ttype))
		{
			ChangeCapture(CAPTUREINFO(ctREUSE, cfCOLLAPSETOMASTER), FALSE, TRUE);
		}
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreTranspFillGeometry(FillAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pAttr - the object specifying the fill type to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current fill type, and restore the specified fill type.
	SeeAlso:	RenderRegion::SetFillType; RenderRegion::SetFillColour

********************************************************************************************/

void RenderRegion::RestoreTranspFillGeometry(TranspFillAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_TRANSPFILLGEOMETRY, pAttr, Temp);

	// The fill attributes need to be reset before drawing anything.
	SetFillAttributes(CHANGEATTR_TRANSP_GEOMETRY);
}

/********************************************************************************************

>	void RenderRegion::SetTranspFillMapping(TranspFillMappingAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pAttr - the object specifying the fill type to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current fill type according to an AttributeValue object, pushing
				the current fill type onto the context stack.
	SeeAlso:	RenderRegion::RestoreFillType; RenderRegion::SetFillColour

********************************************************************************************/

void RenderRegion::SetTranspFillMapping(TranspFillMappingAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute
	if (SaveAttribute(ATTR_TRANSPFILLMAPPING, pAttr, Temp))
	{
		// The fill attributes need to be reset before drawing anything.
		SetFillAttributes(CHANGEATTR_TRANSP_MAPPING);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreTranspFillMapping(TranspFillMappingAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pAttr - the object specifying the fill type to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current fill type, and restore the specified fill type.
	SeeAlso:	RenderRegion::SetFillType; RenderRegion::SetFillColour

********************************************************************************************/

void RenderRegion::RestoreTranspFillMapping(TranspFillMappingAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_TRANSPFILLMAPPING, pAttr, Temp);

	// The fill attributes need to be reset before drawing anything.
	SetFillAttributes(CHANGEATTR_TRANSP_MAPPING);
}

/********************************************************************************************

>	void RenderRegion::SetLineWidth(MILLIPOINT NewLineWidth)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		NewLineWidth - the size to set the line width to.
	Purpose:	Set the line width to a specific size, pushing the current line width 
				onto the context stack.
	SeeAlso:	RenderRegion::RestoreLineColour

********************************************************************************************/

void RenderRegion::SetLineWidth(MILLIPOINT NewLineWidth)
{
	// Don't bother if it doesn't cause a change
	LineWidthAttribute *pAttr = (LineWidthAttribute *) CurrentAttrs[ATTR_LINEWIDTH].pAttr;
	if ((pAttr != NULL) && (NewLineWidth == pAttr->LineWidth))
		return;

	// Create a temporary attribute and use it to set the line colour.
	pAttr = new LineWidthAttribute(NewLineWidth);
	SetLineWidth(pAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetLineWidth(LineWidthAttribute *pAttr,BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the line width to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current line width according to an AttributeValue object, pushing
				the current linewidth onto the context stack.
	SeeAlso:	RenderRegion::RestoreLineWidth

********************************************************************************************/

void RenderRegion::SetLineWidth(LineWidthAttribute *pAttr,BOOL Temp)
{
	// Set up the new attribute
	if (SaveAttribute(ATTR_LINEWIDTH, pAttr, Temp))
	{
		// The line attributes need to be reset before drawing anything.
		SetLineAttributes(CHANGEATTR_LINEWIDTH);

		// NB We need to invalidate the dash pattern too as this depends
		//	  on line width (doesn't seem to matter normally, but PostScript
		//    gets dash patterns wrong otherwise.
		SetLineAttributes(CHANGEATTR_DASHPATTERN);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreLineWidth(LineWidthAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the line width to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current line width, and restore the specified line width.
	SeeAlso:	RenderRegion::SetLineWidth

********************************************************************************************/

void RenderRegion::RestoreLineWidth(LineWidthAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_LINEWIDTH, pAttr, Temp);

	// The line attributes need to be reset before drawing anything.
	SetLineAttributes(CHANGEATTR_LINEWIDTH);

	// NB We need to invalidate the dash pattern too as this depends
	//	  on line width (doesn't seem to matter normally, but PostScript
	//    gets dash patterns wrong otherwise.
	SetLineAttributes(CHANGEATTR_DASHPATTERN);
}

/********************************************************************************************

>	void RenderRegion::SetDashPattern(DashRec& NewDashPattern)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		NewDashPattern - the on-off pattern of the dash.
	Purpose:	Set the dash pattern, pushing the current dash pattern 
				onto the context stack.
	SeeAlso:	RenderRegion::RestoreDashPattern

********************************************************************************************/

void RenderRegion::SetDashPattern(DashRec& NewDashPattern)
{
	// Don't bother if it doesn't cause a change
	DashPatternAttribute *pAttr = (DashPatternAttribute *) CurrentAttrs[ATTR_DASHPATTERN].pAttr;
	if ((pAttr != NULL) && (NewDashPattern == pAttr->DashPattern))
		return;

	// Create a temporary attribute and use it to set the line colour.
	pAttr = new DashPatternAttribute();
	pAttr->SetDashPattern(NewDashPattern);
	SetDashPattern(pAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetDashPattern(StockDash NewDashPattern)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		NewDashPattern - the on-off pattern of the dash.
	Purpose:	Set the dash pattern, pushing the current dash pattern 
				onto the context stack.
	SeeAlso:	RenderRegion::RestoreDashPattern

********************************************************************************************/

void RenderRegion::SetDashPattern(StockDash NewDashPattern)
{
	DashPatternAttribute *TempAttr = new DashPatternAttribute();
	TempAttr->SetStockDashPattern(NewDashPattern);

	// Don't bother if it doesn't cause a change
	DashPatternAttribute *pAttr = (DashPatternAttribute *) CurrentAttrs[ATTR_DASHPATTERN].pAttr;
	if ((pAttr != NULL) && (TempAttr->DashPattern == pAttr->DashPattern))
	{
		delete TempAttr;
		return;
	}

	// Create a temporary attribute and use it to set the line colour.
	SetDashPattern(TempAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetDeviceDashPattern(DashRec& NewDashPattern)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		NewDashPattern - the on-off pattern of the dash.
	Purpose:	Set the dash pattern, pushing the current dash pattern 
				onto the context stack.
	SeeAlso:	RenderRegion::RestoreDashPattern

********************************************************************************************/

void RenderRegion::SetDeviceDashPattern(DashRec& NewDashPattern)
{
	// Don't bother if it doesn't cause a change
	DashPatternAttribute *pAttr = (DashPatternAttribute *) CurrentAttrs[ATTR_DASHPATTERN].pAttr;
	if ((pAttr != NULL) && (NewDashPattern == pAttr->DashPattern))
		return;

	// Create a temporary attribute and use it to set the line colour.
	pAttr = new DashPatternAttribute();
	pAttr->SetDeviceDashPattern(NewDashPattern, GetScaledPixelWidth());
	SetDashPattern(pAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetDeviceDashPattern(StockDash NewDashPattern)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		NewDashPattern - the on-off pattern of the dash.
	Purpose:	Set the dash pattern, pushing the current dash pattern 
				onto the context stack.
	SeeAlso:	RenderRegion::RestoreDashPattern

********************************************************************************************/

void RenderRegion::SetDeviceDashPattern(StockDash NewDashPattern)
{
	DashPatternAttribute *TempAttr = new DashPatternAttribute();
	TempAttr->SetDeviceStockDashPattern(NewDashPattern, GetScaledPixelWidth());

	// Don't bother if it doesn't cause a change
	DashPatternAttribute *pAttr = (DashPatternAttribute *) CurrentAttrs[ATTR_DASHPATTERN].pAttr;
	if ((pAttr != NULL) && (TempAttr->DashPattern == pAttr->DashPattern))
	{
		delete TempAttr;
		return;
	}

	// Create a temporary attribute and use it to set the line colour.
	SetDashPattern(TempAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetDashPattern(DashPatternAttribute *pAttr,BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		pAttr - the object specifying the line width to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current dash pattern according to an AttributeValue object, pushing
				the current dash pattern onto the context stack.
	SeeAlso:	RenderRegion::RestoreDashPattern

********************************************************************************************/

void RenderRegion::SetDashPattern(DashPatternAttribute *pAttr,BOOL Temp)
{
	// Set up the new attribute
	if (SaveAttribute(ATTR_DASHPATTERN, pAttr, Temp))
	{
		// The line attributes need to be reset before drawing anything.
		SetLineAttributes(CHANGEATTR_DASHPATTERN);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreDashPattern(DashPatternAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		pAttr - the object specifying the dash pattern to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current dash pattern, and restore the specified one.
	SeeAlso:	RenderRegion::SetDashPattern

********************************************************************************************/

void RenderRegion::RestoreDashPattern(DashPatternAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_DASHPATTERN, pAttr, Temp);

	// The line attributes need to be reset before drawing anything.
	SetLineAttributes(CHANGEATTR_DASHPATTERN);
}

/********************************************************************************************

>	void RenderRegion::SetStartCap(LineCapType NewStartCap)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		NewStartCap, the new LineCap Type to cap lines with.
	Purpose:	Set the current Line Cap according to an AttributeValue object, pushing
				the current LineCap the context stack.
	SeeAlso:	RenderRegion::RestoreStartCap

********************************************************************************************/

void RenderRegion::SetStartCap(LineCapType NewStartCap)
{
	// Don't bother if it doesn't cause a change
	StartCapAttribute *pAttr = (StartCapAttribute *) CurrentAttrs[ATTR_STARTCAP].pAttr;
	if ((pAttr != NULL) && (NewStartCap == pAttr->StartCap))
		return;

	// Create a temporary attribute and use it to set the Start Cap.
	pAttr = new StartCapAttribute();
	pAttr->StartCap = NewStartCap;
	SetStartCap(pAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetStartCap(StartCapAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		pAttr - the object specifying the dash pattern to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current Line Cap according to an AttributeValue object, pushing
				the current LineCap the context stack.
	SeeAlso:	RenderRegion::RestoreStartCap

********************************************************************************************/

void RenderRegion::SetStartCap(StartCapAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	if (SaveAttribute(ATTR_STARTCAP, pAttr, Temp))
	{
		// The line attributes need to be reset before drawing anything.
		SetLineAttributes(CHANGEATTR_LINECAP);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreStartCap(StartCapAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		pAttr - the object specifying the dash pattern to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current start cap, and restore the specified one.
	SeeAlso:	RenderRegion::SetStartCap

********************************************************************************************/

void RenderRegion::RestoreStartCap(StartCapAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_STARTCAP, pAttr, Temp);

	// The line attributes need to be reset before drawing anything.
	SetLineAttributes(CHANGEATTR_LINECAP);
}

/********************************************************************************************

>	void RenderRegion::SetJoinType(JointType NewJoinType)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		NewJoinType, the new Join Type to join lines with.
	Purpose:	Set the current JoinType according to an AttributeValue object, pushing
				the current JoinType the context stack.
	SeeAlso:	RenderRegion::RestoreJoinType

********************************************************************************************/

void RenderRegion::SetJoinType(JointType NewJoinType)
{
	// Don't bother if it doesn't cause a change
	JoinTypeAttribute *pAttr = (JoinTypeAttribute *) CurrentAttrs[ATTR_JOINTYPE].pAttr;
	if ((pAttr != NULL) && (NewJoinType == pAttr->JoinType))
		return;

	// Create a temporary attribute and use it to set the Join Type.
	pAttr = new JoinTypeAttribute();
	pAttr->JoinType = NewJoinType;
	SetJoinType(pAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetJoinType(JoinTypeAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		pAttr - the object specifying the dash pattern to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current JoinType according to an AttributeValue object, pushing
				the current JoinType the context stack.
	SeeAlso:	RenderRegion::RestoreJoinType

********************************************************************************************/

void RenderRegion::SetJoinType(JoinTypeAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	if (SaveAttribute(ATTR_JOINTYPE, pAttr, Temp))
	{
		// The line attributes need to be reset before drawing anything.
		SetLineAttributes(CHANGEATTR_JOINTYPE);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreJoinType(JoinTypeAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		pAttr - the object specifying the dash pattern to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current JoinType, and restore the specified one.
	SeeAlso:	RenderRegion::SetJoinType

********************************************************************************************/

void RenderRegion::RestoreJoinType(JoinTypeAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_JOINTYPE, pAttr, Temp);

	// The line attributes need to be reset before drawing anything.
	SetLineAttributes(CHANGEATTR_JOINTYPE);
}


/********************************************************************************************

>	void RenderRegion::SetDefaultQuality()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Purpose:	Set the quality attribute to the default setting, pushing the current 
				quality level onto the context stack.
	SeeAlso:	RenderRegion::RestoreQuality

********************************************************************************************/

void RenderRegion::SetDefaultQuality()
{
	// Create a temporary attribute and use it to set the default quality..
	QualityAttribute *pAttr = new QualityAttribute();
	SetQuality(pAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetQuality(QualityAttribute *pAttr,BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the quality to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current display quality according to an AttributeValue object, 
				pushing	the current quality onto the context stack.
	SeeAlso:	RenderRegion::RestoreQuality

********************************************************************************************/

void RenderRegion::SetQuality(QualityAttribute *pAttr,BOOL Temp)
{
	// Set up the new attribute
	SaveAttribute(ATTR_QUALITY, pAttr, Temp);
	RRQuality = pAttr->QualityValue;

	// The render region's rendering objects need to be reset before drawing anything.
	SetQualityLevel();
}

/********************************************************************************************

>	void RenderRegion::RestoreQuality(QualityAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the quality level to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current quality level, and restore the specified quality 
				level.
	SeeAlso:	RenderRegion::SetQuality

********************************************************************************************/

void RenderRegion::RestoreQuality(QualityAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_QUALITY, pAttr, Temp);
	RRQuality = pAttr->QualityValue;

	// The render region's rendering objects need to be reset before drawing anything.
	SetQualityLevel();
}

/********************************************************************************************

>	virtual void RenderRegion::SetQualityLevel()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/97
	Inputs:		-
	Purpose:	Does nothing.

				This used to be a pure function until we found a way of making the base class get called
				(due to the introduction of the path processing code)

	SeeAlso:	RenderRegion::SetQuality

********************************************************************************************/

void RenderRegion::SetQualityLevel()
{
}

/********************************************************************************************

>	void RenderRegion::SetWebAddress(WebAddressAttribute *pAttr,BOOL Temp)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/97
	Inputs:		pAttr - pointer to a Web Address to set
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current WebAddress

				This attribute has no effect on rendering, so there's no need to 
				change anything in the rendering system itself when we do this.

	SeeAlso:	RenderRegion::RestoreWebAddress

********************************************************************************************/

void RenderRegion::SetWebAddress(WebAddressAttribute *pAttr,BOOL Temp)
{
	// Set up the new attribute
	SaveAttribute(ATTR_WEBADDRESS, (AttributeValue*) pAttr, Temp);
}

/********************************************************************************************

>	void RenderRegion::RestoreQuality(QualityAttribute *pAttr, BOOL Temp)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/97
	Inputs:		pAttr - pointer to a Web Address to set
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Restore the Web Address
	SeeAlso:	RenderRegion::SetWebAddress

********************************************************************************************/

void RenderRegion::RestoreWebAddress(WebAddressAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_WEBADDRESS, (AttributeValue*) pAttr, Temp);
}


/********************************************************************************************

>	void RenderRegion::SetStartArrow(StockArrow NewStartArrow)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		NewStartArrow, the new Arrow to draw on the start of lines.
	Purpose:	Set the current StartArrow according to an AttributeValue object, pushing
				the current Arrow the context stack.
	SeeAlso:	RenderRegion::RestoreStartArrow

********************************************************************************************/

void RenderRegion::SetStartArrow(StockArrow NewStartArrow)
{
	// Don't bother if it doesn't cause a change
	StartArrowAttribute *pAttr = (StartArrowAttribute *) CurrentAttrs[ATTR_STARTARROW].pAttr;
	if ((pAttr != NULL) && (pAttr->StartArrow == NewStartArrow))
		return;

	// Create a temporary attribute and use it to set the Start Arrow.
	pAttr = new StartArrowAttribute();
	pAttr->StartArrow = NewStartArrow;
	SetStartArrow(pAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetStartArrow(ArrowRec &NewStartArrow)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		NewStartArrow, the new Arrow to draw on the start of lines.
	Purpose:	Set the current StartArrow according to an AttributeValue object, pushing
				the current Arrow the context stack.
	SeeAlso:	RenderRegion::RestoreStartArrow

********************************************************************************************/

void RenderRegion::SetStartArrow(ArrowRec &NewStartArrow)
{
	// Don't bother if it doesn't cause a change
	StartArrowAttribute *pAttr = (StartArrowAttribute *) CurrentAttrs[ATTR_STARTARROW].pAttr;
	if ((pAttr != NULL) && (pAttr->StartArrow == NewStartArrow))
		return;

	// Create a temporary attribute and use it to set the Start Arrow.
	pAttr = new StartArrowAttribute();
	pAttr->StartArrow = NewStartArrow;
	SetStartArrow(pAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetStartArrow(StartArrowAttribute *pAttr,BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		pAttr - the object specifying the dash pattern to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current StartArrow according to an AttributeValue object, pushing
				the current StartArrow the context stack.
	SeeAlso:	RenderRegion::RestoreStartArrow

********************************************************************************************/

void RenderRegion::SetStartArrow(StartArrowAttribute *pAttr,BOOL Temp)
{
	// Set up the new attribute
	if (SaveAttribute(ATTR_STARTARROW, pAttr, Temp))
	{
		// The line attributes need to be reset before drawing anything.
		SetLineAttributes(CHANGEATTR_STARTARROW);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreStartArrow(StartArrowAttribute *pAttr,BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		pAttr - the object specifying the dash pattern to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current StartArrow, and restore the specified one.
	SeeAlso:	RenderRegion::SetStartArrow

********************************************************************************************/

void RenderRegion::RestoreStartArrow(StartArrowAttribute *pAttr,BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_STARTARROW, pAttr, Temp);

	// The line attributes need to be reset before drawing anything.
	SetLineAttributes(CHANGEATTR_STARTARROW);
}


/********************************************************************************************

>	void RenderRegion::SetEndArrow(StockArrow NewEndArrow)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		NewEndArrow, the new Arrow to draw on the end of lines.
	Purpose:	Set the current EndArrow according to an AttributeValue object, pushing
				the current Arrow the context stack.
	SeeAlso:	RenderRegion::RestoreEndArrow

********************************************************************************************/

void RenderRegion::SetEndArrow(StockArrow NewEndArrow)
{
	// Don't bother if it doesn't cause a change
	EndArrowAttribute *pAttr = (EndArrowAttribute *) CurrentAttrs[ATTR_ENDARROW].pAttr;
	if ((pAttr != NULL) && (pAttr->EndArrow == NewEndArrow))
		return;

	// Create a temporary attribute and use it to set the Start Arrow.
	pAttr = new EndArrowAttribute();
	pAttr->EndArrow = NewEndArrow;
	SetEndArrow(pAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetEndArrow(StockArrow NewEndArrow)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		NewEndArrow, the new Arrow to draw on the end of lines.
	Purpose:	Set the current EndArrow according to an AttributeValue object, pushing
				the current Arrow the context stack.
	SeeAlso:	RenderRegion::RestoreEndArrow

********************************************************************************************/

void RenderRegion::SetEndArrow(ArrowRec &NewEndArrow)
{
	// Don't bother if it doesn't cause a change
	EndArrowAttribute *pAttr = (EndArrowAttribute *) CurrentAttrs[ATTR_ENDARROW].pAttr;
	if ((pAttr != NULL) && (pAttr->EndArrow == NewEndArrow))
		return;

	// Create a temporary attribute and use it to set the Start Arrow.
	pAttr = new EndArrowAttribute();
	pAttr->EndArrow = NewEndArrow;
	SetEndArrow(pAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetEndArrow(EndArrowAttribute *pAttr,BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		pAttr - the object specifying the dash pattern to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current EndArrow according to an AttributeValue object, pushing
				the current EndArrow the context stack.
	SeeAlso:	RenderRegion::RestoreEndArrow

********************************************************************************************/

void RenderRegion::SetEndArrow(EndArrowAttribute *pAttr,BOOL Temp)
{
	// Set up the new attribute
	if (SaveAttribute(ATTR_ENDARROW, pAttr, Temp))
	{
		// The line attributes need to be reset before drawing anything.
		SetLineAttributes(CHANGEATTR_ENDARROW);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreEndArrow(EndArrowAttribute *pAttr,BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		pAttr - the object specifying the dash pattern to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current EndArrow, and restore the specified one.
	SeeAlso:	RenderRegion::SetEndArrow

********************************************************************************************/

void RenderRegion::RestoreEndArrow(EndArrowAttribute *pAttr,BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_ENDARROW, pAttr, Temp);

	// The line attributes need to be reset before drawing anything.
	SetLineAttributes(CHANGEATTR_ENDARROW);
}

/********************************************************************************************

>	void RenderRegion::SetMitreLimit(MILLIPOINT NewMitreLimit)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		NewMitreLimit, the new Arrow to draw on the end of lines.
	Purpose:	Set the current MitreLimit according to an AttributeValue object, pushing
				the current Arrow the context stack.
	SeeAlso:	RenderRegion::RestoreMitreLimit

********************************************************************************************/

void RenderRegion::SetMitreLimit(MILLIPOINT NewMitreLimit)
{
	// Don't bother if it doesn't cause a change
	MitreLimitAttribute *pAttr = (MitreLimitAttribute *) CurrentAttrs[ATTR_MITRELIMIT].pAttr;
	if ((pAttr != NULL) && (pAttr->MitreLimit == NewMitreLimit))
		return;

	// Create a temporary attribute and use it to set the Start Arrow.
	pAttr = new MitreLimitAttribute();
	pAttr->MitreLimit = NewMitreLimit;
	SetMitreLimit(pAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetMitreLimit(MitreLimitAttribute *pAttr,BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		pAttr - the object specifying the dash pattern to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current MitreLimit according to an AttributeValue object, pushing
				the current MitreLimit the context stack.
	SeeAlso:	RenderRegion::RestoreMitreLimit

********************************************************************************************/

void RenderRegion::SetMitreLimit(MitreLimitAttribute *pAttr,BOOL Temp)
{
	// Set up the new attribute
	if (SaveAttribute(ATTR_MITRELIMIT, pAttr, Temp))
	{
		// The line attributes need to be reset before drawing anything.
		SetLineAttributes(CHANGEATTR_MITRELIMIT);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreMitreLimit(MitreLimitAttribute *pAttr,BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		pAttr - the object specifying the dash pattern to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current MitreLimit, and restore the specified one.
	SeeAlso:	RenderRegion::SetMitreLimit

********************************************************************************************/

void RenderRegion::RestoreMitreLimit(MitreLimitAttribute *pAttr,BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_MITRELIMIT, pAttr, Temp);

	// The line attributes need to be reset before drawing anything.
	SetLineAttributes(CHANGEATTR_MITRELIMIT);
}




/********************************************************************************************

>	void RenderRegion::SetWindingRule(WindingType NewWindingRule)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		NewWindingRule - the winding rule to use.
	Purpose:	Set the winding rule, pushing the current winding rule onto the context 
				stack.
	SeeAlso:	RenderRegion::RestoreWindingRule

********************************************************************************************/

void RenderRegion::SetWindingRule(WindingType NewWindingRule)
{
	// Don't bother if it doesn't cause a change
	WindingRuleAttribute *pAttr = 
		(WindingRuleAttribute *) CurrentAttrs[ATTR_WINDINGRULE].pAttr;
	if ((pAttr != NULL) && (NewWindingRule == pAttr->WindingRule))
		return;

	// Create a temporary attribute and use it to set the drawing mode
	pAttr = new WindingRuleAttribute(NewWindingRule);
	SetWindingRule(pAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetWindingRule(WindingRuleAttribute *pAttr,BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the winding rule to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current winding rule according to an AttributeValue object, pushing
				the current winding rule onto the context stack.
	SeeAlso:	RenderRegion::RestoreWindingRule

********************************************************************************************/

void RenderRegion::SetWindingRule(WindingRuleAttribute *pAttr,BOOL Temp)
{
	// Save the current attribute
	if (SaveAttribute(ATTR_WINDINGRULE, pAttr, Temp))
	{
		// The fill attributes need to be reset before drawing anything.
		SetFillAttributes(CHANGEATTR_WINDING_RULE);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreWindingRule(WindingRuleAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the winding rule to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current winding rule, and restore the specified winding rule.
	SeeAlso:	RenderRegion::SetWindingRule

********************************************************************************************/

void RenderRegion::RestoreWindingRule(WindingRuleAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(ATTR_WINDINGRULE, pAttr, Temp);

	// The fill attributes need to be reset before drawing anything.
	SetFillAttributes(CHANGEATTR_WINDING_RULE);
}


/********************************************************************************************

>	void RenderRegion::SetDrawingMode(DrawModeType NewDrawingMode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		NewDrawingMode - the drawing mode to use.
	Purpose:	Set the drawing mode, pushing the current drawing mode onto the context 
				stack.
	SeeAlso:	RenderRegion::RestoreDrawingMode

********************************************************************************************/

void RenderRegion::SetDrawingMode(DrawModeType NewDrawingMode)
{
	// Don't bother if it doesn't cause a change
	DrawingModeAttribute *pAttr = 
		(DrawingModeAttribute *) CurrentAttrs[DrawingModeAttribute::ID].pAttr;
	if ((pAttr != NULL) && (NewDrawingMode == pAttr->DrawingMode))
		return;

	// Create a temporary attribute and use it to set the drawing mode
	pAttr = new DrawingModeAttribute(NewDrawingMode);
	SetDrawingMode(pAttr, TRUE);
}

/********************************************************************************************

>	void RenderRegion::SetDrawingMode(DrawingModeAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the drawing mode to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current drawing mode according to an AttributeValue object, pushing
				the current drawing mode onto the context stack.
	SeeAlso:	RenderRegion::RestoreDrawingMode

********************************************************************************************/

void RenderRegion::SetDrawingMode(DrawingModeAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute
	SaveAttribute(DrawingModeAttribute::ID, pAttr, Temp);
	DrawingMode = pAttr->DrawingMode;

	// The drawing mode needs to be set
	// MarkH - 12/11/99
	// First check to see if we`re dealing with a derived class or using the base class as
	// the base class ( RenderRegion ) 'SetOSDrawingMode()' is a pure virtual which will
	// ultimately crash in a big way if it is called!!!!
	if(!IS_A(this,RenderRegion))
	{
		SetOSDrawingMode();
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreDrawingMode(DrawingModeAttribute *pAttr, BOOL Temp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		pAttr - the object specifying the drawing mode to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current drawing mode, and restore the specified drawing mode.
	SeeAlso:	RenderRegion::SetDrawingMode

********************************************************************************************/

void RenderRegion::RestoreDrawingMode(DrawingModeAttribute *pAttr, BOOL Temp)
{
	// Set up the new attribute
	RestoreAttribute(DrawingModeAttribute::ID, pAttr, Temp);
	DrawingMode = pAttr->DrawingMode;

	// The drawing mode needs to be set
	// MarkH - 12/11/99
	// First check to see if we`re dealing with a derived class or using the base class as
	// the base class ( RenderRegion ) 'SetOSDrawingMode()' is a pure virtual which will
	// ultimately crash in a big way if it is called!!!!
	if(!IS_A(this,RenderRegion))
	{
		SetOSDrawingMode();
	}
}






/********************************************************************************************

>	void RenderRegion::SetTxtFontTypeface(TxtFontTypefaceAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current Text Font Typeface according to an AttributeValue object, 
				pushing the current Font Typeface onto the context stack.
	SeeAlso:	RenderRegion::RestoreTxtFontTypeface

********************************************************************************************/

void RenderRegion::SetTxtFontTypeface(TxtFontTypefaceAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTFONTTYPEFACE, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_FONT);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreTxtFontTypeface(TxtFontTypefaceAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current Text Font Typeface, and restore the specified 
				Typeface value.
	SeeAlso:	RenderRegion::SetTxtFontTypeface

********************************************************************************************/

void RenderRegion::RestoreTxtFontTypeface(TxtFontTypefaceAttribute *pAttr, BOOL Temp)
{
//	BOOL NewAttr = pAttr->HTypeface != RR_TXTFONTTYPEFACE();
	// Restore the Font Typeface attribute
	RestoreAttribute(ATTR_TXTFONTTYPEFACE, (AttributeValue*)pAttr, Temp);
//	if(NewAttr)
	SetTextAttributes(CHANGEATTR_FONT);
}

/********************************************************************************************

>	void RenderRegion::SetTxtBold(TxtBoldAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current Text Boldness according to an AttributeValue object, 
				pushing the current Boldness onto the context stack.
	SeeAlso:	RenderRegion::RestoreTxtBold

********************************************************************************************/

void RenderRegion::SetTxtBold(TxtBoldAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTBOLD, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_BOLD);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreTxtBold(TxtBoldAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current Text Boldness, and restore the specified 
				Boldness value.
	SeeAlso:	RenderRegion::SetTxtBold

********************************************************************************************/

void RenderRegion::RestoreTxtBold(TxtBoldAttribute *pAttr, BOOL Temp)
{
	// Restore the Text Bold attribute
	RestoreAttribute(ATTR_TXTBOLD, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_BOLD);
}


/********************************************************************************************

>	void RenderRegion::SetTxtItalic(TxtItalicAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current Text Italicness according to an AttributeValue object, 
				pushing the current Italicness onto the context stack.
	SeeAlso:	RenderRegion::RestoreTxtItalic

********************************************************************************************/

void RenderRegion::SetTxtItalic(TxtItalicAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTITALIC, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_ITALIC);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreTxtItalic(TxtItalicAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current Text Italicness, and restore the specified 
				Italicness value.
	SeeAlso:	RenderRegion::SetTxtItalic

********************************************************************************************/

void RenderRegion::RestoreTxtItalic(TxtItalicAttribute *pAttr, BOOL Temp)
{
	// Restore the Text Italic attribute
	RestoreAttribute(ATTR_TXTITALIC, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_ITALIC);
}


/********************************************************************************************

>	void RenderRegion::SetTxtFontSize(TxtFontSizeAttribute *pAttr, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the font size to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current Font Size according to an AttributeValue object, 
				pushing the current font size state onto the context stack.

********************************************************************************************/

void RenderRegion::SetTxtFontSize(TxtFontSizeAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTFONTSIZE, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_SIZE);
	}
}



/********************************************************************************************

>	void RenderRegion::RestoreTxtFontSize(TxtFontSizeAttribute *pAttr, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the font size 
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current font size, and restore the specified 
				font size
	SeeAlso:	RenderRegion::SetTxtFontSize

********************************************************************************************/

void RenderRegion::RestoreTxtFontSize(TxtFontSizeAttribute *pAttr, BOOL Temp)
{
	// Restore the Text Italic attribute
	RestoreAttribute(ATTR_TXTFONTSIZE, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_SIZE);
}




/********************************************************************************************

>	void RenderRegion::SetTxtUnderline(TxtUnderlineAttribute *pAttr, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current Text Underline state according to an AttributeValue object, 
				pushing the current Underline state onto the context stack.

********************************************************************************************/

void RenderRegion::SetTxtUnderline(TxtUnderlineAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTUNDERLINE, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_UNDERLINE);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreTxtUnderline(TxtItalicAttribute *pAttr, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current Text underline state, and restore the specified 
				underline value.
	SeeAlso:	RenderRegion::SetTxtUnderline

********************************************************************************************/

void RenderRegion::RestoreTxtUnderline(TxtUnderlineAttribute *pAttr, BOOL Temp)
{
	// Restore the Text Italic attribute
	RestoreAttribute(ATTR_TXTUNDERLINE, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_UNDERLINE);
}

/********************************************************************************************

>	void RenderRegion::SetTxtAspectRatio(TxtAspectRatioAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current Text Aspect Ratio according to an AttributeValue object, 
				pushing the current Aspect Ratio onto the context stack.
	SeeAlso:	RenderRegion::RestoreTxtAspectRatio

********************************************************************************************/

void RenderRegion::SetTxtAspectRatio(TxtAspectRatioAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTASPECTRATIO, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_ASPECT);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreTxtAspectRatio(TxtAspectRatioAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current Text Aspect Ratio, and restore the specified 
				Aspect Ratio value.
	SeeAlso:	RenderRegion::SetTxtAspectRatio

********************************************************************************************/

void RenderRegion::RestoreTxtAspectRatio(TxtAspectRatioAttribute *pAttr, BOOL Temp)
{
	// Restore the Text Aspect Ratio attribute
	RestoreAttribute(ATTR_TXTASPECTRATIO, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_ASPECT);
}

/********************************************************************************************

>	void RenderRegion::SetTxtJustification(TxtJustificationAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current Text Justification according to an AttributeValue object, 
				pushing the current Justification onto the context stack.
	SeeAlso:	RenderRegion::RestoreTxtJustification

********************************************************************************************/

void RenderRegion::SetTxtJustification(TxtJustificationAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTJUSTIFICATION, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_JUSTIFY);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreTxtJustification(TxtJustificationAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current Text Justification, and restore the specified 
				Justification value.
	SeeAlso:	RenderRegion::SetTxtJustification

********************************************************************************************/

void RenderRegion::RestoreTxtJustification(TxtJustificationAttribute *pAttr, BOOL Temp)
{
	// Restore the Text Justification attribute
	RestoreAttribute(ATTR_TXTJUSTIFICATION, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_JUSTIFY);
}

/********************************************************************************************

>	void RenderRegion::SetTxtTracking(TxtTrackingAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current Text Tracking according to an AttributeValue object, 
				pushing the current Tracking onto the context stack.
	SeeAlso:	RenderRegion::RestoreTxtTracking

********************************************************************************************/

void RenderRegion::SetTxtTracking(TxtTrackingAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTTRACKING, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_TRACKING);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreTxtTracking(TxtTrackingAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current Text Tracking, and restore the specified 
				Tracking value.
	SeeAlso:	RenderRegion::SetTxtTracking

********************************************************************************************/

void RenderRegion::RestoreTxtTracking(TxtTrackingAttribute *pAttr, BOOL Temp)
{
	// Restore the Text Tracking attribute
	RestoreAttribute(ATTR_TXTTRACKING, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_TRACKING);
}



/********************************************************************************************

>	void RenderRegion::SetTxtScript(TxtScriptAttribute *pAttr, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current Text Script according to an AttributeValue object, 
				pushing the current Script onto the context stack.
	SeeAlso:	RenderRegion::RestoreTxtScript

********************************************************************************************/

void RenderRegion::SetTxtScript(TxtScriptAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTSCRIPT, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_SCRIPT);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreTxtScript(TxtScriptAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current Text Script, and restore the specified 
				Script value.
	SeeAlso:	RenderRegion::SetTxtScript

********************************************************************************************/

void RenderRegion::RestoreTxtScript(TxtScriptAttribute *pAttr, BOOL Temp)
{
	// Restore the Text Script attribute
	RestoreAttribute(ATTR_TXTSCRIPT, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_SCRIPT);
}


/********************************************************************************************

>	void RenderRegion::SetTxtBaseLine(TxtBaseLineAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current Text BaseLine according to an AttributeValue object, 
				pushing the current BaseLine onto the context stack.
	SeeAlso:	RenderRegion::RestoreTxtBaseLine

********************************************************************************************/

void RenderRegion::SetTxtBaseLine(TxtBaseLineAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTBASELINE, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_BASELINE);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreTxtBaseLine(TxtBaseLineAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current Text BaseLine, and restore the specified 
				BaseLine value.
	SeeAlso:	RenderRegion::SetTxtBaseLine

********************************************************************************************/

void RenderRegion::RestoreTxtBaseLine(TxtBaseLineAttribute *pAttr, BOOL Temp)
{
	// Restore the Text BaseLine attribute
	RestoreAttribute(ATTR_TXTBASELINE, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_BASELINE);
}


/********************************************************************************************

>	void RenderRegion::SetTxtLineSpace(TxtLineSpaceAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Set the current Text LineSpace according to an AttributeValue object, 
				pushing the current LineSpace onto the context stack.
	SeeAlso:	RenderRegion::RestoreTxtLineSpace

********************************************************************************************/

void RenderRegion::SetTxtLineSpace(TxtLineSpaceAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTLINESPACE, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_LINESPACE);
	}
}

/********************************************************************************************

>	void RenderRegion::RestoreTxtLineSpace(TxtLineSpaceAttribute *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - the object specifying the colour to use.
				Tmp - indicates if pAttr points to a temporary attribute that should be
					  deleted when it's finished with.
	Purpose:	Get rid of the current Text LineSpace, and restore the specified 
				LineSpace value.
	SeeAlso:	RenderRegion::SetTxtLineSpace

********************************************************************************************/

void RenderRegion::RestoreTxtLineSpace(TxtLineSpaceAttribute *pAttr, BOOL Temp)
{
	// Restore the Text LineSpace attribute
	RestoreAttribute(ATTR_TXTLINESPACE, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_LINESPACE);
}

/********************************************************************************************

>	void RenderRegion::Set/RestoreTxtxxx(TxtXXXAttribute *pAttr, BOOL Temp)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	13/06/06
	Inputs:		See other Set/Restore routines above
	Purpose:	See other Set/Restore routines above

********************************************************************************************/

void RenderRegion::SetTxtLeftMargin(TxtLeftMarginAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTLEFTMARGIN, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_LEFTMARGIN);
	}
}

void RenderRegion::RestoreTxtLeftMargin(TxtLeftMarginAttribute *pAttr, BOOL Temp)
{
	// Restore the attribute
	RestoreAttribute(ATTR_TXTLEFTMARGIN, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_LEFTMARGIN);
}

void RenderRegion::SetTxtRightMargin(TxtRightMarginAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTRIGHTMARGIN, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_RIGHTMARGIN);
	}
}

void RenderRegion::RestoreTxtRightMargin(TxtRightMarginAttribute *pAttr, BOOL Temp)
{
	// Restore the attribute
	RestoreAttribute(ATTR_TXTRIGHTMARGIN, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_RIGHTMARGIN);
}

void RenderRegion::SetTxtFirstIndent(TxtFirstIndentAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTFIRSTINDENT, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_FIRSTINDENT);
	}
}

void RenderRegion::RestoreTxtFirstIndent(TxtFirstIndentAttribute *pAttr, BOOL Temp)
{
	// Restore the attribute
	RestoreAttribute(ATTR_TXTFIRSTINDENT, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_FIRSTINDENT);
}

void RenderRegion::SetTxtRuler(TxtRulerAttribute *pAttr, BOOL Temp)
{
	// Save the current attribute and set up the new one
	if (SaveAttribute(ATTR_TXTRULER, (AttributeValue*)pAttr, Temp))
	{
		SetTextAttributes(CHANGEATTR_RULER);
	}
}

void RenderRegion::RestoreTxtRuler(TxtRulerAttribute *pAttr, BOOL Temp)
{
	// Restore the attribute
	RestoreAttribute(ATTR_TXTRULER, (AttributeValue*)pAttr, Temp);
	SetTextAttributes(CHANGEATTR_RULER);
}

/********************************************************************************************
>	virtual void RenderRegion::SetOffscreen(OffscreenAttrValue* pAttr)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/2000
	Inputs:		pAttr	the offscreen attribute (eg AttrFeatherValue) to use.

	Purpose:	Uses attribute stacking to offscreen rendering system notified when
				offscreen bitmap has been rendered (ie occurs when subtree is finished
				rendering and attributes are being restored)

	SeeAlso:	RenderRegion::SaveAttribute;
********************************************************************************************/
/*void RenderRegion::SetOffscreen(OffscreenAttrValue* pAttr)
{
	// Push the current offscreen attribute onto the stack, and install the new attribute as
	// the 'current' one. (taken from RenderRegion::SaveAttribute)
	// NB I can't imagine a circumstance where you would want to render a Temp offscreen
	// attribute so I am forcing Temp = FALSE here
	if (TheStack.Push((AttributeValue*)CurrentOffscreenAttr, FALSE))
	{
		CurrentOffscreenAttr = pAttr;
	}
}*/

void RenderRegion::SetOffscreen(OffscreenAttrValue* pAttr)
{
//	TRACEUSER( "Gerry", _T("RenderRegion::SetOffscreen (0x%08x)\n"), pAttr);

	// Karim 23/07/2000
	// Mustn't have feathering overhead for hit-testing, so
	// don't bother doing anything if we're a hit-test RR.
	if (IsHitDetect())
		return;

	// Karim 23/07/2000
	// Don't bother with feathering at all if our quality setting is too low.
	// Note: this test should probably be made in a Feather override of a Node vfn.
	if (RRQuality.GetFillQuality() < Quality::Graduated)
		return;

	//---------------------------------------------
	// Karim 20/07/2000
	// Moved from OffscreenAttrValue::Render(), to avoid checks for render-region types
	// in the feathering code.
	// The following code works out and sets the offscreen attr's
	// offscreen bounding box.

	// Invalid area = intersection of node's and render-region's bounding rects.
	//
	// Notes:	1.	Inflate the node's bounding rect slightly, so it _completely_
	//				encloses the node (sometimes it doesn't quite).
	//			2.	Inflate the RR's clip-rect slightly, so that we don't get
	//				anti-aliasing edge artifacts in the final feather-bmp render.
	//			3.	If printing, then use the GRR's inner rect instead of its clipping
	//				rect, to fix stripey printing of feathers.
	//				This BODGE is necessary because for some reason CurrentClipRect
	//				does not cover a GRR's whole bitmap when printing !?!?
//	MILLIPOINT LargePixelWidth = (MILLIPOINT)ceil(GetScaledPixelWidthDouble());
	MILLIPOINT LargePixelWidth = GetScaledPixelWidth();
	DocRect NodeBB	= pAttr->GetLinkedNode()->GetBoundingRect();
	NodeBB.Inflate( LargePixelWidth );

	DocRect GRegionBB = CurrentClipRect;
// Removed by Phil 2/6/2004
// Seems to be bogus...
//		if (IsPrinting())
//			GRegionBB = InnerRect;
//		else
		GRegionBB.Inflate( 2 * LargePixelWidth );

	// If we should be checking the bounds
	if (CheckOffscreenBounds())
	{
		// Then check them
		if (!NodeBB.IsIntersectedWith(GRegionBB))
			return;
	}

	// calculate their intersection.
	DocRect drOffBB	= NodeBB.Intersection(GRegionBB);

	// Ensure that we have minimum pixel dimensions
	// Expand cliprect if more pixels required
	// NB expanding by GRR->ScaledPixelWidth will ensure we increase bmp size 
	// by exactly the no. of pixels required
	UINT32 RequiredWidth, RequiredHeight;
	pAttr->GetMinimumOffscreenBmpDimensions(&RequiredWidth,&RequiredHeight);
//	WinRect Rect = CalculateWinRect( drOffBB );
	WinRect Rect = OSRenderRegion::DocRectToWin( RenderMatrix, drOffBB, GetPixelsPerInch() );	// NOTE! Use of ScaledPixelWidth in GetPixelsPerInch!
	UINT32 ProposedWidth	= Rect.width;
	UINT32 ProposedHeight	= Rect.height;
	MILLIPOINT ScaledPixelWidth = GetScaledPixelWidth();

	// We have to check if the render matrix involves rotation as we have to
	// expand the DocRect in the other direction
	// First get the components of the matrix
	FIXED16 comps[4];
	RenderMatrix.GetComponents(comps, NULL);
	// It is rotated if a = d = 0 and b = -c
	BOOL bRotated = (comps[0] == FIXED16(0) && comps[3] == FIXED16(0) && comps[1] == -comps[2]);
	if(ProposedWidth < (UINT32)RequiredWidth)
	{
		// make the bitmap wider
		// For odd pixel differences, widen rightwards by extra pixel
		MILLIPOINT ExtraPixelWidth = (MILLIPOINT)(RequiredWidth - ProposedWidth);
		MILLIPOINT Decrement = (ExtraPixelWidth / 2) * ScaledPixelWidth;
		MILLIPOINT Increment = Decrement + ((ExtraPixelWidth % 2) * ScaledPixelWidth);

		// if there is a rotation involved we have to increase the height of the DocRect
		// otherwise we increase the width
		if (bRotated)
		{
			drOffBB.lo.y -= Decrement;
			drOffBB.hi.y += Increment;
		}
		else
		{
			drOffBB.lo.x -= Decrement;
			drOffBB.hi.x += Increment;
		}
	}

	if (ProposedHeight < (UINT32)RequiredHeight)
	{
		// make the bitmap taller
		// For odd pixel differences, stretch upwards by extra pixel
		MILLIPOINT ExtraPixelHeight = (MILLIPOINT)(RequiredHeight - ProposedHeight);
		MILLIPOINT Decrement = (ExtraPixelHeight / 2) * ScaledPixelWidth;
		MILLIPOINT Increment = Decrement + ((ExtraPixelHeight & 1) * ScaledPixelWidth);

		// if there is a rotation involved we have to increase the width of the DocRect
		// otherwise we increase the height
		if (bRotated)
		{
			drOffBB.lo.x -= Decrement;
			drOffBB.hi.x += Increment;
		}
		else
		{
			drOffBB.lo.y -= Decrement;
			drOffBB.hi.y += Increment;
		}
	}

	pAttr->SetOffscreenBoundingRect(drOffBB);

	//---------------------------------------------
	// Now startup the offscreen capture...
	// 
	// If a non-mix attribute is already in scope we must not start capturing transparently
	// because transparent bitmaps can't render non-mix transparencies correctly
	// (See SetTranspFillGeometry and ChangeCapture)
	//
	// If the Master Capture is itself Transparent, then there's nothing we can do to grab
	// any non-mix transparencies correctly so test that first (typical case: Create Bitmap Copy)
	//
	BOOL bStartTransparently = FALSE;
	if (LockedTransparentCaptures())
		bStartTransparently = TRUE;
	else
	{
		TranspFillAttribute* pTransAttr = (TranspFillAttribute *) GetCurrentAttribute(ATTR_TRANSPFILLGEOMETRY);
		UINT32 ttype = pTransAttr->GetTranspType();
		bStartTransparently = (ttype==TT_NoTranspType || ttype==TT_Mix);
		// Also, scan down into the tree and see whether this node has any non-mix that would
		// cause us to revert to non-transparent capture
		// Because it's (probably) quicker to scan the tree than to revert during rendering...
		// Urgh.
		NodeRenderableInk* pLinkedNode = pAttr->GetLinkedNode()->IsAnObject() ? (NodeRenderableInk*)pAttr->GetLinkedNode() : NULL;
		bStartTransparently = bStartTransparently && !(pLinkedNode && pLinkedNode->ContainsNonMixTransparency(this));
	}

	Capture* pCapture = StartCapture(pAttr,
									drOffBB,
									CAPTUREINFO(ctNESTABLE, cfNONE + cfDEBUG),		// Don't allow collapse to master bitmap, CollapseToMaster=0
									bStartTransparently,
									!bStartTransparently
									);
	if (pCapture==NULL)
		return;

	// Save pointer to OffscreenAttr, and move current OffscreenAttr to the attr context stack
//	RenderRegion::SetOffscreen(pAttr);

	// Push the current offscreen attribute onto the stack, and install the new attribute as
	// the 'current' one. (taken from RenderRegion::SaveAttribute)
	// NB I can't imagine a circumstance where you would want to render a Temp offscreen
	// attribute so I am forcing Temp = FALSE here
	if (TheStack.Push((AttributeValue*)CurrentOffscreenAttr, FALSE))
		CurrentOffscreenAttr = pAttr;
}



/********************************************************************************************
>	virtual void RenderRegion::RestoreOffscreen(OffscreenAttrValue* pAttr)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/2000
	Inputs:		pAttr	- the object specifying the new feather
	Purpose:	Inform current offscreen attribute that its offscreen bmp has been rendered,
				and restore the one supplied
	SeeAlso:	RenderRegion::RestoreAttribute
********************************************************************************************/
/*void RenderRegion::RestoreOffscreen(OffscreenAttrValue* pAttr)
{
	// Restore the old attribute.
	CurrentOffscreenAttr = pAttr; 
}*/



void RenderRegion::RestoreOffscreen(OffscreenAttrValue* pAttr)
{
	// End this capture:
	// Don't render captured bitmap back into render region - owner will do that after processing
	// Release bitmap here - if this region is being reset
	if (!IsBeingReset)
	{
		LPBITMAPINFO lpCapturedBitmapInfo = NULL;
		LPBYTE lpCapturedBits = NULL;
		DocRect CaptureRect = CurrentOffscreenAttr->GetOffscreenBoundingRect();	// Initialise to size of feathered object
																// so that StopCapture can return sensible 24BPP bitmap if needed
		Capture* pTopCapture = GetTopCapture();
		ENSURE(pTopCapture && pTopCapture->GetOwner()==CurrentOffscreenAttr, "RestoreOffscreen attr is not owned by top capture\n");
		if (pTopCapture && pTopCapture->GetOwner()==CurrentOffscreenAttr)
		{
			StopCapture(CurrentOffscreenAttr, FALSE, IsBeingReset, &lpCapturedBitmapInfo, &lpCapturedBits, &CaptureRect);
		}

		// Normal restore in process of traversing the tree
		// So tell the offscreen attribute to handle the captured bitmap as it sees fit...
		CurrentOffscreenAttr->OffscreenRenderingCompleted(this, lpCapturedBitmapInfo, lpCapturedBits, CaptureRect);
	}

//	RenderRegion::RestoreOffscreen(pAttr);

	CurrentOffscreenAttr = pAttr; 
}


/********************************************************************************************

>	void RenderRegion::SetClipRegion(ClipRegionAttribute* pAttr, BOOL Temp)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11 April 2000
	Inputs:		pAttr	the new ClipRegionAttribute.
				Temp	whether the new attribute is temporary, and should therefore be
						deleted when it is finished with.
	Purpose:	Set this render-region's current clipping region.
	See also:	ClipRegionAttribute::Render(), RenderRegion::RestoreClipRegion().

********************************************************************************************/
void RenderRegion::SetClipRegion(ClipRegionAttribute* pAttr, BOOL Temp)
{
	SaveAttribute(ATTR_CLIPREGION, (AttributeValue *) pAttr, Temp);
}



/********************************************************************************************

>	void RenderRegion::RestoreClipRegion(ClipRegionAttribute* pAttr, BOOL Temp)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11 April 2000
	Inputs:		pAttr	the new ClipRegionAttribute.
				Temp	whether the new attribute is temporary, and should therefore be
						deleted when it is finished with.
	Purpose:	Restore this render-region's current clipping region, using the attribute 
				supplied.
	See also:	ClipRegionAttribute::Restore(), RenderRegion::SetClipRegion().

********************************************************************************************/
void RenderRegion::RestoreClipRegion(ClipRegionAttribute* pAttr, BOOL Temp)
{
	RestoreAttribute(ATTR_CLIPREGION, (AttributeValue *) pAttr, Temp);
}



/********************************************************************************************

>	virtual void RenderRegion::DrawFixedSystemText(StringBase *TheText,
														DocRect &BoundsRect, UINT32 uFormat)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		TheText - The string to render
				BoundsRect - Determines where the text is to be plotted. The text will also
				be clipped within this bounding rectangle if it exceeds the available space.

	Purpose:	To draw simple text, using the default host-operating-system font.
				The size and style of this font are decided by the host OS (or oil code)
				and cannot be set in any way. To determine how much space is needed to
				display a string with this method, see the SeeAlso.

	Notes:		THIS BASE CLASS METHOD IS UNSUPPORTED! DO NOT CALL IT!
				See OSRenderRegion for the supported versions of this method

	SeeAlso:	OSRenderRegion::DrawFixedSystemText
	SeeAlso:	OSRenderRegion::GetFixedSystemTextSize

********************************************************************************************/

void RenderRegion::DrawFixedSystemText(StringBase *TheText, DocRect &BoundsRect, UINT32 uFormat)
{
	ERROR2RAW("Attempt to call unsupported RenderRegion::DrawFixedSystemText method");
}



/********************************************************************************************

>	virtual void RenderRegion::SetFixedSystemTextColours(DocColour *TextCol,
														 DocColour *Background)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		TextCol - The foreground (text) colour (may be NULL)
				Background - The background colour (may be NULL)

	Purpose:	To set the text and background colour(s) for any FixedSystem text rendered
				in this render region in the future (within the current rendering pass).

	Notes:		THIS BASE CLASS METHOD IS UNSUPPORTED! DO NOT CALL IT!
				See OSRenderRegion for the supported versions of this method

				If either of the colours is passed in as a NULL pointer, that colour will
				not be set, and will remain at the previous setting.

				Note that these values are passed directly to the host OS at the point
				of calling, so you must call this method every time the colour is changed
				(i.e. don't rely on just changing the DocColours that you passed in
				originally, as they aren't remembered in any way). Note also that if you
				call DrawFixedSystemText without a prior call to this method, the colours
				used are undefined.

	SeeAlso:	OSRenderRegion::DrawFixedSystemText

********************************************************************************************/

void RenderRegion::SetFixedSystemTextColours(DocColour *TextCol, DocColour *Background)
{
	ERROR2RAW("Attempt to call unsupported RenderRegion::SetFixedSystemTextColours method");
}



/********************************************************************************************

>	virtual void RenderRegion::GetFixedSystemTextSize(StringBase *TheText,
														DocRect *BoundsRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		TheText - The string to get the rendered size of

	Outputs:	BoundsRect - Returned with the size of the rectangle needed to display
				said text string. This rect always has one corner at (0,0)
	Returns:	-

	Purpose:	To determine how much room is needed to plot a bit of text with
				RenderRegion::DrawFixedSystemText

	Notes:		THIS BASE CLASS METHOD IS UNSUPPORTED! DO NOT CALL IT!
				See OSRenderRegion for the supported versions of this method

	SeeAlso:	OSRenderRegion::DrawFixedSystemText
	SeeAlso:	OSRenderRegion::GetFixedSystemTextSize

********************************************************************************************/

void RenderRegion::GetFixedSystemTextSize(StringBase *TheText, DocRect *BoundsRect, double* atDpi)
{
	if (BoundsRect != NULL)
		*BoundsRect = DocRect(0,0,0,0);

	ERROR2RAW("Attempt to call unsupported RenderRegion::GetFixedSystemTextSize method");
}



/********************************************************************************************

>	void RenderRegion::DrawPath(Path *pPath, PathProcessor *pCaller = NULL, PathShape shapePath=PATHSHAPE_PATH)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/96
	Inputs:		pPath	- Points to the path to be rendered
				pCaller	- NULL, or if you're calling this from a path processor,
						  pass your 'this' pointer in here to support the recursion
				shapePath - (Graham 14/4/97)
							A tag which describes what shape the path is.

							For example, if shapeThis=PATHSHAPE_RECTANGLE, then
							RenderRegions that want to treat rectangles
							differently can do so.

							Ideally we should replace this so that rectangles
							are rendered by calling the DrawRect function, circles
							by calling the DrawCircle function, and so on.

	Purpose:	Renders a path (main entry-point).
				May start a binary-recursive rendering pipeline off by calling
				any active path processors to do their stuff.

	SeeAlso:	RenderRegion::DrawPathToOutputDevice

********************************************************************************************/

void RenderRegion::DrawPath(Path *pPath, PathProcessor *pCaller, PathShape shapePath)
{
	ERROR3IF(pPath == NULL, "Illegal NULL param");

	PathProcessor *pNextProcessor = NULL;
	if (pCaller == NULL)
	{
		pNextProcessor = PathProcessorStack;					// First call
	}
	else
	{
		pNextProcessor = pCaller->GetNextProcessor();			// Recursive call
	}

	// see if any of our processors actually do anything
	while (pNextProcessor != NULL && (!pNextProcessor->DoesActuallyDoAnything(this)))
	{
		pNextProcessor = pNextProcessor->GetNextProcessor();
	}

	// we seem to be getting problems with two path processor brushes on the stack
	// if we blend between a brush and a bevel
	if (pNextProcessor != NULL && pCaller != NULL &&
		pNextProcessor->IsAPathProcessorBrush() && pCaller->IsAPathProcessorBrush())
	{
		BrushHandle ThisHandle = ((PathProcessorBrush*)pCaller)->GetBrushDefinitionHandle();
		BrushHandle NextHandle = ((PathProcessorBrush*)pNextProcessor)->GetBrushDefinitionHandle();
		if (NextHandle == ThisHandle)
			pNextProcessor = NULL;
	}

	// If we found another PathProcessor to call, recursively call it
	if (pNextProcessor != NULL)
	{
		// Does the processor change both fill & stroke independently?
		if (pNextProcessor->WillChangeFillAndStrokeSeparately())
		{
			// Remember current path settings - we have to poke it directly
			// to be efficient about changing it.
			BOOL WasFilled	= pPath->IsFilled;
			BOOL WasStroked	= pPath->IsStroked;

			// Call the PathProcessor to only fill the path, if it is filled.
			// There's a hideous special case in here for non-contoned bitmaps (which have a 
			// fill colour of TRANSPARENT! Arrrrgh!)
			if (WasFilled &&
				(!RR_FILLCOLOUR().IsTransparent() ||
				 ((ColourFillAttribute   *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr)->IsABitmapFill()
				)
			   )
			{
				pPath->IsStroked = FALSE;
				pPath->IsFilled = TRUE;
				pNextProcessor->ProcessPath(pPath, this, shapePath);
			}

			// And then call it again to only stroke the path
			if (WasStroked && !RR_STROKECOLOUR().IsTransparent())
			{
				pPath->IsStroked = TRUE;
				pPath->IsFilled = FALSE;
				pNextProcessor->ProcessPath(pPath, this, shapePath);
			}
			
			// Restore previous path settings
			pPath->IsFilled  = WasFilled;
			pPath->IsStroked = WasStroked;
			return;
		}

		// It can be passed through as a single path
		pNextProcessor->ProcessPath(pPath, this, shapePath);
		return;
	}


	// We have now called any active path processors, so simply output the final
	// path to the output device

	//Graham 14/4/97: Also pass along the PathShape parameter to let the renderregion
	//know if the path is a special shape
	DrawPathToOutputDevice(pPath, shapePath);
}



/********************************************************************************************

>	void RenderRegion::PushPathProcessor(PathProcessor *pProcessor)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96
	Inputs:		pProcessor - the processor to push onto the processor stack
				THIS PROCESSOR IS NOW OWNED BY THE RENDER REGION and will be
				deleted when it is popped.

	Purpose:	Adds a PathProcessor to the rendering PathProcessor stack

	Notes:		The render region must be supplied a new PathProcessor every
				time you push one, because PathProcessors generally have an
				internal state, and this must be preserved when background
				rendering or multi-threading causes the same thing to be 
				rendered "simultaneously".

	SeeAlso:	PathProcessor; RenderRegion::PopPathProcessor

********************************************************************************************/

void RenderRegion::PushPathProcessor(PathProcessor *pProcessor)
{
	ERROR3IF(pProcessor == NULL, "Illegal NULL param");


#if _DEBUG
	// BLOCK
	{
		// In debug builds, scan the entire PathProcessor stack, checking that this
		// processor is ot already in it.
		ERROR3IF(pProcessor->GetNextProcessor() != NULL,
					"PushPathProcessor - processor is already in a stack");
		PathProcessor *pPP = PathProcessorStack;
		while (pPP != NULL)
		{
//			TRACEUSER( "Jason", _T("  "));		// More indent for each processor on the stack
			ERROR3IF(pPP == pProcessor, "PushPathProcessor - Processor pushed TWICE!");
			pPP = pPP->GetNextProcessor();
		}
	}

//	TRACEUSER( "Jason", _T("  >> PushPathProcessor %x\n"), pProcessor);
#endif

	if (pProcessor != NULL)
	{
		pProcessor->SetNextProcessor(PathProcessorStack);		// Make TOS the second
		PathProcessorStack = pProcessor;						// And point TOS at new item
	}
}



/********************************************************************************************

>	void RenderRegion::PopPathProcessor(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96

	Purpose:	Pops a PathProcessor from the top of the rendering PathProcessor stack

	Notes:		The PathProcessor is deleted after being popped.

	SeeAlso:	PathProcessor; RenderRegion::PushPathProcessor

********************************************************************************************/

void RenderRegion::PopPathProcessor(void)
{
#if _DEBUG
	// BLOCK
	{
		PathProcessor *pPP = PathProcessorStack;
		while (pPP != NULL)
		{
//			TRACEUSER( "Jason", _T("  "));		// More indent for each processor on the stack
			pPP = pPP->GetNextProcessor();
		}

//		TRACEUSER( "Jason", _T("<< PopPathProcessor %x\n"), PathProcessorStack);
	}
#endif


	PathProcessor *TOS = PathProcessorStack;

	if (TOS != NULL)
	{
		PathProcessorStack = TOS->GetNextProcessor();	// Link TOS to 2nd processor in chain
		TOS->SetNextProcessor(NULL);					// And make the popped one "clean"

		// Finally, delete the path processor
		delete TOS;
	}
}

/********************************************************************************************

>	void RenderRegion::EnableAllBrushPathProcessors(BOOL Enable)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96
	Inputs:		Enable - whether we wish to enable the processors or not

	Purpose:	Enables or disables all the brush path processors on the stack.

********************************************************************************************/

void RenderRegion::EnableAllBrushPathProcessors(BOOL Enable)
{
	PathProcessor* pPP = PathProcessorStack;

	while (pPP != NULL)
	{
		if (pPP->IsAPathProcessorBrush())
			pPP->SetDisabled(!Enable);
		pPP = pPP->GetNextProcessor();
	}
}

/********************************************************************************************

>	BOOL RenderRegion::DrawPathArrowHeads(DocCoord* Coords, PathVerb* Verbs, INT32 NumCoords)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		The path to render the arrow heads onto.
	Purpose:	Draws arrows heads on the start or end of an open path,
				according to the current ArrowHead Attributes.
	SeeAlso:	-

********************************************************************************************/

BOOL RenderRegion::DrawPathArrowHeads(DocCoord* Coords, PathVerb* Verbs, INT32 NumCoords)
{
	BOOL ArrowRendered = FALSE;

	if (!RR_STARTARROW().IsNullArrow())
	{
		INT32 PathIndex = 0;
		DocCoord ArrowCentre;
		DocCoord ArrowDirection;
		DocCoord ArrowBase;

		// Find the first position of an ArrowHead
		BOOL GotPos = ArrowRec::GetFirstArrowPos(TRUE,
												 Coords, Verbs, NumCoords, 
									   			 &PathIndex, &ArrowCentre, &ArrowDirection);

		while (GotPos)
		{
			DrawArrowHead(RR_STARTARROW(), ArrowCentre, ArrowDirection, &ArrowBase);

			// DMc Update 25/2/99 so that arrows are drawn on the path - not after it !
			// find the coord which caused the arrow to be drawn
			BOOL bFound = FALSE;

			INT32 i;
			for (i = PathIndex - 1  ; i >= 0 && !bFound; i--)
			{
				if (Verbs[i] == PT_MOVETO)
				{
					bFound = TRUE;
				}
			}

			if (bFound && !(RR_STARTARROW().m_bExtendPath))
			{
				// move the coord in the path back by arrow size
				Coords[i+1].x = ArrowBase.x;
				Coords[i+1].y = ArrowBase.y;						
			}

			ArrowRendered = TRUE;

			// Find the next Arrow position (if there are any more subpaths)
			GotPos = ArrowRec::GetNextArrowPos(TRUE,
									 		   Coords, Verbs, NumCoords, 
								     		   &PathIndex, &ArrowCentre, &ArrowDirection);
		}
	}

	if (!RR_ENDARROW().IsNullArrow())
	{
		INT32 PathIndex = 0;
		DocCoord ArrowCentre;
		DocCoord ArrowDirection;
		DocCoord ArrowBase;

		// Find the first position of an ArrowHead
		BOOL GotPos = ArrowRec::GetFirstArrowPos(FALSE,
												 Coords, Verbs, NumCoords, 
									   			 &PathIndex, &ArrowCentre, &ArrowDirection);
		while (GotPos)
		{
			DrawArrowHead(RR_ENDARROW(), ArrowCentre, ArrowDirection, &ArrowBase);

			// move the coordinate on the path
			if (!(RR_ENDARROW().m_bExtendPath))
			{
				Coords[PathIndex-1].x = ArrowBase.x;
				Coords[PathIndex-1].y = ArrowBase.y;
			}

			ArrowRendered = TRUE;

			// Find the next Arrow position (if there are any more subpaths)
			GotPos = ArrowRec::GetNextArrowPos(FALSE,
									 		   Coords, Verbs, NumCoords, 
								     		   &PathIndex, &ArrowCentre, &ArrowDirection);
		}
	}

	return ArrowRendered;
}

/********************************************************************************************

>	BOOL RenderRegion::DrawArrowHead(ArrowRec &ArrowToDraw,
									 DocCoord &Centre, DocCoord &Direction)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		An ArrowRec defining the Arrowhead, the Centre point of the Arrow and a 
				point defining the direction of the Arrow.
	Purpose:	Draw an Arrow head on the start or end of a line.
	SeeAlso:	-

********************************************************************************************/

BOOL RenderRegion::DrawArrowHead(ArrowRec &ArrowToDraw,
								 DocCoord &Centre, DocCoord &Direction, DocCoord * RetnCentre)
{
	// Get a matrix to transform the Arrow
	Trans2DMatrix Trans;
	ArrowToDraw.GetArrowMatrix(Centre, Direction, RR_LINEWIDTH(), &Trans);

	// Get the current Render Matrix, and merge the arrow head
	// matrix with it.
//	Matrix NewRenderMatrix = Trans.GetMatrix();
//	NewRenderMatrix *= RenderMatrix;

	Path* TransPath = new Path();
	if (TransPath == NULL)
		return FALSE;

	BOOL 	ok = TransPath->Initialise(ArrowToDraw.ArrowShape->GetNumCoords());
	if (ok) ok = TransPath->CopyPathDataFrom(ArrowToDraw.ArrowShape);

	if (!ok)
	{
		// Tidy up if we failed
		delete TransPath;
		return FALSE;
	}

	// calculate the start point of the arrow & put it into RetnCentre
	DocCoord Base;
	ArrowToDraw.GetArrowBase(&Base);
	
	Trans.Transform(&Base, 1);

	if (RetnCentre)
	{
		*RetnCentre = Base;
	}

	Trans.Transform(TransPath->GetCoordArray(), 
					TransPath->GetNumCoords() );

    SaveContext();	// We have to change the Line and Fill colours

	// First make sure we don't try and draw Arrows on our Arrow heads !!!
	BOOL OldStartArrowState = RR_STARTARROW().IsNull;				
	BOOL OldEndArrowState 	= RR_ENDARROW().IsNull;				
	RR_STARTARROW().IsNull = TRUE;				
    RR_ENDARROW().IsNull   = TRUE;			

	// Set the fill colour to the line colour and the line colour to transparent
    SetFillColour(RR_STROKECOLOUR());
	SetLineColour(COLOUR_TRANS);

	// Now set the Fill transparency to be a flat transparency, the
	// same as the current line transparency.
	FlatTranspFillAttribute* pAttr = new FlatTranspFillAttribute;
	pAttr->SetStartTransp(RR_STROKETRANSP()->GetStartTransp());
	pAttr->SetTranspType(RR_STROKETRANSP()->GetTranspType());
	SetTranspFillGeometry(pAttr, TRUE);

	// Make sure the Arrow Path is valid.
//	ArrowToDraw.ArrowShape->IsFilled  = TRUE;
//	ArrowToDraw.ArrowShape->IsStroked = FALSE;
	TransPath->IsFilled  = TRUE;
	TransPath->IsStroked = FALSE;

	// Draw the Arrow head
//	DrawPath(ArrowToDraw.ArrowShape);

	DrawPath(TransPath);

	RR_STARTARROW().IsNull = OldStartArrowState;				
    RR_ENDARROW().IsNull   = OldEndArrowState;			

	RestoreContext();	// Get our Attributes Back.

	// Restore the old Rendering Matrix

	delete TransPath;

	return TRUE;
}


BOOL RenderRegion::DrawTransformedBitmap(NodeBitmap *pNodeBitmap)
{
	// If we are not drawing complex shapes and this shape is, then return
	RRCaps Caps;
	GetRenderRegionCaps(&Caps);
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return TRUE;

	if ((!RenderComplexShapes) && (pNodeBitmap->NeedsTransparency()))
		return TRUE;

	KernelBitmap* pkBitmap = pNodeBitmap->GetBitmap();
	if (pkBitmap==NULL || pkBitmap->GetWidth()==0 || pkBitmap->GetHeight()==0)
		return TRUE;

	// By default, we can't do arbitrarily transformed bitmaps - use a bitmap fill.
	SaveContext();

	// No lines on the rectangle
	SetLineColour(COLOUR_TRANS);
	SetLineWidth(0);

	if (RRQuality.GetFillQuality() == Quality::Bitmaps)
	{
		// The quality is set to show Bitmaps, with everything else in outline
		// mode, so we'll fake this by temorarily setting the quality to 'Solid'
		// just while we render this node.

		Quality NewQuality;
		NewQuality.SetQuality(31);	// Force solid fills while we render this node

		QualityAttribute* pQualityAttr = new QualityAttribute(NewQuality);
		SetQuality(pQualityAttr, TRUE);
	}

	// Simple bitmap fill which fills the whole shape
	BitmapFillAttribute *pBitmapAttr = new BitmapFillAttribute;
	pBitmapAttr->GetBitmapRef()->SetBitmap(pNodeBitmap->GetBitmap());

	if (IsVeryMono() && !pNodeBitmap->NeedsTransparency())
	{
		// VeryMono, so plot as black (unless it's a masked bitmap)
		DocColour ColBlack = COLOUR_BLACK;
		pBitmapAttr->SetStartColour(&ColBlack);
	}

	// (ChrisG - 7/11/00) Removed as the CanBeContoned bool is not supported. 
	//	This default value was stopping derived render regions from overriding
	//	this function, so almost all bitmaps were drawn by this code...
	// Err no ChrisG the header file used to be CanBeContoned = TRUE
	// so we DO RUN THIS CODE all the time ie all bitmaps CAN be contoned (sjk 19/11/00)
	else // if(CanBeContoned)
	{
		if (pNodeBitmap->GetStartColour())
			pBitmapAttr->SetStartColour(pNodeBitmap->GetStartColour());
	
		if (pNodeBitmap->GetEndColour())
			pBitmapAttr->SetEndColour(pNodeBitmap->GetEndColour());
	}

	DocCoord *Coords = pNodeBitmap->InkPath.GetCoordArray();

	pBitmapAttr->StartPoint = Coords[3];
	pBitmapAttr->EndPoint   = Coords[2];
	pBitmapAttr->EndPoint2 	= Coords[0];

	// Set bitmap attribute, and get the render region to throw it away when it's finished
	// with (hence the TRUE parameter).
	SetFillGeometry(pBitmapAttr, TRUE);

	// Set the mapping to have no repeat, otherwise we get artifacts at the edges when
	// anti-aliasing is enabled (see bug 1391).
	FillMappingLinearAttribute *pNoRepeatAttr = new FillMappingLinearAttribute;

	// Repeat value of '3' means 'repeat inverted' (there are no symbolic names for
	// these repeat values - shout at Will if you want some!).
//	pNoRepeatAttr->Repeat = 3;
	pNoRepeatAttr->Repeat = 0;

	// Set mapping attribute, and get the render region to throw it away when it's finished
	// with (hence the TRUE parameter).
	SetFillMapping(pNoRepeatAttr, TRUE);

	// Draw the bitmap by rendering a bitmap filled path.
	pNodeBitmap->InkPath.IsFilled = TRUE;
	DrawPath(&pNodeBitmap->InkPath);

	RestoreContext();

	// All done
	return TRUE;
}

/********************************************************************************************

>	EFFECTTYPE RenderRegion::GetFillEffect()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/94
	Returns:	TRUE, if should use HSV fades.
				FALSE, if should use RGB fades.
	Purpose:	Decide what type of colour fade to use.
	Scope:		Public

********************************************************************************************/

EFFECTTYPE RenderRegion::GetFillEffect()
{
	// Get the current Fill Effect
	FillEffectAttribute* FillEffect = RR_FILLEFFECT();

	// What kind of fill effect is this ?
	CCRuntimeClass* Effect = FillEffect->GetRuntimeClass();

	if (Effect == CC_RUNTIME_CLASS(FillEffectFadeAttribute))
		return EFFECT_RGB;			// Use RGB space

	if (Effect == CC_RUNTIME_CLASS(FillEffectRainbowAttribute))
		return EFFECT_HSV_SHORT;	// Use HSV short path

	if (Effect == CC_RUNTIME_CLASS(FillEffectAltRainbowAttribute))
		return EFFECT_HSV_LONG;		// Use HSV INT32 path

	return EFFECT_RGB;				// Default to RGB space	
}



/********************************************************************************************

>	void RenderRegion::DrawDragBounds(DocRect *RectToRender)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95

	Inputs:		RectToRender - The bounding rectangle we wish to render.

	Purpose:	Used to draw a "rectangle" to screen (for dragging). However, the corners of
				the rectangle are passed through the current rendering matrix, so the rect
				may well be rotated or skewed by the time it reaches the screen.

	Notes:		DO NOT confuse this method with DrawDragRect (which will draw a rectangle
				without skew or rotation)

				OSRenderRegions override this method to provide dotted-line redraw of the
				bounds (note: Not a dash-pattern, a simple on/off pixel dottyness). Note that
				OSRenderRegions CALL THIS METHOD to actually draw the path, so taqke care if
				changing this function.

	SeeAlso:	OSRenderRegion::DrawDragRect; OSRenderRegion::DrawDragBounds;
				Range::RenderXOROutlinesOn

********************************************************************************************/

void RenderRegion::DrawDragBounds(DocRect *RectToRender)
{
	Path SquarePath;
	SquarePath.Initialise(12, 12);
	SquarePath.FindStartOfPath();

	SquarePath.InsertMoveTo(RectToRender->lo);
	SquarePath.InsertLineTo(DocCoord(RectToRender->hi.x, RectToRender->lo.y));
	SquarePath.InsertLineTo(RectToRender->hi);
	SquarePath.InsertLineTo(DocCoord(RectToRender->lo.x, RectToRender->hi.y));
	SquarePath.InsertLineTo(RectToRender->lo);
	SquarePath.IsFilled = FALSE;

	DrawPath(&SquarePath);
}

/********************************************************************************************

>	void RenderRegion::DrawDashLine(const DocCoord &StartPoint, const DocCoord &EndPoint)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		StartPoint  - start of the line
				EndPoint	- end of the line
	Purpose:	Draws a dashed line between the two points.
				Uses the technology from DrawDragBounds()

				NOTE: This is the base class implementation that only partially works.
						Only designed to work correctly in OSRenderRegions

	SeeAlso:	OSRenderRegion::DrawDashLine()

********************************************************************************************/

void RenderRegion::DrawDashLine(const DocCoord &StartPoint, const DocCoord &EndPoint)
{
	Path ThePath;
	ThePath.Initialise(12, 12);
	ThePath.FindStartOfPath();

	ThePath.InsertMoveTo(StartPoint);
	ThePath.InsertLineTo(EndPoint);
	ThePath.IsFilled = FALSE;

	DrawPath(&ThePath);
}

/********************************************************************************************

>	virtual void RenderRegion::DrawPixelRect(DocRect *RectToRender)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95
	Inputs:		RectToRender - The rectangle we wish to render.
	Outputs:	-
	Returns:	-
	Purpose:	To try and plot a single pixel rectangle outline. It does this by using filled
				rectangles rather than just drawing the line itself. This is required because
				although GDI and GDraw fills can be made to match up, their outlines can't
				(at the time of writing).
				Assumes fill colour and line colour have already been set up along with a line
				width, usually assumed to be zero which means plot single-pixel lines. Usually,
				the fill colour is set to be the required line colour and the line colour is
				set to be transparent (COLOUR_TRANS).
	SeeAlso:	OSRenderRegion::DrawRect; GRenderRegion::DrawRect; RenderRegion::DrawPixelLine;

********************************************************************************************/

void RenderRegion::DrawPixelRect(DocRect *RectToRender)
{
// Below is some conditional code.
// The correct code is directly below in the IF clause and just in fact calls the standard
// DrawRect call.
// The bodged version is the code in the ELSE clause and renders the single pixel outline
// using filled rectangles. This is required because although GDI and GDraw fills
// can be made to match up, their outlines can't (at the time of writing).
//
// By the way, Gavin says, it's faster to plot four upright rects than to stroke a path
// to become the outline of the page!!!
//
#if 0
		// Draw that page rectangle	using the standard call
    	DrawRect(RectToRender); 
#else
		// Get the scaled pixel size for the view
		FIXED16 ScaledPixelWidth,
				ScaledPixelHeight;
		RenderView->GetScaledPixelSize(&ScaledPixelWidth, &ScaledPixelHeight);

		// Draw a rectangle using 4 filled rectangles.
		DocRect VertRect(RectToRender->lo.x,
						 RectToRender->lo.y,
						 RectToRender->lo.x + ScaledPixelWidth.MakeLong(),
						 RectToRender->hi.y + ScaledPixelHeight.MakeLong());
		DrawRect(&VertRect);				// Left

		VertRect.Translate(RectToRender->Width(),0);
		DrawRect(&VertRect);				// Right

		DocRect HorzRect(RectToRender->lo.x,
						 RectToRender->lo.y,
						 RectToRender->hi.x,
						 RectToRender->lo.y + ScaledPixelHeight.MakeLong());
		DrawRect(&HorzRect);				// Bottom

		HorzRect.Translate(0,RectToRender->Height());
		DrawRect(&HorzRect);				// Top
#endif
}

/********************************************************************************************

>	virtual void RenderRegion::DrawPixelLine(const DocCoord &StartPoint, const DocCoord &EndPoint)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95
	Inputs:		StartPoint - The starting point to render from.
				EndPoint - The end point to render to.
	Outputs:	-
	Returns:	-
	Purpose:	To try and plot a single pixel line.  It does this by using a filled
				rectangle rather than just drawing the line itself. This is required because
				although GDI and GDraw fills can be made to match up, their outlines can't
				(at the time of writing).
				Assumes fill colour and line colour have already been set up along with a line
				width, usually assumed to be zero which means plot single-pixel lines. Usually,
				the fill colour is set to be the required line colour and the line colour is
				set to be transparent (COLOUR_TRANS).
	SeeAlso:	OSRenderRegion::DrawLine; GRenderRegion::DrawLine; RenderRegion::DrawPixelRect;

********************************************************************************************/
void RenderRegion::DrawPixelLine(const DocCoord &StartPoint, const DocCoord &EndPoint)
{
// Below is some conditional code.
// The correct code is directly below in the IF clause and just in fact calls the standard
// DrawLine call.
// The bodged version is the code in the ELSE clause and renders the single pixel line
// using a filled rectangle. This is required because although GDI and GDraw fills
// can be made to match up, their outlines can't (at the time of writing).
//
// By the way, Gavin says, it's faster to plot four upright rects than to stroke a path
// to become the outline of the page!!!
//
#if 0
    	DrawLine(StartPoint, EndPoint); 
#else
		// Get the scaled pixel size for the view
		FIXED16 ScaledPixelWidth,
				ScaledPixelHeight;
		RenderView->GetScaledPixelSize(&ScaledPixelWidth, &ScaledPixelHeight);

		// Draw the line
		DocRect VertRect(StartPoint.x,
						 StartPoint.y,
						 EndPoint.x + ScaledPixelWidth.MakeLong(),
						 EndPoint.y + ScaledPixelHeight.MakeLong());
		DrawRect(&VertRect);
#endif
}


/********************************************************************************************

>	AttributeValue *RenderRegion::GetCurrentAttribute(UINT32 Index)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/95
	Inputs:		Index - the ID of the attribute to be examined.
	Returns:	Pointer to the current attribute of the type requested;
				NULL if the index is out of range.
	Purpose:	Get a pointer to one of the render region's current attributes.  The index
				indicates which kind of attribute should be returned.

********************************************************************************************/

AttributeValue *RenderRegion::GetCurrentAttribute(UINT32 Index)
{
	// Make sure this is a valid attribute index ID
	if (Index < (UINT32)NumCurrentAttrs)
		return CurrentAttrs[Index].pAttr;

	// Illegal index - return error
	return NULL;
}




/********************************************************************************************

>	BOOL RenderRegion::TryToFuzzyClip( Path* pPath, DocCoord* pNewCoords,
										PathVerb* pNewVerbs, INT32* NewNumCoords)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		pPath - The path to try to clip
	Outputs:	pNewCoords - The clipped coords that need rendering
				pNewVerbs - The Clipped set of verbs
				NewNumCoords - The number of coords in this new clipped path
	Returns:	TRUE if it needed to clip the path, FALSE if it did nothing
	Purpose:	This function works out if clipping is needed (based on scale factor etc)
				and tries to clip the paths. If it works it will return TRUE, meaning that
				you should only try to draw that path info found in pNewCoords etc. If
				FALSE is returned pNewCoords etc will all be NULL.
				If this function returns TRUE it will have allocated memory to put
				the new coords and verbs in. It will be your responsibility to delete
				this memory after you have rendered the path.

********************************************************************************************/

BOOL RenderRegion::TryToFuzzyClip(Path* pPath, DocCoord** pNewCoords, PathVerb** pNewVerbs, 
								  UINT32 *NewNumCoords)
{
	// Start out by setting all the return values to something sensible
	*pNewCoords = NULL;
	*pNewVerbs = NULL;
	*NewNumCoords = 0;
	
	// Decide if we need to clip this path in order to draw it
	// Only bother if we are zoomed in a bit (400%)
	if (RenderView->GetViewScale() > FIXED16(4))
	{
		// Make some new coords etc
		DocCoord* pOldCoords = pPath->GetCoordArray();
		PathVerb* pOldVerbs = pPath->GetVerbArray();
		size_t OldNumCoords = (size_t)pPath->GetNumCoords();
		
		// Allow the new path to have a few extra points in it as this is possible.
		// If it gets to be more than this, then it will simply be unclipped
		// Usually the number of points is reduced when clipping.
		size_t NumCoords = OldNumCoords+20;

		// Get some mem for the coords
		DocCoord* Coords = new DocCoord[NumCoords];
		if (Coords==NULL)
			return FALSE;

		// Get some mem for the verbs
		PathVerb* Verbs = new PathVerb[NumCoords];
		if (Verbs==NULL)
		{
			delete [] Coords;
			return FALSE;
		}

		// Actually clip the path to the rectangles
		NumCoords = FuzzyClip( (PPOINT)pOldCoords, pOldVerbs, OldNumCoords, pPath->IsFilled,
								(RECT*)(&InnerRect), (RECT*)(&OuterRect),
								(PPOINT)Coords, Verbs, NumCoords);
		// See if it worked
		if (NumCoords == (size_t)-1)
		{
			// No it did not, so clean up and return
			delete [] Coords;
			delete [] Verbs;
			return FALSE;
		}
		else
		{
		 	// fill in the correct values for the outputs and return TRUE
			*pNewCoords = Coords;
			*pNewVerbs = Verbs;
			*NewNumCoords = (UINT32)NumCoords;
			return TRUE;
		}
	}

	// There was no need to clip, so return false
	return FALSE;
}



/********************************************************************************************

>	void RenderRegion::SetupFuzzyClipRects()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/08/2000
	Purpose:	Sets up the fuzzy clipping rects, basing them on the CurrentClipRect.
				Don't ask me what they're used for - I'm just moving some useful code into
				a function :o)

********************************************************************************************/
void RenderRegion::SetupFuzzyClipRects()
{
	// Set up the Fuzzy Clipping Rects.
	if (!CurrentClipRect.IsValid())
		return;

	// Inner rect is used to clip with, so we need to make it bigger than the region so that
	// things like thick lines do not show up.
	InnerRect = CurrentClipRect;
	INT32 ScaleBy = (RR_LINEWIDTH()/2) + 16 * ScaledPixelWidth;
	InnerRect.Inflate(ScaleBy, ScaleBy);

	if (!InnerRect.IsValid())
		return;

	// The outer rect is used to determine if clipping is needed, so make it
	// a few times bigger than the inner rect.
	OuterRect = InnerRect;
	OuterRect.Inflate(InnerRect.Width()*3, InnerRect.Height()*3);
}



/********************************************************************************************
>	BOOL RenderRegion::RenderChar(WCHAR ch, Matrix* pMatrix)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Inputs:		ch      - unicode value of char
				pMatrix - matrix specifying transforms to place char correctly in document
	Returns:	FALSE if fails
	Purpose:	Render a character,
				using the specified transform and current attributes in the render region
********************************************************************************************/

BOOL RenderRegion::RenderChar(WCHAR ch, Matrix* pMatrix)
{
	ERROR2IF(pMatrix==NULL,FALSE,"RenderRegion::RenderChar() - pMatrix==NULL");
	
	// create the char's path
	Path* pCharPath=CreateCharPath(ch,pMatrix);
	if (pCharPath==NULL)
		return FALSE;

	// if at least one point, draw path using current attibutes in render region
 	if (pCharPath->GetNumCoords()!=0)
		DrawPath(pCharPath);

	// clean up
	delete pCharPath;

	return TRUE;
}

/********************************************************************************************

>	BOOL RenderRegion::WriteNewLine ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/00
	Inputs:		-
	Returns:	TRUE	- Success.
				FALSE	- Failure.
	Purpose:	Writes in a new-line token if necessary. In this class, it's just a stub
				function that's over-ridden in the derived classes.

********************************************************************************************/

BOOL RenderRegion::WriteNewLine ( void )
{
	return TRUE;
}

/********************************************************************************************
>	BOOL RenderRegion::GetAttrdCharBounds(DocRect* pRect, WCHAR ch)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Inputs:		ch      - unicode value of char
	Outputs:	pRect   - bounding box of char in doc
	Returns:	FALSE if fails
	Purpose:	Get the bounding box of the specified char (from a cache) and apply the
				required transform depending on attributes
				(this is a character relative rect!)
********************************************************************************************/

BOOL RenderRegion::GetAttrdCharBounds(DocRect* pBounds, WCHAR ch)
{
	ERROR2IF(pBounds==NULL,FALSE,"RenderRegion::GetAttrdCharBounds() - pBounds==NULL");

	// get defualt bounds from cache
	CharDescription CharDesc(ch, RR_TXTFONTTYPEFACE(), RR_TXTBOLD(), RR_TXTITALIC());
	BOOL ok = FontCache::GetBounds(pBounds,CharDesc);

	// get and apply attribute matrix to bounds
	Matrix matrix;
	if (ok) ok = GetCharAttributeMatrix(&matrix);
	if (ok) ok = matrix.TransformBounds(pBounds);

	// transform inaccuracies cause empty rects to become non-empty
	// so for very small ones to be empty (ok as rect char relative)
	if (ok && pBounds->Width()<3 && pBounds->Height()<3)
		pBounds->MakeEmpty();

	return ok;
}


/********************************************************************************************
>	Path* RenderRegion::CreateCharPath(WCHAR ch, Matrix* pMatrix=NULL)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Inputs:		ch      - unicode value of char
				pMatrix - possible matrix to transform char to correct place in doc
	Returns:	pointer to new path (else NULL if fails)
	Purpose:	Create a path of the specified char,
				using the specified transform and current attributes in the render region
	Note:		The caller is resposible for initialising the path's flags if required
********************************************************************************************/

Path* RenderRegion::CreateCharPath(WCHAR ch, Matrix* pMatrix)
{
	// get overall matrix - attribute matrix concatenated with given matrix if supplied
	Matrix matrix;
	if (GetCharAttributeMatrix(&matrix)==FALSE)
		return NULL;
	if (pMatrix)
		matrix*=*pMatrix;

	// create the default char path
	CharDescription CharDesc(ch, RR_TXTFONTTYPEFACE(), RR_TXTBOLD(), RR_TXTITALIC());
	Path* pPath=FontCache::CreateDefaultCharPath(CharDesc);
	if (pPath==NULL)
		return NULL;

	// apply overall matrix to path
	INT32      points=pPath->GetNumCoords();
	DocCoord* pCoords=pPath->GetCoordArray();
	BOOL ok=(pCoords!=NULL);
	if (ok)	matrix.transform((Coord*)pCoords, points);
	
	// if not OK, delete path and set return value to NULL
	if (!ok)
	{
		delete pPath;
		pPath=NULL;
	}

	return pPath;
}


/********************************************************************************************
>	BOOL GetCharAttributeMatrix(Matrix* pAttrMatrix)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Outputs:	pAttrMatrix -
	Returns:	FALSE if fails
	Purpose:	calc attribute matrix based on FontSize, AspectRatio and Script attributes
********************************************************************************************/

BOOL RenderRegion::GetCharAttributeMatrix(Matrix* pAttrMatrix)
{
	ERROR2IF(pAttrMatrix==NULL,FALSE,"RenderRegion::GetCharAttributeMatrix() - pAttrMatrix==NULL");
	TxtScriptAttribute* pScriptAttr=RR_TXTSCRIPT();
	ERROR2IF(pScriptAttr==NULL,FALSE,"RenderRegion::GetCharAttributeMatrix() - pScriptAttr==NULL");

	MILLIPOINT FontSize = RR_TXTFONTSIZE();
	MILLIPOINT OffsetY  = XLONG(FontSize) * pScriptAttr->Offset;
	FIXED16    fScaleY  = pScriptAttr->Size * Div32By32(FontSize, TextManager::GetDefaultHeight());
	FIXED16    fScaleX  = fScaleY * RR_TXTASPECTRATIO();

	*pAttrMatrix = Matrix(fScaleX,0,0,fScaleY,0,OffsetY);

	return TRUE;
}


/********************************************************************************************

>	DocRect RenderRegion::GetRegionRect()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Returns:	The rect of the actual region. 
	Purpose:	This function returns the size of the whole render region.
				Note that this may be different from the Clip rect. If the region is banded,
				then the Clip rect will hold the rect of the band that is being rendered,
				while the RegionRect will always hold the rect the whole region.

********************************************************************************************/

DocRect RenderRegion::GetRegionRect()
{
	return RegionRect;
}


/********************************************************************************************

>	virtual void RenderRegion::ResizeRegion(DocRect &NewClipRect)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/7/95
	Purpose:	Called when a Region is changed in size (eg. clipped to the client area)

********************************************************************************************/

void RenderRegion::ResizeRegion(DocRect &NewClipRect)
{
	// Just reset the region by default
	ResetRegion(NewClipRect);
}

/********************************************************************************************

>	virtual void RenderRegion::ResetRegion(DocRect &NewClipRect)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/95
	Purpose:	Resets the render region (after it has been merged), so that it no longer
				thinks it is banded etc. It also takes the Regions rectangle from the 
				Current Clipping rectangle, so set this correctly before calling this
				function

********************************************************************************************/

void RenderRegion::ResetRegion(DocRect &NewClipRect)
{
	// Note from Will. We really need to be re-rendering the Paper onto
	// the screen, if we have rendered something in this Region, but we
	// don't seem to be able to do this during a Paint message, as it
	// appears you can only render into the Area specified as invalid by
	// the message.  I tried clearing the clipping region etc, but this
	// made no difference.  There must be some way to do it though.
	// A solution would be to flag that the paper needs re-rendering, 
	// and do it on the next 'StartRender' call.

	Capture* pCapture = GetTopCapture();
	while (pCapture)
	{
		ENSURE(!pCapture->OwnsBitmap(), "Capture unexpectedly owns bitmap in RenderRegion::ResetRegion");

		m_CaptureStack.Pop();
//TRACEUSER( "Phil", _T("ResetRegion Pop/Delete Capture\n"));
		if (pCapture->IsMaster())
		{
			SetRenderToCapture(pCapture, FALSE, FALSE);
		}
		else
		{
			pCapture->FreeDIB();
		}
		delete pCapture;

		pCapture = GetTopCapture();
	}

	// Set the new clipping rect
	SetClipRect(NewClipRect);

	// We set this flag to get the render region to blit the paper on the first
	// time past after it is reset.
	// Unfortunately this causes large amounts of unecessary blitting when scrolling
	// an empty document around with the push tool
	// Arrghh.  No we don't.  We clear the IsPaperRendered flag so that the paper is 
	// rendered into the region on the next render timeslice if the region requires it
	// This should eliminate all unecessary blitting
	// Gerry Iles 13/01/2006
//	NeedsOSPaper = FALSE;
	IsPaperRendered = FALSE;
	
	// No Banding please
	IsRegionBanded = FALSE;
	IsWaitingForRAM = FALSE;
	IsLastBand = TRUE;
	CanRegionBeMerged = TRUE;

	// Set the region for this render region to match the clip rect
	RegionRect = CurrentClipRect;

	// Reset the rendering state
	ResetRender();

	// Make sure that that we re-init the device
	RenderFlags.ValidDevice = FALSE;
}



/********************************************************************************************

>	virtual void RenderRegion::ResetRender()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/2004
	Purpose:	Reset the rendering state of this region

********************************************************************************************/

void RenderRegion::ResetRender()
{
	IsPaperRendered = FALSE;
	IsInkRenderStarted = FALSE;

	IsBeingReset = TRUE;
	{
		// Copy a blank stack over our old one, in order to wipe it clean.
		RenderStack EmptyStack;
		if (!TheStack.Copy(&EmptyStack, this))
		{
			// We failed to copy the empty attribute stack
			ERROR3("Empty Attribute Stack could not be copied");
		}
	}
	IsBeingReset = FALSE;

	// And clean up any errant PathProcessors. (If a node, e.g. a Mould, added a PathProcessor
	// and we aborted rendering halfway through its subtree, nobody calls the mould back
	// to de-initialise, so the PathProcessor would be left lying around)
	while (PathProcessorStack != NULL)
		PopPathProcessor();

	// Now setup the render state 
	CurrentRenderState = NULL;
	SubRenderStateLocked = FALSE;
	if (CurrentSubRenderState!=NULL)
	{
		// get rid of any sub-render context we may have.
		delete CurrentSubRenderState;
		CurrentSubRenderState = NULL;
	}
}



/********************************************************************************************

>	virtual BOOL RenderRegion::SetFirstBand()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95
	Returns:	TRUE if all went OK, FALSE if there was a problem
	Purpose:	This function gives the render region a chance to see if it want to do
				some banding or not. If it decides it does, then this function will set it
				all up ready to start. It should be called before StartRender

********************************************************************************************/

BOOL RenderRegion::SetFirstBand()
{
	// No Banding please
	IsRegionBanded = FALSE;
	IsWaitingForRAM = FALSE;
	IsLastBand = TRUE;
	CanRegionBeMerged = TRUE;
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL RenderRegion::GetNextBand()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Returns:	TRUE if another band is ready to be rendered, FALSE if there were no more
				bands to render in this region.
	Purpose:	This function goes and sets up the next band in the render region ready for
				rendering and returns TRUE if it went OK. If there are no more bands to render
				then the function returns FALSE.
	Errors:		Causes an error if it can not get the next render region ready.
	SeeAlso:	RenderRegion::IsBanded

********************************************************************************************/

BOOL RenderRegion::GetNextBand()
{
	// No more bands here
	return FALSE;
}


/********************************************************************************************

>	BOOL RenderRegion::IsBanded()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Returns:	TRUE if this render region is banded
	Purpose:	To find out if the render region is banded.

********************************************************************************************/

BOOL RenderRegion::IsBanded()
{
	return IsRegionBanded;
}


/********************************************************************************************

>	BOOL RenderRegion::CanBeMerged()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Returns:	TRUE if this render region can be merged now. FALSE if it should not be
				merged.
	Purpose:	This is used to help simplify the merging code with banded region. A banded
				render region at the start of one of its bands looks similar to a render
				region that has done no rendering yet, and may be merged. This function allows
				banded regions to stop themselves from being merged after they have started
				rendering into their first band.

********************************************************************************************/

BOOL RenderRegion::CanBeMerged()
{
	// If we have started rendering ink, then we can not merge
	if (IsInkRenderStarted)
		return FALSE;

	// if we have not, then we may be between bands in a banded render region
	return CanRegionBeMerged;
}



/********************************************************************************************

>	virtual void RenderRegion::GetRenderRegionCaps(RRCaps* pCaps)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Outputs:	pCaps - the flags set for the things we are able to render
	Purpose:	This function allows a render region to declare what it can render and
				what it can not render. This base class version of the function says that
				it can render none of the options. If your render region can do some of these
				things then you should overide this function and set the appropriate flags.

********************************************************************************************/

void RenderRegion::GetRenderRegionCaps(RRCaps* pCaps)
{
	// by default, render regions can do Everything
	pCaps->CanDoAll();
}



/********************************************************************************************

>	BOOL RenderRegion::TestForComplexShape(RRCaps* pCaps)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Returns:	TRUE if the current state of the attributes make this a complex shape.
				FALSE if the host render region could render this with no trouble
	Purpose:	This function is called whenever someone calls one of the primitive render
				functions in a ScanningRenderRegion. This takes a look at all the attributes
				(and anything else it might need to) to see if this represents a complex
				shape or not. If it is a complex shape, all the internal data members of the
				render region are updated accordingly. This function makes use of the RRCaps
				object that was set with the SetHostRRCaps() function to see what things the
				Host render region considers to be complex.
	SeeAlso:	SetHostRRCaps()

********************************************************************************************/

BOOL RenderRegion::TestForComplexShape(RRCaps* pCaps)
{
	// we will start off assuming that this is not a complex shape we are trying to draw
	BOOL IsComplex = FALSE;

	// If we are in very low quality mode then assume everything is simple
	if (RRQuality.GetFillQuality() <= Quality::Bitmaps)
		return FALSE;

	if (pCaps->DoesRegionDoNothing())
		return TRUE;

	// if the host rr can not do transparancy, then see if there is any
	if (!pCaps->Transparency)
	{
		// Find out about the transparancy attribute
		TranspFillAttribute* pTransAttr = RR_FILLTRANSP();
		UINT32 Type = pTransAttr->GetTranspType();
		UINT32* pAmount1 = pTransAttr->GetStartTransp();

		// This may be a grad fill transp
		// we will get ready for this by setting up a var for the end transp
		UINT32  DefaultAmount = 0;
		UINT32* pAmount2 = &DefaultAmount;
		UINT32* pAmount3 = &DefaultAmount;
		UINT32* pAmount4 = &DefaultAmount;

		// get the end point transparencies (if there are any)
		if (pTransAttr->GetEndTransp())
			pAmount2 = pTransAttr->GetEndTransp();

		if (pTransAttr->GetEndTransp2())
			pAmount3 = pTransAttr->GetEndTransp2();
		
		if (pTransAttr->GetEndTransp3())
			pAmount4 = pTransAttr->GetEndTransp3();

		// See if there is any transparency in it
		if ((Type != 1) ||
			(*pAmount1 > 1) || 
			(*pAmount2 > 1) || 
			(*pAmount3 > 1) || 
			(*pAmount4 > 1))
		{
			// we found something that might be a bit transparent
			IsComplex = TRUE;
		}

		// CGS could - check for transparency profiling here as well
		// BUT these no point, since if were dealing with transparency,
		// then IsComplex will have just been set to TRUE by the above, which will cause us
		// to be rendered correctly anyway ....

		// Get the current fill attr
		FillGeometryAttribute* pFillAttr = (FillGeometryAttribute*) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

		if (!IsComplex)
		{
			if (pFillAttr->GetBitmap() != NULL)
			{
				KernelBitmap* pBmp = pFillAttr->GetBitmap();
				if (pBmp && pBmp->IsTransparent())
					IsComplex = TRUE;
			}
		}

		// now check for colour ramp ....

		if (!IsComplex)
		{
			// if there is a colour ramp, then the shape is complex ....
			
			if (pFillAttr->GetColourRamp () != NULL)
			{
				IsComplex = TRUE;
			}
		}

		// now lets check for fill profiling ....

		CProfileBiasGain DefaultBiasGain;

		if (!(pFillAttr->GetProfile () == DefaultBiasGain))
		{
			IsComplex = TRUE;



		}
	}

	// If it is not complex yet, and we can't do bitmaps, see if we can do them
	if ((!IsComplex) && (!pCaps->BitmapFills || !pCaps->PerspectiveBitmapFills))
	{
		// Get the current fill attr
		ColourFillAttribute* pFillAttr = (ColourFillAttribute*) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

		// see if it is a bitmap fill, and if it is, mark the shape as complex
		if (pFillAttr->IS_KIND_OF(BitmapFillAttribute))
		{
			// First check for the simple case of clipped bitmaps, as opposed to tiled
			// bitmap fills.
			FillMappingAttribute* pMapAttr = (FillMappingAttribute*) CurrentAttrs[ATTR_FILLMAPPING].pAttr;
			if ((pMapAttr->Repeat == 1) && 
				(pCaps->ClippedSimpleBitmaps || pCaps->ClippedArbitraryBitmaps))
			{
				// 'Simple' bitmap fill - is it a simple stretch, or has it been 
				// arbitrarily transformed?
				if (!pCaps->ClippedArbitraryBitmaps)
				{
					// Here we check the start and end points - the default arrangement is this:
					//
					//	EndPoint2
					//		^
					//		|
					//		|
					//		|
					//		|
					//		+----------->  EndPoint
					//	StartPoint
					//
					// So we check that the start and end points are aligned in the y axis,
					// and then check that the start and second end point are aligned in
					// the x axis.
					//
					DocCoord Geometry[3];
					Geometry[0] = *(pFillAttr->GetStartPoint());
					Geometry[1] = *(pFillAttr->GetEndPoint());
					Geometry[2] = *(pFillAttr->GetEndPoint2());
					if ((Geometry[0].x != Geometry[2].x) ||
						(Geometry[0].y != Geometry[1].y))
					{
						// It's not a simple stretch - we can't do it!
						IsComplex = TRUE;
					}
				}
			}
			else
			{
				if (!pCaps->BitmapFills)
					// We can't do bitmap fills!
					IsComplex = TRUE;
				else if (pFillAttr->IsPerspective() && !pCaps->PerspectiveBitmapFills)
					// Bitmap fill is perspectivised, and we can't do that.
					IsComplex = TRUE;
			}
		}
	}

	// If it is not complex yet, and we can't do Grad Fills, see if we can do them
	if ((!IsComplex) && (!pCaps->GradFills || !pCaps->PerspectiveGradFills || !pCaps->Grad3and4Fills))
	{
		// Get the current fill attr
		ColourFillAttribute* pFillAttr = (ColourFillAttribute*) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;
		FillMappingAttribute* pMapAttr = (FillMappingAttribute*) CurrentAttrs[ATTR_FILLMAPPING].pAttr;
		
		// see if it is a graduated fill, and if it is, mark the shape as complex
		if (pFillAttr->IsAGradFill())
		{
			if (!pCaps->GradFills)
				// We just can't do graduated fills
				IsComplex = TRUE;
			else if (pFillAttr->IsPerspective() && !pCaps->PerspectiveGradFills)
				// It's perspectivised, and we can't do that.
				IsComplex = TRUE;
			else if (pFillAttr->IsASquareFill() && !pCaps->Grad3and4Fills)
				// We use the same flag rather than put a new one in as this fill type 
				// will probably be supported soon
				IsComplex = TRUE;
			else if (pFillAttr->IsAThreeColFill() && !pCaps->Grad3and4Fills)
				IsComplex = TRUE;
//Mark Howitt, 13/10/97. If the fill is a new repeating grad fill, then that`s really complex!
			else if (pMapAttr->Repeat == 4)
				IsComplex = TRUE;
//CGS, 24/5/2000  Need to do this for the new multi-stage and profiled fills as well ....
			// now check for colour ramp ....
			// if there is a colour ramp, then the shape is real complex ....
			else if (pFillAttr->GetColourRamp () != NULL)
			{
				IsComplex = TRUE;
			}
			// now check for fill profiling ....
			else
			{
				CProfileBiasGain DefaultBiasGain;

				// if their not identical, then were complex!
				if (!(pFillAttr->GetProfile () == DefaultBiasGain))
				{
					IsComplex = TRUE;



				}
			}
		}
	}

	// Test for offscreen attributes.
	// We currently do this for all RRs 'cept those which can do everything.
	if (!IsComplex && !pCaps->DoesRegionDoAll())
	{
		OffscreenAttrValue* pOffAttr = CurrentOffscreenAttr;

		// we're complex if the current attr is not the default.
		if ( ! ((GeometryLinkedAttrValue*)pOffAttr)->IsDefaultFlagSet() )
			IsComplex = TRUE;
	}

	// Test to see whether the RR can clip its output to an arbitrary path.
	// Currently, only GRenderRegion and PostScript RR's can do this.
	if (!IsComplex && !pCaps->ClippedOutput)
	{
		ClipRegionAttribute* pClipAttr = RR_CLIPREGION();
		IsComplex = (pClipAttr->GetClipPath() != NULL);
	}

	// we seem to have problems printing variable width strokes, though the problem
	// goes away if they are transparent, so treat them as complex
	if (PathProcessorStack != NULL && PathProcessorStack->IsAPathProcessorStroke())
		IsComplex = TRUE;


	// return what we found out
	return IsComplex;
}



/********************************************************************************************

>	BOOL RenderRegion::SetRenderComplexShapes(BOOL NewState)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:		NewState - The new state for the RenderComplexShapes flag
	Returns:	The old state of the flag
	Purpose:	This function sets the state of the flag RenderComplexShapes. This flag is
				used by some classes of render region to decide what shapes to draw.
				If the flag is TRUE, then all shapes will be drawn that are asked to be
				drawn. If it is FALSE, only the shapes that are not considered to be
				complex by the render region will rendered. OSRenderRegions use this, as they
				can be used as part of the 3 phase rendering system that needs to render
				only specific nodes.
	SeeAlso:	RenderRegion::TestForComplexShape

********************************************************************************************/

BOOL RenderRegion::SetRenderComplexShapes(BOOL NewState)
{
	// make a note of the old value
	BOOL OldValue = RenderComplexShapes;

	// Set it to the new value
	RenderComplexShapes = NewState;

	// return the old value
	return OldValue;
}



/********************************************************************************************

>	virtual SlowJobResult RenderRegion::DrawMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
								    MaskedRenderRegion* pMask, ProgressDisplay *Progress)


	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		Point - the position to plot the bitmap
				pBitmap - The bitmap that needs plotting
				pMask - The mask render region to use to indicate which bits of pBitmap
				needs to be plotted
				Progress - progress display object used when printing or exporting.
	Returns: 	Status of operation.
	Purpose:	Plots the bitmap using the mask supplied. The base class implementation here
				does nothing.

********************************************************************************************/

SlowJobResult RenderRegion::DrawMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
								    MaskedRenderRegion* pMask, ProgressDisplay *Progress)
{
	return SLOWJOB_SUCCESS;
}





/********************************************************************************************

>	virtual BOOL RenderRegion::WantsGrids()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Returns: 	TRUE
	Purpose:	This function is designed to help with the rendering of grids, as they are
				not always wanted (eg xara eps). If your class of render region does not
				want grids to appear, then overide this function and get it to return FALSE.
				The default behaviour is to return TRUE, which will allow grids to render
				if they want to.

********************************************************************************************/

BOOL RenderRegion::WantsGrids()
{
	return TRUE;
}

/********************************************************************************************

>	void RenderRegion::SetBackgroundColour ( DocColour &Colour )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/00
	Input:		Colour - The background colour of the current document.
	Returns:	-
	Purpose:	Sets the value of mBackground colour, which is used whenever the program
				needs to know what the background colour of a file is.
	SeeAlso:	-

********************************************************************************************/

void RenderRegion::SetBackgroundColour ( DocColour &Colour )
{
	mBackgroundColour = Colour;
}


/********************************************************************************************

>	void RenderRegion::ColourCorrectBitmap(BitmapFillAttribute* Fill, BITMAPINFO *Info,
											RGBQUAD **Palette)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/96
	Inputs:		Fill - NULL, or a pointer to a BFA which describes how this bitmap should
						be contoned

				Info - The bitmap info header

				Palette - Must point to a (RGBQUAD *) which we can fill in (see outputs)
	
	Outputs:	Palette - Will be returned NULL if you should use the original bitmap's
				palette, else is a pointer to a CCMalloc'd block of memory containing a
				colour corrected RGBQUAD palette to use instead -in which case you MUST
				CCFree() the new block when you've finished plotting the bitmap.

	Purpose:	Applies colour correction and/or contoning to a paletted bitmap being
				plotted.

				Contoning and colour-correction of <=8bpp images is achieved by
				replacing the original palette with a corrected version. This means
				that even colour separation can be achieved on <=8bpp with this function.

				Images of more than 8bpp cannot be contoned.
				Images of more than 8bpp should be colour corrected by setting GDraw
				up to do colour correction on the fly (GD->SetBitmapConversionTable).

				This function thus takes a bitmap, and returns a pointer to the
				palette to be plot it with - if this pointer differs from the original
				one, it is then your responsibility to CCFree() the returned
				block to release the temporary memory.

********************************************************************************************/

void RenderRegion::ColourCorrectBitmap(BitmapFillAttribute* Fill, BITMAPINFO *Info,
										RGBQUAD **Palette)
{
	ERROR3IF(Info == NULL || Palette == NULL, "illegal NULL params");

	*Palette = NULL;					// Set up a suitable return default

	// --- If a fill was passed in, check if this is a contoned bitmap
	// If it is, we just provide a colour-corrected contone palette and return
  	if (Fill != NULL && Fill->GetStartColour() != NULL && Fill->GetEndColour() != NULL)
	{
		*Palette = (RGBQUAD *) CCMalloc(256 * sizeof(RGBQUAD));
		if (*Palette == NULL)
		{
			ERROR3("No memory for palette");
			return;
		}
		GradTable32::BuildGraduatedPalette(*Fill->GetStartColour(), *Fill->GetEndColour(),
										   GetRenderView(), GetFillEffect(),
										   256, *Palette, Fill->GetProfile());
		return;
	}
	// --- If it's a paletted bitmap, then we just colour-correct the palette
	if (Info->bmiHeader.biBitCount <= 8)
	{
		// Not contoned, but should we colour-correct?
		if (CurrentColContext->GetColourPlate() != NULL &&
			!CurrentColContext->GetColourPlate()->IsDisabled())
		{
			*Palette = (RGBQUAD *) CCMalloc(256 * sizeof(RGBQUAD));
			if (*Palette == NULL)
			{
				ERROR3("No memory for palette");
				return;
			}

			RGBQUAD *OldPal = Info->bmiColors;

			DocColour temp;
			PColourRGBT Converted;

			for (DWORD i = 0; i < Info->bmiHeader.biClrUsed; i++)
			{
				temp.SetRGBValue(OldPal[i].rgbRed, OldPal[i].rgbGreen, OldPal[i].rgbBlue);

				CurrentColContext->ConvertColour(&temp, (ColourPacked *) &Converted);

				(*Palette)[i].rgbRed		= Converted.Red;
				(*Palette)[i].rgbGreen		= Converted.Green;
				(*Palette)[i].rgbBlue		= Converted.Blue;
				(*Palette)[i].rgbReserved	= OldPal[i].rgbReserved;
			}
		}
		return;
	}
}




/********************************************************************************************

>	BOOL RenderRegion::RenderTree(Node* pRoot,
									BOOL bTimeSlice = TRUE,
									BOOL bClip = TRUE,
									RenderCallback* pCallback = NULL)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/93 (16/06/2004)
	Inputs:		pRoot - pointer to root node
				bTimeSlice - TRUE if rendering should be time-sliced (interruptible)
				bClip - TRUE if only those objects intersecting the clipping region
						should be rendered
				pCallback - Pointer to RenderCallback object to callback for each object or NULL
	Outputs:	TRUE if rendering was interrupted (by reaching the end of a timeslice typically)
				FALSE if rendering was completed
	Purpose:	To render a specified subtree in this RenderRegion.
				The dominating factor in this routine is that it renders the document with as 
				little time overhead as possible. Obviously, this is mainly down to 
				RenderRegions and the tree scanning routines working fast but we can do a bit 
				to help.

********************************************************************************************/
/*
Technical notes:
				This routine stores and restores its position in the document tree from the 
				RenderRegion.

				When the routine hits the end of the tree it will pass a NULL in for the 
				RenderRegion to store as its state. This can be used a a signal that rendering 
				in this RenderRegion is complete and that the RenderRegion can be destroyed.

				The routine contains all the decision making about how the traversal of the
				tree proceeds (not calling out to FindNextForClippedInkRender, for instance)
				so that it can skip across the tree if required.

********************************************************************************************/

BOOL RenderRegion::RenderTree(Node* pRoot,
							  BOOL bTimeSlice,
							  BOOL bClip,
							  RenderCallback* pCallback)
{
//	TRACEUSER( "Gerry", _T("In RenderTree CC = 0x%08x\n"), GetColourContext()->GetColourPlate());
	
	Node* pNode = NULL;
	Node* pRunToNode = NULL;
	DocRect ClipRect;

//	BOOL HasRendered = FALSE;
	BOOL Continue = TRUE;
// TEMP BODGE! DO NOT RELEASE!
//bTimeSlice = FALSE;
	m_bRenderTreeTimeSlice = bTimeSlice;

//	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
//	double ScaledPixelSize = GetScaledPixelWidth();

	// Record time when rendering started
	m_timeRenderTreeStartTime.Sample();

PORTNOTE("other","PerfCounter not used under Linux")
#if defined(__WXMSW__)
	LARGE_INTEGER countStart;
	LARGE_INTEGER countEnd;
	INT64 countOrigTotal = m_countTotal;
	QueryPerformanceCounter(&countStart);
#endif

	// Find where to pick up rendering from...
	// Have we started rendering ink?
	if (bTimeSlice && IsInkRenderStarted)
		pNode = GetRenderState();

	if (!IsInkRenderStarted || pNode==NULL)
	{
		pNode = pRoot;
		SetRenderState(pRoot);
		if (CurrentSubRenderState != NULL)
		{
			// In the middle of rendering so lose this sub-context and set it to NULL
			delete CurrentSubRenderState;
			SetSubRenderState(NULL);
		}

		// We only want to do this once
		IsInkRenderStarted = TRUE;
	}

#ifdef _DEBUG
	try
	{
#endif

	// ---------------------------------------------------
	// This is the new rendering loop...
	//
	// The algorithm is basically this:
	//	Do
	//		Render A Subtree
	//	While more to do
	//
	//  In a bit more detail:
	//	Do
	//		Try to find deepest first child subtree of Node (pushing context onto stack, when going down)
	//		Else Render Node and Try to move to Next
	//			Else Render Parent and Move Next After Parent (pulling context off stack when coming up)
	//	While more to do
	//
	Node* pNextNode = NULL;
	Node* pChildNode = NULL;
	do
	{
//		TRACEUSER( "Gerry", _T("    CC = 0x%08x\n"), GetColourContext()->GetColourPlate());
//		if (GetColourContext()->GetColourPlate() == (ColourPlate*)0xfeeefeee) { __asm {INT32 3}; }
		
		// At this point pNode ALWAYS refers to a node/subtree that has NOT yet been rendered!
		// (Er, unless there's a SubRenderContext stored - in which case we must continue
		// to render the current node without entering its subtree.)
		//
		// -----------------------------------------------
		// --- Test Subtree ---
		//
		// Check whether it's even worth bothering with this subtree...
		// (RenderSubtree does clip rect testing)
		//
//TRACEUSER( "Phil", _T("RenderTree %x %s\n"), pNode, pNode->GetRuntimeClass()->m_lpszClassName);
		SubtreeRenderState state = SUBTREE_ROOTANDCHILDREN;					// Assume normal depth-first behaviour
		SubtreeRenderState callbackstate = SUBTREE_NORENDER;
		if (CurrentSubRenderState)											// If we're partway through a node
			state = CurrentSubRenderState->renderstate;						// Continue rendering that node (usually SUBTREE_ROOTONLY)
		else
			if (pCallback && pCallback->BeforeSubtree(this, pNode, &pNextNode, bClip, &callbackstate))
				state = callbackstate;
			else
			{
				if (pNode->IsRenderable())									// If this node is at all renderable
					state = pNode->RenderSubtree(this, &pNextNode, bClip);	// Allow node to decide where to go
				else
					state = SUBTREE_NORENDER;								// Else don't render anything
			}

		// -----------------------------------------------
//		if (GetColourContext()->GetColourPlate() == (ColourPlate*)0xfeeefeee) { __asm {INT32 3}; }

		if (state==SUBTREE_JUMPTO)
		{
			// Node wants rendering to jump somewhere else
			// This isn't really sorted yet - something needs to be done about context
			// stacking...
			if (pNextNode!=NULL)
			{
				pNode = pNextNode;
				continue;
			}
			// (If pNextNode was NULL then we fall through to finding the next node...)
		}

		if (state==SUBTREE_RUNTO)
		{
			// Node wants rendering to run forward to somewhere else without actually
			// rendering any object but maintaining the attribute stack correctly
			if (pNextNode!=NULL)
			{
				pRunToNode = pNextNode;
				// NOTE! Since we don't preserve pRunToNode outside this routine
				// it must not abort due to timeslicing while pRunToNode!=NULL
			}
			// (If pNextNode was NULL then we fall through to finding the next node...)
		}

		if (state==SUBTREE_ROOTANDCHILDREN || state==SUBTREE_RUNTO)
		{
			// Depth-first, so check for children first...
			// Go down the tree from the current node, looking for child lists to render
			pChildNode = pNode->FindFirstChild();
			if (pChildNode)
			{
				// Then scan the child list
				SaveContext();												// <<<< Going down
				//TRACEUSER( "Phil", _T("SaveContext scope = %x\n"), pNode);

				// Set this child as the next subtree to consider...
				pNode = pChildNode;
				continue;
			}
		}

		// Finished trying to deal with subtree so render subtree root node
		if (state==SUBTREE_ROOTONLY || state==SUBTREE_ROOTANDCHILDREN || state==SUBTREE_RUNTO)
		{
			// No children or root only so render this node directly...
			BOOL bSuspend = RenderNode(pNode, pRunToNode, pCallback);
//			if (GetColourContext()->GetColourPlate() == (ColourPlate*)0xfeeefeee) { __asm {INT32 3}; }
			if (bSuspend) break;
		}

		// Balance the call to RenderSubtree
		if (!(pCallback && pCallback->AfterSubtree(this, pNode)))
			pNode->RenderAfterSubtree(this);
		
		// -----------------------------------------------
		// --- Move on to next subtree ---
		//
		// SUBTREE_NORENDER
		//
		pNextNode = pNode->FindNext();

		// See if we got here because we set up a subrender context especially to do so
		// If so, get rid of this context so that rendering can proceed normally
		if (CurrentSubRenderState && CurrentSubRenderState->IsKindOf(CC_RUNTIME_CLASS(SubTreeRenderContext)))
		{
			delete CurrentSubRenderState;
			CurrentSubRenderState = NULL;
		}

		// -----------------------------------------------
		// --- Render Subtree Parents ---
		//
		// While no next sibling found, look for parent's next sibling...
		if (pNextNode==NULL && pNode!=pRoot && Continue)
		{
			do
			{
				pNode = pNode->FindParent();
				if (pNode)
				{
					// Render the parent node now that its children have all been rendered...
					BOOL bSuspend = RenderNode(pNode, pRunToNode, pCallback);
//					if (GetColourContext()->GetColourPlate() == (ColourPlate*)0xfeeefeee) { __asm {INT32 3}; }
					if (bSuspend) break;

					//TRACEUSER( "Phil", _T("RestoreContext scope = %x\n"), pNode);
					RestoreContext();											// >>>> Coming up

					// Slightly unpleasant to have yet another call to the Node
					// but for things to balance correctly, it needs to be given a chance
					// to do stuff /outside/ the attribute scope created by the calls
					// to SaveContext and RestoreContext...
					//
					// Balance the call to RenderSubtree
					if (!(pCallback && pCallback->AfterSubtree(this, pNode)))
						pNode->RenderAfterSubtree(this);

					pNextNode = pNode->FindNext();

PORTNOTE("other","PerfCounter not used under Linux")
#if defined(__WXMSW__)
					// I'm not really happy about doing this here but profiling will tell us
					// whether it's really a problem...
					QueryPerformanceCounter(&countEnd);
					m_countTotal = countOrigTotal + countEnd.QuadPart - countStart.QuadPart;
#endif

					// See if we should stop coming up out of this subtree because we've run out of time
					Continue = RenderTreeCanContinue();
				}
			}
			while (pNextNode==NULL && pNode!=pRoot && Continue);

			if (!Continue)
			{
				// We have run out of time while following parent links
				// so we need to set things up so that when rendering
				// is restarted, it doesn't go DOWN into the subtree again!!!
				// We do this by setting up a subrender state so that the logic above
				// will drop through the SUBTREE_NORENDER path
				//
				// Note: Leave the current node pointer alone
				//
				SubTreeRenderContext* pReturnHereContext = new SubTreeRenderContext();
				SetSubRenderState(pReturnHereContext);
				break;
			}
		}

		// We should move the current node pointer on if we know that
		// the 
		if (pNextNode && pNode!=pRoot) pNode = pNextNode;

PORTNOTE("other","PerfCounter not used under Linux")
#if defined(__WXMSW__)
		// I'm not really happy about doing this here but profiling will tell us
		// whether it's really a problem...
		QueryPerformanceCounter(&countEnd);
		m_countTotal = countOrigTotal + countEnd.QuadPart - countStart.QuadPart;
#endif
		
		// See if we are time slicing and we still have more time available
		Continue = RenderTreeCanContinue();
	}
	while (pNode && pNode!=pRoot && (Continue || pRunToNode!=NULL));
	//
	// Here endeth the new rendering loop
	// ---------------------------------------------------


#ifdef _DEBUG
	}
	catch (...)
	{
		TRACEUSER( "Phil", _T("Exception during rendering...\n"));
	}
#endif

	if (pNode==pRoot) pNode = NULL;

	// Reset state to new posn in tree.
	SetRenderState(pNode);

	ENSURE(pRunToNode==NULL, "Expected pRunToNode to be NULL at end of RenderTree\n");

//TRACEUSER( "Phil", _T("RenderTree exiting %x %s\n", pNode, pNode?pNode->GetRuntimeClass()->m_lpszClassName:"NULL"));
//	if (pbHaveRendered)
//		*pbHaveRendered = *pbHaveRendered || HasRendered;

	return Continue;
}




/********************************************************************************************

>	BOOL RenderRegion::RenderNode(Node* pNode, Node* pRunToNode, RenderCallback* pCallback)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/2004
	Inputs:		-
	Outputs:	TRUE if RenderTree rendering can continue
				FALSE if RenderTree rendering
	Purpose:	Wrap up the common stuff that's done to every node we render
				Render a single node and find out whether we should suspend rendering
				at this point
				Helper function for RenderTree

********************************************************************************************/

BOOL RenderRegion::RenderNode(Node* pNode, Node* pRunToNode, RenderCallback* pCallback)
{
	BOOL bSuspendRendering = FALSE;
	BOOL bRender = TRUE;

	// Give RenderCallback object a chance to pre-process the rendering...
	if (pCallback)
		bRender = pCallback->BeforeNode(this, pNode);

	// Allow the loop to run forward to the RunTo node without rendering objects if it's set up...
	if (pRunToNode)
	{
		if (pRunToNode==pNode)				// If we are at the RunTo node
			pRunToNode = NULL;				// Then clear the system and allow it to render normally
		else
			if (pNode->IsAnObject())		// Else if this node is an object
				bRender = FALSE;			// Don't let it render anything yet...
	}

	// If we can render this node, go ahead and do it.
	if (bRender && pNode->IsRenderedAsInk() && pNode->IsRenderable())
	{
//TRACEUSER( "Phil", _T("RenderNode Node %x %s\n"), pNode, pNode->GetRuntimeClass()->m_lpszClassName);
		pNode->Render(this);				// Render the node

		if (CurrentSubRenderState!=NULL)	// If the node aborted rendering and set a subrender state
			bSuspendRendering = TRUE;		// Then Stop rendering without moving on the node pointer
	}

	return bSuspendRendering;
}




/********************************************************************************************

>	BOOL RenderRegion::RenderTreeCanContinue()

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/2004
	Inputs:		-
	Outputs:	TRUE if RenderTree rendering can continue
				FALSE if RenderTree rendering should stop
	Purpose:	Decide whether we can continue to render in RenderTree

********************************************************************************************/

BOOL RenderRegion::RenderTreeCanContinue()
{
	// If we are currently capturing offscreen stuff then we prefer to continue rendering
	// because the overhead of composing offscreen bitmaps together is so great
	// so lengthen the timeslice to make this happen less often
	INT32 t = Timeslice;
	if (GetCaptureDepth()>1) t = t*2;
	// if m_bRenderTreeTimeSlice == FALSE
	// or the timeslice hasn't elapsed yet then we can continue
	return !m_bRenderTreeTimeSlice || !m_timeRenderTreeStartTime.Elapsed(t);
}




/********************************************************************************************

>	BOOL RenderRegion::RenderTreeNoCache(Node* pRoot)
					
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/2005
	Inputs:pNode - pointer to root node of subtree to render
	Returns:	FALSE if the render failed
				TRUE if it worked
	Purpose:	Renders a subtree with cacheing disabled

********************************************************************************************/
BOOL RenderRegion::RenderTreeNoCache(Node* pRoot)
{
	if (pRoot == NULL || this == NULL)
	 	return FALSE;

	// pNode may not be in the document tree!
	// So all bets are off regarding cacheing.
	// Therefore we disable cacheing during this render.
	ScopedFlagSetter fsBitmapCacheing(NodeRenderableBounded::bEnableCacheing, FALSE);

	SaveContext();
	RenderTree(pRoot, FALSE, FALSE);
	RestoreContext();

	return TRUE;
}




/********************************************************************************************

>	void RenderRegion::SetBackmostChangedNode(Node* pInvalidNode)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/2004
	Inputs:		-
	Outputs:	-
	Purpose:	-

********************************************************************************************/

void RenderRegion::SetBackmostChangedNode(Node* pInvalidNode)
{
	if (pInvalidNode)
	{
		if (pInvalidNode->FindParent()==NULL)
		{
//			ERROR2RAW("SetBackmostChangedNode called on node with no parent");
			return;
		}

		if (m_pBackmostChangedNode==NULL || pInvalidNode->IsUnder(m_pBackmostChangedNode))
			m_pBackmostChangedNode = pInvalidNode;
	}
}




/********************************************************************************************

>	void RenderRegion::ClearBackmostChangedNode(Node* pNode)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/2004
	Inputs:		-
	Outputs:	-
	Purpose:	-

********************************************************************************************/

void RenderRegion::ClearBackmostChangedNode(Node* pNode)
{
	if (pNode && (pNode == m_pBackmostChangedNode || pNode->IsNodeInSubtree(m_pBackmostChangedNode)))
	{
		m_pBackmostChangedNode = pNode->FindParentSpread();
	}
}




/********************************************************************************************

>	Capture* RenderRegion::StartCapture(CCObject* pOwner,
									 DocRect CaptureRect,
									 CAPTUREINFO cinfo,
									 BOOL bTransparent,
									 BOOL bCaptureBackground,
									 double mpPixelWidth,
									 NodeRenderableInk* pDirectSupplier)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/2004
	Inputs:		pOwnerNode	- Node in tree which "owns" this capture
				CaptureRect - The rectangle to capture
				ctype		- The type of capture required
				bTransparent	- The capture could be transparent RGBT
				bCaptureBackground	- The capture should include the state of the background
	Outputs:	-
	Returns:	TRUE if a Capture was successfully set up
	Purpose:	To start a bitmap capture of whatever is subsequently rendered
	SeeAlso:	GRenderRegion::StopCapture

********************************************************************************************/

Capture* RenderRegion::StartCapture(CCObject* pOwner,
									 DocRect CaptureRect,
									 CAPTUREINFO cinfo,
									 BOOL bTransparent,
									 BOOL bCaptureBackground,
									 double dPPI,
									 NodeRenderableInk* pDirectSupplier)
{
	ENSURE(cinfo.caType==ctNESTABLE, "We only support nestable captures at the moment\n");

	// 1) alloc new mem, and setup state variables for new offscreen rendering
		// mem alloced = bmp
		// vars setup = docrect, gmatrix
	// 2) save current
	// 3) change to new offscreen state

	// If there's already a capture running and this one is nested but wants to change 
	// the transparency type then we must notify existing captures of the change...
	Capture* pCurrentCapture = GetTopCapture();
	if (pCurrentCapture!=NULL && cinfo.caType==ctNESTABLE && !cinfo.caFlags.GrabRendered && pCurrentCapture->IsTransparent() && !bTransparent)
	{
		ChangeCapture(cinfo, bTransparent, bCaptureBackground);
	}

	// If the caller didn't specify the capture resolution in pixels per inch
	// Then set it to the default value of the render region
	// And use the default matrix in our setup calculations
	Matrix CaptureMatrix;
	if (dPPI==0)
	{
//		dPPI = GetPixelsPerInch();

		// Find current top bitmap capture and copy its res and res scale control flag
		Capture* pCap = GetTopCapture();
		ERROR2IF(pCap==NULL, NULL, "There MUST be a bitmap capture in the stack - the master capture at least");
		dPPI = pCap->dPixelsPerInch;
		cinfo.caFlags.CalcScaledPixWidth = pCap->info.caFlags.CalcScaledPixWidth;

		CaptureMatrix = RenderMatrix;
	}
	else
	{
		// If the caller specified a particular DPI we will assume he wants the capture
		// to grab the rendered objects at 100% scale...
		// No need to construct a new matrix in a complex way - the clip rect and
		// object are all stored in doccoords.
//		CaptureMatrix = Identity;
	}

	// In non-Master Captures, make sure that we use a default colour context
	// to prevent multiple colour corrections when rendering bitmaps in "print preview"
	// type modes...
	ColourContext* pColContext = NULL;
	if (!cinfo.caFlags.Master)
	{
		BOOL bOldForce = RenderView->SetForceDefaultColourContexts();
		pColContext = RenderView->GetColourContext(COLOURMODEL_RGBT);
		RenderView->SetForceDefaultColourContexts(bOldForce);
	}
	else
	{
		// Must get RenderRegion's current colour context for Master Capture
		pColContext = CurrentColContext;
	}

	// Create a new capture instance...
	Capture* pNewCapture = new Capture(pOwner, cinfo, CaptureMatrix, CaptureRect, dPPI, pColContext);
	ENSURE(!pNewCapture->IsMaster(), "Can only have one master capture!");

	m_CaptureStack.Push(pNewCapture);

	return pNewCapture;
}




/********************************************************************************************

>	BOOL RenderRegion::StopCapture(CCObject* pOwner,
									BOOL bRender = TRUE,
									BOOL bReleaseBitmap = FALSE,
									LPBITMAPINFO* plpBitmapInfo = NULL,
									LPBYTE* plpBits = NULL,
									DocRect* pCaptureRect = NULL,
									Matrix* pmatTransform = NULL,
									double* pdResolution = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/2004
	Inputs:		pEndNode	- Node in tree which "owns" this capture
	Outputs:	-
	Returns:	TRUE if the Capture resulted in a bitmap being usefully created
	Purpose:	To stop a bitmap capture
	SeeAlso:	GRenderRegion::StartCapture

********************************************************************************************/

BOOL RenderRegion::StopCapture(CCObject* pOwner,
							   BOOL bRender,
							   BOOL bReleaseBitmap,
							   LPBITMAPINFO* plpBitmapInfo,
							   LPBYTE* plpBits,
							   DocRect* pCaptureRect,
							   Matrix* pmatTransform,
							   double* pdResolution)
{
	Capture* pCapture = GetTopCapture();
	if (pCapture==NULL)
		return FALSE;

	if (pCapture->IsMaster())
	{
		ERROR2(FALSE, "StopCapture called on the master capture!\n");
		return FALSE;
	}

	// Pop the capture off the stack now
	m_CaptureStack.Pop();

	delete pCapture;

	return TRUE;
}

	
	
	
/********************************************************************************************

>	BOOL RenderRegion::ChangeCapture(CAPTUREINFO cinfo,
									BOOL bTransparent,
									BOOL bCaptureBackground)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/2004
	Inputs:		-
	Outputs:	-
	Purpose:	To inform the capture stack that things (transparency) have changed

********************************************************************************************/

BOOL RenderRegion::ChangeCapture(CAPTUREINFO cinfo,
								BOOL bTransparent,
								BOOL bCaptureBackground)
{
	// If there's a current RGBT capture active and we are changing to a non-transparent capture
	// Then mark that Capture as no longer being transparent
	// Blit whatever we have so far into the parent bitmap
	// Tell all enclosing captures (further up the stack) that they can no longer be transparent
	// either
	Capture* pCurrentCapture = GetTopCapture();
	if (pCurrentCapture==NULL)
		return FALSE;

	if (pCurrentCapture->IsMaster())
	{
		ENSURE(m_CaptureStack.Size()==1, "Master Capture not bottom capture in ChangeCapture");
		return FALSE;
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL RenderRegion::LockedTransparentCaptures() const

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/2005
	Inputs:		-
	Outputs:	-
	Purpose:	Search down the Capture stack from the top or from the capture specified
				until we find one that owns a bitmap. The master capture at the bottom
				of the stack should ALWAYS own a bitmap!

********************************************************************************************/

BOOL RenderRegion::LockedTransparentCaptures() const
{
	const ListT<Capture*>* pList = m_CaptureStack.GetListT();

	Capture* pCapture = NULL;
	ListT<Capture*>::ConstIterator it = pList->End();
	INT32 nListSize = pList->Size();
	INT32 i=0;

	for (i=0; i < nListSize; i++)
	{
		pCapture = *(--it);
		if (pCapture->OwnsBitmap() &&
			pCapture->IsTransparent() &&
			(pCapture->IsMaster() || pCapture->info.caFlags.LockedTransparent)
			)
			return TRUE;
	}

	return FALSE;
}




/********************************************************************************************?

>	BOOL RenderRegion::RenderBits(LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits, DocRect rect, BOOL bForceNoTransp = TRUE, Node* pEffectsOwner = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/07/2004
	Inputs:		-
	Outputs:	lpBitmapInfo
				lpBits
				rect
	Returns:	TRUE if the bitmap was rendered
	Purpose:	To blit one bitmap defined by a DocRect into the current RenderRegion
	SeeAlso:	GRenderRegion::StartCapture

********************************************************************************************/

BOOL RenderRegion::RenderBits(LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits, DocRect rect, BOOL bForceNoTransp, Node* pEffectsOwner)
{
	return RenderBits(lpBitmapInfo, lpBits, (DocCoord*)&rect, 2, bForceNoTransp, pEffectsOwner);
}

	
	
/********************************************************************************************

>	BOOL RenderRegion::RenderBits(LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits, DocCoord* pCoords, INT32 numcoords, BOOL bForceNoTransp = TRUE, Node* pEffectsOwner = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/07/2004
	Inputs:		-
	Outputs:	lpBitmapInfo
				lpBits
				rect
	Returns:	TRUE if the bitmap was rendered
	Purpose:	To blit one bitmap defined by a DocRect into the current RenderRegion
	SeeAlso:	GRenderRegion::StartCapture

********************************************************************************************/

BOOL RenderRegion::RenderBits(LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits, DocCoord* pCoords, INT32 numcoords, BOOL bForceNoTransp, Node* pEffectsOwner)
{
	ERROR2IF(!(numcoords==2 || numcoords==3), FALSE, "Wrong number of coords passed to RenderBits");

	if (lpBitmapInfo->bmiHeader.biWidth==0 || lpBitmapInfo->bmiHeader.biHeight==0)
		return FALSE;

//	TRACEUSER( "Phil", _T("RenderBits %x %d, %d, %d, %d, %d, %d\n"), lpBits, pCoords[0].x, pCoords[0].y, pCoords[1].x, pCoords[1].y, pCoords[2].x, pCoords[2].y);
	CWxBitmap		   *pBitmap = new CWxBitmap(lpBitmapInfo, lpBits);
	KernelBitmap	   *pkBitmap = new KernelBitmap(pBitmap, TRUE);
	// Assume 8BPP bitmaps must be greyscale...
	if (pkBitmap->GetBPP()==8)
	{
		LPRGBQUAD pPalette = pkBitmap->GetPaletteForBitmap();
		for ( INT32 i=0 ; i<0x100 ; i++ )
			(UINT32&)pPalette[i] = i*0x010101 ;
	}

	RenderBits(pkBitmap, pCoords, numcoords, bForceNoTransp, pEffectsOwner);

	if (pBitmap)
	{
		pBitmap->BMBytes = ((CWxBitmap*)OILBitmap::Default)->BMBytes;
	}
	if (pkBitmap)
	{
		delete pkBitmap;
		pkBitmap = NULL;
	}

	return TRUE;
}

	
	
/********************************************************************************************

>	BOOL RenderRegion::RenderBits(KernelBitmap* pBitmap, DocCoord* pCoords, INT32 numcoords, BOOL bForceNoTransp = TRUE, Node* pEffectsOwner = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/07/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the bitmap was rendered
	Purpose:	To blit one bitmap defined by a DocRect into the current RenderRegion
	SeeAlso:	GRenderRegion::StartCapture

********************************************************************************************/

BOOL RenderRegion::RenderBits(KernelBitmap* pkBitmap, DocCoord* pCoords, INT32 numcoords, BOOL bForceNoTransp, Node* pEffectsOwner)
{
	ERROR2IF(!(numcoords==2 || numcoords==3), FALSE, "Wrong number of coords passed to RenderBits");

	if (pkBitmap->GetWidth()==0 || pkBitmap->GetHeight()==0)
		return FALSE;

//	TRACEUSER( "Phil", _T("RenderBits %x %d, %d, %d, %d, %d, %d\n"), lpBits, pCoords[0].x, pCoords[0].y, pCoords[1].x, pCoords[1].y, pCoords[2].x, pCoords[2].y);
	SaveContext();

	// No lines on the rectangle
	SetLineColour(COLOUR_TRANS);
	SetLineWidth(0);

	BitmapFillAttribute *pBitmapAttr = new BitmapFillAttribute;
	pBitmapAttr->GetBitmapRef()->SetBitmap(pkBitmap);

	Path InkPath;
	if (numcoords==2)
	{
		//	This code was for upright rectangles only...
		InkPath.Initialise();
		InkPath.AddMoveTo(pCoords[0]);
		InkPath.AddLineTo(DocCoord(pCoords[1].x, pCoords[0].y));
		InkPath.AddLineTo(pCoords[1]);
		InkPath.AddLineTo(DocCoord(pCoords[0].x, pCoords[1].y));
		InkPath.AddLineTo(pCoords[0]);
		InkPath.CloseSubPath();

		pBitmapAttr->StartPoint = pCoords[0];
		pBitmapAttr->EndPoint   = DocCoord(pCoords[1].x, pCoords[0].y);
		pBitmapAttr->EndPoint2 	= DocCoord(pCoords[0].x, pCoords[1].y);
	}
	else
	{
		// This version of the same code uses the array of 3 coords describing a parallelogram
		InkPath.Initialise();
		InkPath.AddMoveTo(pCoords[0]);
		InkPath.AddLineTo(pCoords[1]);
		InkPath.AddLineTo(DocCoord(pCoords[2].x+pCoords[1].x-pCoords[0].x, pCoords[2].y+pCoords[1].y-pCoords[0].y));
		InkPath.AddLineTo(pCoords[2]);
		InkPath.AddLineTo(pCoords[0]);
		InkPath.CloseSubPath();

		pBitmapAttr->StartPoint = pCoords[0];
		pBitmapAttr->EndPoint   = pCoords[1];
		pBitmapAttr->EndPoint2 	= pCoords[2];
	}

	// Set bitmap attribute, and get the render region to throw it away when it's finished
	// with (hence the TRUE parameter).
	SetFillGeometry(pBitmapAttr, TRUE);
	SetSmoothingFlags(pBitmapAttr);

	// Set the mapping to have no repeat, otherwise we get artifacts at the edges when
	// anti-aliasing is enabled (see bug 1391).
	FillMappingLinearAttribute *pNoRepeatAttr = new FillMappingLinearAttribute;

	// Repeat value of '3' means 'repeat inverted' (there are no symbolic names for
	// these repeat values - shout at Will if you want some!).
//	pNoRepeatAttr->Repeat = 3;
	pNoRepeatAttr->Repeat = 0;

	// Set mapping attribute, and get the render region to throw it away when it's finished
	// with (hence the TRUE parameter).
	SetFillMapping(pNoRepeatAttr, TRUE);

	// Force a 0% mix transparency (the transparency is in the bitmap)
	FlatTranspFillAttribute NoFillTransp(TT_NoTranspType);
	StrokeTranspAttribute NoLineTransp(TT_NoTranspType);
	if (bForceNoTransp)
	{
		SetTranspFillGeometry(&NoFillTransp, FALSE);
		SetLineTransp(&NoLineTransp, FALSE);
	}

	if (pEffectsOwner && pEffectsOwner->IsBounded())
		((NodeRenderableBounded*)pEffectsOwner)->RenderEffectAttributes(this);

	// Draw the bitmap by rendering a bitmap filled path.
	InkPath.IsFilled = TRUE;
	DrawPath(&InkPath);

	SetSmoothingFlags(NULL);

	RestoreContext();

	return TRUE;
}

	
	
#ifdef _DEBUG
void RenderRegion::DebugTrace()
{
}
#endif
