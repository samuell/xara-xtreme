// $Id: rsmooth.cpp 662 2006-03-14 21:31:49Z alex $
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
// Implementation file containing routines to control the interactive smoothing
// of selected path regions

/*
*/

#include "camtypes.h"
#include "paths.h"
#include "rsmooth.h"
#include "nodepath.h"
#include "docview.h"
#include <math.h>
#include "memory.h"
//#include "mike.h"
#include "undoop.h"
#include "document.h"
#include "objchge.h"
#include "nodeblnd.h"
#include "nodebldr.h"

CC_IMPLEMENT_DYNCREATE( OpRetroSmooth, SelOperation )

DECLARE_SOURCE("$Revision: 662 $");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	RetroSmooth::RetroSmooth()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Purpose:	RetroSmooth constructor
				Sets default values for internal variables used by the RetroSmooth
				class.
	SeeAlso:	-

********************************************************************************************/

RetroSmooth::RetroSmooth()
{
	pRetroNode = NULL;
	pRetroSpread = NULL;
	RetroSmoothAcc = SMOOTH_MIN;
	RetroSmoothing = FALSE;
	RetroPathCRC = 0;
	RetroSelCRC = 0;
	RetroSmoothPercent = 0;

	m_bRender = TRUE;
}



/********************************************************************************************

>	BOOL RetroSmooth::Initialise()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		-
	Returns:	TRUE 	- then the retro smooth initialisation has succedded
				FALSE	- then no memory to perform the retro smooth. The caller should
						  abort the operation.
	Purpose:	Initialise various internal lists of data used by the retrosmooth class.

********************************************************************************************/

BOOL RetroSmooth::Initialise()
{
	BOOL ok = RetroPath.Initialise(12,12);
	if (ok) ok = RetroEorPath.Initialise(12,12);
	if (ok) ok = RetroEorRegion.Initialise(12,12);
	return ok;
}



/********************************************************************************************

>	void RetroSmooth::Changing(NodePath* pNodePath, Spread* pSpread, double smooth)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		pNodePath	= pointer to a node path to smooth
				pSpread		= pointer to the nodes spread
				smooth 		= smoothness level to use
	Returns:
	Purpose:	This function is called when a change has occured to the retrosmooth input
				control device, and it wishes to inform the retrosmoother. The function takes
				a node and spread and renders a new smoothed eor'd version of the path to
				the display device. No changes occur to the NodePath provided. All changes
				occur internally to this smooth class.
				If this is the first of a sequence of calls to the changing function,
				a cached version of the path is created, along with information about the
				selected regions within the path. On subsequent calls, the cached version
				of the path will be used to create a smooth display path. 

********************************************************************************************/

void RetroSmooth::Changing(NodePath* pNodePath, Spread* pSpread, double smooth)
{
/*
	// Find the docview that we are looking at
	DocView* pDocView = DocView::GetCurrent();

	// Refit the path - First work out the current error factor
	double ScaleFactor = (pDocView->GetViewScale()).MakeDouble();
	double ErrorLevel = (64 + (160*smooth)) / ScaleFactor;
	RetroSmoothAcc = ErrorLevel * ErrorLevel;
*/

	// calculate a useful smooth accuracy
	double newacc = SMOOTH_MIN + (SMOOTH_MAX - SMOOTH_MIN)*pow((smooth/100), 3.0);

	if (newacc == RetroSmoothAcc)
		return;

	// keep track of some variables for later
	RetroSmoothAcc = newacc;
	RetroSmoothPercent = smooth;
	pRetroSpread = pSpread;

	// if this is the first call to us then do the setup
	if (!RetroSmoothing)
		if (!RetroSmooth::Starting(pNodePath))
			return;

	// finally we are in a position to start retro activity.
	RetroSmoothing = TRUE;
	RetroSmooth::Update();		

}



/********************************************************************************************

>	void RetroSmooth::Finished()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		-
	Returns:	-
	Purpose:	This function should be called to terminate the retro smooth action. The
				data cached during the calls to RetroSmooth::Changing() will now be used to
				actually affect the path held in the document.

********************************************************************************************/

void RetroSmooth::Finished()
{
	// if no retro path, just do nothing
	if (!pRetroNode || !RetroSmoothing)
		return;
	// tidy up after retro smoothing
	RetroEorRegion.Empty();
	RecordPathRegions(&RetroEorPath);
	RetroPathCRC = RetroEorPath.CalcCRC();
	RetroSelCRC = RetroEorPath.CalcSelectedCRC();
	RetroSmoothing = FALSE;

	// finally create an op to really smooth the tree object
	// path and execute it.
	OpRetroSmooth* pOpRetroSmooth = new OpRetroSmooth;
	if (pOpRetroSmooth != NULL)
		pOpRetroSmooth->DoRetroSmooth(pRetroNode, &RetroEorPath, RetroSmoothAcc);
}


