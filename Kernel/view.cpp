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

// Base class for kernel View objects.

/*
*/

#include "camtypes.h"
#include "camview.h"
//#include "view.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "vstate.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "doccoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "oilcoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "osrndrgn.h"
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camelot.h"
#include "ccdc.h"
#include "chapter.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "scanrr.h"
#include "grndbmp.h"
#include "maskedrr.h"
#include "oilbitmap.h"
#include "princomp.h"
#include "printctl.h"
//#include "prncamvw.h"
//#include "prdlgctl.h"
#include "cameleps.h"
//#include "tim.h"
#include "progress.h"
//#include "prdlgctl.h"
#include "qualattr.h"
#include "pmaskrgn.h"
#include "layer.h"
#include "colcontx.h"
#include "colormgr.h"
//#include "colplate.h"
//#include "ndcchbmp.h"
#include "nodebev.h"
#include "prncamvw.h"

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "prnmks.h"
#include "prnmkcom.h"

#ifdef RALPH
#include "ralphdoc.h"
#endif

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(View, ListItem)

#define new CAM_DEBUG_NEW

#ifdef _DEBUG
//	#define _DEBUG_ATTACH_OPTIMAL_BITMAP
//	#define _DEBUG_ATTACH_OPTIMAL_MASK
#endif

//-----------------------------------------------------------------------------------------//
// Initialise class static variables...
//
View *View::Current = NULL;
PaperRenderRegion *View::pPaperRegion = NULL;
PaperRenderRegion *View::pOnTopRegion = NULL;


/********************************************************************************************

>	View::View()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/23/95
	Purpose:	Initialise a View object.
	SeeAlso:	View

********************************************************************************************/

View::View()
{
	// No connections yet...
	pViewWindow = NULL;
	pDoc = NULL;
	pVState= NULL;

	// Actual size of OIL pixels that this view uses - default to 96dpi.
	PixelWidth = PixelHeight = FIXED16(72000.0 / 96);

	// Scaled size of OIL pixels that this view uses - default to scale of 100%
	ScaledPixelWidth = ScaledPixelHeight = PixelWidth;

	// View scale factor - default to 100%
	Scale = 1.0;

	// Initialise our ColourPlate and colour contexts to NULL pointers
	ForceDefaultColourContexts = FALSE;
	ColPlate = NULL;

	for (INT32 i = 0; i < (INT32) MAX_COLOURMODELS; i++)
	{
		ColourContexts.Context[i] = NULL;
		ShouldDeleteContext[i] = TRUE;
	}

	m_bSolidDrag = FALSE;
}



/********************************************************************************************

>	View::~View()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/23/95
	Purpose:	Cleans up a view object.
	SeeAlso:	View

********************************************************************************************/

View::~View()
{
	if (this == Current)
		SetNoCurrent();

	// Decommission all colour contexts that we own
	ColourContextList  *cclist = ColourContextList::GetList();
	ERROR3IF( cclist == NULL, "No ColourContextList? What's up?" );

	// Delete any old context we had in use (ignoring the ShouldDeleteContext flags)
	for (INT32 i = 0; i < (INT32) MAX_COLOURMODELS; i++)
	{
		if (ColourContexts.Context[i] != NULL)
			cclist->RemoveContext(&(ColourContexts.Context[i]));
	}

	if (ColPlate != NULL)
		delete ColPlate;

	if (pVState != NULL)
	{
		delete pVState;
		pVState=NULL;
	}
}

/********************************************************************************************

>	void View::AllocateDC()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	12/06/2006
	Purpose:	Hints that we've done with our DC
	SeeAlso:	View; PaperRenderRegion.

Note this is merely a hint. This routine is not guaranteed to eb called

********************************************************************************************/

void View::AllocateDC()
{
	if (pViewWindow)
		pViewWindow->AllocateDC();
}

/********************************************************************************************

>	void View::DoneWithDC()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	12/06/2006
	Purpose:	Hints that we've done with our DC
	SeeAlso:	View; PaperRenderRegion.

Note this is merely a hint. This routine is not guaranteed to eb called

********************************************************************************************/

void View::DoneWithDC()
{
	if (pViewWindow)
		pViewWindow->DoneWithDC();
}

/********************************************************************************************

>	void View::Deinit()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/23/95
	Purpose:	De-initialises the view system.  At the moment this involves destroying
				our PaperRenderRegion.
	SeeAlso:	View; PaperRenderRegion.

********************************************************************************************/

void View::Deinit()
{
	// Lose our PaperRenderRegion...
	delete pPaperRegion;
	delete pOnTopRegion;
}


/********************************************************************************************

>	BOOL View::ConnectToOilView(CCamView* pOilView)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/93
	Inputs:		Pointer to view object in the OIL layer which we will mirror.
	Outputs:	None
	Returns:	None
	Purpose:	To set up the bi-directional connection between the OIL view object and the
				View. The OIL view object causes the creation of the View and so
				already has a pointer to it when it calls this routine to set up the
				connection in the other direction.
	Errors:		None

********************************************************************************************/

BOOL View::ConnectToOilView(CCamView* pOilView)
{
	ERROR2IF(this==NULL,FALSE,"View member func called on NULL pointer");

	ERROR2IF(pViewWindow != NULL, FALSE,
				"View::ConnectToOilView called when connection already established");

	pViewWindow = pOilView;

	return TRUE;
}


/********************************************************************************************

>	CCamView* View::GetConnectionToOilView()

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/93
	Returns:	Pointer to the OIL view object which mirrors this one.
	Purpose:	To get a pointer to the OIL view object which mirrors this one. Note that
				this can return NULL if the View does not have a mirroring OIL object. In
				this case the view is closed but the View remains available so that in the
				future a new view will be able to use all of its viewing parameters.

********************************************************************************************/

CCamView* View::GetConnectionToOilView() const
{
	return pViewWindow;
}



/********************************************************************************************

>	Document* View::GetDoc() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/94
	Returns:	A pointer to the kernel Document associated with this View.
	Purpose:	Returns the linake between this View and its Document.

********************************************************************************************/

Document* View::GetDoc() const
{
	return pDoc;
}


/*********************************************************************************************

> 	static View *View::GetCurrent()

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	24/5/93
    Returns:   	Pointer to the current View object.
    Purpose:    Find the current View object which SHOULD have been set as one of the very 
    			first actions during event processing.             
                   			                                     
*********************************************************************************************/

View *View::GetCurrent()
{
#ifdef RALPH
	if(::GetCurrentThreadId() == RalphDocument::GetImportingThreadID())
	{
		return RalphDocument::GetImportingDoc()->GetFirstDocView();
	}
#endif

	return Current;
}


/*********************************************************************************************

> 	BOOL View::SetCurrent()

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	24/5/93
	Returns:	TRUE if function set current correctly
				FALSE otherwise (then an Error has been set)
    Purpose:    Make this object be the 'current' View.

*********************************************************************************************/

BOOL View::SetCurrent()
{
#ifdef RALPH
	// if we are being called from the load thread just ignore 
	if(::GetCurrentThreadId() == RalphDocument::GetImportingThreadID())
		return TRUE;
#endif
	if (Current!=this)
	{
		Current = this;

		if (this != NULL)
		{
			SetViewPixelSize();
			if (pDoc != NULL) 
				pDoc->SetCurrent();
		}
	}

	return TRUE;
}													  



/*********************************************************************************************

> 	static void View::SetNoCurrent()

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	24/5/93
    Purpose:    Set the current View pointer to be NULL, i.e., there is no current
    			View object.
                   			                                     
*********************************************************************************************/

void View::SetNoCurrent()
{
#ifdef RALPH
	// if we are being called from the load thread just ignore 
	if(::GetCurrentThreadId() == RalphDocument::GetImportingThreadID())
		return ;
#endif

	// Set no current View or Document.
	Current = NULL;
	Document::SetNoCurrent();
}



/********************************************************************************************

>	void View::SetViewState(ViewState*)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/93
	Returns:	A pointer to the ViewState object associated with this View.
	Purpose:	Sets the structure which is shared by both View and the OIL view
				object to which it is connected.  NB. PrintViews do NOT share the
				ViewState with the OIL view, because the OIL view is the screen based view,
				so the PrintView is the only one that needs to use the view state.  The
				CCamView always uses the ViewState of its DocView, not its PrintView.

********************************************************************************************/

void View::SetViewState(ViewState* pvs)
{
	ERROR3IF(pvs == NULL, "View::SetViewState called with a null parameter!");
	
	// Justin here: removed this ERROR3 because it stops the workspace restoration code
	// working.  This is a temp fix, just for the 1.1 release.  What we need is a function
	// that allows you to *change* the ViewState this View refers to, and unfortunately
	// the effect of these two ERROR3's is to make that impossible, as you can't pass a new
	// ViewState, or set th existing one to null.
//	ERROR3IF(pVState != NULL, "View::SetViewState called when view already has a view state.");
	
	pVState = pvs;
	
	// Inform the ViewState object which view it is describing.
	pVState->pView = this;
}

/*********************************************************************************************

> 	RenderRegion *View::NewRenderRegion(DocRect InvalidRect, Matrix& ViewMatrix,
										CDC* pDevContext, Spread* pSpread, 
										RenderType rType)
    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	27/5/93
	Inputs:		rType gives a hint of the type of the device
    Returns:	Pointer to a new RenderRegion, or NULL if failed.
	Scope:		Protected
    Purpose:	Make a new RenderRegion of the correct type
			    then do all the common things that have to be done to a new RenderRegion before 
			    returning it.
				If the rectangle passed in is empty then no region will be created!
	Errors:		NULL return if RR::Create or AttachDevice failed.
                   			                                     
*********************************************************************************************/

RenderRegion *View::NewRenderRegion(DocRect InvalidRect, Matrix& ViewMatrix,
									CNativeDC *pDevContext, Spread* pSpread, RenderType rType,
									bool fOwned /*= false*/ )
{
	RenderRegion *pNewRRegion;

	if (rType == RENDERTYPE_SCREENXOR)
	{
		if (pOnTopRegion == NULL)
		{
			// No OnTop rendering region - get a new render region of 
			// the special type RENDERTYPE_SCREENPAPER.
			pOnTopRegion = (PaperRenderRegion *) NewRenderRegion(InvalidRect, ViewMatrix,
																 pDevContext, pSpread,
																 RENDERTYPE_SCREENPAPER, fOwned );
			pOnTopRegion->SaveContext();
		}
		else
		{
			// Change the device of the OnTop rendering region
			pOnTopRegion->AttachDevice(this, pSpread, pDevContext, 
									   ViewMatrix, Scale, InvalidRect, fOwned );
		}

		pNewRRegion = pOnTopRegion;
	}
	else
	{
		// JCF: added 'this' argument to the end of the list.
		pNewRRegion = OSRenderRegion::Create(InvalidRect, ViewMatrix, Scale, rType, this);

		// If the construction was succesful then
		// Attach the new RenderRegion to a Window and its DC for rendering...
		if (pNewRRegion)
		{
			if (!pNewRRegion->AttachDevice(this, pDevContext, pSpread, fOwned))
			{
				// the AttachDevice failed, so tidy up
				delete pNewRRegion;
				pNewRRegion = NULL;
			}
		}
	}

	return pNewRRegion;
}

/*********************************************************************************************

> 	void View::DeInitOnTopRegion()

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	2/5/95
    Purpose:	Deinitialises the static OnTop RenderRegion, used for blob rendering.
                   			                                     
*********************************************************************************************/

void View::DeInitOnTopRegion()
{
	if (pOnTopRegion)
		pOnTopRegion->DetachDevice();
}


/********************************************************************************************

>	void View::MakeNewRenderRegion(Spread *pSpread, DocRect ClipRect, CDC *pDevContext,
								   RenderType rType, BOOL PaintPaper = FALSE, Node* pInvalidNode = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		pSpread - pointer to the spread concerned
				ClipRect - Rectangle to create region from
				pDevContect - pointer to the device context for this region (may be NULL
								if PaintPaper == FALSE)
				rType - type of rendering
				PaintPaper - TRUE if paper should be redrawn immediately, FALSE otherwise
							 (ignored for non-screen render regions).
	Outputs:	-
	Returns:	-
	Purpose:	Makes a new render region and adds it to the list associated with the DocView.
				Depending on the type of render region requested, it may be rendered to
				completion immediately (e.g. print regions are).
				Note that if PaintPaper is FALSE, pDevContext isn't used, so can be NULL.

********************************************************************************************/

void View::MakeNewRenderRegion(Spread *pSpread, DocRect ClipRect, CNativeDC *pDevContext,
							   RenderType rType, BOOL PaintPaper, Node* pInvalidNode)
{
	// Construct the transformation matrix for the spread.
	Matrix RenderMatrix = ConstructRenderingMatrix(pSpread);

	// Go and create the new render region
	RenderRegion *NewRegion = NULL;

	if (pViewWindow != NULL)
	{
//		NewRegion = NewRenderRegion(ClipRect, RenderMatrix,	GetRenderDC(), pSpread, rType);
		// GAT - Is it safe to change the above line to the one below?
		NewRegion = NewRenderRegion(ClipRect, RenderMatrix,	pDevContext, pSpread, rType);
	}

	if (NewRegion == NULL)
	{
		TRACEALL( _T("Not enough memory to create render region\n") );
		return;
	}

    if (rType == RENDERTYPE_SCREEN)
	{
		if (PaintPaper)
		{
			// Call helper function defined below
			RenderPaper(pSpread, ClipRect, pDevContext, RenderMatrix);
		}
		else
		{
			// Caller does not want paper to be rendered, so set the flag to indicate that
			// OS paper rendering is not needed.
			NewRegion->NeedsOSPaper = FALSE;
		}
	}

	NewRegion->SetBackmostChangedNode(pInvalidNode);

	// Add the region to the list of regions to render and start the
	// rendering process off if it has some ink to render
	Camelot.AddRenderRegion(NewRegion);
}



/********************************************************************************************

>	virtual void View::RenderPaper(Spread *pSpread, DocRect ClipRect, CDC *pDevContext, Matrix& RenderMatrix)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/95
	Inputs:		pSpread - pointer to the spread concerned
				ClipRect - Rectangle to create region from
				pDevContect - pointer to the device context for this region (may be NULL
								if PaintPaper == FALSE)
	Outputs:	-
	Returns:	-
	Purpose:	Actually renders the paper onto the screen. Moved from 'MakeNewRenderRegion'
				so that it can be called from outside (eg. ResetRegion)

********************************************************************************************/

void View::RenderPaper(Spread *pSpread, DocRect ClipRect, CNativeDC *pDevContext, Matrix& RenderMatrix)
{
//	TRACE( _T("View::RenderPaper\n") );
	if (pPaperRegion == NULL)
	{
		// No paper rendering region - get a new render region of 
		// the special type RENDERTYPE_SCREENPAPER.
		pPaperRegion = (PaperRenderRegion *) NewRenderRegion(ClipRect, RenderMatrix,
															 pDevContext, pSpread,
															 RENDERTYPE_SCREENPAPER);

		// We save the context here, so that PaperRenderRegion::DetachDevice() can
		// throw away all the attributes.  We only need to do it here because
		// PaperRenderRegion::AttachDevice() (see 2 inches below) calls SaveContext()
		// automagically.
		pPaperRegion->SaveContext();
	}
	else
	{
		// Change the device of the paper rendering region
		pPaperRegion->AttachDevice(this, pSpread, pDevContext, 
								   RenderMatrix, Scale, ClipRect);
	}

	// Set up the rendering system.
	if (!pPaperRegion->StartRender())
	{
		TRACEALL( _T("StartRender failed for paper in OnDraw\n") );
		return;
	}

	// Render the paper objects using the permanent PaperRenderRegion.
	pPaperRegion->SaveContext();
	RenderPaper(pPaperRegion, pSpread);
	pPaperRegion->RestoreContext();
	pPaperRegion->StopRender();
	pPaperRegion->DetachDevice();

	// Draw the DragBlobs.
	Operation* pOp = Operation::GetCurrentDragOp();
	if (pOp != NULL)
		pOp->RenderDragBlobs(ClipRect, pSpread, m_bSolidDrag);
}


/********************************************************************************************

>	virtual void View::RenderPaper(RenderRegion* pRRegion, Spread* pSpread) 

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/93
	Inputs:		pRRegion - The render region to draw in
				pSpread - the spread to draw
	Scope:		Private
	Purpose:	To render the parts of a Document which are not rendered when printing. Ie. all 
		 		the parts of the document that represent the paper on which the ink will be 
				rendered.
				This function does not take any notice of the RenderControl object - it renders 
				until it has finished so that at least the pages, pasteboard, etc. will be 
				drawn when any region is invalidated.

********************************************************************************************/
/*
Technical notes:
				Assumes that the node that it is passed is the first node in the tree and that 
				this node does NOT need to be rendered as part of the Paper view.

********************************************************************************************/

