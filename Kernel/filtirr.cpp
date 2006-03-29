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
// ImagemapRenderRegion

#include "camtypes.h"
#include "filtirr.h"

#include "view.h"
#include "spread.h"
#include "attrmgr.h"

#include "imagemap.h" //For Imagemap
#include "webattr.h"  //For WebAddressAttribute


CC_IMPLEMENT_DYNAMIC(ImagemapRenderRegion, RenderRegion)

//These MIN and MAX macros are used below
#define MIN(a, b)		((a) < (b) ? (a) : (b))
#define MAX(a, b)		((a) < (b) ? (b) : (a))

/********************************************************************************************

>	ImagemapRenderRegion::ImagemapRenderRegion() : RenderRegion()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Purpose:	Default constructor
	
********************************************************************************************/

ImagemapRenderRegion::ImagemapRenderRegion() : RenderRegion()
{
	m_Options=ImagemapFilterOptions();
}

/********************************************************************************************

  >	ImagemapRenderRegion::ImagemapRenderRegion(ImagemapFilterOptions ifoOptions)
									

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Purpose:	Specific constructor, using a set of ImagemapFilterOptions.

				  Note how this contructor works. It calls the RenderRegion constructor:

				  RenderRegion(ClipRect, ConvertMatrix, ViewScale)

				  with ClipRect derived from the ImagemapFilterOptions, ConvertMatrix
				  set to the identity and ViewScale set to 1.

	
********************************************************************************************/

ImagemapRenderRegion::ImagemapRenderRegion(ImagemapFilterOptions ifoOptions) :
RenderRegion(ImagemapFilterOptions::GetSizeOfExportArea(ifoOptions.m_stExportArea), Matrix(), 1)
{
	m_Options=ifoOptions;
}

/********************************************************************************************

  >	ImagemapRenderRegion::~ImagemapRenderRegion()
								

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Purpose:	Destructor

	
********************************************************************************************/

ImagemapRenderRegion::~ImagemapRenderRegion()
{
	m_Imagemap.DeleteAll();
}

/********************************************************************************************

>	void ImagemapRenderRegion::DrawDragRect(DocRect *RectToRender)//

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Purpose:	Should not be called for this kind of render region.
	Errors:		ENSURE failure if called.
	SeeAlso:	RenderRegion::DrawDragRect

********************************************************************************************/

void ImagemapRenderRegion::DrawDragRect(DocRect *RectToRender)
{
	ENSURE(FALSE, "DrawDragRect called while exporting!");
}

/********************************************************************************************

>	void ImagemapRenderRegion::DrawPixel(const DocCoord &Point)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Purpose:	Should not be called for this kind of render region.
	Errors:		ENSURE failure if called.
	SeeAlso:	RenderRegion::DrawPixel

********************************************************************************************/

void ImagemapRenderRegion::DrawPixel(const DocCoord &Point)
{
	ENSURE(FALSE, "DrawPixel called while exporting!");
}

/********************************************************************************************

>	void ImagemapRenderRegion::DrawBlob(DocCoord p, BlobType type)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Purpose:	Should not be called for this kind of render region.
	Errors:		ENSURE failure if called.
	SeeAlso:	RenderRegion::DrawBlob

********************************************************************************************/

void ImagemapRenderRegion::DrawBlob(DocCoord p, BlobType type)
{
	ENSURE(FALSE, "DrawBlob called while exporting!");
}

/********************************************************************************************

>	void ImagemapRenderRegion::DrawCross(const DocCoord &Point, const UINT32 Size)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Purpose:	Should not be called for this kind of render region.
	Errors:		ENSURE failure if called.
	SeeAlso:	RenderRegion::DrawCross

********************************************************************************************/

void ImagemapRenderRegion::DrawCross(const DocCoord &Point, const UINT32 Size)
{
	ENSURE(FALSE, "DrawCross called while exporting!");
}


/********************************************************************************************

>	void ImagemapRenderRegion::DrawPathToOutputDevice(Path *ppthRender, PathShape shapePath=PATHSHAPE_PATH)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Inputs:		ppthRender - Pointer to the path to render
				shapePath  - Tag to tell us what shape the path is. The
								values we are interested in are:

								PATHSHAPE_RECTANGLE (and PATHSHAPE_SQUARE)
								PATHSHAPE_CIRCLE
	
	Purpose:	Renders this path into the ImagemapRenderRegion

				It does this by adding a clickable area into the imagemap list.

	SeeAlso:	RenderRegion::DrawPath

********************************************************************************************/

