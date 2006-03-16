// $Id: dlgview.cpp 662 2006-03-14 21:31:49Z alex $
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

// Simple view class for dialogs to use when rendering using RenderRegions.

/*
*/

#include "camtypes.h"
#include "dlgview.h"
#include "vstate.h"
#include "errors.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC(DialogView, View)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	BOOL DialogView::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/20/95
	Returns:	TRUE if initialised ok;
				FALSE if not
	Purpose:	Initialise the DialogView for rendering.
	SeeAlso:	DialogView

********************************************************************************************/

BOOL DialogView::Init()
{
	// Get pixel size from screen DC.
	INT32					pixwidth, pixheight;
	wxScreenDC().GetSize( &pixwidth, &pixheight ); // NB wxScreenDC() creates temp. wxScreenDC object
	
	// Set up our pixel size
	PixelWidth = FIXED16(72000.0 / pixwidth);
	PixelHeight = FIXED16(72000.0 / pixheight);
	ScaledPixelWidth = PixelWidth;
	ScaledPixelHeight = PixelHeight;

	// Get ourselves a ViewState object
	pVState = new ViewState;
	if (pVState == NULL) 
		return FALSE;

	// Connect this view state to this view
	pVState->pView = this;

	// No document for dialogs
	pDoc = NULL;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	DialogView::~DialogView()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/95
	Purpose:	Destructor - the class leaks viewstates like a privatised water company - the
				destructor has to delete them.

********************************************************************************************/

DialogView::~DialogView()
{
	if (pVState != NULL)
	{
		delete pVState;
		pVState = NULL;
	}
}

/********************************************************************************************

>	BOOL DialogView::ViewStateChanged()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/20/95
	Returns:	TRUE (always).
	Purpose:	None - DialogView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	DialogView

********************************************************************************************/

BOOL DialogView::ViewStateChanged()
{
	return TRUE;
}

/********************************************************************************************

>	void DialogView::SetViewPixelSize()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/20/95
	Purpose:	Sets up the pixel size for this view object.
	SeeAlso:	DialogView

********************************************************************************************/

void DialogView::SetViewPixelSize()
{
	// Set the scaled pixel size
	ScaledPixelWidth = PixelWidth / Scale;
	ScaledPixelHeight = PixelHeight / Scale;
}

/********************************************************************************************

>	void DialogView::ContinueRenderView(RenderRegion*, Spread*, BOOL = TRUE, BOOL = TRUE,
									 BOOL bForceImmediate = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/20/95
	Inputs:		N/A
	Purpose:	None - DialogView is a skeleton class that is only used to access screen
				characteristics.
	Errors:		Always => ERROR3
	SeeAlso:	DialogView

********************************************************************************************/

void DialogView::ContinueRenderView(RenderRegion*, Spread*, BOOL, BOOL,
									 BOOL bForceImmediate)
{
	ERROR3("DialogView::ContinueRenderView() should not be called!");
}

/********************************************************************************************

>	CDC *DialogView::GetRenderDC()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/20/95
	Returns:	NULL (always)
	Purpose:	None - DialogView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	DialogView

********************************************************************************************/

wxDC *DialogView::GetRenderDC()
{
	return NULL;
}

/********************************************************************************************

>	BOOL DialogView::GetForeBackMode()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/20/95
	Returns:	FALSE - dialogs don't do background rendering.
	Purpose:	None - DialogView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	DialogView

********************************************************************************************/

BOOL DialogView::GetForeBackMode()
{
	// DialogViews don't background render.
	return FALSE;
}

/********************************************************************************************

>	void DialogView::SetForeBackMode(BOOL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/20/95
	Inputs:		N/A
	Purpose:	None - DialogView is a skeleton class that is only used to access screen
				characteristics.
	Errors:		ERROR3 always - dialogs cannot background render!
	SeeAlso:	DialogView

********************************************************************************************/

void DialogView::SetForeBackMode(BOOL)
{
	ERROR3("DialogView::SetForeBackMode() should not be called!");
}

/********************************************************************************************

>	DocRect DialogView::GetDocViewRect(Spread*)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/20/95
	Inputs:		N/A
	Returns:	An empty rectangle (always).
	Purpose:	None - DialogView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	DialogView

********************************************************************************************/

DocRect DialogView::GetDocViewRect(Spread*)
{
	DocRect Empty;
	return Empty;
}

/********************************************************************************************

>	void DialogView::SetExtent(DocCoord, DocCoord)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/20/95
	Inputs:		N/A
	Purpose:	None - DialogView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	DialogView

********************************************************************************************/

void DialogView::SetExtent(DocCoord, DocCoord)
{
}

/********************************************************************************************

>	WorkRect DialogView::GetViewRect()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/20/95
	Returns:	An empty rectangle (always).
	Purpose:	None - DialogView is a skeleton class that is only used to access screen
				characteristics.
	SeeAlso:	DialogView

********************************************************************************************/

WorkRect DialogView::GetViewRect()
{
	WorkRect Empty;
	return Empty;
}
