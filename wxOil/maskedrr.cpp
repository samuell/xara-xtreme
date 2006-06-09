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
// The Masked Render region

/*
*/

#include "camtypes.h"
#include "maskedrr.h"
//#include "doccolor.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodebmp.h"
//#include "view.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNAMIC(MaskedRenderRegion, GRenderBitmap)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW


// This table holds the number of bits set for each of the byte values 0 to 255. This table is
// used to help speed up the counting of the number of set pixels in the mask
INT32 MaskedRenderRegion::BitCountTable[256] =
{
	0,  /* 0x0 */	1,  /* 0x1 */	1,  /* 0x2 */	2,  /* 0x3 */	1,  /* 0x4 */
	2,  /* 0x5 */	2,  /* 0x6 */	3,  /* 0x7 */	1,  /* 0x8 */	2,  /* 0x9 */
	2,  /* 0xA */	3,  /* 0xB */	2,  /* 0xC */	3,  /* 0xD */	3,	/* 0xE */
	4,  /* 0xF */	1,  /* 0x10 */	2,  /* 0x11 */	2,  /* 0x12 */	3,  /* 0x13 */
	2,  /* 0x14 */	3,  /* 0x15 */	3,  /* 0x16 */	4,  /* 0x17 */	2,  /* 0x18 */
	3,  /* 0x19 */	3,  /* 0x1A */	4,  /* 0x1B */	3,  /* 0x1C */	4,  /* 0x1D */
	4,  /* 0x1E */	5,  /* 0x1F */	1,  /* 0x20 */	2,  /* 0x21 */	2,  /* 0x22 */
	3,  /* 0x23 */	2,  /* 0x24 */	3,  /* 0x25 */	3,  /* 0x26 */	4,  /* 0x27 */
	2,  /* 0x28 */	3,  /* 0x29 */	3,  /* 0x2A */	4,  /* 0x2B */	3,  /* 0x2C */
	4,  /* 0x2D */	4,  /* 0x2E */	5,  /* 0x2F */	2,  /* 0x30 */	3,  /* 0x31 */
	3,  /* 0x32 */	4,  /* 0x33 */	3,  /* 0x34 */	4,  /* 0x35 */	4,  /* 0x36 */
	5,  /* 0x37 */	3,  /* 0x38 */	4,  /* 0x39 */	4,  /* 0x3A */	5,  /* 0x3B */
	4,  /* 0x3C */	5,  /* 0x3D */	5,  /* 0x3E */	6,  /* 0x3F */	1,  /* 0x40 */
	2,  /* 0x41 */	2,  /* 0x42 */	3,  /* 0x43 */	2,  /* 0x44 */	3,  /* 0x45 */
	3,  /* 0x46 */	4,  /* 0x47 */	2,  /* 0x48 */	3,  /* 0x49 */	3,  /* 0x4A */
	4,  /* 0x4B */	3,  /* 0x4C */	4,  /* 0x4D */	4,  /* 0x4E */	5,  /* 0x4F */
	2,  /* 0x50 */	3,  /* 0x51 */	3,  /* 0x52 */	4,  /* 0x53 */	3,  /* 0x54 */
	4,  /* 0x55 */	4,  /* 0x56 */	5,  /* 0x57 */	3,  /* 0x58 */	4,  /* 0x59 */
	4,  /* 0x5A */	5,  /* 0x5B */	4,  /* 0x5C */	5,  /* 0x5D */	5,  /* 0x5E */
	6,  /* 0x5F */	2,  /* 0x60 */	3,  /* 0x61 */	3,  /* 0x62 */	4,  /* 0x63 */
	3,  /* 0x64 */	4,  /* 0x65 */	4,  /* 0x66 */	5,  /* 0x67 */	3,  /* 0x68 */
	4,  /* 0x69 */	4,  /* 0x6A */	5,  /* 0x6B */	4,  /* 0x6C */	5,  /* 0x6D */
	5,  /* 0x6E */	6,  /* 0x6F */	3,  /* 0x70 */	4,  /* 0x71 */	4,  /* 0x72 */
	5,  /* 0x73 */	4,  /* 0x74 */	5,  /* 0x75 */	5,  /* 0x76 */	6,  /* 0x77 */
	4,  /* 0x78 */	5,  /* 0x79 */	5,  /* 0x7A */	6,  /* 0x7B */	5,  /* 0x7C */
	6,  /* 0x7D */	6,  /* 0x7E */	7,  /* 0x7F */	1,  /* 0x80 */	2,  /* 0x81 */
	2,  /* 0x82 */	3,  /* 0x83 */	2,  /* 0x84 */	3,  /* 0x85 */	3,  /* 0x86 */
	4,  /* 0x87 */	2,  /* 0x88 */	3,  /* 0x89 */	3,  /* 0x8A */	4,  /* 0x8B */
	3,  /* 0x8C */	4,  /* 0x8D */	4,  /* 0x8E */	5,  /* 0x8F */	2,  /* 0x90 */
	3,  /* 0x91 */	3,  /* 0x92 */	4,  /* 0x93 */	3,  /* 0x94 */	4,  /* 0x95 */
	4,  /* 0x96 */	5,  /* 0x97 */	3,  /* 0x98 */	4,  /* 0x99 */	4,  /* 0x9A */
	5,  /* 0x9B */	4,  /* 0x9C */	5,  /* 0x9D */	5,  /* 0x9E */	6,  /* 0x9F */
	2,  /* 0xA0 */	3,  /* 0xA1 */	3,  /* 0xA2 */	4,  /* 0xA3 */	3,  /* 0xA4 */
	4,  /* 0xA5 */	4,  /* 0xA6 */	5,  /* 0xA7 */	3,  /* 0xA8 */	4,  /* 0xA9 */
	4,  /* 0xAA */	5,  /* 0xAB */	4,  /* 0xAC */	5,  /* 0xAD */	5,  /* 0xAE */
	6,  /* 0xAF */	3,  /* 0xB0 */	4,  /* 0xB1 */	4,  /* 0xB2 */	5,  /* 0xB3 */
	4,  /* 0xB4 */	5,  /* 0xB5 */	5,  /* 0xB6 */	6,  /* 0xB7 */	4,  /* 0xB8 */
	5,  /* 0xB9 */	5,  /* 0xBA */	6,  /* 0xBB */	5,  /* 0xBC */	6,  /* 0xBD */
	6,  /* 0xBE */	7,  /* 0xBF */	2,  /* 0xC0 */	3,  /* 0xC1 */	3,  /* 0xC2 */
	4,  /* 0xC3 */	3,  /* 0xC4 */	4,  /* 0xC5 */	4,  /* 0xC6 */	5,  /* 0xC7 */
	3,  /* 0xC8 */	4,  /* 0xC9 */	4,  /* 0xCA */	5,  /* 0xCB */	4,  /* 0xCC */
	5,  /* 0xCD */	5,  /* 0xCE */	6,  /* 0xCF */	3,  /* 0xD0 */	4,  /* 0xD1 */
	4,  /* 0xD2 */	5,  /* 0xD3 */	4,  /* 0xD4 */	5,  /* 0xD5 */	5,  /* 0xD6 */
	6,  /* 0xD7 */	4,  /* 0xD8 */	5,  /* 0xD9 */	5,  /* 0xDA */	6,  /* 0xDB */
	5,  /* 0xDC */	6,  /* 0xDD */	6,  /* 0xDE */	7,  /* 0xDF */	3,  /* 0xE0 */
	4,  /* 0xE1 */	4,  /* 0xE2 */	5,  /* 0xE3 */	4,  /* 0xE4 */	5,  /* 0xE5 */
	5,  /* 0xE6 */	6,  /* 0xE7 */	4,  /* 0xE8 */	5,  /* 0xE9 */	5,  /* 0xEA */
	6,  /* 0xEB */	5,  /* 0xEC */	6,  /* 0xED */	6,  /* 0xEE */	7,  /* 0xEF */
	4,  /* 0xF0 */	5,  /* 0xF1 */	5,  /* 0xF2 */	6,  /* 0xF3 */	5,  /* 0xF4 */
	6,  /* 0xF5 */	6,  /* 0xF6 */	7,  /* 0xF7 */	5,  /* 0xF8 */	6,  /* 0xF9 */
	6,  /* 0xFA */	7,  /* 0xFB */	6,  /* 0xFC */	7,  /* 0xFD */	7,  /* 0xFE */
	8	/* 0xFF */
};




