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
// The Shear transform Operation

/*
*/

#include "camtypes.h"
#include "opshear.h"

#include "app.h"
#include "docview.h"
#include "ink.h"
#include "trans2d.h"
//#include "rik.h"
#include "docview.h"
//#include "clikdrag.h"


DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNCREATE(OpShearTrans, TransOperation)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW




/********************************************************************************************

>	OpShearTrans::OpShearTrans()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Purpose:	Constructor. Does nothing.

********************************************************************************************/

OpShearTrans::OpShearTrans(): TransOperation()
{
	// Reset all the infomation about things
	SelWidth = SelHeight = 0;

	// Default to horizontal
	IsHorizontal = TRUE;

	// Start off with no shearing at all
	ShearBy = FIXED16(0);

	// Set status help text
	StatusHelpID = _R(IDS_SHEARTRANS_STATUS1);
	StatusHelpID2=_R(IDS_SHEARTRANS_STATUS2);
}



/********************************************************************************************

>	void OpShearTrans::SetStartBlob(INT32 StartBlob)

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

void OpShearTrans::SetStartBlob(INT32 StartBlob)
{
	// Only blobs 2, 7, 4 and 5 are relavant. All others are a problem
	ENSURE( (StartBlob==2) || (StartBlob==4) || (StartBlob==5) || (StartBlob==7),
			"Shear started a blob that it should not have done");

	// blobs 2 and 7 are the Middle Top and Middle Bottom blobs
	// and would suggest horizontal shearing
	if ((StartBlob==2) || (StartBlob==7))
		IsHorizontal = TRUE;
	else
		IsHorizontal = FALSE;

}




/********************************************************************************************
>	virtual void OpShearTrans::InitTransformImmediate(OpParam* pParam)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Inputs:		pParam		---		pointer an OpParam whose Param2 member is a pointer
									to a FIXED16 that represents the angle in degrees to 
									shear by.
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void OpShearTrans::InitTransformImmediate(OpParam* pParam)
{
	FIXED16 pDegreesShearAngle = *((FIXED16*)PVOID(pParam->Param2));
	double ShearAngle = pDegreesShearAngle.MakeDouble();

	if (ShearAngle < -89.0)						// Clip the shear angle to a sensible range
		ShearAngle = -89.0;

	if (ShearAngle > 89.0)
		ShearAngle = 89.0;

	ShearAngle = (ShearAngle*2*PI)/360.0;		// Convert to Radians
	ShearBy = (FIXED16)(tan(ShearAngle));		// And get the Shear distance to bung in the matrix
}




/********************************************************************************************

>	virtual void OpShearTrans::InitTransformOnDrag(DocCoord PointerPos, ClickModifiers ClickMods)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The position of the mouse at the start of the drag
				ClickMods - the keyboard modifiers that were active at the start of the drag
	Purpose:	Sets up the parameters needed to build the transform matrix at the start
				of the drag. This base class version of this function does nothing.

********************************************************************************************/

void OpShearTrans::InitTransformOnDrag(DocCoord PointerPos, ClickModifiers ClickMods)
{
	// Make a note of the latest position of the mouse
	LastPos = PointerPos;

	// At the start of the drag the Shear factor will always by none
	ShearBy = FIXED16(0);

	// Find out how big the selection is
//	SelRange* Selection = GetApplication()->FindSelection();
//	DocRect SelRect = Selection->GetBoundingRect();

	// Find the width and the height of the selection
//	SelWidth = SelRect.Width();
//	SelHeight = SelRect.Height();

	// Use the Width and Height of the object AS PASSED IN from the caller
	// (If we just use the width/height of the selection, we may get it all horribly wrong, as
	// they may want to scale with or without including the effects of attributes, which
	// can significantly affect the size of the bounding rectangle to be used)
	SelWidth  = BoundingData.Width;
	SelHeight = BoundingData.Height;
}




/********************************************************************************************

>	virtual void OpShearTrans::UpdateTransformOnDrag(DocCoord PointerPos, Spread*, ClickModifiers& ClickMods)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The latest position of the mouse
				Spread the mouse is over - not used
				Clickmodifiers in place - not used
	Purpose:	Does the calculations needed when the mouse moves to keep all the params
				needed to build the rotation matrix up to date.

********************************************************************************************/

