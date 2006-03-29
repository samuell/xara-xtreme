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
// Implementation of the fill nudge ops

/*
*/

#include "camtypes.h"
#include "fillndge.h"

#include "app.h"
//#include "markn.h"
#include "nodeattr.h"
#include "fillattr.h"
#include "opnudge.h"
#include "trans2d.h"
//#include "will.h"
#include "attrmgr.h"
#include "docview.h"

class Spread;

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(OpFillNudge,Operation)
CC_IMPLEMENT_DYNCREATE(FillGeometryNudger, AttrValueChange)

BOOL OpFillNudge::NudgeCols;		// Shall we nudge colour or transparency fills ?

static CCRuntimeClass* LastNudgeType = NULL;
static DocCoord LastNudgeStart = DocCoord(0,0);
static DocCoord LastNudgeEnd = DocCoord(0,0);
static DocCoord LastNudgeEnd2 = DocCoord(0,0);

/********************************************************************************************

>	static void OpFillNudge::NudgeColours(BOOL cols)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/94
	Purpose:	Sets whether a nudge will nudge colour or transparency fills.

********************************************************************************************/

void OpFillNudge::NudgeColours(BOOL cols)
{
	NudgeCols = cols;
}

/********************************************************************************************

>	void IMPLEMENT_OPFILLNUDGE_CLASS(DIR,X_NUDGE_FACTOR,Y_NUDGE_FACTOR,UNDO_IDS)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (via MarkN)
	Created:	8/3/95
	Inputs:		DIR 			= String postfix specifying the nudge direction and magnitude, e.g. Up1
				X_NUDGE_FACTOR 	= signed INT32 giving the x nudge magnitude and direction
				Y_NUDGE_FACTOR 	= y version of above
				UNDO_IDS		= IDS of undo string to append to the undo/redo men items
	Outputs:	-
	Returns:	
	Purpose:	The macro IMPLEMENT_OPFILLNUDGE_CLASS(DIR,X_NUDGE_FACTOR,Y_NUDGE_FACTOR,UNDO_IDS) implements
				the OpNudgeX Do() function.
				E.g. OpNudgeUp1

				Firstly, CC_IMPLEMENT_DYNCREATE(OpNudgeUp1,OpNudge) is done

				Then itsets the OpNudge protected member vars as follows-
					X_NudgeFactor = 0; 					// I.e. don't nudge in the X dir
					Y_NudgeFactor = 1; 					// I.e. Nudge up by 1 nudge step 
					NudgeUndoIDS  = _R(IDS_UNDO_NUDGE_UP); 	// This string appended to the undo/redo menu items

				It then calls OpNudge::Do() which uses the nudge factors to determine nudging magnitude and
				direction

	Errors:		-
	SeeAlso:	IMPLEMENT_OPNUDGE_CLASS

********************************************************************************************/

