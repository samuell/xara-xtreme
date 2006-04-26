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
// Implementation file for process path functions. These functions give paths
// the ability to flatten themselves, find their lengths etc.

/*
*/

#include "camtypes.h"
#include "pathproc.h"
#include "fixmem.h"
#include "paths.h"
#include "errors.h"
#if !defined(EXCLUDE_FROM_XARLIB)
#include "pen.h"
#else
#define MAXPRESSURE 1023
#endif

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(ProcessPath, CCObject);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



/******************************************************************************************

> 	ProcessFlags::ProcessFlags(BOOL flatten, BOOL quantise, BOOL quantiseall)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		flat	 = if true flatten curves
				quantise = if true quantise lines
	Outputs:	-
	Purpose:	Construct a ProcessFlags object. The flags control the action of
				ProcessPath::Process() function

******************************************************************************************/

ProcessFlags::ProcessFlags(BOOL flatten, BOOL quantise, BOOL quantiseall)
{
	FlattenCurves = flatten;
	QuantiseLines = quantise;
	QuantiseAll = quantiseall;
}




/******************************************************************************************

> 	ProcessPath::ProcessPath(const double flat)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Purpose:	
	Inputs:		flat = How flat the curves should be (around 64 is good)
					   
******************************************************************************************/

ProcessPath::ProcessPath(const double flat)
{
	ProcSource		= NULL;
	ProcCache 		= NULL;
	ProcNumCached	= 0;
	ProcFirstPoint	= TRUE;
	ProcFlatness	= flat;
}




/******************************************************************************************

> 	ProcessPath::~ProcessPath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		-
	Outputs:	-
	Purpose:	ProcessPath class destructor

******************************************************************************************/

ProcessPath::~ProcessPath()
{
	if (ProcCache)
		CCFree(ProcCache);
}



/******************************************************************************************

> 	BOOL ProcessPath::Init(Path* pSource)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Purpose:	

******************************************************************************************/

BOOL ProcessPath::Init(Path* pSource)
{
	// Allocate a cache for ourselves
	// ProcCache = (INT32*) CCMalloc(PROC_CACHE_SIZE*sizeof(INT32));	
	// if (ProcCache == NULL)
	//	return FALSE;

	ProcSource = pSource;
	return TRUE;
} 




/******************************************************************************************

>	BOOL ProcesPath::Process(const ProcessFlags& PFlags)

	Inputs:		PFlags	= Flags to direct the action of this function
	Outputs:	
	Returns: 	BOOL	= TRUE if the path was successfully processed
						= FALSE if something failed during the process
							
	Purpose:	Processes a path. It will scan through the elements of a path, passing
				points to a NewPoint virtual function. It will also flatten curves if
				found and pass the generated points on as well.
	
				Quantise a path to an output path. The output path may already contain
				data, if so the quantised data will be added on the end.
				If the output path points to 'this' path, then having successfully quantised
				to the end of the output path, the original data in the buffer will be
				removed. This allows you to quantise a path to itself.
                If the routine fails, all quantisation data so far created by the routine
                will be removed.

******************************************************************************************/

BOOL ProcessPath::Process(const ProcessFlags& PFlags)
{

	// The idea here is that we create a quantised path. This means we
	// scan through the path descretizing curves and lines dependent
	// on the flatness value given.

    DocCoord* ICoords = ProcSource->GetCoordArray();
	PathVerb* IVerbs = ProcSource->GetVerbArray();

	INT32 numinsource = ProcSource->GetNumCoords();

    INT32 i=0;
    BOOL ok = TRUE;

	// scan through the input verbs
    while ((i<numinsource) && (ok))
    {
		switch (IVerbs[i] & ~PT_CLOSEFIGURE)
		{
			case PT_MOVETO:
				OpenElement(PT_MOVETO, i);
				ok = NewPoint(PT_MOVETO, &ICoords[i]);
				if (CloseElement(ok, PT_MOVETO, i))
					i=(numinsource-1);
				break;

			case PT_LINETO:
				{
					BOOL IsCloseFigure = ((IVerbs[i] & PT_CLOSEFIGURE) != 0);	// Remember if this is the closing element

					OpenElement(PT_LINETO, i);
					if (!PFlags.QuantiseAll)
					{
						if (!PFlags.QuantiseLines)
							ok = NewPoint(PT_LINETO, &ICoords[i]);
						else
						{
							DocCoord End = ICoords[i];
							for (double mu = 0.2; (mu<1.2) && ok; mu+=0.2 )
							{
								DocCoord dest;
								dest.x = (INT32)((1-mu)*ProcPreviousEl.x + mu*End.x);
								dest.y = (INT32)((1-mu)*ProcPreviousEl.y + mu*End.y);
								ok = NewPoint(PT_LINETO, &dest);
							}
						}
					}
					else
					{
						ok = InsertQuantisedLineTo(&ICoords[i], &ProcPreviousEl);
					}
					if (CloseElement(ok, PT_LINETO, i))
						i=(numinsource-1);
					else if (IsCloseFigure)		// If continuing, and this is the end of a closed figure
						CloseFigure();			// then close it off
				}
				break;

			case PT_BEZIERTO:
				{
					BOOL IsCloseFigure = ((IVerbs[i+2] & PT_CLOSEFIGURE) != 0);	// Remember if this is the closing element

					OpenElement(PT_BEZIERTO, i);
					if (!PFlags.FlattenCurves)
						ok = NewPoint(PT_BEZIERTO, &ICoords[i]);
					else
					{
						ok = FlattenCurve(ProcPreviousEl.x, ProcPreviousEl.y,
										  ICoords[i].x, ICoords[i].y,
										  ICoords[i+1].x, ICoords[i+1].y,
										  ICoords[i+2].x, ICoords[i+2].y, (PFlags.QuantiseAll) );
					}
					if (CloseElement(ok, PT_BEZIERTO, i))
						i=(numinsource-1);
					else
					{
						if (IsCloseFigure)		// If continuing, and this is the end of a closed figure
							CloseFigure();		// then close it off

                		i+=2;
					}
				}
				break;

			default: ERROR3("ProcessPath::Process() - unknown path verb!");

    	}
		ICoords = ProcSource->GetCoordArray();
		IVerbs = ProcSource->GetVerbArray();
		i++;
		ProcFirstPoint = FALSE;
		ProcPreviousEl = ICoords[i-1];
    }
	return ok;
}

