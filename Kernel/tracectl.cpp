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
// Tracer control class

/*
*/

#include "camtypes.h"
#include "tracectl.h"
#include "tracergn.h"

#include "ccobject.h"

#include "bitmap.h"
//#include "nodebmp.h"
#include "bitmpinf.h"
#include "bfxalu.h"
#include "bfxpixop.h"
#include "nodepath.h"
#include "lineattr.h"
#include "group.h"
#include "fillval.h"
#include "oilprog.h"
//#include "andy.h"
#include "fillattr.h"
#include "fitcurve.h"
#include "pathproc.h"
#include "progress.h"
//#include "tracerc.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(TraceControl, BitmapEffectBase)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

#define TRACEUSESTATS 0

const INT32 MARKED_PIXEL = 0xFFFFFFFF;
const INT32 CLEAR_PIXEL = 0xFF808080;
const INT32 REGION_PIXEL = 0xFF000000;
const INT32 BIT_SCANCOLOUR = 0x80000000;
const INT32 BIT_THRESHOLD = 0x40000000;
const INT32 BIT_CACHES_THRESHOLD = 0x01000000;
const INT32 BIT_CACHEV_THRESHOLD = 0x02000000;
const INT32 BIT_CACHES_COLOURTHRESHOLD = 0x04000000;
const INT32 BIT_CACHEV_COLOURTHRESHOLD = 0x08000000;
const INT32 BIT_CACHES_POSITIVE = 0x10000000;
const INT32 BIT_CACHEV_POSITIVE = 0x20000000;

#ifdef _DEBUG
#define SetDebugBitmap(n,b) { if (DebugSwitch[n]) { if (!pDebug[n]) pDebug[n]=ALU->NewBitmap(pUndithered,0,0,32); \
							DebugInvalid[n]=TRUE;\
							if (pDebug[n]) { ALU->SetA(pDebug[n]); if (b) {ALU->SetB(b); ALU->ByteCopyBA();} \
							else ALU->ZeroA(); } } }
#else
#define SetDebugBitmap(n,b) { if (FALSE); }
#endif

/*******************************************************************************************

>	TraceControl::TraceControl() : CCObject()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	Constructs the trace control object

*******************************************************************************************/

TraceControl::TraceControl() : BitmapEffectBase()
{
	pOriginal = NULL;
	pProposed = NULL;
	pUndithered = NULL;
	pCurrentPath = NULL;
	pFill = NULL;
	pTraceRegion = NULL;
	pStats = NULL;
	pErrorRegionList = NULL;
	pThresholdPixelOp = NULL;
	pColourThresholdPixelOp = NULL;
	pPositivePixelOp = NULL;

	pTree=NULL;
	pJoinNode=NULL;

#ifdef _DEBUG
	for (INT32 i=0; i<NUM_TRACECTL_DEBUG_BITMAPS; i++) {pDebug[i]=NULL;DebugSwitch[i]=FALSE;DebugInvalid[i]=TRUE;}
#endif

	MaximumInitialAreaErrorThreshold = 0.12;
	InitialAreaErrorThreshold = MaximumInitialAreaErrorThreshold;
	MinimumInitialAreaErrorThreshold = 0.06;
	InitialAreaErrorRatio = 0.5;

	GradFillError = 1.0;
	NewGradFillError = 0.0;
	MinGradFillError = 2.0/256.0;
	GradFillErrorRatio = 0.8;

	MinPixelError=0.45;
	MaxPixelError=2.0;
	PixelError=MaxPixelError;
	PixelErrorRatio=0.5;
	QuantColours=30;

	Blur=64.0;
	Smooth=0.0;
	Log2BPP=5;
	Method = TRACEMETHOD_TRUECOL;

	MinimumArea = 10;

	FoundRegion = FALSE;
	InitialX = 0 ;
	InitialY = 0;

	ScanPointX = 0;
	ScanPointY = 0;

	XSize = 0;
	YSize = 0;

	HavePath = FALSE;

	TotalPasses=0;
	NumPasses=0;
	NumPaths=0;
	NumCusps=0;
	NumPoints=0;
	ProgressDone=FALSE;

	InitBitmap();

	FirstShape = TRUE;
	ShapeFreePass = FALSE;
}       



/*******************************************************************************************

>	TraceControl::~TraceControl()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	Destructor, does nothing

*******************************************************************************************/

TraceControl::~TraceControl()
{
	InitBitmap(); // Free bitmaps if there are any
	RemoveTree();
	if (pTraceRegion) delete pTraceRegion;
	if (pCurrentPath) delete pCurrentPath;
	if (pFill) delete pFill;
	if (pStats) delete pStats;
	if (pErrorRegionList) delete pErrorRegionList;
	if (pThresholdPixelOp) delete pThresholdPixelOp;
	if (pColourThresholdPixelOp) delete pColourThresholdPixelOp;
	if (pPositivePixelOp) delete pPositivePixelOp;


#ifdef _DEBUG
	for (INT32 i=0; i<NUM_TRACECTL_DEBUG_BITMAPS; i++) if (pDebug[i]) delete pDebug[i];
#endif
} 