/********************************************************************************************

>	MaskedRenderRegion::MaskedRenderRegion(	DocRect ClipRegion, Matrix ConvertMatrix,
										FIXED16 ViewScale, UINT32 Depth, double dpi)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		ClipRect - the Clipping rectangle
				ConvertMatrix - the Matrix to render through
				ViewScale - The Scale Factor
				dpi - Dots per inch
	Purpose:	Constructor. Sends everything on to the base class.
				INT32 dpi changed to FIXED16 dpi (12/12/95) to improve the range of values allowed
				at the < 1000dpi settings that we will be using.

********************************************************************************************/

MaskedRenderRegion::MaskedRenderRegion(	DocRect ClipRegion, Matrix ConvertMatrix,
										FIXED16 ViewScale, double dpi)
										: GRenderBitmap(ClipRegion, ConvertMatrix, ViewScale, 1, dpi)
{
	// Some attrs we use. NULL by default
	pFillAttr = NULL;
	pStrokeAttr = NULL;
	Black = DocColour(0,0,0);
	White = DocColour(255,255,255);
}


/********************************************************************************************

>	MaskedRenderRegion::~MaskedRenderRegion()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Purpose:	Default destructor

********************************************************************************************/

MaskedRenderRegion::~MaskedRenderRegion()
{
	// Kill the fill attr
	if (pFillAttr!=NULL)
	{
		delete pFillAttr;
		pFillAttr = NULL;
	}

	// kill the stroke attr
	if (pStrokeAttr!=NULL)
	{
		delete pStrokeAttr;
		pStrokeAttr = NULL;
	}
}


