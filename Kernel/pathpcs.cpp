// $Id: pathpcs.cpp 662 2006-03-14 21:31:49Z alex $
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
// pathpcs - Definition of rendering PathProcessor class

#include "camtypes.h"

#include "pathpcs.h"

#include "paths.h"

CC_IMPLEMENT_DYNAMIC(PathProcessor, CCObject);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



/********************************************************************************************

>	PathProcessor::PathProcessor()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96

	Purpose:	Constructor

********************************************************************************************/

PathProcessor::PathProcessor()
{
	NextProcessor = NULL;
	ParentAttr	  = NULL;
	m_DisableMe   = FALSE;
}



/********************************************************************************************

>	virtual PathProcessor::~PathProcessor()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96

	Purpose:	Destructor
	
	Errors:		If this object still has a non-NULL Next pointer, it will ERROR3 in
				an effort to get you to clean up properly before destruction.

********************************************************************************************/

PathProcessor::~PathProcessor()
{
	ERROR3IF(NextProcessor != NULL, "PathProcessor deleted while still possibly held in stack");
}



/********************************************************************************************

>	virtual BOOL PathProcessor::WillChangeFillAndStrokeSeparately(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96

	Purpose:	Called by the RenderRegion to determine if this PathProcessor affects
				the "fill" and "stroke" portions of the path separately. (Generally
				speaking, only fill/stroke providers will cause these two different
				"bits" of a path to be rendered separately). This call is made BEFORE
				this Processor's ProcessPath function will be called.

				If the caller gets a TRUE back, the stroke and fill paths will be
				rendered separately.

	Notes:		Base class implementation returns FALSE. Derived Stroke and Fill
				processors should probably override this method to return TRUE.

********************************************************************************************/

BOOL PathProcessor::WillChangeFillAndStrokeSeparately(void)
{
	return(FALSE);
}



/********************************************************************************************

>	virtual void PathProcessor::ProcessPath(Path *pPath, RenderRegion *pRender) = 0;

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96

	Purpose:	ABSTRACT FUNCTION - see derived classes

				Called by the RenderRegion to apply the path processing operation to 
				the given path.

	Notes:		* When rendering a path, always pass in your 'this' pointer to
				  RenderRegion::DrawPath, so that you don't start an infinite
				  recursion!

				* To stop rendering of the path, simply return without calling the RR

				* To render this path unchanged, simply call directly back to the RR:
					pRender->DrawPath(pPath, this);
				
				* Only affect the fill of this path if pPath->IsFilled

				* Only affect the stroke of this path if pPath->IsStroked

				* If converting a path into a "filled path" for stroking, the output
				  path should be rendered with IsStroked=FALSE or it'll get a line
				  around the outside!				

				* If you render any attributes, etc, then you should preserve the
				  render region state with calls to SaveContext() and RestoreContext()

********************************************************************************************/


/********************************************************************************************

>	virtual BOOL PathProcessor::DoesActuallyDoAnything(RenderRegion* pRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/10/2000
	Inputs:		pRender - pointer to the render region that we are about to render our path into
	Returns:	TRUE if it is anticipated that this path processor will alter the path in any
				way, FALSE if not
	Purpose:	To determine whether or not our path processor is 'default' i.e. if does not 
				change the path at all.  If so then the render region will be free to use
				DrawPathToOutputDevice which will let us use dash patterns, arrowheads etc.
	
	Errors:		

********************************************************************************************/

BOOL PathProcessor::DoesActuallyDoAnything(RenderRegion* pRender)
{
	return !m_DisableMe;
}


/********************************************************************************************

>	void PathProcessor::SetDisabled(BOOL Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/10/00
	Inputs:		Whether or not to disable this PPB

	Purpose:	Sets the flag which we use to determine whether or not we should do anything
				when it comes to rendering.

********************************************************************************************/

void PathProcessor::SetDisabled(BOOL Value)
{
	m_DisableMe = Value;
}


/********************************************************************************************

>	BOOL PathProcessor::IsDisabled()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/10/00
	Inputs:		-
	Returns:	Whether or not we are disabled
	Purpose:	

********************************************************************************************/

BOOL PathProcessor::IsDisabled()
{
	return m_DisableMe;
}
