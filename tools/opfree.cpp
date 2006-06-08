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
// The Freehand Tool operation

/*
*/


#include "camtypes.h"
#include "opfree.h"

//#include "resource.h"
//#include "mario.h"
//#include "rik.h"
//#include "viewrc.h"
#include "docview.h"
#include "paper.h"
#include "document.h"
//#include "mainfrm.h"
#include "ensure.h"
#include "csrstack.h"
#include "nodepath.h"
#include "paths.h"
#include "fitcurve.h"
#include "app.h"
#include "spread.h"
#include "freehand.h"
#include "pen.h"
#include "progress.h"
#include "attrmgr.h"
#include "blobs.h"
#include "pathedit.h"
#include "fixmem.h"
#include "keypress.h"
//#include "freehres.h"
#include "objchge.h"
#include "bubbleid.h"

#include "lineattr.h"		// Variable-width Path stroking headers
#include "pathtrap.h"
#include "ppstroke.h"
#include "pressure.h"
#include "strkattr.h"
#include "valfunc.h"
#include "nodeblnd.h"
#include "blndtool.h"
#include "ndbldpth.h"
#include "nodebldr.h"
#include "opdrbrsh.h"
#include "ophist.h"
#include "brshattr.h"

// Set things up so that the tool will be listed in the Dialog box
DECLARE_SOURCE("$Revision$");

// Declare the class
CC_IMPLEMENT_DYNCREATE( OpFreeHand, SelOperation )

// Improved memory tracking
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	OpFreeHand::OpFreeHand()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Purpose:	Constructor. This simply sets a few of the operation flags.

********************************************************************************************/

OpFreeHand::OpFreeHand()
{
	// Set all our cursors to NULL to start with
	pFreeHandCursor = NULL;
	pJoinCursor = NULL;
	pStraightCursor = NULL;
	pRubOutCursor = NULL;
	pModifyCursor = NULL;
	
	// Set other default values
	TrackData = NULL;
	StartSpread = NULL;
	PreviousSpread = NULL;
	Smoothness = 512;
	LineSegmentCount = 0;
	CanLineJoin = FALSE;
	IsStraightLineMode = FALSE;
	AddPressureToPath = FALSE;
	FreeHandPressure = 0;

	// The paths that we are joined to, or NULL if we are joined to none
	pJoinInfo = NULL;
	StartPath = NULL;
	EndPath = NULL;
	CloseTo = 0;
	Mu = 0.0;
	IsEndNearEndpoint = FALSE;
	CurrentCursorID = 0;

	m_pNewNodePath = NULL;

}



/********************************************************************************************

>	void OpFreeHand::DoDrag( DocCoord Anchor, Spread *pSpread, INT32 Smooth, 
								NodePath* path, Path* ToolPath )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		Anchor 	- The position of the mouse at the start of the Drag
				pSpread - The spread that the drag started on
				Smooth	- Closeness of fit value
				pFreeHandInfo - Pointer to the info about the Joining set up by the
				freehand tool
				ToolPath - The path to store the mouse tracking info in
	Purpose:	Starts up the drag operation by storing all start positions and setting
				up a Path to store the mouse movement in

********************************************************************************************/

void OpFreeHand::DoDrag(DocCoord Anchor, Spread *pSpread, INT32 Smooth,
						FreeHandJoinInfo* pFreeHandInfo, Path* ToolPath)
{
	TRACEUSER( "Diccon", _T("Do Drag Free, StartPos = %d, %d\n"), Anchor.x, Anchor.y);
// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	// Tell the pressure pen that we're starting a new stroke
	CCPen *pPen = GetApplication()->GetPressurePen();
	if (pPen != NULL)
		pPen->StartStroke();
#endif // VECTOR_STROKING

	// Snap the starting coord if we are not joining to something else
	if ((pFreeHandInfo==NULL) || (pFreeHandInfo->pJoinPath==NULL))
		DocView::SnapCurrent(pSpread, &Anchor, FALSE, TRUE);

	// Make a note of various starting conditions
	TrackData 	= ToolPath;
	Smoothness 	= Smooth;
	pJoinInfo	= pFreeHandInfo;
	StartPath	= pJoinInfo->pJoinPath;
	EndPath		= NULL;

	// Make a mental note of the start point
	StartPoint    = Anchor;
	StartSpread   = pSpread;
	PreviousSpread= pSpread;
	PreviousPoint = Anchor;
	LineSegmentCount = 0;
	IsStraightLineMode = FALSE;
	CanLineJoin = FALSE;

	// Prepare the Track data path and add in the initial click point to the path
	if (!PrepareTrackDataPath())
	{
		// We failed to get the memory to initialise the track data
		InformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
		FailAndExecute();
		End();
		return;
	}
		
	// Create some cursors that I might need
	if (!LoadCursors())
	{
		// The cursors did not load, so fail?
		FailAndExecute();
		End();
		return;
	}

	// Push my new cursor onto the stack
	CurrentCursorID = CursorStack::GPush(pFreeHandCursor, TRUE);
	MyCurrentCursor = pFreeHandCursor;

	// And tell the Dragging system that we need drags to happen
	StartDrag( DRAGTYPE_NOSCROLL );
}



/********************************************************************************************

>	BOOL OpFreeHand::PrepareTrackDataPath()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Tries to initialise the path that we will store the track data in. It also
				sets up the pressure info stuff it there is pressure info coming in.
				If the path initialise ok, then it inserts the initial position (including
				its pressure) into the path. This function will only fail if there
				is no memory to init the path with.

********************************************************************************************/

BOOL OpFreeHand::PrepareTrackDataPath()
{
	TrackData->ClearPath();
	TrackData->FindStartOfPath();

	// Check to see if we want to store pressure, and if so initialise the path.
	AddPressureToPath = FALSE;

// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	
	CCPen *pPen = GetApplication()->GetPressurePen();
	if (IsBrushOp() && pPen != NULL && pPen->GetPressureMode() != PressureMode_None)
	{
		// Yep, we want the pressure info
		AddPressureToPath = TRUE;

		// Lets gets the pressure for the first point in the path.
		FreeHandPressure = pPen->GetPenPressure();

		// try and initialise the extra info in the path and fail if not
		if (!TrackData->InitExtraInfo(CI_PRESSURE))
			return FALSE;
	}
	
#endif // VECTOR_STROKING

	// insert the initial coord into the path and fail if it fails
	if (!TrackData->InsertMoveTo(StartPoint))
		return FALSE;

	// and add pressure info if needed
	if (AddPressureToPath)
 		TrackData->AddExtraInfo(CI_PRESSURE, FreeHandPressure);

	// and tell someone that we did it
	return TRUE;
}


/********************************************************************************************

>	BOOL OpFreeHand::LoadCursors()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/94
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Tries to Load all the cursors used by the Free Hand Operation.

********************************************************************************************/

BOOL OpFreeHand::LoadCursors()
{
	// Try to load all the cursors
	if (FreeHandTool::FreehandPtrCrosshair)
	{
		pFreeHandCursor = new Cursor(TOOLID_FREEHAND, _R(IDC_FREEHANDTOOLCURSOR_X));
		pJoinCursor = new Cursor(TOOLID_FREEHAND, _R(IDC_FREEHANDJOINCURSOR_X));
		pRubOutCursor = new Cursor(TOOLID_FREEHAND, _R(IDC_FREEHANDRUBOUTCUR_X));
		pStraightCursor = new Cursor(TOOLID_FREEHAND, _R(IDC_FREEHANDLINECURSOR_X));
		pModifyCursor = new Cursor(TOOLID_FREEHAND, _R(IDC_FREEHANDMODIFY_X));
	}
	else
	{
		pFreeHandCursor = new Cursor(TOOLID_FREEHAND, _R(IDC_FREEHANDTOOLCURSOR));
		pJoinCursor = new Cursor(TOOLID_FREEHAND, _R(IDC_FREEHANDJOINCURSOR));
		pRubOutCursor = new Cursor(TOOLID_FREEHAND, _R(IDC_FREEHANDRUBOUTCUR));
		pStraightCursor = new Cursor(TOOLID_FREEHAND, _R(IDC_FREEHANDLINECURSOR));
		pModifyCursor = new Cursor(TOOLID_FREEHAND, _R(IDC_FREEHANDMODIFY));
	}

	// See if any of them failed
	if ((pRubOutCursor==NULL)	|| (!pRubOutCursor->IsValid()) ||
		(pJoinCursor==NULL)   	|| (!pJoinCursor->IsValid()) ||
		(pFreeHandCursor==NULL)	|| (!pFreeHandCursor->IsValid()) ||
		(pStraightCursor==NULL)	|| (!pStraightCursor->IsValid()) ||
		(pModifyCursor==NULL)   || (!pModifyCursor->IsValid()))
	{
		// They did, so Tidy up and exit
		delete pFreeHandCursor;
		delete pJoinCursor;
		delete pStraightCursor;
		delete pRubOutCursor;
		delete pModifyCursor;

		// Set them all to NULL
		pFreeHandCursor = NULL;
		pJoinCursor = NULL;
		pStraightCursor = NULL;
		pRubOutCursor = NULL;
		pModifyCursor = NULL;

		// fail
		return FALSE;
	}

	// It worked
	return TRUE;
}



/********************************************************************************************

>	void OpFreeHand::RemoveCursors()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Purpose:	Gets rid of all this operations cursors. It takes the cursor I was using
				off the top of the stack and deletes all the cursors I had allocated.

********************************************************************************************/

void OpFreeHand::RemoveCursors()
{
	// Get rid out my cursor from the top of the cursor stack
	CursorStack::GPop(CurrentCursorID);
	MyCurrentCursor = NULL;
	CurrentCursorID = 0;

	// destroy all the cursors that I had allocated
	delete pFreeHandCursor;
	delete pJoinCursor;
	delete pRubOutCursor;
	delete pStraightCursor;
	delete pModifyCursor;

	// and make sure that they all point to something sensible
	pFreeHandCursor = NULL;
	pJoinCursor = NULL;
	pStraightCursor = NULL;
	pRubOutCursor = NULL;
	pModifyCursor = NULL;
}





/********************************************************************************************

>	void OpFreeHand::SetCursorOnMove(ClickModifiers ClickMods, Spread* pSpread, DocCoord* PointerPos)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		ClickMods - Which modifiers were pressed
				pSpread - The spread that the mouse is over
	Outputs:	PointerPos - The position of the mouse
	Purpose:	Decides which cursor to display depending on which modifiers are pressed and
				where the mouse is. PointerPos can be changed in this function

********************************************************************************************/

void OpFreeHand::SetCursorOnMove(ClickModifiers ClickMods, Spread* pSpread, DocCoord* PointerPos)
{
	// If the mouse is over an EndPoint, use the Join Cursor
	if (CheckMouseOverSelectedPath(PointerPos, pSpread, &IsEndNearEndpoint))
	{
		// Ok the cursor is near the path
		if (IsEndNearEndpoint)
		{
			// it was near one of the paths open ends
			SetCursorAndStatus(JOIN_CURSOR);
			return;
		}

		// Must be near the middle of the path
		// We can only do a modify join if we started the path on the same path
		if ((StartPath==EndPath) && (pJoinInfo->IsNearEndPoint==FALSE))
		{
			SetCursorAndStatus(MODIFY_CURSOR);
			return;
		}
	}

	// See Which of the modifiers are down...	
	if (ClickMods.Alternative1)
	{
		// if Alt is down we are in Straight Line mode
		SetCursorAndStatus(STRAIGHTLINE_CURSOR);
	}
	else
	{
		if (ClickMods.Adjust)
		{
			// If Shift is down we are in RubOut mode
			SetCursorAndStatus(RUBOUT_CURSOR);
		}
		else
		{
			// No Modifiers so use the normal cursor
			SetCursorAndStatus(NORMAL_CURSOR);
		}
	}
}



/********************************************************************************************

>	void OpFreeHand::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								  Spread *pSpread, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
				pSpread - The spread that the mouse is moving over
	Purpose:	Handles the event of the mouse moving during a drag
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpFreeHand::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								  Spread *pSpread, BOOL bSolidDrag)
{
	// Find out the view
	DocView* pView = DocView::GetSelected();

	// Lets whip out the current pressure value
	if (AddPressureToPath)
		FreeHandPressure = ClickMods.Pressure;

	// If drag has moved onto a different spread, convert the coord to be relative to the
	// original spread.
	if (pSpread != StartSpread)
		PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

	// Change the Cursor to display the appropriate thing.
	SetCursorOnMove(ClickMods, StartSpread, &PointerPos);

	// Are we in Straight Line Mode? We won't allow straight line mode when recording pressure
	if (ClickMods.Alternative1)
	{
		// Snap and Constrain if needed
		DocView::SnapCurrent(pSpread, &PointerPos);
		if (ClickMods.Constrain)
			DocView::ConstrainToAngle(PreviousPoint, &PointerPos);

		// If we are in the Straight Line mode rub out the line
		if (IsStraightLineMode)
			RenderEorStraightLine(NULL, pSpread);
		else
		{
			// We are just entering straight line mode, so switch the dragging mode
			// so that the document auto scrolls at the edges of the screen
			if (pView!=NULL)
				pView->ChangeDragType(DRAGTYPE_AUTOSCROLL);
		}

		// Update the mouse position and render the EORed straight line
		StraightLinePos = PointerPos;
		RenderEorStraightLine(NULL, pSpread);
		
		// Finally note that we are in this mode
		IsStraightLineMode = TRUE;
	}
	else
	{
		// See if we were in Straight Line mode before
		if (IsStraightLineMode)
		{
			// We do not want to rub the straight line off, so do no drawing
			// Add the straight line into the path and set the flags as needed
			AddStraightLine();
			PreviousPoint = StraightLinePos;

			// We are just leaving straight line mode, so go back to not scrolling
			if (pView!=NULL)
				pView->ChangeDragType(DRAGTYPE_NOSCROLL);

			// No longer in Straight Line mode, so set the flag
			IsStraightLineMode = FALSE;
		}
		else
		{
			// Either rub out sections of the path, or add new points to it
			if (ClickMods.Adjust)
				RubOutPath(PointerPos, StartSpread);
			else
				AddPointsToPath(PointerPos, StartSpread);
		}
	}

	// Set the last spread to something appropriate
	PreviousSpread = pSpread;
}





/********************************************************************************************

>	void OpFreeHand::DragPointerIdle(DocCoord PointerPos, ClickModifiers ClickMods,
									 Spread* pSpread, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/94
	Inputs:		PointerPos - The position of the mouse
				ClickMods - The modifiers that are held down at the time of the idle event
				pSpread - The spread that the mouse was over at the event
	Purpose:	This function checks the Adjust Click modifier while the mouse is not
				moving to see if it is released during a straight line mode edit. This
				means that the shift key can be toggled in mid drag to create a straight
				line shape

********************************************************************************************/