/********************************************************************************************

>	BOOL MaskedRenderRegion::StartRender()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Starts the render region up. This version just calls the base class and
				allocates a couple of attributes used during the rendering.

********************************************************************************************/

BOOL MaskedRenderRegion::StartRender()
{
	// Call the base class
	if (!GRenderBitmap::StartRender())
		return FALSE;

	// If we got a bitmap, then clear it
	if (pBits!=NULL && pBitmapInfo!=NULL)
	{
		// Find out how many bytes there are to look at
		INT32 TotalBytes = pBitmapInfo->bmiHeader.biSizeImage;
		
		// clear it all out - a Set pixels is transparent, an Unset pixel is not
		memset(pBits, 0xFF, TotalBytes);
	}
		
	// Do our own thing - We need some attrs
	if (pFillAttr==NULL)
		pFillAttr = new FlatTranspFillAttribute(0);

	if (pStrokeAttr==NULL)
		pStrokeAttr = new StrokeTranspAttribute(0);

	// See if we got the ram we wanted
	if ((pFillAttr==NULL) || (pStrokeAttr==NULL))
		return FALSE;

	// if we get here, everything has worked
	return TRUE;
}



/********************************************************************************************

>	void MaskedRenderRegion::SetHostCaps(RenderCaps* pCaps)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		pCaps - pointer to the render caps for the host render region
	Purpose:	Tells the mask region what attributes are considered complex.

********************************************************************************************/

void MaskedRenderRegion::SetHostCaps(RRCaps* pCaps)
{
	// Make a copy of the host regions caps
	HostCaps = *pCaps;
}



/********************************************************************************************

>	virtual void MaskedRenderRegion::DrawPathToOutputDevice(Path* pPath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		pPath - The path that needs rendering
	Purpose:	Renders the path into the mask. It makes sure that all the attributes are
				set up correctly first though

********************************************************************************************/

void MaskedRenderRegion::DrawPathToOutputDevice(Path* pPath, PathShape)
{
	// Save the context
	SaveContext();

	BOOL RenderFill = FALSE;

	// Get the current fill attr
	FillGeometryAttribute* pFillAttr = (FillGeometryAttribute*) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

	if (pFillAttr->GetBitmap() != NULL && pFillAttr->GetBitmap()->GetBPP() <= 8)
	{
	 	INT32 TranspIndex;

		// If it's a masked bitmap fill, then we'll need to actually render it
		// (but we'll force all the non-masked pixels to be rendered black)
		if (pFillAttr->GetBitmap()->GetTransparencyIndex(&TranspIndex))
			RenderFill = TRUE;
	}

	if (RenderFill)
	{
		BOOL OldVMState = RenderFlags.VeryMono;
		RenderFlags.VeryMono = TRUE;	// Forces non-masked pixels to be plotted black

		// Draw the path, with the bitmap fill
		GRenderBitmap::DrawPathToOutputDevice(pPath);

		RenderFlags.VeryMono = OldVMState;
	}
	else
	{
		// Does this shape make use of complex things
		if (TestForComplexShape(&HostCaps))
			SetNextShapeColour(Black);
		else
			SetNextShapeColour(White);

		// Draw the path
		GRenderBitmap::DrawPathToOutputDevice(pPath);
	}

	// restore the context
	RestoreContext();
}


/********************************************************************************************

>	virtual void MaskedRenderRegion::DrawRect(DocRect* pRectToRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		pRectToRender - The Rectangle that we need to render
	Purpose:	This function will examine the attributes etc to see if this represents a
				simple or compex shape. It will set up the attributes according to the result
				of this and then call the base class version of the function.

********************************************************************************************/

void MaskedRenderRegion::DrawRect(DocRect* pRectToRender)
{
	// Save the context
	SaveContext();

	// Does this shape make use of complex things
	if (TestForComplexShape(&HostCaps))
		SetNextShapeColour(Black);
	else
		SetNextShapeColour(White);

	// Draw the path
	GRenderBitmap::DrawRect(pRectToRender);

	// restore the context
	RestoreContext();
}


/********************************************************************************************

>	virtual void MaskedRenderRegion::DrawLine(const DocCoord &StartPoint,
												const DocCoord &EndPoint)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		StartPoint, EndPoint - the coords to draw a line between
	Purpose:	This function will examine the attributes etc to see if this represents a
				simple or compex shape. It will set up the attributes according to the result
				of this and then call the base class version of the function.

********************************************************************************************/