/********************************************************************************************

>	BOOL TraceControl::GetParams(
		TraceMethod * pMethod = NULL,
		INT32 * pMinimumArea = NULL,
		double * pMaximumInitialAreaErrorThreshold = NULL,
		double * pMinimumInitialAreaErrorThreshold = NULL,
		double * pInitialAreaErrorRatio = NULL,
		double * pMinGradFillError = NULL,
		double * pGradFillErrorRatio = NULL,
		double * pMinPixelError = NULL,
		double * pMaxPixelError = NULL,
		double * pPixelErrorRatio = NULL,
		double * pQuantColours = NULL,
		double * pBlur = NULL,
		double * pSmooth = NULL
		);

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		None
	Outputs:	double values of parameters to read. May be set to NULL to not read them
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Gets the tracing parameters
	Errors:		mainly memory.
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL TraceControl::GetParams(
		TraceMethod * pMethod,
		INT32 * pMinimumArea,
		double * pMaximumInitialAreaErrorThreshold,
		double * pMinimumInitialAreaErrorThreshold,
		double * pInitialAreaErrorRatio,
		double * pMinGradFillError,
		double * pGradFillErrorRatio,
		double * pMinPixelError,
		double * pMaxPixelError,
		double * pPixelErrorRatio,
		double * pQuantColours,
		double * pBlur,
		double * pSmooth
		)
{
	if (pMethod)							*pMethod							=Method;
	if (pMinimumArea)						*pMinimumArea						=MinimumArea;
	if (pMaximumInitialAreaErrorThreshold)	*pMaximumInitialAreaErrorThreshold	=MaximumInitialAreaErrorThreshold;
	if (pMinimumInitialAreaErrorThreshold)	*pMinimumInitialAreaErrorThreshold	=MinimumInitialAreaErrorThreshold;
	if (pInitialAreaErrorRatio) 			*pInitialAreaErrorRatio				=InitialAreaErrorRatio;
	if (pMinGradFillError) 					*pMinGradFillError					=MinGradFillError;
	if (pGradFillErrorRatio) 				*pGradFillErrorRatio				=GradFillErrorRatio;
	if (pMinPixelError) 					*pMinPixelError						=MinPixelError;
	if (pMaxPixelError) 					*pMaxPixelError						=MaxPixelError;
	if (pPixelErrorRatio) 					*pPixelErrorRatio					=PixelErrorRatio;
	if (pQuantColours)		 				*pQuantColours						=QuantColours;
	if (pBlur)								*pBlur								=Blur;
	if (pSmooth)							*pSmooth							=Smooth;
	return TRUE;
}

/********************************************************************************************

>	BOOL TraceControl::SetParams(
		TraceMethod * pMethod = NULL,
		INT32 * pMinimumArea = NULL,
		double * pMaximumInitialAreaErrorThreshold = NULL,
		double * pMinimumInitialAreaErrorThreshold = NULL,
		double * pInitialAreaErrorRatio = NULL,
		double * pMinGradFillError = NULL,
		double * pGradFillErrorRatio = NULL,
		double * pMinPixelError = NULL,
		double * pMaxPixelError = NULL,
		double * pPixelErrorRatio = NULL,
		double * pQuantColours = NULL,
		double * pBlur = NULL,
		double * pSmooth = NULL
		)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		^double values of parameters to set. May be set to NULL to not change them
	Outputs:	None
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Sets the tracing parameters
	Errors:		mainly memory.
	Scope:		Public
	SeeAlso:	-

This call can only run after InitBitmaps() as some of the parameters live in the TraceRegion
(yes, this is a bit messy)

********************************************************************************************/

BOOL TraceControl::SetParams(
		TraceMethod * pMethod,
		INT32 * pMinimumArea,
		double * pMaximumInitialAreaErrorThreshold,
		double * pMinimumInitialAreaErrorThreshold,
		double * pInitialAreaErrorRatio,
		double * pMinGradFillError,
		double * pGradFillErrorRatio,
		double * pMinPixelError,
		double * pMaxPixelError,
		double * pPixelErrorRatio,
		double * pQuantColours,
		double * pBlur,
		double * pSmooth
		)
{
	ERROR2IF((!pTraceRegion), FALSE, "SetParams() called without prior call to InitBitmaps()");
	if (pMethod)							Method								=*pMethod;
	if (pMinimumArea)						MinimumArea							=*pMinimumArea;
	if (pMaximumInitialAreaErrorThreshold)	MaximumInitialAreaErrorThreshold	=*pMaximumInitialAreaErrorThreshold;
	if (pMinimumInitialAreaErrorThreshold)	MinimumInitialAreaErrorThreshold	=*pMinimumInitialAreaErrorThreshold;
	if (pInitialAreaErrorRatio) 			InitialAreaErrorRatio				=*pInitialAreaErrorRatio;
	if (pMinGradFillError) 					MinGradFillError					=*pMinGradFillError;
	if (pGradFillErrorRatio) 				GradFillErrorRatio					=*pGradFillErrorRatio;
	if (pMinPixelError) 					MinPixelError						=*pMinPixelError;
	if (pMaxPixelError) 					MaxPixelError						=*pMaxPixelError;
	if (pPixelErrorRatio) 					PixelErrorRatio						=*pPixelErrorRatio;
	if (pQuantColours)	 					QuantColours						=*pQuantColours;
	if (pBlur)								Blur								=*pBlur;
	if (pSmooth)							Smooth								=*pSmooth;
	return TRUE;
}


/********************************************************************************************

>	BOOL TraceControl::InitBitmap( KernelBitmap * theOriginal = NULL);
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		theOriginal = pointer to the bitmap to use as the original to trace
				   		   or NULL to cancel use of bitmap
	Outputs:	theProposed = pointer to a place to store a pointer to the proposed image
						   we'll be using. May be NULL if no record required of this.
				theTemp		= pointer to a place to store a pointer to the temp image
						   we'll be using. May be NULL if no record required of this.
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Initialises trace control object to work on a given set of bitmaps
	Errors:		mainly memory.
	Scope:		Public
	SeeAlso:	-

Also, if we don't have a traceregion or a grad fill we claim these.

********************************************************************************************/

