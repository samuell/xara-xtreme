// $Id: moldpers.cpp 662 2006-03-14 21:31:49Z alex $
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
// Perspective shape implementation

/*
*/

#include "camtypes.h"
#include "moldpers.h"
#include "errors.h"
#include "paths.h"
#include "rndrgn.h"
#include "docview.h"
#include "app.h"
#include "blobs.h"
//#include "justin.h"
#include "moldedit.h"
//#include "mike.h"

#include <math.h>

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_MEMDUMP(GPerspective, GMould)
CC_IMPLEMENT_DYNAMIC(MouldPerspective, MouldGeometry)
CC_IMPLEMENT_DYNCREATE(RecordPerspectiveAction,Action)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW
#define EPSILON 1e-10
#define equal(a,b) ( fabs( (a) - (b) ) <= (EPSILON) )
#define notequal(a,b) ( fabs( (a) - (b) ) > (EPSILON) )

//static POINT InitialPerspective[PER_NUMCOORDS] = { {0,0}, {1,1} };
//static RECT  InitialBounds = { 0,0,1,1 };


/*******************************************************************************************

>	virtual void MouldPerspective::MouldPerspective()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		
	Purpose:	Perspective mould constructor

********************************************************************************************/

MouldPerspective::MouldPerspective()
{
	GridState = 0;			// 0 = Grid off
							// 1 = Grid on
							// 2 = Grid on but temporarily disabled
}



/*******************************************************************************************

>	MouldPerspective::~MouldPerspective()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		
	Purpose:	Perspective mould destructor

********************************************************************************************/

MouldPerspective::~MouldPerspective()
{
}

/*******************************************************************************************

>	BOOL MouldPerspective::Validate(Path* const pPath, UINT32& errorID)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/02/95
	Inputs:		pPath	= the path to use for perspectivising
	Outputs:	errorID	= an id of an error string reporting the problem when failed.
	Returns:	TRUE 	- then the path can be used as an perspective manifold
				FALSE 	- then the path is not suitable for use as an perspective manifold
	Purpose:	This function checks whether the path given is suitabile for use as an
				perspective manifold. Valid paths at present are any set of 5 path elements
				starting with a moveto followed by 4 lineto elements which form a closed
				loop without crossings.

********************************************************************************************/

BOOL MouldPerspective::Validate(Path* const pPath, UINT32& errorID)
{
	// check for a reasonable number of coordinates
	INT32 numc=pPath->GetNumCoords();
	if ((numc!=PER_NUMCOORDS+1))
	{
		errorID = _R(IDE_PERS_NUMCOORDERR);
		return FALSE;
	}

	// now we need a more complex check
	DocCoord* Coords = pPath->GetCoordArray();
	PathVerb* Verbs = pPath->GetVerbArray();

	// we need the first element to be a move to
	if (Verbs[0]!=PT_MOVETO)
	{
		errorID = _R(IDE_PERS_BADELEMENT);
		return FALSE;
	}

	// And the last coordinate must close the loop
	if (Coords[0]!=Coords[numc-1])
	{
		errorID = _R(IDE_PERS_NOTCLOSED);
		return FALSE;
	}

	INT32 i,j;
	// followed by all linetos
	for (i=1; i<numc; i++)
	{
		if ( (Verbs[i] & (~PT_CLOSEFIGURE))!=PT_LINETO )
		{
			errorID = _R(IDE_PERS_NOTALLLINES);
			return FALSE;
		}
	}

	// check no coordinates are the same
	for (i=1; i<numc; i++)
	{
		for (j=i+1; j<numc; j++)
		{
			if (Coords[i] == Coords[j])
			{
				errorID = _R(IDE_PERS_COINCIDENT);
				return FALSE;
			}
		}
	}

	// Make sure there's no folds either
	POINT* p = (POINT*)(Coords);
	if (!WillBeValid(p))
	{
		errorID = _R(IDE_PERS_CROSSING);
		return FALSE;
	}

 	// All is well
	return TRUE;
}


/*******************************************************************************************

	BOOL MouldPerspective::WillBeValid(POINT* P)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		P a pointer to a POINT array of coordinates
	Returns:	TRUE if the point set is valid
	Purpose:	Check whether the point array has any crossing and is thus unsuited for use
				as a manifold

********************************************************************************************/

BOOL MouldPerspective::WillBeValid(POINT* P)
{
	INT32 MinX, MaxX, MinY, MaxY, Width, Depth ;
	DPOINT V0,V1,V2,V3;

	MinX = MaxX = P[0].x ;
	MinY = MaxY = P[0].y ;
	MinX = min( MinX, INT32(P[1].x) ) ; MaxX = max( MaxX, INT32(P[1].x) ) ;
	MinY = min( MinY, INT32(P[1].y) ) ; MaxY = max( MaxY, INT32(P[1].y) ) ;
	MinX = min( MinX, INT32(P[2].x) ) ; MaxX = max( MaxX, INT32(P[2].x) ) ;
	MinY = min( MinY, INT32(P[2].y) ) ; MaxY = max( MaxY, INT32(P[2].y) ) ;
	MinX = min( MinX, INT32(P[3].x) ) ; MaxX = max( MaxX, INT32(P[3].x) ) ;
	MinY = min( MinY, INT32(P[3].y) ) ; MaxY = max( MaxY, INT32(P[3].y) ) ;
	Width = MaxX-MinX ;
	Depth = MaxY-MinY ;

	if (Width==0 || Depth==0)
		return FALSE;

	V0.x = (double)(P[0].x-MinX)/Width ;
	V0.y = (double)(P[0].y-MinY)/Depth ;
	V1.x = (double)(P[1].x-MinX)/Width ;
	V1.y = (double)(P[1].y-MinY)/Depth ;
	V2.x = (double)(P[2].x-MinX)/Width ;
	V2.y = (double)(P[2].y-MinY)/Depth ;
	V3.x = (double)(P[3].x-MinX)/Width ;
	V3.y = (double)(P[3].y-MinY)/Depth ;

	BOOL F  =  (V1.x-V0.x)*(V3.y-V0.y) > (V1.y-V0.y)*(V3.x-V0.x) ;
	if ( F != ((V2.x-V1.x)*(V0.y-V1.y) > (V2.y-V1.y)*(V0.x-V1.x)) ) return FALSE ;
	if ( F != ((V3.x-V2.x)*(V1.y-V2.y) > (V3.y-V2.y)*(V1.x-V2.x)) ) return FALSE ;
	if ( F != ((V0.x-V3.x)*(V2.y-V3.y) > (V0.y-V3.y)*(V2.x-V3.x)) ) return FALSE ;

	return TRUE;
}



/*******************************************************************************************

>	BOOL MouldPerspective::Define(Path* const pPath, DocRect* const pOrigBBox)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pPath	  =	the path to use for moulding
				pOrigBBox = a pointer to the bounding box of the objects to be moulded
						  = NULL then the old bounding box will be assumed.
	returns:	TRUE 	- then Define has succeded
				FALSE 	- then Define has failed to initialise the perspective mould
	Purpose:	This function sets the low level perspective state ready for calls to various
				perspective path generation functions. It serves simply as a way of informing
				the low level perspectiviser of the shape of the current manifold

********************************************************************************************/

