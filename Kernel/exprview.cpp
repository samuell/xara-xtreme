// $Id: exprview.cpp 662 2006-03-14 21:31:49Z alex $
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

// ExpressView - subclass of View for direct rendering to DC


#include "camtypes.h"
#include "exprview.h"

#include "view.h"
#include "vstate.h"
#include "errors.h"
#include "camelot.h"
#include "ccdc.h"
#include "document.h"
#include "rndrgn.h"
#include "spread.h"
#include "node.h"
#include "app.h"
#include "grndbmp.h"
#include "bitmap.h"
#include "statline.h"


DECLARE_SOURCE("$Revision: 662 $");


// Place any IMPLEMENT type statements here
CC_IMPLEMENT_DYNAMIC(ExpressView, View)


#define new CAM_DEBUG_NEW

// Functions follow


/********************************************************************************************

>	ExpressView::ExpressView(Document *pOwnerDoc)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Tim)
	Created:	18/09/96
	Inputs:		pDocument - the document to attach to (i.e. the one to print)
	Purpose:	First stage of two part constructor
	SeeAlso:	ExpressView::Init()

********************************************************************************************/
ExpressView::ExpressView(Document *pOwnerDoc)
{
	// Set up document; no CamView yet.
	pDoc		= pOwnerDoc;
	pViewWindow = NULL;

	m_pBitmapRegion	= NULL;
	m_pCDC			= NULL;
	m_pBitmap		= NULL;
	m_Bpp			= 8;

	m_XScale	= 1.0;
	m_YScale	= 1.0;
}



/********************************************************************************************

>	BOOL ExpressView::Init()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/96
	Purpose:	Second stage of two part constructor
	Returns:	TRUE if successful, FALSE otherwise
	SeeAlso:	ExpressView::ExpressView()

********************************************************************************************/
BOOL ExpressView::Init()
{
	pVState = new ViewState;
	if (pVState == NULL)
	{
		TRACE( _T("Unable to create ViewState\n"));
		return FALSE;
	}
	// Connect this view state to this view
	pVState->pView = this;

	Spread* pSpread = pDoc->FindFirstSpread();
	DocView* pView = pDoc->GetFirstDocView();

	// Get the real bounds, in millipoints, of the document's first spread, converted
	// to pixels.
	GetScaledPixelSize(&ScaledPixelWidth, &ScaledPixelHeight);

	return TRUE;
}


/********************************************************************************************

>	~ExpressView::ExpressView()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Tim)
	Created:	18/09/96
	Purpose:	Destroys a printing view - this is because it has finished printing or
				something went wrong during printing.
	SeeAlso:	ExpressView; View

********************************************************************************************/

ExpressView::~ExpressView()
{
	delete pVState;
	pVState = 0;
}



/********************************************************************************************

>	CDC *ExpressView::GetRenderDC()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Tim)
	Created:	18/09/96
	Returns:	The printer DC.
	Purpose:	Access function for the device context of this ExpressView passed in via
				OnDraw()

********************************************************************************************/

CDC* ExpressView::GetRenderDC()
{
	return m_pCDC;
}




/********************************************************************************************

>	void ExpressView::ContinueRenderView(RenderRegion* pRRegion, Spread* pSpread,
										BOOL fRenderPaper = TRUE,
										BOOL fDeleteRegionAfter = TRUE,
										BOOL bForceImmediate = FALSE)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Tim)
	Created:	18/09/96
	Inputs:		pRRegion - the region to use to render the view.
				pSpread - the spread that is being rendered.
				fRenderPaper --- by default TRUE
				fDeleteRegionAfter --- if TRUE then the View will deallocate the
				render-region when it has finished rendering.  If FALSE then it
				won't.  By default TRUE.
	Purpose:	Used by RenderRegion::DefaultRender() to carry on rendering a view.
				In this case (i.e. printing) it only gets called once, as it calls
				RenderView which prints the view in one go, i.e. we don't do
				background rendering.

********************************************************************************************/

