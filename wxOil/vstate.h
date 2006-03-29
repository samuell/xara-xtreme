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
	A ViewState object contains platform-independent data describing a view onto a
	document.

	vstate.h
*/

#ifndef VSTATE_H
#define VSTATE_H

#include "coord.h"
#include "rect.h"
#include "wrkcoord.h"
#include "wrkrect.h"

class View;

class ViewState
{	
public:

	// Construction & destruction
	ViewState();
	virtual ~ViewState();

#ifdef _DEBUG
	void Dump() const;
#endif

public:

	// Data members (public for easy access)

	View* 	    pView;					// The kernel view object this describes

	Rect		ScreenRect;             // screen dimensions of the platform
	Rect		ParentRect;				// position of parent (MDI frame) window	
	Rect		ViewRect;				// position of this CCamView
	
	Coord 		IconPoint;				// position when iconised

	WorkRect  	WorkAreaExtent;			// the scrollable area

// Removed by Phil, 22/11/94, to try to rationalise the storage of pixel sizes in the system.
//	MILLIPOINT	PixelWidth;          	// pixel width in millipoints
//	MILLIPOINT	PixelHeight;         	// pixel height in millipoints
//	FIXED16 	PixelXScale;			// no. of pixels per millipoint horizontally
//	FIXED16 	PixelYScale;			// and vertically.
	FIXED16		ViewScale;				// this view's current scaling factor for zooms
	
	BOOL 		IsNewView;				// is this a fresh view?
	BOOL 		IsMinimised;			// is view currently iconised?
	BOOL 		IsMaximised;			// is view currently maximised?
	BOOL		AlwaysOnTop;			// TRUE if window has "always on top" attribute
	BOOL		RulersVisible;			// TRUE if Rulers are visible
	BOOL 		ScrollersVisible;		// TRUE if Scrollers are visible
	INT32			zPos;					// relative z-order of the view

	// These are temporary storage used by LoadAppWindowState to store the values of the
	// corresponding DocView flags until they can be written to a created DocView.
	// See MakeViewStateFromKey and CCamView::OnInitialUpdate.
	BOOL _GridShow : 1;
	BOOL _GridSnap : 1;
	BOOL _ObjectsSnap : 1;
	BOOL _MagObjectsSnap : 1;
	BOOL _PrintBorderShow : 1;
	BOOL _LogicalView : 1;
	BOOL _GuidesSnap : 1;
	BOOL _GuidesShow : 1;


	// Functions to access the protected scroll offset and ensure it's pixel aligned...
	WorkCoord	SetScrollPos(WorkCoord sp, BOOL fPixelise = TRUE);
	WorkCoord	GetScrollPos(BOOL Pixelise = TRUE);

protected:
	WorkCoord 	ScrollPosition;			// offset of scroll bars
	
};


#endif	// VSTATE_H