BOOL MouldPerspective::Define(Path* const pPath, DocRect* const pOrigBBox)
{
	ERROR2IF(pPath==NULL && pOrigBBox==NULL, FALSE,"MouldPerspective::Define() called with NULL parameters");

	RECT ObjRect = BuildRectangle(pOrigBBox);
	
	// Save the current state incase we fail
	POINT TempArray[PER_NUMCOORDS];
	RECT TempRect=Perspective.GetSourceBBox();
	Perspective.CopyShape(TempArray);

	// if we're defining a new path then point at it
	POINT* pPoints = TempArray;
	if (pPath!=NULL)
		pPoints=(POINT*)pPath->GetCoordArray();

	// Define the new shape
	Perspective.Define(pPoints,&ObjRect,MouldThreshold);

	// now check it for validity
	if (!Perspective.Valid())
	{
		// if not valid replace the old data
		Perspective.Define(TempArray,&TempRect,MouldThreshold);
		return FALSE;
	}
		
	return TRUE;
}



/*******************************************************************************************

	RECT MouldPerspective::BuildShape(POINT* pPoints, DocRect* const pOrigBBox) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/01/95
	Inputs:		pPoints		= a pointer to a array of INT32 coordinates
				pOrigBBox 	= a pointer to the bounding box of the objects to be moulded
						  	= NULL then the old bounding box will be assumed.
	Returns:	TRUE 	- then BuildShape has succeded
				FALSE 	- then BuildShape has failed to create the mould shape
	Purpose:	

********************************************************************************************/

BOOL MouldPerspective::BuildShape(POINT* pPoints, DocRect* const pOrigBBox)
{
	ERROR2IF(pPoints==NULL,FALSE,"MouldPerspective::BuildShape() called with NULL point list");

	RECT ObjRect = BuildRectangle(pOrigBBox);

	Perspective.Define(pPoints,&ObjRect,MouldThreshold);

	return TRUE;
}



/*******************************************************************************************

>	RECT MouldPerspective::BuildRectangle(DocRect* const pOrigBBox) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/01/95
	Inputs:		pOrigBBox = a pointer to the bounding box of the objects to be perspectivised
						  = NULL then the old bounding box will be assumed.
	Returns:	a RECT structure.			
	Purpose:	Build a RECT structure to pass to the low level perspective code. 

********************************************************************************************/

RECT MouldPerspective::BuildRectangle(DocRect* const pOrigBBox)
{
	RECT ObjRect;
	if (pOrigBBox!=NULL)
	{
		ObjRect.left 	= pOrigBBox->lo.x;
		ObjRect.bottom 	= pOrigBBox->lo.y;
		ObjRect.right 	= pOrigBBox->hi.x;
		ObjRect.top 	= pOrigBBox->hi.y;
	}
	else
		ObjRect = Perspective.GetSourceBBox();

	return ObjRect;
}






/*******************************************************************************************

>	virtual BOOL MouldPerspective::MouldPathToPath(Path* pSourcePath, Path* pDestinPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pSourcePath = pointer to a path object to mould
				pDestinPath = pointer to a path buffer to put the resulting moulded path in
	Outputs:	pDestinPath contains the moulded version of the path
	Returns:	TRUE	- if the mould was successfull
				FALSE	- if no room to create the mould
	Purpose:	Using the defined Perspective, mould one path into another.

********************************************************************************************/

BOOL MouldPerspective::MouldPathToPath(Path* pSourcePath, Path* pDestinPath)
{
	ERROR2IF(pSourcePath==NULL, FALSE, "MouldPerspective::MouldPathToPath() passed a NULL source pointer");
	ERROR2IF(pDestinPath==NULL, FALSE, "MouldPerspective::MouldPathToPath() passed a NULL destin pointer");

	// get the source path data	length
	DWORD ilength = (DWORD)pSourcePath->GetNumCoords();

	// if there's no input path, then there's no output path, and all is well.
	if (ilength<1)
		return TRUE;

	INT32 freespace = ilength*4;
	INT32 maxspace = ilength*16;
	BOOL ok;
	INT32 added;

	Path TempPath;
	if (!TempPath.Initialise(freespace,24))
		return FALSE;

	do
	{
		// get the source object and destin object path data
		POINT* icoords = (POINT*)pSourcePath->GetCoordArray();
		BYTE*  iverbs  = (BYTE*)pSourcePath->GetVerbArray();
		POINT* ocoords = (POINT*)TempPath.GetCoordArray();
		BYTE*  overbs  = (BYTE*)TempPath.GetVerbArray();

		// try to create an enveloped path
		added = Perspective.FitPath(icoords,iverbs,ilength,ocoords,overbs,freespace);

		if (added==-1)
		{
			freespace*=2;
			// create what we think will be enough space for the output path
			ok = TempPath.EnsureVolume(freespace);
			if (!ok) 
				return FALSE;
		}

	} while ((added==-1) && freespace<=maxspace);

	// if we've failed to create the perspectivised object then tidy up
	if (added<=1)
		return FALSE;

	// now lets set up the new flags array
	ok = TempPath.ExternalArraysReplaced(added);
	if (ok)
		TempPath.InitialiseFlags(0,added);

	if (ok)
	{

/* Path trimming, temporarily removed as doesn't fix the showstopper.

		// Ok check for rampant null elements in this path
		DocCoord* Coords;
		PathVerb* Verbs;
		PathFlags* Flags;
		PathVerb CurVerb;
		INT32 i,lmi;
		DocCoord lm;

		TempPath.GetPathArrays(&Verbs,&Coords,&Flags);

	    for (i=0; i<added; i++)
		{
			CurVerb = Verbs[i] & ~PT_CLOSEFIGURE;
			if (CurVerb==PT_MOVETO)
			{
				lm=Coords[i];
				lmi=i;
			}

			if (CurVerb==PT_BEZIERTO)
			{
				if (Verbs[i+2] & PT_CLOSEFIGURE)
				{
					DocCoord p0,p1,p2;
					p0 = Coords[i];
					p1 = Coords[i+1];
					p2 = Coords[i+2];
					if ((p0.x==p1.x) && (p1.x==p2.x) && (p2.x==lm.x) &&
						(p0.y==p1.y) && (p1.y==p2.y) && (p2.y==lm.y) && 
						(i-lmi)>1)
					{
						// We can delete this element!
						Verbs[i-1] |= PT_CLOSEFIGURE;
						if (TempPath.DeleteSection(i,3))
						{
							added-=3;
							i-=3;
						}
						else
							i=added;
					}
				}
				i+=2;
			}
		}
*/
		ok = pDestinPath->CloneFrom(TempPath);
	}

	return ok;
}



/*******************************************************************************************

>	virtual BOOL MouldPerspective::MouldPoint(DocCoord p, DocCoord& q)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		p = the coordinate to be transformed
	Outputs:	q = the transformed version of the coordinate
	Returns:	TRUE if the point has been transformed
				FALSE if q==p
	Purpose:	Using the defined perspective, this function takes the input coordinate
				and calculates its transformed version.

********************************************************************************************/

BOOL MouldPerspective::MouldPoint(DocCoord p,DocCoord& q)
{
	// I aught to check that the point as contained within the source rectangle
	// bbox and do something about it if not. But what to do?
	// It turns out that perspective transforms behave sensibly for points
	// outside the source bbox so we can remove the following checks.
/*
	RECT rect = Perspective.GetSourceBBox();
	if	((p.x<rect.left) || (p.x>rect.right) ||
		(p.y<rect.bottom) || (p.y>rect.top))
	{
		q=p;
		return;
	}
*/
	POINT r,s;

	r.x = p.x;
	r.y = p.y;

	Perspective.FitPoint(r,s);

	q.x = s.x;
	q.y = s.y;

	return TRUE;
}




