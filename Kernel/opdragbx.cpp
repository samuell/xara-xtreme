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

	opdragbx.cpp

	Base-class for easy drag box operations.

*/

#include "camtypes.h"
#include "opdragbx.h"
//#include "app.h"
#include "docview.h"
#include "rndrgn.h"
//#include "stockcol.h"


// Revision information.
DECLARE_SOURCE("$Revision$");

// Standard preliminaries.  We don't use new anywhere but if we ever do this will help us
// use it properly (hee hee).
CC_IMPLEMENT_DYNCREATE(OpDragBox, Operation)
CC_IMPLEMENT_DYNCREATE(OpCentredDragBox, OpDragBox)
#define new CAM_DEBUG_NEW



/**************************************************************************************
	Preference:	DoSolidDragBoxes
	Section:	Dragging
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Range:		FALSE or TRUE
	Purpose:	Determines how the drag operation base-class draws a drag box.  If
				FALSE then the "tranditional" unfilled xor rectangle with a certain
				line colour is drawn.  If FALSE a filled ("solid") xor rectangle of a
				certain	line and fill colour will be drawn.  The default is FALSE.
***************************************************************************************/

BOOL OpDragBox::m_fDoSolidDragBoxes = FALSE;



/********************************************************************************************
>	static BOOL OpDragBox::Init()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Returns:	TRUE if successful, FALSE if otherwise.
	Purpose:	Initialises the OpDragBox base-class drag operation.  Loads preferences.
********************************************************************************************/

BOOL OpDragBox::Init()
{
	return GetApplication()->DeclareSection(TEXT("Dragging"), 5) &&
		   GetApplication()->DeclarePref(TEXT("Dragging"), TEXT("DoSolidDragBoxes"),
							   			 &m_fDoSolidDragBoxes, FALSE, TRUE);
}



/********************************************************************************************
>	OpDragBox::OpDragBox()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Purpose:	Default constructor.  Only accessible to derived classes.
	SeeAlso:	class OpCentredDragBox
********************************************************************************************/

OpDragBox::OpDragBox()
  :	m_pStartSpread(NULL)			// no drag has been started yet
{
	// Empty.
}



/********************************************************************************************
>	BOOL OpDragBox::DoDrag(DragType dt,
						   Spread* pStartSpread,
						   const DocCoord& dcStartPos,
						   ClickModifiers cmods)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95
	Inputs:		dt				what kind of drag to start, eg. DRAGTYPE_AUTOSCROLL
				pSpread			the spread containing the anchor (start) point of the drag
				dcStartPos		the anchor (start) point of the drag within the spread
				cmods			the mouse click modifier state
	Returns:	TRUE if a drag is successfully started
	Purpose:	This is called to start a drag box operation.
	SeeAlso:	OpDragBox::OnDragStarted
********************************************************************************************/

BOOL OpDragBox::DoDrag(DragType dt, Spread* pStartSpread,
					   const DocCoord& dcStartPos, ClickModifiers cmods)
{
	// We use a null spread to indicate that a drag hasn't started yet, so check for this
	// when we reckon we should have a valid spread.
	ERROR3IF(pStartSpread == NULL, "Null StartSpread* passed to OpDragBox::DoDrag");
	
	// Initialise the drag box record to an empty rectangle.
	m_drLastDragBox.lo = m_drLastDragBox.hi = dcStartPos;

	// Try to start the drag.  Note that we don't touch m_pStartSpread if start drag fails,
	// so GetDragStartPos will correctly return "no drag started" if the StartDrag call
	// below fails.
	return StartDrag(dt) && OnDragStarted(m_pStartSpread = pStartSpread,
										  m_dcStartPos = dcStartPos,
										  cmods);
}