void MaskedRenderRegion::DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint)
{
	// Save the context
	SaveContext();

	// Does this shape make use of complex things
	if (TestForComplexShape(&HostCaps))
		SetNextShapeColour(Black);
	else
		SetNextShapeColour(White);

	// Draw the path
	GRenderBitmap::DrawLine(StartPoint, EndPoint);

	// restore the context
	RestoreContext();
}


/********************************************************************************************

>	virtual void MaskedRenderRegion::DrawPixel(const DocCoord &Point)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - the coord to draw the pixel in
	Purpose:	This function will examine the attributes etc to see if this represents a
				simple or compex shape. It will set up the attributes according to the result
				of this and then call the base class version of the function.

********************************************************************************************/

void MaskedRenderRegion::DrawPixel(const DocCoord &Point)
{
	// Save the context
	SaveContext();

	// Does this shape make use of complex things
	if (TestForComplexShape(&HostCaps))
		SetNextShapeColour(Black);
	else
		SetNextShapeColour(White);

	// Draw the path
	GRenderBitmap::DrawPixel(Point);

	// restore the context
	RestoreContext();
}


/********************************************************************************************

>	virtual void MaskedRenderRegion::DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - The position to draw the bitmap at
				pBitmap - the bitmap that needs drawing
	Purpose:	This function will examine the attributes etc to see if this represents a
				simple or compex shape. It will set up the attributes according to the result
				of this and then call the base class version of the function.

********************************************************************************************/

void MaskedRenderRegion::DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap)
{
	// Save the context
	SaveContext();

	// Does this shape make use of complex things
	if (TestForComplexShape(&HostCaps))
		SetNextShapeColour(Black);
	else
		SetNextShapeColour(White);

	// Draw the path
	GRenderBitmap::DrawBitmap(Point, pBitmap);

	// restore the context
	RestoreContext();
}


/********************************************************************************************

>	virtual void MaskedRenderRegion::DrawBitmap(const DocCoord &Point, UINT32 BitmapID,
												UINT32 ToolID)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - the position to draw the bitmap at
				BitmapID - The ID of the bitmap to render
				ToolID - the ID of the tool that the bitmap belongs to
	Purpose:	This function will examine the attributes etc to see if this represents a
				simple or compex shape. It will set up the attributes according to the result
				of this and then call the base class version of the function.

********************************************************************************************/

void MaskedRenderRegion::DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID)
{
	// Save the context
	SaveContext();

	// Does this shape make use of complex things
	if (TestForComplexShape(&HostCaps))
		SetNextShapeColour(Black);
	else
		SetNextShapeColour(White);

	// Draw the path
	GRenderBitmap::DrawBitmap(Point, BitmapID, ToolID);

	// restore the context
	RestoreContext();
}

BOOL MaskedRenderRegion::DrawTransformedBitmap(NodeBitmap *pNodeBitmap)
{
	// Save the context
	SaveContext();

	// Work out if we can do this bitmap directly
	BOOL CanDoBitmap = FALSE;

	// Does this shape make use of complex things
	if (!(TestForComplexShape(&HostCaps) || pNodeBitmap->NeedsTransparency()))
	{
		// See what kind of bitmaps this render region can do
		if (HostCaps.ArbitraryBitmaps)
		{
			// We can do anything...
			CanDoBitmap = TRUE;
		}
		else
		{
			if (HostCaps.SimpleBitmaps)
			{
				// Check that the bitmap area is rectangular, and is perpendicular to
				// the x and y axes.
				if (pNodeBitmap->HasSimpleOrientation(this))
				{
					// Yes - simple scaling - the render region can do this.
					CanDoBitmap = TRUE;
				}
			}
		}
	}

	// Here we set up reasonable attributes - we want to fill the bitmap
	// path but not stroke it.  NB. This is very important - do not remove
	// otherwise the mask will be wrong!
	DocColour			colorBlack( COLOUR_BLACK ); 
	SetFillColour( colorBlack );
	DocColour			colorNone( COLOUR_NONE );
	SetLineColour( colorNone );

	if (CanDoBitmap)
	{
		SetNextShapeColour(White);

		// Draw the path (that represents the bitmap object)
		GRenderBitmap::DrawPathToOutputDevice(&pNodeBitmap->InkPath);
	}
	else
	{
		if (pNodeBitmap->NeedsTransparency())
		{
			// It's a masked bitmap, so we'll need to actually render the non-masked
			// pixels as black 
			BOOL OldVMState = RenderFlags.VeryMono;
			RenderFlags.VeryMono = TRUE;	// Forces non-masked pixels to be plotted black

			// Call base class to plot the bitmap
			RenderRegion::DrawTransformedBitmap(pNodeBitmap);

			RenderFlags.VeryMono = OldVMState;
		}
		else
		{
			SetNextShapeColour(Black);

			// Draw the path (that represents the bitmap object)
			GRenderBitmap::DrawPathToOutputDevice(&pNodeBitmap->InkPath);
		}
	}

	// restore the context
	RestoreContext();

	return TRUE;
}