void OpFreeHand::DragPointerIdle(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag)
{
	// If we are in straight line mode, but the alt button has just come up, then
	// end the straight line we are drawing
	if (IsStraightLineMode && !ClickMods.Alternative1)
	{
		// Add the straight line into the path and set the flags as needed
		AddStraightLine();
		PreviousPoint = StraightLinePos;

		// No longer in Straight Line mode, so set the flag
		IsStraightLineMode = FALSE;
	}
}




/********************************************************************************************

>	void OpFreeHand::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
							   Spread *pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				pSpread - The spread that the drag finished on
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	Handles the drag finishing by rubbing out an EOR on the screen and
				adding the path to the tree and building all the undo that will be
				needed
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpFreeHand::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
							   Spread *pSpread, BOOL Success, BOOL bSolidDrag)
{
	// Erase the whole EORed line
	DocRect ClipRect(0,0,0,0);
	RenderDragBlobs(ClipRect, StartSpread, bSolidDrag);

	// Get rid of all the cursors
	RemoveCursors();

	// Put the hour glass up as we have to and end the drag
	BeginSlowJob();
	EndDrag();

	// Add the new path to the tree if it was valid
	BOOL Worked = FALSE;
	if (Success)
	{
		// were we in the middle of drawing a straight line
		if (IsStraightLineMode)
		{
			// we were so add it in to the track data
			AddStraightLine();
			PreviousPoint = StraightLinePos;
			IsStraightLineMode = FALSE;
		}

		// try and smooth the path and insert it into the tree
		Worked = CompleteOperation();

		// DY 9/99 Check to see if we are editing a blend on a curve, if so we may wish to 
		// change the number of steps in the blend (making use of the path distance).
		NodeGroup* pParent = GetParentOfNodePath();           
		if (pParent!= NULL)
		{
			if (pParent->IS_KIND_OF(NodeBlend))
			{
				InsertChangeBlendStepsAction((NodeBlend*)pParent);               

				ObjChangeFlags cFlags(FALSE,TRUE);
				ObjChangeParam ObjChange(OBJCHANGE_FINISHED,cFlags,NULL,this);
				Worked = UpdateChangedNodes(&ObjChange);
			}
		
		}
	}
// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	// Inform the pressure pen that we've finished the stroke
	CCPen *pPen = GetApplication()->GetPressurePen();
	if (pPen != NULL)
		pPen->EndStroke();
#endif // VECTOR_STROKING



	// If the operation failed, then tidy up
	if (Worked==FALSE)
		FailAndExecute();
	else	
	{
		// Update all the parents of the effected paths.
		ObjChangeFlags cFlags;
		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
		ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
		UpdateChangedNodes(&ObjChange);
	}
	// End the operation properly
	End();
}



/********************************************************************************************

>	BOOL OpFreeHand::DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		pKeyPress - pointer to a keypress object
	Returns:	TRUE if it handled the keypress, FALSE otherwise
	Purpose:	Find out if any of the key modifiers are being pressed when the mouse is
				not moving.

********************************************************************************************/

BOOL OpFreeHand::DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag)
{
	// Need something to see if there was a usful keypress
//	BOOL IsKeyPress = FALSE;

	// and something to keep them in
	ClickModifiers ClickMods;
	ClickMods.Adjust = FALSE;
	ClickMods.Menu = FALSE;
	ClickMods.Constrain = FALSE;
	ClickMods.Alternative1 = FALSE;
	ClickMods.Alternative2 = FALSE;
	ClickMods.Pressure = 0;

	// See if was one of the key we are interested in
	if (pKeyPress->IsConstrain() || pKeyPress->IsAdjust() || pKeyPress->IsAlternative() || pKeyPress->IsRelease())
	{
		// Set up the click modifier
		ClickMods.Constrain = pKeyPress->IsConstrainPressed();
		ClickMods.Adjust = pKeyPress->IsAdjustPressed();
		ClickMods.Alternative1 = pKeyPress->IsAlternativePressed();

		// fake a mouse move message
		DragPointerMove(PreviousPoint, ClickMods, PreviousSpread, FALSE);

		// Tell the keypress people that we used the keypress
		return TRUE;
	}

	// Did not use the click
	return FALSE;
}




/********************************************************************************************

>	BOOL OpFreeHand::CompleteOperation()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Does all the steps required to build a new path, smooth it, try to join it
				with others and finally insert it into the tree.

********************************************************************************************/

BOOL OpFreeHand::CompleteOperation()
{
	// Create a new path to be inserted into the tree
	NodePath* pNewNodePath = NULL;
	if (!CreateNewPath(&pNewNodePath))
		return FALSE;

	if (!SmoothNewCurve(pNewNodePath))
	{
		// Get rid of the path we created and fail
		delete pNewNodePath;
		return FALSE;
	}

	if (!InsertSmoothCurveWithUndo(pNewNodePath))
	{
		// Get rid of the path we created, and fail
		pNewNodePath->CascadeDelete();
		delete pNewNodePath;
		return FALSE;
	}

	// Mark the selection as no longer valid (the Bounding Box will have changed)
	SelRange* Selection = GetApplication()->Selection;
	if (Selection)
		Selection->Update();

	
	// All worked
	return TRUE;
}



/********************************************************************************************

>	BOOL OpFreeHand::CreateNewPath(NodePath** pNewPath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Outputs:	pNewPath - This will be a pointer to an initialised NodePath or NULL
	Returns:	TRUE if it worked, FALSE if something went wrong
	Purpose:	Builds a new Node Path ready to have a Curve fitted into it. If anything
				goes wrong (not enough memory, the path was not worth fitting etc) then
				this function fails.

********************************************************************************************/

BOOL OpFreeHand::CreateNewPath(NodePath** pNewPath)
{
	// Get a pointer to the path and set it to NULL
	NodePath*& pFreeHandPath = *pNewPath;
	pFreeHandPath = NULL;

	// Only bother if they had actually drawn something
	if (LineSegmentCount==0)
		return FALSE;

	// See if the path ever got away from the initial snap coord
	DocCoord* Coords = TrackData->GetCoordArray();
	INT32 NumCoords = TrackData->GetNumCoords();
	BOOL IsAllSame = TRUE;

	// loop through all the coords comparing them with the first one
	for (INT32 i=0; (i<NumCoords) && (IsAllSame); i++)
	{
		// if this coord is different from the first one, then we are ok
		if (Coords[0] != Coords[i])
			IsAllSame = FALSE;
	}

	// If it did not (ie all the coordinates were the same, then chuck it away)
	if (IsAllSame)
		return FALSE;

	// Init the path with the default memory block sizes
	pFreeHandPath = new NodePath;
	if (pFreeHandPath==NULL)
		return FALSE;

	// Ok, we got a new NodePath, try and init the Path inside it
	if (!pFreeHandPath->SetUpPath(24, 12))
	{
		// The path failed to init, so get rid of it and fail
		delete pFreeHandPath;
		pFreeHandPath = NULL;
		return FALSE;
	}

	// Must have worked
	return TRUE;
}



/********************************************************************************************

>	BOOL OpFreeHand::SmoothNewCurve(NodePath* pNewNodePath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		pNewNodePath - The NodePath that we want to smooth the track data into
	Returns:	TRUE if it worked, FALSE if something went wrong
	Purpose:	Sets up the curve fitter and finally smooths the data in the Track data
				buffer into the new node path.

********************************************************************************************/

BOOL OpFreeHand::SmoothNewCurve(NodePath* pNewNodePath)
{
	// To work out how much we need to smooth the curve by, we must know what
	// zoom factor we are currently at
	DocView* pDocView = DocView::GetSelected();
	if (pDocView==NULL)
		return FALSE;

	// When this path is built it will actually replace a section from the middle of the
	// original path, so we must make sure that is starts and ends on the original
	// The start was done before the drag started, the end can be done now.
	if ((StartPath!=NULL) && (StartPath==EndPath) && (pJoinInfo->IsNearEndPoint==FALSE))
	{
		// Find out the coord on the original path where the endpoint got closest to
		PreviousPoint = EndPath->InkPath.ClosestPointTo(Mu, CloseTo);

		// Add it to the path data
		if (TrackData->InsertLineTo(PreviousPoint))
		{
			// and add pressure info if needed
			if (AddPressureToPath)
		 		TrackData->AddExtraInfo(CI_PRESSURE, FreeHandPressure);
		}
	}

	// Find out how flat they need the curve. This number represents
	// how far off the original track data the curve can get before it
	// is split and turned into more curves. The smaller the number
	// the tighter the fit.
	double ScaleFactor = (pDocView->GetViewScale()).MakeDouble();
	double ErrorLevel = (64 + (160*Smoothness)) / ScaleFactor;
	ErrorLevel = ErrorLevel * ErrorLevel;
	
	// FitCurve needs the number of points in the path, which is the 
	// number of Line Segments + 1
	CurveFitObject CurveFitter(&(pNewNodePath->InkPath), ErrorLevel);

	// Try to initialise the curve fitter
	if (!CurveFitter.Initialise(TrackData, LineSegmentCount+1))
		return FALSE;

	// All went ok, so fit the curve to the track data and return success
	CurveFitter.FitCurve();
	return TRUE;
}




/********************************************************************************************

>	BOOL OpFreeHand::InsertSmoothCurveWithUndo(NodePath* pNewNodePath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		pNewNodePath - The NodePath that we are trying to insert into the tree
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Starts the Selection operation (recording the selection state etc) and
				trys to join our new path with any others touching it, before inserting
				it into the tree.

********************************************************************************************/

BOOL OpFreeHand::InsertSmoothCurveWithUndo(NodePath* pNewNodePath)
{
	// Start the selection operation.
	// We don't want to save the end selection status because it will be
	// restored automatically when the operation is redone
	if (!DoStartSelOp(FALSE))
		return FALSE;

	// Now we have to go round testing if we are joined to any other paths
	// and building the appropriate undo info
	if ((StartPath!=NULL) && (StartPath==EndPath) &&
		(pJoinInfo->IsNearEndPoint==FALSE) && (IsEndNearEndpoint==FALSE))
	{
		// This is a case for replacing a section of an existing path
		// with the new smoothed path
		if (!TryToReplaceSection(pNewNodePath))
			return FALSE;
	}
	else
	{
		// If the new curve did not start and end on the same path we made need to make a few adjustments
		if (pJoinInfo->IsNearEndPoint==FALSE)
			StartPath = NULL;

		// this is a case for inserting the path in the normal way
		// Joining it to other paths that it touches etc
		if (!TryToJoinNewPathWithOthers(pNewNodePath))
			return FALSE;
	}

	// Clear the Node path that we made, as it will have been copied into the tree by now
	pNewNodePath->CascadeDelete();
	delete pNewNodePath;

	// It all worked ok
	return TRUE;
}



/********************************************************************************************

>	void OpFreeHand::RubOutPath(DocCoord Pos, Spread* pSpread)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/94
	Inputs:		Pos - The position of the mouse
				pSpread - Pointer to the spread that we are drawing on
	Purpose:	Tries to rub out sections of the path that have already been drawn.

********************************************************************************************/

void OpFreeHand::RubOutPath(DocCoord Pos, Spread* pSpread)
{
	// If the mouse has not moved then do not do anything
	if (Pos==PreviousPoint)
		return;

	// Get hold of the blob manager for finding out the size of a blob
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr==NULL)
		return;

	// Find out about all the coords in the path already
	INT32 NumCoords = TrackData->GetNumCoords();
	DocCoord* Coords = TrackData->GetCoordArray();

	// Get line width information
//	INT32 LineWidth = GetCurrentLineWidth();

	// Find out about the area round the mouse position to consider for rubbing out
	DocRect BlobRect;
	pBlobMgr->GetBlobRect(Pos, &BlobRect);

	// We want to look at the last few coords in the path. So work out how far back to look
	INT32 StopLookingAt = 1;

	// Loop through the last few coords to see if the mouse is near them
	for (INT32 i=NumCoords-2; i>StopLookingAt; i--)
	{
		if (BlobRect.ContainsCoord(Coords[i]))
		{
			// Rub out the line from this coord to the end of the line
			RenderRegion* pRegion = DocView::RenderOnTop( NULL, pSpread, UnclippedEOR );
			while (pRegion)
			{
				for (INT32 n = i+1; n < NumCoords; n++)
					RenderLine(pRegion, TrackData, n);

				// Get the Next render region
				pRegion = DocView::GetNextOnTop( NULL );
			}

			// Rub out the actual path Data
			if (!TrackData->DeleteFromElement(i+1))
			{
				InformError(_R(IDS_PATH_DELETE_ERROR));
				return;
			}

			// Change the operations parameters
			PreviousPoint = Coords[i];
			LineSegmentCount = i;

			// this mouse message is finished with
			return;
		}
	}
}



