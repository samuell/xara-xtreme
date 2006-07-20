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
// The functions that control paths

/*
*/

#include "camtypes.h"
#if !defined(EXCLUDE_FROM_XARLIB)
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#endif
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include <math.h>

//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "pathutil.h"
//#include "handles.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "pathproc.h"
#include "vector.h"

#if !defined(EXCLUDE_FROM_XARLIB)
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "osrndrgn.h"
#include "pen.h"
//	#include "pathutil.h"
#include "blobs.h"
//	#include "fixmem.h"
#include "fitcurve.h"
#include "layermgr.h"
#include "gclip.h"
#include "grndrgn.h"
#include "XaDraw.h"
#include "cstroke.h"
#include "lineattr.h"
#include "attrmap.h"
//	#include "handles.h"
#endif

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// Put my version number into the about box
DECLARE_SOURCE( "$Revision$" );

/********************************************************************************************

>	PathFlags::PathFlags()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/03/94
	Purpose:	Path Flags Constructor. This sets all the path flags to FALSE ready for
				use.

********************************************************************************************/

PathFlags::PathFlags()
{
	IsSelected = FALSE;

	// Flags to aid with the smoothing of curves
	IsSmooth = FALSE;
	IsRotate = FALSE;

	// Flags that help with the EORed rendering as the path is edited
	NeedToRender = FALSE;

	// All Endpoints in the path have this flag set
	IsEndPoint = FALSE;

	// May as well use up the spare bits in this byte for the future
	Spare1 = FALSE;
	Spare2 = FALSE;
	Spare3 = FALSE;
}


/********************************************************************************************

>	Path::Path()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/94
	Purpose:	constructor - just sets a few things to sensible values

********************************************************************************************/

Path::Path()
{
	// We do not have any memory yet
	UnUsedSlots = 0;
	UsedSlots = 0;
	SlotInitSize = 0;
	SlotAllocSize = 0;

	// set all the handles to bad in case they are used
	VerbHandle = BAD_MHANDLE;
	CoordHandle = BAD_MHANDLE;
	FlagsHandle = BAD_MHANDLE;

	// Set the path flags
	IsFilled = FALSE;
	IsStroked = TRUE;

	// Set the current path position
	CurrentPos = 0;

	// Setup the Extra Info Channels
	ExtraInfo = NULL;

#if !defined(EXCLUDE_FROM_XARLIB)
	// Set up the contouring variables
//	m_pGPC = NULL;
//	m_pGSC = NULL;
	m_ContourLength = 0;
	m_IsAnOuterContour = true;
	m_DoClosePath = false;

	// MRH New!
	m_ContourWidth = 100;
	m_ContourJoinS = JOIN_MITER;
	m_ContourCapS = CAPS_BUTT;
	m_ContourFlatness = 200.0;

	// Set the mitre limit to 10 shifted 16. It`s done in two parts as it can cause overflow
	// problems if done as one call.
	m_ContourMitreLimit = 10;
	m_ContourMitreLimit <<= 16;
	m_UseContourMode = TRUE;
#endif
}

/********************************************************************************************

>	Path::~Path()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/94
	Purpose:	Destructor - releases the memory that the path has been using to store coords
				in.

********************************************************************************************/

Path::~Path()
{
	// Free the memory that the path is using to store its data in
	if (VerbHandle != BAD_MHANDLE)
		ReleaseBlock(VerbHandle);
	
	if (CoordHandle != BAD_MHANDLE)
		ReleaseBlock(CoordHandle);

	if (FlagsHandle != BAD_MHANDLE)
		ReleaseBlock(FlagsHandle);

	if (ExtraInfo != NULL)
		delete ExtraInfo;
/*	
	if(m_pGPC != NULL)
	{
		delete m_pGPC;
		m_pGPC = NULL;
	}

	if(m_pGSC != NULL)
	{
		delete m_pGSC;
		m_pGSC = NULL;
	}
*/
}



/********************************************************************************************

>	BOOL Path::Initialise(INT32 InitialSize, INT32 BlockSize)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Inputs:		InitialSize - The number of coords the path can hold initially
				BlockSize - The number of coords to allocate for each time we run out of space
	Returns:	TRUE if we got all the memory we needed, FALSE if not
	Purpose:	Allocates memory in the path for the three arrays - The Verbs, Coordinates
				and the Flags.
	Errors:		Can fail if it runs out of memory

********************************************************************************************/

BOOL Path::Initialise(INT32 InitialSize, INT32 BlockSize)
{
	// CGS (19/6/2000)  A few of camelots programmers seem to be assuming that calling initialise
	// on an already initialised path will clear out all previous pointers.  This is NOT the case
	// (or at least wasn't).  It is not unreasonable for the programmer to expect this to happen
	// (being viewed as a reinitialise), so I have added code to handle deletion automatically.
	// This should fix a considerable amount of camelots memory leaks!

	if (VerbHandle != BAD_MHANDLE)
		ReleaseBlock(VerbHandle);
	
	if (CoordHandle != BAD_MHANDLE)
		ReleaseBlock(CoordHandle);

	if (FlagsHandle != BAD_MHANDLE)
		ReleaseBlock(FlagsHandle);
	
	// The memory manager will not allocate blocks of less that 12 bytes and have to be multiples of 4!
	if (InitialSize<12)
	{
		InitialSize = 12;
	}
	else
	{
		// Make sure it is a multiple of 4, by shifting left 2 and then shifting it back again!
		InitialSize = (InitialSize+3) >> 2;
		InitialSize = InitialSize << 2;
	}

	if (BlockSize<12)
	{
		BlockSize = 12;
	}
	else
	{
		// Make sure it is a multiple of 4, by shifting left 2 and then shifting it back again!
		BlockSize = (BlockSize+3) >> 2;
		BlockSize = BlockSize << 2;
	}

	// try and get memory for the verbs
	VerbHandle = ClaimBlock(sizeof(PathVerb)*InitialSize);
	if (VerbHandle!=BAD_MHANDLE)
	{
		// try and get memory for the coords
		CoordHandle = ClaimBlock(sizeof(DocCoord)*InitialSize);
		if (CoordHandle != BAD_MHANDLE)
		{
			// try and get memory for the flags
			FlagsHandle = ClaimBlock(sizeof(PathFlags)*InitialSize);
			if (FlagsHandle != BAD_MHANDLE)
			{
				// everything worked
				UsedSlots = 0;
				UnUsedSlots = InitialSize;
				SlotInitSize = InitialSize;
				SlotAllocSize = BlockSize;
			}
			else
			{
				// failed to get the flag block, so release the other blocks and fail
				ReleaseBlock( CoordHandle );
				ReleaseBlock( VerbHandle );
				return FALSE;
			}
		}
		else
		{
			// failed to get the coords block, so release the block we already have and fail
			ReleaseBlock( VerbHandle );
			return FALSE;
		}
	}
	else
	{
		// failed to get any memory, so fail
		return FALSE;
	}

	// a total success, so say so.
	return TRUE;
}


/********************************************************************************************

>	BOOL Path::CopyPathDataFrom( Path* SrcPath )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/94
	Inputs:		SrcPath - The path that you want to copy data from
	Returns:	TRUE if it worked, FALSE otherwise. Can fail if there is not enough space
				in the path to copy SrcPaths data into.				
	Purpose:	Copy the coords, verbs and flags from the SrcPath to this path. You must 
				already have initialised this path by calling Path::Initialise(). Pass it
				an Initial size big enough the hold the path you want to copy.

********************************************************************************************/

BOOL Path::CopyPathDataFrom( Path* SrcPath )
{
	// Ensure that a non-null path is being used.
	if ( SrcPath == NULL )
		return FALSE;

	// Find out how much space we need to alloc in this new path
	INT32 NumCoords = SrcPath->GetNumCoords();

	if (UsedSlots+UnUsedSlots < NumCoords)
	{
		// There is not enough space in this path to copy the SrcPath to
		//if (IsUserName("Rik")) TRACE( _T("Not enough space in path to copy data to\n") );
		ERROR3 ("Not enough space in path to copy data to!");
		return FALSE;
	}

	// copy the data across
	memcpy((void*)DescribeHandle(VerbHandle), (void*)DescribeHandle(SrcPath->VerbHandle), 
		   (size_t)NumCoords*sizeof(PathVerb));

	memcpy((void*)DescribeHandle(CoordHandle), (void*)DescribeHandle(SrcPath->CoordHandle), 
		   (size_t)NumCoords*sizeof(DocCoord));

	memcpy((void*)DescribeHandle(FlagsHandle), (void*)DescribeHandle(SrcPath->FlagsHandle), 
		   (size_t)NumCoords*sizeof(PathFlags));

	// If the source path has extra info in it then we need to copy that too
	if (SrcPath->ExtraInfo != NULL)	
		ExtraInfo->CopyExtraInfo(SrcPath->ExtraInfo);

	// Set the vars that tell us how much mem has been used
	UnUsedSlots = (UsedSlots+UnUsedSlots) - NumCoords;
	UsedSlots = NumCoords;

	// copy any other flags of value
	IsFilled = SrcPath->IsFilled;
	IsStroked = SrcPath->IsStroked;

	// Keep track of the current path position
	CurrentPos = 0;

	// Well, as far as I can tell its worked
	return TRUE;
}


/********************************************************************************************

	BOOL Path::CopyPathDataFrom( DocCoord* Coords, PathVerb* Verbs, INT32 NumCoords, 
							 	 BOOL IsFilled = FALSE, BOOL IsStroked = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		Coords - The path coords you want to copy
				Verbs - The verbs you want to copy
				NumCoords - The number of coords to copy
				IsFilled - Is the path filled ?
				IsStroked - Is the path stroked ?

	Returns:	TRUE if it worked, FALSE otherwise. Can fail if there is not enough space
				in the path to copy the data into.			
	Purpose:	Copy the specified coords and verbs to this path. The path's flags will 
				be the defaults as specified in the constructors. You must 
				already have initialised this path by calling Path::Initialise(). Pass it
				an Initial size big enough to hold the path you want to copy.

********************************************************************************************/

BOOL Path::CopyPathDataFrom( DocCoord* Coords, PathVerb* Verbs, INT32 NumCoords, 
							 BOOL Filled, BOOL Stroked)
{

	// Check that this path has enough slots
	if (UsedSlots+UnUsedSlots < NumCoords)
	{
		// There is not enough space in this path to copy the SrcPath to
		TRACEUSER( "Simon", _T("Not enough space in path to copy data to\n") );
		return FALSE;
	}

	// copy the data across
	memcpy((void*)DescribeHandle(VerbHandle), (void*)Verbs, 
		   (size_t)NumCoords*sizeof(PathVerb));

	memcpy((void*)DescribeHandle(CoordHandle), (void*)Coords, 
		   (size_t)NumCoords*sizeof(DocCoord));

	// Every point on the path has the default constructor flags set
	PathFlags f;
	PathFlags* pFlags = (PathFlags*) DescribeHandle(FlagsHandle); 

	for (INT32 i = 0; i< NumCoords; i++)
	{
		pFlags[i] = f; 		
	}

	// Set the vars that tell us how much mem has been used
	UnUsedSlots = (UsedSlots+UnUsedSlots) - NumCoords;
	UsedSlots = NumCoords;

	// Keep track of the current path position
	CurrentPos = 0;	

	IsFilled = Filled; 
	IsStroked = Stroked;

	// Well, as far as I can tell its worked
	return TRUE;

} 

/********************************************************************************************

>	BOOL Path::CreatePathFromDocRect(DocRect* pRect)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/99
	Outputs:	BOOL to say whether or not it was successful
	Purpose:	Creates a filled path from a given DocRect

********************************************************************************************/

BOOL Path::CreatePathFromDocRect(DocRect* pRect)
{
	DocCoord TopLeft(pRect->lo.x,pRect->hi.y);
	DocCoord TopRight(pRect->hi.x,pRect->hi.y);
	DocCoord BottomLeft(pRect->lo.x,pRect->lo.y);
	DocCoord BottomRight(pRect->hi.x,pRect->lo.y);

	InsertMoveTo(TopLeft);
	InsertLineTo(TopRight);
	InsertLineTo(BottomRight);
	InsertLineTo(BottomLeft);
	InsertLineTo(TopLeft);

	IsFilled = TRUE;

	return TRUE;
}


/********************************************************************************************

>	void Path::FindStartOfPath( PathPosition* Pos )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Outputs:	Pos - Holds the position of the start of the path
	Purpose:	Sets Pos to the first element in the path

********************************************************************************************/

BOOL Path::FindStartOfPath()
{
	// set the position to the start of the path
	CurrentPos = 0;

	// see if this a valid path position
	if (UsedSlots==0)
		return FALSE;
	else
		return TRUE;
}


/********************************************************************************************

>	void Path::FindStartOfSubPath()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Inputs:		-
	Outputs:	-
	Purpose:	Searches backwards from the current position until it finds the
				start of the sub path that the current position is in.
	Errors:		ENSUREs if the start of the path does not seem to be a MoveTo

********************************************************************************************/

void Path::FindStartOfSubPath()
{
	ENSURE( CurrentPos<UsedSlots, "Path position was not valid in FindStartOfSubPath" );

	// I am already in a path, so start working backwards until I find the moveto
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	while ((CurrentPos>0) && (Verbs[CurrentPos]!=PT_MOVETO))
		CurrentPos--;

	// The index will either be pointing at a moveto or the begining of the path
	// Either way, it should be a moveto
	ENSURE( (Verbs[CurrentPos]==PT_MOVETO), "Did not find a start to this sub path" );
}

/********************************************************************************************

>	void Path::FindStartOfSubPath( INT32* Index )

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/94
	Inputs:		Index - the index of the current element (pointer!)
	Outputs:	Index - Holds the position of the start of the sub path
	Purpose:	Searches backwards from the current position from Index until it find the 
				start of the subpath (always a moveto).
	Errors:		ENSUREs if the start of the path does not seem to be a MoveTo

********************************************************************************************/

void Path::FindStartOfSubPath(INT32* Index) const
{
	ENSURE( ((*Index)<UsedSlots && (Index)>=0), "Path position was not valid in FindStartOfSubPath" );

	// I am already in a path, so start working backwards until I find the moveto
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	while (((*Index)>0) && (Verbs[*Index]!=PT_MOVETO))
		(*Index)--;

	// The index will either be pointing at a moveto or the begining of the path
	// Either way, it should be a moveto
	ENSURE( (Verbs[(*Index)]==PT_MOVETO), "Did not find a start to this sub path" );
}


/********************************************************************************************

>	void Path::FindEndOfSubPath()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Inputs:		Pos - The position in the path to start looking from
	Outputs:	Pos - the position of the end of the subpath
	Purpose:	Searches forwards until it finds the end of the current sub-path. ie pos will
				point at the last element in the sub path

********************************************************************************************/

void Path::FindEndOfSubPath()
{
	ENSURE( CurrentPos<UsedSlots, "Path position was not valid in FindEndOfSubPath" );

	// I am already in a path, so start working backwards until I find the moveto
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	INT32 PrevIndex;

	do
	{
		PrevIndex = CurrentPos;
		FindNext();
	} while ((CurrentPos<UsedSlots) && (Verbs[CurrentPos]!=PT_MOVETO));

	// So we have either fallen off the end of the world, or we are now on a MoveTo
	// either way we need the info from the previous position
	CurrentPos = PrevIndex;
}

/********************************************************************************************

>	void Path::FindEndOfSubPath(INT32* Index)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Inputs:		Index - The position in the path to start looking from
	Outputs:	Index - the position of the end of the subpath
	Purpose:	Searches forwards until it finds the end of the current sub-path. ie Index will
				point at the last element in the sub path

********************************************************************************************/

void Path::FindEndOfSubPath(INT32* Index) const
{
	ENSURE( (*Index)<UsedSlots, "Path position was not valid in FindEndOfSubPath" );

	// I am already in a path, so start working backwards until I find the moveto
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	INT32 PrevIndex;

	do
	{
		PrevIndex = (*Index);
		FindNext(Index);
	} while (((*Index)<UsedSlots) && (Verbs[(*Index)]!=PT_MOVETO));

	// So we have either fallen off the end of the world, or we are now on a MoveTo
	// either way we need the info from the previous position
	(*Index) = PrevIndex;
}

/********************************************************************************************

>	BOOL Path::GetSubPathEnds(DocCoord* start, DocCoord* end)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	start, end are the coords of the start and end (if function returned TRUE)
	Returns:	TRUE if subpath is open, FALSE otherwise
				CurrentPos in the path is set to the end of the subpath.
	Purpose:	This function returns whether the path is open or closed. This is independent
				of the IsFilled flag, because some programs allow open paths to be filled.
				Basically, if there's no gap in the subpath, it's closed, otherwise it's open.
				These are the same rules as ArtWorks followed.
				Initial implementation just scans the subpath and checks the last element 
				in the subpath.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Path::GetSubPathEnds(DocCoord* start, DocCoord* end)
{
	DocCoord temp1;

	FindStartOfSubPath();
	temp1 = GetCoord();
	FindEndOfSubPath();

// Find out if this path is really closed. If the PT_CLOSEFIGURE flag is set in the end
// verb, the path is closed, so return FALSE, otherwise return TRUE, with *start and *end 
// set up accordingly.

	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	if (Verbs[CurrentPos] & PT_CLOSEFIGURE)
	{
		return FALSE;
	}
	else
	{
		*end = GetEndPoint();
		*start = temp1;
		return TRUE;
	}
}



/********************************************************************************************

>	BOOL Path::FindNext()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Purpose:	Finds the next path element in this path. The position will become invalid
				if you try to search past the start of the path. This can be checked by
				using Pos->IsValid.
	SeeAlso:	Path::FindPrev

********************************************************************************************/

BOOL Path::FindNext()
{
	ENSURE( CurrentPos<=UsedSlots, "Path position was not valid in FindNext" );
	ENSURE( CurrentPos>=0, "Path Position less than zero!" );

	// de-reference the verb array
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// Move to the next path element
	if ((Verbs[CurrentPos] & (~PT_CLOSEFIGURE))==PT_BEZIERTO)
		CurrentPos += 3;
	else
		CurrentPos ++;

	// Check to see if we fell off the end of the path
	if (CurrentPos>=UsedSlots)
	{
		CurrentPos = UsedSlots;
		return FALSE;
	}

	// we did it, so return happy
	return TRUE;
}


/********************************************************************************************

>	BOOL Path::FindNextEndPoint(INT32* index) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/94
	Inputs:		Index	= pointer to a INT32 indicating where to start from
	Outpus:		Index	= points to an index which is the next element
	Returns:	TRUE	if the next end point has been found
				FALSE	if there are no more end points in this path
	Purpose:	Finds the next path endpoint in this path
	SeeAlso:	Path::FindPrevEndPoint

********************************************************************************************/

BOOL Path::FindNextEndPoint(INT32* index) const
{
	ENSURE( (*index)<=UsedSlots, "Path position was not valid in FindNextEndPoint" );
	ENSURE( (*index)>=0, "Path Position less than zero in FindNextEndPoint" );

	// de-reference the verb array
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	(*index)++;

	if ((*index) >= UsedSlots)
	{
		((*index) = UsedSlots-1);
		return FALSE;
	}

	if ((Verbs[(*index)] & (~PT_CLOSEFIGURE))==PT_BEZIERTO)
		(*index) += 2;

	return TRUE;
}



/********************************************************************************************

>	BOOL Path::FindPrevEndPoint(INT32* index)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/1/95
	Inputs:		Index	= pointer to a INT32 indicating where to start from
	Outpus:		Index	= points to an index which is the previous endpoint
	Returns:	TRUE	if the previous end point has been found
				FALSE	if there are no more end points in this path
	Purpose:	Finds the previous path endpoint in this path
	Errors:		ERROR2's if Index is beyond the end of the path, OR before the start
	SeeAlso:	Path::FindNextEndPoint

********************************************************************************************/

BOOL Path::FindPrevEndPoint(INT32* index) const
{
	ERROR2IF(*index > UsedSlots, FALSE, "Path position was not valid in FindPrevEndPoint" );
	ERROR2IF(*index < 0, FALSE, "Path Position less than zero in FindPrevEndPoint" );

	// de-reference the verb array
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	if ((Verbs[*index] & (~PT_CLOSEFIGURE))==PT_BEZIERTO)
		*index -= 3;
	else
		*index -= 1;

	if (*index < 0)
	{
		*index = 0;
		return FALSE;
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL Path::FindNext(INT32* Index) const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Inputs:		Index is a pointer to an index into the path to start from
	Outputs:	Index will point at the next element in the path
	Returns:	TRUE if valid next element found
				FALSE if there is no next element, Index is set to NumCoords
	Purpose:	Finds the next path element in this path. The position will become invalid
				if you try to search past the start of the path. This can be checked by
				using Pos->IsValid.
	SeeAlso:	Path::FindPrev

********************************************************************************************/

BOOL Path::FindNext(INT32* Index) const
{
	ENSURE( (*Index)<=UsedSlots, "Path position was not valid in FindNext" );
	ENSURE( (*Index)>=0, "Path Position less than zero!" );

	// de-reference the verb array
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// Move to the next path element
	if ((Verbs[(*Index)] & (~PT_CLOSEFIGURE))==PT_BEZIERTO)
		(*Index) += 3;
	else
		(*Index) ++;

	// Check to see if we fell off the end of the path
	if ((*Index)>=UsedSlots)
	{
		(*Index) = UsedSlots;
		return FALSE;
	}

	// we did it, so return happy
	return TRUE;
}


/********************************************************************************************

>	void Path::FindPrev()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if valid previous element found
				FALSE if there is no previous element
	Purpose:	Finds the prev path element in this path starting the search from CurrentPos 
	SeeAlso:	Path::FindNext

********************************************************************************************/

BOOL Path::FindPrev()
{
	ENSURE( CurrentPos<=UsedSlots, "Path position was not valid in FindPrev" );
	ENSURE( CurrentPos>=0, "Path Position less than zero!" );

	// de-reference the verb array
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// Move to the prev path element
	if (CurrentPos==UsedSlots)
	{
		// The current pos is at the end of the path, so move to the last element in the path
		CurrentPos --;
	}
	else
	{
		// see if we were now in the middle of a curve
		CurrentPos--;

		if ((Verbs[CurrentPos] & (~PT_CLOSEFIGURE))==PT_BEZIERTO)
			CurrentPos -= 2;
	}

	// Make sure we are still in bounds
	if (CurrentPos<0)
	{
		// we are not, so set the current pos back to the start and fail
		CurrentPos = 0;
		return FALSE;
	}

	// we are in the path, so succeed
	return TRUE;
}



/********************************************************************************************

>	void Path::FindPrev(INT32 *index) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/94
	Inputs:		Index is a pointer to an index into the path to start from
	Outputs:	Index will point at the previous element in the path
	Returns:	TRUE if valid previous element found
				FALSE if there is no previous element
	Purpose:	Finds the previous path element in this path starting from the value (index) 
	SeeAlso:	Path::FindNext

********************************************************************************************/

BOOL Path::FindPrev(INT32* index) const
{
	ENSURE( (*index)<=UsedSlots, "Path position was not valid in FindPrev" );
	ENSURE( (*index)>=0, "Path Position less than zero!" );

	// de-reference the verb array
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// Move to the prev path element
	if ((*index)==UsedSlots)
	{
		// The current pos is at the end of the path, so move to the last element in the path
		(*index) --;
	}
	else
	{
		// see if we were now in the middle of a curve
		(*index)--;

		if ((Verbs[(*index)] & (~PT_CLOSEFIGURE))==PT_BEZIERTO)
			(*index) -= 2;
	}

	// Make sure we are still in bounds
	if ((*index)<0)
	{
		// we are not, so set the current pos back to the start and fail
		(*index) = 0;
		return FALSE;
	}

	// we are in the path, so succeed
	return TRUE;
}






/********************************************************************************************

>	INT32 Path::GetPathPosition()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/02/94
	Returns:	The Coord number in the path that the Current Path Position is at
	Purpose:	Find out where in the path we are

********************************************************************************************/

INT32 Path::GetPathPosition()
{
	return CurrentPos;
}



/********************************************************************************************

>	void Path::SetPathPosition(INT32 NewPos)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/02/94
	Inputs:		NewPos - the Position you want to go to
	Purpose:	Sets the current path position to the value supplied. This represents the
				Coord number in the path

********************************************************************************************/

void Path::SetPathPosition(INT32 NewPos)
{
	// Set the pos to the position supplied
	CurrentPos = NewPos;

	// make sure it was in bounds
	if (CurrentPos>=UsedSlots)
		CurrentPos = UsedSlots-1;

	if (CurrentPos<0)
		CurrentPos = 0;
}




/********************************************************************************************

>	BOOL Path::IsInPath()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/02/94
	Returns:	TRUE if the current position is still in the path
				FALSE if it has gone past the last element in the path
	Purpose:	Determine if the path current position is still inside the path

********************************************************************************************/
BOOL Path::IsInPath()
{
	// We are still in the path if the current position has not gone past the last valid
	// item in the path
	return ((CurrentPos>=0) && (CurrentPos<UsedSlots));
}

/********************************************************************************************

>	INT32 Path::ComparePathToPath(Path* pComparePath, BOOL QuickCheck = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> howitt
	Created:	05/12/00
	Inputs:		A path to compare against.
				A flag to say wether to return as soon as it finds a difference (Default TRUE)
	Returns:	The difference in number of coords + or -.
	Purpose:	Compares athis path to a passed in path. It firswt does a quick check on the
				number of coords and if they are equal does a more thoughour check on the 
				verb array and coord array.

********************************************************************************************/
INT32 Path::ComparePathToPath(Path* pComparePath, BOOL QuickCheck)
{
	// Setup a default return value. This is basically the negative value of this paths
	// number of coords!
	INT32 ThisPathLength = (INT32)GetNumCoords();
	INT32 Difference = -ThisPathLength;

	// Only do any comparing if we`ve got valid pointers
	if(pComparePath)
	{
		// quick check first!
		if(ThisPathLength == pComparePath->GetNumCoords())
		{
			// Get the Coord array and Verb array for both paths.
			DocCoord* SPoint = GetCoordArray();
			DocCoord* CPoint = pComparePath->GetCoordArray();
			PBYTE SVerb = GetVerbArray();
			PBYTE CVerb = pComparePath->GetVerbArray();

			// Reset the Difference Value
			Difference = 0;
			
			// Now go throught each element of the arrays calculating the differences
			for( INT32 i = 0; i < ThisPathLength; i++)
			{
				// If either the points OR verbs are different then increment the Diff counter
				if(SPoint[i] != CPoint[i] || SVerb[i] != CVerb[i])
				{
					Difference++;
					if(QuickCheck)
						break;
				}
			}
		}
		else
		{
			// Ok! there`s different so calcualte the difference
			Difference = ThisPathLength - (INT32)pComparePath->GetNumCoords();
		}
	}

	// return the Differance
	return Difference;
}

/********************************************************************************************

>	PathVerb Path::GetVerb() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	The verb at the current path position
	Purpose:	finds the verb associated with the path position
	Errors:		Ensures if the position if not valid or out of bounds

********************************************************************************************/
PathVerb Path::GetVerb() const
{
	ENSURE( CurrentPos<UsedSlots, "Index out of bounds in GetVerb" );

	// de-reference the verb array
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// return the verb with the close path bit removed
	return ((Verbs[CurrentPos]) & (~PT_CLOSEFIGURE));
}


/********************************************************************************************

>	DocCoord Path::GetCoord() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	The DocCoord at the Current Path position
	Purpose:	finds the DocCoord associated with the path position
	Errors:		Ensures if the position if not valid or out of bounds

********************************************************************************************/

DocCoord Path::GetCoord() const
{
	ENSURE( CurrentPos<UsedSlots, "Index out of bounds in Path::GetCoord" );

	// de-reference the coord array
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);

	return Coords[CurrentPos];
}

/********************************************************************************************

>	PathFlag Path::GetFlags() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	The flag at the current path position
	Purpose:	finds the flags associated with the path position
	Errors:		Ensures if the position if not valid or out of bounds

********************************************************************************************/

PathFlags Path::GetFlags() const
{
	ENSURE( CurrentPos<UsedSlots, "Index out of bounds in GetFlags" );

	// de-reference the flag array
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	return Flags[CurrentPos];	
}




/********************************************************************************************

>	PathTypeEnum Path::GetPathType()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		-
	Outputs:	-
	Returns:	Path type
	Purpose:	This determines whether the path is a line or a shape.
				It does this by seeing if it has a PT_CLOSEFIGURE bit set in its last verb.
				
********************************************************************************************/

PathTypeEnum Path::GetPathType() const
{
	PathVerb* pVerbs = GetVerbArray();

	if (pVerbs[UsedSlots-1] & PT_CLOSEFIGURE)
		return (PATHTYPE_SHAPE);
	else
		return (PATHTYPE_LINE);
}


/********************************************************************************************

>	INT32 Path::FindOppositeControlPoint(INT32 ThisIndex)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/94
	Returns:	The index of the control point opposite the indexed one (or -1)
	Purpose:	Given a bezier control point, this function returns the index of
				the control point that is opposite this one (with an endpoint between).
				This routine takes closed paths into account.

********************************************************************************************/

INT32 Path::FindOppositeControlPoint(INT32 ThisIndex)
{
	// Get arrays of flags and verbs
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// If this isn't a control point, ensure!
	ENSURE(!Flags[ThisIndex].IsEndPoint,"Point is not a control point in FindOppositeControlPoint");

	// First see if this is the first or second control point in a curve element. If the 
	// previous point is an endpoint, then this is the first control point
	if (Flags[ThisIndex-1].IsEndPoint)
	{
		// if the previous endpoint is a moveto, we might have to wrap
		if (Verbs[ThisIndex-1] == PT_MOVETO)
		{
			INT32 i;
			for (i = ThisIndex;i<UsedSlots && Verbs[i]!=PT_MOVETO;i++);	// ; deliberate
			i--;
			// Now i gives us the endpoint of the subpath
			// If the path is closed and the last point is a curve, return the opposite point
			// otherwise return -1
			if ((Verbs[i] & PT_CLOSEFIGURE) && ((Verbs[i] & ~PT_CLOSEFIGURE)==PT_BEZIERTO))
				return i-1;
			else
				return -1;
		}
		else
		{
			if ((Verbs[ThisIndex-1] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
				return ThisIndex-2;
			else
				return -1;
		}
	}
	else
	{
		// This is the second control point. If the next point has the CloseFigure flag
		// set we'll have to scan back. Otherwise, return ThisIndex+2 unless the next element 
		// isn't a curve, or there is no next element.
		if (Verbs[ThisIndex+1] & PT_CLOSEFIGURE)
		{
			// The path is closed - scan back for the moveto
			INT32 i;
			for (i=ThisIndex;Verbs[i] != PT_MOVETO;i--);	// ; deliberate

			// Move to the next point - if it's a BezierTo we return it, otherwise return -1
			if (Verbs[i+1] == PT_BEZIERTO)
				return i+1;
			else
				return -1;
		}
		else
		{
			// The path isn't closed here, so see if there's another element
			if (ThisIndex+2 >= UsedSlots || Verbs[ThisIndex+2] != PT_BEZIERTO)
			{
				// No next element so return -1
				return -1;
			}
			else
			{
				// Next element is also a curve so return index of control point
				return ThisIndex+2;
			}
		}
	}
}

/********************************************************************************************

>	INT32 Path::GetPathByteLength()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	The Length of the path in bytes
	Purpose:	Calculates the length of the path in bytes. This figures includes the size of
				the 3 arrays (Coordinates, Verbs and Flags) only.

********************************************************************************************/

INT32 Path::GetPathByteLength() const
{
	INT32 Total;

	// Add up the size of each of the Arrays
	Total  = UsedSlots * sizeof(DocCoord);
	Total += UsedSlots * sizeof(PathVerb);
	Total += UsedSlots * sizeof(PathFlags);

	return Total;
}


/********************************************************************************************

>	INT32 Path::GetNumElements() const

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Returns:	The number of path elements in all sub paths.
	Purpose:	This function counts the number of elements in the path.
				This ignores MoveTos, hence giving you the total number of visible path elements
				(i.e. all the lines and curves).

********************************************************************************************/

INT32 Path::GetNumElements() const
{
	// The routine relies on the path beginning with a MoveTo
	PathVerb* pVerbs = (PathVerb*) DescribeHandle(VerbHandle);
	INT32 Index=0,Count=0;
	while (FindNextEndPoint(&Index))
	{
		if ((pVerbs[Index] & ~PT_CLOSEFIGURE) != PT_MOVETO)
			Count++;
	}

	return Count;
}

/********************************************************************************************

>	INT32 Path::GetNumSubpaths() const

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Returns:	The number of subpaths in this path
	Purpose:	Gets the number of subpaths in this path

********************************************************************************************/

INT32 Path::GetNumSubpaths() const
{
	//This variable will tell us our current position in the path
	INT32 lCurPos=0;

	//And this will keep a count of the subpaths we find
	INT32 lSubpaths=0;

	//Now, while we are still in the path
	while (lCurPos<GetNumCoords())
	{
		//Set lCurPos to the end of the subpath we are in
		FindEndElOfSubPath(&lCurPos);

		//Add one to the number of subpaths found
		lSubpaths++;

		//And move the current position on by one place, to move it
		//either into the next subpath or off the end of the path
		lCurPos++;
	}

	//And return the number of subpaths we found
	return lSubpaths;
	
}



/********************************************************************************************

>	INT32 Path::GetNumCoords()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	The number of coordinates in the path
	Purpose:	This returns the number of coordinates that make the path up

********************************************************************************************/

INT32 Path::GetNumCoords() const
{
	return UsedSlots;
}



/********************************************************************************************

>	INT32 Path::GetNumSelEndPoints() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Returns:	Number of selected endpoints within this path
	Purpose:	determine how many selected endpoints there are within this path. Useful
				indirectly for finding whether there's a subselection.
	SeeAlso:	IsSubSelected()
				
********************************************************************************************/

INT32 Path::GetNumSelEndPoints() const
{
	// We need to find all the selected control points in the path
	PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);
	INT32 count = 0;

	// look for selected coords
	for (INT32 i=0; i<UsedSlots; i++)
	{
		if ((Flags[i].IsSelected) && (Flags[i].IsEndPoint))
			count++;
	}
	return count;
}



/********************************************************************************************

>	INT32 Path::GetNumEndPoints()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/94
	Returns:	The number of open ends to the path.
	Purpose:	Goes through the path and counts all open ends in the path. For example,
				a straight line has 2 open ends, a closed shape has no open ends and a 
				complex path could have any number of open ends. You should use this function
				before calling GetAllOpenEnds that will fill an array for their coords
				for you.
	SeeAlso:	Path::GetAllOpenEnds

********************************************************************************************/

INT32 Path::GetNumEndPoints() const
{
	// de-reference the verb array
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	INT32 Count = 0;
	for (INT32 i=0; i<UsedSlots; i++)
	{
		if (Flags[i].IsEndPoint)
			Count++;

		if ((i>0) && (Verbs[i]==PT_MOVETO) && (Verbs[i-1] & PT_CLOSEFIGURE))
			Count--;
	}

	// return the number of 'on the path' end points
	return Count;
}

/********************************************************************************************

>	void Path::GetAllOpenEnds()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/94
	Inputs:		MaxElements - The maximum number of coords that can be written to the
				array
	Outputs:	Coords - A array of DocCoords
	Returns:	The number of coords in the array
	Purpose:	Goes through the path and copies the coordinates of all the Open ends
				of the path into the array, until MaxElements elements have been added
				to the array. Call GetNumEndPoints to find out how big to make your
				array in the first place.
	SeeAlso:	Path::GetNumEndPoints

********************************************************************************************/

INT32 Path::GetAllOpenEnds(INT32 MaxElements, DocCoord* EndCoords) const
{
	// de-reference the verb array
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);

	INT32 Count = 0;
	for (INT32 i=0; i<UsedSlots; i++)
	{
		if (Verbs[i]==PT_MOVETO)
		{
			// Count this moveto
			EndCoords[Count] = Coords[i];
			Count++;

			// Check to see we have hit the maximum allowed entries
			if (Count==MaxElements)
				return Count;
			
			if ((i>0) && (Verbs[i-1]&PT_CLOSEFIGURE))
			{
			 	// The previous element was a close figure, so it corresponding MoveTo
				// should not count
				Count--;
			}
			else
			{
				// The previous elment was not a close figure, so it was an Open End
				EndCoords[Count] = Coords[i-1];
				Count++;

				// Check to see we have hit the maximum allowed entries
				if (Count==MaxElements)
					return Count;

			}
		}
	}

	// Check to see if the very last element in the path was a close figure or not
	if (Verbs[UsedSlots-1] & PT_CLOSEFIGURE)
		Count--;
	else
	{
		EndCoords[Count] = Coords[UsedSlots-1];
		Count++;
	}

	// return the number of coords we are stored
	return Count;
}




/********************************************************************************************

>	DocRect Path::GetBoundingRect()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	The bounding box of the path
	Purpose:	Calculates the bounding rectangle of the paths coordinates. This is calculated
				on the fly, so don't over use it

********************************************************************************************/

DocRect Path::GetBoundingRect()	const
{
	// If the path is empty, then return an empty rectangle
	if (UsedSlots==0)
		return DocRect(0,0,0,0);

	// de-reference the coord array
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	DocRect BoundingRect(Coords[0], Coords[0]);

	// include each point in the path to the bounding rectangle
	for (INT32 i=1; i<UsedSlots; i++)
		BoundingRect.IncludePoint(Coords[i]);
			
	// return the completed rectangle
	return BoundingRect;
}


/********************************************************************************************

>	BOOL Path::GetTrueBoundingRect(	DocRect* pRect,	MILLIPOINT LineWidth = 0,
													CCAttrMap* pAttrMap = NULL )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19 May 2000
	Inputs:		pRect		pointer to a container rect for us to put the bounds in.
				
				*** Added by Diccon 22/5/2000  
				LineWidth - the width of the path, note that this defaults to zero
				so you don't have to supply anything unless you want to.

				pAttrMap	ptr to an attrmap, from which to take modification info.
							note that if we can extract a LineWidth value from it, then
							the attrmap's value will override that passed in directly.

	Outputs:	if successful, pRect will contain the bounds of the path,
				otherwise pRect will be made empty.

	Returns:	TRUE if successful,
				FALSE otherwise.

	Purpose:	Calculate the bounding rectangle of a path using GDraw.

	Notes:		This gives the true bounds,  including all the curvy bits - not just
				the bounds of the control points, which is what GetBoundingRect() returns.

				By using one or other of the extra parameters to this method, you can find
				out the bounds for thick lines, or lines with arrow-heads/tails.

	Errors:		ERROR3's with FALSE if pRect is NULL.
	SeeAlso:	GDrawContext::CalcStrokeBBox.

********************************************************************************************/
#if !defined(EXCLUDE_FROM_XARLIB)
BOOL Path::GetTrueBoundingRect(DocRect* pRect, MILLIPOINT LineWidth, CCAttrMap* pAttrMap)
{
	// validate inputs.
	if (pRect == NULL)
	{
		ERROR3("Path::GetTrueBoundingRect; NULL input rectangle!");
		return FALSE;
	}
	if (LineWidth < 0)
	{
		//ERROR3("Negative line width supplied to Path::GetTrueBoundingRect");
		LineWidth = 0;  // we can recover from this
	}

	// check the attr map for any attrs which we think may alter
	// the bounds of the path, eg line width, arrow heads.
	ArrowRec*	pArrow1 = NULL;
	ArrowRec*	pArrow2 = NULL;
	JoinStyles	JoinStyle = JOIN_ROUND;
	if (pAttrMap != NULL)
	{
		AttrLineWidth*		pLineWidth	= NULL;
		AttrStrokeColour*	pLineColour	= NULL;
		AttrStartArrow*		pStartArrow	= NULL;
		AttrEndArrow*		pEndArrow	= NULL;
		AttrJoinType*		pJoinType	= NULL;

		// note that we only use the path's LineWidth if its outline is not transparent.
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrLineWidth), (void*&)pLineWidth);
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStrokeColour), (void*&)pLineColour);

		if (pLineColour != NULL && pLineWidth != NULL)
		{
			LineWidth = 0;
			DocColour* pCol = pLineColour->GetStartColour();
			if (pCol != NULL && !pCol->IsTransparent())
				LineWidth = pLineWidth->Value.LineWidth;
		}

		// ok, if we have a line width then we'll need to look for stuff like
		// arrowheads and join-style (esp. Mitre joins).
		if (LineWidth > 0)
		{
			pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStartArrow), (void*&)pStartArrow);
			if (pStartArrow != NULL)
				pArrow1 = &(pStartArrow->Value.StartArrow);

			pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrEndArrow), (void*&)pEndArrow);
			if (pEndArrow != NULL)
				pArrow2 = &(pEndArrow->Value.EndArrow);

			pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrJoinType), (void*&)pJoinType);
			if (pJoinType != NULL)
			{
				JointType jt = pJoinType->Value.JoinType;

				JoinStyle = (jt == MitreJoin) ? JOIN_MITER :
							(jt == RoundJoin) ? JOIN_ROUND :
												JOIN_BEVEL;
			}
		}
	}

	// ok, let's get the path's bounds!
	pRect->MakeEmpty();
	BOOL fSuccess = FALSE;
	DocRect drBounds;
	GDrawContext *GD = GRenderRegion::GetStaticDrawContext();

	if (GD == NULL)
		return FALSE;

	fSuccess = GD->CalcStrokeBBox(	(POINT*)GetCoordArray(), GetVerbArray(), GetNumCoords(),
									(RECT *)(&drBounds), IsFilled, (DWORD)LineWidth,
									CAPS_ROUND, JoinStyle, NULL ) != -1;

	// if the bounds are invalid then make sure that we flag a failure.
	if (!drBounds.IsValid())
		fSuccess = FALSE;

	// ok so far - if we need to account for any arrowheads, do so now.
	if (fSuccess)
	{
		if (pArrow1 != NULL)
			drBounds = drBounds.Union(pArrow1->GetArrowBoundingRect(this, LineWidth, TRUE));

		if (pArrow2 != NULL)
			drBounds = drBounds.Union(pArrow2->GetArrowBoundingRect(this, LineWidth, FALSE));
	}

	// ok, if we're a success then record the bounds.
	if (fSuccess)
		*pRect = drBounds;

	return fSuccess;
}