#define IMPLEMENT_OPFILLNUDGE_CLASS(DIR,X_NUDGE_FACTOR,Y_NUDGE_FACTOR,UNDO_IDS) \
	CC_IMPLEMENT_DYNCREATE(OpFillNudge ## DIR,OpFillNudge) \
	void OpFillNudge ## DIR::Do(OpDescriptor* pOpDesc) \
	{ \
		 X_NudgeFactor = X_NUDGE_FACTOR; \
		 Y_NudgeFactor = Y_NUDGE_FACTOR; \
		NudgeUndoIDS  = UNDO_IDS; \
		OpFillNudge::Do(pOpDesc); \
	} \

/*OpFillNudgeUp5::Do (OpDescriptor* pOpDesc)
{
	X_NudgeFactor = 0;	Y_NudgeFactor = 1;		flag=TRUE;		NudgeUndoIDS = _R(IDS_UNDO_NUDGE_UP);
	OpFillNudge::Do(pOpDesc);
}*/


IMPLEMENT_OPFILLNUDGE_CLASS	(Up1,		 0, 1,		_R(IDS_UNDO_NUDGE_UP))
IMPLEMENT_OPFILLNUDGE_CLASS (Up5,		 0,	5,		_R(IDS_UNDO_NUDGE_UP))
IMPLEMENT_OPFILLNUDGE_CLASS	(Up10,		 0,	10,		_R(IDS_UNDO_NUDGE_UP))
IMPLEMENT_OPFILLNUDGE_CLASS	(UpFifth,	 0, 0.2,	_R(IDS_UNDO_NUDGE_UP))
IMPLEMENT_OPFILLNUDGE_CLASS	(UpPixel1,	 0, 1,		_R(IDS_UNDO_NUDGE_UP))
IMPLEMENT_OPFILLNUDGE_CLASS	(UpPixel10,	 0, 10,		_R(IDS_UNDO_NUDGE_UP))

IMPLEMENT_OPFILLNUDGE_CLASS	(Down1,		 0,-1,		_R(IDS_UNDO_NUDGE_DOWN))
IMPLEMENT_OPFILLNUDGE_CLASS	(Down5 ,	 0,-5,		_R(IDS_UNDO_NUDGE_DOWN))
IMPLEMENT_OPFILLNUDGE_CLASS	(Down10,	 0,-10,		_R(IDS_UNDO_NUDGE_DOWN))
IMPLEMENT_OPFILLNUDGE_CLASS	(DownFifth,  0, -0.2,	_R(IDS_UNDO_NUDGE_DOWN))
IMPLEMENT_OPFILLNUDGE_CLASS	(DownPixel1, 0, -1,		_R(IDS_UNDO_NUDGE_DOWN))
IMPLEMENT_OPFILLNUDGE_CLASS	(DownPixel10, 0,-10,	_R(IDS_UNDO_NUDGE_DOWN))

IMPLEMENT_OPFILLNUDGE_CLASS	(Left1,		 -1, 0,		_R(IDS_UNDO_NUDGE_LEFT))
IMPLEMENT_OPFILLNUDGE_CLASS	(Left5,		 -5, 0,		_R(IDS_UNDO_NUDGE_LEFT))
IMPLEMENT_OPFILLNUDGE_CLASS	(Left10,	 -10,0,		_R(IDS_UNDO_NUDGE_LEFT))
IMPLEMENT_OPFILLNUDGE_CLASS (LeftFifth,  -0.2,0,	_R(IDS_UNDO_NUDGE_LEFT))
IMPLEMENT_OPFILLNUDGE_CLASS	(LeftPixel1, -1, 0,		_R(IDS_UNDO_NUDGE_LEFT))
IMPLEMENT_OPFILLNUDGE_CLASS	(LeftPixel10,-10, 0,	_R(IDS_UNDO_NUDGE_LEFT))
		

IMPLEMENT_OPFILLNUDGE_CLASS	(Right1,	   1,  0,	_R(IDS_UNDO_NUDGE_RIGHT))
IMPLEMENT_OPFILLNUDGE_CLASS	(Right5,	   5,  0,	_R(IDS_UNDO_NUDGE_RIGHT))
IMPLEMENT_OPFILLNUDGE_CLASS	(Right10,	   10, 0,	_R(IDS_UNDO_NUDGE_RIGHT))
IMPLEMENT_OPFILLNUDGE_CLASS	(RightFifth,   0.2,0,	_R(IDS_UNDO_NUDGE_RIGHT))
IMPLEMENT_OPFILLNUDGE_CLASS	(RightPixel1,  1,  0,	_R(IDS_UNDO_NUDGE_RIGHT))
IMPLEMENT_OPFILLNUDGE_CLASS	(RightPixel10, 10, 0,	_R(IDS_UNDO_NUDGE_RIGHT))


#define new CAM_DEBUG_NEW

MILLIPOINT 	OpFillNudge::NudgeStep	 = 2835;	// The default size of a single nudge (1mm)
//changed from INT32 to double to allow .2 MM Nudging
double		OpFillNudge::X_NudgeFactor = 1;		// Distance nudged in x = NudgeStep * X_NudgeFactor;
double		OpFillNudge::Y_NudgeFactor = 0;		// Distance nudged in y = NudgeStep * Y_NudgeFactor;


/********************************************************************************************

>	void OpFillNudge::Do(OpDescriptor* pOpDesc)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/3/95
	Inputs:		pOpDesc = ptr to the op descriptor
	Purpose:	The path nudge op's Do() function.
	SeeAlso:	OpNudge::Do()

********************************************************************************************/

void OpFillNudge::Do(OpDescriptor* pOpDesc)
{

	//	Get scaled pixel size 
	FIXED16 ScaledPixelWidth, ScaledPixelHeight;
	GetWorkingView()->GetScaledPixelSize(&ScaledPixelWidth, &ScaledPixelHeight) ;
	PixelNudge=ScaledPixelWidth.MakeDouble() ;

	// Determine the nudge factors based on the OpDescriptor used to invoke the nudge operation
	String OpToken = pOpDesc->Token; 

	if		(OpToken == String(OPTOKEN_NUDGEUPPIXEL1))		{ IsPixelNudgeClass = TRUE; }
	else if	(OpToken == String(OPTOKEN_NUDGEUPPIXEL10))		{ IsPixelNudgeClass = TRUE; }

	else if	(OpToken == String(OPTOKEN_NUDGEDOWNPIXEL1))	{ IsPixelNudgeClass = TRUE; }
	else if	(OpToken == String(OPTOKEN_NUDGEDOWNPIXEL10))	{ IsPixelNudgeClass = TRUE; }

	else if	(OpToken == String(OPTOKEN_NUDGELEFTPIXEL1))	{ IsPixelNudgeClass = TRUE; }
	else if	(OpToken == String(OPTOKEN_NUDGELEFTPIXEL10))	{ IsPixelNudgeClass = TRUE; }

	else if	(OpToken == String(OPTOKEN_NUDGERIGHTPIXEL1))	{ IsPixelNudgeClass = TRUE; }
	else if	(OpToken == String(OPTOKEN_NUDGERIGHTPIXEL10))	{ IsPixelNudgeClass = TRUE; }

	else if	(OpToken == String(OPTOKEN_NUDGEUP1))			{ IsPixelNudgeClass = FALSE; }
	else if	(OpToken == String(OPTOKEN_NUDGEUP5))			{ IsPixelNudgeClass = FALSE; }
	else if	(OpToken == String(OPTOKEN_NUDGEUP10))			{ IsPixelNudgeClass = FALSE; }
	else if	(OpToken == String(OPTOKEN_NUDGEUPFIFTH))		{ IsPixelNudgeClass = FALSE; }

	else if	(OpToken == String(OPTOKEN_NUDGEDOWN1))			{ IsPixelNudgeClass = FALSE; }
	else if	(OpToken == String(OPTOKEN_NUDGEDOWN5))			{ IsPixelNudgeClass = FALSE; }
	else if	(OpToken == String(OPTOKEN_NUDGEDOWN10))		{ IsPixelNudgeClass = FALSE; }
	else if	(OpToken == String(OPTOKEN_NUDGEDOWNFIFTH))		{ IsPixelNudgeClass = FALSE; }

	else if	(OpToken == String(OPTOKEN_NUDGELEFT1))			{ IsPixelNudgeClass = FALSE; }
	else if	(OpToken == String(OPTOKEN_NUDGELEFT5))			{ IsPixelNudgeClass = FALSE; }
	else if	(OpToken == String(OPTOKEN_NUDGELEFT10))		{ IsPixelNudgeClass = FALSE; }
	else if	(OpToken == String(OPTOKEN_NUDGELEFTFIFTH))		{ IsPixelNudgeClass = FALSE; }

	else if	(OpToken == String(OPTOKEN_NUDGERIGHT1))		{ IsPixelNudgeClass = FALSE; }
	else if	(OpToken == String(OPTOKEN_NUDGERIGHT5))		{ IsPixelNudgeClass = FALSE; }
	else if	(OpToken == String(OPTOKEN_NUDGERIGHT10))		{ IsPixelNudgeClass = FALSE; }
	else if	(OpToken == String(OPTOKEN_NUDGERIGHTFIFTH))	{ IsPixelNudgeClass = FALSE; }

	// Setup the Offset DocCoord to contain the X and Y translation values	
	DocCoord Offset;

	if (IsPixelNudgeClass)
	{
		Offset.x=(INT32)GetXPixelDisplacement();
		Offset.y=(INT32)GetYPixelDisplacement();
	}
	else if (!IsPixelNudgeClass)
	{
		//TypeCast from double to INT32
		Offset.x=(INT32) GetXNudgeDisplacement();
		Offset.y=(INT32) GetYNudgeDisplacement();
	}

	if (AttrFillGeometry::SelectionCount == 0)
	{
			// Nothing to Nudge !!
		FailAndExecute();
		End();
		return;
	}

	FillGeometryNudger* FillNudger = new FillGeometryNudger();

	if (FillNudger == NULL)
	{
		FailAndExecute();
		End();
		return;
	}

	FillNudger->MutateColourFills(NudgeCols);
	FillNudger->SetNudgeDistance(Offset.x, Offset.y);

	AttributeManager::AttributeSelected(NULL, FillNudger);

	LastNudgeType 	= NULL;
	LastNudgeStart 	= DocCoord(0,0);
	LastNudgeEnd 	= DocCoord(0,0);
	LastNudgeEnd2 	= DocCoord(0,0);
	
	End();
}

/********************************************************************************************

>	void OpFillNudge::GetOpName(String_256* OpName) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/3/95
	Outputs:	The undo string for the operation
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	SeeAlso:	OpNudge::GetOpName()

********************************************************************************************/

void OpFillNudge::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(NudgeUndoIDS);
}  


