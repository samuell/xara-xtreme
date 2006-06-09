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

// class to render into a bitmap (for hit-testing)

/*
*/

#include "camtypes.h"
#include "rendbits.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ccdc.h"
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h"
#include "grndclik.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// RealClickClass is the class name to use for generic 'bitmap click' class whic
// exands to either OSRenderBitmap or GRenderClick

#define	RealClickClass	GRenderClick



#define new CAM_DEBUG_NEW


#ifndef CLR_INVALID
// Win16 doesn't define this
#define	CLR_INVALID	0xFFFFFFFF
#endif



/********************************************************************************************

>	RenderRegion* OSRenderBitmap::Create(DocRect Rect, Matrix ConvertMatrix, FIXED16 Scale, UINT32 Depth)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Creates a region of type OSRenderBitmap (GDI) or GRenderClick (GDraw).
	SeeAlso:	OSRenderRegion::Create

********************************************************************************************/
/*
RenderRegion* OSRenderBitmap::Create(DocRect Rect, Matrix ConvertMatrix, FIXED16 Scale, UINT32 Depth, BOOL colour)
{
	if (colour == FALSE)
		return new GRenderClick( Rect, ConvertMatrix, Scale, Depth, 0.0 );
	else
		return new GRenderClickColour( Rect, ConvertMatrix, Scale, Depth, 0.0 );
}
*/




/********************************************************************************************

>	RenderRegion* OSRenderBitmap::Create(DocRect Rect, Matrix ConvertMatrix, FIXED16 Scale, RenderType rType)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Creates a region of type OSRenderBitmap (GDI) or GRenderClick (GDraw).
	SeeAlso:	OSRenderRegion::Create

********************************************************************************************/

RenderRegion* OSRenderBitmap::Create(DocRect Rect, Matrix ConvertMatrix, FIXED16 Scale, RenderType rType)
{
	switch (rType)
	{
	case RENDERTYPE_MONOBITMAP:
//		TRACE(_T("Creating MONOBITMAP region (%d, %d) - (%d, %d)  GRenderClick\n"), Rect.lo.x, Rect.lo.y, Rect.hi.x, Rect.hi.y);
		return new GRenderClick( Rect, ConvertMatrix, Scale, 1, 0.0 );
		break;

	case RENDERTYPE_COLOURBITMAP:
//		TRACE(_T("Creating COLOURBITMAP region (%d, %d) - (%d, %d)  GRenderClickColour\n"), Rect.lo.x, Rect.lo.y, Rect.hi.x, Rect.hi.y);
		return new GRenderClickColour( Rect, ConvertMatrix, Scale, 32, 0.0 );
		break;

	case RENDERTYPE_HITDETECT:
//		TRACE(_T("Creating HITDETECT region (%d, %d) - (%d, %d)  GRenderClickColourNoPaper\n"), Rect.lo.x, Rect.lo.y, Rect.hi.x, Rect.hi.y);
		return new GRenderClickColourNoPaper( Rect, ConvertMatrix, Scale, 32, 0.0 );
// BODGE TEST! DO NOT RELEASE!
//		return new GRenderClick( Rect, ConvertMatrix, Scale, 1, 0.0 );
		break;

	default:
		ENSURE(FALSE, "Invalid RenderType passed to OSRenderBitmap::Create");
		return NULL;
	}
	return NULL;
}





/********************************************************************************************

>	BitmapContents OSRenderBitmap::GetContents( RenderRegion *pRegion, BOOL justmiddle)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/93
	Inputs:		A render region which must have been created with RenderOnTop(MONOBITMAP).
				if justmiddle is TRUE, only checks on the centre pixel will be done (which
				is faster).
	Outputs:	None
	Returns:	BITMAPCONTENTS_CENTRE if centre pixel is set, _ANY if any pixel is set,
				_NONE if none and _ERROR if failed for some reason.
	Purpose:	To investigate whether anything has been rendered into a bitmap-based
				render region.
	Errors:		Will ENSURE if NULL render region, if it is not really a OSRenderBitmap.
	Scope:		Static

********************************************************************************************/

BitmapContents OSRenderBitmap::GetContents( RenderRegion *pRegion, BOOL JustMiddle)
{
	ERROR2IF( pRegion==NULL, BITMAPCONTENTS_ERROR, "NULL pRegion" );

	ERROR2IF( !pRegion->IsKindOf(CC_RUNTIME_CLASS(GRenderClick)), BITMAPCONTENTS_ERROR, "Wrong class"  );

	RealClickClass *pBitmap = (RealClickClass*)pRegion;	 			// safe to cast now
	return pBitmap->GetContents( JustMiddle );						// then use member fn
}


/********************************************************************************************

>	void OSRenderBitmap::DebugMe( RenderRegion*, INT32 Stretch=1 )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/93
	Inputs:		How much you want the image stretched by
	Outputs:	-
	Returns:	-
	Purpose:	Dupms the current bitmap onto the current view window, so we can see what
				is going on.
	Errors:		-
	Scope:		Public static

********************************************************************************************/

void OSRenderBitmap::DebugMe(RenderRegion *pRegion, INT32 Stretch)
{

	if (pRegion->IsKindOf( CC_RUNTIME_CLASS(GRenderClick) ) )
		((RealClickClass*)pRegion)->DebugMe(Stretch);
	else
		ERROR3("Wrong class for DebugMe");
}