void View::RenderPaper(RenderRegion* pRRegion, Spread* pSpread)
{
	// We always want paper to render to the same display quality level.
	pRRegion->SetDefaultQuality();

	Node* pn = (Node*) pSpread;

	// First render all the paper nodes in the tree through the given RenderRegion...
	while (pn != NULL)
	{
//TRACEUSER("Phil", _T("RenderPaper for %x - %x\n"), pSpread, pn);
		pn->Render(pRRegion);
		pn = pn->FindNextForClippedPaperRender();
	}

#ifndef STANDALONE
	// Render the printable area using the document's PrintControl object, but only if this
	// is a DocView & the ShowPrintBorder's flag is set
	if (GetRuntimeClass() == CC_RUNTIME_CLASS(DocView))
	{
		DocView* pDocView = (DocView*)this;

		if (pDoc!=NULL)
		{
			if (pDocView->GetShowPrintBorders())
			{
				PrintComponent* pPrComp = (PrintComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
				if (pPrComp != NULL)
				{
					PrintControl* pPrCtrl = pPrComp->GetPrintControl();
					if (pPrCtrl != NULL)
						pPrCtrl->RenderPrintBorder(pRRegion);
				}
			}
		}
	}	
#endif
}



/********************************************************************************************

>	virtual void View::RenderPageMarks(Spread *pSpread, 
									   DocRect ClipRect,
									   CDC *pDevContext,
									   Matrix& RenderMatrix
									   RenderType rType)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/96
	Inputs:		pSpread		- pointer to the spread concerned
				ClipRect	- Rectangle to create region from
				pDevContect - pointer to the device context for this region (may be NULL
								if PaintPaper == FALSE)
				RenderMatrix- the render matrix to use.
				rType		- the type of render to give the region

	Outputs:	-
	Returns:	-
	Purpose:	Performs the rendering of page marks, ie crop marks, registration marks etc
				to the screen and printer. This base class version does nothing. All
				mark rendering technology is held in DocView and PrintView.
	SeeAlso:	DocView, PrintView

********************************************************************************************/

BOOL View::RenderPageMarks(RenderRegion *pCurrRegion, Matrix &ViewTrans, DocRect &ClipRect, Spread *pSpread)
{
#if !defined(EXCLUDE_FROM_RALPH)
#ifndef STANDALONE

/*
	// Ask the render region not to render complex shapes
	BOOL OldState = pCurrRegion->SetRenderComplexShapes(FALSE);

	// Create and set an identity matrix. We will cope with
	// the view transform later
	Matrix OldMatrix = pCurrRegion->GetMatrix();
	Matrix Identity;

	// Work out the transformed clip rectangle onto the paper
	DocRect TransformedClipRect(ClipRect);
	ViewTrans.transform(&TransformedClipRect.lo);
	ViewTrans.transform(&TransformedClipRect.hi);

	MILLIPOINT Tmp;

	if (TransformedClipRect.lo.x > TransformedClipRect.hi.x)
	{
		Tmp = TransformedClipRect.lo.x;
		TransformedClipRect.lo.x = TransformedClipRect.hi.x;
		TransformedClipRect.hi.x = Tmp;
	}

	if (TransformedClipRect.lo.y > TransformedClipRect.hi.y)
	{
		Tmp = TransformedClipRect.lo.y;
		TransformedClipRect.lo.y = TransformedClipRect.hi.y;
		TransformedClipRect.hi.y = Tmp;
	}

//	pCurrRegion->SetMatrix(ViewTrans);
	pCurrRegion->SetClipRect(TransformedClipRect);
	pCurrRegion->SetMatrix(Identity);

	// Start the render region and return if it fails
	if (!pCurrRegion->StartRender())
	{
		pCurrRegion->SetMatrix(OldMatrix);

		pCurrRegion->SetRenderComplexShapes(OldState);
		return FALSE;
	}
*/
	// start a save context for attributes
	pCurrRegion->SaveContext();

	if (pCurrRegion!=NULL && pSpread!=NULL)
	{
		if (!IS_A(pCurrRegion,PaperRenderRegion))
		{
			// find this documents print mark component.
			Document* pDoc = (Document*)pSpread->FindOwnerDoc();
			if (pDoc)
			{
				// ok, render all the print marks where necessary.
				PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
				if (pMarksMan!=NULL)
				{
					// find this documents print mark component.
					PrintMarksComponent* pMarksComp = (PrintMarksComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintMarksComponent));
					if (pMarksComp != NULL)
						pMarksMan->RenderPrintMarks(pMarksComp, pCurrRegion, ViewTrans, ClipRect, pSpread);
				}
			}
		}
	}

	pCurrRegion->RestoreContext();
/*
	pCurrRegion->StopRender();

	// Done rendering the print marks
	pCurrRegion->SetMatrix(OldMatrix);
	pCurrRegion->SetClipRect(ClipRect);
	pCurrRegion->SetRenderComplexShapes(OldState);
*/

#endif
#endif
	return TRUE;	
}




/********************************************************************************************

>	void View::OnDraw(CDC* pDevContext, OilRect OilClipRect)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com> & Rik & JustinF
	Created:	10/12/93
	Inputs:		pDevContext - the device context of the view to render.
				OilClipRect - the clipping rectangle of the invalid region (in OilCoords).
	Purpose:	Do the drawing, guv.

********************************************************************************************/

void View::OnDraw( CNativeDC *pDevContext, OilRect OilClipRect )
{
	if (CCamApp::IsDisabled())
		return;						     	// If the system is disabled, ignore

//	TRACE( _T("View::OnDraw\n") );
	
//	TRACE( _T("Rect = (%d, %d, %d, %d)"), OilClipRect.lo.x, OilClipRect.lo.y, OilClipRect.hi.x, OilClipRect.hi.y );

	// find type of rendering device
	const RenderType rType = CCDC::GetType(pDevContext, TRUE);

	// Convert OilClipRect to 64 bit WorkCoords
	WorkRect ClipRect = OilClipRect.ToWork(pVState->GetScrollPos());
//TRACE( _T("View::GetScrollPos : %d %d"),(INT32)(pVState->GetScrollPos().x/750),(INT32)(pVState->GetScrollPos().y/750));
//TRACE( _T("ClipRect : %d %d %d %d\n"),(INT32)(ClipRect.lo.x/750),(INT32)(ClipRect.lo.y/750),(INT32)(ClipRect.hi.x/750),(INT32)(ClipRect.hi.y/750));

	ERROR3IF(ClipRect.lo.x > ClipRect.hi.x, "DocView::OnDraw clipping rect is invalid");
	ERROR3IF(ClipRect.lo.y > ClipRect.hi.y, "DocView::OnDraw clipping rect is invalid");

	// Use the last chapter as a sentinel.
	Chapter* pLastChapter = (Chapter*) pDoc->GetFirstNode()->
								FindNext()->FindLastChild(CC_RUNTIME_CLASS(Chapter));

	// Find all the spreads in the document that intersect the clipping rect, and create
	// a render region for each of them.

	// Iterate over the chapters in the document.
	for (Chapter* pChapter = Node::FindFirstChapter(pDoc);
		 pChapter != 0;
 		 pChapter = pChapter->FindNextChapter())
	{
		// Convert chapter bounding box to logical coords
		WorkRect LogChapterRect;
		DocRect PhysChapterRect = pChapter->GetPasteboardRect(TRUE, this);
		LogChapterRect.lo = PhysChapterRect.lo.ToWork(pDoc, this);
		LogChapterRect.hi = PhysChapterRect.hi.ToWork(pDoc, this);

		BOOL IsLastChapter = (pChapter == pLastChapter);

		// Check to see if this chapter intersects the clipping rectangle.
		// If the chapter is the last one in the document, then the chapter's pasteboard
		// does not include the area of the bottom of the last spread, so we only check
		// the chapter's top boundary.
		if (ClipRect.lo.y <= LogChapterRect.hi.y &&
		    (IsLastChapter || ClipRect.hi.y >= LogChapterRect.lo.y))
		{
			// Find the last spread in the chapter.
			Spread* pLastSpread = (Spread*) pChapter->FindLastChild(CC_RUNTIME_CLASS(Spread));

			// Iterate over the spreads in the chapter.
			for (Spread* pSpread = pChapter->FindFirstSpread();
				 pSpread != 0;
				 pSpread = pSpread->FindNextSpread())
			{
				// Convert spread bounding box to logical coords
				DocRect PhysSpreadRect = pSpread->GetPasteboardRect(TRUE, this);	// Pixelised

				WorkRect LogSpreadRect;
				LogSpreadRect.lo = PhysSpreadRect.lo.ToWork(pSpread, this);
				LogSpreadRect.hi = PhysSpreadRect.hi.ToWork(pSpread, this);

				BOOL IsLastSpread = (pSpread == pLastSpread);

				// Check if spread intersects the clipping rect
				if (ClipRect.lo.y <= LogSpreadRect.hi.y &&
				    ((IsLastChapter && IsLastSpread) || ClipRect.hi.y >= LogSpreadRect.lo.y))
				{
					// Make render region for intersection between spread and cliprect.
					DocRect SpreadClipRect = pSpread->GetWidePasteboard(this);

					// Convert clip rectangle to document coords
					DocRect DocClipRect = OilClipRect.ToDoc(pSpread, this);

					// Clip to spread rectangle
					SpreadClipRect = SpreadClipRect.Intersection(DocClipRect);

					// Make sure that the clip region is valid after the intersection
					if (SpreadClipRect.IsValid() && !SpreadClipRect.IsEmpty())
					{
						// Convert document coords to spread coords and make a render region.
						pSpread->DocCoordToSpreadCoord(&SpreadClipRect);
						MakeNewRenderRegion(pSpread, SpreadClipRect, pDevContext, rType, TRUE);
					}
				}
			}
		}
	}

//	TRACE( _T("Leaving View::OnDraw\n") );
}



/********************************************************************************************

>	BOOL View::SetScrollOffsets(WorkCoord NewTopLeft, BOOL RedrawNeeded = TRUE)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		New scroll offsets
				Flag to control whether redraw is needed or not
	Purpose:	To scroll this view to a new position over the document. The coordinate
				supplied is the coordinate of the top left corner of the viewport onto the
				document. The RedrawNeeded flag is TRUE when any invalid areas created should
				be redrawn immediately and FALSE if they should be ignored.

********************************************************************************************/

BOOL View::SetScrollOffsets(WorkCoord NewTopLeft, BOOL RedrawNeeded)
{
//	TRACE(_T("View::SetScrollOffsets(%d, %d)\n"), (INT32)(NewTopLeft.x), (INT32)(NewTopLeft.y));
	ERROR2IF(this==NULL,FALSE,"View member func called on NULL pointer");

	//Graham 7/12/97: Downgraded to an error 3, otherwise this can go off in retail builds
	if (NewTopLeft.x < (XLONG)0  || NewTopLeft.y > (XLONG)0)
	{
		ERROR3("View::SetScrollOffsets - invalid parameters");
		return FALSE;
	}

	pVState->SetScrollPos(NewTopLeft);			// Set the new scroll offsets
	return TRUE;
}




/********************************************************************************************

>	WorkCoord View::GetScrollOffsets() const

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Returns:	Top left corner of the viewport in WorkCoords.
	Purpose:	To find the position of the top left corner of the display.

********************************************************************************************/

WorkCoord View::GetScrollOffsets() const
{
	ERROR2IF(this==NULL,WorkCoord(0,0),"View member func called on NULL pointer");

	return pVState->GetScrollPos();
}



/*********************************************************************************************

> 	BOOL View::SetViewScale(FIXED16 NewScale)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	31/5/93
	Inputs:		New view scale factor
    Purpose:    Set the viewing scale factor for this view.
				(Also, sets up the scaled pixel size in DocCoord if this View is current.
                   			                                     
*********************************************************************************************/

BOOL View::SetViewScale(FIXED16 NewScale)
{
	ERROR2IF(this==NULL,FALSE,"View member func called on NULL pointer");
	ERROR2IF(pVState==NULL,FALSE,"View not connected to ViewState");

	// Set the new scale
	pVState->ViewScale = Scale = NewScale;

	// Set up the scaled pixel size for this view according to new scale.
	SetViewPixelSize();

	// Inform the Viewstate that it has been changed and that the doc extent may
	// need to be recomputed...
	ViewStateChanged();

	return TRUE;
}




/*********************************************************************************************

> 	FIXED16 View::GetViewScale() const

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	31/5/93
    Returns:   	Viewing scale factor for this view.
    Purpose:    Inquire the viewing scale factor from this View.
                   			                                     
*********************************************************************************************/

FIXED16 View::GetViewScale() const
{
	return Scale;
}




/********************************************************************************************

>	Matrix View::ConstructRenderingMatrix(Spread *pSpread)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com> & Rik
	Created:	10/12/93
	Inputs:		pSpread - the spread that is to be rendered.
	Returns:	The required transformation matrix.
	Purpose:	From the given spread, construct a matrix that will convert spread
				coordinates to OS coordinates.
	SeeAlso:	DocView::OnDraw

********************************************************************************************/

Matrix View::ConstructRenderingMatrix(Spread *pSpread)
{
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

	// Just in case the above hasn't work (the DocCoord conversion seems to produce some rounding
	// errors), Pixelize the offset
	WorkCoordOffset.Pixelise(72000.0/PixelWidth.MakeDouble(),72000.0/PixelHeight.MakeDouble());
	
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

	// Apply scroll-offset translation to move origin to viewing position...
    RenderMatrix *= TranslateToOrigin;

	return RenderMatrix;
}

/********************************************************************************************

>	Matrix View::ConstructScaledRenderingMatrix(Spread *pSpread, double ScaleFactor)

	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/00
	Inputs:		pSpread		= the spread that is to be rendered.
				ScaleFactor = multiplier for current Scale (ie zoom factor)
	Returns:	The required transformation matrix.
	Purpose:	Create a rendermatrix at ScaleFactor precision of the current Scale
				Allows you to create a bitmap with higher/lower resolution than the
				current view
				eg used to create a bitmap of a given invalid region at double the current
				zoom factor (ie synonymous concept to "double resolution" or "double DPI")
	SeeAlso:	View::ConstructRenderingMatrix(Spread *pSpread)

********************************************************************************************/
Matrix View::ConstructScaledRenderingMatrix(Spread *pSpread, double ScaleFactor)
{
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

	FIXED16 ScaledScale = Scale.MakeDouble() * ScaleFactor;
    Matrix ScaleMat(ScaledScale, ScaledScale);

	// The following matrix compositions MUST be performed in this order.
	// If you are tempted to optimise this code MAKE SURE THAT THEY ARE STILL
	// IN THIS ORDER WHEN YOU'VE FINISHED!

	// Apply scale factors to convert from millipoint distances to pixel distances...
    RenderMatrix *= ScaleMat;

	// Apply scroll-offset translation to move origin to viewing position...
    RenderMatrix *= TranslateToOrigin;

	return RenderMatrix;
}


/********************************************************************************************

>	FIXED16 View::GetPixelWidth()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/19/95
	Returns:	The pixel width for this view
	Purpose:	Get the width of a pixel in this view.

********************************************************************************************/

FIXED16 View::GetPixelWidth()
{
	return PixelWidth;
}


/********************************************************************************************

>	FIXED16 View::GetPixelHeight()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/19/95
	Returns:	The pixel height for this view
	Purpose:	Get the height of a pixel in this view.

********************************************************************************************/

FIXED16 View::GetPixelHeight()
{
	return PixelHeight;
}

/********************************************************************************************

>	void View::GetPixelSize(FIXED16 *pPixelWidth, FIXED16 *pPixelHeight)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/10/95
	Outputs:	pPixelWidth, pPixelHeight - pixel size.
	Purpose:	Get the size of a pixel in this view.

********************************************************************************************/

void View::GetPixelSize(FIXED16 *pPixelWidth, FIXED16 *pPixelHeight)
{
	*pPixelWidth = PixelWidth;
	*pPixelHeight = PixelHeight;
}


/********************************************************************************************

>	void View::SetPixelSize(FIXED16 NewPixelWidth, FIXED16 NewPixelHeight)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/10/95
	Inputs:		PixelWidth, PixelHeight - new pixel size to use.
	Purpose:	Set the size of a pixel in this view.

********************************************************************************************/

void View::SetPixelSize(FIXED16 NewPixelWidth, FIXED16 NewPixelHeight)
{
	PixelWidth = NewPixelWidth;
	PixelHeight = NewPixelHeight;
}


/********************************************************************************************

>	FIXED16 View::GetScaledPixelWidth()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/19/95
	Returns:	The scaled pixel width for this view
	Purpose:	Get the width of a scaled pixel in this view.

********************************************************************************************/

FIXED16 View::GetScaledPixelWidth()
{
	return ScaledPixelWidth;
}


/********************************************************************************************

>	FIXED16 View::GetScaledPixelHeight()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/19/95
	Returns:	The scaled pixel height for this view
	Purpose:	Get the height of a scaled pixel in this view.

********************************************************************************************/

FIXED16 View::GetScaledPixelHeight()
{
	return ScaledPixelHeight;
}

/********************************************************************************************

>	void View::GetScaledPixelSize(FIXED16 *pScaledPixelWidth, FIXED16 *pScaledPixelHeight)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/10/95
	Outputs:	pScaledPixelWidth, pScaledPixelHeight - scaled pixel size.
	Purpose:	Get the size of a scaled pixel in this view.

********************************************************************************************/

void View::GetScaledPixelSize(FIXED16 *pScaledPixelWidth, FIXED16 *pScaledPixelHeight)
{
	*pScaledPixelWidth = ScaledPixelWidth;
	*pScaledPixelHeight = ScaledPixelHeight;
}


/****************************************************************************

>	double View::GetConvertToEditableShapesDPI()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/08/2005

	Returns:	The resolution to use for convert top editable shapes 
				operations while this view is current
	Purpose:	Allows derived view classes to override the reolution

****************************************************************************/

double View::GetConvertToEditableShapesDPI()
{
	INT32 iPixPerInch = 0;
	if (!GetApplication()->GetConvertToEditableShapesDPI(&iPixPerInch))
	{
		ERROR3("View::GetConvertToEditableShapesDPI; Couldn't get app's dpi");
		iPixPerInch = 96;
	}

	return(iPixPerInch);
}


/********************************************************************************************

>	void View::SetScaledPixelSize(FIXED16 NewScaledPixelWidth, FIXED16 NewScaledPixelHeight)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/10/95
	Inputs:		NewScaledPixelWidth, NewScaledPixelHeight - new scaled pixel size to use.
	Purpose:	Set the size of a scaled pixel in this view.

********************************************************************************************/

void View::SetScaledPixelSize(FIXED16 NewScaledPixelWidth, FIXED16 NewScaledPixelHeight)
{
	ScaledPixelWidth = NewScaledPixelWidth;
	ScaledPixelHeight = NewScaledPixelHeight;
}