void ImagemapRenderRegion::DrawPathToOutputDevice(Path *ppthRender, PathShape shapePath)
{
	//First check our path parameter
 	if (ppthRender==NULL)
	{
		ERROR2RAW("ImagemapRenderRegion::DrawPathToOutputDevice not given a path!");
		return;
	}

	//Now get the current WebAddressAttribute
	WebAddressAttribute* pwaaCurrent=(WebAddressAttribute*) GetCurrentAttribute(ATTR_WEBADDRESS);

	if (pwaaCurrent==NULL)
	{
		ERROR2RAW("ImagemapRenderRegion::DrawPathToOutputDevice - no current WebAddressAttribute!");
		return;
	}

	
	//IF the current WebAddressAttribute has a URL
	if (pwaaCurrent->HasURL())
	{
		//Then we want to add a clickable area into the imagemap

		//Now, if EITHER the user has specified that this clickable area is a rectangle
		//OR the user has specified all clickable areas should be rectangles
		//OR this path is a rectangle shape
		if (m_Options.m_fAllRectangles 
			|| pwaaCurrent->UseBoundingRectangle()
			|| shapePath==PATHSHAPE_RECTANGLE
			|| shapePath==PATHSHAPE_SQUARE)
		{
			//Then we want to add a clickable area into the imagemap
			
			//So get the rectangle to add
			DocRect rectToAdd=pwaaCurrent->GetClickableRectangleInRendering();

			//And add it
			AddRectangleToImagemap(rectToAdd, pwaaCurrent);

		}
		else
		{
			//It's not a rectangle

			//So, what shape is the path?
			if (shapePath==PATHSHAPE_CIRCLE)
				//Circular. So add a circle to the imagemap
				AddCircleToImagemap(ppthRender, pwaaCurrent);
			else
				//Otherwise, add a polygon to the imagemap
				AddPathToImagemap(ppthRender, pwaaCurrent);

		}//End if/else rectangle

	} //ENd if/else has URL

}


	
/********************************************************************************************

>	void ImagemapRenderRegion::DrawRect(DocRect *RectToRender)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	/08/96
	Input:		RectToRender - The rectangle to render
	Purpose:	Does nothing


********************************************************************************************/

void ImagemapRenderRegion::DrawRect(DocRect *RectToRender)
{
	ENSURE(FALSE, "ImagemapRenderRegion::DrawRect called - this should not happen!");
}

/********************************************************************************************

>	void ImagemapRenderRegion::DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Purpose:	None at present - this function should not be called during ink rendering.
	Errors:		ENSURE failure if called.

********************************************************************************************/

void ImagemapRenderRegion::DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint)
{
	ENSURE(FALSE, "ImagemapRenderRegion::DrawLine called - this should not happen!");
}

/********************************************************************************************

>	MILLIPOINT ImagemapRenderRegion::CalcPixelWidth()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Returns:	Width of pixels on millipoints.
	Purpose:	None for Imagemap render regions - just returns 1.
	Errors:		-
	SeeAlso:	ImagemapRenderRegion::CalcScaledPixelWidth

********************************************************************************************/

MILLIPOINT ImagemapRenderRegion::CalcPixelWidth()
{
	//Return an invalid value
	return 1;
}

/********************************************************************************************

>	MILLIPOINT ImagemapRenderRegion::CalcScaledPixelWidth()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Returns:	Scaled width of pixels on millipoints.
	Purpose:	None for CMX render regions - just returns 1.
	Errors:		ENSURE failure if called.
	SeeAlso:	ImagemapRenderRegion::CalcPixelWidth

********************************************************************************************/

MILLIPOINT ImagemapRenderRegion::CalcScaledPixelWidth()
{
	//Return an invalid value
	return 1;
}

/********************************************************************************************

>	virtual void ImagemapRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Outputs:	pCaps - The details about what types of thing this render region can render
	Purpose:	This function allows render regions to admit to what they can and can not
				render. This allows other areas of the program to come in and help render
				regions out in some situations, if they are unable to render everything.
				eg. an OSRenderRegion can not render transparancy.

********************************************************************************************/

void ImagemapRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)
{
	//This seems to be what all the other overrides do, so I guess
	//I'll do the same
	pCaps->CanDoNothing();

}




/********************************************************************************************

>	virtual BOOL ImagemapRenderRegion::WantsGrids()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Returns: 	FALSE
	Purpose:	very little

********************************************************************************************/

BOOL ImagemapRenderRegion::WantsGrids()
{
	return FALSE;
}

/********************************************************************************************

 >	INT32 ImagemapRenderRegion::Write(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL)
										   
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		pfileToWrite - The file to write to
				pcBuffer	 - Pointer to a text buffer to write to

				Either of the above may be NULL.

  Returns:		The number of TCHARs written.

  Purpose:		Writes the imagemap HTML, either to the file
				or the text buffer or both:
					
				<MAP>
					<AREA SHAPE=RECTANGLE...>
					<AREA SHAPE=RECTANGLE...>
				</MAP>

	
********************************************************************************************/

INT32 ImagemapRenderRegion::Write(CCLexFile* pfileToWrite, TCHAR* pcBuffer)
{
	return m_Imagemap.Write(pfileToWrite, pcBuffer);
		
}

/********************************************************************************************

>	void ImagemapRenderRegion::AddRectangleToImagemap(DocRect rectToScale, WebAddressAttribute* pwaaCurrent)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/97
	Inputs:		rectToAdd	The rectangle to add to the imagemap
				pwaaToAdd	Pointer to the current WebAddressAttribute
	
	Purpose:	Scales the rectangle then adds it to the imagemap list.

	SeeAlso:	ImagemapRenderRegion::DrawPathToOutputDevice

********************************************************************************************/

