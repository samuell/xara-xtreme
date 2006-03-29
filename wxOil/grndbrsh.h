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

//GRenderBrush - render region used for drawing brush strokes



#include "grndrgn.h"
#include "grnddib.h"

class DocRect;

#ifndef INC_GRNDBRSH
#define	INC_GRNDBRSH

#include "grndrgn.h"

/********************************************************************************************

>	class GRenderBrush : public GRenderDIB

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	20/12/99
	Purpose:	A GDraw render region that uses normal DIBs for the bitmap.
				It serves a particularly special purpose in that it provides a number of 
				functions that allow us to blit to the screen very quickly without 
				getting tangled up in the render loop.  This is so that we can draw brush 
				strokes and have them appear on the screen in real time.
				This is achieved by maintaining not one but  four static
				bitmaps.  These work in the following way:

				The main bitmap (lpbits) needs to have the current view rendered into it before
				beginning the brush stroke.
				lpbrushbits - bitmap which has the brush rendered into it.
				
				What we do instead of rendering is to simply combine the brush bitmap with 
				the view bitmap, copy the results to lppreconvertbits and either blit it or 
				convert it(if we are on a <24bit device).

********************************************************************************************/

class GRenderBrush : public GRenderDIB
{
	CC_DECLARE_DYNAMIC(GRenderBrush)

public:
	GRenderBrush(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi);
	~GRenderBrush();

	 
	void DrawToScreenNow(); // the top level function that gets whatever is in the
							// is in the changed BBox onto the screen
	
public:   // access functions
	void SetChangedBBox(DocRect Rect);

public: // functions you need to call to set up the bitmaps
		// before you draw anything
	
	BOOL InitialiseBrushBitmaps(DocRect BrushRect);
	void SetupMainBitmap();
	
	// plots the brush into the main bitmap
	BOOL PlotBrushIntoBigBitmap(DocRect BrushRect);
	
	// captures the screen as a bitmap
	BOOL CaptureView(Spread* pSpread);
	
	// sets the window to the top of the z-order
	BOOL MoveWindowToFront();
	
	BOOL ResetMatrix();

	void Blackout();

	static BOOL Convert16to32( LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits, RGBQUAD* pPalette,
							  LPBITMAPINFO pDestInfo, LPBYTE pDestBits);
	UINT32  GetScreenDepth() { return (UINT32)ScreenDepth; }

protected: // helper functions
	BOOL ConvertBrushBitmap();   // converts the brush bitmap to the right depth
	BOOL TestConvertBitmap();
	
	BOOL ClipBrushBitmaps(RECT* pUsedRect, INT32* pHeight);  //makes sure we only blit the 
														   // the size of the usedrect
	void SetupBitmap(LPBITMAPINFO pBitmapInfo, LPBYTE pBits);
	BOOL InitialiseBrushBitmaps(INT32 Width, INT32 Height, INT32 RenderDepth, INT32 DeviceDepth);

	BOOL CaptureViewByRendering(); // renders the view into our bitmap

	// test to split up the captureview fns for better profiling
	BOOL Blit(HDC DestDC, HDC SourceDC, INT32 Height, INT32 Width, INT32 Left, INT32 Top);
	BOOL GetBits(HDC hDC, HBITMAP hBitmap, INT32 Height, LPBYTE lpbits, LPBITMAPINFO lpInfo);

	virtual void FreeLPBits( LPBITMAPINFO, LPBYTE );

// Karim 21/07/2000
// Overrides so that GRenderBrush doesn't have to worry about feathers
// (which are offscreen attrs).
public:
	virtual void SetOffscreen(OffscreenAttrValue*) {}
	virtual void RestoreOffscreen(OffscreenAttrValue*) {}

//#ifdef NEWFASTBRUSHES
	// CGS 16/1/2001
	// allow us to optimise the code within DrawToScreenNow - so that we do NOT always have
	// to expensively grab hold of the views DC within an interactive loop ....
	void SetView (View*	v)			{ m_pView = v; }
	void SetCamView (CCamView* v)	{ m_pCCamView = v; }
	void SetCDC (CDC* cdc)			{ m_pDevContext = cdc; }
	void SetHDC (HDC hdc)			{ m_DeviceHdc = hdc; }
//#endif

protected:	// Data

	LPBITMAPINFO lpPreConvertBrushBitmapInfo;	
	LPBYTE lpPreConvertBrushBits;
	LPBITMAPINFO lpPostConvertBrushBitmapInfo;
	LPBYTE lpPostConvertBrushBits;
	LPBITMAPINFO lpBrushBitmapInfo;
	LPBYTE lpBrushBits;

	LPBITMAPINFO lpTestInfo;
	LPBYTE		 lpTestBits;

	RGBQUAD* m_pPalette;

//#ifdef NEWFASTBRUSHES
	// CGS 16/1/2001
	// allow us to optimise the code within DrawToScreenNow - so that we do NOT always have
	// to expensively grab hold of the views DC within an interactive loop ....
	View*	m_pView;
	CCamView* m_pCCamView;
	CDC* m_pDevContext;
	HDC m_DeviceHdc;
//#endif
};
	
#endif
