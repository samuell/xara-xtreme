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
// The Squash transform Operation

/*
*/

#include "camtypes.h"
#include "opsquash.h"

#include "app.h"
#include "trans2d.h"
//#include "rik.h"
#include "selector.h"
#include "docview.h"
//#include "clikdrag.h"


DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNCREATE(OpSquashTrans, TransOperation)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW




/********************************************************************************************

>	OpSquashTrans::OpSquashTrans()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Purpose:	Constructor. Does nothing.

********************************************************************************************/

OpSquashTrans::OpSquashTrans(): TransOperation()
{
	// Reset all the infomation about things
	SelWidth = SelHeight = 0;

	// Default to horizontal
	SquashType = SQUASH_RIGHT;

	// Set the default scale factors
	XScale =  YScale = FIXED16(1.0);

	// Set status help
	StatusHelpID = _R(IDS_SQUASHTRANS_STATUS1);
	StatusHelpID2 = _R(IDS_SQUASHTRANS_STATUS2);
}


/********************************************************************************************

>	virtual void OpSquashTrans::InitTransformImmediate(OpParam* pOpParam)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		pOpParam - The parameters that were passed into the operation
	Purpose:	Sets up the transform ready for an immediate rotation. This is called from
				DoWithParam()
	SeeAlso:	TransOperation::DoWithParam()

********************************************************************************************/

void OpSquashTrans::InitTransformImmediate(OpParam* pOpParam)
{
	// Depending on the which way we are scaling this object
	switch (SquashType)
	{
		case SQUASH_UP :
		case SQUASH_DOWN :
		{
			// Stretch it vertically
			XScale = FIXED16(1.0);
			YScale = *((FIXED16*)PVOID(pOpParam->Param2));
			break;
		}

		case SQUASH_LEFT :
		case SQUASH_RIGHT :
		{
			// Stretch it horizintally
			XScale = *((FIXED16*)PVOID(pOpParam->Param2));
			YScale = FIXED16(1.0);
			break;
		}
	}
}




/********************************************************************************************

>	void OpSquashTrans::SetStartBlob(INT32 StartBlob)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/94
	Inputs:		StartBlob  - the Number of the blob that the operation was initiated on,
				if applicabale. They are numbered as follows :-
				MonoOn
				1  2  3
				4     5
				6  7  8
				MonoOff
				All operations that were not started from one of the 8 blobs should use 0
	Purpose:	Allows the operations to know how it was started. This operation is
				interested in the horizontal / vertical nature of the transform

********************************************************************************************/

void OpSquashTrans::SetStartBlob(INT32 StartBlob)
{
	switch (StartBlob)
	{
		case 2:
			SquashType = SQUASH_UP;
			break;

		case 4:
			SquashType = SQUASH_LEFT;
			break;

		case 5:
			SquashType = SQUASH_RIGHT;
			break;

		case 7:
			SquashType = SQUASH_DOWN;
			break;

		default:
			ERROR3("OpSquashTrans started from a blob that it should not have done");
			break;
	}
}




/********************************************************************************************

>	virtual void OpSquashTrans::InitTransformOnDrag(DocCoord PointerPos, ClickModifiers ClickMods)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The position of the mouse at the start of the drag
				ClickMods - the keyboard modifiers that were active at the start of the drag
	Purpose:	Sets up the parameters needed to build the transform matrix at the start
				of the drag. This base class version of this function does nothing.

********************************************************************************************/

void OpSquashTrans::InitTransformOnDrag(DocCoord PointerPos, ClickModifiers ClickMods)
{
	// Make a note of the latest position of the mouse
	LastPos = PointerPos;

	// At the start of the drag the Squash factor will always by none
	XScale = FIXED16(1);
	YScale = FIXED16(1);

	// Use the Width and Height of the object AS PASSED IN from the caller
	// (If we just use the width/height of the selection, we may get it all horribly wrong, as
	// they may want to scale with or without including the effects of attributes, which
	// can significantly affect the size of the bounding rectangle to be used)
	SelWidth  = BoundingData.Width;
	SelHeight = BoundingData.Height;
}




/********************************************************************************************

>	virtual void OpSquashTrans::UpdateTransformOnDrag(DocCoord PointerPos, Spread*, ClickModifiers&)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The latest position of the mouse
				Spread the mouse is over - not used
				Clickmodifiers in place - not used
	Purpose:	Does the calculations needed when the mouse moves to keep all the params
				needed to build the rotation matrix up to date.
				Note by Phil: This algorithm needs four cases because the scale factors
				involved must be positive whether dragging the left or right handle. It should
				only go negative when the pointer crosses a line through the transform origin.

********************************************************************************************/