/********************************************************************************************

>	DocRect Path::GetBlobRect()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/94
	Returns:	The bounding box of the path and blobs
	Purpose:	Calculates the bounding rectangle of the paths coordinates expanded by the
				current blob size.
				This is calculated on the fly, so don't over use it

********************************************************************************************/

DocRect Path::GetBlobRect()	const
{
	// If the path is empty, then return an empty rectangle
	if (UsedSlots==0)
		return DocRect(0,0,0,0);

	// de-reference the coord array
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	DocRect BlobRect;

	for (INT32 i=0; i<UsedSlots; i++)
	{
		DocRect TempRect;
		GetApplication()->GetBlobManager()->GetBlobRect(Coords[i],&TempRect);

		if (i==0)
			BlobRect = TempRect;					// For first coord, set BlobRect = rect of blob
		else
			BlobRect = BlobRect.Union(TempRect);	// for other coords, find the union
	}

	// return the completed rectangle
	return BlobRect;
}
#endif


/********************************************************************************************

>	DocCoord Path::GetEndPoint()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	The Endpoint of the Path Element pointed at by Current Path position
	Purpose:	Finds the endpoint of a given path element and returns it. For MoveTos and
				LineTos, the point returned will be its associated coord, but for a CurveTo
				the last coord will be returned
	Errors:		Ensures if the position is not valid or out of bounds

********************************************************************************************/

DocCoord Path::GetEndPoint()
{
	ENSURE( CurrentPos<UsedSlots, "Index out of bounds in GetEndPoint" );

	// de-reference the verb array
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// de-reference the coord array
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	
	if ((Verbs[CurrentPos] & (~PT_CLOSEFIGURE))==PT_BEZIERTO)
		return Coords[CurrentPos+2];
	else
		return Coords[CurrentPos];
}



/********************************************************************************************

>	DocCoord Path::GetControl1()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Returns:	DocCoord - the Coord of the first control point
	Purpose:	Finds the first control point of a bezier

********************************************************************************************/

DocCoord Path::GetControl1()
{
	ENSURE( CurrentPos<UsedSlots, "Index out of bounds in GetEndPoint" );

	// de-reference the coord array
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	
	ENSURE((((PathVerb*) DescribeHandle(VerbHandle)) [CurrentPos] & (~PT_CLOSEFIGURE))==PT_BEZIERTO, "Not a Bezier in Path::GetControl1()");

	return Coords[CurrentPos];
}


/********************************************************************************************

>	DocCoord Path::GetControl2()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Returns:	DocCoord - The coord of the seconds control point
	Purpose:	Finds the second control point of a Bezier

********************************************************************************************/

DocCoord Path::GetControl2()
{
	ENSURE( CurrentPos<UsedSlots, "Index out of bounds in GetEndPoint" );

	// de-reference the arrays
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	
	ENSURE((((PathVerb*) DescribeHandle(VerbHandle))[CurrentPos+1] & (~PT_CLOSEFIGURE))==PT_BEZIERTO, "Not a Bezier in Path::GetControl2()");

	return Coords[CurrentPos+1];
}



/********************************************************************************************

>	DocCoord* Path::GetCoordArray()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	Pointer to the Array of coords held by the path.
	Purpose:	Gives you access to the coordinate array. This array is simply a list of all
				the coordinates in the path, in order. This should be very useful if you need
				to perform transformations or other actions that effect all the coordinates.
				The coordinates are in the corect order for the NT PolyDraw() function handle.

********************************************************************************************/

DocCoord* Path::GetCoordArray() const
{
	// de-reference the coord array
	return (DocCoord*) DescribeHandle(CoordHandle);
}


/********************************************************************************************

>	PathVerb* Path::GetVerbArray()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	A pointer to the verb array held by the path. 
	Purpose:	This array holds a list of all the path elements used in the path. 
				The structure used is the same as the NT structure and can in fact be passed
				striaght on to the NT PolyDraw function.
				The verbs must be one of the following PC_MoveTo, PC_LineTo or PC_CurveTo.
				LineTo and CurveTo verbs can be combined with a ClosePath Flag

********************************************************************************************/

PathVerb* Path::GetVerbArray() const
{
	// de-reference the Verb array
	return (PathVerb*) DescribeHandle(VerbHandle);
}


/********************************************************************************************

>	PathFlag* Path::GetFlagArray()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	A pointer to an array of Path Flags
	Purpose:	This array holds a set of flags for each and every coordinate in the path.
				These flags are used to record information specific to each coordinate, such
				as its selection state, to determine if it should smooth this part of the
				path etc
	SeeAlso:	PathFlag

********************************************************************************************/

PathFlags* Path::GetFlagArray() const
{
	// de-reference the Flag array
	return (PathFlags*) DescribeHandle(FlagsHandle);
}



/********************************************************************************************

>	void Path::GetPathArrays(PathVerb** VerbArray, DocCoord** CoordArray = NULL, PathFlags** FlagsArray = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/95
	Inputs:		VerbArray - pointer to a pointer to a PathVerb object
				CoordArray - pointer to a pointer to a DocCoord
				FlagsArray - pointer to a pointer to a PathFlags object
	Outputs:	VerbArray points to the verbs array
				CoordArray points to the coordinates array
				FlagsArray points to the flags array
	Returns:	-
	Purpose:	Easy way of getting the path pointers into variables.  Combines three function
				calls into one.
	Errors:		-

********************************************************************************************/
void Path::GetPathArrays(PathVerb** VerbArray, DocCoord** CoordArray, PathFlags** FlagsArray)
{
	if (VerbArray != NULL)
		*VerbArray = (PathVerb*) DescribeHandle(VerbHandle);
	if (CoordArray != NULL)
		*CoordArray = (DocCoord*) DescribeHandle(CoordHandle);
	if (FlagsArray != NULL)
		*FlagsArray = (PathFlags*) DescribeHandle(FlagsHandle);
}



/********************************************************************************************

>	BOOL Path::AddMoveTo(DocCoord p1, PathFlags* NewFlags=NULL)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		p1 - the coord to draw the line to
				NewFlags - Flags associtaed with the coord
	Returns:	TRUE if it worked, FALSE if it failed to get more memory
	Purpose:	Adds a new MoveTo to the end of the path
	SeeAlso:	Path::AddLineTo; Path::AddCurveTo

********************************************************************************************/

BOOL Path::AddMoveTo(DocCoord p1, PathFlags* NewFlags)
{
	// Sadly there is no way for the outside world to set the insert position
	// to the end of the path!! SetPathPosition() for instance sets the currentpos to
	// usedslots-1, when passed usedslots which means you can never ever call InsertLineTo
	// and expect a line to be added to the pathend, it will be inserted before the element
	// which is the last in the path. Ok so this function does the job.
		
	CurrentPos = UsedSlots;
	return InsertMoveTo(p1, NewFlags);
}



/********************************************************************************************

>	BOOL Path::AddLineTo(DocCoord p1, PathFlags* NewFlags=NULL)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		p1 - the coord to draw the line to
				NewFlags - Flags associtaed with the coord
	Returns:	TRUE if it worked, FALSE if it failed to get more memory
	Purpose:	Adds a new LineTo to the end of the path
	SeeAlso:	Path::AddMoveTo; Path::AddCurveTo

********************************************************************************************/

BOOL Path::AddLineTo(DocCoord p1, PathFlags* NewFlags)
{
	CurrentPos = UsedSlots;
	return InsertLineTo(p1, NewFlags);
}



/********************************************************************************************

>	void Path::AddCurveTo(DocCoord p1, DocCoord p2, DocCoord p3, PathFlags* NewFlags=NULL)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		p1 - The first control point
				p2 - The second control point
				p3 - The endpoint
				NewFlags - The flags to set the new coords to
	Returns:	FALSE if there was an error, TRUE otherwise
	Purpose:	Adds a Curve to the end of the path
	Errors:		Can fail if it can not get enough memory to add the curve
	SeeAlso:	Path::AddLineTo; Path::AddMoveTo

********************************************************************************************/

BOOL Path::AddCurveTo(DocCoord p1, DocCoord p2, DocCoord p3, PathFlags* NewFlags)
{
	CurrentPos = UsedSlots;
	return InsertCurveTo(p1,p2,p3,NewFlags);
}

/********************************************************************************************

>	void Path::AddCurveTo(DocCoord p3, PathFlags* NewFlags=NULL)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/94
	Inputs:		p3 - The endpoint
				NewFlags - The flags to set the new coords to
	Returns:	FALSE if there was an error, TRUE otherwise
	Purpose:	Adds a straight line version of a curve to the end of the path. The control
				points are positioned 1/3 and 2/3 along the line between its end points
	Errors:		Can fail if it can not get enough memory to add the curve
	SeeAlso:	Path::AddLineTo; Path::AddMoveTo

********************************************************************************************/

BOOL Path::AddCurveTo(DocCoord p3, PathFlags* NewFlags)
{
	ERROR1IF( UsedSlots==0,FALSE,"No previous path elements when calling Path::AddCurveTo()" );
	// set the current pos.
	CurrentPos = UsedSlots;

	// Find the coord of the last point in the path
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	DocCoord prev = Coords[CurrentPos-1];

	// Create some temp coords
	DocCoord p1,p2;

	// calculate the intermediary control points
	p1.x = (2*prev.x + p3.x)/3;
	p1.y = (2*prev.y + p3.y)/3;
	p2.x = (prev.x + 2*p3.x)/3;
	p2.y = (prev.y + 2*p3.y)/3;

	return InsertCurveTo(p1,p2,p3,NewFlags);
}



/********************************************************************************************

>	BOOL Path::InsertMoveTo(DocCoord p1, PathFlags* NewFlags = NULL)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Inputs:		p1 - The coordinate to move to
				NewFlags - The flags associated with the coord
	Returns:	TRUE if it worked, FALSE otherwise
	Purpose:	Inserts a MoveTo element into the middle of the Path and asks for extra
				memory if we need it. It will return FALSE if we fail to get the memory.

********************************************************************************************/

BOOL Path::InsertMoveTo(DocCoord p1, PathFlags* NewFlags)
{
	// We need only 1 empty slot to add a MoveTo
	if (!MakeSpaceInPath(1))
	{
		ERROR( _R(IDS_PATH_MEM_ERROR), FALSE );
	}

	// get all the pointers
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	// Prepare the flags
	PathFlags UseFlags;
	if (NewFlags!=NULL)
		UseFlags = *NewFlags;

	UseFlags.IsEndPoint = TRUE;

	// There is now a hole in the middle of the path. We can fill in the appropriate details
	// and call it a day
	Verbs[CurrentPos] = PT_MOVETO;
	Coords[CurrentPos] = p1;
	Flags[CurrentPos] = UseFlags;

	// Update the memory usage vars
	UsedSlots++;
	UnUsedSlots--;

	// move the position indicator to point to the thing after our new item
	SyncExtraInfo();
	CurrentPos++;

	return TRUE;
}



/********************************************************************************************

>	BOOL Path::InsertLineTo(DocCoord p1, PathFlags* NewFlags=NULL)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Inputs:		p1 - the coord to draw the line to
				NewFlags - Flags associtaed with the coord
	Returns:	TRUE if it worked, FALSE if it failed to get more memory
	Purpose:	Inserts a LineTo into the path at the Current Path Position

********************************************************************************************/

BOOL Path::InsertLineTo(DocCoord p1, PathFlags* NewFlags)
{
	// We need only 1 empty slot to add a LineTo
	if (!MakeSpaceInPath(1))
	{
		ERROR( _R(IDS_PATH_MEM_ERROR), FALSE );
	}

	// get all the pointers
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	// Prepare the flags
	PathFlags UseFlags;
	if (NewFlags!=NULL)
		UseFlags = *NewFlags;

	UseFlags.IsEndPoint = TRUE;

	// There is now a hole in the middle of the path. We can fill in the appropriate details
	// and call it a day
	Verbs[CurrentPos] = PT_LINETO;
	Coords[CurrentPos] = p1;
	Flags[CurrentPos] = UseFlags;

	// Update the memory usage vars
	UsedSlots++;
	UnUsedSlots--;

	// move the position indicator to point to the thing after our new item
	SyncExtraInfo();
	CurrentPos++;

	return TRUE;
}



/********************************************************************************************

>	void Path::InsertCurveTo(DocCoord p1, DocCoord p2, DocCoord p3, PathFlags* NewFlags=NULL)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Inputs:		p1 - The first control point
				p2 - The second control point
				p3 - The endpoint
				NewFlags - The flags to set the new coords to
	Returns:	FALSE if there was an error, TRUE otherwise
	Purpose:	Inserts a Curve into the path at the Current Path Position
	Errors:		Can fail if it can not get enough memory to add the curve
	SeeAlso:	Path::InsertLineTo; Path::InsertMoveTo

********************************************************************************************/

BOOL Path::InsertCurveTo(DocCoord p1, DocCoord p2, DocCoord p3, PathFlags* NewFlags)
{
	// We need 3 empty slot to add a CurveTo
	if (!MakeSpaceInPath(3))
	{
		ERROR( _R(IDS_PATH_MEM_ERROR), FALSE );
	}

	// get all the pointers
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	// Prepare the flags
	PathFlags UseFlags;
	if (NewFlags!=NULL)
		UseFlags = *NewFlags;

	UseFlags.IsEndPoint = FALSE;

	// There is now a hole in the middle of the path.
	// We can fill in the appropriate details for the first control point
	Verbs[CurrentPos] = PT_BEZIERTO;
	Coords[CurrentPos] = p1;
	Flags[CurrentPos] = UseFlags;

	SyncExtraInfo();
	CurrentPos++;

	// and the second control point
	Verbs[CurrentPos] = PT_BEZIERTO;
	Coords[CurrentPos] = p2;
	Flags[CurrentPos] = UseFlags;
	CurrentPos++;

	// and the endpoint
	UseFlags.IsEndPoint = TRUE;
	Verbs[CurrentPos] = PT_BEZIERTO;
	Coords[CurrentPos] = p3;
	Flags[CurrentPos] = UseFlags;
	CurrentPos++;

	// Update the memory usage vars
	UsedSlots += 3;
	UnUsedSlots -= 3;

	// The Current Path Position has already been updated and now points to the item
	// after the curveto (or off the end of the path, if this is the last element in the path

	return TRUE;
}



/********************************************************************************************

>	BOOL Path::CloseSubPath()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	TRUE if it worked, FALSE if it fails (ie you tried to close a path that only
				had a PT_MOVETO element in it.
	Purpose:	To indicate that a path is closed with a line, you should call CloseSubPath
				with the Current Path Position indicator somewhere inside the sub-path that
				you want closed. This function will adjust the verb of the last line segment
				in the sub-path by adding the PT_CLOSEFIGURE element into as described in the
				NT Prog Reference guides for the PolyDraw() function (page 304 of book 4 at
				last count)
				The Current Path Position is left on the element at the end of the subpath

********************************************************************************************/

BOOL Path::CloseSubPath()
{
	// need to search for either a MoveTo or the end of the path and insert a close
	// path just before it. We start from the position supplied
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// We are inside the path somewhere
	while (CurrentPos+1<UsedSlots)
	{
		if (Verbs[CurrentPos+1]==PT_MOVETO)
		{
			// go back one and close the path
			ENSURE( Verbs[CurrentPos]!=PT_MOVETO, "You can not close a path at a moveto" );
			if (Verbs[CurrentPos]==PT_MOVETO)
				return FALSE;

			Verbs[CurrentPos] = Verbs[CurrentPos] | PT_CLOSEFIGURE;
			return TRUE;
		}

		CurrentPos++;
	}

	// Make sure we are in the path
	if (CurrentPos>=UsedSlots)
		CurrentPos = UsedSlots-1;

	// We are right at the end of the path, so change the last path element
	ENSURE( Verbs[CurrentPos]!=PT_MOVETO, "You can not close a path at a moveto" );
	if (Verbs[CurrentPos]==PT_MOVETO)
		return FALSE;

	Verbs[CurrentPos] = Verbs[CurrentPos] | PT_CLOSEFIGURE;
	CurrentPos++;
	return TRUE;
}

/********************************************************************************************

>	BOOL Path::DeleteSection(INT32 StartSlot, INT32 NumSlots)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/94
	Inputs:		StartSlot - The slot number to start deleting from
				NumSlots - The number of slots to delete
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Deletes sections out of the middle of the path. Used by DeleteElement and
				for Deleting sub paths etc.

********************************************************************************************/

BOOL Path::DeleteSection(INT32 StartSlot, INT32 NumSlots)
{
	// Saftey Checks
	ENSURE( StartSlot<UsedSlots, "PathPosition is off end of path in Path::DeleteElement" );
	ENSURE( StartSlot>=0, "PathPosition is off beginning of path in Path::DeleteElement" );
	ENSURE( NumSlots>0, "Trying to delete Zero slots from the path!" );

	if (NumSlots==0)
		return TRUE;

	// Find out how much memory to move about and adjust the element counters
	INT32 SlotsToMove = UsedSlots-StartSlot-NumSlots;

	// and move it all about if we need to
	if (SlotsToMove>0)
	{
		// Dereference the pointers
		PathVerb*  Verbs  = (PathVerb*)  DescribeHandle(VerbHandle);
		DocCoord*  Coords = (DocCoord*)  DescribeHandle(CoordHandle);
		PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);

		memmove((void*)(&Verbs[StartSlot]), (void*)(&Verbs[StartSlot+NumSlots]), SlotsToMove*sizeof(PathVerb));
		memmove((void*)(&Flags[StartSlot]), (void*)(&Flags[StartSlot+NumSlots]), SlotsToMove*sizeof(PathFlags));
		memmove((void*)(&Coords[StartSlot]), (void*)(&Coords[StartSlot+NumSlots]), SlotsToMove*sizeof(DocCoord));
		if (ExtraInfo != NULL)
			ExtraInfo->ShiftDownExtraInfo(StartSlot, NumSlots, SlotsToMove);
	}

	// Update the path details
	UsedSlots   -= NumSlots;
	UnUsedSlots += NumSlots;

	return Compact();
}


/********************************************************************************************

>	void Path::DeleteElement()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Purpose:	Deletes the element at the Current Path Position. May free some memory if
				the path gets to have too much unused space in the path. The Current Path
				Position points at the slot before the one being deleted

********************************************************************************************/

BOOL Path::DeleteElement()
{
	// Dereference the pointers
	PathVerb*  Verbs  = (PathVerb*)  DescribeHandle(VerbHandle);
//	DocCoord*  Coords = (DocCoord*)  DescribeHandle(CoordHandle);

	// Saftey Checks
	ENSURE( CurrentPos<UsedSlots, "PathPosition is off end of path in Path::DeleteElement" );
	ENSURE( CurrentPos>=0, "PathPosition is off beginning of path in Path::DeleteElement" );
	ENSURE( ((PathFlags*) DescribeHandle(FlagsHandle))[CurrentPos].IsEndPoint==TRUE, "Delete Element called when Path Position was not on an endpoint");

	// Find the start of the element
	INT32 StartSlot = CurrentPos;

	// Work out how many slots we need to delete
	INT32 NumSlots;
	if ((Verbs[CurrentPos] & (~PT_CLOSEFIGURE)) == PT_BEZIERTO)
	{
		NumSlots  = 3;
		StartSlot -= 2;
	}
	else
	{
		NumSlots = 1;
	}

	// Delete the element
	BOOL RetVal = DeleteSection(StartSlot, NumSlots);

	// tidy up the Current Path Position
	CurrentPos = StartSlot-1;
	if (CurrentPos<0)
		CurrentPos = 0;

	return RetVal;
}




/********************************************************************************************

>	BOOL Path::DeleteFromElement(INT32 ElementNum)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/94
	Inputs:		ElementNum - the number of the element to start deleting from.
	Returns:	TRUE if the deletion was a success
	Purpose:	Deletes all the elements in the path from ElementNum to the end
				of the path

********************************************************************************************/

BOOL Path::DeleteFromElement(INT32 ElementNum)
{
	// Find out how many slots there are from ElementNum to the end
	INT32 SlotsToDelete = UsedSlots-ElementNum;

	// go and delete the end of the path
	BOOL RetVal = DeleteSection(ElementNum, SlotsToDelete);

	// Set the current Path position to just after the path, ready for further additions
	CurrentPos = UsedSlots;

	return RetVal;
}


/********************************************************************************************

>	BOOL Path::ClearPath()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/02/94
	Returns:	TRUE if it was able to free up the unused parts of the path, FALSE if there
				was a memory de-alloc error
	Purpose:	Empties the path of all its entries and tries to free up excessive amounts of
				unused memory left over at the end. It does keep some memory though, ready
				for a new path to be put into it.
				It also now resets CurrentPos back to zero.

********************************************************************************************/

BOOL Path::ClearPath()
{
	if (UsedSlots==0)
		return TRUE;

 	BOOL ok = DeleteSection(0,UsedSlots);
	if (ok)
		CurrentPos=0;
	return ok;
}


/********************************************************************************************

>	BOOL Path::ClearPath(BOOL compress)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		compress - a boolean, to describe whether to compress the paths memory
						   or not.
	Returns:	TRUE if it was able to free up the unused parts of the path, FALSE if there
				was a memory de-alloc error
	Purpose:	Empties the path of all its entries.
				If compress is TRUE the routine will try to free up excessive amounts of
				unused memory left over at the end. 
				It also now resets CurrentPos back to zero.
	SeeAlso:	Path::Compact();

********************************************************************************************/

BOOL Path::ClearPath(BOOL compress)
{
	if (UsedSlots>0)
	{
		// reset the path definitions
		UnUsedSlots+=UsedSlots;
		UsedSlots=0;
		CurrentPos=0;
		// make sure we compress memory if necessary
		if (compress)
			Compact();						// ignore any error coming from here
	}	
	return TRUE;
} 


/********************************************************************************************

>	BOOL Path::IsSelected()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	TRUE if the point is selected, FALSE if not
	Purpose:	return the Selected flag for the coordinate

********************************************************************************************/

BOOL Path::IsSelected()
{
	ENSURE( CurrentPos<UsedSlots, "PathPosition is out of Bounds in Path::IsSelected" );

	// Get the flags array
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	// and return the selected flag
	return Flags[CurrentPos].IsSelected;
}


/********************************************************************************************

>	BOOL Path::IsEndPoint()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/02/94
	Returns:	TRUE if this point is an EndPoint (ie Not a Bezier control point)
	Purpose:	return the IsEndPoint flag for the coordinate

********************************************************************************************/

BOOL Path::IsEndPoint()
{
	ENSURE( CurrentPos<UsedSlots, "PathPosition is out of Bounds in Path::IsEndPoint" );

	// Get the flags array
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	// and return the selected flag
	return Flags[CurrentPos].IsEndPoint;
}



/********************************************************************************************

>	BOOL Path::IsSmooth()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	TRUE if the point is Smooth, FALSE if not
	Purpose:	returns the Smooth flag for the Current Path Position

********************************************************************************************/

BOOL Path::IsSmooth()
{
	ENSURE( CurrentPos<UsedSlots, "PathPosition is out of Bounds in Path::IsSmooth" );

	// Get the flags array
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	// and return the selected flag
	return Flags[CurrentPos].IsSmooth;
}



/********************************************************************************************

>	BOOL Path::IsRotate()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94
	Returns:	TRUE if the point has the rotate flag set, FALSE if not
	Purpose:	returns the Rotate flag for the coordinate

********************************************************************************************/

BOOL Path::IsRotate()
{
	ENSURE( CurrentPos<UsedSlots, "PathPosition is out of Bounds in Path::IsRotate" );

	// Get the flags array
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	// and return the selected flag
	return Flags[CurrentPos].IsRotate;
}



/********************************************************************************************

>	BOOL Path::IsNearOpenEnd(const DocRect& BlobRect)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/94
	Inputs:		BlobRect - The rect to see if any of the endpoints are in
	Returns:	TRUE if the TestCoord is close to one of the endpoints
	Purpose:	Determines if the Coordinate is near any of the path open ends. This
				function looks through all the open ends in complex paths and returns
				TRUE if any of them are near the TestCoord

********************************************************************************************/

BOOL Path::IsNearOpenEnd(const DocRect& BlobRect, INT32* SlotNum)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Dereference a few pointers
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// Start searching all the coords for EndPoints
	INT32 EndSlot = UsedSlots-1;
	for (INT32 i=UsedSlots-1; i>=0; i--)
	{
		// Are we at the start of a sub-path yet
		if (Verbs[i] == PT_MOVETO)
		{
			// Is this sub path open?
			if ((Verbs[EndSlot] & PT_CLOSEFIGURE) == 0)
			{
				// This is an open sub path, so test if they are close to the TestCoord
				if (BlobRect.ContainsCoord(Coords[i]))
				{
					*SlotNum = i;
					return TRUE;
				}

				if (BlobRect.ContainsCoord(Coords[EndSlot]))
				{
					*SlotNum = EndSlot;
					return TRUE;
				}
			}

			// That sub path did not intersect the rect, so carry on
			EndSlot = i-1;
		}
	}
#endif
	// No luck, so fail
	return FALSE;
}


/********************************************************************************************

>	BOOL Path::IsOpenEnd(const INT32 index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/95
	Inputs:		index = index of element to check
	Returns:	TRUE if this index is set to either end of an open subpath
	Purpose:	Tests to see if the index provided specifies either the first or last
				element in an open subpath. ie the index is a true end point.

********************************************************************************************/

BOOL Path::IsOpenEnd(const INT32 index)
{
	// Do a quick sanity check
	if (index<0 || index>=UsedSlots || UsedSlots==0)
		return FALSE;

	// 0<=index<=UsedSlots-1
	// Dereference the verbs
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// Check its not the end index
	INT32 end = index;
	FindEndElOfSubPath(&end);
	if (Verbs[end] & PT_CLOSEFIGURE)
		return FALSE;

	if (end==index)
		return TRUE;

	// Check for a start point
	return (Verbs[index]==PT_MOVETO);

}



/********************************************************************************************

>	BOOL Path::IsComplexPath()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/94
	Returns:	TRUE if the Path is Complex
	Purpose:	Tests to see if the path is a complex path (ie it has multiple sub-paths)

********************************************************************************************/

BOOL Path::IsComplexPath()
{
	// Get the path verbs
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	INT32 SubPaths = 0;
	for (INT32 i=0; i<UsedSlots; i++)
	{
		// Count all the MoveTos in the path
		if (Verbs[i] == PT_MOVETO)
			SubPaths++;
	}

	// Make sure that there was a moveto in this path. This should not happen
	ENSURE( SubPaths>0, "Path::IsComplexPath() - Found a Path with no MoveTo in it!" );

	// If there was only one SubPath then it is simple, otherwise it is complex
	if (SubPaths==1)
		return FALSE;
	else
		return TRUE;
}


/********************************************************************************************

>	void Path::GetFlags( PathFlags* NewFlags)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/02/94
	Outputs:	NewFlags - Holds the flags for the current path position
	Purpose:	returns all the flags for the current path position

********************************************************************************************/

void Path::GetFlags( PathFlags* NewFlags)
{
	ENSURE( CurrentPos<UsedSlots, "PathPosition is out of Bounds in Path::IsEndPoint" );

	// Get the flags array
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	// and return the selected flag
	*NewFlags = Flags[CurrentPos];
}


/********************************************************************************************

>	void Path::SetPathFlag( const PathFlags &NewFlags )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/02/94
	Inputs:		NewFlags - The flags you want to set
	Purpose:	Sets the flags at the current path position to the flags provided

********************************************************************************************/

void Path::SetFlags( const PathFlags &NewFlags )
{
	ENSURE( CurrentPos<UsedSlots, "PathPosition is out of Bounds in Path::IsEndPoint" );

	// Get the flags array
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	// and set the flags
	Flags[CurrentPos] = NewFlags;
}


/********************************************************************************************

	void Path::InitialiseFlags()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		-
	Outputs:	The path's flags array contains valid flags for each of its verbs
	Returns:	-
	Purpose:	This generates a legal path flags array based on the all path verbs. 
				It sets all flags to default values and then scans all
				end points setting their end point flag to TRUE
	SeeAlso:	-

********************************************************************************************/

void Path::InitialiseFlags()
{
	InitialiseFlags(0,UsedSlots);
}


/********************************************************************************************

	void Path::InitialiseFlags( const UINT32 Startindex, const INT32 Len)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/11/94
	Inputs:		Startindex  = index of element at which to start
				Len	   		= num verbs to set corresponding flags for
	Outputs:	The path's flags array contains valid flags for each of its verbs
	Returns:	-
	Purpose:	This generates a legal path flags array based on the path verbs within the
				specified region. It sets all flags to default values and then scans all
				end points setting their end point flag to TRUE
	SeeAlso:	-

********************************************************************************************/

void Path::InitialiseFlags(const INT32 Startindex,
						   const INT32 Len)
{
	ERROR3IF(Startindex<0,"startindex off start of path in Path::SetPathFlags()");
	ERROR3IF(Startindex+Len>UsedSlots,"(startindex+len)	of end of path in Path::SetPathFlags()"); 

	// Get the flags array
	PathFlags* pFlags = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb* pVerbs = (PathVerb*)   DescribeHandle(VerbHandle);

	InitialiseFlags(pVerbs+Startindex,pFlags+Startindex,Len);
}



/********************************************************************************************

	void Path::InitialiseFlags(PathVerb* pVerbs, PathFlags* pFlags, const INT32 Len)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		pVerbs = ptr to a path verbs array
				pFlags = ptr to buffer to place the initialised flags
				Len	   = num verbs in pVerbs, and num flags in pFlags
	Outputs:	pFlags array contains valid flags for each of the verbs in pVerbs
	Returns:	-
	Purpose:	This generates a legal path flags array based on the path verbs in pVerbs within the
				specified region. It sets all flags in pFlags to default values and then scans all
				end points setting their end point flag to TRUE
	SeeAlso:	-

********************************************************************************************/

void Path::InitialiseFlags(PathVerb* pVerbs, PathFlags* pFlags, const INT32 Len)
{
	INT32 n;

	// Reset all the path flags to default values
	PathFlags DefaultPathFlags;
	for (n=0; n < Len; n++)
		pFlags[n] = DefaultPathFlags;

	// Scan the verbs, so that the end point flags can be set correctly
	for (n=0; n < Len; n++)
	{
		PathVerb Verb = pVerbs[n] & ~PT_CLOSEFIGURE;
		switch (Verb)
		{
			case PT_LINETO:
			case PT_MOVETO:
				pFlags[n].IsEndPoint = TRUE;
				break;

			case PT_BEZIERTO:
				n += 2;
				ERROR3IF(n>=Len,"Found a PT_BEZIERTO, but third pt is off the end of the array");
				pFlags[n].IsEndPoint = TRUE;
				break;

			default:
				ERROR3_PF(("Illegal path verb found : %c",Verb));
				break;
		}
	}
}



/********************************************************************************************

>	void Path::DumpPath()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/02/94
	Purpose:	Dumps the whole path out to the debug terminal

********************************************************************************************/

void Path::DumpPath()
{
#ifdef _DEBUG
	TRACE( _T("\nStarting Path Dump\n") );
	TRACE( _T("[%d=PT_MOVETO, %d=PT_LINETO, %d=PT_BEZIERTO, %d=PT_CLOSEFIGURE]\n"), 
		   PT_MOVETO, PT_LINETO, PT_BEZIERTO, PT_CLOSEFIGURE );
	TRACE( _T("UsedSlots=%ld, UnUsedSlots=%ld, CurrentPos=%ld\n"), UsedSlots, UnUsedSlots, CurrentPos );
	TRACE( _T("Path Size in Bytes=%ld\n"), GetPathByteLength() );
	TRACE( _T("\tVerb\tCoord\n") );

	// de-reference the pointers
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	// loop through all the coords, displaying their type and position
	for (INT32 i=0; i<UsedSlots; i++)
	{
		TRACE( _T("%d\t%d\t(%ld, %ld)"), i, Verbs[i], Coords[i].x, Coords[i].y );
		if (Flags[i].IsSmooth)
			TRACE( _T("\tS"));
		else
			TRACE( _T("\t "));

		if (Flags[i].IsRotate)
			TRACE( _T("R"));
		else
			TRACE( _T(" "));

		if (Flags[i].IsEndPoint)
			TRACE( _T("E"));
		else
			TRACE( _T(" "));

		if (Flags[i].IsSelected)
			TRACE( _T("(sel)"));

		TRACE( _T("\n"));
	}
#endif
}



/********************************************************************************************

>	void Path::GetDebugDetails()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>; Karim 26/05/2000
	Created:	20/12/94
	Purpose:	Dumps the whole path out during tree display debugging.

				(Karim)
				I'm updating this method to give a little bit more info, so's it isn't
				necessary to manually dump path data every time you need to. It doesn't
				currently seem to be called from anywhere, so my changes should have zero
				effect on existing code. Please feel free to use this method and alter it
				to display more info if required.

				Info returned:
					*	Number of handles and bytes used by the path data.
					*	Whether the path is filled or not.
					*	Each handle's verbs, coords and flags
							Verbs - L - LineTo
									C - CurveTo
									M - MoveTo
									S - ClosePath

	Notes:		For example of use, see NodeClipViewController::GetDebugDetails().

********************************************************************************************/
void Path::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	String_256 TempStr;

	// output number of handles and space used by the path.
	INT32 nSlots = GetNumCoords();
	TempStr._MakeMsg( TEXT("#1%ld handles, #2%ld bytes used\r\n"),	nSlots,
																	GetPathByteLength() );
	(*Str) += TempStr;

	// output whether or not the path is filled.
	TempStr._MakeMsg( TEXT("Path is#1%s Filled and is#2%s Stroked\r\n"),
																IsFilled	? "" : " not",
																IsStroked	? "" : " not" );
	(*Str) += TempStr;

	// output the path's handle information.
	(*Str) += TEXT( "Num\tType\tX Coord\tY Coord\r\n" );
	PathVerb *	Verbs	= GetVerbArray();
	DocCoord *	Coords	= GetCoordArray();
	PathFlags *	Flags	= GetFlagArray();
	for (INT32 i=0; i < nSlots; i ++)
	{
		String_32 VerbStr;
		VerbStr._MakeMsg(TEXT("#1%d ("), Verbs[i]);
		if (Verbs[i] & PT_CLOSEFIGURE)	VerbStr += TEXT("S");
		if (Verbs[i] & PT_LINETO)		VerbStr += TEXT("L");
		if (Verbs[i] & PT_BEZIERTO)		VerbStr += TEXT("C");
		if (Verbs[i] & PT_MOVETO)		VerbStr += TEXT("M");
		VerbStr += TEXT(")");

		TempStr._MakeMsg( TEXT("#1%d.\t#2%S\t#3%ld,\t#4%ld\t"),
						  i, &VerbStr, Coords[i].x, Coords[i].y );

		if (Flags[i].IsSmooth)		TempStr += TEXT("S");
		if (Flags[i].IsRotate)		TempStr += TEXT("R");
		if (Flags[i].IsEndPoint)	TempStr += TEXT("E");
		if (Flags[i].IsSelected)	TempStr += TEXT("(sel)");

		TempStr += TEXT("\r\n");
		(*Str) += TempStr;
	}
