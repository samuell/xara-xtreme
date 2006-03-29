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

	opdragbx.h

	Base-class for easy drag box operations.
*/

#ifndef OPDRAGBX_H
#define OPDRAGBX_H

#include "ops.h"

enum  DragType;
enum  StockColour;
class Spread;
class DocCoord;
class DocRect;
class RenderRegion;
class ClickModifiers;


/********************************************************************************************

>	class OpDragBox : public Operation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95
	Purpose:	This is a base class for operations that use a drag box to select an area
				within a view, such as the selector and the zoom tools.  This class looks
				after the chore of correctly drawing and updating the xored drag box.

				To use this class derive a new operation from it and override the
				OnDragEnded function to do something useful with the drag box it is passed.
				You can	optionally also override the OnDragStarted, OnPointerMoved, or
				OnPointerIdle functions, eg. to set status text or perform intermediate
				calculations, and the SetBoxColours and CalcDragRect functions to customise
				the "look and feel" of the drag box.

				To run a drag, create an instance of your derived class and call its
				DoDrag() function, passing the mouse position etc that should be available
				in a tool.
	SeeAlso:	class OpCentredDragBox

********************************************************************************************/

class OpDragBox : public Operation
{
public:
	// This function is called on startup, and reads preferences etc.
	static BOOL Init();

	// This function initiates a drag, returning TRUE if it is successful.
	BOOL DoDrag(DragType dt,
				Spread* pStartSpread,
				const DocCoord& dcStartPos,
				ClickModifiers cmods);

protected:
	// Creation & destruction.  The constructor is only available to derived classes (it
	// makes no sense to create objects of this type, only derivatives).
	OpDragBox();

	// You can override these On... functions to do something useful (by default they do
	// nothing).  They can all return FALSE to cancel the drag.  This function is called when
	// the drag is started.
	virtual BOOL OnDragStarted(Spread* pStartSpread,
							   const DocCoord& dcStartPos,
							   ClickModifiers cmods);

	// This function is called when the pointer moves.
	virtual BOOL OnPointerMoved(Spread* pBoxSpread,
							 	const DocRect& drDragBox,
							 	ClickModifiers cmods);

	// This function is called when the pointer hasn't moved for a little while.
	virtual BOOL OnPointerIdle(Spread* pBoxSpread,
							   const DocRect& drDragBox,
							   ClickModifiers cmods);

	// This function is called when the drag finishes, and is passed the final drag box the
	// user has drawn and a drag success code.
	virtual BOOL OnDragEnded(Spread* pBoxSpread,
						   	 const DocRect& drDragBox,
						   	 ClickModifiers cmods,
						   	 BOOL fDragOK);

	// This function can be overridden to calculate the drag box in a different way, eg. for
	// a drag box that grows from a centre point to a corner, instead of the default behaviour
	// which calculates a drag box from one corner to its opposite.
	virtual DocRect CalcDragBox(const DocCoord& dcStartPos, const DocCoord& dcMousePos) const;

	// This function can be overriden to set the line colour for the drag box.  By default it
	// returns COLOUR_XORSELECT, as per user-interface guidelines for selection xor-ing.
	virtual StockColour GetBoxColour() const;

	// If you override the above CalcDragBox function so that the start position is no longer a
	// corner of the drag box, when you get the final drag box in OnDragEnded it may be
	// difficult to work out from it where the drag started, so call this which always knows.
	BOOL GetDragStartPos(Spread** ppStartSpread, DocCoord* pdcStartPos) const;

	// Miscellaneous helpers.  These may well be useful in a derived class.
	static MILLIPOINT Min(MILLIPOINT a, MILLIPOINT b);
	static MILLIPOINT Max(MILLIPOINT a, MILLIPOINT b);

private:
	// Implementation of a drag operation as defined by Operation.
	void DragPointerMove(DocCoord dcMousePos, ClickModifiers cmods, Spread* pSpread, BOOL bSolidDrag);
	void DragPointerIdle(DocCoord dcMousePos, ClickModifiers cmods, Spread* pSpread, BOOL bSolidDrag);
	void DragFinished(DocCoord dcMousePos, ClickModifiers cmods, Spread* pSpread, BOOL fOK, BOOL bSolidDrag);
	void RenderDragBlobs(DocRect drClipRect, Spread* pRenderSpread, BOOL bSolidDrag);

	// These functions erase the old drag box and draw the new one.
	void UpdateSolidDragBox(const DocRect& drNewDragBox);
	void UpdateUnfilledDragBox(const DocRect& drNewDragBox);
	
	// This function draws a render-on-top xored rectangle at the given spread coordinates.
	void DrawXorRect(const DocRect& drClip,	Spread* pspdXor, const DocRect& drXor) const;

	// Data members.
	Spread*	 m_pStartSpread;				// the spread where the drag started
	DocCoord m_dcStartPos;					// the point where the drag started
	DocRect	 m_drLastDragBox;				// the last extent of the drag box

	// Preferences.
	static BOOL m_fDoSolidDragBoxes;		// whether to do solid xored drag boxes or just
											// unfilled rectangles.
	
	CC_DECLARE_DYNCREATE(OpDragBox);
};



/********************************************************************************************

>	class OpCentredDragBox : public OpDragBox

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Purpose:	This is a base class for drag operations that would like a drag box to be
				centred on the initial click point, such as (optionally) the zoom tool.
				Apart from providing a new default CalcDragBox function, it is used the
				same way as OpDragBox.
	SeeAlso:	class OpDragBox

********************************************************************************************/

class OpCentredDragBox : public OpDragBox
{
protected:
	// Creation & destruction.
	OpCentredDragBox();

	// Overridables.
	virtual DocRect CalcDragBox(const DocCoord& dcStartPos, const DocCoord& dcMousePos) const;

private:	
	CC_DECLARE_DYNCREATE(OpCentredDragBox);
};

#endif	// !OPDRAGBX_H
