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
// DragBmp.cpp - Bitmap DragInformation base class

/*
*/


#include "camtypes.h"

#include "dragbmp.h"
#include "dragmgr.h"
#include "keypress.h"
#include "camframe.h"
#include "bitmap.h"
#include "bitmpinf.h"	
#include "oilbitmap.h"

#include "grndrgn.h"
#include "gdrawcon.h"
#include "dibutil.h"

#include "gbrush.h"
#include "camelot.h"	// for IsWin32s()
#include "app.h"

#include "sgtree.h"
#include "grndbmp.h"
#include "docview.h"
#include "spread.h"
#include "palman.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(BitmapDragInformation, DragInformation)

#define new CAM_DEBUG_NEW

const INT32 CellSize = 180;
const INT32 NullSize = 112;

// Default to 50% transparent drags
INT32 BitmapDragInformation::DragTransparency = 50;

/********************************************************************************************

>	void BitmapDragInformation::BitmapDragInformation() 
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  

	Purpose:	Conmstructor - do not call this default constructor.

********************************************************************************************/

BitmapDragInformation::BitmapDragInformation()
{
//	ERROR3("Default BitmapDragInformation constructor called");	
}



/********************************************************************************************

>	void BitmapDragInformation::BitmapDragInformation(KernelBitmap * Bitmap,
													  INT32 xSize,
													  INT32 ySize, 
													  INT32 xOffset,
													  INT32 yOffset,
													  BOOL IsAdjust) 
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	
	Inputs:		Bitmap - The KernelBitmap being dragged
				xSize - The Width (in Pixels) of the Drag Rectangle.
				ySize - The Height (in Pixels) of the Drag Rectangle.
				xOffset - The offset (in Pixels) from the mouse to the top of the bitmap.
				yOffset - The offset (in Pixels) from the mouse to the left edge of the bitmap.
				IsAdjust - TRUE if this is an adjust-drag

	Purpose:	Constructor

********************************************************************************************/

BitmapDragInformation::BitmapDragInformation(	KernelBitmap * DragBitmap,
												INT32 xSize,
												INT32 ySize, 
												INT32 xOffset,
												INT32 yOffset,
												BOOL IsAdjust)
	:DragInformation(IsAdjust)
{
	// The Kernel Bitmap we are dragging
	TheBitmap = DragBitmap;

	TempInfo = NULL;
	TempBits = NULL;

	MemDC = NULL;
	Bitmap = NULL;
	DragMask = NULL;
	MaskBitmap = NULL;

	// Set up a few things about this drag
	DoesSolidDrag = TRUE;
	
	// offset of Bitmap swatch from the pointer
	SolidDragOffset.x = 0;
	SolidDragOffset.y = 16;	   
	
	BitmapInfo Info;

	INT32 BitmapWidth  = (TheBitmap != NULL) ? CellSize : NullSize;
	INT32 BitmapHeight = (TheBitmap != NULL) ? CellSize : NullSize;

	// The aspect ratio of the bitmap
	double Ratio = 1.0;

	if (ySize == 0 && 
		TheBitmap != NULL &&
		TheBitmap->ActualBitmap != NULL &&
		TheBitmap->ActualBitmap->GetInfo( &Info ))
	{
		BitmapWidth  = Info.PixelWidth;
		BitmapHeight = Info.PixelHeight;

		// Calculate the aspect ratio of the bitmap
		Ratio = double(Info.RecommendedWidth)/double(Info.RecommendedHeight);
	}

	// If the user supplied the sizes, keep their aspect	
	if(xSize > 0 && ySize > 0)
		Ratio = double(xSize) / double(ySize);

	// size of the solid drag redraw
	SolidDragSize.x = (xSize > 0) ? xSize : BitmapWidth;
	SolidDragSize.y = (ySize > 0) ? ySize : BitmapHeight;

	if (xSize > 0 && ySize == 0)
		SolidDragSize.y = INT32(SolidDragSize.x / Ratio);

	if (SolidDragSize.x > CellSize)
	{
		SolidDragSize.x = CellSize;
		SolidDragSize.y = INT32(SolidDragSize.x / Ratio);
	}

	if (SolidDragSize.y > CellSize)
	{
		SolidDragSize.y = CellSize;
		SolidDragSize.x = INT32(SolidDragSize.y * Ratio);
	}

	// Offset relative to bitmap centre
	SolidDragOffset.x += (xOffset - SolidDragSize.x/2);
	SolidDragOffset.y += (yOffset - SolidDragSize.y/2);	   

	DragRect.x = SolidDragSize.x;
	DragRect.y = SolidDragSize.y;
}