/*******************************************************************************************

>	virtual BOOL MouldPerspective::MouldBitmapToTile(Blit* pSourceBlit, Blit* pDestinBlit)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pSourceBlit = pointer to a bitmap to Perspective
				pDestinBlit = pointer to an output tile to mould into
	Outputs:	pDestinBlit = Perspectived version of the bitmap
	Returns:	TRUE	- if the mould was successfull
				FALSE	- if no room to create the mould
	Purpose:	Create an Perspectived version of a bitmap, ready to be rendered to a device

********************************************************************************************/

BOOL MouldPerspective::MouldBitmapToTile(KernelBitmap* pSourceBlit, KernelBitmap* pDestinBlit)
{
	// currently not implemented
	return FALSE;
}


/*******************************************************************************************

>	virtual void MouldPerspective::MouldPathRender(Path* pPath, RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pPath	= pointer to a path to perspectivise
				pRegion	= pointer to a region to render into
	Purpose:	Render a given path using the current perspective settings, to the output
				region

********************************************************************************************/

void MouldPerspective::MouldPathRender(Path* pPath, RenderRegion* pRegion)
{
}




/*******************************************************************************************

>	virtual void MouldPerspective::MouldBitmapRender(KernelBitmap* pBlit,
													 DocCoord* pParallel,
													 RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pBlit		= pointer to a kernel bitmap
				pParallel	= pointer to a four point parallelogram
				pRegion		= pointer to a region to render into
	Purpose:

********************************************************************************************/

void MouldPerspective::MouldBitmapRender(KernelBitmap* pBlit, 
										 DocCoord* Parallel,
										 RenderRegion* pRegion)
{
	pRegion->SaveContext();

	// No lines on the rectangle
	pRegion->SetLineColour(COLOUR_TRANS);

	pRegion->RestoreContext();
}

/*******************************************************************************************

>	virtual void MouldPerspective::ToggleControlBlobs(Spread* pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pSpread = pointer to a spread to render into
	Purpose:	

********************************************************************************************/

void MouldPerspective::ToggleControlBlobs(Spread* pSpread)
{
	// if we're in a suspended state do nothing!
	if (GridState>1) return;
	// toggle the grid state.
	GridState ^= 1;
	if (pSpread!=NULL)
 		RenderGrid(pSpread);
}

/*******************************************************************************************

>	virtual void MouldPerspective::Enable/DisableControlBlobs()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		-
	Purpose:	Used when an perspective is undergoing a drag, we disable normal blob rendering
				
********************************************************************************************/

void MouldPerspective::EnableControlBlobs()
{
	if (GridState>1) GridState--;
}

void MouldPerspective::DisableControlBlobs()
{
	if (GridState>0) GridState++;
}


/*******************************************************************************************

>	virtual void MouldPerspective::RenderControlBlobs(RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pRegion = pointer to a region to render into
	Purpose:	Draw on the perspective grid points on if they are supposed to be shown.

********************************************************************************************/

void MouldPerspective::RenderControlBlobs(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(pRegion==NULL,"MouldPerspective::RenderControlBlobs passed a NULL region");
	if (pRegion==NULL)
		return;
	if (GridState==1)
		RenderGrid(pRegion);

	RenderVanishingPoints(pRegion);
#endif
}


/*******************************************************************************************

>	virtual void MouldPerspective::RenderDragBlobs(RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pRegion = pointer to a region to render into
	Purpose:	Draw on the perspective grid points on if they are supposed to be shown.

********************************************************************************************/

void MouldPerspective::RenderDragBlobs(Spread* pSpread)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(pSpread==NULL,"MouldPerspective::RenderControlBlobs passed a NULL spread");
	if (pSpread==NULL)
		return;
	if (GridState>0)
		RenderGrid(pSpread);

	RenderVanishingPoints(pSpread);
#endif
}

/*******************************************************************************************

>	void MouldPerspective::RenderGrid(Spread* pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pSpread = pointer to a spread to render into
	Purpose:	Draw on the perspective grid points on if they are supposed to be shown.

********************************************************************************************/

void MouldPerspective::RenderGrid(Spread* pSpread)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(pSpread==NULL,"MouldPerspective::RenderGrid() passed a NULL spread");
	DocRect Rect = Perspective.GetBoundingRect();
	RenderRegion* pRegion = DocView::RenderOnTop( &Rect, pSpread, ClippedEOR );
	while ( pRegion )
	{
		RenderGrid(pRegion);
		pRegion = DocView::GetNextOnTop(NULL);
	}	
#endif
}


/*******************************************************************************************

>	virtual void MouldPerspective::RenderGrid(RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pRegion = pointer to a region to render into
	Purpose:	Draw on the perspective vanishing points if they intersect the current
				region

********************************************************************************************/

void MouldPerspective::RenderGrid(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(pRegion==NULL,"MouldPerspective::RenderBlobs() passed a NULL region");
	if (pRegion==NULL) return;

	// get the base class to render
	MouldGeometry::RenderControlBlobs(pRegion);
	// Now render any vanishing points
	pRegion->SetLineWidth(0); 								// Means single-pixel lines
	pRegion->SetLineColour(GREEN);
	pRegion->SetFillColour(GREEN);
	// now render the grid
	RenderGridPoints(pRegion);
#endif
}



/*******************************************************************************************

>	void MouldPerspective::RenderGridPoints(RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/01/95
	Inputs:		pRegion = pointer to a region to render into
	Purpose:	Draw all the prespective grid points on if they are supposed to be shown.

********************************************************************************************/

void MouldPerspective::RenderGridPoints(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// We need to scan through the origin bounding rectangle, calculating
	// an MxN rectangle of points, transforming them all and rendering them to the region

	INT32 p0 = 8;
	INT32 p1 = 4*p0;
	INT32 a,b,x,y;
	POINT sU,dU;
	DocCoord pU;
	RECT Bounds = Perspective.GetSourceBBox();
	INT32 dX = Bounds.right - Bounds.left;
	INT32 dY = Bounds.top - Bounds.bottom;

	for (a=1; a<p0; a++)
	{
		x = Bounds.left + a*dX/p0;
		y = Bounds.bottom + a*dY/p0;

		for (b=1; b<p1; b++)
		{
			if (b % (p1/p0))
			{
				sU.x = Bounds.left + b*dX/p1;
				sU.y = y;
				Perspective.FitPoint(sU,dU);
				pU.x = dU.x;
				pU.y = dU.y;
				pRegion->DrawPixel(pU); 
			}

			sU.x = x;
			sU.y = Bounds.bottom + b*dY/p1;
			Perspective.FitPoint(sU,dU);
			pU.x = dU.x;
			pU.y = dU.y;
			pRegion->DrawPixel(pU);
		}
	}
#endif
}


/*******************************************************************************************

>	void MouldPerspective::RenderVanishingPoints(Spread* pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/95
	Inputs:		pSpread = pointer to a spread to render into
	Purpose:	Draw on the perspective vanishing points on.

********************************************************************************************/

void MouldPerspective::RenderVanishingPoints(Spread* pSpread)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(pSpread==NULL,"MouldPerspective::RenderVanishingPoints() passed a NULL spread");
	DocCoord v0,v1;
 	GetVanishingPoints(v0,v1);
	RenderVanishingPoint(pSpread,v0);
	RenderVanishingPoint(pSpread,v1);