/********************************************************************************************

>	void RetroSmooth::Invalidate()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Returns:	-
	Purpose:	Invalidates all cached information about the currently active retro
				smoothing path. Cached paths and region buffers are compacted.

********************************************************************************************/

void RetroSmooth::Invalidate()
{
	pRetroNode = NULL;
	pRetroSpread = NULL;
	RetroSmoothAcc = SMOOTH_MIN;
	RetroSmoothing = FALSE;
	RetroPathCRC = 0;
	RetroSelCRC = 0;

	RetroPath.ClearPath(TRUE);	
	RetroEorPath.ClearPath(TRUE);
	RetroEorRegion.Empty();
}


/********************************************************************************************

>	double RetroSmooth::ReturnCachedAccuracy(Path* pPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		pPath = pointer to a path 
	Returns:	a percentage value for the current smoothness of this curve
	Purpose:	This function can be used to determin the smoothness of any curve. It will
				simply check for a match between the path given and an internal cached
				representation. If a match is found the current smoothness is returned, ie
				the last value passed to the RetroSmooth::Changing() function. If no match
				is found a value of 0 is returned, meaning the curve has not been smoothed. 

********************************************************************************************/

double RetroSmooth::ReturnCachedAccuracy(Path* pPath)
{
	// calculate the cyclic redundancy for this path.
	INT32 new_crc = pPath->CalcCRC();
	INT32 sel_crc = pPath->CalcSelectedCRC();

	if ((RetroPathCRC == new_crc) && (RetroSelCRC == sel_crc))
 		return RetroSmoothPercent;

	return 0;
}



/********************************************************************************************

	BOOL RetroSmooth::Starting(NodePath* pDocNodePath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		pDocNodePath = pointer to a node path to smooth
	Returns:
	Purpose:	Set up the internal data structures for this new NodePath. This allows us
				to continue resmoothing the path given new smooth values, until the outside
				world calls RetroSmooth::Finished. We cache the path and its selected regions
				to help us with some hairy redraw problems.
	SeeAlso:	RetroSmooth::Update()

********************************************************************************************/

BOOL RetroSmooth::Starting(NodePath* pDocNodePath)
{
	if (pRetroSpread == NULL)
		return FALSE;

	// calculate the cyclic redundancy for this path.
	INT32 new_crc = pDocNodePath->InkPath.CalcCRC();
	INT32 sel_crc = pDocNodePath->InkPath.CalcSelectedCRC();

	if (!new_crc)
		return FALSE;

	if ((RetroPathCRC == new_crc) && (RetroSelCRC == sel_crc))
 	{
		pRetroNode = pDocNodePath;
		TranslateRetroData(&(pDocNodePath->InkPath));
		return TRUE;
	}

	// if our data doesn't match the path given then we are now
	// in the situation where we need to reto smooth a new path
	// if there are no selected points in the path then we have
	// nothing to retro smooth
	if (!sel_crc)
		return FALSE;

	return NewPathToSmooth(pDocNodePath);
}


/********************************************************************************************

	void RetroSmooth::TranslateRetroData(Path* pPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		pPath = a path to translate the cached retro path to
	Returns:	-
	Purpose:	Translate the retro path data to the origin of pPath 

********************************************************************************************/

void RetroSmooth::TranslateRetroData(Path* pPath)
{
	ENSURE(pPath !=NULL, "NULL path passed to RetroSmooth::TranslateRetroData()");

	if (pPath == NULL)
		return;

	if (pPath->GetNumCoords() <= 0)
		return;

	DocCoord* Coords = pPath->GetCoordArray();
	DocCoord* RCoords = RetroPath.GetCoordArray();

	INT32 transx = Coords[0].x - RCoords[0].x;
	INT32 transy = Coords[0].y - RCoords[0].y;

	RetroPath.Translate(transx,transy);
	RetroEorPath.Translate(transx,transy);
}



/********************************************************************************************

	void RetroSmooth::Update()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		-
	Returns:	-
	Purpose:	We have been given a new smooth value. We need to resmooth our internal
				data and render an eor version to the display device.
				What we try to do (to avoid hideous eoring problems) is this
				(1) Create a temporary path and copy our cached path data to it
				(2) Create a temp region list to save smoothed region start end indexes
				(3) Go through a loop smoothing each region in the temp eor path
					whilst recording the new smoothed region indexes as we go.
				(4) Check this new data against our previous cached eorpath and region
					list. 
				(5)	If all match then
					 	do nothing as the smoothed path has not changed	since the last
						smooth even though we know the smooth value has.
					else
						render the cached eor path to the output device, removing the item
						copy the temporary eor path and region list to the cache eor
						path and region list
						render the cached eor path to the output device
				(6) exit 	

********************************************************************************************/