/********************************************************************************************

>	BOOL ProcessPath::FlattenCurve(INT32 Px0,INT32 Py0,
								   INT32 Px1,INT32 Py1,
								   INT32 Px2,INT32 Py2,
								   INT32 Px3,INT32 Py3)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/93
	Inputs:		Px0-Py3	=	These are the x and y coords for the 4 control points of
							a bezier
	Outputs:	-
	Purpose:	Generates a set of points on the bezier curve specified. The points start
				at (Px0,Py0) and traverse the curve to (Px3,Py3). If the points are joined
				with straight lines, the lines (given a good flatness value) should
				approximate the curve accurately enough to appear continuous. Obviously this
				is soley dependent on the flat value passed to the function. A value around
				64 is a good approximator.

********************************************************************************************/

BOOL ProcessPath::FlattenCurve(	INT32 Px0,INT32 Py0,
								INT32 Px1,INT32 Py1,
								INT32 Px2,INT32 Py2,
								INT32 Px3,INT32 Py3, BOOL QuantiseAll)
{
	INT32 diff;

	INT32 dx0 = (Px1*3 - Px0*2 - Px3);
	if (dx0 < 0) dx0 = -dx0;
	//dx0 = dx0 < 0 ? -dx0 : dx0;

	INT32 dy0 = (Py1*3 - Py0*2 - Py3);
	if (dy0 < 0) dy0 = -dy0;
	//dy0 = dy0 < 0 ? -dy0 : dy0;
	
	// Get the line's distance from the curve
	if (dx0 >= dy0)
		diff = 3*dx0 + dy0;
	else
		diff = dx0 + 3*dy0;
		
	// Is the straight line close enough to the curve ?
	if (diff > ProcFlatness)
	{
		// Not close enough so split it into two and recurse for each half
		BOOL ok = FlattenSplit(Px0,Py0, Px1,Py1, Px2,Py2, Px3,Py3, QuantiseAll);
    	return ok;
    }
    
	INT32 dx1 = (Px2*3 - Px0 - Px3*2);
	if (dx1 < 0) dx1 = -dx1;
	//dx1 = dx1 < 0 ? -dx1 : dx1;

	INT32 dy1 = (Py2*3 - Py0 - Py3*2);
	if (dy1 < 0) dy1 = -dy1;
	//1 = dy1 < 0 ? -dy1 : dy1;
	
	// Get the line's distance from the curve
	if (dx1 >= dy1)
		diff = 3*dx1 + dy1;
	else
		diff = dx1 + 3*dy1;

	// Is the straight line close enough to the curve ?
	if (diff > ProcFlatness)
	{
		// Not close enough so split it into two and recurse for each half
		BOOL ok = FlattenSplit(Px0,Py0, Px1,Py1, Px2,Py2, Px3,Py3, QuantiseAll);
    	return ok;
    }

	DocCoord npoint;
	npoint.x = Px3;
	npoint.y = Py3;

	// Line is now close enough so call the virtual function
    if (!QuantiseAll)
    	return NewPoint(PT_LINETO, &npoint);
    else
	{
		DocCoord spoint;
		spoint.x = Px0;
		spoint.y = Py0;
    	return InsertQuantisedLineTo(&npoint,&spoint);
	}
}



/******************************************************************************************

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/93
    Purpose:	Recursive partner to FlattenCurve.
    			NOT to be called by anything other than FlattenCurve.

******************************************************************************************/