/********************************************************************************************

>	virtual void MaskedRenderRegion::DrawDragRect(DocRect *RectToRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		RectToRender - The Rectangle
	Purpose:	None - Does nothing

********************************************************************************************/

void MaskedRenderRegion::DrawDragRect(DocRect *RectToRender)
{
}


/********************************************************************************************

>	virtual void MaskedRenderRegion::DrawBlob(DocCoord p, BlobType type)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		p - A coord
				type - The type of blob
	Purpose:	None - Does nothing

********************************************************************************************/

void MaskedRenderRegion::DrawBlob(DocCoord p, BlobType type)
{
}


/********************************************************************************************

>	virtual void MaskedRenderRegion::DrawCross(const DocCoord &Point, const UINT32 Size)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - a Coord
				Size - the size of the cross
	Purpose:	None - Does nothing

********************************************************************************************/

void MaskedRenderRegion::DrawCross(const DocCoord &Point, const UINT32 Size)
{
}


/********************************************************************************************

>	virtual void MaskedRenderRegion::DrawBitmapBlob(const DocCoord &Point,
													KernelBitmap* BlobShape)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - A Coord
				BlobShape - the bitmap to use
	Purpose:	None - Does nothing

********************************************************************************************/

void MaskedRenderRegion::DrawBitmapBlob(const DocCoord &Point, KernelBitmap* BlobShape)
{
}


/********************************************************************************************

>	virtual void MaskedRenderRegion::DrawBitmapBlob(const DocCoord &Point, UINT32 BitmapID,
														UINT32 ToolID)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - A coord
				BitmapID - the ID of a bitmap
				ToolID - the Id of the tool the bitmap resource is in
	Purpose:	None - Does nothing

********************************************************************************************/

void MaskedRenderRegion::DrawBitmapBlob(const DocCoord &Point, ResourceID resID )
{
}





/********************************************************************************************

>	void MaskedRenderRegion::SetNextShapeColour(DocColour& RequiredCol)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		RequiredCol - The colour to replace fill and stroke colours with
	Purpose:	Sets all the attributes up so that we can render into our mask. It will
				boil everything down to a flat fill and stroke ready to be rendered into
				the mask. The colour passed in will probably be Black or White, but I left
				it open in case I thought of some other use for this.

********************************************************************************************/

void MaskedRenderRegion::SetNextShapeColour(DocColour& RequiredCol)
{
	// Find out about the fill attr
	ColourFillAttribute* pColFillAttr = (ColourFillAttribute*) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;
	if ((pColFillAttr!=NULL) && (pColFillAttr->IS_KIND_OF(GradFillAttribute)))
	{
		// This is a grad fill, so just set the colour as required
		SetFillColour(RequiredCol);
	}
	else
	{
		// Is there a fill colour of any kind
		DocColour MyFillColour = RR_FILLCOLOUR();

		// See if it is 'no colour'
		if (!MyFillColour.IsTransparent())
		{
			// Nope, its a real colour alright, so we had better set the fill colour
			SetFillColour(RequiredCol);
		}
	}

	// Find out about the line colour
	DocColour MyStrokeColour = RR_STROKECOLOUR();

	// See if it is 'no colour'
	if (!MyStrokeColour.IsTransparent())
	{
		// Nope, its a real colour alright, so we had better set the fill colour
		SetLineColour(RequiredCol);
	}

	// Set Fill transp to none
	if (pFillAttr!=NULL)
		RenderRegion::SetTranspFillGeometry(pFillAttr, FALSE);

	// Set the stroke transp to none
	if (pStrokeAttr!=NULL)
		RenderRegion::SetLineTransp(pStrokeAttr, FALSE);
}




/********************************************************************************************

>	BOOL MaskedRenderRegion::SpreadMask()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Returns:	TRUE if the mask was spread, FALSE if there was no mask in the first place
	Purpose:	Spreads the mask out by a pixel. This allows us to make the area that is
				blitted slightly bigger to cover up any differences between the device
				resolution and the bitmap resolution. This function will grow the mask
				above, below, to the left and right by a single pixel. If this is not
				enough, call this function more than once.

********************************************************************************************/