#endif
}


#if !defined(EXCLUDE_FROM_XARLIB)

/********************************************************************************************

>	void Path::RenderPathBlobs(Spread* pSpread)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/02/94
	Inputs:		pSpread - spread to render into
	Purpose:	Draws all the selection blobs for a path. Useful for putting the blobs on or
				taking them back off again.
				This goes into its own RenderOnTop loop and calls RenderPathBlobs(RenderRegion* pRender).

********************************************************************************************/

void Path::RenderPathBlobs(Spread* pSpread)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Go into a RenderOnTop loop
	RenderRegion* pRender = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
	while (pRender!=NULL)
	{
		RenderPathBlobs(pRender);
		pRender = DocView::GetNextOnTop(NULL);
	}
#endif
}



/********************************************************************************************

>	void Path::RenderPathBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> (split from pSpread varient by Markn)
	Created:	9/11/94
	Inputs:		pRender - The render region to draw into
	Purpose:	Draws all the selection blobs for a path. Useful for putting the blobs on or
				taking them back off again

********************************************************************************************/

void Path::RenderPathBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Dereference the pointers
	DocCoord*  Coords = (DocCoord*)  DescribeHandle(CoordHandle);
	PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb*  Verbs  = (PathVerb*)  DescribeHandle(VerbHandle);

	BOOL PrevIsEndPoint;
	DocCoord StartCoord = Coords[0];

	// loop through the path and render each of the blobs
	INT32 endcount = 0;		// Count the number of selected endpoints
	INT32 EndPtIndex = 0;	// Index of only selected endpoint
	for (INT32 i=0; i<UsedSlots; i++)
	{
		if (Flags[i].IsEndPoint)
		{
			// Render a blob
			if (!((Verbs[i] & PT_CLOSEFIGURE) && Coords[i]==StartCoord))
			{
				DrawBlob(pRender, Coords[i], Flags[i].IsSelected);
				// count the number of selected endpoints
				if (Flags[i].IsSelected)
				{
					EndPtIndex = i;
					endcount++;
				}
			}
			//GdiFlush();
			PrevIsEndPoint = TRUE;
			if (Verbs[i] == PT_MOVETO)
				StartCoord = Coords[i];
		}
	}
	// Now render the control point blobs if there was just one selected endpoint
	if (endcount == 1)
	{
		RenderPathControlBlobs(pRender, EndPtIndex);
	}
#endif
}



/********************************************************************************************

>	void Path::RenderPathSelectedControlBlobs(Spread* pSpread, BOOL Removing = TRUE)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/1/95
	Inputs:		pSpread - pointer to the spread containing the path
				Removing - TRUE if the blobs are being removed (ie rendered off)
	Purpose:	Draws all the control points selection blobs for an entire path. If you are
				to change the selection flags on a path then call this to remove all selection
				blobs, do your change, then call this again to draw the new blobs.
				This goes into its own RenderOnTop loop.

********************************************************************************************/

void Path::RenderPathSelectedControlBlobs(Spread* pSpread, BOOL Removing)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// If removing then render on into pending render regions first
//	if (Removing)
//		RenderSelectedControlBlobsToPendingRegions(pSpread);

	// Render the blobs on the path
	RenderRegion* pRender = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
	while (pRender!=NULL)
	{
		RenderPathSelectedControlBlobs(pSpread, pRender);
		pRender = DocView::GetNextOnTop(NULL);
	}	

	// If putting back on then render back off in pender render regions
//	if (!Removing)
//		RenderSelectedControlBlobsToPendingRegions(pSpread);
#endif
}



/********************************************************************************************

>	void Path::RenderPathSelectedControlBlobs(Spread* pSpread, RenderRegion* pRender)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/95
	Inputs:		pSpread - pointer to the spread containing the path
				pRender - pointer to region to render into
	Purpose:	Draws all the control points selection blobs for an entire path into a 
				specified render region.  Called by the above function
	SeeAlso:	Path::RenderPathSelectedControlBlobs

********************************************************************************************/
void Path::RenderPathSelectedControlBlobs(Spread* pSpread, RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(pSpread == NULL, "pSpread was NULL");
	ERROR3IF(pRender == NULL, "pRender was NULL");
	if ((pRender == NULL) || (pSpread == NULL))
		return ;

	// loop through the path and count the selected blobs
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	const INT32 NumCoords = GetNumCoords();
	INT32 endcount = 0;		// Count the number of selected endpoints
	INT32 EndPtIndex = 0;	// Index of only selected endpoint
	for (INT32 i=0; i<NumCoords; i++)
	{
		if (Flags[i].IsEndPoint && Flags[i].IsSelected)
		{
			if (!(Verbs[i] & PT_CLOSEFIGURE))
			{
				EndPtIndex = i;
				endcount++;
				DrawBlob(pRender, Coords[i], TRUE);
				DrawBlob(pRender, Coords[i], FALSE);
			}
		}
	}
	if (endcount == 1)
	{
		// Remove the control points and lines
		RenderPathControlBlobs(pRender, EndPtIndex);		
	}
#endif
}



/********************************************************************************************

>	void Path::RenderSelectedControlBlobsToPendingRegions(Spread* pSpread)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/95
	Inputs:		pSpread - pointer to the spread containing the path
	Purpose:	Draws all the control points selection blobs for an entire path into 
				all the pending render regions.
	SeeAlso:	Path::RenderPathSelectedControlBlobs

********************************************************************************************/
void Path::RenderSelectedControlBlobsToPendingRegions(Spread* pSpread)
{
#if !defined(EXCLUDE_FROM_RALPH)
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	if (!pRegionList->IsEmpty())
	{
		RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	

		while (pRegion != NULL)
		{
			// Check the RenderRegion is for the same spread.
			if (pRegion->GetRenderSpread() == pSpread)
			{
				// Render the blobs into this Render Region.
				RenderPathSelectedControlBlobs(pSpread, pRegion);
			}

			// Get the Next render region
			pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
		}
	}
#endif
}


/********************************************************************************************

>	void Path::RenderPathControlBlobs(Spread* pSpread, INT32 EndPtIndex)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> (from Rik Code)
	Created:	19/10/94
	Inputs:		pSpread - pointer to the spread containing the path
				EndPtIndex - the index of the endpoint around which the control points
				are to be rendered.
	Purpose:	Draws the control points selection blobs for an endpoint on a path. Used
				by RenderPathBlobs and also the Bezier tool.
				This goes into its own RenderOnTop loop.

********************************************************************************************/

void Path::RenderPathControlBlobs(Spread* pSpread, INT32 EndPtIndex)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Go into a RenderOnTop loop
	RenderRegion* pRender = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
	while (pRender!=NULL)
	{
		RenderPathControlBlobs(pRender,EndPtIndex);		
		pRender = DocView::GetNextOnTop(NULL);
	}
#endif
}



/********************************************************************************************

>	void Path::RenderPathControlBlobs(RenderRegion* pRender, INT32 EndPtIndex)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> (from Rik Code - split from pSpread varient by Markn)
	Created:	9/11/94
	Inputs:		pRender = ptr to render region to render into
				EndPtIndex - the index of the endpoint around which the control points
				are to be rendered.
	Purpose:	Draws all the control points selection blobs for a path. Used
				by RenderPathBlobs and also the Bezier tool.

********************************************************************************************/

void Path::RenderPathControlBlobs(RenderRegion* pRender, INT32 EndPtIndex)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Dereference the pointers
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
//	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	// Render the appropriate control points
	// We render the two control points adjacent to the only
	// selected endpoint, but we also render the two control points beyond 
	// those, because their positions might affect the curve.
	// A special case is when we have a closed path containing 
	// only a moveto and a curveto (it could happen) in which case
	// we'll have to spot that, and only render two controls instead
	// of four

	// find the index of the previous ctrl point of this endpoint
	INT32 i = FindPrevControlPoint(EndPtIndex);
	if (i>=0)
	{
		DrawControlBlob(pRender, Coords[i]);
		DrawControlLine(pRender, Coords[i], Coords[EndPtIndex]);
		DrawControlBlob(pRender, Coords[i-1]);
		DrawControlLine(pRender, Coords[i-1], Coords[i-2]);
	}

	// now do the next control point. Don't render if the next
	// is still part of the same path
	INT32 j = FindNextControlPoint(EndPtIndex);
	if (j>=0 && j!=i-1)
	{
		DrawControlBlob(pRender, Coords[j]);
		DrawControlLine(pRender, Coords[j], Coords[EndPtIndex]);

		DrawControlBlob(pRender, Coords[j+1]);
		DrawControlLine(pRender, Coords[j+1], Coords[j+2]);
	}
#endif
}





/********************************************************************************************

>	void Path::RenderPathPenBlobs(Spread* pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/94
	Inputs:		pSpread - The render spread to draw into
	Purpose:	Draws all the selection blobs and pen blobs for a path. Useful for putting 
				the blobs on or	taking them back off again

********************************************************************************************/

void Path::RenderPathPenBlobs(Spread* pSpread)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Go into a RenderOnTop loop
	RenderRegion* pRender = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
	while (pRender!=NULL)
	{
		RenderPathPenBlobs(pRender);
		pRender = DocView::GetNextOnTop(NULL);
	}
#endif
}


/********************************************************************************************

>	void Path::RenderPathPenBlobs(RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com> (via Rik)
	Created:	28/09/94
	Inputs:		pRegion - The render region to draw into
	Purpose:	Draws all the selection blobs and pen blobs for a path. Useful for putting 
				the blobs on or	taking them back off again

********************************************************************************************/

void Path::RenderPathPenBlobs(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Dereference the pointers
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	DocCoord StartCoord = Coords[0];
	INT32 SelEndPts = 0;

	// loop through the path and render each of the blobs
	INT32 EndPtIndex = 0;	// Index of only selected endpoint
	INT32 i;
	for (i=0; i<UsedSlots; i++)
	{
		if (Flags[i].IsEndPoint)
		{
			// Render a blob
			if (!((Verbs[i] & PT_CLOSEFIGURE) && Coords[i]==StartCoord))
			{
				DrawBlob(pRegion, Coords[i], Flags[i].IsSelected);
				// count the number of selected endpoints
				if (Flags[i].IsSelected)
				{
					EndPtIndex = i;
					SelEndPts++;
				}
			}
			if (Verbs[i] == PT_MOVETO)
				StartCoord = Coords[i];
		}
	}

	// now make sure the correct control handles are drawn
	if (SelEndPts == 1)
	{
		if (Flags[EndPtIndex].IsRotate && IsOpenEnd(EndPtIndex))
		{ 
			if (Verbs[EndPtIndex] == PT_MOVETO)
				i = FindNextControlPoint(EndPtIndex);
			else
				i = FindPrevControlPoint(EndPtIndex);

			if (i>=0)
			{
				DocCoord GhostPt;
				GhostPt.x = Coords[EndPtIndex].x - (Coords[i].x - Coords[EndPtIndex].x);
				GhostPt.y = Coords[EndPtIndex].y - (Coords[i].y - Coords[EndPtIndex].y);

				pRegion -> SetLineColour(COLOUR_BEZIERLINE);
				pRegion -> SetFillColour(COLOUR_TRANS);
				pRegion -> DrawLine(GhostPt, Coords[i]);

				pRegion -> SetLineColour(COLOUR_TRANS);
				pRegion -> SetFillColour(COLOUR_UNSELECTEDBLOB);
				pRegion -> DrawBlob(GhostPt, BT_UNSELECTED);
				pRegion -> DrawBlob(Coords[i], BT_UNSELECTED);
			}
		}
	}
#endif
}

#endif	// EXCLUDE_FROM_XARLIB


/********************************************************************************************

>	INT32 Path::FindPrevControlPoint(INT32 Index)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Index gives the endpoint whose previous control point I want
	Outputs:	-
	Returns:	Index to the control point (or -1 if none)
	Purpose:	This routine will scan back from an endpoint to find the attached bezier 
				control point. It takes account of closed and open paths and complex
				paths, and will wrap around the path if necessary.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 Path::FindPrevControlPoint(INT32 Index)
{
//	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
//	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	INT32 i;
	if (Verbs[Index] == PT_MOVETO)
	{
		// This is the start of a path - if the path is open, return -1
		// otherwise return the index of the last control point. If the last 
		// element is a line, return -1
		i = Index;
		FindEndOfSubPath(&i);
		// If the last segment isn't a bezier, return -1
		if (Verbs[i] != PT_BEZIERTO)
			return -1;

		// if the CLOSEFIGURE flag is set in the final element, return i+1
		// otherwise return -1
		if (Verbs[i+2] & PT_CLOSEFIGURE)
			return i+1;
		return -1;
	}
	else
	{
		// It isn't the start of a path, so see what kind of endpoint this is
		// and if it is a bezier, return the control point, otherwise return -1
		if ((Verbs[Index] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
			return Index-1;
		return -1;
	}
}

/********************************************************************************************

>	INT32 Path::FindNextControlPoint(INT32 Index)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Index gives the endpoint whose next control point I want
	Outputs:	-
	Returns:	Index to the control point (or -1 if none)
	Purpose:	This routine will scan forwards from an endpoint to find the attached 
				bezier control point. It takes account of closed and open paths and 
				complex paths, and will wrap around the path if necessary.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 Path::FindNextControlPoint(INT32 Index)
{
//	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
//	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	
	// See if this is the end of a path (no more elements or next point is moveto)
	if (Index+1 == UsedSlots || Verbs[Index+1] == PT_MOVETO)
	{
		// This is the last element, so see if we should wrap around
		if (Verbs[Index] & PT_CLOSEFIGURE)
		{
			// The path is closed, so we have to scan backwards to find the start
			INT32 i=Index;
			FindStartOfSubPath(&i);		// i points at start of subpath
			
			// return next control point if next element is a bezier, -1 otherwise
			if (Verbs[i+1] == PT_BEZIERTO)
				return i+1;
			return -1;
		}
		else
		{
			// Path is open, so no next control point
			return -1;
		}
	}
	else
	{
		// This element isn't the last in this subpath, so look at next element
		// if not bezier, return -1, otherwise return next element
		if (Verbs[Index+1] == PT_BEZIERTO)
			return Index+1;
		return -1;
	}
	return -1;
}


#if !defined(EXCLUDE_FROM_XARLIB)

/********************************************************************************************

>	void Path::DrawBlob(RenderRegion* pRender, const DocCoord& Coord, BOOL DrawSelected)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/02/94
	Inputs:		pRender - Render region to draw to
				Coord - the coord that the blob should be centred on
				DrawSelected - Draw a selected blob or an unselected blob
	Purpose:	Draws a single blob, either selected or un-selected

********************************************************************************************/

void Path::DrawBlob(RenderRegion* pRender, const DocCoord& Coord, BOOL DrawSelected)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (DrawSelected)
	{
		// Set the colours to draw the selected blob colours
		pRender->SetLineColour(COLOUR_BEZIERBLOB);
		pRender->SetFillColour(COLOUR_TRANS);

		// Draw a Selected blob to the render region
		pRender->DrawBlob(Coord, BT_SELECTED);
	}
	else
	{
		// Draw an UnSelected Blob to the render region
		// Set the colours back
//		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(Coord, BT_UNSELECTED);
	}
#endif
}


/********************************************************************************************

>	void Path::DrawControlBlob(RenderRegion* pRender, const DocCoord& Coord)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/02/94
	Inputs:		pRender - Render region to draw to
				Coord - the coord that the blob should be centred on
	Purpose:	Draws a blob for a Bezier control point

********************************************************************************************/

void Path::DrawControlBlob(RenderRegion* pRender, const DocCoord& Coord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Draw a Control blob to the render region
//	pRender->SetLineColour(COLOUR_BEZIERBLOB);
	pRender->SetLineColour(COLOUR_NONE);		// A line colour leads to bad rendering probs
												// on some video drivers. Best solution is not
												// to use one.
	pRender->SetFillColour(COLOUR_BEZIERBLOB);
	pRender->DrawBlob(Coord, BT_UNSELECTED);
#endif
}


/********************************************************************************************

>	void Path::DrawControlLine(RenderRegion* pRender, const DocCoord& Start, const DocCoord& End)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/02/94
	Inputs:		pRender - Render region to draw to
				Coord - the coord that the blob should be centred on
	Purpose:	Draws a line from the Bezier control point to its endpoint

********************************************************************************************/

void Path::DrawControlLine(RenderRegion* pRender, const DocCoord& Start, const DocCoord& End)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Draw a line to the render region
	pRender->SetLineColour(COLOUR_BEZIERLINE);
	pRender->DrawLine( Start, End );
#endif
}

#endif


/********************************************************************************************

>	void Path::ClearSubSelection()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/94
	Purpose:	Goes through the path and sets all the IsSelected flags to FALSE, clearing
				any selected control points in the path

********************************************************************************************/

void Path::ClearSubSelection()
{
	// We need to find all the selected control points in the path and de-select them
	PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);

	for (INT32 i=0; i<UsedSlots; i++)
		Flags[i].IsSelected = FALSE;
} 



/********************************************************************************************

>	void Path::EnsureSelection(BOOL UseStarts)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/1/95
	Inputs:		UseStarts - TRUE if the selection state of the start of subpaths should be used
				the set the ends.  FALSE if the end sel state should be used to set the starts
	Purpose:	Goes through the path ensuring that the selection status of the ends/starts
				of subpaths equal the selection state of the start/ends of that subpath
				Also selects/deselectes control points as appropiate

********************************************************************************************/

void Path::EnsureSelection(BOOL UseStarts)
{
#if !defined(EXCLUDE_FROM_RALPH)
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// Ensure the selection state of the sub paths
	const INT32 NumCoords = GetNumCoords();
	if (UseStarts)
	{
		INT32 Start = 0;
		INT32 End = 0;
		while (End < NumCoords-1)
		{
			FindEndElOfSubPath(&End);
			if (Verbs[End] & PT_CLOSEFIGURE)
				Flags[End].IsSelected = Flags[Start].IsSelected;
			Start = ++End;
		}
	}
	else
	{
		INT32 Close = NumCoords-1;
		INT32 Offset = NumCoords-1;
		while (Offset > -1)
		{
			FindStartOfSubPath(&Offset);
			if (Verbs[Close] & PT_CLOSEFIGURE)
				Flags[Offset].IsSelected = Flags[Close].IsSelected;
			Close = Offset-1;
			Offset = Close;
		}
	}

	// Now set the selection state of the  control points.depending on their endpoints
	for (INT32 loop = 0; loop < NumCoords; loop++)
	{
		if (!Flags[loop].IsEndPoint)
		{
			ERROR3IF(loop==0, "Path started with a non-endpoint!");
			ERROR3IF(Verbs[loop]!=PT_BEZIERTO, "Found an non-endpoint that wasn't a Bezier");
			if (loop > 0)
			{
				if (Flags[loop-1].IsEndPoint)
					Flags[loop].IsSelected = Flags[loop-1].IsSelected;
				else
					Flags[loop].IsSelected = Flags[loop+1].IsSelected;
			}
		}
	}
#endif
}



/********************************************************************************************

>	void Path::SetAllSubSelection()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/1/95
	Purpose:	Goes through the path and sets all the IsSelected flags to TRUE,  also
				selecting all control points in the path

********************************************************************************************/

void Path::SetAllSubSelection()
{
	PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);

	for (INT32 i=0; i<UsedSlots; i++)
		Flags[i].IsSelected = TRUE;
}



/********************************************************************************************

>	BOOL Path::IsSubSelection()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/94
	Returns:	TRUE if any of the paths coords were selected
	Purpose:	determine if the path has a subselection

********************************************************************************************/

BOOL Path::IsSubSelection()
{
	// We need to find all the selected control points in the path and de-select them
	PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);

	// look for a selected coord
	for (INT32 i=0; i<UsedSlots; i++)
	{
		// if we find one then return TRUE
		if (Flags[i].IsSelected)
			return TRUE;
	}

	// we did not find one, so return FALSE
	return FALSE;
}



/********************************************************************************************

>	BOOL Path::FindSelectionLimits(INT32 index, INT32* p, INT32* q)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/94
	Inputs:		index	= search start point
	Outputs:	p		= index of first selected point in region
				q		= index of last selected point in region

	Returns:	TRUE	if the point set {index,....,UsedSlots-1} contains a set of selected 
						control handles {p,...,q} where p and q are the limits of the 
						selection and all points between are selected.
				FALSE 	if no points are selected within the region.
				 
	Purpose:	Finds the next connected region of selected control handles.

********************************************************************************************/

BOOL Path::FindSelectionLimits(INT32 index, INT32* p, INT32* q)
{
	ENSURE( index<UsedSlots, "Path position was not valid in FindSelectionLimits" );
	ENSURE( index>=0, "Path Position less than zero in FindSelectionLimits" );

	INT32 start = index;

	// clamp the index
	if (start<0)
		start=0;
	if (start>=UsedSlots)
		start=UsedSlots-1;

	if (!FindNextSelected(&start))
		return FALSE;

	INT32 end = start;
	INT32 search = start;
		
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	BOOL done = FALSE;

	while (FindNextEndPoint(&search) && (!done))
	{
		if ((Flags[search].IsSelected) && (Flags[search].IsEndPoint))
		{
			if (Verbs[search] != PT_MOVETO)
				end = search;
			else
				done = TRUE;
		}
		else
			done = TRUE;
	}

	*p = start;
	*q = end; 

	return TRUE;
}



/********************************************************************************************

>	void Path::ExpandRange(INT32* start, INT32* end, INT32 num)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/94
	Inputs:		start	= pointer to an index holding the region start
				end		= pointer to an index holding the region end
				num		= number of endpoints to expand the region by 
	Outputs:	start	= will be updated
				end		= will be updated
	Purpose:	given the region [*start, *end] expand the region in both directions
				by num elements. The region will not be expanded beyond its outer limits
				described by the set {moveto,.....,endofsubpath}.
	
********************************************************************************************/

void Path::ExpandRange(INT32* start, INT32* end, INT32 num)
{
	
	ENSURE( (*start)<UsedSlots, "start index was not valid in ExpandRange" );
	ENSURE( (*start)>=0, "start index less than zero in ExpandRange" );

	ENSURE( (*end)<UsedSlots, "end index was not valid in ExpandRange" );
	ENSURE( (*end)>=0, "end index less than zero in ExpandRange" );

	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// clamp the indexes

	INT32 starti = *start;
	INT32 endi = *end;

	if (starti<0) 
		starti = 0;
	if (endi<0)
		endi = 0;

	if (starti >= UsedSlots)
		starti = UsedSlots-1;
	if (endi >= UsedSlots)
		endi = UsedSlots-1;

	INT32 prev = starti;
	INT32 next = endi;

	while (num>0)
	{
		if (Verbs[starti] != PT_MOVETO)
		{ 
			if (FindPrev(&prev))
			{
				if (Flags[prev].IsEndPoint)
					starti = prev;
			}
		}

		if (FindNextEndPoint(&next))
		{	
			if ((Flags[next].IsEndPoint) &&
				(Verbs[next] != PT_MOVETO))
					endi = next;
		}					
		num-=1;
	}
	*start = starti;
	*end = endi;
}




/********************************************************************************************

>	BOOL Path::MakeSpaceInPath(INT32 NumSlots)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/02/94
	Inputs:		NumSlots - The number of slots you need to put your new thing in
	Returns:	TRUE	if it was able to find the memory,
				FALSE	if it failed
	Purpose:	Checks to see if there is already enough memory available to make the
				insertion and if not will try to allocate some more memory.
				If this works it will move the path 
				(after the position indicated by the path class variable CurrentPos)
				along to make room for the correct number of slots at that point.

********************************************************************************************/

BOOL Path::MakeSpaceInPath(INT32 NumSlots)
{
	if (!MakeSpaceAtEnd(NumSlots))
		return FALSE;

	return OpenGap(NumSlots,CurrentPos);
}


/********************************************************************************************

>	BOOL Path::MakeSpaceInPath(INT32 NumSlots, INT32 Position)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/02/94
	Inputs:		NumSlots - The number of slots you need to put your new thing in
				Position - The position at which to	put the slots
	Returns:	TRUE	if it was able to find the memory, 
				FALSE	if it failed
	Purpose:	Checks to see if there is already enough memory available to make the
				insertion and if not will try to allocate some more memory. If this works
				it will move the path after the position indicated by Position along to make
				room for the correct number of slots at that point.

********************************************************************************************/

BOOL Path::MakeSpaceInPath(INT32 NumSlots, INT32 Position)
{
	if (!MakeSpaceAtEnd(NumSlots))
		return FALSE;

	return OpenGap(NumSlots,Position);
}




/********************************************************************************************

>	BOOL Path::MakeSpaceAtEnd(INT32 NumSlots)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	01/02/94
	Inputs:		NumSlots - The number of Unused slots you need free at the end of the path
	Returns:	TRUE 	if it was able to find the memory, 
				FALSE 	if it failed
	Purpose:	Will make enough room for NumSlots unused slots at the end of the path. 

********************************************************************************************/

