// $Id: grndbrsh.cpp 662 2006-03-14 21:31:49Z alex $
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

//GRenderBrush - implementation of the render region used for 
// drawing brushes



#include "camtypes.h"
#include "grndbrsh.h"
#include "grnddib.h"
#include "grndrgn.h"
#include "rndrgn.h"
#include "osrndrgn.h"
#include "gdrawasm.h"
#include "palman.h"
#include "scrvw.h"
#include "scrcamvw.h"
#include "view.h"
#include "fixmem.h"
#include "spread.h"
#include "progress.h"
#include "freehand.h"
#include "freeinfo.h"
//#include "freehres.h"
#include  "offscrn.h"
#include "wbitmap.h"
#include "oilruler.h"

static RealLogPalette StandardPalette = 
{
	0x300,
	256
};

static BYTE NoTransTable[256];						// default translation table

static BOOL ClippedPathIsValid;


#include "defcons.tab"

// TODOG: Nasty local preference...
static INT32 LargeGradTables = 0;
	  
// setting this to non-zero controls fresh bitmap initialisation
#define	FILL_BITMAP	0

CC_IMPLEMENT_DYNAMIC( GRenderBrush, GRenderDIB )

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	GRenderBrush::GRenderBrush(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GRenderBrush constructor. Doesn't do anything actually, just calls base class.
				INT32 dpi changed to double dpi (12/12/95) to improve the range of values allowed
				at the < 1000dpi settings that we will be using.
	Errors:		-
	SeeAlso:	GRenderRegion::GRenderRegion

********************************************************************************************/

GRenderBrush::GRenderBrush(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi)
	: GRenderDIB( ClipRegion, ConvertMatrix, ViewScale, Depth, dpi)
{
	lpPreConvertBrushBitmapInfo  = NULL;
	lpPostConvertBrushBitmapInfo = NULL;
	lpPreConvertBrushBits		 = NULL;
	lpPostConvertBrushBits		 = NULL;
	lpBrushBitmapInfo			 = NULL;
	lpBrushBits					 = NULL;
	m_pPalette                   = NULL;

//#ifdef NEWFASTBRUSHES
	m_pView = NULL;
	m_pCCamView = NULL;
	m_pDevContext = NULL;
	m_DeviceHdc = NULL;
//#endif
}



/********************************************************************************************

>	GRenderBrush::~GRenderBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GRenderBrush destructor. Frees up the bitmaps.
	Errors:		-

********************************************************************************************/

GRenderBrush::~GRenderBrush()
{	
	if (lpPreConvertBrushBitmapInfo != NULL && lpPreConvertBrushBits != NULL)
		FreeLPBits(lpPreConvertBrushBitmapInfo, lpPreConvertBrushBits);

	
	if (lpPostConvertBrushBitmapInfo != NULL && lpPostConvertBrushBits != NULL)
		FreeLPBits(lpPostConvertBrushBitmapInfo, lpPostConvertBrushBits);


	if (lpBrushBitmapInfo != NULL && lpBrushBits != NULL)
		FreeLPBits(lpBrushBitmapInfo, lpBrushBits);

	if (lpBits != NULL && lpBitmapInfo != NULL)
	{
		FreeLPBits( lpBitmapInfo, lpBits );
		SetBitmapPointers(NULL, NULL);
	}
	TRACEUSER( "Gavin", _T("GRenderBrush::~GRenderBrush - RenderFlags.Rendering = FALSE;\n"));
	RenderFlags.Rendering = FALSE;
	// will call GRenderDIBs destructor here
}

/********************************************************************************************

>	void GRenderBrush::DrawToScreenNow()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/99
	Inputs:		
	Outputs:	
	Purpose:	This is a bit of a bodge function so that we could get the brush to draw 
				itself to the screen ASAP.  It bypasses all the regular rendering rubbish 
				and gets straight down to blitting.  
				

********************************************************************************************/

void GRenderBrush::DrawToScreenNow()
{
	// this fn. basically blits to the screen, so we need to know, where on the source bitmap
	// we take our data, where on the destination bitmap we put it, and what size chunk we move
	
	INT32 DestX = WRect.left;
	INT32 DestY = WRect.top;
	INT32 ViewWidth = WRect.right - WRect.left;
	INT32 ViewHeight= WRect.bottom - WRect.top;
	INT32 Width = 0;
	INT32 Height = 0;
	INT32 SourceX = WRect.left;
	INT32 SourceY = WRect.top;
	
	// if ChangedBBox is set then we don't have to blit so much
	RECT UsedRect;
			
	if (GetDrawContext()->GetChangedBBox( &UsedRect ))
	{
		if ((UsedRect.right < UsedRect.left) ||
			(UsedRect.bottom< UsedRect.top ))
		{
			// Changed box is illegal so exit
			return;
		}
		else
		{
			View*	pView = NULL;
			CCamView* pCCamView = NULL;
			CDC* pDevContext = NULL;
			HDC DeviceHdc = NULL;
			
			// find out about our device 
			if (m_pView)		{ pView = m_pView; }
			else				{ m_pView = View::GetCurrent(); }
			if (m_pCCamView)	{ pCCamView = m_pCCamView; }
			else				{ pCCamView = pView->GetConnectionToOilView(); }
			if (m_pDevContext)	{ pDevContext = m_pDevContext; }
			else				{ pDevContext = ((ScreenCamView*)pCCamView)->GetRenderDC(); }
			if (m_DeviceHdc)	{ DeviceHdc = m_DeviceHdc; }
			else				{ DeviceHdc = pDevContext->GetSafeHdc(); }

			if (ScreenDepth < 24)
			{
				INT32 OldHeight = -1;
				OldHeight = lpPreConvertBrushBitmapInfo->bmiHeader.biHeight;
		
				Width = UsedRect.right - UsedRect.left;
				Height = UsedRect.bottom - UsedRect.top;
				
				if (!ClipBrushBitmaps(&UsedRect, &Height))
					return;
				SourceX = UsedRect.left;
				DestX = UsedRect.left; 
				DestY = UsedRect.top; 

				// bit of a hack because DIBs are stored upside down
				SourceY = ViewHeight - UsedRect.bottom;
		
				if (Height > 0)
				{
					// copy from the big bitmap to the small brush bitmap
					if (!DIBUtil::CopyBitmapSection(lpBitmapInfo, lpBits, 
								lpPreConvertBrushBitmapInfo, lpPreConvertBrushBits,
								SourceY, SourceX))
					{
						ERROR3("Failed to copy bitmap section");
						return;
					}
					
					// convert it to screen depth
					ConvertBrushBitmap();
				
					if (ScreenDepth == 8)
						SetPaletteEntries(lpPostConvertBrushBitmapInfo, RenderDC);

					
					// blit the post-converted bitmap
					INT32 res = SetDIBitsToDevice( DeviceHdc, /*RenderDC->m_hDC,*/ //hDCScreen,
												DestX , DestY,
												Width,
												Height,
												0, 0,				// lower left of DIB (upside down)
												0, Height,						// actual bitmap height in entirety
												lpPostConvertBrushBits,
												lpPostConvertBrushBitmapInfo,
												DIB_RGB_COLORS);
																
					/*
					TestConvertBitmap();
					INT32 res = SetDIBitsToDevice( RenderDC->m_hDC,
											0, ViewHeight,
											ViewWidth,
											ViewHeight,
											0, 0,				// lower left of DIB (upside down)
											0, ViewHeight,						// actual bitmap height in entirety
											lpTestBits,
											lpTestInfo,
											DIB_RGB_COLORS);
					*/
					if (!res)
					{
						TRACEUSER( "Diccon", _T("SetDIB Failed.  DestY = %d, Height = %d\n"), DestY, Height);
						//ERROR3("SetDIB failed in GRenderRegion::DrawToScreenNow");
						DWORD error = GetLastError();
						TRACEUSER( "Diccon", _T("Failed to set Bits in DrawToScreen Now, Error %d\n"), error);
					}
				}
				else
					TRACEUSER( "Diccon", _T("SetDIB Failed.  DestY = %d, Height = %d\n"), DestY, Height);
				// restore the old height
				//if (OldHeight != -1)
				lpPreConvertBrushBitmapInfo->bmiHeader.biHeight = OldHeight;
				lpPostConvertBrushBitmapInfo->bmiHeader.biHeight = OldHeight;
				return;
			}
			else
			{
				//we don't need to convert, just call the win32 function
				SourceX += UsedRect.left;
				DestX += UsedRect.left;
				SourceY += UsedRect.top;
				DestY += UsedRect.top;

				Width = UsedRect.right - UsedRect.left;
				Height = UsedRect.bottom - UsedRect.top;
		
				DWORD *rgb = (DWORD *)lpBitmapInfo->bmiColors ;
				*rgb++ = 0x00ff0000 ;
				*rgb++ = 0x0000ff00 ;
				*rgb++ = 0x000000ff ;
			
				CanDoDeepDIBs = TRUE;
				const UINT32 TotalHeight = (UINT32)lpBitmapInfo->bmiHeader.biHeight;		// height of complete DIB

			
				const INT32 DIBSourceY = TotalHeight - (Height + SourceY);
		
				INT32 res = SetDIBitsToDevice(   DeviceHdc, 
												DestX, DestY,
												
												Width,
												Height,
												SourceX, DIBSourceY,				// lower left of DIB (upside down)
												0, TotalHeight,						// actual bitmap height in entirety
												lpBits,
												lpBitmapInfo,
												DIB_RGB_COLORS);
				
				if (res == 0)
				{
						TRACEUSER( "Diccon", _T("SetDIB Failed.  DestY = %d, Height = %d\n"), DestY, Height);
						//ERROR3("SetDIB failed in GRenderRegion::DrawToScreenNow");
						DWORD error = GetLastError();
						TRACEUSER( "Diccon", _T("Failed to set Bits in DrawToScreen Now, Error %d\n"), error);
				}
#if 0
				static DWORD startCount = 0;
				static INT32 fps = 0;
				if (startCount != 0)
				{
					if ((GetTickCount () - startCount) <= 100)
					{
						fps++;
					}
					else
					{
						TRACEUSER( "ChrisS", _T("%i, "), fps);
						//TextOut (DeviceHdc, 0, 0, data, lstrlen (data));
						startCount = 0;
						fps = 0;
					}
				}
				else
				{
					startCount = GetTickCount ();
				}
				
				/*RECT bounds;
				bounds.left = DestX;
				bounds.top = DestY;
				bounds.right = DestX + Width;
				bounds.bottom = DestY + Height;

				FrameRect (DeviceHdc, &bounds, (HBRUSH) GetStockObject (BLACK_BRUSH));*/
#endif

			//	PlotBitmap( RenderDC->m_hDC, DIB_RGB_COLORS, lpBitmapInfo, lpBits, DestX, DestY,
			//				Width, Height, hPalette, SourceX, SourceY );
				
			}
		//	DeleteDC(hDCScreen);
		}
	}	

}


/********************************************************************************************

>	BOOL GRenderBrush::InitialiseBrushBitmaps(DocRect BrushRect)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/99
	Inputs:		BrushRect - the size of the brush
	Outputs:	
	Returnd:	TRUE if successful, FALSE otherwise
	Purpose:	Public interface to the initialisation function, you only need to pass in the
				size of the brush and the rest is taken care of for you

********************************************************************************************/

BOOL GRenderBrush::InitialiseBrushBitmaps(DocRect BrushRect)
{
	if (BrushRect.IsEmpty())
	{
		ERROR3("Rect is empty");
		return FALSE;
	}

	RECT WinRect  = OSRenderRegion::DocRectToWin(RenderMatrix, BrushRect, PixelsPerInch);
	INT32 Height =  WinRect.bottom - WinRect.top;
	INT32 Width = WinRect.right - WinRect.left;
	INT32 RenderDepth = 32;
	INT32 DeviceDepth = ScreenDepth;

	return InitialiseBrushBitmaps(Width, Height, RenderDepth, DeviceDepth);
}



/********************************************************************************************

>	BOOL GRenderBrush::InitialiseBrushBitmaps(INT32 Width, INT32 Height, INT32 RenderDepth, 
												INT32 DeviceDepth)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/99
	Inputs:		Width  = width of the brush object in pixels
				Height - height of the brush object in pixels
				RenderDepth - required BPP of the bitmap (no reason for this to be < 32)
				DeviceDepth - BPP of the device we are drawing to
	Outputs:	
	Returns:    TRUE if successful, FALSE if we fail (most likely memory)
	Purpose:	allocates bitmaps of the correct size, depth etc. 

********************************************************************************************/


BOOL GRenderBrush::InitialiseBrushBitmaps(INT32 Width, INT32 Height, INT32 RenderDepth, INT32 DeviceDepth)
{
	// quick check
	if (Width <=0 || Height <= 0)
	{
		ERROR3("illegal height or width value");
		return FALSE;
	}

//	TRACE( _T("Initialising brush bitmaps\n"));

	// if the bitmaps already exist then delete them
	if ((lpPreConvertBrushBits != NULL) && (lpPreConvertBrushBitmapInfo != NULL))
		FreeLPBits(lpPreConvertBrushBitmapInfo, lpPreConvertBrushBits);

	
	if ((lpPostConvertBrushBits != NULL) && (lpPostConvertBrushBitmapInfo != NULL))
		FreeLPBits(lpPostConvertBrushBitmapInfo, lpPostConvertBrushBits); 

	// get the pre-conversion bitmap
	lpPreConvertBrushBitmapInfo = GetLPBits(Width, Height, RenderDepth, &lpPreConvertBrushBits);

	if (lpPreConvertBrushBitmapInfo == NULL)
	{
		ERROR3("Unable to allocate pre-convert bitmap");
		return FALSE;
	}
//	TRACEUSER( "Diccon", _T("Allocated bitmapinfo: 0x%x, bitmap: 0x%x\n"),lpPreConvertBrushBitmapInfo, lpPreConvertBrushBits);
	// get the post-conversion bitmap
	lpPostConvertBrushBitmapInfo = GetLPBits(Width, Height, DeviceDepth, &lpPostConvertBrushBits);

	if (lpPostConvertBrushBitmapInfo == NULL)
	{
		ERROR3("Unable to allocate post-convert bitmap");
		return FALSE;
	}
//	TRACEUSER( "Diccon", _T("Allocated bitmapinfo: 0x%x, bitmap: 0x%x\n"),lpPostConvertBrushBitmapInfo, lpPostConvertBrushBits);

	// we're not using the brush bitmap any more
/*	lpBrushBitmapInfo = GetLPBits(Width, Height, RenderDepth, &lpBrushBits);
	if (lpBrushBitmapInfo == NULL)
	{
		ERROR3("Unable to allocate post-convert bitmap");
		return FALSE;
	}
*/	
	// get the palette
	LPRGBQUAD pSourcePalette = &(lpBitmapInfo->bmiColors[0]);
	LPRGBQUAD pDestPalette   = &(lpPreConvertBrushBitmapInfo->bmiColors[0]);

	if (!DIBUtil::CopyPalette(pSourcePalette, pDestPalette, lpBitmapInfo->bmiHeader.biClrUsed))
	{
		ERROR3("Failed to copy palette");
		FreeLPBits(lpPreConvertBrushBitmapInfo, lpPreConvertBrushBits);
		FreeLPBits(lpPostConvertBrushBitmapInfo, lpPostConvertBrushBits);
		return FALSE;
	}
	
	return TRUE;
}


/********************************************************************************************

>	BOOL GRenderBrush::ClipBrushBitmaps(RECT* pUsedRect, INT32* pHeight)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/99
	Inputs:		pUsedRect - the screen RECT that we are about to blit

	Outputs:	pHeight - pointer to an INT32 representing the new height of the bitmap, or rather
				the maximum height that we will blit.
	Returns:    TRUE if successful, FALSE if input params are wrong
	Purpose:	When we come to blit at the edge of the view we will end up access violating 
				if we simply proceed regardless.  This function checks to see how close we are
				to the edge and 'pretends' that the bitmap is only big enough to go up up to the
				edge and not beyond.
********************************************************************************************/

BOOL GRenderBrush::ClipBrushBitmaps(RECT* pUsedRect, INT32* pHeight)
{
	// first get the size of the big bitmap	
	INT32 ViewWidth = WRect.right - WRect.left;
	INT32 ViewHeight= WRect.bottom - WRect.top;


	// if we've gone off the screen then just return now
	if (pUsedRect->top >= ViewHeight || pUsedRect->bottom <= 0
		|| pUsedRect->left >= ViewWidth || pUsedRect->right <= 0)
		return FALSE;

	INT32 TopAdjust = 0;
	INT32 BottomAdjust = 0;
	// otherwise clip the offscreen area
	if (pUsedRect->bottom > ViewHeight)
	{
		BottomAdjust = pUsedRect->bottom - ViewHeight;
		pUsedRect->bottom = ViewHeight;
	}

	if (pUsedRect->top <  0)
	{
		TopAdjust = pUsedRect->top;
		pUsedRect->top = 0;
	}
		
	if (pUsedRect->right > ViewWidth)
		pUsedRect->right = ViewWidth;
	if ( pUsedRect->left < 0)
		pUsedRect->left = 0;

	INT32 SourceY = ViewHeight - pUsedRect->bottom;
//	TRACEUSER( "Diccon", _T("Top = %d, SourceY = %d\n"), pUsedRect->top, SourceY);
	// going off the top of the screen is a bit of a special case for some reason
	if (SourceY + *pHeight >= ViewHeight)
	{
		//TRACEUSER( "Diccon", _T("SourceY  = %d, Height = %d\n"), SourceY, Height);
		// Adjust the height of the bitmaps, we need to do the post-convert one as well
		// or else the convert will fail
		INT32 Adjuster = (ViewHeight - (SourceY+*pHeight));
		TRACEUSER( "Diccon", _T("Adjust = %d\n"), Adjuster);
		*pHeight += Adjuster;
		lpPreConvertBrushBitmapInfo->bmiHeader.biHeight = *pHeight;
		lpPostConvertBrushBitmapInfo->bmiHeader.biHeight = *pHeight;
	//	TRACEUSER( "Diccon", _T("Height = %d\n"), *pHeight);
		//TRACEUSER( "Diccon", _T("SourceY = %d\n"), SourceY);		
	}
	else if (TopAdjust != 0)
	{
		*pHeight -= TopAdjust;
		lpPreConvertBrushBitmapInfo->bmiHeader.biHeight = *pHeight;
		lpPostConvertBrushBitmapInfo->bmiHeader.biHeight = *pHeight;
	}
	else if (BottomAdjust != 0)
	{
		*pHeight -= BottomAdjust;
		lpPreConvertBrushBitmapInfo->bmiHeader.biHeight = *pHeight;
		lpPostConvertBrushBitmapInfo->bmiHeader.biHeight = *pHeight;
	}
	return TRUE;
}



/********************************************************************************************

>	BOOL GRenderBrush::PlotBrushIntoBigBitmap(DocRect BrushRect)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/99
	Inputs:		BrushRect - the location in the view where we wish to plot the brush
	Outputs:	
	Returnd:	TRUE if successful, FALSE otherwise
	Purpose:	Plots the brush bitmap into the view bitmap, just like rendering really 
				but quicker.

********************************************************************************************/


BOOL GRenderBrush::PlotBrushIntoBigBitmap(DocRect BrushRect)
{
	if (BrushRect.IsEmpty())
	{
		ERROR3("Brush rect is empty");
		return FALSE;
	}

	if (lpBitmapInfo == NULL || lpBits == NULL 
		|| lpBrushBitmapInfo == NULL || lpBrushBits == NULL)
	{
		ERROR3("Bitmaps not initialised");
		return FALSE;
	}

	INT32 PixelSize = CalcPixelWidth();

	// Get the Bitmap width and height in Millipoints
	INT32 Width  = lpBrushBitmapInfo->bmiHeader.biWidth  * PixelSize;
	INT32 Height = lpBrushBitmapInfo->bmiHeader.biHeight * PixelSize;

	POINT Plot[3]; 
	Plot[0].x = BrushRect.lox;			 Plot[0].y = BrushRect.loy;
	Plot[1].x = BrushRect.lox + Width;   Plot[1].y = BrushRect.loy;
	Plot[2].x = BrushRect.lox;			 Plot[2].y = BrushRect.loy + Height;

	INT32 Style = 1;
	LPRGBQUAD pRGB = &(lpBitmapInfo->bmiColors[0]);
	if (!GetDrawContext()->PlotBitmap(&(lpBrushBitmapInfo->bmiHeader), lpBrushBits, 
									Style, Plot, pRGB, NULL, NULL, NULL,
									NoTransTable))
	{
		ERROR3("Failed to plot bitmap");
		return FALSE;
	}
	
	return TRUE;
}



/********************************************************************************************

>	BOOL GRenderBrush::ConvertBrushBitmap()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/99
	Inputs:		
	Outputs:	
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Takes the contents of lppreconvertbrushbits which will be 32BPP, and calls GDraw to
				convert the contents and put it into lppostconvertbrushbits which has the same BPP 
				as the screen
********************************************************************************************/


BOOL GRenderBrush::ConvertBrushBitmap()
{
	if (lpPreConvertBrushBitmapInfo == NULL || lpPreConvertBrushBits == NULL 
		|| lpPostConvertBrushBitmapInfo == NULL || lpPostConvertBrushBits == NULL)
	{
		ERROR3("Bitmaps not initialised");
		return FALSE;
	}

	GDrawContext *GDC = GetDrawContext();
	
	INT32 ColourFlag = DIB_RGB_COLORS;

	INT32 result = GDC->ConvertBitmap(&(lpPreConvertBrushBitmapInfo->bmiHeader), lpPreConvertBrushBits,
							&(lpPostConvertBrushBitmapInfo->bmiHeader), lpPostConvertBrushBits, DitherStyle8Bit);

	// 16 bit bmps don't like having their palette entries set.
	if (ScreenDepth == 8)
		ColourFlag = SetPaletteEntries(lpPostConvertBrushBitmapInfo, RenderDC);
	else
		ColourFlag = DIB_RGB_COLORS;
	if (result == -1)
	{
		ERROR3("Failed to convert bitmap");
		return FALSE;
	}
	return TRUE;

}

BOOL GRenderBrush::TestConvertBitmap()
{
	if (lpTestInfo == NULL || lpTestBits == NULL || lpBitmapInfo == NULL || lpBits == NULL)
	{
		ERROR3("bitmaps not initialised");
		return FALSE;
	}


	GDrawContext *GDC = GetDrawContext();
	
	INT32 ColourFlag = DIB_RGB_COLORS;

	INT32 result = GDC->ConvertBitmap(&(lpBitmapInfo->bmiHeader), lpBits, &(lpTestInfo->bmiHeader), 
										lpTestBits, DitherStyle8Bit);


	if (result == -1)
	{
		ERROR3("Failed to convert bitmap");
		return FALSE;
	}
	return TRUE;

}



/********************************************************************************************

>	void GRenderBrush::SetupMainBitmap()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/99
	Inputs:		
	Outputs:	
	Purpose:	Public interface which sets up our main bitmap for GDraw to use.  Note that
				the compression flag must be set to false or else things go wrong.

********************************************************************************************/


void GRenderBrush::SetupMainBitmap()
{
	if (lpBitmapInfo == NULL || lpBits == NULL)
	{
		ERROR3("Bitmaps are unititialised in GRenderBrush::SetupMainBitmap");
		return;
	}
	// we need to make sure the compression flag is off
	lpBitmapInfo->bmiHeader.biCompression = 0;
	SetupBitmap(lpBitmapInfo, lpBits);
}



/********************************************************************************************

>	void GRenderBrush::SetupBitmap(LPBITMAPINFO pBitmapInfo, LPBYTE pBits)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/99
	Inputs:		pBitmapInfo - info of the bitmap to be set up
				pBits       - pointer to the bits
	Outputs:	
	Purpose:	Similar to GRenderRegion;;StartRender, initialises the bitmap for GDraw
				to render into it.  
********************************************************************************************/


void GRenderBrush::SetupBitmap(LPBITMAPINFO pBitmapInfo, LPBYTE pBits)
{
	if (pBitmapInfo == NULL || pBits == NULL)
	{
		ERROR3("invalid parameters passed into GRenderBrush::SetupBitmap");
		return;
	}

	// Determine the correct hinting value for the current output mode, and call gavin to
	// set up his offscreen bitmap.
/*	DWORD GavinHint = DIBUtil::GetGavinBlitFormat(ScreenDepth, BitmapDepth, ScreenHinting);
	//TRACEUSER( "Diccon", _T("About to setup bitmap\n"));
	GetDrawContext()->SetupBitmapNoInitialise( pBitmapInfo->bmiHeader.biWidth, pBitmapInfo->bmiHeader.biHeight,
						pBitmapInfo->bmiHeader.biBitCount, pBits, GavinHint, m_DoCompression );
						*/
	GetDrawContext()->SetBitmap( &pBitmapInfo->bmiHeader, pBits );
//	TRACEUSER( "Diccon", _T("Bitmap Setup OK\n"));
	if (RenderDC != NULL)
	{
		if (ScreenDepth == 8)
		{
			// use frame windows palette if desired
			if (PaletteManager::UsePalette())
				hPalette = *(PaletteManager::GetPalette());
		}

		if (hPalette != NULL)
			hPrevPalette = PaletteManager::StartPaintPalette(RenderDC->m_hDC);

		if (BitmapDepth == 8)
		{
			// If we are not error diffusing then set the dither style
			if ((DitherStyle8Bit != 0) && (DitherStyle8Bit != 3))
				GetDrawContext()->SetDitherStyle((DitherStyle) DitherStyle8Bit);		// Stupid prototype
		}

		if (ScreenDepth == 8)
		{
			// Whether we set a palette or not, we need this call on all 8-bit screens
			GRenderRegion::GColInit(RenderDC->m_hDC);
		}
	}

	// Work out our matrix and set it 
	ResetMatrix();

	
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread != NULL)
	{
		// get the bounding rect of the view and set it as the clip rect
		DocView* pDocView = DocView::GetCurrent();
		DocRect ViewRect = pDocView->GetDocViewRect(pSpread);
		pSpread->DocCoordToSpreadCoord(&ViewRect);

		RECT WinRect  = OSRenderRegion::DocRectToWin(RenderMatrix, ViewRect, PixelsPerInch);	
		GetDrawContext()->DeviceClipRectangle(&WinRect);
	}
	
	// setup code borrowed from StartRender

	// set up anti-aliasing & flatness, using the SetQualityLevel function
	SetQualityLevel();
	GetDrawContext()->SetHintingFlag(TRUE);										// Always hint lines!!!

	GetDrawContext()->SetDashAdjustmentFlag(FALSE);

	// work out the brush alignment
	// algorithm based on code from OSRenderRegion::InitAttributes
	POINT DocOrigin, OSOrigin;
	DocOrigin.x = 0;
	DocOrigin.y = 0;
		
	// Then we transform it into OSCoords
	GetDrawContext()->TransformPath( &DocOrigin, &OSOrigin, 1, &CurrentGMatrix );

	// Once we've shifted then right by FX, they're in pixel co-ords, which is what
	// SetHalftoneOrigin is expecting them to be in
	const INT32 BrushX = OSOrigin.x >> FX;
	const INT32 BrushY = -(OSOrigin.y >> FX);					// has to be negated to work

	GetDrawContext()->SetHalftoneOrigin( BrushX, BrushY );
	
}