BOOL ProcessPath::FlattenSplit(INT32 Px0,INT32 Py0,
							   INT32 Px1,INT32 Py1,
					  		   INT32 Px2,INT32 Py2,
							   INT32 Px3,INT32 Py3, BOOL QuantiseAll)
{

	// Calculate the first half of the curve
	INT32 lx0 = Px0;
	INT32 ly0 = Py0;
	INT32 lx1 = (Px0 + Px1)/2;
	INT32 ly1 = (Py0 + Py1)/2;
	INT32 lx2 = (Px0 + 2*Px1 + Px2)/4;
	INT32 ly2 = (Py0 + 2*Py1 + Py2)/4;
	INT32 lx3 = (Px0 + 3*Px1 + 3*Px2 + Px3)/8;
	INT32 ly3 = (Py0 + 3*Py1 + 3*Py2 + Py3)/8;

	// Calculate the second half of the curve
	INT32 rx0 = lx3;
	INT32 ry0 = ly3;
	INT32 rx1 = (Px1 + 2*Px2 + Px3)/4;
	INT32 ry1 = (Py1 + 2*Py2 + Py3)/4;
	INT32 rx2 = (Px2 + Px3)/2;
	INT32 ry2 = (Py2 + Py3)/2;
	INT32 rx3 = Px3;
	INT32 ry3 = Py3;

	// Recurse for both halfs of the curve
	BOOL 	ok = FlattenCurve(lx0,ly0, lx1,ly1, lx2,ly2, lx3,ly3, QuantiseAll);
    if (ok) ok = FlattenCurve(rx0,ry0, rx1,ry1, rx2,ry2, rx3,ry3, QuantiseAll);

    return ok;
}

/******************************************************************************************

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/95
    Purpose:	inserts a quantised lineto

******************************************************************************************/

BOOL ProcessPath::InsertQuantisedLineTo(DocCoord * pEnd, DocCoord * pStart)
{
	INT32 dx = pEnd->x - pStart->x;
	INT32 dy = pEnd->y - pStart->y;
	INT32 pdx = (dx<0)?(-dx):dx;
	INT32 pdy = (dy<0)?(-dy):dy;
	INT32 maxdist = (pdx<pdy)?pdy:pdx;
	INT32 points = (INT32)((((double)maxdist/ProcFlatness)) + 0.5);
	if (points<=1) return NewPoint(PT_LINETO, pEnd);
	if (points>10) points=10;
	DocCoord intermediate;
	INT32 n;
	for (n=1; n<=points; n++)
	{
		double d=((double)n/(double)points);
		intermediate.x=pStart->x + (INT32)(0.5+d*(double)dx);		
		intermediate.y=pStart->y + (INT32)(0.5+d*(double)dy);
		if (!NewPoint(PT_LINETO, &intermediate)) return FALSE;
	}
	return TRUE;
}

/******************************************************************************************

	void ProcessPath::OpenElement(PathVerb Verb, INT32 index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		Verb	= verb of opening element.
				index	= index of opening element.
	Purpose:	Does nothing

******************************************************************************************/

void ProcessPath::OpenElement(PathVerb Verb, INT32 index)
{
	// Base class null function
}

/******************************************************************************************

	BOOL ProcessDistance::CloseElement(BOOL done, PathVerb Verb, INT32 index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Inputs:		done	= true if the new point function procesed all new points in the
						  open element correctly,
						  false if it did not.
				Verb	= verb of closing element.
				index	= index of closing element.
	Outputs:
	Returns:	FALSE to continue processing the next element
				TRUE to stop processing and return done
	Purpose:	Does nothing

******************************************************************************************/

BOOL ProcessPath::CloseElement(BOOL done, PathVerb Verb, INT32 index)
{
	// Base class null function
	return FALSE;
}

/******************************************************************************************

>	virtual void ProcessPath::CloseFigure(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/97

	Purpose:	Derived-class interface.
				Called after closing a LINETO or BEZIERTO element which constitutes
				the end of this figure (subpath) (as indicated by this element having
				the PT_CLOSEFIGURE flag set).

				Used by the path stroking ProcessPathToTrapezoids class to allow it
				to correctly handle joining the start & end of a closed figure.

				The base class does nothing

	SeeAlso:	ProcessPathToTrapezoids::CloseFigure

******************************************************************************************/

void ProcessPath::CloseFigure(void)
{
}









/******************************************************************************************

>	ProcessFlatten::ProcessFlatten(const double flat)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		flat = How flat the curves should be (around 64 is good)
	Purpose:	Constructor for ProcessFlatten

******************************************************************************************/

ProcessFlatten::ProcessFlatten(const double flat) : ProcessPath(flat)
{
}


/******************************************************************************************

>	BOOL ProcessFlatten::FlattenPath(const ProcessFlags& PFlags, Path* pSource, Path* pDestin)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		PFlags	= Flags to control ProcessPath
				pSource	= a pointer to the path to flatten
				pDestin	= a pointer to the path to take the flattened data
	Outputs:	pDestin contains the flattened version of the path
	Returns:	TRUE if the path was flattened successfully
				FALSE then an error was signalled during processing, possibly
				due to lack of memory.
	Purpose:	Flatten a path object.
	SeeAlso:	Path::Flatten()

******************************************************************************************/

BOOL ProcessFlatten::FlattenPath(const ProcessFlags& PFlags, Path* pSource, Path* pDestin)
{
	BOOL ok = ProcessPath::Init(pSource);
	if (ok)
	{
		FlattenOutput = pDestin;
		FlattenOutput->SetPathPosition(pDestin->GetNumCoords());	
		ok = ProcessPath::Process(PFlags);
	}
	return ok;
}


/******************************************************************************************

>	BOOL ProcessFlatten::NewPoint(PathVerb Verb, DocCoord& Coord)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Purpose:	

******************************************************************************************/

BOOL ProcessFlatten::NewPoint(PathVerb Verb, DocCoord* pCoord)
{
	if (Verb == PT_MOVETO)
		return FlattenOutput->AddMoveTo(*pCoord);

	if (Verb == PT_LINETO)
		return FlattenOutput->AddLineTo(*pCoord);

	return TRUE;
}