/********************************************************************************************

>	void BitmapDragInformation::~BitmapDragInformation()
 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	
	Purpose:	Destructor

********************************************************************************************/

BitmapDragInformation::~BitmapDragInformation()
{
	if (Bitmap)
	{
//		Bitmap->DeleteObject();
		delete Bitmap;
	}

	if (MaskBitmap)
	{
		delete MaskBitmap;
	}

	if (DragMask)
	{
//		DragMask->ActualBitmap->IsTemp = TRUE;
//		DragMask->ActualBitmap->Detach();
		delete DragMask;
	}
}

/********************************************************************************************

>	virtual UINT32 BitmapDragInformation::ApplyBitmapToSelection()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/95
	Returns:	Apply current drag Bitmap to selection - set current fill/stroke attributes
	Purpose:	Applies the dragged Bitmap to the current selection
   
********************************************************************************************/

void BitmapDragInformation::ApplyBitmapToSelection()
{
}



/********************************************************************************************

>	virtual UINT32 BitmapDragInformation::GetCursorID()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/95
	Returns:	a cursor ID to set during this drag
	Purpose:	Determines what cursor to display during a Bitmap drag
   
********************************************************************************************/

UINT32 BitmapDragInformation::GetCursorID()
{
	return GetCursorID(NULL);
}



/********************************************************************************************

>	virtual BOOL BitmapDragInformation::GetStatusLineText(String_256 * TheText)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/95

	Outputs:	TheText - If it returns TRUE, this is filled in with status line text
	Returns:	Whether String is valid

	Purpose:	provide status line text for this drag
   
********************************************************************************************/

BOOL BitmapDragInformation::GetStatusLineText(String_256 * TheText)
{
	if (TheBitmap == NULL || TheBitmap->ActualBitmap == NULL)
		return FALSE;

	ERROR2IF(TheText==NULL,FALSE,"NULL string in GetStatusLineText()");

	return GetStatusLineText(TheText, NULL);
}

/********************************************************************************************

>	virtual UINT32 BitmapDragInformation::GetCursorID(DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/95
	Returns:	a cursor ID to set during this drag
	Purpose:	Determines what cursor to display during a Bitmap drag
   
********************************************************************************************/

UINT32 BitmapDragInformation::GetCursorID(DragTarget* pDragTarget)
{
	return 0;
}

/********************************************************************************************

>	virtual BOOL BitmapDragInformation::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/95

	Outputs:	TheText - If it returns TRUE, this is filled in with status line text
	Returns:	Whether String is valid

	Purpose:	provide status line text for this drag
   
********************************************************************************************/

BOOL BitmapDragInformation::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)
{
	if (TheBitmap == NULL || TheBitmap->ActualBitmap == NULL)
		return FALSE;

	ERROR2IF(TheText==NULL,FALSE,"NULL string in GetStatusLineText()");

	return FALSE;
}


