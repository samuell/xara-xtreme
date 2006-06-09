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

// The ViewState object - contains a platform-independent description of a view onto
// a document.

#include "camtypes.h"
#include "vstate.h"
//#include "oilcoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// Construction and destruction.

ViewState::ViewState()
{
	// Set all the params to defaults
	pView = NULL;

	// Make all rects empty
	ScreenRect.MakeEmpty();
	ParentRect.MakeEmpty();
	ViewRect.MakeEmpty();
	WorkAreaExtent.MakeEmpty();
	IconPoint = Coord(0,0);

	ViewScale = 0;
	zPos = 0;
	
	IsNewView = TRUE;
	IsMinimised = FALSE;
	IsMaximised = FALSE;
	AlwaysOnTop = FALSE;
	RulersVisible = FALSE;
	ScrollersVisible = TRUE;
	ScrollPosition = WorkCoord(0,0);

	_GridShow = _GridSnap = _ObjectsSnap = _MagObjectsSnap = _PrintBorderShow = FALSE;
	_LogicalView = _GuidesSnap = _GuidesShow = FALSE;
}



ViewState::~ViewState()
{
	// empty
}




/********************************************************************************************
>	WorkCoord ViewState::SetScrollPos(WorkCoord sp, BOOL fPixelise = TRUE) 

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/11/94
	Inputs:		New scroll position and whether to pixelise it.
	Outputs:	Pixelised scroll position.
	Returns:	None
	Purpose:	Set the scroll position in WorkCoords for use by DocView and CamView.
	Errors:		None

********************************************************************************************/

WorkCoord ViewState::SetScrollPos(WorkCoord sp, BOOL fPixelise /* = TRUE */)
{
	ERROR3IF(!fPixelise, "ViewState::SetScrollPos called with Pixelise = FALSE");

//	TRACE( _T("VS::SetScrollPos(%d, %d)\n"), (INT32)(sp.x), (INT32)(sp.y));

	FIXED16 PixelWidth, PixelHeight;	
	ERROR2IF(pView == NULL, ScrollPosition, "No attached view in ViewState::SetScrollPos");
	pView->GetPixelSize(&PixelWidth, &PixelHeight);

	INT32 pw = PixelWidth.MakeLong();
	INT32 ph = PixelHeight.MakeLong();

	if (sp.x < 0) sp.x -= (pw - 1);			// Overcome the flooring of negative values
	if (sp.y < 0) sp.y -= (ph - 1);			// towards 0 by the upcoming divisions...

	ScrollPosition.x = (sp.x / pw) * pw;
	ScrollPosition.y = (sp.y / ph) * ph;

	return ScrollPosition;
}




/********************************************************************************************
>	WorkCoord ViewState::GetScrollPos(BOOL Pixelise = TRUE) 

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/11/94
	Inputs:		None
	Outputs:	None
	Returns:	Scroll position
	Purpose:	Set the scroll position in WorkCoords for use by DocView and CamView.
	Errors:		None

********************************************************************************************/
/*
	Technical notes:
	This routine provides restricted access to the ScrollPosition member of ViewState.
	It ensures that the scroll position is aligned to a whole pixel boundary at all times by
	dividing and re-multiplying by the pixel size (YUK).

	Note that when a negative value is divided in C++ it is done so without regard for the
	sign of the value.
	E.g.
		21 / 4 = 5	Result has been truncated towards 0, moving Y coordinates down
		-21/ 4 = 5  Result has been truncated towards 0 again, moving Y coordinates UP!!!
	In other words when the values are coordinates the division truncates them in different
	directions depending which side of the origin they are on!
	This is obviously not required and so any negative values are adjusted before the division
	to ensure the end result is movement in the same direction as positive values.

********************************************************************************************/

WorkCoord ViewState::GetScrollPos(BOOL Pixelise)
{
	ERROR3IF(!Pixelise, "ViewState::GetScrollPos called with Pixelise = FALSE");
	return(ScrollPosition);

#if FALSE
	WorkCoord temp = ScrollPosition;

	// If we have a NULL view, return quietly, cos this is probably in 
	// some rendering code.
	if (Pixelise)
	{
		FIXED16 PixelWidth, PixelHeight;	
		ERROR2IF(pView == NULL, temp, "No attached view in ViewState::GetScrollPos");
		pView->GetPixelSize(&PixelWidth, &PixelHeight);
	
		INT32 pw = PixelWidth.MakeLong();
		INT32 ph = PixelHeight.MakeLong();

		if (temp.x < 0) temp.x -= (pw - 1);			// Overcome the flooring of negative values
		if (temp.y < 0) temp.y -= (ph - 1);			// towards 0 by the upcoming divisions...

		temp.x = (temp.x / pw) * pw;
		temp.y = (temp.y / ph) * ph;
	}
	return temp;
#endif
}