BOOL MaskedRenderRegion::SpreadMask()
{
	// If there is a bitmap
	if (pBits!=NULL && pBitmapInfo!=NULL)
	{
		// Get the scan line size in bytes
		INT32 ScanLineByteSize = (pBitmapInfo->bmiHeader.biWidth+7)/8;

		// Round it up to the nearest word boundary. Note, this is still a count of bytes
		INT32 ScanLineWordSize = ((ScanLineByteSize+3)/4)*4;
		
		// Start looping through all the scan lines in the mask,
		// going from top to bottom, moving set pixels up
		LPBYTE CurrentByte = NULL;
		LPBYTE ByteAbove = NULL;
		INT32 i;
		for (i=1; i<pBitmapInfo->bmiHeader.biHeight; i++)
		{
			// point CurrentByte at the start of the next scan line
			CurrentByte = pBits + (i*ScanLineWordSize);
			ByteAbove = pBits + ((i-1)*ScanLineWordSize);

			// Go along the scan line
			BOOL SetHighBit = FALSE;
			for (INT32 j=0; j<ScanLineByteSize; j++)
			{
				// Try and set the leading pixel on the current byte
				BYTE Mask = 0x01;
				BYTE Current = BYTE(~(*CurrentByte));

				// Loop though the mask
				BOOL Found = FALSE;
				while (Mask!=0)
				{
					// See if we have found the least sig. pixel yet
					if ((Mask & Current) != 0)
					{
						Found = TRUE;
						break;
					}

					// Move the mask along a bit
					Mask = Mask << 1;
				}

				// Set the high bit if needed
				if (SetHighBit)
				{
					Current = Current | 0x80;
					SetHighBit = FALSE;
				}

				// If we found a bit in this byte, set the bit next to the least sig bit
				if (Found)
				{
					if (Mask==0x01)
						SetHighBit = TRUE;
					else
						Current = Current | (Mask>>1);
				}

				// Or this byte with the one in the scan line above
				BYTE Result = BYTE(~(*ByteAbove)) | Current;
				*ByteAbove = BYTE(~Result);

				// Move to the next byte
				CurrentByte++;
				ByteAbove++;
			}
		}

		// Start looping through all the scan lines in the mask,
		// going from bottom to top, moving set pixels down
		CurrentByte = NULL;
		LPBYTE ByteBelow = NULL;
		for (i=pBitmapInfo->bmiHeader.biHeight-2; i>=0; i--)
		{
			// point CurrentByte at the start of the next scan line
			CurrentByte = pBits + (i*ScanLineWordSize) + ScanLineByteSize;
			ByteBelow = pBits + ((i+1)*ScanLineWordSize) + ScanLineByteSize;

			// Go along the scan line
			BOOL SetLowBit = FALSE;
			for (INT32 j=ScanLineByteSize-1; j>=0; j--)
			{
				// Try and set the leading pixel on the current byte
				BYTE Mask = 0x80;
				BYTE Current = BYTE(~(*CurrentByte));

				// loop though the mask
				BOOL Found = FALSE;
				while (Mask!=0)
				{
					// See if we have found the least sig. pixel yet
					if ((Mask & Current) != 0)
					{
						Found = TRUE;
						break;
					}

					// Move the mask along a bit
					Mask = Mask >> 1;
				}

				// Set the low bit if needed
				if (SetLowBit)
				{
					Current = Current | 0x01;
					SetLowBit = FALSE;
				}

				// If we found a bit in this byte, set the bit next to the most sig bit
				if (Found)
				{
					if (Mask==0x80)
						SetLowBit = TRUE;
					else
						Current = Current | (Mask<<1);
				}

				// Or this byte with the one in the scan line below
				BYTE Result = BYTE(~(*ByteBelow)) | Current;
				*ByteBelow = BYTE(~Result);

				// Move to the next byte
				CurrentByte--;
				ByteBelow--;
			}
		}
	}
	
	return TRUE;
}


/********************************************************************************************

>	INT32 MaskedRenderRegion::FindCoverage(BOOL AnyCoverageReturns100 = FALSE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95

	Inputs:		AnyConverageReturns100 - In most (if not all) calls to this function,
				the caller is only interested in whether coverage is equal to zero
				or not (i.e. "Do I have to bother rendering anything?"). If you set
				this flag to TRUE, this function will return 100% immediately if it
				finds a single pixel set in the mask, or 0% only if there are no pixels
				set in the mask. This is thus a much faster test of "do we need to render"
				because it usually doesn't have to scan far before it finds a pixel to plot.
				(Added by Jason, 1/7/96)

	Returns:	0 - 100. This represents the percentage of the masked bitmap that has pixels
				set in it. A value of 0 means that there are no pixels set in the mask. You
				can take this value to mean 'do not bother trying to display the bitmap cos
				none of it is visible'. A value of 100 means that every pixel in the mask is set
				so no clever masked plotting of the bitmap is required.
				(or, if the AnyConverageReturns100 flag was set, a value of 100 just means
				that there was at least one pixel requiring plotting)

	Purpose:	Finds out how much of the mask has been set.

********************************************************************************************/