/********************************************************************************************

>	void BitmapDragInformation::OnClick(INT32 Flags,POINT Point) 
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	Inputs:		
	Outputs:	-
	Returns:	-

	Purpose:	This is called if a drag was attempted but never started because it was a 
				click all along. In the base class, the default action is to make the
				Bitmap bar think that the dragged Bitmap has been clicked.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BitmapDragInformation::OnClick(INT32 Flags, POINT Point)
{
	if(KeyPress::IsAdjustPressed())
		Flags = -1;
}

BOOL BitmapDragInformation::StartGalleryItemDrag(SGDisplayItem* pGalleryItem)
{
/*
//	pGalleryItem->CalculateMyRect(&FormatInfo, &MiscInfo);

	DocRect ClipRegion(0,0, 100*750,100*750);

	Matrix ConvertMatrix;
	FIXED16 ViewScale = 1;

	CDC* pDisplayDC = new CDC();
	pDisplayDC->CreateDC("DISPLAY", NULL, NULL, NULL); 

	UINT32 Depth = GetDeviceCaps( pDisplayDC->m_hDC, BITSPIXEL ) * 
									GetDeviceCaps( pDisplayDC->m_hDC, PLANES );

	INT32 dpi   = GetDeviceCaps( pDisplayDC->m_hDC, LOGPIXELSX );

	GRenderBitmap* pRegion 		= new GRenderBitmap(ClipRegion, ConvertMatrix, ViewScale, 
												Depth, dpi);

	GRenderBitmap* pMaskRegion 	= new GRenderBitmap(ClipRegion, ConvertMatrix, ViewScale, 
													1, dpi);

	DocView *View = DocView::GetCurrent();
	Spread *pSpread = View->FindEnclosingSpread(OilCoord(0,0));

	pRegion->AttachDevice(View, pDisplayDC, pSpread);
	pMaskRegion->AttachDevice(View, pDisplayDC, pSpread);

	// Make a Mask Bitmap
	pMaskRegion->StartRender();
  	((SGDisplayLineAttrItem*)pGalleryItem)->Render(pMaskRegion, ClipRegion);

	// Make the Drag Bitmap
	pRegion->StartRender();
  	((SGDisplayLineAttrItem*)pGalleryItem)->Render(pRegion, ClipRegion);

	OILBitmap* pOilMaskBmp = pMaskRegion->ExtractBitmap();
	DragMask = new KernelBitmap(pOilMaskBmp);	

	OILBitmap* pOilDragBmp = pRegion->ExtractBitmap();
	TheBitmap = new KernelBitmap(pOilDragBmp);	

	delete pMaskRegion;
	delete pRegion;
	delete pDisplayDC;

*/
	return TRUE;
}