/********************************************************************************************

>	void OpFreeHand::AddPointsToPath(DocCoord Pos, Spread* pSpread)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/94
	Inputs:		Pos - The position of the mouse
				pSpread - Pointer to the spread that we are drawing on
	Purpose:	Adds a new point to the path of mouse moves. When the drag
				finishes the mouse moves will be smoothed

********************************************************************************************/

void OpFreeHand::AddPointsToPath(DocCoord PointerPos, Spread* pSpread)
{
	// If this coord is the same as the last one, then do not bother adding to the track data
	if (PreviousPoint == PointerPos)
		return;
 
	// Insert the move
	if (TrackData->InsertLineTo(PointerPos))
	{
		// and add pressure info if needed
		if (AddPressureToPath)
	 		TrackData->AddExtraInfo(CI_PRESSURE, FreeHandPressure);
	}
	else
	{
		// Oh no, we ran out of mem in the middle of making a new path
		// Tidy up and report back to HQ
		EndDrag();

		// Tell the World that it all went horribly wrong
		InformError();

		// Remove it from the tree and delete it
		TrackData->ClearPath();
		
		// End this operation and return
		FailAndExecute();
		End();
		return;
	}
	
	// Now we have to render a new bit of EORed data to the screen
	RenderLine(NULL, pSpread, TrackData, TrackData->GetNumCoords()-1);

	// Make a note of the coord, inc the line count and mark joining to the start as active.
	PreviousPoint = PointerPos;
	LineSegmentCount++;
	CanLineJoin = TRUE;
}


/********************************************************************************************

>	void OpFreeHand::AddStraightLine()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/94
	Purpose:	Adds a straight line to the Path, that has been made by the Straight Line
				Mode. This means it has to set some flags in the path to stop the curve
				fitter from smoothing this straight line segment out

********************************************************************************************/

void OpFreeHand::AddStraightLine()
{
	PathFlags Flags;
	Flags.Spare1 = TRUE;
	
	// If this coord is the same as the last one, then do not bother adding to the track data
	if (PreviousPoint == StraightLinePos)
		return;

	// add the line segment into the path
	if (TrackData->InsertLineTo(StraightLinePos, &Flags))
	{
		// and add pressure info if needed
		if (AddPressureToPath)
	 		TrackData->AddExtraInfo(CI_PRESSURE, FreeHandPressure);
	}
	else
	{
		// Oh no, we ran out of mem in the middle of making a new path
		// Tidy up and report back to HQ
		EndDrag();

		// Tell the World that it all went horribly wrong
		InformError();

		// Remove it from the tree and delete it
		TrackData->ClearPath();
		
		// End this operation and return
		FailAndExecute();
		End();
		return;
	}

	// Count this bit of line
	LineSegmentCount++;
	CanLineJoin = TRUE;
}


/********************************************************************************************

>	BOOL OpFreeHand::CheckMouseOverSelectedPath(DocCoord* PointerPos, Spread* pSpread,
												BOOL* IsNearEndpoint)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/94
	Inputs:		PointerPos - The position of the mouse
				pSpread - the spread that the cursor is over
	Outputs:	IsNearEndPoint - if this function returns TRUE this param is valid, otherwise
				it is not. It will be TRUE if the cursor was found to be close to an
				open end of the path, or FALSE if it was near the middle of the path
	Returns:	TRUE if the pointer was over the end of a selected line, FALSE if it was not
	Purpose:	Checks to see if the mouse is over the endpoint of any of the
				selected paths in the document and if it is, it makes a note of
				the path and which end it was attached to

********************************************************************************************/

BOOL OpFreeHand::CheckMouseOverSelectedPath(DocCoord* PointerPos, Spread* pSpread, BOOL* IsNearEndpoint)
{
	// Assume we do not find a path
	EndPath = NULL;

	// Find out about the blob manager so we can find the size of a blob
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr==NULL)
		return FALSE;

	// Find out about the size of a blob
	DocRect BlobRect;
	pBlobMgr->GetBlobRect(*PointerPos, &BlobRect);

	// Find the selected range of objects
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	// see if the selection is on the same spread as the drag
	if (pNode!=NULL)
	{
		// Get the spread and return if it is different from the one with the cursor
		Spread* pNodeSpread = pNode->FindParentSpread();
		if (pNodeSpread!=pSpread)
			return FALSE;		
	}

	// Walk through the selection
	while (pNode!=NULL)
	{
		if (pNode->IsNodePath())
		{
			// Now we know it's a NodePath, get a pointer to the Path object within it, so
			// we can find any endpoints
			Path* ThisPath = &(((NodePath*)pNode)->InkPath);
			
			// See if it touches any point in this path
			INT32 CloseSlot = 0;
			if (ThisPath->IsNearOpenEnd(BlobRect, &CloseSlot))
			{
				// Yep, better work out where and set the path where is all happens
				ThisPath->SetPathPosition(CloseSlot);
				*PointerPos = ThisPath->GetCoord();
				*IsNearEndpoint = TRUE;
				EndPath = (NodePath*)pNode;
				return TRUE;
			}

			// See if it is near the middle of the path
			// This is only relavent if this node is the same as the one we started on
			// and we started in the middle of it
			if ((StartPath!=NULL) && (StartPath==pNode) && (pJoinInfo->IsNearEndPoint==FALSE))
			{
				INT32 Range = BlobRect.Width() / 2;
				if (ThisPath->IsPointCloseTo(*PointerPos, Range*Range, &CloseTo, &Mu))
				{
					// OK, we are near the path we started on
					// see if we are in the same sub-path as we started on
					if (AreSlotsInSameSubPath(CloseTo, pJoinInfo->CloseSlot, ThisPath))
					{
						// Yep, better work out where and set the path where is all happens
						*IsNearEndpoint = FALSE;
						EndPath = (NodePath*)pNode;
						return TRUE;
					}
				}
			}
    	}	

		// Now find the next selected node
		pNode = Selected->FindNext(pNode);
	}

	// we will see if this point is anywhere near the start point
	if ((BlobRect.ContainsCoord(StartPoint)) && (StartPath==NULL))
	{
		// Note the path that we are starting at and modify the coord
		// This will make the start and end points the same, causing the path to be closed
		*PointerPos = StartPoint;

		// Wait till the cursor has moved out of the original snap range before we start joining
		if (CanLineJoin==TRUE)
		{
			*IsNearEndpoint = TRUE;
			return TRUE;
		}
	}

	// Did not find a join
	return FALSE;
}



/********************************************************************************************

>	BOOL OpFreeHand::AreSlotsInSameSubPath(INT32 Slot1, INT32 Slot2, Path* pPath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/95
	Inputs:		Slot1, Slot2 - The slot numbers of the 2 points in question.
				pPath - the path that the slots live in
	Returns:	TRUE if both these slots live in the same sub-path of pPath,
				FALSE if they are in different sub-paths, or there was an error.
	Purpose:	This function is used when editing lines with the freehand tool. If the
				drag was started in the middle of a path and the cursor is over that path
				again, we need to know if we can complete the edit. This is only possible if
				the start and end points are both on the same sub-path in the path.
				This function is used to determine if the 2 points are indeed in the same
				sub-path to allow the tool to display correct cursor etc.

********************************************************************************************/

BOOL OpFreeHand::AreSlotsInSameSubPath(INT32 Slot1, INT32 Slot2, Path* pPath)
{
	// saftey check
	ERROR2IF(pPath==NULL, FALSE, "NULL pointer passed to OpFreeHand::AreSlotsInSameSubPath");

	// We need to look at the verb array in the path.
	PathVerb* pVerbs = pPath->GetVerbArray();
	INT32 NumSlots = pPath->GetNumCoords();

	// find the largest and smallest slot numbers
	INT32 First = Slot1<Slot2 ? Slot1 : Slot2;
	INT32 Last = Slot1<Slot2 ? Slot2 : Slot1;

	// Make sure that the slot numbers make sense
	ERROR2IF(Last>NumSlots, FALSE, "Bad Slot numbers in OpFreeHand::AreSlotsInSameSubPath");

	// loop from the first to the last slot looking for MoveTos
	while (Last>First)
	{
		// See if the current last slot is a MoveTo
		if (pVerbs[Last]==PT_MOVETO)
			return FALSE;

		// move the last point back
		Last--;
	}

	// We did not find any MoveTos between the points
	return TRUE;	
}



/********************************************************************************************

>	void OpFreeHand::SetCursorAndStatus(CursorType CurType)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/94
	Inputs:		CurType - The type of cursor to change to
	Purpose:	Chanages the cursor to the type requested and sets the status bar message
				to the appropriate message for the mode we are in.

********************************************************************************************/

void OpFreeHand::SetCursorAndStatus(CursorType CurType)
{
	// Now, if FlipCursor = TRUE, we flip it!
	Cursor* WhichCursor = NULL;
	String_256 StatusMsg("");

	switch (CurType)
	{
		case NORMAL_CURSOR:
			WhichCursor = pFreeHandCursor;
			StatusMsg.Load(_R(IDS_FREEHANDDRAG));
			break;

		case JOIN_CURSOR:
			WhichCursor = pJoinCursor;
			StatusMsg.Load(_R(IDS_FREEHANDDRAGJOIN));
			break;

		case STRAIGHTLINE_CURSOR:
			WhichCursor = pStraightCursor;
			StatusMsg.Load(_R(IDS_FREEHANDSTRAIGHT));
			break;

		case RUBOUT_CURSOR:
			WhichCursor = pRubOutCursor;
			StatusMsg.Load(_R(IDS_FREEHANDRUBOUT));
			break;

		case MODIFY_CURSOR:
			WhichCursor = pModifyCursor;
			StatusMsg.Load(_R(IDS_FREEHANDMODIFY));
			break;

		default:
			return;
	}

	// Change Status bar message and the cursor
	GetApplication()->UpdateStatusBarText(&StatusMsg);
	if (WhichCursor != MyCurrentCursor)
	{
		// set this cursor as the current cursor and immediately display it
		CursorStack::GSetTop(WhichCursor, CurrentCursorID);

		// remember this is our current cursor
		MyCurrentCursor = WhichCursor;
	}
}


/********************************************************************************************

>	BOOL OpFreeHand::TryToJoinNewPathWithOthers(NodePath* FreePath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/94
	Inputs:		FreePath - The new path that we will try to join with other paths
	Returns:	TRUE if it worked, FALSE if it did not
	Purpose:	Tries to Join the new path with any of the existing paths

********************************************************************************************/

BOOL OpFreeHand::TryToJoinNewPathWithOthers(NodePath* FreePath)
{
	ObjChangeFlags cFlagsDelete(TRUE);
	ObjChangeFlags cFlagsReplace(FALSE,TRUE);
	ObjChangeFlags cFlagsChanging;
	ObjChangeParam ObjChange;
	
	if (StartPath != NULL && EndPath != NULL && StartPath != EndPath)
	{
		// Cope with the case when the two separate paths are being joined together with this freehand line.
		// In this case, the EndPath is hidden (effectively deleted), and the StartPath is replaced in the tree
		// by hiding it and inserting the resultant NodePath in its place

		// Is it ok to delete the end path?
		ObjChange.Define(OBJCHANGE_STARTING,cFlagsDelete,EndPath,this);
		if (!EndPath->AllowOp(&ObjChange))
			return TRUE;	// Pretend that we've done it.

		// Is it ok to replace the start path?
		ObjChange.Define(OBJCHANGE_STARTING,cFlagsReplace,StartPath,this);
		if (!StartPath->AllowOp(&ObjChange))
			return TRUE;	// Pretend that we've done it.
	}
	else if (StartPath != NULL || EndPath != NULL)
	{
		// Some joining will take place with either in StartPath or EndPath

		// Either the StartPath or the EndPath will be hidden and replaced by another NodePath
		ObjChange.Define(OBJCHANGE_STARTING,cFlagsChanging,StartPath,this);

		// If we have a start path, the op will happen to the start path
		if (StartPath != NULL && !StartPath->AllowOp(&ObjChange))
			return TRUE;	// Pretend that we've done it.

		// If we DONT have a start path, the op will happen to the end path
		if (StartPath == NULL && !EndPath->AllowOp(&ObjChange))
			return TRUE;	// Pretend that we've done it.
	}

	if (StartPath == NULL)
	{
		if (EndPath == NULL)
		{
		 	// Trivial Case - No Joining needed
			NodePath* JoinPath = MakeCopy(FreePath);
			if (JoinPath == NULL)
				return FALSE;

			// Try to close it
			JoinPath->InkPath.TryToClose();

			// Apply some attributes
			if (!ApplyAttributes(JoinPath, GetWorkingDoc()))
				return FALSE;


			if (!AddPressureAttribute(JoinPath))
				return(FALSE);

			// Insert the node and invalidate the region 
			if (!DoInsertNewNode(JoinPath, StartSpread, FALSE))
				return FALSE;
			
			if (!DoInvalidateNodeRegion(JoinPath, TRUE))
				return FALSE;

			m_pNewNodePath = JoinPath;
			// Tell the freehand tool that it has a previous path
			SetRetroPath(JoinPath, 0, JoinPath->InkPath.GetNumCoords());
		}
		else
		{
			if (EndPath->InkPath.IsComplexPath())
			{
				// Complex Join (End of new path Joined)
				if (!ComplexJoin(FreePath, EndPath))
					return FALSE;
			}
			else
			{
				// Simple Join (End of new path Joined)
				if (!SimpleJoin(FreePath, EndPath))
					return FALSE;
			}
		}
	}
	else
	{
		if (EndPath == NULL)
		{
			if (StartPath->InkPath.IsComplexPath())
			{
				// Complex Join (Start of new path Joined)
				if (!ComplexJoin(FreePath, StartPath))
					return FALSE;
			}
			else
			{
				// Simple Join (Start of New Path Joined)
				if (!SimpleJoin(FreePath, StartPath))
					return FALSE;
			}
		}
		else
		{
			if (StartPath == EndPath)
			{
				if (StartPath->InkPath.IsComplexPath())
				{
					// Complex to Same Complex Join
					if (!ComplexToComplexJoin(FreePath, StartPath))
						return FALSE;
				}
				else
				{
					// Simple to Same Simple Join (Start = End)
					if (!SimpleJoin(FreePath, StartPath))
						return FALSE;
				}
			}
			else
			{
				// Compex To Complex Join (Start and End different and complex)
				// Complex To Simple Join (Start and End different and start complex)
				// Complex To Simple Join (Start and End different and end complex)
				// Simple To Simple Join (Start and End Different)
				if (!VeryComplexToComplexJoin(FreePath, StartPath, EndPath))
					return FALSE;
			}
		}
	}

	// Worked
	return TRUE;
}



