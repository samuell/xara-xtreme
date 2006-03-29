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
/*
	
	zoomops.cpp

	Zoom tool operations.

*/

#include "camtypes.h"
#include "zoomops.h"
#include "zoomtool.h"

//#include "xlong.h"
#include "wrkcoord.h"
#include "wrkrect.h"
#include "doccoord.h"

//#include "app.h"
#include "csrstack.h"
//#include "document.h"
#include "docview.h"
#include "spread.h"
//#include "progress.h"
#include "selector.h"		// for static InflateByBlobBorder function

//#include "ngcore.h"
//#include "ngitem.h"

//#include "zoomres.h"
//#include "justin.h"
//#include "justin3.h"
//#include "mario.h"			// for _R(IDE_NOMORE_MEMORY)
#include "bubbleid.h"

#if !defined(EXCLUDE_FROM_RALPH)
//#include "rulers.h"
#endif

#ifdef _BATCHING   
//#include "sglcart.h"
//#include "camdoc.h"
#endif

#include "brushmsg.h"
//#include "ngscan.h" // For CreateDisplayScan()


// Revision information.
DECLARE_SOURCE("$Revision: 662 $");

// Standard preliminaries.
CC_IMPLEMENT_DYNCREATE(OpZoom, OpCentredDragBox)
CC_IMPLEMENT_DYNCREATE(OpZoomIn, OpZoom)
CC_IMPLEMENT_DYNCREATE(OpZoomOut, OpZoom)

#if !defined(EXCLUDE_FROM_RALPH)
CC_IMPLEMENT_DYNCREATE(ZoomInfoBarOp, InformationBarOp)
#endif

#define new CAM_DEBUG_NEW


// This makes the computer go "beep!".
//extern void Beep();


////////////////////////////////////////////////////////////////////////////////////////////
// class OpZoom

/*********************************************************************************************
	Preference:	FixZoomClickPoint
	Section:	Zoom Tool
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Range:		FALSE or TRUE
	Purpose:	When zooming in or out with a mouse click, this determines whether to fix
				the clicked point in the resulting zoomed view at the same screen-relative
				position, or to centre the zoomed view on the clicked point.  By default
				this is FALSE (don't fix, do centre).
**********************************************************************************************/

BOOL OpZoom::m_fFixZoomClickPoint = FALSE;



/**************************************************************************************
	Preference:	RadialZoomDragBox
	Section:	Zoom Tool
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Range:		FALSE or TRUE
	Purpose:	Determines how the zoom drag operation invokes by the zoom tool will
				draw its drag box.  If FALSE (the default) then the tool will draw the
				box from one corner to its opposite.  This is similar to the selector's
				drag box.  If TRUE then the tool will draw its centre box centred on
				the point where the drag started and extending to the current mouse
				position.  The effect is to centre the zoom on the first clicked point.
***************************************************************************************/

BOOL OpZoom::m_fRadialZoomDragBox = FALSE;



/**************************************************************************************
	Preference:	ShowZoomInOutCursors
	Section:	Zoom Tool
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Range:		0 ... 1
	Purpose:	Determines whether to flash a "zooming-in" or "zooming-out" cursor
				when a zoom is being calculated, and a special drag cursor when dragging
				in the zoom tool.
				
				Some people think this adds a groovy, polished feel to the program.
				Others don't, which is why this preference is 0 by default (ie. no
				special zoom cursors).
***************************************************************************************/

UINT32 OpZoom::m_nfShowZoomCursors = 0;


// The zoom array holds the 'prefered' scale factors. As we zoom in and out we will
// try and zoom to one of these values first and then step along the table.
INT32 OpZoom::ZoomTable[cZoomTableSize] =
{
	25601,					// probably the highest zoom factor in the world . . .
	16000,
	8000,
	4000,					// these are PERCENTAGES of a view's normal size.
	2000,
	1000,
	500,
	200,
	100,					// the default, 100%, at position cDefaultZoomIndex
	75,
	50,
	25,
	10,
};

                                      

/********************************************************************************************
>	OpZoom::OpZoom()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Purpose:	Default constructor.  To run a zoom drag, call the DoDrag() function.
********************************************************************************************/

OpZoom::OpZoom()
	  :	m_csrZoomIn((Tool_v1*)NULL, _R(IDCSR_ZOOM_IN)),				// we only need these cursors when an OpZoom
	 	m_csrZoomOut((Tool_v1*)NULL, _R(IDCSR_ZOOM_OUT)),			
		m_csrZoomDrag((Tool_v1*)NULL, _R(IDCSR_ZOOM_DRAG))			// is going to be run
{
//	ERROR3IF(!m_csrZoomIn.IsValid() || !m_csrZoomOut.IsValid() || !m_csrZoomDrag.IsValid(),
//				"Failed to load cursor(s) in OpZoom::OpZoom");
}

/********************************************************************************************

>	virtual void OpZoom::Do(OpDescriptor* pZoomOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/95
	Inputs:		pointer to the OpZoomDescriptor being invoked
	Purpose:	Performs the zoom operation
	SeeAlso:	OpZoomDescriptor::FakeInvoke;

********************************************************************************************/
			
void OpZoom::Do(OpDescriptor *pOpDesc)
{
	// Do what FakeInvoke does but call the DoZoom function instead of HandleButtonMsg for
	// the zoom descriptor.
	OpZoomDescriptor* pZoomOpDesc = (OpZoomDescriptor*) pOpDesc;
	if (pZoomOpDesc != 0 && pZoomOpDesc->IsAvailable())
	{
		// DoZoom function must End the operation.
		pZoomOpDesc->DoZoom(this);
	}
}


/********************************************************************************************
>	static void OpZoom::SpreadToWork(Spread* pspdIn, const DocCoord& dcIn,
									 WorkCoord* pwcOut)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		pspdIn		the spread containing the document coordinate
				dcIn		the position within the spread
	Outputs:	pwcOut		the equivalent work coordinate in the current view
	Returns:	-
	Purpose:	Converts a document spread coordinate into a work coordinate in the
				current view.
	SeeAlso:	OpZoom::WorkToSpread
********************************************************************************************/

void OpZoom::SpreadToWork(Spread* pspdIn, const DocCoord& dcIn, WorkCoord* pwcOut)
{
	// Caller must provide a location to store the converted coordinate.
	ERROR3IF(pwcOut == 0, "No output in OpZoom::SpreadToWork");

	// Find the current DocView.
	DocView* pDocView = DocView::GetCurrent();
	ERROR3IF(pDocView == 0, "No current DocView in OpZoom::SpreadToWork");

	// Convert.
	if (pDocView && pwcOut)
		*pwcOut = ((DocCoord&) dcIn).ToWork(pspdIn, pDocView);
}



/********************************************************************************************
>	static void OpZoom::SpreadToWork(Spread* pspdIn, const DocRect& drIn,
									 WorkRect* pwrOut)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		pspdIn		the spread containing the document rectangle
				dcIn		the rectangle within the spread
	Outputs:	pwcOut		the equivalent work rectangle in the current view
	Returns:	-
	Purpose:	Converts a document spread rectangle into a work rectangle in the
				current view.
	SeeAlso:	OpZoom::WorkToSpread
********************************************************************************************/

void OpZoom::SpreadToWork(Spread* pspdIn, const DocRect& drIn, WorkRect* pwrOut)
{
	// Convert low and high corners of the rectangle.
	SpreadToWork(pspdIn, drIn.lo, &pwrOut->lo);
	SpreadToWork(pspdIn, drIn.hi, &pwrOut->hi);
}



/********************************************************************************************
>	static INT32 OpZoom::GetPresetZoomPercent(INT32 nPreset)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		nPreset			which preset zoom factor, with 0 the highest and
								cZoomTableSize-1 the lowest.
	Returns:	A standard preset zoom factor, as a percentage.
	Purpose:	
********************************************************************************************/

INT32 OpZoom::GetPresetZoomPercent(INT32 nPreset)
{
	ERROR3IF(nPreset < 0 || nPreset >= cZoomTableSize,
				"Bad preset in OpZoom::GetPresetZoomPercent");
	return ZoomTable[nPreset];
}



/********************************************************************************************
>	static FIXED16 OpZoom::GetPresetZoomScale(INT32 nPreset)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		nPreset			which preset zoom factor, with 0 the highest and
								cZoomTableSize-1 the lowest.
	Returns:	A standard preset zoom factor, as a scale factor.
	Purpose:	
********************************************************************************************/

FIXED16 OpZoom::GetPresetZoomScale(INT32 nPreset)
{
	ERROR3IF(nPreset < 0 || nPreset >= cZoomTableSize,
				"Bad preset in OpZoom::GetPresetZoomScale");
	return ((FIXED16) ZoomTable[nPreset]) / 100;
}



/********************************************************************************************
>	BOOL OpZoom::DoDrag(Spread* pStartSpread,
						const DocCoord& dcStartPos,
						ClickModifiers cmods)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		pStartSpread		the spread containing the initial mouse postion
				dcStartPos			the initial mosue position, ie, where the drag started
				cmods				the state of the mouse modifier keys
	Purpose:	Starts an auto-scroll drag operation at the given mouse position.  Note that
				this function hides the base-class DoDrag() function.
	SeeAlso:	OpDragBox::DoDrag
********************************************************************************************/

BOOL OpZoom::DoDrag(Spread* pStartSpread, const DocCoord& dcStartPos, ClickModifiers cmods)
{
	return OpDragBox::DoDrag(DRAGTYPE_AUTOSCROLL, pStartSpread, dcStartPos, cmods);
}



/********************************************************************************************
>	BOOL OpZoom::OnDragStarted(Spread*, const Doccoord&, ClickModifiers)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		(not used)
	Returns:	TRUE (always)
	Purpose:	Called by OpZoom's base class, OpDragBox, when a drag has been started.
				Sets the status line text to a helpful comment.
********************************************************************************************/

BOOL OpZoom::OnDragStarted(Spread*, const DocCoord&, ClickModifiers)
{
	// Remember where the drag started.  We have to "bodge" this as it is apparently too
	// difficult to translate document coordinates to work coordinates during a drag, so
	// we can't use the mouse position info passed to us by the OpDragBox base-class.
	DocView* pDocView = DocView::GetCurrent();
	ERROR3IF(pDocView == 0, "No current DocView in OpZoom::OnDragStarted");
	m_wcStartPos = pDocView->GetClickWorkCoord();

	// Reset this flag.
	m_fStatusTextShown = FALSE;

	// Possibly set the zoom-cursor, saving the old one, and return success.
	if (m_nfShowZoomCursors) m_ncsrSaveDrag = CursorStack::GPush(&m_csrZoomDrag);
	return TRUE;
}



/********************************************************************************************
>	BOOL OpZoom::OnPointerMoved(Spread*, const DocRect&, ClickModifiers)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Inputs:		(not used)
	Returns:	TRUE, meaning continue dragging.
	Purpose:	If this function is called then a drag has definitely started, so update
				the status bar.  We don't do this on a button-down event.
********************************************************************************************/

BOOL OpZoom::OnPointerMoved(Spread*, const DocRect&, ClickModifiers)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// The first time through this function set the status-line text.  We don't really
	// want to do this on a button-down event as not all such events lead to drags.
	if (!m_fStatusTextShown)
	{
		// Set the status line text.  If we manage to do it then don't bother doing it
		// again until another drag really begins.
		String_256 str(_R(IDS_ZOOMOP_STATUSHELP));
		m_fStatusTextShown = GetApplication()->UpdateStatusBarText(&str);	
	}
#endif
	
	// All fine, continue dragging.
	return TRUE;
}



/********************************************************************************************
>	BOOL OpZoom::OnDragEnded(Spread* pBoxSpread,
							 const DocRect& drDragBox,
							 ClickModifiers cmods,
							 BOOL fDragOK)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		pBoxSpread			the spread where the drag was started
				drDragBox			the last extent of the drag box drawn by the user
				cmods				the state of the mouse click modifiers (not used)
				fDragOK				the FALSE the drag failed or was cancelled
	Returns:	TRUE (always)
	Purpose:	Called when the drag is successfully finished.  We check to see if the
				mouse actually moved noticably during the drag. If it did not, then a
				simple click operation is performed. If it did though, the area of the
				drag box will be zoomed in on.
********************************************************************************************/