BOOL Path::MakeSpaceAtEnd(INT32 NumSlots)
{
	ERROR2IF(SlotAllocSize <= 0,FALSE,"MakeSpaceAtEnd has ZERO Slot Allocation Size!");
	if (UnUsedSlots<NumSlots)
	{
		// Find out how many slots to ask for and make sure it is a multiple of 4
		INT32 SlotsNeeded = NumSlots-UnUsedSlots;
			SlotsNeeded = ((INT32)(SlotsNeeded/SlotAllocSize))*SlotAllocSize+SlotAllocSize;
			SlotsNeeded = ((SlotsNeeded+3) >> 2) << 2;

		// we are out of spare slots, so try to alloc some extra ones
		if (!IncreaseBlock(VerbHandle, sizeof(PathVerb)*SlotsNeeded))
			return FALSE;
				
		if (!IncreaseBlock(CoordHandle, sizeof(DocCoord)*SlotsNeeded))
			return FALSE;

		if (!IncreaseBlock(FlagsHandle, sizeof(PathFlags)*SlotsNeeded))
			return FALSE;

		if (ExtraInfo != NULL)
			if (!ExtraInfo->IncreaseExtraBlocks(SlotsNeeded))
				return FALSE;

		// Got the memory back that we asked for, so update a few vars
		UnUsedSlots += SlotsNeeded;
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL Path::OpenGap(INT32 NumSlots)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	01/02/94
	Inputs:		NumSlots - The size of the gap to open
	Returns:	TRUE 	if it was able to find the memory, 
				FALSE 	if it failed
	Purpose:	Will open a gap of NumSlots size in this path. The gap is positioned at
				CurrentPos (A path class variable)

********************************************************************************************/

BOOL Path::OpenGap(INT32 NumSlots)
{
	return OpenGap(NumSlots, CurrentPos);
}

/********************************************************************************************

>	BOOL Path::OpenGap(INT32 NumSlots, INT32 Position)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	01/02/94
	Inputs:		NumSlots = The size of the gap to open
				Position = The index at which to open the gap (0...UsedSlots)
	Returns:	TRUE 	if it was able to open the gap
				FALSE 	if it failed
	Purpose:	Will open up a gap of NumSlots at the position Position within the path.
				If Position is set to the end of the path ie the same value as usedslots, 
				no data will be moved.
				FALSE will ofcourse always be returned if there is not enough free space
				in the form of unused slots on the end of the path for the gap to open.
				Use MakeSpaceInPath() if you know there isn't enough room.
	See Also:	MakeSpaceInPath, MakeSpaceAtEnd

********************************************************************************************/

BOOL Path::OpenGap(INT32 NumSlots, INT32 Position)
{
	if (UnUsedSlots<NumSlots)
	{		   
		ERROR3("Path::OpenGap() overflow: Gap request is too large");
		return FALSE;
	}

	if (Position<0 || Position>UsedSlots)
	{
		ERROR3("Path::OpenGap() called with an illegal gap position");
		return FALSE;
	}

	INT32 SlotsToMove = UsedSlots-Position;

	if (SlotsToMove>0)
	{
		PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
		memmove( (void*)(&Verbs[Position+NumSlots]), (void*)(&Verbs[Position]), SlotsToMove*sizeof(PathVerb) );

		DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
		memmove( (void*)&Coords[Position+NumSlots], (void*)&Coords[Position], SlotsToMove*sizeof(DocCoord) );

		PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
		memmove( (void*)&Flags[Position+NumSlots], (void*)&Flags[Position], SlotsToMove*sizeof(PathFlags) );

		if (ExtraInfo != NULL)
			ExtraInfo->ShiftUpExtraInfo(Position, NumSlots, SlotsToMove);
	}

	return TRUE;
}


/********************************************************************************************

> 	BOOL Path::Compact()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		-
	Returns:	FALSE if the path failed to compact
	Purpose:	Removes any extraneous unused slots in a path. It will a maximum of 
				2n-1 slots free where n is the size of an allocation chunk when increasing
				the buffer (see Path::Initialise)

********************************************************************************************/

BOOL Path::Compact()
{
	// Calculate the buffer reduction factor.
    INT32 reduction = ((UnUsedSlots / SlotAllocSize)-1)*SlotAllocSize;

    if (reduction>0)
	{
		// reduce the number of free slots NOW, incase m out of the n
		// following reductions fail.
		UnUsedSlots -= reduction;

		if ((DecreaseBlock(VerbHandle, sizeof(PathVerb)*reduction)==FALSE) ||
		    (DecreaseBlock(FlagsHandle, sizeof(PathFlags)*reduction)==FALSE) ||
	    	(DecreaseBlock(CoordHandle, sizeof(DocCoord)*reduction)==FALSE) ||
			(ExtraInfo!=NULL && ExtraInfo->DecreaseExtraBlocks(reduction)==FALSE))
		{
			// Memory shrink failed, so pass that back to the person using the Path
			// The path should still be useable at this stage so long as all handles
			// are valid.
			return FALSE;
		}
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL Path::InsertSection(INT32 StartSlot, INT32 NumSlots)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Inputs:		StartSlot is the path slot where we want to insert the section
				NumSlots is how many slots we want to insert
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This routine is the corollary to DeleteSection - it inserts a section
				into the path at a given position and a given size, but does not put
				any data into the path. Note that this routine is likely to move the
				path data around in memory, so after calling it you should re-read any
				pointers to the path you might have had.
	Errors:		-
	SeeAlso:	Path::DeleteSection

********************************************************************************************/

BOOL Path::InsertSection(INT32 StartSlot, INT32 NumSlots)
{
	INT32 OldPosition = CurrentPos;
	CurrentPos = StartSlot;
	if (!MakeSpaceInPath(NumSlots))
	{
		CurrentPos = OldPosition;
		return FALSE;
	}
	CurrentPos = OldPosition;
	UsedSlots+=NumSlots;
	UnUsedSlots -= NumSlots;
	return TRUE;
}


/********************************************************************************************

>	BOOL Path::EnsureVolume(INT32 NumSlots)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/02/95
	Inputs:		NumSlots - The minimum number of slots you need in the path arrays
	Returns:	TRUE if it was able to find the memory, FALSE if it failed
	Purpose:	This function ensures there are >= NumSlots in the path arrays.
				It may try to alter the size of the path arrays by increasing the number
				of unused slots to satisfy the request. It will never decrease the size of
				the arrays, if NumSlots < UsedSlots+UnusedSlots then the volume is
				automatically ensured. This allows path buffers to be resized ready for
				cloning.
				
********************************************************************************************/

BOOL Path::EnsureVolume(INT32 NumSlots)
{
	INT32 Slots=NumSlots-UsedSlots;
	if (Slots>0)
		return MakeSpaceAtEnd(Slots);
	return TRUE;
}


/********************************************************************************************

>	void Path::SmoothCurve(	BOOL SetRenderFlags = FALSE, 
							BOOL SnapEnds = FALSE, 
							INT32 SnapIndex = 0)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Inputs:		SetRenderFlag is TRUE if the smoothing should set the NeedToRender flags on points
				as it's smoothing.
				SnapEnds tells the routine to snap two open ends together
				SnapIndex is an index into the subpath which is to be snapped
	Purpose:	Smooths the path on the areas that have the IsSmooth flag set to TRUE.
				This can change any or all of the path.

********************************************************************************************/

void Path::SmoothCurve(BOOL SetRenderFlags, BOOL SnapEnds, INT32 SnapIndex)
{

	// This is a less efficient (slightly) version of this routine (in that there is a 
	// certain amount of moving backward and forward in the path, but only for the start
	// and end points) but it copes with end snapping

	DocCoord* Coords = GetCoordArray();
	PathVerb* Verbs = GetVerbArray();
	PathFlags* Flags = GetFlagArray();
	INT32 StartSnapSub = 0;
	INT32 EndSnapSub = 0;		// Start and end of the subpath we're snapping
	DocCoord tempcoord(0,0);
	
	// Find the start and end of the subpath we're snapping
	if (SnapEnds)
	{
		StartSnapSub = EndSnapSub = SnapIndex;
		FindStartOfSubPath(&StartSnapSub);
		FindEndElOfSubPath(&EndSnapSub);
	}
	
	if (SetRenderFlags)
	{
		// As well as smoothing, we also set the NeedToRender flag on any path that's changed.
		// Notice that we never clear the NeedToRender flags - if they're rendered once, they're
		// rendered forever.

		for (INT32 i = 0; i < UsedSlots; i++)
		{
			if (Verbs[i] == PT_BEZIERTO && !(Flags[i].IsEndPoint) && Flags[i].IsSmooth)
			{
				tempcoord = SmoothControlPoint(i, (SnapEnds && (i>=StartSnapSub && i<=EndSnapSub)));
				if (Coords[i] != tempcoord)
				{
					if (Flags[i+1].IsEndPoint)
						Flags[i+1].NeedToRender = TRUE;
					else
						Flags[i+2].NeedToRender = TRUE;
				}
				Coords[i] = tempcoord;
			}
		}

	}
	else
	{
		for (INT32 i = 0; i < UsedSlots; i++)
		{
			if (Verbs[i] == PT_BEZIERTO && !(Flags[i].IsEndPoint) && Flags[i].IsSmooth)
				Coords[i] = SmoothControlPoint(i, (SnapEnds && (i>=StartSnapSub && i<=EndSnapSub)));
		}
	}
}


/*
{
	// these are used to remember the position of the last endpoint and the one before that

	INT32 LastEndPoint = 0;
	INT32 LastLastEndPoint = 0;

	// get pointers to the arrays and find the number of coords
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	for (INT32 i=0; i<UsedSlots; i++)
	{
		if (Flags[i].IsEndPoint)
		{
			if (Flags[LastEndPoint].IsSmooth)
			{
				// Find out the most recent verbs
				PathVerb LastVerb = Verbs[LastEndPoint] & (~PT_CLOSEFIGURE);
				PathVerb ThisVerb = Verbs[i] & (~PT_CLOSEFIGURE);

				// The Previous point had the smooth flag on, so we had better deal with it
				switch (LastVerb)
				{
					case PT_MOVETO :
						if (ThisVerb == PT_BEZIERTO)
							CalcPointEnd( Coords[i], Coords[LastEndPoint], &Coords[i-2] );
						break;

					case PT_LINETO :
						if (ThisVerb == PT_BEZIERTO)
							CalcPointLine( Coords[i], Coords[LastEndPoint], Coords[LastLastEndPoint], &Coords[i-2] ); 
						break;

					case PT_BEZIERTO :
						if (ThisVerb == PT_BEZIERTO)
							CalcDoubleCurve( Coords[LastLastEndPoint], Coords[LastEndPoint], Coords[i], &Coords[LastEndPoint-1], &Coords[i-2] );

						if (ThisVerb == PT_LINETO)
							CalcPointLine( Coords[LastLastEndPoint], Coords[LastEndPoint], Coords[i], &Coords[LastEndPoint-1] );

						if (ThisVerb == PT_MOVETO)
							CalcPointEnd( Coords[LastLastEndPoint], Coords[LastEndPoint], &Coords[LastEndPoint-1] );

						break;
						
					default:
						break;
				}
			}

			// Get all the past coords worked out
			LastLastEndPoint = LastEndPoint;
			LastEndPoint = i;
		}
	}



}

*/


/********************************************************************************************

>	void Path::SmartSmoothCurve(BOOL StartAndEndSnapped,
								BOOL JoinedToAnother,
								Path* OtherPath,
								BOOL JoinStartOfThis,
								BOOL JoinStartOfOther,
								BOOL RecordUndo,
								NodePath* UndoPath,
								Operation* UndoOperation)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		StartAndEndSnapped	TRUE if this path's start and end points should snap
				JoinedToAnother		Flag which tells me if an endpoint on this path has
									temporarily been joined to another path
				OtherPath			This is the path it's joined to if so
				JoinStartOfThis		TRUE if it's the start of this path that's joined
				JoinStartofOther	TRUE if it's the start of the other that's joined
				RecordUndo			TRUE if undo information should be recorded
				UndoPath			The NodePath that this path belongs to (undo purposes)
				UndoOperation		Currently executing operation (undo purposes)
	Outputs:	-
	Returns:	-
	Purpose:	This function will handle the smoothing of a path. It is designed to take
				account of all the possibilities that might occur (hence the huge list of 
				parameters). The various places where this function might be needed are:

				*	While dragging around points on a single path - this uses a temporary 
					path and does not need to worry about undo. The only possibility for 
					strangeness here is that the start and end points might snap together.

				*	While dragging around points on more than one path - not only might start 
					and end snap, but the dragged point might need to snap to other paths 
					being dragged at the same time

				*	When operating directly on a NodePath (for example when toggling the 
					smoothness of a point). This is when Undo needs to be worried about. 
					In this case it is likely that all the other flags will be default values.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/*
void Path::SmartSmoothCurve(BOOL StartAndEndSnapped,
							BOOL JoinedToAnother,
							Path* OtherPath,
							BOOL JoinStartOfThis,
							BOOL JoinStartOfOther,
							BOOL RecordUndo,
							NodePath* UndoPath,
							Operation* UndoOperation)
{
	INT32 NumPointsChanged = 0;				// If we're undoing, we need to count
}

*/

/********************************************************************************************

>	DocCoord Path::SmoothControlPoint(	INT32 Index,
										BOOL StartAndEndSnapped,
										BOOL JoinedToAnother,
										BOOL OtherIsCurve,
										const DocCoord& OtherCoord)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		Index				Index into this path of the control point
				StartEndSnapped		TRUE if the start of the path snaps to the end
				JoinedToAnother		TRUE if this path joins to another one
				OtherIsCurve		TRUE if the other path element is a curve
									FALSE if other path element is a line
				OtherCoord			Coordinate of element in other path

	Outputs:	-
	Returns:	Coordinates of the smoothed point
	Purpose:	Calculates the smoothed position of a control point in a path. The smoothing
				is calculated based on the endpoint to which the control point is attached, 
				and the endpoints on either side of this point. Since we are guaranteed that 
				one of these segments is a curve (that's where the control point comes from)
				the other possibilities are: The curve just terminates (at the end of a path);
				The curve is attached to another curve; It is attached to a line; It is at the end, 
				but the end joins the start of the same path, which could be a line or a curve; 
				and the curve could be at the end, but snapping to a completely separate path segment
				which might either be a line or curve (this is what the JoinedToAnother, OtherIsCurve and
				OtherCoord parameters indicate.

	Errors:		-
	SeeAlso:	Path::SmartSmoothCurve
	Documentation:	Implemnt\smooth.doc

********************************************************************************************/

DocCoord Path::SmoothControlPoint(	INT32 Index,
									BOOL StartAndEndSnapped,
									BOOL JoinedToAnother,
									BOOL OtherIsCurve,
									const DocCoord& OtherCoord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	PathVerb* Verbs = GetVerbArray();
	DocCoord* Coords = GetCoordArray();
	PathFlags* Flags = GetFlagArray();

	// temp vars for calculating smoothness
	
	DocCoord p;		// Endpoint opposite the control point
	DocCoord q;		// Endpoint directly adjacent to control point
	DocCoord r;		// Endpoint on same side of control point but not adjacent
	DocCoord ret;	// Temp for returning value

	// We get the parameters for the calculation from different places depending on whether we're
	// at the first or second control point. We know this by looking at the EndPoint flags

	if (Flags[Index-1].IsEndPoint)
	{
		// This is the first control point of a curve

/*

	These are the possibilities with the control point as the first one in the curve:

	1: Curve/Curve
	...
	m/l/c	p
	c		a
	c		b
	c		q	<- endpoint to which point is attached
	c		c	<- Control point we're smoothing
	c		d
	c		r	
	...

	2: Line/Curve
	...
	m/l/c	p
	l		q
	c		a	<- Control point we're smoothing
	c		b
	c		r
	...

	3: Curve/Open line
	m		q
	c		a	<- control point we're smoothing
	c		b
	c		r
	...

	4: Curve/snapping to curve at end
	m		q
	c		a	<- control point we're snapping
	c		b
	c		r
	...
	l/c		p
	c		c
	c		d
	c		q

	5: Curve/snapping to line at end
	m		q
	c		a	<- control point we're snapping to
	c		b
	c		r
	...
	l/c		p
	l		q

	6: Curve/snapping to curve in another path
	7: Curve/snapping to line in another path
	m		q
	c		a	<- control point we're smoothing
	c		b
	c		r
	...

	point p = OtherCoord
*/

		// The position of this point depends on: The previous endpoint (which is the point
		// to which this control point is 'attached'), the endpoint before that one, and the 
		// endpoint of the bezier segment containing the control point.
		// point q is the endpoint of the previous element (which always exists where we want it)

		q = Coords[Index-1];

		// point r is the endpoint of this curve element - again, always there

		r = Coords[Index+2];

		// point p, on the other hand, is problematical. It might not exist at all (if the 
		// previous element was a MoveTo and the path is open). It might be a lineto. It 
		// might be a CurveTo element, which is the simplest case. Or, it might be any of 
		// those three elements on a completely different path. Or it might be a curve or 
		// a line at the opposite end of the path (if the CLOSEFIGURE flag is set or the 
		// StartAndEndSnapped flag is TRUE).

		if (Verbs[Index-1] == PT_BEZIERTO)
		{
			// Simple - p = the start of this curve (case 1)
			p = Coords[Index-4];
			CalcPointCurve(p,q,r,&ret);

			// Bodge inserted by Peter to fix the problem that occurs when you have a closed path
			// consisting of just two Bezier segments.  In this case the returned position is wrong
			// - it positions the control point over the opposite one - so we will just flip it around
			/* Commented out by Markn 17/1/95 - replaced by new bodge written by Peter
			if ((GetNumCoords() == 7) && StartAndEndSnapped)
			{
				ret = q - (ret - q);
			}
			*/

			// New version of above bodge also by Peter (put in by Markn - 17/1/95)
			if ((GetNumCoords() == 7) && ((Verbs[GetNumCoords()-1] & PT_CLOSEFIGURE) || StartAndEndSnapped))
			{
				ret = q - (ret - q);
			}
			return ret;
		}
		else if	(Verbs[Index-1] == PT_LINETO)
		{
			// Simple - previous is a lineto, so get its start point and calculate (case 2)
			p = Coords[Index-2];
			CalcPointLine(p,q,r,&ret);
			return ret;
		}
		else
		{
			
			// Scan along to find the end element. This we find by scanning forward until
			// we find a MoveTo or we go off the end. Either way, the previous element is
			// the one we want.

			INT32 i = Index;
			while (i<UsedSlots && Verbs[i] != PT_MOVETO)
				i++;
			
			// i either points after the end of the path, or at the next MoveTo. Move to the
			// previous element (could be the endpoint of a curve).

			i--;
			
			if (StartAndEndSnapped || (Verbs[i] & PT_CLOSEFIGURE) )
			{
				// We have to get p from the end of the subpath (cases 4 & 5)
				if ((Verbs[i] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
				{
					// Case 4
					p = Coords[i-3];
					CalcPointCurve(p,q,r,&ret);

					// Bodge inserted by Peter to fix the problem that occurs when you have a path consisting
					// of just two Bezier segments.  In this case the returned position is wrong - it
					// positions the control point over the opposite one - so we will just flip it around
					if (GetNumCoords() == 7)
					{
						ret = q - (ret - q);
					}

					return ret;
				}
				else
				{
					// Case 5
					p = Coords[i-1];
					CalcPointLine(p,q,r,&ret);
					return ret;
				}
			}
			else
			{
				// This path hasn't snapped, and isn't closed. Check if it snaps with another
				if (JoinedToAnother)
				{
					// Cases 6 & 7
					p = OtherCoord;
					if (OtherIsCurve)
					{
						// Case 6
						CalcPointCurve(p,q,r,&ret);
					}
					else
					{
						// Case 7
						CalcPointLine(p,q,r,&ret);
					}
					return ret;
				}
				else
				{
					// There's no snapping at all, so this point has to be calculated as an endpoint
					CalcPointEnd(q,r,&ret);
					return ret;
				}
			}
		}
	}
	else
	{
		// This is the second control point of a curve

/*
	These are the possible situations, with sections of path data

	1: Curve/Curve
		...
		m/l/c	r
		c		a
		c		b	<- Control point pointed to by Index
		c		q	<- this is the endpoint to which the control point is connected
		c 		c
		c		d
		c		p
		...

	2: Curve/Line
		...
		m/l/c	r
		c		a
		c		b	<- Control point pointed to by Index
		c		q	<- this is the endpoint to which the control point is connected
		l		p
		...

	3: Curve/open line
		...
		m/l/c	r
		c		a
		c		b	<- Control point pointed to by Index
		c		q	<- this is the endpoint to which the control point is connected
		either nothing or a moveto
		...

	4: Curve/snapping to curve at start
		m		q
		c		c
		c		d
		c		p
		...
		l/c		r
		c		a
		c		b	<- Control point pointed to by index
		c		q	<- Enpoint to which control is connected
		either nothing or moveto, CLOSEFIGURE or SnapToEnd set
		...

	5: Curve/snapping to line at start
		m		q
		l		p
		...
		l/c		r
		c		a
		c		b	<- Control point pointed to by index
		c		q	<- Enpoint to which control is connected
		either nothing or moveto, CLOSEFIGURE or SnapToEnd set
		...

	6: Curve/snapping to curve in another path
	7: Curve/snapping to line in another path
		...
		m/l/c	r
		c		a
		c		b	<- Control point pointed to by index
		c		q	<- Enpoint to which control is connected
		nothing/moveto

		point p = OtherCoord

*/

		// Point q is the endpoint of this curve

		q = Coords[Index+1];

		// Point r is the start point of this curve (always guaranteed to be there)

		r = Coords[Index-2];

		// Point p (as before) is difficult. It ight be a curve element. it might be a line
		// element. It might not exist at all (i.e. this might be the end point of an open subpath).
		// It also might be the start point of this subpath, either because this path is closed
		// or because we've been told to snap. Or even, it might be an element at the start or end 
		// of another path. My God, what a choice!

		// First check for the simple cases of curve/curve or curve/line (1 & 2)
		if (Index+2<UsedSlots && Verbs[Index+2] != PT_MOVETO)
		{
			// The next element is either a curve or a line
			if (Verbs[Index+2] == PT_BEZIERTO)
			{
				// It's a curve, so point p is its endpoint
				p = Coords[Index+4];
				CalcPointCurve(p,q,r,&ret);
				return ret;
			}
			{
				// It's a line, so point p is the endpoint of that
				p = Coords[Index+2];
				CalcPointLine(p,q,r,&ret);
				return ret;
			}
		}
		// Now we know there's nothing after this curve, so check snapping and stuff
		else if (StartAndEndSnapped || (Verbs[Index+1] & PT_CLOSEFIGURE))
		{
			// We're snapping to the start of this path (4 & 5) , so we have to find 
			// the start of this subpath
			INT32 i = Index;
			FindStartOfSubPath(&i);			// i returned pointing at start element
			i++;							// Point at first element in path
			if (Verbs[i] == PT_BEZIERTO)
			{
				p = Coords[i+2];
				CalcPointCurve(p,q,r,&ret);
				return ret;
			}
			else
			{
				p = Coords[i];
				CalcPointLine(p,q,r,&ret);
				return ret;
			}

		}
		else if (JoinedToAnother)
		{
			// Joined to another path (6 & 7)
			p = OtherCoord;
			if (OtherIsCurve)
			{
				CalcPointCurve(p,q,r,&ret);
			}
			else
			{
				CalcPointLine(p,q,r,&ret);
			}
			return ret;
		}
		else
		{
			// The only thing left is a curve at the end of the path (3)
			CalcPointEnd(q,r,&ret);
			return ret;
		}
	}
#else
	return DocCoord(0,0);
#endif
}


/********************************************************************************************

>	BOOL Path::IsSubPathClosed(INT32 Index)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94
	Inputs:		Index is the index of a point in the subpath
	Outputs:	-
	Returns:	TRUE if subpath is closed, FALSE otherwise
	Purpose:	Scans to the end of this subpath to see if it has the PT_CLOSEFIGURE flag set
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Path::IsSubPathClosed(INT32 Index)
{
	INT32 i = Index;
	PathVerb* Verbs = GetVerbArray();
	
	// skip through until we hit the end or a MoveTo
	do
	{
		i++;
	} while (i<UsedSlots && Verbs[i] != PT_MOVETO);

	// i points to the firsy element after this subpath (i.e. the top of the subpath)
	// Check the CloseFigure flag on the previous element to see if the subpath is closed
	if (Verbs[i-1] & PT_CLOSEFIGURE)
		return TRUE;
	return FALSE;
}



/********************************************************************************************

>	BOOL Path::IsClosed()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05 April 2000
	Returns:	TRUE if all subpaths are cloesd,
				FALSE otherwise.
	Purpose:	Tells you whether all subpaths of this path are closed.

	Notes:		This code taken from BevelHelpers::IsPathClosed() - seems more appropriate
				for it to live here.

	See also:	IsSubPathClosed

********************************************************************************************/
BOOL Path::IsClosed()
{
	INT32 StartIndex	= 0;
	INT32 EndIndex	= 0;

	// test each subpath for closure.
	while (StartIndex < UsedSlots)
	{
		EndIndex = StartIndex;
		FindEndElOfSubPath(&EndIndex);

		if (!IsSubPathClosed(StartIndex))
			return FALSE;
		
		StartIndex = EndIndex + 1;
	}

	return TRUE;
}



/********************************************************************************************

>	void Path::CalcDoubleCurve( DocCoord& p1, DocCoord& p2, DocCoord& p3, 
								DocCoord* ControlPoint1, DocCoord* ControlPoint2 )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Inputs:		p1, p2, p3 - The End points of the path elements before, after and at the
				the current position in the path
	Outputs:	ControlPoint1, ControlPoint2 - Pointers to the control points on either
				side of the current point
	Purpose:	Calculates the position of the two control points on either side of p2,
				using the position of the coords of either side of it to help (p1 & p3)
	SeeAlso:	Path::CalcPointLine; Path::CalcPointEnd

********************************************************************************************/

void Path::CalcDoubleCurve( DocCoord& p1, DocCoord& p2, DocCoord& p3, DocCoord* ControlPoint1, DocCoord* ControlPoint2 )
{
	// The positions of the two control points adjacent to p2 depend on:
	// a) The distance from p2 of p1 and p3, and
	// b) The angle between p1p2p3. The following logic is used:
	//
	// The distance from p2 to ControlPoint1 is 1/3 the distance from p2 to p1
	// The angle from p2 to ControlPoint1 and the horizontal is 90 degrees from the angle of the line
	// bisecting the angle p1p2p3. Whether it is -90 or +90 depends on whether the acute angle between p1p2p3
	// is negative or positive


	// Work out the angles
	double Alpha, Beta, Theta;
	Alpha = atan2((double)p3.y-p2.y, (double)p3.x-p2.x);			// Alpha = angle between line p2p3 and horizontal
	Beta = atan2((double)p1.y-p2.y, (double)p1.x-p2.x);				// beta = angle between line p2p1 and horizontal
	Theta = (Beta + ((Alpha - Beta)/2.0));			// Theta = bisection of Alpha and Beta

	// And calculate the distance between the various control points
	double Distp2p1, Distp2p3, lx, ly;
	lx = p2.x - p1.x;
	ly = p2.y - p1.y;
	Distp2p1 = sqrt( lx*lx + ly*ly ) / 3.0;

	lx = p2.x - p3.x;
	ly = p2.y - p3.y;
	Distp2p3 = sqrt( lx*lx + ly*ly ) / 3.0;

	if (Alpha - Beta < 0)
	{
		// Angle p2p1 is greater than p2p3, so we have to swap the angle used

		ControlPoint1->x = p2.x + (INT32)(Distp2p1 * cos(Theta + PI/2.0));
		ControlPoint1->y = p2.y + (INT32)(Distp2p1 * sin(Theta + PI/2.0));
		ControlPoint2->x = p2.x + (INT32)(Distp2p3 * cos(Theta - PI/2.0));
		ControlPoint2->y = p2.y + (INT32)(Distp2p3 * sin(Theta - PI/2.0));

	}
	else
	{
		ControlPoint1->x = p2.x + (INT32)(Distp2p1 * cos(Theta - PI/2.0));
		ControlPoint1->y = p2.y + (INT32)(Distp2p1 * sin(Theta - PI/2.0));
		ControlPoint2->x = p2.x + (INT32)(Distp2p3 * cos(Theta + PI/2.0));
		ControlPoint2->y = p2.y + (INT32)(Distp2p3 * sin(Theta + PI/2.0));

	}


//	// Work out all the angles
//	double Alpha, Beta, Theta;
//	Alpha = atan2( (p3.y-p2.y), (p3.x - p2.x) );
//	Beta  = atan2( (p2.x - p1.x), (p2.y - p1.y) );
//	Theta = (PI/2.0 - Alpha - Beta) / 2.0;

//	// And calculate the distance between the various control points
//	double DistA, DistB, lx, ly;
//	lx = p2.x - p1.x;
//	ly = p2.y - p1.y;
//	DistA = sqrt( lx*lx + ly*ly ) / 3.0;

//	lx = p2.x - p3.x;
//	ly = p2.y - p3.y;
//	DistB = sqrt( lx*lx + ly*ly ) / 3.0;
//
//	if (Theta < PI/2.0)
//	{
  //
//	// Set the control point
//	ControlPoint1->x = p2.x - (INT32)(DistA*sin(Theta+Beta));
//	ControlPoint1->y = p2.y - (INT32)(DistA*cos(Theta+Beta));

//	ControlPoint2->x = p2.x + (INT32)(DistB*cos(Theta+Alpha));
//	ControlPoint2->y = p2.y + (INT32)(DistB*sin(Theta+Alpha));
	
//	}
//	else
//	{

//	// Set the control point
//	ControlPoint1->x = p2.x + (INT32)(DistA*sin(Theta+Beta));
//	ControlPoint1->y = p2.y + (INT32)(DistA*cos(Theta+Beta));

//	ControlPoint2->x = p2.x - (INT32)(DistB*cos(Theta+Alpha));
//	ControlPoint2->y = p2.y - (INT32)(DistB*sin(Theta+Alpha));
	
//
//	}

}

/********************************************************************************************

>	void Path::CalcPointCurve( DocCoord& p1, DocCoord& p2, DocCoord& p3, DocCoord* ControlPoint )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Inputs:		p1, p2, p3 - The End points of the path elements before, after and at the
				the current position in the path
	Outputs:	ControlPoint - Pointer to the control point that appears just after p2.
	Purpose:	Calculates the position of the control point adjacent to p2 between p2 and p3,
				using the position of the coords of either side of it to help (p1 & p3).

				imagine the following path structure:
				moveto	p1
				curveto	pa
				curveto	pb
				curveto	p2	<- This is the endpoint to which the ctrl point is attached
				curveto	pc	<- This is the control point being calculated
				curveto	pd
				curveto	p3


	SeeAlso:	Path::CalcPointLine; Path::CalcPointEnd

********************************************************************************************/

void Path::CalcPointCurve( DocCoord& p1, DocCoord& p2, DocCoord& p3, DocCoord* ControlPoint )
{
	// Work out all the angles
	double Alpha, Beta, Theta;
	Alpha = atan2((double)p3.y-p2.y, (double)p3.x-p2.x);
	Beta = atan2((double)p1.y-p2.y, (double)p1.x-p2.x);
	Theta = Beta - ((Beta - Alpha)/2.0);


	// And calculate the distance between the various control points
	double Dist, lx, ly;
	lx = p2.x - p3.x;
	ly = p2.y - p3.y;
	Dist = sqrt( lx*lx + ly*ly ) / 3.0;

	if (Beta - Alpha < 0)
		Theta += PI/2.0;
	else
		Theta -= PI/2.0;

	ControlPoint->x = p2.x + (INT32)(Dist * cos(Theta));
	ControlPoint->y = p2.y + (INT32)(Dist * sin(Theta));

/*
	if (Theta < PI/2.0)
	{

		// Set the control point
		ControlPoint->x = p2.x - (INT32)(Dist*sin(Theta+Beta));
		ControlPoint->y = p2.y - (INT32)(Dist*cos(Theta+Beta));
	}
	else
	{

		// Set the control point
		ControlPoint->x = p2.x + (INT32)(Dist*sin(Theta+Beta));
		ControlPoint->y = p2.y + (INT32)(Dist*cos(Theta+Beta));
		
	}
*/

}

/********************************************************************************************

>	void Path::CalcPointLine( DocCoord& p1, DocCoord& p2, DocCoord& p3, DocCoord* ControlPoint )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Inputs:		p1, p2, p3 - The End points of the path elements before, after and at the
				the current position in the path
	Outputs:	ControlPoint - Pointers to the control point between p2 and p3
	Purpose:	Calculates the position of the control point that appears after p2, which
				is part of the curve ending at p3. The control point should be colinear
				with p1 & p2 and 1/3 the distance between p2 & p3 from p2!
	SeeAlso:	Path::CalcPointLine; Path::CalcPointEnd

********************************************************************************************/

void Path::CalcPointLine( DocCoord& p1, DocCoord& p2, DocCoord& p3, DocCoord* ControlPoint )
{
	// Calc 1/3 the distance from p2 to p3
	double DistA, DistB, lx, ly;
	lx = p2.x - p3.x;
	ly = p2.y - p3.y;
	DistA = sqrt( lx*lx + ly*ly ) / 3.0;

	// calc the length of p1 to p2
	lx = p2.x - p1.x;
	ly = p2.y - p1.y;
	DistB = sqrt( lx*lx + ly*ly );

	// Calculate the scale factor
	double Factor = (DistB==0.0) ? 1.0 : (DistA / DistB);
	lx *= Factor;
	ly *= Factor;

	// Calc the position of the new control points
	ControlPoint->x = p2.x + (INT32)lx;
	ControlPoint->y = p2.y + (INT32)ly;
}


/********************************************************************************************

>	void Path::CalcPointEnd( DocCoord& p1, DocCoord& p2, DocCoord* ControlPoint )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Inputs:		p1, p2 - The coords of the last two points in the curve
	Outputs:	ControlPoint - The Coord of the last control point
	Purpose:	Calculates the position of the last control point in the sub-path
	SeeAlso:	Path::CalcPointLine; Path::CalcPointCurve

********************************************************************************************/

void Path::CalcPointEnd( DocCoord& p1, DocCoord& p2, DocCoord* ControlPoint )
{
	ControlPoint->x = p1.x + ((p2.x - p1.x) / 3);
	ControlPoint->y = p1.y + ((p2.y - p1.y) / 3);
}





/********************************************************************************************

>	void Path::CalcRotate( DocCoord& p2, DocCoord* p1, DocCoord* p3, DocCoord& MouseOffset )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/93
	Inputs:		p1, p2, p3 - The End points of the path elements before, after and at the
				the current position in the path
	Outputs:	ControlPoint - Pointers to the control point between p2 and p3
	Purpose:	
	SeeAlso:	Path::CalcPointLine; Path::CalcPointEnd

********************************************************************************************/

void Path::CalcRotate( DocCoord& p2, DocCoord* p1, DocCoord* p3, DocCoord& MouseOffset )
{
	double dx, dy, Length;

	// Find the length of the vector from the middle to the point not being dragged
	dx = p2.x - p3->x;
	dy = p2.y - p3->y;
	Length = sqrt( dx*dx + dy*dy );

	// Move the point being dragged
	p1->x += MouseOffset.x;
	p1->y += MouseOffset.y;

	// Calculate the vector from the point being dragged to the centre
	double VecLen;
	dx = p1->x - p2.x;
	dy = p1->y - p2.y;
	VecLen = sqrt( dx*dx + dy*dy );
	if (VecLen == 0.0)
		// the dragged point is over the centre point, so leave the other control point where it is
		return;

	// Normalise the vector and
	// Scale the vector by the Length of the other side
	dx = dx * Length / VecLen;
	dy = dy * Length / VecLen;

	// Set the opposite point to the centre offset by the vector
	p3->x = p2.x - (INT32)dx;
	p3->y = p2.y - (INT32)dy;
}




/********************************************************************************************

>	void Path::Reverse()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/94
	Purpose:	Reverses this path, so that it is rendered and traversed etc in the other
				direction. This involves reversing the order of all the coords etc in the
				path

********************************************************************************************/

void Path::Reverse()
{
	ReverseSection(0, UsedSlots);
}


/********************************************************************************************

>	void Path::ReverseSection(INT32 StartSlot)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/94
	Inputs:		StartSlot - The slot in the path where the sub path begins
	Purpose:	Reverses the direction of the section of the path specified. This is
				typically used to reverse subpaths of a complex path. It is also called
				by Reverse, to reverse the entire path
	Errors:		ENSUREs if StartSlot is not at the start of a subpath

********************************************************************************************/

void Path::ReverseSection(INT32 FirstPos, INT32 SubPathLen)
{
	// We will need to change all the arrays
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// First we will reverse all the Coords and their flags
	INT32 LastPos  = FirstPos+SubPathLen-1;
	INT32 Start = FirstPos;
	DocCoord  SwapCoord;
	PathFlags SwapFlag;
	while (FirstPos<LastPos)
	{
		// change the order of the coords in the array
		SwapCoord = Coords[FirstPos];
		Coords[FirstPos] = Coords[LastPos];
		Coords[LastPos]  = SwapCoord;

		// Change the order of the flags associated with the coord
		SwapFlag = Flags[FirstPos];
		Flags[FirstPos] = Flags[LastPos];
		Flags[LastPos]  = SwapFlag;

		// find the next two coords/flags to swap
		FirstPos++;
		LastPos--;
	}

	// Now reverse the verbs.
	// The first verb always stays as a MoveTo and everything else gets sorted around it
	FirstPos = Start+1;
	LastPos  = Start+SubPathLen-1;
	PathVerb SwapVerb;
	while (FirstPos<LastPos)
	{
		SwapVerb = Verbs[FirstPos];
		Verbs[FirstPos] = Verbs[LastPos];
		Verbs[LastPos]  = SwapVerb;

		// Find the next two verbs to swap
		FirstPos++;
		LastPos--;
	}

	// added by Ed 23/5/95 - fix up screwed up closed paths!
	// ie first element of subpath has PT_CLOSEFIGURE rather than last!
	INT32 StartOfSubPath     = Start;
	INT32 EndElOfLastSubPath = Start+SubPathLen-1;

#ifdef _DEBUG
	// check assumption that we are reversing a set of complete subpaths
	INT32 temp=StartOfSubPath;
	FindStartOfSubPath(&temp);
	ERROR3IF(StartOfSubPath!=temp,"Path::ReverseSection() - section does not start at the start of a subpath");
	temp=EndElOfLastSubPath-1;
	FindEndElOfSubPath(&temp);
	ERROR3IF(EndElOfLastSubPath!=temp,"Path::ReverseSection() - section does not end at the end of a subpath");
#endif

	// for each subpath, if the second element has an erroneous PT_CLOSEFIGURE,
	// move it to the end element of the subpath
	while (StartOfSubPath<=EndElOfLastSubPath)
	{
		INT32 EndElOfSubPath=StartOfSubPath;
		FindEndElOfSubPath(&EndElOfSubPath);
		if (Verbs[StartOfSubPath+1] & PT_CLOSEFIGURE)
		{
			Verbs[StartOfSubPath+1]&=~PT_CLOSEFIGURE;
			Verbs[EndElOfSubPath]  |= PT_CLOSEFIGURE;
		}
		StartOfSubPath=EndElOfSubPath+1;
	}

	return;
}



#if 0
/********************************************************************************************

>	void Path::Join(const Path& Other, BOOL BuildComplexPath = TRUE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/94
	Inputs:		Other - the path to Join with this path
				BuildComplexPath - Defaults to TRUE. This determines if the first MoveTo of
				the 'Other' path is copied across. By default it is.
	Purpose:	Joins the other path to this path to make a complex path. If BuildComplexPath
				is FALSE then the first MoveTo is thrown away (it assumes that the MoveTo
				has the same coord as the end point of 'this' path).

********************************************************************************************/

BOOL Path::Join(const Path& Other, BOOL BuildComplexPath )
{
	// Make space to copy all the data from the other path into this one
	CurrentPos = UsedSlots;
	if (MakeSpaceInPath(Other.UsedSlots) == FALSE)
		// Failed to get extra memory
		return FALSE;

	// We will need all the arrays for both paths
	DocCoord*  Coords = (DocCoord*)  DescribeHandle(CoordHandle);
	PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb*  Verbs  = (PathVerb*)  DescribeHandle(VerbHandle);
	
	DocCoord*  OtherCoords = (DocCoord*)  DescribeHandle(Other.CoordHandle);
	PathFlags* OtherFlags  = (PathFlags*) DescribeHandle(Other.FlagsHandle);
	PathVerb*  OtherVerbs  = (PathVerb*)  DescribeHandle(Other.VerbHandle);

	// Copy all the data, skipping the first verb (MoveTo)
	INT32 DestPos = UsedSlots;
	INT32 StartCopyFrom = (BuildComplexPath ? 0 : 1);
	for (INT32 i=StartCopyFrom; i<Other.UsedSlots; i++)
	{
		// Copy this coord and its associated data
		Coords[DestPos] = OtherCoords[i];
		Flags[DestPos]  = OtherFlags[i];
		Verbs[DestPos]  = OtherVerbs[i];

		// move to the next position
		DestPos++;
		UsedSlots++;
		UnUsedSlots--;
	}

	// Update the paths info about itself
	if (BuildComplexPath==FALSE)
	{
		// Find the coord of the last element in the path
		CurrentPos = UsedSlots-1;
		DocCoord EndPoint = Coords[CurrentPos];

		// Find the coord of the first element in the sub path
		FindStartOfSubPath();
		DocCoord StartPoint = Coords[CurrentPos];

		// if they are the same, then close the path
		if (EndPoint == StartPoint)
		{
			// Make the path closed and mark it as filled
			Verbs[UsedSlots-1] |= PT_CLOSEFIGURE;
			IsFilled = TRUE;
		}
	}

	// and say that it worked
	return TRUE;
}

#endif

/********************************************************************************************

>	void Path::TryToClose()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/94
	Purpose:	Tries to close the path. If the start coord and the end coord are the same,
				it will close the path with a PT_CLOSEFIGURE and set the IsFilled flag. It
				searches all the sub-paths in the path and if any of them are closed, the
				whole path is marked as filled.

********************************************************************************************/

void Path::TryToClose()
{
	// Dereference the pointers to the coords and the verbs
	DocCoord*  Coords = (DocCoord*)  DescribeHandle(CoordHandle);
	PathVerb*  Verbs  = (PathVerb*)  DescribeHandle(VerbHandle);

	// Need to try all the sub paths, so loop through the whoel path
	INT32 EndOfSubPath = UsedSlots-1;
	for (INT32 i=UsedSlots-1; i>=0; i--)
	{
		// Have we found the begining of a sub-path
		if (Verbs[i]==PT_MOVETO)
		{
			// yes - is its start and end coords the same
			if (Coords[i] == Coords[EndOfSubPath])
			{
				// This path has the same start and end coord
				// so mark it as closed and fill it
				Verbs[EndOfSubPath] |= PT_CLOSEFIGURE;
				IsFilled = TRUE;
			}

			// Update the EndOfSubPath variable
			EndOfSubPath = i-1;
		}
	}
}



/********************************************************************************************

>	BOOL Path::MergeTwoPaths(const Path& Other)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> (Split to arrays varient by Markn 22/11/94)
	Created:	27/4/94
	Inputs:		Other - A reference to another path to merge with this one
	Returns:	TRUE if the merge was a success, FALSE if it failed (lack of memory)
	Purpose:	Combines the two paths into a single complex path

********************************************************************************************/

BOOL Path::MergeTwoPaths(const Path& Other)
{
	// Make space to copy all the data from the other path into this one
	CurrentPos = UsedSlots;
	if (!MakeSpaceInPath(Other.UsedSlots))
		return FALSE;

	DocCoord*  OtherCoords = (DocCoord*)  DescribeHandle(Other.CoordHandle);
	PathVerb*  OtherVerbs  = (PathVerb*)  DescribeHandle(Other.VerbHandle);
	PathFlags* OtherFlags  = (PathFlags*) DescribeHandle(Other.FlagsHandle);

	return MergeTwoPaths(OtherCoords,OtherVerbs,OtherFlags,Other.UsedSlots,Other.IsFilled);
}


/********************************************************************************************

>	BOOL Path::MergeTwoPaths(DocCoord* OtherCoords,PathVerb* OtherVerbs,PathFlags* OtherFlags,INT32 Length,BOOL Filled)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> (Split from Path& varient by Markn 22/11/94)
	Created:	27/4/94
	Inputs:		OtherCoords = ptr to a coords array
				OtherVerbs  = ptr to a verbs array
				OtherFlags  = ptr to a flags array
				Length		= the number of elements in the three arrays
				Filled		= TRUE if the arrays represent a filled path
	Returns:	TRUE if the merge was a success, FALSE if it failed (lack of memory)
	Purpose:	Appends the data in the three given arrays to the arrays in this path

********************************************************************************************/

BOOL Path::MergeTwoPaths(DocCoord* OtherCoords,PathVerb* OtherVerbs,PathFlags* OtherFlags,INT32 Length,BOOL Filled)
{
	// Do we have enough room?
	if (UnUsedSlots < Length)
		return FALSE;

	// We will need all the arrays for both paths
	DocCoord*  Coords = (DocCoord*)  DescribeHandle(CoordHandle);
	PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb*  Verbs  = (PathVerb*)  DescribeHandle(VerbHandle);
	
	// Copy all the data
	INT32 DestPos = UsedSlots;
	for (INT32 i=0; i<Length; i++)
	{
		// Copy this coord and its associated data
		Coords[DestPos] = OtherCoords[i];
		Flags[DestPos]  = OtherFlags[i];
		Verbs[DestPos]  = OtherVerbs[i];

		// move to the next position
		DestPos++;
		UsedSlots++;
		UnUsedSlots--;
	}

	// If the other path was closed and marked as filled, then
	// the new path should also be filled
	if (Filled)
		IsFilled = TRUE;

	// Put current pos to the end of the used slots
	CurrentPos = UsedSlots;

	// and say that it worked
	return TRUE;
}


#if !defined(EXCLUDE_FROM_XARLIB)
/********************************************************************************************

>	double Path::CalculateFlatnessValueFromPath(double DividerValue = 375.0, double LowerLimit = 1.0,
												double UpperLimit = 375.0)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/2000
	Inputs:		Divider Value which is incorperated into the calculation.
				Lower and Upper Limits that the return value should be within.
	Returns:	New calculated flatness value OR 0.0 if the path is empty!
	Purpose:	To calculate a suitable flatness value to be used with Contouring, Clipping
				and Stroking. The value is based on the actual size of the paths dimensions.

********************************************************************************************/
double Path::CalculateFlatnessValueFromPath(double DividerValue, double LowerLimit, double UpperLimit)
{
	// Check to see if this path is empty, if so then just return 0.
	if(UsedSlots == 0)
		return 2.0;

	// Check to see if we`ve been given suitable values to use!
	if(LowerLimit < 1.0)
	{
		ERROR3("When I said give me a lower limit, I meant one larger than 1!");
		LowerLimit = 2.0;
	}

	if(UpperLimit < 1.0 || UpperLimit < LowerLimit)
	{
		ERROR3("When I said give me an Upper limit, I meant one larger than the Lower limit!");
		UpperLimit = 375.0;
	}

	if(DividerValue < 1.0)
	{
		ERROR3("When I said give me a Divider Value, I meant one greater than 1!");
		DividerValue = 375.0;
	}

	// Set up the CalcFlatness value
	double CalcFlatness = 375.0;

	// Get the smallest dimesion, Width or Height.
	DocRect drBounds = GetBoundingRect();
	double Smallest = drBounds.Height() < drBounds.Width() ? (double)drBounds.Height() : (double)drBounds.Width();

	// now make the flatness value equal to the smallest dimesion divided by the passed in DividerValue!
	CalcFlatness = Smallest / DividerValue;

	// Check to see if we`re within the specified limits
	if(LowerLimit > CalcFlatness) CalcFlatness = LowerLimit;
	if(UpperLimit < CalcFlatness) CalcFlatness = UpperLimit;

	TRACEUSER( "MarkH", _T("Calculated Flatness = %lf\n"),CalcFlatness);

	// Return the new calculated flatness value!
	return CalcFlatness;
}

/********************************************************************************************

>	INT32 Path::ClipPathToPath(const Path& Src,const Path* pDest,UINT32 Flags,
						UINT32 Tolerance = 100,double SrcFlatness = 750.0,double ClipFlatness = 750.0);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		Src			= source path to clip
				pDest 		= ptr to path to place the result in
				Flags		= Gavin's ClipPathToPath flags (see below)
				Tolerence 	= Two points closer than this value are considered the same point
				SrcFlatness = the value used to flatten the src path
				ClipPath	= the value used to flatten the clip path
	Outputs:	If returned a value >=0, *pDest contains the resultant path.
	Returns:	The length of the resultant path in pDest, or -1 for an error.

	Purpose:	This is an interface to Gavin's ClipPathToPath.

				'this' path is used as the clip path. Src is clipped to this path and the result is placed
				in pDest.

				'Flags' controls how Src will be clipped to this path (taken from gclip.h - complaints go to Gavin)
					CLIPPING_STYLE		- Clip style
						001					- 	  Source AND NOT Clip (subtract Clip from Source)
						010					-     Source AND     Clip (Intersection)
						011					-     Source
						100					- NOT Source AND     Clip (subtract Source from Clip)
						101					-     Source EOR	 Clip
						110					-	  				 Clip
						111					-     Source OR      Clip (Union)
										- If stroked then
						000					- Source AND     Clip (Intersection)
						???					- Source AND NOT Clip
					CLIPPING_SOURCE_WINDING - Source path winding.
					CLIPPING_CLIP_WINDING	- Clip path winding.
					CLIPPING_IS_STROKED		- Source is stroked, not filled
					CLIPPING_IS_CLOSED		- Source path is to be closed (only applicable if stroked).

					NOTE: 	The path placed in pDest will not have its flags array initialised. If you require
							this (e.g. the path is part of a NodePath and is going in the tree) call
							Path::InitialiseFlags() on pDest. If you don't, the path won't be editable via the
							bezier tool.

********************************************************************************************/
INT32 Path::ClipPathToPath(const Path& Src,Path* const pDest,UINT32 Flags,
						UINT32 Tolerance,double SrcFlatness,double ClipFlatness)
{
	ERROR2IF(pDest == NULL,-1,"pDest == NULL");

	// We need a temp path to place the result in (return -1 if it fails to initialise)
	Path Output;
 	if (!Output.Initialise()) return -1;

	// Mark Howitt - 1/9/00
	// Guessing is not good practice. The major problem in this function is that we need
	// to allocate memory before we know how much to allocate!!! This results in more memory than needed being used
	// and also the risk of under estimating leading to failure!
	// The way to fix this is get Gavin to do the allocation for us and then return the amount. This is currently the
	// way the contouring code works which results in no guessing and exact memory allocation. NO WASTE!
	// To quickly fix this problem I`m going to make it so that if it comes back with buffer overflow, we double the
	// allocation and try again. If this fails after 5 times then return -1 and error2 with failure. This is still not
	// very good but will fix a majority of the bugs we are seeing. MUST BE REPLACED!!!!!

	// Try and guess how big the destination path's buffers will have to be in order to take the result.
	INT32 NewSize = ((UsedSlots + Src.UsedSlots)*2)+1000;

	// Make sure the destination path is big enough
	if (!Output.MakeSpaceInPath(NewSize)) return -1;

	// Set up the params for ClipPathToPath, Gavin-style!
	PPOINT SPoints,CPoints,OPoints ;
	PBYTE  STypes ,CTypes ,OTypes  ;
	UINT32  SLength,CLength,OLength ;

	if ( (Flags & 7)==6 && !(Flags & 0x40) )
	{
		SPoints = NULL;
		STypes  = NULL;
		SLength = 0;
	}
	else
	{
		SPoints = (PPOINT)	Src.GetCoordArray();
		STypes  = (PBYTE) 	Src.GetVerbArray();
		SLength = 			Src.UsedSlots;
	}
	if ( (Flags & 7)==3 && !(Flags & 0x40) )
	{
		CPoints = NULL;
		CTypes  = NULL;
		CLength = 0;
	}
	else
	{
		CPoints = (PPOINT)	GetCoordArray();
		CTypes  = (PBYTE) 	GetVerbArray();
		CLength = 			UsedSlots;
	}

	OPoints = (PPOINT)	Output.GetCoordArray();
	OTypes  = (PBYTE) 	Output.GetVerbArray();
	OLength = NewSize;
	INT32 len = -2;
	INT32 Passes = 0;

	// Mark Howitt 1/9/00
	// New loop to help fix under allocation bugs. NEEDS REPLACING WITH BETTER CODE!!! See above!
	while(len == -2 && Passes++ < 10)
	{
		len = ::ClipPathToPath(	SPoints,STypes,SLength,SrcFlatness,
										CPoints,CTypes,CLength,ClipFlatness,
										Flags,Tolerance,
										OPoints,OTypes,OLength-10);		// -10 to safe guard against possible minor buffer overflow

		if(len == -1)
			return -1;
		else if(len == -2)
		{
			// Ok, we under estimated so double it and reset the output path variables!
			OLength <<= 1;
			if (!Output.EnsureVolume(OLength)) return -1;
			Output.ClearPath();
			OPoints = (PPOINT)	Output.GetCoordArray();
			OTypes  = (PBYTE) 	Output.GetVerbArray();
		}
	}

	// If we get here with len == -2 then we failed in 5 attempts to allocate enough space!
	if(len < 0)
		return -1;

	// Clear pDest and make sure there's enough space in pDest for the result
	pDest->ClearPath();
	if (!pDest->MakeSpaceInPath(len)) return -1;

	// Copy the resultant path into it.
	if (!pDest->MergeTwoPaths(Output.GetCoordArray(),Output.GetVerbArray(),Output.GetFlagArray(),len,Src.IsFilled))
		return -1;

	// YES! we have a result
	return len;	
}



/********************************************************************************************

BOOL Path::StrokePathToPath(MILLIPOINT 	LineWidth 	= 250, 
							LineCapType LineCap	  	= LineCapButt,
							JointType 	JoinStyle 	= MitreJoin, 
							DashType 	*pDash	  	= NULL,
							Path* 		pDest	  	= NULL,
							DWORD 		Flatness	= 200,
							BOOL 		Close	  	= FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		LineWidth 	= line width in millipoints
				LineCap		= line cap style
				JoinStyle	= join style (what else!)
				pDash		= ptr to dash pattern (NULL means no dash pattern)
				pDest 		= ptr to path to place result in (NULL means put result in 'this' path)
				Flatness	= the value used to flatten the path
				Close		= TRUE means the path is closed, FALSE otherwise
	Outputs:	-
	Returns:	TRUE if OK, FALSE if not enough memory, or the resultant path has no elements
	Purpose:	Strokes the path, and places result in pDest.
				If pDest == NULL, the result is placed back in this path
	SeeAlso:	-

********************************************************************************************/
BOOL Path::StrokePathToPath(MILLIPOINT 	LineWidth, 
							LineCapType LineCap,
							JointType 	JoinStyle, 
							DashType 	*pDash,
							Path* 		pDest,
							double 		Flatness,
							BOOL 		Close)
{
	if (pDest == NULL) pDest = this;

	UINT32 NewFlatness = (UINT32)Flatness;

	INT32 NumCoords  = GetNumCoords();
	INT32 OutputSize = (NumCoords*40)+1000;
	Path OutputPath;
	if (!OutputPath.Initialise(OutputSize)) return FALSE;

	DocCoord*	IPoints = 			GetCoordArray();
	BYTE*		ITypes	= (BYTE*) 	GetVerbArray();
	DWORD		ILength = (DWORD)  	NumCoords;

	DocCoord* 	OPoints = 			OutputPath.GetCoordArray();
	BYTE*		OTypes	= (BYTE*) 	OutputPath.GetVerbArray();
	DWORD		OLength = (DWORD)  	OutputSize;

	// Karim 22/11/2000
	// Modified - if line-width is 0, then we don't want dashes.
	DashType Dash;
	Dash.Length = 0;
	if (pDash == NULL || LineWidth == 0) pDash = &Dash;

	INT32 len = -10;

	// keep checking to see if we had an overflow!
	while(len < -2)
	{
		len = GRenderRegion::StrokePathToPath(IPoints,ITypes,ILength,
									OPoints,OTypes,OLength,
									Close,LineWidth,NewFlatness,LineCap,JoinStyle,pDash);

		if(len < -2)
		{
			OLength -= len;
			OutputPath.EnsureVolume(OLength);
			OPoints = 			OutputPath.GetCoordArray();
			OTypes	= (BYTE*) 	OutputPath.GetVerbArray();
		}
	}

	OutputPath.UsedSlots    = len;
	OutputPath.UnUsedSlots -= len;
	OutputPath.IsFilled		= TRUE;

	pDest->ClearPath();
	if (len > 0)
	{
		if (!pDest->MergeTwoPaths(OutputPath)) return FALSE;
		pDest->InitialiseFlags();
		INT32 n = pDest->GetNumCoords();
		PathVerb* pVerbs = pDest->GetVerbArray();
		pVerbs[n-1] |= PT_CLOSEFIGURE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
//	Author :		MarkH
//	Last UpDate :	10/07/00
//	Function :		InitializeContourValues(UINT32 Width, JoinStyles JoinS, CapStyles CapS,
//									bool IsAnOuterContour, bool ClosePath, UINT32 Flatness,
//									UINT32 MitreLimit)
//
//	Inputs :		Width - Max Contour Width required.
//					JoinStyle - Contour Join Style.
//					CapStyle - Contour Cap Style.
//					IsAnOuterContour - Set to TRUE if outer required, FALSE for inner!
//					ClosePath - TRUE if the path is to be closed
//					UseContourmode - If true use the contouring, if FALSE use strokepathtopath
//					Flatness - required flatness
//					MitreLimit - MitreLimit required
//	Return :		New Path Length for the countour
//	Purpose :		This function is used to initialize the GenPathContour class with
//					the current path information and the required width,joinstyle and so on...
//					This should be called first before any other call to the GenPathContour
//					Functions!
//
//////////////////////////////////////////////////////////////////////////////////////
INT32 Path::InitializeContourValues(UINT32 Width, JoinStyles JoinS, BOOL IsAnOuterContour, double Flatness,
								   BOOL ClosePath, BOOL UseContourMode, CapStyles CapS, UINT32 MitreLimit)
{
	m_ContourWidth = Width;
	m_ContourJoinS = JoinS;
	m_ContourCapS = CapS;
	m_IsAnOuterContour = IsAnOuterContour;
	m_DoClosePath = ClosePath;
	m_UseContourMode = UseContourMode;
	m_ContourFlatness = Flatness;
	m_ContourMitreLimit = MitreLimit;

	return GetNumCoords();
}

//////////////////////////////////////////////////////////////////////////////////////
//	Author :		MarkH
//	Last UpDate :	10/07/00
//	Function :		GetContourForStep(Path* pDest, double StepValue = 1.0)
//
//	Inputs :		pDest - The path to place the new contour path into
//					StepValue - 0.0 to 1.0 value of the required step
//	Return :		New Path Length for the current Step.
//	Purpose :		This function is used to get the path at the requested StepValue.
//					This flavour of this function actually takes just the destination
//					path and fills it in for you automatically 
//////////////////////////////////////////////////////////////////////////////////////
INT32 Path::GetContourForStep(Path* pDest, double StepValue)
{
	// First Check to see if the Path pasted in is valid!
	ERROR2IF(pDest==NULL,0,"Init GenPathContour Recieved a NULL Pointer!");

	PPOINT SPoints = (PPOINT)GetCoordArray();
	PBYTE STypes = (PBYTE)GetVerbArray();
	INT32 SLength = GetNumCoords();

	PPOINT OPoints = NULL;
	PBYTE OTypes = NULL;
	PPOINT INPoints = NULL;
	PBYTE INTypes = NULL;

	ERROR3IF(SLength < 0,"We`ve got a problem got a path with negative length!");

	Path CopyPath;
	CopyPath.Initialise();

	Path StrokedPath;
	StrokedPath.Initialise();

	INT32 PathLength = -1;
	INT32 Width = 100;
	Width = (INT32)(StepValue * m_ContourWidth);

	JointType JoinS = (m_ContourJoinS==JOIN_MITER) ? MitreJoin : (m_ContourJoinS==JOIN_ROUND) ? RoundJoin : BevelledJoin;
	LineCapType CapS = (m_ContourCapS==CAPS_BUTT) ? LineCapButt : (m_ContourCapS==CAPS_ROUND) ? LineCapRound : LineCapSquare;

	TRACEUSER( "MarkH", _T("Contouring with flatness %lf\n"),m_ContourFlatness);
	INT32 NewPathLength = -2;
	INT32 Passes = 0;

	// If we`re not using the contouring mode and we want to use the StrokePathToPAth function then do it here!
	if(!m_UseContourMode)
		PathLength = StrokePathToPath(Width, CapS, JoinS, NULL, &StrokedPath, m_ContourFlatness, m_DoClosePath);

	if(pDest->GetNumCoords() > 0)
	{
		// Now copy the Destination path as it needs to be clipped with the intermediate Path!
		CopyPath.CloneFrom(*pDest);
		pDest->ClearPath();
	}
	else
	{
		pDest->SetPathPosition(0);
	}

	// Check again to see if we`re doing the contour mode or the stroke path to path mode
	if(m_UseContourMode)
	{
		// now initialize and get the Points and Type pointers from a tempary path
		// that holds the intermediate step
		GenPathContour GStroke;
		// [Phil, 18/08/2005] Should this be assigning to NewPathLength? The test marked !!! below will always fail...
		PathLength = GStroke.ContourPath(SPoints, STypes, SLength, &OPoints, &OTypes,
										Width, m_ContourMitreLimit, m_ContourFlatness,
										m_ContourJoinS, (m_IsAnOuterContour == TRUE));

		INT32 OLength = (PathLength * 4)+1000;

		if ( PathLength > 0 && OPoints && OTypes)
		{
			if ( !m_IsAnOuterContour )
			{
				// Create two new Arrays which are larger than the current OPoints and OTypes
				// as gavin allocates his own memory for these points!
				INPoints = new POINT[OLength];
				INTypes = new BYTE[OLength];

				if(INPoints && INTypes)
				{
					memcpy(INPoints,OPoints,sizeof(POINT) * PathLength);
					memcpy(INTypes,OTypes,PathLength);
					delete OPoints;
					OPoints = INPoints;
					delete OTypes;
					OTypes = INTypes;
				}
				else
				{
					delete OTypes;
					delete OPoints;

					// If the NewPathLength is 0 then there`s no point continueing so
					// make sure the destination path contains the data it came in with
					// and return!
					if(NewPathLength == 0)				// !!!
						pDest->CloneFrom(CopyPath);
					
					return NewPathLength;
				}

				RECT obbox ;
				XaDraw_CalcSimpleBBox( OPoints,PathLength,&obbox ) ;
				OPoints[PathLength+0].x = obbox.right +0x1000 ;
				OPoints[PathLength+0].y = obbox.top   -0x1000 ;
				OPoints[PathLength+1].x = obbox.right +0x1000 ;
				OPoints[PathLength+1].y = obbox.bottom+0x1000 ;
				OPoints[PathLength+2].x = obbox.left  -0x1000 ;
				OPoints[PathLength+2].y = obbox.bottom+0x1000 ;
				OPoints[PathLength+3].x = obbox.left  -0x1000 ;
				OPoints[PathLength+3].y = obbox.top   -0x1000 ;
				OTypes[PathLength+0] = PT_MOVETO ;
				OTypes[PathLength+1] = PT_LINETO ;
				OTypes[PathLength+2] = PT_LINETO ;
				OTypes[PathLength+3] = PT_LINETO|PT_CLOSEFIGURE ;
				
				while(NewPathLength == -2 && Passes++ < 6)
				{
					NewPathLength = ::ClipPathToPath(OPoints, OTypes, PathLength+4, m_ContourFlatness,
													OPoints+PathLength, OTypes+PathLength, 4, m_ContourFlatness,
													4 | 1<<4 | 1<<5, 30,
													OPoints, OTypes, OLength);
				}

				PathLength = NewPathLength;

				if(NewPathLength <= 0)
				{
					delete OTypes;
					delete OPoints;

					// If the NewPathLength is 0 then there`s no point continueing so
					// make sure the destination path contains the data it came in with
					// and return!
					if(NewPathLength == 0)
						pDest->CloneFrom(CopyPath);
					
					return NewPathLength;
				}
			}

			pDest->EnsureVolume(OLength);

			PPOINT DPoints = (PPOINT)pDest->GetCoordArray();
			PBYTE DTypes = (PBYTE)pDest->GetVerbArray();
//			INT32 DLength = pDest->GetNumCoords();

			PPOINT CPoints = (PPOINT)CopyPath.GetCoordArray();
			PBYTE CTypes = (PBYTE)CopyPath.GetVerbArray();
			INT32 CLength = CopyPath.GetNumCoords();

			NewPathLength = -2;

			while(NewPathLength == -2 && Passes++ < 6)
			{
				NewPathLength = ::ClipPathToPath(	OPoints, OTypes, PathLength, m_ContourFlatness,
												CPoints, CTypes, CLength, m_ContourFlatness,
												7 | 1<<4, 30,
												DPoints, DTypes, OLength);

				if(NewPathLength == -2)
				{
					OLength <<= 1;
					pDest->EnsureVolume(OLength);
					pDest->SetPathPosition(0);
					DPoints = (PPOINT)pDest->GetCoordArray();
					DTypes = (PBYTE)pDest->GetVerbArray();
					TRACEUSER( "MarkH", _T("Path to short so using new length %d\n"),OLength);
				}
			}

			if(NewPathLength <= 0)
			{
				delete OTypes;
				delete OPoints;

				// If the NewPathLength is 0 then there`s no point continueing so
				// make sure the destination path contains the data it came in with
				// and return!
				if(NewPathLength == 0)
					pDest->CloneFrom(CopyPath);

				return NewPathLength;
			}

			pDest->UsedSlots += NewPathLength;
			pDest->UnUsedSlots -= NewPathLength;

			delete OTypes;
			delete OPoints;

			// Now try to close the destination path and tidyup the slot allocations!
			pDest->TryToClose();
		}
		else
		{
			delete OTypes;
			delete OPoints;

			// If the NewPathLength is 0 then there`s no point continueing so
			// make sure the destination path contains the data it came in with
			// and return!
//			if(NewPathLength == 0)
			if (NewPathLength <=0)
			{
				pDest->CloneFrom(CopyPath);
				return 0;
			}
		}
	}
	else
	{
		// With the stroke path to path method we need to clean up the resultant path!
		PathLength = CopyPath.ClipPathToPath(StrokedPath, pDest, 7|(1<<4), 30, m_ContourFlatness, m_ContourFlatness);
		
		if(PathLength <= 0)
		{
			// If the NewPathLength is 0 then there`s no point continueing so
			// make sure the destination path contains the data it came in with
			// and return!
			if(PathLength == 0)
				pDest->CloneFrom(CopyPath);

			TRACEUSER( "MarkH", _T("ClipFailed so copying path! PATHS\n"));
		}
	}

	// Return the New Path Length
	return PathLength;
}

#endif	// EXCLUDE_FROM_XARLIB


/********************************************************************************************

	BOOL Path::ExternalArraysAdded(const INT32 Added)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/94
	Inputs:		Added = the number of new elements added to path arrays.
	Returns:	TRUE if the external arrays have been validated
				FALSE if the external array additions have been ignored
	Purpose:	A hidden function to adjust the number of elements in a path. This has been
				added by me as a last resort and should not be used by others. (Hence hidden
				from the documentation). It is required to cope with changes in the path
				data external to the path class. This occurs when doing really nasty things
				like moulding paths.

********************************************************************************************/

BOOL Path::ExternalArraysAdded(const INT32 Added)
{
	ERROR1IF(Added<1 || Added>UnUsedSlots, FALSE, "Path::ExternalArraysAdded() called with illegal additon");

	UsedSlots+=Added;
	UnUsedSlots-=Added;

	return TRUE;
} 


/********************************************************************************************

	BOOL Path::ExternalArraysReplaced(const INT32 Size)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/94
	Inputs:		Size = the number of elements replaced in the path arrays
	Returns:	TRUE 	if the external arrays have been validated
				FALSE 	if the external array replacements have been ignored
	Purpose:	A hidden function to adjust the number of elements in a path. This has been
				added by me as a last resort and should not be used by others. (Hence hidden
				from the documentation).

********************************************************************************************/

BOOL Path::ExternalArraysReplaced(const INT32 Size)
{
	ERROR1IF(Size<1 || Size>(UsedSlots+UnUsedSlots), FALSE, "Path::ExternalArraysCloned() called with an illegal clone");

	UnUsedSlots	= (UsedSlots+UnUsedSlots)-Size;
	UsedSlots	= Size;

	return TRUE;
}


/********************************************************************************************

>	BOOL Path::CopySectionFrom(const Path& Other, INT32 StartIndex, INT32 NumToCopy)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Inputs:		Other - A reference to another path to merge with this one
	Returns:	TRUE if the merge was a success, FALSE if it failed (lack of memory)
	Purpose:	Copies a specified section from a path to the end of this one

********************************************************************************************/

BOOL Path::CopySectionFrom(const Path& Other, INT32 StartIndex, INT32 NumToCopy)
{
	return MergeSectionFrom(UsedSlots, Other, StartIndex, NumToCopy);
} 


/********************************************************************************************

>	BOOL Path::CopySectionTo(Path* Dest, INT32 StartIndex, INT32 NumToCopy)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/09/94
	Inputs:		Dest - A pointer to another path to add the data to
	Returns:	TRUE if the copy was a success, FALSE if it failed (lack of memory)
	Purpose:	Copies a specified section from this path to the end of another path
	
********************************************************************************************/

BOOL Path::CopySectionTo(Path* Dest, INT32 StartIndex, INT32 NumToCopy)
{
	return MergeSectionTo(StartIndex, NumToCopy, Dest, Dest->UsedSlots);
}




/********************************************************************************************

>	BOOL Path::MergeSectionFrom(INT32 DestinPos, const Path& Source, INT32 SourcePos, INT32 Length)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		DestinPos	= Index of where to copy path elements to
							  All existing items from this index onwards will move up.
				Source		= Object to take elements from
				SourcePos	= Position in source to start copying elements from
				Length 		= Number of elements to copy over

	Returns:	TRUE if the merge was a success, FALSE if it failed (lack of memory)
	Purpose:	Copies a specified section from a source path to a specified position in
				this path

********************************************************************************************/

BOOL Path::MergeSectionFrom(INT32 DestinPos, const Path& Source, INT32 SourcePos, INT32 Length)
{

	// Make space to copy all the data from the other path into this one
	CurrentPos = DestinPos;
	if (!MakeSpaceInPath(Length))
		// Failed to get extra memory
		return FALSE;

	// We will need all the arrays for both paths
	DocCoord*  DCoords = (DocCoord*)  DescribeHandle(CoordHandle);
	PathFlags* DFlags  = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb*  DVerbs  = (PathVerb*)  DescribeHandle(VerbHandle);
	
	DocCoord*  SCoords = (DocCoord*)  DescribeHandle(Source.CoordHandle);
	PathFlags* SFlags  = (PathFlags*) DescribeHandle(Source.FlagsHandle);
	PathVerb*  SVerbs  = (PathVerb*)  DescribeHandle(Source.VerbHandle);

	// Copy all the data
	memmove((void*)(DCoords+DestinPos), (void*)(SCoords+SourcePos), Length*sizeof(DocCoord));
	memmove((void*)(DVerbs+DestinPos),  (void*)(SVerbs+SourcePos),  Length*sizeof(PathVerb));
	memmove((void*)(DFlags+DestinPos),  (void*)(SFlags+SourcePos),  Length*sizeof(PathFlags));

	UsedSlots+=Length;
	UnUsedSlots-=Length;

	// and say that it worked
	return TRUE;
}



/********************************************************************************************

>	BOOL Path::MergeSectionTo(SourcePos, Length, Path* Destin, INT32 DestinPos)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		SourcePos	= Position within this path to copy data from
				Length		= Number of items to copy
				Destin		= Pointer to path to copy items to
				DestinPos	= Position in destin path top copy data to
							  All existing items from this index and beyond will move up
	Returns:	TRUE if the copy was a success, FALSE if it failed (lack of memory)
	Purpose:	Copies a specified section from this path to a specific position within
				another path. A gap is created if necessary to copy the data into

********************************************************************************************/

BOOL Path::MergeSectionTo(INT32 SourcePos, INT32 Length, Path* Destin, INT32 DestinPos)
{
	// Make space to copy all the data this path to the destination

	Destin->CurrentPos = DestinPos;
	if (!Destin->MakeSpaceInPath(Length))
		// Failed to get extra memory
		return FALSE;

	// We will need all the arrays for both paths
	DocCoord*  SCoords = (DocCoord*)  DescribeHandle(CoordHandle);
	PathFlags* SFlags  = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb*  SVerbs  = (PathVerb*)  DescribeHandle(VerbHandle);
	
	DocCoord*  DCoords = (DocCoord*)  DescribeHandle(Destin->CoordHandle);
	PathFlags* DFlags  = (PathFlags*) DescribeHandle(Destin->FlagsHandle);
	PathVerb*  DVerbs  = (PathVerb*)  DescribeHandle(Destin->VerbHandle);

	// Copy all the data
	memmove((void*)(DCoords+DestinPos), (void*)(SCoords+SourcePos), Length*sizeof(DocCoord));
	memmove((void*)(DVerbs+DestinPos),  (void*)(SVerbs+SourcePos),  Length*sizeof(PathVerb));
	memmove((void*)(DFlags+DestinPos),  (void*)(SFlags+SourcePos),  Length*sizeof(PathFlags));

	Destin->UsedSlots+=Length;
	Destin->UnUsedSlots-=Length;

	// and say that it worked
	return TRUE;
}


/********************************************************************************************

>	BOOL Path::JoinToAnother(Path* OtherPath, INT32 MainIndex, INT32 OtherIndex)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/94
	Inputs:		OtherPath is the path which we're joining to this one
				MainIndex is the index of the point in this path we're joining
				OtherIndex is the index of the point in the other path we're joining to
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This routine is used to join the end of a line to the end of another line.
				It will cope with the various possiblities: That both subpaths are in the same
				complex paths, that one or other of the subpaths are in different complex paths,
				and that one of the paths might need reversing.
				The other path is left untouched. This path will contain all subpaths from that one.
				This routine assumes that the two subpaths are different - it will not join the
				start and end of the same subpath.
	Errors:		Returns FALSE if it failed (lack of memory, for example)
	SeeAlso:	-

********************************************************************************************/

BOOL Path::JoinToAnother(Path* OtherPath, INT32 MainIndex, INT32 OtherIndex)
{
	// First, get some indices pointing at the start and end of these subpaths
	INT32 MainSubStart;			// Start of subpath in main path (this)
	INT32 MainSubEnd;			// End of subpath in this path
	INT32 OtherSubStart;			// Start of subpath in other path
	INT32 OtherSubEnd;			// End of subpath in other path

	MainSubStart = MainSubEnd = MainIndex;
	FindStartOfSubPath(&MainSubStart);		
	FindEndElOfSubPath(&MainSubEnd);

	OtherSubStart = OtherSubEnd = OtherIndex;
	OtherPath->FindStartOfSubPath(&OtherSubStart);
	OtherPath->FindEndElOfSubPath(&OtherSubEnd);

	ERROR2IF(((this == OtherPath) && (MainSubStart == OtherSubStart)), FALSE, "Attempted to join this subpath with itself");

	// Because we'll be messing around with the path, it's easier if we make the
	// two indices relative to the starts of their respective subpaths
	MainIndex = MainIndex - MainSubStart;
	OtherIndex = OtherIndex - OtherSubStart;

	// Now that we know which subpaths we're interested in, let's see if we have to join the two paths

	if (this != OtherPath)
	{
		// OtherPath is not the same, so copy it into this one and adjust the subpath indices
		OtherSubStart += UsedSlots;
		OtherSubEnd += UsedSlots;
		//OtherIndex += UsedSlots;
		if (!MergeTwoPaths(*OtherPath))
			return FALSE;
	}

	// Now this path contains both subpaths (and others, maybe)
	// If the subpaths were originally in the same path, it's possible that the other subpath
	// is below the main subpath. We don't want that possibility, so swap them if that's the case
	if (OtherSubStart < MainSubStart)
	{
		INT32 temp = OtherSubStart;
		OtherSubStart = MainSubStart;
		MainSubStart = temp;
		temp = OtherSubEnd;
		OtherSubEnd = MainSubEnd;
		MainSubEnd = temp;
		temp = OtherIndex;
		OtherIndex = MainIndex;
		MainIndex = temp;
	}

	// See if the two subpaths are adjacent now
	if (OtherSubStart - MainSubEnd >1)
	{
		// The two subpaths are not adjacent, so let's shuffle the path around...
		// It's just possible that the Othersection might be at the top of the path so move the
		// main section to the top.
		CurrentPos = UsedSlots;
		if (!MakeSpaceInPath(MainSubEnd-MainSubStart+1))
			return FALSE;
		if (!InsertSectionAtEnd(this, MainSubStart, MainSubEnd-MainSubStart+1))
			return FALSE;
		DeleteSection( MainSubStart, MainSubEnd-MainSubStart+1);

		// Recalculate the subpath indices
		MainSubEnd = MainSubEnd-MainSubStart+1;		// Change into length
		MainSubStart = UsedSlots - MainSubEnd;
		OtherSubStart -= MainSubEnd;
		OtherSubEnd -= MainSubEnd;
		MainSubEnd = UsedSlots-1;

		// Now, are the subpaths adjacent? Main is now above Other
		if (MainSubStart - OtherSubEnd > 1)
		{
			// Do the same as above, but with Other instead of Main
			CurrentPos = UsedSlots;
			if (!MakeSpaceInPath(OtherSubEnd-OtherSubStart+1))
				return FALSE;
			if (!InsertSectionAtEnd(this, OtherSubStart, OtherSubEnd-OtherSubStart+1))
				return FALSE;
			DeleteSection( OtherSubStart, OtherSubEnd-OtherSubStart+1);

			// Recalculate the subpath indices
			OtherSubEnd = OtherSubEnd-OtherSubStart+1;		// Change into length
			OtherSubStart = UsedSlots - OtherSubEnd;
			MainSubStart -= OtherSubEnd;
			MainSubEnd -= OtherSubEnd;
			OtherSubEnd = UsedSlots-1;


		}
		
		if (OtherSubStart < MainSubStart)
		{
			// Swap Main and Other indices
			INT32 temp = MainSubStart;
			MainSubStart = OtherSubStart;
			OtherSubStart = temp;
			temp = MainSubEnd;
			MainSubEnd = OtherSubEnd;
			OtherSubEnd = temp;
			temp = OtherIndex;
			OtherIndex = MainIndex;
			MainIndex = temp;
		}
	}

	// Now we are guaranteed that the two subpaths are adjacent and that Main is below Other
	// Let's see if we have to reverse anything
	MainIndex+=MainSubStart;
	OtherIndex+=OtherSubStart;

	if (MainIndex == MainSubStart)
	{
		// Because Main is below Other, this means we have to reverse this subpath...
		ReverseSection(MainSubStart, MainSubEnd-MainSubStart+1);
		MainIndex = MainSubEnd;
	}

	if (OtherIndex == OtherSubEnd)
	{
		// We have to reverse the other subpath
		ReverseSection(OtherSubStart, OtherSubEnd-OtherSubStart+1);
		OtherIndex = OtherSubStart;
	}

	// At INT32 last, it's possible just to join the subpaths - we do this by deleting the
	// MoveTo between them.
	DeleteSection(OtherSubStart,1);

	// And clear selection, then select just the point we want
	ClearSubSelection();
	PathVerb* Verbs = GetVerbArray();
	PathFlags* Flags = GetFlagArray();
	Flags[MainSubEnd].IsSelected = TRUE;
	if (Verbs[MainSubEnd] == PT_BEZIERTO)
		Flags[MainSubEnd-1].IsSelected = TRUE;
	if (Verbs[MainSubEnd+1] == PT_BEZIERTO)
		Flags[MainSubEnd+1].IsSelected = TRUE;

	return TRUE;
}


/********************************************************************************************

>	BOOL Path::SimpleJoin(const Path& Other, INT32 *OtherStartSlot, BOOL* IsOtherReversed)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/94
	Inputs:		Other - the path to join with this path
	Outputs:	OtherStartSlot - The Slot number in the combined path where the path
				Other starts.
				IsOtherReversed - TRUE if the 'Other' path was reversed during the joining
				process. FALSE if it was not
	Returns:	TRUE if the Join Worked, FALSE if it failed
	Purpose:	Joins two paths to form a new, longer path. Both the paths invloved are
				simple (ie not complex).

********************************************************************************************/

BOOL Path::SimpleJoin(Path* Other, INT32 *OtherStartSlot, BOOL* IsOtherReversed, BOOL* ThisReversed)
{
	// We have not reversed the other path yet
	*IsOtherReversed = FALSE;

	// Make a note of where the new path will start after the merge
	INT32 PathStart = UsedSlots;
	
	if (ThisReversed != NULL)
		*ThisReversed = FALSE;
	// Do we need to reverse any elements of this path
	INT32 StartSlot, NumSlots;
	BOOL JoinedAtStart;
	DocCoord*  OtherCoords = (DocCoord*)  DescribeHandle(Other->CoordHandle);

	// Find out if we need to reverse this path to help with the joining
	if (FindJoinedSubPathInfo(OtherCoords[0], &StartSlot, &NumSlots, &JoinedAtStart))
	{
		// Maybe need to reverse this path
		if (JoinedAtStart)
		{
			Reverse();
			if (ThisReversed != NULL)
				*ThisReversed = TRUE;
		}
	}

	// And check to see if the other end needs checking as well.
	if (FindJoinedSubPathInfo(OtherCoords[Other->UsedSlots-1], &StartSlot, &NumSlots, &JoinedAtStart))
	{
		// Reverse the joining path always
		Other->Reverse();

		// We have reversed the other path
		*IsOtherReversed = TRUE;

		// and maybe reverse this path
		if (JoinedAtStart)
		{
			Reverse();
			if (ThisReversed != NULL)
				*ThisReversed = TRUE;
		}
	}
	
	// merge the paths
	if (MergeTwoPaths(*Other)==FALSE)
		return FALSE;

	// Make sure that the new path starts with a MoveTo
	#ifdef _DEBUG
		PathVerb* Verbs  = (PathVerb*) DescribeHandle(VerbHandle);
		ENSURE( Verbs[PathStart]==PT_MOVETO, "Start of SubPath is not a MoveTo in SimpleJoin()" );
	#endif

	// The merge must have worked, so remove the initial MoveTo
	CurrentPos = PathStart;
	DeleteElement();

	// See if we can make it a closed path
	TryToClose();

	// report that it all worked
	CurrentPos = UsedSlots;
	*OtherStartSlot = PathStart-1;
	return TRUE;
}




/********************************************************************************************

>	BOOL Path::ComplexJoin(Path* Other, INT32 *OtherStartSlot, BOOL* IsOtherReversed)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/94
	Inputs:		Other - The New path that needs Joining in
	Outputs:	OtherStartSlot - The Slot number in the combined path where the path
				Other starts.
				IsOtherReversed - TRUE if the 'Other' path was reversed during the joining
				process. FALSE if it was not
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Tries to Join the New Path (must be simple) to a complex path

********************************************************************************************/

BOOL Path::ComplexJoin(Path* Other, INT32 *OtherStartSlot, BOOL* IsOtherReversed)
{
	// We have not reversed the other path yet
	*IsOtherReversed = FALSE;

	// Do we need to reverse any elements of this path
	INT32 StartSlot = 0;
	INT32 NumSlots = 0;
	INT32 JoinStart = 0;
	INT32 JoinLen = 0;
	BOOL JoinedAtStart = FALSE;
	BOOL JoinFound = FALSE;
	DocCoord*  OtherCoords = (DocCoord*)  DescribeHandle(Other->CoordHandle);

	// Find out if we need to reverse this path to help with the joining
	if (FindJoinedSubPathInfo(OtherCoords[0], &StartSlot, &NumSlots, &JoinedAtStart))
	{
		// We found a join
		JoinFound = TRUE;
		JoinStart = StartSlot;
		JoinLen = NumSlots;

		// Maybe need to reverse this path
		if (JoinedAtStart)
			ReverseSection(StartSlot, NumSlots);
	}

	// And check to see if the other end needs checking as well.
	if (FindJoinedSubPathInfo(OtherCoords[Other->UsedSlots-1], &StartSlot, &NumSlots, &JoinedAtStart))
	{
		// We found a join
		JoinFound = TRUE;
		JoinStart = StartSlot;
		JoinLen = NumSlots;

		// Reverse the joining path always
		Other->Reverse();

		// We have reversed the other path
		*IsOtherReversed = TRUE;

		// and maybe reverse this path
		if (JoinedAtStart)
			ReverseSection(StartSlot, NumSlots);
	}

	// If we found a join then everything is fabby
	if (JoinFound)
	{
		// Make sure that there is room in the path to copy one of the sub-paths
		CurrentPos = UsedSlots;
		if (!MakeSpaceInPath(JoinLen))
			return FALSE;

		// copy the sub path
		if (!InsertSectionAtEnd(this, JoinStart, JoinLen))
			return FALSE;

		// delete the original version of the sub path
		DeleteSection(JoinStart, JoinLen);
		
		// Make sure that there is enough room for the new path
		CurrentPos = UsedSlots;
		if (!MakeSpaceInPath(Other->UsedSlots-1))
			return FALSE;

		// copy the new path
		*OtherStartSlot = UsedSlots-1;
		if (!InsertSectionAtEnd(Other, 1, Other->UsedSlots-1))
			return FALSE;
	}
	else
	{
		// We did not find a join
		return FALSE;
	}

	// See if we can close anything
	TryToClose();

	// Must have worked, so Move the insertion point to the end and return happy
	CurrentPos = UsedSlots;
	return TRUE;
}


/********************************************************************************************

>	BOOL Path::ComplexToSameComplexJoin(Path* Other, INT32 *OtherStartSlot, BOOL* IsOtherReversed)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/94
	Inputs:		Other - The path to Join with this complex path.
	Outputs:	OtherStartSlot - The Slot number in the combined path where the path
				Other starts.
				IsOtherReversed - TRUE if the 'Other' path was reversed during the joining
				process. FALSE if it was not
	Purpose:	This path is assumed to be complex and Other is assumed to be a simple
				path. The start and end point of the Simple Path must be the same as
				the start or end point of one of the sub paths of this complex path.
				The function Moves the two sub paths that are touched by the simple
				path to the end of the path, and inserts the simple path between them.
				This means that the two touched sub-paths and the new simple path
				are combined into a single sub path. This may also mean that this
				complex path is no longer complex

********************************************************************************************/

BOOL Path::ComplexToSameComplexJoin(Path* Other, INT32 *OtherStartSlot, BOOL* IsOtherReversed)
{
	INT32 BackStartSlot, BackNumSlots;
	INT32 FrontStartSlot, FrontNumSlots;
	BOOL FrontJoinedAtStart, BackJoinedAtStart;

	// We have not reversed the other path yet
	*IsOtherReversed = FALSE;

	// Get a few arrays
	DocCoord*  OtherCoords = (DocCoord*)  DescribeHandle(Other->CoordHandle);

	// First we have to gather data about the subpaths we are joined to
	if (!FindJoinedSubPathInfo(OtherCoords[0], &FrontStartSlot, &FrontNumSlots, &FrontJoinedAtStart))
		return FALSE;

	// And the other sub path, Fail if we there is no intersection
	if (!FindJoinedSubPathInfo(OtherCoords[Other->UsedSlots-1], &BackStartSlot, &BackNumSlots, &BackJoinedAtStart))
		return FALSE;

	// If this was trying to join a sub path to the other end of the same sub path
	// then we need to use a different function
	if (FrontStartSlot == BackStartSlot)
		return ComplexJoin(Other, OtherStartSlot, IsOtherReversed);

	// Build a path to store the combined paths in
	Path InsertPath;
	InsertPath.Initialise(FrontNumSlots+BackNumSlots+Other->UsedSlots);

	// Reverse the first subpath if we need to
	if (FrontJoinedAtStart)
		ReverseSection(FrontStartSlot, FrontNumSlots);
	
	// Reverse the Second subpath if we need to
	if (!BackJoinedAtStart)
		ReverseSection(BackStartSlot, BackNumSlots);

	// Make copys of all the path sections
	// The path joined to the start of the new path
	if (!InsertPath.InsertSectionAtEnd(this, FrontStartSlot, FrontNumSlots))
		return FALSE;

	// followed by the new path
	*OtherStartSlot = InsertPath.UsedSlots-1;
	if (!InsertPath.InsertSectionAtEnd(Other, 1, Other->UsedSlots-1))
		return FALSE;

	// followed by the path joined to the end of the new path
	if (!InsertPath.InsertSectionAtEnd(this, BackStartSlot+1, BackNumSlots-1))
		return FALSE;

	// Get rid of the original sub-path
	DeleteSection(FrontStartSlot, FrontNumSlots);

	// Go find the Other Coords array again as it may have moved
	OtherCoords = (DocCoord*)  DescribeHandle(Other->CoordHandle);

	// Find the location of the Second path again, as deleting the first path may have moved it
	if (!FindJoinedSubPathInfo(OtherCoords[Other->UsedSlots-1], &BackStartSlot, &BackNumSlots, &BackJoinedAtStart))
		return FALSE;

	// Get rid of the second sub-path
	DeleteSection(BackStartSlot, BackNumSlots);

	// Make enough space in the path to store a copy of the two subpaths and the new path
	CurrentPos = UsedSlots;
	if (!MakeSpaceInPath(InsertPath.UsedSlots))
		return FALSE;

	// copy the Insert Path to the end of the original path
	*OtherStartSlot += UsedSlots;
	if (!InsertSectionAtEnd(&InsertPath, 0, InsertPath.UsedSlots))
		return FALSE;

	// Must have worked, so return happy
	CurrentPos = UsedSlots;
	return TRUE;
}


/********************************************************************************************

>	BOOL Path::FindJoinedSubPathInfo(const DocCoord& JoinCoord, INT32* Start,
									INT32* Length, BOOL* JoinAtStart)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/94
	Inputs:		JoinCoord - The Coord to compare with the start and end points od sub paths
	Outputs:	Start - Pointer to an INT32 to hold the starting slot number of the sub path
				Length - The number of slots in the found Sub Path
				JoinAtStart - TRUE if JoinCoord intersected the start of the Sub Path, 
							FALSE otherwise
	Returns:	TRUE if an intersection was found, FALSE if not
	Purpose:	This function searchs the path, compareing JoinCoord with the start and end
				points of each of the Sub-paths in the path. If it is the same as one of the
				start or end points, then the output params are filled and TRUE is returned.

********************************************************************************************/

BOOL Path::FindJoinedSubPathInfo(const DocCoord& JoinCoord, INT32* Start, INT32* Length, BOOL* JoinAtStart)
{
	// We will need all the arrays for both paths
	DocCoord*  Coords = (DocCoord*)  DescribeHandle(CoordHandle);
	PathVerb*  Verbs  = (PathVerb*)  DescribeHandle(VerbHandle);

	// walk through all the coords, looking for start and end points in SubPaths
	INT32 EndPos = UsedSlots-1;
	for (INT32 i=UsedSlots-1; i>=0; i--)
	{
		// See if we are at the start of a sub path
		if (Verbs[i] == PT_MOVETO)
		{
			// Set these to the start and end positions of the sub path
			*Start = i;
			*Length = EndPos - i + 1;

			// We are, so check the start and end of the sub path against the coord
			if (Coords[i] == JoinCoord)
			{
				// This is Joined at the start, so set the values and return
				*JoinAtStart = TRUE;
				return TRUE;
			}

			if (Coords[EndPos] == JoinCoord)
			{
				// This is Joined at the End, so set the values and return
				*JoinAtStart = FALSE;
				return TRUE;
			}

			EndPos = i-1;
		}
	}

	// We did not find an intersection with the subpaths, so fail
	return FALSE;
}



/********************************************************************************************

>	BOOL Path::InsertSectionAtEnd(const Path* Other, INT32 StartSlot, INT32 NumSlots)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/94
	Inputs:		Other - Pointer to the path to copy data from
				StartSlot - The slot in the path to start copying from
				NumSlots - The number of slots to copy
	Returns:	TRUE if the data was copied OK, FALSE if it failed
	Purpose:	Copies a section (usually a sub-path) of a path to the end of another path.
				This is used extensivly to Join Paths together. If there is not enougth
				space in this path, then the operation will fail - It will NOT try to make
				the path bigger. It is up to the caller to ensure that there are sufficent
				UnUsedSlots in the path before calling.

********************************************************************************************/

BOOL Path::InsertSectionAtEnd(const Path* Other, INT32 StartSlot, INT32 NumSlots)
{
	// Make sure that there is room in the path to store this extra info
	if (UnUsedSlots<NumSlots)
	{
		TRACE( _T("There was not enough room in this path to copy data into"));
		return FALSE;
	}

	// We need to know about the path arrays
	DocCoord*  Coords = (DocCoord*)  DescribeHandle(CoordHandle);
	PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb*  Verbs  = (PathVerb*)  DescribeHandle(VerbHandle);

	// and the new path
	DocCoord*  OtherCoords = (DocCoord*)  DescribeHandle(Other->CoordHandle);
	PathFlags* OtherFlags  = (PathFlags*) DescribeHandle(Other->FlagsHandle);
	PathVerb*  OtherVerbs  = (PathVerb*)  DescribeHandle(Other->VerbHandle);

	// Go though and copy all the element from one path to the other
	for (INT32 i=StartSlot; i<StartSlot+NumSlots; i++)
	{
		ENSURE(UnUsedSlots>0, "Ran out of space in a path that should have enough space in it" );
		if (UnUsedSlots<=0)
			return FALSE;

		// Copy the coords
		Coords[UsedSlots] = OtherCoords[i];
		Flags[UsedSlots] = OtherFlags[i];
		Verbs[UsedSlots] = OtherVerbs[i];

		// Move on to the next slots
		UsedSlots++;
		UnUsedSlots--;
	}

	return TRUE;
}





/********************************************************************************************

>	BOOL Path::FindNearestPoint(DocCoord pos, UINT32 flags, INT32* position)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/94
	Inputs:		pos - mouse position to be tested
				flags - POINTFLAG_ENDPOINTS = look at endpoints
						POINTFLAG_CONTROLPOINTS - look at control points
						
						(the flag POINTFLAG_ENDSFIRST has been removed and has
						no effect - the endpoints are always scanned first)
	Outputs:	position - index to the point that was matched
						   unchanged if no match
	Returns:	TRUE if a match was found, FALSE if not
	Purpose:	This function searches for a point in the path which is both selected
				and lies inside the largest blob bounding rectangle surrounding 'pos'.
				Points checked for are as follows...
				if ENDPOINTS is set
					check all selected endpoints in this path.
				if CONTROLPOINTS is set
					check all control points lying directly next to selected endpoints
					The rules are that control points are only enabled when a single end
					point is selected and hence a maximum of 2 control points will be
					checked.
				If any of these points are close enough	to be clicked on, the function 
				returns TRUE, with position holding the index to the point that was clicked

********************************************************************************************/

BOOL Path::FindNearestPoint(DocCoord pos, UINT32 flags, INT32* position)
{
#if !defined(EXCLUDE_FROM_XARLIB)
	// If neither ENDPOINTS or CONTROLPOINTS flags are set, return FALSE since the caller
	// obviously doesn't want to look at anything!
	if (!(flags & (POINTFLAG_ENDPOINTS | POINTFLAG_CONTROLPOINTS)))
		return (FALSE);

	// Always set POINTFLAG_ENDSFIRST
	flags |= POINTFLAG_ENDSFIRST;

	// Get arrays for this path
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	PathFlags* PFlags = (PathFlags*) DescribeHandle(FlagsHandle);

	// Get a pointer to the DocView so we can do blob proximity checks
	DocView* pDocView = DocView::GetSelected();
	ERROR3IF(pDocView == NULL, "Path::FindNearestPoint: Can't find selected DocView");
	if (pDocView == NULL)
		return(FALSE);

	DocRect BlobRect;
	// INT32 	numloops;
	
	// Now to calculate a rectangle around the pointer position which is
	// the same size as the biggest possible blob. We then go through the
	// path and see if any of the coordinates fall into that rectangle.
	// This is a very cunning reversal of the normal logic - go through the
	// path, find a blob rectangle for each point, then see if the pointer 
	// coordinate falls inside it. The only problem with this method is that
	// it assumes that all blobs are the same size, but since this is only 
	// for click detection, and we need a little bit of leeway, I think 
	// it's all right. Anyway, the usual code for this also makes this assumption,
	// although it doesn't rely on it.

	OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), pos,
								BT_SELECTEDLARGEST, &BlobRect);

	// Now BlobRect holds a rectangle around the pointer position.

	// We have to first go through and look at the endpoints, counting them as we go
	INT32 EndCount = 0;
	INT32 EndIndex = 0;
	INT32 i;
	
	for (i=0; i<UsedSlots; i++)
	{
		// check to see if it's an endpoint, and if we want this kind of point
		if ((PFlags[i].IsEndPoint && (flags & POINTFLAG_ENDPOINTS)))
		{
			if (BlobRect.ContainsCoord(Coords[i]))
			{
				*position = i;
				return(TRUE);
			}
		}
		if (PFlags[i].IsEndPoint && PFlags[i].IsSelected && ((Verbs[i] & PT_CLOSEFIGURE) == 0))
		{
			EndCount++;
			EndIndex = i;
		}
	}

	// Now we've scanned the path for endpoints, let's look at any adjacent control points
	// if EndCount == 1, EndIndex is the index of the selected enpoint, ie there is only one
	// selected endpoint and its control handles will be enabled.

	if ( (EndCount==1) && (flags & POINTFLAG_CONTROLPOINTS) )
	{
		INT32 Index = FindPrevControlPoint(EndIndex);
		if (Index>=0)
		{
			// There is a previous control point, so look at it (and the one before it)
			if (BlobRect.ContainsCoord(Coords[Index]))
			{
				*position = Index;
				return TRUE;
			}

			if (BlobRect.ContainsCoord(Coords[Index-1]))
			{
				*position = Index-1;
				return TRUE;
			}
		}

		// Now look at the next control point (if it's there)
		Index = FindNextControlPoint(EndIndex);
		if (Index>=0)
		{
			// Look at the next one
			if (BlobRect.ContainsCoord(Coords[Index]))
			{
				*position = Index;
				return TRUE;
			}

			if (BlobRect.ContainsCoord(Coords[Index+1]))
			{
				*position = Index+1;
				return TRUE;
			}
		}
	}
#endif
	return FALSE;
}

/****
	// We have to do two passes of the path if the caller is interested in both endpoints
	// and control points, and they requested endpoints first. Otherwise, we only need to
	// run round once

	if (flags == POINTFLAG_ENDSFIRST | POINTFLAG_CONTROLPOINTS | POINTFLAG_ENDSFIRST)
	{
		numloops = 2;
		flags = POINTFLAG_ENDPOINTS | POINTFLAG_ENDSFIRST;
	}
	else
		numloops = 1;

	INT32 count;
	for (count=1; count<=numloops; count++ )
	{
		INT32 i;
		for (i=0;i<UsedSlots ;i++ )
		{
			// check to see if it's an endpoint or control point, and if we want 
			// this kind of point
			if (
				(PFlags[i].IsEndPoint && (flags & POINTFLAG_ENDPOINTS)) 
			    || 
			    (!(PFlags[i].IsEndPoint) && (flags & POINTFLAG_CONTROLPOINTS) && PFlags[i].IsSelected)
			   )
			{
				if (BlobRect.ContainsCoord(Coords[i]))
				{
					*position = i;
					return(TRUE);
				}
			}

		}
		// Now we've looked at the endpoints, let's look at the control points
		// if we were only doing one pass anyway, this line has no effect
		flags = POINTFLAG_CONTROLPOINTS;
	}


*****/



/********************************************************************************************

>	double Path::SqrDistanceToPoint(const DocCoord pt, INT32* NearEl, double* mu )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		pt		= DocCoord of point of interest (eg MouseCoord)
   	Outputs:	NearEl	= path index pointer of closest element to be retured
				mu		= element parameter of closest point to be returned
	Returns:	a double, being the closest distance on the path
				or -1 if there are no points in the path, mu being unaffected
	Purpose:	Finds the distance to the closest point on the path. For efficiency
				it does not evaluate the closest point. Use this routine in conjunction
				with ClosestPoint to find the actual closest point.

********************************************************************************************/
 
double Path::SqrDistanceToPoint(const DocCoord pt, INT32* NearEl, double* mu)
{
	// Get the coordinate list and verb list for this path instance

	DocCoord* Coords = GetCoordArray();
	PathVerb* Verbs = GetVerbArray();

	// check for the presence of at least some elements

	if (UsedSlots==0)
	{
		*NearEl = -1;
		return -1;
	}

	// calculate the distance to the first control point

	double fdist;
	double ldist;
	double tmu;

	INT32 el=-1;

	fdist = PathUtil::SqrDistance(Coords[0], pt);

	for (INT32 i=0; i<UsedSlots; i++)
	{
		switch (Verbs[i] & ~PT_CLOSEFIGURE)
		{

			case PT_LINETO:
				ldist = PathUtil::SqrDistanceToLine(&Coords[i-1], pt, &tmu);
				if (ldist<=fdist) 
				{
					fdist=ldist;
					el=i;
					*mu=tmu;
				} 
				break;

			case PT_BEZIERTO:
			 	ldist = PathUtil::SqrDistanceToCurve(MAXPATHDIFFRATE, &Coords[i-1], pt, &tmu);
				if (ldist<=fdist) 
				{
					fdist=ldist;
					el=i;
					*mu=tmu;
				}
				i+=2;							// skip next two bezierto el.
				break;
		}
	}

	if (el>-1)									// have we an element? 
	{
		*NearEl = el;
		return fdist;
	}

	*NearEl = -1;
	return -1;
}



/********************************************************************************************

>	DocCoord Path::ClosestPointTo(const double t, const INT32 index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		t		 = closest distance parameter
				index	 = closest distance path element index
	Outputs:
	Returns:	DocCoord, the closest point on the path
	Purpose:	This function returns the point at which the closest distance on the path
				occurs.
	Errors:		Ensures the indexed path element is either a lineto or bezierto
	
********************************************************************************************/

DocCoord Path::ClosestPointTo(const double t, const INT32 index)
{
	// Get the coord list and verb list for this path instance

	DocCoord* Coords = GetCoordArray();
	PathVerb* Verbs = GetVerbArray();
	PathVerb Verb = Verbs[index] & ~PT_CLOSEFIGURE;

	if ((t==0) && (index==0))
		return Coords[0];

	ENSURE(UsedSlots>1, "Not enough elements in path");
	ENSURE(index>=0, "invalid formal parameter: index");
	ENSURE((Verb == PT_LINETO) || (Verb == PT_BEZIERTO), 
			"Unable to find point on path. Index does not specify line or curve");

	if (index<UsedSlots)
	{
		switch (Verb)
		{
			case PT_LINETO:
				return PathUtil::PointOnLine(t,&Coords[index-1]);
				break;
			case PT_BEZIERTO:
				return PathUtil::PointOnCurve(t,&Coords[index-1]);
				break;
		}
	}

	// code should not really get here! if it does, return the first element
	// of the curve

	return (Coords[0]);
}





/********************************************************************************************

>	BOOL Path::IsPointCloseTo(const DocCoord ICoord, 
							  INT32 range,
							  INT32* CloseElement, 
							  double* mu )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		ICoord 			= DocCoord of point of interest (eg MouseCoord)
				range			= specifes the squared perpendicular distance away from
								  the path forming a region inside which a ICoord is said
								  to be 'in range'
	Outputs:	NearElement  	= path index pointer of closest element to be retured
				mu				= element parameter of closest point to be returned
	Returns:	True if ICoord is in range.
	Purpose:	This routine can be used to determin whether a point is close enough to
				a path to allow some action to occur. The range input parameter can be
				used to determin the size of the sensitive area.

********************************************************************************************/

BOOL Path::IsPointCloseTo(const DocCoord ICoord,
						  INT32 range,
						  INT32* NearElement, 
						  double* mu )
{
	double dist = SqrDistanceToPoint(ICoord, NearElement, mu);
//	if (IsUserName("Jim")) TRACE( _T("Distance from path = %f\n"),dist);
	if (dist<0) return FALSE;					// return false (not close to path)
	return (dist<range);
}





/********************************************************************************************

>	BOOL Path::SplitAtPoint(const DocCoord SplitPt,
							INT32* SplitElement,
							INT32* NumElements,
							PathVerb* Verbs,
							DocCoord* Coords)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		SplitPt		 = DocCoord to use to split the path as close as possible to

	Outputs:	SplitElement = The element index at which the split occured
				NumElements	 = The number of elements generated by the split operation
				Verbs		 = a list of NumElements path verbs.
				Coords		 = a list of NumElements doc coords generated by the split

	Returns:	TRUE	= if the path could be split, 
				FALSE	= if SplitPt was closest to an already existing control vertex
						  and hence is unable to split the element.

	Purpose:	This function finds the element (line or curve) on which the closest point to
				SplitPt occurs.	A parameter t describes the point on this element and is used
				to split the element into two distinct pieces. The verbs and coordinates of
				these pieces are returned to the caller in the output lists. NumElements will
				describe how many path components have been created and stored in the lists
				This will be a value of 2 for lines and	6 for curves.

********************************************************************************************/

BOOL Path::SplitAtPoint(const DocCoord SplitPt,
						INT32* SplitElement,
						UINT32* NumElements,
						PathVerb* Verbs,
						DocCoord* Coords)

{
	double mu;
	double dist = SqrDistanceToPoint(SplitPt, SplitElement, &mu);
	if (dist < 0.0) return FALSE;

	PathVerb* InVerbs = GetVerbArray();
	DocCoord* InCoords = GetCoordArray();

	PathVerb vb = InVerbs[*SplitElement] & ~PT_CLOSEFIGURE;
	ENSURE((vb == PT_LINETO) || (vb == PT_BEZIERTO), 
			"Unable to find point on path. Index does not specify line or curve");

	switch (vb)
	{
		case PT_LINETO:
			return PathUtil::SplitLine(mu, &InCoords[(*SplitElement)-1], NumElements, Verbs, Coords);
			break;
		case PT_BEZIERTO:
			return PathUtil::SplitCurve(mu, &InCoords[(*SplitElement)-1], NumElements, Verbs, Coords);
			break;
	}

	return FALSE;
}



/********************************************************************************************

>	BOOL Path::PointCloseToLine(DocCoord pos, INT32* position)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/94
	Inputs:		pos - mouse position to be tested
	Outputs:	position - index to the point that was matched
						   unchanged if no match
	Returns:	TRUE if a match was found, FALSE if not
	Purpose:	This function takes the coordinate pos, and sees if it is close
				to a line (close enough to drag the line around). It returns
				TRUE if the point is close enough, and position is an index to
				the element that it was closest to

				Since this routine requires maths beyond that of which I am
				capable, it returns FALSE at the moment

********************************************************************************************/

BOOL Path::PointCloseToLine(DocCoord pos, INT32* position)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARLIB)
/*
	double d;
	INT32 Near;
	double Param;

	d = SqrDistanceToPoint(pos, &Near, &Param);

	TRACE( _T("Index=%d\n"),Near);

	DocCoord coord;
	coord = ClosestPointTo(Param, Near);

	TRACE( _T("x=%d, y=%d\n"),coord.x,coord.y);

 */

	DocRect temp;
	GetApplication()->GetBlobManager()->GetBlobRect(DocCoord(0,0),&temp);
	double mu;
	INT32 width = (temp.Width())/2;	
	return (IsPointCloseTo(pos, width*width, position, &mu));

//	return(FALSE);
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL Path::ClosestSelectedEndpoint(DocCoord pos, INT32* position, double* distance)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/94
	Inputs:		pos - mouse position to be tested
	Outputs:	position - 	index to the point that was matched
							unchanged if no match
				distance - 	distance between the specified point and the
							closest selected endpoint (unchanged if there are none)
	Returns:	TRUE if a match was found, FALSE if not
	Purpose:	This function finds the closest selected line end to the
				cursor position passed in. If the path has no selected line
				ends, the function returns FALSE

********************************************************************************************/

BOOL Path::ClosestSelectedEndpoint(DocCoord pos, INT32* position, double* distance)
{
	// Get arrays for this path
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	PathFlags* PFlags = (PathFlags*) DescribeHandle(FlagsHandle);

	double nearest = 0;
	double tempdistance = 0;
	BOOL foundany = FALSE;

	// Scan from the end of the path to the start. We remember the last point in the subpath
	// and go back until we hit a MoveTo. then we look at both the elements, and if the 
	// CloseFigure flag isn't set in the end element, the subpath must be open, in which
	// case we look at the endpoints and see if they are closer to pos than others.

	INT32 endpoint = UsedSlots-1;
	for (INT32 i=UsedSlots-1;i>=0;i--)
	{
		if (Verbs[i] == PT_MOVETO)
		{
			// only check the ends if CLOSEFIGURE is not set in the end verb
			if (!(Verbs[endpoint] & PT_CLOSEFIGURE))
			{
				// first check the moveto
				tempdistance = Coords[i].Distance(pos);
				if ((PFlags[i].IsSelected) && (!foundany || (nearest>tempdistance)))
				{
					*position = i;
					nearest = tempdistance;
					foundany = TRUE;
				}

				// Now check the end point
				tempdistance = Coords[endpoint].Distance(pos);
				if ((PFlags[endpoint].IsSelected) && (!foundany || (nearest>tempdistance)))
				{
					*position = endpoint;
					nearest = tempdistance;
					foundany = TRUE;
				}
			}
			endpoint = i-1;
		}
	}
	return(foundany);
	
}


/********************************************************************************************

>	BOOL Path::CloneFrom(const Path& Other)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		Other = pointer to a path to copy data from
	Returns		TRUE if successful
				FALSE if unable in increase the size of this path to cope with all
				data in 'Other'
	Purpose:	Replace the data within this path with that of Other path

********************************************************************************************/

BOOL Path::CloneFrom(const Path& Other)
{
	// Make space to copy all the data from the other path into this one
	INT32 SlotsToCopy = Other.UsedSlots;
	INT32 SlotsToAdd  = SlotsToCopy-UsedSlots;

	if (SlotsToAdd>0)
	{
		if (!MakeSpaceAtEnd(SlotsToAdd))
			return FALSE;
	}

	// We will need all the arrays from the other path
	DocCoord*  SCoords = (DocCoord*)  DescribeHandle(Other.CoordHandle);
	PathVerb*  SVerbs  = (PathVerb*)  DescribeHandle(Other.VerbHandle);
	PathFlags* SFlags  = (PathFlags*) DescribeHandle(Other.FlagsHandle);

	// Get the destination arrays
	DocCoord*  DCoords = (DocCoord*)  DescribeHandle(CoordHandle);
	PathVerb*  DVerbs  = (PathVerb*)  DescribeHandle(VerbHandle);
	PathFlags* DFlags  = (PathFlags*) DescribeHandle(FlagsHandle);

	// copy all arrays from the source to destination
	if (SlotsToCopy>0)
	{
		if (SCoords != NULL) memmove((void*)(DCoords), (void*)(SCoords), SlotsToCopy*sizeof(DocCoord));
		if (SVerbs  != NULL) memmove((void*)(DVerbs),  (void*)(SVerbs),  SlotsToCopy*sizeof(PathVerb));
		if (SFlags  != NULL) memmove((void*)(DFlags),  (void*)(SFlags),  SlotsToCopy*sizeof(PathFlags));
	}

	// Now update the array variables
	UsedSlots   += SlotsToAdd;
	UnUsedSlots -= SlotsToAdd;
	CurrentPos 	 = UsedSlots;

	return TRUE;
}





/********************************************************************************************

>	BOOL Path::RetroReplaceSection(INT32 StartSlot, INT32 NumSlots, Path* NewPath, BOOL KeepStart)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/94
	Inputs:		StartSlot - The slot to start replacing from
				NumSlots - The number of slots to replace
				NewPath - The new path to place in the path to replace the elements from
				StartSlot to StartSlot+NumSlots
				KeepStart - TRUE if the initial moveto of the NewPath should be pasted
				into the path, FALSE if you want to throw away the initial moveto.
	Returns:	TRUE if it worked, FALSE if it failed (Could not alloc memory if NewPath
				is bigger than the section it was replacing
	Purpose:	Deletes all the elements in the path from StartSlot to StartSlot+NumSlots
				and then inserts the whole of NewPath into the hole. If the hole is not
				big enough, then it is made bigger and if it is too big, then it is shrunk.

********************************************************************************************/

BOOL Path::RetroReplaceSection(INT32 StartSlot, INT32 Len, Path* NewPath, BOOL KeepStart)
{
	// Saftey Checks
	ENSURE( StartSlot<UsedSlots, "PathPosition is off end of path in Path::RetroReplaceSection" );
	ENSURE( StartSlot>=0, "PathPosition is off beginning of path in Path::RetroReplaceSection" );

	// Make enough space in the path to copy the new path in after the marked section has been removed
	INT32 SlotsToAdd = NewPath->UsedSlots - Len;
	if (SlotsToAdd>0)
	{
		// We need more space as the new path is bigger than the gap
		CurrentPos = StartSlot+1;
		if (!MakeSpaceInPath(SlotsToAdd))
			// Failed to get the memory, so fail
			return FALSE;

		// Update the usage counters
		UsedSlots += SlotsToAdd;
		UnUsedSlots -= SlotsToAdd;
	}

	// Dereference the pointers
	PathVerb*  Verbs  = (PathVerb*)  DescribeHandle(VerbHandle);
	DocCoord*  Coords = (DocCoord*)  DescribeHandle(CoordHandle);
	PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);

	// If the section to be replaced is bigger than the new path, then we should move the end of
	// the path down a bit
	if (SlotsToAdd<0)
	{
		// Find out how much memory to move about
		INT32 SlotToMoveFrom = StartSlot+Len;
		INT32 SlotToMoveTo   = StartSlot+NewPath->UsedSlots;
		INT32 SlotsToMove = UsedSlots-SlotToMoveFrom;

		// Does it need moving
		if (SlotsToMove>0)
		{
			// yep, move it
			memmove((void*)(&Verbs[SlotToMoveTo]), (void*)(&Verbs[SlotToMoveFrom]), SlotsToMove*sizeof(PathVerb));
			memmove((void*)(&Flags[SlotToMoveTo]), (void*)(&Flags[SlotToMoveFrom]), SlotsToMove*sizeof(PathFlags));
			memmove((void*)(&Coords[SlotToMoveTo]), (void*)(&Coords[SlotToMoveFrom]), SlotsToMove*sizeof(DocCoord));
		}

		// Change the path flags (SlotsToAdd is negative in here)
		UsedSlots += SlotsToAdd;
		UnUsedSlots -= SlotsToAdd;
	}

	// Find out about the pointers to the new path
	PathVerb*  NewVerbs  = (PathVerb*)  DescribeHandle(NewPath->VerbHandle);
	DocCoord*  NewCoords = (DocCoord*)  DescribeHandle(NewPath->CoordHandle);
	PathFlags* NewFlags  = (PathFlags*) DescribeHandle(NewPath->FlagsHandle);

	// Now we have to copy NewPath into the hole
	INT32 ReadStart, ReadLen, WriteStart;

	// Work out which section we are copying
	if (KeepStart)
	{
		// Here we are keeping the initial moveto in the replacement path
		ReadStart = 0;
		ReadLen = NewPath->UsedSlots;
		WriteStart = StartSlot;
	}
	else
	{
		// Here we are throwing away the Moveto in the replacement path
		ReadStart = 1;
		ReadLen = NewPath->UsedSlots-1;
		WriteStart = StartSlot+1;
	}

	// Go though the path replacing all the relavent sections
	for (INT32 i=ReadStart; i<ReadStart+ReadLen; i++)
	{
		// Copy the data from the new path into the old path
		Verbs[WriteStart] = NewVerbs[i];
		Flags[WriteStart] = NewFlags[i];
		Coords[WriteStart] = NewCoords[i];

		// increment the destination slot
		WriteStart++;		
	}

	// See if this is a closed shape
	TryToClose();

	// All worked, so return happy
	return TRUE;
}


/******************************************************************************************

>	BOOL Path::SmoothRegions1(const double error, BOOL smoothall, BOOL reselect)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		error		= a double describing the max error to use when smoothing
				smoothall	= true then ignore selected points within this path and
							  smooth the whole path.
				reselect	= a boolean indicating whether the path sections should be 
							  re selected after smoothing

	Outputs:    Operates on this path. the path will be smoothed meaning that it
	            will change in an unpredictable manner. The number of elements in
	            the path and control point positioning will change, if TRUE is
	            returned.

	Returns:	FALSE	then unable to smooth the curve
				TRUE	then the path has been smoothed successfully

	Purpose:	Given a path, search for regions of interest and smooth these regions.
				Regions are defined by sequences of connected selected points ie
				
					if P is the set of all control points [p0,....,pn]
					then a region Q is a connected subset of P , [pi....pj]
				    where all members of Q are selected.

				If smoothall is true the whole path is smoothed.	
	            You should call this function with a path which you do not mind being
	            corrupted, ie changed in an unpredictable way. The function may
	            successfully smooth a number of regions within the path, then fail on
	            one particular region. The result will be a path with m out of n regions
	            smoothed.

******************************************************************************************/

BOOL Path::SmoothRegions(const double error, BOOL smoothall, BOOL reselect)
{
	INT32 index = 0;
	INT32 start = 0;
	INT32 end = UsedSlots-1;

	if (smoothall)
	{
		INT32 sel;
		(reselect) ? sel=4 : sel=0;
    	return (Path::SmoothSection(start,&end,error, sel));
	}
	else
	{		
		while ((index<UsedSlots) && (FindSelectionLimits(index,&start,&end)))
		{
			// increase the region by 1 either way and smooth it.
			INT32 nstart = start;
			INT32 nend = end;

			ExpandRange(&nstart,&nend,1);

			if (nstart != nend)
			{
				INT32 sel = 0;
				if (reselect)
				{
					if (nstart<start) sel |=1 ;
					if (nend>end) sel |=2 ;
					if (sel==0) sel=4;
				}
				BOOL ok = Path::SmoothSection(nstart, &nend, error, sel);
				if (!ok) return FALSE;
			}
			index = nend+1;
		}
	}
	return TRUE;
}


/******************************************************************************************

>	BOOL Path::SmoothRegions(const double error, BOOL smoothall)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		error		= a double describing the max error to use when smoothing
				smoothall	= true then ignore selected points within this path and
							  smooth the whole path.

	Outputs:    Operates on this path. the path will be smoothed meaning that it
	            will change in an unpredictable manner. The number of elements in
	            the path and control point positioning will change, if TRUE is
	            returned.

	Returns:	FALSE 	then unable to smooth the curve
				TRUE 	then the path has been smoothed successfully

	Purpose:	Given a path, search for regions of interest and smooth these regions.
				Regions are defined by (start/end) selected points with any number of
				none selected points inbetween.  
				If smoothall is true or there are no selected regions within the path,
				the whole path is smoothed.
	            You should call this function with a path which you do not mind being
	            corrupted, ie changed in an unpredictable way. The function may
	            successfully smooth a number of regions within the path, then fail on
	            one particular region. The result will be a path with m out of n regions
	            smoothed.

******************************************************************************************/
/*
BOOL Path::SmoothRegions(const double error, BOOL smoothall)
{

	// ok, hunt through the path for a section to smooth
	// if we cannot find a section, smooth the whole lot.

	INT32 first = 0;
	INT32 last = UsedSlots-1;
    INT32 nregions = 0;
	INT32 numselpoints = 0;
    BOOL ok = TRUE;

	if (!smoothall)
		numselpoints = GetNumSelEndPoints();

	INT32 count = numselpoints;

	switch (numselpoints)
    {
    	case 0:
		case 1:
	    	return (Path::SmoothSection(0,&last,error));
    		break;

        default:
			ok = FindNextSelected(&first);
			while (ok)
			{
				count--;
				// find the next region of interest
				last = first+1;
			    ok = FindNextSelected(&last);
                if (ok)
                {
					count--;
					if (count == 1)
					{
						last++;
	                	ok = FindNextSelected(&last);
	                    if (ok)
	                    	count--;
					}

					if (ok)
					{	
						ok = Path::SmoothSection(first, &last, error);
						if (ok)
						{
			   				first = last+1;
					   		ok = FindNextSelected(&first);
						}
					}
				}
			}
			break;
	}
	if (count == 0) 
		return TRUE;
	return FALSE;
}

*/


/******************************************************************************************

>	BOOL Path::SmoothSection(INT32 index1, INT32* index2, const double error, BOOL reselect)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		index1		= path element index to begin smoothing from.
				index2  	= path element index to smooth to.
							  -1 then smooth to the end of the path.
				error   	= a double describing the max error to use when smoothing
				selection	= 0 then do not reselect the smoothed regions
							= 1 then reselect all but the first point in the section
							= 2 then reselect all but the last point in the section
								a boolean indicating whether the smoothed region should
							= 3 then reselect all but both end points
							= 4 reselect all the points in the section

    Outputs:    index2  = the new index of the end of the smoothed section
                this    = the altered path, smoothed between index1, index2
				
    Returns:    BOOL	= TRUE if the path was smoothed successfully
    					= FALSE then the path remains unchanged

    Purpose:    Smooth a path between the given section delimited by [index1,index2].

******************************************************************************************/

BOOL Path::SmoothSection(INT32 index1, INT32* index2, const double error, INT32 selection)
{
	INT32 last = *index2;
	if (last==-1)
		last = UsedSlots-1;

	// Saftey Checks
	ERROR3IF( index1>=last, "Section delimiters are invalid in Path::SmoothSection");
	ERROR3IF( last>=UsedSlots, "Section end is off end of path in Path::SmoothSection");
	ERROR3IF( index1<0, "Section start is negative in Path::SmoothSection");

	// Release safety checks
	if (index1>=last)
		return FALSE;
	if (last>=UsedSlots)
		return FALSE;
	if (index1<0)
		return FALSE;

	INT32 numtosmooth = last-index1+1;

 	// We are smoothing a section of the path so extract a copy from
	// the specified section.
	Path PathSection;
	BOOL ok = PathSection.Initialise(numtosmooth,12);

	if (ok)
	{
		ok = MakePathFromSection(index1, numtosmooth, &PathSection);
		if (ok) 
		{
			// smooth the section completely
			ok = PathSection.Smooth(error);

			if (ok)
			{
		    	// stick the smoothed path back in as a section and return a new
			  	// index2 value
				ok = RetroReplaceSection(index1, numtosmooth, &PathSection, FALSE);
				if (ok)
				{
					// set up the last coordinate index return param
					*index2 = index1 + PathSection.GetNumCoords() - 1;

					// if we need to reselect the points then go to it
					if (selection)
					{
						PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
						INT32 start = index1;
						INT32 end = *index2;
						if (!(selection & 4))
						{
							if (selection & 1)
								FindNextEndPoint(&start);
							if (selection & 2)
								FindPrev(&end);
						}
						for (INT32 i=start; i<=end; i++)
							Flags[i].IsSelected = TRUE;
					}
				}
			}
		}
	}
	return ok;
}


/********************************************************************************************

>	BOOL Path::MakePathFromSection(const INT32 Start, const INT32 Length, Path* pDestin)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		Start	= Position within this path to copy data from
				Length	= Number of items to copy
				pDestin	= pointer to path to put copied section in.
				
	Returns:	TRUE if the copy was a success, FALSE if it failed (lack of memory)
	Purpose:	Copies a specified section from this path to a specific position within
				another path. A gap is created if necessary to copy the data into

********************************************************************************************/

BOOL Path::MakePathFromSection(const INT32 Start, const INT32 Length, Path* pDestin)
{
	// Validate the input params
	ERROR3IF(Start>(UsedSlots-1),"MakePathFromSection given an illegal start index");
	if (Start > (UsedSlots-1))
		return FALSE;

	ERROR3IF((Start+Length)>UsedSlots,"MakePathFromSection given an illegal Length");
	if ((Start+Length) > UsedSlots)
		return FALSE;

//	PathVerb* SourceVerbs = GetVerbArray();

	BOOL ok = MergeSectionTo(Start, Length, pDestin, 0);
	if (ok)
	{
		PathVerb* pDestinVerbs = pDestin->GetVerbArray();
		pDestinVerbs[0] = PT_MOVETO;
	}
	return ok;
}





/******************************************************************************************

>	BOOL Path::Smooth(const double error)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
    Inputs:  	error   = a double describing the max error to use when smoothing

    Outputs:    Operates on this path. the path will be smoothed meaning that it
	            will change in an unpredictable manner. The number of elements in
	            the path and control point positioning will change, if TRUE is
	            returned.

    Returns:    BOOL	= TRUE if the path was smoothed successfully
    					= FALSE then consider 'this' path has been corrupted
    					
    Purpose:    Smooth a path, using the fitcurve function, which may turn out to be NOT
    			the way to do it but I'll give it a try. You should generally call this
				funcion with a copy of the path you want to smooth. This smooth function
				will corrupt the path if it fails.
				Call the function SmoothSection() describing your entire path if you
				wish to smooth a path without corrupting anything on failure.
	SeeAlso:	SmoothSection

******************************************************************************************/

BOOL Path::Smooth(const double error)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARLIB)
	double QuantOrder = 1024;

	INT32 orignumcoords = UsedSlots;

 	// create a path and quantise to it
	Path QuantPath;
	QuantPath.Initialise(24,12);

	// quantise original path to QuantPath
    Quantise(QuantOrder, &QuantPath);

	// create and fit a smooth curve to the data points.
    CurveFitObject Smoothed(&QuantPath, error);
    BOOL ok = Smoothed.Initialise(&QuantPath,QuantPath.GetNumCoords());

    if (ok)
	{
		Smoothed.FitCurve();

		// now that we've smoothed the curve lets check on the number
		// of coordinates we've generated. If its more, then we should
		// leave the original path as it was. If its less then replace
		if (QuantPath.GetNumCoords()<orignumcoords)
			ok = CloneFrom(QuantPath);
		
	}
	return ok;
#else
	return FALSE;
#endif
}



/******************************************************************************************

>	BOOL Path::Quantise(const double threshold, Path* pOutput)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
    Inputs:		threshold	= How quantised the curves should be (around 64 is good)
    Outputs:    pOutput		= will contain the quantised path   
    Returns:    BOOL		= TRUE if the path was flattened successfully
    						= FALSE then output path will be unaffected
    					
    Purpose:    Quantise a path. All curves within the path will be approximated by
				straight lines. pOutput may point at 'this' path ie a path can be
				quantised to itself. If the quantising fails for some reason, the
				output path will be unaffected EVEN if you're quantising a path to
				itself.
				Note, Quantising differs from flattening in that it will approximate
				straight lines within a path, generating n colinear points for each
				line it meets. Whereas flattening does nothing with lines.
    
******************************************************************************************/

BOOL Path::Quantise(const double threshold, Path* pOutput)
{
	// when flattening, flatten curves but dont quantise lines
	ProcessFlags QuantFlags(TRUE,TRUE);
	return CreateFlatPath(QuantFlags, threshold, pOutput);
}


/******************************************************************************************

>	BOOL Path::Flatten(const double flatness, Path* pOutput)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
    Inputs:		flatness	= How flat the curves should be (around 64 is good)
    Outputs:    pOutput		= will contain the flattened path   
    Returns:    BOOL		= TRUE if the path was flattened successfully
    						= FALSE then output path will be unaffected
    					
    Purpose:    Flatten a path. All curves within the path will be approximated by
				straight lines. pOutput may point at 'this' path ie a path can be
				flattened to itself. If the flattening fails for some reason, the
				output path will be unaffected even if you are flattening a path to
				itself.
    
******************************************************************************************/

BOOL Path::Flatten(const double flatness, Path* pOutput)
{
	// when flattening, flatten curves but dont quantise lines
	ProcessFlags FlatFlags(TRUE,FALSE);
	return CreateFlatPath(FlatFlags, flatness, pOutput);
}


/******************************************************************************************

>	BOOL Path::CreateFlatPath(ProcessFlags PFlags, const double flatness, Path* pOutput)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
    Inputs:		PFlags		= process flags, to control the path processor function
							  (see classes ProcessPath and ProcessFlags)
    		  	flatness	= How flat the curves should be (around 64 is good)
    
    Outputs:    pOutput		= will contain the flattened path   
    
    Returns:    BOOL		= TRUE if the path was flattened successfully
    						= FALSE then output path will be unaffected
    					
    Purpose:    Flatten a path. All curves within the path will be approximated by
				straight lines. pOutput may point at 'this' path ie a path can be
				flattened to itself. If the flattening fails for some reason, the
				output path will be unaffected even if you're flattening a path to
				itself.
    
******************************************************************************************/

BOOL Path::CreateFlatPath(const ProcessFlags& PFlags, const double flatness, Path* pOutput)
{

	// create a flattening object
	ProcessFlatten FlattenObj(flatness);
//	INT32 numcoords = pOutput->GetNumCoords();

	// ok, generate a flattened path on the end of Output
	return FlattenObj.FlattenPath(PFlags, this, pOutput);

}



/******************************************************************************************
>	double Path::SqrLength()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
    Inputs:		-
    Outputs:	-		
    Returns:    The squared length of the path defined in the same coordinate system as 
    			the paths points.		
    Purpose:    Find the length of a path in millipoints. The length is calculated by
				flattening the path and finding straight line segment lengths and so is
				a discrete numeric approximation to a line integral.
******************************************************************************************/
double Path::SqrLength()
{
	double Acc = 64;
	double Len = 0;
	ProcessLength LengthObj(Acc);


	BOOL ok = LengthObj.PathLength(this, &Len);
	if (ok)
		return Len;
	return 0;

}



/******************************************************************************************
>	double Path::GetPathLength(double dFlatness = 64.0)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
    Inputs:		-
    Outputs:	-		
    Returns:	The length of the path in millipoints
    Purpose:	Find the length of a path in millipoints.
******************************************************************************************/
double Path::GetPathLength(double dFlatness)
{
	double TotalLength = 0.0;
	INT32 LastEndPoint = GetNumCoords()-1;
	
	if (LastEndPoint==-1)
		return 0.0;

	BOOL ok = DistanceTo(LastEndPoint, 1.0, &TotalLength, dFlatness);
	ERROR3IF(!ok, "Path.DistanceTo failed");

	if (ok)
		return TotalLength;
	else
		return 0.0;
}



/********************************************************************************************

>	DocCoord Path::ConvSqrDistToCoord(const double sqrdist)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Inputs:		sqrdist	= squared distance along the path
	Outputs:	
	Returns:	a doccoord which is dist units along the path
	Purpose:	Finds the coordinate which is dist units along 'this' path


********************************************************************************************/
 
DocCoord Path::ConvSqrDistToCoord(const double sqrdist)
{
	INT32 Index;
	double mu = DistReparameterise(sqrdist, &Index);
	return ClosestPointTo(mu, Index); 	
}



/********************************************************************************************

>	double Path::DistReparameterise(const double sqrdist, INT32* Element)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Inputs:		sqrdist	= squared distance along the path
	Outputs:	Element = index of element which bounds dist 
						  as a half open interval, (El.Plo < dist <= El.Phi)
	Returns:	a double, being the parameter along the returned element, 
				which is dist units along the whole path. 
	Purpose:
	SeeAlso:	ConvDistToCoord()
				ClosestPointTo()

********************************************************************************************/

double Path::DistReparameterise(const double sqrdist, INT32* Element)
{
	// set up a distance object and call its processor
	double Acc = 64;
	ProcessDistance DistanceObj(Acc);
	return DistanceObj.PathDistance(sqrdist, this, Element);
}




/********************************************************************************************

>	INT32	Path::CalcCRC()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		-
	Returns:	The CRC checksum for the path, relative to the first coord
	Purpose:	Calculates a cyclic redundancy check value for a path.
				This CRC is invariant under translation only.
				It can be used to determine whether a new path matches that of an old
				path whose CRC we already know, whether the new path has been translated
				or not.

********************************************************************************************/

INT32	Path::CalcCRC()
{
	return CalcRegionCRC(0,UsedSlots-1);
}


/********************************************************************************************

>	INT32 Path::CalcRegionCRC(const INT32 StartIndex, const INT32 EndIndex)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>/Mike
	Created:	11/11/94
	Inputs:		StartIndex = index into coord array to start calculating from
				EndIndex   = index into coord array to end calculation (inclusive)
	Returns:	The CRC checksum for the region, relative to StartIndex
	Purpose:	Calculates a cyclic redundancy check value for a path region.
				This CRC is only invariant under translation.
				It can be used to determine whether a new path matches that of an old
				path whose CRC we already know, whether the new path has been translated
				or not.

********************************************************************************************/

INT32 Path::CalcRegionCRC(const INT32 StartIndex, const INT32 EndIndex)
{
	INT32 TotalCRC = 0;
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);

	for (INT32 i=StartIndex; i<(EndIndex+1); i++)
	{
		// Get the coords relative from the first coord
		// making this process transparent to translation
		// Add it in to the CRC total
		TotalCRC += (Coords[i].x-Coords[StartIndex].x) + 
					(Coords[i].y-Coords[StartIndex].y);
	}
	return TotalCRC;
}



/********************************************************************************************

>	INT32	Path::CalcSelectedCRC()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		-
	Returns:	The CRC checksum for the path, relative to the first coord
	Purpose:	Calculates a cyclic redundancy check value for a paths selected coordinates.
				This CRC is invariant under translation only.
				It can be used to determine whether a new path matches that of an old
				path whose CRC we already know, whether the new path has been translated
				or not.

********************************************************************************************/

INT32	Path::CalcSelectedCRC()
{
	return CalcSelPointsCRC(0,UsedSlots-1);
}


/********************************************************************************************

>	INT32 Path::CalcSelPointsCRC(const INT32 StartIndex, const INT32 EndIndex)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>/Mike
	Created:	11/11/94
	Inputs:		StartIndex = index into coord array to start calculating from
				EndIndex   = index into coord array to end calculation (inclusive)
	Returns:	The CRC checksum for the region, relative to StartIndex
	Purpose:	Calculates a cyclic redundancy check value for a paths selected coordinates.
				This CRC is invariant under translation only.
				It can be used to determine whether a new path matches that of an old
				path whose CRC we already know, whether the new path has been translated
				or not.
	
********************************************************************************************/

INT32 Path::CalcSelPointsCRC(const INT32 StartIndex, const INT32 EndIndex)
{
	INT32 TotalCRC = 0;
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);
	PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);

	for (INT32 i=StartIndex; i<(EndIndex+1); i++)
	{
		if (Flags[i].IsSelected)
			TotalCRC += (Coords[i].x-Coords[StartIndex].x) + 
						(Coords[i].y-Coords[StartIndex].y);
	}
	return TotalCRC;
}


