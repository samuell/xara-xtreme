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

// PrintView - subclass of DocView used for printing.

/*
*/

#include "camtypes.h"

#include "prntview.h"

#include "errors.h"
#include "camelot.h"
#include "ccdc.h"
#include "document.h"
#include "rndrgn.h"
#include "spread.h"
#include "node.h"
#include "princomp.h"
#include "printctl.h"
#include "app.h"
#include "prncamvw.h"
#include "vstate.h"


DECLARE_SOURCE("$Revision$");


CC_IMPLEMENT_DYNAMIC(PrintView, View)


#define new CAM_DEBUG_NEW


/********************************************************************************************

>	PrintView::PrintView(Document *pOwnerDoc)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/95
	Inputs:		pDocument - the document to attach to (i.e. the one to print)
	Purpose:	None
	SeeAlso:	PrintView; View

********************************************************************************************/

PrintView::PrintView(Document *pOwnerDoc)
{
	// Set up document; no CamView yet.
	pDoc = pOwnerDoc;
	pViewWindow = NULL;

	// No DC yet
	PrintDC = NULL;

	// BODGE ALERT!!! Do this properly sometime.
	pVState = new ViewState;
	if (pVState == NULL) 
		AfxThrowMemoryException();

	// Connect this view state to this view
	pVState->pView = this;

	// Get print information for this document.
	PrintComponent *pPrint = 
		(PrintComponent *) pOwnerDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
	if (pPrint == NULL)
	{
		ERROR2RAW("Unable to find PrintComponent in document.");
		return;
	}

	PrintControl *pPrintControl = pPrint->GetPrintControl();
	if (pPrintControl == NULL)
	{
		ERROR2RAW("Unable to find PrintControl object in document component.");
		return;
	}

	PrintMethodType PrintType = pPrintControl->GetPrintMethod();

	// Work out whether or not to Anti-alias printing
	if (PrintType == PRINTMETHOD_AABITMAP)
	{
		Quality AntiAlias(Quality::QualityMax);
		RenderQuality = AntiAlias;
	}
}

/********************************************************************************************

>	~PrintView::PrintView()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/95
	Purpose:	Destroys a printing view - this is because it has finished printing or
				something went wrong during printing.
	SeeAlso:	PrintView; View

********************************************************************************************/

PrintView::~PrintView()
{
}



/********************************************************************************************

>	CDC *PrintView::GetRenderDC()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/09/95
	Returns:	The printer DC.
	Purpose:	Access function for the device context of this printing view object.
	SeeAlso:	View; DocView

********************************************************************************************/

CDC *PrintView::GetRenderDC()
{
	return PrintDC;
}




/********************************************************************************************

>	void PrintView::ContinueRenderView(RenderRegion* pRRegion, Spread* pSpread,
									   BOOL fRenderPaper = TRUE,
									   BOOL fDeleteRegionAfter = TRUE,
									 BOOL bForceImmediate = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/09/95
	Inputs:		pRRegion - the region to use to render the view.
				pSpread - the spread that is being rendered.
				fRenderPaper --- by default TRUE, ignored anyway because we
								 have *REAL* paper to scribble on.
				fDeleteRegionAfter --- if TRUE then the View will deallocate the
				render-region when it has finished rendering.  If FALSE then it
				won't.  By default TRUE.
	Purpose:	Used by RenderRegion::DefaultRender() to carry on rendering a view.
				In this case (i.e. printing) it only gets called once, as it calls
				RenderView which prints the view in one go, i.e. we don't do
				background rendering.

********************************************************************************************/