/********************************************************************************************

>	BOOL OpFreeHand::SimpleJoin(NodePath* FreePath, NodePath* OldPath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/94
	Inputs:		FreePath - The New freehand Path that is to be joined
				OldPath - The path that we are trying to Join to
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Joins a Simple path with the new freehand path and builds all the undo that
				is needed in the process.

********************************************************************************************/

BOOL OpFreeHand::SimpleJoin(NodePath* FreePath, NodePath* OldPath)
{
	// First we should make a note of the region that we will need redrawing
	DocRect InvalidRect = FreePath->GetBlobBoundingRect();
	
	DocRect OriginalRect = OldPath->GetBoundingRect();
//	Spread* pSpread = Document::GetSelectedSpread();

	InvalidateBrushRegion(OldPath);


	// Need to know where in the joined path that the new section will be
	INT32 NumSlots = FreePath->InkPath.GetNumCoords();
	INT32 StartSlot = 0;
//	INT32 OldNumSlots = OldPath->InkPath.GetNumCoords();
	
//	DocCoord* pCoords = FreePath->InkPath.GetCoordArray();
//	DocCoord* pOldCoords = OldPath->InkPath.GetCoordArray();

	// Make a copy of the node we are joining with
	NodePath* JoinPath = MakeCopy(OldPath);
	if (JoinPath == NULL)
		return FALSE;

	// first find out if we are editing a brush (ugh, yes I know)
	AttrBrushType* pAttrBrush;
	NodeAttribute* pAttr = NULL;
	BrushHandle Handle = BrushHandle_NoBrush;
	JoinPath->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), &pAttr);
	if (pAttr != NULL)
	{
		pAttrBrush = (AttrBrushType*)pAttr;
		Handle = pAttrBrush->GetBrushHandle();
	}

	// find out if the two paths join at the start
	BOOL PathReversed = FALSE;

	// before Joining them
	BOOL NewPathReversed = FALSE;
	
	if (Handle != BrushHandle_NoBrush)
		SimpleJoinBrush(JoinPath, &(FreePath->InkPath));
	
	if (!JoinPath->InkPath.SimpleJoin(&FreePath->InkPath, &StartSlot, &NewPathReversed, &PathReversed))
		return FALSE;

	// Insert the new object
	if (!InsertNewNode(JoinPath, InvalidRect, OldPath))
		return FALSE;

	// Reverse the original mouse move data if the fitted path was reversed
	if (NewPathReversed)
		TrackData->Reverse();
	
	// if we are a brush then we don't ever want the path reversed, 
	if (Handle != BrushHandle_NoBrush && PathReversed)
	{
		TrackData->Reverse();
		JoinPath->InkPath.Reverse();
		StartSlot = 0;
	}

	// Tell the freehand tool all about it
	SetRetroPath(JoinPath, StartSlot, NumSlots);
	
	m_pNewNodePath = JoinPath;

	InvalidateBrushRegion(JoinPath);
	
	// It Worked!
	return TRUE;
}



/********************************************************************************************

>	BOOL OpFreeHand::ComplexJoin(NodePath* FreePath, Nodepath* OldPath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/94
	Inputs:		FreePath - The New freehand Path that is to be joined
				OldPath - The path that we are trying to Join to
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Joins a Complex path with the new freehand path and builds all the undo that
				is needed in the process.

********************************************************************************************/

BOOL OpFreeHand::ComplexJoin(NodePath* FreePath, NodePath* OldPath)
{
	// First we should make a note of the region that we will need redrawing
	DocRect InvalidRect = FreePath->GetBlobBoundingRect();

	// Need to know where in the joined path that the new section will be
	INT32 NumSlots = FreePath->InkPath.GetNumCoords();
	INT32 StartSlot;

	// Make a copy of the node we are joining with
	NodePath* JoinPath = MakeCopy(OldPath);
	if (JoinPath == NULL)
		return FALSE;

	// before Joining them
	BOOL NewPathReversed = FALSE;
	if (!JoinPath->InkPath.ComplexJoin(&FreePath->InkPath, &StartSlot, &NewPathReversed))
		return FALSE;

	// Insert the new object
	if (!InsertNewNode(JoinPath, InvalidRect, OldPath))
		return FALSE;

	// Reverse the original mouse move data if the fitted path was reversed
	if (NewPathReversed)
		TrackData->Reverse();

	// Tell the freehand tool all about it
	SetRetroPath(JoinPath, StartSlot, NumSlots);
	
	m_pNewNodePath = JoinPath;
	// It Worked!
	return TRUE;
}



/********************************************************************************************

>	BOOL OpFreeHand::ComplexToComplexJoin(NodePath* FreePath, NodePath* OldPath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/94
	Inputs:		FreePath - The New freehand Path that is to be joined
				OldPath - The path that we are trying to Join to
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Joins a Complex path with the new freehand path and builds all the undo that
				is needed in the process. The new freehand path joins two of the complex 
				paths sub-paths together.

********************************************************************************************/

BOOL OpFreeHand::ComplexToComplexJoin(NodePath* FreePath, NodePath* OldPath)
{
	// First we should make a note of the region that we will need redrawing
	DocRect InvalidRect = FreePath->GetBlobBoundingRect();

	// Need to know where in the joined path that the new section will be
	INT32 NumSlots = FreePath->InkPath.GetNumCoords();
	INT32 StartSlot;

	// Make a copy of the of the path we are Joining to
	NodePath* JoinPath = MakeCopy(OldPath);
	if (JoinPath==NULL)
		return FALSE;

	// Join the new path with the one already there
	BOOL NewPathReversed = FALSE;
	if (!JoinPath->InkPath.ComplexToSameComplexJoin(&FreePath->InkPath, &StartSlot, &NewPathReversed))
		return FALSE;

	// Insert the new object
	if (!InsertNewNode(JoinPath, InvalidRect, OldPath))
		return FALSE;

	// Reverse the original mouse move data if the fitted path was reversed
	if (NewPathReversed)
		TrackData->Reverse();

	// Tell the freehand tool all about it
	SetRetroPath(JoinPath, StartSlot, NumSlots);

	// It Worked!
	return TRUE;
}



/********************************************************************************************

>	BOOL OpFreeHand::VeryComplexToComplexJoin(NodePath* FreePath, NodePath* TopPath, NodePath* BotPath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/94
	Inputs:		FreePath - The New freehand Path that is to be joined
				TopPath -	The path to join with the new freehand path. This is the path
							that is highest in the Z order of the two paths
				BotPath - 	The path to join with the new freehand path. This is the path
							that is lowest in the Z order of the two paths
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Joins the new freehand path with two other paths (Which may or may not be
				complex) and builds all the undo info that is needed.

********************************************************************************************/

BOOL OpFreeHand::VeryComplexToComplexJoin(NodePath* FreePath, NodePath* TopPath, NodePath* BotPath)
{
	// First we should make a note of the region that we will need redrawing
	DocRect InvalidRect = FreePath->GetBlobBoundingRect();

	// Need to know where in the joined path that the new section will be
	INT32 NumSlots = FreePath->InkPath.GetNumCoords();
	INT32 StartSlot;

	// Make a copy of the of the path we are Joining to
	NodePath* JoinPath = MakeCopy(TopPath);
	if (JoinPath==NULL)
		return FALSE;

	// Merge in the other complex path
	if (!JoinPath->InkPath.MergeTwoPaths(BotPath->InkPath))
		return FALSE;

	// Join the new path with the one already there
	BOOL NewPathReversed = FALSE;
	if (!JoinPath->InkPath.ComplexToSameComplexJoin(&FreePath->InkPath, &StartSlot, &NewPathReversed))
		return FALSE;

	// Insert the new object
	if (!InsertNewNode(JoinPath, InvalidRect, TopPath, BotPath))
		return FALSE;

	// Reverse the original mouse move data if the fitted path was reversed
	if (NewPathReversed)
		TrackData->Reverse();

	// Tell the freehand tool all about it				
	SetRetroPath(JoinPath, StartSlot, NumSlots);

	// It Worked
	return TRUE;
}



/********************************************************************************************

>	BOOL OpFreeHand::ApplyAttributes(NodePath* NewPath, Document *pDocument)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/94
	Inputs:		NewPath - The NodePath to apply the current attributes to
				pDocument - the document this path will be placed into.
	Returns:	TRUE if it worked, FALSE if it did not
	Purpose:	Applies the current attributes to the Node passed in. It has to find the
				current document to do this and will ENSURE if it fails to find one.

********************************************************************************************/

BOOL OpFreeHand::ApplyAttributes(NodePath* NewPath, Document *pDocument)
{
	// Find the current document to get the current attributes from
	ENSURE(pDocument!=NULL, "Null Document while setting attributes for new NodePath");
	if (pDocument!=NULL)
	{
		// Apply the current attributes to the path
		if (pDocument->GetAttributeMgr().ApplyCurrentAttribsToNode((NodeRenderableInk*)NewPath))
		{
			// ok special case exception here, if we have a brush attribute then delete it
			Node* pNode = NewPath->FindFirstChild(CC_RUNTIME_CLASS(AttrBrushType));
			if (pNode)
			{
				pNode->CascadeDelete();
				delete pNode;
			}

			return TRUE;
		}
	}

	// We failed to apply the attributes, so fail
	return FALSE;
}



/********************************************************************************************

>	BOOL OpFreeHand::AddPressureAttribute(NodePath *NewPath)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Inputs:		NewPath - The NodePath to apply the attributes to

	Returns:	TRUE if it worked, FALSE if it did not

	Purpose:	If pressure recording is enabled, this method builds attributes to 
				apply to the path in order to attach the pressure as a variable-width
				stroking description.

				This attaches an AttrStrokeType to control stroking, and an AttrVariableWidth
				to record the pressure values

********************************************************************************************/

BOOL OpFreeHand::AddPressureAttribute(NodePath *pNewPath)
{
	if (IsBrushOp())
		return TRUE;
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING

	ERROR3IF(GetWorkingDoc() == NULL, "No working doc");
	ERROR3IF(pNewPath == NULL, "Illegal NULL param");
	ERROR3IF(TrackData == NULL, "Trackdata is NULL");

	// If pressure recording is disabled, then we don't need to do anything
	if (TrackData == NULL || !AddPressureToPath)
		return(TRUE);

	// --- Generate a smoothed pressure function from the recorded pressure samples
	// If this fails, then we still return TRUE, and the path just loses pressure information
	PressureSmoother Bob;
	ValueFunction *pValFunc = Bob.Smooth(TrackData, GetCurrentLineWidth());
	if (pValFunc == NULL)
		return(TRUE);

	// We only want to override the stroke type if it is a simple constant-width line
	// - if there is any new-style stroke type applied, we won't try to override it
	BOOL ApplyType = FALSE;
	
	// BLOCK
	{
		Node *pNode = pNewPath->FindFirstChild();
		if (pNode != NULL)
			pNode = pNode->FindNext(CC_RUNTIME_CLASS(AttrStrokeType));

		ApplyType = (pNode == NULL);	// If there are no StrokeType attrs applied already, we must apply one
		if (!ApplyType)
		{
			// If there is another StrokeType, but it will not stroke using the variable width info,
			// (i.e. has a NULL PathProcessor attached to it) we will need to replace it.
			StrokeTypeAttrValue *pSAV = (StrokeTypeAttrValue *) ((AttrStrokeType *)pNode)->GetAttributeValue();
			if (pSAV->GetPathProcessor() == NULL)
				ApplyType = TRUE;
		}
	}

	// --- Create a simple variable-width path stroke attribute
	AttrStrokeType *pStrokeType = NULL;
	if (ApplyType)
	{
		pStrokeType = new AttrStrokeType;
		if (pStrokeType == NULL)
			return(TRUE);				// Return TRUE - we'll only lose the pressure info

		PathProcessorStroke *pProcessor = new PathProcessorStroke;
		if (pProcessor == NULL)
		{
			delete pStrokeType;
			return(TRUE);				// Return TRUE - we'll only lose the pressure info
		}

		// And attach the stroker to the attribute
		((StrokeTypeAttrValue *)pStrokeType->GetAttributeValue())->SetPathProcessor(pProcessor);
	}

	// --- Create a pressure table variable-width attribute
	AttrVariableWidth *pVarWidth = new AttrVariableWidth;
	if (pVarWidth != NULL)
		((VariableWidthAttrValue *)pVarWidth->GetAttributeValue())->SetWidthFunction(pValFunc);

	if (pVarWidth == NULL)
	{
		if (pStrokeType != NULL)
			delete pStrokeType;
		return(TRUE);				// Return TRUE - we'll only lose the pressure info
	}

	// --- Now apply the attributes to the path, removing any of a similar type which
	// are already applied to it.
	if (ApplyType)
	{
		DoRemoveAttrTypeFromSubtree(pNewPath, pStrokeType->GetRuntimeClass());
		pStrokeType->AttachNode(pNewPath, FIRSTCHILD, FALSE, FALSE);
	}

	// And apply the variable width attribute
	DoRemoveAttrTypeFromSubtree(pNewPath, pVarWidth->GetRuntimeClass());
	pVarWidth->AttachNode(pNewPath, FIRSTCHILD, FALSE, FALSE);

#endif // VECTOR_STROKING

	return(TRUE);
}