void OpShearTrans::UpdateTransformOnDrag(DocCoord PointerPos, Spread* pClickSpread, ClickModifiers& ClickMods)
{
	// Make sure that the cursor does not wrap around at the edge of spreads
	if (pClickSpread != StartSpread)
		PointerPos = MakeRelativeToSpread(StartSpread, pClickSpread, PointerPos);

	if (!ClickMods.Constrain)
		DocView::SnapSelected(pClickSpread, &PointerPos, FALSE, TRUE);

	// Make a note of the latest position of the mouse
	LastPos = PointerPos;

	double dx = PointerPos.x - CentreOfTrans.x;
	double dy = PointerPos.y - CentreOfTrans.y;

	// Work out the shear factor and angle
	double ShearAngle;
	if (IsHorizontal)
	{
		// Work out the horizontal shearing
		dy = StartPos.y - CentreOfTrans.y;
		ShearBy = FIXED16( dx / dy);
		ShearAngle = atan2(dx, dy);
	}
	else
	{
		// Work out the vertical shearing
		dx = StartPos.x - CentreOfTrans.x;
		ShearBy = FIXED16( dy / dx);
		ShearAngle = atan2(dy, dx);
	}

	// Update the boundingdata infomation about the constrain
	ShearAngle = (ShearAngle / (2.0*PI)) * 360.0;
	BoundingData.Shear = (ANGLE) ShearAngle;
	BoundingData.ShearChanged = TRUE;
}





/********************************************************************************************

>	void OpShearTrans::ConstrainDrag(DocCoord* PointerPos)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/94
	Outputs:	PointerPos - Adjusts the mouse position so that it is constrained
	Purpose:	Constrains the drag by the constrain angle. If the angle becomes too shallow
				(eg 90 degrees) the constrain will stop constraining and let it move freely.

********************************************************************************************/

void OpShearTrans::ConstrainDrag(DocCoord* PointerPos)
{
	// Make a copy of the coord in case we want to put it back
	DocCoord WorkingCopy = *PointerPos;
	BOOL UseWorkingCopy = FALSE;

	// Snap the pointer coord so that it runs along one of the rays
	DocView::ConstrainToAngle(CentreOfTrans, &WorkingCopy);

	// find out the x & y distance from the centre
	INT32 dx = WorkingCopy.x - CentreOfTrans.x;
	INT32 dy = WorkingCopy.y - CentreOfTrans.y;

	if (IsHorizontal)
	{
		if (dy!=0)
		{
			// This is a horizontal drag, so make sure that the pointer pos
			// lies on the horizon line level with the starting position
			// We need 64 bit numbers in the middle of the calculation so
			// use something that does it for us.
			dx = Mul32Div32(dx, StartPos.y-CentreOfTrans.y, dy);

			// only need to change the x component as that is all that is used
			WorkingCopy.x = CentreOfTrans.x + dx;
			WorkingCopy.y = StartPos.y;
			UseWorkingCopy = TRUE;
		}
	}
	else
	{
		if (dx!=0)
		{
			// Same but in the other axis
			dy = Mul32Div32(dy, StartPos.x-CentreOfTrans.x, dx) ;

			// Only change the y component this time
			WorkingCopy.y = CentreOfTrans.y + dy;
			WorkingCopy.x = StartPos.x;
			UseWorkingCopy = TRUE;
		}
	}

	// See if the constrain was valid
	if (UseWorkingCopy)
		*PointerPos = WorkingCopy;
}

/********************************************************************************************

>	void OpShearTrans::BuildMatrix()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Purpose:	Builds the transform matrix required to Shear the selection about the
				point CentreOfRot

********************************************************************************************/

void OpShearTrans::BuildMatrix()
{
	// Translate to the origin
	Transform = Matrix(-CentreOfTrans.x, -CentreOfTrans.y);

	// We will need a matrix to put the shear in
	Matrix Shear;

	// Work out how much to shear the selection by
	if (IsHorizontal)
	{
		// Build the shear matrix for a horizontal shear
		Shear = Matrix(FIXED16(1), FIXED16(0), FIXED16(ShearBy), FIXED16(1), 0, 0);
	}
	else
	{
		// Build the shear matrix for a vertical shear
		Shear = Matrix(FIXED16(1), FIXED16(ShearBy), FIXED16(0), FIXED16(1), 0, 0);
	}

	// Apply the shear
	Transform *= Shear;

	// Translate back again
	Transform *= Matrix(CentreOfTrans.x, CentreOfTrans.y);
}




/********************************************************************************************

>	BOOL OpShearTrans::Declare()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpShearTrans::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_SHEARTRANS),
								CC_RUNTIME_CLASS(OpShearTrans),
								OPTOKEN_SHEAR,
								TransOperation::GetState)); 
}