void RetroSmooth::Update()
{
	INT32 numcoords = RetroPath.GetNumCoords();

	// create a temp path to smooth
	Path RenderPath;
	if (!RenderPath.Initialise(numcoords, 12))
		return;
	RenderPath.CloneFrom(RetroPath);

	DynArray TempRegions;
	if (!TempRegions.Initialise(RetroEorRegion.NumItems(),12))
		return;

    INT32 Region = 0;
	INT32 index = 0;
	INT32 start,end;
	// now scan the path for regions of interest
    while ((index<numcoords) && RenderPath.FindSelectionLimits(index,&start,&end))
    {
   		INT32 nstart = start;
		INT32 nend = end;
        RenderPath.ExpandRange(&nstart,&nend,1);

		Set region;
		region.first = nstart;
		region.last = nend;
		region.Finclusive = !(nstart<start);
		region.Linclusive = !(nend>end);

		INT32 sel=0;
		if (nstart<start) sel |= 1;
		if (nend>end) sel |= 2;
		if (sel==0) sel=4;

        if (nstart != nend)
        {
			if (RetroSmoothAcc != SMOOTH_MIN)
			{
				BOOL ok = RenderPath.SmoothSection(nstart,&region.last,RetroSmoothAcc,sel);
        		if (!ok)
					return;
			}

			if (!TempRegions.AddItem(region))
				return;

            Region++;
        }

		numcoords = RenderPath.GetNumCoords();
        index = region.last;

		if (!RenderPath.FindNextEndPoint(&index))
			index=numcoords;
    }

	// now we've smoothed the path, we need to check to see if its worth
	// updating the version on screen.

	INT32 new_crc = RenderPath.CalcCRC();
	if (new_crc != RetroPathCRC)
	{
		// get rid of the old eor path and put the new path back on again.
		if (RenderRegions())
		{
			RetroEorPath.CloneFrom(RenderPath);
			RetroEorRegion.CloneFrom(&TempRegions);
	    	RenderRegions();
			RetroPathCRC = new_crc;
		}
	}
}



/********************************************************************************************

	statci BOOL RetroSmooth::FinishedNoUndo(NodePath* pNodePath, double SmoothValue)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		pNodePath	= pointer to a node path to use, the should generally be the same
				nodepath that was previously passed in to Changing()
				
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Finishes off the smoothing operation by replacing the path in pNodePath with
				the Retropath that we have generated.  This differs from the regular Finished
				function because it does not alter the tree and it does not launch an operation,
				therefore it is not undoable.
				
	See Also:   If you want to know why it had to be non-undoable look at the implementation
				of OpDrawBrush, where it is used in this way.
********************************************************************************************/

BOOL RetroSmooth::FinishedNoUndo(NodePath* pNodePath)
{
	ERROR2IF(pNodePath == NULL, FALSE, "pNodePath is NULL in RetroSmooth::RetroSmoothNoUndo");
	
	BOOL ok = TRUE;

#ifdef _DEBUG
	ok = RetroEorPath.CheckPathValid();
#endif

	if (ok)
	{
		pNodePath->InkPath.ClearPath(FALSE);
		pNodePath->InkPath.CopyPathDataFrom(&RetroEorPath);
	}
	RetroEorPath.ClearPath(FALSE);

	return ok;
}
	

/********************************************************************************************

	void RetroSmooth::SetRenderFlag(BOOL Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		Value - the value to set the flag
	Returns:
	Purpose:	This sets the flag that says whether or not to bother doing EOR rendering.
				
	Notes:		You may wish to set the flag to false if you are using the retrosmoother separately
				from the Bezier tool.  It will mean that you can call one-off smoothing procedures
				without having to put up with all those blobs being rendered.

********************************************************************************************/

void RetroSmooth::SetRenderFlag(BOOL Value)
{
	m_bRender = Value;
}


/********************************************************************************************

	BOOL RetroSmooth::NewPathToSmooth(NodePath* pNodePath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		pNodePath	= pointer to a node path to use.
	Returns:
	Purpose:	Given a new document node path object, we need to initialise various
				buffers to hold temporary data.
				
	Notes:		We keep a copy of the path data to refer back to each time a retro smooth
				changing message comes through. This allows us to go back to the original
				shape of the curve and smooth from that each time the user starts dragging
				the slider around, on the same path.
				We also keep a transient copy of the path that we are actually smoothing
				so that we can draw and undraw the sections which are changing.

********************************************************************************************/