/*

void ViewState::Serialize(CArchive& ar)
{
	// Serialize the base class first.
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		// Save the size of the screen.
		ar << ScreenRect.lo.x << ScreenRect.lo.y
		   << ScreenRect.hi.x << ScreenRect.hi.y;

		// Save the size of the parent (MDI) frame window.
		ar << ParentRect.lo.x << ParentRect.lo.y
		   << ParentRect.hi.x << ParentRect.hi.y;

		// Save the size of this view window.
		ar << ViewRect.lo.x << ViewRect.lo.y
		   << ViewRect.hi.x << ViewRect.hi.y;

		// Save the position of this view when an icon.
		ar << IconPoint.x << IconPoint.y;

		// Save the scroller position.
		INT32 dwHigh;
		UINT32 dwLow;
		ScrollPosition.x.GetHighLow(&dwHigh, &dwLow);
		ar << dwHigh << dwLow;
		ScrollPosition.y.GetHighLow(&dwHigh, &dwLow);
		ar << dwHigh << dwLow;

		// Save the zoom scaling factor.  For now, this must be done by casting it
		// into a double, until Andy lets me change fixed16.h
        ar << ViewScale.MakeDouble();

		// Save the "state" of this view, ie. Is it an icon?  Is it top-most? etc.
		ar << INT32(IsMinimised) << INT32(IsMaximised) << INT32(HasFocus)
		   << INT32(AlwaysOnTop);
 	}
	else
	{
		// Load the previous size of the screen.
		ar >> ScreenRect.lo.x >> ScreenRect.lo.y
		   >> ScreenRect.hi.x >> ScreenRect.hi.y;

		// Load the size of the parent (MDI) frame window.
		ar >> ParentRect.lo.x >> ParentRect.lo.y
		   >> ParentRect.hi.x >> ParentRect.hi.y;

		// Load the size of this view window.
		ar >> ViewRect.lo.x >> ViewRect.lo.y
		   >> ViewRect.hi.x >> ViewRect.hi.y;

		// Load the position of this view when an icon.
		ar >> IconPoint.x >> IconPoint.y;

		// Load the scroller position.
		INT32 dwHigh;
		UINT32 dwLow;
		ar >> dwHigh >> dwLow;
		ScrollPosition.x.SetHighLow(dwHigh, dwLow);
		ar >> dwHigh >> dwLow;
		ScrollPosition.y.SetHighLow(dwHigh, dwLow);

		// Load the scale factor, which was serialized as a double.
		double fp;
		ar >> fp;
		ViewScale = (FIXED16) fp;

		// Load the "state" of this view, ie. Is it an icon?  Is it top-most? etc
		INT32 a, b, c, d;
		ar >> a >> b >> c >> d;
		IsMinimised = (BOOL) a;
		IsMaximised = (BOOL) b;
		HasFocus    = (BOOL) c;
		AlwaysOnTop = (BOOL) d;
		
		// Finally, indicate that this is not a new view that has a default size etc
		// but an old one, so it should be set to match the data in the ViewState.
		IsNewView = FALSE;
	}
}

*/


#ifdef _DEBUG

void ViewState::Dump() const
{
	TCHAR			   *pszTrue = wxT("TRUE"), *pszFalse = wxT("FALSE");

	TRACE( wxT("\nDUMP OF VIEWSTATE AT 0x%lX\n"), this );

	// Window attributes.
	TRACE( _T("\tScreenRect:\t\tlow = (%ld, %ld)\t\thigh = (%ld, %ld)\n"),
		  ScreenRect.lo.x, ScreenRect.lo.y, ScreenRect.hi.x, ScreenRect.hi.y);
	TRACE( _T("\tParentRect:\t\tlow = (%ld, %ld)\t\thigh = (%ld, %ld)\n"),
		  ParentRect.lo.x, ParentRect.lo.y, ParentRect.hi.x, ParentRect.hi.y);
	TRACE( _T("\tViewRect:\t\tlow = (%ld, %ld)\t\thigh = (%ld, %ld)\n"),
		  ViewRect.lo.x, ViewRect.lo.y, ViewRect.hi.x, ViewRect.hi.y);
	TRACE( _T("\tIconPoint:\t\t(%ld, %ld)\n"), IconPoint.x, IconPoint.y);
	TRACE( _T("\tIsNewView:\t\t%s\n"), (IsNewView ? pszTrue : pszFalse));
	TRACE( _T("\tIsMinimised:\t%s\n"), (IsMinimised ? pszTrue : pszFalse));
	TRACE( _T("\tIsMaximised:\t%s\n"), (IsMaximised ? pszTrue : pszFalse));
	TRACE( _T("\tAlwaysOnTop:\t%s\n"), (AlwaysOnTop ? pszTrue : pszFalse));

	// Document view attributes.
	TRACE( _T("\tWork-Area Ext:\t(%ld, %ld)  -->  (%ld, %ld)\n"),
		  INT32(WorkAreaExtent.lo.x), INT32(WorkAreaExtent.lo.y),
		  INT32(WorkAreaExtent.hi.x), INT32(WorkAreaExtent.hi.y) );
	TRACE( _T("\tScroll Offset:\t(%ld, %ld)\n"),
		  INT32(ScrollPosition.x), INT32(ScrollPosition.y) );
//	TRACE( _T("\tPixel Width:\t%ld\n\tPixelHeight:\t%ld\n"), PixelWidth, PixelHeight);
}

#endif
