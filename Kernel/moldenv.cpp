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
// Envelope shape implementation

/*
*/

#include "camtypes.h"
#include "moldenv.h"
#include "errors.h"
#include "paths.h"
#include "rndrgn.h"
//#include "osrndrgn.h"
#include "docview.h"
#include "ops.h"
#include "trans2d.h"
#include "pathproc.h"
//#include "mike.h"
#include "genv.h"

#include <math.h>

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC(MouldEnvelopeBase,MouldGeometry)
CC_IMPLEMENT_DYNAMIC(MouldEnvelope,MouldEnvelopeBase)
CC_IMPLEMENT_DYNAMIC(MouldEnvelope2x2,MouldEnvelopeBase)
CC_IMPLEMENT_DYNCREATE(RecordEnvelopeAction,Action)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/*******************************************************************************************

>	MouldEnvelopeBase::MouldEnvelopeBase() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		
	Purpose:	EnvelopeBase mould constructor

********************************************************************************************/

MouldEnvelopeBase::MouldEnvelopeBase()
{
	BlobState = 0;
	pEnvelope = NULL;
}


/*******************************************************************************************

>	MouldEnvelopeBase::~MouldEnvelopeBase()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		
	Purpose:	EnvelopeBase mould destructor

********************************************************************************************/

MouldEnvelopeBase::~MouldEnvelopeBase()
{
	if (pEnvelope!=NULL)
	{
		delete pEnvelope;
		pEnvelope=NULL;
	}
}


/*******************************************************************************************

>	BOOL MouldEnvelopeBase::ValidMouldingPath(INT32 ncoords, INT32 nelements, Path* const pPath, UINT32& errorID)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		
	Purpose:	Check to see whether a mould path is suitable for use

********************************************************************************************/

BOOL MouldEnvelopeBase::ValidMouldingPath(INT32 ncoords, INT32 nelements, Path* const pPath, UINT32& errorID)
{
	// now we need a more complex check
	DocCoord* Coords = pPath->GetCoordArray();
	PathVerb* Verbs = pPath->GetVerbArray();

	// we need the first element to be a move to
	if (Verbs[0]!=PT_MOVETO)
	{
		errorID = _R(IDE_ENV_BADELEMENT);
		return FALSE;
	}

	// followed by either lineto or bezierto els
	INT32 nume=0;
	PathVerb V;

	for (INT32 i=1; i<ncoords; i++)
	{
		V = Verbs[i] & (~PT_CLOSEFIGURE);
		switch (V)
		{
			case PT_LINETO:
				nume+=1;
				break;
			case PT_BEZIERTO:
				nume+=1; i+=2;
				break;
			default:
				errorID = _R(IDE_ENV_NUMCOORDERR);
				return FALSE;
				break;
		}
		if (nume>nelements)
		{
			errorID = _R(IDE_ENV_NUMCOORDERR);
			return FALSE;
		}
	}

	// And the last coordinate must close the loop
	if (Coords[0]!=Coords[ncoords-1])
	{
		errorID = _R(IDE_ENV_NOTCLOSED);
		return FALSE;
	}

	return TRUE;
}


/*******************************************************************************************

	static BOOL MouldEnvelopeBase::WillBeValid(INT32 ncoords, POINT* P)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		ncoords = the number of coordinates in P
				P a pointer to a POINT array of coordinates
	Returns:	TRUE if the point set is valid
	Purpose:	Check whether the point array suited for use as a manifold

********************************************************************************************/

BOOL MouldEnvelopeBase::WillBeValid(INT32 ncoords, POINT* P)
{
	INT32 MinX, MaxX, MinY, MaxY, Width, Depth ;

	MinX = MaxX = P[0].x ;
	MinY = MaxY = P[0].y ;

	for (INT32 i=1; i<ncoords; i++)
	{
		MinX = min( MinX, INT32(P[i].x) );
		MaxX = max( MaxX, INT32(P[i].x) );
		MinY = min( MinY, INT32(P[i].y) );
		MaxY = max( MaxY, INT32(P[i].y) );
	}

	Width = MaxX-MinX ;
	Depth = MaxY-MinY ;

	if (Width==0 || Depth==0)
		return FALSE;

	return TRUE;
}


/*******************************************************************************************

>	BOOL MouldEnvelopeBase::Define(Path* const pPath, DocRect* const pOrigBBox, INT32 ncoords)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pPath	  =	the path to use for enveloping
				pOrigBBox = a pointer to the bounding box of the objects to be enveloped
						  = NULL then the old bounding box will be assumed.
	returns:	TRUE 	- then SetUp has succeded
				FALSE 	- then setup has failed to initialise itself
	Purpose:	This function sets the low level envelope state ready for calls to various
				envelope path generation functions. It serves simply as a way of informing
				the low level enveloper of the shape of the current envelope

********************************************************************************************/

BOOL MouldEnvelopeBase::Define(Path* const pPath, DocRect* const pOrigBBox, INT32 ncoords)
{
	ERROR2IF(pPath==NULL,FALSE,"MouldEnvelopeBase::Define() called with NULL path");
	ERROR2IF(ncoords>ENV_MAXCOORDS, FALSE, "MouldEnvelopeBase::Define() - ncoords too large");
	INT32 numc = pPath->GetNumCoords();

	POINT* pPoints = (POINT*)pPath->GetCoordArray();
	POINT  qPoints[ENV_MAXCOORDS+1];
	POINT* rPoints = pPoints;

	if (numc<(ncoords+1))
	{
		if (!ConvertShape(ncoords,qPoints,pPath,numc))
			return FALSE;
		rPoints = qPoints;
	}

	return BuildShape(rPoints,pOrigBBox);
}


/*******************************************************************************************

>	RECT MouldEnvelopeBase::BuildShape(POINT* pPoints, DocRect* const pOrigBBox) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/01/95
	Inputs:		pPoints		= a pointer to a array of INT32 coordinates
				pOrigBBox 	= a pointer to the bounding box of the objects to be enveloped
						  	= NULL then the old bounding box will be assumed.
	Returns:	TRUE 	- then BuildShape has succeded
				FALSE 	- then BuildShape has failed to create the mould shape
	Purpose:	

********************************************************************************************/

BOOL MouldEnvelopeBase::BuildShape(POINT* pPoints, DocRect* const pOrigBBox)
{
	ERROR2IF(pPoints==NULL,FALSE,"MouldEnvelopeBase::BuildShape() called with NULL point list");
	ERROR3IF(pEnvelope==NULL,"MouldEnvelopeBase::BuildShape() - pEnvelope is NULL!");

	if (pOrigBBox!=NULL)
	{
		if (pOrigBBox->IsEmpty())
			return FALSE;
		if (!pOrigBBox->IsValid())
			return FALSE;
	}

	RECT ObjRect = BuildRectangle(pOrigBBox);
	return pEnvelope->Define(pPoints,&ObjRect,MouldThreshold);
}


/*******************************************************************************************

>	RECT MouldEnvelopeBase::BuildRectangle(DocRect* const pOrigBBox) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/01/95
	Inputs:		pOrigBBox = a pointer to the bounding box of the objects to be enveloped
						  = NULL then the old bounding box will be assumed.
	Returns:	a RECT structure.			
	Purpose:	Build a RECT structure to pass to the low level envelope code. 

********************************************************************************************/

RECT MouldEnvelopeBase::BuildRectangle(DocRect* const pOrigBBox)
{
	ERROR3IF(pEnvelope==NULL,"MouldEnvelopeBase::BuildRectangle() - pEnvelope is NULL!");
	RECT ObjRect;

	if (pOrigBBox!=NULL)
	{
		// set up the low level envelope data.
		ObjRect.left 	= pOrigBBox->lo.x;
		ObjRect.bottom 	= pOrigBBox->lo.y;
		ObjRect.right 	= pOrigBBox->hi.x;
		ObjRect.top 	= pOrigBBox->hi.y;
	}
	else
		ObjRect = pEnvelope->GetSourceBBox();

	return ObjRect;
}




