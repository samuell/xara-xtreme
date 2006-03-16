// $Id: pmaskrgn.h 662 2006-03-14 21:31:49Z alex $
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
// The Printing Masked Render Region

#ifndef INC_PMASKRGN
#define INC_PMASKRGN

#include "maskedrr.h"


/********************************************************************************************

>	class PrintingMaskedRenderRegion : public MaskedRenderRegion

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/95
	Purpose:	This is a simplified version of the MaskedRenderRegion. It does not have to
				cope with a mix of complex and simple shapes to be rendered, but knows that
				there will be a run of complex shapes and then all the rest will be simple.

********************************************************************************************/

class PrintingMaskedRenderRegion : public MaskedRenderRegion
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(PrintingMaskedRenderRegion)

public:
	// Construction/Destruction etc
	PrintingMaskedRenderRegion(DocRect, Matrix, FIXED16, double, BOOL Printing);
	~PrintingMaskedRenderRegion();
	
	// Function to tell us if we are in a complex run or not
	void SetMaskDrawingMode(BOOL OnOff);

	// Render Functions we have to overide
	virtual void DrawPathToOutputDevice(Path *PathToRender, PathShape shapePath=PATHSHAPE_PATH);
	virtual void DrawRect(DocRect *RectToRender);
	virtual void DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint);
	virtual void DrawPixel(const DocCoord &Point);

	// Functions that are supposed to draw bitmaps
	virtual void DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap);
	virtual void DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID = NULL);
	virtual BOOL DrawTransformedBitmap(NodeBitmap *pNodeBitmap);

	// Function to handle the rendering of chars into the mask
	virtual BOOL RenderChar(WCHAR ch, Matrix* pMatrix);

	// Used to calculate the size of the render region required
	virtual WinRect CalculateWinRect( Matrix& RenderMatrix, const DocRect& docrect,
									  const double dpi);
	virtual GMATRIX MakeGavinMatrix(Matrix NewRenderMatrix, DocRect ClipRect, double dPixelsPerInch, BOOL bMasterCapture);

// Karim 19/12/2000
// Overrides so that PrintingMaskedRenderRegion does not render offscreen attrs.
// The printing loops are fairly loopy & need looking at, but in any case I see
// no current point in rendering a bitmap-processing attr into a mask.
public:
	virtual void SetOffscreen(OffscreenAttrValue*)
	{
	}
	virtual void RestoreOffscreen(OffscreenAttrValue*)
	{
	}

// Member vars
private:
	BOOL IsComplexRun;
};

#endif  // INC_PMASKRGN