BOOL RetroSmooth::NewPathToSmooth(NodePath* pNodePath)
{
	pRetroNode = pNodePath;

	// clear the path elements but dont bother moving memory
	RetroPath.ClearPath(FALSE);	
	RetroEorPath.ClearPath(FALSE);

	// take a local copy of some useful variables
	Path* pPath = &(pNodePath->InkPath);
	INT32 numcoords = pPath->GetNumCoords();

	if (RetroPath.MakeSpaceInPath(numcoords))
	{
		if (RetroEorPath.MakeSpaceInPath(numcoords))
		{
			RetroPath.CopyPathDataFrom(pPath);
			RetroEorPath.CopyPathDataFrom(pPath);
		}
		else
		{
			RetroPath.Compact();
			return FALSE;
		}
	}
	else
		return FALSE;

	// clear any regions
	RetroEorRegion.Empty();

	// try to record the path regions
	if (!RecordPathRegions(pPath))
	{
		RetroPath.ClearPath(TRUE);
		RetroEorPath.ClearPath(TRUE);
		return FALSE;
	}

	return TRUE;
}



/********************************************************************************************

	BOOL RetroSmooth::RenderRegions()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		-
	Returns:	TRUE	- then regions have been eor rendered
				FALSE	- then no regions have been rendered
	Purpose:	Scan through the cached region list, rendering the path and blobs in
				each region.

	Extra Note: Diccon 12/4/2000 I want to use the RetroSmoother as a one-off smoothing 
				procedure without the EOR Rendering, therefore I have added an extra flag
				which returns TRUE from this function without actually rendering anything.
********************************************************************************************/

BOOL RetroSmooth::RenderRegions()
{
	// if our flag is set then just return 
	if (!m_bRender)
		return TRUE;

	// check we've actually got a region to render	
	INT32 numregions = RetroEorRegion.NumItems();
	if (!numregions)
		return TRUE;

	INT32 regionsize = FindMasterRegion();
	if (!regionsize)
		return FALSE;

	// create a render path to copy sections into
	Path RenderPath;	   
	if (!RenderPath.Initialise(regionsize,12))
		return FALSE;

	// Go into a RenderOnTop loop
	RenderRegion* pRegion = DocView::RenderOnTop(NULL, pRetroSpread, ClippedEOR);
	while (pRegion!=NULL)
	{
		// render each region by copying the section into our temp path
		// and rendering each of these.		
		INT32 reg = numregions;
		while (reg>0)
		{
			reg--;

			INT32 start = RetroEorRegion[reg].first;
			if (!RetroEorRegion[reg].Finclusive)
				RetroEorPath.FindNextEndPoint(&start);

			INT32 end = RetroEorRegion[reg].last;
			if (!RetroEorRegion[reg].Linclusive)
				RetroEorPath.FindPrev(&end);

			INT32 length = end - start + 1;
			if (length>0)
			{
				// make a path to render
				if (RetroEorPath.MakePathFromSection(start, length, &RenderPath))
				{
					RenderPath.RenderPathBlobs(pRegion);
					// set a colour during our render
//					pRegion->SetLineColour(COLOUR_XOREDIT);
//					pRegion->SetLineWidth(0);
//					pRegion->DrawPath(&RenderPath);
					RenderPath.ClearPath(FALSE);
				}
			}
		}
		pRegion = DocView::GetNextOnTop(NULL);
	}

	return TRUE;
}


/********************************************************************************************

	INT32 RetroSmooth::FindMasterRegion()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:
	Returns:	The largest region
	Purpose:	Scans the region list and finds the region with the greatest number of
				coordinates in it, returning that number

********************************************************************************************/

INT32 RetroSmooth::FindMasterRegion()
{
	INT32 numcoords = RetroEorRegion.NumItems();
	INT32 largest=0;

	for (INT32 i=0; i<numcoords; i++)
	{
		INT32 length = RetroEorRegion[i].last - RetroEorRegion[i].first +1;
		if (length>largest)
			largest = length;
	}
	return largest;
}



/********************************************************************************************

	BOOL RetroSmooth::MatchRegions(NodePath* pNodePath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		pNodePath - a nodepath object pointer
	Returns:	TRUE	- if the selected regions within pNodePath match those of our
						  cached path.
				FALSE	- if no match is found.

	Purpose:	Scan through the path data in pNodePath and match selected regions
				with our cached version. If the regions match, then we may be able to
				use our previously cached none smooth path as the source for a new
				retro active smooth, rather than pNodePath.

********************************************************************************************/