BOOL OpZoom::OnDragEnded(Spread*, const DocRect&, ClickModifiers, BOOL fDragOK)
{
	// Pop the cursor we possibly pushed in OnStartDrag.
	if (m_nfShowZoomCursors) CursorStack::GPop(m_ncsrSaveDrag);
	
	// If the drag didn't happen we do nothing.  We don't want FailAndExecute() called
	// though, so return TRUE.
	if (!fDragOK) return TRUE;

	// This could take a while . . .
//	BeginSlowJob();

	// Get the current view.
	DocView* pDocView = DocView::GetCurrent();
	ERROR3IF(pDocView == 0, "No current DocView in OpZoom::OnDragEnded");

	// Find the half of the absolute width and height of the drag box.
	WorkCoord wcLast = pDocView->GetClickWorkCoord();
	WorkRect wrZoom;

	if (m_fRadialZoomDragBox)
	{
		XLONG nWidth2 = m_wcStartPos.x - wcLast.x;
		if (nWidth2 < 0) nWidth2 = -nWidth2;
		XLONG nHeight2 = m_wcStartPos.y - wcLast.y;
		if (nHeight2 < 0) nHeight2 = -nHeight2;

		// Make a rectangle of this width and height, centred on the click point.
		wrZoom = WorkRect(m_wcStartPos.x - nWidth2, m_wcStartPos.y - nHeight2,
					      m_wcStartPos.x + nWidth2, m_wcStartPos.y + nHeight2);
	}
	else
	{
		#undef  MIN
		#undef  MAX
		#define MIN(a,b) (((a)<(b))?(a):(b))
		#define MAX(a,b) (((a)>(b))?(a):(b))
		
		wrZoom = WorkRect(MIN(m_wcStartPos.x, wcLast.x),
			   			  MIN(m_wcStartPos.y, wcLast.y),
			   			  MAX(m_wcStartPos.x, wcLast.x),
			   			  MAX(m_wcStartPos.y, wcLast.y));
	}

	// We must first check if we have a significant drag box.  If the size of the box
	// is less than the Camelot "click radius" we will ignore the drag and treat the
	// operation as a single-click "zoom in" action by the user.

	// Graeme (11/11/99): The comparison is now done against a constant value
	// (ZOOM_MIN_DRAG), which is defined in zoomops.h. dDragDistance gives an absolute
	// value, rather than testing against separate x and y values.

	// Zoom in bug fixed by Chris Snook/Graeme (14/12/99) ....

	double dDragDistance = sqrt ( ( (double) wrZoom.Width () * (double) wrZoom.Width () ) +
								  ( (double) wrZoom.Height () * (double) wrZoom.Height () ) );

	if ( dDragDistance < ZOOM_MIN_DRAG || wrZoom.Width() <= 0 || wrZoom.Height() <= 0 )
	{
		// Drag is insignificant so zoom in instead.
		ZoomIn(m_wcStartPos, FALSE);					// OpDragBox will end this op
	}
	else
	{
		// Do a proper mouse-drag zoom.
		ZoomInOnRect(wrZoom, FALSE);					// OpDragBox will end this op
	}
	
	// tell people things have changed on screen
	TRACEUSER( "Diccon", _T("OnDragEnded\n"));
	BROADCAST_TO_ALL(ScreenChangeMsg(TRUE));

	// Remove the hour-glass and return the success code.
//	EndSlowJob();
	return TRUE;
}



/********************************************************************************************
>	DocRect OpZoom::CalcDragBox(const DocCoord& dcStartPos, const DocCoord& dcMousePos) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		dcStartPos			the position where the drag was started
				dcMousePos			the current position of the mouse
	Returns:	A document (spread) rectangle the is the current bounds of the drag box.
	Purpose:	Overrides the default OpDragBox method to allow the drag box to be drawn
				from the centre (start of drag) to the corner (current mouse position), if
				the 'ZoomDragOnCentre' preference allows.  If it doesn't the default method
				is called instead.
	SeeAlso:	OpDragBox::CalcDragBox
********************************************************************************************/

DocRect OpZoom::CalcDragBox(const DocCoord& dcStartPos, const DocCoord& dcMousePos) const
{
	// Do what the preferences say.
	return (m_fRadialZoomDragBox) ? OpCentredDragBox::CalcDragBox(dcStartPos, dcMousePos)
								  : OpDragBox::CalcDragBox(dcStartPos, dcMousePos);
}



/********************************************************************************************
>	void OpZoom::ZoomOut(const WorkCoord& wcZoom, BOOL fEndOp = TRUE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		wcZoom			the point to zoom out at
				fEndOp			whether to end this operation after this zoom
	Purpose:	Zooms out of the current DocView by one step.  By default this function
				will call the End() function for this operation afterwards.
********************************************************************************************/

void OpZoom::ZoomOut(const WorkCoord& wcZoom, BOOL fEndOp)
{
	// Find out the current view.
	DocView* pDocView = DocView::GetCurrent();
	ERROR3IF(pDocView == 0, "No selected DocView in OpZoom::ZoomOut");

	if (pDocView)
	{
		// Get the current view's scaling factor as a rounded precentage.
		INT32 nOldScalePercent = ((pDocView->GetViewScale() * 100) + FIXED16_DBL(0.5)).MakeInt();

		// Search forwards through the zoom table until we find a lower zoom factor, or
		// until we hit the end of the table, meaning no lower zoom factor exists.
		for (INT32 i = 0; i < cZoomTableSize; i++)
		{
			// Found a lower zoom?  If so, set it, remember it's position in the zoom table
			// and return.
			if (nOldScalePercent > GetPresetZoomPercent(i))
			{
				// Do the zoom.  We will (optionally) end the operation.
				pDocView->SetZoomTableIndex(i);
				ZoomAtPoint(wcZoom, GetPresetZoomScale(i), FALSE);
				break;
			}
		}
	}

	// We must correctly end the operation.
	if (fEndOp) End();

	// tell people things have changed on screen
	TRACEUSER( "Diccon", _T("ZoomOut\n"));
	BROADCAST_TO_ALL(ScreenChangeMsg(TRUE));
}



/********************************************************************************************
>	void OpZoom::ZoomOut(Spread* pZoomSpread, const DocCoord& dcZoomPos, BOOL fEndOp = TRUE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		pZoomSpread		the spread containing the point to zoom out at
				dcZoomPos		the point to zoom out at
				fEndOp			whether to end this operation after this zoom
	Purpose:	Zooms out of the current DocView by one step.  By default this function
				will call the End() function for this operation afterwards.
********************************************************************************************/

void OpZoom::ZoomOut(Spread* pZoomSpread, const DocCoord& dcZoomPos, BOOL fEndOp)
{
	WorkCoord wcZoom;
	SpreadToWork(pZoomSpread, dcZoomPos, &wcZoom);
	ZoomOut(wcZoom, fEndOp);
}



/********************************************************************************************
>	void OpZoom::ZoomIn(const WorkCoord& wcZoom, BOOL fEndOp = TRUE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		wcZoom			the point to zoom in at
				fEndOp			whether to end this operation after this zoom
	Purpose:	Zooms into the current DocView by one step.  By default this function
				will call the End() function for this operation afterwards.
********************************************************************************************/

void OpZoom::ZoomIn(const WorkCoord& wcZoom, BOOL fEndOp)
{
	// Find out the current view.
	DocView* pDocView = DocView::GetCurrent();
	ERROR3IF(pDocView == 0, "No current DocView in OpZoom::ZoomIn");
	
	if (pDocView)
	{
		// Find the current view's scaling factor.  By converting this to a percentage
		// and rounding we improve the accuracy of the comparison below.
		INT32 nOldScalePercent = ((pDocView->GetViewScale() * 100) + FIXED16_DBL(0.5)).MakeInt();

		// Search backwards until we find a higher zoom, or the end of the table (meaning that
		// there is no higher zoom).
		for (INT32 i = cZoomTableSize - 1; i >= 0; i--)
		{
			// Found a higher zoom?  If so, set it, remember it's position and return.
			if (nOldScalePercent < GetPresetZoomPercent(i))
			{
				// Do the zoom.  We will (optionally) end the operation.
				pDocView->SetZoomTableIndex(i);
				ZoomAtPoint(wcZoom, GetPresetZoomScale(i), FALSE);
				break;
			}
		}
	}

	// End the operation if the caller wants us to.
	if (fEndOp) End();
}



/********************************************************************************************
>	void OpZoom::ZoomIn(Spread* pZoomSpread, const DocCoord& dcZoomPos, BOOL fEndOp = TRUE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		pZoomSpread		the spread containing the point to zoom in at
				dcZoomPos		the point to zoom in at
				fEndOp			whether to end this operation after this zoom
	Purpose:	Zooms in on the current DocView by one step.  By default this function
				will call the End() function for this operation afterwards.
********************************************************************************************/

void OpZoom::ZoomIn(Spread* pZoomSpread, const DocCoord& dcZoomPos, BOOL fEndOp)
{
	WorkCoord wcZoom;
	SpreadToWork(pZoomSpread, dcZoomPos, &wcZoom);
	ZoomIn(wcZoom, fEndOp);
}



/********************************************************************************************
>	void OpZoom::ZoomInOnRect(const WorkRect& wrZoom, BOOL fEndOp = TRUE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		wrZoom			the rectangle to zoom in on.
				fEndOp			whether to end this operation after this zoom
	Purpose:	Zooms so that the given work rectangle completely fills the current
				DocView view extents.  By default this function	will call the End()
				function for this operation afterwards.
********************************************************************************************/

void OpZoom::ZoomInOnRect(const WorkRect& wrZoom, BOOL fEndOp)
{
	// Make sure the zoom rectangle isn't empty, or we'll divide by zero later on.
	if (wrZoom.IsEmpty())
	{
		ERROR3("Empty zoom rectangle passed to OpZoom::ZoomInOnRect");
		return;
	}

	// Find out the current view and save its scale factor and scroll offsets.
	DocView* pDocView = DocView::GetCurrent();
	ERROR3IF(pDocView == 0, "No current DocView in OpZoom::ZoomInOnRect");
	// Get out now if view 0 otherwise we will access violate later on
	if (pDocView == 0)
		return;
	
	OpZoomPrevZoomDescriptor::SaveZoom(pDocView);

	// Find out the size of the window and the given zoom rectangle in work coordinates.
	WorkRect wrView = pDocView->GetViewRect();

	// Get the current scale factor.
	double fpOldScaleFactor = pDocView->GetViewScale().MakeDouble();

	// Find out the ratios of the width and height (zoom rect to view rect).  We know
	// that the divisors are not zero, or we would not have entered this function.
	double fpWidthFactor  = (double)wrView.Width ()/wrZoom.Width ();
	double fpHeightFactor = (double)wrView.Height()/wrZoom.Height();
	double fpMinFactor = (fpWidthFactor < fpHeightFactor) ? fpWidthFactor : fpHeightFactor;

	// The new zoom factor is proportional to the above
	double fpNewScaleFactor = fpOldScaleFactor * fpMinFactor;

	// Push the zoom-in or zoom-out cursor, depending on whether we are zooming in or
	// out and the state of the preference.
	INT32 ncsrOldID=0;
	if (ZoomTool::IsSelectedTool() && m_nfShowZoomCursors)
	{
		ncsrOldID = CursorStack::GPush((fpNewScaleFactor > fpOldScaleFactor)
											? &m_csrZoomIn
											: &m_csrZoomOut);
	}
	
	// Convert the new scale factor to a percentage and check that it's in bounds.  If it
	// isn't adjust it to the lowest/highest zoom factors.
	INT32 nZoomPercent = (INT32) (fpNewScaleFactor * 100);
	if (nZoomPercent < GetPresetZoomPercent(cZoomTableSize - 1))
	{
		fpNewScaleFactor = GetPresetZoomScale(cZoomTableSize - 1).MakeDouble();
	}
	else if (nZoomPercent > GetPresetZoomPercent(0))
	{
		fpNewScaleFactor = GetPresetZoomScale(0).MakeDouble();
	}

	// Set the scroll offsets to the middle of the box.
	WorkCoord wcScrollOffset;
	wcScrollOffset.x = (wrZoom.lo.x + wrZoom.hi.x) / 2;
	wcScrollOffset.y = (wrZoom.lo.y + wrZoom.hi.y) / 2;

	// Scale the value to the correct number of pixels. 
	double fpRatio = fpNewScaleFactor / fpOldScaleFactor;
	wcScrollOffset.x = XLONG(wcScrollOffset.x*fpRatio);
	wcScrollOffset.y = XLONG(wcScrollOffset.y*fpRatio);

	// Offset it by half the size of the window.  WHY?  AND WHY THE MakeLong() CALL?
	wcScrollOffset.x = wcScrollOffset.x - (wrView.Width() / 2);
	wcScrollOffset.y += wrView.Height() / 2;

	// Check the scroll offsets bounds as we can't have scroll offsets anywhere.
	if (wcScrollOffset.x < 0) wcScrollOffset.x = 0;
	if (wcScrollOffset.y > 0) wcScrollOffset.y = 0;

	// Set the scale. This must be done before the scroll offset is set or
	// strangeness will follow . . .
	pDocView->SetViewScale((FIXED16) fpNewScaleFactor);
	pDocView->SetScrollOffsets(wcScrollOffset, FALSE);

	// Update the combo-box containing scale factors.
	pDocView->SetZoomTableIndex(cFractionalZoomIndex);
#ifndef RALPH
	OpZoomComboDescriptor::Update();
#endif

	// We have to force a redraw as the zoom has changed, reset the cursor, and end the
	// operation.
	pDocView->ForceRedraw(TRUE);
	if (ZoomTool::IsSelectedTool() && m_nfShowZoomCursors) CursorStack::GPop(ncsrOldID);
	if (fEndOp) End();


	// tell people things have changed on screen
	TRACEUSER( "Diccon", _T("ZoomInOnRect\n"));
	BROADCAST_TO_ALL(ScreenChangeMsg(TRUE));
}