/*******************************************************************************************

>	virtual void MouldEnvelopeBase::ToggleControlBlobs(Spread* pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pSpread = pointer to a spread to render into
	Purpose:	

********************************************************************************************/

void MouldEnvelopeBase::ToggleControlBlobs(Spread* pSpread)
{
	// if we're in a suspended state do nothing!
	if (BlobState>1) return;
	// toggle the grid state.
	BlobState ^= 1;

	if (pSpread!=NULL)
 		RenderGrid(pSpread);
}

/*******************************************************************************************

>	virtual void MouldEnvelopeBase::Enable/DisableControlBlobs()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		-
	Purpose:	Used when an envelope is undergoing a drag, we disable normal blob rendering
				
********************************************************************************************/

void MouldEnvelopeBase::EnableControlBlobs()
{
	if (BlobState>1) BlobState--;
}

void MouldEnvelopeBase::DisableControlBlobs()
{
	if (BlobState>0) BlobState++;
}


/*******************************************************************************************

>	virtual void MouldEnvelopeBase::RenderControlBlobs(RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pRegion = pointer to a region to render into
	Purpose:	Draw on the envelope grid points on if they are supposed to be shown.

********************************************************************************************/

void MouldEnvelopeBase::RenderControlBlobs(RenderRegion* pRegion)
{
	ERROR3IF(pRegion==NULL,"MouldEnvelopeBase::RenderControlBlobs passed a NULL region");
	if (pRegion==NULL) return;
	if (BlobState!=1) return;

	RenderGrid(pRegion);
}


/*******************************************************************************************

>	virtual void MouldEnvelopeBase::RenderDragBlobs(RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pRegion = pointer to a region to render into
	Purpose:	Draw on the envelope grid points on if they are supposed to be shown.

********************************************************************************************/

void MouldEnvelopeBase::RenderDragBlobs(Spread* pSpread)
{
	ERROR3IF(pSpread==NULL,"MouldEnvelopeBase::RenderDragBlobs passed a NULL spread");
	if (pSpread==NULL) return;
	if (BlobState<1) return;

 	RenderGrid(pSpread);
}


/*******************************************************************************************

>	void MouldEnvelopeBase::RenderGrid(Spread* pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pSpread = pointer to a spread to render into
	Purpose:	Draw on the envelope grid points on if they are supposed to be shown.

********************************************************************************************/

void MouldEnvelopeBase::RenderGrid(Spread* pSpread)
{
	ERROR3IF(pEnvelope==NULL,"MouldEnvelopeBase::RenderGrid - no envelope pointer");
	DocRect Rect = pEnvelope->GetBoundingRect();
	RenderRegion* pRegion = DocView::RenderOnTop( &Rect, pSpread, ClippedEOR );
	while ( pRegion )
	{
		RenderGrid(pRegion);
		pRegion = DocView::GetNextOnTop(NULL);
	}	
}


/*******************************************************************************************

>	void MouldEnvelopeBase::RenderGrid(RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pRegion = pointer to a region to render into
	Purpose:	Draw on the envelope grid points on if they are supposed to be shown.

********************************************************************************************/

void MouldEnvelopeBase::RenderGrid(RenderRegion* pRegion)
{
	ERROR3IF(pRegion==NULL,"MouldEnvelopeBase::RenderControlBlobs() passed a NULL region");
	if (pRegion==NULL) return;

	// get the base class to render
	MouldGeometry::RenderControlBlobs(pRegion);
	// set the colour of our outline
	pRegion->SetLineWidth(0); 								// Means single-pixel lines
	pRegion->SetLineColour(COLOUR_XOREDIT);
	pRegion->SetFillColour(COLOUR_XOREDIT);
	// now render the grid
	RenderGridPoints(pRegion);
}


/*******************************************************************************************

>	void MouldEnvelopeBase::RenderGridPoints(RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/01/95
	Inputs:		pRegion = pointer to a region to render into
	Purpose:	Draw on the envelope grid points on if they are supposed to be shown.

********************************************************************************************/

void MouldEnvelopeBase::RenderGridPoints(RenderRegion* pRegion)
{
	// We need to scan through the origin bounding rectangle, calculating
	// an MxN rectangle of points, transforming them all and rendering them to the region

	ERROR3IF(pEnvelope==NULL,"MouldEnvelopeBase::RenderGridPoints() - pEnvelope is NULL!");

	INT32 p0 = 8;
	INT32 p1 = 4*p0;
	INT32 a,b,x,y;
	POINT sU,dU;
	DocCoord pU;
	RECT Bounds = pEnvelope->GetSourceBBox();
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
				pEnvelope->FitPoint(sU,dU);
				pU.x = dU.x;
				pU.y = dU.y;
				pRegion->DrawPixel(pU);
			}

			sU.x = x;
			sU.y = Bounds.bottom + b*dY/p1;
			pEnvelope->FitPoint(sU,dU);
			pU.x = dU.x;
			pU.y = dU.y;
			pRegion->DrawPixel(pU);
		}
	}
}



/********************************************************************************************

>	DocRect MouldEnvelopeBase::GetSourceRect()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/95
	Inputs:		
	Returns:	a doc rect.
	Purpose:	Return the current definition of the mould envelope source rectangle.

********************************************************************************************/

DocRect MouldEnvelopeBase::GetSourceRect()
{ 
	ERROR3IF(pEnvelope==NULL,"MouldEnvelopeBase::GetSourceRect() - pEnvelope is NULL!");

 	RECT Rect = pEnvelope->GetSourceBBox();
	return ConvRectToDocRect(Rect);
}


/*******************************************************************************************

>	virtual BOOL MouldEnvelopeBase::MouldPathToPath(Path* pSourcePath, Path* pDestinPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pSourcePath = pointer to a path object to mould
				pDestinPath = pointer to a path buffer to put the resulting moulded path in
							  this should be at least an initialise path.
	Outputs:	pDestinPath contains the moulded version of the path
	Returns:	TRUE	- if the mould was successfull
				FALSE	- if no room to create the mould

	Purpose:	Using the defined envelope, this function takes the input path shape and
				generates a moulded output path. The output path may contain data on entry
				to this routine, the enveloped path will simply be added to the end of the
				data already there.	If the input path is closed, the output path will be
				closed likewise.

********************************************************************************************/