/********************************************************************************************

>	NodePath* OpFreeHand::MakeCopy(Node* pOriginalNode)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/94
	Inputs:		pOriginalNode - The node to do a 'deep' copy of
	Returns:	The Copied node (NULL if it failed)
	Purpose:	Copies the node pOriginalNode and all its children an returns a pointer
				to them. It makes sure that the Node is a NodePath (Which it should be).

********************************************************************************************/

NodePath* OpFreeHand::MakeCopy(Node* pOriginalNode)
{
	// Make a copy of the node we are joining with
	Node* pNode;
	if (!pOriginalNode->NodeCopy(&pNode))
		return NULL;
	
	// Check that the new node is OK
	ENSURE(pNode!=NULL, "Copied node seems to be NULL in TryToJoinNewPathWithOthers()");
	ENSURE(pNode->IsKindOf(CC_RUNTIME_CLASS(NodePath)), "Copied node was not a Path");

	// return it
	return (NodePath*) pNode;
}


/********************************************************************************************

>	BOOL OpFreeHand::InsertNewNode(NodePath* pNewNode, DocRect& Invalid,
								   Node* pOldNode, Node* pOtherOld = NULL)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/94
	Inputs:		pNewNode - The Node to add to the tree
				Invalid - The region that needs to be invalidated
				pOldNode - The node that we joined to and will be hidden
				pOtherOld - We may need to hide 2 other paths. This should be NULL(default)
				or a pointer to a Node
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Inserts a new path into the tree and hides the original node that it was
				Joined to. It will also invalidate the region of the new path that is
				different.

********************************************************************************************/

BOOL OpFreeHand::InsertNewNode(NodePath* pNewNode, DocRect& Invalid, Node* pOldNode, Node* pOtherOld)
{
	// Insert the new object
	if (!DoInsertNewNode(pNewNode, pOldNode, NEXT, FALSE))
		return FALSE;

	// Invalidate the appropriate region.
	// if the filled state of the path has changed, then we need to invalidate the whole thing
	ENSURE(pOldNode->IsKindOf(CC_RUNTIME_CLASS(NodePath)), "Joining to something that was not a path");

//	if (pNewNode->InkPath.IsFilled == ((NodePath*)pOldNode)->InkPath.IsFilled)
//	{
		// Invalidate only the new bit
//		Spread* pSpread = pOldNode->FindParentSpread();
//		if (!DoInvalidateRegion(pSpread, Invalid))
//			return FALSE;
//	}
//	else

	// invalidate the whole thing
	if (!DoInvalidateNodeRegion(pNewNode, TRUE))
		return FALSE;		

	// Hide the original
	if (!DoHideNode(pOldNode, TRUE))
		return FALSE;

	// and hide the other original node if it existed
	if (pOtherOld != NULL)
		if (!DoHideNode(pOtherOld, TRUE))
			return FALSE;

	// Make sure that the Selection Cache is refreshed
	SelRange* Selection = GetApplication()->Selection;
	if (Selection)
		Selection->Update();

	// It worked
	return TRUE;
}





/********************************************************************************************

>	void OpFreeHand::SetRetroPath(NodePath* pNodePath, INT32 Start, INT32 Len)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/94
	Inputs:		pNodePath - The new path that we want to set up for retro fitting
				Start - The Start slot of the new section
				Len - The number of slots the new section takes up
	Purpose:	Calls the freehand tool with details about the new path, so that future
				changes in the smoothness will re-build the new path

********************************************************************************************/

void OpFreeHand::SetRetroPath(NodePath* pNodePath, INT32 Start, INT32 Len)
{
	// Go find the current tool
	Tool* pTool = Tool::GetCurrent();
	if (pTool)
	{
		// Make sure that it is the freehand tool, and if so set the previous path
		if (pTool->GetID()==TOOLID_FREEHAND)
			((FreeHandTool*)pTool)->SetPreviousPath(pNodePath, Start, Len);
	}
}




/********************************************************************************************

>	void OpFreeHand::RenderLine(RenderRegion *pRender, Path *pPath, INT32 CoordIndex, BOOL StartIndex = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/1/97
	Inputs:		pRender - the RenderReigon to draw into
				pPath - The path to draw a line segment from
				CoodIndex - The index of the end coordinate of the line to render. This
							will render an EORed line from Coords[CoordIndex-1] to
							Coords[CoordIndex]
				StartIndex - is Index the start index?  If not then we simply render between
				Index-1 and Index, otherwise we render between Index and NumCoords -1
	Purpose:	Renders an EORed line between 2 coordinates in the path.
				May render pressure information feedback using the pressure stored with
				the path corrdinates.

********************************************************************************************/

void OpFreeHand::RenderLine(RenderRegion *pRender, Path *pPath, INT32 Index, BOOL StartIndex)
{
	ERROR3IF(pRender == NULL || pPath == NULL, "illegal NUL params");
	ERROR3IF(Index < 1 || Index >= pPath->GetNumCoords(), "Out of range coordinate");

	INT32 EndIndex = Index;
	if (!StartIndex)
	{
		EndIndex = Index;
		
	}
	else
		EndIndex = pPath->GetNumCoords() - 1;

	Index--;
	DocCoord *Coord = pPath->GetCoordArray();

	// Always draw the blue centreline
	pRender->SetLineColour(COLOUR_XORNEW);
	pRender->DrawLine(Coord[Index], Coord[EndIndex]);
	
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING

	// If we're showing pressure info, and the points are not coincident, and the average
	// line width is large enough to avoid hideous EOR mess, render parallel width lines
	if (!AddPressureToPath)
		return;

	PathWidth *Width = pPath->GetWidthArray();
	if (Width == NULL)
		return;

	const INT32 CurrentLineWidth = GetCurrentLineWidth();
	const double WidthMul = (double)CurrentLineWidth / (double)EXTRAVALUEMAX;
	INT32 StartWidth	= (INT32) ((double)Width[Index-1] * WidthMul);
	INT32 EndWidth	= (INT32) ((double)Width[Index]   * WidthMul);

	if (StartWidth > CurrentLineWidth)		// Damage control in case of rampant pens giving
		StartWidth = CurrentLineWidth;		// out of range values. You may laugh, but that's
	if (EndWidth > CurrentLineWidth)		// exactly what the pen code was doing until I
		EndWidth = CurrentLineWidth;		// just fixed it. This just stops wild rampancy

	if (Coord[Index-1] != Coord[Index] && (StartWidth + EndWidth) > pRender->GetPixelWidth() * 8)
	{
		pRender->SetLineColour(COLOUR_MIDGREY);

		// Get the normal to the centreline and normalise it to unit length
		NormCoord Normal( Coord[Index-1].y - Coord[Index].y, -(Coord[Index-1].x - Coord[Index].x));
		Normal.Normalise();

		// Now plot the two parallel edges
		DocCoord P1(Coord[Index-1].x + (INT32)(Normal.x * StartWidth), Coord[Index-1].y + (INT32)(Normal.y * StartWidth));
		DocCoord P2(Coord[Index].x   + (INT32)(Normal.x * EndWidth),   Coord[Index].y   + (INT32)(Normal.y * EndWidth));
		pRender->DrawLine(P1, P2);

		DocCoord P3(Coord[Index-1].x - (INT32)(Normal.x * StartWidth), Coord[Index-1].y - (INT32)(Normal.y * StartWidth));
		DocCoord P4(Coord[Index].x   - (INT32)(Normal.x * EndWidth),   Coord[Index].y   - (INT32)(Normal.y * EndWidth));
		pRender->DrawLine(P3, P4);
	}
#endif // VECTOR_STROKING
}



/********************************************************************************************

>	void OpFreeHand::RenderLine(DocRect* Rect, Spread* pSpread, Path *pPath, INT32 CoordIndex,
								BOOL StartIndex = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/1/97
	Inputs:		pRender - the RenderReigon to draw into
				pPath - The path to draw a line segment from
				CoodIndex - The index of the end coordinate of the line to render. This
							will render an EORed line from Coords[CoordIndex-1] to
							Coords[CoordIndex]
				StartIndex - is Index the start index?  If not then we simply render between
				Index-1 and Index, otherwise we render between Index and NumCoords -1
	Purpose:	Renders an EORed line between 2 coordinates in the path.
				May render pressure information feedback using the pressure stored with
				the path corrdinates.

********************************************************************************************/

void OpFreeHand::RenderLine(DocRect* Rect, Spread* pSpread, Path *pPath, INT32 Index, BOOL StartIndex)
{
	RenderRegion* pRegion = DocView::RenderOnTop(Rect, pSpread, UnclippedEOR );
	while ( pRegion )
	{
		RenderLine(pRegion, pPath, Index, StartIndex);

		// get the next region to draw in
		pRegion = DocView::GetNextOnTop(Rect);
	}	
}




/********************************************************************************************

>	void OpFreeHand::RenderEorStraightLine(RenderRegion *pRender,
										DocCoord *pStart, INT32 StartWidth,
										DocCoord *pEnd, INT32 EndWidth)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/1/97
	Inputs:		pRender - the render region to render intp
				pStart - The start of the line (non-NULL)
				StartWidth - The line width (in millipoints) at pStart
				pEnd - The end of the line (non-NULL)
				EndWidth- The line width (in millipoints) at pEnd

	Purpose:	Simply renders a straight-line segment to the given render region

				However, it may also elect to show width information by rendering
				parallel straight line segments on either side of the line

********************************************************************************************/

void OpFreeHand::RenderEorStraightLine(RenderRegion *pRender,
										DocCoord *pStart, INT32 StartWidth,
										DocCoord *pEnd, INT32 EndWidth)
{
	ERROR3IF(pRender == NULL || pStart == NULL || pEnd == NULL, "illegal NUL params");

	// Always draw the blue centreline
	pRender->SetLineColour(COLOUR_XORNEW);
	pRender->DrawLine(*pStart, *pEnd);

// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING

	// If we're showing pressure info, and the points are not coincident, and
	// the average line width is large enough to avoid hideous EOR mess, render parallel width lines
	if (AddPressureToPath && *pStart != *pEnd && (StartWidth + EndWidth) > pRender->GetPixelWidth() * 8)
	{
		pRender->SetLineColour(COLOUR_MIDGREY);

		// Get the normal to the centreline and normalise it to unit length
		NormCoord Normal( pStart->y - pEnd->y, -(pStart->x - pEnd->x));
		Normal.Normalise();

		// Now plot the two parallel edges
		DocCoord P1(pStart->x + (INT32)(Normal.x * StartWidth), pStart->y + (INT32)(Normal.y * StartWidth));
		DocCoord P2(pEnd->x   + (INT32)(Normal.x * EndWidth),   pEnd->y   + (INT32)(Normal.y * EndWidth));
		pRender->DrawLine(P1, P2);

		DocCoord P3(pStart->x - (INT32)(Normal.x * StartWidth), pStart->y - (INT32)(Normal.y * StartWidth));
		DocCoord P4(pEnd->x   - (INT32)(Normal.x * EndWidth),   pEnd->y   - (INT32)(Normal.y * EndWidth));
		pRender->DrawLine(P3, P4);
	}

#endif // VECTOR_STROKING
}



/********************************************************************************************

>	void OpFreeHand::RenderEorStraightLine(DocRect* Rect, Spread* pSpread,
											DocCoord *pStart, DocCoord *pEnd)
	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/94
	Inputs:		Rect - The cliprect
				pSpread - The spread to draw to
				pStart - The start of the line (NULL means "PreviousPoint")
				pEnd - The end of the line (NULL means "StraightLinePos")
				
	Purpose:	Renders the EORed rubber band straight line that is used in Straight Line
				Mode. It simply draws a single Straight Line segment

********************************************************************************************/

void OpFreeHand::RenderEorStraightLine(DocRect* Rect, Spread* pSpread, DocCoord *pStart, DocCoord *pEnd)
{
	if (pStart == NULL)
		pStart = &PreviousPoint;
	if (pEnd == NULL)
		pEnd = &StraightLinePos;

	INT32 LineWidth = GetCurrentLineWidth();

	RenderRegion* pRegion = DocView::RenderOnTop(Rect, pSpread, UnclippedEOR );
	while ( pRegion )
	{
		RenderEorStraightLine(pRegion, pStart, LineWidth, pEnd, LineWidth);

		// get the next region to draw in
		pRegion = DocView::GetNextOnTop(Rect);
	}
}



/********************************************************************************************

>	INT32 OpFreeHand::GetCurrentLineWidth(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/1/97

	Returns:	A line width in millipoints.
				NOTE that this is half the normal linewidth value (i.e. it's the line radius)

	Purpose:	Determines the current line width (the width of any future
				lines the user will draw) by asking the attribute manager.

********************************************************************************************/

INT32 OpFreeHand::GetCurrentLineWidth(void)
{
	ERROR3IF(GetWorkingDoc() == NULL, "No working doc!");
	AttrLineWidth *pWidthAttr = (AttrLineWidth *) GetWorkingDoc()->GetAttributeMgr().
					GetCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), CC_RUNTIME_CLASS(AttrLineWidth));
	INT32 LineWidth = 1000;
	if (pWidthAttr != NULL)
		LineWidth = pWidthAttr->Value.LineWidth / 2;

	return(LineWidth);
}



/********************************************************************************************

>	virtual void OpFreeHand::RenderDragBlobs( DocRect Rect, Spread* pSpread, BOOL bSolidDrag )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Inputs:		Rect - The Rectangle that needs to be re-rendered
				pSpread - The spread on which the rendering is to take place
	Purpose:	EORs the whole path onto the screen a segment at a time. It needs to 
				do it a segment at a time as that is how it is drawn as the path is
				created. This has to redraw it identically or there will be bits of
				EORed stuff left all over the place.
	SeeAlso:	OpFreeHand::RenderLineSegment()

********************************************************************************************/

