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
// Mould Geometry class implementation. 
// The mould geometry class defines the structure of derived classes such as MouldEnvelope
// and MouldPerspective

#include "camtypes.h"
#include "moldenv.h"
#include "errors.h"
#include "paths.h"
#include "rndrgn.h"
#include "trans2d.h"
#include "tranform.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(MouldGeometry, CCObject)
CC_IMPLEMENT_DYNAMIC(MouldTransform, TransformBase)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/*******************************************************************************************

	MouldGeometry::MouldGeometry()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/02/94
	Inputs:		
	Purpose:	Construct a mould geometry

********************************************************************************************/

MouldGeometry::MouldGeometry()
{
	MouldThreshold = 32;
}


/*******************************************************************************************

	MouldGeometry::SetThreshold(const INT32 t)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/02/94
	Inputs:		
	Purpose:	Set the threshold value for this mould object. Thresholds should be in the
				range 1..4095 inclusive.

********************************************************************************************/

void MouldGeometry::SetThreshold(const INT32 t)
{
	if (t>0 && t<4096)
		MouldThreshold=t;
}

/*******************************************************************************************

>	virtual void MouldGeometry::MouldPathRender(Path* pPath, RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pPath	= pointer to a path to mould
				pRegion	= pointer to a region to render into
	Purpose:	This function renders a path directly. It uses the current setup manifold
				and generates a temporary render path.

********************************************************************************************/

void MouldGeometry::MouldPathRender(Path* pPath, RenderRegion* pRegion)
{
	Path RenderPath;
	if (!(RenderPath.Initialise(12,12))) return;
	if (!MouldPathToPath(pPath,&RenderPath)) return;

	pRegion->DrawPath(&RenderPath);
}



/*******************************************************************************************

>	virtual void MouldGeometry::MouldBitmapRender(  KernelBitmap* pBlit,
													DocCoord*pParallel,
													RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/93
	Inputs:		pBlit		= pointer to a bitmap
				pParallel	= pointer to a parallelogram
				pRegion		= pointer to a region to render into
	Purpose:

********************************************************************************************/

void MouldGeometry::MouldBitmapRender(	KernelBitmap* pBlit,
										DocCoord* pParallel,
										RenderRegion* pRegion)
{
}



/********************************************************************************************

>	virtual MouldGeometry* MouldPathShape::MakeCopy()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/93
	Inputs:		-
	Outputs:	a pointer to a new mould geometry object.
				NULL if unable to create the object.
	Purpose:	Make a copy of this mould geometry object and return it.

********************************************************************************************/

MouldGeometry* MouldGeometry::MakeCopy()
{
	// create a new Geometry
	MouldGeometry* pGeometry = new MouldGeometry;
	if (pGeometry == NULL)
		return NULL;

	BOOL ok = CopyContents(pGeometry);
	if (!ok)
	{
		delete pGeometry;
		return NULL;
	}

	return (pGeometry);
}


/********************************************************************************************

>	BOOL MouldGeometry::CopyContents(MouldGeometry* pGeometry)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/93
	Inputs:		pGeometry = a pointer to copy of this geometry data.
	Outputs:	
	Returns:	TRUE if the data has been copied correctly
				FALSE if failed
	Purpose:	Make a copy of this mouldgeometry classes private data

********************************************************************************************/

BOOL MouldGeometry::CopyContents(MouldGeometry* pGeometry)
{
	ERROR3IF(pGeometry==NULL, "MouldGeometry::CopyContents() passed a null pointer");
		
	// copy all our locals here
	pGeometry->MouldThreshold = MouldThreshold;

	return TRUE;
}





/*******************************************************************************************

>	virtual BOOL MouldGeometry::OnClick(DocCoord Coord, 
										ClickType CType,
										ClickModifiers Mods,
										Spread* pSpread,
										NodeMould* pNodeMould)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/93
	Inputs:		Coord		= coordinate of mouse
				CType		= the type of click (single, double, drag)
				Mods		= the click modifiers
				pSpread		= a pointer to the spread the click occured over
				pNodeMould	= a pointer to the parent mould object
	Purpose:

********************************************************************************************/

BOOL MouldGeometry::OnClick(DocCoord Coord, 
							ClickType CType,
							ClickModifiers Mods,
							Spread* pSpread,
							NodeMould* pNodeMould)
{
	// do nothing at the moment.
	return FALSE;
}