void OpSquashTrans::UpdateTransformOnDrag(DocCoord PointerPos, Spread* pClickSpread, ClickModifiers& ClickMods)
{
	// Make sure that the cursor does not wrap around at the edge of spreads
	if (pClickSpread != StartSpread)
		PointerPos = MakeRelativeToSpread(StartSpread, pClickSpread, PointerPos);

	// Apply snapping to the pointer pos
	if (!ClickMods.Constrain)
		DocView::SnapSelected(pClickSpread, &PointerPos, FALSE, TRUE);

	// Make a note of the latest position of the mouse
	LastPos = PointerPos;
	INT32 Offset;

	// copy the height anf width params so that we can party on them
	INT32 WorkingHeight = SelHeight;
	INT32 WorkingWidth = SelWidth;

	// if Adjust is being used, adjust the height and width so that scaling still works
	if (ClickMods.Adjust)
	{
		WorkingHeight /= 2;
		WorkingWidth /= 2;
	}

	// Avoid fp div zero bugs.
	if (WorkingHeight == 0)
		WorkingHeight = 1;
	if (WorkingWidth == 0)
		WorkingWidth = 1;

	// Work out the shear factor
	switch (SquashType)
	{
		case SQUASH_UP:
			Offset = LastPos.y - CentreOfTrans.y;
			YScale = FIXED16( (double)Offset / (double)WorkingHeight);
		break;

		case SQUASH_LEFT:
			Offset = CentreOfTrans.x - LastPos.x;
			XScale = FIXED16( (double)Offset / (double)WorkingWidth);
		break;

		case SQUASH_RIGHT:
			Offset = LastPos.x - CentreOfTrans.x;
			XScale = FIXED16( (double)Offset / (double)WorkingWidth);
		break;

		case SQUASH_DOWN:
			Offset = CentreOfTrans.y - LastPos.y;
			YScale = FIXED16( (double)Offset / (double)WorkingHeight);
		break;

	}

	// Deal with Constrain...
	// This has to be done in here and not in the ConstrainDrag function because it has to
	// modify the scale factors computed by the switch statement above.
	if (ClickMods.Constrain)
	{
		// Constrained squash forces scale to whole number multiple of original size...
		FIXED16 Sign;

		Sign = (XScale<0 ? -1 : 1);					// Compute sign of pure scale
		XScale+=FIXED16((double)0.5);				// Round scale to nearest integer
		XScale = XScale.MakeShort();
		if (XScale==0) XScale = Sign;				// Prevent scale from being zero by setting
													// it to +/- 1 depending on sign of pure scale.
		Sign = (YScale<0 ? -1 : 1);
		YScale+=FIXED16((double)0.5);
		YScale = YScale.MakeShort();
		if (YScale==0) YScale = Sign;
	}

	BoundingData.XScale = XScale;
	BoundingData.YScale = YScale;
	BoundingData.ScaleChanged = TRUE;
	// See also, UpdateTransformBoundingData below!
}



/********************************************************************************************

>	void OpSquashTrans::BuildMatrix()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Purpose:	Builds the transform matrix required to Shear the selection about the
				point CentreOfRot

********************************************************************************************/

void OpSquashTrans::BuildMatrix()
{
	// Translate to the origin
	Transform = Matrix(-CentreOfTrans.x, -CentreOfTrans.y);

	// We will need a matrix to put the Scale in
	Matrix Squash(XScale, YScale);

	// translate back again
	Matrix TransFromOrigin(CentreOfTrans.x, CentreOfTrans.y);

	// Combine the 3 of them into a single matrix
	Transform *= Squash;
	Transform *= TransFromOrigin;
}




/********************************************************************************************

>	BOOL OpSquashTrans::Declare()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpSquashTrans::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_SQUASHTRANS),
								CC_RUNTIME_CLASS(OpSquashTrans),
								OPTOKEN_SQUASH,
								TransOperation::GetState)); 
}



/********************************************************************************************

>	void OpSquashTrans::UpdateTransformBoundingData()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/9/94
	Purpose:	Override the base class version of this fn to ensure that new bounds
				are calculated and placed in the BoundingData structure.

********************************************************************************************/

void OpSquashTrans::UpdateTransformBoundingData()
{
#ifndef STANDALONE
	if (pSelTool != NULL)
	{
		ComputeNewBounds();

		// Tell the tool about the current transform bounding data
		pSelTool->DragMove(&BoundingData);
	}
#endif
}