void OpFreeHand::RenderDragBlobs( DocRect Rect, Spread* pSpread, BOOL bSolidDrag )
{
	// Work out which bits we need to draw
	DocRect* pRect;
	if (Rect.IsEmpty())
		pRect = NULL;
	else
		pRect = &Rect;

	// Get information on the path we've got to render
	INT32 NumCoords = TrackData->GetNumCoords();
//	DocCoord* Coords = TrackData->GetCoordArray();

	// Find out the current line width
//	INT32 LineWidth = GetCurrentLineWidth();

	// Render into all the available rectangles
	RenderRegion* pRegion = DocView::RenderOnTop(pRect, pSpread, UnclippedEOR );
	while ( pRegion )
	{
		// Rather than just calling DrawPath, we render each line ourselves - this allows the variable
		// width indicator lines to be rendered as well, and ensures all the EORing remains consistent.
		for (INT32 n = 1; n < NumCoords; n++)
			RenderLine(pRegion, TrackData, n);

		// get the next region to draw in
		pRegion = DocView::GetNextOnTop(pRect);
	}

	if (IsStraightLineMode)
		RenderEorStraightLine(pRect, pSpread);
}




/********************************************************************************************

>	BOOL OpFreeHand::TryToReplaceSection(NodePath* pNewNodePath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/94
	Inputs:		pNewNodePath - The New Path that we want to use instead of a section of
				an existing path
	Purpose:	Replaces a section from the path that the drag was started and finished on
				with the newly drawn curve section.

********************************************************************************************/

BOOL OpFreeHand::TryToReplaceSection(NodePath* pNewNodePath)
{
	// The elements that the split occured on
	INT32 FirstElement, SecondElement, NewElements;

	ObjChangeFlags cFlags;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,EndPath,this);
	if (!EndPath->AllowOp(&ObjChange, TRUE))
		return TRUE;	// Pretend that we've done it.

	// Invalidate it as it was
	if (!DoInvalidateNodeRegion(EndPath, TRUE))
		return FALSE;		

	// Split old path at start and end points
	if (!SplitAtPoint(StartPoint, &FirstElement, &NewElements))
		return FALSE;

	if (!SplitAtPoint(PreviousPoint, &SecondElement, &NewElements))
		return FALSE;
	
	// We may need to adjust the first element if the second element was
	// before it in the path
	if (SecondElement<=FirstElement)
	{
		// Swap the elements around and move the first one back a bit
		INT32 Temp = FirstElement + (NewElements/2);
		FirstElement = SecondElement;
		SecondElement = Temp;

		// We will also need to reverse the original curve here
		// (as well as the original track data)
		pNewNodePath->InkPath.Reverse();
		TrackData->Reverse();
		ReverseBrushPressure(); // if we are a brush we will want to reverse our pressure data
	}

	// Go and see if which part of the path we should be replacing
	if (!EndPath->InkPath.IsComplexPath() && EndPath->InkPath.IsFilled)
	{
		// Here we need to see if we will replace the wrong bit of the path and try and do something
		// about it if we are. The wrong bit is defined as follows. If you were to draw a filled shape
		// such as an ellipse and modified it by drawing a small loop around one of the control points
		// you should get an ellipse with a small notch cut out of it. However, if you do this around
		// the Start/End control point you end up with no ellipse and just a small notch.
		// To get round this, we always try to replace the smallest number of control points possible.
		INT32 HalfElements = EndPath->InkPath.GetNumCoords() / 2;
		INT32 ReplaceElements = SecondElement - FirstElement;
		if (HalfElements < ReplaceElements)
		{
			// We were trying to replace more than half of the elements in the path, so actually
			// replace the other, large half of the path
			
			// Here we have to change the the start element of the path, so we have
			// to hide the original and make a copy of it
			BOOL IsOk = FALSE;
			Node* pCopyNode = NULL;
			CALL_WITH_FAIL(EndPath->NodeCopy(&pCopyNode), this, IsOk);

			// See if we managed to make a copy of the node
			if (!IsOk)
				return FALSE;

			// Insert the copy
			if (!DoInsertNewNode((NodeRenderableBounded*)pCopyNode, EndPath, NEXT, FALSE))
				return FALSE;

			// Hide the original
			if (!DoHideNode(EndPath, TRUE))
				return FALSE;

			// Copy the pointer back to where it used to be
			EndPath = (NodePath*) pCopyNode;
			ERROR3IF(!EndPath->IS_KIND_OF(NodePath), "Copy of a Node path was not a node path");

			// Make the Last replace element the start/end of the path
			if (EndPath->InkPath.ChangeStartElement(SecondElement-1))
			{
				// It worked, so modify the Start and End Elements to match
				FirstElement = 1;
				SecondElement = EndPath->InkPath.GetNumCoords() - ReplaceElements;

				// We will also need to reverse the original curve here
				// (as well as the original track data)
				pNewNodePath->InkPath.Reverse();
				TrackData->Reverse();
			}
		}

	}

	// replace section with new path
	INT32 NumElements = SecondElement - FirstElement;
	if (!ReplaceMiddleOfPath(pNewNodePath, FirstElement, NumElements))
		return FALSE;

	// Mark the nodes bounding rect as invalid
	EndPath->InvalidateBoundingRect();
	
	TRACEUSER( "Diccon", _T("Replacing middle of Path\n"));
	InvalidateBrushRegion(EndPath);
	// and invalidate how it is now
	if (!DoInvalidateNodeRegion(EndPath, TRUE))
		return FALSE;		

	// Failed to do it
	return TRUE;
}





/********************************************************************************************

>	BOOL OpFreeHand::SplitAtPoint(const DocCoord& SplitPoint)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/94
	Inputs:		SplitPoint - The coord that we want the split to happen close to
	Outputs:	SplitAt - The Slot number of the new end point
				NewElements - The number of new elements inserted into the path
	Returns:	TRUE if it was able to split the path ok, FALSE if it failed
	Purpose:	Splits the path, building undo info, at the coord supplied.

********************************************************************************************/

