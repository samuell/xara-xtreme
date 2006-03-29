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

#include "camtypes.h"
// camconfig.h must be included immediately after camtypes.h
#include "camconfig.h"

#include "GDrawIntf.h"
#include "offscrn.h"
#include "grnddib.h"		// GRenderDIB
//#include "mario.h"			// _R(IDE_NOMORE_MEMORY)
#include "XaDrwOld.h"		// GDraw device context funcitons
//#include "ndcchbmp.h"		// NodeCacheBitmap
#include "oilbitmap.h"		// WinBitmap class
#include "capturemanager.h"	// CaptureManager

#include "prntview.h"		// for PrintView. I'm only temporary - delete me when done with.

CC_IMPLEMENT_MEMDUMP(GRenderRegionWrapper, CC_CLASS_MEMDUMP)

#define new CAM_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								GRenderRegionWrapper
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************
>	static GRenderRegionWrapper* GRenderRegionWrapper::GetAppropriateRenderWrapper (
					GRenderRegion* pActiveRR,
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp
			)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	If there is no active render region then create and initialise a new 
				GRenderDIB (via the QuickRenderer wrapper)
				Otherwise we will need a GDrawBorrower which will allow us to restore
				GDraw to the state which the active render region has it in
				If we have to change dpi, then we use a new ConcurrentRenderer
	SeeAlso:	
 ********************************************************************************************/
GRenderRegionWrapper* GRenderRegionWrapper::GetAppropriateRenderWrapper (
					GRenderRegion* pActiveRR,
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp)
{
	if(NeedsTransp)
	{
		ENSURE(bpp==32,"Can only handle transp when rendering to 32bpp bmp");
		if(bpp!=32)
			return NULL;
	}

	GRenderRegionWrapper* pNewWrap = NULL;

	if(pActiveRR == NULL)
	{
		pNewWrap = (GRenderRegionWrapper*) new QuickRenderer();
	}
	else
	{
		if(ResFactor!=1.0 || bpp!=32)
		{
			pNewWrap = (GRenderRegionWrapper*) new ConcurrentRenderer();
		}
		else
		{
			// can only use offscreen rendering system in 32bpp mode and
			// using same resolution as screen (ie same kernel matrix)
			pNewWrap = (GRenderRegionWrapper*) new GDrawBorrower();
		}
	}

	if(!pNewWrap->Initialise(pActiveRR, ResFactor, AreaOfDocumentToRender, bpp, NeedsTransp))
	{
		// failed to init. free mem.
		delete pNewWrap;
		return NULL;
	}

	// initialised and ready to render
	return pNewWrap;
}



/********************************************************************************************

>	ConcurrentRenderer* GRenderRegionWrapper::GetConcurrentRenderer(
															GRenderRegion* pSourceRR,
															double ResFactor,
															DocRect AreaOfDocumentToRender,
															UINT32 bpp,
															BOOL NeedsTransp
															)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/07/2000

	Inputs:		pSourceRR				The render-region to base ourself upon.
										If NULL, then we will use the current view settings.
				ResFactor				Scale at which to render things, RELATIVE to the
										source RR's scale factor, as we're assuming you'll
										want to be at the same scale for most uses.
				AreaOfDocumentToRender	The area to render, in millipoints.
				bpp						bits per pixel. must be 32 if you want transparency.
				NeedsTransp				Whether you want transparency on or not.

	Returns:	The RenderWrapper created, or
				NULL if unsuccessful.

	Purpose:	Create and use a concurrent renderer.
				This flavour of render wrapper derives from QuickRenderer, and creates
				a fresh GRenderRegion, whose rendering matrix is based on the source RR.

				Here's an example without any error-checking:

					ConcurrentRenderer*	pRendWrap	 = GRenderRegionWrapper::GetConcurrentRenderer(...)
					GRenderRegion*		pOffscreenRR = pRendWrap->GetRenderRegion();
					pOffscreenRR->SetFillColour(COLOUR_BLACK);
					pOffscreenRR->SetLineColour(COLOUR_TRANS);
					pOffscreenRR->DrawPath(pPathToDraw);
					pRendWrap->GetBitmapPointers(&pBmpInfo, &pBmpBits);
					pRendWrap->RestorePreviousRendererState();

	Errors:		ERROR2 if pSourceRR is NULL or you ask for a transparent, non-32bpp RR.

	See also:	BitmapShadower::RenderShadowedNodeOffscreen() for a real-use example.

********************************************************************************************/
ConcurrentRenderer* GRenderRegionWrapper::GetConcurrentRenderer(
						GRenderRegion* pSourceRR,
						double ResFactor,
						DocRect AreaOfDocumentToRender,
						UINT32 bpp,
						BOOL NeedsTransp	)
{
	ERROR2IF(NeedsTransp && (bpp != 32), NULL, "Can only do transp with 32 bpp bitmaps");

	ConcurrentRenderer* pNewWrap = new ConcurrentRenderer;
	if (!pNewWrap->Initialise(pSourceRR, ResFactor, AreaOfDocumentToRender, bpp, NeedsTransp))
	{
		delete pNewWrap;
		pNewWrap = NULL;
	}

	return pNewWrap;
}