BOOL TraceControl::InitBitmap( KernelBitmap * theOriginal)
{
#ifdef _DEBUG
	for (INT32 i=0; i<NUM_TRACECTL_DEBUG_BITMAPS; i++) if (pDebug[i]) {delete pDebug[i];pDebug[i]=NULL;DebugInvalid[i]=TRUE;}
#endif

	if (pErrorRegionList)
	{
		// We don't want this around
		delete pErrorRegionList;
		pErrorRegionList = NULL;
	}

	if (pThresholdPixelOp) delete pThresholdPixelOp;
	if (pColourThresholdPixelOp) delete pColourThresholdPixelOp;
	if (pPositivePixelOp) delete pPositivePixelOp;
	pThresholdPixelOp = NULL;
	pColourThresholdPixelOp = NULL;
	pPositivePixelOp = NULL;

	XSize = 0;
	YSize = 0;

	if (!pTraceRegion) if ((pTraceRegion /*assign*/ = new TraceRegion) == NULL) return FALSE;
	if (!pFill) if ((pFill /*assign*/ = new LinearFillAttribute) == NULL) return FALSE;

	if (!theOriginal) // They've asked us not to use this bitmap after all
	{
		// if we have a proposed copy, delete it
		if (pProposed) delete pProposed;
		if (pUndithered) delete pUndithered;

		pProposed = NULL;
		pUndithered = NULL;

		// record no original
		pOriginal = NULL;
		return TRUE;	// We succeeded
	}
	// if we have a proposed copy, delete it
	if (pProposed) delete pProposed;
	if (pUndithered) delete pUndithered;

	if (!RemoveTree(TRUE)) return FALSE;
	ProgressDone=0;

	pProposed = NULL;
	pUndithered = NULL;

	// Make up some pixel ops - if any memory allocs fail pOriginal will be NULL still
	if (Method == TRACEMETHOD_TRUECOL)
	{
		if ((pThresholdPixelOp = /*assign*/ new BfxThresholdPixelOp) ==NULL) return FALSE;
		if ((pColourThresholdPixelOp = /*assign*/ new BfxColourThresholdPixelOp) ==NULL) return FALSE;
		if ((pPositivePixelOp = /*assign*/ new /*BfxPositivePixelOp*/BfxColourThresholdPixelOp) ==NULL) return FALSE;
	}
	else
	{
		if ((pThresholdPixelOp = /*assign*/ new BfxThresholdPixelOpPseudo) ==NULL) return FALSE;
		if ((pColourThresholdPixelOp = /*assign*/ new BfxColourThresholdPixelOpPseudo) ==NULL) return FALSE;
		if ((pPositivePixelOp = /*assign*/ new /*BfxPositivePixelOp*/BfxColourThresholdPixelOpPseudo) ==NULL) return FALSE;
	}


	// Record for us
	pOriginal = theOriginal;

	FirstShape = TRUE;

	return TRUE; 
}

/********************************************************************************************

>	BOOL TraceControl::RemoveTree(BOOL DeleteIt = TRUE)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		DeleteIt = TRUE to delete the tree, FALSE to merely disclaim responsibility for it
	Outputs:	None
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Relinquishes control of the tree structure
	Errors:		mainly memory.
	Scope:		Public
	SeeAlso:	-

This call would be called with TRUE to delete the tree for instance on destruction of the object
or if the trace was cancelled, but with FALSE if the tree was inserted into a doc.

This call does nothing if the tree isn't there

********************************************************************************************/