INT32 MaskedRenderRegion::FindCoverage(BOOL AnyCoverageReturns100)
{
	INT32 TotalPixels = 0;
	INT32 TotalClearPixels = 0;

	if (pBits!=NULL && pBitmapInfo!=NULL)
	{
		// Get the scan line size in bytes
		const INT32 ScanLineByteSize = (pBitmapInfo->bmiHeader.biWidth+7)/8;

		// Round it up to the nearest word boundary. Note, this is still a count of bytes
		const INT32 ScanLineWordSize = ((ScanLineByteSize+3)/4)*4;

		// Work out how many padding pixels (bits) are on the end of the scanline
		const INT32 EndPixels = 8- ((ScanLineByteSize*8) - pBitmapInfo->bmiHeader.biWidth);

		// The last byte of each scanline may not use all its pixels, so we build a mask,
		// which is used to discount padding pixels from consideration
		BYTE Mask = 0x0;
		{
			BYTE Bit = 0x80;
			for (INT32 i=0; i<EndPixels; i++)
				Mask |= Bit>>i;
		}


		// Start looping through all the scan lines in the mask
		LPBYTE CurrentByte = NULL;
		for (INT32 i=0; i<pBitmapInfo->bmiHeader.biHeight; i++)
		{
			// point CurrentByte at the start of the next scan line
			CurrentByte = pBits + (i*ScanLineWordSize);

			// Count this scan line of pixels
			TotalPixels += pBitmapInfo->bmiHeader.biWidth;

			// Go along the scan line, counting pixels
			for (INT32 j=0; j<ScanLineByteSize-1; j++)
			{
				// look up the current byte
				TotalClearPixels += BitCountTable[*CurrentByte];

				// Move to the next byte
				CurrentByte ++;
			}

			BYTE EndByte = *CurrentByte & Mask;
			TotalClearPixels += BitCountTable[EndByte];

			// If we just want to know if the entire mask is clear, then check if we found
			// any non-masked pixels at all in this scanline - if so, return immediately.
			if (AnyCoverageReturns100 && TotalClearPixels != TotalPixels)
				return(100);
		}
	}

	// The mask was empty or missing
	if ((TotalClearPixels==TotalPixels) || (TotalPixels==0))
	{
		TRACEUSER( "Rik", _T("Coverage = 0\n"));
		return 0;
	}

	// Every pixel was set
	if (TotalClearPixels==0)
	{
		TRACEUSER( "Rik", _T("Coverage = 100\n"));
		return 100;
	}

	// Work out the result
	INT32 Result = 100 - ((TotalClearPixels*100) / TotalPixels);
	
	// Rounding errors could give this misleading result
	if (Result<=0)
		Result = 1;

	// and this one, so change them
	if (Result>=100)
		Result = 99;

	// Tell them what percentage of pixels were set
	TRACEUSER( "Rik", _T("Coverage = %ld\n"), Result);
	return Result;
}



/********************************************************************************************

>	INT32 MaskedRenderRegion::FindMaskDpi()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/95
	Returns:	The Dpi of the mask in this region
	Purpose:	This function is used to find out the DPI that the region was created at.
				This is important as the device it is displaying to may have a different
				dpi and the difference has to be compensated for.
				Returns a INT32 form of the dpi
				As most printing and masking related items only require non-fractional dpis
				then this should not be a problem. So return a rounded up form of the stored
				double form. 

********************************************************************************************/

INT32 MaskedRenderRegion::FindMaskDpi()
{
	// tell them about the dpi
	// Need to return a rounded form of the stored double dpi
	return (INT32)(PixelsPerInch + 0.5);
}

/********************************************************************************************

>	double MaskedRenderRegion::FindMaskDpi()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Returns:	The Dpi of the mask in this region
	Purpose:	This function is used to find out the DPI that the region was created at.
				This is important as the device it is displaying to may have a different
				dpi and the difference has to be compensated for.
				Returns the new double form of the dpi

********************************************************************************************/

//double MaskedRenderRegion::FindMaskDpi()
//{
	// tell them about the dpi
//	return PixelsPerInch;
//}


/********************************************************************************************

>	void MaskedRenderRegion::GetFirstMaskRegion(MaskRegion* pMaskInfo)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95
	Outputs:	pMaskInfo - holds info about the first region as output
	Purpose:	This function finds the first scan line section in the mask that indicates
				pixels need to be set. When it finds the next section it fills in the info
				about it in pMaskInfo and returns.
				If there are no regions to be found in the mask then the Length member
				of pMaskInfo will be set to zero. This is your clue that there are no
				scan line sections to process.
	SeeAlso:	MaskedRenderRegion::GetNextMaskRegion; struct MaskRegion

********************************************************************************************/

void MaskedRenderRegion::GetFirstMaskRegion(MaskRegion* pMaskInfo)
{
	// Set up the starting conditions
	pMaskInfo->x = 0;
	pMaskInfo->y = 0;
	pMaskInfo->Length = 0;

	// start looking for a bit we have to blit
	ScanMaskFrom(pMaskInfo);
}