BOOL MouldEnvelopeBase::MouldPathToPath(Path* pSourcePath, Path* pDestinPath)
{
	ERROR2IF(pSourcePath==NULL, FALSE, "MouldEnvelopeBase::MouldPathToPath() passed a NULL source pointer");
	ERROR2IF(pDestinPath==NULL, FALSE, "MouldEnvelopeBase::MouldPathToPath() passed a NULL destin pointer");

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
		added = pEnvelope->FitPath(icoords,iverbs,ilength,ocoords,overbs,freespace,FALSE);

		if (added==-1)
		{
			freespace*=2;
			// create what we think will be enough space for the output path
			ok = TempPath.EnsureVolume(freespace);
			if (!ok) 
				return FALSE;
		}

	} while ((added==-1) && freespace<=maxspace);

	// if we've failed to create the enveloped object then tidy up
	if (added<=1)
		return FALSE;

	// now lets set up the new flags array
	ok = TempPath.ExternalArraysReplaced(added);
	if (ok)
		TempPath.InitialiseFlags(0,added);

	if (ok)
	{
/*
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

>	virtual BOOL MouldEnvelopeBase::MouldPoint(DocCoord p, DocCoord& q)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		p = the coordinate to be transformed
	Outputs:	q = the transformed version of the coordinate
	Returns:	TRUE if the point has been transformed
				FALSE if q==p
	Purpose:	Using the defined envelope, this function takes the input coordinate
				and calculates its transformed version.

********************************************************************************************/

BOOL MouldEnvelopeBase::MouldPoint(DocCoord p,DocCoord& q)
{
	// I aught to check that the point as contained within the source rectangle
	// bbox and do something about it if not. But what to do?

	RECT rect = pEnvelope->GetSourceBBox();

	if	((p.x<rect.left) || (p.x>rect.right) ||
		(p.y<rect.bottom) || (p.y>rect.top))
	{
		q=p;
		return FALSE;
	}

	POINT r,s;

	r.x = p.x;
	r.y = p.y;

	pEnvelope->FitPoint(r,s);

	q.x = s.x;
	q.y = s.y;

	return TRUE;
}


/*******************************************************************************************

>	virtual BOOL MouldEnvelopeBase::MouldBitmapToTile(Blit* pSourceBlit, Blit* pDestinBlit)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pSourceBlit = pointer to a bitmap to envelope
				pDestinBlit = pointer to an output tile to mould into
	Outputs:	pDestinBlit = enveloped version of the bitmap
	Returns:	TRUE	- if the mould was successfull
				FALSE	- if no room to create the mould
	Purpose:	Create an enveloped version of a bitmap, ready to be rendered to a device

********************************************************************************************/

BOOL MouldEnvelopeBase::MouldBitmapToTile(KernelBitmap* pSourceBlit, KernelBitmap* pDestinBlit)
{
	// currently not implemented
	return FALSE;
}


/*******************************************************************************************

>	BOOL MouldEnvelopeBase::ConvertShape(INT32 ncoords, POINT* dPoints, Path* const pPath, const INT32 numcoords)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		ncoords   = number of output coordinates in this mould
				dPoints	  = array of points to hold the calculated coordinates
				pPath	  =	the path to use for enveloping
				numcoords = the number of coordinaties in pPath
	Outputs:	dPoints	  = the destination point set.
	returns:	TRUE 	- then the shape has been converted.
				FALSE 	- then this convert has failed.
	Purpose:	Converts a path into a shape suitable for use as a mould. This function can
				be used in two contexts, dependent on the value of dPoints. If this is NULL
				on entry, an output path will be calculated but not stored in the output
				buffer (obvoiusly). You could then use the return value of the function for
				input path validation ie if the function returns false to this type of call
				you can assume the input path is not valid ie cannot be converted. You could
				assume this of course whatever dPoints is set to but it gives you the ability
				to first try to convert the path and then allocate space for the output if
				you so wish.

********************************************************************************************/

BOOL MouldEnvelopeBase::ConvertShape(INT32 ncoords, POINT* dPoints, Path* const pPath, const INT32 numcoords)
{
	POINT* sPoints = (POINT*)pPath->GetCoordArray();
	PathVerb* sVerbs = pPath->GetVerbArray();

	INT32 out=(ncoords+1);
	INT32 i=0; 
	INT32 j=0;

	while ((i<numcoords) && (out>0))
	{
		switch ((sVerbs[i] & ~PT_CLOSEFIGURE))
		{
			case PT_MOVETO:
			case PT_BEZIERTO:
				out--;
				if (out>=0)
				{
					if (dPoints)
					{
						dPoints[j].x=sPoints[i].x;
						dPoints[j].y=sPoints[i].y;
					}
					i++; j++;
				}
				break;

			case PT_LINETO:
				out-=3;
				if (out>=0)
				{
					if (dPoints)
					{
						dPoints[j+0].x = (2*sPoints[i-1].x + sPoints[i].x)/3;
						dPoints[j+0].y = (2*sPoints[i-1].y + sPoints[i].y)/3;
						dPoints[j+1].x = (sPoints[i-1].x + 2*sPoints[i].x)/3;
						dPoints[j+1].y = (sPoints[i-1].y + 2*sPoints[i].y)/3;
						dPoints[j+2].x = sPoints[i].x;
						dPoints[j+2].y = sPoints[i].y;
					}
					i++; j+=3; 
				}
				break;
			default:
				return FALSE;
				break;
		}
	}
	return (out==0);
}

/*******************************************************************************************

>	virtual void MouldEnvelopeBase::MouldPathRender(Path* pPath, Spread* pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pPath	= pointer to a path to envelope
				pRegion	= pointer to a region to render into
	Purpose:

********************************************************************************************/

void MouldEnvelopeBase::MouldPathRender(Path* pPath, Spread* pSpread)
{
}




/*******************************************************************************************

>	virtual void MouldEnvelopeBase::MouldBitmapRender(KernelBitmap* pBlit,
													  DocCoord* pParallel,
													  RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pBlit		= pointer to a kernel bitmap
				pParallel	= pointer to a four point parallelogram
				pRegion		= pointer to a region to render into
	Purpose:

********************************************************************************************/

void MouldEnvelopeBase::MouldBitmapRender(KernelBitmap* pBlit, 
										  DocCoord* Parallel,
										  RenderRegion* pRegion)
{
	pRegion->SaveContext();

	// No lines on the rectangle
	pRegion->SetLineColour(COLOUR_TRANS);

	pRegion->RestoreContext();

}





///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************

>	MouldEnvelope::MouldEnvelope() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		
	Purpose:	Envelope mould constructor

********************************************************************************************/

MouldEnvelope::MouldEnvelope() : EnvNumCoords(ENV_NUMCOORDS)
{
}

/*******************************************************************************************

>	MouldEnvelope::~MouldEnvelope()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		
	Purpose:	Envelope mould destructor

********************************************************************************************/

MouldEnvelope::~MouldEnvelope()
{
}


/*******************************************************************************************

>	MouldEnvelope::Initialise()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		
	Purpose:	Initialise a mould envelope to contain the correct geometry

********************************************************************************************/

BOOL MouldEnvelope::Initialise()
{
	ERROR3IF(MouldEnvelopeBase::pEnvelope!=NULL, "MouldEnvelope::Initialise() - pEnvelope not NULL!");
	GEnvelope* pGEnvelope = new GEnvelope;
	if (pGEnvelope==NULL)
		return FALSE;
	MouldEnvelopeBase::pEnvelope = pGEnvelope;
	return TRUE;
}


/*******************************************************************************************

>	BOOL MouldEnvelope::Validate(Path* const pPath, UINT32& errorID)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/02/95
	Inputs:		pPath	= the path to use for enveloping
	Outputs:	errorID	= an id of an error string reporting the problem when failed.
	Returns:	TRUE 	- then the path can be used as an envelope
				FALSE 	- then the path is not suitable for use as an envelope
	Purpose:	This function checks whether the path given is suitabile for use as an
				envelope manifold. Valid paths at present are any set of 5 path elements
				starting with a moveto followed by line or bezier elements

********************************************************************************************/

BOOL MouldEnvelope::Validate(Path* const pPath, UINT32& errorID)
{
	// check for a reasonable number of coordinates
	INT32 numc=pPath->GetNumCoords();
	if ((numc<5) || numc>(EnvNumCoords+1)) 
	{
		errorID = _R(IDE_ENV_NUMCOORDERR);
		return FALSE;
	}

	if (!ValidMouldingPath(numc, 4, pPath, errorID))
		return FALSE;

	// Make sure there's no folds either
	POINT* p = (POINT*)pPath->GetCoordArray();
	if (!WillBeValid(p))
	{
		errorID = _R(IDE_ENV_NOWIDTHHEIGHT);
		return FALSE;
	}

	// Ok, now if necessary do a pseudo conversion
	if (numc<(EnvNumCoords+1))
	{
		if (!ConvertShape(EnvNumCoords,NULL,pPath,numc))
		{
			errorID = _R(IDE_ENV_NUMCOORDERR);
			return FALSE;
		}
	}

	// All is well, we should be able to use this shape
	return TRUE;
}



/*******************************************************************************************

	static BOOL MouldEnvelope::WillBeValid(POINT* P)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		P a pointer to a POINT array of coordinates
	Returns:	TRUE if the point set is valid
	Purpose:	Check whether the point array suited for use as a manifold

********************************************************************************************/

BOOL MouldEnvelope::WillBeValid(POINT* P)
{
	return MouldEnvelopeBase::WillBeValid(ENV_NUMCOORDS, P);
}


/*******************************************************************************************

>	BOOL MouldEnvelope::MakeValidFrom(Path **Out, Path *In, INT32 *CornersHint = 0)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		Out = variable to output the new Path in. In = path to create a valid one from.
	Returns:	whether it managed to create one or not
	Purpose:	creates a valid moulding path from an non-invalid path.

********************************************************************************************/

BOOL MouldEnvelope::MakeValidFrom(Path **Out, Path *In, INT32 *CornersHint)
{
	// get the bound rect of the path
	DocRect Bounds = In->GetBoundingRect();
	DocCoord TL(Bounds.lo.x, Bounds.hi.y);
	DocCoord BR(Bounds.hi.x, Bounds.lo.y);

	// run through the path finding points and work out which is closest to each corner
	INT32 Corners[4] = {-1, -1, -1, -1};

	INT32 BottomLeftDist=-1;
	INT32 BottomRightDist=-1;
	INT32 TopLeftDist=-1;
	INT32 TopRightDist=-1;

	// get the coord arrays
	DocCoord *Coords = In->GetCoordArray();
	PathVerb *Verbs = In->GetVerbArray();
	INT32 Entries = In->GetNumCoords();
	if(Entries < 2)
		return FALSE;

	INT32 Elements = 0;

	// run through
	for(INT32 Pass = 0; Pass < 4; Pass++)
	{
		INT32 dy, dx, dist;
		INT32 BezCount = 0;
		INT32 c;

		for(c = 0; c < Entries; c++)
		{
			BOOL LookAtThis = FALSE;

			switch(Verbs[c] & (~PT_CLOSEFIGURE))
			{
				case PT_LINETO:
					LookAtThis = TRUE;
					break;

				case PT_BEZIERTO:
					BezCount++;
					if(BezCount == 3)
					{
						BezCount = 0;
						LookAtThis = TRUE;
					}
					break;

				default:
					break;
			}
						#define CHECKAGAINST(var, vdist, near) { 			\
							dx = Coords[c].x - near.x;						\
							dy = Coords[c].y - near.y;						\
							dist = (INT32)sqrt((double)((dx * dx) + (dy * dy)));	\
							BOOL notused = TRUE;							\
							for(INT32 l = 0; l < 4; l++)						\
							{												\
								if(Corners[l] == c)							\
									notused = FALSE;						\
							}												\
							if(notused && (var == -1 || dist < vdist))		\
							{												\
								vdist = dist;								\
								var = c;									\
							}												\
						}

			if(LookAtThis)
			{
				switch(Pass)
				{
					case 0:
						Elements++;		// only count elements on pass 0
						CHECKAGAINST(Corners[0], BottomLeftDist, Bounds.lo)
						break;

					case 1:
						CHECKAGAINST(Corners[1], TopLeftDist, TL)
						break;
				
					case 2:
						CHECKAGAINST(Corners[2], TopRightDist, Bounds.hi)
						break;

					case 3:
						CHECKAGAINST(Corners[3], BottomRightDist, BR)
						break;

					default:
						ERROR3("Unknown pass in MouldEnvelope::MakeValidFrom");
						break;
				}
			}

			if((Verbs[c] & (~PT_CLOSEFIGURE)) == PT_MOVETO && c != 0)
				break;			// end at end of first sub path
		}
	}

	// got enough elements?
	if(Elements < 4)
	{
		return FALSE;
	}

	// got a hint about the corners?
	if(CornersHint != 0)
	{
		// yes - copy them in to the corners array we already have
		for(INT32 l = 0; l < 4; l++)
		{
			Corners[l] = CornersHint[l];
		}
	}

	// sort the points...
	INT32 s, l;
	for(s = 0; s < 3; s++)
	{
		for(l = 0; l < 3; l++)
		{
			if(Corners[l] > Corners[l+1])
			{
				INT32 Temp = Corners[l];
				Corners[l] = Corners[l+1];
				Corners[l+1] = Temp;
			}
		}
	}

	// make a path object
	Path *OutputShape = new Path;
	if(OutputShape == 0 || !OutputShape->Initialise())
	{
		delete OutputShape;
		return FALSE;
	}

	OutputShape->FindStartOfPath();

	// go through appoximating beziers to each segment
	INT32 Side;
	for(Side = 0; Side < 4; Side++)
	{
		// work out which coords to start from
		INT32 StartCoord = Corners[Side];
		INT32 EndCoord;
		if(Side != 3)
			EndCoord = Corners[Side + 1];
		else
			EndCoord = Corners[0];
	
		ERROR2IF(StartCoord == EndCoord, FALSE, "Two corners the same in MakeValidFrom!");

		// generate a path
		INT32 CoordsNeeded = EndCoord - StartCoord;
		if(CoordsNeeded < 0)
			CoordsNeeded = 0 - CoordsNeeded;

		Path *SidePath = new Path;
		if(SidePath == 0 || !SidePath->Initialise(CoordsNeeded + 8, 12))
		{
			delete SidePath;
			delete OutputShape;
			return FALSE;
		}

		// OK, we've got a path with plenty of space in it - make the path of the side
		SidePath->FindStartOfPath();
	
		DocCoord StartPoint, EndPoint;
		
		// make a moveto... 
		if(!SidePath->InsertMoveTo(Coords[StartCoord]))
			return FALSE;

		StartPoint = Coords[StartCoord];

		// insert the coordinates
		BOOL WillWrap = FALSE;

		if(StartCoord > EndCoord)
			WillWrap = TRUE;

		INT32 At = StartCoord + 1;
		while((At <= EndCoord) || WillWrap)
		{
			// check for wrapping round
			if(WillWrap && (At >= Entries))
			{
				At = 0;
				WillWrap = FALSE;
			}
			else
			{
				switch(Verbs[At] & (~PT_CLOSEFIGURE))
				{
					case PT_LINETO:
						if(!SidePath->InsertLineTo(Coords[At]))
							return FALSE;

						At++;
						break;

					case PT_BEZIERTO:
						if(!SidePath->InsertCurveTo(Coords[At], Coords[At+1], Coords[At+2]))
							return FALSE;

						At += 3;
						break;
		
					default:
						At++;
						break;
				}
			}

		}

		EndPoint = Coords[EndCoord];

		// got it... now approximate a bezier to what we got.

		// find the path length
		ProcessLength PathLengthProcess(64);
		double fPathLength=0;
		if(!PathLengthProcess.PathLength(SidePath, &fPathLength))
			return FALSE;

		// find coords at 33% and 66% of the path length
		ProcessPathDistance PathDistanceProcess(64);

		DocCoord Point1;
		BOOL Found;
		if(!PathDistanceProcess.GetCoordAndTangent(&Point1, 0, &Found, fPathLength / 3, SidePath))
			return FALSE;

		DocCoord Point2;
		if(!PathDistanceProcess.GetCoordAndTangent(&Point2, 0, &Found, (fPathLength * 2) / 3, SidePath))
			return FALSE;
	
		// do some stuff which Mathematica emitted to find the control points for this segment
		DocCoord Control1, Control2;

		Control1.x = ((-5 * StartPoint.x) + (2 * EndPoint.x) + (18 * Point1.x) - (9 * Point2.x)) / 6;
		Control1.y = ((-5 * StartPoint.y) + (2 * EndPoint.y) + (18 * Point1.y) - (9 * Point2.y)) / 6;
		Control2.x = ((2 * StartPoint.x) - (5 * EndPoint.x) - (9 * Point1.x) + (18 * Point2.x)) / 6;
		Control2.y = ((2 * StartPoint.y) - (5 * EndPoint.y) - (9 * Point1.y) + (18 * Point2.y)) / 6;
	
		// if this is the first bit coord, we need a move to
		if(Side == 0)
		{
			if(!OutputShape->InsertMoveTo(StartPoint))
				return FALSE;
		}
		
		// right then, add the nice curve...
		if(!OutputShape->InsertCurveTo(Control1, Control2, EndPoint))
			return FALSE;
	}

	// close the shape
	if(!OutputShape->CloseSubPath())
		return FALSE;
	
	// set the return pointer
	(*Out) = OutputShape;

	return TRUE;
}