/******************************************************************************************

>	ProcessLength::ProcessLength(const double flat)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Purpose:	Constructor for processlength.

******************************************************************************************/

ProcessLength::ProcessLength(const double flat) : ProcessPath(flat)
{
}


/******************************************************************************************

>	BOOL ProcessLength::PathLength(Path* pSource, double* SqrLength, INT32 index = -1)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		pSource		= a pointer to a path
	Outputs		SqrLength	= the squared length of the path
	Purpose:	Returns an approximation to the length of a path, in 72000 of an inch
				units.

******************************************************************************************/

BOOL ProcessLength::PathLength(Path* pSource, double* Length, INT32 index)
{
	ERROR2IF(pSource==NULL, FALSE, "NULL path passed to ProcessLength::PathLength()");
	if (index<0)
		index = (pSource->GetNumCoords())+1;

	CurrLength 		= 0;
	ElementLength	= 0;
	UserIndex 		= index;

	ProcessFlags PFlags;
	BOOL ok = ProcessPath::Init(pSource);
	if (ok)	ok = ProcessPath::Process(PFlags);
	if (ok) *Length = CurrLength;

	return ok;
}


/******************************************************************************************

	void ProcessLength::OpenElement(PathVerb Verb, INT32 index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		Verb	= verb of opening element.
				index	= index of opening element.
	Purpose:	Called from ProcessPath which is about to open a new path element.

******************************************************************************************/

void ProcessLength::OpenElement(PathVerb Verb, INT32 index)
{
	ElementLength = 0;
}


/******************************************************************************************

>	BOOL ProcessLength::NewPoint(PathVerb Verb, DocCoord* pCoord)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Purpose:	

******************************************************************************************/

BOOL ProcessLength::NewPoint(PathVerb Verb, DocCoord* pCoord)
{
	if (!ProcFirstPoint && Verb==PT_LINETO)
	{
		double p0 = PrevCoord.x - pCoord->x;
		double p1 = PrevCoord.y - pCoord->y;
		ElementLength += sqrt((p0*p0)+(p1*p1));
	}
	PrevCoord = *pCoord;
	return TRUE;
}



/******************************************************************************************

	BOOL ProcessLength::CloseElement(BOOL done, PathVerb Verb, INT32 index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		done	= true if the new point function procesed all new points in the
						  open element correctly,
						  false if it did not.
				Verb	= verb of closing element.
				index	= index of closing element.
	Outputs:
	Returns:	FALSE to continue processing the next element
				TRUE to stop processing and return done
	Purpose:	This function is called after processing all new points on a path element.
				The path element being a curve or line segment within the path passed to
				ProcessDistance::PathDistance().

******************************************************************************************/

BOOL ProcessLength::CloseElement(BOOL done, PathVerb Verb, INT32 index)
{
	if (index>UserIndex)
		// terminate scan now
		return TRUE;

	// continue summing element lengths
	CurrLength+=ElementLength;
	return FALSE;
}





/******************************************************************************************

>	ProcessDistance::ProcessDistance(const double flat)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Inputs:		flat	= flattness threshold to be used when processing the path
	Purpose:	Constructor for processdistance.

******************************************************************************************/

ProcessDistance::ProcessDistance(const double flat) : ProcessPath(flat)
{
	ElementLength = 0;
	ElementParam = 0;
	ElementIndex = 0;


}


/******************************************************************************************

>	double ProcessDistance::PathDistance(const double dist, Path* pSource, INT32* index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Inputs:		sqrdist	=	a distance along the path pSource
				pSource	=	a pointer to a path
				
	Outputs:	index	=	the index of an element in the path
	Returns:	double	=	an element parameter p where 0<=p<=1 such that p is the 
							squared distance along the path pSource.
	Purpose:	Returns information about a point, which lies a particular distance along
				a path.
	SeeAlso:	Path::ClosestPointTo(), which will return a doccoord given a parameter p
				and index i.

******************************************************************************************/

double ProcessDistance::PathDistance(const double dist, Path* pSource, INT32* index)
{
	ENSURE(dist>=0, "distance along path is negative in ProcessDistance::PathDistance");

	if (dist>0)
		Distance = dist;
	else
		Distance = 0;

	// if we have a simple position then return the start
	if (Distance == 0)
	{
		*index = 0;
		return 0;
	}

	// set up the processpath process.
	ProcessFlags PFlags;
	BOOL ok = ProcessPath::Init(pSource);
	if (ok)	ok = ProcessPath::Process(PFlags);
	if (ok) *index = ElementIndex;

	return ElementParam;
}


/******************************************************************************************

	void ProcessDistance::OpenElement(PathVerb Verb, INT32 index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		Verb	= verb of opening element.
				index	= index of opening element.
	Purpose:	Called from ProcessPath which is about to open a new path element.

******************************************************************************************/

void ProcessDistance::OpenElement(PathVerb Verb, INT32 index)
{
	ElementLength = 0;
	ElementParam = 0;
	ElementIndex = 0;
}


/******************************************************************************************

	BOOL ProcessDistance::NewPoint(PathVerb Verb, DocCoord* pCoord)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Purpose:	Called from ProcessPath which has generated a new point on the open
				element.

******************************************************************************************/