void ImagemapRenderRegion::AddRectangleToImagemap(DocRect rectToScale, WebAddressAttribute* pwaaCurrent)
{
	//First get the origin of the export area and the DPI
	DocCoord dcOrigin=ImagemapFilterOptions::GetOriginOfExportArea(m_Options.m_stExportArea);
	double dDPI=m_Options.m_dDPI;

	//Scale the rectangle
	rectToScale.lo.Scale(dcOrigin, dDPI);
	rectToScale.hi.Scale(dcOrigin, dDPI);

	//Now, in scaling the rectangle we may have made it invalid (for example, we
	//may have scaled rectToScale.lo so that it is actually higher than recToScale.hi).

	//So create a new valid rectangle
	DocRect rectToAdd;

	rectToAdd.lo.x=MIN(rectToScale.lo.x, rectToScale.hi.x);
	rectToAdd.hi.x=MAX(rectToScale.lo.x, rectToScale.hi.x);
	rectToAdd.lo.y=MIN(rectToScale.lo.y, rectToScale.hi.y);
	rectToAdd.hi.y=MAX(rectToScale.lo.y, rectToScale.hi.y);						

	//And add it to the imagemap
	m_Imagemap.AddRectangle(rectToAdd, pwaaCurrent->m_url.GetWebAddress(), pwaaCurrent->m_pcFrame);
	
}

/********************************************************************************************

>	void ImagemapRenderRegion::AddCircleToImagemap(Path* ppthToScale, WebAddressAttribute* pwaaCurrent)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/97
	Inputs:		ppthToAdd	The circle to add to the imagemap
				pwaaToAdd	Pointer to the current WebAddressAttribute
	
	Purpose:	Scales the path then adds it to the imagemap list as a circle

	SeeAlso:	ImagemapRenderRegion::DrawPathToOutputDevice

********************************************************************************************/

void ImagemapRenderRegion::AddCircleToImagemap(Path* ppthToScale, WebAddressAttribute* pwaaCurrent)
{
	//First get the origin of the export area and the DPI
	DocCoord dcOrigin=ImagemapFilterOptions::GetOriginOfExportArea(m_Options.m_stExportArea);
	double dDPI=m_Options.m_dDPI;

	//Now make a copy of the path
	Path pthToAdd;
	
	pthToAdd.Initialise(ppthToScale->GetNumCoords());

	pthToAdd.CopyPathDataFrom(ppthToScale);

	//Scale it
	pthToAdd.Scale(dcOrigin, dDPI);

	//And add it to the imagemap
	m_Imagemap.AddCircle(&pthToAdd, pwaaCurrent->m_url.GetWebAddress(), pwaaCurrent->m_pcFrame);
	
}

/********************************************************************************************

>	void ImagemapRenderRegion::AddPathToImagemap(Path* ppthToAdd, WebAddressAttribute* pwaaCurrent)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/97
	Inputs:		ppthToAdd	The path to add to the imagemap
				pwaaToAdd	Pointer to the current WebAddressAttribute
	
	  Purpose:	This function goes through the subpaths of the path one by one.
			
				For each closed subpath, it:
				a. Scales the subpath
				b. Flattens the subpath
				c. Adds the result to the imagemap

	SeeAlso:	ImagemapRenderRegion::DrawPathToOutputDevice

********************************************************************************************/

void ImagemapRenderRegion::AddPathToImagemap(Path* ppthToAdd, WebAddressAttribute* pwaaCurrent)
{
	//First get the origin of the export area and the DPI
	DocCoord dcOrigin=ImagemapFilterOptions::GetOriginOfExportArea(m_Options.m_stExportArea);
	double dDPI=m_Options.m_dDPI;

	//Now, how many subpaths are there in this path?
	INT32 lNumSubpaths=ppthToAdd->GetNumSubpaths();

	//For each subpath in the path
	for (INT32 l=0; l<lNumSubpaths; l++)
	{
		//Create a new path
		Path pthSubpath;

		pthSubpath.Initialise(ppthToAdd->GetNumCoords());

		//And copy the next subpath into it
		ppthToAdd->MakePathFromSubPath(l, &pthSubpath);

		//Now, if that subpath is closed
		if (pthSubpath.IsSubPathClosed(0))
		{
			//Then we want to add it to the imagemap

			//So scale it to dDPI and by dcOrigin
			pthSubpath.Scale(dcOrigin, dDPI);
	
			//Now we need to flatten it.
			//This means creating a new path, because otherwise Path::Flatten
			//goes wrong

			Path pthFlattened;

			pthFlattened.Initialise(pthSubpath.GetNumCoords());

			//So, if we should flatten the path
			if (m_Options.m_ffApprox!=FF_NOTATALL)
				//Then do it
				pthSubpath.Flatten(m_Options.m_ffApprox, &pthFlattened);
			else
				//Otherwise, simply copy the path across
				pthFlattened.CopyPathDataFrom(&pthSubpath);

			//Then add the flattened path to the imagemap
			m_Imagemap.AddPolygon(&pthFlattened, pwaaCurrent->m_url.GetWebAddress(), pwaaCurrent->m_pcFrame);
		}
	}
			
	
}