/********************************************************************************************
>	BOOL OpDragBox::GetDragStartPos(Spread** ppStartSpread, DocCoord* pdcStartPos) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Outputs:	ppStartSpread		where to store the spread that the drag started on
				pdcStartPos			where to store the position that the drag started
	Returns:	TRUE if a drag has been started, FALSE if one hasn't been started yet
	Purpose:	Gets the the start position of the drag operation, if one has been started.
				Note that either of the output paramters may be NULL, which means that the
				caller doesn't require that particular output.
********************************************************************************************/

BOOL OpDragBox::GetDragStartPos(Spread** ppStartSpread, DocCoord* pdcStartPos) const
{
	// If a drag hasn't started yet then return fail code.
	if (m_pStartSpread == NULL) return FALSE;

	// Copy the required information to the output parameters and return that a drga has
	// been started.
	if (ppStartSpread != NULL) *ppStartSpread = m_pStartSpread;
	if (pdcStartPos != NULL) *pdcStartPos = m_dcStartPos;
	return TRUE;
}



/********************************************************************************************
>	static inline MILLIPOINT OpDragBox::Min(MILLIPOINT a, MILLIPOINT b)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95
	Purpose:	Helper function to return the smallest of two ordinates.
********************************************************************************************/

inline MILLIPOINT OpDragBox::Min(MILLIPOINT a, MILLIPOINT b)
{
	return (a < b) ? a : b;
}



/********************************************************************************************
>	static inline MILLIPOINT OpDragBox::Max(MILLIPOINT a, MILLIPOINT b)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95
	Purpose:	Helper function to return the largest of two ordinates.
********************************************************************************************/

inline MILLIPOINT OpDragBox::Max(MILLIPOINT a, MILLIPOINT b)
{
	return (a > b) ? a : b;
}



/********************************************************************************************
>	void OpDragBox::DragPointerMove(DocCoord dcMousePos, ClickModifiers cmods, Spread* pSpread, BOOL bSolidDrag)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		dcMousePos		the current position of the mouse
				cmods			the state of the mouse modifiers
				pSpread			the spread containing the mouse
	Purpose:	This is called every time the mouse moves, during a drag.  It updates the
				xored drag box and calls the OnPointerMoved function so a derived class can
				do something useful.
	SeeAlso:	OpDragBox::OnPointerMoved; OpDragBox::UpdateSolidDragBox;
				OpDragBox::UpdateUnfilledDragBox
********************************************************************************************/

void OpDragBox::DragPointerMove(DocCoord dcMousePos, ClickModifiers cmods, Spread* pSpread, BOOL bSolidDrag)
{
	// If the mouse has moved to another spread than translate its position into the
	// new spread's coordinate system.
	if (pSpread != m_pStartSpread && pSpread != NULL)
	{
		dcMousePos = MakeRelativeToSpread(m_pStartSpread, pSpread, dcMousePos);
	}

	// Calculate the new drag rectangle.
	DocRect drNewDragBox = CalcDragBox(m_dcStartPos, dcMousePos);
	ERROR3IF(!drNewDragBox.IsValid(), "Invalid drag box in OpDragBox::DragPointerMove");

	// Call the appropriate update function, according to whether we are drawing solid
	// drag boxes or unfilled drag boxes.
	if (m_fDoSolidDragBoxes)
	{
		UpdateSolidDragBox(drNewDragBox);
	}
	else
	{
		UpdateUnfilledDragBox(drNewDragBox);
	}
	
	// Update our record of the last drag box and call the derived class.
	if (!OnPointerMoved(m_pStartSpread, m_drLastDragBox = drNewDragBox, cmods))
	{
		// Cancel the drag and operation.
		EndDrag();
		End();
	}
}



/********************************************************************************************
>	void OpDragBox::UpdateSolidDragBox(const DocRect& drNewDragBox)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		drNewDragBox		the new extent of the drag box, in the spread
									coordinates of the spread where the drag started
	Purpose:	Calculates which rectangles to xor to change the last drawn solid drag box
				into the new solid drag box.
	SeeAlso:	OpDragBox::DragPointerMove; OpDragBox::UpdateUnfilledDragBox
********************************************************************************************/