void ExpressView::ContinueRenderView(RenderRegion* pRRegion, Spread* pSpread,
								   BOOL fRenderPaper, BOOL fDeleteRegionAfter,
									 BOOL bForceImmediate)
{
	// Render the document in one operation.
	Matrix mxRndr = ConstructRenderingMatrix(pSpread);

	// Scroll offsets etc may have changed, so transfer matrix.
	pRRegion->SetMatrix(mxRndr);

#ifdef RALPH
	// Wait until it's safe to enter
	CCamApp::EnterGDrawCriticalSection();
#endif

	// Tell the RenderRegion that I am going to start rendering.
	if (!pRRegion->StartRender())
	{
		TRACEALL( _T("StartRender failed in RenderView\n"));
															// will be deleted quickly
	}
	else
	{
		// Never render the paper but do all the ink (non-paper) parts of the document.
		pRRegion->RenderTree(pSpread, FALSE);

		// Reset state to new posn in tree.
		pRRegion->StopRender();

	}
	// Our render regions are always Bitmap ones
	// Assume there's only one
	GRenderBitmap* pGRBRegion = (GRenderBitmap*)pRRegion;
	ERROR3IF(!pGRBRegion->IS_KIND_OF(GRenderBitmap), "pGRBRegion isn't kinda GRenderBitmap");
	m_pBitmap = pGRBRegion->ExtractBitmapCopy();

#ifdef RALPH
	// Let another process have a go
	CCamApp::ExitGDrawCriticalSection();
#endif


	if (fDeleteRegionAfter)
	{
		delete pRRegion;
	}
}


/********************************************************************************************

>	BOOL ExpressView::ViewStateChanged()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Tim)
	Created:	18/09/96
	Returns:	TRUE if all ok;
				FALSE if not.
	Purpose:	Inform the ExpressView that its OIL view object has changed the shared 
				ViewState structure.
	SeeAlso:	View::ViewStateChanged

********************************************************************************************/

BOOL ExpressView::ViewStateChanged()
{
	// Sanity checks.
	ERROR2IF(this==NULL,FALSE,"DocView member func called on NULL pointer");
	ERROR2IF(pDoc==NULL,FALSE,"ViewStateChanged: There MUST be an owner doc for this view!");
	
	DocCoord	ExtentLo;						// coord of bottom-left of logical extent
	DocCoord	ExtentHi;						// coord of top-right of logical extent

	// Read DocCoords extent of document & set extent in platform-indy ViewState struct.
	pDoc->GetExtents(&ExtentLo, &ExtentHi, &PhysExtent, this);

	// Set extent in platform-indy ViewState struct
	SetExtent(ExtentLo, ExtentHi);				

	return TRUE;
}



/********************************************************************************************

>	void ExpressView::SetViewPixelSize()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Tim)
	Created:	18/09/96
	Purpose:	Sets up the normal and scaled pixel sizes according to this view.

********************************************************************************************/

void ExpressView::SetViewPixelSize()
{
	// Set the scaled pixel size
	ScaledPixelWidth = PixelWidth / m_XScale;
	ScaledPixelHeight = PixelHeight / m_YScale;
}



/*********************************************************************************************

> 	void ExpressView::SetForeBackMode(BOOL NewFlag)

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	10/2/95
	Inputs:		NewFlag - New bg render flag.
    Purpose:    Set the background render flag for this view.
	Notes:		Should never be asked to render in background
                   			                                     
*********************************************************************************************/

void ExpressView::SetForeBackMode(BOOL NewFlag)
{
	// Nobody should try to set the printing to be background yet.
	ERROR3IF(NewFlag, "Trying to make a ExpressView background render!");
}



/*********************************************************************************************

> 	BOOL ExpressView::GetForeBackMode()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Tim)
	Created:	18/09/96
    Returns:   	FALSE always, indicating foreground rendering only.
    Purpose:    To find out if background rendering is on
                   			                                     
*********************************************************************************************/

BOOL ExpressView::GetForeBackMode()
{
	// ExpressViews don't background render yet.
	return FALSE;
}


/********************************************************************************************

>	void ExpressView::SetExtent(DocCoord lolog, DocCoord hilog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Phil)
	Created:	18/09/96
	Inputs:		Two DocCoords describing the low and high corners of a rectangle in logical
				space describing the full extent of the document. I.E. the bottom-left corner
				of the last chapter and the top-right corner of the first.
	Purpose:	Informs the ExpressView that the extent of the document has changed in some way and 
				that the view must be altered to take this into account (namely the 
				scrollbars).

********************************************************************************************/