/********************************************************************************************

>	void BitmapDragInformation::OnDrawSolidDrag(wxPoint Origin, wxDC * TheDC, DragTarget* pDragTarget) 
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	Inputs:		TheDC - pointer to a DC ready to draw into.
	Outputs:	-
	Returns:	-
	Purpose:	allows the drag to draw into a Device Context - DragManager will look after
				painting the background	etc.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BitmapDragInformation::OnDrawSolidDrag(wxPoint Origin, wxDC * TheDC, DragTarget* pDragTarget)
{
	return OnDrawSolidDrag(Origin, TheDC);
}

/********************************************************************************************

>	void BitmapDragInformation::OnDrawSolidDrag(wxPoint Origin, wxDC* TheDC) 
	 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	Inputs:		TheDC - pointer to a DC ready to draw into.
	Outputs:	-
	Returns:	-
	Purpose:	allows the drag to draw into a Device Context - DragManager will look after
				painting the background	etc.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BitmapDragInformation::OnDrawSolidDrag(wxPoint Origin, wxDC* TheDC)
{
//	CPalette * OldPal = NULL;

//	if (PaletteManager::UsePalette())
//	{
//		OldPal = PaletteManager::StartPaintPalette( TheDC);
//	}

	if (TheBitmap != NULL && TheBitmap->ActualBitmap != NULL)
	{	
		PlotBitmap((CWxBitmap*)TheBitmap->ActualBitmap, Origin, DragRect, TheDC);
	}
	else
	{
		// get a gbrush brush
		GBrush GDrawBrush;
		GDrawBrush.Init(TheDC);
		GDrawBrush.Start();

		DWORD ScreenWord = 0x007F7F7F;

		wxBrush *pDragBrush = NULL;

		pDragBrush = new wxBrush;
	
		BOOL BrushCreate = TRUE;

		if (pDragBrush != NULL )
		{
			// create the Brush and	Pen
			if (GDrawBrush.Available())
			{
				GDrawBrush.GetLogBrush((COLORREF)(ScreenWord), pDragBrush);
			}
			else
				*pDragBrush=wxBrush(wxColour(0x7F,0x7F,0x7F));
			
			wxPen MyPen(*wxBLACK);
		
			//the rectangle to draw into
			wxRect DrawRect;

			if(DragRect.GetWidth() == 0 || DragRect.GetHeight() == 0)
				DrawRect = wxRect(Origin.x,Origin.y, NullSize, NullSize);
			else
				DrawRect = wxRect(Origin.x,Origin.y, DragRect.GetWidth(), DragRect.GetHeight());

			// select brushes and pens ..
			TheDC->SetBrush(*pDragBrush);
			TheDC->SetPen(MyPen);

			// draw an ellipse
	 		// TheDC->Ellipse(&DrawRect);

			// draw some text
			// String_256 Str("Blobby");
			// TheDC->TextOut(Origin.x, Origin.y, (TCHAR *)Str, Str.Length());		

			// draw a rectangle 			 		
	 		TheDC->DrawRectangle(DrawRect);

			TheDC->SetBrush(*wxTRANSPARENT_BRUSH);
			TheDC->SetPen(*wxTRANSPARENT_PEN);

			delete pDragBrush;
		}
		else
			BrushCreate = FALSE;

		// Finish with GBrush
		GDrawBrush.Stop();
	}

//	if (OldPal)
//		PaletteManager::StopPaintPalette(TheDC, OldPal);
	
	return TRUE;
}


/********************************************************************************************

>	BOOL BitmapDragInformation::PlotBitmap(	CWxBitmap *wxBM, 
											CPoint Origin, CSize Size, 
											CDC * RenderDC)
 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  
	Inputs:		RenderDC - pointer to a DC ready to draw into.
	Outputs:	-
	Returns:	FALSE, if the plot failed.
	Purpose:	Plots the specified Bitmap into the DC, during a solid drag.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BitmapDragInformation::PlotBitmap(	CWxBitmap *wxBM, 
										wxPoint Origin, wxSize Size, 
										wxDC* RenderDC)
{
	if ((wxBM == NULL) ||
		(wxBM->BMInfo==NULL) ||
		(wxBM->BMBytes==NULL)
	   )
		return FALSE;

	wxPoint TopLeft  = Origin;

	wxPoint BottomLeft;
	BottomLeft.x = TopLeft.x;
	BottomLeft.y = TopLeft.y + Size.y;

	wxPoint BottomRight;
	BottomRight.x = TopLeft.x + Size.x;
	BottomRight.y = TopLeft.y + Size.y;

	INT32 DestWidth  = BottomRight.x - BottomLeft.x;
	INT32 DestHeight = BottomLeft.y  - TopLeft.y;

// This has been commented out because the hPal is only used inside a comment
// If it is needed then just use *(pPal) and let MFC sort it out...

	// get the HPALETTE to pass to the plot bitmap call - crucial for quality Win32s DDB plotting
	// uses sneaky HPALETTE operator
//	HPALETTE hPal = NULL;
//	if (PaletteManager::UsePalette())
//		hPal = *(PaletteManager::GetPalette());

	wxMemoryDC MemDC;

	// Get the palette to use (will be NULL if palette not needed)
	wxPalette* pPal = PaletteManager::GetPalette();

	// If this is the first time, then we need to create the Cached Bitmap
	if (Bitmap == NULL)
	{
		Bitmap = new wxBitmap(DestWidth, DestHeight);
		if (Bitmap == NULL)
		{
			return FALSE;
		}

		INT32 bpp = wxBM->GetBPP();

		if (MaskBitmap)
		{
			delete MaskBitmap;
			MaskBitmap = NULL;
		}

		// For now, only do the mask bitmap where we have a 32bpp representation
		// as I haven't tested whether Gavin works OK with (say) a palette with
		// transparency in it
		if ((bpp==32) && TheBitmap->IsTransparent())
		{
			// If this fails, we simply treat it as non-transparent. Hey ho
			MaskBitmap = new wxBitmap(DestWidth, DestHeight);
		}

		MemDC.SelectObject(*Bitmap);

PORTNOTE("other", "disabled palettes in dragbmp")
#ifndef EXCLUDE_FROM_XARALX
		if (PaletteManager::UsePalette())
 			MemDC.SelectPalette(pPal, FALSE);
#endif

		// Since the GDI cannot cope with anything other than very simple bitmaps,
		// we will get GDraw to render the bitmap into a screen compatible DIB, and
		// then get the GDI to plot that

		GDrawContext* GD = GRenderRegion::GetStaticDrawContext();

		// Setup a default matrix (we'll do everything in pixel-speak)

		GMATRIX GMatrix;

		GMatrix.AX = 1<<(16 + FX);
		GMatrix.AY = 0;
		GMatrix.BX = 0;
		GMatrix.BY = 1<<(16 + FX);

		GMatrix.CX = 0;
		GMatrix.CY = 0;

	// Create a Tempory bitmap compatible with the output device

		INT32 DeviceDepth;

PORTNOTE("other", "Assume 24bpp intermediate bitmap in DragBmp");
#ifndef EXCLUDE_FROM_XARALX
		DeviceDepth = GetDeviceCaps( RenderDC->m_hDC, BITSPIXEL ) * 
										GetDeviceCaps( RenderDC->m_hDC, PLANES );
#else
		DeviceDepth = 24;
#endif

		if (DeviceDepth == 24)
			DeviceDepth = 32;	// GDraw cannot plot to 24-bit bitmaps

		TempInfo = AllocDIB(DestWidth, DestHeight, DeviceDepth, &TempBits);
		if (TempInfo==NULL)
		{
			TRACEALL( _T("Out of memory during BitmapDragInformation::PlotBitmap"));
			return FALSE;
		}

#ifdef _DEBUG
		*((DWORD *)(void *)(TempBits))=0xDEADBEEF;
#endif

		// We may need to get a palette for the DIB.

		DIBPal = DIB_RGB_COLORS;

		if (DeviceDepth <= 8)	// We only need a palette for 256 colours or less.
		{
			DIBPal = GRenderRegion::SetPaletteEntries( TempInfo, RenderDC );
		}

	// Setup GDraw with our Tempory Bitmap and Identity Matrix

		GD->SetupBitmap(TempInfo->bmiHeader.biWidth,
						TempInfo->bmiHeader.biHeight,
						TempInfo->bmiHeader.biBitCount, 
						TempBits );

		GD->SetMatrix( &GMatrix );

		COLORREF DefaultColour = 0xFFFFFFFF;

		RGBQUAD TempPalette[256];
		RGBQUAD* Palette = wxBM->BMInfo->bmiColors;
	
		if (bpp <= 8)
		{
			INT32 NumCols;
			switch (bpp)
			{
				case 1:
					NumCols = 2;
					break;

				case 2:
					NumCols = 4;
					break;

				case 4:
					NumCols = 16;
					break;

				case 8:
					NumCols = 256;
					break;

				default:
					NumCols = 256;
					break;
			}

			for (INT32 i=0; i<NumCols; i++)
			{
				if (Palette[i].rgbReserved == 0xFF)
				{
					// We'll use a copy of the palette ...
					memcpy(TempPalette, Palette, NumCols*sizeof(RGBQUAD));

					// so we can force this entry to be white
					TempPalette[i].rgbRed   	= 0xFF;
					TempPalette[i].rgbGreen 	= 0xFF;
					TempPalette[i].rgbBlue  	= 0xFF;

					Palette = TempPalette;
					break;
				}
			}
		}

	// Now set the bitmap fill

		POINT PGram[3];
		PGram[0].x = 0;				PGram[0].y = 0;
		PGram[1].x = DestWidth;		PGram[1].y = 0;
		PGram[2].x = 0;				PGram[2].y = DestHeight;

		GD->SetContone(0);
		GD->SetBias(3, 0.0);
		GD->SetGain(3, 0.0);
		GD->SetTileSmoothingFlag(TRUE/*FALSE*/);
		GD->SetTileFilteringFlag(TRUE/*FALSE*/);


		GD->SetBitmapFill(	&(wxBM->BMInfo->bmiHeader),
							wxBM->BMBytes,
							1,
							PGram,
							DefaultColour,
							Palette,
							NULL, NULL, NULL,
							NULL,
							0
							);

	// Now plot a filled rectangle

		RECT BmpRect;
		BmpRect.left 	=  0;
		BmpRect.top 	=  DestHeight;
		BmpRect.right 	=  DestWidth;
		BmpRect.bottom 	=  0;

		GD->FillRectangle(&BmpRect);

		// Now we use GRenderRegion to plot it (far easier than farting about
		// with conversion which was the previous technique...)

		GRenderRegion::StaticPlotBitmap(&MemDC, DIBPal, TempInfo, TempBits, 0, 0, DestWidth, DestHeight, pPal, 0, 0);

		if (MaskBitmap)
		{
			// Make the rectangle black
			MemDC.SelectObject(*MaskBitmap);
			GD->SetColour(0);
			GD->FillRectangle(&BmpRect);

			// Contone between white and white, but use the transparency values
			GD->SetColour(0xFFFFFF);
			GD->SetContone(1, 0xFFFFFF, 0xFFFFFF);
			GD->SetBias(3, 0.0);
			GD->SetGain(3, 0.0);
			GD->SetTileSmoothingFlag(TRUE/*FALSE*/);
			GD->SetTileFilteringFlag(TRUE/*FALSE*/);
			GD->SetBitmapFill(	&(wxBM->BMInfo->bmiHeader),
								wxBM->BMBytes,
								0x4000,
								PGram,
								DefaultColour,
								Palette,
								NULL, NULL, NULL,
								NULL,
								0
								);
			GD->FillRectangle(&BmpRect);

			GRenderRegion::StaticPlotBitmap(&MemDC, DIBPal, TempInfo, TempBits, 0, 0, DestWidth, DestHeight, pPal, 0, 0);
		}

		FreeDIB(TempInfo, TempBits);

	}
	// Finally plot the Bitmap onto the output RenderDC
	if (RenderDC)
		RenderDC->DrawBitmap(*Bitmap, TopLeft.x, TopLeft.y, true);

	// Yipeee !! We done it
   	return TRUE;
}