#endif
}


/*******************************************************************************************

>	void MouldPerspective::RenderVanishingPoint(Spread* pSpread, const DocCoord& dcPos)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/95
	Inputs:		dcPos = a doc coord at which to display the vanish point
	Purpose:	Draws a particular prespective vanishing point on.

********************************************************************************************/

void MouldPerspective::RenderVanishingPoint(Spread* pSpread, const DocCoord& dcPos)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect Rect;
	if (dcPos.x!=PER_UNDEFINED)
	{
		Rect=CalcBlobClipRect(dcPos);
		RenderRegion* pRegion = DocView::RenderOnTop(&Rect, pSpread, ClippedEOR );
		while ( pRegion )
		{
			pRegion->SetLineColour(COLOUR_XORSELECT);
PORTNOTE("other", "DrawBitmapBlob removed");
//			pRegion->DrawBitmapBlob(dcPos, _R(IDBMP_HANDLE_CENTRE));
			pRegion = DocView::GetNextOnTop(NULL);
		}	
	}
#endif
}

/*******************************************************************************************

>	void MouldPerspective::RenderVanishingPoints(RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/95
	Inputs:		pRegion = pointer to a region to render into
	Purpose:	Draws the prespective vanishing points on.

********************************************************************************************/

void MouldPerspective::RenderVanishingPoints(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	pRegion->SetLineColour(COLOUR_XORSELECT);

	// first read the vanishing point positions
	DocCoord v0,v1;
 	GetVanishingPoints(v0,v1);

PORTNOTE("other", "DrawBitmapBlob removed");
//	if (v0.x!=PER_UNDEFINED)
//		pRegion->DrawBitmapBlob(v0, _R(IDBMP_HANDLE_CENTRE));
//	if (v1.x!=PER_UNDEFINED)
//		pRegion->DrawBitmapBlob(v1, _R(IDBMP_HANDLE_CENTRE));
#endif
}


void MouldPerspective::ShowVPoints()
{
	DocCoord v0,v1;
 	GetVanishingPoints(v0,v1);
	// TRACEUSER( "Mike", _T("V0=(%d,%d)\n"),v0.x,v0.y);
	// TRACEUSER( "Mike", _T("V1=(%d,%d)\n"),v1.x,v1.y);
}


/********************************************************************************************

>	DocRect MouldPerspective::CalcBlobClipRect(const DocCoord& dcPos)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/95
	Inputs:		dcPos		the centre of the blob that the clipping rectangle will surround
	Outputs:	-
	Returns:	A clipping rectangle for use by RenderDragBlobs.
	Purpose:	Constructs a rectangle that surrounds the given point 
	Errors:		-

********************************************************************************************/

DocRect MouldPerspective::CalcBlobClipRect(const DocCoord& dcPos)
{
#if !defined(EXCLUDE_FROM_RALPH)
	BlobManager* pBlobManager = GetApplication()->GetBlobManager();
	INT32 nSize = pBlobManager->GetBlobSize() + 2;
	return DocRect(DocCoord(dcPos.x - nSize, dcPos.y - nSize),
				   DocCoord(dcPos.x + nSize, dcPos.y + nSize));
#else
	return DocRect(0,0,0,0);
#endif
}


/********************************************************************************************

>	virtual DocRect MouldPerspective::GetBlobBoundingRect()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/94
	Returns:	DocRect - Returns the bounding rect of the path and its blobs
	Purpose:	This calculates the bounding box of the mould and adds in the influence of
				the selection blobs. It does not consider if the blobs are visible or not,
				it just gives the bounding box that they would occupy if they were visible

********************************************************************************************/

DocRect MouldPerspective::GetBlobBoundingRect()
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord v0,v1;
 	GetVanishingPoints(v0,v1);

	DocRect Rect;

	if (v0.x!=PER_UNDEFINED)
		Rect=Rect.Union(CalcBlobClipRect(v0));

	if (v1.x!=PER_UNDEFINED)
		Rect=Rect.Union(CalcBlobClipRect(v1));

	return Rect;
#else
	return DocRect(0,0,0,0);
#endif
}


/********************************************************************************************

>	void MouldPerspective::GetVanishingPoints(DocCoord& p0, DocCoord& p1)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/95
	Inputs:
	Outputs:	p0 = the first vanishing point position	 (0x80000000 if none existant)
				p1 = the second vanishing point position (0x80000000 if none existant)
	Returns:	
	Purpose:	Reads the values of this defined perspectives vanishing points.

********************************************************************************************/

void MouldPerspective::GetVanishingPoints(DocCoord& p0, DocCoord& p1)
{
	POINT v0,v1;
	Perspective.VanishingPoints(v0,v1);
	p0.x=v0.x; p0.y=v0.y;
	p1.x=v1.x; p1.y=v1.y;
}


/********************************************************************************************

>	BOOL MouldPerspective::MoveVanishingPoint(DocCoord OldPoint, DocCoord& NewPoint)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/95
	Inputs:		OldPoint = A last known vanishing point position
				NewPoint = What should hopefully become a new position
	Outputs:	NewPoint = The new point will be updated to hold the actual position of
						   the vanishing point. It may be slightly different from NewPoint
						   due to rounding error.
	Returns:	TRUE if the mould vanishing point has been altered, 
				FALSE if no change has occured, due to either
				(1) the old vanishing point could not be located
				(2) the new vanishing point causes the perspective map to become invalid
	Purpose:	Moves a vanishing point from OldPoint to NewPoint and redefines the
				perspective shape to suit.
				If the perspective moves into an invalid position the routine will return
				FALSE and leave the stored perspective as was.

********************************************************************************************/

BOOL MouldPerspective::MoveVanishingPoint(DocCoord OldPoint, DocCoord& NewPoint)
{
#if !defined(EXCLUDE_FROM_RALPH)
	BOOL Flag;
	if (OldPoint.x==PER_UNDEFINED || OldPoint.y==PER_UNDEFINED)
		return FALSE;

	POINT p;
	p.x=OldPoint.x;
	p.y=OldPoint.y;

	if (!Perspective.WhichVanishingPoint(p, Flag))
		return FALSE;

	// Save the current state incase we fail
	POINT TempArray[PER_NUMCOORDS];
	Perspective.CopyShape(TempArray);
	POINT TempArray1[PER_NUMCOORDS];
	Perspective.CopyShape(TempArray1);
	INT32 SCode = Perspective.GetVPointState();

	p.x=NewPoint.x;
	p.y=NewPoint.y;

	Perspective.DragVanishingPoints(p,Flag,TempArray1);
	BuildShape(TempArray1,NULL);
	INT32 ECode = Perspective.GetVPointState();

	// now check it for validity
	if ((!Perspective.Valid()) || (SCode!=ECode))
	{
		// if not valid replace the old data
		BuildShape(TempArray,NULL);
		return FALSE;
	}

	DocCoord v0,v1;
	GetVanishingPoints(v0,v1);
	Flag ? (NewPoint=v1) : (NewPoint=v0);
#endif
	return TRUE;
}



/********************************************************************************************

>	void MouldPerspective::CopyShape(DocCoord* pCoords)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/95
	Inputs:		pCoords = A pointer to an array of doc coords
	Outputs:	pCoords contains the five coordinates of the perspective shape
	Purpose:	Make a copy of the current perspective shape, placing the output coords
				in pCoords, most likely a path coordinate array.
				Note, the routine expects pCoords to point to an array of at least five
				coordinates.

********************************************************************************************/