/********************************************************************************************

>	void MaskedRenderRegion::GetNextMaskRegion(MaskRegion* pMaskInfo)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95
	Inputs:		pMaskInfo - Holds info about the last region that was found
	Outputs:	pMaskInfo - but holds info about the next region as output
	Purpose:	This function finds the next scan line section in the mask that indicates
				pixels need to be set. It uses the info in the pMaskInfo param to work out
				where to start looking from. When it finds the next section, which could
				be on the same scan line as the present one, it fills in the info about it
				in pMaskInfo and returns.
				If there are no more regions to be found in the mask then the Length member
				of pMaskInfo will be set to zero. This is your clue that there are no more
				scan line sections to process.
	SeeAlso:	MaskedRenderRegion::GetFirstMaskRegion; struct MaskRegion

********************************************************************************************/

void MaskedRenderRegion::GetNextMaskRegion(MaskRegion* pMaskInfo)
{
	// Find the end of the last scan section	
	// Move to the end of the last region
	pMaskInfo->x += pMaskInfo->Length;

	// are we at the end of the scan line
	if (pMaskInfo->x >= pBitmapInfo->bmiHeader.biWidth)
	{
		pMaskInfo->x = 0;
		pMaskInfo->y++;
	}

	// Reset the length to zero
	pMaskInfo->Length = 0;

	// start looking for a bit we have to blit
	ScanMaskFrom(pMaskInfo);
}



/********************************************************************************************

>	void MaskedRenderRegion::ScanMaskFrom(MaskRegion* pMaskInfo)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		pMaskInfo - Holds the position to start looking from
	Outputs:	pMaskInfo - Hold info about the next scan section to look at
	Purpose:	Searchs the mask for a section that means we will have to blit something.
				The starting position of the section and the length of the section are put
				into pMaskInfo and returned. pMaskInfo->Length will be set to zero if there
				are no more sections in the mask.

********************************************************************************************/

void MaskedRenderRegion::ScanMaskFrom(MaskRegion* pMaskInfo)
{
	// Assume we will find no more scan sections
	pMaskInfo->Length = 0;

	// get a mask to look along the bits in the mask
	INT32 Mask = 0x80;
	INT32 XStartPos = pMaskInfo->x;
	INT32 YStartPos = pMaskInfo->y;

	// Set the mask up correctly, as the X starting pos may not be at the start of a byte
	if (XStartPos!=0)
	{
		// Go through the byte, setting the bits back to 1 as needed
		INT32 PixelNum = (XStartPos/8) * 8;
		while (PixelNum < XStartPos)
		{
			// Shift the bit along
			Mask = Mask>>1;

			// Count the pixel
			PixelNum++;
		}
	}

	// Find out how many bytes in a scan line
	INT32 ScanLineSize = (pBitmapInfo->bmiHeader.biWidth+7)/8;
	ScanLineSize = (ScanLineSize+3)&~3;

	// We start out looking for the start of a region
	BOOL LookingForStart = TRUE;

	// a pointer to the byte we are dealing with
	LPBYTE pByte = NULL;

	// try and find the start of the first region
	for (INT32 j=YStartPos; j<pBitmapInfo->bmiHeader.biHeight; j++)
	{
		// Get the byte we are interested in
		pByte = pBits + (j*ScanLineSize) + (XStartPos/8);

		// start going accross the scan line
		for (INT32 i=XStartPos; i<pBitmapInfo->bmiHeader.biWidth; i++)
		{
			// Find out what this bit is (1 or 0)
			INT32 ThisBit = (*pByte) & Mask;

			// what are we looking for - the start or the end of a region.
			if (LookingForStart)
			{
				if (ThisBit == 0)
				{
					// Set the start position in the mask region
					pMaskInfo->x = i;
					pMaskInfo->y = j;
					
					// This bit could be all there is of a 1 pixel wide region
					pMaskInfo->Length = 1;

					// we are no longer looking for the start of the region
					LookingForStart = FALSE;
				}
			}
			else
			{
				// We are looking for the end of the region
				if (ThisBit!=0)
				{
					// we have reached the end of the region
					return;
				}
				else
				{
					// we are still in the region, so count the pixel
					pMaskInfo->Length++;
				}
			}

			// Move the mask bit along to the next pixel
			Mask = Mask>>1;
			
			// if we have exhasuted this byte, go on to the next one and reset the mask
			if (Mask==0)
			{
				Mask = 0x80;
				pByte++;
			}
		}
		
		// if we have found part of a region at the end of the scan line, return it
		if (pMaskInfo->Length>0)
			return;

		// Start the next scan line from the beginning
		XStartPos = 0;
		Mask = 0x80;
	}
}


