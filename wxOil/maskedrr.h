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
// The Masked Render Region

#ifndef INC_MASKEDRR
#define INC_MASKEDRR

#include "grndbmp.h"
#include "rrcaps.h"

class FlatTranspFillAttribute;
class StrokeTranspAttribute;


/********************************************************************************************

<	struct MaskRegion

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95
	Comment:	This struct holds info that is used by the MaskedRenderRegion::GetFirstMaskRegion
				and MaskedRenderRegion::GetNextMaskRegion. This struct is used to define the
				1 pixel high regions that are extracted from the mask a scan line at a time.
				It is a simple struct with the following public member vars in it :-
				INT32 x - The X coord of the start of the masked region
				INT32 y - The Y Coord of the start of the masked region
				INT32 Length - The length in pixels of the masked region, or 0 if there are no
				more regions in the mask
	SeeAlso:	MaskedRenderRegion::GetFirstMaskRegion; MaskedRenderRegion::GetNextMaskRegion

********************************************************************************************/

struct MaskRegion
{
	INT32 x;
	INT32 y;
	INT32 Length;
};



/********************************************************************************************

>	class MaskedRenderRegion : public GRenderBitmap

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Purpose:	The Masked Render region is used when rendering to regions that can not
				do all the fabby things that GRender Regions are capable of (eg Transparancy).
				They are used as part of the 3 phase rendering process, in the middle phase.
				In this phase all the objects that are deemed to be too complicated to be
				rendered by the host render region are actually rendered into a GRenderRegion.
				Each of the individual objects that are complex are also rendered into this
				Mono Masked Render Region. At the end of the process this Masked Render region
				is used to identify the minimum area of the render region that needs to be
				blitted into the host render region.

********************************************************************************************/

class MaskedRenderRegion : public GRenderBitmap
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(MaskedRenderRegion)

public:
	// Construction/Destruction etc
	MaskedRenderRegion(DocRect, Matrix, FIXED16, double);
	~MaskedRenderRegion();
	
	// Start render function
	BOOL StartRender();

	// Function to tell us about the host render region
	void SetHostCaps(RRCaps* pCaps);

	// Render Functions we have to overide
	virtual void DrawPathToOutputDevice(Path *PathToRender, PathShape shapePath=PATHSHAPE_PATH);
	virtual void DrawRect(DocRect *RectToRender);
	virtual void DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint);
	virtual void DrawPixel(const DocCoord &Point);

	// Functions that are supposed to draw bitmaps
	virtual void DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap);
	virtual void DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID = NULL);
	virtual BOOL DrawTransformedBitmap(NodeBitmap *pNodeBitmap);

	// Other Pure virtual drawing functions that have to be overidden
	virtual void DrawDragRect(DocRect *RectToRender);
	virtual void DrawBlob(DocCoord p, BlobType type);
	virtual void DrawCross(const DocCoord &Point, const UINT32 Size);
	virtual void DrawBitmapBlob(const DocCoord &Point, KernelBitmap* BlobShape);
	virtual void DrawBitmapBlob(const DocCoord &Point, ResourceID resID );

	// Function to help out during the masked blit
	BOOL SpreadMask();
	INT32 FindCoverage(BOOL AnyCoverageReturns100 = FALSE);
	INT32 FindMaskDpi();
	//FIXED16 FindMaskDpi();

	// Functions to help find all the regions in the mask that need plotting
	void GetFirstMaskRegion(MaskRegion* pMaskInfo);
	void GetNextMaskRegion(MaskRegion* pMaskInfo);

public:
	virtual void SetOffscreen(OffscreenAttrValue*)
	{
	}
	virtual void RestoreOffscreen(OffscreenAttrValue*)
	{
	}

protected:
	// Function to help the GetFirst/NextMaskRegion functions
	void ScanMaskFrom(MaskRegion* pMaskInfo);

	// Function to help the AboutToRenderComplex/SimpleShape functions
	void SetNextShapeColour(DocColour& RequiredCol);

	// The colours
	DocColour Black;
	DocColour White;

// Member vars
protected:
	// The Host render regions caps
	RRCaps HostCaps;

	// Some attrs used as we go
	FlatTranspFillAttribute* pFillAttr;
	StrokeTranspAttribute* pStrokeAttr;

	static INT32 BitCountTable[256];
};

#endif  // INC_MASKEDRR

 