void PrintView::ContinueRenderView(RenderRegion* pRRegion, Spread* pSpread,
								   BOOL fRenderPaper, BOOL fDeleteRegionAfter,
									 BOOL bForceImmediate)
{
	// Disable bitmap caching but remember the old state so we can set it back
	ScopedFlagSetter fsBitmapCacheing(NodeRenderableBounded::bEnableCacheing, FALSE);
	
	// make sure the region is ok
	if (pRRegion==NULL)
		return;

	// We need to find out what sort of rendering we need to do, based on the options
	// in the print dialog. Start by finding the view and the doc we are printing
	Document* pDoc = GetDoc();
	if (pDoc!=NULL)
	{
		// Find print control object for this document, to see if we are
		// printing via bitmap or normally
		PrintComponent *pPrintComponent = (PrintComponent *)
			pDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
		
		// see if it exists or not.
		if (pPrintComponent==NULL)
		{
			if (fDeleteRegionAfter) delete pRRegion;
			return;
		}

		// Find the Print Control out of the component
		PrintControl *pPrintControl = pPrintComponent->GetPrintControl();
		if (pPrintControl == NULL)
		{
			if (fDeleteRegionAfter) delete pRRegion;
			return;
		}

		// and finally find out the printing method
		PrintMethodType PrintType = pPrintControl->GetPrintMethod();
		if ((PrintType == PRINTMETHOD_AABITMAP) || (PrintType == PRINTMETHOD_BITMAP))
		{
			// Get the region to clip all the rendering to
			DocRect ClipRect = pRRegion->GetClipRect();
			Matrix ViewTrans = pRRegion->GetMatrix();

			// Tell the RenderRegion that I am going to start rendering.
			if (!pRRegion->StartRender())
			{
				if (fDeleteRegionAfter) delete pRRegion;
				return;
			}

			// Render the spreads paper marks where necessary
			RenderPageMarks(pRRegion, ViewTrans, ClipRect, pSpread);

			// No - time to start rendering ink nodes - find the first one.
			pRRegion->SetSubRenderState(NULL);

			// Work out whether to print just selected objects, or all of them.
			if (pPrintControl->GetObjPrintRange() == PRINTRANGEOBJ_ALL)
			{
				// Print *all* objects
				pRRegion->RenderTree(pSpread, FALSE);
			}
			else
			{
				// Print only *selected objects*
				// (We do this by not rendering any bounded object that is not selected).
				//
				// This is in a different loop so that we don't impact performance of
				// normal printing.
				PrintRenderCallback MyCallback(this);
				pRRegion->RenderTree(pSpread, FALSE, TRUE, &MyCallback);
			}

			// Finished rendering.
			pRRegion->StopRender();

			// The region has finished rendering - delete it.
			if (fDeleteRegionAfter) delete pRRegion;
		}
		else
		{
			// Build the view matrix to use
			Matrix ViewMatrix = pRRegion->GetMatrix();
			
			// Try and render the view using the 3 pass system
			if (PrintMonitor::PrintMaskType==PrintMonitor::MASK_OPTIMAL)
				RenderOptimalView(pRRegion, ViewMatrix, pSpread, FALSE);
			else
				RenderSimpleView(pRRegion, ViewMatrix, pSpread, FALSE);
		}
	}
}


/********************************************************************************************

>	BOOL PrintRenderCallback::BeforeNode(Node* pNode, RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/2004
	Inputs:		pNode - pointer to node about to be rendered by RenderTree
				pRender - pointer to RenderRegion running RenderTree loop
	Outputs:	-
	Returns:	TRUE if node can be rendered
				FALSE if not
	Purpose:	Handle the callback from RenderTree that has been requested by PrintView

********************************************************************************************/

BOOL PrintRenderCallback::BeforeNode(RenderRegion* pRender, Node* pNode)
{
	return (m_pPrintView->IsPrintableNodeSelected(pNode));
}


/********************************************************************************************

>	BOOL PrintView::ViewStateChanged()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/09/95
	Returns:	TRUE if all ok;
				FALSE if not.
	Purpose:	Inform the PrintView that its OIL view object has changed the shared 
				ViewState structure.
	SeeAlso:	View::ViewStateChanged

********************************************************************************************/

BOOL PrintView::ViewStateChanged()
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

>	void PrintView::SetPixelSize()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/10/95
	Purpose:	Sets up the normal and scaled pixel sizes according to this view.

********************************************************************************************/

void PrintView::SetViewPixelSize()
{
	// Set the scaled pixel size
	ScaledPixelWidth = PixelWidth / Scale;
	ScaledPixelHeight = PixelHeight / Scale;
}



/*********************************************************************************************

> 	void PrintView::SetForeBackMode(BOOL NewFlag)

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	10/2/95
	Inputs:		NewFlag - New bg render flag.
    Purpose:    Set the background render flag for this view.
                   			                                     
*********************************************************************************************/

void PrintView::SetForeBackMode(BOOL NewFlag)
{
	// Nobody should try to set the printing to be background yet.
	ERROR3IF(NewFlag, "Trying to make a PrintView background render!");
}



/*********************************************************************************************

> 	BOOL PrintView::GetForeBackMode()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	10/2/95
    Returns:   	TRUE if background rendering is on
    Purpose:    To find out if background rendering is on
                   			                                     
*********************************************************************************************/

BOOL PrintView::GetForeBackMode()
{
	// PrintViews don't background render yet.
	return FALSE;
}