/********************************************************************************************

>	void Path::Translate(const DocCoord& coord)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		x = x coordinate to translate path by
				y = y coordinate to translate path by
	Returns:	
	Purpose:	Translate all coordinate within 'this' path.

********************************************************************************************/

void Path::Translate(const DocCoord& coord)
{
	INT32 x=coord.x;
	INT32 y=coord.y;
	Translate(x,y);
}


/********************************************************************************************

>	void Path::Translate(const INT32 x, const INT32 y)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		x = x coordinate to translate path by
				y = y coordinate to translate path by
	Returns:	
	Purpose:	Translate all coordinate within 'this' path.

********************************************************************************************/

void Path::Translate(const INT32 x, const INT32 y)
{
	// if there's any translation then apply it
	if (x || y)
	{
		DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);

		for (INT32 i=0; i<UsedSlots; i++)
		{
			Coords[i].x += x;
			Coords[i].y += y;
		}
	}
}



/********************************************************************************************

>	BOOL Path::InitExtraInfo(ChannelIndex Index)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Inputs:		The Index of the Channel to Initialise.
	Returns:	TRUE if we got all the memory we needed, FALSE if not
	Purpose:	Allocates memory in the path for the extra info arrays.
	Errors:		May error if it runs out of memory.

********************************************************************************************/

