// $Id: tranlate.cpp 662 2006-03-14 21:31:49Z alex $
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
// Operation for doing an interactive Translation of the selection

/*
*/

#include "camtypes.h"
#include "tranlate.h"

#include "docview.h"
//#include "rik.h"
//#include "clikdrag.h"

// Register my files version number
DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNCREATE(OpTranslateTrans, TransOperation)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW





/********************************************************************************************

>	OpTranslateTrans::OpTranslateTrans()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Purpose:	Constructor. Does nothing.

********************************************************************************************/

OpTranslateTrans::OpTranslateTrans(): TransOperation()
{
	//Set status line help
	StatusHelpID = _R(IDS_TRANSLTRANS_STATUS1);
	StatusHelpID2 = _R(IDS_TRANSLTRANS_STATUS2);
	CanScaleLines = FALSE;
}




/********************************************************************************************

>	void OpTranslateTrans::InitTransformImmediate(OpParam* pOpParam)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		pOpParam - The parameters that were passed into the operation
	Purpose:	Sets up the transform ready for an immediate translation. This is called from
				DoWithParam()
	SeeAlso:	TransOperation::DoWithParam()

********************************************************************************************/

void OpTranslateTrans::InitTransformImmediate(OpParam* pOpParam)
{
	// Set the initial position
	StartPos = DocCoord(0,0);
	RawStartPos = StartPos;
	MagStartPos = StartPos;

	// and copy the offset to translate by from Param2
	DocCoord* Offset = (DocCoord*)( PVOID(pOpParam->Param2) );
	LastPos.x = Offset->x;
	LastPos.y = Offset->y;

	OriginalGridOffset.x=0;
	OriginalGridOffset.y=0;
}


/********************************************************************************************

>	virtual void OpTranslateTrans::InitTransformOnDrag(DocCoord PointerPos, ClickModifiers ClickMods)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The position of the mouse at the start of the drag
				ClickMods - the keyboard modifiers that were active at the start of the drag
	Purpose:	Sets up the parameters needed to build the transform matrix at the start
				of the drag. This base class version of this function does nothing.

********************************************************************************************/

void OpTranslateTrans::InitTransformOnDrag(DocCoord PointerPos, ClickModifiers ClickMods)
{
	// make a note of the current mouse position
	LastPos = PointerPos;

	// Record the offset from the mouse pos to the grid
	OriginalGridOffset = GetStartPos();
	DocView::ForceSnapToGrid(StartSpread, &OriginalGridOffset);
	OriginalGridOffset = GetStartPos() - OriginalGridOffset;
}




/********************************************************************************************

>	virtual void OpTranslateTrans::UpdateTransformOnDrag(DocCoord PointerPos, Spread* pSpread,
									ClickModifiers& ClickMods)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The latest position of the mouse
	Purpose:	Does the calculations needed when the mouse moves to keep all the params
				needed to build the rotation matrix up to date.

********************************************************************************************/

void OpTranslateTrans::UpdateTransformOnDrag(DocCoord PointerPos, Spread* pSpread, ClickModifiers& ClickMods)
{
	if (!ClickMods.Constrain)
	{
		// Apply snapping to the pointer pos
		// First apply magnetic snapping alone
		if (MagneticGripPoint && DocView::SnapSelected(pSpread, &PointerPos, TRUE, FALSE))
		{
			// Magnetic snapping worked!
		}
		else
		{
			// Magnetic snapping failed!
			// If magnetic snapping failed then try grid snapping
			// on the adjusted coordinate
			if (ClickMods.Alternative1)
			{
				PointerPos = PointerPos - OriginalGridOffset;
				DocView::SnapSelected(pSpread, &PointerPos, FALSE, TRUE);
				PointerPos = PointerPos + OriginalGridOffset;
			}
			else
			{
				DocCoord Offset = PointerPos - LastPos;
				DocRect Bounds(	BoundingData.x, BoundingData.y,
								BoundingData.x+BoundingData.Width, BoundingData.y+BoundingData.Height
							   );
				Bounds.lo = Bounds.lo + Offset;
				Bounds.hi = Bounds.hi + Offset;
				DocRect SnappedBounds = Bounds;
				DocView::SnapSelected(pSpread,&SnappedBounds,LastRawPos,RawPos);
				PointerPos = PointerPos + (SnappedBounds.lo - Bounds.lo);
			}
		}
	}

	// Work out the offset from the last mouse pos
	INT32 dx = PointerPos.x - LastPos.x;
	INT32 dy = PointerPos.y - LastPos.y;

	// Add the offset into the structure
	BoundingData.x += dx;
	BoundingData.y += dy;
	BoundingData.XYChanged = TRUE;

	// Make a mental note of the current position
	LastPos = PointerPos;

	// Update the current spread
	CurrentSpread = pSpread;

	// Have a look to see if we are on a different spread now
	if (pSpread != StartSpread)
		CanChangeToNewSpread = TRUE;
	else
		CanChangeToNewSpread = FALSE;
}



/********************************************************************************************

>	void OpTranslateTrans::BuildMatrix()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Purpose:	Builds the transform matrix required to rotate the selection about the
				point CentreOfRot by the required number of degrees

********************************************************************************************/

void OpTranslateTrans::BuildMatrix()
{
	// Build a translation matrix by takeing the offset from the last mouse position
	// to the start mouse position
		Transform = Matrix(LastPos.x-GetStartPos().x, LastPos.y-GetStartPos().y);
}



/********************************************************************************************

>	virtual BOOL OpTranslateTrans::ShouldPointerBeOffset()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/94
	Returns:	FALSE
	Purpose:	Tells the base class of the operation that we want our mouse coords to be
				left alone

********************************************************************************************/

BOOL OpTranslateTrans::ShouldPointerBeOffset()
{
	return FALSE;
}




/********************************************************************************************

>	virtual void OpTranslateTrans::ConstrainDrag(DocCoord* PointerPos)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The current position of the mouse that needs to be constrained
	Outputs:	PointerPos - The position of the mouse after it has been constrained
	Purpose:	Will constrain the mouse position to lie along rays from the centre of
				rotation at the constrain angle apart (45 degrees by default)

********************************************************************************************/

void OpTranslateTrans::ConstrainDrag(DocCoord* PointerPos)
{
	// Lock the mouse to move along the axis or diagonally
	DocCoord Blobby = GetStartPos();
	DocView::ConstrainToAngle(Blobby, PointerPos);
}


/********************************************************************************************

>	BOOL OpTranslateTrans::Declare()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpTranslateTrans::Declare()
{
	return (RegisterOpDescriptor(0, _R(IDS_SELECTOR_MOVE), CC_RUNTIME_CLASS(OpTranslateTrans),
								OPTOKEN_TRANSLATE, TransOperation::GetState)); 
}