/********************************************************************************************

>	void PrintView::SetExtent(DocCoord lolog, DocCoord hilog)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Inputs:		Two DocCoords describing the low and high corners of a rectangle in logical
				space describing the full extent of the document. I.E. the bottom-left corner
				of the last chapter and the top-right corner of the first.
	Purpose:	Informs the PrintView that the extent of the document has changed in some way and 
				that the view must be altered to take this into account (namely the 
				scrollbars).

********************************************************************************************/

void PrintView::SetExtent(DocCoord lolog, DocCoord hilog)
{
	// "Pixelise" the extent DocCoords.
	// Effectively, this helps ensure that the spread will be aligned to a whole pixel boundary
	// and allows both GDraw and GDI to consistently plot the same pixels when rendering
	// the same primitive.
	// We need to do this even when printing because things like 
	// DocCoord::FindEnclosingChapter() default to pixelising extents so we get errors
	// because our coords do not lie within the pixelised extents.
	lolog.Pixelise(this);
	hilog.Pixelise(this);

	// Convert the extent given in DocCoord to WorkCoords...
	pVState->WorkAreaExtent.lo = lolog.ToWork(pDoc, this);
	pVState->WorkAreaExtent.hi = hilog.ToWork(pDoc, this);
}






/********************************************************************************************

>	WorkRect PrintView::GetViewRect()

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	A WorkCoords rectangle describing the viewport onto the document which this
				view is displaying. I.E. The top left corner of this rectangle is the same as
				the value returned by GetScrollOffsets().
	Purpose:	To find the rectangle describing the viewport onto the document.

********************************************************************************************/

WorkRect PrintView::GetViewRect()
{
	ERROR3("GetViewRect() called on unimplemented printing class!");
	return WorkRect(0,0,0,0);
}




/********************************************************************************************

>	DocRect PrintView::GetDocViewRect(Spread* pSpread)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/93
	Inputs:		pSpread - the spread that we are interested in
	Returns:	A DocCoords rectangle describing the viewport onto the document which this
				view is displaying. I.E. The top left corner of this rectangle is the same as
				the value returned by GetScrollOffsets() except in DocCoords.
	Purpose:	To find the rectangle describing the viewport onto the document.

********************************************************************************************/

DocRect PrintView::GetDocViewRect(Spread* pSpread)
{
	ERROR3("GetDocViewRect() called on unimplemented printing class!");
	return DocRect(0,0,0,0);
}




/********************************************************************************************

>	void PrintView::AttachToDC(CDC *pDC)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/10/95
	Inputs:		pDC - the printer DC to use (NULL if detaching).
	Purpose:	Attach a PrintView to a particular printer DC.  We need this because there
				is no special 'PrintCamView' to ask for it, so ScreenCamView gives us this
				DC when it receives an OnDraw() call, so we know which DC to print to.
	SeeAlso:	View::OnDraw; ScreenCamView::OnDraw; PrintView::SetPixelSize

********************************************************************************************/

void PrintView::AttachToDC(CDC *pDC)
{
	// Use this DC.
	PrintDC = pDC;

	// If its a real DC (i.e. we are not being detached from a DC) then
	// get and remember the pixel size (used by PrintView::SetPixelSize()).
	if (PrintDC != NULL)
	{
		INT32 pixwidth = pDC->GetDeviceCaps(LOGPIXELSX);
		INT32 pixheight = pDC->GetDeviceCaps(LOGPIXELSY);
		
		// Set our own idea of pixel size.
		PixelWidth  = FIXED16(72000.0 / pixwidth);
		PixelHeight = FIXED16(72000.0 / pixheight);
	}
}


/********************************************************************************************

>	void PrintView::MakeNewRenderRegion(Spread* pSpread, DocRect ClipRect, CDC* pDevContext,
										RenderType rType, BOOL PaintPaper, Node* pInvalidNode = NULL)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		pSpread - The Spread to render
				ClipRect - the Clipping rectangle of the bit to draw
				pDevContext - the Device Context to render to
				rType - the Type of the render region
				PaintPaper - Should we draw the paper?
	Purpose:	Renders the document to the printer. This function will build all the render
				regions needed to do a print.
	SeeAlso:	View::MakeNewRenderRegion

********************************************************************************************/