BOOL RetroSmooth::MatchRegions(NodePath* pNodePath)
{
	// ok, scan through all sel regions in this cached
	// object and match against those in the searchpath
	// if they all match, then return true.

	
	return TRUE;
}


/********************************************************************************************

>	BOOL RetroSmooth::RecordPathRegions(Path* pPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		pPath =
	Returns:
	Purpose:	Record the selected regions of the path on the original regions list
				or the smoothed regions

********************************************************************************************/

BOOL RetroSmooth::RecordPathRegions(Path* pPath)
{
    INT32 index = 0;
    INT32 ncoords = pPath->GetNumCoords();

	// The reasons are deeply rooted why I'm about to use a doccoord class
	// to hold two longs which represent the start and end of a region.
	// Its because I cannot use 'templates' at the moment to define an
	// associative array, so the dynamic array class I'm about to used is
	// typed to hold doccoords instead of generic 'types'.

	INT32 start;
	INT32 end;
	
	while ( (index<ncoords) && pPath->FindSelectionLimits(index,&start,&end) )
	{
		Set PtSet;
		PtSet.first = start;
		PtSet.last = end;

		pPath->ExpandRange(&PtSet.first,&PtSet.last,1);

		PtSet.Finclusive = !(PtSet.first<start);
		PtSet.Linclusive = !(PtSet.last>end);

		if (!RetroEorRegion.AddItem(PtSet))
		{
			index = -1;
			break;
		}
		index=PtSet.last;
        if (!pPath->FindNextEndPoint(&index))
        	index=ncoords;
	}

	// make sure we've tidied up incase of failure
	if (index==-1)
	{
    	RetroEorRegion.Empty();
		return FALSE;
	}

	return TRUE;
}


/********************************************************************************************

	void RetroSmooth::AlterRegion(INT32 index, INT32 newstart, INT32 newend) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Purpose:	Alters a specific regions start,end indexes.

********************************************************************************************/

void RetroSmooth::AlterRegion(INT32 index, INT32 newstart, INT32 newend)
{
	Set set;
	set.first = newstart;
	set.last = newend;
	set.Finclusive = TRUE;
	set.Linclusive = TRUE;
	RetroEorRegion[index] = set;
}


/********************************************************************************************

>	OpRetroSmooth::OpRetroSmooth() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Purpose:	OpRetroSmooth() constructor
	SeeAlso:	-

********************************************************************************************/

OpRetroSmooth::OpRetroSmooth()
{
	// Dummy constructor
}
  


/********************************************************************************************

>	BOOL OpRetroSmooth::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpRetroSmooth initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpRetroSmooth::Init()
{
	return (RegisterOpDescriptor(0,									// tool ID
 								_R(IDS_RETROSMOOTHOP),					// string resource ID
								CC_RUNTIME_CLASS(OpRetroSmooth),	// runtime class for Op
 								OPTOKEN_RETROSMOOTH,			// Ptr to token string
 								OpRetroSmooth::GetState,			// GetState function
 								0,									// help ID
 								_R(IDBBL_RETROSMOOTHOP),				// bubble help ID
								0									// resource ID =0
								));

}               


/********************************************************************************************

>	OpState	OpRetroSmooth::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpRetroSmooth
	Purpose:	For finding the OpRetroSmooth state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpRetroSmooth::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	// Always enabled at the moment.
	return OpSt;
}

/********************************************************************************************

>	void OpRetroSmooth::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.
					
********************************************************************************************/

void OpRetroSmooth::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_RETROSMOOTHOP));
}



/********************************************************************************************

>	void OpRetroSmooth::DoRetroSmooth(NodePath* pThisNode, double smoothacc)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Inputs:		pThisNode = pointer to a path node in the tree to smooth
				smoothacc = accuracy value to smooth to
	Outputs:	-
	Returns:	-
	Purpose:	This operation will smooth a node path given an accuracy. It will create a
				new node in the tree after the node specified and hide node passed if all 
				is succesfull.

********************************************************************************************/
	