/*******************************************************************************************

>	BOOL MouldEnvelope::Define(Path* const pPath, DocRect* const pOrigBBox)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pPath	  =	the path to use for enveloping
				pOrigBBox = a pointer to the bounding box of the objects to be enveloped
						  = NULL then the old bounding box will be assumed.
	returns:	TRUE 	- then SetUp has succeded
				FALSE 	- then setup has failed to initialise itself
	Purpose:	This function sets the low level envelope state ready for calls to various
				envelope path generation functions. It serves simply as a way of informing
				the low level enveloper of the shape of the current envelope

********************************************************************************************/

BOOL MouldEnvelope::Define(Path* const pPath, DocRect* const pOrigBBox)
{
	ERROR2IF(pPath==NULL,FALSE,"MouldEnvelope::Define() called with NULL path");
	return MouldEnvelopeBase::Define(pPath,pOrigBBox,EnvNumCoords);
}


/*******************************************************************************************

>	void MouldEnvelope::Transform(Path* const pPath, DocRect* const pRect, TransformBase& Trans )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/01/95
	Inputs:		pMould	= A pointer to the node mould object associated with this envelope
				Trans	= A transformation object
	Purpose:	Make sure the envelope mould data is transformed correctly.
				Note, all we need do here is redefine the low level envelope by calling
				Define() again with the current NodePath shape. We are ofcourse assuming
				that the child node path has already been transformed and so is correct.
				This is the case for NodeMould.Transform() which calls this function.
				It ensures all children are transformed first before it calls us passing
				in the transformed path itself.

********************************************************************************************/