BOOL ProcessDistance::NewPoint(PathVerb Verb, DocCoord* pCoord)
{
	if (!ProcFirstPoint)
	{
		double p0 = PrevCoord.x - pCoord->x;
		double p1 = PrevCoord.y - pCoord->y;
		ElementLength += sqrt((p0*p0)+(p1*p1));
	}
	PrevCoord = *pCoord;
	return TRUE;
}


/******************************************************************************************

	BOOL ProcessDistance::CloseElement(BOOL done, PathVerb Verb, INT32 index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		done	= true if the new point function procesed all new points in the
						  open element correctly,
						  false if it did not.
				Verb	= verb of closing element.
				index	= index of closing element.
	Outputs:
	Purpose:	This function is called after processing all new points on a path element.
				The path element being a curve or line segment within the path passed to
				ProcessDistance::PathDistance().
				The function will use the element length calculated by NewPoint and
				decrement the total path length by this amount. If at some point the
				working distance goes negative, a parameter is calculated along the
				element. 

******************************************************************************************/

BOOL ProcessDistance::CloseElement(BOOL done, PathVerb Verb, INT32 index)
{
	if (done)
	{
		Distance-=ElementLength;
		INT32 nextindex = index;
		if ((Distance>0) && ProcSource->FindNextEndPoint(&nextindex))
			return FALSE;

		Distance+=ElementLength;
		ElementIndex = index;

		if (ElementLength>0)
			ElementParam = Distance / ElementLength;
		else
			ElementParam = 0;

		return TRUE;

	}
	return FALSE;
}



///////////////////////////////////////////////////////////////////////////////////////////
// ProcessPathDistance

/******************************************************************************************
>	ProcessPathDistance::ProcessPathDistance(const double flat)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/95
	Inputs:		flat	= flattness threshold to be used when processing the path
	Purpose:	Constructor for ProcessPathDistance
******************************************************************************************/

ProcessPathDistance::ProcessPathDistance(const double flat) : ProcessPath(flat)
{
	// just calls base class for now

	// initialise caching members - DY
	m_LastFoundIndex = 0;
	m_LastFoundDistance = 0;
	CurrentDist   = 0.0;
	m_bDrawingBrush = FALSE;
}


/******************************************************************************************
>	BOOL ProcessPathDistance::GetCoordAndTangent(DocCoord* pCoord, double* pTangent,
												 BOOL* pFound, double Dist, Path* pPath)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/95
	Purpose:	Find the coords and tangent at a distance along a path
				If the distance is not actually on the path, extrapolate the ends of the
	Inputs:		Dist     - distance along path for which info is desired
				pPath    - path to process
	Outputs:	pCoord   - coord   of desired distance along path
				pTangent - tangent of desired distance along path
				pFound   - TRUE if point found (ie distance not longer than path)
	Returns:	FALSE if fails
				path as lines at the tangent at the last points and flag not (really) found
******************************************************************************************/

BOOL ProcessPathDistance::GetCoordAndTangent(DocCoord* pCoord, double* pTangent, BOOL* pFound,
											 double Dist, Path* pPath, UINT32* pPressure)
{
	ERROR2IF(pCoord==NULL && pTangent==NULL,FALSE,"ProcessPathDistance::GetCoordAndTangent() - no output pointers specified!");
	ERROR2IF(pFound==NULL,FALSE,"ProcessPathDistance::GetCoordAndTangent() - pFound==NULL");
	ERROR2IF( pPath==NULL,FALSE,"ProcessPathDistance::GetCoordAndTangent() - pPath==NULL");

	DesiredDist   = Dist;
	CurrentDist   = 0;
	Found         = FALSE;
	CoordAtDist   = DocCoord(0,0);
	TangentAtDist = 0;
	PressureAtDist = 0;

	// set up the processpath process.
	ProcessFlags PFlags;
	BOOL ok=ProcessPath::Init(pPath);
	if (ok)	ok= Process(PFlags, 0); //ProcessPath::Process(PFlags);

	if (ok)
	{
		if (Found==FALSE)
		{
			// get last 2 points on line
			DocCoord* pPathCoords=pPath->GetCoordArray();
			ERROR2IF(pPathCoords==NULL,FALSE,"ProcessPathDistance::GetCoordAndTangent() - pPathCoords==NULL");
			INT32 NumPathCoords=pPath->GetNumCoords();
			ERROR2IF(NumPathCoords<2,FALSE,"ProcessPathDistance::GetCoordAndTangent() - NumPathCoords < 2");
			DocCoord LastCoord=pPathCoords[NumPathCoords-1];
			DocCoord PrevCoord=pPathCoords[NumPathCoords-2];

			double dx=LastCoord.x-PrevCoord.x;
			double dy=LastCoord.y-PrevCoord.y;
			double LineLength=sqrt(dx*dx+dy*dy);

			if (LineLength>0)
			{
				double FractOfLine = (DesiredDist-CurrentDist)/LineLength;
				double x = LastCoord.x+dx*FractOfLine;
				double y = LastCoord.y+dy*FractOfLine;

				CoordAtDist   = DocCoord((MILLIPOINT)x,(MILLIPOINT)y);
				TangentAtDist = atan2(dy,dx);
			}
			else
			{
				CoordAtDist	  =	LastCoord;
				TangentAtDist = 0;
			}
		}
		else
			if (Dist<0) Found=FALSE;	// if not actually on path, flag not found on path
		

		*pFound = Found;
		if (pCoord)   *pCoord   = CoordAtDist;
		if (pTangent) *pTangent = TangentAtDist;
		if (pPressure != NULL)
		{
			*pPressure = PressureAtDist;
		//	TRACEUSER( "Diccon", _T("Pressure At Dist = %d\n"), PressureAtDist);
		}
	}

	return ok;
}