/********************************************************************************************
>	void OpZoom::ZoomInOnRect(Spread* pZoomSpread,
							  const DocRect& drZoomRect,
							  BOOL fEndOp = TRUE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		pZoomSpread		the spread containing the rectangle to zoom in on
				drZoomRect		the rectangle to zoom in on.
				fEndOp			whether to end this operation after this zoom
	Purpose:	Zooms so that the given spread rectangle completely fills the current
				DocView view extents.  By default this function	will call the End()
				function for this operation afterwards.
********************************************************************************************/

void OpZoom::ZoomInOnRect(Spread* pZoomSpread, const DocRect& drZoomRect, BOOL fEndOp)
{
	// Call the WorkRect version of this function to do the zoom.
	WorkRect wrZoom;
	SpreadToWork(pZoomSpread, drZoomRect, &wrZoom);
	ZoomInOnRect(wrZoom, fEndOp);
}



/********************************************************************************************
>	void OpZoom::ZoomAtPoint(const WorkCoord& wcZoom, FIXED16 fxNewScaleFactor,
							 BOOL fEndOp = TRUE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		wcZoom					the point within the current view to zoom at,
										in work coordinates.
				fxNewScaleFactor		the required scale factor
				fEndOp					whether to end this operation after this zoom
	Purpose:	Sets the current DocView to show the given point at the given scale factor.
				By default this function will call the End() function for this operation
				afterwards.
********************************************************************************************/
/********************************************************************************************
Technical notes:
	What the zoom does. This routine finds out the current scroll offsets and makes
	a vector from the clicked point to the top corner of the window (ie the scroll
	offsets).  It knows the ratio of the old scale factor and the new scale factor
	and uses this to calculate a new point from the click point along the vector.

	For example, if the ratio of the old and new zooms is one half then the new top
	corner of the window will be a point on the vector, half way between the click
	point and the old top corner of the window.

	Since the document will be changing size and since the scroll offsets are
	measured in pixels, these will be changing after the zoom has happened, so we
	have to compensate for that as well. All that remains to do after that is check
	that the offsets are valid and then set them.
********************************************************************************************/

void OpZoom::ZoomAtPoint(const WorkCoord& wcZoom, FIXED16 fxNewScaleFactor, BOOL fEndOp)
{
	// Get the current view.
	DocView* pDocView = DocView::GetCurrent();
	ERROR3IF(pDocView == 0, "No current DocView in OpZoom::ZoomAtPoint");

	// Save the view's current zoom and find out current scaling factor.
	OpZoomPrevZoomDescriptor::SaveZoom(pDocView);
	FIXED16 fxOldScaleFactor = pDocView->GetViewScale();

	// Push the zoom-in or zoom-out cursor, depending on whether we are zooming in or
	// out.
	INT32 ncsrOldID=0;
	if (ZoomTool::IsSelectedTool() && m_nfShowZoomCursors)
	{
		ncsrOldID = CursorStack::GPush((fxNewScaleFactor > fxOldScaleFactor)
											? &m_csrZoomIn
											: &m_csrZoomOut);
	}

	// Find the current view size in 64-bit work coordinates.
	WorkRect wrView = pDocView->GetViewRect();

	// Work out the ratio of the new and old scale factors.
	double fpLineRatio = fxOldScaleFactor.MakeDouble() / fxNewScaleFactor.MakeDouble();

	// We either centre the clicked point or keep it fixed, depending on the preferences.
	// Calculate the new position of the top-left of the view accordingly.
	WorkCoord wcNewTopCorner = wcZoom;
	if (m_fFixZoomClickPoint)
	{
		// Find the vector from the click point to the top corner of the window, and
		// from this work out the new position of the top-left corner of the view.
		wcNewTopCorner.x += XLONG(fpLineRatio * (wrView.lo.x - wcZoom.x));
		wcNewTopCorner.y += XLONG(fpLineRatio * (wrView.hi.y - wcZoom.y));
	}
	else
	{
		// Find the vector from the centre of the view to the top-left corner, scaled by
		// the line-ratio.  Add this vector to the click point to work out the new position
		// of the top-left corner of the view.
		wcNewTopCorner.x -= XLONG(fpLineRatio * wrView.Width ()/2);
		wcNewTopCorner.y += XLONG(fpLineRatio * wrView.Height()/2);
	}

	// Scale the value to the correct number of pixels.  We convert to doubles as the
	// fixed-point numbers can lose precision at high zooms.
	double fpPixRatio = fxNewScaleFactor.MakeDouble() / fxOldScaleFactor.MakeDouble();
	wcNewTopCorner.x  = XLONG(fpPixRatio * wcNewTopCorner.x);
	wcNewTopCorner.y  = XLONG(fpPixRatio * wcNewTopCorner.y);
		
	// Set the new scale factor in the view.
	pDocView->SetViewScale(fxNewScaleFactor);	

	// Make sure that the new scroll offset is within workspace bounds and set it.
	if (wcNewTopCorner.x < 0) wcNewTopCorner.x = 0;
	if (wcNewTopCorner.y > 0) wcNewTopCorner.y = 0;
	pDocView->SetScrollOffsets(wcNewTopCorner, FALSE);

#ifndef RALPH
	// Update the combo-box containing scale factors.
	OpZoomComboDescriptor::Update();
#endif

	// Finally redraw the whole window, reset the cursor, and end the operation.
	pDocView->ForceRedraw(TRUE);
	if (ZoomTool::IsSelectedTool() && m_nfShowZoomCursors) CursorStack::GPop(ncsrOldID);
	if (fEndOp) End();

	// tell people things have changed on screen
	TRACEUSER( "Diccon", _T("ZoomAtPoint\n"));
	BROADCAST_TO_ALL(ScreenChangeMsg(TRUE));
}



/********************************************************************************************
>	void OpZoom::ZoomAtPoint(Spread* pZoomSpread, const DocCoord& dcZoomPos,
							 FIXED16 fxNewScaleFactor, BOOL fEndOp = TRUE)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		pZoomSpread				the spread containing the point to zoom at
				dcZoomPos				the point to at.  If the CentreOnZoomClick preference
										is TRUE then this point will remain fixed on-screen,
										otherwise this point will be centred on-screen.
				fxNewScaleFactor		the required scale factor
				fEndOp					whether to end this operation after this zoom
	Purpose:	Sets the current DocView to show the given point with the spread at the
				given scale factor.  By default this function will call the End() function
				for this operation afterwards.
********************************************************************************************/

void OpZoom::ZoomAtPoint(Spread* pZoomSpread, const DocCoord& dcZoomPos,
						 FIXED16 fxNewScaleFactor, BOOL fEndOp)
{
	// Now we can just pass to the WorkCoord version of this function.
	WorkCoord wcZoom;
	SpreadToWork(pZoomSpread, dcZoomPos, &wcZoom);
	ZoomAtPoint(wcZoom, fxNewScaleFactor, fEndOp);
}



/******************************************************************************************
>	BOOL OpZoom::GetStatusLineText(String_256* pText, Spread*, DocCoord, ClickModifiers)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		Spread*, DocCoord, ClickModifiers - not used
	Outputs:	pText - 
	Returns:	TRUE if returning valid text
	Purpose:	get status line help for zoom drag op
	Errors:		this==0
******************************************************************************************/

BOOL OpZoom::GetStatusLineText(String_256* pText, Spread*, DocCoord, ClickModifiers)
{
	ERROR2IF(this == 0, FALSE, "OpZoom::GetStatusLineText() - this == 0");
	ERROR2IF(pText == 0, FALSE, "OpZoom::GetStatusLineText() - pText == 0");
	return pText->Load(_R(IDS_ZOOMOP_STATUSHELP));
}



/********************************************************************************************
>	static BOOL OpZoom::Declare()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Returns:	TRUE if everything went OK, FALSE otherwise
	Purpose:	"Registers" the zoom operation's OpDescriptors, loads preferences, and
				makes sure the zoom-in and zoom-out cursors are valid.
	SeeAlso:	class OpZoomComboDescriptor; class OpZoomPrevZoomDescriptor;
				class OpZoomFitSpreadDescriptor; class OpZoomFitDrawingDescriptor
********************************************************************************************/

BOOL OpZoom::Declare()
{
	// Try to create OpDescriptors for each zoom gadget.
	ERRORIF(!new OpZoomComboDescriptor ||
			!new OpZoomPrevZoomDescriptor ||
			!new OpZoomFitSpreadDescriptor ||
			!new OpZoomFitDrawingDescriptor ||
			!new OpZoomFitSelectedDescriptor ||
			!new OpZoomFitRectDescriptor,
			_R(IDE_NOMORE_MEMORY),
			FALSE);

#if !defined(EXCLUDE_FROM_RALPH)
	// Try to load preferences.
	ERRORIF(!GetApplication()->DeclareSection(TEXT("Zoom Tool"), 5) ||
			!GetApplication()->DeclarePref(TEXT("Zoom Tool"), TEXT("FixZoomClickPoint"),
							 			   &m_fFixZoomClickPoint, FALSE, TRUE) ||
			!GetApplication()->DeclarePref(TEXT("Zoom Tool"), TEXT("RadialZoomDragBox"),
							   			   &m_fRadialZoomDragBox, FALSE, TRUE) ||
			!GetApplication()->DeclarePref(TEXT("Zoom Tool"), TEXT("ShowZoomCursors"),
							   			   &m_nfShowZoomCursors, 0, 1),
			_R(IDE_BAD_INI_FILE),
			FALSE);
#endif

	// Success.
	return TRUE;
}



/********************************************************************************************
>	void OpZoom::MouseWheelZoomAtPoint(const WorkCoord& wcZoom, INT32 nNewScalePercent,
							 BOOL fEndOp = TRUE)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> (BLATANTLY ripped from JustinF & Rik)
	Created:	17/11/2000
	Inputs:		wcZoom					the point within the current view to zoom at,
										in work coordinates.
				fxNewScaleFactor		the required scale factor
				fEndOp					whether to end this operation after this zoom
	Purpose:	Sets the current DocView to show the given point at the given scale factor.
				By default this function will call the End() function for this operation
				afterwards.
********************************************************************************************/

void OpZoom::MouseWheelZoomAtPoint(const WorkCoord& wcZoom, FIXED16 fxNewScaleFactor, BOOL fEndOp)
{
	// Get the current view.
	DocView* pDocView = DocView::GetCurrent();
	ERROR3IF(pDocView == 0, "No current DocView in OpZoom::ZoomAtPoint");

	// Save the view's current zoom and find out current scaling factor.
	OpZoomPrevZoomDescriptor::SaveZoom(pDocView);
	FIXED16 fxOldScaleFactor = pDocView->GetViewScale();

	// Push the zoom-in or zoom-out cursor, depending on whether we are zooming in or
	// out.
	INT32 ncsrOldID=0;
	if (ZoomTool::IsSelectedTool() && m_nfShowZoomCursors)
	{
		ncsrOldID = CursorStack::GPush((fxNewScaleFactor > fxOldScaleFactor)
											? &m_csrZoomIn
											: &m_csrZoomOut);
	}

	// Find the current view size in 64-bit work coordinates.
	WorkRect wrView = pDocView->GetViewRect();

	// Work out the ratio of the new and old scale factors.
	double fpLineRatio = fxOldScaleFactor.MakeDouble() / fxNewScaleFactor.MakeDouble();

	// We either centre the clicked point or keep it fixed, depending on the preferences.
	// Calculate the new position of the top-left of the view accordingly.
	WorkCoord wcNewTopCorner = wcZoom;

//	if (!m_fFixZoomClickPoint)
//	{
		// Find the vector from the click point to the top corner of the window, and
		// from this work out the new position of the top-left corner of the view.
		wcNewTopCorner.x += XLONG(fpLineRatio * (wrView.lo.x - wcZoom.x));
		wcNewTopCorner.y += XLONG(fpLineRatio * (wrView.hi.y - wcZoom.y));
//	}
//	else
//	{
//		// Find the vector from the centre of the view to the top-left corner, scaled by
//		// the line-ratio.  Add this vector to the click point to work out the new position
//		// of the top-left corner of the view.
//		wcNewTopCorner.x -= MakeXLong(fpLineRatio * (wrView.Width().MakeDouble() / 2));
//		wcNewTopCorner.y += MakeXLong(fpLineRatio * (wrView.Height().MakeDouble() / 2));
//	}

	// Scale the value to the correct number of pixels.  We convert to doubles as the
	// fixed-point numbers can lose precision at high zooms.
	double fpPixRatio = fxNewScaleFactor.MakeDouble() / fxOldScaleFactor.MakeDouble();
	wcNewTopCorner.x  = XLONG(fpPixRatio * wcNewTopCorner.x);
	wcNewTopCorner.y  = XLONG(fpPixRatio * wcNewTopCorner.y);
		
	// Set the new scale factor in the view.
	pDocView->SetViewScale(fxNewScaleFactor);	

	// Make sure that the new scroll offset is within workspace bounds and set it.
	if (wcNewTopCorner.x < 0) wcNewTopCorner.x = 0;
	if (wcNewTopCorner.y > 0) wcNewTopCorner.y = 0;
	pDocView->SetScrollOffsets(wcNewTopCorner, FALSE);

#ifndef RALPH
	// Update the combo-box containing scale factors.
	OpZoomComboDescriptor::Update();
#endif

	// Finally redraw the whole window, reset the cursor, and end the operation.
	pDocView->ForceRedraw(TRUE);
	if (ZoomTool::IsSelectedTool() && m_nfShowZoomCursors) CursorStack::GPop(ncsrOldID);
	if (fEndOp) End();

	// tell people things have changed on screen
	TRACEUSER( "Diccon", _T("ZoomAtPoint\n"));
	BROADCAST_TO_ALL(ScreenChangeMsg(TRUE));
}