void MouldPerspective::CopyShape(DocCoord* pCoords)
{
	POINT TempArray[PER_NUMCOORDS];
	Perspective.CopyShape(TempArray);

	INT32				i;
	for( i = 0; i < PER_NUMCOORDS; i++ )
	{
		pCoords[i].x = TempArray[i].x;
		pCoords[i].y = TempArray[i].y;
	}

	// copy the last coordinate in to.
	pCoords[i].x=TempArray[0].x;
	pCoords[i].y=TempArray[0].y;
}


/********************************************************************************************

>	virtual MouldGeometry* MouldPerspective::MakeCopy()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/94
	Inputs:
	Outputs:	a pointer to a new mould perspective object
				NULL if unable to create the object.
	Purpose:	Make a copy of this mould perspective object and return it.

********************************************************************************************/

MouldGeometry* MouldPerspective::MakeCopy()
{
	// create a new shape
	MouldPerspective* pShape = new MouldPerspective;
	if (pShape == NULL)
		return NULL;

	BOOL ok = CopyContents(pShape);
	if (!ok)
	{
		delete pShape;
		return NULL;
	}

	return (pShape);
}


/********************************************************************************************

>	BOOL MouldPerspective::CopyContents(MouldPerspective* pPerspective)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/95
	Inputs:		pPerspective = a pointer to a copy of a perspective object.
	Outputs:	
	Returns:	TRUE if the data has been copied correctly
				FALSE if failed
	Purpose:	Make a copy of this mould perspective class private data

********************************************************************************************/

BOOL MouldPerspective::CopyContents(MouldPerspective* pPerspective)
{
	ERROR3IF(pPerspective==NULL, "MouldPerspective::CopyContents() passed a null pointer");
	// ask the base class to copy its bits first
	if (!MouldGeometry::CopyContents(pPerspective))
		return FALSE;

	// now lets copy data about ourselves
	POINT TempArray[PER_NUMCOORDS];
	RECT TempRect = Perspective.GetSourceBBox();
	DocRect Rect = ConvRectToDocRect(TempRect);
	Perspective.CopyShape(TempArray);

	// define this shape with the details
	return (pPerspective->BuildShape(TempArray, &Rect));
}


/********************************************************************************************

>	DocRect MouldPerspective::GetSourceRect()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/95
	Inputs:		
	Returns:	a doc rect.
	Purpose:	Return the current definition of the mould perspective source rectangle.

********************************************************************************************/

DocRect MouldPerspective::GetSourceRect()
{ 
 	RECT Rect = Perspective.GetSourceBBox();
	return ConvRectToDocRect(Rect);
}



/*******************************************************************************************

>	virtual BOOL MouldPerspective::OnClick( DocCoord Coord, 
											ClickType CType,
											ClickModifiers Mods,
											Spread* pSpread,
											NodeMould* pNodeMould)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		Coord	= coordinate of mouse
				CType	= the type of click (single, double, drag)
				Mods	= the click modifiers
				pSpread	= a pointer to the spread the click occured over
	Returns		TRUE	if we processed the click
				FALSE	if none of the vanishing points were under the click position
	Purpose:	Checks to see if any of the perspective vanishing points have been clicked
				on. If not return FALSE

********************************************************************************************/

BOOL MouldPerspective::OnClick( DocCoord PointerPos, 
								ClickType CType,
								ClickModifiers Mods,
								Spread* pSpread,
								NodeMould* pNodeMould)
{
	BOOL Claimed=FALSE;

#ifndef STANDALONE

	// we only handle the click if we can confirm that object blobs are being displayed.
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr == NULL)
		return FALSE;
	if (!pBlobMgr->GetCurrentInterest().Object)
		return FALSE;

	PerspectiveClick WhatToDo = DetermineClickEffect(PointerPos,pSpread);

	switch (CType)
	{
		case CLICKTYPE_SINGLE:
			switch (WhatToDo)
			{
				case PClick_EditVPoint0:
				{
					Claimed=TRUE;					
					OpDragOrigin* pOpDragOrigin = new OpDragOrigin;
					if (pOpDragOrigin!=NULL)
						pOpDragOrigin->DoDragVanishPoint(pSpread,PointerPos,Mods,pNodeMould,FALSE);
				}
				break;

				case PClick_EditVPoint1:
				{
					Claimed=TRUE;
					OpDragOrigin* pOpDragOrigin = new OpDragOrigin;
					if (pOpDragOrigin!=NULL)
						pOpDragOrigin->DoDragVanishPoint(pSpread,PointerPos,Mods,pNodeMould,TRUE);
				}
				break;
				default: break;
			}					
			break;

		case CLICKTYPE_DOUBLE:
			break;
		case CLICKTYPE_DRAG:
			break;
		default: break;
	}					

#endif

	return Claimed;
}

/*******************************************************************************************

>	virtual BOOL MouldPerspective::OnMouseMove(	DocCoord Coord, 
												Spread* pSpread,
												ClickModifiers Mods
												INT32* ctype,
												INT32* msgres)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		Coord	= coordinate of mouse
				Mods	= the click modifiers
				pSpread	= a pointer to the spread the click occured over
	Returns:	ctype 	= the tool cursor type to use over this point
				msgres	= the message resource type to use over this point
	Purpose:

********************************************************************************************/

BOOL MouldPerspective::OnMouseMove(DocCoord Coord, 
								Spread* pSpread,
								ClickModifiers Mods,
								INT32* ctype,
								INT32* msgres)
{
#ifndef STANDALONE

	ERROR2IF(ctype==NULL,FALSE,"MouldPerspective::OnMouseMove passed a null cursor type ptr");
	ERROR2IF(msgres==NULL,FALSE,"MouldPerspective::OnMouseMove passed a null msg res ptr");

	PerspectiveClick Click = DetermineClickEffect(Coord, pSpread);
	switch (Click)
	{
		case PClick_EditVPoint0:
		case PClick_EditVPoint1:
			*ctype = 2;
			*msgres = 2;
			return TRUE;
			break;
		default: break;
	}

#endif

	return FALSE;
}

/**************************************************************************************************************

> 	PerspectiveClick MouldPerspective::DetermineClickEffect(DocCoord PointerPos, Spread* pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/1/95
	Inputs:		PointerPos is the mouse position
				pSpread is a pointer to the spread containing the mouse position
	Outputs:	-
	Returns:	A click type, which defines what edit action should be taken.
	Purpose:	Used when single clicking. This routine determines what effect a click will have.
				The perspective mould provides draggable vanishing point blobs so we need to check
				whether someones trying to click and drag one of these.

***************************************************************************************************************/

PerspectiveClick MouldPerspective::DetermineClickEffect(DocCoord PointerPos,Spread* pSpread)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect Rect;
	DocCoord v0,v1;

	GetVanishingPoints(v0,v1);

	if (v0.x!=PER_UNDEFINED)
	{
		Rect=CalcBlobClipRect(v0);
		if (Rect.ContainsCoord(PointerPos))
			return PClick_EditVPoint0;
	}

	if (v1.x!=PER_UNDEFINED)
	{
		Rect=CalcBlobClipRect(v1);
		if (Rect.ContainsCoord(PointerPos))
			return PClick_EditVPoint1;
	}

#endif
	return PClick_DoNothing;	
}




