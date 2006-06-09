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

//#include "oilcoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "node.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "wrkcoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rect.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "paper.h"
//#include "xmatrix.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "chapter.h"
//#include "view.h" - in camtypes.h [AUTOMATICALLY REMOVED]

DECLARE_SOURCE("$Revision$");




/*********************************************************************************************
>	OilCoord::OilCoord(MILLIPOINT xx, MILLIPOINT yy)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27th August 1993
	Inputs:		Two INT32s (measured in millipoints).
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OIL coordinate.
	Errors:		-
	Scope:		Public

**********************************************************************************************/ 

OilCoord::OilCoord(MILLIPOINT xx, MILLIPOINT yy) : Coord(xx, yy)
{
}









/********************************************************************************************

>	WinCoord OilCoord::ToWin(View *pView) const;

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/93
	Inputs:		The view we are converting to.
	Purpose:	Converts an OilCoord to a WinCoord.

********************************************************************************************/

WinCoord OilCoord::ToWin(View *pView) const
{
	// Note that we have to negate the y coord, because Windows starts with 0 at the top
	// and then positive coordinates in a downward direction, i.e. the opposite to
	// Camelot's coordinate systems.

	// NB. More importantly, we add 1 to the y coord, because the flipping of the y axis
	//     causes a misalignment in the pixel systems.  This is because Camelot coords
	//	   specify the bottom left of the pixel, whereas GDI coords specify the top-left.
	//	   (See coord.doc for more details)

//	return WinCoord(INT32(MPtoPixel(x, xPixelScale)), -INT32(MPtoPixel(y, yPixelScale) + 1));

	// New info: (Phil, 17/11/94)
	// The one pixel bodge is no longer required because the pixel model has been modified
	// so that pixel coordinates are in the centres of pixels, not on any edge.
	// This allows coordinate systems to be negated without any extra work.

	// Get pixel size for this view
	FIXED16 PixelWidth, PixelHeight;
	pView->GetPixelSize(&PixelWidth, &PixelHeight);

	// Do the conversion and return the results.
	return WinCoord(INT32(MPtoPixel(x, PixelWidth)), -INT32(MPtoPixel(y, PixelHeight) ));
}


/********************************************************************************************

>	WinCoord OilCoord::ToWin( const INT32 dpi ) const;

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/93
	Inputs:		Transform Matrix.
				dpi	the value of the destination dpi rather than assuming screen dpi 
	Outputs:	None
	Returns:	None.
	Purpose:	Same as above in that it converts an OilCoord to a WinCoord but instead of
				assuming screen dpi, as in using x/yPixelScale, it uses the specified dpi
				to work out the transformation.
	Errors:		None.

********************************************************************************************/

//WinCoord OilCoord::ToWin( const INT32 dpi ) const
//{
	// See above ToWin routine for conversion comments
	// Work out our x and y pixel scaling factors for the specified destination dpi
//	FIXED16 PixScale = 72000.0/dpi;

//	return WinCoord(INT32(MPtoPixel(x, PixScale)), -INT32(MPtoPixel(y, PixScale) ));
//}

/********************************************************************************************

>	WinCoord OilCoord::ToWin( const double dpi ) const;

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/93
	Inputs:		Transform Matrix.
				dpi	the value of the destination dpi rather than assuming screen dpi 
	Outputs:	None
	Returns:	None.
	Purpose:	Same as above in that it converts an OilCoord to a WinCoord but instead of
				assuming screen dpi, as in using x/yPixelScale, it uses the specified dpi
				to work out the transformation.
				Different to above in that it takes a double dpi instead of a INT32 dpi. Now
				superceeds the above. Changed 12/12/95.
	Errors:		None.

********************************************************************************************/

WinCoord OilCoord::ToWin( const double dpi ) const
{
	// See above ToWin routine for conversion comments
	// Work out our x and y pixel scaling factors for the specified destination dpi
	FIXED16 PixScale = 72000.0/dpi;

	return WinCoord(INT32(MPtoPixel(x, PixScale)), -INT32(MPtoPixel(y, PixScale) ));
}




/********************************************************************************************

>	WorkCoord OilCoord::ToWork(const WorkCoord& ScrollOffset) const

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/93
	Inputs:		Scroll offset.
	Outputs:	None
	Returns:	None.
	Purpose:	Converts an OilCoord to a WorkCoord.
	Errors:		None.

********************************************************************************************/

WorkCoord OilCoord::ToWork(const WorkCoord& ScrollOffset) const
{
	// Add the scroll offsets to get workarea values...
	return WorkCoord(ScrollOffset.x + x, ScrollOffset.y + y);
}




/********************************************************************************************

>	void OilCoord::ToWork(const WorkCoord& scrollOffset, WorkCoord* result ) const

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/93
	Inputs:		Scroll offset.
	Outputs:	None
	Returns:	None.
	Purpose:	Converts an OilCoord to a WorkCoord.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

void OilCoord::ToWork(const WorkCoord& scrollOffset, WorkCoord* result ) const
{
// Implementation...
	// Then add the scroll offsets to get workarea values...
	result->x = scrollOffset.x + x; 
	result->y = scrollOffset.y + y;
}




/********************************************************************************************

>	DocCoord OilCoord::ToDoc(const Spread* pSpread, View *pView) const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/93
	Inputs:		pSpread - The spread that we know that the oil coord is in
				View - the view to use when converting.
	Returns:	This coordinate as measured in DocCoords
	Purpose:	Convert an OilCoord to a DocCoord

********************************************************************************************/

DocCoord OilCoord::ToDoc(const Spread* pSpread, View* pView) const
{
	DocCoord	Return;
	WorkCoord	Point((XLONG)(this->x), (XLONG)(this->y));

	// First, convert the OilCoord into a LogicalCoord...
	FIXED16 ViewScale = pView->GetViewScale();
	WorkCoord ScrollOffset = pView->GetScrollOffsets();

	Point.x = MakeXLong( double(Point.x + ScrollOffset.x) / ViewScale.MakeDouble());
	Point.y = MakeXLong( double(Point.y + ScrollOffset.y) / ViewScale.MakeDouble());

	// Find the parent chapter
	Chapter* pChapter = (Chapter*) pSpread->FindParent();
	ENSURE( pChapter != NULL, "Spread had no parent" );
	ENSURE( pChapter->IsKindOf(CC_RUNTIME_CLASS(Chapter)), "Chapter is not a chapter" );

	// Find the depth of the chapter
	XLONG ChapDepth = pChapter->GetChapterDepth();

	// Find the chapters position
	DocCoord ChapterPos;
	ChapterPos.x = pChapter->GetPasteboardRect(TRUE, pView).lo.x; 
	ChapterPos.y = pChapter->GetPasteboardRect(TRUE, pView).hi.y;

	// Build the return value up
	Return.x = Point.x + ChapterPos.x;
	Return.y = Point.y + MakeXLong(ChapterPos.y) + ChapDepth;

	return (Return);
	
}