/********************************************************************************************
>	void GRenderRegionWrapper::NewWrappedRRCreated(GRenderRegion* pGrr)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	Allow parent to set flags on any newly created GRenderRegions
	SeeAlso:	
 ********************************************************************************************/
void GRenderRegionWrapper::NewWrappedRRCreated(GRenderRegion* pGrr)
{
	pGrr->SetIsWrapped(TRUE);
}

/********************************************************************************************
>	void GRenderRegionWrapper::GetBitmapPointers(LPBITMAPINFO* bi, LPBYTE *by)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	Passes back the DIB pointers which WinBitmaps are comprised of
	SeeAlso:	
 ********************************************************************************************/
void GRenderRegionWrapper::GetBitmapPointers(LPBITMAPINFO* bi, LPBYTE *by)
{
	GRenderRegion *pGrr = GetRenderRegion();
	*bi = pGrr->pBitmapInfo;
	*by = pGrr->pBits;
}

// Karim 06/07/2000
// I'm commenting these pre-processor directives out, to fix a bug with the new shadowing
// code, where shadows at anything other than 100% zoom are incorrectly scaled. This is a
// BODGE, and this code should be revisited at a later date.

//#ifdef _DEBUG
// allows QuickRenderer::CreateGRenderDIB to access the bmp pointers of render region it creates
// NB exploits GRenderRegionWrapper's 'friend'ship with GRenderRegion, just used in DEBUG builds
void GRenderRegionWrapper::GetBitmapPointers(GRenderRegion* pGrr, LPBITMAPINFO* bi, LPBYTE *by)
{
	*bi = pGrr->pBitmapInfo;
	*by = pGrr->pBits;
}
//#endif

/********************************************************************************************
>	void GRenderRegionWrapper::SetBitmapPointers(LPBITMAPINFO* bi, LPBYTE *by)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/06/2000
	Purpose:	Set DIB pointers within the RenderRegion.
				Primarily used to prevent Quick&Concurrent Renderers from deleting
				the bitmap contained within their underlying GRenderDIBs
	SeeAlso:	QuickRenderer::RestorePreviousRendererState
 ********************************************************************************************/
void GRenderRegionWrapper::SetBitmapPointers(LPBITMAPINFO bi, LPBYTE by)
{
	GRenderRegion *pGrr = GetRenderRegion();
	pGrr->SetBitmapPointers(bi, by);
}

/********************************************************************************************
>	KernelBitmap* GRenderRegionWrapper::GetKernelBitmap()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	Encapsulates bitmap pointers of underlying render region in a KernelBitmap
	SeeAlso:	
 ********************************************************************************************/