BOOL GRenderBrush::MoveWindowToFront()
{
	
	// move our main frame to the front
	CMainFrame* pMainFrame = GetMainFrame();
	ERROR2IF(pMainFrame == NULL, FALSE, "Wheres our mainframe gone!!??");
	HWND OurHWnd = pMainFrame->GetSafeHwnd();
	SetWindowPos(OurHWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	CWnd* pFirst = pMainFrame->GetWindow(GW_HWNDFIRST);
	HWND HWFirst = pFirst->GetSafeHwnd();

	// there may be a window on top of us, check to see.  
	// Basically we check downwards from the Desktop windo checking to see if there
	// is an on top window, if so we minimize it, unless it is the taskbar
	CWnd* pNextWnd = pFirst;
	UINT32 Counter = 0;
	HWND hTaskBar = FindWindow("Shell_TrayWnd", NULL);
            
	CWnd* pLast = pNextWnd;
	while (pNextWnd != NULL)
	{
		pLast = NULL;
		if (pNextWnd->GetExStyle() & WS_EX_TOPMOST)
		{
			if (pNextWnd->IsWindowVisible() && (pNextWnd->GetSafeHwnd() != hTaskBar)) 
			{
				Counter++;
				pLast = pNextWnd;
			}
		}
		
		pNextWnd = pNextWnd->GetNextWindow();
	//	if (pLast != NULL)
	//		pLast->ShowWindow(SW_MINIMIZE);
	}
	
	pMainFrame->SetActiveWindow();
	pMainFrame->UpdateWindow();
//	TRACEUSER( "Diccon", _T("Found %d on top windows\n"), Counter);
	if (Counter == 0)
		return TRUE;

	return FALSE;
}


BOOL GRenderBrush::ResetMatrix()
{
	View*	pView = View::GetCurrent();
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread != NULL)
	{
		Matrix	RenderMat = pView->ConstructRenderingMatrix(pSpread);	
		GMATRIX GMat;
		GMat = MakeGavinMatrix(RenderMat, CurrentClipRect, PixelsPerInch, FALSE);

		SetGavinMatrix(&GMat);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL GRenderBrush::CaptureView(Spread* pSpread)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/99
	Inputs:		pSpread - contains the view we wish to grab
	Outputs:	
	Purpose:	Captures the screen (or rather the part of the screen that holds the View) as a bitmap 

********************************************************************************************/

BOOL GRenderBrush::CaptureView(Spread* pSpread)
{
	ERROR3IF(!MasterCaptureIsCurrent() && GetCaptureDepth()>0, "Can't 'CaptureView' while Captures are running");

//	TRACEUSER( "Diccon", _T("Entering CaptureView\n"));
	// put up an hour glass
	String_32 ProgString = "Preparing for brush stroke, please wait..";
	Progress Prog(&ProgString, -1, FALSE);
	
	// make sure we're at the front ( we may still have a window on top though)
	BOOL DoCapture = MoveWindowToFront();

	// If we have a bitmap then ditch it
	if (lpBitmapInfo!=NULL && lpBits != NULL)
	{
		FreeLPBits(lpBitmapInfo, lpBits);
		lpBitmapInfo = NULL;
		lpBits       = NULL;
	}

	// get the current screen depth
	HDC hDCScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
	ScreenDepth = GetDeviceCaps(hDCScreen, BITSPIXEL);
	
	// if we have an 8-bit screen or there is a window on top of us then
	// don't capture the screen, render the view normally.  A bit slower, if the document is complex
	if (ScreenDepth == 8 || !DoCapture) 
	{
		DeleteDC(hDCScreen);
		CaptureViewByRendering();
	//	TRACEUSER( "Diccon", _T("Rendered View\n"));
		return TRUE;
	}

	// Get a new one to replace it
	lpBitmapInfo = GetLPBits(WRect.right-WRect.left, WRect.bottom-WRect.top,
								32, &lpBits);

	// See if we got it
	if (lpBitmapInfo==NULL)
		return FALSE;


	//  make a compatible DC to put the bitmap in
	HDC hDCCompatible = CreateCompatibleDC(hDCScreen);


	// get the window so we can do client to screen to find out how big
	// the view is in pixels
	
	View* pView = View::GetCurrent();
	if (pView == NULL)
	{
		ERROR3("No view in GRenderBrush::CaptureView");
		DeleteDC(hDCScreen);
		DeleteDC(hDCCompatible);
		return FALSE;
	}
	CCamView *pRenderWindow = pView->GetConnectionToOilView();
	if (pRenderWindow == NULL)
	{
		ERROR3("No window in GRenderBrush::CaptureView");
		DeleteDC(hDCScreen);
		DeleteDC(hDCCompatible);
		return FALSE;
	}

	RECT Rect = CalculateWinRect(RenderMatrix, CurrentClipRect, PixelsPerInch );
	pRenderWindow->ClientToScreen( &Rect );

	// get some locals to store the dimensions - remember that DIBs are upside down
	INT32 ViewHeight = Rect.bottom - Rect.top ; 
	INT32 ViewWidth =  Rect.right - Rect.left;
	INT32 ViewTop = Rect.top;
	INT32 ViewLeft = Rect.left;

	// if the rulers are on then we must shift in a little bit
	DocView* pDocView = DocView::GetCurrent();
	
	if (pDocView->AreRulersVisible())
	{
		INT32 RulerSize = OILRuler::GetRenderWidth();
		ViewTop += RulerSize+1;
		ViewLeft += RulerSize+1;
	}

  // allocate the bitmap
	HBITMAP hBMScreen = CreateCompatibleBitmap(hDCScreen, ViewWidth, ViewHeight);

	ERROR2IF(hBMScreen == NULL, FALSE, "unable to allocate bitmap for the view");

	// select the bitmap into the DC
	if (!SelectObject(hDCCompatible, hBMScreen))
	{
		ERROR3("Error selecting bitmap into DC");
		DeleteDC(hDCScreen);
		DeleteDC(hDCCompatible);
		DeleteObject(hBMScreen);
		return FALSE;
	}
	
	hPrevPalette = PaletteManager::StartPaintPalette(hDCScreen);	
	// blit it
	/*if (!BitBlt(hDCCompatible, 
				0, 0,
				ViewWidth, ViewHeight,
				hDCScreen,
				ViewLeft , ViewTop,
				SRCCOPY))*/
	if (!Blit(hDCCompatible, hDCScreen, ViewHeight, ViewWidth, ViewLeft, ViewTop))
	{
		ERROR3("Error selecting bitmap into DC");
		DeleteDC(hDCScreen);
		DeleteDC(hDCCompatible);
		DeleteObject(hBMScreen);
		return FALSE;
	}


	
	// Now we have to convert it to 32bit and use it as our member bitmap
	
	// first get a pointer to the bitmap bits
	INT32	BitmapSize = ViewHeight * ViewWidth * ScreenDepth;
	
	LPBYTE lpScreenBits = NULL; //(LPBYTE)CCMalloc(BitmapSize);//(LPBYTE)malloc(BitmapSize);
	
	LPBITMAPINFO lpScreenInfo = GetLPBits(Rect.right-Rect.left, Rect.bottom-Rect.top,
								ScreenDepth, &lpScreenBits);
	if (lpScreenBits == NULL)
	{
		ERROR3("error getting screen bits");
		DeleteDC(hDCScreen);
		DeleteDC(hDCCompatible);
		DeleteObject(hBMScreen);
		return FALSE;
	}
	
	// now setup a bitmapinfo
	//LPBITMAPINFO lpScreenInfo = (LPBITMAPINFO)CCMalloc(sizeof(BITMAPINFO));
	if (lpScreenInfo == NULL)
	{
		ERROR3("error getting bitmapinfo");
		CCFree((LPVOID)lpScreenBits);
		DeleteDC(hDCScreen);
		DeleteDC(hDCCompatible);
		DeleteObject(hBMScreen);
		return FALSE;
	}
	
	INT32 Colours = 0;
	
	switch (ScreenDepth)
	{
		case 1:
			Colours = 2;
			break;
		case 4:
			Colours = 16;
			break;
		case 8:
			Colours = 256;
			break;
		case 16:  // for 16 bit and upwards we can pass in zero
			Colours = 0;
			break;
		case 24:
			Colours = 0;
			break;
		case 32:
			Colours = 0;
			break;
		default:
			ERROR3("Illegal screen depth");
			break;
	}
	

	lpScreenInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	lpScreenInfo->bmiHeader.biWidth			= ViewWidth;
	lpScreenInfo->bmiHeader.biHeight		= ViewHeight;
	lpScreenInfo->bmiHeader.biPlanes		= 1;
	lpScreenInfo->bmiHeader.biBitCount		= ScreenDepth;
	lpScreenInfo->bmiHeader.biCompression	= BI_RGB;
	lpScreenInfo->bmiHeader.biXPelsPerMeter = 3780;				// Default to 96 dpi
	lpScreenInfo->bmiHeader.biYPelsPerMeter = 3780;
	lpScreenInfo->bmiHeader.biClrUsed		= Colours;
	lpScreenInfo->bmiHeader.biClrImportant	= 0;
	lpScreenInfo->bmiHeader.biSizeImage 	= BitmapSize;
	
	
	

	// now get the DIBits
	 INT32 num =  (GetDIBits(hDCCompatible, hBMScreen,
					0, ViewHeight, 
					lpScreenBits, lpScreenInfo,
					DIB_RGB_COLORS)); 
	//if (!GetBits(hDCCompatible, hBMScreen, ViewHeight, lpScreenBits, lpScreenInfo))
	if (num ==0 )
	{
		ERROR3("Failed to get bitmap bits");
		DeleteDC(hDCScreen);
		DeleteDC(hDCCompatible);
		DeleteObject(hBMScreen);
		delete lpScreenInfo;
		CCFree((LPVOID)lpScreenBits);
		return FALSE;
	}

	// quick sanity check
	if (lpBitmapInfo == NULL)
		ERROR3("What the *?!& is going on with my bitmaps!?");

	GDrawContext *GDC = GetDrawContext();
	// convert it to 32bit, using the already allocated bitmap and bitmapinfo
	INT32 result = 0;
	if (ScreenDepth == 8)
	{
		SetPaletteEntries(lpScreenInfo);
		
		/*result = DIBUtil::Convert8to32(lpScreenInfo, lpScreenBits, lpScreenInfo->bmiColors,
							  lpBitmapInfo, lpBits);
		
		INT32 result = GDC->ConvertBitmap(&(lpBitmapInfo->bmiHeader), lpBits,
								&(lpScreenInfo->bmiHeader), lpScreenBits, DitherStyle8Bit);
		
		 */
		/*
		GDC->SetChangedBBox(&Rect);
		BitmapDepth = 8;
		DisplayBits();
		BitmapDepth = 32;
		*/
	/*	INT32 res = SetDIBitsToDevice( hDCScreen,
									0, Rect.top,
									ViewWidth,
									ViewHeight,
									0, 0,				// lower left of DIB (upside down)
									0, ViewHeight,						// actual bitmap height in entirety
									lpScreenBits,
									lpScreenInfo,
									DIB_RGB_COLORS);
	*/	
		result = DIBUtil::Convert8to32(lpScreenInfo, lpScreenBits, lpScreenInfo->bmiColors,
							  lpBitmapInfo, lpBits);
	

		
	
	}
	if (ScreenDepth == 16)
	{
		RGBQUAD* pPalette = lpScreenInfo->bmiColors;
		result = Convert16to32(lpScreenInfo, lpScreenBits, pPalette,
							  lpBitmapInfo, lpBits);
		SetupMainBitmap();
	
		/* Test code for figuring out the bitmap shearing problem
		GDrawContext *GDC = GetDrawContext();
		
		INT32 ColourFlag = DIB_RGB_COLORS;

		INT32 result = GDC->ConvertBitmap(&(lpBitmapInfo->bmiHeader), lpBits,
								&(lpScreenInfo->bmiHeader), lpScreenBits, DitherStyle8Bit);
		
		PatBlt( hDCScreen, 0, Rect.top, ViewWidth, ViewHeight, BLACKNESS );
		// blit the post-converted bitmap
		INT32 res = SetDIBitsToDevice( hDCScreen,
									0, Rect.top,
									ViewWidth,
									ViewHeight,
									0, 0,				// lower left of DIB (upside down)
									0, ViewHeight,						// actual bitmap height in entirety
									lpScreenBits,
									lpScreenInfo,
									DIB_RGB_COLORS);
		TRACEUSER( "Diccon", _T("Blitted post-converted bitmap\n")); */
	}
	if (ScreenDepth == 24)
	{
		result = GDC->ConvertBitmap(&(lpScreenInfo->bmiHeader), lpScreenBits,
							&(lpBitmapInfo->bmiHeader), lpBits, DitherStyle8Bit);
	}
	if (ScreenDepth == 32)
	{
		// blit the post-converted bitmap
	/*	INT32 res = SetDIBitsToDevice( hDCScreen,
									0, Rect.top,
									ViewWidth,
									ViewHeight,
									0, 0,				// lower left of DIB (upside down)
									0, ViewHeight,						// actual bitmap height in entirety
									lpScreenBits,
									lpScreenInfo,
									DIB_RGB_COLORS);
		*/
		FreeLPBits(lpBitmapInfo, lpBits);
		lpBitmapInfo = lpScreenInfo;
		lpBits       = lpScreenBits;
		lpScreenInfo = NULL;
		lpScreenBits = NULL;

	}

	// Update the master capture
	Capture* pMasterCapture = GetMasterCapture();
	pMasterCapture->lpBitmapInfo = lpBitmapInfo;
	pMasterCapture->lpBits = lpBits;

	// free up all that lovely memory and stuff
	DeleteDC(hDCScreen);
	DeleteDC(hDCCompatible);
	DeleteObject(hBMScreen);
	if (lpScreenBits != NULL)
		FreeLPBits(lpScreenInfo, lpScreenBits);
	lpScreenInfo	= NULL;
	lpScreenBits    = NULL;
	if (hPrevPalette)
			PaletteManager::StopPaintPalette(hDCScreen, hPrevPalette);
	if (result == -1)
	{
		ERROR3("Failed to convert bitmap");
		return FALSE;
	}
	else	
		return TRUE;
}

// these fns.just perform their win32 namesakes, moved out of the main fn. so 
// that they could be profiled
BOOL GRenderBrush::GetBits(HDC hMemDC, HBITMAP hBitmap, INT32 Height, LPBYTE lpbits, LPBITMAPINFO lpInfo)
{
// now get the DIBits
	INT32 num =  (GetDIBits(hMemDC, hBitmap,
					0, Height, 
					lpBits, lpInfo,
					DIB_RGB_COLORS));	
	return (num != 0);
}

BOOL GRenderBrush::Blit(HDC DestDC, HDC SourceDC, INT32 Height, INT32 Width, INT32 Left, INT32 Top)
{
	if (BitBlt(DestDC, 
				0, 0,
				Width, Height,
				SourceDC,
				Left , Top,
				SRCCOPY))
		return TRUE;
	else
		return FALSE;
	
}



/********************************************************************************************

>	BOOL GRenderBrush::CaptureViewByRendering()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Renders the view into this render region
	
	Notes:  Due to the ongoing palette problems capturing the view from the screen in 8-bit mode
			this will serve to capture the background for the time being. Obviously it is not ideal
			as rendering the view can be very slow.


********************************************************************************************/

BOOL GRenderBrush::CaptureViewByRendering()
{
	// first things first, we need the view, docview and spread
	DocView* pDocView = DocView::GetCurrent();
	View* pView = View::GetCurrent();
	Spread* pSpread = Document::GetSelectedSpread();
	Document* pDoc = Document::GetCurrent();
	if (pSpread == NULL || pDocView == NULL || pView == NULL || pDoc == NULL)
	{
		ERROR3("Invalid entry conditions to GRenderBrush::CaptureViewByRendering");
		return FALSE;
	}

	ERROR3IF(!MasterCaptureIsCurrent() && GetCaptureDepth()>0, "Can't 'CaptureView' while Captures are running");

	// Convert our window rect to doccoords because the concurrent render needs it	

	WinRect WinRect(WRect.left, WRect.top, WRect.right, WRect.bottom);
	OilRect ORect = WinRect.ToOil(pView);
	
	DocRect ViewRect = ORect.ToDoc(pSpread, pView);
	
	pSpread->DocCoordToSpreadCoord(&ViewRect);
	
	
	// Setup a concurrent renderer.
	ConcurrentRenderer* pRendWrap = NULL;

	// The last argument to this call is a little odd, basically for some reason under a
	// 16bpp display the background paper is rendered incorrectly if our bitmap
	// gets initialised in GRenderRegion::InitBmpBits.  Therefore we ask for it to 
	// not be transparent, (although it is actually transparent, being 32 bit).
	pRendWrap = GRenderRegionWrapper::GetConcurrentRenderer(this,
															1.0,
															ViewRect,
															32,
															FALSE);
	if (pRendWrap == NULL)
		return FALSE;

	GRenderRegion* pOffscreenRR = pRendWrap->GetRenderRegion();
	
	// we're saving the context for paranoia's sake, and 'cos it doesn't hurt.
	pOffscreenRR->SaveContext();

	// ConcurrentRenderer created, now we need to render the entire view into it
	pDocView->RenderEntireView(pOffscreenRR, pSpread);

	// restore the context.
	pOffscreenRR->RestoreContext();


	// extract the bitmap from the offscreen RR before the RR is closed down, 
	// this is somewhat labourious
	KernelBitmap *	pBitmap = pRendWrap->GetKernelBitmap();

	if (pBitmap == NULL)
	{
		ERROR3("Bitmap is NULL in GRenderBrush::CaptureViewByRendering");
		delete pOffscreenRR;
		return FALSE;
	}
	WinBitmap* pWinBitmap = (WinBitmap*)pBitmap->GetActualBitmap();
	if (pWinBitmap == NULL)
	{
		ERROR3("Bitmap is NULL in GRenderBrush::CaptureViewByRendering");
		delete pOffscreenRR;
		delete pBitmap;
		return FALSE;
	}
	LPBYTE pBits = NULL; //(LPBYTE)pWinBitmap->GetBitmapBits();
	LPBITMAPINFO pBitmapInfo = NULL; //pWinBitmap->GetBitmapInfo();
	pWinBitmap->ExtractBitsAndInfo(&pBits, &pBitmapInfo);

	if (pBits == NULL || pBitmapInfo == NULL)
	{
		ERROR3("Bitmap is NULL in GRenderBrush::CaptureViewByRendering");
		delete pOffscreenRR;
		delete pBitmap;
		delete pWinBitmap;
		return FALSE;
	}	

	// now delete our existing bitmap and assign the new on
	if (lpBitmapInfo!=NULL && lpBits != NULL)
	{
		FreeLPBits(lpBitmapInfo, lpBits);
		lpBitmapInfo = NULL;
		lpBits       = NULL;
	}
	lpBits = pBits;
	lpBitmapInfo = pBitmapInfo;

	// Update the master capture
	Capture* pMasterCapture = GetMasterCapture();
	pMasterCapture->lpBitmapInfo = lpBitmapInfo;
	pMasterCapture->lpBits = lpBits;

	// delete our bitmap container
	delete pBitmap;

	// restore gdraw 
	pRendWrap->RestorePreviousRendererState();
	

	return TRUE;
}

/********************************************************************************************

>	void GRenderBrush::FreeLPBits( LPBITMAPINFO lpBMI, LPBYTE lpB )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/94
	Inputs:		Bitmap characteristics.
	Outputs:	-
	Returns:	-
	Purpose:	Frees up a bitmap allocated with GetLPBits.
	Errors:		-
	SeeAlso:	GRenderDIB::FreeLPBits

********************************************************************************************/

void GRenderBrush::FreeLPBits( LPBITMAPINFO lpBMI, LPBYTE lpB )
{
	// If we are banded, then free the memory from the limited heap
	FreeDIB(lpBMI, lpB, NULL, FALSE);
}


/********************************************************************************************

>	static BOOL DIBUtil::Convert8to32(LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits, LPQUAD pPalette
									  LPBITMAPINFO *pDestInfo, LPBYTE *pDestBits)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/2000
	Inputs:		pSourceInfo 	- A information header for the bitmap to be copied
				pSourceBits		- the actual bits for the bitmap to be copied
				pPalette		- pointer to the palette 
				pDestInfo		- pointer to an ALLOCATED bitmapinfo structure 
				pDestBits		- pointer to an ALLOCATED 32bit bitmap

	Outputs:	pDestInfo		- pointer to the copied information block
				pDestBits		- pointer ot the copied bits data
				
	
	Returns: 	True if copied ok, false otherwise
	Purpose:	Generates a 32bit form of the input 8bit bitmap, note that the destination 
				bitmapinfo and bitmap must be allocated prior to this function

********************************************************************************************/

#define	EXPAND(x)	( (x<<3) | (x>>2) )

BOOL GRenderBrush::Convert16to32(LPBITMAPINFO pSourceInfo, LPBYTE pSourceBits, RGBQUAD* pPalette,
									  LPBITMAPINFO pDestInfo, LPBYTE pDestBits)
{
	ERROR2IF(pSourceBits == NULL, FALSE, "NULL source bitmap");
	ERROR2IF(pSourceInfo == NULL, FALSE, "NULL source info");
	ERROR2IF(pDestInfo == NULL, FALSE, "NULL destination info");
	ERROR2IF(pDestBits == NULL, FALSE, "Null destination bitmap");
	
	// most info will be the same so memcpy is the quickest way
	//memcpy(pSourceInfo, pDestInfo, sizeof(BITMAPINFO));

	// these will all be the same
	pDestInfo->bmiHeader.biSize			 = sizeof(BITMAPINFOHEADER);
	pDestInfo->bmiHeader.biWidth		 = pSourceInfo->bmiHeader.biWidth;
	pDestInfo->bmiHeader.biHeight		 = pSourceInfo->bmiHeader.biHeight;
	pDestInfo->bmiHeader.biPlanes		 = pSourceInfo->bmiHeader.biPlanes;
	pDestInfo->bmiHeader.biCompression	 = pSourceInfo->bmiHeader.biCompression;
	pDestInfo->bmiHeader.biXPelsPerMeter = pSourceInfo->bmiHeader.biXPelsPerMeter;
	pDestInfo->bmiHeader.biYPelsPerMeter = pSourceInfo->bmiHeader.biYPelsPerMeter;
	pDestInfo->bmiHeader.biClrImportant	 = 0;

	// get the size of the new bitmap
	INT32 size = pDestInfo->bmiHeader.biWidth *  pDestInfo->bmiHeader.biHeight * 4;
	
	//these will be different from source
	pDestInfo->bmiHeader.biBitCount         = 32;
	pDestInfo->bmiHeader.biSizeImage        = size;
	pDestInfo->bmiHeader.biClrUsed          = 0;


	// convert from 16 bit to 32 bit
	INT32 bmpsize = pSourceInfo->bmiHeader.biWidth * pSourceInfo->bmiHeader.biHeight;
	
	// we need some padding just in case we are not DWORD aligned
	INT32 padding = pSourceInfo->bmiHeader.biWidth % 2;
	if (padding)
		padding = 2 - padding;


	LPBYTE pSourceEnd = pSourceBits;
	LPBYTE pDestEnd = pDestBits;

	for (INT32 j = 0; j < bmpsize; j++)
	{
		pSourceEnd++;
		pDestEnd++;
	}

	LPWORD pSource = (LPWORD)pSourceBits;
	LPBYTE pDest = pDestBits;
	for (INT32 i = 0; i < bmpsize; i ++)
	{
		if (i && i % pSourceInfo->bmiHeader.biWidth == 0)
			pSource += padding;
		
		const WORD Data = *pSource;
		
		const BYTE Blue = Data & 0x1F;
		*pDest++ = EXPAND(Blue);

		const BYTE Green = (Data>>5) & 0x1F;
		*pDest++ = EXPAND(Green);

		const BYTE Red = (Data>>10) & 0x1F;
		*pDest++ = EXPAND(Red);

		*pDest = 0;
		pDest++;
	
	//	*pSource +=1;
		pSource++;
		
		
	
	}
	return TRUE;

}

/********************************************************************************************

>	void GRenderBrush::SetChangedBBox(DocRect Rect)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/99
	Inputs:		Rect - the area to specify as changed
	Outputs:	
	Purpose:	For some reason when drawing brush steps into a render region there was a problem
				with having the bbox set itself, so this is a way to do it manually
				

********************************************************************************************/


void GRenderBrush::SetChangedBBox(DocRect Rect)
{
	RECT WinRect  = OSRenderRegion::DocRectToWin(RenderMatrix, Rect, PixelsPerInch);
	GetDrawContext()->SetChangedBBox(&WinRect);
}