void OpRetroSmooth::DoRetroSmooth(NodePath* pThisNode, Path* pPath, double smoothacc)
{
	PORTNOTETRACE("other","OpRetroSmooth::DoRetroSmooth - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL ok;
	// remember the selection before the operation
	if (!DoStartSelOp(FALSE,FALSE))
	{
		FailAndExecute(); End(); return;
	}

	// also check for blends needing to be re-initialised
	NodeBlender * pBlender = NULL;
	NodeBlend * pBlend = NULL;
	NodeBlend* pOrigBlend = NULL;
	pBlender = (NodeBlender *)pThisNode->FindNext(CC_RUNTIME_CLASS(NodeBlender));
	if (!pBlender) pBlender = (NodeBlender *)pThisNode->FindPrevious(CC_RUNTIME_CLASS(NodeBlender));
	
	if (pBlender)
	{	
		pBlend = (NodeBlend *)pBlender->FindParent();
		//pBlend->forOpRetroSmooth = FALSE;
		pOrigBlend = pBlend;
	}

	// Invalidate the region
	CALL_WITH_FAIL( DoInvalidateNodeRegion(pThisNode, TRUE, FALSE), this, ok);
	if (!ok)
	{
		FailAndExecute(); End(); return;
	}

	ObjChangeFlags cFlags;
	cFlags.ReplaceNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,pThisNode,this);
	if (pThisNode->AllowOp(&ObjChange))
	{
		// create the new node path
		CALL_WITH_FAIL( DoMakeNodeFromPath(pThisNode, pPath, NEXT, TRUE), this, ok);
		if (!ok)
		{
			FailAndExecute(); End(); return;
		}

		// see if were part of a blend ....

		pBlender = (NodeBlender *)pThisNode->FindNext(CC_RUNTIME_CLASS(NodeBlender));

		if (pBlender)
		{
			pBlend = (NodeBlend *)pBlender->FindParent();

			if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), pBlender, TRUE, TRUE) != AC_OK)
			{
				ERROR2RAW("Couldn't Initialise blend action");
			}

			// re-insert this action at the head of the list
			
			pBlender->Reinit((NodeRenderableInk*) pThisNode->FindNext (), pBlender->GetNodeEnd(), FALSE);

			// Invalidate the region
			CALL_WITH_FAIL( DoInvalidateNodeRegion(pBlender, TRUE, FALSE), this, ok);
			if (!ok)
			{
				FailAndExecute(); End(); return;
			}
		}
		else
		{
			pBlender = (NodeBlender *)pThisNode->FindPrevious(CC_RUNTIME_CLASS(NodeBlender));

			if (pBlender)
			{
				pBlend = (NodeBlend *)pBlender->FindParent();

				if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), pBlender, TRUE, TRUE) != AC_OK)
				{
					ERROR2RAW("Couldn't Initialise blend action");
				}

				// re-insert this action at the head of the list
				
				pBlender->Reinit(pBlender->GetNodeStart(), (NodeRenderableInk*) pThisNode->FindNext (), FALSE);

				// Invalidate the region
				CALL_WITH_FAIL( DoInvalidateNodeRegion(pBlender, TRUE, FALSE), this, ok);
				if (!ok)
				{
					FailAndExecute(); End(); return;
				}
			}
		}

		if (pBlender)
		{
			NodeBlend* ptrBlend = (NodeBlend*) pBlender->FindParent ();

			ERROR3IF (!IS_A (ptrBlend, NodeBlend), "NodeBlend is not a NodeBlend!");

			BOOL done = FALSE;
			NodeBlender* ptrNode = ptrBlend->FindFirstBlender ();

			while (!done)
			{
				if (ptrNode != pBlender)
				{
					if (ptrNode->GetNodeStart () == pThisNode)
					{
						if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), ptrNode, TRUE, TRUE) != AC_OK)
						{
							ERROR2RAW("Couldn't Initialise blend action");
						}
						
						ptrNode->Reinit((NodeRenderableInk*) pThisNode->FindNext (), NULL, FALSE);
					}
					if (ptrNode->GetNodeEnd () == pThisNode)
					{
						if (InitBlendAction::InitOnBlender(this, GetUndoActionList(),  ptrNode, TRUE, TRUE) != AC_OK)
						{
							ERROR2RAW("Couldn't Initialise blend action");
						}

						ptrNode->Reinit(NULL, (NodeRenderableInk*) pThisNode->FindNext (), FALSE);
					}

					// Invalidate the region
					CALL_WITH_FAIL( DoInvalidateNodeRegion(ptrNode, TRUE, FALSE), this, ok);
					if (!ok)
					{
						FailAndExecute(); End(); return;
					}
				}

				ptrNode = ptrBlend->FindNextBlender (ptrNode);

				if (!ptrNode)
				{
					done = TRUE;
				}
			}
		}

		ObjChange.Define(OBJCHANGE_FINISHED,cFlags,pThisNode,this);
		if (!UpdateChangedNodes(&ObjChange))
		{
			FailAndExecute(); End(); return;
		}

		// Now we've formed a smoothed path, let's hide the original
		CALL_WITH_FAIL( DoHideNode(pThisNode,TRUE), this, ok);
		if (!ok)
		{
			FailAndExecute(); End(); return;
		}
	}
	//pOrigBlend->forOpRetroSmooth = FALSE;
	// DoSmoothNodePath(pThisNode,smoothacc),this,ok);
	End();