/******************************************************************************************
>	INT32 ProcessPathDistance::GetCoordAndTangentWithCache(DocCoord* pCoord, double* pTangent,
												 BOOL* pFound, double Dist, Path* pPath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/95
	Inputs:		Dist     - distance along path for which info is desired
				pPath    - path to process
	Outputs:	pCoord   - coord   of desired distance along path
				pTangent - tangent of desired distance along path
				pFound   - TRUE if point found (ie distance not longer than path)
	Returns:	The number of coords processed, or -1 if failure
	Purpose:	Find the coords and tangent at a distance along a path
				If the distance is not actually on the path, extrapolate the ends of the
				path as lines at the tangent at the last points and flag not (really) found
				This variant is designed for paths that use this function a lot.  To avoid
				processsing the entire path many times we keep track of how many 
				coords have already been processed.

	Note:      The caching currently only works if you are looking for consecutive points
				on the same path, for example whilst drawing a brush stroke.
******************************************************************************************/

INT32 ProcessPathDistance::GetCoordAndTangentWithCache(DocCoord* pCoord, 
													  double* pTangent, BOOL* pFound,
											          double Dist, Path* pPath, UINT32* pPressure)
{
	ERROR2IF(pCoord==NULL && pTangent==NULL,FALSE,"ProcessPathDistance::GetCoordAndTangent() - no output pointers specified!");
	ERROR2IF(pFound==NULL,FALSE,"ProcessPathDistance::GetCoordAndTangent() - pFound==NULL");
	ERROR2IF( pPath==NULL,FALSE,"ProcessPathDistance::GetCoordAndTangent() - pPath==NULL");
 
	// we wish to continue processing from the last point that we found
	INT32 PrevCoordIndex = m_LastFoundIndex;
	INT32 NumProcessed = m_LastFoundIndex;

	DesiredDist   = Dist;
	CurrentDist   = m_LastFoundDistance;
	Found         = FALSE;
	CoordAtDist   = DocCoord(0,0);
	TangentAtDist = 0;
	PressureAtDist = 0;
	m_bDrawingBrush = TRUE;
	// set up the processpath process.
	ProcessFlags PFlags;
	
	BOOL bPressure = (pPressure != NULL && pPath->HasWidth());
	// if we are already partway along the path then we need to get the
	// previous point as it is needed in NewPoint
	if (NumProcessed > 0)
	{
		DocCoord* pCoords = pPath->GetCoordArray();
		PrevCoord = pCoords[PrevCoordIndex];
		if (bPressure)
		{
			UINT32* pPressureArray = pPath->GetWidthArray();
			if (pPressureArray != NULL)
				m_PrevPressure = pPressureArray[PrevCoordIndex];
			else
			{
				ERROR3("Wheres the pressure array?");
				m_PrevPressure = (UINT32)(MAXPRESSURE / 2);
			}
		}
	}
	
	BOOL ok=ProcessPath::Init(pPath);
	//TRACEUSER( "Diccon", _T("Desired    = %f\n"), Dist);
	//TRACEUSER( "Diccon", _T("Starting   = %f\n"), CurrentDist);

	if (ok)	NumProcessed = Process(PFlags, NumProcessed);

	if (NumProcessed != -1)
	{
		if (Found==FALSE) // this can happen if we are on the first couple of points
		{
			// get last 2 points on line
			DocCoord* pPathCoords=pPath->GetCoordArray();
			ERROR2IF(pPathCoords==NULL,FALSE,"ProcessPathDistance::GetCoordAndTangent() - pPathCoords==NULL");
			INT32 NumPathCoords=pPath->GetNumCoords();
			//ERROR2IF(NumPathCoords<2,FALSE,"ProcessPathDistance::GetCoordAndTangent() - NumPathCoords < 2");
			if (NumPathCoords >= 2)
			{
				DocCoord LastCoord=pPathCoords[NumPathCoords-1];
				DocCoord PrevCoord=pPathCoords[NumPathCoords-2];

				double dx=LastCoord.x-PrevCoord.x;
				double dy=LastCoord.y-PrevCoord.y;
				double LineLength=sqrt(dx*dx+dy*dy);

				if (LineLength>0)
				{
					double FractOfLine = (DesiredDist-CurrentDist)/LineLength;
					double x = LastCoord.x+dx*FractOfLine;
					double y = LastCoord.y+dy*FractOfLine;

					CoordAtDist   = DocCoord((MILLIPOINT)x,(MILLIPOINT)y);
					TangentAtDist = atan2(dy,dx);
					Found = TRUE;
				}
				else
				{
					CoordAtDist	  =	LastCoord;
					TangentAtDist = 0;
					Found = TRUE;
				}
			}
			else //if we only have one coordinate so far
			{
				CoordAtDist=pPathCoords[0];
				TangentAtDist = 0;
				Found = TRUE;
			}
		}
		else
		{
			if (Dist<0) Found=FALSE;	// if not actually on path, flag not found on path
			
		}
		*pFound = Found;
		if (pCoord)   *pCoord   = CoordAtDist;
		if (pTangent) *pTangent = TangentAtDist;
		if (pPressure != NULL)
			*pPressure = PressureAtDist;
	}
	//m_LastFoundDistance = CurrentDist;
	//TRACEUSER( "Diccon", _T("Last Index = %d\n"), m_LastFoundIndex);
	//TRACEUSER( "Diccon", _T("Last Dist  = %f\n"), m_LastFoundDistance);

	//TRACEUSER( "Diccon", _T("FOUND      = %d\n"), Found);

	return NumProcessed;
}
/******************************************************************************************
>	BOOL ProcessPathDistance::NewPoint(PathVerb Verb, DocCoord* pCoord)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/95
	Inputs:		Verb   - current point verb (only processes PT_LINETO)
				pCoord - current point coords
	Returns:	TRUE
	Purpose:	Called from ProcessPath which has generated a new point on the open element.
				If the length of this line causes the cummulaive length to exceed the
				desired length, get the tangent of this line then use the remaing distance
				(Desired-Current) to linearly interpolate to the desired coord on the line
******************************************************************************************/