BOOL Path::InitExtraInfo(ChannelIndex Index)
{
	TRACEUSER( "Will", _T("InitExtraInfo, Index=%d\n"), Index );
	ENSURE(Index <= NUMEXTRACHANNELS, "Index out of range in InitExtraInfo");
						
	// If we haven't any extra info yet then we need to create some
	if (ExtraInfo == NULL)
	{
		ExtraInfo =  new PathExtraInfo();
	 	if (ExtraInfo == NULL)
			return FALSE;
	}

	// Now we can initialise the channel.
	// We use the same initial slot size as was used for the main path data.
	return ExtraInfo->Init(Index, SlotInitSize);
}

/********************************************************************************************

>	BOOL Path::AddExtraInfo(ChannelIndex Index, INT32 ExtraValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Inputs:		-
	Returns:	TRUE if all went well, FALSE if not
	Purpose:	Adds an extra Info Value at the current path position.
	Errors:		Can fail if it runs out of memory

********************************************************************************************/

BOOL Path::AddExtraInfo(ChannelIndex Index, INT32 ExtraValue)
{
	ENSURE(ExtraInfo != NULL, "ExtraInfo Pointer is NULL in AddExtraInfo");
	return ExtraInfo->Add(Index, ExtraValue);
}

/********************************************************************************************

>	void Path::SyncExtraInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Inputs:		-
	Returns:	-
	Purpose:	Sets the Extra Info Ptr to the current Path Position.
	Errors:		-

********************************************************************************************/