/*******************************************************************************************

>	virtual BOOL MouldGeometry::OnMouseMove(DocCoord Coord, 
											Spread* pSpread,
											ClickModifiers Mods
											INT32* ctype,
											INT32* msgres)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/93
	Inputs:		Coord	= coordinate of mouse
				Mods	= the click modifiers
				pSpread	= a pointer to the spread the click occured over
	Returns:	ctype 	= the tool cursor type to use over this point
				msgres	= the message resource type to use over this point
	Purpose:

********************************************************************************************/

BOOL MouldGeometry::OnMouseMove(DocCoord Coord, 
								Spread* pSpread,
								ClickModifiers Mods,
								INT32* ctype,
								INT32* msgres)
{
	// do nothing at the moment.
	return FALSE;
}


DocRect MouldGeometry::ConvRectToDocRect(RECT& rect0)
{
	DocRect rect1;
	rect1.lo.x = rect0.left;
	rect1.lo.y = rect0.bottom;
	rect1.hi.x = rect0.right;
	rect1.hi.y = rect0.top;
	return rect1;
}


RECT MouldGeometry::ConvDocRectToRect(DocRect& rect0)
{
	RECT rect1;
	rect1.left 	= rect0.lo.x;
	rect1.bottom= rect0.lo.y;
	rect1.right	= rect0.hi.x;
	rect1.top 	= rect0.hi.y;
	return rect1;
}




/*******************************************************************************************

>	MouldTransform::MouldTransform(MouldShape* pShape) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/94
	Inputs:		pShape = a pointer to a mould shape object
	Purpose:	MouldTransform constructor sets the	internal shape pointer.

********************************************************************************************/

MouldTransform::MouldTransform(MouldGeometry* pGeom)
{
	ERROR3IF(pGeom==NULL, "MouldTransform constructor passed a null shape");

	pMouldGeom = pGeom;
	TransFills = TRUE;
	TransLines = TRUE;

//	FIXED16 a(1);
//	SetScaleFactor(a);
}


/*******************************************************************************************

>	MouldTransform::MouldTransform() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/94
	Inputs:		
	Purpose:	MouldTransform destructor

********************************************************************************************/

MouldTransform::~MouldTransform()
{
	// does nothing at moment
}


/*******************************************************************************************

>	MouldTransform::Transform(DocCoord* Coords, INT32 NumCoords) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/94
	Inputs:		Coords 		= a pointer to a set of document coordinates
				NumCoords	= the number of coordinates to transform.
	Outputs:	Coords holds the results of the transformation. All coordinates will have
				been processed.
	Purpose:	Takes each coordinate from Coords in turn, transforming it by the defined
				mould and writing back the result.

********************************************************************************************/

void MouldTransform::Transform(DocCoord* Coords, INT32 NumCoords)
{
	ERROR3IF(Coords==NULL, "MouldTransform::Transform() called with illegal coord pointer");

	for (INT32 i=0; i<NumCoords; i++)
		pMouldGeom->MouldPoint(Coords[i],Coords[i]);
}


/*******************************************************************************************

>	MouldTransform::TransformPath(Path* const pSource, Path* pDestin) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/01/94
	Inputs:		pSource	= a pointer to the source path to transform
				pDestin = a pointer to an initialised path to take the result of the
						  transformation.
	Outputs		pDestin holds the result of the transformation
	Purpose:	This function will create a destination path by transforming the coords in
				the source path. However, unlike MouldTransform::Transform() there is not
				a 1:1 correspondence between these points. ie this complex transform produces
				complex results. The output path can be much larger than the input.
				You could theoretically call the Transform function on the coordinates of the
				path. The nature of the transform (being none affine) will give you an 
				inaccurate result for lines and an invalid result for curves.
				So its better to use this function for paths.
				I suppose the criteria is, if you dont mind a linear response in the output
				domain of what you are transforming, (such as simple transforms of grad
				fills) then you can use Transform() otherwise, use this function if you
				can represent your data as paths.

********************************************************************************************/

BOOL MouldTransform::TransformPath(Path* const pSource, Path* pDestin)
{
	ERROR2IF(pSource==NULL,FALSE,"MouldTransform::TransformPath() called with a null source path");
	ERROR2IF(pDestin==NULL,FALSE,"MouldTransform::TransformPath() called with a null source path");

	return pMouldGeom->MouldPathToPath(pSource,pDestin);
}