BOOL TraceControl::RemoveTree(BOOL DeleteIt)	
{
	if (!DeleteIt) {pTree = NULL; pJoinNode=NULL; return TRUE;}

	if (pTree)
	{
		Node * pFirstChild = pTree->FindFirstChild();
		if (pFirstChild) pTree->DeleteChildren(pFirstChild);
		delete pTree;
		pTree = NULL;
		pJoinNode = NULL;
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL TraceControl::GetBitmaps( KernelBitmap * * theOriginal = NULL,
									KernelBitmap * * theProposed = NULL,
									KernelBitmap * * theUndithered = NULL	)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		None
	Outputs:	theOriginal = pointer to a place to store a pointer to the original to trace
				   		   we'll be using. May be NULL if no record required of this.
				theProposed = pointer to a place to store a pointer to the proposed image
						   we'll be using. May be NULL if no record required of this.
				theTemp		= pointer to a place to store a pointer to the temp image
						   we'll be using. May be NULL if no record required of this.
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Returns the current bitmap pointers
	Errors:		None yet
	Scope:		Public
	SeeAlso:	-

None the bitmap pointers may be NULL if not initialised yet (for instance if no tracing has
yet been done).

********************************************************************************************/

BOOL TraceControl::GetBitmaps( KernelBitmap * * theOriginal,
								KernelBitmap * * theProposed, KernelBitmap * * theUndithered)
{

	if (theOriginal) *theOriginal = pOriginal;
	if (theProposed) *theProposed = pProposed;
	if (theUndithered) *theUndithered = pUndithered;
	return TRUE;
}

/********************************************************************************************

>	BOOL TraceControl::Trace(BOOL * Done)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		None
	Outputs:	Done - pointer to BOOL to indicate completion
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Spits out a single object into the trace output stream
	Errors:		Many possible
	Scope:		Public
	SeeAlso:	-

This is the central routine that actually orchestrates tracing of a single object. Note that
a return of FALSE indicates an error condition (and tracing should be abandonned). Successful
completion is indicated by the setting of the Done flag. It is guaranteed that (a) the Done
flag will be either cleared or set (unless you are silly enough to pass a Null pointer in 0
in which case you wouldn't be able to check it anyway), and (b) the Done flag will be clear
if an error condition occurs. Thus Done ALWAYS indicates successful completion.

********************************************************************************************/

BOOL TraceControl::Trace(BOOL * Done)
{
	// First set done flag to false after checking it exists. This way all
	// exits will be clean.
	ERROR2IF(!Done, FALSE, "Don't call TraceControl::Trace with a NULL Done ptr");
	*Done = FALSE;

	ERROR2IF((!pOriginal), FALSE, "Don't call TraceControl::Trace without calling InitBitmaps() first");

	// Welcome to control algorithm No.2. Hopefully this one won't turn out exponential.
	//
	// Minor introduction to state storage:
	//
	// Various things about the state of tracing are stored across path generation. These are:
	//		InitialAreaErrorThreshold - this decreases monotonically throughout the trace
	//		ErrorRegionList - (see MarkInitialArea()) and it's information in top byte of ProposedImage
	//
	// No, OK it wasn't exponential, merely square. Now I allege we have an O(n) algorithm or at least
	// an O(n^(3/2)) one. Welcome to algorithm no. 3 and the world of the BfxPixelOp.

	// Do some stuff on the first call
	if (FirstShape)
	{
		if (!RemoveTree(TRUE)) return FALSE;

		if ((pTree = /*assign*/ new NodeGroup) ==NULL ) return FALSE;
		if ((pJoinNode = /*assign*/ new AttrStrokeColour(pTree, FIRSTCHILD)) == NULL) return FALSE;
		DocColour TheColour(COLOUR_NONE);
		((AttrStrokeColour *)pJoinNode)->SetStartColour(&TheColour);
		NodePath * pRect;
		AttrFillGeometry * pRectCol;
		if (!(((pRect = /*assign*/ new NodePath(pJoinNode, NEXT))!=NULL) && pRect->SetUpPath(32, 32))) return FALSE;
		if (!(((pRectCol = /*assign*/ new AttrFlatColourFill(pRect, FIRSTCHILD))!=NULL))) return FALSE;
		pJoinNode=pRect;
	
		TRACEUSER( "Alex", _T("> Removing dither\n"));
		if (pProposed) delete pProposed;
		if (pUndithered) delete pUndithered;

		pProposed = NULL;
		pUndithered = NULL;

		ERROR2IF(!ALU, FALSE, "OK, where's the ALU gone?");

		if (!ALU->SetB(pOriginal)) return FALSE;
		INT32 NumColours = (INT32)(17.5+(255.0-17.0)*QuantColours);
		if (Method==TRACEMETHOD_GREYSCALE) NumColours = (INT32)(2.5+(255.0-2.0)*QuantColours);
		if (!ALU->RemoveDither(&pUndithered, (INT32)(Blur*2.55+0.5), NumColours, Method)) return FALSE;
		ERROR2IF(!pUndithered, FALSE, "How come remove dither failed to generate a KB without generating an error");

		EndSlowJob();
		String_64 ProcessMessage(_R(IDS_TRACEDLG_TRACING));
		BeginSlowJob(100, FALSE, &ProcessMessage);

		// Now we should go and claim proposed bitmap the size of the undithered one
		if ((pProposed = /*assign*/ ALU->NewBitmap(pUndithered,0,0/*,32*/)) == NULL)
		{
			delete pUndithered;
			pUndithered = NULL;
			return FALSE;
		}

		ALU->SetA(pProposed);
		ALU->ZeroA(0xFF);

		if (!ALU->GetSize(pUndithered, &XSize, &YSize)) return FALSE;

		if (!(
			pThresholdPixelOp->SetBitmap(pUndithered, BIT_CACHEV_THRESHOLD, BIT_CACHES_THRESHOLD, FALSE) &&
			pColourThresholdPixelOp->SetBitmap(pUndithered, BIT_CACHEV_COLOURTHRESHOLD, BIT_CACHES_COLOURTHRESHOLD, FALSE) &&
			pPositivePixelOp->SetBitmap(pUndithered, BIT_CACHEV_POSITIVE, BIT_CACHES_POSITIVE, FALSE) &&

			TRUE)) return FALSE;

		TRACEUSER( "Alex", _T("> First shape, here we go\n"));
	 	FirstShape = FALSE;
		ShapeFreePass = FALSE;
		InitialAreaErrorThreshold = MaximumInitialAreaErrorThreshold;
		GradFillError = 1.0;
		NewGradFillError = 0.0;
		if (!(
			pThresholdPixelOp->ClearEntireCache() &&
			pColourThresholdPixelOp->ClearEntireCache() &&
			pPositivePixelOp->ClearEntireCache() &&
			TRUE)) return FALSE;
		
		// set aux bitmaps up so we can start reading colours
		if (!pPositivePixelOp->SetAuxilliaryBitmaps(NULL, pProposed, pUndithered, 0, 0))
			return FALSE;


#ifdef _DEBUG
		for (INT32 i=0; i<NUM_TRACECTL_DEBUG_BITMAPS; i++) if (pDebug[i]) {delete pDebug[i];pDebug[i]=NULL;DebugInvalid[i]=TRUE;}
#endif
		SetDebugBitmap(0, pUndithered);

		TotalPasses=0;
		NumPasses=0;
		NumPaths=0;
		NumCusps=0;
		NumPoints=0;
		ProgressDone=0;

		double iaet=InitialAreaErrorThreshold;
		while (TRUE)
		{

			// This is the same loop we use
			if (iaet<=MinimumInitialAreaErrorThreshold)
				iaet=MinimumInitialAreaErrorThreshold;
			TotalPasses++;

			if (iaet-1E-10<=MinimumInitialAreaErrorThreshold)
				break;
			iaet *= InitialAreaErrorRatio;
		}


		// build a path
		pRect->InkPath.FindStartOfPath();

		// Start at bottom left corner
		pRect->InkPath.InsertMoveTo(DocCoord(0,0));
		pRect->InkPath.InsertLineTo(DocCoord(0,YSize<<8));
		pRect->InkPath.InsertLineTo(DocCoord(XSize<<8,YSize<<8));
	    pRect->InkPath.InsertLineTo(DocCoord(XSize<<8,0));
		pRect->InkPath.InsertLineTo(DocCoord(0,0));

		// Close the path properly
		pRect->InkPath.CloseSubPath();
		pRect->InkPath.IsFilled = TRUE; // What lovely encapsulation & abstraction...

		// And give it a vaguely appropriate colour
		GFStartC = GFEndC = (DWORD)(pPositivePixelOp->ReadOriginalPixel(0, 0)) &0x00FFFFFF;
		INT32 R = 0;
		INT32 G = 0;
		INT32 B = 0;
		pPositivePixelOp->TranslateToRGB(GFStartC, pUndithered, &R, &G, &B);
		DocColour RectColour( R, G, B );
		GFStartX = 0;
		GFStartY = 0;
		GFEndX = XSize;
		GFEndY = YSize;
		pRectCol->SetStartColour(&RectColour);
		return PlotGradFill(TRUE, FALSE);
		// And next time do some more interesting shapes as FirstShape is FALSE
	}

	// Set proposed path to whole bitmap
	HavePath = FALSE;

	// There are two embedded exits from the main region finding loop - when we've found a beneficial region we
	// want to stick in the tree, and when we have decided the trace is complete.	
	while (TRUE)
	{
		ERROR1IF(BreakKeyHit(), FALSE, _R(IDS_ABORT));

		// Get the next error area out.
		// Occasionally (when there are no regions left in the error region list), this will result in a 
		// complete pass through the bitmap determining threshold error (and a recalculation of the threshold)
		// and the building up of a new error list. At the end we know that the temp bitmap is marked with
		// REGION_PIXEL
		if (!MarkInitialArea()) return FALSE;

		if (!FoundRegion)
		{
			// Wow! We've found *no* regions whose errors are greater than the Minimum threshold
			// error specified. This means (amazingly) we've done the trace!
			*Done = TRUE;
			return TRUE;
		}

		//TRACEUSER( "Alex", _T("Found Initial region at %d, %d, colour %x\n"),InitialX, InitialY, ScanPointColour);


		// We do a number of passes to refine the area. These consist of tracing the current region, finding the best
		// grad fill for it, and finding the region that grad fill positiviely affects. We must do at least two
		// passes.
		INT32 Passes = 0;
		BOOL UseColourThresholdPixelOp = TRUE;
		do
		{
			// InitialX & InitialY are set up. Our next step is to trace the region. We trace at first with
			// the minimum accuracy (i.e. the max PixelError) and reduce the accuracy until we have got a path
			// that works. Works means it has at least n% of the pixels of the original region.
			PixelError = MaxPixelError;
			BOOL RegionTraceable = FALSE;
			while ((PixelError>=MinPixelError) && !RegionTraceable)		
			{
				if (!(
					pTraceRegion->SetParams(&PixelError) &&
					TracePath(UseColourThresholdPixelOp?pColourThresholdPixelOp:pPositivePixelOp) &&
							  						// Trace region to path
					CalculateStatistics() &&		// Calculate grad fill stats
					TRUE)) return FALSE;
				if (PixelError>MinPixelError)
				{
					PixelError *= PixelErrorRatio;
					if (PixelError<MinPixelError) PixelError=MinPixelError;
				} else PixelError=0.0; // Ensures loop terminates
				RegionTraceable = (!pStats) || (pStats->N > 2); // Grrr with all these fancy pixel ops we can't get the area
												   // as it's not calculated yet. Oh well.
				//( ((double)(pStats->N))/((double)RegionArea)>PixelAccuracyRatio );
			}

			UseColourThresholdPixelOp = FALSE;

			if (RegionTraceable)
			{
				HavePath = TRUE;

				// We have to plot the things twice at the moment to do the error calc until we've performed
				// more GDraw mungery
				if (!(
					FormGradFill() &&  				// Determine best grad fill
//					PlotGradFill(FALSE, FALSE) &&	// Plot proposed + grad fill to temp
					TRUE)) return FALSE;
/*
We'll have to do this another way (like just do it on the cached area)

				if (FoundRegion && (NewGradFillError >= GradFillError))
				{
					TRACEUSER( "Alex", _T("> This region doesn't reduce the error!\n"));
					FoundRegion=FALSE; // We've gone seriously awry!
				}
*/
				INT32 theErrorThreshold = (INT32)(0.5+3.0*DoubleSquare(InitialAreaErrorThreshold * 255.0));
				if (!pPositivePixelOp->ClearCachedArea()) return FALSE;
				// This is a bodge until we have a way to calculate the true grad fill colour
				if (!pPositivePixelOp->SetAuxilliaryBitmaps(NULL, pProposed, pUndithered, theErrorThreshold,
						GFStartC /*pPositivePixelOp->ReadOriginalPixel(InitialX, InitialY) &0x00FFFFFF */))
					return FALSE;

				// BODGE! We start in the middle and look around for somewhere helpful
				FoundRegion = pPositivePixelOp->IsInRegion(InitialX, InitialY);
				INT32 xadj;
				INT32 yadj;
				if (!FoundRegion) for (yadj=-1; (yadj<=1) && !FoundRegion; yadj++) 
					for (xadj=-1; (xadj<=1) && !FoundRegion; xadj++)
					if (pPositivePixelOp->IsInRegion(InitialX+xadj, InitialY+yadj))
					{
						FoundRegion=TRUE;
						InitialX+=xadj;
						InitialY+=yadj; // break done by setting FoundRegion as we can't do 2 level break
					}
						
				// OK OK so this clears out the cache. We've lost a maximum of 9 pixels.
				if (FoundRegion) if (!CheckMinimumArea(pPositivePixelOp)) return FALSE;
				// Sets FoundRegion
				RegionArea = MinimumArea + 1; // Bodge

				// If we marked any points as OK
				if (FoundRegion)
				{
					// Is the current path & grad fill good enough for us?
					// We must have done at least one top to bottom and one top to here pass. If we have
					// done the maximum number of passes that's OK too. Other than that, we continue refining
					// if the rate of decent of error is looking good.
					if (TRUE /*don't refine while we're not doing grad fills*/
						|| (Passes>=/*MaxRefinementPasses*/1) || ((Passes != 0) && !(FALSE /* some other loop criterion*/)))
					{
						
						if (!PlotGradFill(TRUE, TRUE)) return FALSE;
						if ((Smooth>3.0) && pCurrentPath)
						{
//							if (!pCurrentPath->InkPath.Smooth(Smooth*(256.0*65536.0/100.0))) return FALSE;

							INT32 orignumcoords = pCurrentPath->InkPath.GetNumCoords();

						 	// create a path and quantise to it
							Path QuantPath;
							INT32 startpoints=((orignumcoords>24)?orignumcoords:24)*5;
							QuantPath.Initialise(startpoints,(startpoints>2400)?2400:startpoints);

							// quantise original path to QuantPath
						    //pCurrentPath->InkPath.Quantise(64.0, &QuantPath);

							double coordsmooth = Smooth*10.24; /* Smooth/100.0*4.0*256.0; */
							
							ProcessFlags QuantFlags(TRUE,FALSE,TRUE);
							pCurrentPath->InkPath.CreateFlatPath(QuantFlags, coordsmooth, &QuantPath);

							// create and fit a smooth curve to the data points.
						    
						    // Full range up to 4 pixels out
						    CurveFitObject Smoothed(&QuantPath, coordsmooth*coordsmooth);

						    if (Smoothed.Initialise(&QuantPath,QuantPath.GetNumCoords()))
							{
								Smoothed.FitCurve();
								QuantPath.CloseSubPath();
								QuantPath.IsFilled=TRUE;

								// now that we've smoothed the curve lets check on the number
								// of coordinates we've generated. If its more, then we should
								// leave the original path as it was. If its less then replace
								if (QuantPath.GetNumCoords()<orignumcoords)
									if (!pCurrentPath->InkPath.CloneFrom(QuantPath)) return FALSE;
		
							}
						}

						ShapeFreePass=FALSE;
						//if (pErrorRegionList) pErrorRegionList->Empty(); // This is a bodge to ensure reevaluation
						GradFillError = NewGradFillError;

						if (pCurrentPath) NumPoints+=pCurrentPath->InkPath.GetNumCoords()/3;
						NumPaths++;
						//TRACEUSER( "Alex", _T("> Inserting path covering %d points after %d refinement(s)\n\n"),pStats->N, Passes);

						INT32 R = 0;
						INT32 G = 0;
						INT32 B = 0;
						pPositivePixelOp->TranslateToRGB(GFStartC, pUndithered, &R, &G, &B);
						DocColour TheColour( R, G, B );

						AttrFillGeometry * pFillGeometry = new AttrFlatColourFill;
						if (!pFillGeometry) return FALSE; //new sets error
						
						pFillGeometry->SetStartColour(&TheColour);
						pFillGeometry->AttachNode(pCurrentPath, FIRSTCHILD);

						// The first node has to be attached under pTree, the rest to the right
						if (pJoinNode)
						{
							pCurrentPath->AttachNode(pJoinNode, NEXT);
						}
						else
						{
							pCurrentPath->AttachNode(pTree, FIRSTCHILD);
						}

						pJoinNode = pCurrentPath;
						pCurrentPath = NULL;

						return TRUE;
					}

					// We do a refinement
				}
			}
			else
			{
				FoundRegion = FALSE;				// OK, so technically we found a region, but as we can't find
													// a path that encloses it, we might as well pretend we haven't...
			}

			Passes++;
		} while (FoundRegion);			// repeat refinement if pur region hasn't disappeared - else try next one
		TRACEUSER( "Alex", _T("< Rejecting malformed error region\n"));
		HavePath = FALSE;
	}

	
	// We should never get here
	ERROR3("OK, How come a while(TRUE) terminated? Has someone inserted a break or what?");
	return FALSE;
}

/*

Here's the old ROD code:

		// Don't loop if we have a low enough grad fill error, or it is reducing slower than GradFillErrorRatio
	} while (((GradFillError > MinGradFillError) &&
			( (PreviousGradFillError / GradFillError) > (1.0/GradFillErrorRatio)))
			|| (PreviousGradFillError ==0.0 ));
*/

/********************************************************************************************

>	BOOL TraceControl::GetProgress(INT32 * pPercent = NULL,
							 INT32 * pNumPaths = NULL,
							 INT32 * pNumCusps = NULL,
							 INT32 * pNumPoints = NULL,
							 BOOL * pProgressDone = NULL)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		Pointers to things to fill in
	Outputs:	*pPercent = percentage of way through trace
				*pNumPaths = number of paths
				*pNumCusps = number of cusps
				*pNumPoints = number of points
				*pProgressDone = TRUE if complete else false
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Returning information for status line function
	Errors:		None yet
	Scope:		Public
	SeeAlso:	None

The pointers may be NULL if you're not interested in getting them filled in

********************************************************************************************/

BOOL TraceControl::GetProgress(INT32 * pPercent,
							 INT32 * pNumPaths,
							 INT32 * pNumCusps,
							 INT32 * pNumPoints,
							 BOOL * pProgressDone)	
{
	if (pProgressDone) *pProgressDone = ProgressDone && pOriginal;
	if (pPercent)
	{
		INT32 percent=ProgressDone?100:(INT32)(1.0+100.0*(
			((double)ScanPointY/(double)(YSize?YSize:1)+(double)NumPasses)/(double)(TotalPasses?TotalPasses:1) ));
		if ((percent<0 ) || (!pOriginal) || (!ProgressDone && ScanPointX==0 && ScanPointY==0 && NumPasses==0)) percent=0;
		if (percent>100) percent=100;
		*pPercent=percent;
	}
	if (pNumPaths) *pNumPaths=NumPaths;
	if (pNumCusps) *pNumCusps=NumCusps;
	if (pNumPoints) *pNumPoints=NumPoints;
	return TRUE;
}


/********************************************************************************************

>	BOOL TraceControl::CheckMinimumArea(BfxPixelOp * pBfxPixelOp)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		pBfxPixelOp = pointer to the pixel op to use
	Outputs:	None
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Determines whether atleast (MinimumArea) pixels are within the region
	Errors:		None yet
	Scope:		Public
	SeeAlso:	TraceControl::FloodSearch

FoundRegion is set according the the result. The search starts at InitialX/InitialY

This call *must* be done immediately (yes, immediately) after the cache is cleared. If not
you lose all your caching. Har har.

********************************************************************************************/

BOOL TraceControl::CheckMinimumArea(BfxPixelOp * pBfxPixelOp)
{
	return pBfxPixelOp->CheckMinimumArea(MinimumArea, InitialX, InitialY, &FoundRegion);
}

/********************************************************************************************

>	BOOL TraceControl::SetPath(NodePath * pNodePath)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		pNodePath = pointer to the NP to use or NULL for whole image
	Outputs:	None
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Sets up the NP to use
	Errors:		None yet
	Scope:		Public
	SeeAlso:	-

It is the callers responsibility to deal with the old NP (in terms of memory management) if
this call is used.

********************************************************************************************/

BOOL TraceControl::SetPath(NodePath * pNodePath)
{
	pCurrentPath = pNodePath;
	if (!pCurrentPath) HavePath = FALSE;
	return TRUE;
}

/********************************************************************************************

>	NodePath * TraceControl::GetPaths()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		None
	Outputs:	None
	Returns:	^CurrentPath or NULL if none
	Purpose:	Returns the current path
	Errors:		None yet
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

Node * TraceControl::GetPaths()
{
	return pTree;
}

/********************************************************************************************

>	BOOL TraceControl::MarkInitialArea()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Marks out intial area in temp bitmap
	Errors:		Many possible
	Scope:		Public
	SeeAlso:	-

We find the next error region in the error region list and ensure that this is marked as
REGION_PIXEL in the temporary bitmap. We do this by copying from the top byte of the
Proposed bitmap if there are more regions on the list, otherwise we generate a new set of
error regions in the following manner:

The temporary bitmap is marked such that all those points whose squared errors exceed
InitialAreaError^2 are set and all others cleared. Then BuildErrorList is called to build up
the error list (well that's a surprise).

Note we don't actually need the path for this as we can assume that the whole path is set,
which is fair enough as the initial area will only be required when we haven't got a path.

********************************************************************************************/

BOOL TraceControl::MarkInitialArea()
{
	ERROR3IF (HavePath, "MarkInitialArea() called when a path was set up. Wierd...");
	ERROR3IF ((!pUndithered) || (!pProposed), "MarkInitialArea() missing some bitmaps");

	BOOL FirstCall = FALSE;

	if (!pErrorRegionList)
	{
		if ((pErrorRegionList = /*assign*/ new BfxErrorRegionList) == NULL) return FALSE;
		if (!pErrorRegionList->Init(1000))
		{
			delete pErrorRegionList;
			pErrorRegionList = NULL;
			return FALSE;
		}
		FirstCall = TRUE;
	}

	// What the main loop does: (just in case you were wondering)
	//
	// Tries to find something on the error region list
	// If it's now empty:
	//  tries the next scan point.
	//  (If the next scanpoint is off the end of the bitmap, resets it and tries again using a lower threshold,
	//   if it still can't find one, we're done)
	//  Marks regions of approximately that colour which form the new error region list (sorted by size)
	// Takes the next error region
	// Repeats until the next error region is greater or equal to 2 pixels in size.
	do
	{

		pErrorRegionList->Next(); // Move on to next one (doens't move if EOL)
		while (!pErrorRegionList->GetCurrent(&InitialX,&InitialY,&RegionArea)) // at end of list
		{
			// Go back to start
			pErrorRegionList->ResetRead();

			if (TRUE || ShapeFreePass || !pErrorRegionList->GetCurrent(&InitialX,&InitialY,&RegionArea)) // if no items on new list
			{
				double AError = 0.0;
				double BError = 0.0;
				INT32 TotalPixels = 0;
				INT32 MarkedPixels = 0;

				INT32 theErrorThreshold = (INT32)(0.5+3.0*DoubleSquare(InitialAreaErrorThreshold * 255.0));
				

				// Find the next ScanPoint
				BOOL FoundScanPoint=FALSE;
				
				do
				{
					// Don't look for one if we haven't done the very first area mark yet
					if (!FirstCall)
					{
						theErrorThreshold = (INT32)(0.5+3.0*DoubleSquare(InitialAreaErrorThreshold * 255.0));
						if (!(ALU->SetB(pProposed) && ALU->SetT(pUndithered))) return FALSE;
						if (!(ALU->ScanThreshold(theErrorThreshold, &ScanPointX, &ScanPointY, NULL, &FoundScanPoint, TRUE))) return FALSE;
					}
										
					// If we haven't done the first area mark yet, or didn't find a point, we need to mark the area
					if (FirstCall || !FoundScanPoint)
					{
						// Reset ScanPoint
						ScanPointX=0;
						ScanPointY=0;
						
						// Only on noninitial calls do we need to reduce the threshold
						if (!FirstCall)
						{
							NumPasses++;
							if (InitialAreaErrorThreshold-1E-10<=MinimumInitialAreaErrorThreshold)
							{
								FoundRegion = FALSE; // We haven't & can't find an error region - so all done!
								ProgressDone=TRUE;
								return TRUE;	     // OK though
							}
							InitialAreaErrorThreshold *= InitialAreaErrorRatio;
						}
						
						// We're certainly not an initial call now
						FirstCall=FALSE;

						// Calculate new error threshold
						if (InitialAreaErrorThreshold<=MinimumInitialAreaErrorThreshold)
							InitialAreaErrorThreshold=MinimumInitialAreaErrorThreshold;
						TRACEUSER( "Alex", _T("> Reducing InitialAreaErrorThreshold to %f\n"),InitialAreaErrorThreshold);

					}				
				} while (!FoundScanPoint);

				// OK, we've found a ScanPoint to start off with. Hurrah. Now we take it's colour and build an ErrorRegion
				// List based on that colour

				// Clear the bit off, we don't want to deal with this pixel again unless it is remarked at a lower
				// threshold. Note that if we actuially plot over this pixel, We later clear this bit for anything
				// positively affected
				//pUndithered->PlotPixel(ScanPointX,ScanPointY, pUndithered->ReadPixel(ScanPointX, ScanPointY) & ~BIT_THRESHOLD);



				// Obtain the pixels colour
				ScanPointColour =pPositivePixelOp->ReadOriginalPixel(ScanPointX, ScanPointY) & 0x00FFFFFF;				


				// Now mark on thresholded error. What this means is we're marking all the pixels of a similar colour
				// to the one which we've just found which are 'wrong' in the proposed bitmap as well. We'll use this
				// as the first basis of our trace. We hope the area is large enough to give us a good approximation to
				// a grad fill. This will hopefully extend over a larger area on the first refinement if (say) we've
				// picked a grad filled band.
				if (!pColourThresholdPixelOp->ClearCachedArea()) return FALSE;
				if (!pColourThresholdPixelOp->SetAuxilliaryBitmaps(NULL, pProposed, pUndithered, theErrorThreshold, ScanPointColour))
					return FALSE;
				
				// For the time being just insert the area we are using
				InitialX=ScanPointX; InitialY=ScanPointY;

				ScanPointX++;
				if (ScanPointX>=XSize) {ScanPointX=0;ScanPointY++;} // we deliberately let this overrun

				// Check minimum area & set FoundRegion
				if (!CheckMinimumArea(pColourThresholdPixelOp)) return FALSE;
				RegionArea = MinimumArea + 1; // Bodge

				if (!pColourThresholdPixelOp->IsInRegion(InitialX, InitialY))
				{
					FoundRegion=FALSE;
					ERROR3IF(IsUserName("Alex"),"ScanThreshold lied");
				}

				if (!pErrorRegionList->Empty()) return FALSE;
				if ((FoundRegion) && (RegionArea>=MinimumArea)) if (!(pErrorRegionList->Insert(InitialX, InitialY, RegionArea))) return FALSE;
				if (!(pErrorRegionList->Sort() && pErrorRegionList->ResetRead())) return FALSE; 
				
			}
			// Mark begining of (as yet) shape free pass
			ShapeFreePass = TRUE;
		}

		// We've got one
		FoundRegion = TRUE;

	} while // validation - Loop if it's NOT (valid region area, AND this pixel set AND any adjacent pixel set)
		(!(	(RegionArea>MinimumArea) &&
			(pColourThresholdPixelOp->IsInRegion(InitialX, InitialY)) &&
			( // an adjacent pixel is set
		  		pColourThresholdPixelOp->IsInRegion(InitialX-1, InitialY  ) ||
		  		pColourThresholdPixelOp->IsInRegion(InitialX+1, InitialY  ) ||
		  		pColourThresholdPixelOp->IsInRegion(InitialX,   InitialY-1) ||
		  		pColourThresholdPixelOp->IsInRegion(InitialX-1, InitialY-1) ||
				pColourThresholdPixelOp->IsInRegion(InitialX-1, InitialY-1) ||
		  		pColourThresholdPixelOp->IsInRegion(InitialX,   InitialY+1) ||
		  		pColourThresholdPixelOp->IsInRegion(InitialX-1, InitialY+1) ||
		  		pColourThresholdPixelOp->IsInRegion(InitialX-1, InitialY+1) )
		));

	return (pErrorRegionList->MarkUsed());
}


/********************************************************************************************

>	BOOL TraceControl::TracePath(BfxPixelOp * pBfxPixelOp)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	A NodePath is traced from the temporary bitmap
	Errors:		Many possible
	Scope:		Public
	SeeAlso:	-

The marked pixels from the temporary bitmap which are connected to Initial{X,Y} are turned
into a NodePath which is then stored in CurrentPath.

********************************************************************************************/

BOOL TraceControl::TracePath(BfxPixelOp * pBfxPixelOp)
{
	if (!pCurrentPath)
		if (!(((pCurrentPath = /*assign*/ new NodePath)!=NULL) && pCurrentPath->SetUpPath(32, 32))) return FALSE;
	ERROR3IF ((!pUndithered) || (!pProposed) || (!pBfxPixelOp), "TracePath() missing some bitmaps");

	return( pTraceRegion->UseBitmap(pUndithered) &&
			pTraceRegion->UsePath(&(pCurrentPath->InkPath)) &&
			pTraceRegion->Trace(InitialX, InitialY, pBfxPixelOp) &&
			TRUE );
}

/********************************************************************************************

>	BOOL TraceControl::CalculateStatistics()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	To determine the stats that help us find the optimum grad fill.
	Errors:		Many possible
	Scope:		Public
	SeeAlso:	-

GDraw is called to plot the trace path in a way such that we obtain various useful stats.

********************************************************************************************/

BOOL TraceControl::CalculateStatistics()
{
#if TRACEUSESTATS
	if (!pStats) if ((pStats = /*assign*/ new KernelStatistics) == NULL) return FALSE;

	ERROR3IF ((!pUndithered) || (!pProposed), "CalculateStatistics() missing some bitmaps");
	ERROR2IF ((!pCurrentPath), FALSE, "CalculateStatistics() called without path to grad fill"); 
	
	if (!(ALU->SetA(pUndithered))) return FALSE;

	//TRACEUSER( "Alex", _T("Calc stats\n"));

	return (ALU->GetStatistics(&(pCurrentPath->InkPath), pStats));
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	BOOL TraceControl::FormGradFill()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Calculate the optimum grad fill from the stats available.
	Errors:		Many possible
	Scope:		Public
	SeeAlso:	-
																								    
We do some cunning processing and end up with a grad fill attribute. We use the statistics
from CalculateStatistics above.

********************************************************************************************/

BOOL TraceControl::FormGradFill()
{
	GFStartX = 0;
	GFStartY = 0;
	GFEndX = XSize;
	GFEndY = YSize;

#if TRACEUSESTATS
	ERROR3IF((!pStats), "How come we got here without a statistics block then?!");
	INT32 R = (INT32)(pStats->R * 255.0);
	INT32 G = (INT32)(pStats->G * 255.0);
	INT32 B = (INT32)(pStats->B * 255.0);
#if 0
UINT32 pval = (UINT32)(pStats->Pixel);
R = (pval/*>>0*/) & 0xFF; // Bitmaps are TRGB, colourrefs are TBGR
G = (pval>>8) & 0xFF;
B = (pval>>16) & 0xFF;
#endif

	if (R<0) R=0;
	if (G<0) G=0;
	if (B<0) B=0;
	if (R>255) R=255;
	if (G>255) G=255;
	if (B>255) B=255;

	GFStartC = GFEndC = (DWORD)(R | (G<<8) | (B<<16));

	//TRACEUSER( "Alex", _T("Trying colour R=%d G=%d B=%d N=%d\n"),R,G,B,pStats->N);

	return TRUE;

#else

	GFStartC = GFEndC = (DWORD)(pPositivePixelOp->ReadOriginalPixel(InitialX, InitialY)) &0x00FFFFFF;
	return TRUE;

#endif
}

/********************************************************************************************

>	BOOL TraceControl::PlotGradFill(BOOL ToProposed, BOOL UsePath = TRUE)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		ToProposed set if to plot to Proposed rather than temp
				UsePath = TRUE to plot path, FALSE to plot whole area
	Outputs:	None
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Plots the grad fill on top of the proposed bitmap into the temp bitmap
	Errors:		Many possible
	Scope:		Public
	SeeAlso:	-

Pretty much a straight GDraw call or two.

********************************************************************************************/

BOOL TraceControl::PlotGradFill(BOOL ToProposed, BOOL UsePath)
{
	ERROR3IF ((!pUndithered) || (!pProposed) || (!ToProposed && !pUndithered), "PlotGradFill() missing some bitmaps");
	if (UsePath) ERROR2IF ((!pCurrentPath), FALSE, "PlotGradFill() called without path to grad fill"); 

	if (ToProposed)
	{
		if (!(ALU->SetA(pProposed))) return FALSE;
	}
	else
	{
		if (!(ALU->SetA(pUndithered) && ALU->SetB(pProposed) && ALU->ByteCopyBA())) return FALSE;
	}

	return (ALU->GradFillPath(UsePath?&(pCurrentPath->InkPath):NULL, GFStartC, GFEndC,
							  GFStartX, GFStartY, GFEndX, GFEndY, FALSE, TRUE));
}