/********************************************************************************************

>	virtual void MouldPerspective::Transform(Path* const pNewGeom,
											 DocRect* const pRect,
											 TransformBase& Trans )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pNewGeom = a new mould shape to use.
				Trans	 = The transform to be applied.
	Purpose:	As the actual perspective mould shape is a nodepath inside the mould parent
				which this mouldperspecitive object is associated with, the path is passed
				into this function whenever the mould parent receives a transform call.
				The path has already been transformed so all we need to do here is update
				the low level perspective bits.

********************************************************************************************/

void MouldPerspective::Transform(Path* const pNewGeom,
								 DocRect* const pRect,
								 TransformBase& Trans )
{
	// transform the blob origins, vanishing points etc
	Define(pNewGeom,pRect);
}



/***********************************************************************************************

>	virtual ChangeCode MouldPerspective::RecordContext(UndoableOperation* pOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		pOp	= pointer to a running undoable operation
	Outputs:	-
	Purpose:	Record our context on the undo. In this overridden function we simply create
				an action to perform the undo/redo of our perspective shape.

***********************************************************************************************/

ChangeCode MouldPerspective::RecordContext(UndoableOperation* pOp)
{
	if (pOp!=NULL)
	{
		RecordPerspectiveAction* PerAction;
		ActionCode Act;

		// call the actions static init function to get the action going.
		Act = RecordPerspectiveAction::Init(pOp, pOp->GetUndoActionList(), this, (Action**)(&PerAction));

		if (Act == AC_FAIL)
			return CC_FAIL;
		if (Act == AC_NORECORD)
			return CC_NORECORD;
	}
	return CC_OK;
}


/***********************************************************************************************

>	virtual ChangeCode MouldPerspective::RecordBlobs(UndoableOperation* pOp, Spread* pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95
	Inputs:		pOp	= pointer to a running undoable operation
	Outputs:	-
	Purpose:	Record our blobs on the undo. In this overridden function we simply store
				our blob rectangles so that they will redraw correctly during undo and
				redo.

***********************************************************************************************/

ChangeCode MouldPerspective::RecordBlobs(UndoableOperation* pOp, Spread* pSpread)
{
#ifndef STANDALONE

	if (pOp!=NULL)
	{
		ActionCode	Act;
		DocCoord 	v0,v1;
		DocRect 	Rect;

		GetVanishingPoints(v0,v1);

		if (v0.x!=PER_UNDEFINED)
		{
			Rect=CalcBlobClipRect(v0);
			Act=InvalidateRectAction::DoRecord(pOp,Rect,pSpread);

			if (Act == AC_FAIL) 
				return CC_FAIL;
			if (Act == AC_NORECORD)
				return CC_NORECORD;
		}

		if (v1.x!=PER_UNDEFINED)
		{
			Rect=CalcBlobClipRect(v1);
			Act=InvalidateRectAction::DoRecord(pOp,Rect,pSpread);

			if (Act == AC_FAIL)
				return CC_FAIL;
			if (Act == AC_NORECORD)
				return CC_NORECORD;
		}

	}
#endif

	return CC_OK;
}





/********************************************************************************************

	RecordPerspectiveAction::RecordPerspectiveAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Purpose:	Constructor for the action to undo Perspective modification

********************************************************************************************/

RecordPerspectiveAction::RecordPerspectiveAction()
{
	pCPerspective=NULL;
}


/********************************************************************************************

	RecordPerspectiveAction::~RecordPerspectiveAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Purpose:	Destructor for the action to undo Perspective modification

********************************************************************************************/

RecordPerspectiveAction::~RecordPerspectiveAction()
{
}


/********************************************************************************************

>	ActionCode RecordPerspectiveAction::Init(Operation* pOp,
											 ActionList* pActionList,
											 Action** NewAction)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		pOp 		= a pointer to the operation to which this action belongs
				pActionList = is the action list to which this action should be added

	Outputs:	NewAction 	= a pointer to a pointer to an action, allowing the function to 
							  return a pointer to the created action.
	Returns:	ActionCode, 
					= AC_OK			if the action was created correctly
					= AC_NORECORD	if no memory to undo/redo but go ahead anyway without undo
					= AC_FAIL		stop the operation

	Purpose:	This is the function which creates an instance of this action. If there is
				no room in the undo buffer (which is determined by the base class Init 
				function called within)	the function will either return AC_NORECORD which
				means the operation can continue, but no undo information needs to be stored,
				or AC_OK which means the operation should continue AND record undo information.
				If the function returns AC_FAIL, there was not enough memory to record the 
				undo information, and the user has decided not to continue with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode RecordPerspectiveAction::Init(Operation* pOp,
										 ActionList* pActionList,
										 MouldPerspective* pPerspective,
										 Action** NewAction)
{
	ActionCode Ac = AC_FAIL;
	if (pPerspective!=NULL)
	{
		UINT32 ActSize = sizeof(RecordPerspectiveAction);
		Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(RecordPerspectiveAction), NewAction);
		if (Ac==AC_OK)
		{
			RecordPerspectiveAction* pAct = ((RecordPerspectiveAction*)*NewAction);
			if (pAct)
			{
				pAct->pCPerspective = pPerspective;
				pPerspective->Perspective.CopyShape(pAct->RecordArray);
			}
		}
	}
	return Ac;
}

/********************************************************************************************

>	ActionCode RecordPerspectiveAction::Execute()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, 
					= AC_OK			if the action was created correctly
					= AC_NORECORD	if no memory to undo/redo but go ahead anyway without undo
					= AC_FAIL		stop the operation
	Purpose:	This is the virtual function that is called when the action is executed
				by the Undo/Redo system. This is the function that actually undoes the Perspective
				change action by swapping the current internal definition of the Perspective with
				the contexts of itself.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ActionCode RecordPerspectiveAction::Execute()
{
	// try to create a redo record
	RecordPerspectiveAction* EnvAction;
	ActionCode Act;
	Act = RecordPerspectiveAction::Init(pOperation, pOppositeActLst, pCPerspective, (Action**)(&EnvAction));
	
	// for undo, simply copy 'this' record over the shape.
	// No No , we dont need to check for AC_OK, hands off! we've done all that in the
	// init function which tries to record the current state of the Perspective
	if (pCPerspective)
		pCPerspective->BuildShape(RecordArray,NULL);

	return Act;
}






/********************************************************************************************

>	static BOOL PerspectiveShapes::Rectangular()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/94
	Inputs:		pPath = a pointer to an empty path (should be initialised but empty)
	Outputs:	a unit rectangular perspective, ready to be scaled to fit the selection.
	Returns:	TRUE if the rectangular perspective manifold has been created
				FALSE if no memory to create the perspective.
	Purpose:	Creates a rectangular perspective. The perspective manifold is defined on a
				millipoint square coordinate system. ie its outer coorinates are
				(0,0), (72000,72000).
				So to scale the perspective perform Coord = (ScaleFactor * Coord / 72000)

********************************************************************************************/

BOOL PerspectiveShapes::Rectangular(Path* pPath)
{
	ERROR2IF(pPath==NULL,FALSE,"NULL path pointer passed to PerspectiveShapes::Rectangular()");

	DocCoord point[PER_NUMCOORDS];

	point[0].x = 0;	
	point[0].y = 0;
	point[1].x = 0;
	point[1].y = 72000;
	point[2].x = 72000;
	point[2].y = 72000;
	point[3].x = 72000;
	point[3].y = 0;

	return MakeShape(pPath,point);
}