void ExpressView::SetExtent(DocCoord lolog, DocCoord hilog)
{
	// "Pixelise" the extent DocCoords.
	// Effectively, this helps ensure that the spread will be aligned to a whole pixel boundary
	// and allows both GDraw and GDI to consistently plot the same pixels when rendering
	// the same primitive.
	lolog.Pixelise(this);
	hilog.Pixelise(this);

	// Convert the extent given in DocCoord to WorkCoords...
	pVState->WorkAreaExtent.lo = lolog.ToWork(pDoc, this);
	pVState->WorkAreaExtent.hi = hilog.ToWork(pDoc, this);
}






/********************************************************************************************

>	WorkRect ExpressView::GetViewRect()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Phil)
	Created:	18/09/96
	Returns:	A WorkCoords rectangle describing the viewport onto the document which this
				view is displaying. I.E. The top left corner of this rectangle is the same as
				the value returned by GetScrollOffsets().
	Purpose:	To find the rectangle describing the viewport onto the document.

********************************************************************************************/

WorkRect ExpressView::GetViewRect()
{
	ERROR3("GetViewRect() called");
	return WorkRect(0,0,0,0);
}




/********************************************************************************************

>	DocRect ExpressView::GetDocViewRect(Spread* pSpread)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Phil)
	Created:	18/09/96
	Inputs:		pSpread - the spread that we are interested in
	Returns:	A DocCoords rectangle describing the viewport onto the document which this
				view is displaying. I.E. The top left corner of this rectangle is the same as
				the value returned by GetScrollOffsets() except in DocCoords.
	Purpose:	To find the rectangle describing the viewport onto the document.

********************************************************************************************/

DocRect ExpressView::GetDocViewRect(Spread* pSpread)
{
	ERROR3("GetDocViewRect() called");
	return DocRect(0,0,0,0);
}




/********************************************************************************************

>	Matrix View::ConstructRenderingMatrix(Spread *pSpread)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Tim & Rik)
	Created:	18/09/96
	Inputs:		pSpread - the spread that is to be rendered.
	Returns:	The required transformation matrix.
	Purpose:	From the given spread, construct a matrix that will convert spread
				coordinates to OS coordinates.
	SeeAlso:	ExpressView::OnDraw

********************************************************************************************/
Matrix ExpressView::ConstructRenderingMatrix(Spread *pSpread)
{
	ERROR2IF(pSpread == NULL, Matrix(), "pSpread == NULL");

	// Get the PIXELISED origin of spread coordinates, in document coords
	DocCoord SpreadCoordOrigin = pSpread->GetSpreadCoordOrigin(TRUE, this);

	// Convert it into logical Work Coords
	// NOTE: We convert this via the spread, because the Spread Coord Origin can now
	// lie *outside* the current chapter/spread pasteboard bounds, and so the version
	// of this call that takes a document pointer can't find the enclosing chapter!
	// Of course, we know the enclosing chapter, because this point is in a known spread!
	WorkCoord WorkCoordOffset = SpreadCoordOrigin.ToWork(pSpread, this);

	// Offset it by the window scroll position
	WorkCoordOffset.x -= pVState->GetScrollPos().x;
	WorkCoordOffset.y -= pVState->GetScrollPos().y;
	
	// Construct the transformation matrix for the spread.
	Matrix RenderMatrix;

	// We can chop the 64bit values down to 32bit now, as we have them in the correct range
    Matrix TranslateToOrigin( (INT32)WorkCoordOffset.x, (INT32)WorkCoordOffset.y);

    Matrix ScaleMat(Scale, Scale);

	// The following matrix compositions MUST be performed in this order.
	// If you are tempted to optimise this code MAKE SURE THAT THEY ARE STILL
	// IN THIS ORDER WHEN YOU'VE FINISHED!

	// Apply scale factors to convert from millipoint distances to pixel distances...
    RenderMatrix *= ScaleMat;

	// And to the bitmap size
    RenderMatrix *= m_ScaleMatrix;

	// Apply scroll-offset translation to move origin to viewing position...
    RenderMatrix *= TranslateToOrigin;

	return RenderMatrix;
}


/********************************************************************************************

>	void ExpressView::MakeNewRenderRegion(	Spread* pSpread, DocRect ClipRect, 
										CDC* pDevContext, RenderType rType, BOOL PaintPaper, Node* pInvalidNode = NULL)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Rik)
	Created:	18/09/96
	Inputs:		pSpread - The Spread to render
				ClipRect - the Clipping rectangle of the bit to draw
				pDevContext - the Device Context to render to
				rType - the Type of the render region
				PaintPaper - Should we draw the paper?
	Purpose:	Renders the document to the printer. This function will build all the render
				regions needed to do a print.
	SeeAlso:	View::MakeNewRenderRegion

********************************************************************************************/