void OpDragBox::UpdateSolidDragBox(const DocRect& drNewDragBox)
{
	// Set up four update rectangles.  We will xor none, some, or all of these rectangles to
	// produce the new drag box.
	DocRect drUpdate[4];
	INT32 nHowMany;

	// Find the intersection of the last and new rectangles.  We will exclude this from
	// any xoring as it is already xored.
	DocRect drCommonDragBox = drNewDragBox.Intersection(m_drLastDragBox);

	// Calculate the rectangles that need to be xored to change the last into the new
	// drag box.  This depends on how they overlap.
	if (drCommonDragBox.IsEmpty())
	{
		// There's no intersection between the last drag box and the new drag box, even
		// though they share a common corner.  So xor the full extent of both drag rects.
		drUpdate[0] = m_drLastDragBox;
		drUpdate[1] = drNewDragBox;
		nHowMany = 2;
	}
	else if (drNewDragBox.ContainsRect(m_drLastDragBox))
	{
		// The new drag rect completely contains the last one, so clip out the last.
		nHowMany = ((DocRect&) drNewDragBox).SplitRect(m_drLastDragBox, drUpdate);
	}
	else if (m_drLastDragBox.ContainsRect(drNewDragBox))
	{
		// The last drag rect completely contains the new one, so clip out the new.
		nHowMany = m_drLastDragBox.SplitRect(drNewDragBox, drUpdate);
	}
	else
	{
		// The drag rectangles overlap but neither completely contains the other, so set
		// the xor rectangles to be each drag rectangle less the common drag rectangle.
		nHowMany = m_drLastDragBox.SplitRect(drCommonDragBox, &drUpdate[0]);
		nHowMany += ((DocRect&) drNewDragBox).SplitRect(drCommonDragBox, &drUpdate[nHowMany]);
	}

	// Sanity check.
	ERROR3IF(nHowMany < 0 || nHowMany > 4,
				"Wrong number of split rects in OpDragBox::UpdateSolidDragBox\n");

	// Draw the xor rects.
	for (INT32 i = 0; i < nHowMany; i++)
	{
		DrawXorRect(drUpdate[i], m_pStartSpread, drUpdate[i]);
	}
}



/********************************************************************************************
>	void OpDragBox::UpdateUnfilledDragBox(const DocRect& drNewDragBox)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		drNewDragBox		the new extent of the drag box, in the spread
									coordinates of the spread where the drag started
	Purpose:	Calculates which rectangles to xor to change the last drawn solid drag box
				into the new solid drag box.
	SeeAlso:	OpDragBox::DragPointerMove; OpDragBox::UpdateSolidDragBox
********************************************************************************************/

void OpDragBox::UpdateUnfilledDragBox(const DocRect& drNewDragBox)
{
	// Remove the old drag box.
	DrawXorRect(m_drLastDragBox, m_pStartSpread, m_drLastDragBox);

	// Draw the new drag box.
	DrawXorRect(drNewDragBox, m_pStartSpread, drNewDragBox);
}



/********************************************************************************************
>	void OpDragBox::DragPointerIdle(DocCoord dcMousePos, ClickModifiers cmods, Spread* pSpread, BOOL bSolidDrag)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		dcMousePos		the current position of the mouse
				cmods			the state of the mouse modifiers
				pSpread			the spread containing the mouse
	Purpose:	This is called every time the mouse hasn't moved for a while during a drag. 
				It calls the OnPointerIdle function so a derived class can do something
				useful with this time, such as hit-testing etc.
	SeeAlso:	OpDragBox::OnPointerIdle
********************************************************************************************/

