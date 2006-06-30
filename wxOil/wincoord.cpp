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

//#include "wincoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "oilcoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "view.h" - in camtypes.h [AUTOMATICALLY REMOVED]

DECLARE_SOURCE("$Revision$");



/********************************************************************************************
>	WinCoord::WinCoord(INT32 x, INT32 y)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/93
	Inputs:		Two integers (either signed 16 or 32 bits, depending on the Windows platform).
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a WinCoord.  Calls CPoint::CPoint(x, y)
	Errors:		None.
	See Also:	class CPoint

********************************************************************************************/

WinCoord::WinCoord(INT32 x, INT32 y) : wxPoint(x, y)
{
}



/********************************************************************************************
>	WinCoord::WinCoord(INT32 x, INT32 y)

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/93
	Inputs:		Two integers (either signed 16 or 32 bits, depending on the Windows platform).
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a WinCoord.  Calls CPoint::CPoint(x, y)
	Errors:		None.
	See Also:	class CPoint

********************************************************************************************/

WinCoord::WinCoord(wxPoint pt) : wxPoint(pt)
{
}



/********************************************************************************************
>	OilCoord WinCoord::ToOil(View *pView, BOOL PixelCentre = FALSE) const

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/93
	Inputs:		-
	Outputs:	-
	Returns:	The position, as an OIL coordinate, of the bottom-left corner of the pixel
				located at (this) in the Windows coordinate system.
	Purpose:	Converts a WinCoord to a OilCoord.
	Errors:		None.
	See Also:	OilCoord::ToWin()

********************************************************************************************/

OilCoord WinCoord::ToOil(View *pView, BOOL PixelCentre) const
{
	// Note that we have to negate the y coord, because Windows starts with 0 at the top
	// and then positive coordinates in a downward direction, i.e. the opposite to
	// Camelot's coordinate systems.
	// NB. More importantly, we add 1 to the y coord, because the flipping of the y axis
	//     causes a misalignment in the pixel systems.  This is because Camelot coords
	//	   specify the bottom left of the pixel, whereas GDI coords specify the top-left.
	//	   (See coord.doc for more details)
//	return OilCoord(LongMulFixed16(x, OilCoord::PixelWidth()),
//				   -LongMulFixed16(y + 1, OilCoord::PixelHeight()));

	// New info: (Phil, 17/11/94)
	// The one pixel bodge is no longer required because the pixel model has been modified
	// so that pixel coordinates are in the centres of pixels, not on any edge.
	// This allows coordinate systems to be negated without any extra work.
//	return OilCoord(LongMulFixed16(x, OilCoord::PixelWidth()),
//				   -LongMulFixed16(y, OilCoord::PixelHeight()));
	
	FIXED16 PixelWidth, PixelHeight;
	pView->GetPixelSize(&PixelWidth, &PixelHeight);
	OilCoord temp = OilCoord(LongMulFixed16(x, PixelWidth),
				   			-LongMulFixed16(y, PixelHeight)
				   			);
	if (PixelCentre)
	{
		// Coordinate is a click coord which is different than normal rectangle coords
		// Rectangle coords need to specify the joints between pixels
		// Click coords need to specify pixel centres
		// So shift this coord to the centre of the pixel above and to the right of the
		// joint specified by the raw OilCoord.
		// The amount added is just less than half a pixel so that GDraw's anti-aliasing
		// will draw thin lines predictably.
		temp.x += (PixelWidth.MakeLong()*15)/32;
		temp.y -= (PixelHeight.MakeLong()*15)/32;
	}

	return temp;
}