#endif
}   



/********************************************************************************************

>	DynArray::DynArray()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Purpose:	DynArray constructor

********************************************************************************************/

DynArray::DynArray()
{
	UsedSlots = 0;
	UnusedSlots = 0;
	SlotInitSize = 0;
	SlotAllocSize = 0;

    BlockHandle = BAD_MHANDLE;
}


/********************************************************************************************

>	DynArray::~DynArray()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Purpose:	Destructor - releases the memory that the template class has 
				been using to store items in.

********************************************************************************************/

DynArray::~DynArray()
{
	if (BlockHandle != BAD_MHANDLE)
	ReleaseBlock(BlockHandle);
}


/********************************************************************************************

>  	BOOL DynArray::Initialise(INT32 inititems, INT32 newitems)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		inititems = The number of items of  the list can hold initially
				newitems  = The number of items to allocate for each time we run 
							out of space
	Returns:	TRUE if we got all the memory we needed, FALSE if not
	Purpose:	Allocates memory for a list to hold a particular type of object
	Errors:		Can fail if it runs out of memory. The function will return
				FALSE

********************************************************************************************/

BOOL DynArray::Initialise(INT32 inititems, INT32 newitems)
{
	ERROR3IF((newitems & 3),"DynArray::Initialise called with none quad byte 'new' multiple"); 

	if (inititems<12)
		inititems=12;

	if (newitems<12)
		newitems=12;

	if (BlockHandle == BAD_MHANDLE)
	    BlockHandle = ClaimBlock(sizeof(ITEM_TYPE)*inititems);
	else
	{
		// some ones initialising an already existant buffer
		// so lets just resize it to fit.
		INT32 SizeDiff = (UsedSlots+UnusedSlots)	- inititems;
		BOOL ok;
    	if (SizeDiff>0)
    		ok = IncreaseBlock(BlockHandle,sizeof(ITEM_TYPE)*SizeDiff);
		else
		{
			SizeDiff = -SizeDiff;
			ok = DecreaseBlock(BlockHandle,sizeof(ITEM_TYPE)*SizeDiff);
		}
		if (!ok)
			return FALSE;
	}
	
    if (BlockHandle==BAD_MHANDLE)
    	return FALSE;
		
   	UsedSlots = 0;
   	UnusedSlots = inititems;
   	SlotInitSize = inititems;
   	SlotAllocSize = newitems;

    return TRUE;
}



/********************************************************************************************

>  	TYPE_ITEM* DynArray::GetFirstItem()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		-
	Returns:	a pointer to the start of this dynamic array.
	Purpose:	Get a pointer to the start of this array. The ptr can then be used in
				conjunction with the [] operator to access the stored values with array
				operations.

********************************************************************************************/

ITEM_TYPE* DynArray::GetFirstItem()
{
	return ((ITEM_TYPE*)DescribeHandle(BlockHandle));
}



/********************************************************************************************

>  	BOOL DynArray::MakeSpaceInList(const INT32 NumSlots)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		NumSlots = the number of slots to ensure free
	Returns:	TRUE if NumSlots are now freely available
				FALSE if unable to ensure NumSlots of free space.
	Purpose:	Ensure that there are NumSlots free within this dynamic array.

********************************************************************************************/

BOOL DynArray::MakeSpaceInList(const INT32 NumSlots)
{
	if (UnusedSlots<NumSlots)
	{
    	INT32 Needed = NumSlots-UnusedSlots+SlotAllocSize;
    	if (!IncreaseBlock(BlockHandle,sizeof(ITEM_TYPE)*Needed))
    		return FALSE;
        UnusedSlots += Needed;
	}
    return TRUE;
}


/********************************************************************************************

>  	BOOL DynArray::AddItem(ITEM_TYPE& item)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		a reference to an item to store in the array
	Returns:	TRUE if the item was successfully stored
				FALSE if the item could not be stored due to lack of memory
	Purpose:	Store the array item by increasing the size of the array by one item
				and placing the item at this position.

********************************************************************************************/

BOOL DynArray::AddItem(ITEM_TYPE& item)
{
	if (!MakeSpaceInList(1))
       	return FALSE;

   	ITEM_TYPE* List = (ITEM_TYPE*) DescribeHandle(BlockHandle);

   	List[UsedSlots] = item;

	UsedSlots++;
	UnusedSlots--;

	return TRUE;
}



/********************************************************************************************

>  	void DynArray::Empty(BOOL compress = TRUE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		-
	Returns:	-
	Purpose:	Remove all stored items from this array. All array accesses via the 
				[] operator will now be faulted. Use AddItem() to begin reconstructing
				values in the array.

********************************************************************************************/