BOOL OpFreeHand::SplitAtPoint(const DocCoord& SplitPoint, INT32* SplitAt, INT32* NewElements)
{
	// First we need to get some space to put any new coords etc that will be generated
	INT32 SplitElement;
	UINT32 NumElements;
	PathVerb	NewVerbs[6];
	DocCoord	NewCoords[6];
	PathFlags	NewFlags[6];

	// First we had better go and see if this point is close to any of the existing coords
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr!=NULL)
	{
		// There was a blob manager, so we could test the coords
		// Find out about the area round the mouse position to consider for rubbing out
		DocRect BlobRect;
		DocCoord BlobPoint = SplitPoint;
		pBlobMgr->GetBlobRect(BlobPoint, &BlobRect);

		// Find out about all the coords in the path already
		PathFlags* Flags = EndPath->InkPath.GetFlagArray();
		DocCoord* Coords = EndPath->InkPath.GetCoordArray();
		INT32 NumCoords = EndPath->InkPath.GetNumCoords();

		// Loop through all the coords in the path
		for (INT32 i=0; i<NumCoords; i++)
		{
			// Is this coord close to the point specified
			if ((Flags[i].IsEndPoint==TRUE) && (BlobRect.ContainsCoord(Coords[i])))
			{
				// Yep, set the outputs to the correct values and return Success
				*SplitAt = i+1;
				*NewElements = 0;

				// Make sure that the rotate flags are unset around the coord
				Flags[i].IsRotate = FALSE;
				if (i>0)
					Flags[i-1].IsRotate = FALSE;

				if (i<NumCoords-1)
					Flags[i+1].IsRotate = FALSE;

				// And tell caller that it worked
				return TRUE;
			}
		}
	}

	// Try and split the path. This returns FALSE if the path did not need splitting
	if (EndPath->InkPath.SplitAtPoint(SplitPoint, &SplitElement, &NumElements, NewVerbs, NewCoords))
	{
		// Make a note of where the new endpoint will be
		*SplitAt = SplitElement+(NumElements/2);
		*NewElements = NumElements;

		// The path was split, so we know where, and how, so let's party on the path
		PathVerb* Verbs = EndPath->InkPath.GetVerbArray();
		PathFlags* Flags = EndPath->InkPath.GetFlagArray();
		DocCoord* Coords = EndPath->InkPath.GetCoordArray();
//		UINT32	NumCoords = EndPath->InkPath.GetNumCoords();	
		INT32 NumToChange;

		// We're adding something, either a line or curve - check which
		if ((Verbs[SplitElement] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
		{
			// Adding a curve - number to change = 6
			NumToChange = 6;

			// Initialise the flags appropriately
			for (INT32 i=0; i<6; i++)
			{
				// copy the flags from the old curve (repeat the flag in the first 3 elements)
				NewFlags[i] = Flags[SplitElement+(i%3)];
				NewFlags[i].IsSelected = FALSE;

				// Want it to be a cusp in the middle
				if ((i>0) && (i<4))
				{
					NewFlags[i].IsSmooth = FALSE;
					NewFlags[i].IsRotate = FALSE;
				}
			}

			// If this was happening at the end of a path,
			// then make sure the close figure flag is set correctly
			if (Verbs[SplitElement+2] & PT_CLOSEFIGURE)
				NewVerbs[5] |= PT_CLOSEFIGURE;
		}
		else
		{
			// The slit happened in a straight line section
			NumToChange = 2;
			NewFlags[0] = NewFlags[1] = Flags[SplitElement];
			NewFlags[0].IsSmooth = NewFlags[0].IsRotate = FALSE;
			NewFlags[0].IsSelected = FALSE;

			// Make sure that the close figure flag is maintained
			if (Verbs[SplitElement] & PT_CLOSEFIGURE)
				NewVerbs[1] |= PT_CLOSEFIGURE;
		}

		// Start performing actions to insert the split point with full undo
		Action* UnAction;
		ActionCode Act = RemovePathElementAction::Init(this, &UndoActions, NumToChange/2,
													SplitElement, (Action**)(&UnAction));

		// if we failed to init the action, then fail this operation
		if (Act==AC_FAIL)
			return FALSE;

		// record details about the path in the action before we start inserting stuff
		((RemovePathElementAction*)UnAction)->RecordPath(EndPath);

		// Move to the place in the path where the split took place
		EndPath->InkPath.SetPathPosition(SplitElement);
		PathFlags InsertFlags;
		BOOL      InsertWorked;

		// insert a curve or a line, depending on where the split happened
		if (NumToChange==6)
			InsertWorked = EndPath->InkPath.InsertCurveTo(NewCoords[0], NewCoords[1], NewCoords[2], &InsertFlags);
		else
			InsertWorked = EndPath->InkPath.InsertLineTo(NewCoords[0], &InsertFlags);

		// see if the insertion worked
		if (!InsertWorked)
			return FALSE;
				
		// Build an action to record the changes we're about to make to the path
		ModifyPathAction* ModAction;
		Act = ModifyPathAction::Init(this, &UndoActions, NumToChange, (Action**)(&ModAction));
		if (Act==AC_FAIL)
			return FALSE;
	
		// If the action is happy...
		if (Act!=AC_NORECORD)
		{
			// Some pointers to what has changed
			PathVerb* ChangedVerbs;
			PathFlags* ChangedFlags;
			DocCoord* ChangedCoords;
			INT32* ChangedIndices;

			// memory for the details about what has changed
			ALLOC_WITH_FAIL(ChangedVerbs, (PathVerb*) CCMalloc(NumToChange * sizeof(PathVerb)), this);
			ALLOC_WITH_FAIL(ChangedFlags, (PathFlags*) CCMalloc(NumToChange* sizeof(PathFlags)), this);
			ALLOC_WITH_FAIL(ChangedCoords, (DocCoord*) CCMalloc(NumToChange* sizeof(DocCoord)), this);
			ALLOC_WITH_FAIL(ChangedIndices, (INT32*) CCMalloc(NumToChange* sizeof(INT32)), this);

			// See of all the allocations worked
			if (!ChangedVerbs || !ChangedFlags || !ChangedCoords || !ChangedIndices)
			{
				// They did not, so free them up and fail
				if (ChangedVerbs) CCFree(ChangedVerbs);
				if (ChangedFlags) CCFree(ChangedFlags);
				if (ChangedCoords) CCFree(ChangedCoords);
				if (ChangedIndices) CCFree(ChangedIndices);

				return FALSE;
			}

			// Reread the pointers, in case they've changed
			Verbs = EndPath->InkPath.GetVerbArray();
			Flags = EndPath->InkPath.GetFlagArray();
			Coords = EndPath->InkPath.GetCoordArray();

			// Now record the arrays...
			for (INT32 i=0; i<NumToChange; i++)
			{
				// record which elements changed and what they used to be
				ChangedIndices[i] = SplitElement+i;
				ChangedVerbs[i] = Verbs[SplitElement+i];
				ChangedFlags[i] = Flags[SplitElement+i];
				ChangedCoords[i] = Coords[SplitElement+i];
			}

			// and store the info in the action
			ModAction->StoreArrays(ChangedVerbs, ChangedFlags, ChangedCoords, ChangedIndices, EndPath);
		}

		// Reread the pointers, in case they've changed
		Verbs = EndPath->InkPath.GetVerbArray();
		Flags = EndPath->InkPath.GetFlagArray();
		Coords = EndPath->InkPath.GetCoordArray();

		// Now that the undo rigmarole has been done, let's change the path
		for (INT32 i=0; i<NumToChange; i++)
		{
			Verbs[SplitElement+i] = NewVerbs[i];
			Flags[SplitElement+i] = NewFlags[i];
			Coords[SplitElement+i] = NewCoords[i];
		}
	}
	else
	{
		// else we tried to split at an existing control point (endpoint)
		// That means that there were no new endpoints
		*NewElements = 0;
		*SplitAt = SplitElement;

		// See if we can find the control point in question
		DocCoord* Coords = EndPath->InkPath.GetCoordArray();
		INT32 NumCoords = EndPath->InkPath.GetNumCoords();

		// see if the split point is over the last point on the curve
		if (Coords[NumCoords-1]==SplitPoint)
			*SplitAt = NumCoords;
	}	

	// all worked
	return TRUE;
}




/********************************************************************************************

>	BOOL OpFreeHand::ReplaceMiddleOfPath(NodePath* pNewPath, INT32 FirstChangedIndex,
										INT32 NumElements)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Inputs:		pNewPath - The New curve that has just been draw
				FirstChangedIndex - The first coord in the path that is to be replaced
				NumElements - The number of elements in the path that need replacing
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	This will replace a section of the path with a new path, building undo
				as it goes. This function will also set up the Retro Curve fitting data
				so that it is able to retro fit the new section of the path.

********************************************************************************************/

BOOL OpFreeHand::ReplaceMiddleOfPath(NodePath* pNewPath, INT32 FirstChangedIndex, INT32 NumElements)
{
	// if we are a brush then do our special smoothing
	//etroSmoothBrush(pNewPath);

	// if we are editing a brush with timestamp or pressure lists 
	EditBrushLists(pNewPath, FirstChangedIndex, NumElements);
	
	// If there is something to remove from this path, the do it
	if (NumElements>0)
	{
		// Make ourselves an insert action as we are about to delete the
		// old bits of the path we no longer want
		ActionCode Act;
		InsertPathElementAction* ModAction;
		Act = InsertPathElementAction::Init(this, &UndoActions, NumElements, FirstChangedIndex,
										   (Action**)(&ModAction));
	
		// Did we init the action ok
		if (Act == AC_FAIL)
			return FALSE;

		// If the action will let us, record the details of what we want to delete
		if (Act!=AC_NORECORD)
		{
			// We need to store info about all the elements we are going to delete
			PathVerb* ChangedVerbs;
			DocCoord* ChangedCoords;
			PathFlags* ChangedFlags;

			// Get some memory to put them in
			ALLOC_WITH_FAIL(ChangedVerbs, (PathVerb*)CCMalloc(NumElements*sizeof(PathVerb)), this);
			ALLOC_WITH_FAIL(ChangedCoords, (DocCoord*)CCMalloc(NumElements*sizeof(DocCoord)), this);
			ALLOC_WITH_FAIL(ChangedFlags, (PathFlags*)CCMalloc(NumElements*sizeof(PathFlags)), this);

			// Make sure that we got the memory
			if (!ChangedVerbs || !ChangedCoords || !ChangedFlags)
			{
				if (ChangedVerbs) CCFree(ChangedVerbs);
				if (ChangedCoords) CCFree(ChangedCoords);
				if (ChangedFlags) CCFree(ChangedFlags);
				return FALSE;
			}

			// Get pointers to all the arrays of the path
			PathVerb* Verbs = EndPath->InkPath.GetVerbArray();
			DocCoord* Coords = EndPath->InkPath.GetCoordArray();
			PathFlags* Flags = EndPath->InkPath.GetFlagArray();

			// Now copy the data from the path into the arrays
			for (INT32 i=0; i<NumElements; i++)
			{
				ChangedVerbs[i] = Verbs[FirstChangedIndex+i];
				ChangedCoords[i] = Coords[FirstChangedIndex+i];
				ChangedFlags[i] = Flags[FirstChangedIndex+i];
			}

			// And ask the action to make a note of them
			ModAction->RecordPath(ChangedVerbs, ChangedFlags, ChangedCoords, EndPath);
		}

		// Delete the elements from the path that we no longer want
		EndPath->InkPath.DeleteSection(FirstChangedIndex, NumElements);
	}


	// Next we have to add the new points, so build undo to remove them
	Action* UnAction;
	INT32 NumCoords = pNewPath->InkPath.GetNumCoords() - 1;
	ActionCode Act = RemovePathElementAction::Init(this, &UndoActions, NumCoords,
												FirstChangedIndex, (Action**)(&UnAction));

	// if we failed to init the action, then fail this operation
	if (Act==AC_FAIL)
		return FALSE;

	// record details about the path in the action before we start inserting stuff
	((RemovePathElementAction*)UnAction)->RecordPath(EndPath);

	// Insert stuff here
	EndPath->InkPath.InsertSection(FirstChangedIndex, NumCoords);

	// Get pointers to all the arrays of the original path
	PathVerb* Verbs = EndPath->InkPath.GetVerbArray();
	DocCoord* Coords = EndPath->InkPath.GetCoordArray();
	PathFlags* Flags = EndPath->InkPath.GetFlagArray();

	// Get the pointer to all the arrays of the new path data
	PathVerb* NewVerbs = pNewPath->InkPath.GetVerbArray();
	DocCoord* NewCoords = pNewPath->InkPath.GetCoordArray();
	PathFlags* NewFlags = pNewPath->InkPath.GetFlagArray();

	// Copy the new path data into the hole we have made
	FirstChangedIndex--;
	for (INT32 i=1; i<=NumCoords; i++)
	{
		Verbs[FirstChangedIndex+i] = NewVerbs[i];
		Coords[FirstChangedIndex+i] = NewCoords[i];
		Flags[FirstChangedIndex+i] = NewFlags[i];
	}

	// set up retro stuff here
	SetRetroPath(EndPath, FirstChangedIndex, pNewPath->InkPath.GetNumCoords());

	return TRUE;
}



/********************************************************************************************

>	BOOL OpFreeHand::EditBrushLists(NodePath* pNewPath, INT32 FirstChangedIndex,
										INT32 NumElements)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/2000
	Inputs:		pNewPath - The New curve that has just been draw
				FirstChangedIndex - The first coord in the path that is to be replaced
				NumElements - The number of elements in the path that need replacing
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	When a path is edited that has a timestamping brush applied to it we those 
				points are stored in a list that is generated when the path is drawn.  If we 
				subsequently want to edit that path we have to insert or remove points from 
				that list.  This function works out what points should be inserted or removed,
				and performs that action on the applied brush.

********************************************************************************************/

BOOL OpFreeHand::EditBrushLists(NodePath* pNewPath, INT32 FirstChangedIndex, INT32 NumElements)
{
	// what we are going to do here is find out whether or not the path we are editing is brushed
	// and if so whether or not it uses pressure data.  If so then we will generate a list of pressure
	// data and insert it.
	ERROR2IF(pNewPath == NULL, FALSE, "pNewPath is NULL in OpFreeHand::EditPressureList");

	/* First up, find the nearest applied brush attribute. If there isn't one or
	it has no pressure list then we don't have to bother with all this */
	AttrBrushType* pAttrBrush = NULL;

	pAttrBrush = EndPath->GetAppliedBrushAttribute();
	if (pAttrBrush == NULL)
		return TRUE;

	// if we are attempting to edit an existing brush that does not have an existing pressure
	// cache then leave now
	if (!pAttrBrush->ContainsPressureCache())
		return TRUE;
	
	// most of the action takes place in the attribute value so get that also
	BrushAttrValue* pVal = (BrushAttrValue*)pAttrBrush->GetAttributeValue();
	if (pVal == NULL)
		return TRUE;

	/* next, record the following distances:
	- original path length;
	- distance to first changed point;
	- distance to last changed point;
	*/

//	double OriginalLength = EndPath->InkPath.GetPathLength();
	MILLIPOINT DistToFirstPoint = -1;
	MILLIPOINT DistToLastPoint = -1;
	DocCoord* pCoords = EndPath->InkPath.GetCoordArray();

	DocCoord FirstChangedPoint;
	DocCoord LastChangedPoint;

	if (pCoords == NULL)
		return FALSE;
	if (FirstChangedIndex + NumElements > EndPath->InkPath.GetNumCoords())
	{
		ERROR3("Illegal number of coordinates");
		return FALSE;
	}

	// get the coordinates from the array
	FirstChangedPoint = pCoords[FirstChangedIndex];
	LastChangedPoint  =  pCoords[FirstChangedIndex + NumElements];

	EndPath->InkPath.GetDistanceToPoint(FirstChangedPoint, &DistToFirstPoint);
	EndPath->InkPath.GetDistanceToPoint(LastChangedPoint, &DistToLastPoint);

	// Now find the indices into the pressure list that correspond to those distances
	INT32 StartPressureIndex = pVal->GetPressureListIndexAtDistance(DistToFirstPoint);
	if (StartPressureIndex == -1)
	{
		ERROR3("StartPressureIndex is -1 in OpDrawBrush::EditPressureList");
		return FALSE;
	}
	
	INT32 EndPressureIndex = pVal->GetPressureListIndexAtDistance(DistToLastPoint);
	if (EndPressureIndex == -1 || EndPressureIndex <= StartPressureIndex)
	{
		ERROR3("EndPressureIndex is invalid in OpDrawBrush::EditPressureList");
		return FALSE;
	}
	UINT32 NumObjects = EndPressureIndex - StartPressureIndex;

	CDistanceSampler* pSampler = OpDrawBrush::GeneratePressureData(pAttrBrush, DistToFirstPoint, DistToLastPoint,
												(MILLIPOINT)pNewPath->InkPath.GetPathLength());	

	if (pSampler == NULL)  // did it fail, of so we must quit
		return FALSE;
	/* now get the pressure points list and ask the attribute to clear all the points
	between the edited distances*/

	// Invalidate it as it was
	pAttrBrush->ClearCachedRect();
	DocRect Rect = pAttrBrush->GetAttrBoundingRect(EndPath);
	EndPath->ReleaseCached();
	Spread* pSpread = Document::GetSelectedSpread();
	if (!DoInvalidateRegion(pSpread, Rect))
		return FALSE;
	// now make a removepoints action
	RemovePressurePointsAction* pAction; 
	
	if (RemovePressurePointsAction::Init(this, &UndoActions, pAttrBrush, (UINT32)StartPressureIndex, NumObjects, NULL, &pAction) == AC_FAIL)
		return FALSE;

	// now insert our new points into the attribute
	AddPressurePointsAction* pAddAction;

	// find out how many points we're inserting
	UINT32 NumAddPoints = pSampler->GetNumItems();
	
	if (AddPressurePointsAction::Init(this, &UndoActions, pAttrBrush, pSampler, (UINT32)StartPressureIndex, NumAddPoints,  &pAddAction) == AC_FAIL)
		return FALSE;
	
	// invalidate the whole thing3
	pAttrBrush->ClearCachedRect();
	Rect = pAttrBrush->GetAttrBoundingRect(EndPath);	
	EndPath->ReleaseCached();
	if (!DoInvalidateRegion(pSpread, Rect))
		return FALSE;
	return TRUE;
	
}


/********************************************************************************************

>	BOOL OpFreeHand::SimpleJoinBrush(NodePath* pInsertedNode, Path* pNewPath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/2000
	Inputs:		pInsertedNode - the nodepath that we have just inserted
				pNewPath - The New curve that has just been drawn
				
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Currently this only has to do work if we are editing a brush containing pressure
				data.  In this case we must generate a new list of pressure info and insert it
				into the brush.

********************************************************************************************/

BOOL OpFreeHand::SimpleJoinBrush(NodePath* pInsertedNode, Path* pNewPath)
{
	if (pInsertedNode == NULL || pNewPath == NULL)
	{
		ERROR3("Null inputs to OpDrawBrush::SimpleJoinBrush");
		return FALSE;
	}
	
	// first check to see if the edited path has a brush applied to it,
	// if it doesn't then we'll leave
	NodeAttribute* pAttr;
	AttrBrushType* pAttrBrush;
	pInsertedNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), &pAttr);
	if (pAttr == NULL)
		return TRUE;

	pAttrBrush = (AttrBrushType*)pAttr;
	// if our brush has a 'no brush' handle then likewise
	if (pAttrBrush->GetBrushHandle() == BrushHandle_NoBrush)
		return TRUE;

	// if our brush is not either timestamping or has a pressure list then we will leave aswell
	if (!pAttrBrush->ContainsPressureCache() && !pAttrBrush->IsTimeStamping())
		return TRUE;

	// Invalidate the brush rect
	NodePath* EditPath = StartPath;
	if (EditPath == NULL)
		EditPath = EndPath;

	if (EditPath != NULL)
	{
		pAttrBrush->ClearCachedRect();
		DocRect BRect = pAttrBrush->GetAttrBoundingRect(EditPath);
		EditPath->ReleaseCached();
		Spread* pSpread = Document::GetSelectedSpread();
		if (pSpread != NULL)
			DoInvalidateRegion(pSpread, BRect);
	}
	else
		return FALSE;

	// find out what type of join we have 
	SimpleJoinType JoinType = GetSimpleJoinType(pNewPath, &(pInsertedNode->InkPath));
	if (JoinType == JOINTYPE_NONE)
	{
		ERROR3("No join type in OpDrawBrush::SimpleJoinBrush");
		return TRUE;
	}

	// we need to know the distance along the original path where we want to insert the
	// new points.  As this is a simple join it will either be at the beginning or the end.
	// we also need to know the distance to the end of the new points, as this is required
	// for the undo.
	MILLIPOINT NewPathLength = (MILLIPOINT)pNewPath->GetPathLength();
	MILLIPOINT OldPathLength = -1;
	if (StartPath != NULL)
		OldPathLength = (MILLIPOINT)StartPath->InkPath.GetPathLength();
	else 
	{
		if (EndPath != NULL)
			OldPathLength = (MILLIPOINT)EndPath->InkPath.GetPathLength();
	}

	MILLIPOINT InsertDistance = -1;
	MILLIPOINT EndInsertDistance = -1;
	INT32 StartIndex = 0; //for the pressure insert

	// ask the attribute for its pressure cache
	CDistanceSampler* pAttrData = pAttrBrush->GetPressureCache();
	if (pAttrData == NULL)
		return FALSE;
	// according to the type of join we will want to insert our new data at the beginning or the
	// end, we may also want to reverse it.  We need the distance values to give to the pressure
	// generator function
	switch (JoinType)
	{
		case JOINTYPE_NEWSTART_TO_OLDSTART:
			InsertDistance = -1;
			EndInsertDistance = 0; // indicates that we are inserting at the start
			StartIndex = 0; 
			break;
		case JOINTYPE_NEWSTART_TO_OLDEND:
			InsertDistance = OldPathLength; // indicates that we are inserting at the end
			EndInsertDistance = -1;
			StartIndex = pAttrData->GetInternalIndexFromDistance(OldPathLength);
			if (StartIndex==-1) StartIndex = pAttrData->GetNumItems();
			break;
		case JOINTYPE_NEWEND_TO_OLDSTART:  
			InsertDistance = -1;
			EndInsertDistance = 0;
			StartIndex = 0; 
			break;
		case JOINTYPE_NEWEND_TO_OLDEND:
			InsertDistance = OldPathLength;
			EndInsertDistance = -1;
			StartIndex = pAttrData->GetInternalIndexFromDistance(OldPathLength);	
			if (StartIndex==-1) StartIndex = pAttrData->GetNumItems();
			
			break;
		default:
			ERROR3("Unknown join type in OpDrawBrush::SimpleJoinBrush");
			return FALSE;
	}
	
	
	// get ourselves a data cache to insert courtesy of our static function
	CDistanceSampler* pSampler = OpDrawBrush::GeneratePressureData(pAttrBrush, InsertDistance, EndInsertDistance, NewPathLength);
	if (pSampler == NULL)
		return FALSE;
	

	// make the action to insert the pressure list
	
	AddPressurePointsAction* pAction;
	UINT32 NumPoints = pSampler->GetNumItems();

	if (AddPressurePointsAction::Init(this, &UndoActions, pAttrBrush, pSampler, 
									StartIndex, NumPoints, &pAction) == AC_FAIL)
		return FALSE;
	
	// Invalidate the brush rect again
	if (EditPath != NULL)
	{
		pAttrBrush->ClearCachedRect();
		DocRect BRect = pAttrBrush->GetAttrBoundingRect(EditPath);
		EditPath->ReleaseCached();
		Spread* pSpread = Document::GetSelectedSpread();
		if (pSpread != NULL)
			DoInvalidateRegion(pSpread, BRect);
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL OpFreeHand::RetroSmoothBrush(NodePath* pNewPath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/2000
	Inputs:		pNewPath - The New curve that has just been drawn
			
	Returns:	TRUE 
	Purpose:	As above, the base class version does nothing

********************************************************************************************/

BOOL OpFreeHand::RetroSmoothBrush(NodePath* pNewNodePath)
{
	return TRUE;
}



/********************************************************************************************

>	BOOL OpFreeHand::ReverseBrushPressure()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/2000
	Inputs:		-
			
	Returns:	TRUE 
	Purpose:	The base class version does nothing, derived class should reverse its pressure cache

********************************************************************************************/

BOOL OpFreeHand::ReverseBrushPressure()
{
	return TRUE;
}

/********************************************************************************************

>	BOOL OpFreeHand::GetSimpleJoinType(NodePath* pNewNodePath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/2000
	Inputs:		pNewNodePath - The New Path that we want to join to an existing path
				ReversedPath
	Returns:	the type of join, (an enumerated type, see the header file)
	Purpose:	This function works out how the new path joins onto the existing path

********************************************************************************************/

SimpleJoinType OpFreeHand::GetSimpleJoinType(Path* pNewPath, Path* pExistingPath)
{
	if (pNewPath == NULL || pExistingPath == NULL)
		return JOINTYPE_NONE;

	// we must join to one of these paths
	if (StartPath == NULL && EndPath == NULL)
		return JOINTYPE_NONE;
	// get the new coordinate array
	DocCoord* pNewCoords = pNewPath->GetCoordArray();
	if (pNewCoords == NULL)
		return JOINTYPE_NONE;

	INT32 NumCoords = pNewPath->GetNumCoords();
	// get the start and end coordinates of the new path
	DocCoord NewStart = pNewCoords[0];
	DocCoord NewEnd   = pNewCoords[NumCoords-1];
	
	SimpleJoinType ReturnVal = JOINTYPE_NONE;
	/*
	DocCoord* pOldCoords = pExistingPath->GetCoordArray();
	if (pOldCoords != NULL)
	{
		INT32 NumOldCoords = pExistingPath->GetNumCoords();
		DocCoord OldStart = pOldCoords[0];
		DocCoord OldEnd = pOldCoords[NumOldCoords -1];

		// ok we don't have to be dead on here
		MILLIPOINT NewStartToOldStart = NewStart.Distance(OldStart);
		MILLIPOINT NewStartToOldEnd   = NewStart.Distance(OldEnd);
	
		// for some reason these must be reversed
		if (NewStartToOldStart < NewStartToOldEnd)
			ReturnVal = JOINTYPE_NEWSTART_TO_OLDSTART;
		else
			ReturnVal = JOINTYPE_NEWSTART_TO_OLDEND;
	}
	return Retural;
	*/
	if (StartPath != NULL)
	{
		DocCoord* pOldCoords = StartPath->InkPath.GetCoordArray();
		if (pOldCoords != NULL)
		{
			INT32 NumOldCoords = StartPath->InkPath.GetNumCoords();
			DocCoord OldStart = pOldCoords[0];
			DocCoord OldEnd = pOldCoords[NumOldCoords -1];

			// ok we don't have to be dead on here
			MILLIPOINT NewStartToOldStart = (MILLIPOINT)NewStart.Distance(OldStart);
			MILLIPOINT NewStartToOldEnd   = (MILLIPOINT)NewStart.Distance(OldEnd);
		
			
			if (NewStartToOldStart < NewStartToOldEnd)
				ReturnVal = JOINTYPE_NEWSTART_TO_OLDSTART;
			else
				ReturnVal = JOINTYPE_NEWSTART_TO_OLDEND;
		}
	}

	// if endpath is not null then we are joining the end of the new path to one of the
	// ends of the old path
	if (EndPath != NULL)
	{
		DocCoord* pOldCoords = EndPath->InkPath.GetCoordArray();
		if (pOldCoords != NULL)
		{
			INT32 NumOldCoords = EndPath->InkPath.GetNumCoords();
			DocCoord OldStart = pOldCoords[0];
			DocCoord OldEnd = pOldCoords[NumOldCoords-1];

			// ok we don't have to be dead on here
			const MILLIPOINT NewEndToOldStart = (MILLIPOINT)NewEnd.Distance(OldStart);
			const MILLIPOINT NewEndToOldEnd   = (MILLIPOINT)NewEnd.Distance(OldEnd);
		
			if (NewEndToOldStart < NewEndToOldEnd)
				ReturnVal = JOINTYPE_NEWEND_TO_OLDSTART;
			else
				ReturnVal = JOINTYPE_NEWEND_TO_OLDEND;
		}
	}

	return ReturnVal;		
}
/********************************************************************************************

>	BOOL OpFreeHand::Declare()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpFreeHand::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_FREE_HAND_TOOL),
								CC_RUNTIME_CLASS(OpFreeHand), 
								OPTOKEN_FREEHAND,
								OpFreeHand::GetState,
								0,	/* help ID */
								_R(IDBBL_FREEHANDTOOLOP),
								0	/* bitmap ID */));
}