////////////////////////////////////////////////////////////////////////////////////////////
// class OpZoomIn
OpZoomIn::OpZoomIn()
{
}

BOOL OpZoomIn::Init()
{
	BOOL ok = RegisterOpDescriptor(
									0, 
									_R(IDS_ZOOMIN),
									CC_RUNTIME_CLASS(OpZoomIn), 
									OPTOKEN_ZOOMIN,
									OpZoomIn::GetState,
									0,  /* help ID */
									0,	/* bubble ID */
									0	/* bitmap ID */
									);
	return ok;
}

/***********************************************************************************************

>	static OpState OpZoomIn::GetState(String_256* Description, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/97
	Inputs:		Description = ptr to place description of why this op can't happen
				pOpDesc     = ptr to the Op Desc associated with this op
	Outputs:	-
	Returns:	An OpState object
	Purpose:    Func for determining the usability of this op
	SeeAlso:	-
		
***********************************************************************************************/
OpState OpZoomIn::GetState(String_256* Description, OpDescriptor*)
{
	OpState State;
	return State;
}

/********************************************************************************************

>	virtual void OpZoomIn::Do(OpDescriptor* pOpDesc)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/00
	Inputs:		pointer to the OpZoomDescriptor being invoked
	Purpose:	Calls the base class function to zoom in!

********************************************************************************************/
void OpZoomIn::Do(OpDescriptor *pOpDesc)
{
	// Ok! lets zoom in!
	DocView* pDocView = DocView::GetCurrent();

	if(pDocView)
	{
		const WorkRect ZoomRect = pDocView->GetViewRect();
		ZoomIn(ZoomRect.Centre());
	}
	else
	{
		ERROR3IF(pDocView == 0, "No current DocView found!");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// class OpZoomOut
OpZoomOut::OpZoomOut()
{
}

BOOL OpZoomOut::Init()
{
	BOOL ok = RegisterOpDescriptor(
									0, 
									_R(IDS_ZOOMOUT),
									CC_RUNTIME_CLASS(OpZoomOut), 
									OPTOKEN_ZOOMOUT,
									OpZoomOut::GetState,
									0,  /* help ID */
									0,	/* bubble ID */
									0	/* bitmap ID */
									);
	return ok;
}

/***********************************************************************************************

>	static OpState OpZoomOut::GetState(String_256* Description, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/3/97
	Inputs:		Description = ptr to place description of why this op can't happen
				pOpDesc     = ptr to the Op Desc associated with this op
	Outputs:	-
	Returns:	An OpState object
	Purpose:    Func for determining the usability of this op
	SeeAlso:	-
		
***********************************************************************************************/
OpState OpZoomOut::GetState(String_256* Description, OpDescriptor*)
{
	OpState State;
	return State;
}

/********************************************************************************************

>	virtual void OpZoomOut::Do(OpDescriptor* pOpDesc)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/00
	Inputs:		pointer to the OpZoomDescriptor being invoked
	Purpose:	Calls the base class function to zoom Out!

********************************************************************************************/
void OpZoomOut::Do(OpDescriptor *pOpDesc)
{
	// Ok! lets zoom Out!
	DocView* pDocView = DocView::GetCurrent();

	if(pDocView)
	{
		const WorkRect ZoomRect = pDocView->GetViewRect();
		ZoomOut(ZoomRect.Centre());
	}
	else
	{
		ERROR3IF(pDocView == 0, "No current DocView found!");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// class OpZoomDescriptor

/********************************************************************************************
>	OpZoomDescriptor::OpZoomDescriptor(const TCHAR* pcszToken, UINT32 wStatusID,
								   	   UINT32 wHelpID, UINT32 wBubbleID,
									   UINT32 resourceID=0, UINT32 controlID=0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		pcszToken		the "token" (OLE verb?) of the associated Operation
				wStatusID		string resource ID of status-bar text
				wHelpID			string ID of help text/jump (?)
				wBubbleID		string ID of bubble-help text
	Outputs:	-
	Returns:	-
	Purpose:	Constructs the base class of the OpZoom's OpDescriptors
	Errors:		-
	SeeAlso:	class OpZoomFitSpreadDescriptor; class OpZoomFitDrawingDescriptor;
				class OpZoomFitSelectedDescriptor; class OpZoomPrevZoomDescriptor
********************************************************************************************/

OpZoomDescriptor::OpZoomDescriptor(const TCHAR* pcszToken, UINT32 wStatusID,
								   UINT32 wHelpID, UINT32 wBubbleID, UINT32 resourceID, UINT32 controlID)
  :	OpDescriptor(0,									// tool ID
  				 wStatusID,							// string ID of text in status bar
  				 CC_RUNTIME_CLASS(OpZoom),
				 (TCHAR*) pcszToken,
				 GetState,
				 wHelpID,							// help link ID
				 wBubbleID,							// bubble help string ID
				 resourceID,
				 controlID,
				 TRUE)								// wants to receive messages
{
	// Empty.
}



/********************************************************************************************
>	static OpState OpZoomDescriptor::GetState(String_256* pDesc, OpDescriptor* pOpDesc)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		pDesc       --- A pointer to a String. GetState fills this with the
								appropriate details for the conditions arising eg. why
								"Previous Zoom" is greyed out.
				pOpDesc		--- A pointer to the OpDescriptor whose state is being
								queried.
	Returns:	An OpState containing the flags that show what is valid.
	Purpose:	Returns the state that this zoom operation should appear in the menus 
				or as a buttom, for example - greyed out, or ticked.
	SeeAlso:	OpZoomDescriptor::IsAvailable
********************************************************************************************/

OpState OpZoomDescriptor::GetState(String_256*, OpDescriptor* pOpDesc)
{
	BOOL fCanDo = ((OpZoomDescriptor*) pOpDesc)->IsAvailable();
/*	TRACEUSER( "JustinF", _T("OpZoomDescriptor::GetState for %-12s (0x%lX) - %-8s at %lu ms\n"),
							(LPCTSTR) 	pOpDesc->Token,
							(UINT32)		pOpDesc,
							(LPCTSTR)	(fCanDo ? TEXT("OK") : TEXT("Greyed")),
							(UINT32)		::GetTickCount());
*/	return OpState(FALSE, !fCanDo);
}



/********************************************************************************************
>	virtual BOOL OpZoomDescriptor::IsAvailable()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the Operation is available.  By default this returns TRUE if there
				is a "selected" document, a spread, and a rectangle to zoom in on.
	Purpose:	Default "GetState"-like function for OpZoomDescriptor derivatives.
	Errors:		-
	SeeAlso:	OpZoom::GetState; DocView::GetSelected; OpState::OpState;
				OpZoomDescriptor::GetSpread; OpZoomDescriptor::GetRect
********************************************************************************************/

BOOL OpZoomDescriptor::IsAvailable()
{
	// Try to get the "selected" view.
	DocView* pDocView = DocView::GetCurrent();
	if (pDocView == 0)
	{
//		TRACEUSER( "JustinF", _T("\tNo current DocView in OpZoomDescriptor::IsAvailable\n"));
		return FALSE;
	}

	// Find the relevant spread, if any.
	Spread* pSpread = GetSpread(pDocView);
	if (pSpread == 0)
	{
//		TRACEUSER( "JustinF", _T("\tNo relevant spread in OpZoomDescriptor::IsAvailable\n"));
		return FALSE;
	}

	DocRect ZoomRect = GetRect(pSpread);
	BOOL Empty = ZoomRect.IsEmpty();
#ifdef _DEBUG
	// Is there actually no relevant rectangle within the spread to zoom on?
	if (Empty)
	{
//		TRACEUSER( "JustinF", _T("\tEmpty rectangle in OpZoomDescriptor::IsAvailable\n"));
	}																				  
#endif

	// Find the relevant zoom rectangle, if any.
	return !Empty;
}



/********************************************************************************************
>	virtual MsgResult OpZoomDescriptor::Message(Msg* pMsg)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default message despatcher for OpZoomDescriptor derivatives.  Passes
				button messages to HandleButtonMsg (virtual).
	Errors:		-
	SeeAlso:	OpZoomDescriptor::HandleButtonMsg
********************************************************************************************/

MsgResult OpZoomDescriptor::Message(Msg* pMsg)
{
	// Check if the message is an OpDesc message.
	if (!MESSAGE_IS_A(pMsg, OpDescMsg)) return OK;
	
	// Cast it into the correct type etc.
	OpDescMsg* pOpDescMsg = (OpDescMsg*) pMsg;
	
	// Process the message . . .
	if (pOpDescMsg->OpDesc == this && MESSAGE_IS_A(pOpDescMsg, OpDescControlMsg))
	{
		// Cast to a control message, unpack, and despatch to the handler.
		OpDescControlMsg* pControlMsg = (OpDescControlMsg*) pOpDescMsg;
		if (pControlMsg->DlgMsg == DIM_LFT_BN_CLICKED)
		{
			if (IsAvailable())
				return HandleButtonMsg(pControlMsg->pDlgOp, pOpDescMsg->SetGadgetID);
#if !defined(EXCLUDE_FROM_RALPH)
			else
//				Beep();
				wxBell();
#endif
		}
	}

	// Let the base class do its stuff on the message.
	return OpDescriptor::Message(pMsg);
}



/********************************************************************************************
>	virtual BOOL OpZoomDescriptor::DoZoom(OpZoom * pOpZoom)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/95
	Inputs:		pZoomOp		pointer to the OpZoom to be used
	Returns:	True if operation has happened correctly
	Purpose:	Is the base zoom descriptor class function for performing a zoom operation.
				It does this by calling (virtual) GetSpread to find the	spread to zoom in on,
				then (virtual) GetRect to find the rectangle within	the spread. The (virtual)
				function AdjustRect is called to modify the	resultant rectangle. Derived
				classes can override these functions to	customise the effects of particular
				zoom operation.
				Performs the zoom, after saving the current zoom settings, then updates
				the zoom combo box.
				Similar button to HandleButtonMsg but is designed to be called by the Do()
				operator in OpZoom and hence from keyboard short-cuts or menu operations.
	Errors:		-
	SeeAlso:	OpZoom::Do;
				OpZoomDescriptor::GetSpread; OpZoomDescriptor::GetRect;
				OpZoomDescriptor::AdjustRect
********************************************************************************************/

BOOL OpZoomDescriptor::DoZoom(OpZoom* pZoomOp)
{
	ERROR2IF(pZoomOp == 0, FALSE, "OpZoomDescriptor::DoZoom called with no operation");

	// Get the current view.
	DocView* pDocView = DocView::GetCurrent();
	ERROR3IF(pDocView == 0, "No current DocView in OpZoomDescriptor::HandleButtonMsg");
	if (pDocView == 0)
		return FAIL;
		
	// Find the relevant spread, if any.
	Spread* pSpread = GetSpread(pDocView);
	ERROR3IF(pSpread == 0, "No relevant spread - can't do zoom\n");
	if (pSpread == 0)
		return FAIL;

	// Find the relevant zoom rectangle.
	DocRect drBounds = GetRect(pSpread);
	ERROR3IF(drBounds.IsEmpty(), "Relevant rectangle is empty - can't do zoom\n");

	// Allow derived class to adjust the document rectangle if it so desires.
	AdjustRect(&drBounds);

	// Do the zoom, the scaling factor of which will not appear in the zoom op's table.
    pZoomOp->ZoomInOnRect(pSpread, drBounds);

	return TRUE;	
}



/********************************************************************************************
>	virtual MsgResult OpZoomDescriptor::HandleButtonMsg(DialogOp* pdlgop, CGadgetID gid)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		pdlgop			pointer to the dialog operation ("unpacked" from a message)
				gid				the gadget ID associated with this OpDescriptor
	Outputs:	-
	Returns:	The message result code (either EAT_MSG or OK)
	Purpose:	Handles a button click message, by calling (virtual) GetSpread to find the
				spread to zoom in on, then (virtual) GetRect to find the rectangle within
				the spread.  The (virtual) function AdjustRect is called to modify the
				resultant rectangle.  Derived classes can override these functions to
				customise the effects of particular buttons.  Performs the zoom, after
				saving the current zoom settings, then updates the zoom combo box.
	Errors:		-
	SeeAlso:	OpZoomDescriptor::GetSpread; OpZoomDescriptor::GetRect;
				OpZoomDescriptor::AdjustRect
********************************************************************************************/

MsgResult OpZoomDescriptor::HandleButtonMsg(DialogOp*, CGadgetID)
{
	// Try to create an instance of the zoom operation.
	OpZoom* pZoomOp = new OpZoom;
	ERRORIF(pZoomOp == 0, _R(IDE_NOMORE_MEMORY), FAIL);

	// Do the zoom, the scaling factor of which will not appear in the zoom op's table.
	// DoZoom will End the operation
	BOOL ok = DoZoom(pZoomOp);
	if (!ok) return FAIL;
	return OK;
}



/********************************************************************************************
>	virtual Spread* OpZoomDescriptor::GetSpread(DocView* pDocView) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		pDocView		the DocView containing the spread.
	Outputs:	-
	Returns:	A pointer to the relevant spread to zoom in on.  By default this is the
				document page, ie. the first spread in the document.
	Purpose:	Gets a spread given a DocView.
	Errors:		-
	SeeAlso:	OpZoomDescriptor::HandleButtonMsg; View::GetDoc; Document::FindFirstSpread
********************************************************************************************/

Spread* OpZoomDescriptor::GetSpread(DocView* pDocView) const
{
	return pDocView->GetDoc()->FindFirstSpread();
}



/********************************************************************************************
>	virtual DocRect OpZoomDescriptor::GetRect(Spread* pSpread)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		pSpread			the spread to zoom in on
	Outputs:	-
	Returns:	A rectangle within the spread to zoom in on.  By default this is the
				"bounding rectangle" of the given spread.
	Purpose:	Gets a rectangle given a spread.
	Errors:		-
	SeeAlso:	OpZoomDescriptor::HandleButtonMsg; Spread::GetBoundingRect
********************************************************************************************/

DocRect OpZoomDescriptor::GetRect(Spread* pSpread)
{
	return pSpread->GetBoundingRect();
}



/********************************************************************************************
>	virtual void OpZoomDescriptor::AdjustRect(DocRect* pRect) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/94
	Inputs:		pRect				pointer to a DocRect to adjust
	Outputs:	The adjusted DocRect.
	Returns:	-
	Purpose:	In the base class version inflates the given document rectangle by
				5% in each direction.
	Errors:		-
	SeeAlso:	OpZoomDescriptor::HandleButtonMsg; OpZoomDescriptor::GetRect
********************************************************************************************/

void OpZoomDescriptor::AdjustRect(DocRect* pRect) const
{
	INT32 xinc = pRect->Width() / 20;			// find 5% of the width and height
	INT32 yinc = pRect->Height() / 20;
	if (xinc < 1) xinc = 1;						// make sure we inflate by some amount
	if (yinc < 1) yinc = 1;
	pRect->lo.x -= xinc;						// inflate the given rectangle
	pRect->lo.y -= yinc;
	pRect->hi.x += xinc;
	pRect->hi.y += yinc;
}



/********************************************************************************************
>	static void OpZoomDescriptor::FakeInvoke(TCHAR* pszToken)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Inputs:		pszToken	---		pointer to the "invoked" OpDescriptor's token
	Outputs:	-
	Returns:	-
	Purpose:	"Fakes" a button message for OpDescriptor classes derived from
				OpZoomDescriptor.  Searches for the OpDescriptor associated with the
				specified token.  If it finds it then casts it to an OpZoomDescriptor*,
				checks if the associated button isn't greyed, and if not, calls its
				HandleButtonMsg function.  Finally it makes sure that the percentage
				scale factor of the selected DocView is updated in the zoom combo box.
	Errors:		-
	SeeAlso:	OpZoomComboDescriptor::OnSelectionChanged
********************************************************************************************/

void OpZoomDescriptor::FakeInvoke(TCHAR* pszToken)
{
	// Try to find the OpDescriptor.
	OpZoomDescriptor* pZoomOpDesc = (OpZoomDescriptor*) FindOpDescriptor(pszToken);
	
	// If that worked then call its button handler.
	if (pZoomOpDesc != 0 && pZoomOpDesc->IsAvailable())
	{
		pZoomOpDesc->HandleButtonMsg(0, 0);
	}
#ifndef RALPH
	else
	{
		// If it didn't then refresh the zoom combo with some percentages.
//		Beep();
		wxBell();
		OpZoomComboDescriptor::Update();
	}
#endif
}

/********************************************************************************************

>	static void OpZoomDescriptor::FakeZoomToRect(DocRect* pRect)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/96
	Inputs:		pRect, pointer to a rect to zoom to
	Outputs:	-
	Returns:	-
	Purpose:	"Fakes" a button message for the ZoomRect OpDescriptor, using the specified,
				rectangle.
	Errors:		-

********************************************************************************************/

void OpZoomDescriptor::FakeZoomToRect(const DocRect& rect)
{
	// Try to find the OpDescriptor.
	OpZoomDescriptor* pZoomOpDesc = (OpZoomDescriptor*) FindOpDescriptor(OPTOKEN_ZOOMRECT);
	
	// If that worked then call its button handler.
	if (pZoomOpDesc != 0)
	{
		((OpZoomFitRectDescriptor*) pZoomOpDesc)->SetZoomRect(rect);
		pZoomOpDesc->HandleButtonMsg(0, 0);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// class OpZoomFitSpreadDescriptor

/********************************************************************************************
>	OpZoomFitSpreadDescriptor::OpZoomFitSpreadDescriptor()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OpDescriptor for the "Zoom to spread" button.
	Errors:		-
	SeeAlso:	class OpZoomDescriptor
********************************************************************************************/

OpZoomFitSpreadDescriptor::OpZoomFitSpreadDescriptor()
  : OpZoomDescriptor(OPTOKEN_ZOOMSPREAD, _R(IDS_ZOOMSPREADSTATUSTEXT),
  					 0, _R(IDBBL_FIT_TO_SPREAD))
{
	// default to always scaling in AdjustRect
	m_DontScale = TRUE;
}



/********************************************************************************************
>	virtual DocRect OpZoomFitSpreadDescriptor::GetRect(Spread* pSpread)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		pSpread			pointer to a spread
	Outputs:	-
	Returns:	The "page bounds" of the given spread.
	Purpose:	Provides the rectangle that OpZoomDescriptor::HandleButtonMsg will
				zoom in on.
	Errors:		-
	SeeAlso:	OpZoomDescriptor::HandleButtonMsg; OpZoomFitSpreadDescriptor::AdjustRect;
				Spread::GetPageBounds
********************************************************************************************/

DocRect OpZoomFitSpreadDescriptor::GetRect(Spread* pSpread)
{
/*
#ifdef _DEBUG
	if (IsUserName("JustinF"))
	{
		static DocRect drOld = pSpread->GetPageBounds();
		DocRect drNew = pSpread->GetPageBounds();
		
		if (drNew != drOld)
		{
			TRACE( _T("In OpZoomFitSpreadDesriptor - Spread::GetPageBounds has changed!\n")
				  "\t- drOld is %ld x %ld\n\t- drNew is %ld x %ld\n",
					(INT32) drOld.Width(), (INT32) drOld.Height(),
					(INT32) drNew.Width(), (INT32) drNew.Height());
		}
				
		return drOld = drNew;
	}
#endif
*/
	// These are both in Document coords not spread coords!
	DocRect SpreadSize = pSpread->GetPageBounds();
	DocRect PasteBoardSize = pSpread->GetPasteboardRect(FALSE);
	// If the two sizes are the same then this means we have a zero sized pasteboard
	// In this case, we should not try and scale up the resulting rectangle in AdjustRect
	// So note this state for later use.
	// Cannot do a straight equality check as we will have rounding errors.
	const MILLIPOINT delta = 1000;
	if (
		(SpreadSize.lo.x > PasteBoardSize.lo.x - delta) && (SpreadSize.lo.x < PasteBoardSize.lo.x + delta) &&
		(SpreadSize.hi.x > PasteBoardSize.hi.x - delta) && (SpreadSize.hi.x < PasteBoardSize.hi.x + delta) &&
		(SpreadSize.lo.y > PasteBoardSize.lo.y - delta) && (SpreadSize.lo.y < PasteBoardSize.lo.y + delta) &&
		(SpreadSize.hi.y > PasteBoardSize.hi.y - delta) && (SpreadSize.hi.y < PasteBoardSize.hi.y + delta)
	   )
	{
		m_DontScale = FALSE;
	}
	else
	{
		m_DontScale = TRUE;
	}

	return SpreadSize;
}



/********************************************************************************************
>	virtual void OpZoomFitSpreadDescriptor::AdjustRect(DocRect* pRect) const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95
	Inputs:		pRect				pointer to a DocRect to adjust
	Outputs:	The adjusted DocRect.
	Returns:	-
	Purpose:	Override the base class version of the fcuntion which inflates the given
				document rectangle by 5% in each direction for one which inflates it by 2%.
	Errors:		-
	SeeAlso:	OpZoomDescriptor::AdjustRect; OpZoomDescriptor::GetRect
********************************************************************************************/

void OpZoomFitSpreadDescriptor::AdjustRect(DocRect* pRect) const
{
	if (m_DontScale)
	{
		INT32 xinc = pRect->Width() / 50;			// find 2% of the width and height
		INT32 yinc = pRect->Height() / 50;
		if (xinc < 1) xinc = 1;						// make sure we inflate by some amount
		if (yinc < 1) yinc = 1;
		pRect->lo.x -= xinc;						// inflate the given rectangle
		pRect->lo.y -= yinc;
		pRect->hi.x += xinc;
		pRect->hi.y += yinc;
	}
}



////////////////////////////////////////////////////////////////////////////////////////////
// class OpZoomFitDrawingDescriptor

/********************************************************************************************
>	OpZoomFitDrawingDescriptor::OpZoomFitDrawingDescriptor()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OpDescriptor for the "Zoom to drawing" button.
	Errors:		-
	SeeAlso:	class OpZoomDescriptor
********************************************************************************************/

OpZoomFitDrawingDescriptor::OpZoomFitDrawingDescriptor()
  : OpZoomDescriptor(OPTOKEN_ZOOMDRAWING, _R(IDS_ZOOMDRAWINGSTATUSTEXT),
  					 0, _R(IDBBL_FIT_TO_DRAWING))
{
	// Empty.
}



/********************************************************************************************
>	virtual DocRect OpZoomFitDrawingDescriptor::GetRect(Spread* pSpread)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		pSpread			pointer to a spread
	Outputs:	-
	Returns:	The "bounds" of the drawing on the given spread, ignoring invisible
				layers.
	Purpose:	Provides the rectangle that OpZoomDescriptor::HandleButtonMsg will
				zoom in on.
	Errors:		-
	SeeAlso:	OpZoomDescriptor::HandleButtonMsg; OpZoomFitSpreadDescriptor::AdjustRect;
				Spread::GetPageVisibleBounds
********************************************************************************************/

DocRect OpZoomFitDrawingDescriptor::GetRect(Spread* pSpread)
{
	return pSpread->GetPageVisibleBounds();
}



////////////////////////////////////////////////////////////////////////////////////////////
// class OpZoomFitSelectedDescriptor

/********************************************************************************************
>	OpZoomFitSelectedDescriptor::OpZoomFitSelectedDescriptor()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OpDescriptor for the "Zoom to selected objects" button.
	Errors:		-
	SeeAlso:	class OpZoomDescriptor
********************************************************************************************/

OpZoomFitSelectedDescriptor::OpZoomFitSelectedDescriptor()
  : OpZoomDescriptor(OPTOKEN_ZOOMSELECTED, _R(IDS_ZOOMSELECTEDSTATUSTEXT),
  					 0, _R(IDBBL_FIT_TO_SELECTED))
{
	// Empty.
}



/********************************************************************************************
>	virtual Spread* OpZoomFitSelectedDescriptor::GetSpread(DocView* pDocView) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		pDocView		pointer to a DocView showing selected objects
	Outputs:	-
	Returns:	A pointer to the first spread within the viewed document that
				contains selected objects.
	Purpose:	Provides the spread that OpZoomDescriptor::HandleButtonMsg will zoom
				in on.
	Errors:		-
	SeeAlso:	OpZoomDescriptor::HandleButtonMsg; DocView::GetFirstSelectedSpread
********************************************************************************************/

Spread* OpZoomFitSelectedDescriptor::GetSpread(DocView* pDocView) const
{
	return pDocView->GetFirstSelectedSpread();
}



/********************************************************************************************
>	virtual DocRect OpZoomFitSelectedDescriptor::GetRect(Spread* pSpread)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/94
	Inputs:		pSpread				pointer to the spread containing the selection
	Outputs:	-
	Returns:	The bounding rectangle of the selected objects.
	Purpose:	Assuming that some objects are selected, this calculates the bounding
				rectangle of the selected objects, including blobs.
	Errors:		ERROR2/3 failure if there isn't any selected objects - if this happens
				then something is screwy, as SelRange immediately before reported
				that there are selected objects, or this function wouldn't have been
				called.
	SeeAlso:	OpZoomFitSelectedDescriptor::GetSpread; SelRange::GetBlobBoundingRect
********************************************************************************************/

DocRect OpZoomFitSelectedDescriptor::GetRect(Spread* pSpread)
{
	// If this function is called then we know that there is a valid selection
	// somewhere.
	SelRange* pSel = GetApplication()->FindSelection();

	// Doesn't do any harm to check, though!
	ERROR3IF(pSel == 0 || pSel->FindFirst() == 0,
				"Can't find a selected node in OpZoomFitSelectedDescriptor::GetRect");
	ERROR3IF(pSpread != pSel->FindFirst()->FindParent(CC_RUNTIME_CLASS(Spread)),
				"Spread pointer has changed between calls on SelRange");
	
	// We really need to call the equivalent of GetUnionBlobBoundingRect() for the SelRange,
	// but this function doesn't exist, so we mimic its result here.
	DocRect drSel = pSel->GetBlobBoundingRect().Union(pSel->GetBoundingRect());

	// And convert it into document coordinates
	pSpread->SpreadCoordToDocCoord(&drSel);

	// Return the bounding rectangle of the selection in "proper" document coordinates.
	return drSel;
}



/********************************************************************************************
>	virtual void OpZoomFitSelectedDescriptor::AdjustRect(DocRect* pRect) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/94
	Inputs:		pRect				pointer to a DocRect to adjust
	Outputs:	The adjusted DocRect.
	Returns:	-
	Purpose:	Inflates the given rectangle by the same amount as the selector tool when it
				draws bounds blobs around the selection.  This ensures that after a "Fit to
				Selected" zoom the bounds blobs of the zoomed object will be visible.
	Errors:		-
	SeeAlso:	OpZoomFitSelectedDescriptor::GetRect; SelectorTool::InflateByBlobBorder
********************************************************************************************/

void OpZoomFitSelectedDescriptor::AdjustRect(DocRect* pRect) const
{
#ifndef STANDALONE
	SelectorTool::InflateByBlobBorder(pRect);
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////
// class OpZoomFitRectDescriptor

/********************************************************************************************
>	OpZoomFitRectDescriptor::OpZoomFitRectDescriptor()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OpDescriptor for zooming to a specified rect.
	Errors:		-
	SeeAlso:	class OpZoomDescriptor
********************************************************************************************/

OpZoomFitRectDescriptor::OpZoomFitRectDescriptor()
  : OpZoomDescriptor(OPTOKEN_ZOOMRECT, 0, 0, 0),
	TheRect(0, 0, 0, 0)
{
	// Empty.
}

/********************************************************************************************

>	void OpZoomFitRectDescriptor::SetZoomRect(const DocRect& rect)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/96
	Inputs:		pRect
	Outputs:	-
	Purpose:	Sets the rectangle that the op will zoom to.
	Errors:		-

********************************************************************************************/

void OpZoomFitRectDescriptor::SetZoomRect(const DocRect& rect)
{
	TheRect = rect;
}

/********************************************************************************************
>	virtual DocRect OpZoomFitRectDescriptor::GetRect(Spread* pSpread)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/96
	Inputs:		pSpread			pointer to a spread
	Outputs:	-
	Returns:	The "bounds" of the drawing on the given spread, ignoring invisible
				layers.
	Purpose:	Provides the rectangle that OpZoomDescriptor::HandleButtonMsg will
				zoom in on.
	Errors:		-
	SeeAlso:	OpZoomDescriptor::HandleButtonMsg; OpZoomFitSpreadDescriptor::AdjustRect;
				Spread::GetPageVisibleBounds
********************************************************************************************/

DocRect OpZoomFitRectDescriptor::GetRect(Spread* pSpread)
{
	DocRect t = TheRect;
	// Convert the value from spread coordinates to document coords
	pSpread->SpreadCoordToDocCoord(&t);

	return t;
	//return TheRect;
}


////////////////////////////////////////////////////////////////////////////////////////////
// class OpZoomPrevZoomDescriptor

/********************************************************************************************
>	OpZoomPrevZoomDescriptor::OpZoomPrevZoomDescriptor()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OpDescriptor for the "Previous zoom" button.
	Errors:		-
	SeeAlso:	class OpZoomDescriptor
********************************************************************************************/

OpZoomPrevZoomDescriptor::OpZoomPrevZoomDescriptor()
  : OpZoomDescriptor(OPTOKEN_ZOOMPREV, _R(IDS_ZOOMPREVSTATUSTEXT), 0, _R(IDBBL_PREV_ZOOM))
{
	// Empty.
}



/********************************************************************************************
>	virtual BOOL OpZoomPrevZoomDescriptor::IsAvailable()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the "previous zoom" button is available
	Purpose:	Decides whether the "Previous zoom" button is enabled, which it is if
				there is a "selected" DocView and at least one zoom has been performed
				upon it.
	Errors:		-
	SeeAlso:	OpZoom::GetState; OpZoomDescriptor::IsAvailable
********************************************************************************************/

BOOL OpZoomPrevZoomDescriptor::IsAvailable()
{
	// Check for a selected DocView, if any (the base class IsAvailable does this), and
	// "grey" the button if it doesn't exist or hasn't been zoomed previously.
	DocView* pDocView = DocView::GetCurrent();
	return pDocView != 0 && pDocView->GetPrevZoomIndex() != cUninitZoomIndex;
}



/********************************************************************************************
>	static void OpZoomPrevZoomDescriptor::SaveZoom(DocView* pDocView)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		pDocView		the DocView whose current "zoom setting" is to be saved
	Outputs:	-
	Returns:	-
	Purpose:	Saves the given DocView's view-scale, scroll-offsets, and index into
				OpZoom::ZoomTable.  Used by the "Previous zoom" button.  This function
				is called by all zoom tool routines that perform a zoom.
	Errors:		-
	SeeAlso:	OpZoomPrevZoomDescriptor::HandleButtonMsg; DocView::SetPrevZoomIndex;
				DocView::SetPrevZoomScale; DocView::SetPrevZoomOffset
********************************************************************************************/

void OpZoomPrevZoomDescriptor::SaveZoom(DocView* pDocView)
{
	// Save data about the current zoom.
	ERROR3IF(pDocView == 0, "No DocView in OpZoomPrevZoomDescriptor::SaveZoom");
	if (pDocView != 0)
	{
		pDocView->SetPrevZoomIndex(pDocView->GetZoomTableIndex());
		pDocView->SetPrevZoomScale(pDocView->GetViewScale());
		pDocView->SetPrevZoomOffset(pDocView->GetScrollOffsets());
	}

#if !defined(EXCLUDE_FROM_RALPH)
	// Force the GetState function to be called again.
	DialogBarOp::SetSystemStateChanged();

	// If we have saved a "previous zoom" then there is a new zoom, hence the status-
	// bar text must have become invalid.
	ZoomTool::InvalidateStatusText();
#endif
}



/********************************************************************************************
>	virtual BOOL OpZoomPrevZoomDescriptor::DoZoom(OpZoom* pOpZoom)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/95
	Inputs:		pZoomOp		pointer to the OpZoom to be used, can be 0 for this.
	Returns:	True if operation has happened correctly
	Purpose:	Overrides the base zoom descriptor class function for performing a zoom
				operation. This one is different as it just needs to invoke the previous zoom
				that was in operation.
				Performs the zoom, after saving the current zoom settings, then updates
				the zoom combo box.
				Similar button to HandleButtonMsg but is designed to be called by the Do()
				operator in OpZoom and hence from keyboard short-cuts or menu operations.
	Errors:		-
	SeeAlso:	OpZoom::Do;	OpZoomDescriptor::DoZoom;
				OpZoomDescriptor::GetSpread; OpZoomDescriptor::GetRect;
				OpZoomDescriptor::AdjustRect
********************************************************************************************/

BOOL OpZoomPrevZoomDescriptor::DoZoom(OpZoom* pZoomOp)
{
	// ZoomOp can be null.
//	ERROR2IF(pZoomOp == 0, FALSE, "No operation in OpZoomPrevZoomDescriptor::DoZoom")

	// Get the selected DocView.
	DocView* pDocView = DocView::GetCurrent();
	ERROR2IF(pDocView == 0, FAIL, "No DocView in OpZoomPrevZoomDescriptor::HandleClickMsg");

	DialogBarOp::SetSystemStateChanged();

	// Record the current scale and scroll offset.
	INT32 nIndex = pDocView->GetZoomTableIndex();
	FIXED16 fxScale = pDocView->GetViewScale();
	WorkCoord wcOffset = pDocView->GetScrollOffsets();
	
	// Restore the previous scale factor and scroll offsets.
	pDocView->SetZoomTableIndex(pDocView->GetPrevZoomIndex());
    pDocView->SetViewScale(pDocView->GetPrevZoomScale());
	pDocView->SetScrollOffsets(pDocView->GetPrevZoomOffset(), FALSE);

	// Remember the "previous previous" zoom . . .
	pDocView->SetPrevZoomIndex(nIndex);
	pDocView->SetPrevZoomScale(fxScale);
	pDocView->SetPrevZoomOffset(wcOffset);

#ifndef RALPH
	// Update the combo-box etc.
	OpZoomComboDescriptor::Update();
	ZoomTool::InvalidateStatusText();
#endif

	// Finally, force a redraw and return the success code.
	pDocView->ForceRedraw(TRUE);
	
	// End the operation as we have finished
	if (pZoomOp)
		pZoomOp->End();
	
	// tell people things have changed on screen
	TRACEUSER( "Diccon", _T("DoZoom\n"));
	BROADCAST_TO_ALL(ScreenChangeMsg(TRUE));
	return TRUE;	
}

/********************************************************************************************
>	virtual MsgResult OpZoomPrevZoomDescriptor::HandleButtonMsg(DialogOp*, CGadgetID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		(not used, see OpZoomDescriptor::HandleButtonMsg)
	Outputs:	-
	Returns:	The message result code (always OK)
	Purpose:	Responds to a click on the "Previous zoom" button by reading the
				saved zoom parameters in the "selected" DocView, if any, and resetting
				the DocView to them.
	Errors:		-
	SeeAlso:	OpZoomPrevZoomDescriptor::SaveZoom; DocView::SetZoomTableIndex;
				DocView::SetPrevZoomIndex; DocView::SetPrevZoomOffset;
				DocView::SetPrevZoomScale
********************************************************************************************/

MsgResult OpZoomPrevZoomDescriptor::HandleButtonMsg(DialogOp*, CGadgetID)
{
	// Just call the DoZoom operator but with 0 as we have no valid operation
	return DoZoom(0) ? OK : FAIL;
}



////////////////////////////////////////////////////////////////////////////////////////////
// class OpZoomComboDescriptor

/********************************************************************************************
>	OpZoomComboDescriptor::OpZoomComboDescriptor()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OpDescriptor for the zoom control's combo-box, which
				contains a list of percentage zoom factors.
	Errors:		-
	SeeAlso:	class OpZoomDescriptor
********************************************************************************************/

OpZoomComboDescriptor::OpZoomComboDescriptor()
  :	OpZoomDescriptor(OPTOKEN_ZOOMCOMBO, _R(IDS_ZOOMCOMBOSTATUSTEXT), 0, _R(IDBBL_ZOOM_COMBO), _R(IDCB_ZOOM_COMBO_BOX), _R(IDCB_ZOOM_COMBO_BOX))
{
	
	// Empty.
}



/********************************************************************************************
>	BOOL OpZoomComboDescriptor::IsAvailable()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the zoom combo is available for use, FALSE otherwise.
	Purpose:	Checks if their is a "selected" DocView, if so you can use the zoom combo
				on it.
	Errors:		-
	SeeAlso:	OpZoom::GetState; OpZoomDescriptor::IsAvailable
********************************************************************************************/

BOOL OpZoomComboDescriptor::IsAvailable()
{
	return DocView::GetSelected() != 0;
}



/********************************************************************************************
>	virtual MsgResult OpZoomComboDescriptor::Message(Msg* pMsg)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		pMsg			a pointer to a kernel message to process
	Outputs:	-
	Returns:	A message result code (generally OK)
	Purpose:	Despatches a message, if relevant to the appropriate handler for it,
				after "unpacking" it into a more useful form.
	Errors:		-
	SeeAlso:	OpZoomComboDescriptor::HandleViewChangeMsg;
				OpZoomComboDescriptor::HandleCreateMsg;
				OpZoomComboDescriptor::HandleSelectMsg;
				class DocViewMsg
********************************************************************************************/

MsgResult OpZoomComboDescriptor::Message(Msg* pMsg)
{
	// Does the message mean that something has happened to the DocView?
	if (MESSAGE_IS_A(pMsg, DocViewMsg))
	{
		DocViewMsg* pViewMsg = (DocViewMsg*) pMsg;
		switch (pViewMsg->State)
		{
			case DocViewMsg::BORN:
				// Initialise our data in the DocView when it is constructed.
                pViewMsg->pDocView->SetZoomTableIndex(cDefaultZoomIndex);
				pViewMsg->pDocView->SetPrevZoomIndex(cUninitZoomIndex);
				return OK;

			case DocViewMsg::SELCHANGED:
				// Handle the selected view changing.
				return HandleViewChangeMsg(pViewMsg->pNewDocView);

			case DocViewMsg::SCALECHANGED:
				// Handle the scale being changed. Since it can be set to anything,
				// say that it is fractional so the field is updated properly.
				pViewMsg->pDocView->SetZoomTableIndex(cFractionalZoomIndex);
				return HandleViewChangeMsg(pViewMsg->pDocView);

			default:
				// Ignore all other messages.
				return OK;
		}
	}

	// If we have dropped down the combo-box - make sure it is upto date!
	if (MESSAGE_IS_A(pMsg, OpDescControlMsg))
	{
		OpDescControlMsg* pOpDescControlMsg = (OpDescControlMsg*) pMsg;

		if (pOpDescControlMsg->DlgMsg == DIM_LISTDROPPED)
		{
			if (DocView::GetCurrent() != 0)
			{
// We really don't want the name gallery to have any effect on the zoom dropdown...
//				CreateDisplayScan().Scan();
				Update(TRUE);
			}
		}
	}

	// Check if the message is an OpDesc message.
	if (!MESSAGE_IS_A(pMsg, OpDescMsg)) return OK;
	
	// Cast it into the correct type etc.
	OpDescMsg* pOpDescMsg = (OpDescMsg*) pMsg;
		
	// Is it for us?
	if (pOpDescMsg->OpDesc != this) return OK;
	


	// Process the message . . .
	if (MESSAGE_IS_A(pOpDescMsg, OpDescControlCreateMsg))
		return HandleCreateMsg(pOpDescMsg->pDlgOp, pOpDescMsg->SetGadgetID);

	// Let base class do the default.
	return OpZoomDescriptor::Message(pMsg);
}



/********************************************************************************************
>	MsgResult OpZoomComboDescriptor::HandleViewChangeMsg(DocView* pSelectedDocView)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		pSelectedDocView		points to the DocView that is now "selected",
										or 0 if there isn't a selected view.
	Returns:	A message result code (always OK)
	Purpose:	Updates the zoom control combo-box with the zoom factor of a DocView
				as the user moves from view to view.
	Errors:		Out of memory
	SeeAlso:	OpZoomComboDescriptor::UpdateComboWithViewScale;
				OpZoomComboDescriptor::Message
********************************************************************************************/

MsgResult OpZoomComboDescriptor::HandleViewChangeMsg(DocView* pSelectedDocView)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ZoomTool::InvalidateStatusText();
	String_256 dummy;
	return (pSelectedDocView != 0 
				// There is a new view, so set the combos to the scale factor.
				? UpdateComboWithViewScale(pSelectedDocView)
				// There is no view, so blank the zoom combos.
				: UpdateAllCombos(&dummy))
						? OK : FAIL;
#else
	return OK;
#endif
}



/********************************************************************************************
>	BOOL OpZoomComboDescriptor::UpdateAllCombos(String_256* pStr)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		pStr			the text to be placed in all zoom combos
	Returns:	TRUE if successful.
	Errors:		Out of memory
	Purpose:	Updates the zoom combo-box(es) with the given text.  The function is
				complicated by design deficiencies in the Operation system!
	Errors:		-
	SeeAlso:	OpDescriptor::BuildGadgetList; DocView::GetZoomTableIndex
********************************************************************************************/

BOOL OpZoomComboDescriptor::UpdateAllCombos(String_256* pStr)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// We must set the combos to something!
	ERROR3IF(pStr == 0, "No parameter in OpZoomComboDescriptor::UpdateAllCombos");

	// Create a list for the dialogue manager to put gadget ID's on.
	List* pGadgetList = new List;
	ERRORIF(pGadgetList == 0, _R(IDE_NOMORE_MEMORY), FALSE);

	// See if the dialogue manager can remember where its controls live, or even
	// what identifiers they possess . . .
	if (BuildGadgetList(pGadgetList))
	{	
		// Success.  Walk the generated list
		ListItem* pListItem = pGadgetList->GetHead();
		while (pListItem != 0)
		{
			// Set the edit field of each gadget (which according to Simon will all be
			// combo-boxes) to the given percentage text.
			GadgetListItem* pGadgetItem = (GadgetListItem*) pListItem;
			pGadgetItem->pDialogOp->SetStringGadgetValue(pGadgetItem->gidGadgetID,
															pStr,
															FALSE,
															-1);

			// This can be useful when bamboozled by the Operations system . . .
/*			TRACEUSER( "JustinF", _T("\t\tUpdating zoom combo at 0x%lX (GID# %lX)\n"),
						(UINT32) pGadgetItem, (UINT32) pGadgetItem->gidGadgetID);
*/		
			// Do the next control in the list, if any.
			pListItem = pGadgetList->GetNext(pListItem);
		}
	}
/*	else
	{
		// Something went wrong!
		TRACEUSER( "JustinF", _T("Couldn't build gadget list in ")
							 "OpZoomComboDescriptor::UpdateAllCombos!\n");
	}
*/
	// Don't forget to delete the list afterwards!
	pGadgetList->DeleteAll();
	delete pGadgetList;
#endif

	return TRUE;
}



/********************************************************************************************
>	BOOL OpZoomComboDescriptor::UpdateComboWithViewScale(DocView* pDocView)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		pDocView		the DocView whose zoom factor will appear in the
								zoom control's combo-box.
	Returns:	TRUE if successful.
	Errors:		Out of memory
	Purpose:	Updates the combo-box with the DocView's current percentage zoom
				factor.  This works even if there is more than one combo-box.  The
				function is complicated by design deficiencies in the Operation system!
	Errors:		-
	SeeAlso:	OpDescriptor::BuildGadgetList; DocView::GetZoomTableIndex
********************************************************************************************/

BOOL OpZoomComboDescriptor::UpdateComboWithViewScale(DocView* pDocView)
{
	// Get the DocView's scale percentage.  If it's -1 then we must calculate the
	// percentage from the raw scale factor, otherwise we can look it up.
	INT32 nScalePercent;
	String_256 txt;

	if (pDocView)
	{
		INT32 nTableIndex = pDocView->GetZoomTableIndex();
		if (nTableIndex == cUninitZoomIndex)
		{
			// This value is set by DocView's constructor, and means that the view has not
			// been given a ZoomTableIndex yet.  Give it the default, ie. 100%.
			pDocView->SetZoomTableIndex(nTableIndex = cDefaultZoomIndex);
			nScalePercent = OpZoom::GetPresetZoomPercent(nTableIndex);
		}
		else if (nTableIndex == cFractionalZoomIndex)
		{
			// Convert from a fixed16 scale factor to a percentage.  This must be rounded
			// to the nearest percent.
			nScalePercent = ((pDocView->GetViewScale() * 100) + FIXED16_DBL(0.5)).MakeInt();
		}
		else
		{
			nScalePercent = OpZoom::GetPresetZoomPercent(nTableIndex);
		}
	
		// Convert the percentage to a formatted text string.
		txt.MakeMsg(_R(IDS_ZOOM_INFO_FORMAT), nScalePercent);
	}

	// Update all existing zoom combos with the text.
	return UpdateAllCombos(&txt);
}



/********************************************************************************************
>	BOOL OpZoomComboDescriptor::RefreshList()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Purpose:	Fills the zoom control's combo-box with a list of percentage scale
				factors and set names, corresponding to those held in OpZoom::ZoomTable[]
	Returns:	TRUE if successful.
	Errors:		Out of memory
	SeeAlso:	OpZoomComboDescriptor::Message; DialogOp::SetStringGadgetValue
********************************************************************************************/

BOOL OpZoomComboDescriptor::RefreshList()
{
	List* pList = new List;
	ERRORIF(pList == 0, _R(IDE_NOMORE_MEMORY), FALSE);

	// See if the dialogue manager can remember where its controls live, or even
	// what identifiers they possess . . .
	if (BuildGadgetList(pList))
	{	
		// Success.  Walk the generated list
		ListItem* pListItem = pList->GetHead();
		while (pListItem != 0)
		{
			DialogOp* pBarOp = ((GadgetListItem*) pListItem)->pDialogOp;
			CGadgetID gid = ((GadgetListItem*) pListItem)->gidGadgetID;

			// Start with a clean sheet.
			pBarOp->DeleteAllValues(gid);

			// Set each percentage zoom factor at successive positions in the combo-box.
			String_256 txt;
			INT32 i;
			for (i = 0; i < cZoomTableSize; i++)
			{
				txt.MakeMsg(_R(IDS_ZOOM_INFO_FORMAT), OpZoom::GetPresetZoomPercent(i));
				pBarOp->SetStringGadgetValue(gid, &txt, FALSE, i);
			}

			// Insert a divider so it all looks nice.
			String_256 strDivide(_R(IDS_ZOOM_COMBO_DIVIDER));
			pBarOp->SetStringGadgetValue(gid, &strDivide, FALSE, i++);

			// Add the text commands after the percentages.
			txt.Load(_R(IDS_ZOOMSPREADCOMBOTEXT));
			pBarOp->SetStringGadgetValue(gid, &txt, FALSE, i++);
			txt.Load(_R(IDS_ZOOMDRAWINGCOMBOTEXT));
			pBarOp->SetStringGadgetValue(gid, &txt, FALSE, i++);

#ifndef STANDALONE
			txt.Load(_R(IDS_ZOOMSELECTEDCOMBOTEXT));
			pBarOp->SetStringGadgetValue(gid, &txt, FALSE, i++);
#endif

			txt.Load(_R(IDS_ZOOMPREVIOUSCOMBOTEXT));
			pBarOp->SetStringGadgetValue(gid, &txt, FALSE, i++);

			// Set the required depth of the combo-box when it drops down.
			pBarOp->SetComboListLength(gid);

			// Blank the string to start off with
			txt=_T("");
			pBarOp->SetStringGadgetValue(gid, &txt, FALSE, -1);

			// Do the next control in the list, if any.
			pListItem = pList->GetNext(pListItem);
		}

		pList->DeleteAll();
	}

	delete pList;
	return TRUE;
}



/********************************************************************************************
>	MsgResult OpZoomComboDescriptor::HandleCreateMsg(DialogOp* popdlgThis,
													 CGadgetID gidThis)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		popdlgThis			points to a DialogOp (a control?)
				gidThis				gadget ID of control
	Outputs:	-
	Returns:	A message result (always EAT_MSG)
	Purpose:	Fills the zoom control's combo-box with a list of percentage scale
				factors, corresponding to those held in OpZoom::ZoomTable[]
	Errors:		-
	SeeAlso:	OpZoomComboDescriptor::Message; DialogOp::SetStringGadgetValue
********************************************************************************************/

MsgResult OpZoomComboDescriptor::HandleCreateMsg(DialogOp* popdlgThis, CGadgetID gidThis)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Delegate delegate delegate!
	if (!RefreshList()) return FAIL;

	// Make sure the visuals reflect all this.
	DocView* pDocView = DocView::GetCurrent();
	if (pDocView != 0) UpdateComboWithViewScale(pDocView);
#endif
	// Processed message successfully.
	return EAT_MSG;
}



/********************************************************************************************
>	void OpZoomComboDescriptor::OnSelectionChange(OpDescControlMsg* pCtrlMsg,
												  List* pGadgetList)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Inputs:		pCtrlMsg	---		pointer to the message from a control
				pGadgetList	---		list of all gadgets associated with this OpDescriptor.
	Outputs:	-
	Returns:	-
	Purpose:	Handles the user changing the selection, or typing text, in the zoom factor
				combo-box.   Reads the selected text, checking for "%" or "x" which
				indicates a percentage zoom factor (absolute) or a multiplier (relative),
				converts to a number and performs the requested zoom on the selected
				view.
	Errors:		-
	SeeAlso:	OpZoomComboDescriptor::StringToLong; OpZoomDescriptor::FakeInvoke
********************************************************************************************/

void OpZoomComboDescriptor::OnSelectionChange(OpDescControlMsg* pCtrlMsg, List*)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Check that we have reasonable input.
	ERROR3IF(pCtrlMsg == 0 || pCtrlMsg->pDlgOp == 0,
				"No OpDescriptor message in OpZoomComboDescriptor::OnSelectionChange");
	if (pCtrlMsg == 0 || pCtrlMsg->pDlgOp == 0) return;

	// Get the selected DocView etc.
	DocView* pDocView = DocView::GetCurrent();
	ERROR3IF(pDocView == 0,
				"No current DocView in OpZoomComboDescriptor::OnSelectionChange");
	if (pDocView == 0) return;

	// If the selected text is actually one of the zoom commands, eg. "(Fit to) Spread",
	// then dispatch that to the appropriate handler.
	WORD wComboIndex;
	pCtrlMsg->pDlgOp->GetValueIndex(pCtrlMsg->SetGadgetID, &wComboIndex);
	INT32 nComboIndex = (short) wComboIndex;
 	switch (nComboIndex)
	{
		case cFitToSpreadIndex:
			FakeInvoke(OPTOKEN_ZOOMSPREAD);
			return;

		case cFitToDrawingIndex:
			FakeInvoke(OPTOKEN_ZOOMDRAWING);
			return;

		case cFitToSelectedIndex:
			FakeInvoke(OPTOKEN_ZOOMSELECTED);
			return;

		case cPreviousZoomIndex:
			FakeInvoke(OPTOKEN_ZOOMPREV);
			return;

		case cComboDivider1:
		case cComboDivider2:
//			Beep();
			wxBell();
			OpZoomComboDescriptor::Update();
			return;

		default:
			break;
	}

	// Have we selected a set item from the list?
// We really don't want the name gallery to have any effect on the zoom dropdown...
/*	INT32 n = nComboIndex - cComboDivider2;
	if (n > 0)
	{
		NameGallery *pNameGallery = NameGallery::Instance();
		if (pNameGallery)
		{
			SGUsedNames* pUsedNames = pNameGallery->GetUsedNames();
			if (pUsedNames != 0)
			{
				SGNameItem* pItem = (SGNameItem*) pUsedNames->GetChild();
				if (pItem != 0)
				{
					for (; pItem != 0; pItem = (SGNameItem*) pItem->GetNext())
						if (!pItem->GetSetBounds().IsEmpty() && --n == 0)
						{
							// Extract the bounds of the chosen set and zoom to them.
							ERROR3IF(!pItem->GetSetBounds().IsValid(),
								"OpZoomComboDescriptor::OnSelectionChange: invalid bounds");
							OpZoomDescriptor::FakeZoomToRect(pItem->GetSetBounds());
							return;
						}

					Beep();
					ERROR3("OpZoomComboDescriptor::OnSelectionChange: can't find SGNameItem");
					return;
				}
			}
		}
	}
*/

	// Extract the highlighted text.  We pass a pointer to our "StringToLong" function
	// that knows how to extract a zoom factor from the user's input.
	BOOL bIsValid;
	INT32 nPercent = pCtrlMsg->pDlgOp->GetLongGadgetValue(pCtrlMsg->SetGadgetID,
														 INT32_MIN, INT32_MAX,
														 _R(IDE_INVALIDZOOMFACTOR),
														 &bIsValid, StringToLong);
	
    // Check if the input is valid.  If it isn't then reset the result.  The
    // GetStringGadgetValue will have reported the error if the input isn't valid.
	if (!bIsValid)
	{
		UpdateComboWithViewScale(pDocView);
		return;
	}

	// If the returned value is negative it represents a "multiplier", which must
	// first be converted to a percentage.
	if (nPercent < 0)
	{
		INT32 nIndex = pDocView->GetZoomTableIndex();
		if (nIndex == cFractionalZoomIndex)
			nPercent = -nPercent * ((pDocView->GetViewScale() * 100) + FIXED16(0.5)).MakeInt();
		else
			nPercent = -nPercent * OpZoom::GetPresetZoomPercent(nIndex);

		// If the zoom factor is larger than the permitted maximum then adjust it.
		INT32 nMaxPercent = OpZoom::GetPresetZoomPercent(0);
		if (nPercent > nMaxPercent) nPercent = nMaxPercent;
	}

	// Begin by assuming the percentage isn't in the table.
	INT32 nIndex = cFractionalZoomIndex;

	// Look for the percentage in the zoom factor table.
	for (INT32 i = 0; i < cZoomTableSize; i++)
	{
		if (OpZoom::GetPresetZoomPercent(i) == nPercent)
		{
			// Found the percentage in the table.
			nIndex = i;
			break;
		}
	}

	// Try to create an OpZoom operation.
	OpZoom* pZoomOp = new OpZoom;
	if (pZoomOp == 0)
	{
		// We ran out of memory, so sorry.
		InformError(_R(IDE_NOMORE_MEMORY));
		return;
	}

	// Find the mid-point of the view, in work coordinates.
	WorkRect wrView = pDocView->GetViewRect();
	WorkCoord wcMidView((wrView.lo.x + wrView.hi.x) / 2,
						(wrView.lo.y + wrView.hi.y) / 2);
	
	// Ask the operation to zoom at this point.
	pDocView->SetZoomTableIndex(nIndex);
	pZoomOp->ZoomAtPoint(wcMidView, ((FIXED16) (INT32) nPercent) / 100);
#endif
}



/********************************************************************************************
>	static BOOL OpZoomComboDescriptor::StringToLong(const StringBase& strIn, INT32* pnOut)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Inputs:		pstrIn		---		pointer to the string containing text
				pnOut		--- 	pointer to the number which will contain the result
	Outputs:	-
	Returns:	TRUE if the text is successfully parsed, FALSE otherwise.
	Purpose:	Parses the text within the zoom factor combo-box, checking for a percentage
				factor ("%") or a multiplier factor ("x"), converting it to a number.
	Errors:		-
	SeeAlso:	OpZoomComboDescriptor::OnSelectionChange; DialogOp::GetLongGadgetValue
********************************************************************************************/

BOOL OpZoomComboDescriptor::StringToLong(const StringBase& strIn, INT32* pnOut)
{
	const StringBase * pstrIn = &strIn;
	ERROR2IF(pstrIn == 0 || pnOut == 0, FALSE,
			"No parameter(s) in OpZoomComboDescriptor::StringToLong");

	// Remove any leading or trailing whitespace.  psczStart is set to point to first non-
	// whitespace character (or the terminating null), pszEnd points to the last.  If the
	// string is entirely whitespace these two will cross over, and we return FALSE.
	const TCHAR* psczStart = (const TCHAR*) *pstrIn;
	const TCHAR* psczForward = psczStart;
	const TCHAR* psczBack = psczStart + pstrIn->Length() - 1;

	while (StringBase::IsSpace(*psczForward))
		psczForward = _tcsinc(psczForward);
	
	while (psczBack > psczForward && StringBase::IsSpace(*psczBack))
		psczBack = _tcsdec(psczStart, psczBack);
	
	if (psczForward >= psczBack) return FALSE;

	// Check if the string ends with a '%' or an 'x'.  If it is an 'x' then the number
	// is a multipler, eg. "2 x" (two times).  If it is a '%', or not there at all,
	// then it represents a percentage.
	BOOL bIsMultiplier = FALSE;
	if (*psczBack == TEXT('x') || *psczBack == TEXT('X'))
	{
		// Parse a multiplier.  Skip over the 'x'.
		psczBack--;
		bIsMultiplier = TRUE;
	}
	else if (*psczBack == TEXT('%'))
	{
		// Parse a percentage.  Skip over the '%'
		psczBack--;
	}
	else if (!StringBase::IsNumeric(*psczBack))
	{
		// Can't recognise the format - not a number.
		return FALSE;
	}

	// Make a working copy of what is left of the string.
	String_256 strWork;
	pstrIn->Mid(&strWork, (INT32) (psczForward - psczStart),
				(INT32) (psczBack - psczForward) + 1);

	// Convert the remaining text into a number and return it.  Percentages and
	// multipliers cannot be negative or zero (especially as multipliers are returned
	// back as negative to distinguish them from percentages).
	if (!Convert::StringToLong(strWork, pnOut) || *pnOut <= 0) return FALSE;
	
	// Make sure it's within allowed bounds.
	INT32 nMaxPercent = OpZoom::GetPresetZoomPercent(0);
	INT32 nMinPercent = OpZoom::GetPresetZoomPercent(cZoomTableSize - 1);

	if (*pnOut > nMaxPercent) *pnOut = nMaxPercent;
	if (*pnOut < nMinPercent && !bIsMultiplier) *pnOut = nMinPercent;    
    if (bIsMultiplier) *pnOut = -(*pnOut);

	return TRUE;
}



/********************************************************************************************
>	static BOOL OpZoomComboDescriptor::Update(BOOL fRefreshList = FALSE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		fRefreshList	---		if TRUE then regenerate all items in the
										list (by default, just regenerate the item
										shown in the edit field).
	Purpose:	Gets a pointer to the (one and only) OpZoomComboDescriptor in the
				system, and calls its member function UpdateComboWithViewScale for
				the "selected" DocView, if any.  Available publicly to other zoom
				tool classes, to be called when they modify a DocView's zoom factor.
	Returns:	FALSE if there's an error.
	Errors:		Out of memory.
	SeeAlso:	OpZoomComboDescriptor::UpdatecomboWithViewScale;
				OpDescriptor::FindOpDescriptor; DocView::GetSelected
********************************************************************************************/

BOOL OpZoomComboDescriptor::Update(BOOL fRefreshList)
{
	// Get the "selected" DocView, ie. the view that the user has made top-most.
	DocView* pDocView = DocView::GetCurrent();
	ERROR3IF(pDocView == 0, "No current DocView in OpZoomComboDescriptor::Update");
	if (pDocView == 0) return FALSE;

// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER
PORTNOTE("other", "Removed Ruler usage");
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// update the rulers after every scale change
	if (pDocView->GetpRulerPair()) pDocView->GetpRulerPair()->Update();
#endif
#endif // WEBSTER

	// Find the one and only OpZoomComboDescriptor object.
	OpDescriptor* popd = OpDescriptor::FindOpDescriptor(OPTOKEN_ZOOMCOMBO);
	ERROR3IF(popd == 0, "Can't find OpZoomComboDescriptor in "
						 "OpZoomFitSelectedDescriptor::HandleButtonMsg");
	if (popd == 0) return FALSE;
	
	// Update it with values of the Selected DocView.
	OpZoomComboDescriptor* pCombo = (OpZoomComboDescriptor*) popd;
	if (fRefreshList && !pCombo->RefreshList()) return FALSE;
	return pCombo->UpdateComboWithViewScale(pDocView);

}


#if !defined(EXCLUDE_FROM_RALPH)

////////////////////////////////////////////////////////////////////////////////////////////
// Classes ZoomInfoBarOp and ZoomInfoBarOpCreate.

/********************************************************************************************
>	ZoomInfoBarOp::ZoomInfoBarOp()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a zoom info-bar operation.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/
#if 0
ZoomInfoBarOp::ZoomInfoBarOp()
  :	InformationBarOp(0)
{
	// Empty.
}
#endif


/********************************************************************************************
>	virtual MsgResult ZoomInfoBarOp::Message(Msg* pMsg)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/95
	Inputs:		-
	Outputs:	-
	Returns:	The result of processing the message.
	Purpose:	Handles a Camelot message for the zoom info-bar.
	Errors:		-
	SeeAlso:	DialogBarOp::Message
********************************************************************************************/

MsgResult ZoomInfoBarOp::Message(Msg* pMsg)
{
	// Pass on to base class for default handling.
	return DialogBarOp::Message(pMsg);
}



/********************************************************************************************
>	virtual DialogBarOp* ZoomInfoBarOpCreate::Create()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to a DialogBarOp, which represents an info-bar.
	Purpose:	Constructs a ZoomInfoBarOp (or derivative) on the heap and returns a
				pointer to it.  Used internally by the info-bar system.
	Errors:		-
	SeeAlso:	class ZoomInfoBarOp; class InformationBarOp
********************************************************************************************/

DialogBarOp* ZoomInfoBarOpCreate::Create()
{
	return new ZoomInfoBarOp;
}

#endif