/********************************************************************************************

>	KernelBitmap* BitmapDragInformation::GetSolidDragMask()
 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95		  
	Returns:	-
	Purpose:	Get a 1 bpp mask to use for the solid drag.
	SeeAlso:	-

We (ab)use this call to ensure an initial plot is done, which in turn sets up the mask,
which has to be done before the first plot (on setup)

********************************************************************************************/

KernelBitmap* BitmapDragInformation::GetSolidDragMask()
{
	if (TheBitmap != NULL && TheBitmap->ActualBitmap != NULL)
	{	
		wxBitmap bitmap(1,1);
		wxMemoryDC DC;
		DC.SelectObject(bitmap);
		PlotBitmap((CWxBitmap*)TheBitmap->ActualBitmap, wxPoint(0,0), DragRect, &DC);
	}
	
	return DragMask;
}

/********************************************************************************************

>	INT32 BitmapDragInformation::GetDragTransparency()
 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  
	Returns:	-
	Purpose:	Specifies how transparent a drag should be.
				A value of 0, will cause a normal solid blit.
				Any other value (between 0 and 100), will cause a masked blit.
	SeeAlso:	-

********************************************************************************************/

INT32 BitmapDragInformation::GetDragTransparency()
{
	return BitmapDragInformation::DragTransparency;
}

/********************************************************************************************

>	static BOOL BitmapDragInformation::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Returns:	-
	Purpose:	Initialise any Bitmap Drag stuff. (eg. ini file preferences).
	SeeAlso:	-

********************************************************************************************/

BOOL BitmapDragInformation::Init()
{
	// Setup preference for drag transparency
	if (GetApplication()->DeclareSection(_T("Dragging"), 1))
	{
		GetApplication()->DeclarePref( NULL, _T("BitmapDragTransparency"), 
								&DragTransparency, 0, 100);
	}

	return TRUE;
}

