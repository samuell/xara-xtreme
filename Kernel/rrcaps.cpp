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
// RRCaps are the render region caps. This lets us know what a render region can and can't
// render.

/*
*/

#include "camtypes.h"
#include "rrcaps.h"

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_MEMDUMP(RRCaps, CC_CLASS_MEMDUMP)

// Better Memory Dumps
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	RRCaps::RRCaps()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Purpose:	Constructor. Sets all the params to FALSE

********************************************************************************************/

RRCaps::RRCaps()
{
	// by default we can render nothing
	CanDoNothing();
}



/********************************************************************************************

>	RRCaps::RRCaps(const RRCaps& Other)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		Other - the one to copy
	Returns:	this one
	Purpose:	copies the data from one RRCaps object to another

********************************************************************************************/

RRCaps::RRCaps(const RRCaps& Other)
{
	// Copy all the params from the other one
	Transparency = Other.Transparency;
	GradFills = Other.GradFills;
	PerspectiveGradFills = Other.PerspectiveGradFills;
	BitmapFills = Other.BitmapFills;
	PerspectiveBitmapFills = Other.PerspectiveBitmapFills;
	LineAttrs = Other.LineAttrs;
	ArrowHeads = Other.ArrowHeads;
	DashPatterns = Other.DashPatterns;
	SimpleBitmaps = Other.SimpleBitmaps;
	ArbitraryBitmaps = Other.ArbitraryBitmaps;
	ClippedSimpleBitmaps = Other.ClippedSimpleBitmaps;
	ClippedArbitraryBitmaps = Other.ClippedArbitraryBitmaps;
	Grad3and4Fills = Other.Grad3and4Fills;
	ClippedOutput = Other.ClippedOutput;
}



/********************************************************************************************

>	RRCaps& RRCaps::operator=(RRCaps &Other)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		Other - the one to copy
	Returns:	this one
	Purpose:	copies the data from one RRCaps object to another

********************************************************************************************/

RRCaps& RRCaps::operator=(const RRCaps &Other)
{
	// Copy all the params from the other one
	Transparency = Other.Transparency;
	GradFills = Other.GradFills;
	PerspectiveGradFills = Other.PerspectiveGradFills;
	BitmapFills = Other.BitmapFills;
	PerspectiveBitmapFills = Other.PerspectiveBitmapFills;
	LineAttrs = Other.LineAttrs;
	ArrowHeads = Other.ArrowHeads;
	DashPatterns = Other.DashPatterns;
	SimpleBitmaps = Other.SimpleBitmaps;
	ArbitraryBitmaps = Other.ArbitraryBitmaps;
	ClippedSimpleBitmaps = Other.ClippedSimpleBitmaps;
	ClippedArbitraryBitmaps = Other.ClippedArbitraryBitmaps;
	Grad3and4Fills = Other.Grad3and4Fills;
	ClippedOutput = Other.ClippedOutput;

	// return it
	return *this;
}

/********************************************************************************************

>	RRCaps::~RRCaps()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Purpose:	default desctuctor

********************************************************************************************/

RRCaps::~RRCaps()
{
	// does nothing
}



/********************************************************************************************

>	void RRCaps::CanDoAll()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Purpose:	Sets all the flags in the RRCaps object to TRUE

********************************************************************************************/

void RRCaps::CanDoAll()
{
	// we can do everything
	Transparency = TRUE;
	GradFills = TRUE;
	PerspectiveGradFills = TRUE;
	BitmapFills = TRUE;
	PerspectiveBitmapFills = TRUE;
	LineAttrs = TRUE;
	ArrowHeads = TRUE;
	DashPatterns = TRUE;
	SimpleBitmaps = TRUE;
	ArbitraryBitmaps = TRUE;
	ClippedSimpleBitmaps = TRUE;
	ClippedArbitraryBitmaps = TRUE;
	Grad3and4Fills = TRUE;
	ClippedOutput = TRUE;
}


/********************************************************************************************

>	void RRCaps::CanDoNothing()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Purpose:	Sets all the flags in the RRCaps object to FALSE

********************************************************************************************/

void RRCaps::CanDoNothing()
{
	// we can do nothing :-(
	Transparency = FALSE;
	GradFills = FALSE;
	PerspectiveGradFills = FALSE;
	BitmapFills = FALSE;
	PerspectiveBitmapFills = FALSE;
	LineAttrs = FALSE;
	ArrowHeads = FALSE;
	DashPatterns = FALSE;
	SimpleBitmaps = FALSE;
	ArbitraryBitmaps = FALSE;
	ClippedSimpleBitmaps = FALSE;
	ClippedArbitraryBitmaps = FALSE;
	Grad3and4Fills = FALSE;
	ClippedOutput = FALSE;
}



/********************************************************************************************

>	BOOL RRCaps::DoesRegionDoAll()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Returns:	TRUE if the region that owns this RRCaps can do everything
	Purpose:	This function helps to identify render regions that need no special
				treatment early on. If this function returns TRUE it means that there is
				nothing the region that this caps object belongs to can't do.

********************************************************************************************/

BOOL RRCaps::DoesRegionDoAll()
{
	// See if everything is set to TRUE
	if ((Transparency==TRUE) && 
		(GradFills==TRUE) && 
		(PerspectiveGradFills==TRUE) && 
		(BitmapFills==TRUE) && 
		(PerspectiveBitmapFills==TRUE) && 
		(SimpleBitmaps==TRUE) &&
		(ArbitraryBitmaps==TRUE) &&
		(ClippedSimpleBitmaps==TRUE) &&
		(ClippedArbitraryBitmaps==TRUE) &&
		(LineAttrs==TRUE) &&
		(ArrowHeads==TRUE) &&
		(DashPatterns==TRUE) &&
		(Grad3and4Fills==TRUE) &&
		(ClippedOutput==TRUE))
		return TRUE;

	// something was not set to TRUE
	return FALSE;
}


/********************************************************************************************

>	BOOL RRCaps::DoesRegionDoNothing()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Returns:	TRUE if the region that owns this RRCaps can do nothing.
	Purpose:	This function helps to identify render regions that need special
				treatment early on. If this function returns TRUE it means that there is
				nothing the region can do.

********************************************************************************************/

BOOL RRCaps::DoesRegionDoNothing()
{
	// See if everything is set to TRUE
	if ((Transparency==FALSE) && 
		(GradFills==FALSE) && 
		(PerspectiveGradFills==FALSE) && 
		(BitmapFills==FALSE) && 
		(PerspectiveBitmapFills==FALSE) && 
		(SimpleBitmaps==FALSE) &&
		(ArbitraryBitmaps==FALSE) &&
		(ClippedSimpleBitmaps==FALSE) &&
		(ClippedArbitraryBitmaps==FALSE) &&
		(LineAttrs==FALSE) &&
		(ArrowHeads==FALSE) &&
		(DashPatterns==FALSE) &&
		(Grad3and4Fills==FALSE) &&
		(ClippedOutput==FALSE))
		return TRUE;

	// something was not set to TRUE
	return FALSE;
}