void PrintView::MakeNewRenderRegion(Spread* pSpread, DocRect ClipRect, CDC* pDevContext, RenderType rType, BOOL PaintPaper, Node* pInvalidNode)
{
	// Construct the transformation matrix for the spread.
	Matrix RenderMatrix = ConstructRenderingMatrix(pSpread);

	// Go and create the new render region
	RenderRegion* pNewRegion = NULL;
	if (pViewWindow != NULL)
		pNewRegion = NewRenderRegion(ClipRect, RenderMatrix, GetRenderDC(), pSpread, rType);

	// See if it worked
	if (pNewRegion == NULL)
	{
		TRACEALL( _T("Not enough memory to create render region\n"));
		return;
	}

	pNewRegion->SetBackmostChangedNode(pInvalidNode);

	// Add the region to the list of regions to render and start the
	// rendering process off if it has some ink to render
	Camelot.AddRenderRegion(pNewRegion);

	// Here we will actually want to create all the strange types of render region
	// that we will need and render them on the spot instead of putting them in
	// the list of render regions. Don't forget to remove the #include app.h
	// when the use of the Camelot var goes away.
}


/********************************************************************************************

>	Matrix PrintView::ConstructRenderingMatrix(Spread *pSpread)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/04/95
	Inputs:		pSpread - the spread that is to be rendered.
	Returns:	The required transformation matrix.
	Purpose:	From the given spread, construct a matrix that will convert spread
				coordinates to OS coordinates.
	SeeAlso:	DocView::OnDraw

********************************************************************************************/

Matrix PrintView::ConstructRenderingMatrix(Spread *pSpread)
{
	// First, translate page to origin, taking account of rotation flag.
	DocCoord TranslateToOrigin;
	DocRect	PatchClipRect = PatchInfo.GetClipRect(FALSE,FALSE);

	if (PatchInfo.Rotate)
	    TranslateToOrigin = DocCoord(-PatchClipRect.lo.x, -PatchClipRect.lo.y);
	else
    	TranslateToOrigin = DocCoord(-PatchClipRect.lo.x, -PatchClipRect.hi.y);

	// Construct the translation matrix
	Matrix TranslateToOriginMatrix = Matrix(TranslateToOrigin.x, TranslateToOrigin.y);

	// Do the optional rotation
	Matrix RotateMatrix(FIXED16(0));
	if (PatchInfo.Rotate)
		RotateMatrix = Matrix(FIXED16(270));

	// Do the scale operation (convert from percentage to ratio)
	FIXED16 fxScale = PatchInfo.Scale / FIXED16(100);
	Matrix PrintScaleMatrix(fxScale, fxScale);

	// Find the patch height so we can work out our final translation
	MILLIPOINT PatchHeight;
	PatchHeight = ((PatchInfo.Rotate) ? (PatchClipRect.Width()) : (PatchClipRect.Height()));

	// Calculate the translation to move our patch rectangle to the bottom of the paper
	MILLIPOINT YShift = -(PatchInfo.PaperHeight - ((MILLIPOINT) (PatchHeight * fxScale)));
	MILLIPOINT XShift = 0;

	// Conceptually speaking we now have a transform which scales the patch (area to print)
	// and positions it located at (0,-PaperHeight) bottom left on the paper with (x,y) increasing positive
	// to the left and up. This is the model used in the print control class to calculate a patch
	// position when working out best fit/multi/curstom fit. That class calculates +ive x,y translations
	// to fit pages onto paper considering (0,0) to be bottom left. ok so we now simply need to add
	// in those calculated translations and we're done.

	XShift += PatchInfo.XTrans;
	YShift += PatchInfo.YTrans;

	Matrix PatchTranslateMatrix(XShift, YShift);

	// Generate a flip matrix if necessary
	Matrix FlipX;
	if (PatchInfo.EmulsionDown)
	{
		if (PatchInfo.Rotate)
			FlipX = Matrix(FIXED16(1), FIXED16(0), FIXED16(0), FIXED16(-1), 0, -PatchInfo.PaperHeight);
		else
			FlipX = Matrix(FIXED16(-1), FIXED16(0), FIXED16(0), FIXED16(1), PatchInfo.PaperWidth, 0);
	}

	// MarkN wrote (way back when first descovered)
	// For some reason, NT takes the top-left corner of the printable area as the origin of the paper
	// so we compensate for that here by adjusting our X & Y translations accordingly

	// NB. this used to be for NT only, but it seems that MS have brought Win32s and Win95
	// 	   print borders into line with NT's setup so there is a consistent Win32 interface.

	// Mike wrote (more recently)
	// I've stollen the code out of PrintControl, which used to do this but now doesn't.
	// Finally fix the operating system origin. The operating system drivers
	// render everything as if (PALeftMargin, PATopMargin) was the origin, so
	// we need to shift everything slightly.
	Matrix FixOpTranslate(-PatchInfo.PALeftMargin,PatchInfo.PATopMargin);

	// The following matrix compositions MUST be performed in this order.
	// If you are tempted to optimise this code MAKE SURE THAT THEY ARE STILL
	// IN THIS ORDER WHEN YOU'VE FINISHED!

	// Start with identity
	Matrix RenderMatrix;

	// Translate to Origin
    RenderMatrix *= TranslateToOriginMatrix;

	// Optional rotate
	if (PatchInfo.Rotate)
		RenderMatrix *= RotateMatrix;

	// Scale (as in Print UI)
	RenderMatrix *= PrintScaleMatrix;

	// Patch translation
	RenderMatrix *= PatchTranslateMatrix;

	// Optional Flip around for emulsion down if nec.
	if (PatchInfo.EmulsionDown)
		RenderMatrix *= FlipX;

	// Opsys translation
	RenderMatrix *= FixOpTranslate;

	// All done.
	return RenderMatrix;
}