/********************************************************************************************

>	ColourContext *View::GetColourContext(ColourModel Model, BOOL ReturnNULLIfNone = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96

	Inputs:		Model - The colour model of the context you want to find
				ReturnNULLIfNone - if FALSE, an appropriate view will be found, by
					searching my parent document and global contexts if necessary.
					if TRUE, then if this view hasn't got it's own special context
					it will immediately return NULL.

	Returns:	NULL, or a pointer to an appropriate context.

	Purpose:	To find an appropriate colour context in the given colour model
				for use when rendering into this view. This allows us to control
				colour separation and correction down to a per-view level.

	Notes:		This only functions for RGB and CMYK contexts at present. If you wish
				to render to a view using an output context of a different type, you
				will need to upgrade tios method.

				ONLY Use the returned pointer temporarily - If the view is deleted
				or if the ColourPlate options on the view are changed, the context
				will be deleted. You should thus always call this function to get
				the current Context - it's cached, and quick after the first call.

	SeeAlso:	View::GetColourPlate; View::SetColourPlate; ColourContext; ColourManager

********************************************************************************************/

ColourContext *View::GetColourContext(ColourModel Model, BOOL ReturnNULLIfNone)
{
	// If our internal flag is set, we will return a default global colour context
	// This allows View::RenderOptimalBitmapPhase to create RenderRegions which will
	// not colour spearate or correct (as we want to do the correction as a post process
	// on the produced bitmap)
	if (ForceDefaultColourContexts)
		return(ColourManager::GetColourContext(Model, pDoc));


	// Under normal circumstances, if we have a special ColourPlate, we look in our
	// cache for an appropriate context. First, we'll make sure an appropriate context
	// is cached (We don't do this if the caller specified ReturnNULLIfNone)
	if (!ReturnNULLIfNone && ColourContexts.Context[Model] == NULL && ColPlate != NULL)
	{
		// It's not cached yet - create a new context of this type
		ColourContext *NewCC = NULL;

		switch(Model)
		{
			case COLOURMODEL_RGBT:
				NewCC = new ColourContextRGBT(this);
				break;

			case COLOURMODEL_CMYK:
				NewCC = new ColourContextCMYK(this);
				break;

			default:
				ERROR3("View::GetColourContext only supports RGB/CMYK contexts at present");
				break;
		}

		if (NewCC != NULL)
		{
			// Copy our ColourPlate
			ColourPlate *NewPlate = new ColourPlate(*ColPlate);

			if (NewPlate == NULL)
			{
				// Failure - delete the ColourContext and abandon the attempt
				delete NewCC;
				NewCC = NULL;
			}
			else
			{
				// Attach the ColourPlate to the context, add it into our cache of ColourContexts,
				// and add the new context to the context list
				NewCC->SetColourPlate(NewPlate);

				ColourContextList *cclist = ColourContextList::GetList();
				ERROR3IF(cclist == NULL, "No ColourContextList? What's up?");

				ColourContexts.Context[Model] = NewCC;
				cclist->AddContext(&(ColourContexts.Context[Model]));
			}
		}
	}

	// See if we've got a cached context we can return
	ColourContext *cc = ColourContexts.Context[Model];

	// If we own our own colour context, then use it. Otherwise, find a suitable
	// context from our parent document; if that has none, then use a global default.
	// (We don't do this if the caller specified ReturnNULLIfNone)
	if (cc == NULL && !ReturnNULLIfNone)
		cc = ColourManager::GetColourContext(Model, pDoc);

	return(cc);
}



/********************************************************************************************

>	ColourPlate *View::GetColourPlate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/96

	Returns:	NULL, or a pointer to the current ouptut ColourPlate

	Purpose:	To find out what colour separation options are currently in use by this view

	SeeAlso:	View::SetColourPlate; View::GetColourContext

********************************************************************************************/

ColourPlate *View::GetColourPlate(void)
{
	return(ColPlate);
}



/********************************************************************************************

>	void View::SetColourPlate(ColourPlate *NewPlate, BOOL bSendContextChanged = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/96

	Inputs:		NewPlate - NULL (to remove any attached plate), or a pointer to a new
				ColourPlate to use - (the existing COlourPLate attached to this view
				will be deleted, and replaced with a copy of this new ColourPlate)

	Purpose:	Sets a new colour plate (colour separation description for this view.
				Any future requests for colour contexts for this view (e.g. the next
				time it is redrawn, etc) will return a ColourContext using this new
				ColourPlate.

	Notes:		A copy is made of the ColourPlate object - you still own the one you
				lent us.

	SeeAlso:	View::GetColourPlate; View::GetColourContext

********************************************************************************************/

void View::SetColourPlate(ColourPlate *NewPlate, BOOL bSendContextChanged)
{
	if (ColPlate != NULL)
	{
		delete ColPlate;
		ColPlate = NULL;
	}

	if (NewPlate != NULL)
		ColPlate = new ColourPlate(*NewPlate);

	// Wipe any cached colour contexts for the old ColourPlate
	ColourContextList *cclist = ColourContextList::GetList();
	ERROR3IF(cclist == NULL, "No ColourContextList? What's up?");

	for (INT32 i = 0; i < (INT32) MAX_COLOURMODELS; i++)
	{
		if (ColourContexts.Context[i] != NULL)
		{
			if (ShouldDeleteContext[i])
				cclist->RemoveContext(&(ColourContexts.Context[i]));
			else
			{
				// We didn't cache this context, so we shouldn't delete it
				// Instead, we should just change it over to use the new ColourPlate
				ColourPlate *NewPlate = NULL;
				if (ColPlate != NULL)
					NewPlate = new ColourPlate(*ColPlate);

				// Attach the ColourPlate to the context - even if it is NULL this is safe
				ColourContexts.Context[i]->SetColourPlate(NewPlate);
			}
		}
	}

	// And if necessary, inform everybody that the colour separation options for 
	// the selected view have been changed
	if (bSendContextChanged && this == DocView::GetSelected())
		ColourManager::SelViewContextHasChanged();
}



/********************************************************************************************

>	void View::SetColourContext(ColourModel Model, ColourContext *NewContext = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96

	Inputs:		Model	   - The colour model of this context
				NewContext - NULL to delete any existing context attached to this view,
							 or a pointer to the new context to use. Any old attached
							 context will be deleted. The new context is now "owned" by the
							 view, which will delete it when it is finished with it.

	Purpose:	Sets a new colour context for this view. 

				When we render to a view, an appropriate CMYK or RGB context is used
				for output, fetched by calling View::GetColourContext. Normally, the view
				will get a global default context, attach a copy of the View's ColourPlate
				(see View::SetColourPlate) to it, and return that. However, if you set
				a special context, this will be used instead of a global default.
				
				This is only currently used by the printing system to use a CMYK context
				which knows how to colour-correct separated output for the output printer
				device.

	Notes:		Once given to a view, the ColourContext is "Owned" by the view, and will
				be deleted when the view is deleted, or when this function is called to
				set a different context active. Call this function with a NULL pointer
				to clear any existing ColourContext attached to the view.

				The View will attach the context to the global ColourContext list for you

				The View will always attach its own ColourPlate to the ColourContext
				that you pass in, overriding any ColourPlate you may have given it.

				(Implementation) The View keeps a special array of BOOLs next to its
				Array of contexts, which tell it whether the context in the cache
				is one created on demand by the view (which it can delete when the
				ColourPlate is changed) or one passed in by the caller (which should
				only be deleted on destruction or Context change)

	SeeAlso:	View::SetColourPlate; View::GetColourPlate; View::GetColourContext

********************************************************************************************/

void View::SetColourContext(ColourModel Model, ColourContext *NewContext)
{
	ColourContextList *cclist = ColourContextList::GetList();
	ERROR3IF(cclist == NULL, "No ColourContextList? What's up?");

	if (ColourContexts.Context[Model] != NULL)
		cclist->RemoveContext(&(ColourContexts.Context[Model]));

	ColourContexts.Context[Model] = NewContext;
	ShouldDeleteContext[Model] = TRUE;		// Reset the auto-delete flag by default

	if (NewContext != NULL)
	{
		ERROR3IF(Model != NewContext->GetColourModel(),
					"View::SetColourContext - Model must match colour context's model!");

		ColourPlate *NewPlate = NULL;
		if (ColPlate != NULL)
			NewPlate = new ColourPlate(*ColPlate);

		// Attach our ColourPlate to the context, add it into our cache of ColourContexts,
		// and add the new context to the context list
		NewContext->SetColourPlate(NewPlate);

		cclist->AddContext(&(ColourContexts.Context[Model]));

		if (ColourContexts.Context[Model] != NULL)
		{
			// Instruct ourselves not to auto-delete this context from the cache
			ShouldDeleteContext[Model] = FALSE;
		}
	}
}

/********************************************************************************************

>	ProgressDisplay::ProgressDisplay()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/25/95
	Purpose:	Initialise the progress display object.  Defaults to no progress display.

********************************************************************************************/

ProgressDisplay::ProgressDisplay()
{
	PORTNOTETRACE("printing","Disabled CCPrintInfo");
#ifndef EXCLUDE_FROM_XARALX
	//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
#ifndef STANDALONE
	pPrintInfo = CCPrintInfo::GetCurrent();
#else
	pPrintInfo = NULL;
#endif
#endif //webster
#endif
	DoProgressDisplay = FALSE;
	IsPrinting = FALSE;
	NumNodesRendered = 0;
	LastProgressUpdate = 0;
	ProgressInterval = 1;
	TotalNodes = 0;

	// Currently we use a scale factor of 256 so that we get decent resolution even
	// when exporting only a few nodes that have lots of transparency.
	// We use 256 (a power of 2) so that muls and divs are fast.
	ProgressScaleFactor = 256;
}


/********************************************************************************************

>	void ProgressDisplay::SetUp(RenderRegion *pRender, ScanningRenderRegion *pScanner)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/25/95
	Inputs:		pRender - the render region we are going to render into.
				Scanner - the scanning render region that contains information about
						  what shapes we are going to render into this region.
	Purpose:	Sets up the progress display object based on the render regions passed in.
	SeeAlso:	ScanningRenderRegion

********************************************************************************************/

void ProgressDisplay::SetUp(RenderRegion *pRender, ScanningRenderRegion *pScanner)
{
#ifndef STANDALONE
	// Work out if we need a progress display
	IsPrinting = pRender->IsPrinting();
	if (!IsPrinting && !IS_A(pRender, CamelotEPSRenderRegion))
		// No - stop here
		return;

	// We need a progress display
	DoProgressDisplay = TRUE;

	// Find out how many nodes the stages need to render...
	BOOL FoundLastComplex = FALSE;
	FirstStageCount = 0;
	SecondStageCount = 0;
	ThirdStageCount = 0;

	DocRect ClipRect = pRender->GetClipRect();
	Spread *pSpread = pRender->GetRenderSpread();

	// ----------------------------------------------------------------
	// NOTE! This is the last use of FindFirstForClippedInkRender!
	// It should be replaced by RenderTree and a Callback object
	Node *pNode = pSpread->FindFirstForClippedInkRender(&ClipRect, pRender);
	// ----------------------------------------------------------------

	Node *pLastComplexNode = pScanner->GetLastComplexObject();

	// Work out whether to count just selected objects, or all of them.
	BOOL CountAllObjects = TRUE;
PORTNOTE("printing","Disabled CCPrintInfo")
#ifndef EXCLUDE_FROM_XARALX
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	if (IsPrinting && pPrintInfo != NULL)
	{
		PrintControl *pPrCtrl = pPrintInfo->GetPrintControl();
		ERROR3IF(pPrCtrl == NULL, "Unable to find PrintControl object from PrintInfo.");
		CountAllObjects = (pPrCtrl->GetObjPrintRange() == PRINTRANGEOBJ_ALL);
	}
#endif //webster
#endif
	if (CountAllObjects)
	{
		// Count *all* objects
		while (pNode != NULL)
		{
			// Count the node that we have
			if (FoundLastComplex)
				ThirdStageCount++;
			else
				FirstStageCount++;

			// Have we found the last complex node yet?
			// (If so, then we start incrementing ThirdStageCount instead of 
			// FirstStageCount).
			if (pNode == pLastComplexNode)
				FoundLastComplex = TRUE;

			// Find another one to count
			pNode = pNode->FindNextForClippedInkRender(&ClipRect, pRender);
		}
	}
	else
	{
		// Count only *selected objects*
		// (We do this by not rendering any bounded object that is not selected).
		//
		// This is in a different loop so that we don't impact performance of
		// normal printing.
		while (pNode != NULL)
		{
			// Count the node that we have, unless it is bounded but not selected.
			if (View::IsPrintableNodeSelected(pNode))
			{
				if (FoundLastComplex)
					ThirdStageCount++;
				else
					FirstStageCount++;
			}

			// Have we found the last complex node yet?
			// (If so, then we start incrementing ThirdStageCount instead of 
			// FirstStageCount).
			if (pNode == pLastComplexNode)
				FoundLastComplex = TRUE;

			// Find another one to render
			pNode = pNode->FindNextForClippedInkRender(&ClipRect, pRender);
		}
	}


	// Ok - now we need to set the slider range for the print progress dialog.
	// We do this on a number of nodes basis - the first and second stages render the
	// same number of nodes, so we count the 'first' count twice, and then add the
	// 'third' count.
	if (pScanner->GetNumComplex() != 0)
	{
		SecondStageCount = FirstStageCount;

		// Check to see if we do not do the first stage, - if not then we do the whole
		// thing as a bitmap without bothering with the mask (becuase it's too much for
		// most printer drivers to cope with).
		if ((PrintMonitor::PrintMaskType==PrintMonitor::MASK_SIMPLE) &&
			(CCDC::GetType(pRender->GetRenderDC(), TRUE) != RENDERTYPE_PRINTER_PS))
		{
			FirstStageCount = 0;
		}
	}

	INT32 Range = FirstStageCount + SecondStageCount + ThirdStageCount;
PORTNOTE("printing","Disabled CCPrintInfo")
#ifndef EXCLUDE_FROM_XARALX
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	if (IsPrinting)
	{
//		if (pPrintInfo == NULL)
//		{
//			ERROR2RAW("No PrintInfo object found!");
//			InformError();
//			return;
//		}

		if (pPrintInfo != NULL)
		{
			pPrintInfo->SetSliderSubRangeMax(Range * ProgressScaleFactor);
			pPrintInfo->SetSliderSubRangePos(0);
		}
	}
	else
#endif //wesbter
#endif
	{
		// Start progress display (with no initial delay) for Camelot EPS export
		String_64 ExportMsg(_R(IDT_EXPORTMSG_CAMEPS));
		BeginSlowJob(Range * ProgressScaleFactor, FALSE, &ExportMsg);
	}

	// Provide a progress update for every 1%
	ProgressInterval = (Range * ProgressScaleFactor) / 100;
#else
	// Do nothing as not required on standalone viewer
	return;
#endif

}


/********************************************************************************************

>	BOOL ProgressDisplay::IncProgress(INT32 NumNodes = 1)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/25/95
	Inputs:		NumNodes - the number of nodes that have just been rendered.
	Returns:	TRUE if updated ok;
				FALSE if user requested that operation is aborted, e.g. presing Escape.
	Purpose:	Updates the rendered node count by the number specified.  If the progress
				display needs updating, then it is updated.
	SeeAlso:	ProgressDisplay

********************************************************************************************/