KernelBitmap* GRenderRegionWrapper::GetKernelBitmap()
{
	LPBITMAPINFO bi;
	LPBYTE by;
	GetBitmapPointers(&bi,&by);

	LPBITMAPINFOHEADER	bh = &bi->bmiHeader;
	if(bh->biCompression==0x80000001)
	{
		GetRenderRegion()->GetDrawContext()->ConvertBitmap(
									bh, by,
									bh,	by,
									0
								);
	}

	CWxBitmap* pTempBmp= new CWxBitmap(bi, by);
	KernelBitmap* kBmp = new KernelBitmap(pTempBmp,TRUE);
	return kBmp;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								GDrawBorrower
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************
>	GDrawBorrower::GDrawBorrower()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	Constructor
	SeeAlso:	
 ********************************************************************************************/
GDrawBorrower::GDrawBorrower()
{
	m_pBorrowedRR = NULL;
}

/********************************************************************************************
>	GDrawBorrower::~GDrawBorrower()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	DeConstructor
	SeeAlso:	
 ********************************************************************************************/
GDrawBorrower::~GDrawBorrower()
{
	ENSURE(m_pBorrowedRR==NULL,"Deconstructor not meant to be called like this. Use RestorePreviousRendererState()");
}

/********************************************************************************************
>	virtual BOOL GDrawBorrower::Initialise(
					GRenderRegion* pActiveRR,
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp )
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	Puts pActiveRR into offscreen rendering state
	SeeAlso:	
 ********************************************************************************************/
BOOL GDrawBorrower::Initialise(
					GRenderRegion* pActiveRR,
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp )
{
	ERROR2IF(!(bpp==32),FALSE,"Can only do 32bpp offscreen bmps at present.");

	Capture* pCapture = m_pBorrowedRR->StartCapture(this, AreaOfDocumentToRender, CAPTUREINFO(ctNESTABLE, cfNONE), NeedsTransp, FALSE);
	return (pCapture!=NULL);
}

/********************************************************************************************
>	virtual BOOL GDrawBorrower::RestorePreviousRendererState()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	Pops top offscreen state and restores GDraw state via offscreen rendering
				system call
				NB Bitmaps not freed - only offscreen state structure memory
	SeeAlso:	
 ********************************************************************************************/
BOOL GDrawBorrower::RestorePreviousRendererState()
{
	// restore GDRAW state and free offscreen state mem
	BOOL bOK = m_pBorrowedRR->StopCapture(this, FALSE, FALSE);
	m_pBorrowedRR = NULL;
	delete this;
	return bOK;
}

/********************************************************************************************
>	GDrawBorrower::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////
//
//								QuickRenderer
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************
>	QuickRenderer::QuickRenderer()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	Constructor
	SeeAlso:	
 ********************************************************************************************/
QuickRenderer::QuickRenderer()
{
	m_pNewRR = NULL;
}

/********************************************************************************************
>	QuickRenderer::~QuickRenderer()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	DeConstructor
	SeeAlso:	
 ********************************************************************************************/
QuickRenderer::~QuickRenderer()
{
	ENSURE(m_pNewRR==NULL,"Deconstructor not meant to be called like this. Use RestorePreviousRendererState()");
}

/********************************************************************************************
>	virtual BOOL QuickRenderer::Initialise(
					GRenderRegion* pActiveRR,
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp )
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
BOOL QuickRenderer::Initialise(
					GRenderRegion* pActiveRR,
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp )
{
	ENSURE(pActiveRR==NULL,"GDraw state about to be change unrevocably, and it appears there is a render region active");

	// Setup a new GRenderDIB (NB also initialises memory)
	m_pNewRR = CreateGRenderDIB(ResFactor, AreaOfDocumentToRender, bpp, NeedsTransp);

	if(m_pNewRR)
		return TRUE;
	else
		return FALSE;
}

/********************************************************************************************
>	virtual void QuickRenderer::RestoreSimple()
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/07/2004
	Purpose:	Free up the RenderRegion we created. Nothing to restore
				NB deliberately prevents ~GRenderDIB from freeing the bitmap
				that it has just rendered by setting it's internal bmp pointers
				to NULL
	SeeAlso:
	NB:			It is up to you to free the bitmap otherwise a mem leak will occur!
 ********************************************************************************************/
void QuickRenderer::RestoreSimple()
{
	// assumes you have already got pointers to the rendered bitmap, and don't want it to be deleted
	SetBitmapPointers(NULL,NULL);		// NB now that we are using GRenderDIB need to be careful not to DisplayBits() in StopRender()
	if (m_pNewRR)
	{
		m_pNewRR->StopRender();
		delete m_pNewRR;
		m_pNewRR = NULL;
	}
}

/********************************************************************************************
>	virtual BOOL QuickRenderer::RestorePreviousRendererState()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	Free up the RenderRegion we created. Nothing to restore
				NB deliberately prevents ~GRenderDIB from freeing the bitmap
				that it has just rendered by setting it's internal bmp pointers
				to NULL
	SeeAlso:
	NB:			It is up to you to free the bitmap otherwise a mem leak will occur!
 ********************************************************************************************/
BOOL QuickRenderer::RestorePreviousRendererState()
{
	RestoreSimple();

	delete this;
	return TRUE;
}

/********************************************************************************************
>	static GRenderDIB* QuickRenderer::CreateGRenderDIB(
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp,
					Matrix* pSourceMat)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	Create and start a GRenderDIB ready for rendering
	SeeAlso:	
	Errors:		Returns NULL if unable to alloc mem for bitmaps in StartRender()
 ********************************************************************************************/
GRenderDIB* QuickRenderer::CreateGRenderDIB(
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp,
					Matrix* pSourceMat)
{
	/////////////////////////////////////////////////////////////////////////////////////////////
	//								Setup a new GRenderDIB
	/////////////////////////////////////////////////////////////////////////////////////////////
	GRenderDIB* pNewGRR=NULL;
	BOOL DoWantBMPSmoothing = FALSE;
	BOOL ok;

	// Setup new GRenderDIB based on current Screen resolution and ResFactor multiplier
	View* pView = View::GetCurrent();

	// get the top spread
	Spread* pSpread = Document::GetSelectedSpread();

	// if we've been passed a source matrix, then we'll scale it up by ResFactor and use that.
// DEBUG:
	// Note that we currently check whether the view is for printing, solely so that this code
	// can live on the net without breaking printing of shadows more than it already is.
	Matrix Mat;
	if (pSourceMat != NULL)
	{
		FIXED16 Scale;
		DocCoord dcTrans;

		pSourceMat->Decompose(&Scale, NULL, NULL, NULL, &dcTrans);

		// Karim 04/09/2000
		// BODGE, to cope with printing reflected images.
		// If the given matrix contains a reflection, then its determinant < 0.
		// If this is so, then we'll scale the matrix by -1, which corrects the output.
		if (pSourceMat->Det() < 0.0)
			Scale *= FIXED16(-1);

		Mat = Matrix(Scale * FIXED16(ResFactor), Scale * FIXED16(ResFactor));
		Mat.SetTranslation(dcTrans);

//		Mat *= *pSourceMat;
	}

	// otherwise, we'll use the view's settings, scaled up by ResFactor.
	else
	{
		Mat = pView->ConstructScaledRenderingMatrix(pSpread, ResFactor);
	}

	// get the scale factor out of the matrix.
	FIXED16 Scale;
	Mat.Decompose(&Scale);

	//	double dpi = Scale.MakeDouble() * PIXELS_PER_INCH;
	double dpi = 0.0;	// use screen dpi (ie PIXELS_PER_INCH)

	// Create a new GRenderDIB region
	pNewGRR = new GRenderDIB(AreaOfDocumentToRender, Mat, Scale, bpp, dpi);
	ERROR2IF(pNewGRR == NULL,FALSE,"Failed to create a GRenderDIB!");

	// Ensure bmp mem is not allocated from limited memory
	NewWrappedRRCreated((GRenderRegion*) pNewGRR);

	// State flags + pixel width calculations
	if(!pNewGRR->AttachDevice(pView, NULL, pSpread))		// view and spread from previous RR rather than Current - nb in create bitmap copy case
	{
		ERROR3("Cannot attach devices");
		
		delete pNewGRR;
		pNewGRR = NULL;
		return NULL;
	}

	pNewGRR->m_DoCompression = NeedsTransp;
	
	ok = pNewGRR->InitDevice();
	if (!ok)
	{
		delete pNewGRR;
		pNewGRR = NULL;
		return NULL;
	}
	pNewGRR->InitAttributes();
	pNewGRR->RRQuality.SetQuality(QUALITY_MAX);
	pNewGRR->SetQualityLevel();
	pNewGRR->SetLineAttributes();
	pNewGRR->SetFillAttributes();

	// NB following call gets GRendRegion to create the bitmap into which GDraw will render
	BOOL Started = pNewGRR->StartRender();

// Karim 06/07/2000
// I'm commenting these pre-processor directives out, to fix a bug with the new shadowing
// code, where shadows at anything other than 100% zoom are incorrectly scaled. This is a
// BODGE, and this code should be revisited at a later date.

//#ifdef _DEBUG
	// Set DIB dpi indicators correctly - so they are correct if adding to bitmap gallery (not required)
	double	PixPerMetre =	PIXELS_PER_INCH * INCHES_PER_METRE;
			PixPerMetre *=	Scale.MakeDouble();

	LPBITMAPINFO bi;
	LPBYTE by;
	GetBitmapPointers(pNewGRR,&bi,&by);
	if(bi)
	{
		bi->bmiHeader.biXPelsPerMeter = (INT32) (PixPerMetre + 0.5);
		bi->bmiHeader.biYPelsPerMeter = (INT32) (PixPerMetre + 0.5);
	}
//#endif

	if(Started)
	{
		pNewGRR->SaveContext();

		if(DoWantBMPSmoothing)
		{
			(pNewGRR->GetDrawContext())->SetBitmapSmoothingFlag(TRUE);
		}

		return pNewGRR;
	}
	else
	{
//		ENSURE(FALSE,"StartRender failed to alloc bmp for GDraw to render into.");
		delete pNewGRR;
		return NULL;
	}

}

/********************************************************************************************
>	QuickRenderer::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////
//
//								ConcurrentRenderer
//
/////////////////////////////////////////////////////////////////////////////////////////////

ConcurrentRenderer::ConcurrentRenderer()
{
	m_pOldGD = NULL;
	m_pNewGD = NULL;
	m_pView = NULL;
}

ConcurrentRenderer::~ConcurrentRenderer()
{
	if (m_pOldGD != NULL || m_pNewGD != NULL)
	{
		TRACEUSER( "Gerry", _T("GDraw context not restored in ConcurrentRenderer::~ConcurrentRenderer\n"));
	}
}

/********************************************************************************************
>	virtual BOOL ConcurrentRenderer::Initialise(
					GRenderRegion* pActiveRR,
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp
			)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/06/2000
	Purpose:	
	SeeAlso:	
 ********************************************************************************************/
BOOL ConcurrentRenderer::Initialise(
					GRenderRegion* pActiveRR,
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp
			)
{
	// if we have no source render-region, then we need to manufacture a rendering matrix
	// for ourself.
	// to generate our render-matrix, we use the current view and spread, together with
	// a user-specified dpi value.
	Matrix	SourceMat;
	if (pActiveRR == NULL)
	{
		View* pView = View::GetCurrent();
		if (pView == NULL)
			return FALSE;

		double PixelsPerInch = pView->GetConvertToEditableShapesDPI();

		Spread* pSpread = Document::GetSelectedSpread();
		if (pSpread == NULL)
			return FALSE;

		FIXED16			ViewPixelWidth	= pView->GetScaledPixelWidth();
		const double	AppPixelWidth	= MILLIPOINTS_PER_INCH / (double)PixelsPerInch;
		double ViewScaleFactor = ViewPixelWidth.MakeDouble() / AppPixelWidth;

		SourceMat = pView->ConstructScaledRenderingMatrix(pSpread, ViewScaleFactor);
	}

	// we have a source render-region, so get its matrix and scale up ResFactor
	// so that our offscreen RR would effectively have the same scaled pixel width
	// as its source render-region (not taking into account the original value of ResFactor).
	else
	{
		SourceMat = pActiveRR->GetMatrix();
		const double ActualPixelWidth = (double)pActiveRR->GetPixelWidth();
		const double DefPixelWidth = MILLIPOINTS_PER_INCH / (double)GRenderRegion::GetDefaultDPI();
		ResFactor *= DefPixelWidth / ActualPixelWidth;
	}

//	m_pView = pActiveRR->GetRenderView();
//	m_pView = View::GetCurrent();
//	if (m_pView)
//	{
//		TRACEUSER( "Gerry", _T("Forcing default context\n"));
//		m_bOldForce = m_pView->SetForceDefaultColourContexts();
//	}

	m_pNewGD = new GDrawAsm;
	if (m_pNewGD && m_pNewGD->Init())
	{
		// Save current device context
		m_pOldGD = GRenderRegion::SetTempDrawContext(m_pNewGD);
//		m_pOldGD = GRenderRegion::GetStaticDrawContext();
//		GRenderRegion::GD = pGDAsm;
	}

	// Setup a new GRenderDIB (NB also initialises memory).
	m_pNewRR = CreateGRenderDIB(ResFactor, AreaOfDocumentToRender, bpp, NeedsTransp, &SourceMat);
	if (m_pNewRR != NULL)
	{
		return TRUE;
	}

	// If we get here then we have failed so clean up and return false
	delete m_pNewGD;
	m_pNewGD = NULL;
	GRenderRegion::SetTempDrawContext(m_pOldGD);
	m_pOldGD = NULL;
	return FALSE;
}

/********************************************************************************************
>	ConcurrentRenderer::
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/02/2000
	Purpose:	
	SeeAlso:	QuickRenderer::RestorePreviousRendererState()
 ********************************************************************************************/
BOOL ConcurrentRenderer::RestorePreviousRendererState()
{
	// Dismantle the GRenderDIB render region we created
	// WHILE it's still in the temp GDraw Context that we created before we created 
	// the GRenderDIB
	QuickRenderer::RestoreSimple();

	// Now restore the device context's state, and free the memory used
	if(!m_pOldGD)
	{
		ERROR2(FALSE,"State of previous device context was not saved.");
	}
	else
	{
		if (GRenderRegion::SetTempDrawContext(m_pOldGD) != m_pNewGD)
		{
			TRACEUSER( "Gerry", _T("Not m_pNewGD when restoring\n"));
		}
		m_pOldGD = NULL;
		delete m_pNewGD;
		m_pNewGD = NULL;
	}

	// Behave like other versions of this function and delete ourselves (urgh)
	delete this;

	return TRUE;
}