void Path::SyncExtraInfo()
{
	if (ExtraInfo == NULL)
		return;

	ExtraInfo->Sync(CurrentPos);
}

/********************************************************************************************

>	PathWidth Path::GetWidth() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/94
	Inputs:		-
	Returns:	The width of the path at the current path postion.
	Purpose:	Fetches width values from the path.
	Errors:		-

********************************************************************************************/

PathWidth Path::GetWidth() const
{
	if (ExtraInfo == NULL)
		return 0;

	return ExtraInfo->GetWidthInfo();	
}

/********************************************************************************************

>	PathWidth* Path::GetWidthArray()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/94
	Returns:	A pointer to an array of Path Width Info.
	Purpose:	The array holds the width of all the points in the path.
				DO NOT MODIFY THE CONTENTS OF THE ARRAY.

********************************************************************************************/

PathWidth* Path::GetWidthArray()
{
	if (ExtraInfo == NULL)
		return NULL;

	return ExtraInfo->GetWidthArray();	
}

/********************************************************************************************

>	BOOL Path::HasWidth()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/94
	Returns:	TRUE if width information is available.
	Purpose:	To determine if the path has any width information.

********************************************************************************************/

BOOL Path::HasWidth()
{
	if (ExtraInfo == NULL)
		return FALSE;

	return ExtraInfo->HasWidthInfo();
}

/********************************************************************************************

>	void Path::MapWidth(ChannelIndex Index)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/94
	Returns:	-
	Purpose:	Alters the mapping of the Width Information to one of the ExtraInfo channels.

********************************************************************************************/

void Path::MapWidth(ChannelIndex Index)
{
	ENSURE(Index <= NUMEXTRACHANNELS, "Index out of range in MapWidth()");

	ExtraInfo->MapWidthInfo(Index);
}












/********************************************************************************************

	PathExtraInfo

********************************************************************************************/

/********************************************************************************************

>	PathExtraInfo::PathExtraInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/94
	Inputs:		-
	Returns:	-
	Purpose:	Constructor for PathExtraInfo class.
	Errors:		-

********************************************************************************************/

PathExtraInfo::PathExtraInfo()
{
	CurrentExtraPos = 0;
	WidthChannel = CI_PRESSURE;		// Default mapping of Pressure to Width

	INT32 i;
	for (i = 0; i < NUMEXTRACHANNELS; ++i)
		ExtraInfoHandles[i] = BAD_MHANDLE;		// Clear all the handles.

	for (i = 0; i < NUMEXTRACHANNELS; ++i)
		Scaling[i] = 1;							// Clear all the scaling values.

// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	// Set the Pressure scaling so that the Maximum pressure is always stored as EXTRAVALUEMAX
	//if ((Camelot.GetPressurePen())->IsPressureOn())
	//	Scaling[CI_PRESSURE] = fixed16((INT32)EXTRAVALUEMAX/((INT32)(Camelot.GetPressurePen())->GetPressureMax()));
#endif // VECTOR_STROKING
}

/********************************************************************************************

>	PathExtraInfo::~PathExtraInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/94
	Inputs:		-
	Returns:	-
	Purpose:	Destructor for PathExtraInfo class.
	Errors:		-

********************************************************************************************/

PathExtraInfo::~PathExtraInfo()
{
	// Scan thru all the extra channels that we know of, and release and blocks
	// that we have claimed.
	for (INT32 Index = 0; Index < NUMEXTRACHANNELS; ++Index)
	{
		// Have we claimed a block for this channel ?
		if (ExtraInfoHandles[Index] != BAD_MHANDLE)
		{
			ReleaseBlock(ExtraInfoHandles[Index]);
			TRACEUSER( "Will", _T("Released Block with Index=%d\n"),Index);
		}
	}
}

/********************************************************************************************

>	BOOL PathExtraInfo::Init(ChannelIndex Index, INT32 SlotInitSize)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Inputs:		The Index of the channel to Init, and the initial size for it.
	Returns:	TRUE if we got all the memory we needed, FALSE if not
	Purpose:	Allocates memory in the path for the extra info arrays.
	Errors:		Can fail if it runs out of memory

********************************************************************************************/

BOOL PathExtraInfo::Init(ChannelIndex Index, INT32 SlotInitSize)
{
	TRACEUSER( "Will", _T("InitExtraInfo, Index=%d\n"), Index );
	ENSURE(Index <= NUMEXTRACHANNELS, "Index out of range in InitExtraInfo");
						
	if (ExtraInfoHandles[Index] != BAD_MHANDLE)
		return TRUE;								// Already Initialised

	// Claim an initial block of memory for this Index.
	ExtraInfoHandles[Index] = ClaimBlock(sizeof(PathExtraElement)*SlotInitSize);

	// Check that the block claimed ok.
	return (ExtraInfoHandles[Index] != BAD_MHANDLE);
}

/********************************************************************************************

>	BOOL PathExtraInfo::Add(ChannelIndex Index, INT32 ExtraValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Inputs:		The channel to add to, and the value to add.
	Returns:	TRUE if all went well, FALSE if not
	Purpose:	Adds an extra Info Value at the current path position.
				The value stored will be scaled so that it is between 0 and 65536.
	Errors:		-

********************************************************************************************/

BOOL PathExtraInfo::Add(ChannelIndex Index, INT32 ExtraValue)
{
	TRACEUSER( "Will", _T("AddExtraInfo, Index=%d, Value=%d\n"),Index, ExtraValue);
	ENSURE(Index <= NUMEXTRACHANNELS, "Index out of range in PathExtraInfo::Add()");

	// get pointer to info array
	PathExtraElement* Info = (PathExtraElement*) DescribeHandle(ExtraInfoHandles[Index]);

	// Wop the info in the array at the postion coresponding to the last point inserted.
	Info[CurrentExtraPos] = XLONG(ExtraValue) * Scaling[Index];

	return TRUE;
}

/********************************************************************************************

>	void PathExtraInfo::Sync(INT32 CurrentPos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Inputs:		The path position to set the extrainfo ptr to.
	Returns:	-
	Purpose:	Sets the Extra Info Ptr to the current Path Postion.
	Errors:		-

********************************************************************************************/

void PathExtraInfo::Sync(INT32 CurrentPos)
{
	TRACEUSER( "Will", _T("SyncExtraInfo, Pos=%d\n"),CurrentPos);
	CurrentExtraPos = CurrentPos;
}

/********************************************************************************************

>	BOOL PathExtraInfo::IncreaseExtraBlocks(INT32 SlotsNeeded)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Returns:	TRUE is the Blocks were increase correctly.
	Purpose:	Allocates more memory for extra info.

********************************************************************************************/

BOOL PathExtraInfo::IncreaseExtraBlocks(INT32 SlotsNeeded)
{
	TRACEUSER( "Will", _T("IncreaseExtraBlocks, Slots=%d\n"),SlotsNeeded);

	// Scan thru all our channels and increase their size by 'SlotsNeeded'.
	for (INT32 Index = 0; Index < NUMEXTRACHANNELS; ++Index)
	{
		if (ExtraInfoHandles[Index] != BAD_MHANDLE)
			if (!IncreaseBlock(ExtraInfoHandles[Index], sizeof(PathExtraElement)*SlotsNeeded))
				return FALSE;
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL PathExtraInfo::DecreaseExtraBlocks(INT32 SlotsLost)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Returns:	TRUE is the Blocks were increase correctly.
	Purpose:	De-allocates memory used for extra info.

********************************************************************************************/

BOOL PathExtraInfo::DecreaseExtraBlocks(INT32 SlotsLost)
{
	TRACEUSER( "Will", _T("DecreaseExtraBlocks, Slots=%d\n"),SlotsLost);

	// Scan thru all our channels and decrease their size by 'SlotsNeeded'.
	for (INT32 Index = 0; Index < NUMEXTRACHANNELS; ++Index)
	{
		if (ExtraInfoHandles[Index] != BAD_MHANDLE)
			if (!DecreaseBlock(ExtraInfoHandles[Index], sizeof(PathExtraElement)*SlotsLost))
				return FALSE;
	}
	return TRUE;
}

/********************************************************************************************

>	void PathExtraInfo::ShiftUpExtraInfo(INT32 CurrentPos, INT32 NumSlots, INT32 SlotsToMove)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Returns:	-
	Purpose:	Shifts up the ExtraInfo memory to make room for more.

********************************************************************************************/

void PathExtraInfo::ShiftUpExtraInfo(INT32 CurrentPos, INT32 NumSlots, INT32 SlotsToMove)
{
	TRACEUSER( "Will", _T("ShiftUpExtraInfo, NumSlots=%d, SlotsToMove=%d\n"),NumSlots, SlotsToMove);

	// The channel data has been resized and so we need to shift it around a bit.
	for (INT32 Index = 0; Index < NUMEXTRACHANNELS; ++Index)
	{
		if (ExtraInfoHandles[Index] != BAD_MHANDLE)
		{
			PathExtraElement* ExtraInfoPtr = (PathExtraElement*) DescribeHandle(ExtraInfoHandles[Index]);
			memmove( (void*)&ExtraInfoPtr[CurrentPos+NumSlots], (void*)&ExtraInfoPtr[CurrentPos], SlotsToMove*sizeof(PathExtraElement) );
		}
	}

}

/********************************************************************************************

>	void PathExtraInfo::ShiftDownExtraInfo(INT32 StartSlot, INT32 NumSlots, INT32 SlotsToMove)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Returns:	-
	Purpose:	Shifts down the ExtraInfo memory when it gets shrunk.

********************************************************************************************/

void PathExtraInfo::ShiftDownExtraInfo(INT32 StartSlot, INT32 NumSlots, INT32 SlotsToMove)
{
	TRACEUSER( "Will", _T("ShiftDownExtraInfo, Start=%d, NumSlots=%d, SlotsToMove=%d\n"),
			StartSlot, NumSlots, SlotsToMove);

	// The channel data has been resized and so we need to shift it around a bit.
	for (INT32 Index = 0; Index < NUMEXTRACHANNELS; ++Index)
	{
		if (ExtraInfoHandles[Index] != BAD_MHANDLE)
		{
			PathExtraElement* ExtraInfoPtr = (PathExtraElement*) DescribeHandle(ExtraInfoHandles[Index]);
			memmove( (void*)&ExtraInfoPtr[StartSlot], (void*)&ExtraInfoPtr[StartSlot+NumSlots], SlotsToMove*sizeof(PathExtraElement) );
		}
	}

}

/********************************************************************************************

>	void PathExtraInfo::CopyExtraInfo(PathExtraInfo* SrcInfo)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Returns:	-
	Purpose:	Copies all the ExtraInfo from another path.

********************************************************************************************/

void PathExtraInfo::CopyExtraInfo(PathExtraInfo* SrcInfo)
{
	ENSURE(FALSE, "Warning. Path Extra Info lost in CopyExtraInfo()");
}

// Extra Info Mapping Functions

/********************************************************************************************

>	void PathExtraInfo::MapWidthInfo(ChannelIndex Index)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/94
	Returns:	-
	Purpose:	Alters the mapping of the Width Information to one of the ExtraInfo channels.

********************************************************************************************/

void PathExtraInfo::MapWidthInfo(ChannelIndex Index)
{
	ENSURE(Index <= NUMEXTRACHANNELS, "Index out of range in MapWidthInfo()");
	WidthChannel = Index;
}

/********************************************************************************************

>	PathWidth PathExtraInfo::GetWidthInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/94
	Returns:	-
	Purpose:	Get the width at the current path position.

********************************************************************************************/

PathWidth PathExtraInfo::GetWidthInfo()
{
	// de-reference the Extra Info array
	PathExtraElement* WidthInfo = (PathExtraElement*) DescribeHandle(ExtraInfoHandles[WidthChannel]);

	return (PathWidth)WidthInfo[CurrentExtraPos];	
}

/********************************************************************************************

>	PathWidth* PathExtraInfo::GetWidthArray()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/94
	Returns:	A pointer to an array of Path Width Information
	Purpose:	The array holds the width of the path.

********************************************************************************************/

PathWidth* PathExtraInfo::GetWidthArray()
{
	// de-reference an Extra Info array
	return (PathWidth*) DescribeHandle(ExtraInfoHandles[WidthChannel]);
}

/********************************************************************************************

>	BOOL PathExtraInfo::HasWidthInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/94
	Returns:	TRUE if width information is available.
	Purpose:	To determine if the path has any width information.

********************************************************************************************/

BOOL PathExtraInfo::HasWidthInfo()
{
	return (WidthChannel!=-1);
}

/********************************************************************************************

>	BOOL Path::CheckPathValid()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Returns:	TRUE if the path was OK, FALSE if there was something wrong with it
	Purpose:	Tests a path to see if it is valid. This is only built into DEBUG versions
				of Camelot so always surround calls to it with an ifdef _DEBUG block.
				If the path is not valid in some way, this function will dump an explanation
				to the debug terminal and return FALSE.

********************************************************************************************/

#ifdef _DEBUG

BOOL Path::CheckPathValid()
{
	BOOL IsValid = TRUE;

	// Go through the path making sure that it is a valid path
	PathVerb*  Verbs  = (PathVerb*)  DescribeHandle(VerbHandle);
//	DocCoord*  Coords = (DocCoord*)  DescribeHandle(CoordHandle);
//	PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);

	// Make sure there is at least a single lineto in the path
	if (UsedSlots<2)
	{
		TRACE( _T("Path too short! It had less than 2 components (ie one or none!)"));
		IsValid = FALSE;
	}

	// Make sure that the path does not end in a MoveTo
	if (Verbs[UsedSlots-1]==PT_MOVETO)
	{
		TRACE( _T("The last element of the path was a MoveTo. This is bad..."));
		IsValid = FALSE;
	}

	// make sure that the first element in the path is a MoveTo
	if (Verbs[0]!=PT_MOVETO)
	{
		TRACE( _T("First element of the path is NOT a MoveTo. This is really bad"));
		IsValid = FALSE;
	}

	INT32 CurveCount = 0;
	for (INT32 i=0; i<UsedSlots; i++)
	{
		// make sure that CurveTos come in groups of 3
		if ((Verbs[i] & ~PT_CLOSEFIGURE)!=PT_BEZIERTO)
		{
			if ((CurveCount!=0) && ((CurveCount%3)!=0))
			{
				TRACE( _T("CurveTo elements were not a multiple of 3 (%d)"), CurveCount);
				IsValid = FALSE;
			}

			CurveCount = 0;
		}
		else
			CurveCount++;

		// see if there is a MoveTo after a close figure
		if ((Verbs[i] & PT_CLOSEFIGURE) && (i<UsedSlots-1) && (Verbs[i+1]!=PT_MOVETO))
		{
			TRACE( _T("A Close Figure was not followed by the end of the path or a MoveTo"));
			IsValid = FALSE;
		}
	}

	if ((CurveCount!=0) && ((CurveCount%3)!=0))
	{
		TRACE( _T("CurveTo elements were not a multiple of 3 (%d)"), CurveCount);
		IsValid = FALSE;
	}

	if (IsValid == FALSE)
		// Put a break point here to trap all the invalid paths
		TRACE( _T("This path has failed the Validity Check"));

	// we passed all the tests, so return TRUE
	return IsValid;
}

#endif

/********************************************************************************************

>	INT32 Path::BreakInTwo(Path* pChildPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Inputs:		pChildPath, a pointer to a split path which should have been passed to new()
	Outputs:	-
	Returns:   -1		If unable to claim enough memory to split the path.
						Note pChildPath is unchanged and 'this' is also unaffected
				0		If no selected points were found in the path.
				1		Then the current path is opened at the first selected point.
						pChildPath is unaffected.
				2		Then the current path now contains the left child data of the
						split and pChildPath points at a new path object containing
						the right child data. 
								
	Purpose:	This function will attempt to break the current path at its first
				selected point.
				All selected points should still be in place in both parent and child.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


INT32 Path::BreakInTwo(Path* pChildPath)
{
	// Get arrays of flags,verbs and points.

	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);

	// p,q indexes to selected point and endof(sub)path
	INT32 p = -1;
	INT32 q = -1;
	INT32 s = 0;

	// vp,vq local verb holders
	PathVerb vp;
	PathVerb vq;

	while (p++, FindNextSelected(&p))
	{
		q = p;

        FindEndElOfSubPath(&q);							// returns index of last element in this subpath/path
        if (p>q) return 0;								// just a quick check on sanity.

		s = p;
		FindStartOfSubPath(&s);							// grab the start of the subpath

		if (p==q)
		{
			if (!(Verbs[p] & PT_CLOSEFIGURE)) continue;	// ignore selected points at end of open subpaths
			p=s;
		}	

		vp = (Verbs[p] & ~PT_CLOSEFIGURE);
		vq = (Verbs[q]);


		// Have we found a selected start point?
		if (vp == PT_MOVETO)
		{
			if (vq & PT_CLOSEFIGURE)
			{
				// subpath figure is closed, so open the path and resolve flags
				Verbs[q] &= ~PT_CLOSEFIGURE;
				if (Flags[q].IsSelected)
				{
					Flags[q].IsSelected = FALSE;
					Flags[q].IsSmooth = FALSE;
					if (Verbs[q] == PT_BEZIERTO)
					{
						Flags[q-1].IsSelected = FALSE;
						Flags[q-1].IsSmooth = FALSE;
					}
					if (Verbs[p+1] == PT_BEZIERTO)
						Flags[p+1].IsSmooth = FALSE;
				}
				return 1;
			}
			continue;
		}

		// Note these elements should not need to be checked against PT_CLOSEFIGURE as
		// (p!=q) and q is the following close element.
		
		if ((vp == PT_LINETO) || (vp == PT_BEZIERTO))
		{

			// ok, we have a selected element on a lineto/curveto. We need to check
			// whether we are in a closed subpath or an open one

			if ( vq & PT_CLOSEFIGURE )					// is this sub path closed?
            {

				// We need to reorder the subpath points and open the path.
				// This involves
				// (1) insert moveto(p) at p+1
				// (2) increment q
				// (3) remove closefigure at q
				// (4) replace moveto(s) by lineto(s)
				// (5) rotate the whole path left/right until element p
				//     lies at element s

				// subpath figure is closed, so open the path in parent

				if ((Coords[s].x == Coords[q].x) && (Coords[s].y == Coords[q].y))
				{

					// I'm not totally convinced about this but here goes nothing!
					// When dealing with a chain of bezier control points, the previous 
					// and following points are selected also. What we select and deselect
					// depends on which point (the start or end) we want selected when the
					// path is opened. I thought it would be sensible to leave the end point
					// selected but the click logic then does not allow you to drag the damn
					// point around! as it still lies on top of the deselected start point.
					// So lets try to select the new moveto and see what happens.

					Flags[p].IsSelected = FALSE;			// deselect the current control point
					Flags[p].IsSmooth = FALSE;
					if (vp == PT_BEZIERTO)
					{
						Flags[p-1].IsSelected = FALSE;
						Flags[p-1].IsSmooth = FALSE;
					}

					CurrentPos = s;
					DeleteElement();						// Get rid of the original moveto
					CurrentPos = p;
					InsertMoveTo(Coords[p-1]);

					Flags = (PathFlags*) DescribeHandle(FlagsHandle);
					Verbs = (PathVerb*) DescribeHandle(VerbHandle);

					Flags[p].IsSelected = TRUE;				// make sure the moveto is selected
					Flags[p].IsSmooth = FALSE;				// but not smooth

					if ((Verbs[p+1] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
						Flags[p+1].IsSmooth = FALSE;

					Verbs[q] &= ~PT_CLOSEFIGURE;			// open the subpath.
					RotateElementsLeft(s,q,p-s);			// rotate elements 
					return 1;
				}
				else
				{

					// this section definitely needs more work!

					CurrentPos = p+1;
					if (!InsertMoveTo(Coords[p]))
						return -1;
				
					q++;

					Verbs = (PathVerb*) DescribeHandle(VerbHandle);
					Verbs[q] &= ~PT_CLOSEFIGURE;			// open the subpath.
					Verbs[s] = PT_LINETO;					// alter move to line
					RotateElementsLeft(s,q,p-s+1);			// rotate elements 
					return 1;
				}
			}
			else
			{
				// I need to alter the current path object and create a completely new
				// path object in the tree. This will contain the split section of the
				// parent path. This involves

				// (1) create newpath(p => q) by
				// 	 (1.1) creating enough space for elements (p-q + 1)
				//   (1.2) copying elements in
				//   (1.3) replacing verb(p) by moveto
				// (2) removal of elements (p+1 => q)

				// Initialise the path with enough space for splinter

				q = UsedSlots-1;

				if (!pChildPath->Initialise(q-p+1,12))	
					return -1;

				// Copy the section out of the current path into the child	
				if (!(CopySectionTo(pChildPath, p, q-p+1)))
					return -1;

				// Alter the childs first element once split, to a moveto.
				PathVerb* pChildVerbs = (PathVerb*) DescribeHandle(pChildPath->VerbHandle);
				PathFlags* pChildFlags = (PathFlags*) DescribeHandle(pChildPath->FlagsHandle);
				pChildVerbs[0] = PT_MOVETO;

				// Make sure the child paths first element is no longer smooth
				pChildFlags[0].IsSmooth = FALSE;
				if (pChildVerbs[1] == PT_BEZIERTO)
					pChildFlags[1].IsSmooth = FALSE;

				// Make sure the parent subpath start is deselected. It may have been selected
				// by the previous instructions from a previous iteration.
				Flags[s].IsSelected = FALSE;
				if (Verbs[s] == PT_BEZIERTO)
					Flags[s+1].IsSelected = FALSE;

				// Remove the parents end point smooth bit, but leave it selected
				Flags[p].IsSmooth = FALSE;
				if (vp == PT_BEZIERTO)
					Flags[p-1].IsSmooth = FALSE;

				// ok remove the necessary elements from the original path
				DeleteFromElement(p+1);

				return 2;
			}
		}
	}
	return 0;
}





/********************************************************************************************

>	void Path::RotateElementsLeft(INT32* Start, INT32 End, INT32 Rotate)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/09/94
	Inputs:		Start	= The index of the first element in the component list
				End		= The index	of the last element in the component list
				Rotate	= The number of positions to rotate left by.
						  This may be negative, in which case the path data will
						  be rotated right by the specified amount.
	Outputs:	-
	Returns:	-
	Purpose:	Rotates the specified elements in a path by	the number of positions
				indicated. Elements will be rotated from right to left.
		
	
********************************************************************************************/

void Path::RotateElementsLeft(const INT32 Start, const INT32 End, INT32 Rotate)
{
	// just make sure the start and end positions are sensible

	INT32 Width = End-Start+1;
	if (Width<2) return;
	Rotate = Rotate % Width;
	if (Rotate==0) return;
	if (Start<0) return;
	if (End>=UsedSlots) return;

	// make a rotate right into a rotate left
	if (Rotate<0) Rotate=Width-Rotate;

	INT32 Shift = 64;
	INT32 Step = Rotate/Shift;
	INT32 Residual = Rotate % Shift;

	UINT32 Size = sizeof(PathVerb);
	if (Size<sizeof(PathFlags)) Size=sizeof(PathFlags);
	if (Size<sizeof(DocCoord)) Size=sizeof(DocCoord);

	void* TempBuff = (void*)CCMalloc(Shift*Size);

	// Get arrays of flags,verbs and points.
	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);
	DocCoord* Coords = (DocCoord*) DescribeHandle(CoordHandle);

	if (TempBuff!=NULL)
	{
		void* Destin;
		void* Source;
		while (Step>0)
		{
			Destin = TempBuff;
			Source = (void*)&Flags[Start];
			memmove(Destin, Source, Shift*sizeof(PathFlags));
			Destin = (void*)&Flags[Start];
			Source = (void*)&Flags[Start+Shift];
			memmove(Destin, Source, (Width-Shift)*sizeof(PathFlags));
			Destin = (void*)&Flags[End-Shift+1];
			Source = TempBuff;
			memmove(Destin, Source, Shift*sizeof(PathFlags));

			Destin = TempBuff;
			Source = (void*)&Verbs[Start];
			memmove(Destin, Source, Shift*sizeof(PathVerb));
			Destin = (void*)&Verbs[Start];
			Source = (void*)&Verbs[Start+Shift];
			memmove(Destin, Source, (Width-Shift)*sizeof(PathVerb));
			Destin = (void*)&Verbs[End-Shift+1];
			Source = TempBuff;
			memmove(Destin, Source, Shift*sizeof(PathVerb));

			Destin = TempBuff;
			Source = (void*)&Coords[Start];
			memmove(Destin, Source, Shift*sizeof(DocCoord));
			Destin = (void*)&Coords[Start];
			Source = (void*)&Coords[Start+Shift];
			memmove(Destin, Source, (Width-Shift)*sizeof(DocCoord));
			Destin = (void*)&Coords[End-Shift+1];
			Source = TempBuff;
			memmove(Destin, Source, Shift*sizeof(DocCoord));

			Step--;
		}
		if (Residual>0)
		{

			Destin = TempBuff;
			Source = (void*)&Flags[Start];
			memmove(Destin, Source, Residual*sizeof(PathFlags));
			Destin = (void*)&Flags[Start];
			Source = (void*)&Flags[Start+Residual];
			memmove(Destin, Source, (Width-Residual)*sizeof(PathFlags));
			Destin = (void*)&Flags[End-Residual+1];
			Source = TempBuff;
			memmove(Destin, Source, Residual*sizeof(PathFlags));

			Destin = TempBuff;
			Source = (void*)&Verbs[Start];
			memmove(Destin, Source, Residual*sizeof(PathVerb));
			Destin = (void*)&Verbs[Start];
			Source = (void*)&Verbs[Start+Residual];
			memmove(Destin, Source, (Width-Residual)*sizeof(PathVerb));
			Destin = (void*)&Verbs[End-Residual+1];
			Source = TempBuff;
			memmove(Destin, Source, Residual*sizeof(PathVerb));

			Destin = TempBuff;
			Source = (void*)&Coords[Start];
			memmove(Destin, Source, Residual*sizeof(DocCoord));
			Destin = (void*)&Coords[Start];
			Source = (void*)&Coords[Start+Residual];
			memmove(Destin, Source, (Width-Residual)*sizeof(DocCoord));
			Destin = (void*)&Coords[End-Residual+1];
			Source = TempBuff;
			memmove(Destin, Source, Residual*sizeof(DocCoord));

		}
		CCFree(TempBuff);
	}
	else
	{
		PathFlags TempFlags;
		PathVerb TempVerb;
		DocCoord TempCoord;

		while (Rotate>0)
		{
			TempFlags = Flags[Start];
			TempVerb = Verbs[Start];
			TempCoord =	Coords[Start];
			for (INT32 i=Start; i<End; i++)
			{
				Flags[i] = Flags[i+1];
				Verbs[i] = Verbs[i+1];
				Coords[i] = Coords[i+1];
			}
			Flags[End]=TempFlags;
			Verbs[End]=TempVerb;
			Coords[End]=TempCoord;
			Rotate--;
		}
	}
}

/********************************************************************************************

>	BOOL Path::ChangeStartElement(INT32 StartIndex)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/11/94
	Inputs:		StartIndex - The index to the element in the path that you want to be the start
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	This will rotate the path so that the element that was at StartIndex is now at
				index 0.
				You should only call this for closed paths that don't contain subpaths.
	Errors:		In debug builds, an error is given if the last coord != first coord
	
********************************************************************************************/

BOOL Path::ChangeStartElement(INT32 StartIndex)
{
	if (StartIndex < 1 || StartIndex >= UsedSlots)
	{
		ERROR3_PF(("StartIndex out of range : %ld",StartIndex));
		return FALSE;
	}

	DocCoord*  pCoords = GetCoordArray();
	PathVerb*  pVerbs  = GetVerbArray();
	PathFlags* pFlags  = GetFlagArray();

	ERROR3IF(pCoords[0] != pCoords[UsedSlots-1],"Not closed because first coord != last coord");

	// Make sure the last verb has the PT_CLOSEFIGURE flag cleared
	pVerbs[UsedSlots-1] = pVerbs[UsedSlots-1] & ~PT_CLOSEFIGURE;

	// Delete the MoveTo
	CurrentPos = 0;
	DeleteElement();

	ERROR3IF(UnUsedSlots < 1,"DeleteElement() didn't leave me any unused slots");
	if (UnUsedSlots < 1) return FALSE;

	// Rotate the element at StartIndex to index 0
	RotateElementsLeft(0,UsedSlots-1,StartIndex-1);

	// We need a duplicate element type & verb to finish the path off, so copy on end of arrays
	pCoords[UsedSlots] = pCoords[0];
	pVerbs [UsedSlots] = pVerbs [0] | PT_CLOSEFIGURE;	// Make sure last verb has also got this flag set
	pFlags [UsedSlots] = pFlags [0];

	// The first element is always a MoveTo
	pVerbs[0] = PT_MOVETO;

	// Update the path vars.
	UsedSlots++;
	UnUsedSlots--;

	return TRUE;
}


/********************************************************************************************

>	BOOL Path::FindNextSelected(INT32* Index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/09/94
	Inputs:		Index - The position in the path to start looking from
	Outputs:	Index - the position of the next selected element 
	Returns:	TRUE	if there exists a selected point at or beyond index
				FALSE	if there is no more selected points along the path
	Purpose:	Searches the point set {*index, usedslots-1} for a selected end point
				and returns its index if found.  
	
********************************************************************************************/

BOOL Path::FindNextSelected(INT32* Index)
{
	ENSURE( (*Index)<=UsedSlots, "Path position was not valid in FindNextSelected" );
	ENSURE( (*Index)>=0, "Path Position less than zero in FindNextSelected" );

	PathFlags* Flags = (PathFlags*) DescribeHandle(FlagsHandle);

	while (   
			((*Index)<UsedSlots) && 
			!(	(Flags[(*Index)].IsSelected) && 
	        	(Flags[*Index].IsEndPoint)
	         )
	      )
			(*Index)++;
	
	return ((*Index)<UsedSlots);
	
}
	
/********************************************************************************************

>	void Path::FindEndElOfSubPath(INT32* Index) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/94
	Inputs:		Index - The position in the path to start looking from
	Outputs:	Index - the position of the last element of the subpath, ie the final bezier
						control point, or the final line to etc.
	Purpose:	Searches forwards until it finds the last element of the current sub-path.
	 			ie
				  0  1  2  3
				{MT,CT,CT,CT} => Index = 3.
	See Also:	FindEndOfSubPath (Note equivelent)

********************************************************************************************/

void Path::FindEndElOfSubPath(INT32* Index) const
{
	ENSURE( (*Index)<UsedSlots, "Path position was not valid in FindEndOfSubPath" );
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	do
	{
		(*Index)++;
	} while (((*Index)<UsedSlots) && (Verbs[(*Index)]!=PT_MOVETO));

	(*Index)--;
}



/********************************************************************************************

> INT32 Path::NumSplinters()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com> (& Markn)
	Created:	4/9/94
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Returns the number of splinters a selected path will be broken into
				when calls to BreakPath is called. Splinters include opened subpaths,
				ie if the algorithm will open a closed subpath, this is classed as a
				spinter along with breaking a path in two.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 Path::NumSplinters()
{
	// New implementation by Markn 15/2/95
	// This version will return 2 when there's a selected point in the middle of a line.  The old code
	// returned 1 in this case

	INT32 Index = -1;
	INT32 EndIndex = -1;
	INT32 LastEndIndex = -1;
	INT32 NumSplinters = 0;
	BOOL SubPathOpen;

	PathVerb* pVerbs = GetVerbArray();

	while (Index++, FindNextSelected(&Index))
	{
		PathVerb Verb = pVerbs[Index] & ~PT_CLOSEFIGURE;

		// Ignore selected moveto elements
		if (Verb == PT_MOVETO)
			continue;

		EndIndex = Index;
        FindEndElOfSubPath(&EndIndex);	// returns index of last element in this subpath/path
		SubPathOpen = !(pVerbs[EndIndex] & PT_CLOSEFIGURE);

		// ignore selected end points when the subpath is open
		if (Index == EndIndex && SubPathOpen)
			continue;

		NumSplinters++;

		if (EndIndex != LastEndIndex)
		{
			LastEndIndex = EndIndex;

			// if we have found a selected point in the middle of an open subpath
			// an extra splinter will be produced.
			if (SubPathOpen)
				NumSplinters++;
		}
	}

	return NumSplinters;

/*
	// Get array of verbs
	PathVerb* Verbs = (PathVerb*) DescribeHandle(VerbHandle);

	// p,q indexes to selected point and endof(sub)path
	// vp,vq local verb holders

	INT32 p = -1;
	INT32 q = -1;
	PathVerb vp;
	PathVerb vq;

	INT32 splinters = 0;
	while (p++, FindNextSelected(&p))
	{
		q = p;

        FindEndElOfSubPath(&q);							// returns index of last element in this subpath/path
        if (p>q) break;									// just a quick check on sanity.

		if (p==q)
		{
			if (!(Verbs[p] & PT_CLOSEFIGURE)) continue;	// ignore selected points at end of open subpaths
		}	

		vp = (Verbs[p] & ~PT_CLOSEFIGURE);
		vq = (Verbs[q]);

		if (vp==PT_MOVETO)
		{
			if (!(vq & PT_CLOSEFIGURE))					// is this sub path already open?
			{
				p++;									// move forward past moveto
				continue;								// continue do loop
			}
		}

		if ((vp == PT_MOVETO) || (vp == PT_LINETO) || (vp == PT_BEZIERTO))
			splinters++;
	}	

	return splinters;
*/
}