BOOL ProgressDisplay::IncProgress(INT32 NumNodes)
{
#ifndef STANDALONE

	NumNodesRendered++;

	if (!DoProgressDisplay)
		// No progress display needed.
		return TRUE;
PORTNOTE("printing","Disabled CCPrintInfo")
#ifndef EXCLUDE_FROM_XARALX
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	if (IsPrinting && (pPrintInfo != NULL) && (pPrintInfo->m_bContinuePrinting == FALSE))
		// User has cancelled job
		return FALSE;
#endif //webster
#endif
	if ((NumNodesRendered * ProgressScaleFactor) > (LastProgressUpdate + ProgressInterval))
	{
		// Time to update the progress display.
		LastProgressUpdate = NumNodesRendered * ProgressScaleFactor;
PORTNOTE("printing","Disabled CCPrintInfo")
#ifndef EXCLUDE_FROM_XARALX
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
		if (IsPrinting && pPrintInfo != NULL)
		{
			// Update slider
			pPrintInfo->SetSliderSubRangePos(LastProgressUpdate);

			// Does user want to suspend printing?
			if (pPrintInfo->Abort())
				return FALSE;
		}
		else
#endif //webster
#endif
			return ContinueSlowJob(LastProgressUpdate);
	}

#endif

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL ProgressDisplay::FirstStageDone()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/25/95
	Returns:	TRUE if updated ok;
				FALSE if user requested that operation is aborted, e.g. presing Escape.
	Purpose:	Should be called when the first stage of 3-pass rendering is complete.
	SeeAlso:	ProgressDisplay::SecondStageDone

********************************************************************************************/

BOOL ProgressDisplay::FirstStageDone()
{
#ifndef STANDALONE

	NumNodesRendered = FirstStageCount;

	if (!DoProgressDisplay)
		// No progress display needed.
		return TRUE;
PORTNOTE("printing","Disabled CCPrintInfo")
#ifndef EXCLUDE_FROM_XARALX
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	if (IsPrinting && pPrintInfo != NULL)
		pPrintInfo->SetSliderSubRangePos(NumNodesRendered * ProgressScaleFactor);
	else
#endif //webster
#endif
		return ContinueSlowJob(NumNodesRendered * ProgressScaleFactor);

#endif

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL ProgressDisplay::SecondStageDone()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/25/95
	Returns:	TRUE if updated ok;
				FALSE if user requested that operation is aborted, e.g. presing Escape.
	Purpose:	Should be called when the second stage of 3-pass rendering is complete.
	SeeAlso:	ProgressDisplay::FirstStageDone

********************************************************************************************/

BOOL ProgressDisplay::SecondStageDone()
{
#ifndef STANDALONE

	NumNodesRendered = FirstStageCount + SecondStageCount;

	if (!DoProgressDisplay)
		// No progress display needed.
		return TRUE;
PORTNOTE("printing","Disabled CCPrintInfo")
#ifndef EXCLUDE_FROM_XARALX
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	if (IsPrinting && pPrintInfo != NULL)
	{
		// Update slider
		pPrintInfo->SetSliderSubRangePos(NumNodesRendered * ProgressScaleFactor);

		// Does user want to suspend printing?
		if (pPrintInfo->Abort())
			return FALSE;
	}
	else
#endif //webster
#endif
		return ContinueSlowJob(NumNodesRendered * ProgressScaleFactor);

#endif

	// All ok
	return TRUE;
}

/********************************************************************************************

>	void ProgressDisplay::AllStagesDone()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/25/95
	Purpose:	Should be called when the 3-pass rendering has been finished.  This
				causes the progress bar to be de-initialised when exporting Camelot EPS.
	SeeAlso:	ProgressDisplay

********************************************************************************************/

void ProgressDisplay::AllStagesDone()
{
#ifndef STANDALONE
	// If we are exporting EPS, end the progress indicator as we're all done.
	if (DoProgressDisplay && !IsPrinting)
		EndSlowJob();
#endif
}


/********************************************************************************************

>	void ProgressDisplay::StartBitmapPhase(INT32 NumBands)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/25/95
	Inputs:		NumBands - the number of bands required to render the bitmap phase.
	Purpose:	Gets ready to render the banded bitmap phase of the rendering.
				Works out how much each band should update the progress position by.
	SeeAlso:	ProgressDisplay::RenderedBitmapPhaseBand

********************************************************************************************/

void ProgressDisplay::StartBitmapPhase(INT32 NumBands)
{
	if (!DoProgressDisplay)
		return;

	// Work out how much of the progress range we can use for each band.
	BandSize = (SecondStageCount * ProgressScaleFactor) / NumBands;
	BandOffset = 0;
}

/********************************************************************************************

>	void ProgressDisplay::StartBitmapPhaseBand(INT32 TotalNumScanlines)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/25/95
	Inputs:		TotalNumScanLines - the height of this ban in pixels.
	Purpose:	Inform the progress display that a band is about to start being rendered,
				and how many scanlines high it will be.
	SeeAlso:	ProgressDisplay::BitmapPhaseBandRenderedTo

********************************************************************************************/

void ProgressDisplay::StartBitmapPhaseBand(INT32 TotalNumScanlines)
{
	// Remember how high this band is.
	BandHeight = TotalNumScanlines;

	// Work out how much to move the progress display by for each scanline.
	BandIncrement = BandSize / BandHeight;
}


/********************************************************************************************

>	BOOL ProgressDisplay::BitmapPhaseBandRenderedTo(INT32 ScanlinesRendered)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/25/95
	Inputs:		ScanlinesRendered - the numbr of scanlines rendered so far in this band.
	Returns:	TRUE if updated ok;
				FALSE if user requested that operation is aborted, e.g. presing Escape.
	Purpose:	Update the progress to reflect how many scanlines have been rendered in
				this band of the bitmap rendering phase.
	SeeAlso:	ProgressDisplay::StartBitmapPhaseBand

********************************************************************************************/

BOOL ProgressDisplay::BitmapPhaseBandRenderedTo(INT32 ScanlinesRendered)
{
#ifndef STANDALONE

	if (!DoProgressDisplay)
		// No progress display needed.
		return TRUE;

	// Work out how far into this band we have got.
	BandOffset = ScanlinesRendered * BandIncrement;
	if (BandOffset > BandSize)
		BandOffset = BandSize;
	INT32 ProgressPos = (NumNodesRendered * ProgressScaleFactor) + BandOffset;

PORTNOTE("printing","Disabled CCPrintInfo")
#ifndef EXCLUDE_FROM_XARALX
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	// Update progress indicators
	if (IsPrinting && pPrintInfo != NULL)
	{
		// Update slider
		pPrintInfo->SetSliderSubRangePos(ProgressPos);

		// Does user want to suspend printing?
		if (pPrintInfo->Abort())
			return FALSE;
	}
	else
#endif //webster
#endif
		return ContinueSlowJob(ProgressPos);

#endif

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL ProgressDisplay::EndBitmapPhaseBand()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/25/95
	Returns:	TRUE if updated ok;
				FALSE if user requested that operation is aborted, e.g. presing Escape.
	Purpose:	Another band of the bitmap rendering phase has been finished, so update
				the progress display.

********************************************************************************************/

BOOL ProgressDisplay::EndBitmapPhaseBand()
{
#ifndef STANDALONE

	if (!DoProgressDisplay)
		return TRUE;

	// Move on, but limit to range! (in case of cock-ups)
	NumNodesRendered += (BandSize / ProgressScaleFactor);
	if (NumNodesRendered > (FirstStageCount + SecondStageCount))
		NumNodesRendered = (FirstStageCount + SecondStageCount);
PORTNOTE("printing","Disabled CCPrintInfo")
#ifndef EXCLUDE_FROM_XARALX
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	if (IsPrinting && pPrintInfo != NULL)
	{
		// Update slider
		pPrintInfo->SetSliderSubRangePos(NumNodesRendered * ProgressScaleFactor);

		// Does user want to suspend printing?
		if (pPrintInfo->Abort())
			return FALSE;
	}
	else
#endif //webster
#endif
		return ContinueSlowJob(NumNodesRendered * ProgressScaleFactor);
#endif
	// All ok
	return TRUE;
}


/********************************************************************************************

>	SlowJobResult View::RenderSimpleNodes(Node *pNode, RenderRegion *pRender,
							 ProgressDisplay& Progress, Node *pLastComplexNode = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/25/95
	Inputs:		pNode - the node to start rendering at.
				pRender - the render region to render the nodes into.
				Progress - usual progress info object.
				pLastComplexNode - optional: if not NULL, then stop rendering when this
											 node is reached, otherwise render to completion.
	Returns:	SLOWJOB_SUCCESS if rendered ok;
				SLOWJOB_FAILURE if an error occured;
				SLOWJOB_USERABORT if the user intervened - e.g. pressed Escape.
	Purpose:	Function to render nodes 'normally' during 3-pass rendering.  This copes
				with stopping at the specified last complex node, and with only printing
				selected objects if the user has requested it.
	SeeAlso:	ProgressInfo; View::RenderSimpleView

********************************************************************************************/

SlowJobResult View::RenderSimpleNodes(Node *pNode, RenderRegion *pRender,
							 ProgressDisplay& Progress, Node *pLastComplexNode)
{
#ifndef STANDALONE
	DocRect ClipRect = pRender->GetClipRect();

	// Work out whether we need to render all objects, or just the selected ones.
	BOOL RenderAllObjects = TRUE;
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	PORTNOTETRACE("print","Disabled CCPrintInfo");
#ifndef EXCLUDE_FROM_XARALX
	if (pRender->IsPrinting())
	{
		CCPrintInfo *pPrintInfo = CCPrintInfo::GetCurrent();
//		ERROR2IF(pPrintInfo == NULL, SLOWJOB_FAILURE, "No print info while printing!");
		if (pPrintInfo != NULL)
		{
			PrintControl *pPrCtrl = pPrintInfo->GetPrintControl();
			RenderAllObjects = (pPrCtrl->GetObjPrintRange() == PRINTRANGEOBJ_ALL);
		}
	}
#endif
#endif //webster
	// Render nodes into specified render region
	while ((pNode!=NULL) && (pNode != pLastComplexNode))
	{
		if (pNode->IsRenderable() && (RenderAllObjects || IsPrintableNodeSelected(pNode)))
		{
			// Render the node that we have
			pNode->Render(pRender);

			// If progress display is needed, update it if it's time.
			if (!Progress.IncProgress())
				return SLOWJOB_USERABORT;
		}

		// Find another one to render
		pNode = pNode->FindNextForClippedInkRender(&ClipRect, pRender);
	}

	// All ok
	return SLOWJOB_SUCCESS;
#else
	return SLOWJOB_FAILURE;
#endif
}


/********************************************************************************************

>	BOOL View::IsPrintableNodeSelected(Node *pNode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/28/95
	Inputs:		pNode - the node to check.
	Returns:	TRUE if the node should be printed;
				FALSE if not.
	Purpose:	Determine whether a given node should be rendered when we are printing
				with "selected objects only" enabled.  It checks to see if the node is
				bounded - if it isn't, then it's an attribute or similar and so is always
				rendered.  If it is bounded then return TRUE if it is selected or if it is
				the child of a selected object.  Otherwise we return FALSE to indicate that
				it should not be printed.
	SeeAlso:	View::RenderSimpleNodes

********************************************************************************************/

BOOL View::IsPrintableNodeSelected(Node *pNode)
{
	if (pNode->IsBounded())
	{
		// We must check the selected status of this node.
		// Even if this node is not selected, its parent might be, so we
		// scan upwards until we find a layer, or a selected node.
		Node *pTestNode = pNode;
		while ((pTestNode != NULL) && 
			   (!pTestNode->IsSelected()) && 
			   (!pTestNode->IsLayer()))
		{
			pTestNode = pTestNode->FindParent();
		}

		// If we failed to find a parent at some point then it can't possibly be selected
		if (pTestNode == NULL)
			return(FALSE);

		// If it is selected then we'd better print this node
		if (pTestNode->IsSelected())
			return(TRUE);

		// If the original node is a shadow then we should test the right sibling
		// and print this node if that is selected
		if (pNode->IsAShadow())
		{
			Node* pRightSibling = pNode->FindNext();
			if (pRightSibling && pRightSibling->IsSelected())
				return(TRUE);
		}

		// Fall back to not printing this node
		return(FALSE);

		// Ok, what did we find?
//		return ((pTestNode != NULL) && pTestNode->IsSelected());
	}
	else
	{
		// It is a non-bounded node such as an attribute so we render it always.
		return TRUE;
	}
}




// This should be moved somewhere sensible when dependencies stop being an issue

/********************************************************************************************

>	class ScannerRenderCallback : public RenderCallback

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/12/2004
	Purpose:	Handling callbacks from RenderTree
	Notes:		RenderTree function calls the main function in this class when it's about
				to render any Node.

********************************************************************************************/

class ScannerRenderCallback : public RenderCallback
{
public:
	ScannerRenderCallback(View* pView, BOOL bRenderAll, ScanningRenderRegion* pScanRR)
	{
		m_pView = pView;
		m_bRenderAll = bRenderAll;
		m_pScanRR = pScanRR;
	}

	virtual BOOL BeforeNode(RenderRegion* pRegion, Node* pNode)
	{
		BOOL bRender = m_bRenderAll || m_pView->IsPrintableNodeSelected(pNode);
//		TRACE( _T("SRC# BeforeNode    0x%08x - %s	returning %s\n", pNode, pNode->GetRuntimeClass()->GetClassName(), bRender ? "true" : "false"));
//		TRACE( _T("SRC# BN 0x%08x %s\n", pNode, bRender ? "true" : "false"));
		m_pScanRR->SetRenderState(pNode);
		return(bRender);
	}

	virtual BOOL BeforeSubtree(RenderRegion* pRegion, Node* pNode, Node** ppNextNode, 
									BOOL bClip, SubtreeRenderState* pState)
	{
//		TRACE( _T("SRC# BeforeSubtree 0x%08x - %s\n"), pNode, pNode->GetRuntimeClass()->GetClassName());
//		TRACE( _T("SRC# BS 0x%08x\n"), pNode);
		m_pScanRR->SetRenderState(pNode);
		return(FALSE);
	}

//	virtual BOOL AfterSubtree(RenderRegion* pRegion, Node* pNode);

private:
	View* m_pView;
	BOOL m_bRenderAll;
	ScanningRenderRegion* m_pScanRR;
};


class OptimalPrintRenderCallback : public RenderCallback
{
public:
	OptimalPrintRenderCallback(View* pView, Spread* pSpread, BOOL bRenderAll, ScanningRenderRegion* pScanRR,
								BOOL bPrintPaper, INT32* pNodesSoFar, ProgressDisplay* pProgress, BOOL bCount)
	{
		m_pView = pView;
		m_bRenderAll = bRenderAll;
		m_pScanRR = pScanRR;
		m_bPrintPaper = bPrintPaper;
		m_pNextAction = m_pScanRR->FindFirstFromList();
		m_bNextComplex = m_pScanRR->IsThisNodeComplex();
		m_NextBoundsRect = m_pScanRR->GetSpanBounds();
		m_bDoingComplex = FALSE;
		m_pSpread = pSpread;
		m_pNodesSoFar = pNodesSoFar;
		m_pProgress = pProgress;
		m_bCount = bCount;
		m_ComplexCount = 0;
	}

	virtual BOOL BeforeNode(RenderRegion* pRegion, Node* pNode)
	{
		BOOL bRender = (!m_bDoingComplex || (pNode->IsAnAttribute() || pNode->IsANodeClipView())) &&
						(m_bRenderAll || m_pView->IsPrintableNodeSelected(pNode));
//		TRACE( _T("OPRC# BeforeNode    0x%08x - %s	returning %s\n", pNode, pNode->GetRuntimeClass()->GetClassName(), bRender ? "true" : "false"));

		if (!m_bCount && m_bDoingComplex)
			m_ComplexCount++;
		else if (m_pNodesSoFar)
			(*m_pNodesSoFar)++;

		if (m_bCount)
			return(FALSE);

		if (m_pProgress)
		{
			if (!m_pProgress->SetNodesRendered(*m_pNodesSoFar))
			{
				TRACE( _T("*****************************\n"));
				TRACE( _T("************ User aborted job\n"));
				TRACE( _T("*****************************\n"));
			}
		}

		return(bRender);
	}

	virtual BOOL BeforeSubtree(RenderRegion* pRegion, Node* pNode, Node** ppNextNode, 
									BOOL bClip, SubtreeRenderState* pState)
	{
//		TRACE( _T("OPRC# BeforeSubtree 0x%08x - %s\n"), pNode, pNode->GetRuntimeClass()->GetClassName());

		if (!m_bCount && pNode == m_pNextAction)
		{
			BOOL bIsComplex = m_bNextComplex;
			TRACEUSER("noone", _T("OPRC# BeforeSubtree 0x%08x - %s\n"), pNode, pNode->GetRuntimeClass()->GetClassName());
			TRACEUSER("noone", _T("Hit Start of %s span\n"), bIsComplex ? _T("complex") : _T("simple") );
			// If we have been doing a complex span then we need to render it now
			if (m_bDoingComplex)
			{
				TRACEUSER("noone", _T("Try to render the last complex span NumNodes = %d\n"), m_ComplexCount);
				RenderLastComplexSpan(pRegion, pNode);
			}
			
			m_SpanBoundsRect = m_NextBoundsRect;
			m_pNextAction = m_pScanRR->FindNextFromList();
			m_bNextComplex = m_pScanRR->IsThisNodeComplex();
			m_NextBoundsRect = m_pScanRR->GetSpanBounds();
			TRACEUSER("noone", _T("NextBounds = (%d, %d) - (%d, %d)\n"), m_NextBoundsRect.lo.x, m_NextBoundsRect.lo.y, 
				m_NextBoundsRect.hi.x, m_NextBoundsRect.hi.y );
			TRACEUSER("noone", _T("NextAction is %s 0x%08x - %s\n"), m_bNextComplex ? _T("complex") : _T("simple"), m_pNextAction, 
				m_pNextAction ? m_pNextAction->GetRuntimeClass()->GetClassName() : _T("") );
			m_ComplexCount = 0;
			m_pComplexStart = bIsComplex ? pNode : NULL;
			m_bDoingComplex = bIsComplex;
		}
		
		return(FALSE);
	}

//	virtual BOOL AfterSubtree(RenderRegion* pRegion, Node* pNode);

	BOOL FinishRendering(RenderRegion* pRegion)
	{
		if (!m_bCount && m_bDoingComplex)
		{
			RenderLastComplexSpan(pRegion, NULL);
		}

		return true; 
	}

protected:
	BOOL RenderLastComplexSpan(RenderRegion* pRegion, Node* pNextSpanStart)
	{
		// Call RenderOptimalBitmapPhase to handle this
		SlowJobResult BitmapResult;
		DocRect ClipRect = pRegion->GetClipRect();
		ClipRect = ClipRect.Intersection(m_SpanBoundsRect);
//		TRACE( _T("ClipRect = (%d, %d) - (%d, %d)\n"), ClipRect.lox, ClipRect.loy, ClipRect.hix, ClipRect.hiy);
		if (ClipRect.IsValid() && !ClipRect.IsEmpty())
		{
			Matrix ViewTrans = pRegion->GetMatrix();
//			TRACE( _T("Rendering from 0x%08x - %s\n", m_pComplexStart, m_pComplexStart?m_pComplexStart->GetRuntimeClass()->m_lpszClassName:""));
//			TRACE( _T("Rendering to   0x%08x - %s\n", pNextSpanStart, pNextSpanStart?pNextSpanStart->GetRuntimeClass()->m_lpszClassName:""));
			BitmapResult = m_pView->RenderOptimalBitmapPhase(ClipRect, ViewTrans, m_pSpread, pRegion,
											  m_pComplexStart, pNextSpanStart, m_bRenderAll,
											  m_bPrintPaper, *m_pNodesSoFar, m_pProgress, m_ComplexCount);
		}

		return true; 
	}

private:
	View* m_pView;
	Spread* m_pSpread;
	BOOL m_bRenderAll;
	ScanningRenderRegion* m_pScanRR;
	BOOL m_bPrintPaper;
	INT32* m_pNodesSoFar;
	ProgressDisplay* m_pProgress;

	Node* m_pNextAction;		// The Node pointer that starts the next run
	Node* m_pComplexStart;
	BOOL m_bNextComplex;		// TRUE if the next action is for a complex span
	BOOL m_bDoingComplex;		// TRUE if currently handling a complex span
	BOOL m_bCount;
	INT32 m_ComplexCount;
	DocRect m_SpanBoundsRect;
	DocRect m_NextBoundsRect;
};


class OptimalBitmapRenderCallback : public RenderCallback
{
public:
	typedef enum
	{
		RS_BEFORECOMPLEX = 0,
		RS_INCOMPLEX,
		RS_AFTERCOMPLEX,
	} RenderStateType;
	
	OptimalBitmapRenderCallback(View* pView, BOOL bRenderAll, Node* pFirstComplex, Node* pNextNormal, BOOL bRenderMask,
				double RegionArea, INT32* pNodesSoFar, ProgressDisplay* pProgress, BOOL bCount, double NodeIncrement = 1.0)
	{
		m_pView = pView;
		m_bRenderAll = bRenderAll;
		m_pFirstComplex = pFirstComplex;
		m_pNextNormal = pNextNormal;
		m_bRenderMask = bRenderMask;
		m_RenderState = RS_BEFORECOMPLEX;
		m_RegionArea = RegionArea;
		m_pNodesSoFar = pNodesSoFar;
		m_pProgress = pProgress;
		m_bCount = bCount;
		m_NodeIncrement = NodeIncrement;
		m_dNodesSoFar = m_pNodesSoFar ? *pNodesSoFar : 0;
	}

	virtual BOOL BeforeNode(RenderRegion* pRegion, Node* pNode)
	{
		if (m_bCount)
		{
			if (m_pNodesSoFar)
				(*m_pNodesSoFar)++;

			return(FALSE);
		}
		else
		{
			m_dNodesSoFar += m_NodeIncrement;
			if (m_pNodesSoFar)
				*m_pNodesSoFar = (INT32)m_dNodesSoFar;
			if (m_pProgress)
			{
				if (!m_pProgress->SetNodesRendered(*m_pNodesSoFar))
				{
					TRACE( _T("*****************************\n"));
					TRACE( _T("************ User aborted job\n"));
					TRACE( _T("*****************************\n"));
				}
			}
		}
//		BOOL bRender = (m_bRenderAll || m_pView->IsPrintableNodeSelected(pNode)) && pNode->NeedsToRender(pRegion);
		BOOL bRender = (m_bRenderAll || m_pView->IsPrintableNodeSelected(pNode));
		if (bRender)
		{
			if (m_bRenderMask)
			{
				switch (m_RenderState)
				{
					case RS_BEFORECOMPLEX:
						// We must skip nodes except for attributes and clipviews
						bRender = FALSE;
						if (pNode->IsAnAttribute() || pNode->IsANodeClipView())
						{
							// Let it render normally
							bRender = TRUE;
						}
						break;

					case RS_INCOMPLEX:
						// Use the standard case below
						break;
					
					case RS_AFTERCOMPLEX:
			 			// if it is bounded, then decide if it is worth doing this
						// Don't do this for NodeClipView's or large objects clipped by small ones
						// Will not get clipped when they are rendered causing holes in the complex
						// area that really shouldn't be there
						if (pNode->IsBounded() && !pNode->IsANodeClipView())
						{
							// Find out the bounds
							DocRect BoundsRect = ((NodeRenderableBounded*)pNode)->GetBoundingRect();
							
							// if it is small, then don't draw it
							double BoundsArea = (double)BoundsRect.Width() * (double)BoundsRect.Height();
							if (BoundsArea > 0)
							{
								// work out the coverage of this node as a percentage of the whole region
								if (m_RegionArea != 0)
								{
									BoundsArea = (BoundsArea * 100) / m_RegionArea;
									if (BoundsArea < 6.0)
										bRender = FALSE;
								}
							}
						}
						break;

					default:
						TRACE( _T("OBRC# Bad RenderState in BeforeNode\n"));
						break;
				}			
			}
			else	// m_bRenderMask
			{

				switch (m_RenderState)
				{
					case RS_BEFORECOMPLEX:
					case RS_INCOMPLEX:
						// Use the standard case below
						break;
					
					case RS_AFTERCOMPLEX:
						// Must skip everything until the end
						bRender = FALSE;
						break;

					default:
						TRACE( _T("OBRC# Bad RenderState in BeforeNode\n"));
						break;
				}			
			}
		}		

//		TRACE( _T("OBRC%s# BeforeNode    0x%08x - %s	returning %s\n", m_bRenderMask?"M":"B", pNode, pNode->GetRuntimeClass()->GetClassName(), bRender ? "true" : "false"));
		return(bRender);
	}

	virtual BOOL BeforeSubtree(RenderRegion* pRegion, Node* pNode, Node** ppNextNode, BOOL bClip, SubtreeRenderState* pState)
	{
//		if (!m_bCount)
//		{
//			TRACE( _T("OBRC%s# BeforeSubtree 0x%08x - %s\n", m_bRenderMask?"M":"B"), pNode, pNode->GetRuntimeClass()->GetClassName());
//		}

		switch (m_RenderState)
		{
			case RS_BEFORECOMPLEX:
				if (pNode == m_pFirstComplex)
				{
//					TRACE( _T("OBRC# Hit FirstComplex\n"));
					if (m_bRenderMask)
					{
						// We must switch the render region to drawing complex
						PrintingMaskedRenderRegion* pMaskRegion = (PrintingMaskedRenderRegion*)pRegion;
						pMaskRegion->SetMaskDrawingMode(TRUE);
					}
					
					// And change state to be in the complex span
					m_RenderState = RS_INCOMPLEX;
				}
				break;

			case RS_INCOMPLEX:
				if (pNode == m_pNextNormal)
				{
//					TRACE( _T("OBRC# Hit NextNormal\n"));
					if (m_bRenderMask)
					{
						// We must switch the render region to drawing simple
						PrintingMaskedRenderRegion* pMaskRegion = (PrintingMaskedRenderRegion*)pRegion;
						pMaskRegion->SetMaskDrawingMode(FALSE);
					}
					
					// And change state to be after the complex span
					m_RenderState = RS_AFTERCOMPLEX;

					if (!m_bRenderMask)
					{
						*pState = SUBTREE_NORENDER;
						return(TRUE);
					}
				}
				break;

			case RS_AFTERCOMPLEX:
				break;

			default:
				TRACE( _T("OBRC# Bad RenderState in BeforeSubtree\n"));
				break;
		}
		return(FALSE);
	}

//	virtual BOOL AfterSubtree(RenderRegion* pRegion, Node* pNode);

private:
	View* m_pView;
	BOOL m_bRenderAll;
	Node* m_pFirstComplex;
	Node* m_pNextNormal;
	BOOL m_bRenderMask;			// TRUE if currently handling a complex span
	RenderStateType m_RenderState;
	double m_RegionArea;
	INT32* m_pNodesSoFar;
	ProgressDisplay* m_pProgress;
	BOOL m_bCount;
	double m_NodeIncrement;
	double m_dNodesSoFar;
};



/********************************************************************************************

>	RenderViewResult View::RenderOptimalView(RenderRegion* pRender, Matrix& ViewTrans,
											 Spread* pSpread, BOOL PrintPaper)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/95
	Inputs:		pRender - The render region to render into
				ViewTrans - the transform matrix to use for the rendering
				pSpread - the spread to render
				PrintPaper - TRUE if the paper should be rendered as well
	Returns:	RENDERVIEW_SUCCESS	 - view was rendered successfully
				RENDERVIEW_FAILURE	 - an error occured while rendering the view.
				RENDERVIEW_NOTNEEDED - the view was not rendered as the usual RenderView
									   call can be used instead.
				RENDERVIEW_USERABORT - the user aborted the rendering operation.
									   (e.g. pressing Escape, or aborting the print job)
	Purpose:	This function has been designed along similar lines to the RenderView()
				function found elsewhere in this file. This version of the function is
				slightly different though. This version of the function is able to render
				complex objects, such as transparent objects into normal render regions.
				It does this by first scanning through all the objects that need rendering
				to see if any of them require complex things. If they do, then part of the
				rendering is done into a bitmap and blitted over the rest of the shapes using
				a masked blit. All very complicated really! This is the optimal version of
				this function, that will render individual complex shapes to the device
				instead of building them all up and blitting them in one go. This will
				produce the best quality results as as many objects as possible will be
				rendered to the resolution of the device.
	SeeAlso:	DocView::RenderView

********************************************************************************************/

RenderViewResult View::RenderOptimalView(RenderRegion* pRender, Matrix& ViewTrans, Spread* pSpread,
										BOOL PrintPaper)
{
//	TRACEUSER( "Gerry", _T("View::RenderOptimalView\n"));
	ERROR3IF(PrintMonitor::PrintMaskType!=PrintMonitor::MASK_OPTIMAL, "PrintMaskType must be OPTIMAL here\n");

	// Find out what the host render region is capable of
	RRCaps Caps;
	pRender->GetRenderRegionCaps(&Caps);

	// Whether or not the rendering was successful.
	RenderViewResult Result = RENDERVIEW_FAILURE;

	// Find out the Clipping rectangle of the render region to draw into
	DocRect ClipRect = pRender->GetClipRect();

	BOOL Printing = pRender->IsPrinting();

	// Create and set up a new Scanning render region
	ScanningRenderRegion Scanner(Printing);

	// Attach a device to the scanning render region
	// Since this rr does no real rendering, it does not need a Device context
	Scanner.AttachDevice(this, (CNativeDC*) NULL, pSpread);

	// Get it ready to render
	Scanner.SetMatrix(ViewTrans);
	Scanner.SetClipRect(ClipRect);
	
	// Start the render region and return if it fails
	if (Scanner.StartRender())
	{			
		// and tell the scanning render region about the caps of the host region
		Scanner.SetHostRRCaps(Caps);

		// Work out whether we need to render all objects, or just the selected ones.
		BOOL RenderAllObjects = TRUE;
PORTNOTE("printing", "Disabled printing")
#ifndef EXCLUDE_FROM_XARALX
		CCPrintInfo *pPrintInfo = NULL;
		if (Printing)
		{
			pPrintInfo = CCPrintInfo::GetCurrent();
			if (pPrintInfo != NULL)
			{
				PrintControl *pPrCtrl = pPrintInfo->GetPrintControl();
				RenderAllObjects = (pPrCtrl->GetObjPrintRange() == PRINTRANGEOBJ_ALL);
			}
		}

		BOOL ReallyPrinting = Printing && (pPrintInfo != NULL);

		// We going to analyse the document
		if (pPrintInfo != NULL)
			pPrintInfo->SetAnalysing();
#endif

		ScannerRenderCallback ScanCallback(this, RenderAllObjects, &Scanner);

		// Call RenderTree to do the rendering
		Scanner.RenderTree(pSpread, FALSE, TRUE, &ScanCallback);

		// Thats all the nodes rendered, so stop rendering
		Scanner.StopRender();

		// Ok, we now have a Scanning render region that has all the info we need to know in it.
#ifdef _DEBUG
		// Dump out the node span info
//		Scanner.DumpNodeRuns();
#endif

PORTNOTE("printing", "Disabled printing")
#ifndef EXCLUDE_FROM_XARALX
		// We going to print the document now
		if (pPrintInfo != NULL)
			pPrintInfo->SetPrinting();
#endif

		// Set up the matrix and clipping rect
		pRender->SetMatrix(ViewTrans);
		pRender->SetClipRect(ClipRect);

		// Ask the render region not to render complex shapes
		pRender->SetRenderComplexShapes(FALSE);
	
		// Start the render region and get out if it fails
		if (!pRender->StartRender())
			goto OptimalRenderError;

		// I think it likely we need to put something in here.
		if (Printing)
		{
			// Render the spreads paper marks where necessary
			RenderPageMarks(pRender, ViewTrans, ClipRect, pSpread);
		}

		// Draw the paper if we have been asked to do so.
		if (PrintPaper)
		{
			// Render all paper nodes from there into the real render region
			pRender->SaveContext();
			RenderPaper(pRender, pSpread);
			pRender->RestoreContext();

			if (pRender->NeedsOSPaper)
			{
				// This render region uses the OS to render paper, so if we don't
				// render anything except paper then there is no need for e.g.
				// GRenderRegions to blit anything.  Hence we call this function
				// to clear the changed bbox.
				pRender->SetClean(TRUE, FALSE);
			}

			// The Paper is now done
			pRender->IsPaperRendered = TRUE;
		}

		ProgressDisplay Progress;
		Progress.SetUpOptimal(pRender, &Scanner);
		INT32 NodesRendered = 0;

//		TRACE( _T("Starting main render\n"));
		// We need to call RenderTree for the spread
		// We pass the ScanningRenderRegion to the RenderCallback object
		// so that it can tell which objects are normal and which are complex
		// We also pass the progress object so that can be updated
		OptimalPrintRenderCallback MyCallback(this, pSpread, RenderAllObjects, &Scanner, PrintPaper, &NodesRendered, &Progress, FALSE);
		pRender->RenderTree(pSpread, FALSE, FALSE, &MyCallback);
		MyCallback.FinishRendering(pRender);

		// Examine the callback object to see if anything went wrong

		// And stop rendering
		pRender->StopRender();

		// All worked, so return success
		Result = RENDERVIEW_SUCCESS;
	}
	
OptimalRenderError:
#ifdef _DEBUG
	if (Result != RENDERVIEW_SUCCESS)
		TRACEUSER( "Gerry", _T("RenderOptimalView failed\n"));
#endif

	// Unlink the RenderRegion from list and error if it was not there
	BOOL InList = Camelot.DeleteRenderRegion(pRender);
    CAM_USE(InList); // Suppress unused variable warning on retail build
	ERROR3IF(!InList, "DocView::RenderOptimalView tried to delete a RenderRegion not in the list");

	// Instruct the render region to commit suicide. This call doesn't work on a CamelotEPS
	// render region.
	pRender->ConditionalSuicide ();

//	TRACEUSER( "Gerry", _T("End of View::RenderOptimalView\n"));

	return Result;
}

/********************************************************************************************

>	SlowJobResult View::RenderOptimalBitmapPhase(DocRect& ClipRect, Matrix& ViewTrans, Spread* pSpread,
											RenderRegion* pRender, Node* pLastNormalInRun,
											Node* pLastComplexInRun, BOOL RenderAllObjects,
											BOOL bPrintPaper,
											INT32& NodesSoFar, ProgressDisplay& Progress)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/95
	Inputs:		ClipRect - the Clipping rectangle
				ViewTrans - the view matrix to use
				pSpread - the Spread to render to
				pRender - the host render region
				pLastNormalInRun - The last normal node in this run
				pLastComplexInRun - the last complex node in the run
				RenderAllObjects - TRUE if we render all objects, FALSE if its just the selection
				NodesSoFar - the number of nodes rendered so far
				Progress - the Progress display object
	Outputs:	NodesSoFar - The Number of nodes rendered after the Complex ones have been done
	Returns:	SLOWJOB_SUCCESS - if all went well
				SLOWJOB_FAILURE - if all went bad 
				SLOWJOB_USERABORT - if the user press 'esc'
	Purpose:	Renders the masked bitmap part of the process. This function will get called
				many times during the typical rendering of the document (once for each run
				of complex nodes in the document).

	Notes:		All rendering to the bitmap is done in composite colour (by ignoring the
				ColourPlate attached to this view). The bitmap is then plotted into the output
				RenderRegion, at which point it will be separated/corrected according to the
				ColourPlate settings. This is for 2 reasons:
				1) If we didn't stop it here, the output would be colour corrected twice, 
				   and thus come out wrong
				2) Bleach transparency does not work correctly on pre-separated colours,
				   so we must render using composite colour in order to nmake such
				   transparent objects come out correctly.

********************************************************************************************/

SlowJobResult View::RenderOptimalBitmapPhase(DocRect& ClipRect, Matrix& ViewTrans, Spread* pSpread,
											RenderRegion* pRender, Node* pLastNormalInRun,
											Node* pLastComplexInRun, BOOL RenderAllObjects,
											BOOL bPrintPaper, INT32& NodesSoFar, ProgressDisplay* pProgress,
											INT32 TotalProgress)
{
	TRACEUSER( "Gerry", _T("View::RenderOptimalBitmapPhase TotalProgress = %d\n"), TotalProgress);

	INT32 EndProgress = NodesSoFar + TotalProgress;

	// work out the DPI - this is a bit complicated really
	double Dpi = 96.0;
	if (!pRender->IS_KIND_OF(GRenderRegion) && pRender->IsPrinting())
	{
		// we are printing, so ask the print options
		PrintControl *pPrintControl = GetPrintControl();
		ERROR2IF(pPrintControl == NULL, SLOWJOB_FAILURE, "No print control in RenderOptimalView");
		Dpi = (double) pPrintControl->GetDotsPerInch();
	}
	else if (IS_A(pRender, CamelotEPSRenderRegion))
	{
		// Use DPI as set in EPS export options dialog.
		Dpi = (double) EPSFilter::XSEPSExportDPI;
	}
	else
	{
		// Default to something reasonable (only really used for screens).
		Dpi = (double) (72000.0 / PixelWidth.MakeDouble());
	}

	// Markn 27/10/95
	// Scan the layers finding the union of the bounds of all the layers that should be rendered
	// This will be used to calc the Bitmap clip rect
	DocRect BitmapClipRect;
	Layer* pLayer = pSpread->FindFirstLayer();
	while (pLayer != NULL)
	{
		if (pLayer->NeedsToRender(pRender))
			BitmapClipRect = BitmapClipRect.Union(pLayer->GetBoundingRect());

		pLayer = pLayer->FindNextLayer();
	}
	// Make sure that the BitmapClipRect does not incude an area that's not included in the entry ClipRect
	BitmapClipRect = BitmapClipRect.Intersection(ClipRect);

	// Now we make sure the clip rect is pixel aligned
	INT32 TempPixels;
	double dPixelWidth = 72000.0 / Dpi;
	if (BitmapClipRect.Width() < dPixelWidth || BitmapClipRect.Height() < dPixelWidth)
	{
		TRACE( _T("Very small rectangle\n"));
	}
	TempPixels = (INT32)(((double)BitmapClipRect.lo.x) / dPixelWidth);
	BitmapClipRect.lo.x = (INT32)(((double)TempPixels) * dPixelWidth);
	if (BitmapClipRect.Width() < dPixelWidth)
		BitmapClipRect.hi.x = (INT32)(BitmapClipRect.lo.x + dPixelWidth);
	
	TempPixels = (INT32)(((double)BitmapClipRect.lo.y) / dPixelWidth);
	BitmapClipRect.lo.y = (INT32)(((double)TempPixels) * dPixelWidth);
	if (BitmapClipRect.Height() < dPixelWidth)
		BitmapClipRect.hi.y = INT32(BitmapClipRect.lo.y + dPixelWidth);

	// Markn 26/10/95
	// Find out if this render region is printing or not.
	// This is used as a param to the constructors of the bitmap-related render regions so that they
	// have the same printing state as the main render region.
	// If they don't, some nodes that shouldn't render when printing (e.g. background layers) will get 
	// rendered into the bitmaps.
	BOOL Printing = pRender->IsPrinting();

	// During this function, we force the View::GetColourContext function to return default
	// global contexts to anyone who asks, in order to disable any attempts to
	// colour separate or correct the output (we want a normal composite bitmap which
	// will be colour corrected by the main render region when we call PlotBitmap, below)
	BOOL OldForceState = ForceDefaultColourContexts;
	ForceDefaultColourContexts = TRUE;

	// Create a new GDraw context so we don't have to keep stopping and starting our regions
	GDrawAsm TempContext;
	if (!TempContext.Init())
	{
		return SLOWJOB_FAILURE;
	}

	// Use the temporary GDrawContext
	GDrawContext* pOldGD = GRenderRegion::SetTempDrawContext(&TempContext);
	if (pOldGD == NULL)
	{
		TRACEUSER( "Gerry", _T("NULL GDraw context returned from SetTempDrawContext\n"));
	}

	// Feathers require that the ScaleFactor of the render region be set correctly 
	// or they will only render correctly at 100%
	FIXED16 TempScale;
	ViewTrans.Decompose(&TempScale, NULL, NULL, NULL, NULL);

	// Also create a masked mono bitmap and Set them up
	PrintingMaskedRenderRegion MaskedBitmap(BitmapClipRect, ViewTrans, TempScale, Dpi, Printing);
	MaskedBitmap.AttachDevice(this, NULL, pSpread);

	// Create the colour Render Region as well
	GRenderBitmap BitmapRR(BitmapClipRect, ViewTrans, TempScale, 32, Dpi, Printing);
	BitmapRR.AttachDevice(this, NULL, pSpread);

	// ask for the bands to overlap
	BitmapRR.SetOverlapBands(TRUE);
	MaskedBitmap.SetOverlapBands(TRUE);

	// Start the first band in both of our renderregions
	if (!BitmapRR.SetFirstBand() || !MaskedBitmap.SetFirstBand())
	{
		ForceDefaultColourContexts = OldForceState;
		return SLOWJOB_FAILURE;
	}

	// Count the nodes in this complex run
//	BOOL StartCounting = FALSE;
	INT32 TotalBands = MaskedBitmap.GetNumBands();

	// Count the nodes to be rendered so we can calculate the increment to use
	if (!MaskedBitmap.StartRender())
	{
		// Restore the ForceDefaultContext flag
		ForceDefaultColourContexts = OldForceState;
		GRenderRegion::SetTempDrawContext(pOldGD);
		return SLOWJOB_FAILURE;
	}

	// Find out what the host can do
	RRCaps HostCaps;
	pRender->GetRenderRegionCaps(&HostCaps);
	MaskedBitmap.SetHostCaps(&HostCaps);
	INT32 TotalNodes = 0;

	OptimalBitmapRenderCallback CountCallback(this, RenderAllObjects, pLastNormalInRun, pLastComplexInRun, TRUE, 0, &TotalNodes, pProgress, TRUE);
	MaskedBitmap.RenderTree(pSpread, FALSE, FALSE, &CountCallback);

	// Tell the render region we are done rendering
	MaskedBitmap.StopRender();

	double NodesPerBand = 0.0;
	if (TotalBands != 0)
		NodesPerBand = ((double)TotalProgress) / ((double)TotalBands * 3.0);
	double NodeIncrement = 0.0;
	if (TotalNodes != 0)
		NodeIncrement = NodesPerBand / ((double) TotalNodes);

//	TRACEUSER( "Gerry", _T("TotalNodes - %ld\n"), TotalNodes);
//	TRACEUSER( "Gerry", _T("TotalBands - %ld\n"), TotalBands);
//	TRACEUSER( "Gerry", _T("NodesPerBand - %f\n"), NodesPerBand);
//	TRACEUSER( "Gerry", _T("NodeIncrement - %f\n"), NodeIncrement);

	INT32 CurrentBand = 0;

	// loop through all the available bands
	BOOL MoreBands = FALSE;
	do
	{
		// Update the current band variable
		CurrentBand++;
//		TRACEUSER( "Gerry", _T("Printing Band %d of %d\n"),CurrentBand,TotalBands);

		// Start it up
		if (!MaskedBitmap.StartRender())
		{
			// Restore the ForceDefaultContext flag
			ForceDefaultColourContexts = OldForceState;
			GRenderRegion::SetTempDrawContext(pOldGD);
			return SLOWJOB_FAILURE;
		}

		// Find out what the host can do
		RRCaps HostCaps;
		pRender->GetRenderRegionCaps(&HostCaps);
		MaskedBitmap.SetHostCaps(&HostCaps);

		// Fill the mask in white
		DocRect DrawRect = ClipRect;

		// Inflate the rect by 2 pixels
		DrawRect.Inflate((INT32)(2*72000.0/Dpi + 0.5)); 

		// Tell the masked render region that the following nodes are Simple
		MaskedBitmap.SetMaskDrawingMode(FALSE);

		MaskedBitmap.SaveContext();
		DocColour white(255,255,255);
		MaskedBitmap.SetFillColour(white);
		MaskedBitmap.DrawRect(&DrawRect);
		MaskedBitmap.RestoreContext();

		// Find out the size of the Region
		DocRect SizeOfRegion = MaskedBitmap.GetClipRect();

		// start actually rendering nodes in to the mask. Just the ones in this
		// complex run are actually rendering solid into the mask. All other nodes,
		// before and after the run are punched out.
		double RegionArea = (double)SizeOfRegion.Width() * (double)SizeOfRegion.Height();
		OptimalBitmapRenderCallback MaskCallback(this, RenderAllObjects, pLastNormalInRun, pLastComplexInRun, TRUE, RegionArea, &NodesSoFar, pProgress, FALSE, NodeIncrement);
		MaskedBitmap.RenderTree(pSpread, FALSE, FALSE, &MaskCallback);

		ENSURE(MaskedBitmap.GetCaptureDepth()==1, "Mask capture nesting not unwound correctly in RenderOptimalBitmapPhase\n");

		// Tell the render region we are done rendering
		MaskedBitmap.StopRender();

		// Make the mask a little bigger, to cover any errors from different dpis
		MaskedBitmap.SpreadMask();

#ifdef _DEBUG_ATTACH_OPTIMAL_MASK
OILBitmap* pMaskBitmap = MaskedBitmap.ExtractBitmapCopy();
KernelBitmap* pRealMaskBmp = new KernelBitmap(pMaskBitmap, TRUE);
pRealMaskBmp->AttachDebugCopyToCurrentDocument("TestMask");
delete pRealMaskBmp;
#endif

		// Draw the colour bitmap
		if (MaskedBitmap.FindCoverage()>0)
		{
			// Start rendering into the bitmap
			{
				// Start the render region up.
				BOOL ok = BitmapRR.StartRender();
				if (!ok)
				{
					// Restore the ForceDefaultContext flag
					ForceDefaultColourContexts = OldForceState;
					GRenderRegion::SetTempDrawContext(pOldGD);
					return SLOWJOB_FAILURE;
				}
			}

			// Should draw a big white rectangle here, into the bitmap render region or
			// transparent objects will not fade to white where you can see the paper under them
			// Draw it into the real bitmap
			if (bPrintPaper)
			{
				BitmapRR.SaveContext();
				RenderPaper(&BitmapRR, pSpread);
				BitmapRR.RestoreContext();
			}
			else
			{
				BitmapRR.SaveContext();
				DocColour white(255,255,255);
				BitmapRR.SetFillColour(white);
				BitmapRR.DrawRect(&DrawRect);
				BitmapRR.RestoreContext();
			}

			// We always anti-alias the bitmap output
			QualityAttribute *pAttr = new QualityAttribute();
			pAttr->QualityValue.SetQuality(QUALITY_MAX);
			BitmapRR.SetQuality(pAttr, TRUE);

			OptimalBitmapRenderCallback ImageCallback(this, RenderAllObjects, pLastNormalInRun, pLastComplexInRun, FALSE, RegionArea, &NodesSoFar, pProgress, FALSE, NodeIncrement);
			BitmapRR.RenderTree(pSpread, FALSE, FALSE, &ImageCallback);

			ENSURE(BitmapRR.GetCaptureDepth()==1, "Bitmap capture nesting not unwound correctly in RenderOptimalBitmapPhase\n");

			// Stop rendering
			BitmapRR.StopRender();

			// Get the bitmaps from the render region
			OILBitmap* pFullBitmap = BitmapRR.ExtractBitmap();
			KernelBitmap* pRealBmp = new KernelBitmap(pFullBitmap, TRUE);

#ifdef _DEBUG_ATTACH_OPTIMAL_BITMAP
pRealBmp->AttachDebugCopyToCurrentDocument("TestBmp");
#endif

			// Restore the ForceDefaultContext flag around the output rendering section
			ForceDefaultColourContexts = OldForceState;

			if (GRenderRegion::SetTempDrawContext(pOldGD) != &TempContext)
			{
				TRACEUSER( "Gerry", _T("Not &TempContext when restoring\n"));
			}
			pOldGD = NULL;

//			TRACE( _T("Rendering bitmap with mask\n"));
			// Save the context
			pRender->SaveContext();

			// Render the bitmap using the mask
			DocRect BandClipRect = BitmapRR.GetClipRect();
			SlowJobResult Result = pRender->DrawMaskedBitmap(BandClipRect, pRealBmp, &MaskedBitmap, NULL);
			if (Result != SLOWJOB_SUCCESS)
			{
				// Either something has gone wrong, or an error has occured, so clean up.

				// Get rid of the bitmaps. We have to detach it once, as it was attached 
				// when extracted from the render region.
//				pFullBitmap->Detach();
				delete pRealBmp;
				pRealBmp = NULL;

				return SLOWJOB_FAILURE;
			}

			// restore the context and stop rendering
			pRender->RestoreContext();

			NodesSoFar += (INT32)NodesPerBand;
			if (pProgress)
			{
				if (!pProgress->SetNodesRendered(NodesSoFar))
				{
					TRACE( _T("*****************************\n"));
					TRACE( _T("************ User aborted job\n"));
					TRACE( _T("*****************************\n"));
				}
			}

			ForceDefaultColourContexts = TRUE;

			// Use the temporary GDrawContext
			pOldGD = GRenderRegion::SetTempDrawContext(&TempContext);
			if (pOldGD == NULL)
			{
				TRACEUSER( "Gerry", _T("NULL GDraw context returned from SetTempDrawContext\n"));
			}

			// get rid of the bitmaps. We have to detach it once, as it was attached when extracted from the rr
//			pFullBitmap->Detach();
			delete pRealBmp;
			pRealBmp = NULL;
		}
		else
		{
//			TRACEUSER( "Gerry", _T("No mask coverage\n"));

			NodesSoFar += (INT32)(NodesPerBand * 2.0);
			if (pProgress)
			{
				if (!pProgress->SetNodesRendered(NodesSoFar))
				{
					TRACE( _T("*****************************\n"));
					TRACE( _T("************ User aborted job\n"));
					TRACE( _T("*****************************\n"));
				}
			}
		}

		// Ensure all captures are cleared before setting up next band
		BitmapRR.FreeOffscreenState();

		// Get the next bands ready
		BOOL MoreMaskedBands = MaskedBitmap.GetNextBand();
TRACEUSER( "Phil", _T("GetNextBand (Before) %d, %d\n"), BitmapRR.GetClipRect().lo.y, BitmapRR.GetClipRect().hi.y);
		MoreBands = BitmapRR.GetNextBand();
TRACEUSER( "Phil", _T("GetNextBand (After)  %d, %d\n"), BitmapRR.GetClipRect().lo.y, BitmapRR.GetClipRect().hi.y);
		ERROR3IF(MoreMaskedBands!=MoreBands, "Bands don't match");
        if (MoreMaskedBands!=MoreBands)
	       MoreBands = FALSE;

	} while (MoreBands);


	// Restore the ForceDefaultContext flag
	ForceDefaultColourContexts = OldForceState;

	if (GRenderRegion::SetTempDrawContext(pOldGD) != &TempContext)
	{
		TRACEUSER( "Gerry", _T("Not &TempContext when restoring\n"));
	}
	pOldGD = NULL;
	
	// All went well so makesure the progress is at the right point
	NodesSoFar = EndProgress;
	if (pProgress)
		pProgress->SetNodesRendered(NodesSoFar);

	TRACEUSER( "Gerry", _T("End of View::RenderOptimalBitmapPhase\n"));
	return SLOWJOB_SUCCESS;
}

class SimplePrintRenderCallback : public RenderCallback
{
public:
	SimplePrintRenderCallback(View* pView, Spread* pSpread, BOOL bRenderAll, ScanningRenderRegion* pScanRR, BOOL bDoMasked, BOOL bPrintPaper, INT32* pNodesSoFar, ProgressDisplay* pProgress)
	{
		m_pView = pView;
		m_bRenderAll = bRenderAll;
		m_pScanRR = pScanRR;
		m_pFirstComplex = pScanRR->GetFirstComplexObject();
		m_pLastComplex = pScanRR->GetLastComplexObject();
		m_bDoMasked = bDoMasked;
		m_bPrintPaper = bPrintPaper;
		m_bDoComplex = FALSE;
		m_pSpread = pSpread;
		m_pNodesSoFar = pNodesSoFar;
		m_pProgress = pProgress;
	}

	virtual BOOL BeforeNode(RenderRegion* pRegion, Node* pNode)
	{
//		BOOL bRender = (pNode->IsAnAttribute() || pNode->IsANodeClipView()) && (m_bRenderAll || m_pView->IsPrintableNodeSelected(pNode));
		BOOL bRender = m_bRenderAll || m_pView->IsPrintableNodeSelected(pNode);
//		TRACE( _T("SPRC# BeforeNode    0x%08x - %s	returning %s\n", pNode, pNode->GetRuntimeClass()->GetClassName(), bRender ? "true" : "false"));
		if (pNode == m_pLastComplex)
			m_bDoComplex = TRUE;
		return(bRender);
	}

	virtual BOOL BeforeSubtree(RenderRegion* pRegion, Node* pNode, Node** ppNextNode, BOOL bClip, SubtreeRenderState* pState)
	{
//		TRACE( _T("Warning - SimplePrintRenderCallback::BeforeSubtree called\n") );
//		TRACE( _T("SPRC# BeforeSubtree 0x%08x - %s\n"), pNode, pNode->GetRuntimeClass()->GetClassName());

		if (m_bDoComplex)
		{
			// We need to render the complex span and start skipping
			m_bDoComplex = FALSE;

			DocRect ComplexRect;
			DocRect ClipRect = pRegion->GetClipRect();
			if (m_bDoMasked)
			{
				// Render all the complex bits into a bitmap and mask blit it into the render region
				ComplexRect = m_pScanRR->GetComplexBounds();
				ComplexRect = ComplexRect.Intersection(ClipRect);
			}
			else
			{
				// No masking - just do everything as a rectangular bitmap.
				ComplexRect = ClipRect;
			}

			if (ComplexRect.IsValid())
			{
//				TRACE( _T("Entire complex span rendered here\n"));
//				TRACE( _T("ComplexRect = (%d, %d) - (%d, %d)\n"), ComplexRect.lo.x, ComplexRect.lo.y, ComplexRect.hi.x, ComplexRect.hi.y);
				// Call RenderBitmapPhase to handle this
				SlowJobResult BitmapResult;
				Matrix ViewTrans = pRegion->GetMatrix();
				BitmapResult = m_pView->RenderBitmapPhase(ComplexRect, ViewTrans, m_pSpread, pRegion,
												  m_pFirstComplex, m_pLastComplex, m_bRenderAll,
												  m_bPrintPaper, *m_pProgress);
			}
		}
		
		return(FALSE);
	}

//	virtual BOOL AfterSubtree(RenderRegion* pRegion, Node* pNode);

private:
	View* m_pView;
	Spread* m_pSpread;
	ScanningRenderRegion* m_pScanRR;
	Node* m_pFirstComplex;
	Node* m_pLastComplex;
	BOOL m_bDoMasked;
	BOOL m_bRenderAll;
	BOOL m_bPrintPaper;
	INT32* m_pNodesSoFar;
	ProgressDisplay* m_pProgress;

	BOOL m_bDoComplex;		// TRUE to make next call to BeforeSubtree render the complex phase
};


class SimpleBitmapRenderCallback : public RenderCallback
{
public:
	typedef enum
	{
		RS_BEFORECOMPLEX = 0,
		RS_INCOMPLEX,
		RS_AFTERCOMPLEX,
	} RenderStateType;
	
	SimpleBitmapRenderCallback(View* pView, BOOL bRenderAll, Node* pFirstComplex, Node* pLastComplex, BOOL bRenderMask)
	{
		m_pView = pView;
		m_bRenderAll = bRenderAll;
		m_pFirstComplex = pFirstComplex;
		m_pLastComplex = pLastComplex;
		m_bRenderMask = bRenderMask;
		m_RenderState = RS_BEFORECOMPLEX;
	}

	virtual BOOL BeforeNode(RenderRegion* pRegion, Node* pNode)
	{
//		BOOL bRender = (m_bRenderAll || m_pView->IsPrintableNodeSelected(pNode)) && pNode->NeedsToRender(pRegion);
		BOOL bRender = (m_bRenderAll || m_pView->IsPrintableNodeSelected(pNode));
		if (m_RenderState == RS_AFTERCOMPLEX)
			bRender = FALSE;
		if (m_RenderState == RS_INCOMPLEX && pNode == m_pLastComplex)
		{
//			TRACE( _T("SBRC# Hit LastComplex\n"));
			m_RenderState = RS_AFTERCOMPLEX;
		}

//		if (m_bRenderMask)
//			TRACE( _T("SBRCM# BeforeNode    0x%08x - %s	returning %s\n", pNode, pNode->GetRuntimeClass()->GetClassName(), bRender ? "true" : "false"));
//		else
//			TRACE( _T("SBRCB# BeforeNode    0x%08x - %s	returning %s\n", pNode, pNode->GetRuntimeClass()->GetClassName(), bRender ? "true" : "false"));
		return(bRender);
	}

	virtual BOOL BeforeSubtree(RenderRegion* pRegion, Node* pNode, Node** ppNextNode, BOOL bClip, SubtreeRenderState* pState)
	{
//		if (m_bRenderMask)
//			TRACE( _T("SBRCM# BeforeSubtree 0x%08x - %s\n"), pNode, pNode->GetRuntimeClass()->GetClassName());
//		else
//			TRACE( _T("SBRCB# BeforeSubtree 0x%08x - %s\n"), pNode, pNode->GetRuntimeClass()->GetClassName());

		if (pNode == m_pFirstComplex)
		{
			if (m_RenderState == RS_BEFORECOMPLEX)
			{
//				TRACE( _T("SBRC# Hit FirstComplex\n"));
				// Change state to be in the complex span
				m_RenderState = RS_INCOMPLEX;
			}
		}
		
		return(FALSE);
	}

//	virtual BOOL AfterSubtree(RenderRegion* pRegion, Node* pNode);

private:
	View* m_pView;
	BOOL m_bRenderAll;
	Node* m_pFirstComplex;
	Node* m_pLastComplex;
	BOOL m_bRenderMask;			// TRUE if rendering the mask bitmap
	RenderStateType m_RenderState;
};



/********************************************************************************************

>	RenderViewResult View::RenderSimpleView(RenderRegion* pRender, Matrix& ViewTrans,
								Spread* pSpread, BOOL PrintPaper)


	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		pRender - The render region to render into
				ViewTrans - the transform matrix to use for the rendering
				pSpread - the spread to render
				PrintPaper - TRUE if the paper should be rendered as well
	Returns:	RENDERVIEW_SUCCESS	 - view was rendered successfully
				RENDERVIEW_FAILURE	 - an error occured while rendering the view.
				RENDERVIEW_NOTNEEDED - the view was not rendered as the usual RenderView
									   call can be used instead.
				RENDERVIEW_USERABORT - the user aborted the rendering operation.
									   (e.g. pressing Escape, or aborting the print job)
	Purpose:	This function has been designed along similar lines to the RenderView()
				function found elsewhere in this file. This version of the function is
				slightly different though. This version of the function is able to render
				complex objects, such as transparent objects into normal render regions.
				It does this by first scanning through all the objects that need rendering
				to see if any of them require complex things. If they do, then part of the
				rendering is done into a bitmap and blitted over the rest of the shapes using
				a masked blit. All very complicated really!
	SeeAlso:	DocView::RenderView

********************************************************************************************/

/*
Technical notes:
	The process...

	Build a scanning render region
	Scanning render the document

	see if there were any complex shapes.
		Render the document up to the complex shapes as normal

		Start a masked Bitmap render region
		render the appropriate bits of the document into it

		blit the bitmap into the render region

	render all remaining shapes (may be all the shapes, if there were no complex ones)
*/
RenderViewResult View::RenderSimpleView(RenderRegion* pRender, Matrix& ViewTrans, Spread* pSpread,
										BOOL PrintPaper)
{
#ifndef STANDALONE
//	TRACEUSER( "Gerry", _T("View::RenderSimpleView\n"));

	BOOL bIsOnScreen = pRender->IS_KIND_OF(GRenderRegion);

	// if the preference says we should not be in this function then call the other function
	if (!bIsOnScreen &&	PrintMonitor::PrintMaskType==PrintMonitor::MASK_OPTIMAL)
		return RenderOptimalView(pRender, ViewTrans, pSpread, PrintPaper);

	// Find out what the host render region is capable of
	RRCaps Caps;
	pRender->GetRenderRegionCaps(&Caps);

	// Whether or not the rendering was successful.
	RenderViewResult Result = RENDERVIEW_FAILURE;

	// Find out the Clipping rectangle of the render region to draw into
	DocRect ClipRect = pRender->GetClipRect();

	// Work out whether we need to render all objects, or just the selected ones.
	BOOL RenderAllObjects = TRUE;
PORTNOTE("other", "disabled CCPrintInfo");
#ifndef EXCLUDE_FROM_XARALX
#ifndef WEBSTER
	CCPrintInfo *pPrintInfo = NULL;
	if (!bIsOnScreen && pRender->IsPrinting())
	{
		pPrintInfo = CCPrintInfo::GetCurrent();
//		ERROR2IF(pPrintInfo == NULL, RENDERVIEW_FAILURE, "No print info while printing!");
		if (pPrintInfo != NULL)
		{
			PrintControl *pPrCtrl = pPrintInfo->GetPrintControl();
			RenderAllObjects = (pPrCtrl->GetObjPrintRange() == PRINTRANGEOBJ_ALL);
		}
	}
#endif //webster
#endif
	// Create and set up a new Scanning render region
	ScanningRenderRegion Scanner(pRender->IsPrinting());

	BOOL bDoScan = TRUE;
	
	if (bIsOnScreen)
	{
		// Now check the print method to determine if we need to do a three-pass render
		// Get document pointer
		Document* pDoc = GetDoc();

		// Get print information for this document.
		PrintComponent *pPrint = (PrintComponent *) pDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
		if (pPrint)
		{
			PrintControl *pPrintControl = pPrint->GetPrintControl();
			if (pPrintControl)
			{
				if (pPrintControl->GetPrintMethod() != PRINTMETHOD_NORMAL)
				{
					bDoScan = FALSE;
				}
			}
		}
	}

	// Declare some useful vars before the scanning chunk
//	Node* pFirstInkNode = NULL;
//	Node* pNode = NULL;
	if (bDoScan)
	{
		// Attach a device to the scanning render region
		// Since this rr does no real rendering, it does not need a Device context
		Scanner.AttachDevice(this, (CNativeDC *)NULL, pSpread);

		// Get it ready to render
		Scanner.SetMatrix(ViewTrans);
		Scanner.SetClipRect(ClipRect);
		
		// Start the render region and return if it fails
		if (!Scanner.StartRender())
			return RENDERVIEW_FAILURE;
		
		// and tell the scanning render region about the caps of the host region
		Scanner.SetHostRRCaps(Caps);

		//	WEBSTER-ranbirr-13/11/96
PORTNOTE("other", "disabled CCPrintInfo");
#ifndef EXCLUDE_FROM_XARALX
#ifndef WEBSTER
		// We going to analyse the document
		if (pPrintInfo != NULL)
			pPrintInfo->SetAnalysing();
#endif //webster
#endif
		// Find the first node to render
//		pFirstInkNode = pSpread->FindFirstForUnclippedInkRender(&Scanner);
//		Scanner.SetRenderState(pFirstInkNode);

		ScannerRenderCallback ScanCallback(this, RenderAllObjects, &Scanner);

		// Call RenderTree to do the rendering
		Scanner.RenderTree(pSpread, FALSE, FALSE, &ScanCallback);
		
		// Thats all the nodes rendered, so stop rendering	
		Scanner.StopRender();
	}
	
	//	WEBSTER-ranbirr-13/11/96
PORTNOTE("other", "disabled CCPrintInfo");
#ifndef EXCLUDE_FROM_XARALX
#ifndef WEBSTER
	// We going to print the document now
	if (pPrintInfo != NULL)
		pPrintInfo->SetPrinting();
#endif //webster
#endif

	// Ok, we now have a Scanning render region that has all the info we need to know in it.
	// See if there were any complex shapes in the region
	// If we didn't do a scan then pretend there is one complex node
	/*INT32 NumComplex =*/ bDoScan ? Scanner.GetNumComplex() : 1;

	// Oh well, there are complex shapes to draw, so we had better do the comlpex thing

	pRender->SetMatrix(ViewTrans);
	pRender->SetClipRect(ClipRect);
	
	// Ask the render region not to render complex shapes
	pRender->SetRenderComplexShapes(FALSE);
	
	// Start the render region and return if it fails
	if (!pRender->StartRender())
		return RENDERVIEW_FAILURE;

	// I think it likely we need to put something in here.
	if (pRender->IsPrinting())
	{
		// Render the spreads paper marks where necessary
		RenderPageMarks(pRender, ViewTrans, ClipRect, pSpread);
	}

	// Draw the paper if we have been asked to do so.
	if (PrintPaper)
	{
		// Render all paper nodes from there into the real render region
		pRender->SaveContext();
		RenderPaper(pRender, pSpread);
		pRender->RestoreContext();

		if (pRender->NeedsOSPaper)
		{
			// This render region uses the OS to render paper, so if we don't
			// render anything except paper then there is no need for e.g.
			// GRenderRegions to blit anything.  Hence we call this function
			// to clear the changed bbox.
			pRender->SetClean(TRUE, FALSE);
		}

		// The Paper is now done
		pRender->IsPaperRendered = TRUE;
	}

	// Find the first and last complex node in the document
	Node* pFirstComplexNode = Scanner.GetFirstComplexObject();	
	Node* pLastComplexNode = Scanner.GetLastComplexObject();

	// Create and initialise the progress display.
	// (NB. we don't always do progress display, e.g. for screens we don't bother)
	ProgressDisplay Progress;
//	Progress.SetUp(pRender, &Scanner);
	INT32 NodesRendered = 0;

	// Find the first node to render
//	pFirstInkNode = pSpread->FindFirstForUnclippedInkRender(pRender);
	if (!bDoScan)
	{
		pFirstComplexNode = pSpread;
		pLastComplexNode = NULL;
	}

	TRACE( _T("pFirstComplexNode = 0x%08x\n"), pFirstComplexNode);
	TRACE( _T("pLastComplexNode  = 0x%08x\n"), pLastComplexNode);
	
//	pRender->SetRenderState(pFirstInkNode);

	// Find out about the first node and the first complex node
//	pNode = pRender->GetRenderState();

	// Check to see if we should do a masked blit.
	// We always do it under NT, or to a PostScript printer or EPS file; under other conditions
	// we are controlled by the printing preference.
	BOOL DoMaskedBlit = FALSE;
	if (//IsWin32NT() || 
		bIsOnScreen ||
		(PrintMonitor::PrintMaskType==PrintMonitor::MASK_MASKED) ||
		(CCDC::GetType(pRender->GetRenderDC(), TRUE) == RENDERTYPE_PRINTER_PS) ||
		IS_A(pRender, CamelotEPSRenderRegion))
	{
		DoMaskedBlit = bDoScan;				// Don't render simple phase if not scanning
	}
	SimplePrintRenderCallback MyCallback(this, pSpread, RenderAllObjects, &Scanner, DoMaskedBlit, PrintPaper, &NodesRendered, &Progress);
	pRender->RenderTree(pSpread, FALSE, FALSE, &MyCallback);

	pRender->StopRender();

	// If we get here, it all worked
	Result = RENDERVIEW_SUCCESS;

//ExitRenderSimpleView:

	// Unlink the RenderRegion from list and error if it was not there
	BOOL InList = Camelot.DeleteRenderRegion(pRender);
	ERROR2IF(!InList,RENDERVIEW_FAILURE,"DocView::RenderView tried to delete a RenderRegion not in the list");

	// Instruct the render region to commit suicide. This call doesn't work on a CamelotEPS
	// render region.
	pRender->ConditionalSuicide ();

	// Finish progress bar for EPS export
	if (Result == RENDERVIEW_SUCCESS)
		Progress.AllStagesDone();

//	TRACEUSER( "Gerry", _T("End of View::RenderSimpleView\n"));

	return Result;

#else
	return RENDERVIEW_USERABORT;
#endif
}

/********************************************************************************************

>	SlowJobResult View::RenderBitmapPhase(DocRect& ClipRect, Matrix& ViewTrans, Spread* pSpread,
								 		  RenderRegion* pHostRegion, Node* pFirstComplex, Node* pLastComplex,
								  		  BOOL bRenderAll, BOOL bPrintPaper, ProgressDisplay& Progress)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		ClipRect - the clipping rectangle of the region
				ViewTrans - the matrix to render through
				pSpread - The Spread we are drawing
				pHostRegion - The Render Region we are rendering on behalf of
				pFirstComplex - The first complex node that needs rendering
				pLastComplex - The last complex node that needs rendering
				Progress - the usual progress display object.
	Returns:	TRUE if everything was fabby, FALSE if there was a problem
	Purpose:	This function is called from RenderSimpleView if it ends up having to
				do the 3 phase rendering system. This function performs the bitmap phase,
				where all the objects deemed to be complex by the Host render region are
				rendered into a bitmap using GDraw and a mono, mask bitmap. The bitmap
				is blitted into the host render region using the mask bitmap, minimising
				the amount of bitmaps ending up in the view (important for things like
				postscript that do not really like bitmaps very much).
	SeeAlso:	DocView::RenderSimpleView; ProgressDisplay

********************************************************************************************/

/*
	IMPORTANT TECHNICAL NOTE:

	Note that we must scan the tree without using the region's clip rectangle, because otherwise
	we will miss the 'last complex node' when searching, because e.g. it is outside the
	clipping rectangle of a particular band, so we will keep rendering until we reach the top 
	of the drawing object stack, which is bad.  Therefore, we scan without doing region tests,
	but we check whether the node actually needs to render using the current band's
	clipping region.

	This means we don't render too much in each band, and we stop rendering when we should.
*/

SlowJobResult View::RenderBitmapPhase(DocRect& ComplexClipRect, Matrix& ViewTrans, Spread* pSpread,
							 		  RenderRegion* pHostRegion, Node* pFirstComplex, Node* pLastComplex,
							 		  BOOL bRenderAll, BOOL bPrintPaper, ProgressDisplay& Progress)
{
#ifndef STANDALONE
//	TRACEUSER( "Gerry", _T("View::RenderBitmapPhase\n"));

	BOOL bIsOnScreen = pHostRegion->IS_KIND_OF(GRenderRegion);

	ERROR3IF(!bIsOnScreen &&
			 PrintMonitor::PrintMaskType==PrintMonitor::MASK_OPTIMAL,
			"PrintMaskType must be SIMPLE or MASKED here\n");

	// This needs to be done in 2 phases - one to create the bitmap we will blit into the
	// host render region and one to create the mask to show us which parts of the bitmap
	// need to be blitted. First we will create the bitmap to blit into the host.
	// create the bitmap render region
//	CNativeDC* pDC = pHostRegion->GetRenderDC();

	double Dpi;
	if (bIsOnScreen)
	{
		Dpi = (double) (72000.0 / PixelWidth.MakeDouble());
//		Dpi = Dpi / 4.0;
	}
	else if (pHostRegion->IsPrinting())
	{
		PrintControl *pPrintControl = GetPrintControl();
		ERROR2IF(pPrintControl == NULL, SLOWJOB_FAILURE, "No print control in RenderBitmapPhase()");
		Dpi = (double) pPrintControl->GetDotsPerInch();
	}
	else if (IS_A(pHostRegion, CamelotEPSRenderRegion))
	{
		// Use DPI as set in EPS export options dialog.
		Dpi = (double) EPSFilter::XSEPSExportDPI;
	}
	else
	{
		// Default to something reasonable (only really used for screens).
		Dpi = (double) (72000.0 / PixelWidth.MakeDouble());
	}

	BOOL bDoScan = TRUE;
	if (bIsOnScreen)
	{
		// Now check the print method to determine if we need to do a three-pass render
		// Get document pointer
		Document* pDoc = GetDoc();

		// Get print information for this document.
		PrintComponent *pPrint = (PrintComponent *) pDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
		if (pPrint)
		{
			PrintControl *pPrintControl = pPrint->GetPrintControl();
			if (pPrintControl)
			{
				if (pPrintControl->GetPrintMethod() != PRINTMETHOD_NORMAL)
				{
					bDoScan = FALSE;
				}
			}
		}
	}

	// Check to see if we should do a masked blit.
	// We always do it under NT, or to a PostScript printer; under other conditions
	// we are controlled by the printing preference.
	BOOL DoMaskedBlit = FALSE;
	if (//IsWin32NT() || 
		bIsOnScreen ||
		(PrintMonitor::PrintMaskType==PrintMonitor::MASK_MASKED) ||
		(CCDC::GetType(pHostRegion->GetRenderDC(), TRUE) == RENDERTYPE_PRINTER_PS) ||
		IS_A(pHostRegion, CamelotEPSRenderRegion))
	{
		DoMaskedBlit = TRUE;
	}

	// Work out the clip rect to use
	DocRect ClipRect = ComplexClipRect;
	ClipRect.Inflate((INT32)(2*72000.0/Dpi + 0.5));

	// We will need the clip rect of the whole region too
	DocRect RClipRect = pHostRegion->GetRegionRect();

//	pHostRegion->StopRender();

	// During this function, we force the View::GetColourContext function to return default
	// global contexts to anyone who asks, in order to disable any attempts to
	// colour separate or correct the output (we want a normal composite bitmap which
	// will be colour corrected by the main render region when we call PlotBitmap, below)
	BOOL OldForceState = ForceDefaultColourContexts;
	ForceDefaultColourContexts = TRUE;

	// Create a new GDraw context so we don't have to keep stopping and starting our regions
	GDrawAsm TempContext;
	if (!TempContext.Init())
	{
		ForceDefaultColourContexts = OldForceState;
		return SLOWJOB_FAILURE;
	}

	// Replace the static GDraw context with our temp one
//	GDrawContext* pOldGD = NULL;

	GDrawContext* pOldGD = GRenderRegion::SetTempDrawContext(&TempContext);
	if (pOldGD == NULL)
	{
		TRACEUSER( "Gerry", _T("NULL GDraw context returned from SetTempDrawContext\n"));
	}

	// Feathers require that the ScaleFactor of the render region be set correctly 
	// or they will only render correctly at 100%
	FIXED16 TempScale;
	ViewTrans.Decompose(&TempScale, NULL, NULL, NULL, NULL);

	// Set it up
	GRenderBitmap BitmapRR(ClipRect, ViewTrans, TempScale, 32, Dpi);
	BitmapRR.AttachDevice(this, NULL, pSpread);

	// Update progress display if necessary.
	Progress.StartBitmapPhase(BitmapRR.GetNumBands());

	// Also create a masked mono bitmap and Set them up
	MaskedRenderRegion MaskedBitmap(ClipRect, ViewTrans, TempScale, Dpi);
	MaskedBitmap.AttachDevice(this, NULL, pSpread);

	// ask for the bands to overlap
	BitmapRR.SetOverlapBands(TRUE);
	MaskedBitmap.SetOverlapBands(TRUE);

	// Start the first band
	if (!BitmapRR.SetFirstBand())
	{
		ForceDefaultColourContexts = OldForceState;
		return SLOWJOB_FAILURE;
	}

	// in the masked region as well
	if (!MaskedBitmap.SetFirstBand())
	{
		ForceDefaultColourContexts = OldForceState;
		return SLOWJOB_FAILURE;
	}

	INT32 CurrentBand = 0;

	// loop through all the available bands
	BOOL MoreBands = FALSE;
	do
	{
		CurrentBand++;
//		TRACEUSER( "Gerry", _T("Printing Band %d\n"),CurrentBand);

		// Start rendering into the bitmap
		if (!BitmapRR.StartRender())
		{
			GRenderRegion::SetTempDrawContext(pOldGD);
			ForceDefaultColourContexts = OldForceState;
			return SLOWJOB_FAILURE;
		}

		// Should draw a big white rectangle here, into the bitmap render region or
		// transparent objects will not fade to white where you can see the paper under them
		DocRect DrawRect = ClipRect;
		// Inflate the rect by 2 pixels
		DrawRect.Inflate( (INT32)(2*72000.0/Dpi + 0.5) );

		if (bPrintPaper)
		{
			BitmapRR.SaveContext();
			RenderPaper(&BitmapRR, pSpread);
			BitmapRR.RestoreContext();
		}
		else
		{
			// Draw it into the real bitmap
			BitmapRR.SaveContext();
			DocColour white(255,255,255);
			BitmapRR.SetFillColour(white);
			BitmapRR.DrawRect(&DrawRect);
			BitmapRR.RestoreContext();
		}

		// We anti-alias the transparency mask 
		QualityAttribute *pAttr = new QualityAttribute();
		pAttr->QualityValue.SetQuality(QUALITY_MAX);
		BitmapRR.SetQuality(pAttr, TRUE);
//		BitmapRR.RRQuality.SetQuality(QUALITY_MAX);
//		BitmapRR.SetQualityLevel();

		RRCaps HostCaps;
		pHostRegion->GetRenderRegionCaps(&HostCaps);

		SimpleBitmapRenderCallback ImageCallback(this, bRenderAll, pFirstComplex, pLastComplex, FALSE);
		BitmapRR.RenderTree(pSpread, FALSE, FALSE, &ImageCallback);

		// Stop rendering
		BitmapRR.StopRender();

		// Start it up
		if (!MaskedBitmap.StartRender())
		{
			GRenderRegion::SetTempDrawContext(pOldGD);
			ForceDefaultColourContexts = OldForceState;
			return SLOWJOB_FAILURE;
		}

		// Now, either we are rendering a masked bitmap, in which case we only want the complex
		// objects to render in the mask, or we are rendering the objects with a rectangular
		// bitmap, in which case we want *all* objects to render into the mask.
		if (DoMaskedBlit)
		{
			// Find out what the host can do
			pHostRegion->GetRenderRegionCaps(&HostCaps);
		}
		else
		{
			// Mark this region as not able to do anything, so the bitmap render region
			// will do everything for it (i.e. all objects will render into the mask).
			HostCaps.CanDoNothing();
		}

		MaskedBitmap.SetHostCaps(&HostCaps);

		// Fill the mask in "simple"
		MaskedBitmap.SaveContext();
		DocColour white(255,255,255);
		MaskedBitmap.SetFillColour(white);
		MaskedBitmap.DrawRect(&DrawRect);
		MaskedBitmap.RestoreContext();

		if (!bDoScan)
		{
			HostCaps.CanDoNothing();
			MaskedBitmap.SetHostCaps(&HostCaps);
		}

		SimpleBitmapRenderCallback MaskCallback(this, bRenderAll, pFirstComplex, pLastComplex, TRUE);
		MaskedBitmap.RenderTree(pSpread, FALSE, FALSE, &MaskCallback);

		// Tell the render region we are done rendering
		MaskedBitmap.StopRender();
		
		// Make the mask a little bigger, to cover any errors from different dpis
		MaskedBitmap.SpreadMask();

		// Get the mask bitmap from the render region
//		OILBitmap* pMaskBitmap = MaskedBitmap.ExtractBitmapCopy();
//		KernelBitmap* pRealMaskBmp = new KernelBitmap(pMaskBitmap, TRUE);
//		pRealMaskBmp->AttachDebugCopyToCurrentDocument("TestMask");
//		delete pRealMaskBmp;
		
		// Get the bitmaps from the render region
		OILBitmap* pFullBitmap = BitmapRR.ExtractBitmap();
		KernelBitmap* pRealBmp = new KernelBitmap(pFullBitmap, TRUE);

//		pRealBmp->AttachDebugCopyToCurrentDocument("TestBmp");

		// Restore the ForceDefaultContext flag around the output rendering section
		ForceDefaultColourContexts = OldForceState;
		
		// Restore the original GDrawContext
		if (GRenderRegion::SetTempDrawContext(pOldGD) != &TempContext)
		{
			TRACEUSER( "Gerry", _T("Not &TempContext when restoring\n"));
		}
		pOldGD = NULL;

		// Now render the bitmap we have into the host render region.
//		if (pHostRegion->StartRender())
//		{
			// Save the context
			pHostRegion->SaveContext();

			// Render the bitmap using the mask
			DocRect BandClipRect = BitmapRR.GetClipRect();
//			TRACE( _T("BandClipRect = (%d, %d) - (%d, %d)\n"), BandClipRect.lo.x, BandClipRect.lo.y, BandClipRect.hi.x, BandClipRect.hi.y);
//			TRACE( _T("BandMatrix = \n"));
//			Matrix HostMatrix = pHostRegion->GetMatrix();
//			HostMatrix.Dump();
			SlowJobResult Result = pHostRegion->DrawMaskedBitmap(BandClipRect, pRealBmp, 
																 &MaskedBitmap, &Progress);

			if (Result != SLOWJOB_SUCCESS)
			{
				// Either something has gone wrong, or an error has occured, so clean up.

				// Get rid of the bitmaps. We have to detach it once, as it was attached 
				// when extracted from the render region.
//				pFullBitmap->Detach();
				delete pRealBmp;
				pRealBmp = NULL;

				return Result;
			}

			// restore the context and stop rendering
			pHostRegion->RestoreContext();
//			pHostRegion->StopRender();
//		}

		ForceDefaultColourContexts = TRUE;

		// Make sure we are using the temporary GDrawContext
		pOldGD = GRenderRegion::SetTempDrawContext(&TempContext);
		if (pOldGD == NULL)
		{
			TRACEUSER( "Gerry", _T("NULL GDraw context returned from SetTempDrawContext\n"));
		}

		// get rid of the bitmaps. We have to detach it once, as it was attached when extracted from the rr
//		pFullBitmap->Detach();
		delete pRealBmp;
		pRealBmp = NULL;

		// Ensure all captures are cleared before setting up next band.
		MaskedBitmap.FreeOffscreenState();
		BitmapRR.FreeOffscreenState();

		// Get the next bands ready
		BOOL MoreMaskedBands = MaskedBitmap.GetNextBand();
		MoreBands = BitmapRR.GetNextBand();
        CAM_USE(MoreMaskedBands); // suppress unused variable wanring on retail builds
		ERROR3IF(MoreMaskedBands!=MoreBands, "Bands don't match");

		// Update progress display
		if (!Progress.EndBitmapPhaseBand())
		{
			ForceDefaultColourContexts = OldForceState;
			// User has aborted operation
			return SLOWJOB_USERABORT;
		}

	} while (MoreBands);

#endif

	// Restore the ForceDefaultContext flag
	ForceDefaultColourContexts = OldForceState;

	// Restore the original GDrawContext
	if (GRenderRegion::SetTempDrawContext(pOldGD) != &TempContext)
	{
		TRACEUSER( "Gerry", _T("Not &TempContext when restoring\n"));
	}
	pOldGD = NULL;

//	pHostRegion->StartRender();
	
//	TRACEUSER( "Gerry", _T("End of View::RenderBitmapPhase\n"));

	// all worked
	return SLOWJOB_SUCCESS;
}


/********************************************************************************************

>	SlowJobResult View::RenderSimpleNodesUnclipped(Node *pNode, RenderRegion *pRender,
							 ProgressDisplay& Progress, Node *pLastComplexNode = NULL)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/2004
	Inputs:		pNode - the node to start rendering at.
				pRender - the render region to render the nodes into.
				Progress - usual progress info object.
				pLastComplexNode - optional: if not NULL, then stop rendering when this
											 node is reached, otherwise render to completion.
	Returns:	SLOWJOB_SUCCESS if rendered ok;
				SLOWJOB_FAILURE if an error occured;
				SLOWJOB_USERABORT if the user intervened - e.g. pressed Escape.
	Purpose:	Function to render nodes 'normally' during 3-pass rendering.  This copes
				with stopping at the specified last complex node, and with only printing
				selected objects if the user has requested it.
	SeeAlso:	ProgressInfo; View::RenderSimpleView

********************************************************************************************/

SlowJobResult View::RenderSimpleNodesUnclipped(Node *pNode, RenderRegion *pRender,
							 ProgressDisplay& Progress, Node *pLastComplexNode)
{
#ifndef STANDALONE
	// Work out whether we need to render all objects, or just the selected ones.
	BOOL RenderAllObjects = TRUE;
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
PORTNOTE("printing", "Disabled printing")
#ifndef EXCLUDE_FROM_XARALX
	if (pRender->IsPrinting())
	{
		CCPrintInfo *pPrintInfo = CCPrintInfo::GetCurrent();
//		ERROR2IF(pPrintInfo == NULL, SLOWJOB_FAILURE, "No print info while printing!");
		if (pPrintInfo != NULL)
		{
			PrintControl *pPrCtrl = pPrintInfo->GetPrintControl();
			RenderAllObjects = (pPrCtrl->GetObjPrintRange() == PRINTRANGEOBJ_ALL);
		}
	}
#endif
#endif //webster
	// Render nodes into specified render region
	while ((pNode!=NULL) && (pNode != pLastComplexNode))
	{
		if (pNode->IsRenderable() && (RenderAllObjects || IsPrintableNodeSelected(pNode)) && pNode->NeedsToRender(pRender))
		{
			// Render the node that we have
			pNode->Render(pRender);

			// If progress display is needed, update it if it's time.
			if (!Progress.IncProgress())
				return SLOWJOB_USERABORT;
		}

		// Find another one to render
		pNode = pNode->FindNextForUnclippedInkRender(pRender);
	}

	// All ok
	return SLOWJOB_SUCCESS;
#else
	return SLOWJOB_FAILURE;
#endif
}


/********************************************************************************************

>	PrintControl *View::GetPrintControl()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/09/95
	Returns:	Pointer to the PrintControl object, or 
				NULL if none found, or the view is not attached to a document.
	Purpose:	Simple way of getting the PrintControl object associated with this view's
				document.
	Errors:		If no print control found => ERROR2

********************************************************************************************/

PrintControl *View::GetPrintControl()
{
#ifndef STANDALONE

	if (pDoc == NULL)
		// This view is not attached to a document (e.g. it's a DialogView)
		return NULL;

	// Find print control object for this document, to see if we are
	// printing via bitmap, and so if we need to band the output.
	PrintComponent *pPrintComponent = (PrintComponent *)
		pDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
	ERROR2IF(pPrintComponent == NULL, NULL, 
			 "Unable to find PrintComponent object in document.");

	PrintControl *pPrintControl = pPrintComponent->GetPrintControl();
	ERROR2IF(pPrintControl == NULL, NULL, 
			 "Unable to find PrintControl object in document component.");

	return pPrintControl;

#else
	return NULL;
#endif
}




/********************************************************************************************

>	void ProgressDisplay::SetUpOptimal(RenderRegion *pRender, ScanningRenderRegion* pScanner)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/95
	Inputs:		pRender - the render region we are rendering to
				pScanner - the Scanning render region that knows about the things we are rendering
	Purpose:	This function sets up the Progress display ready to start rendering the nodes.
	SeeAlso:	ProgressDisplay::SetUp

********************************************************************************************/

void ProgressDisplay::SetUpOptimal(RenderRegion *pRender, ScanningRenderRegion* pScanner)
{
#ifndef STANDALONE
	// Work out if we need a progress display
	IsPrinting = pRender->IsPrinting();
	if (!IsPrinting && !IS_A(pRender, CamelotEPSRenderRegion))
		// No - stop here
		return;

	// We need a progress display
	DoProgressDisplay = TRUE;
	DocRect ClipRect = pRender->GetClipRect();
	Spread *pSpread = pRender->GetRenderSpread();
//	Node *pNode = pSpread->FindFirstForClippedInkRender(&ClipRect, pRender);
	TotalNodes = 0;

	// Work out whether to count just selected objects, or all of them.
	BOOL CountAllObjects = TRUE;
PORTNOTE("printing","Disabled CCPrintInfo")
#ifndef EXCLUDE_FROM_XARALX
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	if ((IsPrinting) && (pPrintInfo!=NULL))
	{
		PrintControl *pPrCtrl = pPrintInfo->GetPrintControl();
		CountAllObjects = (pPrCtrl->GetObjPrintRange() == PRINTRANGEOBJ_ALL);
	}
#endif //webster
#endif

	OptimalPrintRenderCallback MyCallback(pRender->GetRenderView(), pSpread, CountAllObjects, pScanner, FALSE, &TotalNodes, NULL, TRUE);
	pRender->RenderTree(pSpread, FALSE, FALSE, &MyCallback);

	TRACE( _T("TotalNodes = %d\n"), TotalNodes);

PORTNOTE("printing","Disabled CCPrintInfo")
#ifndef EXCLUDE_FROM_XARALX
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	// Set up the slider according to the printing flag
	if (IsPrinting)
	{
//		if (pPrintInfo == NULL)
//		{
//			ERROR2RAW("No PrintInfo object found!");
//			InformError();
//			return;
//		}

		if (pPrintInfo != NULL)
		{
			// actually set the slider
			TRACEUSER( "Gerry", _T("Setting Progress slider up to %ld\n"), TotalNodes * ProgressScaleFactor);
			pPrintInfo->SetSliderSubRangeMax(TotalNodes * ProgressScaleFactor);
			pPrintInfo->SetSliderSubRangePos(0);
		}
	}
	else
#endif //webster
#endif
	{
		// Start progress display (with no initial delay) for Camelot EPS export
		String_64 ExportMsg(_R(IDT_EXPORTMSG_CAMEPS));
		BeginSlowJob(TotalNodes*ProgressScaleFactor, FALSE, &ExportMsg);
	}

	// Provide a progress update for every 1%
	ProgressInterval = (TotalNodes * ProgressScaleFactor) / 100;
	TRACE( _T("ProgressInterval = %d\n"), ProgressInterval);
#endif
}



/********************************************************************************************

>	BOOL ProgressDisplay::SetNodesRendered(INT32 NumNodes)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/95
	Inputs:		NumNodes - the number of nodes that have been rendered so far
	Returns:	TRUE if all went well, FALSE if we should stop now.
	Purpose:	Lets the rendering loop tell the display system when a node has been rendered.
				It can act on this to update the progress display whenever needed.

********************************************************************************************/

BOOL ProgressDisplay::SetNodesRendered(INT32 NumNodes)
{
#ifndef STANDALONE
	// If we have advanced a node, then update
	if (NumNodes>NumNodesRendered)
	{
//		TRACE( _T("SetNodesRendered = %d\n"), NumNodes);
		NumNodesRendered = NumNodes;
	}

	if (!DoProgressDisplay)
		// No progress display needed.
		return TRUE;
PORTNOTE("printing","Disabled CCPrintInfo")
#ifndef EXCLUDE_FROM_XARALX
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	if (IsPrinting && (pPrintInfo != NULL) && (pPrintInfo->m_bContinuePrinting == FALSE))
		// User has cancelled job
		return FALSE;
#endif //webster
#endif
	if ((NumNodesRendered * ProgressScaleFactor) > (LastProgressUpdate + ProgressInterval))
	{
		// Time to update the progress display.
		LastProgressUpdate = NumNodesRendered * ProgressScaleFactor;
	//	WEBSTER-ranbirr-13/11/96
PORTNOTE("printing","Disabled CCPrintInfo")
#ifndef EXCLUDE_FROM_XARALX
#ifndef WEBSTER
		if (IsPrinting && pPrintInfo != NULL)
		{
			// Update slider
			pPrintInfo->SetSliderSubRangePos(LastProgressUpdate);

			// Does user want to suspend printing?
			if (pPrintInfo->Abort())
				return FALSE;
		}
		else
#endif //webster
#endif
			return ContinueSlowJob(LastProgressUpdate);
	}

#endif

	// All ok
	return TRUE;
}