void OpDragBox::DragPointerIdle(DocCoord dcMousePos, ClickModifiers cmods, Spread* pSpread, BOOL bSolidDrag)
{
	// If the mouse has moved to another spread than translate its position into the
	// new spread's coordinate system.
	if (pSpread != m_pStartSpread && pSpread != NULL)
	{
		dcMousePos = MakeRelativeToSpread(m_pStartSpread, pSpread, dcMousePos);
	}

	// Calculate the new drag rectangle.  Note that as the mouse is supposed to be
	// stationary we do NOT redraw the drag box or update our records of it.
	DocRect drNewDragBox = CalcDragBox(m_dcStartPos, dcMousePos);
	
	// Call the derived class, optionally cancelling the drag.
	if (!OnPointerIdle(m_pStartSpread, drNewDragBox, cmods))
	{
		// Cancel the drag and operation.
		EndDrag();
		End();
	}
}



/********************************************************************************************
>	void OpDragBox::DragFinished(DocCoord dcMousePos, ClickModifiers cmods,
								 Spread* pSpread, BOOL fOK, BOOL bSolidDrag)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95
	Inputs:		dcMousePos		the last position of the mouse
				cmods			the mouse click modifier state at the end
				pSpread			the spread containing the mouse
				fOK				whether the drag was cancelled or not
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes.  It removes the drag box and
				calls the OnEndDrag function in a derived class.
	SeeAlso:	OpDragBox::OnDragEnded
********************************************************************************************/

void OpDragBox::DragFinished(DocCoord dcMousePos, ClickModifiers cmods,
							 Spread* pSpread, BOOL fOK, BOOL bSolidDrag)
{
	// Remove the last drag box.
	RenderDragBlobs(m_drLastDragBox, m_pStartSpread, bSolidDrag);

	// It's possible that the mouse is in a different position to where it was on the last
	// call to DragPointerMove, so recalculate the drag box (but don't draw it, of course).
	// Here, check for a change in the spread containing the mouse.
	if (pSpread != m_pStartSpread && pSpread != NULL)
	{
		dcMousePos = MakeRelativeToSpread(m_pStartSpread, pSpread, dcMousePos);
	}

	// Recalculate the drag box.
	m_drLastDragBox = CalcDragBox(m_dcStartPos, dcMousePos);

	// Call the derived class handler, passing the drag box and the success code.
	if (!OnDragEnded(m_pStartSpread, m_drLastDragBox, cmods, EndDrag() && fOK))
	{
		FailAndExecute();
	}

	// Finally, end the operation.
	End();
}



/********************************************************************************************
>	void OpDragBox::RenderDragBlobs(DocRect drClip, Spread* pSpread, BOOL bSolidDrag)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/05
	Inputs:		drClip			the clip rectangle to draw against
				pSpread			the spread containing the clip rectangle
	Purpose:	Xors a rectangle onto the screen to mark out the size of the current
				drag box.
	SeeAlso:	OpDragBox::DrawXorRect
********************************************************************************************/

void OpDragBox::RenderDragBlobs(DocRect drClip, Spread* pSpread, BOOL bSolidDrag)
{
	DrawXorRect(drClip, pSpread, m_drLastDragBox);
}



/********************************************************************************************
>	void OpDragBox::DrawXorRect(const DocRect& drClip, Spread* pspdXor,
								const DocRect& drXor) const
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95
	Inputs:		drClip			the clipping rectangle to draw against
				pspdXor			pointer to the spread to draw the xor rect onto
				drXor			the position of the xor rect within the spread
	Purpose:	Xors the given rectangle, clipping against the clip rectangle.  Works out
				how to fill the rectangle according to the DoSolidDragBoxes preference.
	SeeAlso:	OpDragBox::DragPointerMove; OpDragBox::RenderDragBlobs
********************************************************************************************/