/********************************************************************************************

>	void PrintView::OnDraw(CDC* pDevContext, OilRect OilClipRect)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Inputs:		pDevContext - the device context of the view to render.
				OilClipRect - the clipping rectangle of the invalid region (in OilCoords).
	Purpose:	Do the printing, guv.

********************************************************************************************/

void PrintView::OnDraw(CDC* pDevContext, OilRect OilClipRect)
{
	if (CCamApp::IsDisabled())
		return;						     	// If the system is disabled, ignore

	// Find out what sort of (printing) device context we have.
	RenderType rType = CCDC::GetType(pDevContext, TRUE);

	// Find out which spread we are rendering
	Spread *pSpread = Document::GetSelectedSpread(); // PatchInfo.Spread

	// Get the rendering matrix for this view.
	Matrix RenderMatrix = ConstructRenderingMatrix(pSpread);

	// Get its inverse matrix
	Matrix InverseMatrix = RenderMatrix.Inverse();

	// Use this to convert OIL coords to spread coords
	DocRect ClipRect;
	ClipRect.lo.x = OilClipRect.lo.x;
	ClipRect.lo.y = OilClipRect.lo.y;
	ClipRect.hi.x = OilClipRect.hi.x;
	ClipRect.hi.y = OilClipRect.hi.y;
	InverseMatrix.transform(&ClipRect.lo);
	InverseMatrix.transform(&ClipRect.hi);

	// Rendering matrix may involve a rotation when printing so correct the rectangle
	// for this if necessary.
	CorrectRotatedRectangle((Rect *) &ClipRect);
	ERROR3IF(ClipRect.lo.x > ClipRect.hi.x, "PrintView::OnDraw clipping rect is invalid");
	ERROR3IF(ClipRect.lo.y > ClipRect.hi.y, "PrintView::OnDraw clipping rect is invalid");

	// Get the page clip rect
	DocRect SpreadClipRect = PatchInfo.GetClipRect(FALSE,FALSE);
	RenderMatrix.transform(&SpreadClipRect.lo);
	RenderMatrix.transform(&SpreadClipRect.hi);
	CorrectRotatedRectangle((Rect *) &SpreadClipRect);
	PatchInfo.InflateRectBy(&SpreadClipRect,TRUE,TRUE);
	InverseMatrix.transform(&SpreadClipRect.lo);
	InverseMatrix.transform(&SpreadClipRect.hi);
	CorrectRotatedRectangle((Rect *) &SpreadClipRect);

	// Clip to spread rectangle
	SpreadClipRect = SpreadClipRect.Intersection(ClipRect);

	// Make sure that the clip region is valid after the intersection
	if ((SpreadClipRect.IsValid()) && (!SpreadClipRect.IsEmpty()))
	{
		MakeNewRenderRegion(pSpread, SpreadClipRect, pDevContext, rType, TRUE);

	}
}


/********************************************************************************************

>	void PrintView::CorrectRotatedRectangle(Rect *pRect)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/28/95
	Inputs:		pRect - the rectangle to correct.
	Outputs:	pRect - the corrected rectangle.
	Purpose:	Patches up rectangles after having been transformed by the rendering
				matrix.  Because printing can involve a rotation through a multiple of
				90 degress, the corners can become swapped, so we check for this and swap
				them back around if necessary.

********************************************************************************************/

void PrintView::CorrectRotatedRectangle(Rect *pRect)
{
	MILLIPOINT Tmp;

	if (pRect->lo.x > pRect->hi.x)
	{
		Tmp = pRect->lo.x;
		pRect->lo.x = pRect->hi.x;
		pRect->hi.x = Tmp;
	}

	if (pRect->lo.y > pRect->hi.y)
	{
		Tmp = pRect->lo.y;
		pRect->lo.y = pRect->hi.y;
		pRect->hi.y = Tmp;
	}
}