void MouldEnvelope::Transform(Path* const pPath, DocRect* const pRect, TransformBase& Trans )
{
	ERROR3IF(pPath==NULL,"MouldEnvelope::Transform() passed a NULL path shape");
	Define(pPath,pRect);	
}




/***********************************************************************************************

>	virtual ChangeCode MouldEnvelope::RecordContext(UndoableOperation* pOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		pOp	= pointer to a running undoable operation
	Outputs:	-
	Purpose:	Record our context on the undo. In this overridden function we simply create
				an action to perform the undo/redo of our envelope shape.

***********************************************************************************************/

ChangeCode MouldEnvelope::RecordContext(UndoableOperation* pOp)
{
	if (pOp!=NULL)
	{
		RecordEnvelopeAction* EnvAction;
		ActionCode Act;

		// call the actions static init function to get the action going.
		Act = RecordEnvelopeAction::Init(pOp, pOp->GetUndoActionList(), this, (Action**)(&EnvAction));

		if (Act == AC_FAIL)
			return CC_FAIL;
		if (Act == AC_NORECORD)
			return CC_NORECORD;
	}
	return CC_OK;
}


/********************************************************************************************

>	virtual MouldGeometry* MouldEnvelope::MakeCopy()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/95
	Inputs:		-
	Outputs:	a pointer to a new mould envelope object.
				NULL if unable to create the object.
	Purpose:	Make a copy of this mould envelope object and return it.

********************************************************************************************/

MouldGeometry* MouldEnvelope::MakeCopy()
{
	// create a new Geometry
	MouldEnvelope* pGeometry = new MouldEnvelope;
	if (pGeometry == NULL)
		return NULL;

	if (!pGeometry->Initialise())
	{
		delete pGeometry;
		return NULL;
	}

	if (!CopyContents(pGeometry))
	{
		delete pGeometry;
		return NULL;
	}

	return ((MouldGeometry*)pGeometry);
}


/********************************************************************************************

>	BOOL MouldEnvelope::CopyContents(MouldEnvelope* pEnvelope)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/95
	Inputs:		pEnvelope = a pointer to a copy of a mould envelope.
	Outputs:	
	Returns:	TRUE if the data has been copied correctly
				FALSE if failed
	Purpose:	Make a copy of this mouldenvelope class private data

********************************************************************************************/