/********************************************************************************************

>	OpState OpFreeHand::GetState(String_256* Description, OpDescriptor*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpFreeHand::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}

/********************************************************************************************

>	NodeBlend* OpFreeHand::GetParentOfNodePath()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	inputs :    -
	outputs:    
	returns:    pointer to the first nodeblend in the selection if successful, 
				NULL otherwise
	Purpose:	To determine if the current selection is a blend on a curve


*********************************************************************************************/

NodeGroup* OpFreeHand::GetParentOfNodePath()
{
	NodeGroup* pParent = NULL;
	if (m_pNewNodePath == NULL)  // if no blendpath node then there won't be a group parent
		return NULL;
	if (!m_pNewNodePath->IsANodeBlendPath())
		return NULL;

	pParent = (NodeGroup*)m_pNewNodePath->FindParent();
	
	ERROR2IF(!(pParent->IS_KIND_OF(NodeGroup)), NULL, "Node is not NodeGroup");
	return pParent;

}


/********************************************************************************************

>	void OpFreeHand::InsertChangeBlendStepsAction(NodeBlend* pNodeBlend)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/99
	Inputs:		the node blend to change
	Outputs:	-
	Returns:	-
	Purpose:	To adjust the number of steps in a blend on a path as a result of the path
				being edited (which is why its here rather than in the blend code).
				First the number of steps is calculated from the new path distance and if
				different a new action is inserted
	Errors:		Error2 if pNodeBlend is null
	SeeAlso:	called from OpNodePathEditBlob::DragFinished or 
				OpNodePathAddEndpoint::CompleteThisPath

********************************************************************************************/
BOOL OpFreeHand::InsertChangeBlendStepsAction(NodeBlend* pNodeBlend)
{
	
	ERROR2IF(pNodeBlend==NULL, FALSE, "NodeBlend pointer is NULL");
	// do we wish to keep the distance between steps constant?  If so then determine
	// how many steps we need with the new length and make an action.
	if (pNodeBlend->GetEditState() == EDIT_DISTANCE)
	{
		UINT32 NewNumSteps = 0;
		UINT32 OldNumSteps = pNodeBlend->GetNumBlendSteps();
		double DistanceEntered = pNodeBlend->GetDistanceEntered();
		NodeBlendPath* pNodeBlendPath = pNodeBlend->GetNodeBlendPath(0);
		double NewPathLength = pNodeBlendPath->GetPathLength();
		
		NewNumSteps = (UINT32)(NewPathLength/DistanceEntered);
		

		BOOL ok = TRUE;  
	
		if (OldNumSteps != NewNumSteps)
		{
			ChangeBlendStepsAction* pAction;
					ok = (InvalidateBoundsAction::Init(this,&UndoActions,pNodeBlend,TRUE) != AC_FAIL);
			if (ok) ok = ChangeBlendStepsAction::Init(this,&UndoActions,pNodeBlend,OldNumSteps,DistanceEntered, &pAction) != AC_FAIL;
			if (ok) ok = DoInvalidateNodeRegion(pNodeBlend,TRUE,FALSE);
			if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,pNodeBlend,TRUE) != AC_FAIL);
			pNodeBlend->SetNumBlendSteps(NewNumSteps);

			// added 9/9/99 now we are shifting the last object along the curve to ensure precise
			// step distances.  To do this we must set the proportion of the curve used in the 
			// NodeBlender objects.
			if (ok)
			{
				double PathDistanceUsed = NewNumSteps * DistanceEntered;
				double PathProportion = PathDistanceUsed / NewPathLength;
//				double PathDistanceUnused = NewPathLength - PathDistanceUsed;

				if (PathProportion != 1.0)
				{
					//ChangeBlenderAction* pBlenderAction;
					ChangeBlenderOpParam BlenderParam;
					BlenderParam.m_ChangeType = CHANGEBLENDER_PATHEND;
							
				
					NodeBlender* pNodeBlender = pNodeBlend->FindFirstBlender();
					INT32 NumBlenders = pNodeBlend->GetNumBlenders();
						
					while (pNodeBlender != NULL)
					{
						NumBlenders--;
						if (NumBlenders ==0)
						{									
							BlenderParam.m_NewPathEnd = PathProportion;
							ok = ChangeBlenderAction::Init(this, &UndoActions, pNodeBlender, BlenderParam);
							if (ok)
							{
								DocCoord NewPoint;
								double ExtraParam = 0.0;  //passed to the function but not used afterwards
								ok = pNodeBlender->GetPointOnNodeBlendPath(1.0,&NewPoint,&ExtraParam);

								if (ok)
								{
									NodeRenderableInk* pEnd = pNodeBlender->GetNodeEnd();
									NodeBlend* pNodeBlend = pNodeBlender->GetNodeBlend();

									ok = ((pEnd != NULL) && (pNodeBlend != NULL));
					
									if (ok) 
										ok = pNodeBlend->TransformNodeToPoint(pEnd,&NewPoint,this,ExtraParam);
								}
							}
						}	
						pNodeBlender = pNodeBlend->FindNextBlender(pNodeBlender);
					}
				} // end if (pathproportion..
			} // end if(ok)

		}
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL OpFreeHand::InvalidateBrushRegion(NodePath* pNodePath)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	inputs :    pNodePath - the nodepath that we have edited
	outputs:    
	returns:    TRUE if successful, or if we are not really editing a brush, FALSE if something
				went wrong
	Purpose:	We may be editing a brushed path with the FHT so this function makes sure the
				correct region is invalidated


*********************************************************************************************/

BOOL OpFreeHand::InvalidateBrushRegion(NodePath* pNodePath)
{
	ERROR2IF(pNodePath == NULL, FALSE, "Nodepath is NULL in OpFreeHand::InvalidateBrushRegion");
	
	AttrBrushType* pAttrBrush;
	NodeAttribute* pAttr = NULL;
	BrushHandle Handle = BrushHandle_NoBrush;
	pNodePath->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), &pAttr);
	if (pAttr != NULL)
	{
		pAttrBrush = (AttrBrushType*)pAttr;
		Handle = pAttrBrush->GetBrushHandle();
	}
	if (Handle == BrushHandle_NoBrush)
		return TRUE;

	pAttrBrush->ClearCachedRect();
	pAttrBrush->FlushCache();
	DocRect BRect = pNodePath->GetBoundingRect(); //pAttrBrush->GetAttrBoundingRect(pNodePath);
	pNodePath->ReleaseCached();
	pAttrBrush->ClearCachedRect();
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread != NULL)
		return DoInvalidateRegion(pSpread, BRect);	
	else 
		return FALSE;
}