BOOL PerspectiveShapes::LeftWall(Path* pPath)
{
	ERROR2IF(pPath==NULL,FALSE,"NULL path pointer passed to PerspectiveShapes::LeftWall()");

	DocCoord point[PER_NUMCOORDS];

	point[0].x = 0;	
	point[0].y = 0;
	point[1].x = 0;
	point[1].y = 72000;
	point[2].x = 72000;
	point[2].y = 45000;
	point[3].x = 72000;
	point[3].y = 27000;

	return MakeShape(pPath,point);
}


BOOL PerspectiveShapes::RightWall(Path* pPath)
{
	ERROR2IF(pPath==NULL,FALSE,"NULL path pointer passed to PerspectiveShapes::RightWall()");

	DocCoord point[PER_NUMCOORDS];

	point[0].x = 0;	
	point[0].y = 27000;
	point[1].x = 0;
	point[1].y = 45000;
	point[2].x = 72000;
	point[2].y = 72000;
	point[3].x = 72000;
	point[3].y = 0;

	return MakeShape(pPath,point);
}


BOOL PerspectiveShapes::Floor(Path* pPath)
{
	ERROR2IF(pPath==NULL,FALSE,"NULL path pointer passed to PerspectiveShapes::Floor()");

	DocCoord point[PER_NUMCOORDS];

	point[0].x = 0;	
	point[0].y = 0;
	point[1].x = 27000;
	point[1].y = 72000;
	point[2].x = 45000;
	point[2].y = 72000;
	point[3].x = 72000;
	point[3].y = 0;

	return MakeShape(pPath,point);
}


BOOL PerspectiveShapes::Ceiling(Path* pPath)
{
	ERROR2IF(pPath==NULL,FALSE,"NULL path pointer passed to PerspectiveShapes::Ceiling()");

	DocCoord point[PER_NUMCOORDS];

	point[0].x = 27000;	
	point[0].y = 0;
	point[1].x = 0;
	point[1].y = 72000;
	point[2].x = 72000;
	point[2].y = 72000;
	point[3].x = 45000;
	point[3].y = 0;

	return MakeShape(pPath,point);
}



BOOL PerspectiveShapes::MakeShape(Path* pPath, DocCoord* point)
{
	PathFlags flags;
	flags.IsSelected = FALSE;

	// Build a perspective manifold
	BOOL ok = (pPath->AddMoveTo(point[0],&flags));
	if (!ok) return FALSE;

	ok = (pPath->AddLineTo(point[1],&flags));
	if (!ok) return FALSE;

	ok = (pPath->AddLineTo(point[2],&flags));
	if (!ok) return FALSE;

	ok = (pPath->AddLineTo(point[3],&flags));
	if (!ok) return FALSE;

	ok = (pPath->AddLineTo(point[0],&flags));
	if (!ok) return FALSE;

	ok = (pPath->CloseSubPath());

	return (ok);
}





/////////////////////////////////////////////////////////////////////////////////////////////////
//
// GPerspec.cpp
//
/////////////////////////////////////////////////////////////////////////////////////////////////

GPerspective::GPerspective()
{
	// Initialise the perspective with sensible values
	P0.x = 0; P0.y = 0;
	P1.x = 0; P1.y = 1;
	P2.x = 1; P2.y = 1;
	P3.x = 1; P3.y = 0;

	ScaleView();
	CalcViewConsts();
}


BOOL GPerspective::Redefine( POINT *P )
{
	// Read the four point perspective
	P0.x = P[0].x ; P0.y = P[0].y ;
	P1.x = P[1].x ; P1.y = P[1].y ;
	P2.x = P[2].x ; P2.y = P[2].y ;
	P3.x = P[3].x ; P3.y = P[3].y ;

	// Calc the bounds of the perspective
	CalcViewBBox();
	if (Width<1024) Width=1024;
	if (Depth<1024) Depth=1024;

	ScaleView();
	CalcViewConsts();

	return TRUE;
}


void GPerspective::CalcViewBBox()
{
	MinX = MaxX = P0.x ;
	MinY = MaxY = P0.y ;
	MinX = min( MinX, INT32(P1.x) ) ; MaxX = max( MaxX, INT32(P1.x) ) ;
	MinY = min( MinY, INT32(P1.y) ) ; MaxY = max( MaxY, INT32(P1.y) ) ;
	MinX = min( MinX, INT32(P2.x) ) ; MaxX = max( MaxX, INT32(P2.x) ) ;
	MinY = min( MinY, INT32(P2.y) ) ; MaxY = max( MaxY, INT32(P2.y) ) ;
	MinX = min( MinX, INT32(P3.x) ) ; MaxX = max( MaxX, INT32(P3.x) ) ;
	MinY = min( MinY, INT32(P3.y) ) ; MaxY = max( MaxY, INT32(P3.y) ) ;
	Width = MaxX-MinX ;
	Depth = MaxY-MinY ;
}


void GPerspective::ScaleView()
{
	// normalise the view
	V0.x = (double)(P0.x-MinX)/Width ;
	V0.y = (double)(P0.y-MinY)/Depth ;
	V1.x = (double)(P1.x-MinX)/Width ;
	V1.y = (double)(P1.y-MinY)/Depth ;
	V2.x = (double)(P2.x-MinX)/Width ;
	V2.y = (double)(P2.y-MinY)/Depth ;
	V3.x = (double)(P3.x-MinX)/Width ;
	V3.y = (double)(P3.y-MinY)/Depth ;
}


void GPerspective::CalcViewConsts()
{
	// Calculate the view constants
	double N, D ;
	D = (V1.y-V0.y)*(V2.x-V3.x) - (V1.x-V0.x)*(V2.y-V3.y) ;

	if (( I0flag = (fabs(D) >= EPSILON) ))
	{
		N = (V3.y-V0.y)*(V2.x-V3.x) - (V3.x-V0.x)*(V2.y-V3.y) ;
		I0pos = (N<0.0) ^ (D<0.0) ;
		I0.x = V0.x+(V1.x-V0.x)*N/D	;
		I0.y = V0.y+(V1.y-V0.y)*N/D	;
		A.x = (V0.x-I0.x)*(V1.x-I0.x) ;
		A.y = (V0.y-I0.y)*(V1.y-I0.y) ;
		B.x = (V3.x-I0.x)*(V2.x-I0.x) ;
		B.y = (V3.y-I0.y)*(V2.y-I0.y) ;
	}		

	D = (V0.y-V3.y)*(V1.x-V2.x) - (V0.x-V3.x)*(V1.y-V2.y) ;

	if (( I1flag = (fabs(D) >= EPSILON) ))
	{
		N = (V2.y-V3.y)*(V1.x-V2.x) - (V2.x-V3.x)*(V1.y-V2.y) ;
		I1pos = (N<0.0) ^ (D<0.0) ;
		I1.x = V3.x+(V0.x-V3.x)*N/D	;
		I1.y = V3.y+(V0.y-V3.y)*N/D	;
	}		
}



void GPerspective::CopyShape(POINT* P)
{
	P[0].x = P0.x ; P[0].y = P0.y ; 
	P[1].x = P1.x ; P[1].y = P1.y ;
	P[2].x = P2.x ; P[2].y = P2.y ;
	P[3].x = P3.x ; P[3].y = P3.y ;
}