BOOL MouldEnvelope::CopyContents(MouldEnvelope* pCopyEnvelope)
{
	ERROR3IF(pCopyEnvelope==NULL, "MouldEnvelope::CopyContents() passed a null pointer");
	// ask the base class to copy its bits first
	if (!MouldGeometry::CopyContents(pCopyEnvelope))
		return FALSE;

	// now lets copy data about ourselves
	POINT* TempArray = new POINT[EnvNumCoords];
	if (TempArray==NULL)
		return FALSE;

	RECT TempRect = pEnvelope->GetSourceBBox();
	DocRect Rect = ConvRectToDocRect(TempRect);
	pEnvelope->CopyShape(TempArray);

	// define this shape with the details
	BOOL ok = (pCopyEnvelope->BuildShape(TempArray, &Rect));

	delete [] TempArray;
	return ok;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************************

>	MouldEnvelope2x2::MouldEnvelope2x2() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		
	Purpose:	Envelope mould constructor

********************************************************************************************/

MouldEnvelope2x2::MouldEnvelope2x2() : EnvNumCoords(ENV_NUMCOORDS2X2)
{
}

/*******************************************************************************************

>	MouldEnvelope2x2::~MouldEnvelope2x2()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		
	Purpose:	Envelope2x2 mould destructor

********************************************************************************************/

MouldEnvelope2x2::~MouldEnvelope2x2()
{
}


/*******************************************************************************************

>	MouldEnvelope2x2::Initialise()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		
	Purpose:	Initialise a mould envelope to contain the correct geometry

********************************************************************************************/

BOOL MouldEnvelope2x2::Initialise()
{
	ERROR3IF(MouldEnvelopeBase::pEnvelope!=NULL, "MouldEnvelope2x2::Initialise() - pEnvelope not NULL!");
	GEnvelope2x2* pGEnvelope = new GEnvelope2x2;
	if (pGEnvelope==NULL)
		return FALSE;
	MouldEnvelopeBase::pEnvelope = pGEnvelope;
	return TRUE;
}


/*******************************************************************************************

>	BOOL MouldEnvelope2x2::Validate(Path* const pPath, UINT32& errorID)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/02/95
	Inputs:		pPath	= the path to use for enveloping
	Outputs:	errorID	= an id of an error string reporting the problem when failed.
	Returns:	TRUE 	- then the path can be used as an envelope
				FALSE 	- then the path is not suitable for use as an envelope
	Purpose:	This function checks whether the path given is suitabile for use as an
				envelope manifold. Valid paths at present are any set of 5 path elements
				starting with a moveto followed by line or bezier elements

********************************************************************************************/

BOOL MouldEnvelope2x2::Validate(Path* const pPath, UINT32& errorID)
{
	// check for a reasonable number of coordinates
	INT32 numc=pPath->GetNumCoords();
	if ((numc<5) || numc>(EnvNumCoords+1)) 
	{
		errorID = _R(IDE_ENV_NUMCOORDERR);
		return FALSE;
	}

	if (!ValidMouldingPath(numc, 8, pPath, errorID))
		return FALSE;

	// Make sure there's no folds either
	POINT* p = (POINT*)pPath->GetCoordArray();
	if (!WillBeValid(p))
	{
		errorID = _R(IDE_ENV_NOWIDTHHEIGHT);
		return FALSE;
	}

	// Ok, now if necessary do a pseudo conversion
	if (numc<(EnvNumCoords+1))
	{
		if (!ConvertShape(EnvNumCoords,NULL,pPath,numc))
		{
			errorID = _R(IDE_ENV_NUMCOORDERR);
			return FALSE;
		}
	}

	// All is well
	return TRUE;
}



/*******************************************************************************************

	static BOOL MouldEnvelope2x2::WillBeValid(POINT* P)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		P a pointer to a POINT array of coordinates
	Returns:	TRUE if the point set is valid
	Purpose:	Check whether the point array suited for use as a manifold

********************************************************************************************/

BOOL MouldEnvelope2x2::WillBeValid(POINT* P)
{
	return MouldEnvelopeBase::WillBeValid(ENV_NUMCOORDS2X2, P);
}


/*******************************************************************************************

>	BOOL MouldEnvelope2x2::MakeValidFrom(Path **Out, Path *In, INT32 *CornersHint = 0)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		Out = variable to output the new Path in. In = path to create a valid one from.
	Returns:	whether it managed to create one or not
	Purpose:	creates a valid moulding path from an non-invalid path.

********************************************************************************************/

BOOL MouldEnvelope2x2::MakeValidFrom(Path **Out, Path *In, INT32 *CornersHint)
{
	return FALSE;
}


/*******************************************************************************************

>	BOOL MouldEnvelope2x2::Define(Path* const pPath, DocRect* const pOrigBBox)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pPath	  =	the path to use for enveloping
				pOrigBBox = a pointer to the bounding box of the objects to be enveloped
						  = NULL then the old bounding box will be assumed.
	returns:	TRUE 	- then SetUp has succeded
				FALSE 	- then setup has failed to initialise itself
	Purpose:	This function sets the low level envelope state ready for calls to various
				envelope path generation functions. It serves simply as a way of informing
				the low level enveloper of the shape of the current envelope

********************************************************************************************/

BOOL MouldEnvelope2x2::Define(Path* const pPath, DocRect* const pOrigBBox)
{
	ERROR2IF(pPath==NULL,FALSE,"MouldEnvelope2x2::Define() called with NULL path");
	return MouldEnvelopeBase::Define(pPath,pOrigBBox,EnvNumCoords);
}

/*******************************************************************************************

>	void MouldEnvelope2x2::Transform(Path* const pPath, DocRect* const pRect, TransformBase& Trans )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/01/95
	Inputs:		pMould	= A pointer to the node mould object associated with this envelope
				Trans	= A transformation object
	Purpose:	Make sure the envelope mould data is transformed correctly.
				Note, all we need do here is redefine the low level envelope by calling
				Define() again with the current NodePath shape. We are ofcourse assuming
				that the child node path has already been transformed and so is correct.
				This is the case for NodeMould.Transform() which calls this function.
				It ensures all children are transformed first before it calls us passing
				in the transformed path itself.

********************************************************************************************/

void MouldEnvelope2x2::Transform(Path* const pPath, DocRect* const pRect, TransformBase& Trans )
{
	ERROR3IF(pPath==NULL,"MouldEnvelope2x2::Transform() passed a NULL path shape");
	Define(pPath,pRect);	
}




/***********************************************************************************************

>	virtual ChangeCode MouldEnvelope2x2::RecordContext(UndoableOperation* pOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		pOp	= pointer to a running undoable operation
	Outputs:	-
	Purpose:	Record our context on the undo. In this overridden function we simply create
				an action to perform the undo/redo of our envelope shape.

***********************************************************************************************/

ChangeCode MouldEnvelope2x2::RecordContext(UndoableOperation* pOp)
{
	if (pOp!=NULL)
	{
		RecordEnvelopeAction* EnvAction;
		ActionCode Act;

		// call the actions static init function to get the action going.
		Act = RecordEnvelopeAction::Init(pOp, pOp->GetUndoActionList(), this, (Action**)(&EnvAction));

		if (Act == AC_FAIL)
			return CC_FAIL;
		if (Act == AC_NORECORD)
			return CC_NORECORD;
	}
	return CC_OK;
}


/********************************************************************************************

>	virtual MouldGeometry* MouldEnvelope2x2::MakeCopy()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/95
	Inputs:		-
	Outputs:	a pointer to a new mould envelope object.
				NULL if unable to create the object.
	Purpose:	Make a copy of this mould envelope object and return it.

********************************************************************************************/

MouldGeometry* MouldEnvelope2x2::MakeCopy()
{
	// create a new Geometry
	MouldEnvelope2x2* pGeometry = new MouldEnvelope2x2;
	if (pGeometry == NULL)
		return NULL;

	if (!pGeometry->Initialise())
	{
		delete pGeometry;
		return NULL;
	}

	if (!CopyContents(pGeometry))
	{
		delete pGeometry;
		return NULL;
	}

	return ((MouldGeometry*)pGeometry);
}


/********************************************************************************************

>	BOOL MouldEnvelope2x2::CopyContents(MouldEnvelope2x2* pEnvelope)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/95
	Inputs:		pEnvelope = a pointer to a copy of a mould envelope.
	Outputs:	
	Returns:	TRUE if the data has been copied correctly
				FALSE if failed
	Purpose:	Make a copy of this mouldenvelope class private data

********************************************************************************************/

BOOL MouldEnvelope2x2::CopyContents(MouldEnvelope2x2* pCopyEnvelope)
{
	ERROR3IF(pCopyEnvelope==NULL, "MouldEnvelope::CopyContents() passed a null pointer");
	// ask the base class to copy its bits first
	if (!MouldGeometry::CopyContents(pCopyEnvelope))
		return FALSE;

	// now lets copy data about ourselves
	POINT* TempArray = new POINT[EnvNumCoords];
	if (TempArray==NULL)
		return FALSE;

	RECT TempRect = pEnvelope->GetSourceBBox();
	DocRect Rect = ConvRectToDocRect(TempRect);
	pEnvelope->CopyShape(TempArray);

	// define this shape with the details
	BOOL ok = (pCopyEnvelope->BuildShape(TempArray, &Rect));

	delete [] TempArray;
	return ok;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

	RecordEnvelopeAction::RecordEnvelopeAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Purpose:	Constructor for the action to undo envelope modification

********************************************************************************************/

RecordEnvelopeAction::RecordEnvelopeAction()
{
	pCEnvelope=NULL;
}


/********************************************************************************************

	RecordEnvelopeAction::~RecordEnvelopeAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Purpose:	Destructor for the action to undo envelope modification

********************************************************************************************/

RecordEnvelopeAction::~RecordEnvelopeAction()
{
}


/********************************************************************************************

>	ActionCode RecordEnvelopeAction::Init(  Operation* pOp,
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

ActionCode RecordEnvelopeAction::Init(  Operation* pOp,
										ActionList* pActionList,
										MouldEnvelopeBase* pRecEnvelope,
										Action** NewAction)
{
	ActionCode Ac = AC_FAIL;
	if (pRecEnvelope!=NULL)
	{
		UINT32 ActSize = sizeof(RecordEnvelopeAction);
		Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(RecordEnvelopeAction), NewAction);
		if (Ac==AC_OK)
		{
			RecordEnvelopeAction* pAct = ((RecordEnvelopeAction*)*NewAction);
			if (pAct)
			{
				// Save a pointer to the Envelope and save the defining shape
				pAct->pCEnvelope = pRecEnvelope;
				pRecEnvelope->pEnvelope->CopyShape(pAct->RecordArray);
			}
		}
	}
	return Ac;
}

/********************************************************************************************

>	ActionCode RecordEnvelopeAction::Execute()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, 
					= AC_OK			if the action was created correctly
					= AC_NORECORD	if no memory to undo/redo but go ahead anyway without undo
					= AC_FAIL		stop the operation
	Purpose:	This is the virtual function that is called when the action is executed
				by the Undo/Redo system. This is the function that actually undoes the envelope
				change action by swapping the current internal definition of the envelope with
				the contexts of itself.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ActionCode RecordEnvelopeAction::Execute()
{
	// try to create a redo record
	RecordEnvelopeAction* EnvAction;
	ActionCode Act;
	Act = RecordEnvelopeAction::Init(pOperation, pOppositeActLst, pCEnvelope, (Action**)(&EnvAction));
	
	// for undo, simply copy 'this' record over the shape.
	// No No , we dont need to check for AC_OK, hands off! we've done all that in the
	// init function which tries to record the current state of the envelope
	if (pCEnvelope)
		pCEnvelope->BuildShape(RecordArray,NULL);

	return Act;
}




/********************************************************************************************

>	static BOOL EnvelopeShapes::Rectangular(Path* pPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/94
	Inputs:		pPath = a pointer to an empty path (should be initialised but empty)
	Outputs:	pPath = a unit rectangular envelope, ready to be scaled to fit the selection.
	Returns:	TRUE 	if the rectangular envelope manifold has been created
				FALSE 	if no memory to create the envelope.

	Purpose:	Creates a rectangular envelope. The envelope manifold is defined on a
				millipoint square coordinate system. ie its outer coorinates are
				(0,0), (72000,72000).
				So to scale the envelope perform Coord = (ScaleFactor * Coord / 72000)

********************************************************************************************/

BOOL EnvelopeShapes::Rectangular(Path* pPath)
{
	ERROR2IF(pPath==NULL,FALSE,"NULL path pointer passed to EnvelopeShapes::Rectangular()");

	PathFlags flags;
	flags.IsSelected = FALSE;

	// Build a default rectangular manifold
	DocCoord point(0,0);
	BOOL ok = (pPath->AddMoveTo(point,&flags));
	if (!ok) return FALSE;

	point.y = 72000;
	ok = (pPath->AddCurveTo(point,&flags));
	if (!ok) return FALSE;

	point.x = 72000;
	ok = (pPath->AddCurveTo(point,&flags));
	if (!ok) return FALSE;

	point.y = 0;
	ok = (pPath->AddCurveTo(point,&flags));
	if (!ok) return FALSE;

	point.x = 0;
	ok = (pPath->AddCurveTo(point,&flags));
	if (!ok) return FALSE;

	ok = (pPath->CloseSubPath());

	return (ok);
}


/********************************************************************************************

>	static BOOL EnvelopeShapes::Rectangular2x2(Path* pPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/94
	Inputs:		pPath = a pointer to an empty path (should be initialised but empty)
	Outputs:	pPath = a unit rectangular envelope, ready to be scaled to fit the selection.
	Returns:	TRUE 	if the rectangular envelope manifold has been created
				FALSE 	if no memory to create the envelope.

	Purpose:	Creates a rectangular envelope. The envelope manifold is defined on a
				millipoint square coordinate system. ie its outer coorinates are
				(0,0), (72000,72000).
				So to scale the envelope perform Coord = (ScaleFactor * Coord / 72000)

********************************************************************************************/

BOOL EnvelopeShapes::Rectangular2x2(Path* pPath)
{
	ERROR2IF(pPath==NULL,FALSE,"NULL path pointer passed to EnvelopeShapes::Rectangular()");

	PathFlags flags;
	flags.IsSelected = FALSE;

	// Build a default rectangular manifold
	DocCoord point(0,0);
	BOOL ok = (pPath->AddMoveTo(point,&flags));
	if (!ok) return FALSE;

	point.y = 72000/2;
	ok = (pPath->AddCurveTo(point,&flags));
	if (!ok) return FALSE;

	point.y = 72000;
	ok = (pPath->AddCurveTo(point,&flags));
	if (!ok) return FALSE;

	point.x = 72000/2;
	ok = (pPath->AddCurveTo(point,&flags));
	if (!ok) return FALSE;

	point.x = 72000;
	ok = (pPath->AddCurveTo(point,&flags));
	if (!ok) return FALSE;

	point.y = 72000/2;
	ok = (pPath->AddCurveTo(point,&flags));
	if (!ok) return FALSE;

	point.y = 0;
	ok = (pPath->AddCurveTo(point,&flags));
	if (!ok) return FALSE;

	point.x = 72000/2;
	ok = (pPath->AddCurveTo(point,&flags));
	if (!ok) return FALSE;

	point.x = 0;
	ok = (pPath->AddCurveTo(point,&flags));
	if (!ok) return FALSE;

	ok = (pPath->CloseSubPath());

	return (ok);
}


/********************************************************************************************

>	static BOOL EnvelopeShapes::Circular(Path* pPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/01/94
	Inputs:		pPath = a pointer to an empty path (should be initialised but empty)
	Outputs:	pPath = a unit cicular envelope, ready to be scaled to fit the selection.
	Returns:	TRUE 	if the circular envelope manifold has been created
				FALSE 	if no memory to create the envelope.

	Purpose:	Creates a circular envelope. The envelope manifold is defined on a
				millipoint square coordinate system. ie its outer coorinates are
				(0,0), (72000,72000).
				So to scale the envelope perform Coord = (ScaleFactor * Coord / 72000)

********************************************************************************************/

BOOL EnvelopeShapes::Circular(Path* pPath)
{
	ERROR2IF(pPath==NULL,FALSE,"NULL path pointer passed to EnvelopeShapes::Circular()");

	PathFlags flags;
	flags.IsSelected = FALSE;
	DocRect Rect (0,0,72000,72000);
	BOOL ok = MakeEllipse(&Rect, pPath, &flags);

	DocCoord* pCoords = pPath->GetCoordArray();
	INT32 numcoords = pPath->GetNumCoords();

	if (ok)
	{
		Matrix Mat0(-36000,-36000);
		Trans2DMatrix Trans0(Mat0);
		Trans0.Transform( pCoords, numcoords );

		ANGLE Ang = (ANGLE)(45);
		Matrix Mat1(Ang);
		Trans2DMatrix Trans1(Mat1);
		Trans1.Transform( pCoords, numcoords );

		Matrix Mat2(36000,36000);
		Trans2DMatrix Trans2(Mat2);
		Trans2.Transform( pCoords, numcoords );
	}
	else
		pPath->ClearPath();

	return ok;
}



/********************************************************************************************

>	static BOOL EnvelopeShapes::Concave(Path* pPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/01/94
	Inputs:		pPath = a pointer to an empty path (should be initialised but empty)
	Outputs:	pPath = a unit concave envelope, ready to be scaled to fit the selection.
	Returns:	TRUE 	if the concave envelope manifold has been created
				FALSE 	if no memory to create the envelope.

	Purpose:	Creates a concave envelope. The envelope manifold is defined on a
				millipoint square coordinate system. ie its outer coorinates are
				(0,0), (72000,72000).
				So to scale the envelope perform Coord = (ScaleFactor * Coord / 72000)

********************************************************************************************/

BOOL EnvelopeShapes::Concave(Path* pPath)
{
	ERROR2IF(pPath==NULL,FALSE,"NULL path pointer passed to EnvelopeShapes::Concave()");

	PathFlags flags;
	flags.IsSelected = FALSE;
	BOOL ok;

	DocRect Rect(0,0,72000,72000);
  	DocCoord p1,p2,p3;

	DocCoord point(0,0);
	ok = (pPath->AddMoveTo(point,&flags));
	if (!ok) return FALSE;

	p1.x = (5 * Rect.lo.x + 3 * Rect.hi.x)/8;
	p1.y = (6 * Rect.lo.y + 2 * Rect.hi.y)/8;
	p2.x = p1.x;
	p2.y = (2 * Rect.lo.y + 6 * Rect.hi.y)/8;
	p3.x = Rect.lo.x;
	p3.y = Rect.hi.y;

	ok = (pPath->AddCurveTo(p1,p2,p3,&flags));
	if (!ok) return FALSE;

	point.x = 72000;
	point.y = 72000;
	ok = (pPath->AddLineTo(point,&flags));
	if (!ok) return FALSE;

	p1.x = (3*Rect.lo.x + 5*Rect.hi.x)/8;
	p1.y = (2*Rect.lo.y + 6*Rect.hi.y)/8;
	p2.x = p1.x;
	p2.y = (6*Rect.lo.y + 2*Rect.hi.y)/8;
	p3.x = Rect.hi.x;
	p3.y = Rect.lo.y;

	ok = (pPath->AddCurveTo(p1,p2,p3,&flags));
	if (!ok) return FALSE;
	
	point.x = 0;
	point.y = 0;
	ok = (pPath->AddLineTo(point,&flags));
	if (!ok) return FALSE;

	ok = (pPath->CloseSubPath());

	return ok;
}


/********************************************************************************************

>	static BOOL EnvelopeShapes::Banner(Path* pPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/01/94
	Inputs:		pPath = a pointer to an empty path (should be initialised but empty)
	Outputs:	pPath = a unit banner envelope, ready to be scaled to fit the selection.
	Returns:	TRUE 	if the banner envelope manifold has been created
				FALSE 	if no memory to create the envelope.

	Purpose:	Creates a banner envelope. The envelope manifold is defined on a
				millipoint square coordinate system. ie its outer coorinates are
				(0,0), (72000,72000).
				So to scale the envelope perform Coord = (ScaleFactor * Coord / 72000)

********************************************************************************************/

BOOL EnvelopeShapes::Banner(Path* pPath)
{
	ERROR2IF(pPath==NULL,FALSE,"NULL path pointer passed to EnvelopeShapes::Banner()");

	PathFlags flags;
	flags.IsSelected = FALSE;
	BOOL ok;

	DocRect Rect(0,0,72000,72000);
  	DocCoord p1,p2,p3;

	DocCoord point(0,0);
	ok = (pPath->AddMoveTo(point,&flags));
	if (!ok) return FALSE;

	point.y = 72000;
	ok = (pPath->AddLineTo(point,&flags));
	if (!ok) return FALSE;

	p1.x = (6*Rect.lo.x + 2*Rect.hi.x)/8;
	p1.y = (3*Rect.lo.y + 5*Rect.hi.y)/8;
	p2.x = (2*Rect.lo.x + 6*Rect.hi.x)/8;
	p2.y = p1.y;
	p3.x = Rect.hi.x;
	p3.y = Rect.hi.y;

	ok = (pPath->AddCurveTo(p1,p2,p3,&flags));
	if (!ok) return FALSE;

	point.x = 72000;
	point.y = 0;
	ok = (pPath->AddLineTo(point,&flags));
	if (!ok) return FALSE;

	p1.x = (2*Rect.lo.x + 6*Rect.hi.x)/8;
	p1.y = (5*Rect.lo.y + 3*Rect.hi.y)/8;
	p2.x = (6*Rect.lo.x + 2*Rect.hi.x)/8;
	p2.y = p1.y;
	p3.x = Rect.lo.x;
	p3.y = Rect.lo.y;

	ok = (pPath->AddCurveTo(p1,p2,p3,&flags));
	if (!ok) return FALSE;
	
	ok = (pPath->CloseSubPath());

	return ok;
}




/*******************************************************************************************

>	static BOOL EnvelopeShapes::MakeEllipse(DocRect* pRect, Path* pPath,PathFlags* pFlags)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/01/95
	Inputs:		pRect  = a pointer to a rectangle to fit the ellipse in
				pPath  = a pointer to an empty path (should be initialised but empty)
				pFlags = flags to use for each path control point
	Outputs:	
	Purpose:	Create an elliptically shaped closed path which is the same size as and
				has the same position as the rectangle specified.

********************************************************************************************/

BOOL EnvelopeShapes::MakeEllipse(DocRect* pRect, Path* pPath, PathFlags* pFlags)
{
	ERROR2IF(pRect==NULL,FALSE,"NULL rectangle pointer passed to EnvelopeShapes::MakeEllipse()");
	ERROR2IF(pPath==NULL,FALSE,"NULL path pointer passed to EnvelopeShapes::MakeEllipse()");

	DocCoord p0,p1,p2;
	INT32 dx,dy;

	dy = pRect->hi.y - pRect->lo.y;
	dx = pRect->hi.x - pRect->lo.x;

	double ratio = 1.0/4.4672;

	p0.x = pRect->lo.x;
	p0.y = pRect->lo.y + (INT32)(dy/2);

	BOOL ok = (pPath->AddMoveTo(p0,pFlags));
	if (!ok) return FALSE;

	p0.x = 0;
	p0.y = pRect->hi.y - (INT32)(dy*ratio);
	p1.x = pRect->lo.x + (INT32)(dx*ratio);
	p1.y = pRect->hi.y;
	p2.x = pRect->lo.x + (INT32)(dx/2);
	p2.y = pRect->hi.y;

	ok = (pPath->AddCurveTo(p0,p1,p2,pFlags));
	if (!ok) return FALSE;

	p0.x = pRect->hi.x - (INT32)(dx*ratio);
	p0.y = pRect->hi.y;
	p1.x = pRect->hi.x;
	p1.y = pRect->hi.y - (INT32)(dy*ratio);
	p2.x = pRect->hi.x;
	p2.y = pRect->lo.y + (INT32)(dy/2);

	ok = (pPath->AddCurveTo(p0,p1,p2,pFlags));
	if (!ok) return FALSE;

	p0.x = pRect->hi.x;
	p0.y = pRect->lo.y + (INT32)(dy*ratio);
	p1.x = pRect->hi.x - (INT32)(dx*ratio);
	p1.y = pRect->lo.y;
	p2.x = pRect->lo.x + (INT32)(dx/2);
	p2.y = pRect->lo.y;

	ok = (pPath->AddCurveTo(p0,p1,p2,pFlags));
	if (!ok) return FALSE;

	p0.x = pRect->lo.x + (INT32)(dx*ratio);
	p0.y = pRect->lo.y;
	p1.x = pRect->lo.x;
	p1.y = pRect->lo.y + (INT32)(dy*ratio);
	p2.x = pRect->lo.x;	
	p2.y = pRect->lo.y + (INT32)(dy/2);

	ok = (pPath->AddCurveTo(p0,p1,p2,pFlags));
	if (!ok) return FALSE;

	ok = (pPath->CloseSubPath());

	return ok;
}