/********************************************************************************************

>	BOOL Path::EnsureValid(BOOL* ChangesMade = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		Pointer to a BOOL to return a result in (you can pass in NULL)
	Outputs:	ChangesMade will be TRUE if changes have been made to the path, FALSE if otherwise
	Returns:	TRUE for success, FALSE for failure (see below)
	Purpose:	Will attempt to make a path into a correct state.  Checks are :-  
				1. All subpaths must start with a MoveTo
				2. Consecutive MoveTos are removed
				3. Zero element paths are unrepairable
				4. MoveTos at the end of the path are removed
				5. Should be three BezierTos in a row, only the last can be CloseFigure
				6. A LineTo to the same location as the previous point is removed.

	ERRORs:		FALSE is returned if the path was knackered OR if we encountered an error in
				trying to repair it.  (SetError will have been called)

********************************************************************************************/
BOOL Path::EnsureValid(BOOL* ChangesMade)
{

	BOOL PathOK = FALSE;
	BOOL Temp;
	BOOL* pChangeFlag;
	pChangeFlag = (ChangesMade == NULL) ? &Temp : ChangesMade;
	*pChangeFlag = FALSE;

	while (!PathOK)
	{
		// Recache the path pointers as things may have moved
		PathVerb*  Verbs  = (PathVerb*)  DescribeHandle(VerbHandle);
		DocCoord*  Coords = (DocCoord*)  DescribeHandle(CoordHandle);
		PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);
		INT32 NumCoords = GetNumCoords();

		// (3) Check for zero element path
		if (NumCoords == 0) 
		{
			TRACEALL( _T("Empty path detected\n") );
			Error::SetError(_R(IDE_INVALID_PATH));
			return FALSE;
		}

		INT32 Counter = 0;

		while (Counter < NumCoords)
		{
			// Recache the path pointers in case the previous pass caused the arrays to move
			Verbs  = (PathVerb*)  DescribeHandle(VerbHandle);
			Coords = (DocCoord*)  DescribeHandle(CoordHandle);
			Flags  = (PathFlags*) DescribeHandle(FlagsHandle);

			// (1) Check that all subpaths start with a MoveTo
			if ( (Counter == 0) || (Verbs[Counter-1] & PT_CLOSEFIGURE) )
			{
				if ((Verbs[Counter] & ~PT_CLOSEFIGURE) != PT_MOVETO)
				{
					// Insert a MoveTo at the same location as this point
					TRACEALL( _T("Inserting a MoveTo at %d\n"), Counter );
					SetPathPosition(Counter);
					*pChangeFlag = TRUE;
					if (!InsertMoveTo(Coords[Counter]))
						return FALSE;
					break;
				}
			}

			// (2) Check there are no two consecutive MoveTos
			if ( (Counter != NumCoords-1) && (Verbs[Counter] == PT_MOVETO) )
			{
				if (Verbs[Counter+1] == PT_MOVETO)
				{
					// Remove this MoveTo
					TRACEALL( _T("Removing a MoveTo at %d\n"), Counter );
					SetPathPosition(Counter);
					*pChangeFlag = TRUE;
					if (!DeleteElement())
						return FALSE;
					break;
				}
			}

			// (4) Check for trailing moveto at the end of a path
			if ( (Counter == NumCoords-1) && (Verbs[Counter] == PT_MOVETO) )
			{
				// Remove this MoveTo
				TRACEALL( _T("Removing a MoveTo at the end (%d)\n"), Counter );
				SetPathPosition(Counter);
				*pChangeFlag = TRUE;
				if (!DeleteElement())
					return FALSE;
				break;
			}

			switch (Verbs[Counter] & ~PT_CLOSEFIGURE)
			{
				case PT_MOVETO:	
					// MoveTos shouldn't have CloseFigures
					if (Verbs[Counter] != PT_MOVETO)
					{
						TRACEALL( _T("Detected a MoveTo+CloseFigure combination\n at %d"), Counter );
						Verbs[Counter] = PT_MOVETO;
						*pChangeFlag = TRUE;
					}
					Counter++;
					break;
				case PT_LINETO:
					// If the coord of this is the same as the last (which must be a MoveTo) then delete it
					if ((Counter > 0) && (Coords[Counter] == Coords[Counter-1]) && (Verbs[Counter-1] == PT_MOVETO))
					{
						TRACEALL( _T("Removing a LineTo at %d\n"), Counter );
						SetPathPosition(Counter);
						*pChangeFlag = TRUE;

						// If the LineTo has the PT_CLOSEFIGURE bit set, ensure that the element
						// before it has this bit set.
						// If we don't then it is possible for shapes to become lines.
						if ((Verbs[Counter] & PT_CLOSEFIGURE) && (Verbs[Counter-1] != PT_MOVETO))
							Verbs[Counter-1] |= PT_CLOSEFIGURE;

						// Delete the surplus LineTo element
						if (!DeleteElement())
							return FALSE;

						Counter = NumCoords+1;		// So we go round the entire path again
					}
					else
					{
						// This point should be endpoint
						if (!Flags[Counter].IsEndPoint)
						{
							TRACEALL( _T("Fixing endpoint status at %d\n"), Counter );
							*pChangeFlag = TRUE;
							Flags[Counter].IsEndPoint = TRUE;
						}
						Counter ++;
					}
					break;
				case PT_BEZIERTO:
					// (5) Should be three BezierTos in a row, only the last can be CloseFigure
					if (Counter >= NumCoords-2)
					{
						TRACEALL( _T("Path ends with a short Bezier section\n") );
						return FALSE;
					}
					if ( (Verbs[Counter] != PT_BEZIERTO) || (Verbs[Counter+1] != PT_BEZIERTO) ||
												((Verbs[Counter] & ~PT_CLOSEFIGURE) != PT_BEZIERTO) )
					{
						TRACEALL( _T("InvalidBezier section detected starting at %d\n"), Counter );
						return FALSE;
					}
					Flags[Counter++].IsEndPoint = FALSE;
					Flags[Counter++].IsEndPoint = FALSE;
					Flags[Counter++].IsEndPoint = TRUE;
					break;
				default:
					TRACEALL( _T("This path is *very* corrupt (Invalid verb found)\n") );
					return FALSE;
			}
			if (Counter == NumCoords)
				PathOK = TRUE;
		}
	}

	return TRUE;
}



/********************************************************************************************

>	void Path::ClearNeedToRender()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets the NeedToRender flags on all of the path elements to FALSE
	Errors:		-

********************************************************************************************/
void Path::ClearNeedToRender()
{
	PathFlags* Flags  = (PathFlags*) DescribeHandle(FlagsHandle);
	for (INT32 loop = GetNumCoords()-1; loop >= 0 ; loop--)
	{
		Flags[loop].NeedToRender = FALSE;
	}
}



/********************************************************************************************

>	BOOL Path::DistanceTo(const INT32 index, const double t, double* distance, double dFlatness=64)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/06/95
	Inputs:		index  = an index into the path arrays describing the first coordinate
						 of an element.
				t	   = a parameter 0..1 describing a point on the path element
	Outputs:	distance- a 72000 inch measurement
	Returns:	TRUE if distance was calculated
				FALSE if not
	Purpose:	This function takes a description of a point lying on a path. The point is
				described by (i,t) where i is the index of the element t lies in, and t
				is a parametric variable 0<=t<=1.
				The function calculates the distance along this path to the point described
				by (i,t).
				This routine complements SqrDistanceToPoint() which calculates the pair (i,t)
				for a given coordinate close to the path. 

	SeeAlso:	SqrDistanceToPoint
	
********************************************************************************************/
BOOL Path::DistanceTo(INT32 index, double t, double* distance, double dFlatness)
{
	PathVerb* InVerbs = GetVerbArray();
	PathVerb vb = InVerbs[index] & ~PT_CLOSEFIGURE;
	DocCoord* InCoords = GetCoordArray();
	double LoDist = 0.0;
	double HiDist = 0.0;

	if (GetNumCoords()==0)
		return FALSE;
	
	// If t is near 1 then we can skip forward to the next segment
	// this is to avoid math problems with splitting beziers near their ends
	INT32 IndexEP = index;
	if (t>=0.99995 && vb==PT_BEZIERTO)
	{
		// Move up to next complete segment
		if (IndexEP != GetNumCoords()-1)
		{
			if (!FindNextEndPoint(&IndexEP))
				return FALSE;
			t = 0.0;
		}
	}
	
	// find the length to the path up to the start of the nearest element
	// specified by index.
	ProcessLength LengthObj(dFlatness);
	if (!LengthObj.PathLength(this, &LoDist, IndexEP-1))
		return FALSE;

	// ok we need to evaluate the distance along the
	// path element i at parameter 0<=t<1
	switch (vb)
	{
		case PT_LINETO:
		{
			double TotalLength = InCoords[index-1].Distance(InCoords[index]);
			HiDist = TotalLength * t;
		}
		break;

		case PT_BEZIERTO:
		{
			if ((t>0.00005) && (t<0.99995))		// avoids problems at the extremes.
			{
				UINT32 NumElements;

				PathVerb NewVerbs[6];
				DocCoord NewCoords[6];
				Path 	 TempPath;

				BOOL	ok = TempPath.Initialise(12,12);
				if (ok) ok = PathUtil::SplitCurve(t, &InCoords[index-1], &NumElements, NewVerbs, NewCoords);
				if (ok) ok = TempPath.CopyPathDataFrom(NewCoords, NewVerbs, 3);
				if (ok)
				{
					TempPath.SetPathPosition(0);
					ok = TempPath.InsertMoveTo(InCoords[index-1]);
				}
				if (ok)
				{
					ProcessLength LengthObj2(64);
					ok = LengthObj2.PathLength(&TempPath, &HiDist);
				}
				if (!ok)
					return FALSE;
			}
		}
		break;

		default:
			ERROR3("Unable to find point on path. Index does not specify line or curve");
			break;
	}

	*distance = LoDist+HiDist;

	return TRUE;
}



/********************************************************************************************

>	BOOL Path::DistanceTo(DocCoord Coord, double* distance, double dFlatness=64.0)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/06/95
	Inputs:		Coord		- A coordinate
	Outputs:	distance	- a 72000 inch measurement
	Returns:	TRUE if distance was calculated
				FALSE if not
	Purpose:	This function takes as input a coordinate (usually a mouse click point). It
				will calculate the closest point on the path to this coordinate and return
				the distance along the path from its start point to this coordinate
	
********************************************************************************************/
BOOL Path::DistanceTo(DocCoord Coord, double* dist, double dFlatness)
{
	Path TempPath;
	INT32 SplitAt;
	INT32 NumNewEl;

	if (!TempPath.Initialise(GetNumCoords(),24))
		return FALSE;

	if (!TempPath.CopyPathDataFrom(this))
		return FALSE;

	if (!TempPath.SplitAtPoint(Coord, &SplitAt, &NumNewEl))
		return FALSE;

	ProcessLength LengthObj(dFlatness);
	if (!LengthObj.PathLength(&TempPath, dist, SplitAt))
		return FALSE;

	return TRUE;
}



/********************************************************************************************

>	BOOL Path::SplitAtPoint(const DocCoord& SplitPoint, INT32* SplitAt, INT32* NewElements)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com> (from Riks routine in OpFreeHand)
	Created:	15/06/95
	Inputs:		SplitPoint	- The coord that we want the split to happen close to
	Outputs:	SplitAt 	- The Slot number of the new end point
				NewElements - The number of new elements inserted into the path
	Returns:	TRUE if it was able to split the path ok,
				FALSE if it failed
	Purpose:	This function takes a coordinate (typically not on but) close to this path 
				and splits the path at a calculated point on the path which happens to be
				closest to the input coord. By Split I mean a new coordinate in will appear
				in the path, but the path will not have changed its shape in anyway.

********************************************************************************************/

BOOL Path::SplitAtPoint(const DocCoord& SplitPoint, INT32* SplitAt, INT32* NewElements)
{
 	INT32 		SplitElement=0;
	UINT32 		NumElements;
	PathVerb	NewVerbs[6];
	DocCoord	NewCoords[6];
	PathFlags	NewFlags[6];

	// Try and split the path. This returns FALSE if the path did not need splitting
	if (SplitAtPoint(SplitPoint, &SplitElement, &NumElements, NewVerbs, NewCoords))
	{
		// Make a note of where the new endpoint will be
		*SplitAt = (SplitElement+(NumElements/2)-1);
		*NewElements = NumElements;

		// The path was split, so we know where, and how, so let's party on the path
		PathVerb* Verbs = GetVerbArray();
		PathFlags* Flags = GetFlagArray();
//		DocCoord* Coords = GetCoordArray();
//		UINT32 NumCoords = GetNumCoords();	
		INT32 NumToChange;

		// We're adding something, either a line or curve - check which
		if ((Verbs[SplitElement] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
		{
			// Adding a curve - number to change = 6
			NumToChange = 6;

			// Initialise the flags appropriately
			for (INT32 i=0; i<6; i++)
			{
				// copy the flags from the old curve (repeat the flag in the first 3 elements)
				NewFlags[i] = Flags[SplitElement+(i%3)];
				NewFlags[i].IsSelected = FALSE;

				// Want it to be a cusp in the middle
				if ((i>0) && (i<4))
				{
					NewFlags[i].IsSmooth = FALSE;
					NewFlags[i].IsRotate = FALSE;
				}
			}

			// If this was happening at the end of a path,
			// then make sure the close figure flag is set correctly
			if (Verbs[SplitElement+2] & PT_CLOSEFIGURE)
				NewVerbs[5] |= PT_CLOSEFIGURE;
		}
		else
		{
			// The slit happened in a straight line section
			NumToChange = 2;
			NewFlags[0] = NewFlags[1] = Flags[SplitElement];
			NewFlags[0].IsSmooth = NewFlags[0].IsRotate = FALSE;
			NewFlags[0].IsSelected = FALSE;

			// Make sure that the close figure flag is maintained
			if (Verbs[SplitElement] & PT_CLOSEFIGURE)
				NewVerbs[1] |= PT_CLOSEFIGURE;
		}

		// Move to the place in the path where the split took place
		SetPathPosition(SplitElement);
		PathFlags InsertFlags;
		BOOL      InsertWorked;

		// insert a curve or a line, depending on where the split happened
		if (NumToChange==6)
			InsertWorked = InsertCurveTo(NewCoords[0], NewCoords[1], NewCoords[2], &InsertFlags);
		else
			InsertWorked = InsertLineTo(NewCoords[0], &InsertFlags);

		// see if the insertion worked
		if (!InsertWorked)
			return FALSE;
	}
	else
	{
		// else we tried to split at an existing control point (endpoint)
		// That means that there were no new endpoints
		*NewElements = 0;
		*SplitAt = (SplitElement-1);

		// See if we can find the control point in question
		DocCoord* Coords = GetCoordArray();
		INT32 NumCoords = GetNumCoords();

		// see if the split point is over the last point on the curve
		if (Coords[NumCoords-1]==SplitPoint)
			*SplitAt = NumCoords;
	}	

	// all worked
	return TRUE;
}



/********************************************************************************************
>	BOOL Path::GetPointAtDistance(MILLIPOINT Distance, DocCoord* pPoint)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/96
	Inputs:		Distance - a length along the path
	Outputs:	pPoint - the coordinate of the point that distance along the path
				pTangent = tangent at this point (can be NULL if tangent not required)
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Gets the coordinate of a point a certain distance along this path
********************************************************************************************/
BOOL Path::GetPointAtDistance(MILLIPOINT Distance, DocCoord* pPoint, double* pTangent, UINT32* pPressure)
{
	ERROR2IF(pPoint==NULL, FALSE, "NULL output parameter");
	ERROR2IF(Distance<0, FALSE, "-ve distance");

	ProcessPathDistance PathProc(64);
	BOOL Found = FALSE;
	BOOL ok = PathProc.GetCoordAndTangent(pPoint, pTangent, &Found, Distance, this, pPressure);

	return ok && Found;
}



/********************************************************************************************
>	BOOL Path::GetDistanceToPoint(DocCoord Point, MILLIPOINT* Distance)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/96
	Inputs:		Point - A DocCoord				
	Outputs:	Distance - a length along the path
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Gets the distance along the path for a given point
********************************************************************************************/
BOOL Path::GetDistanceToPoint(DocCoord Point, MILLIPOINT* Distance)
{
	ERROR2IF(Distance==NULL, FALSE, "NULL output parameter");

	INT32 NextEndpoint = 0;
	double dDistance = 0.0;
	double mu = 0.0;
	BOOL ok = (SqrDistanceToPoint(Point, &NextEndpoint, &mu) != -1);
	ERROR3IF(!ok, "SqrDistanceToPoint failed");
	if (ok)
	{
		ok = DistanceTo(NextEndpoint, mu, &dDistance);
		ERROR3IF(!ok, "Path::DistanceTo failed");
	}

	*Distance = (MILLIPOINT)dDistance;

	return ok;
}



/********************************************************************************************

>	INT32 FindNonColinear(const DocCoord CoordArray[], const UINT32& ulNumCoords)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/08/96
	Inputs:		CoordArray:	The path with which to comapre this one
				Tolerance:	The accuracy to which this path transformed to the otherPath
							may be considered equal.
	Returns:	The index in the array of the first point
				-1 if a straight line
	Purpose:	Support function for IsIsometric
				Finds the first of three points in the given CoordArray that are not in a
				straight line.

********************************************************************************************/
INT32 FindNonColinear(const DocCoord CoordArray[], const UINT32& ulNumCoords)
{
	UINT32 index = 0;
	UINT32 x1,y1,x2,y2;
	// Three points are colinear if the vectors between them have a zero cross-product

	while (index < ulNumCoords - 2)
	{
		// Might save time to work out the first two outside the loop
		// Depends on number of linear paths really
		x1 = CoordArray[index + 1].x - CoordArray[index].x;
		y1 = CoordArray[index + 1].y - CoordArray[index].y;

		x2 = CoordArray[index + 2].x - CoordArray[index + 1].x;
		y2 = CoordArray[index + 2].y - CoordArray[index + 1].y;

		if (x1 * y2 != x2 * y1)
		{
			return index;
		}
		++index;
	}

	return -1;
}


/********************************************************************************************

>	BOOL IsNear(const T& x, const T& value, const T& Tolerance)
						
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/08/96
	Inputs:		x1:			The first of the two values to compare
				x2:			The second of the two values to compare
				Tolerance:	The value within which x1 is considered to be near to x2
	Returns:	TRUE if the two values have an absolute difference greater than or equal to
				the given Tolerance
				FALSE otherwise
	Purpose:	Support function for IsIsometric
				Determines whether two values are near enough to each other to be considered
				equal.
	Notes:		This is a templated function with <class T>

********************************************************************************************/
template <class T>
inline BOOL IsNear(const T& x1, const T& x2, const T& Tolerance)
{
	return (ABS(x1 - x2) <= Tolerance) ? TRUE : FALSE;
}



/********************************************************************************************

>	BOOL IsNear(const T& x, const T& value, const T& Tolerance)
						
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/08/96
	Inputs:		x1:			The first of the two values to compare
				x2:			The second of the two values to compare
				Tolerance:	The value within which x1 is considered to be near to x2
	Returns:	TRUE if the two values have an absolute difference greater than or equal to
				the given Tolerance
				FALSE otherwise
	Purpose:	Support function for IsIsometric
				Determines whether two values are near enough to each other to be considered
				equal.
	Notes:		This is a templated function with <class T>

********************************************************************************************/

/********************************************************************************************

>	BOOL Solve3Simultaneous(const CCVector3& constsX0, const CCVector3& constsX1,
						const CCVector3& constsY0, double& v0, double& v1, double& v2)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/08/96
	Inputs:		constsX0:	First values in the 3 simultaneous equations
				constsX1:	Second values in the 3 simultaneous equations
				constsY0:	Third values in the 3 simultaneous equations
	Outputs:	v1,v2,v3:	The three solution values
	Returns:	TRUE if there were a solution
				FALSE otherwise
	Purpose:	Support function for IsIsometric
				Solves 3 simultaneous equations for the three values v1,v2,v3

********************************************************************************************/
BOOL Solve3Simultaneous(const CCVector3& constsX0, const CCVector3& constsX1, const CCVector3& constsY0, double& v0, double& v1, double& v2)
{
	const double Tolerance = 1e-10;

	double DivA = (constsX0.v0 - constsX0.v1) * (constsY0.v0 - constsY0.v2) - (constsX0.v0 - constsX0.v2) * (constsY0.v0 - constsY0.v1);
	if (IsNear(DivA, 0.0, Tolerance))
	{
		return FALSE;
	}

	double DivB = (constsY0.v0 - constsY0.v1) * (constsX0.v0 - constsX0.v2) - (constsY0.v0 - constsY0.v2) * (constsX0.v0 - constsX0.v1);
	if (IsNear(DivB, 0.0, Tolerance))
	{
		return FALSE;
	}

	v0 = ((constsY0.v0 - constsY0.v2) * (constsX1.v0 - constsX1.v1) - (constsY0.v0 - constsY0.v1) * (constsX1.v0 - constsX1.v2)) / DivA;


	v1 = ((constsX1.v0 - constsX1.v1) * (constsX0.v0 - constsX0.v2) - (constsX1.v0 - constsX1.v2) * (constsX0.v0 - constsX0.v1)) / DivB;
	

	v2 = constsX1.v0 - v0 * constsX0.v0 - v1 * constsY0.v0;

	return TRUE;
}

/*
inline void Transform(const DocCoord& xy0,
						const double& a, const double& b, const double& c, const double& d, 
						const double& e, const double& f,
					DocCoord& xy1)
{
	xy1.x = (INT32)(a * (double)xy0.x + c * (double)xy0.y + e);
	xy1.y = (INT32)(b * (double)xy0.x + d * (double)xy0.y + f);
}
*/

/********************************************************************************************

>	BOOL Path::IsIsometric(const Path& otherPath, Matrix* pTransform, 
							const double& Tolerance = 1.0)


	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/08/96
	Inputs:		OtherPath:	The path with which to compare this one
				Tolerance:	The accuracy to which this path transformed to the otherPath
							may be considered equal.
	Outputs:	pTransform:	A pointer to the resulting transformation matrix if the return
							value is TRUE
	Returns:	TRUE : this path is Isometric to the otherPath
				FALSE : if not Isometric or inaccuracy losses
	Purpose:	Determines whether two paths are isometric.
				In this context, isometric does not include true isomtric paths with
				different starting points.
				This can then be used to provide a transformation of this path, resulting in 
				the given otherPath.

********************************************************************************************/
BOOL Path::IsIsometric(const Path& OtherPath, Matrix* pTransform, const double& Tolerance) const
{
	// Check path lengths for equality
	if (GetNumCoords() != OtherPath.GetNumCoords())
	{
		// If not equal can't possibly be the same
		return FALSE;
	}
	
	// Determine the transformation matrix from the first three coords
	DocCoord*	ThisCoordArray = GetCoordArray();
	DocCoord*	OtherCoordArray = OtherPath.GetCoordArray();

	double	a, b, c, d, e, f = Tolerance;		// Matrix components

	// Find First Non-Colinear points
	INT32 First = FindNonColinear(ThisCoordArray, GetNumCoords());
	if (First == -1)
	{
		// this path was a straight line.
		// If the other one is we might be in with a chance
		First = FindNonColinear(OtherCoordArray, OtherPath.GetNumCoords());
		if (First != -1)
		{
			// Unfortunately it had a kink so it can't match
			return FALSE;
		}
		// Shears don't work with the following. But then I doubt they're that common
		DocCoord ef(- ThisCoordArray[0].x, - ThisCoordArray[0].y);

		DocCoord v1(ThisCoordArray[1].x - ThisCoordArray[0].x, ThisCoordArray[1].y - ThisCoordArray[0].y);
		DocCoord v2(OtherCoordArray[1].x - OtherCoordArray[0].x, OtherCoordArray[1].y - OtherCoordArray[0].y);
		double v1dotv2 = v1.x * v2.x + v1.y * v2.y;
		double modv1sqrd = v1.x * v1.x + v1.y * v1.y;
		double v1crossv2 = v1.x * v2.y - v1.y * v2.x;
		a = v1dotv2 / modv1sqrd;
		b = v1crossv2 / modv1sqrd;
		c = -b;
		d = a;
		e = a * (double)ef.x - b * (double)ef.y + OtherCoordArray[0].x;
		f = b * (double)ef.x + a * (double)ef.y + OtherCoordArray[0].y;
	}
	else
	{
		// We have three non-colinear points:

		/*
			In fact, we have:
				ax00 + cy00 + e = x10
				bx00 + dy00 + f = y10
			&
				ax01 + cy01 + e = x11
				bx01 + dy01 + f = y11
			&
				ax02 + cy02 + e = x12
				bx02 + dy02 + f = y12

			Hence need to solve 2 sets of simultaneous equations for (a,c,e) & (b,d,f)
		*/

		// Try to find a solution for the a,b,e part
		CCVector3	x0s(ThisCoordArray[First].x, ThisCoordArray[First + 1].x, ThisCoordArray[First + 2].x);
		CCVector3	y0s(ThisCoordArray[First].y, ThisCoordArray[First + 1].y, ThisCoordArray[First + 2].y);
		CCVector3	x1s(OtherCoordArray[First].x, OtherCoordArray[First + 1].x, OtherCoordArray[First + 2].x);

		if (!Solve3Simultaneous(x0s, x1s, y0s, a, c, e))
		{
			return FALSE;
		}

		// Try to find a solution for the (b,d,f) part
		CCVector3	y1s(OtherCoordArray[First].y, OtherCoordArray[First + 1].y, OtherCoordArray[First + 2].y);

		if (!Solve3Simultaneous(x0s, y1s, y0s, b, d, f))
		{
			return FALSE;
		}
	}

	// Determine the transformation matrix from the first three coords
	Matrix Solution(a, b, c, d, INT32(e), INT32(f));

	// Check the flags & bits	
	PathVerb*	ThisVerbArray	= GetVerbArray();
	PathVerb*	OtherVerbArray	= OtherPath.GetVerbArray();

	INT32 lTolerance = (INT32)Tolerance;
	// For each remaining coordinate in this path, check we can transform to the otherPath
	DocCoord CurrentCoord;
	for (INT32 i = 0; i < GetNumCoords(); ++i)
	{
//		Transform(ThisCoordArray[i], a, b, c, d, e, f, CurrentCoord);

		// Compare the flags & bits (unlikely to be different if coords same though)
		if (ThisVerbArray[i] != OtherVerbArray[i])
		{
			return FALSE;
		}
		CurrentCoord = ThisCoordArray[i];
		Solution.transform(&CurrentCoord);		
		if (!IsNear(CurrentCoord.x, OtherCoordArray[i].x, lTolerance) ||
			!IsNear(CurrentCoord.y, OtherCoordArray[i].y, lTolerance))
		{
			return FALSE;
		}
	}

	*pTransform = Solution;

	return TRUE;
}

/********************************************************************************************

 >	void Path::Scale(const DocCoord dcOrigin, const double dDPI=96.0)
										   
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		dDPI		The DPI by which to scale the path
				dcOrigin	The origin by which to scale the path

							This point should be the top left hand
							corner of the region that is used to create
							a bitmap.

  
  Purpose:		This function is called on a path specified in DocCoords, to
				convert it into pixel coordinates.

				This is used in imagemaps, to convert a path specified in the
				document to a polygon to specify an imagemap clickable area
				over a bitmap.

				This function does the following to all the coordinates in
				the path

				a. Subtracts the origin dcOrigin
				b. Inverts the y-axis
				c. Multiplies the coordinates to convert them to the number
				   of DPI specified.

	
  SeeAlso:		Imagemap::AddPolygon()
	
********************************************************************************************/
void Path::Scale(const DocCoord dcOrigin, const double dDPI)
{
	//First find out how many coordinates we have to process
	INT32 lNumCoords=GetNumCoords();

	//And get a pointer to the array
	DocCoord* pdcArray=GetCoordArray();

	//For every coord in the array
	for (INT32 l=0; l<lNumCoords; l++)
	{
		//Get a pointer to the coordinate we are changing
		DocCoord* pdcThisCoord=&pdcArray[l];

		if (pdcThisCoord==NULL)
		{
			ERROR2RAW("Path::Scale - coord error");
			return;
		}

		//And scale it
		pdcThisCoord->Scale(dcOrigin, (float)dDPI);
	}
}

/********************************************************************************************

 >	BOOL Path::MakePathFromSubPath(const INT32 lSubpathIndex, Path* ppthToFill)
					
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/97 - Enough is enough
	Inputs:		lSubpathIndex	Index of the subpath to copy - that is,
								0 copies the first subpath
								1 copies the second
								etc
				ppthToFill	Path to copy the subpath into
				
	Outputs:	ppthToFill
	Returns:	TRUE if there were no problems
											     
  Purpose:		Makes a subpath into a path in its own right.
	
  SeeAlso:		Imagemap::DrawPathToOutputDevice()
	
********************************************************************************************/

BOOL Path::MakePathFromSubPath(const INT32 lSubpathIndex, Path* ppthToFill)
{
	//Check our variables
	ERROR2IF(ppthToFill==NULL,FALSE, "Path::MakePathFromSubPath - null parameter");
	ERROR2IF(lSubpathIndex>(GetNumSubpaths()-1),FALSE, "Path::MakePathFromSubPath - null parameter");

	//First we must find the start and end of the subpath

	//This variable will find the start of the subpath
	INT32 lStart=0;

	//For each subpath we must skip over
	for (INT32 l=0; l<lSubpathIndex; l++)
	{
		//Move to the end of the current subpath
		FindEndElOfSubPath(&lStart);

		//Move on one place to take us into the next subpath
		lStart++;

		//And if we haven't overshot the end of the path
		if (lStart<GetNumCoords())
		{
			//Move to the beginning of that subpath
			FindStartOfSubPath(&lStart);
		}
	}

	//If we have overshot the end of the path, return FALSE
	ERROR2IF(lStart>GetNumCoords(),FALSE, "Path::MakePathFromSubPath failed");

	//lLookPos is now pointing to the start of the subpath we want to copy
	//This variable will find the end of that subpath
	INT32 lEnd=lStart;

	//And move it to the end of the subpath
	FindEndElOfSubPath(&lEnd);

	//Now, copy the subpath into the path
	MakePathFromSection(lStart, (lEnd-lStart+1), ppthToFill);

	//And return TRUE
	return TRUE;
}

/********************************************************************************************

>	INT32 Path::CalcArea()
					
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/03/2005
	Inputs:		-
	Outputs:	-
	Returns:	Area of path in path units (typically millipoints)
				Or -1 if no area could be calculated
	Purpose:	Calculate the area of this path
	SeeAlso:	-
	
********************************************************************************************/

XLONG Path::CalcArea()
{
	// Check whether it's closed!
	if (!IsFilled)
		return 0;

	// First, use ClipPathToPath to get a path that has no self-intersections...

	// Next flatten it
	Path FlatPath;
	FlatPath.Initialise();

	BOOL bOK = Flatten(64, &FlatPath);
	if (!bOK)
		return -1;

	// Finally, compute the area under each flattened segment and sum them
	// (+dx adds to area, -dx subtracts from area)
	INT32 i = 0;
	XLONG area = 0;
	XLONG dx = 0;
	DocRect bounds = GetBoundingRect();
	INT32 baseline = bounds.lo.y;
//	PathFlags* pFlags = FlatPath.GetFlagArray();
	DocCoord* pCoords = FlatPath.GetCoordArray();
	PathVerb* pVerbs  = FlatPath.GetVerbArray();
	DocCoord origin(0,0);
	for (i=0; i < FlatPath.GetNumCoords(); i++)
	{
		if (pVerbs[i] == PT_MOVETO)
			origin = pCoords[i];
		else if (pVerbs[i] == PT_LINETO)
		{
			// area of this segment is area of rectangle under origin.y + area of triangle from origin to current point
			dx = pCoords[i].x - origin.x;
			area += dx*(XLONG)(origin.y-baseline) + dx*(XLONG)((ABS(pCoords[i].y - origin.y))/2);

			origin = pCoords[i];
		}
		else if (pVerbs[i] == PT_CLOSEFIGURE)
		{
			// do nothing
		}
		else
			ERROR3("Unexpected path verb");
	}

	// Counter-clockwise paths will generate -ve areas
	return ABS(area);
}