BOOL GPerspective::Valid()
{
	BOOL F = (
		((V0.x==V1.x) && (V0.y==V1.y)) ||
		((V1.x==V2.x) && (V1.y==V2.y)) ||
		((V2.x==V3.x) && (V2.y==V3.y)) ||
		((V3.x==V0.x) && (V3.y==V0.y))
		);

	if (F) return FALSE;

	F  =  (V1.x-V0.x)*(V3.y-V0.y) > (V1.y-V0.y)*(V3.x-V0.x) ;
	if ( F != ((V2.x-V1.x)*(V0.y-V1.y) > (V2.y-V1.y)*(V0.x-V1.x)) ) return FALSE ;
	if ( F != ((V3.x-V2.x)*(V1.y-V2.y) > (V3.y-V2.y)*(V1.x-V2.x)) ) return FALSE ;
	if ( F != ((V0.x-V3.x)*(V2.y-V3.y) > (V0.y-V3.y)*(V2.x-V3.x)) ) return FALSE ;
	return TRUE ;
}


BOOL GPerspective::WhichVanishingPoint(POINT v, BOOL& b)
{
	if (I0flag)
	{
		POINT a;
		InverseScale(I0,a);
		if (a.x==v.x && a.y==v.y)
		{
			b=FALSE;
			return TRUE;
		}
	}

	if (I1flag)
	{
		POINT a;
		InverseScale(I1,a);
		if (a.x==v.x && a.y==v.y)
		{
			b=TRUE;
			return TRUE;
		}
	}
	return FALSE;
}


INT32 GPerspective::GetVPointState()
{
	INT32 num=0;
	if (I0pos) num+=1;
	if (I1pos) num+=2;
	return num;
}

void GPerspective::VanishingPoints( POINT& a, POINT& b )
{
	if ( I0flag )
		InverseScale( I0, a ) ;
	else
		a.x = a.y = 0x80000000 ;
	if ( I1flag )
		InverseScale( I1, b ) ;
	else
		b.x = b.y = 0x80000000 ;
}



void GPerspective::DragVanishingPoints( POINT& NewI, BOOL Flag, LPPOINT P )
{
	DPOINT I ;
	Scale( NewI, I ) ;
	if ( !Flag )
		if ( !I0pos )
		{
			Intersect( I, V0, V1, V2, P[1] ) ;
			Intersect( I, V3, V1, V2, P[2] ) ;
		}
		else
		{
			Intersect( I, V1, V0, V3, P[0] ) ;
			Intersect( I, V2, V0, V3, P[3] ) ;
		}
	else
		if ( I1pos )
		{
			Intersect( I, V0, V3, V2, P[3] ) ;
			Intersect( I, V1, V3, V2, P[2] ) ;
		}
		else
		{
			Intersect( I, V3, V0, V1, P[0] ) ;
			Intersect( I, V2, V0, V1, P[1] ) ;
		}
}

/////////////////////////////////////////////////////////////////////////////////////////////////


BOOL GPerspective::FitPath (
						CONST POINT* IPoints,
						CONST BYTE*  ITypes,
						DWORD ILength,
						LPPOINT pOPoints,
						LPBYTE  pOTypes,
						DWORD pOLength,
						BOOL Close
					)
{
	OPoints = pOPoints ;
	OTypes  = pOTypes  ;
	OLength = pOLength ;

	DPOINT     C2, C4 ;
	DPOINT P0, P2, P4, P6 ;
	DPOINT T0, T2, T4, T6 ;
	while ( ILength )
	{
		switch ( *ITypes & PT_MOVETO )
		{
		case PT_MOVETO :
		case PT_LINETO :
			--ILength ;
			ScaleSrc( *IPoints++, P0 ) ;
			Transform( P0, T0 ) ;
			if ( !GenOp( *ITypes++, T0 ) ) return -1 ;
			break ;
		case PT_BEZIERTO :
			ILength -= 3 ;
			ScaleSrc( *IPoints++, C2 ) ;
			ScaleSrc( *IPoints++, C4 ) ;
			ScaleSrc( *IPoints++, P6 ) ;
			P2.x = (8*P0.x+12*C2.x+ 6*C4.x+  P6.x)/27 ;
			P2.y = (8*P0.y+12*C2.y+ 6*C4.y+  P6.y)/27 ;
			P4.x = (  P0.x+ 6*C2.x+12*C4.x+8*P6.x)/27 ;
			P4.y = (  P0.y+ 6*C2.y+12*C4.y+8*P6.y)/27 ;
			Transform( P2, T2 ) ;
			Transform( P4, T4 ) ;
			Transform( P6, T6 ) ;
			if ( !GenCurve( C2, C4, P0, P2, P4, P6, T0, T2, T4, T6 ) ) return -1 ;
			P0.x = P6.x ; P0.y = P6.y ;
			T0.x = T6.x ; T0.y = T6.y ;
			ITypes += 3 ;
			*(OTypes-1) = *(ITypes-1) ;
			break ;
		default :
			return -1 ;
		}
	}
	if ( ILength )
		return -1 ;
	return pOLength-OLength ;
}



void GPerspective::Transform( DPOINT a, DPOINT& b )
{
	DPOINT p, q ;
	p.x = V0.x ;
	p.y = V0.y ;
	q.x = V3.x ;
	q.y = V3.y ;
	if ( notequal(a.y,0) )
		if ( I0flag )
		{
			if ( notequal(V0.x,V1.x) )
				p.x = I0.x+A.x/(a.y*(V0.x-V1.x)+V1.x-I0.x) ;
			if ( notequal(V0.y,V1.y) )
				p.y = I0.y+A.y/(a.y*(V0.y-V1.y)+V1.y-I0.y) ;
			if ( notequal(V3.x,V2.x) )
				q.x = I0.x+B.x/(a.y*(V3.x-V2.x)+V2.x-I0.x) ;
			if ( notequal(V3.y,V2.y) )
				q.y = I0.y+B.y/(a.y*(V3.y-V2.y)+V2.y-I0.y) ;
		}
		else
		{
			p.x = V0.x+a.y*(V1.x-V0.x) ;
			p.y = V0.y+a.y*(V1.y-V0.y) ;
			q.x = V3.x+a.y*(V2.x-V3.x) ;
			q.y = V3.y+a.y*(V2.y-V3.y) ;
		}
	b.x = p.x ;
	b.y = p.y ;
	if ( notequal(a.x,0) )
		if ( I1flag )
		{
			if ( notequal(p.x,q.x) )
				b.x = I1.x+(p.x-I1.x)*(q.x-I1.x)/(a.x*(p.x-q.x)+q.x-I1.x) ;
			if ( notequal(p.y,q.y) )
				b.y = I1.y+(p.y-I1.y)*(q.y-I1.y)/(a.x*(p.y-q.y)+q.y-I1.y) ;
		}
		else
		{
			b.x = p.x+a.x*(q.x-p.x) ;
			b.y = p.y+a.x*(q.y-p.y) ;
		}
}


void GPerspective::Intersect( DPOINT I, DPOINT P0, DPOINT P1, DPOINT P2, POINT& O )
{
	double N, D ;
	DPOINT T ;
	N = (P1.y-P0.y)*(P2.x-P1.x) - (P1.x-P0.x)*(P2.y-P1.y) ;
	D = ( I.y-P0.y)*(P2.x-P1.x) - ( I.x-P0.x)*(P2.y-P1.y) ;
	T.x = P0.x+(I.x-P0.x)*N/D ;
	T.y = P0.y+(I.y-P0.y)*N/D ;
	InverseScale( T, O ) ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