void OpDragBox::DrawXorRect(const DocRect& drClip, Spread* pspdXor, const DocRect& drXor) const
{
	// Check if we have nothing to do.
	if (drXor.IsEmpty()) return;

	// Ask the derived class what colour its drag box is.  If we are doing solid drags we
	// want to use the same colour to fill the box as well, otherwise we fill with no colour.
	StockColour scLineColour = GetBoxColour();
	StockColour scFillColour = (m_fDoSolidDragBoxes) ? scLineColour : COLOUR_NONE;
	
	// Xor the rect on all supplied render-regions.
	RenderRegion* pRegion = DocView::RenderOnTop(&((DocRect&) drClip), pspdXor, ClippedEOR);
	while (pRegion != NULL)
	{
		// Set the line and fill colours.
		pRegion->SetLineColour(scLineColour);
		pRegion->SetFillColour(scFillColour);

		// Draw the xored drag box and go on to the next render-region.
		pRegion->DrawDragRect(&((DocRect&) drXor));
		pRegion = DocView::GetNextOnTop(&((DocRect&) drClip));
	}
}



/********************************************************************************************
>	virtual DocRect OpDragBox::CalcDragBox(const DocCoord& dcStartPos,
										   const DocCoord& dcMousePos) const
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		dcStartPos			the start position of the drag
				dcMousePos			the current position of the mouse
	Outputs:	-
	Returns:	The extent of the drag box, as a millipoint DocRect.
	Purpose:	Calculates the current extent of the drag box.  The default implementation
				takes the start point as one corner and the mouse point as another corner,
				but you could, for example, override this to take the start point as the
				centre of the drag box (useful for the zoom tool).
	Errors:		-
	SeeAlso:	OpDragBox::DragPointerMove; OpDragBox::DragFinished
********************************************************************************************/

DocRect OpDragBox::CalcDragBox(const DocCoord& dcStartPos, const DocCoord& dcMousePos) const
{
	return DocRect(Min(dcStartPos.x, dcMousePos.x),
				   Min(dcStartPos.y, dcMousePos.y),
				   Max(dcStartPos.x, dcMousePos.x),
				   Max(dcStartPos.y, dcMousePos.y));
}



/********************************************************************************************
>	virtual StockColour OpDragBox::GetBoxColour() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95
	Returns:	The stock colour of the drag box's line.  By default this is
				COLOUR_XORSELECT.
	Purpose:	Allows customisation of the colour of the drag box.
	SeeAlso:	OpDragBox::DrawXorRect
********************************************************************************************/

StockColour OpDragBox::GetBoxColour() const
{
	return COLOUR_XORSELECT;
}



/********************************************************************************************
>	virtual BOOL OpDragBox::OnDragStarted(Spread* pStartSpread,
										  const DocCoord& dcStartPos,
										  ClickModifiers cmods)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95
	Inputs:		pStartSpread		the spread where the drag was started
				dcStartPos			the point within the spread where the drag was started
				cmods				the state of the mouse click modifiers
	Returns:	FALSE to cancel the drag, TRUE to continue.
	Purpose:	By default does nothing.  This function can be overriden to so something
				useful (eg. set the status bar text).  Note that this function will NOT be
				called if Operation::StartDrag fails.  If you return FALSE then the DoDrag()
				call will also return FALSE (generally notifying the op runner that the
				drag failed).
	SeeAlso:	OpDragBox::DoDrag
********************************************************************************************/

BOOL OpDragBox::OnDragStarted(Spread*, const DocCoord&, ClickModifiers)
{
	return TRUE;
}



/********************************************************************************************
>	virtual BOOL OpDragBox::OnPointerMoved(Spread* pBoxSpread,
										   const DocRect& drDragBox,
										   ClickModifiers cmods)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95
	Inputs:		pBoxSpread			the spread where the drag was started
				drDragBox			the rectangle within the spread defining the drag box
				cmods				the state of the mouse click modifiers
	Returns:	FALSE to cancel the drag, TRUE to continue.
	Purpose:	By default does nothing.  This function can be overriden to so something
				useful (eg. set the status bar text).  Note that if you return FALSE from
				this function EndDrag() and End() will be called.
	SeeAlso:	OpDragBox::DragPointerMove
********************************************************************************************/