/********************************************************************************************

							 	FillGeometryNudger

********************************************************************************************/


/********************************************************************************************

>	UINT32 FillGeometryNudger::GetAttrNameID(void)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/94
	Purpose:	Get a textual description of this object.

********************************************************************************************/

UINT32 FillGeometryNudger::GetAttrNameID(void)
{
	return (_R(IDS_VALUE_CHANGE));
}

/********************************************************************************************

>	AttrFillGeometry* FillGeometryNudger::MutateFill(AttrFillGeometry* FillToMutate)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/94
	Inputs:		The fill that is to be changed.
	Purpose:	Nudges a fill geometry.

********************************************************************************************/

AttrFillGeometry* FillGeometryNudger::MutateFill(AttrFillGeometry* FillToMutate)
{
	if (!FillToMutate->IsVisible() ||
		FillToMutate->GetSelectionCount() == 0)
		return NULL;

	// Make a copy of this Fill to change
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	// Construct the translation matrix
	Trans2DMatrix TransMat(X_NudgeDistance, Y_NudgeDistance);
	
	BOOL isARampBlob = FALSE;

	// And apply it to the fill
	
	NewFill->TransformSelectedControlPoints(TransMat, &isARampBlob);

	if (LastNudgeType == FillToMutate->GetRuntimeClass())
	{
		BOOL IsSame = TRUE;

		if (FillToMutate->GetStartPoint() && 
			*FillToMutate->GetStartPoint() != LastNudgeStart)
			IsSame = FALSE;	

		if (FillToMutate->GetEndPoint() && 
			*FillToMutate->GetEndPoint() != LastNudgeEnd)
			IsSame = FALSE;	

		if (FillToMutate->GetEndPoint2() && 
			*FillToMutate->GetEndPoint2() != LastNudgeEnd2)
			IsSame = FALSE;	

		if (IsSame)
			return NewFill;
	}

	Node *pNode = FillToMutate;
	while ((pNode != NULL) && !pNode->IsSpread())
		pNode = pNode->FindParent();

	if (pNode == NULL)
		return NewFill;			// We're not really in the tree

	Spread* pSpread = (Spread*)pNode;

	DocRect OldBounds = FillToMutate->GetBlobBoundingRect();
	DocRect NewBounds = NewFill->GetBlobBoundingRect();

	DocRect Bounds = OldBounds.Union(NewBounds);

	RenderRegion* pRegion = DocView::RenderOnTop(&Bounds, pSpread, UnclippedEOR);
	while (pRegion)
	{
		if (!isARampBlob)
		{
			// MUST do a full redraw of the fill blobs
			FillToMutate->RenderFillBlobs(pRegion);
			NewFill->RenderFillBlobs(pRegion);
		}
		else
		{
			FillToMutate->DisableRampRedraw ();
			FillToMutate->RenderFillBlobs(pRegion);
			FillToMutate->EnableRampRedraw ();
			NewFill->DisableRampRedraw ();
			NewFill->RenderFillBlobs(pRegion);
			NewFill->EnableRampRedraw ();
		}

		// Get the Next render region
		pRegion = DocView::GetNextOnTop(&Bounds);
	}

	AttrFillGeometry::LastRenderedMesh = NULL;

	LastNudgeType 	= FillToMutate->GetRuntimeClass();

	if (FillToMutate->GetStartPoint())
		LastNudgeStart 	= *FillToMutate->GetStartPoint();

	if (FillToMutate->GetEndPoint())
		LastNudgeEnd 	= *FillToMutate->GetEndPoint();

	if (FillToMutate->GetEndPoint2())
		LastNudgeEnd2 	= *FillToMutate->GetEndPoint2();

	return NewFill; 
}

/********************************************************************************************

>	void FillGeometryNudger::SetNudgeDistance(INT32 xOffset, INT32 yOffset)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/94
	Purpose:	Set the distance to nudge.

********************************************************************************************/

void FillGeometryNudger::SetNudgeDistance(INT32 xOffset, INT32 yOffset)
{
	X_NudgeDistance = xOffset;
	Y_NudgeDistance = yOffset;
}