void DynArray::Empty(BOOL compress)
{
	if (UsedSlots>0)
	{
		// reset the path definitions
		UnusedSlots+=UsedSlots;
		UsedSlots=0;

		// make sure we compress memory if necessary
		if (compress)
			Compact();						// ignore any error coming from here
	}	
}



/********************************************************************************************

>  	void DynArray:::DeleteItems(INT32 firstitem, INT32 numitems)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		first item	= an index of the first item to start the deletion at (0..n-1)
				num items	= the number of items to delete	(0<=n<=NumItems)
	Returns:	-
	Purpose:	Delete a set of items from this dynamic array.

				Example:

					  0 1 2 3 4 5 6 7 8 9		  		   				   0 1 2 3 4 5
				L = [ 9,8,7,6,5,4,3,2,1,0 ]     DeleteItems(3, 4)	 L = [ 9,8,7,2,1,0 ]

********************************************************************************************/

void DynArray::DeleteItems(INT32 firstitem, INT32 numitems)
{
	ENSURE(firstitem<UsedSlots, "Specified position is off end of array in DynArray::DeleteItems");
	ENSURE(firstitem>=0, "Specified position is off begining of array in DynArray::DeleteItems");

	if (numitems<=0)
		return;

	ITEM_TYPE* List = (ITEM_TYPE*) DescribeHandle(BlockHandle);

	INT32 Moving = UsedSlots-(firstitem+numitems);

	if (Moving>0)
		memmove((void*)(&List[firstitem]), (void*)(&List[firstitem+numitems]), Moving*sizeof(ITEM_TYPE));

	UsedSlots   -= numitems;
	UnusedSlots += numitems;

	Compact();
}


/********************************************************************************************

>  	void DynArray::Compact()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		-
	Returns:	-
	Purpose:	Checks for excessive amounts of unused space left at the end of a dynamic 
				array, (possibly left from a Empty(FALSE) call)  and removes it. The quantity
				of space removed is always an exact multiple of newitems (See Initialise())

********************************************************************************************/

void DynArray::Compact()
{
    INT32 unused = UnusedSlots / SlotAllocSize;
	unused-=1;
    if (unused>0)
	{
    	if (DecreaseBlock(BlockHandle, sizeof(ITEM_TYPE)*(unused*SlotAllocSize)))
			UnusedSlots-=unused*SlotAllocSize;
	}
}


/********************************************************************************************

>  	ITEM_TYPE& DynArray::operator[](const INT32 i)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		i = the index of an item in this dynamic array 
	Returns:	A reference to the indexed item in this dynamic array
	Purpose:	Overload the array operator to allow array access functionality for this
				dynamic set of items. This functions provides both item reteival and item
				assignment for example
				MyArray[i] = item,	 	item = MyArray[i]
	Errors:		Access or assignment will be faulted if the index i is outside the range
				of items stored in the array.

********************************************************************************************/

ITEM_TYPE& DynArray::operator[](const INT32 i)
{
	ENSURE(i>=0 && i<UsedSlots, "index out of range in Template list operator[]");
	ITEM_TYPE* List = (ITEM_TYPE*) DescribeHandle(BlockHandle);
	return (List[i]);
}


/********************************************************************************************

>	BOOL DynArray::CloneFrom(ITEM_TYPE* SourceArray)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		SourceArray = pointer to a source dynamic array	
	Returns		TRUE 	if successful
				FALSE 	if unable in increase the size of this dynamic array to cope with all
						data in the supplied array.
	Purpose:	Replace the data within this dynamic array with that of the other array
			    supplied

********************************************************************************************/

BOOL DynArray::CloneFrom(DynArray* SourceArray)
{
	// Make space to copy all the data from the other path into this one
	INT32 SlotsToCopy = SourceArray->UsedSlots;
	INT32 SlotsToAdd = SlotsToCopy-UsedSlots;

	if (SlotsToAdd>0)
	{
		if (!MakeSpaceInList(SlotsToAdd))
			return FALSE;
	}

	// Get the arrays
	ITEM_TYPE* pSource = (ITEM_TYPE*) DescribeHandle(SourceArray->BlockHandle);
	ITEM_TYPE* pDestin = (ITEM_TYPE*) DescribeHandle(BlockHandle);

	memmove((void*)(pDestin), (void*)(pSource), SlotsToCopy*sizeof(ITEM_TYPE));

	// Now update the array variables
	UsedSlots += SlotsToAdd;
	UnusedSlots -= SlotsToAdd;

	return TRUE;
}