BOOL ProcessPathDistance::NewPoint(PathVerb Verb, DocCoord* pCoord)
{
	ERROR2IF(pCoord==NULL,FALSE,"ProcessPathDistance::NewPoint() - pCoord==NULL");
	ERROR2IF(Verb!=PT_LINETO && Verb!=PT_MOVETO,FALSE,"ProcessPathDistance::NewPoint() - unknown path verb");

	
	if (!Found && !ProcFirstPoint && Verb==PT_LINETO)
	{
		double dx=pCoord->x-PrevCoord.x;
		double dy=pCoord->y-PrevCoord.y;
		double LineLength=sqrt(dx*dx+dy*dy);

		double NextDist=CurrentDist+LineLength;
		
		m_LastFoundDistance = CurrentDist;
		//TRACEUSER( "Diccon", _T("m_LastFound = %f\n"), m_LastFoundDistance);
		if (NextDist>=DesiredDist)
		{
			if (LineLength>0)
			{
				double FractOfLine = (DesiredDist-CurrentDist)/LineLength;
				double x = PrevCoord.x+dx*FractOfLine;
				double y = PrevCoord.y+dy*FractOfLine;
				CoordAtDist   = DocCoord((MILLIPOINT)x,(MILLIPOINT)y);
				TangentAtDist = atan2(dy,dx);
				
			}
			else
			{
				CoordAtDist	  =	PrevCoord;
				TangentAtDist = 0;
			}
			Found=TRUE;
		}

		CurrentDist=NextDist;
	}
 
	PrevCoord=*pCoord;
	
	return TRUE;
}

/******************************************************************************************
>	BOOL ProcessPathDistance::NewPoint(PathVerb Verb, DocCoord* pCoord)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/95
	Inputs:		Verb   - current point verb (only processes PT_LINETO)
				pCoord - current point coords
	Returns:	TRUE
	Purpose:	Called from ProcessPath which has generated a new point on the open element.
				If the length of this line causes the cummulaive length to exceed the
				desired length, get the tangent of this line then use the remaing distance
				(Desired-Current) to linearly interpolate to the desired coord on the line
******************************************************************************************/

BOOL ProcessPathDistance::NewPointA(PathVerb Verb, DocCoord* pCoord, UINT32* pPressure)
{
	ERROR2IF(pCoord==NULL,FALSE,"ProcessPathDistance::NewPoint() - pCoord==NULL");
	ERROR2IF(Verb!=PT_LINETO && Verb!=PT_MOVETO,FALSE,"ProcessPathDistance::NewPoint() - unknown path verb");

	
	if (!Found && !ProcFirstPoint && Verb==PT_LINETO)
	{
		double dx=pCoord->x-PrevCoord.x;
		double dy=pCoord->y-PrevCoord.y;
		double LineLength=sqrt(dx*dx+dy*dy);

		double NextDist=CurrentDist+LineLength;
		
		m_LastFoundDistance = CurrentDist;
		//TRACEUSER( "Diccon", _T("m_LastFound = %f\n"), m_LastFoundDistance);
		if (NextDist>=DesiredDist)
		{
			if (LineLength>0)
			{
				double FractOfLine = (DesiredDist-CurrentDist)/LineLength;
				double x = PrevCoord.x+dx*FractOfLine;
				double y = PrevCoord.y+dy*FractOfLine;
				CoordAtDist   = DocCoord((MILLIPOINT)x,(MILLIPOINT)y);
				TangentAtDist = atan2(dy,dx);
				if (pPressure != NULL)
					PressureAtDist = (UINT32)((*pPressure * (1-FractOfLine)) + (m_PrevPressure * FractOfLine));
			}
			else
			{
				CoordAtDist	  =	PrevCoord;
				TangentAtDist = 0;
				if (pPressure != NULL)
					PressureAtDist = m_PrevPressure;
			}
			Found=TRUE;
		}

		CurrentDist=NextDist;
	}
 
	PrevCoord=*pCoord;
	m_PrevPressure = *pPressure;
	return TRUE;
}


