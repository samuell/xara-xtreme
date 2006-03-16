// $Id: metaview.cpp 662 2006-03-14 21:31:49Z alex $
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

// Simple view class for metafiles to use when rendering using RenderRegions.

/*
*/

#include "camtypes.h"
#include "metaview.h"
#include "vstate.h"
#include "errors.h"


CC_IMPLEMENT_DYNAMIC(MetafileView, View)

#define new CAM_DEBUG_NEW


/********************************************************************************************

>	BOOL MetafileView::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Returns:	TRUE if initialised ok;
				FALSE if not
	Purpose:	Initialise the MetafileView for rendering.
	SeeAlso:	MetafileView

********************************************************************************************/

BOOL MetafileView::Init()
{
	// Metafiles are always exported in the same format.
	// This is HIENGLISH which has 1000 pixels per inch.
	// As a result, we know that there are 72 millipoints per pixel.
	PixelWidth  = FIXED16(72);
	PixelHeight = FIXED16(72);
	
	// The scaled version are the same
	ScaledPixelWidth  = PixelWidth;
	ScaledPixelHeight = PixelHeight;

	// Get ourselves a ViewState object
	pVState = new ViewState;
	if (pVState == NULL) 
		return FALSE;

	// Connect this view state to this view
	pVState->pView = this;

	// No document for metafiles
	pDoc = NULL;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL MetafileView::ViewStateChanged()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Returns:	TRUE (always).
	Purpose:	None - MetafileView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	MetafileView

********************************************************************************************/

BOOL MetafileView::ViewStateChanged()
{
	return TRUE;
}

/********************************************************************************************

>	void MetafileView::SetViewPixelSize()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Purpose:	Sets up the pixel size for this view object.
	SeeAlso:	MetafileView

********************************************************************************************/

void MetafileView::SetViewPixelSize()
{
	// Set the scaled pixel size
	ScaledPixelWidth = PixelWidth;
	ScaledPixelHeight = PixelHeight;
}

/********************************************************************************************

>	void MetafileView::ContinueRenderView(RenderRegion*, Spread*, BOOL = TRUE, BOOL = TRUE,
									 BOOL bForceImmediate = FALSE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Inputs:		N/A
	Purpose:	None - MetafileView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	MetafileView
	Errors:		Always => ERROR3

********************************************************************************************/

void MetafileView::ContinueRenderView(RenderRegion*, Spread*, BOOL, BOOL,
									 BOOL bForceImmediate)
{
	ERROR3("MetafileView::ContinueRenderView() should not be called!");
}

/********************************************************************************************

>	CDC *MetafileView::GetRenderDC()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Returns:	NULL (always)
	Purpose:	None - MetafileView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	MetafileView

********************************************************************************************/

CDC *MetafileView::GetRenderDC()
{
	return NULL;
}

/********************************************************************************************

>	BOOL MetafileView::GetForeBackMode()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Returns:	FALSE - Metafiles don't do background rendering.
	Purpose:	None - MetafileView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	MetafileView

********************************************************************************************/

BOOL MetafileView::GetForeBackMode()
{
	// MetafileViews don't background render.
	return FALSE;
}

/********************************************************************************************

>	void MetafileView::SetForeBackMode(BOOL)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Inputs:		N/A
	Purpose:	None - MetafileView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	MetafileView
	Errors:		ERROR3 always - dialogs cannot background render!

********************************************************************************************/

void MetafileView::SetForeBackMode(BOOL)
{
	ERROR3("MetafileView::SetForeBackMode() should not be called!");
}

/********************************************************************************************

>	DocRect MetafileView::GetDocViewRect(Spread*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Inputs:		N/A
	Returns:	An empty rectangle (always).
	Purpose:	None - MetafileView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	MetafileView

********************************************************************************************/

DocRect MetafileView::GetDocViewRect(Spread*)
{
	DocRect Empty;
	return Empty;
}

/********************************************************************************************

>	void MetafileView::SetExtent(DocCoord, DocCoord)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Inputs:		N/A
	Purpose:	None - MetafileView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	MetafileView

********************************************************************************************/

void MetafileView::SetExtent(DocCoord, DocCoord)
{
}

/********************************************************************************************

>	WorkRect MetafileView::GetViewRect()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Returns:	An empty rectangle (always).
	Purpose:	None - MetafileView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	MetafileView

********************************************************************************************/

WorkRect MetafileView::GetViewRect()
{
	WorkRect Empty;
	return Empty;
}




/********************************************************************************************

>	INT32 MetafileView::GetMetafileFlatness()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Returns:	The flatness to use for metafiles
	Purpose:	Allows the format that metafiles are saved in to change, without changing
				everything else in the world.

********************************************************************************************/

INT32 MetafileView::GetMetafileFlatness()
{
	// Say how much we are prepared to flatten paths
	return 1024;
}




/********************************************************************************************

>	INT32 MetafileView::GetMetafileDPI()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Returns:	the number of pixels per inch for the metafile we are creating
	Purpose:	Allows the format that metafiles are saved in to change, without changing
				everything else in the world.

********************************************************************************************/

INT32 MetafileView::GetMetafileDPI()
{
	// We are currently using HIENGLISH mapping mode, which is at 1000 dpi
	return 1000;
}




/********************************************************************************************

>	INT32 MetafileView::GetMetafileMapMode()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Returns:	The mapping mode to use for metafiles. This is currently MM_HIENGLISH
	Purpose:	Allows the format that metafiles are saved in to change, without changing
				everything else in the world.

********************************************************************************************/

INT32 MetafileView::GetMetafileMapMode()
{
	// return the mapping mode we want to use
	return MM_ANISOTROPIC;
}



/********************************************************************************************

>	void MetafileView::GetScaleFactor(FIXED16* pXScale, FIXED16* pYScale)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Outputs:	pXScale, pYScale - the x and y scale factor to render the drawing to,
				to get the best range of values from the coord system.
	Purpose:	Allows the format that metafiles are saved in to change, without changing
				everything else in the world.

********************************************************************************************/

void MetafileView::GetScaleFactor(FIXED16* pXScale, FIXED16* pYScale)
{
	// boost the scale to get the most out of the coord range.
	// We get this figure as we need to get 1000 dpi, but a scale of 1 will give 96 dpi
	// ie 1000/96 = 10.42
	*pXScale = FIXED16(1);
	*pYScale = FIXED16(1);
}