void ExpressView::MakeNewRenderRegion(	Spread* pSpread, DocRect ClipRect, CDC* pDevContext, 
										RenderType rType, BOOL PaintPaper, Node* pInvalidNode)
{
	// Construct the transformation matrix for the spread.
	Matrix RenderMatrix = ConstructRenderingMatrix(pSpread);

	// Go and create the new render region
	m_pBitmapRegion = NULL;
	double dpi = pDevContext->GetDeviceCaps(LOGPIXELSX);
	// We'll restrict it to 96dpi for now to avoid huge bitmaps
	if (dpi > 96.0)
	{
		TRACE( _T("dpi request for %f\n"), dpi);
		dpi = 96.0;
	}

	m_pBitmapRegion = new GRenderBitmap (ClipRect, RenderMatrix, Scale, m_Bpp, dpi, 
									FALSE, XARADITHER_ERROR_DIFFUSION, 0, TRUE);

	// See if it worked
	if (m_pBitmapRegion == NULL)
	{
		TRACEALL( _T("Not enough memory to create render region\n"));
		return;
	}

	// If the construction was succesful then
	// Attach the new RenderRegion to a Window and its DC for rendering...
	if (!m_pBitmapRegion->AttachDevice(this, pDevContext, pSpread))
	{
		// the AttachDevice failed, so tidy up
		delete m_pBitmapRegion;
		return;
	}

	m_pBitmapRegion->SetBackmostChangedNode(pInvalidNode);
}


/********************************************************************************************

>	BOOL ExpressView::DoRender(CDC* pDC, const DocRect& drClip, const DocRect& BitmapSize, 
								BPP Bpp)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/96
	Returns:	TRUE if completed successfully
				FALSE otherwise
	Purpose:	Renders immediately this ExpressView's bitmap


********************************************************************************************/

BOOL ExpressView::DoRender(CDC* pDC, DocRect drClip, const DocRect& BitmapSize, 
								BPP Bpp)
{
	ERROR2IF(pDC == NULL, FALSE, "pDC == NULL");

	TRACEUSER( "Colin", _T("dpi %d drClip %ld,%ld\tBitmapSize %ld,%ld BPP %d Quality %d\n"), 
				pDC->GetDeviceCaps(LOGPIXELSX),
				drClip.hix, drClip.hiy, 
				BitmapSize.hix, BitmapSize.hiy,
				Bpp,
				RenderQuality.GetQuality());
	// Remember the current view so we can give it back after rendering
	View* pCurrentView = GetCurrent();

	m_pCDC	= pDC;
	m_Bpp	= Bpp;

	// Convert clip rect to OIL coords
	Spread* pSpread = pDoc->FindFirstSpread();
	ERROR2IF(!pSpread, FALSE, "pSpread NULL");

	pSpread->DocCoordToSpreadCoord(&drClip);
	OilRect orClip = drClip.ToOil(pSpread, this);

	// Scale to the bitmap size
	m_ScaleMatrix = Matrix(drClip, BitmapSize);

	// Construct the invalid render regions
	OnDraw(pDC, orClip);

	BOOL bSuccessful = FALSE;
	// And render them into the given DC
	if (m_pBitmapRegion != NULL)
	{
#ifndef RALPH
		// Start the rendering indicator going.
		GetApplication()->GetpStatusLine()->SetRenderIndicator(Rendering);
#endif
		// Render it.
		Error::RenderThreadIn();		// Make errors close window
		m_pBitmapRegion->DefaultRender();
		Error::RenderThreadOut();		// Normal errors

#ifndef RALPH
		// Disable rendering indicator.
		GetApplication()->GetpStatusLine()->SetRenderIndicator(NotRendering);
#endif
		bSuccessful = TRUE;
	}
	// Restore the view
	pCurrentView->SetCurrent();
	return bSuccessful;
}


/********************************************************************************************

>	OILBitmap* ExpressView::GetBitmap() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/96
	Returns:	A pointer to the currently rendered bitmap
	Purpose:	Having called DoRender() to render the document into a bitmap of the given
				specification, we can get a pointer to it here.

********************************************************************************************/
OILBitmap* ExpressView::GetBitmap() const
{
	return m_pBitmap;
}