/******************************************************************************************

>	INT32 ProcesPathDistance::Process(const ProcessFlags& PFlags, INT32 AlreadyProcessed)

	Inputs:		PFlags	= Flags to direct the action of this function
				AlreadyProcessed - the number of coordinates already processed
	Outputs:	
	Returns: 	The number of coordinates processed (in total)  or -1 for failure
							
	Purpose:	Processes a path. It will scan through the elements of a path, passing
				points to a NewPoint virtual function. It will also flatten curves if
				found and pass the generated points on as well.
	
				Quantise a path to an output path. The output path may already contain
				data, if so the quantised data will be added on the end.
				If the output path points to 'this' path, then having successfully quantised
				to the end of the output path, the original data in the buffer will be
				removed. This allows you to quantise a path to itself.
                If the routine fails, all quantisation data so far created by the routine
                will be removed.

				This variant only processes coordinates after AlreadyProcessed

******************************************************************************************/

INT32 ProcessPathDistance::Process(const ProcessFlags& PFlags, INT32 AlreadyProcessed)
{

	// The idea here is that we create a quantised path. This means we
	// scan through the path descretizing curves and lines dependent
	// on the flatness value given.

    DocCoord* ICoords = ProcSource->GetCoordArray();
	PathVerb* IVerbs = ProcSource->GetVerbArray();

	UINT32*     IPressure = NULL;
	if (ProcSource->HasWidth())
		IPressure = ProcSource->GetWidthArray();
	
	//if (IPressure == NULL)
	//	ERROR3("No pressure array");

	INT32 numinsource = ProcSource->GetNumCoords();

    INT32 i=AlreadyProcessed;
    BOOL ok = TRUE;
	if ( i > 0)
	{
		ProcFirstPoint = FALSE;
	}
	
	// scan through the input verbs
    while ((i < numinsource) && (ok) && (!Found))
    {
	//	if (IPressure != NULL)
	//		TRACEUSER( "Diccon", _T("PathProc Pressure =  %d\n"), IPressure[i]);
		switch (IVerbs[i] & ~PT_CLOSEFIGURE)
		{
			case PT_MOVETO:
				OpenElement(PT_MOVETO, i);
				if (IPressure != NULL)
					ok = NewPointA(PT_MOVETO, &ICoords[i], &IPressure[i]);
				else
					ok = NewPoint(PT_MOVETO, &ICoords[i]);

				if (CloseElement(ok, PT_MOVETO, i))
					i=(numinsource-1);
				break;

			case PT_LINETO:
				{
					BOOL IsCloseFigure = ((IVerbs[i] & PT_CLOSEFIGURE) != 0);	// Remember if this is the closing element

					OpenElement(PT_LINETO, i);
					if (!PFlags.QuantiseAll)
					{
						if (!PFlags.QuantiseLines)
							if (IPressure != NULL)
								ok = NewPointA(PT_LINETO, &ICoords[i], &IPressure[i]);
							else
								ok = NewPoint(PT_LINETO, &ICoords[i]);
						else
						{
							DocCoord End = ICoords[i];
							for (double mu = 0.2; (mu<1.2) && ok; mu+=0.2 )
							{
								DocCoord dest;
								dest.x = (INT32)((1-mu)*ProcPreviousEl.x + mu*End.x);
								dest.y = (INT32)((1-mu)*ProcPreviousEl.y + mu*End.y);
								ok = NewPoint(PT_LINETO, &dest);
							}
						}
					}
					else
					{
						ok = InsertQuantisedLineTo(&ICoords[i], &ProcPreviousEl);
					}
					if (CloseElement(ok, PT_LINETO, i))
						i=(numinsource-1);
					else if (IsCloseFigure)		// If continuing, and this is the end of a closed figure
						CloseFigure();			// then close it off
				}
				break;

			case PT_BEZIERTO:
				{
					BOOL IsCloseFigure = ((IVerbs[i+2] & PT_CLOSEFIGURE) != 0);	// Remember if this is the closing element

					OpenElement(PT_BEZIERTO, i);
					if (!PFlags.FlattenCurves)
						ok = NewPoint(PT_BEZIERTO, &ICoords[i]);
					else
					{
						ok = FlattenCurve(ProcPreviousEl.x, ProcPreviousEl.y,
										  ICoords[i].x, ICoords[i].y,
										  ICoords[i+1].x, ICoords[i+1].y,
										  ICoords[i+2].x, ICoords[i+2].y, (PFlags.QuantiseAll) );
					}
					if (CloseElement(ok, PT_BEZIERTO, i))
						i=(numinsource-1);
					else
					{
						if (IsCloseFigure)		// If continuing, and this is the end of a closed figure
							CloseFigure();		// then close it off

                		i+=2;
					}
				}
				break;

			default: ERROR3("ProcessPath::Process() - unknown path verb!");

    	}
		ICoords = ProcSource->GetCoordArray();
		IVerbs = ProcSource->GetVerbArray();
		i++;
		ProcFirstPoint = FALSE;
		ProcPreviousEl = ICoords[i-1];
    }

	// we are recording the point previous to the one we just found. 
	// i represents the next point, i-1 is the point we just found, so 
	// the one before that is i-2
	if (Found)
		m_LastFoundIndex = i -2;

	INT32 ReturnValue;
	// we have processed i-1 points
	if (ok)
		ReturnValue = i-1; 
	else
		ReturnValue = -1;

	return ok;
}