BOOL OpDragBox::OnPointerMoved(Spread*, const DocRect&, ClickModifiers)
{
	return TRUE;
}



/********************************************************************************************
>	virtual BOOL OpDragBox::OnPointerIdle(Spread* pBoxSpread,
										  const DocRect& drDragBox,
										  ClickModifiers cmods)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95
	Inputs:		pBoxSpread			the spread where the drag was started
				drDragBox			the rectangle within the spread defining the drag box
				cmods				the state of the mouse click modifiers
	Returns:	FALSE to cancel the drag, TRUE to continue.
	Purpose:	By default does nothing.  This function can be overriden to so something
				useful (eg. set the status bar text).  Note that if you return FALSE from
				this function EndDrag() and End() will be called.
	SeeAlso:	OpDragBox::DragPointerIdle
********************************************************************************************/

BOOL OpDragBox::OnPointerIdle(Spread*, const DocRect&, ClickModifiers)
{
	return TRUE;
}



/********************************************************************************************
>	virtual BOOL OpDragBox::OnDragEnded(Spread* pBoxSpread,
										const DocRect& drDragBox,
										ClickModifiers cmods,
										BOOL fDragOK);
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95
	Inputs:		pBoxSpread			the spread where the drag was started
				drDragBox			the rectangle within the spread defining the drag box
				cmods				the state of the mouse click modifiers
				fDragOK				the drag has not failed or been cancelled, its a good 'un
	Returns:	FALSE to cancel the op, TRUE for success.  Note that FailAndExecute() will
				be called for this operation if you return FALSE from this function. If your
				operation requires different failure handling (or no failure handling at all)
				then you should perform the	handling yourself and return TRUE.
	Purpose:	You should override this function to do something useful (eg. set the status
				bar text, perform a selection operation).  It would be nice to make this
				a pure virtual function, as it doesn't make any sense not to override this
				function - a drag op should do something as a result of a mouse drag, but
				unfortunately the MFC/CC run-time dynamic creation won't allow it.
	SeeAlso:	OpDragBox::DragFinished
********************************************************************************************/

BOOL OpDragBox::OnDragEnded(Spread*, const DocRect&, ClickModifiers, BOOL)
{
	return TRUE;
}



/********************************************************************************************
>	OpCentredDragBox::OpCentredDragBox()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Purpose:	Default constructor for a centred drag-box operation.
	SeeAlso:	class OpDragBox
********************************************************************************************/

OpCentredDragBox::OpCentredDragBox()
{
	// Empty.
}



/********************************************************************************************
>	virtual DocRect OpCentredDragBox::CalcDragBox(const DocCoord& dcStartPos,
												  const DocCoord& dcMousePos) const
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		dcStartPos			the position where the drag was started
				dcMousePos			the current position of the mouse
	Returns:	A document (spread) rectangle the is the current bounds of the drag box.
	Purpose:	Overrides the default OpDragBox method to allow the drag box to be drawn
				from the centre (start of drag) to the corner (current mouse position).
	SeeAlso:	OpDragBox::CalcDragBox
********************************************************************************************/

DocRect OpCentredDragBox::CalcDragBox(const DocCoord& dcStartPos,
									  const DocCoord& dcMousePos) const
{
	// Find the half of the absolute width and height of the drag box.
	MILLIPOINT nWidth2 = dcStartPos.x - dcMousePos.x;
	if (nWidth2 < 0) nWidth2 = -nWidth2;
	MILLIPOINT nHeight2 = dcStartPos.y - dcMousePos.y;
	if (nHeight2 < 0) nHeight2 = -nHeight2;
	
	// Return a rectangle of this width and height centre on the click point.
	return DocRect(dcStartPos.x - nWidth2, dcStartPos.y - nHeight2,
				   dcStartPos.x + nWidth2, dcStartPos.y + nHeight2);
}
