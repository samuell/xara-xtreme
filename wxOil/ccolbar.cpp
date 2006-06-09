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
// CColBar.cpp - implementation of the CColourBar class

/*
*/


	/*	IMPORTANT NOTE!
	 *	===============
	 *	
	 *	When working on the Colour Bar, it is VITAL that you remember that it
	 *	differs from much of Camelot in that it works exclusively on the
	 *	SelectedDocument (rather than CurrentDocument).
	 *	Thus, you need to be careful to ensure that Current == Selected if you
	 *	add any subroutine calls, or else you may shaft everything.
	 */



#include "camtypes.h"

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camelot.h"
#include "ccolbar.h"
#include "colcontx.h"
#include "coldlog.h"
#include "colmsg.h"
#include "colormgr.h"
#include "colourix.h"
#include "colpick.h"
#include "comattrmsg.h"
//#include "cursor.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ctrlhelp.h"
#include "devcolor.h"
//#include "doccolor.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "dragmgr.h"
//#include "galres.h"			// For drag cursors
//#include "ink.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "jason.h"
#include "keypress.h"
#include "lineattr.h"
#include "linwthop.h"
#include "camframe.h"
#include "opgrad.h"
#include "palman.h"
//#include "range.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h"		// For push tool cursor
#include "sgcolour.h"		// For AutoScroll preference
#include "statline.h"
#include "cartprov.h"

// define this to disable use of DragManagerOp to drag colours
//#define DISABLE_COLOUR_DRAGS

// --------- ****
// DocColours (doccolor.h) currently provide a special debugging form of MakeRef...
// that trace usage of indexedcolours back to the file/line where they were first
// referenced. However, the colourbar creates many temp. ref.s during redraws,
// so we disable this debugging stuff for this file. This is fairly safe, as we
// are reasonably sure that the colour bar treats IndexedColours with proper respect.
#undef MakeRefToIndexedColour
// ---------


// Default preference styles for the colour bar (size of strip cells and scrollbar)
// Note that the Create function reads the preference and clips it to 0..MAXDEFAULTSIZES range
typedef struct
{
	INT32 Cell;
	INT32 Scrollbar;
} ColBarDefaultSizeInfo;

const INT32 MAXDEFAULTSIZES = 4;
ColBarDefaultSizeInfo ColBarDefaultSize[MAXDEFAULTSIZES] = 
{
	{12, 0},  	// Small (no scroll bar)
	{16, 0},	// Medium (no scroll bar)
	{16, 9},	// Medium
	{24, 11}	// Large
};


/********************************************************************************************

	Preference:		ColourBarMode
	Section:		Displays
	Range:			0..3
	Purpose:		Specifies the 'size' at which the colour bar is displayed, with
					values of:

					MonoOn
						0	Small
						1	Medium, no scrollbar
						2	Medium with scrollbar (default)
						3	Large
					MonoOff

	Notes:			Normal preference, available from the options dialogue.
	SeeAlso:		ColourEditDlg

********************************************************************************************/

static INT32 DefaultSizeIndex = 2;	// User preference 'ColourBarMode'- indexes the above array


/********************************************************************************************

	Preference:		ColourCellsDivided
	Section:		Displays
	Range:			0 (off) or 1 (on)
	Purpose:		If set to 0, colour cells in the colour line will butt up against each
					other. If set to 1, a single pixel grey line will be used to divide the
					cells, to make them easier to distinguish.

********************************************************************************************/

static INT32 ColourCellsDivided = 0;



// Redraw Constants
const INT32 SCROLLBUTTON_LEFT  = 0;	// Scroll button identifiers
const INT32 SCROLLBUTTON_RIGHT = 1;

const INT32 CX_BORDER  = 2;			// Number of pixels of blank space around the edge
const INT32 CY_BORDER  = 0;			// (On top of the default 1 pixel border)

const INT32 InterControlGap = 2;		// The horizontal gap between the bar's 'controls'
const INT32 ButtonWidth	  = 11;		// Width of the scrollbar left/right buttons

// The following variables are calculated and filled in by SetSizes(), but
// I've put some reasonable defaults in. Note that on construction, CellSize
// and ScrollHeight are passed to SetSizes in order to calculate the others.

static INT32 BarHeight	  = 25;		// Total height of the bar window in pixels
static INT32 CellSize		  = 16;		// Each Colour Cell is NxN pixels in size - preference
static INT32 ScrollHeight	  = 9;  	// The mini scrollbar height in pixels

									// The No-Colour 'cell' is a normal cell size
#define NoColourSize (CellSize)

									// Width and height of the edit button
#define EditButtonSizeX (CellSize)
#define EditButtonSizeY (CellSize)


// Click values - this is; nothing, a colour index (0..?), or a scroll bar/arrow
const INT32 CLICKED_NOTHING		 = 10000;	// Nothing
const INT32 CLICKED_INDICATORS	 = 10001;	// Over the current colour indicators
const INT32 CLICKED_LEFTSCROLL	 = 10002;	// Over the left scroll arrow
const INT32 CLICKED_RIGHTSCROLL	 = 10003;	// Over the right scroll arrow
const INT32 CLICKED_SCROLLBAR		 = 10004;	// Over the scroll sausage
const INT32 CLICKED_SCROLLBARLEFT	 = 10005;	// Over the left end of the scrollbar
const INT32 CLICKED_SCROLLBARRIGHT = 10006;	// Over the right end of the scrollbar
const INT32 CLICKED_SCROLLSTRIP	 = 10007;	// Over the strip, with push modifier key
const INT32 CLICKED_NOCOLOURCELL	 = 10008;	// Over the 'No Colour' cell
const INT32 CLICKED_EDITBUTTON	 = 10009;	// Over the 'edit colour' button
const INT32 CLICKED_NEWBUTTON		 = 10010;	// Over the 'new colour' button


// Cell values for special cell 'indices' (the no-colour cell, and no-cell-at-all-mate)
const INT32 CELLINDEX_NOCOLOURCELL	= -1;
const INT32 CELLINDEX_NOCELL 			= -2;


// Static member variables
//ListComparator CColourBar::SortFunction = NULL;		// We have no sort mode by default
CColourBar *CColourBar::TheColourBar = NULL;		// There is no bar currently available
BOOL CColourBar::BarIsVisible = TRUE;				// Bars will default to being visible


IMPLEMENT_DYNAMIC_CLASS(CColourBar, wxWindow)
CC_IMPLEMENT_DYNAMIC(EditButtonDragTarget, OilDragTarget)
CC_IMPLEMENT_DYNAMIC(ColourLineDragTarget, OilDragTarget)




/********************************************************************************************

>	inline Document *SetCurrentDoc(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94
	Inputs:		-
	Outputs:	-
	Returns:	The old Current Document

	Purpose:	The ColourBar works exclusively on the SELECTED Doc.
				Thus, on entry to any of its routines which call routines
				outside this file (ccolbar.cpp), it must ensure that CurrentDoc
				is equal to SelectedDoc.
				This local macro inline does this, and returns the old setting of
				CurrentDoc so that the caller can restore the previous CurrentDoc
				on exit.

	Scope:		private (to winoil\ccolbar.cpp)
	SeeAlso:	RestoreCurrentDoc

********************************************************************************************/

inline Document *SetCurrentDoc(void)
{
	Document *OldCurrentDoc = Document::GetCurrent();
	Document *NewCurrentDoc = Document::GetSelected();

	if (NewCurrentDoc != NULL && NewCurrentDoc != OldCurrentDoc)
		NewCurrentDoc->SetCurrent();

	return(OldCurrentDoc);
}



/********************************************************************************************

>	inline void RestoreCurrentDoc(Document *OldCurrentDoc)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94
	Inputs:		The old current document to restore
	Outputs:	-
	Returns:	-

	Purpose:	The ColourBar works exclusively on the SELECTED Doc.
				After calling SetCurrentDoc on entry to a routine, you should call
				RestoreCurrentDoc on exit to restore the old current document./

	Scope:		private (to winoil\ccolbar.cpp)
	SeeAlso:	SetCurrentDoc

********************************************************************************************/

inline void RestoreCurrentDoc(Document *OldCurrentDoc)
{
	if (OldCurrentDoc != NULL)
		OldCurrentDoc->SetCurrent();
}


/********************************************************************************************

>	void EditButtonDragTarget::EditButtonDragTarget() 
	 
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
EditButtonDragTarget::EditButtonDragTarget(wxWindow* TheWindow, wxRect *ClientArea)
	: OilDragTarget(TheWindow, ClientArea)
{
//	TRACEUSER("Gerry", _T("EditButtonDragTarget created"));	
}



/********************************************************************************************

>	void EditButtonDragTarget::ProcessEvent(DragEventType Event,
						DragInformation *pDragInfo,
						wxPoint *pMousePos, KeyPress* pKeyPress) 
	 
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Event Handler for Edit Drag 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL EditButtonDragTarget::ProcessEvent(DragEventType Event,
						DragInformation *pDragInfo,
						wxPoint *pMousePos, KeyPress* pKeyPress)
{
//	TRACEUSER("Gerry", _T("EditButtonDragTarget::ProcessEvent"));

	// Not a colour drag? That is kindof unexpected, but lets exit before
	// we embarrass ourselves

	if (!pDragInfo->IsKindOf(CC_RUNTIME_CLASS(ColourDragInformation)))
		return(FALSE);

	switch(Event)
	{
		case DRAGEVENT_COMPLETED:
			if (CColourBar::TheColourBar != NULL &&
				CColourBar::TheColourBar->m_pCurrentColourList != NULL)
			{
				if (CanDropHere(pDragInfo))
				{
					ColourDragInformation *CDI = (ColourDragInformation *) pDragInfo;
					IndexedColour *Col = NULL;

					if (CDI->IsLibraryColour())
					{
						// We must copy the library colour into the document, but first check with
						// the user that this is what they intended.
					#ifndef WEBSTER
						// Camelot builds ask the user to check that this is what they want
						if (InformError(_R(IDE_CANTEDITLIBCOLOUR), _R(IDS_COPYLIBCOLOUR), _R(IDS_CANCEL)) == 1)
							Col = CDI->GetColourForDocument(Document::GetSelected());
					#else
						// Webster builds just do it. Simplify the UI for the user
						Col = CDI->GetColourForDocument(Document::GetSelected());
					#endif
					}
					else
						Col = CDI->GetInitiallyDraggedColour();

					if (Col != NULL)
						CColourBar::TheColourBar->EditAColour(CColourBar::TheColourBar->m_pCurrentColourList, Col);
					return(TRUE);
				}
			}
			break;


		case DRAGEVENT_MOUSESTOPPED:
		case DRAGEVENT_MOUSEMOVED:
		case DRAGEVENT_MOUSEIDLE:
			// Return TRUE to claim the mouse while over our target area, so that
			// our cursor shape is used
			return(TRUE);

		default:
			break;
 	}

	// Allow unknown/unwanted events to pass on to other targets
	return(FALSE);
}



/********************************************************************************************

>	void EditButtonDragTarget::GetCursorID()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> (Rewritten, Jason)
	Created:	10/1/95 (25/3/96)
	Purpose:	Set cursor over this target

********************************************************************************************/

UINT32 EditButtonDragTarget::GetCursorID()
{
//	TRACEUSER("Gerry", _T("EditButtonDragTarget::GetCursorID"));

	DragInformation *pDragInfo = DragManagerOp::GetCurrentDragInfo();
	if (CanDropHere(pDragInfo))
		return _R(IDC_CANDROP_EDITBUTTON);

	return 0;
}



/********************************************************************************************

>	virtual BOOL EditButtonDragTarget::GetStatusLineText(String_256 * TheText)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> (Rewritten, Jason)
	Created:	15/1/95 (25/3/96)
	Returns:	Whether String is valid
	Purpose:	provide status line text for this target
   
********************************************************************************************/

BOOL EditButtonDragTarget::GetStatusLineText(String_256 *TheText)
{
//	TRACEUSER("Gerry", _T("EditButtonDragTarget::GetStatusLineText"));

	ERROR2IF(TheText==NULL,FALSE,_T("NULL string in GetStatusLineText()"));

	// Call our helper function to find out if this can be dropped here
	DragInformation *pDragInfo = DragManagerOp::GetCurrentDragInfo();
	if (CanDropHere(pDragInfo))
	{
		String_256 DragString(_R(IDS_COLBAR_DRAGDROP));
		*TheText = DragString;
		return TRUE;
	}

	return FALSE;
}



/********************************************************************************************

>	BOOL EditButtonDragTarget::CanDropHere(DragInformation *pDragInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/96

	Returns:	Whether this is something that can be dropped on the edit button

	Purpose:	Internal helper function
   
********************************************************************************************/

BOOL EditButtonDragTarget::CanDropHere(DragInformation *pDragInfo)
{
//	TRACEUSER("Gerry", _T("EditButtonDragTarget::CanDropHere"));

	if (Document::GetSelected() == NULL)		// No selected docuement? can't edit anything then
		return(FALSE);

	if (pDragInfo != NULL && pDragInfo->IsKindOf(CC_RUNTIME_CLASS(ColourDragInformation)))
	{
		ColourDragInformation *CDInfo = (ColourDragInformation *) pDragInfo;

		// We always allow drops of library colours onto us
		if (CDInfo->IsLibraryColour())
			return(TRUE);

		// Get the dragged colour and the current colour list
		IndexedColour *Col  = CDInfo->GetInitiallyDraggedColour();

		if (Col != NULL)	// NULL means "no colour" or a library colour, which can't be edited
		{
			ColourList *ColList = NULL;
			if (CDInfo->GetParentDoc() != NULL)
				ColList = CDInfo->GetParentDoc()->GetIndexedColours();

			return(ColourEditDlg::CanYouEditThis(ColList, Col));
		}
	}

	return(FALSE);
}


/********************************************************************************************

>	void ColourLineDragTarget::ColourLineDragTarget() 
	 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
ColourLineDragTarget::ColourLineDragTarget(wxWindow* TheWindow, wxRect *ClientArea)
	: OilDragTarget(TheWindow, ClientArea)
{
//	TRACEUSER("Gerry", _T("ColourLineDragTarget created"));
}



/********************************************************************************************

>	void ColourLineDragTarget::ProcessEvent(DragEventType Event,
						DragInformation *pDragInfo,
						wxPoint *pMousePos, KeyPress* pKeyPress) 
	 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95		  

	Purpose:	Event Handler for Edit Drag - see the base class for details

	SeeAlso:	WinoilDragTarget::ProcessEvent

********************************************************************************************/

BOOL ColourLineDragTarget::ProcessEvent(DragEventType Event,
						DragInformation *pDragInfo,
						wxPoint *pMousePos, KeyPress* pKeyPress)
{
//	TRACEUSER("Gerry", _T("ColourLineDragTarget::ProcessEvent"));

	// Not a colour drag? That is kindof unexpected, but lets exit before
	// we embarrass ourselves
	if (!pDragInfo->IsKindOf(CC_RUNTIME_CLASS(ColourDragInformation)))
		return(FALSE);

	CColourBar *pColourLine = CColourBar::TheColourBar;

	switch(Event)
	{
		case DRAGEVENT_COMPLETED:
			if (KeyPress::IsConstrainPressed() &&
				pColourLine != NULL && pColourLine->m_pCurrentColourList != NULL)
			{
				IndexedColour *Col = ((ColourDragInformation *) pDragInfo)->GetInitiallyDraggedColour();

				// Check that the colour being dragged is in the current colour list
				if (Col != NULL && pColourLine->m_pCurrentColourList->FindPosition(Col) < 0)
					Col = NULL;

				// If it's NULL, then it's "no colour", a library colour, or a colour we couldn't
				// find in the current colour line display, so we ignore it
				if (Col != NULL)
				{
					// The Colour 'Col' has been dragged and dropped into the colour line
					// with the 'constrain' key held down. In this case, the user wants
					// to rearrange the colour line, so we detect which colour the pointer
					// is over, and insert 'Col' before/after that colour.

					BOOL ToLeft = FALSE;
					INT32 CellIndex = pColourLine->WhereIsMouse(*pMousePos, NULL, &ToLeft);

					if (CellIndex >= 0)
					{
						// The colour was dropped onto a legal colour cell - move it
						ColourList *TheList = pColourLine->m_pCurrentColourList;
						ERROR3IF(TheList == NULL, _T("No current colour list!"));
						
						IndexedColour *Target = (IndexedColour *) TheList->GetUndeletedHead();
						while (CellIndex > 0 && Target)
						{
							Target = (IndexedColour *) TheList->GetUndeletedNext(Target);
							CellIndex--;
						}

						// Now move the item into the appropriate position
						if (Target != NULL && Target != Col)
						{
							if (ToLeft)
							{
								if (TheList->GetUndeletedNext(Col) != Target)
								{
									TheList->RemoveItem(Col);
									TheList->InsertBefore(Target, Col);

									// And inform the world (colour line, gallery, etc) of the change
									ColourManager::ColourListHasChanged(TheList);
								}
							}
							else
							{
								if (TheList->GetUndeletedNext(Target) != Col)
								{
									TheList->RemoveItem(Col);
									TheList->InsertAfter(Target, Col);

									// And inform the world (colour line, gallery, etc) of the change
									ColourManager::ColourListHasChanged(TheList);
								}
							}
						}
					}
				}

				return(TRUE);
			}
			break;


		case DRAGEVENT_MOUSESTOPPED:
		case DRAGEVENT_MOUSEMOVED:
		case DRAGEVENT_MOUSEIDLE:
			// Return TRUE to claim the mouse while over our target area, so that
			// our cursor shape is used
			return(TRUE);

		default:
			break;
 	}

	// Allow unknown/unwanted events to pass on to other targets
	return(FALSE);
}



/********************************************************************************************

>	void ColourLineDragTarget::GetCursorID()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Purpose:	Base Method to set cursor over this target


********************************************************************************************/

UINT32 ColourLineDragTarget::GetCursorID()
{
//	TRACEUSER("Gerry", _T("ColourLineDragTarget::GetCursorID"));

	DragInformation * pDragInfo = DragManagerOp::GetCurrentDragInfo();
	if (pDragInfo != NULL && KeyPress::IsConstrainPressed())
	{
		if (pDragInfo->IsKindOf(CC_RUNTIME_CLASS(ColourDragInformation)))
		{
			IndexedColour *Col = ((ColourDragInformation *) pDragInfo)->GetInitiallyDraggedColour();

			if (Col != NULL)
			{
				if (CColourBar::TheColourBar != NULL)
				{
					BOOL CanDrop = FALSE;

					// Determine if the mouse cursor should be "insert on left" or
					// insert on right" of the colour under the pointer
					BOOL ToLeft = FALSE;
					wxPoint MousePos = wxGetMousePosition();
					MousePos = CColourBar::TheColourBar->ScreenToClient(MousePos);

					INT32 CellIndex = CColourBar::TheColourBar->WhereIsMouse(MousePos, NULL, &ToLeft);
					if (CellIndex >= 0)
					{
						// The pointer is over a legal colour cell
						ColourList *TheList = CColourBar::TheColourBar->m_pCurrentColourList;
						ERROR3IF(TheList == NULL, _T("No current colour list!"));
					
						IndexedColour *Target = (IndexedColour *) TheList->GetUndeletedHead();
						while (CellIndex > 0 && Target != NULL)
						{
							Target = (IndexedColour *) TheList->GetUndeletedNext(Target);
							CellIndex--;
						}

						// Now, check that it is a position in which the colour can be dropped.
						// We disallow any drop which will leave the colour where it was.
						if (Target != NULL && Target != Col)
						{
							if (ToLeft)
								CanDrop = (TheList->GetUndeletedNext(Col) != Target);
							else
								CanDrop = (TheList->GetUndeletedNext(Target) != Col);
						}

						// If we are sure it's OK to drop here, we return the cursor to use
						if (CanDrop)
						{
						 	if (ToLeft)
							 	return _R(IDC_DROPLEFT);

					 		return _R(IDC_DROPRIGHT);
						}

						// else drop out to "no-drop" cursor
					}
				}
			}
		}
	}

	// Ask for "can't drop here" cursor. Really ought to rename this resource ID soon!
	return _R(IDC_DRAGGING_COLOUR);
}



/********************************************************************************************

>	virtual BOOL ColourLineDragTarget::GetStatusLineText(String_256 * TheText)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Returns:	Whether String is valid
	Purpose:	provide status line text for this target
   
********************************************************************************************/

BOOL ColourLineDragTarget::GetStatusLineText(String_256 * TheText)
{
//	TRACEUSER("Gerry", _T("ColourLineDragTarget::GetStatusLineText"));

	ERROR2IF(TheText==NULL,FALSE,_T("NULL string in GetStatusLineText()"));

	DragInformation * pDragInfo = DragManagerOp::GetCurrentDragInfo();

	if (pDragInfo != NULL && CColourBar::TheColourBar != NULL)
	{
		if (pDragInfo->IsKindOf(CC_RUNTIME_CLASS(ColourDragInformation)))
		{
			if (((ColourDragInformation *) pDragInfo)->GetInitiallyDraggedColour() != NULL)
			{
				// If we are sure it's OK to drop here, we return the cursor to use
		 		String_256 DragString(_R(IDS_COLBAR_REARRANGE));
				*TheText = DragString;
				return TRUE;
			}
		}
	}

	return FALSE;
}



BEGIN_EVENT_TABLE( CColourBar, wxWindow )
	EVT_LEFT_DOWN(		CColourBar::OnLButtonDown)
	EVT_LEFT_DCLICK(	CColourBar::OnLButtonDblClk)
	EVT_LEFT_UP(		CColourBar::OnLButtonUp)
	EVT_RIGHT_DOWN(		CColourBar::OnRButtonDown)
	EVT_RIGHT_DCLICK(	CColourBar::OnRButtonDblClk)
	EVT_RIGHT_UP(		CColourBar::OnRButtonUp)
	EVT_MOTION(			CColourBar::OnMouseMove)
	EVT_TIMER(		2,	CColourBar::OnTimer)
	EVT_PAINT(			CColourBar::OnPaint)
	EVT_SIZE(			CColourBar::OnSize)
END_EVENT_TABLE()


/********************************************************************************************

>	CColourBar::CColourBar()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	CColourBar constructor
	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

CColourBar::CColourBar()
{
	m_pCurrentColourList = NULL;

	LeftmostColour = 0;			// Initially, are at left end of the colourstrip

	LastLineCell = CELLINDEX_NOCELL;	// We have not drawn any indicator diamonds anywhere
	LastFillCell = CELLINDEX_NOCELL;
	LastEndCell  = CELLINDEX_NOCELL;
	LastDiamondShape = TRUE;

	IndentedButton = CLICKED_NOTHING;

	m_cxLeftBorder = 1;			// Automatic 1 pixel gap at left end of bar
	m_cyTopBorder = 0;
	m_cxRightBorder = 0;
	m_cyBottomBorder = 0;

	TheColourBar = this;

	MsgHandler = NULL;

	m_DragTimer.SetOwner(this, 2);

	DragInfo.TimerOn = 0;
	DragInfo.MouseCaptured = FALSE;

	PushCursor = NULL;

	OldBarHeight = 0;

	// We have no current colour gallery to access the fixed gallery section colours
	m_pColourGallery = NULL;
}



/********************************************************************************************

>	BOOL CColourBar::~CColourBar()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys the object, as one might expect.
	Notes:		IMPORTANT - After calling the destructor (delete ColourBar) it is
				VITAL that you then call RecalcLayout() for the main frame window in
				which the Colour Bar is appearing (the one you passed in to PB::Create().
				If you don't do this, the bar won't disappear until the next time the main 
				window is resized or a tool is chosen.

				The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

CColourBar::~CColourBar()
{
	TheColourBar = NULL;	// There is no longer a colour bar around	

	if (MsgHandler != NULL)
		delete MsgHandler;	// Kill our message handler

	ReleaseAllClaims();		// Ensure all temp. window claims (mouse/timer) released

	if (PushCursor)
		delete PushCursor;
}



/********************************************************************************************

>	BOOL CColourBar::InitPrefs(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the initialisation of the Colour bar prefs was successful.
	Purpose:	Initialisation of ColourBar preference option(s)
	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

BOOL CColourBar::InitPrefs(void)
{
	if (MsgHandler == NULL)
		MsgHandler = new ColourBarMsgHandler;

	if (MsgHandler == NULL)
		return(FALSE);

	Camelot.DeclareSection(TEXT("Displays"), 8);
	Camelot.DeclarePref(TEXT("Displays"), TEXT("ColourBarMode"), &DefaultSizeIndex);
	Camelot.DeclarePref(TEXT("Displays"), TEXT("ColourCellsDivided"), &ColourCellsDivided);

	return(TRUE);
}



/********************************************************************************************

>	BOOL CColourBar::Create(CFrameWnd *ParentWnd,
							DWORD dwstyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
							UINT32 nID = _R(AFX_IDW_STATUS_BAR));

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Inputs:		ParentWnd - The parent frame window (in Camelot, there can be only one)
				dwstyle, nID - you shouldn't have to mess with these (cf CStatusBar)
	Outputs:	-
	Returns:	TRUE if the initialisation of the Colour bar was successful.
	Purpose:	Creates a window and associates it with this CColourBar object. The window
				appears at the bottom of the main frame window, just like the status bar.

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

BOOL CColourBar::Create(wxWindow* pParent, UINT32 id)
{
	if (!wxWindow::Create(pParent, id, wxDefaultPosition, wxSize(BarHeight, BarHeight), wxNO_BORDER))
		return(FALSE);

#if FALSE
	ASSERT_VALID(pParentWnd);   // must have a parent

	// Create a new WndClass- this should be identical to CControlBar's WndClass but
	// we also would kindof like double clicks if it isn't too much trouble, Mr MFC
	CString WndClassName = AfxRegisterWndClass(CS_DBLCLKS, NULL, //Cursor::Arrow->Handle(),
									(HBRUSH)(COLOR_BTNFACE + 1));

	// Do we want to show this bar after it's created?
	BOOL Show = dwStyle & WS_VISIBLE;

	// Make sure the style flags only has legal flags set (VC6 change)
	dwStyle &= CBRS_ALL;

#if WIN32
	// this element of CControlBar does not exist in MFC 2.5 16-bit
	m_dwStyle = dwStyle;
#endif

	// create the wxWindow*
	wxRect rect;
	rect.SetRectEmpty();
	if (!CWnd::Create(WndClassName, NULL, dwStyle, rect, pParentWnd, nID))
		return(FALSE);
		// NOTE: Parent must resize itself for control bar to be resized

	if (Show)
		ShowWindow(SW_SHOW);
#endif

	InitPrefs();
	SetDisplayMode();

	InvalidateBestSize();

	IndentedButton = CLICKED_NOTHING;

	return(TRUE);
}



/********************************************************************************************

>	wxSize CColourBar::DoGetBestSize() const

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/03/06
	Inputs:		-
	Outputs:	-
	Returns:	Best size of this window

********************************************************************************************/

wxSize CColourBar::DoGetBestSize() const
{
	TRACEUSER("Gerry", _T("CColourBar::DoGetBestSize"));
	wxRect AvailableRect = GetParent()->GetClientRect();
	AvailableRect.height = BarHeight;
	INT32 Height = ((CColourBar*)this)->CalculateNewBarHeight(&AvailableRect);
	INT32 Width = GetParent()->GetClientSize().x;

	return(wxSize(Width, Height));
}

/********************************************************************************************

>	void CColourBar::SetSizes(INT32 NewCellSize, INT32 NewScrollbarSize)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		NewCellSize - The new size in pixels of the square colour cells
				This should be an even number (it is truncated if necessary)

				NewScrollbarSize - The new height of the scroll bar in pixels

	Outputs:	-
	Returns:	-

	Purpose:	Sets the base sizes of the colour strip and colour scroll bar
				in the colour bar window. Other features of the bar are automatically
				resized to fit the new sizing. The bar will be redrawn. If the 
				combined height of the bar elements has now changed, the bar will
				be resized (by calling the parent's RecalcLayout method).

	Notes:		Setting NewScrollBarSize == 0 enters a special mode where the scroll
				sausage is not drawn, and the scroll arrows sit at either end of the
				colour strip. In this size, it is recommended that
				CellSize == ButtonWidth (i.e. about 10 pixels), to make the buttons
				square and therefore not entirely disgusting looking

				The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

void CColourBar::SetSizes(INT32 NewCellSize, INT32 NewScrollbarSize)
{
	ENSURE (NewCellSize >= 4 && NewCellSize <= 64,
		"Silly CellSize passed to CColourBar::SetSizes()");

	ENSURE (NewScrollbarSize == 0 ||
			(NewScrollbarSize >= 3 && NewScrollbarSize <= 24),
		"Silly ScrollbarSize passed to CColourBar::SetSizes()");

	NewCellSize &= ~1;		// Force Cellsize to be an even value

//	BOOL ResizeFrame = (NewCellSize + NewScrollbarSize != CellSize + ScrollHeight);

	CellSize = NewCellSize;
	ScrollHeight = NewScrollbarSize;

	BarHeight = CellSize + ScrollHeight + (2 * CY_BORDER) +
					m_cyTopBorder + m_cyBottomBorder + 1;

//	if (ResizeFrame)
	{
		SetMinSize(wxSize(8, BarHeight));

		InvalidateBestSize();
		GetMainFrame()->UpdateFrameManager();
	}
}



/********************************************************************************************

>	static void CColourBar::SetDisplayMode(INT32 NewDisplayMode = -1)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94

	Inputs:		NewDisplayMode - The mode (0..3) or -1 to re-read the preference value

	Purpose:	Sets the Colour Bar display mode to the new value.
				Current legal mode values are 0..3 (small, med, med+scroll, large)
				Illegal values will cause it to resort to the default mode, medium+scroll (2)
				The default value of -1 is used to mean 'use the preference setting'.
				This allows the prefs dialogue to cause the ColourBar to sit up and
				take notice of changed preference settings.
				The preference setting ColourBarMode will be set to the new value

	Notes:		This static function may be called at any time. If a colour bar is not
				open when the call is made, the new setting will be used for the next
				colour bar created.

********************************************************************************************/

void CColourBar::SetDisplayMode(INT32 NewDisplayMode)
{
	if (NewDisplayMode == -1)
		NewDisplayMode = DefaultSizeIndex;

	if (NewDisplayMode < 0 || NewDisplayMode >= MAXDEFAULTSIZES)  
		NewDisplayMode = 2;

	DefaultSizeIndex = NewDisplayMode;		// Remember this as a preference setting

	if (TheColourBar != NULL)				// If there's a bar, set it to the new size
	{
		TheColourBar->SetSizes(ColBarDefaultSize[NewDisplayMode].Cell,
								ColBarDefaultSize[NewDisplayMode].Scrollbar);
		TheColourBar->Refresh();			// Force a redraw of the bar to ensure it's up to date
	}
}




/********************************************************************************************

>	INT32 CColourBar::GetNumberOfColours()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/97
	Inputs:		-
	Returns:	The number of colours we wish to display on the bar.
	Purpose:	Determines the number of colours that we wish to display on the ColourBar.

********************************************************************************************/

INT32 CColourBar::GetNumberOfColours()
{
	INT32 total = 0;
	
	// Get the actually colours in the selected document
	INT32 DocColours = 0;

	// If the user has the preference set then include and hence show the document
	// colours
	if (ColourSGallery::ShowDocumentColours)
	{
		if (m_pCurrentColourList == NULL)
			m_pCurrentColourList = ColourManager::GetColourList();

		if (m_pCurrentColourList != NULL)
			DocColours = (INT32) m_pCurrentColourList->GetUndeletedCount();

		total += DocColours;
	}

PORTNOTE("other","Removed ColourSGallery usage")
#if !defined(EXCLUDE_FROM_XARALX)
	// try and add in any fixed sections required
	m_pColourGallery = ColourSGallery::GetInstance();
	if (m_pColourGallery != NULL)
	{
		// This will create the groups if they're not there yet
		m_pColourGallery->MakeSureGroupsHaveBeenCreated();

		// Find the Netscape palette library
		SGDisplayLibColGroup * pLibGroup = m_pColourGallery->GetFirstLibGroup();
		BOOL Found = FALSE;
		while (!Found && pLibGroup)
		{
			// Add in any items that are flagged as being required
			if (pLibGroup->DisplayInColourLine())
			{
				// pLibGroup->DeVirtualise();	// This is dangerous as it may change things like current doc!
												// Disabled as it causes problems during start up
				total += pLibGroup->CountChildren();
			}

			pLibGroup = m_pColourGallery->GetNextLibGroup(pLibGroup);
		}
	}
#endif
	return total;
}

/********************************************************************************************

>	static BOOL CColourBar::EnsureLibraryColoursPresent()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/97
	Inputs:		-
	Returns:	True if worked correctly
	Purpose:	Ensures that any colour library sections we require are present.

********************************************************************************************/

BOOL CColourBar::EnsureLibraryColoursPresent()
{
PORTNOTE("other","Removed ColourSGallery usage")
#if !defined(EXCLUDE_FROM_XARALX)
	// try and ensure all our required colour gallery sections are present and de-virtilised
	ColourSGallery * pColourGallery = ColourSGallery::GetInstance();
	if (pColourGallery != NULL)
	{
		// This will create the groups if they're not there yet
		pColourGallery->MakeSureGroupsHaveBeenCreated();

		// Find the Netscape palette library
		SGDisplayLibColGroup * pLibGroup = pColourGallery->GetFirstLibGroup();
		while (pLibGroup)
		{
			// Add in any items that are flagged as being required
			if (pLibGroup->DisplayInColourLine())
			{
				pLibGroup->DeVirtualise();
			}

			pLibGroup = pColourGallery->GetNextLibGroup(pLibGroup);
		}

		if (TheColourBar != NULL)			// If there's a bar, set it to the new size
		{
			// Force a redraw of the bar to ensure it's up to date
			// This checks that the size is correct
			PaletteHasChanged(NULL);
			//TheColourBar->Invalidate();		
		}
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL CColourBar::IsColourPickerOverStripRect (wxWindow* colourPicker, wxPoint mousePt)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/99
	Inputs:		colourPicker - The wxWindow* of the claimed colour picker control
				mousePt - the current mouse location that the colour picker is supplying
	Returns:	True if mousePt is in StripRect
				False otherwise
				Throws ENSURE error if colourPicker is NOT a colour picker control
	Purpose:	This function helps to determin how colour picker controls should
				behave when they are over the colour bar

********************************************************************************************/

BOOL CColourBar::IsColourPickerOverStripRect (wxWindow* colourPicker, wxPoint mousePt)
{
	return((colourPicker == this) && StripRect.Inside(mousePt));

#if FALSE
	// firstly, lets check for obvious insanity
	String_256 ClassNameStr;  // The control type

	// Find out the class type of the gadget
	GetClassName (colourPicker, (TCHAR*) ClassNameStr, 255);

	//BOOL returnVal = FALSE;

	if (ClassNameStr == String_8(TEXT("cc_colPicker")))
	{
		if (StripRect.PtInRect(mousePt))
		{
			return (TRUE);
		}
		else
		{
			return (FALSE);
		}
	}

	ENSURE(FALSE, _T("IsColourPickerOverStripRect called for invalid colour picker control"));
	return (FALSE);
#endif
}

/********************************************************************************************

>	BOOL DoColourPickerColour (wxWindow* colourPicker, wxPoint mousePt, IndexedColour ** pTheCol=NULL)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/99
	Inputs:		colourPicker - The wxWindow* of the claimed colour picker control
	Returns:	True if success
				False otherwise
				Throws ENSURE error if colourPicker is NOT a colour picker control
	Purpose:	Instruct the colour edit dialog to use that colour on the colour bar (of which
				the colour picker is over).

********************************************************************************************/

BOOL CColourBar::DoColourPickerColour (wxWindow* colourPicker, wxPoint mousePt, IndexedColour **pTheCol/*=NULL*/)
{
	if (colourPicker == this)
	{
		UINT32 ColourCell = WhereIsMouse (mousePt);
		static UINT32 lastColourCell = (UINT32)-1;

		if ((((INT32)ColourCell) >= 0)/* && (lastColourCell != ColourCell)*/)		// we are over a valid colour
		{
			BOOL validColour = FALSE;
			
			if (m_pCurrentColourList == NULL)		// Ensure the current ColourList pointer is valid
			{
				m_pCurrentColourList = ColourManager::GetColourList();
			}

			IndexedColour *TheColour = NULL;

			// If the user has the preference set then include and hence show the document
			// colours
			if (ColourSGallery::ShowDocumentColours)
			{
				TheColour = FindColourByIndex(ColourCell);
			}

			if (TheColour != NULL)
			{
				validColour = TRUE;
			}
			else
			{
				// We didn't find the colour in the document colours section
				// so check any library sections that may be present
				SGDisplayLibColour *pLibColour = NULL;
				DocColour * pTheDocColour = FindLibColourByIndex(ColourCell, &pLibColour);
				TheColour = ColourManager::GenerateNewUnnamedColour (m_pCurrentColourList, pTheDocColour);

				if (pTheDocColour && pLibColour)
				{
					validColour = TRUE;
				}
			}

			if (validColour && pTheCol)
				*pTheCol = TheColour;
			
			if ((lastColourCell != ColourCell) && validColour)
			{
				ColourEditDlg* pColourEditDlg = ColourEditDlg::GetColourEditDlg ();
				ASSERT (pColourEditDlg);

				pColourEditDlg->SetEditingColour (TheColour);
				lastColourCell = ColourCell;
			}
			
			return (TRUE);
		}
		else
		{
			return (TRUE);
		}
	}

	ENSURE(FALSE, "DoColourPickerColour called for invalid colour picker control");

	return (FALSE);
}


/********************************************************************************************

>	BOOL CColourBar::HasNoScrollBar(INT32 DisplayedColours, INT32 MaxColours,
									wxRect *pStripRect = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94

	Inputs:		DisplayedColours - -1 or the number of colours that can be displayed in
				the bar
				MaxColours - -1 or the number of colours available for display
 				(these values will be calculated for you if you pass in -1, but
				if you've already calculated them it's silly to reinvent the wheel)

				pStripRect - NULL to use the precalculated CColourBar StripRect, or
				a pointer to a wxRect that holds a StripRect. (Used when calculating
				things based on a projected window size rather than the current
				cached window size)

	Returns:	TRUE if there is no need for a scroll bar
	Purpose:	Determines if the ColourBar actually need bother with a scrollbar
				(It does not need one if it can display all the available colours)

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

BOOL CColourBar::HasNoScrollBar(INT32 DisplayedColours, INT32 MaxColours, wxRect *pStripRect)
{
	if (BarHeight == 0)				// In small mode, always has arrows
		return(FALSE);

	if (pStripRect == NULL)			// By default, use our 'global' strip rectangle
		pStripRect = &StripRect;

	if (DisplayedColours < 0)
		DisplayedColours = pStripRect->width / CellSize;

	if (MaxColours < 0)
	{
		MaxColours = GetNumberOfColours();

		/* FIXEDCOLOURS
		if (m_pCurrentColourList == NULL)
			m_pCurrentColourList = ColourManager::GetColourList();

		if (m_pCurrentColourList == NULL)
			return(FALSE);

		MaxColours = (INT32) m_pCurrentColourList->GetUndeletedCount();
		*/
	}

	return(MaxColours <= DisplayedColours);
}



/********************************************************************************************

>	void CColourBar::CalculateRects(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Inputs:		-
	Outputs:	The following data members are calculated on exit:
				WindowRect		- The usable client-are of the ColourBar window
				StripRect		- The area in which the colour cells are drawn
				IndicatorRect	- The area the current colour indicator is drawn in
				ScrollBarRect	- The area in which the scrollbar is drawn
				EditButtonRect	- The area in which the Edit Button is drawn
				NewButtonRect	- The area in which the New Button is drawn [DEFUNCT]
	
	Returns:	-
	Purpose:	Calculates various useful rectangular regions to be found within the
				Colour Bar window. Returns the screen rectangles ready for redraw.

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

				IMPORTANT: If you change the rect calculations, be sure to change the 2
				occurences of a related calculation in CColBar::CalculateNewBarHeight, below

	SeeAlso:	CColourBar::CalculateScrollButtonRects;
				CColourBar::CalculateRectsInternal

********************************************************************************************/

void CColourBar::CalculateRects(void)
{
	// Calculate inside of window area
	WindowRect = GetClientRect();

	CalculateRectsInternal(&WindowRect,
							&StripRect, &IndicatorRect, &ScrollBarRect,
							&EditButtonRect, &NewButtonRect);

	// Now, calculate the inside rect (the area of the window which lies inside the borders
	// which have been drawn by the base class DoPaint() function). The only things that
	// should be using this rectangle (rather than the rects calculated above) should be
	// the 2 bits in the DoPaint method which fill undrawn areas of the bar with grey.
//	CalcInsideRect(&WindowRect);
}



/********************************************************************************************

>	void CColourBar::CalculateRectsInternal(wxRect *pWindowRect,
						wxRect *pStripRect, wxRect *pIndicatorRect, wxRect *pScrollBarRect,
						wxRect *pEditButtonRect, wxRect *pNewButtonRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Inputs:		pWindowRect		- The usable client-area of the ColourBar window

	Outputs:	pStripRect		- The area in which the colour cells are drawn
				pIndicatorRect	- The area the current colour indicator is drawn in
				pScrollBarRect	- The area in which the scrollbar is drawn
				pEditButtonRect	- The area in which the Edit Button is drawn
				pNewButtonRect	- The area in which the New Button is drawn [DEFUNCT]

				NONE of these inputs/outputs may be NULL!
	
	Returns:	-
	Purpose:	Calculates various useful rectangular regions to be found within the
				Colour Bar window. Returns the screen rectangles ready for redraw.

				Identical to the CalculateRects function, but you pass in the rects for
				it to plonk the results into. WindowRect is untouched. All other rects
				are updated from WindowRect. The contents on entry of all but WindowRect
				are unimportant.

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

				USE CalculateRects UNLESS ABSOLUTELY NECESSARY

	SeeAlso:	CColourBar::CalculateRects

********************************************************************************************/

void CColourBar::CalculateRectsInternal(wxRect *pWindowRect,
						wxRect *pStripRect, wxRect *pIndicatorRect, wxRect *pScrollBarRect,
						wxRect *pEditButtonRect, wxRect *pNewButtonRect)
{
	ERROR3IF(pWindowRect == NULL || pStripRect == NULL || pIndicatorRect == NULL ||
			 pScrollBarRect == NULL || pEditButtonRect == NULL || pNewButtonRect == NULL,
				_T("NULL parameter passed to CColourBar::CalculateRectsInternal"));
	
	wxRect TempRect(*pWindowRect);

//	CalcInsideRect(&TempRect);
	TempRect.Inflate(-CX_BORDER, -CY_BORDER);

	// Calculate the current-colour indicator rectangle, and ensure it's a square
	*pIndicatorRect = TempRect;
	pIndicatorRect->width = pIndicatorRect->height;

	// Subtract the indicator width from the available space. We now always make the indicator
	// 'BarHeight' pixels wide: This means that there is a gap to its right when there is
	// no scrollbar (because the square indicator shrinks), but this means the colourstrip
	// width is not dependant on whether or not it has a scrollbar (which in turn is dependant
	// upon the colourstrip width - a nasty circular reference!)
	TempRect.x += BarHeight + InterControlGap;
	TempRect.width -= BarHeight + InterControlGap;

	// Calculate the new and edit button positions
	*pNewButtonRect = TempRect;
	pNewButtonRect->width = EditButtonSizeX;
	pNewButtonRect->height = EditButtonSizeY;

	*pEditButtonRect = *pNewButtonRect;
	// The new button isn't current present so make the edit button the same
//	pEditButtonRect->x += EditButtonSizeX + InterControlGap;

	// Subtract new, edit, and no-colour buttons from the TempRect
	TempRect.x += NoColourSize + EditButtonSizeX + (InterControlGap * 2);
	TempRect.width -= NoColourSize + EditButtonSizeX + (InterControlGap * 2);

	// Calculate scrollbar and colour strip rectangles in the remaining space
	*pStripRect = TempRect;

	// This may be called fairly early on so if no colour list then get out immediately
	if (m_pCurrentColourList == NULL)		// No colours, so give safe default and exit
	{
		*pScrollBarRect = TempRect;
		return;
	}

	// INT32 MaxColours = (INT32) m_pCurrentColourList->GetUndeletedCount(); FIXED COLOURS
	INT32 MaxColours = GetNumberOfColours();
	// Removed 4/8/97 Neville as causes a redraw problem when switching from document with two colours
	// in to one with none in.
/*	if (MaxColours <= 0)
	{
		pScrollBarRect->CopyRect(&TempRect);
		return;
	} */

	INT32 Width = pStripRect->width;

	// Normally, the buttons are flush with the ends of the colourstrip, but in
	// this mode, they sit outside the ends of the colourstrip instead
	if (ScrollHeight == 0)
		Width -= 2 * (ButtonWidth + 1);

	if (MaxColours * CellSize < Width)
		pStripRect->width = MaxColours * CellSize;
	else
		pStripRect->width = Width - (Width % CellSize);

	if (ScrollHeight == 0)
	{
		// We want no scroll bar, just arrows on either end of the colourstrip
		// As the arrows are drawn on either end (outside) of the scroll rect,
		// we must now move both rects one scroll button width to the right

		pStripRect->Offset(ButtonWidth + 1, 0);
		*pScrollBarRect = *pStripRect;

		// And then make the scroll bar rect slightly bigger so there is a small
		// gap between the buttons and the strip
		pScrollBarRect->Inflate(2, 0);
	}
	else
	{
		// If there is no need for a scrollbar, StripRect expands to fill the space
		if (HasNoScrollBar(-1, MaxColours))
		{
			// For now we will force the strip rect to only be CellSize high
			// because the window resizing code isn't working properly
			pStripRect->height = CellSize;
			return;
		}

		*pScrollBarRect = *pStripRect;
		// ScrollBarRect is only ScrollHeight pixels high.
		// We also subtract off the space for the scroll arrows
		pScrollBarRect->y = pScrollBarRect->y + pScrollBarRect->height - ScrollHeight;
		pScrollBarRect->height = ScrollHeight;
		pScrollBarRect->Inflate(-(ButtonWidth-1), 0);

		// Colour strip is immediately above scroll rect: same width, CellSize height
		pStripRect->height = CellSize;
		pStripRect->y = pScrollBarRect->y - CellSize;
	}
}



/********************************************************************************************

>	BOOL CColourBar::CalculateSausageRect(wxRect *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Inputs:		-
	Outputs:	If successful, Result contains the sausage rect
	Returns:	TRUE if it succeeds, FALSE if the scrollbar/sausage is non existent
	Purpose:	Calculates the scroll sausage area within the scroll bar rectangle
	Notes:		You must call CalculateRects and cache the value of TotalNumColours
				prior to calling this function - see DoPaint for everything that needs
				to be done.
				The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

BOOL CColourBar::CalculateSausageRect(wxRect *Result)
{
	// If the scroll bar hasn't got enough room to exist, it does not appear
	if (ScrollBarRect.width < 5)
		return(FALSE);

	// If we are in Med/Large mode and have no scrollbar, it doesn't appear
	if (HasNoScrollBar(-1, -1))
		return(FALSE);

	*Result = ScrollBarRect;

	UINT32 NumDisplayedColours = StripRect.width / CellSize;
	Result->Inflate(-1, -1);

	if (NumDisplayedColours < TotalNumColours)
	{
		UINT32 BarWidth = Result->width;
		// Calculate the midpoint and width of the scroll 'sausage'
		//   MidPointColour = LeftmostColour + NumDisplayedColours/2;
		//   Mid = (MidPointColour / TotalColours) * BarWidth;
		// We calculate everything at *2 magnification to avoid rounding errors
		UINT32 Mid = (((LeftmostColour * 2) + NumDisplayedColours) * BarWidth) /
								(TotalNumColours * 2);

		BarWidth = (BarWidth * NumDisplayedColours) / TotalNumColours;

		if (BarWidth < (UINT32)4)
			BarWidth = 4;			// Try to keep some sausage visible!

		Result->x += Mid - (BarWidth / 2);
		Result->width = BarWidth;

		// Clip the values in case of rounding errors causing a pixel or 2 error
		if (Result->x <= ScrollBarRect.x)
			Result->x = ScrollBarRect.x + 1;

		// Clip rounding error, also make sure that errors don't make bar 1 pixel
		// short when at the very rightmost scroll position (this douubles up the
		// black lines around scroll sausage/arrow and looks bad)
		if (Result->GetRight() >= ScrollBarRect.GetRight() ||
			TotalNumColours-NumDisplayedColours <= LeftmostColour)
			Result->SetRight(ScrollBarRect.GetRight() - 1);
	}

	return(TRUE);
}



/********************************************************************************************

>	void CColourBar::CalculateNoColourCellRect(wxRect *Rect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/94
	Inputs:		-
	Outputs:	Rect - will be filled in with the caclculated rect
	Returns:	-
	Purpose:	Calculates the 'NoColour' colour cell position, just to the left of
				the ColourStrip
	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

void CColourBar::CalculateNoColourCellRect(wxRect *Rect)
{
	*Rect = StripRect;
	if (ScrollHeight == 0)				// If in 'small' mode, correct for scroll arrow
		Rect->x -= ButtonWidth;
//	else if (!HasNoScrollBar(-1, -1))	// otherwise if there IS a scroll bar
//		Rect->Offset(0,1);				// then move the rectangle down a pixel

	Rect->x -= NoColourSize + InterControlGap;
	Rect->width = NoColourSize;
	Rect->height = NoColourSize;
}



/********************************************************************************************

>	static void Paint3dPlinth(wxDC* pDC, wxRect *rect, BOOL Indent)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Inputs:		pDC - The DC to draw into
				rect - The rectangle to draw around the OUTSIDE of with a plinth 
				PlinthOut - FALSE to indent, TRUE to pop-out the plinth.
	Outputs:	-
	Returns:	-
	Purpose:	Draws a rectangular 3d plinth around the edge of the given rectangle

********************************************************************************************/

static void Paint3dPlinth(wxDC* pDC, wxRect *rect, BOOL PlinthOut)
{
	wxPen Black(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
	wxPen White(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));

	wxPoint Points[3];
	pDC->SetPen((PlinthOut) ? White : Black);
	Points[0] = wxPoint(rect->x, rect->GetBottom());
	Points[1] = wxPoint(rect->x, rect->y);
	Points[2] = wxPoint(rect->x + rect->width, rect->y);
	pDC->DrawLines(3, Points);

	pDC->SetPen((PlinthOut) ? Black : White);
	Points[0] = wxPoint(rect->GetRight(), rect->y + 1);	// Ensure corner pixel is correct
	Points[1] = wxPoint(rect->GetRight(), rect->GetBottom());
	Points[2] = wxPoint(rect->x, rect->GetBottom());
	pDC->DrawLines(3, Points);

	pDC->SetPen(wxNullPen);
}



/********************************************************************************************

>	void CColourBar::PaintEditOrNewButton(wxDC *pDC, BOOL Normal, BOOL IsEditButton)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/94
	Inputs:		-
	Outputs:	pDC - The DC into which to draw
				Normal - FALSE for an indented button, TRUE for normal (raised)
				IsEditButton - TRUE to draw the Edit button, FALSE to draw the New button
	Returns:	-
	Purpose:	Draws the colour bar "edit colour" or "new colour" button
	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

void CColourBar::PaintEditOrNewButton(wxDC *pDC, BOOL Normal, BOOL IsEditButton)
{
	wxRect TheRect;									// Determine the rect to draw
	if (IsEditButton)
		TheRect = EditButtonRect;
	else
#if FALSE
		TheRect = NewButtonRect;
#else
	{
		ERROR3(_T("Attempt to paint 'new colour' button, which no longer exists!"));
		return;
	}
#endif

	wxPen FramePen(wxColour(0,0,0));
	pDC->SetPen(FramePen);
	wxBrush TransBrush(wxColour(0,0,0), wxTRANSPARENT);
	pDC->SetBrush(TransBrush);
	pDC->DrawRectangle(TheRect);
	pDC->SetPen(wxNullPen);
	pDC->SetBrush(wxNullBrush);

	TheRect.Inflate(-1, -1);					// ...with a plinth inside it
	Paint3dPlinth(pDC, &TheRect, Normal);

	wxPen TransPen(wxColour(0,0,0), 1, wxTRANSPARENT);
	pDC->SetPen(TransPen);
	wxBrush GreyBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));	// Fill with default colour
	TheRect.Inflate(-1, -1);
	pDC->SetBrush(GreyBrush);
	pDC->DrawRectangle(TheRect);
	pDC->SetBrush(wxNullBrush);
	pDC->SetPen(wxNullPen);

	ResourceID bitmap = 0;

	INT32 BmpSize = (CellSize > 15) ? 12 : 8;			// size of glyph bitmaps (8x8 and 12x12)
		
	if (IsEditButton)								// Load appropriate glyph bitmap
	{
		if (CellSize > 15)
			bitmap=_R(IDB_LEDITCOL);		// 12x12 Edit glyph
		else
			bitmap=_R(IDB_EDITCOL);			// 8x8 Edit glyph
	}
	else
	{
		if (CellSize > 15)
			bitmap=_R(IDB_LNEWCOL);			// 12x12 New glyph
		else
			bitmap=_R(IDB_NEWCOL);			// 8x8 New glyph
	}

	wxBitmap * pBitmap = CamArtProvider::Get()->FindBitmap(bitmap);

	TheRect.Inflate((BmpSize - TheRect.GetWidth())/2, (BmpSize - TheRect.GetHeight())/2);

	// Blit the glyph to screen, including its grey mask areas
	if (pBitmap)
		pDC->DrawBitmap(*pBitmap, TheRect.GetLeft(), TheRect.GetTop(), TRUE);
}



/********************************************************************************************

>	void CColourBar::PaintScrollButton(wxDC *pDC, INT32 ButtonID, BOOL Indented)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		-
	Outputs:	pDC - The DC into which to draw
				ButtonID - SCROLLBUTTON_LEFT or _RIGHT: specifies which one to draw
				Normal - FALSE for an indented button, TRUE for normal (raised)
	Returns:	-
	Purpose:	Draws a scroll button at the appropriate end of the scroll bar
				The button is scaled to fit into (ButtonWidth x ScrollHeight)
	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

void CColourBar::PaintScrollButton(wxDC *pDC, INT32 ButtonID, BOOL Normal)
{
	wxRect ButtonRect(ScrollBarRect);

	if (ButtonID == SCROLLBUTTON_LEFT)
		ButtonRect.x -= ButtonWidth - 1;
	else
		ButtonRect.x += ButtonRect.width - 1;

	ButtonRect.width = ButtonWidth;

	wxPen FramePen(wxColour(0,0,0));
	pDC->SetPen(FramePen);
	wxBrush TransBrush(wxColour(0,0,0), wxTRANSPARENT);
	pDC->SetBrush(TransBrush);
	pDC->DrawRectangle(ButtonRect);

	pDC->SetPen(wxNullPen);
	pDC->SetBrush(wxNullBrush);

	// Paint the button border plinth just inside the black outline
	ButtonRect.Inflate(-1, -1);
	Paint3dPlinth(pDC, &ButtonRect, Normal);

	// Calculate the left/right scroll arrow position
	INT32 CenterX = ButtonRect.x + (ButtonRect.width / 2);
	INT32 CenterY = ButtonRect.y + (ButtonRect.height / 2);
	INT32 offset  = (ButtonID == SCROLLBUTTON_LEFT) ? -1 : 1;

	wxPen TransPen(wxColour(0,0,0), 1, wxTRANSPARENT);
	pDC->SetPen(TransPen);

	// Fill the button face area with the user preference colour
	wxBrush GreyBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	ButtonRect.Inflate(-1, -1);
	pDC->SetBrush(GreyBrush);
	pDC->DrawRectangle(ButtonRect);

	// Paint the left/right scroll arrow (will overwrite the border if small bar)
	wxPoint Triangle[3];
	Triangle[0].x = CenterX - offset;
	Triangle[0].y = CenterY + 2;
	Triangle[1].x = CenterX + offset;
	Triangle[1].y = CenterY;
	Triangle[2].x = CenterX - offset;
	Triangle[2].y = CenterY - 2;

	wxBrush FrameBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWFRAME));
	pDC->SetBrush(FrameBrush);
	pDC->SetPen(FramePen);
	pDC->DrawPolygon(3, Triangle);

	pDC->SetBrush(wxNullBrush);
	pDC->SetPen(wxNullPen);
}



/********************************************************************************************

>	void CColourBar::PaintScrollBar(wxDC *pDC)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Inputs:		pDC - The DC to paint into
				ScrollBarRect - The rectangle into which the s.b. should be painted
	Outputs:	-
	Returns:	-
	Purpose:	Redraws the ColourBar window's mini proportional scroll bar
	Notes:		You must call CalculateRects and cache the value of TotalNumColours
				prior to calling this function - see DoPaint for everything that needs
				to be done.

				The scrollbar rect is painted *totally* i.e. all contained pixels are
				overwritten with colour, so it can be used in updates to redraw without
				any flicker (When invalidating the rect, pass FALSE to stop it filling
				with grey first)

				The ColourBar works exclusively on the SELECTED Doc. Change with care

	SeeAlso:	CColourBar::DoPaint

********************************************************************************************/

void CColourBar::PaintScrollBar(wxDC *pDC)
{
	ERROR3IF(m_pCurrentColourList == NULL,
			_T("CColourBar::PaintScrollBar entered with NULL Current Colour List"));

	wxRect SausageRect;
	BOOL HasSausage = CalculateSausageRect(&SausageRect);

	wxPen TransPen(wxColour(0,0,0), 1, wxTRANSPARENT);

	if (HasSausage && ScrollHeight != 0)
	{
		// Paint the scrollbar outline rectangle
		wxPen FramePen(wxColour(0,0,0));
		wxBrush TransBrush(wxColour(0,0,0), wxTRANSPARENT);
		pDC->SetBrush(TransBrush);
		pDC->SetPen(FramePen);
		pDC->DrawRectangle(ScrollBarRect);

		// Paint the grey fill on either side of the sausage
		wxBrush GreyBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
		pDC->SetBrush(GreyBrush);
		pDC->SetPen(TransPen);

		wxRect TheRect(SausageRect);
		TheRect.x = ScrollBarRect.x + 1;
		TheRect.width = SausageRect.x - TheRect.x;			// Area left of sausage
		if (TheRect.width > 0)
			pDC->DrawRectangle(TheRect);

		TheRect.x = SausageRect.x + SausageRect.width;		// Area right of sausage
		TheRect.width = ScrollBarRect.GetRight() - TheRect.x;
		if (TheRect.width > 0)
			pDC->DrawRectangle(TheRect);

		// Fill the center of the sausage plinth with btnface grey as well
		if (SausageRect.width > 1)
		{
			TheRect = SausageRect;
			TheRect.x++;
			TheRect.width--;
			TheRect.y++;
			TheRect.height--;
			pDC->DrawRectangle(TheRect);
		}

		// Ensure it has a black line at each end to make it visible
		SausageRect.Inflate(1, 1);
		pDC->SetPen(FramePen);
		pDC->DrawRectangle(SausageRect);

		pDC->SetPen(wxNullPen);
		pDC->SetBrush(wxNullBrush);

		// Paint the Sausage plinth
		SausageRect.Inflate(-1, -1);
		Paint3dPlinth(pDC, &SausageRect, TRUE);
	}

	if (HasSausage || ScrollHeight == 0)
	{
		// Now paint on the scroll buttons
		PaintScrollButton(pDC, SCROLLBUTTON_LEFT, TRUE);
		PaintScrollButton(pDC, SCROLLBUTTON_RIGHT, TRUE);
	}
}



/********************************************************************************************

>	wxBrush *CColourBar::GetColourBrush(COLORREF rgb)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		rgb - An RGB ScreenWord for passing to GDI or GBrush in order to create
				a brush of the desired colour.
	Outputs:	-
	Returns:	A pointer to a wxBrush for the given colour. NOTE that it is up to the caller
				to dispose of this brush using delete.
				This pointer will be NULL if the brush could not be created
	Purpose:	Calculates a logical brush for use in painting. If GDraw is available
				via CColourBar::GDrawBrush, GDraw will be used. If not, GDI will be used.
	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

wxBrush *CColourBar::GetColourBrush(COLORREF rgb)
{
	wxBrush* pBrush = NULL;

	if (GDrawBrush.Available())
	{
		pBrush = new wxBrush;

		if (pBrush != NULL)
		{
			GDrawBrush.GetLogBrush(rgb, pBrush);
		}
	}
	else
		pBrush = new wxBrush(rgb);

	return pBrush;
}



/********************************************************************************************

>	void CColourBar::PaintColourCell(wxDC *pDC, wxRect *Rect, DocColour *RectCol)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Inputs:		pDC - The DC to draw into
				rect - The rectangle to plot the colour cell into
				RectCol - The colour with which to fill the rectangle, or NULL
				if the cell is to be filled to represent 'no' colour or 'many'
				colours in a selection.
	Outputs:	-
	Returns:	-
	Purpose:	Draws the given rectangle as a 'Colour cell' for the given DocColour.
				The top half of the rect is solid-filled with the colour, while the
				bottom 2 quarters are filled with the colour as it would appear on top
				of a white background and black bgnd respectively
				(if the col isn't fractionally transparent, rect is just solid filled)

	Notes:		If GBrush has been initialised before calling this function, it will be
				used to plot the colour cells.
				The ColourBar works exclusively on the SELECTED Doc. Change with care

	SeeAlso:	CColourBar::PaintColourStrip; CColourBar::DoPaint

********************************************************************************************/

void CColourBar::PaintColourCell(wxDC *pDC, wxRect *Rect, DocColour *RectCol)
{		   
	Document *ScopeDoc = Document::GetSelected();
	if (ScopeDoc == NULL) return;

	wxPen TransPen(wxColour(0,0,0), 1, wxTRANSPARENT);
	pDC->SetPen(TransPen);

	if (RectCol == NULL)		// No colour (eg 'many' colours in selection)
	{
//		pDC->SetBkColor(RGB(255,255,255));				// ensure white background
		wxBrush HatchBrush(wxColour(0,0,0), wxBDIAGONAL_HATCH);	// Fill with black/white hatch
		pDC->SetBrush(HatchBrush);
		pDC->DrawRectangle(*Rect);
		pDC->SetPen(wxNullPen);
		pDC->SetBrush(wxNullBrush);
		return;
	}

	if (RectCol->IsTransparent())
	{
//		pDC->SetBkColor(RGB(255,255,255));				// ensure white background
		wxBrush HatchBrush(wxColour(0,0,0), wxCROSSDIAG_HATCH);	// Fill with black/white hatch
		pDC->SetBrush(HatchBrush);
		pDC->DrawRectangle(*Rect);
		pDC->SetPen(wxNullPen);
		pDC->SetBrush(wxNullBrush);
		return;
	}

	// Find a context to render with. We'll use the Selected View's context if possible,
	// so that we render using the same colour correction/separation mode. If there
	// is no selected view, then we will look for a global context.
	ColourContextRGBT *CCrgbt = (ColourContextRGBT *)
	   			ColourManager::GetColourContext(COLOURMODEL_RGBT, DocView::GetSelected());

	ERROR3IF(CCrgbt == NULL, _T("Can't find an RGB colour context to render with!"));

	DWORD ScreenWord = ConvertColourToTransScreenWord(CCrgbt, RectCol);
	
	wxBrush *Brush = GetColourBrush((COLORREF) (ScreenWord & 0x00ffffff));

	IndexedColour *Parent = RectCol->FindParentIndexedColour();
// Webster shows spots as normal library colours
#ifndef WEBSTER
	if (Parent != NULL && Parent->IsSpotOrTintOfSpot())
	{
		// Spot colours are shown as circular swatches in order to make them more obvious
		// (First we must fill the background with BTNFACE to clear the corners)
		wxBrush BackBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
		pDC->SetBrush(BackBrush);
		pDC->DrawRectangle(*Rect);

		// Try to make the rectangle more square, so we always render a circle
		wxRect Square(*Rect);
		if (Square.width > Square.height)
			Square.Inflate(-(Square.width - Square.height) / 2, 0);
		else if (Square.width < Square.height)
			Square.Inflate(0, -(Square.height - Square.width) / 2);

		pDC->SetBrush(*Brush);
		pDC->DrawEllipse(Square);
	}
	else
#endif // WEBSTER
	{
		// Process colours are shown as rectangles
		pDC->SetBrush(*Brush);
		pDC->DrawRectangle(*Rect);

		if (ColourCellsDivided)
		{
			// If the user wants a grey line between the colour cells to make them easier
			// to distinguish, draw a line.
			wxBrush GreyBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
			wxRect LineRect(*Rect);
			LineRect.x = LineRect.GetRight();
			LineRect.width = 1;
			pDC->SetBrush(GreyBrush);
			pDC->DrawRectangle(LineRect);
		}
	}

	pDC->SetPen(wxNullPen);
	pDC->SetBrush(wxNullBrush);
	delete Brush;
}



/********************************************************************************************

>	void CColourBar::PaintNoColourCell(wxDC *pDC)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		pDC - The DC to draw into
	Outputs:	-
	Returns:	-
	Purpose:	Draws the 'NoColour' colour cell just to the left of the ColourStrip
	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care
	SeeAlso:	CColourBar::DoPaint

********************************************************************************************/

void CColourBar::PaintNoColourCell(wxDC *pDC)
{		   
	wxRect Rect;
	CalculateNoColourCellRect(&Rect);

	wxPen FramePen(wxColour(0,0,0));
	pDC->SetPen(FramePen);
	wxBrush TransBrush(wxColour(0,0,0), wxTRANSPARENT);
	pDC->SetBrush(TransBrush);
	pDC->DrawRectangle(Rect);

	Rect.Inflate(-1, -1);

	wxPen TransPen(wxColour(0,0,0), 1, wxTRANSPARENT);
	pDC->SetPen(TransPen);

//	pDC->SetBkColor(RGB(255,255,255));				// ensure white background
	wxBrush HatchBrush(wxColour(0,0,0), wxCROSSDIAG_HATCH);	// Fill with black/white hatch
	pDC->SetBrush(HatchBrush);
	pDC->DrawRectangle(Rect);

	pDC->SetBrush(wxNullBrush);
	pDC->SetPen(wxNullPen);

	// Check if the current line/fill colours are 'no colour', and if so, draw diamonds
	// as appropriate on the 'no colour' cell.
	DocColour *LineColour;
	DocColour *FillColour;
	DocColour *EndColour;
	BOOL DiamondType = ColourManager::GetCurrentLineAndFillColours(&LineColour,
																&FillColour, &EndColour);

	// If the linecolour is transparent, plot a line diamond
	if (LineColour != NULL && LineColour->IsTransparent())
		PaintIndicatorDiamond(pDC, Rect, CBCOLOUR_LINE, CELLINDEX_NOCOLOURCELL, DiamondType);

	// Plot a fill diamond (flat fill) or 1 or 2 grad fill 'arrows'
	if (FillColour != NULL && FillColour->IsTransparent())
	{
		CBColourType TheType = CBCOLOUR_FILL;					// It is a flat fill
		if (EndColour != NULL && EndColour->IsTransparent())
			TheType = CBCOLOUR_START;							// Er..no it's a start gradfill

		PaintIndicatorDiamond(pDC, Rect, TheType, CELLINDEX_NOCOLOURCELL, DiamondType);
	}

	if (EndColour != NULL && EndColour->IsTransparent())
		PaintIndicatorDiamond(pDC, Rect, CBCOLOUR_END, CELLINDEX_NOCOLOURCELL, DiamondType);
}


/********************************************************************************************

>	void CColourBar::PaintLibColourCell(wxDC *pDC, wxRect *Rect, DocColour *RectCol, BOOL SpotColour = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	20/3/97
	Inputs:		pDC - The DC to draw into
				rect - The rectangle to plot the colour cell into
				RectCol - The colour with which to fill the rectangle.
				SpotColour - falg to say if the colour should be rendered as a spot colour or not
	Outputs:	-
	Returns:	-
	Purpose:	Draws the given rectangle as a 'Colour cell' for the given DocColour.
	SeeAlso:	CColourBar::PaintColourCell; CColourBar::DoPaint

********************************************************************************************/

void CColourBar::PaintLibColourCell(wxDC *pDC, wxRect *Rect, DocColour *RectCol, BOOL SpotColour)
{		   
	Document *ScopeDoc = Document::GetSelected();
	if (ScopeDoc == NULL) return;

	if (RectCol == NULL)		// No colour (eg 'many' colours in selection)
	{
//		pDC->SetBkColor(RGB(255,255,255));				// ensure white background
		wxBrush HatchBrush(wxColour(0,0,0), wxBDIAGONAL_HATCH);	// Fill with black/white hatch
		pDC->SetBrush(HatchBrush);
		pDC->DrawRectangle(*Rect);
		pDC->SetBrush(wxNullBrush);
		return;
	}

	// Find a context to render with. We'll use the Selected View's context if possible,
	// so that we render using the same colour correction/separation mode. If there
	// is no selected view, then we will look for a global context.
	ColourContextRGBT *CCrgbt = (ColourContextRGBT *)
	   			ColourManager::GetColourContext(COLOURMODEL_RGBT, DocView::GetSelected());

	ERROR3IF(CCrgbt == NULL, _T("Can't find an RGB colour context to render with!"));

	DWORD ScreenWord = ConvertColourToTransScreenWord(CCrgbt, RectCol);
	
	wxBrush *Brush = GetColourBrush((COLORREF) (ScreenWord & 0x00ffffff));

	// Library colours are shown as diamond swatches in order to make them more obvious
	// (First we must fill the background with BTNFACE to clear the corners)
	wxBrush BackBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	pDC->SetBrush(BackBrush);
	pDC->DrawRectangle(*Rect);

	// Try to make the rectangle more square, so we always render a circle
	wxRect Square(*Rect);
	if (Square.width > Square.height)
		Square.Inflate(-(Square.width - Square.height) / 2, 0);
	else if (Square.width < Square.height)
		Square.Inflate(0, -(Square.height - Square.width) / 2);

	// Generate a polygon definition of the diamond shape
	// When plotting gradfill start/end colour, the top/bottom point is flattened
	// to make an up or down arrow shape
	INT32 MidY = Square.y + (Square.height / 2);
	INT32 MidX = Square.x + (Square.width / 2);
	wxPoint Diamond[4];
	Diamond[0].x = MidX;
	Diamond[0].y = Square.y;
	Diamond[1].x = Square.x + Square.width;
	Diamond[1].y = MidY;
	Diamond[2].x = MidX;
	Diamond[2].y = Square.y + Square.height;
	Diamond[3].x = Square.x;
	Diamond[3].y = MidY;

	// plot it
	if (!SpotColour)
	{
		// Non spot colour so render the colour into the diamond
		pDC->SetBrush(*Brush);
		pDC->DrawPolygon(4, Diamond);
		pDC->SetBrush(wxNullBrush);
	}
	else
	{
		// Spot colour so render a black diamond and then the usual spot colour
		// render a black diamond
		wxBrush BlackBrush(wxColour(0,0,0));
		pDC->SetBrush(BlackBrush);
		pDC->DrawPolygon(4, Diamond);
		pDC->SetBrush(wxNullBrush);

		// Try to make the rectangle more square, so we always render a circle
		// 1 - 0.707 = 0.293  where 0.707 is cos and sin 45
		//INT32 QuarterX = (INT32)(0.2 * (double)((Rect->right - Rect->left)/2));
		//INT32 QuarterY = (INT32)(0.2 * (double)((Rect->top - Rect->bottom)/2));
		// But we could do this a lot quicker with this rough approximation
		//INT32 QuarterX = (INT32)((Square.right - Square.left)/10.0 + 0.5);
		//INT32 QuarterY = (INT32)((Square.top - Square.bottom)/10.0 + 0.5);
		INT32 QuarterX = Square.width / 10;
		INT32 QuarterY = Square.height / 10;
		wxRect EllipseSquare(Square.x + QuarterX, Square.y + QuarterY,
							Square.width - (QuarterX * 2), Square.height - (QuarterY * 2));

		// render a spot colour ellipse
		pDC->SetBrush(*Brush);
		pDC->DrawEllipse(EllipseSquare);
		pDC->SetBrush(wxNullBrush);
	}

	// clear out the brush we created
	delete Brush;
}

/********************************************************************************************

>	void CColourBar::PaintIndicators(wxDC *pDC)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/94
	Inputs:		pDC - The DC to paint into
				ColPalette - The list of colours displayed in the colour strip
	Outputs:	-
	Returns:	-
	Purpose:	Redraws the ColourBar colour indicators (current fg/bg colours)
	Notes:		You must call CalculateRects and cache the value of TotalNumColours
				prior to calling this function - see DoPaint for everything that needs
				to be done.
				If GBrush has been initialised before calling this function, it will be
				used to plot the colour cells.
				The ColourBar works exclusively on the SELECTED Doc. Change with care
	SeeAlso:	CColourBar::DoPaint

********************************************************************************************/

void CColourBar::PaintIndicators(wxDC *pDC)
{
	// Save the current ColourCellsDivided preference state and set it to OFF
	INT32 OldCellState = ColourCellsDivided;
	ColourCellsDivided = FALSE;


	// Find the current line/fill colours to plot...
	DocColour *LineColour;
	DocColour *FillColour;
	DocColour *EndGradColour;
	ColourManager::GetCurrentLineAndFillColours(&LineColour, &FillColour, &EndGradColour);

	// Plot the line colour as a large colour cell filling IndicatorRect
	// pass NULL for many colours
	PaintColourCell(pDC, &IndicatorRect, LineColour);

	// Work out where the inside rectangle is, and plot it as a colour cell
	// (If you change this calculation, fix the other calc. of 'DeflateBy' below)
	wxRect InnerRect(IndicatorRect);
	INT32 DeflateBy = (INT32) (abs(IndicatorRect.height) * 2) / 5;
	if (DeflateBy < 4)
		DeflateBy = 4;	// Line indicator is min. of 2 pixels wide
	InnerRect.Inflate(-DeflateBy / 2, -DeflateBy / 2);

	if (EndGradColour != NULL)
	{
		// We want to display the start and end colours for a grad fill, so split the
		// fill indicator square into 2 rectangles, one above the other
		wxRect TopRect(InnerRect);
		TopRect.height = TopRect.height / 2;
		InnerRect.y += TopRect.height;
		InnerRect.height -= TopRect.height;

		// And paint the end colour at the bottom
		PaintColourCell(pDC, &TopRect, EndGradColour);
	}

	// And fill the remaining part of inner rect with the fill (or grad-fill start) colour
	PaintColourCell(pDC, &InnerRect, FillColour);

	// And restore the saved preference state
	ColourCellsDivided = OldCellState;
}



/********************************************************************************************

>	void CColourBar::PaintIndicatorDiamond(wxDC *pDC, const wxRect &CellRect,
											CBColourType ColourType, INT32 CellIndex, BOOL DiamondType)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		pDC - The DC to paint into

				CellRect - The Colour Cell to paint the indicator into

				ColourType - Indicates where and how to plot the diamond (different places
				and shapes are used for lines, files, and gradfills)

				CellIndex - The index of the cell (or CELLINDEX_NOCOLOURCELL) which
				is being redrawn (this is cached to remember where the diamond is
				for future updates of the diamond position)

				DiamondType - FALSE for 'diamond', TRUE for 'plus' shape
				(pluses are used to indicate colours when there is no selection, and
				we are thus displaying the current default attrs, not the selected colours)

	Outputs:	-
	Returns:	-
	Purpose:	Draws a small diamond in the top left/right corner of a colour cell
				to indicate that the given cell is the current line/fill colour

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

	SeeAlso:	CColourBar::DoPaint

********************************************************************************************/

void CColourBar::PaintIndicatorDiamond(wxDC *pDC, const wxRect &CellRect,
										CBColourType ColourType, INT32 CellIndex, BOOL DiamondType)
{
	// Calculate the position to plot the diamond (top left or top right corner)
	INT32 MidY = CellRect.y + (CellRect.height / 4);
	INT32 MidX;
	if (ColourType == CBCOLOUR_LINE)
		MidX = CellRect.x + (CellRect.width * 3 / 4);
	else
		MidX = CellRect.x + (CellRect.width / 4);

	if (DiamondType)
	{
		// Plot the plus shape
		wxBrush TransBrush(wxColour(0,0,0), wxTRANSPARENT);
		pDC->SetBrush(TransBrush);
		wxPen WhitePen(wxColour(255, 255, 255));
		wxRect VertRect(MidX - 3, MidY - 1, 7, 3);
		wxRect HorzRect(MidX - 1, MidY - 3, 3, 7);
		pDC->SetPen(WhitePen);
		pDC->DrawRectangle(VertRect);
		pDC->DrawRectangle(HorzRect);

		wxPen BlackPen(wxColour(0, 0, 0));
		pDC->SetPen(BlackPen);
		pDC->DrawLine(MidX - 2, MidY, MidX + 3, MidY);
		pDC->DrawLine(MidX, MidY - 2, MidX, MidY + 3);
		pDC->SetPen(wxNullPen);
		pDC->SetBrush(wxNullBrush);
	}
	else
	{
		// Generate a polygon definition of the diamond shape
		// When plotting gradfill start/end colour, the top/bottom point is flattened
		// to make an up or down arrow shape
		wxPoint Diamond[4];
		Diamond[0].x = MidX - 3;
		Diamond[0].y = MidY;
		Diamond[1].x = MidX;
		Diamond[1].y = MidY - ((ColourType == CBCOLOUR_START) ? 0 : 3);
		Diamond[2].x = MidX + 3;
		Diamond[2].y = MidY;
		Diamond[3].x = MidX;
		Diamond[3].y = MidY + ((ColourType == CBCOLOUR_END) ? 0 : 3);

		// plot it
		wxPen WhitePen(wxColour(255, 255, 255));
		wxBrush BlackBrush(wxColour(0, 0, 0));
		pDC->SetPen(WhitePen);
		pDC->SetBrush(BlackBrush);
		pDC->DrawPolygon(4, Diamond);
		pDC->SetPen(wxNullPen);
		pDC->SetBrush(wxNullBrush);
	}

	// Remember where we plotted this diamond
	switch (ColourType)
	{
		case CBCOLOUR_LINE:
			LastLineCell = CellIndex;
			break;
		
		case CBCOLOUR_END:
			LastEndCell  = CellIndex;
			break;

		case CBCOLOUR_FILL:
		case CBCOLOUR_START:
			LastFillCell = CellIndex;
			break;

		default:
			ERROR3(_T("Unknown colour type"));
			break;
	}

	LastDiamondShape = DiamondType;
}



/********************************************************************************************

>	void CColourBar::PaintColourStrip(wxDC *pDC, const wxRect &ClipRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/94
	Inputs:		pDC - The DC to paint into
				ClipRect - The rectangle into which we're drawing. Cells outside this
				rectangle will not be rendered, to improve renderintg speed.
	Outputs:	-
	Returns:	-
	Purpose:	Redraws the ColourBar colour strip
	Notes:		You must call CalculateRects and cache the value of TotalNumColours
				prior to calling this function - see DoPaint for everything that needs
				to be done.
				If GBrush has been initialised before calling this function, it will be
				used to plot the colour cells.

				The ColourBar works exclusively on the SELECTED Doc. Change with care

	SeeAlso:	CColourBar::DoPaint

********************************************************************************************/

void CColourBar::PaintColourStrip(wxDC *pDC, const wxRect &ClipRect)
{
	// Find pointers to the fill/line IndexedColours so we will know when we hit
	// them as we draw, and can then draw indicator diamonds on the colour cells.
	IndexedColour *CurrentLineColour = NULL;
	IndexedColour *CurrentFillColour = NULL;	// FlatFill colour or GradFill Start colour
	IndexedColour *CurrentEndColour = NULL;		// GradFill End colour
	BOOL DiamondType;

	BOOL IsAGradFill = FALSE;					// True if FillColour is the Gradfill Start colour

	{
		DocColour *LineColour = NULL;
		DocColour *FillColour = NULL;
		DocColour *EndColour  = NULL;
		DiamondType = ColourManager::GetCurrentLineAndFillColours(&LineColour,
															&FillColour, &EndColour);
		if (LineColour)
			CurrentLineColour = LineColour->FindParentIndexedColour();

		if (FillColour)
			CurrentFillColour = FillColour->FindParentIndexedColour();	

		if (EndColour)
		{
			IsAGradFill = TRUE;
			CurrentEndColour  = EndColour->FindParentIndexedColour();
		}
	}

	// Calculate the screen rectangle of the leftmost visible cell
	wxRect CellRect(StripRect);
	CellRect.width = CellSize;

	INT32 CellIndex = (INT32) LeftmostColour;	// keep track of each cell's index as we draw

	// Draw the colours in the strip
	BOOL RedrawFinished = FALSE;
	// If the user has the preference set then include and hence show the document
	// colours
	if (ColourSGallery::ShowDocumentColours)
	{
		// Find the leftmost displayed colour in the DisplayList sequence
		DocColour Temp;
		IndexedColour *TheColour = FindColourByIndex((UINT32) LeftmostColour);
		while (TheColour != NULL)
		{ 
			// Paint the colour cell, but only bother if it's in the invalid region
			// This speeds up redraw, especially when ScrollTheStrip only wishes to redraw
			// a few colour cells at one end of the strip.
			if ((CellRect.x + CellRect.width) >= ClipRect.x)
			{
				Temp.MakeRefToIndexedColour(TheColour);

				PaintColourCell(pDC, &CellRect, &Temp);	// Paint this Cell

				if (TheColour == CurrentLineColour)		// Paint current LineCol indicator
					PaintIndicatorDiamond(pDC, CellRect, CBCOLOUR_LINE, CellIndex, DiamondType);

				if (TheColour == CurrentFillColour)		// Paint Current Fill/Start indicator
					PaintIndicatorDiamond(pDC, CellRect,
										  (IsAGradFill) ? CBCOLOUR_START : CBCOLOUR_FILL,
										  CellIndex, DiamondType);

				if (TheColour == CurrentEndColour)		// Paint Current End indicator
					PaintIndicatorDiamond(pDC, CellRect, CBCOLOUR_END,  CellIndex, DiamondType);
			}

			CellRect.x += CellSize;					// Move on to next Cell

			// if no more room in the strip for painting, or have updated the clip
			// rectangle fully, we can stop redrawing
			if ((CellRect.x + CellRect.width) > (StripRect.x + StripRect.width) || 
				CellRect.x > (ClipRect.x + ClipRect.width))
			{
				RedrawFinished = TRUE;
				break;
			}

			CellIndex++;		// Increment the current cell index

			// And find the next colour
			TheColour = m_pCurrentColourList->GetUndeletedNext(TheColour);
		}

		// If the redraw is finished then return now rather than doing extra work
		if (RedrawFinished)
			return;
	}

	// Now go through the library colours that we have been asked to display
	// A bit of similar code to the above may be recognised here!!!!
	SGDisplayLibColour *pLibColour = NULL;
	DocColour * pRedrawCol = FindLibColourByIndex(CellIndex, &pLibColour);
	while (pRedrawCol != NULL && pLibColour != NULL)
	{
		// Paint the colour cell, but only bother if it's in the invalid region
		// This speeds up redraw, especially when ScrollTheStrip only wishes to redraw
		// a few colour cells at one end of the strip.
		if ((CellRect.x + CellRect.width) >= ClipRect.x)
		{
			// Paint this Cell
#ifndef WEBSTER
			// In Camelot use a special colour library item drawing routine
			PaintLibColourCell(pDC, &CellRect, pRedrawCol, pLibColour->IsASpotColour());
#else
			// In Webster draw spots as standard library colours
			PaintLibColourCell(pDC, &CellRect, pRedrawCol, FALSE);
#endif
		}

		CellRect.x += CellSize;					// Move on to next Cell

		// if no more room in the strip for painting, or have updated the clip
		// rectangle fully, we can stop redrawing
		if ((CellRect.x + CellRect.width) > (StripRect.x + StripRect.width) || 
			CellRect.x > (ClipRect.x + ClipRect.width))
		{
			RedrawFinished = TRUE;
			break;
		}

		CellIndex++;		// Increment the current cell index

		// And find the next colour
		// This could be speeded up but is ok for now
		pRedrawCol = FindLibColourByIndex(CellIndex, &pLibColour);
	}
}


/*********************************************************************************************
>	afx_msg void CColourBar::OnPaint()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Redraws the colour bar
	Errors:		-

**********************************************************************************************/ 
void CColourBar::OnPaint( wxPaintEvent &evnt )
{
	// Always create this so that the area is validated
	wxPaintDC dc(this);

	if (!CCamApp::IsDisabled())
	{
		DoPaint(&dc);
	}
}



/********************************************************************************************

>	void CColourBar::DoPaint(wxDC* pDC)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Inputs:		pDC - The DC to paint into
	Outputs:	-
	Returns:	-
	Purpose:	Redraws the ColourBar window contents

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

void CColourBar::DoPaint(wxDC* pDC)
{
	// If we're in the middle of doing something important (having a bit of an error)
	// then we just pretend that we redraw, and abort, to save going re-entrant.
	if (CCamApp::IsDisabled())
	{
		TRACE( _T("Colour bar repaint has been aborted: the system is disabled (due to an error/ensure?)\n"));
		// The paint condition will have been cleared by the creation of the wxPaintDC
//		::ValidateRect(m_hWnd, NULL);								// Clear paint condition
		return;
	}

//	ASSERT_VALID(this);
//	ASSERT_VALID(pDC);

	Error::RenderThreadIn();		// Handle errors/exceptions during rendering properly

//	CControlBar::DoPaint(pDC);      // draw border

	Document *OldCurrentDoc = SetCurrentDoc();

	// Get the current colour list, and calculate the rects to be redrawn
	m_pCurrentColourList = ColourManager::GetColourList();
	CalculateRects();

	wxPen TransPen(wxColour(0,0,0), 1, wxTRANSPARENT);
	pDC->SetPen(TransPen);

	wxBrush GreyBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	if (m_pCurrentColourList == NULL)// || m_pCurrentColourList->GetUndeletedCount() == 0)
	{
		// No colours, so just fill the entire bar with grey
		pDC->SetBrush(GreyBrush);
		pDC->DrawRectangle(WindowRect);
		pDC->SetBrush(wxNullBrush);
		pDC->SetPen(wxNullPen);
		RestoreCurrentDoc(OldCurrentDoc);

		Error::RenderThreadOut();
		return;
	}

	wxRect ClipRect;
	pDC->GetClippingBox(&ClipRect.x, &ClipRect.y, &ClipRect.width, &ClipRect.height);

	if (ClipRect.IsEmpty())
		ClipRect = GetClientRect();

	// Fill everything to the right of the colourstrip with BTNFACE grey, to ensure we 
	// overwrite any stuff that might have been drawn there and not erased (on some updates
	// we purposely do not bother with erasing the background, to reduce flicker)
	wxRect TheRect(WindowRect);
	INT32 OffsetLeft = StripRect.x + StripRect.width - TheRect.x;
	TheRect.x += OffsetLeft;
	TheRect.width -= OffsetLeft;
	pDC->SetBrush(GreyBrush);
	pDC->DrawRectangle(TheRect);
	pDC->SetBrush(wxNullBrush);
	pDC->SetPen(wxNullPen);

//	TotalNumColours = m_pCurrentColourList->GetUndeletedCount(); // FIXEDCOLOURS
	TotalNumColours = GetNumberOfColours();
	if ((ScrollHeight == 0 || !HasNoScrollBar(-1, (INT32) TotalNumColours)))
	{
		wxRect AllScrollBarRect(ScrollBarRect);				// ScrollBarRect does not include the
		AllScrollBarRect.x -= ButtonWidth + 1;				// scroll arrows, which extend outside
		AllScrollBarRect.width += (ButtonWidth + 1) * 2;	// this rect by ButtonWidth + 1 pixels

		if (ClipRect.Intersects(AllScrollBarRect))
			PaintScrollBar(pDC);
	}

	PaintNoColourCell(pDC);
	if (ClipRect.Intersects(EditButtonRect))
		PaintEditOrNewButton(pDC, (IndentedButton != CLICKED_EDITBUTTON), TRUE);

#if FALSE
	if (ClipRect.Intersects(NewButtonRect))
		PaintEditOrNewButton(pDC, (IndentedButton != CLICKED_NEWBUTTON), FALSE);
#endif

	// Set up palette and GBrush for Spiffy redraw
//	CPalette * OldPal = NULL;
//	if (PaletteManager::UsePalette())
//	{
//		OldPal = PaletteManager::StartPaintPalette(pDC);
//	}

	GDrawBrush.Init(pDC);
	GDrawBrush.Start();

	// Draw Indicators and colour strip, using GBrush if possible
	if (ClipRect.Intersects(IndicatorRect))
		PaintIndicators(pDC);

	if (ClipRect.Intersects(StripRect))
		PaintColourStrip(pDC, ClipRect);

	// Finish with GBrush
	GDrawBrush.Stop();
//	if (OldPal)
//		PaletteManager::StopPaintPalette(pDC, OldPal);

	Error::RenderThreadOut();

	RestoreCurrentDoc(OldCurrentDoc);
}



/********************************************************************************************

>	void CColourBar::ForceRedrawOfRect(const wxRect &RectToDraw)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		RectToDraw - The rectangle to force-redraw. This should be
				one of the member rectangles (ScrollBarRect, StripRect, IndicatorRect)
	Outputs:	-
	Returns:	-
	Purpose:	Causes the given portion of the colour bar to be redrawn
	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

void CColourBar::ForceRedrawOfRect(const wxRect &RectToDraw)
{
	wxRect OffsetRectToDraw(RectToDraw);
	wxRect RealWindowRect;
	
	// Convert the screen rectangle coords into window *offsets*
	RealWindowRect = GetClientRect();
	OffsetRectToDraw.Offset(-RealWindowRect.x, -RealWindowRect.y);

	RefreshRect(OffsetRectToDraw, false);
}



/********************************************************************************************

>	INT32 CColourBar::WhereIsMouse(wxPoint MousePos, wxRect *TheRectItIsIn = NULL,
									BOOL *ToLeft = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94

	Inputs:		MousePos - The point you wish to check

	Outputs:	TheRectItIsIn - if non-NULL, this rect is filled in with a copy of
				the rectangle in which the mouse pointer lies. This is indeterminate
				in the case of a CLICKED_NOTHING return value.

				ToLeft - if non-NULL, and if the mouse was over a colour cell, this
				boolean is filled in TRUE if the pointer is over the left side of the
				cell, or FALSE if over the right. Used for drag'n'drop to determine
				which side of a colour to insert a dropped colour into.

	Returns:	A positive (0..MaxColours) value indicates the given position
				lies over a colour cell of that index.
				A negative value indicates one of the special 'controls' in the
				window (scroll bar, colour indicators etc) - see the constants
				CLICKED_XXXX defined at the top of ccolbar.cpp

	Purpose:	Checks which part of the ColourBar the given point lies over (if
				any) and returns a value indicating where it is.

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

INT32 CColourBar::WhereIsMouse(wxPoint MousePos, wxRect *TheRectItIsIn, BOOL *ToLeft)
{
	if (ToLeft != NULL)		// Sensible default output
		*ToLeft = TRUE;

	if (StripRect.Inside(MousePos))		// Check ColourStrip - MUST be checked first!
	{
		if (TheRectItIsIn != NULL)
			*TheRectItIsIn = StripRect;

		// If necessary, determine if the pointer is in the left or right half of the colour cell
		if (ToLeft != NULL)
		{
			if ( (MousePos.x - StripRect.x) % CellSize >= CellSize / 2)
			{
				// The pointer is in the right hand side of the cell, so return ToLeft=FALSE
				*ToLeft = FALSE;
			}
		}

		return((INT32) (((MousePos.x - StripRect.x) / CellSize) + LeftmostColour));
	}

	if (IndicatorRect.Inside(MousePos))	// Check Indicator patch
	{
		if (TheRectItIsIn != NULL)
			*TheRectItIsIn = IndicatorRect;
		return(CLICKED_INDICATORS);
	}

	if (EditButtonRect.Inside(MousePos))
	{
		if (TheRectItIsIn != NULL)
			*TheRectItIsIn = EditButtonRect;
		return(CLICKED_EDITBUTTON);
	}

#if FALSE
/*
	if (NewButtonRect.Inside(MousePos))
	{
		if (TheRectItIsIn != NULL)
			*TheRectItIsIn = NewButtonRect;
		return(CLICKED_NEWBUTTON);
	}
*/
#endif

	wxRect TheRect;

	if (!HasNoScrollBar(-1, -1))			// Check Scrollbar rectangle (if any)
	{
		if (ScrollHeight != 0 && ScrollBarRect.Inside(MousePos) &&
			CalculateSausageRect(&TheRect))
		{
			if (TheRectItIsIn != NULL)
				*TheRectItIsIn = TheRect;

			if (MousePos.x < TheRect.x)	// Left of the sausage
			{
				if (TheRectItIsIn != NULL)
				{
					TheRectItIsIn->x = ScrollBarRect.x;
					TheRectItIsIn->width = (TheRect.x - 1 - TheRectItIsIn->x);
				}

				return(CLICKED_SCROLLBARLEFT);
			}

			if (MousePos.x > (TheRect.x + TheRect.width))	// Right of the sausage
			{
				if (TheRectItIsIn != NULL)
				{
					TheRectItIsIn->x = TheRect.x + TheRect.width + 1;
					TheRectItIsIn->width = ScrollBarRect.x + ScrollBarRect.x - TheRectItIsIn->x;
				}
				return(CLICKED_SCROLLBARRIGHT);
			}

			return(CLICKED_SCROLLBAR);	   	// Middle of the sausage
		}

		TheRect = ScrollBarRect;			// Check left scroll button
		TheRect.x -= ButtonWidth;
		TheRect.width = ButtonWidth;
		if (TheRect.Inside(MousePos))
		{
			if (TheRectItIsIn != NULL)
				*TheRectItIsIn = TheRect;
			return(CLICKED_LEFTSCROLL);
		}

		TheRect = ScrollBarRect;		   	// Check right scroll button
		TheRect.x += TheRect.width;
		TheRect.width = ButtonWidth;
		if (TheRect.Inside(MousePos))
		{
			if (TheRectItIsIn != NULL)
				*TheRectItIsIn = TheRect;
			return(CLICKED_RIGHTSCROLL);
		}
	}

	CalculateNoColourCellRect(&TheRect);	// Check NoColour cell
	if (TheRect.Inside(MousePos))
	{
		if (TheRectItIsIn != NULL)
			*TheRectItIsIn = TheRect;
		return(CLICKED_NOCOLOURCELL);
	}

	return(CLICKED_NOTHING);				// Not over anything important
}



/********************************************************************************************

>	void CColourBar::ScrollTheStrip(INT32 ScrollBy, BOOL RedrawAlways = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		ScrollBy - The number of cells (eg +1, -1) by which to scroll the strip
				RedrawAlways - TRUE to always redraw the strip & scrollbar, even if the
				scroll position does not change.
	Outputs:	-
	Returns:	-
	Purpose:	Scrolls the colour strip to the left or right. Does not allow scrolling
				past the ends of the available list. Causes the window to be redrawn.

	Notes:		Passing in ScrollBy=0 has the effect of ensuring that the current scroll
				position is legal for the current colour list - if not, it will be
				corrected and the bar will be redrawn. This can thus be used to ensure
				the bar is updated correctly when paging to a different colour list

				This recalculates the layout rects and recaches the current colour list
				for itself, so may be called at any time.

				The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

void CColourBar::ScrollTheStrip(INT32 ScrollBy, BOOL RedrawAlways)
{
	Document *OldCurrentDoc = SetCurrentDoc();

	m_pCurrentColourList = ColourManager::GetColourList();
	if (m_pCurrentColourList == NULL)
	{
		RestoreCurrentDoc(OldCurrentDoc);
		return;
	}

	INT32 NewLeftmost = (INT32) (LeftmostColour + ScrollBy);

	CalculateRects();

	// INT32 MaxColours = (INT32) m_pCurrentColourList->GetUndeletedCount(); // FIXEDCOLOURS
	INT32 MaxColours = GetNumberOfColours();

	INT32 DisplayedColours = StripRect.width / CellSize;

	if (NewLeftmost > MaxColours - DisplayedColours)
		NewLeftmost = MaxColours - DisplayedColours;

	if (NewLeftmost < 0)
		NewLeftmost = 0;

	if ((UINT32) NewLeftmost != LeftmostColour || RedrawAlways)
	{
		ScrollBy = (INT32) (NewLeftmost - LeftmostColour); // Calculate REAL ScrollBy

		LeftmostColour = NewLeftmost;				// Update the scroll position
		if (ScrollHeight != 0)						// If there is a scroll bar...
			ForceRedrawOfRect(ScrollBarRect);		// ...force redraw it

PORTNOTE("other","Removed ScrollWindowEx usage")
#if !defined(EXCLUDE_FROM_XARALX)
		if (abs(ScrollBy) < DisplayedColours)
		{
			// Scroll the StripRect using ScrollWindowEx (this blits the existing
			// window contents across, then invalidates the bit that needs redrawing)
			wxRect RectToScroll(StripRect);
			if (ScrollBy < 0)
				RectToScroll.width += -ScrollBy * CellSize;
			else
			{
				RectToScroll.x -= ScrollBy * CellSize;
				RectToScroll.width += ScrollBy * CellSize;
			}

			ScrollWindow(-ScrollBy * CellSize, 0);
			Update();
		}
		else
#endif
			ForceRedrawOfRect(StripRect);	// Force redraw the entire colour strip
	}

	RestoreCurrentDoc(OldCurrentDoc);
}



/********************************************************************************************

>	void CColourBar::ApplyColour(IndexedColour *TheColour, BOOL SetLineColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/94 (separated from CellClicked() - originally written 23/6/94)
	Inputs:		TheColour - The IndexedColour to apply, or NULL to apply 'no colour' (complete
							transparency)
				SetLineColour - TRUE to set this as a line colour, FALSE to set as a fill colour

	Purpose:	Applies the given IndexedColour as a stroke/fill colour attribute to the
				selected objects. This also makes the colour the 'current' attribute, etc,
				if desired by the user.

	Scope:		private

	Notes:		Works on CurrentDoc, so ensure Current == Selected before calling this function

********************************************************************************************/

void CColourBar::ApplyColour(IndexedColour *TheColour, BOOL SetLineColour)
{
	NodeAttribute *Attrib = NULL;
	DocColour ColourToApply(COLOUR_TRANS);

	if (TheColour != NULL)
		ColourToApply.MakeRefToIndexedColour(TheColour);

	if (SetLineColour)
	{
		// Line colour selected so create a line colour attribute
		Attrib = new AttrStrokeColourChange;
		if (Attrib == NULL)
			return;

		((AttrStrokeColourChange *)Attrib)->SetStartColour(&ColourToApply);

		// AttributeSelected knows what to do with a selected attribute
		AttributeManager::AttributeSelected(NULL, Attrib); 

/*
		// Bodge Alert !!
		// To make blends work properly, we need to set the line transparecy.
		// We set it to Max if the colour is 'Tranaparent' or zero otherwise
		Attrib = new AttrStrokeTranspChange;
		if (Attrib == NULL)
			return;

		UINT32 Transp;

		if (ColourToApply.IsTransparent())
			Transp = 255;
		else
			Transp = 0;

		((AttrStrokeTranspChange *)Attrib)->SetStartTransp(&Transp);

		// AttributeSelected knows what to do with a selected attribute
		AttributeManager::AttributeSelected(NULL, Attrib); 
*/
	}
	else
	{	
		// Rather than setting the flat fill colour, we do an AttrColourChange, which
		// (I assume) will handle things like selected grad fill points, etc.
		Attrib = new AttrColourChange;
		if (Attrib == NULL)
			return;

		((AttrColourChange *)Attrib)->SetStartColour(&ColourToApply);

		// AttributeSelected knows what to do with a selected attribute
		AttributeManager::AttributeSelected(NULL, Attrib); 
	}

	// Note: Redraw of affected parts of the colour bar (indicator diamonds, etc) will
	// automatically occur as a result of the message which the AttributeManager will
	// broadcast in the above call. Thus we need do no more.
}


/********************************************************************************************

>	void CColourBar::CellClicked(INT32 CellIndex, BOOL SetLineColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		CellIndex - The index of the clicked colour cell (0..MaxColours)
				SetLineColour - TRUE to set the line colour to the given colour
				FALSE to set the fill color to the given colour
	Outputs:	-
	Returns:	-
	Purpose:	Processes a click on a given colour cell by setting the line/fill
				colour of the selected objects, or if nothing is selected, setting
				the default line/fill colour.

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

				If you add code to this routine, note the way in which we return - the
				selected order must always be deselected before exit

********************************************************************************************/

void CColourBar::CellClicked(INT32 CellIndex, BOOL SetLineColour)
{
	if ((CellIndex >= CLICKED_NOTHING && CellIndex != CLICKED_NOCOLOURCELL) ||
			m_pCurrentColourList == NULL)
		return;

	// Ensure that CurrentDoc == SelectedDoc
	Document *OldCurrentDoc = SetCurrentDoc();

	// Create an attribute of the correct type
	IndexedColour *TheColour  = NULL;
	if (CellIndex != CLICKED_NOCOLOURCELL)
	{
		// If the user has asked for document colours in the colour line then check these first
		if (ColourSGallery::ShowDocumentColours)
			TheColour = FindColourByIndex(CellIndex);

		// If we didn't find it there check the library colours
		if (TheColour == NULL)
		{
			// FIXED COLOURS
			// It is not one of the standard document colours so check the fixed library colours
			// If it finds the correct one then this will create an indexed colour and add it to
			// the currently selected document
			TheColour = FindLibIndexedColourByIndex(CellIndex);
			if (TheColour == NULL)
			{
				// It is an unknown colour so just exit
				// Before exiting the function, we must always restore CurrentDoc
				RestoreCurrentDoc(OldCurrentDoc);
				return;
			}
		}
	}

	ApplyColour(TheColour, SetLineColour); 		// Apply this colour to the selection

	RestoreCurrentDoc(OldCurrentDoc);			// Restore previous CurrentDoc value
}



/********************************************************************************************

>	TCHAR *CColourBar::HelpCallbackHandler(wxWindow* Window, INT32 Item, void *UserData)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/94 (moved to new CColourBar from ColourBar 23/6/94)
	Inputs:		Window - The window handle for which bubble help is required (ignored)
				Item - The item (in our case the colour index) on which we want help
				UserData - user data passed on to us by the help system - ignored

	Purpose:	Callback handler for providing Bubble help over the Colour Bar window.

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

TCHAR *CColourBar::HelpCallbackHandler(wxWindow* Window, INT32 Item, void *UserData)
{
	static String_64 HelpStringStore;
	StringBase *HelpString = NULL;

	if (TheColourBar == NULL || Item == CLICKED_NOTHING)
		return(NULL);

//	ASSERT_VALID(TheColourBar);

	if (Item >= CLICKED_NOTHING)
	{
		switch(Item)
		{
			case CLICKED_SCROLLBAR:			// 'Drag to scroll'
				HelpStringStore.MakeMsg(_R(IDS_COLBAR_HSCROLL));
				HelpString = &HelpStringStore;
				break;

			case CLICKED_SCROLLBARLEFT:		// 'Click to scroll'
			case CLICKED_SCROLLBARRIGHT:
			case CLICKED_LEFTSCROLL:
			case CLICKED_RIGHTSCROLL:
				HelpStringStore.MakeMsg(_R(IDS_COLBAR_HBUTTONS));
				HelpString = &HelpStringStore;
				break;

			case CLICKED_INDICATORS:		// 'Current line/fill colours'
				HelpStringStore.MakeMsg(_R(IDS_COLBAR_HINDICATOR));
				HelpString = &HelpStringStore;
				break;

			case CLICKED_NOCOLOURCELL:
				HelpStringStore.MakeMsg(_R(IDS_COLBAR_HNOCOLOUR));
				HelpString = &HelpStringStore;
				break;

			case CLICKED_EDITBUTTON:
				HelpStringStore.MakeMsg(_R(IDS_COLBAR_HEDITBUTTON));
				HelpString = &HelpStringStore;
				break;

#if FALSE
			case CLICKED_NEWBUTTON:
				HelpStringStore.MakeMsg(_R(IDS_COLBAR_HNEWBUTTON));
				HelpString = &HelpStringStore;
				break;
#endif
		}
	}
	else
	{
		// We are over a colour cell - bubble-help its name
		IndexedColour *TheColour = NULL;
		// If the user has the preference set then include and hence show the document
		// colours
		if (ColourSGallery::ShowDocumentColours)
			TheColour = TheColourBar->FindColourByIndex(Item);

		if (TheColour != NULL)
		{
			HelpStringStore = *(TheColour->GetName());
			HelpString = &HelpStringStore;
		}
		else
		{
PORTNOTE("other","Removed ColourSGallery usage")
#if !defined(EXCLUDE_FROM_XARALX)
			// Check to see if its a colour library item
			SGDisplayLibColour *pLibColour = NULL;
			DocColour * pTheColour = TheColourBar->FindLibColourByIndex(Item, &pLibColour);
			if (pTheColour && pLibColour)
			{
				// Get the name of the item from the LibColour gallery item
				// which is keeping it safe for us
				String_256 Buffer;
				pLibColour->GetNameText(&Buffer);
				HelpStringStore = Buffer;
				HelpString = &HelpStringStore;
			}
#endif
		}
	}

	if (HelpString != NULL)
		return((TCHAR *) *HelpString);

	return(NULL);
}



/********************************************************************************************

>	static void CColourBar::PaletteHasChanged(ColourList *ChangedList)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		ChangedList - NULL if the list has been paged to a different one,
				or a pointer to the list in which a change has been made.
				
	Purpose:	This will cause the colourbar to redraw itself with the new list
				if necessary, and cache new values for the new current list.

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

void CColourBar::PaletteHasChanged(ColourList *ChangedList)
{
	if (TheColourBar == NULL)		// There is no colour bar!
		return;

	// Scroll (if necessary) to show the current colour indicators properly
	TheColourBar->SelectionHasChangedInternal();

	//   a) Scroll the strip by 0 cells to ensure it is scrolled to a legal position
	TheColourBar->ScrollTheStrip(0);

	//   b) Force redraw the window. We pass FALSE so the window does not flicker as
	//      it is updated (as the Paint() function completely fills the window anyway)
	//   c) If the bar needs to add/remove a scrollbar, force a layout recalculation
	wxRect TheRect = TheColourBar->GetClientRect();
	INT32 NewBarHeight = TheColourBar->CalculateNewBarHeight(&TheRect);

	TheColourBar->OldBarHeight = TheColourBar->GetCurrentBarHeight();

	if (NewBarHeight == TheColourBar->OldBarHeight)
		TheColourBar->Refresh(false);		// Redraw, but don't erase bground (stops flicker)
	else
	{
		TheColourBar->Refresh(true);		// Redraw (clear to grey for tidier resize)
		GetMainFrame()->UpdateFrameManager();	// Resize to add/remove the scrollbar
	}
}



/********************************************************************************************

>	INT32 CColourBar::CalculateColourCellIndex(IndexedColour *TheColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94
	Inputs:		TheColour - The colour to find the index of
	Returns:	The index (0..NumColours-1) of the colour cell containing the
				colour, or CELLINDEX_NOCELL (-2) if the colour wasn't found

	Purpose:	Searches the colour list to find the given colour, returning its
				index in the list if it is found, or a negative value if it is not.

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

INT32 CColourBar::CalculateColourCellIndex(IndexedColour *TheColour)
{
	m_pCurrentColourList = ColourManager::GetColourList();

	if (TheColour == NULL || m_pCurrentColourList == NULL)
		return(CELLINDEX_NOCELL);

	INT32 Index = 0;

	// If the user has asked for document colours in the colour line then check these first
	// Indexed Colours are document colours and so can never exist in the library groups.
	if (ColourSGallery::ShowDocumentColours)
	{
		IndexedColour *TestColour = m_pCurrentColourList->GetUndeletedHead();

		while (TestColour != NULL)
		{ 
			if (TheColour == TestColour)
				return((INT32) Index);

			TestColour = m_pCurrentColourList->GetUndeletedNext(TestColour);
			Index++;
		}
	}

	// Didn't find a match for this colour in the list
	return(CELLINDEX_NOCELL);
}



/********************************************************************************************

>	INT32 CColourBar::CalculateColourCellIndex(DocColour *TheColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		TheColour - the colour whose Cell index you wish to calculate
	Returns:	CELLINDEX_NOCOLOURCELL, CELLINDEX_NOCELL, or the index of the given colour

	Purpose:	Given a DocColour, will determine which *visible* colour cell, if any,
				is displaying that colour, anmd return its Cell Index.

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

INT32 CColourBar::CalculateColourCellIndex(DocColour *TheColour)
{
	if (TheColour == NULL)
		return(CELLINDEX_NOCELL);

	if (TheColour->IsTransparent())
		return(CELLINDEX_NOCOLOURCELL);

	return (CalculateColourCellIndex(TheColour->FindParentIndexedColour()));
}



/********************************************************************************************

>	void CColourBar::ForceRedrawOfCell(INT32 CellIndex)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94
	Inputs:		CellIndex - Index of the colour cell to redraw
	Purpose:	Forces a redraw of the given Cell in the colour strip. The index is from
				the first colour in the list - if this is outside the displayed portion of
				the colour strip, no action will be taken
	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

void CColourBar::ForceRedrawOfCell(INT32 CellIndex)
{
	if (CellIndex == CELLINDEX_NOCOLOURCELL)		// Redraw No-colour cell
	{
		wxRect Rect;
		CalculateNoColourCellRect(&Rect);

		ForceRedrawOfRect(Rect);
		return;
	}

	if (CellIndex < (INT32)LeftmostColour)			// Cell is not visible (off left of strip)
		return;

	// Calculate position of the colour cell in the strip...
	wxRect RedrawRect(StripRect);					// Copy the striprect

													// Calculate left edge of the cell
	RedrawRect.x += CellSize * (CellIndex - LeftmostColour);
	if (RedrawRect.x > (StripRect.x + StripRect.width))	// Cell is not visible (off right of strip)
		return;

	RedrawRect.width = CellSize;					// Set width of the cell
	ForceRedrawOfRect(RedrawRect);					// redraw the sucker
}



/********************************************************************************************

>	void CColourBar::SelectionHasChangedInternal(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94
	Purpose:	This will cause the colourbar to redraw any necessary portions of the
				colour strip and indicators to indicate the colour of the selection
				(or the default line/fill colours if there is no selection)

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

void CColourBar::SelectionHasChangedInternal(void)
{
	static Document *LastDocument = NULL;
	static INT32 LastClickedLineCell = CELLINDEX_NOCELL;
	static INT32 LastClickedFillCell = CELLINDEX_NOCELL;
	static INT32 LastClickedEndCell  = CELLINDEX_NOCELL;

	// If the user has not asked for document colours in the colour line then 
	// no need to do any of the checks below
	if (!ColourSGallery::ShowDocumentColours)
	{
		ForceRedrawOfRect(TheColourBar->IndicatorRect);	 		// And redraw the colour indicators
		return;
	}

	// If swapped documents then forget the last clicked cells
	if (LastDocument != Document::GetSelected())
	{
		LastClickedFillCell = LastClickedEndCell = LastClickedLineCell = CELLINDEX_NOCELL;
		LastDocument = Document::GetSelected();
	}

	DocColour *LineColour;
	DocColour *FillColour;
	DocColour *EndColour;

	BOOL DiamondShape = ColourManager::GetCurrentLineAndFillColours(&LineColour,
																&FillColour, &EndColour);
	INT32 MinorScrollTo = CELLINDEX_NOCELL;
	INT32 MajorScrollTo = CELLINDEX_NOCELL;

	// Determine if we're changing from flatfill display to gradfill or vice-versa. In this case,
	// the Fill diamond will change between arow and diamond - if the fill happens to be the same
	// colour, this will leave the screen state incorrect, so we must forceredraw it.
	BOOL MustRedrawFill = FALSE;
	if (EndColour != NULL)
	{
		if (LastEndCell == CELLINDEX_NOCELL)					// Change from flat to grad
			MustRedrawFill = TRUE;
	}
	else
	{
		if (LastEndCell != CELLINDEX_NOCELL)					// Change from grad to flat
			MustRedrawFill = TRUE;
	}

	// Now redraw (only as necessary) the line, fill and end-fill cells, and determine
	// the major and minor cell positions to autoscroll to.
	if (LineColour == NULL)										// Is there a line colour?
	{
		ForceRedrawOfCell((INT32) LastLineCell);	   				// No - Remove the old indicator diamond
		LastLineCell = CELLINDEX_NOCELL;						// ... and remember it is not displayed
	}
	else
	{
		INT32 CellIndex = CalculateColourCellIndex(LineColour);

		if (CellIndex != LastClickedLineCell)					// Set Minor scroll position, and
			MinorScrollTo = LastClickedLineCell = CellIndex;	// remember where we last put diamond

		if (MinorScrollTo == CELLINDEX_NOCOLOURCELL)			// NoColour doesn't cause scrolling
			MinorScrollTo = CELLINDEX_NOCELL;

		// If selected cell changed (or diamond shape has changed)...
		if (CellIndex != LastLineCell || LastDiamondShape != DiamondShape)
		{
			ForceRedrawOfCell((INT32)LastLineCell);				// Redraw old colour cell
			ForceRedrawOfCell(CellIndex);						// Redraw new colour cell
			LastLineCell = CELLINDEX_NOCELL;
			// (LastLineCell will be automagically updated by the PaintDiamond code)
		}
	}

	if (FillColour == NULL)										// is there a Fill colour?
	{
		ForceRedrawOfCell((INT32)LastFillCell);					// No- Remove the old indicator diamond
		LastFillCell = CELLINDEX_NOCELL;						// ... and remember it is not displayed
	}
	else
	{
		INT32 CellIndex = CalculateColourCellIndex(FillColour);

		if (CellIndex != LastClickedFillCell)					// Set Major scroll position, and
			MajorScrollTo = LastClickedFillCell = CellIndex;	// remember where we last put diamond

		if (MajorScrollTo == CELLINDEX_NOCOLOURCELL)			// NoColour doesn't cause scrolling
			MajorScrollTo = CELLINDEX_NOCELL;

		// If selected cell changed (or diamond shape has changed)...
		if (CellIndex != LastFillCell || LastDiamondShape != DiamondShape || MustRedrawFill)
		{
			ForceRedrawOfCell((INT32)LastFillCell);				// Redraw old colour cell
			ForceRedrawOfCell(CellIndex);						// Redraw new colour cell
			LastFillCell = CELLINDEX_NOCELL;
			// (LastFillCell will be automagically updated by the PaintDiamond code)
		}
	}

	if (EndColour == NULL)
	{
		ForceRedrawOfCell((INT32)LastEndCell);					// No- Remove the old indicator diamond
		LastEndCell = CELLINDEX_NOCELL;							// ... and remember it is not displayed
	}
	else
	{
		INT32 OtherMajor = CELLINDEX_NOCELL;
		INT32 CellIndex = CalculateColourCellIndex(EndColour);

		if (CellIndex != LastClickedEndCell)					// Set alternat major scroll position, and
			OtherMajor = LastClickedEndCell = CellIndex;		// remember where we last put diamond

		if (OtherMajor == CELLINDEX_NOCOLOURCELL)				// NoColour doesn't cause scrolling
			OtherMajor = CELLINDEX_NOCELL;

		// If selected cell changed (or diamond shape has changed)...
		if (CellIndex != LastEndCell || LastDiamondShape != DiamondShape)
		{
			ForceRedrawOfCell((INT32)LastEndCell);				// Redraw old colour cell
			ForceRedrawOfCell(CellIndex);						// Redraw new colour cell
			LastEndCell = CELLINDEX_NOCELL;
			// (LastEndCell will be automagically updated by the PaintDiamond code)
		}

		// And now decide which of the 3 colours are most important, and decide on major and minor
		// scroll-to positions. By default this is already set up to be Fill,Line, but if the end-grad
		// colour is not no-colour, it may override the line colour
		if (OtherMajor != CELLINDEX_NOCELL)
		{
			if (MajorScrollTo == CELLINDEX_NOCELL)
				MajorScrollTo = OtherMajor;			// There's only one non-transparent fill, so use it
			else
			{
				if (MinorScrollTo == CELLINDEX_NOCELL)
					MinorScrollTo = OtherMajor;		// There's no line colour, so just make it the minor
				else
				{
					// We have 3 colours - we will remove the middle one, to maximise the chance of
					// them all being shown (if left/right are visible, the middle must also be visible)

					if (MinorScrollTo < MajorScrollTo)
					{
						if (OtherMajor < MinorScrollTo)
							MinorScrollTo = OtherMajor;
						else if (OtherMajor > MajorScrollTo)
							MajorScrollTo = OtherMajor;
					}
					else
					{
						if (OtherMajor > MinorScrollTo)
							MinorScrollTo = OtherMajor;
						else if (OtherMajor < MajorScrollTo)
							MajorScrollTo = OtherMajor;
					}
				}
			}
		}
	}

	
	// OK, let's autoscroll - Preferably both Major and Minor can be shown, but if not, it is better
	// to show Major (fill) than Minor (line).

	if (MajorScrollTo == CELLINDEX_NOCELL)	// Only one diamond, so make it the major one
	{
		MajorScrollTo = MinorScrollTo;
		MinorScrollTo = CELLINDEX_NOCELL;
	}

	if (MajorScrollTo != CELLINDEX_NOCELL)
	{
		// We have 1 or 2 cells that we'd really like visible in the bar if possible.
		// The MajorScrollTo is the most important one, so we ensure it is visible, and
		// then shift slightly from that scroll pos, if possible, to get the minor in too
		// It's also important to minimise scrolling (i.e if the targets can all be seen
		// already then we will not move them)

		// Calculate how many colour cells are visible
		INT32 NumDisplayedColours = StripRect.width / CellSize;

		BOOL TwoDiamonds = TRUE;			// There are either 1 or 2 diamonds to try to show

		// Convert the cell index into a desired scroll offset from current pos (scroll-by)
		MajorScrollTo -= LeftmostColour;
		if (MinorScrollTo == CELLINDEX_NOCELL)
		{
			MinorScrollTo = MajorScrollTo;	// Ensure we scroll to Major one only
			TwoDiamonds = FALSE;			// There is only one diamond to try to show
		}
		else
			MinorScrollTo -= LeftmostColour;

		// If one of the 2 diamonds is not already visible, we need to scroll
		if (MajorScrollTo < 0 || MajorScrollTo >= NumDisplayedColours ||
			MinorScrollTo < 0 || MinorScrollTo >= NumDisplayedColours)
		{
			INT32 ScrollBy = 0;

			// One or both diamonds are not visible, so we'll have to try scrolling...
			if (TwoDiamonds && abs(MajorScrollTo - MinorScrollTo) <= NumDisplayedColours)
			{
				if (abs(MajorScrollTo - MinorScrollTo) >= NumDisplayedColours - 1)
				{
					// The diamonds fit exactly at the left/right ends of the bar. We must
					// special case this (rounding errors in the average may get the posn
					// one cell out in this case) - scroll the leftmost diamond to the
					// left end of the bar.

					ScrollBy = min(MinorScrollTo, MajorScrollTo);
				}
				else
				{
					// Both diamonds should fit on if we just scroll to the average position
					ScrollBy = (MajorScrollTo + MinorScrollTo) / 2;
					ScrollBy -= NumDisplayedColours / 2;		// Turn into center-relative scroll
 				}
			}
			else
			{
				// There is only one diamond, or we can only fit one of the diamonds
				// on, so scroll to the major one
				ScrollBy = MajorScrollTo - NumDisplayedColours / 2;	// Turn into center-relative scroll
			}

#ifndef EXCLUDE_GALS
			if (
				ColourSGallery::AutoScrollSelection && 
				ScrollBy != 0)
			{
				ScrollTheStrip(ScrollBy);
			}
#endif
		}
	}

	ForceRedrawOfRect(TheColourBar->IndicatorRect);	 		// And redraw the colour indicators
}



/********************************************************************************************

>	static void CColourBar::SelectionHasChanged(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94
	Purpose:	This will cause the colourbar to redraw any necessary portions of the
				colour strip and indicators to indicate the colour of the selection
				(or the default line/fill colours if there is no selection)

	Notes:		This is just a veneer. It is a static function which finds the current
				ColourBar (if any) and passes the call onto its SelectionHasChangedInternal
				function
				The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

void CColourBar::SelectionHasChanged(void)
{
	if (TheColourBar != NULL)
		TheColourBar->SelectionHasChangedInternal();
}



/********************************************************************************************

>	void CColourBar::ReleaseAllClaims(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Inputs:		-
	Purpose:	Ensures that all our temporary claims (timers, mouse capture) are
				released.

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care
				
********************************************************************************************/

void CColourBar::ReleaseAllClaims(void)
{
	if (DragInfo.TimerOn)
	{
		m_DragTimer.Stop();
		DragInfo.TimerOn = FALSE;
	}

	if (HasCapture())
	{
		ReleaseMouse();
	}
	DragInfo.MouseCaptured = FALSE;
}



/********************************************************************************************

>	void CColourBar::SetATimer(BOOL Delay)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Inputs:		Delay - TRUE if this is initial keyboard-delay-before-autorepeat,
				FALSE for auto-repeat

	Purpose:	Sets a timer to cause a timer message to be sent to us after
				the keyboard auto-repeat delay. 
				If we are already waiting for a timer event, the current timer will
				be replaced by the new one.

	Notes:		Delay is currently ignored, because Charles didn't like it (and I
				agree with him).

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care
				
********************************************************************************************/

void CColourBar::SetATimer(BOOL Delay)
{
	if (DragInfo.TimerOn)
		m_DragTimer.Stop();

	UINT32 CallbackTime = 1000;
	UINT32 RepsPerSec = 5;

#if FALSE
	if (Delay)
	{
		::SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &RepsPerSec, 0);

		RepsPerSec = 4 - RepsPerSec;
	}
	else
	{
		::SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &RepsPerSec, 0);

		RepsPerSec++;
	}
#endif

	if (RepsPerSec > 1)
		CallbackTime = (UINT32) 1000 / RepsPerSec;

	m_DragTimer.Start(CallbackTime);
	DragInfo.TimerOn = TRUE;
}



/********************************************************************************************

>	void CColourBar::CaptureTheMouse(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Inputs:		-
	Purpose:	Captures the mouse for drags/autorepeat. Nothing will be done if
				we already have mouse capture.

	Notes:		When capturing the mouse, you should also set the value of
				DragInfo.CaptureRect to the area within which the mouse was captured.
				This allows the other handlers to determine if the mouse strays
				out of the relevant area (in which case scrolling/autorepeat will
				cease). If not set, this will cause odd effects.

				The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

void CColourBar::CaptureTheMouse(void)
{
	TRACEUSER("Gerry", _T("CColourBar::CaptureTheMouse"));

	if (DragInfo.MouseCaptured)
	{
		TRACEUSER("Gerry", _T("Already got capture"));
		return;
	}

	CaptureMouse();
	DragInfo.MouseCaptured = TRUE;
}



/********************************************************************************************

>	void CColourBar::EditAColour(ColourList *DisplayList, IndexedColour *TheColour,
									BOOL LineColour = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		DisplayList - The ColourList in which the colour resides.
				TheColour - The Colour to be edited
				LineColour - TRUE if this should be the line colour (ignored unless
							TheColour and DisplayList are both non-NULL)
	Outputs:	-
	Returns:	-
	Purpose:	Edits the given colour.

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

	Scope:		private
	Errors:		-
	SeeAlso:	ColourPicker::EditColour

********************************************************************************************/

void CColourBar::EditAColour(ColourList *DisplayList, IndexedColour *TheColour,
								BOOL LineColour)
{
	ColourPicker ColPicker;

	if (DisplayList == NULL || TheColour == NULL)
		ColPicker.EditColour(NULL, NULL, LineColour);	// Open, but don't edit anything
	else
		ColPicker.EditColour(DisplayList, TheColour);
}



/********************************************************************************************

>	static void CColourBar::RedrawGBrushAreas(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Purpose:	Force-redraws all areas of the colourbar that depend upon GBrush
				for their rendering. Used by winoil\gbrush.cpp when GBrush rendering
				is enabled or disabled in order to redraw the colourbar.

	Notes:		The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

void CColourBar::RedrawGBrushAreas(void)
{
	if (TheColourBar == NULL)
		return;

	TheColourBar->CalculateRects();
	TheColourBar->ForceRedrawOfRect(TheColourBar->StripRect);	
	TheColourBar->ForceRedrawOfRect(TheColourBar->IndicatorRect);	
	TheColourBar->Update();
}



/********************************************************************************************

>	IndexedColour *CColourBar::FindColourByIndex(UINT32 Index)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Returns:	NULL, or a pointer to the IndexedColour we are interested in.

	Purpose:	Given the current Display List, finds the 'Index'th item, and
				dereferences it to find the actual IndexedColour that lies
				at that list position in the current colour bar display order.

	Notes:		Ensures that m_pCurrentColourList is up to date. If no list can be found,
				it returns NULL.

				Ignores 'deleted' colours held in the colour list

	Scope:		private (to ccolourbar)

********************************************************************************************/

IndexedColour *CColourBar::FindColourByIndex(UINT32 Index)
{
	if (m_pCurrentColourList == NULL)
		m_pCurrentColourList = ColourManager::GetColourList();

	if (m_pCurrentColourList == NULL)
	{
		ERROR3(_T("CColourBar::FindColourByIndex can't handle NULL Colour Lists"));
		return(NULL);
	}

	// First, check if the index is in the main colour list
	UINT32 DocColours = (UINT32) m_pCurrentColourList->GetUndeletedCount();
	if (Index < DocColours)
		return(m_pCurrentColourList->FindUndeletedItem(Index));
	
	// not in this list so return NULL
	return NULL;
}

/********************************************************************************************

>	DocColour * CColourBar::FindLibColourByIndex(UINT32 Index, SGDisplayLibColour **ppLibColour = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/97
	Inputs:		The index of the item required
				An optional SGDisplayLibColour pointer, defaults to null.
	Returns:	NULL, or a pointer to the DocColour we are interested in.
	Purpose:	Given the current Display List, finds the 'Index'th item, and
				dereferences it to find the actual DocColour that lies
				at that list position in the current colour bar display order.
				This will only look through the Library colours and hence returns DocColours
				rather than indexed colours.

********************************************************************************************/

DocColour * CColourBar::FindLibColourByIndex(UINT32 Index, SGDisplayLibColour **ppLibColour)
{
	// First, check if the index is in the main colour list
	UINT32 DocColours = 0;
	// If the user has the preference set then include and hence show the document
	// colours
	if (ColourSGallery::ShowDocumentColours)
	{
		DocColours = (UINT32) m_pCurrentColourList->GetUndeletedCount();
		if (Index < DocColours)
			return NULL;
	}

PORTNOTE("other","Removed ColourSGallery usage")
#if !defined(EXCLUDE_FROM_XARALX)
	// The new index count has to have these doc colours removed from the original index
	UINT32 NewIndex = Index - DocColours;

	// Index is beyond the colours in the current document and so try the colour libraries
	m_pColourGallery = ColourSGallery::GetInstance();
	UINT32 items = 0;
	if (m_pColourGallery != NULL)
	{
		// This will create the groups if they're not there yet
		m_pColourGallery->MakeSureGroupsHaveBeenCreated();

		// Find the Netscape palette library
		SGDisplayLibColGroup * pLibGroup = m_pColourGallery->GetFirstLibGroup();
		BOOL Found = FALSE;
		while (!Found && pLibGroup)
		{
			// Add in any items that are flagged as being required
			if (pLibGroup->DisplayInColourLine())
			{
				// pLibGroup->DeVirtualise();	// This is dangerous as it may change current doc!
												// Disabled as it causes problems during start up
				items = pLibGroup->CountChildren();
				if (NewIndex < items)
				{
					return pLibGroup->GetItemColour(NewIndex, ppLibColour);
				}
				else
				{
					// remove these items from the count
					NewIndex -= items;
				}
			}

			pLibGroup = m_pColourGallery->GetNextLibGroup(pLibGroup);
		}
	}
#endif
	// Nothing found
	return NULL;
} 

/********************************************************************************************

>	IndexedColour * CColourBar::FindLibIndexedColourByIndex(UINT32 Index)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/97
	Inputs:		The index of the item required
	Returns:	NULL, or a pointer to the IndexedColour we are interested in.
	Purpose:	Given the current Display List, finds the 'Index'th item, and
				dereferences it to find the actual DocColour that lies
				at that list position in the current colour bar display order.
				This will only look through the Library colours and hence finds DocColours
				rather than indexed colours. This versions is the same as above but then
				goes and creates an IndexedColour in the current document and returns this.

********************************************************************************************/

IndexedColour * CColourBar::FindLibIndexedColourByIndex(UINT32 Index)
{
	// If we have no current colour list then nothing doing
	if (m_pCurrentColourList == NULL)
		return NULL;

	IndexedColour *pNewCol = NULL;

	SGDisplayLibColour *pLibColour = NULL;
	DocColour * pTheColour = FindLibColourByIndex(Index, &pLibColour);

	if (pTheColour == NULL || pLibColour == NULL)
		return NULL;

	// We have discovered a library DocColour - we must copy it into the document before trying to apply it
	// Make a temporary IndexedColour from the library colour
	pNewCol = new IndexedColour(*pTheColour);

	if (pNewCol != NULL)
	{
		// Set the colour's name to the same as the library item (but make sure it's unique)
		String_256 Buffer;
		pLibColour->GetNameText(&Buffer);
		pNewCol->SetName(Buffer);
// If we do this then the EnsureColourIsInDocument wont do all its work.
// It will handle duplicate names for us
//		if (m_pCurrentColourList->GenerateUniqueColourName(pNewCol->GetName(), (String_64 *) &Buffer))
//			pNewCol->SetName(Buffer);

		// If it's a spot colour, make it so
		if (pLibColour->IsASpotColour())
			pNewCol->SetLinkedParent(NULL, COLOURTYPE_SPOT);

		// Copy the colour into the destination document (merging it with existing
		// colours so we won't keep creating new copies of the same colour as it's applied)
		DocColour ColourToApply;
		ColourToApply.MakeRefToIndexedColour(pNewCol);
		ColourManager::EnsureColourIsInDocument(NULL, Document::GetSelected(), &ColourToApply);

		// Delete the temporary IndexedColour we used
		delete pNewCol;

		// And remember the new colour we've just made
		pNewCol = ColourToApply.FindParentIndexedColour();
	}

	// return the result to the caller
	return pNewCol;
}



/********************************************************************************************

>	static void CColourBar::Show(BOOL ShowOrHide = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/94
	Inputs:		HideOrShow - TRUE to Show the window (the default)
							 FALSE to hide the window

	Purpose:	Tells the colour bar to show or hide itself. The bar will remain
				visible/hidden until the next call to this method. If no colour bar
				currently exists, the desired state will be remembered and used
				on creation of any new bar.

	Scope:		public

********************************************************************************************/

void CColourBar::Show(BOOL ShowOrHide)
{
	BarIsVisible = ShowOrHide;					// Remember our current state

	if (TheColourBar != NULL)					// If a colour bar exists...
	{
		if (BarIsVisible)
			TheColourBar->Show(true);			// Show or hide the window
		else
			TheColourBar->Show(false);

		GetMainFrame()->UpdateFrameManager();	// And ensure it places itself usefully!
	}
}


/********************************************************************************************

>	void CColourBar::OnSize(wxSizeEvent &event)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		all ignored
	Purpose:	Handle resizing of our window. This ensures that the scroll offset
				is within the new bounds of the colour strip, and forces a redraw.

********************************************************************************************/

void CColourBar::OnSize(wxSizeEvent &event)
{
	TRACEUSER("Gerry", _T("CColourBar::OnSize(%d, %d)\n"), event.m_size.x, event.m_size.y);
	if (m_pCurrentColourList == NULL)
	{
		Refresh();		// No colour list - just ensure we're redrawn OK, and exit
		return;
	}

	INT32 NewWidth = event.GetSize().x;
	INT32 NewHeight = event.GetSize().y;

	// Calculate what the new window rect is going to be (width is all the it really needs
	// to know) and from it, calculate where ther striprect is going to be placed. From
	// this, we can calculate how many colours will be displayed at the new window size.
	wxRect AvailableRect(0, 0, NewWidth, NewHeight);

	wxRect tStripRect;		// Temporary lookalikes to the global cached rects
	wxRect tIndicatorRect;
	wxRect tScrollBarRect;
	wxRect tEditButtonRect;
	wxRect tNewButtonRect;	// Warning - this button is now defunct

	CalculateRectsInternal(&AvailableRect,
							&tStripRect, &tIndicatorRect, &tScrollBarRect,
							&tEditButtonRect, &tNewButtonRect);

	INT32 NewDisplayedColours = tStripRect.width / CellSize;
	
	//INT32 MaxColours = (INT32) m_pCurrentColourList->GetUndeletedCount(); // FIXEDCOLOURS
	INT32 MaxColours = (INT32)GetNumberOfColours();

	// If height of bar unchanged, we may not need to redraw (stop solid-drag flicker)...
	if (NewHeight == OldBarHeight)
	{
		INT32 OldDisplayedColours = StripRect.width / CellSize;

		// If we can display exactly the same number of colours, no need to redraw
		if (NewDisplayedColours == OldDisplayedColours)
			return;

		// if all colours were & still-are displayed, then we need not redraw
		if (NewDisplayedColours >= MaxColours && OldDisplayedColours >= MaxColours)
			return;
	}

	// Ensure the scroll position is valid (similar to ScrollTheStrip(0))
	INT32 NewLeftmost = (INT32)LeftmostColour;

	if (NewLeftmost > MaxColours - NewDisplayedColours)
		NewLeftmost = MaxColours - NewDisplayedColours;
	if (NewLeftmost < 0)
		NewLeftmost = 0;

	LeftmostColour = (UINT32) NewLeftmost;

	Refresh(false);			// Force-redraw the entire window

	// Finally, remember the new size of the bar so we don't unnecessarily redraw
	// if we are resized again soon (i.e. solid drag)
	OldBarHeight = NewHeight;
}



/********************************************************************************************

>	static INT32 CColourBar::GetCurrentBarHeight(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/96

	Returns:	The height, in pixels, the bar is currently taking on screen
				If there is no colour bar currently displayed, the return value is 0

	Purpose:	Determine how high the colour bar currently is

	SeeAlso:	CColourBar::CalculateNewBarHeight; CColourBar::OnSize

********************************************************************************************/

INT32 CColourBar::GetCurrentBarHeight(void)
{
	if (TheColourBar == NULL || !TheColourBar->BarIsVisible)
		return(0);

	wxRect ClientRect = TheColourBar->GetClientRect();
	return(ClientRect.height);
}



/********************************************************************************************

>	INT32 CColourBar::CalculateNewBarHeight(wxRect *AvailableRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/94

	Inputs:		AvailableRect - The space available for the colourbar to fit into.
				Only the available x space is significant.
	Outputs:	-
	Returns:	The height, in pixels, the bar will need to be at this width in order to
				contain all its 'controls'.

	Purpose:	Determine how high the colour bar should be - this is difficult, because
				the height depends upon whether we want a scrollbar, and this in turn
				depends upon how wide the colourstrip is.

	Notes:		AvailableRect is now preserved by this call

	SeeAlso:	CColourBar::GetCurrentBarHeight; CColourBar::OnSize

********************************************************************************************/

INT32 CColourBar::CalculateNewBarHeight(wxRect *AvailableRect)
{
	TRACEUSER("Gerry", _T("CColourBar::CalculateNewBarHeight"));

	if (ScrollHeight == 0)						// Has no scroll bar, so always the same height
	{
		TRACEUSER("Gerry", _T("   returning %d"), BarHeight);
		return(BarHeight);
	}

	// Work out how many colours we want to display in the window.
	// Removed this line so that FullScreen mode (which destroys and recreates the colour 
	// gallery) does not leave us with a cached pointer to garbage memory.
	// if (m_pCurrentColourList == NULL)
		m_pCurrentColourList = ColourManager::GetColourList();

	if (m_pCurrentColourList == NULL)				// No colours; we won't display anything anyway
	{
//		TRACEUSER("Gerry", _T("   returning %d"), BarHeight - ScrollHeight);
//		return(BarHeight - ScrollHeight);
		return(CellSize + 4);
	}

	// Calculate where all the rectangles are going to be placed if the window is resized
	// to the 'AvailableRect'. From this, we get the projected StripRect size, which allows
	// us to project the number of colours we will be able to display.
	
	wxRect tStripRect;		// Temporary lookalikes to the global cached rects
	wxRect tIndicatorRect;
	wxRect tScrollBarRect;
	wxRect tEditButtonRect;
	wxRect tNewButtonRect;		// Warning - this button is now defunct

	CalculateRectsInternal(AvailableRect,
							&tStripRect, &tIndicatorRect, &tScrollBarRect,
							&tEditButtonRect, &tNewButtonRect);

	INT32 DisplayedColours = tStripRect.width / CellSize;

	// We can display all the colours, we don't need a scrollbar, so only need CellSize+4 pixels
	//if ((INT32) m_pCurrentColourList->GetUndeletedCount() <= DisplayedColours) // FIXEDCOLOURS
	if ((INT32)GetNumberOfColours() <= DisplayedColours)
	{
//		TRACEUSER("Gerry", _T("   returning %d"), BarHeight - ScrollHeight);
//		return(BarHeight - ScrollHeight);
		return(CellSize + 4);
	}

	// We can't display all colours, so we need the desired BarHeight, which includes the
	// size of the colourstrip and the scrollbar (if enabled)
	TRACEUSER("Gerry", _T("   returning %d"), BarHeight);
	return(BarHeight);
}



/********************************************************************************************

>	LRESULT CColourBar::OnSizeParent(WPARAM, LPARAM lParam)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		dunno really. WPARAM and LPARAM as per normal for this sorta thing
	Purpose:	Handle resizing of our parent - ask for the amount of client
				space we want to occupy. This is done by setting a member
				variable and then calling up to a base class method.

	Notes:		This function is implemented differently for MFC 2 and 3.
				Remember to change both sections as appropriate when editing it.

********************************************************************************************/
#if FALSE
LRESULT CColourBar::OnSizeParent(WPARAM, LPARAM lParam)
{
	INT32 HeightOfBar = 0;			// By default, bar height = 0 (for standalone code)

	// Ask for the amount of room we desire.
	// This must be done differently for MFC versions 2 and 3
// ---------------------------------------------------------------------
#if _MFC_VER < 0x300

#ifndef STANDALONE							// The standalone version leaves HeightOfBar = 0 (no bar)
	if ((m_dwStyle & WS_VISIBLE) != 0)		// If visible, we want to grab some space
	{
		AFX_SIZEPARENTPARAMS FAR* lpLayout = (AFX_SIZEPARENTPARAMS FAR*)lParam;

		wxRect AvailableRect;				// The available space for us to fill
		AvailableRect.CopyRect(&lpLayout->rect);

		HeightOfBar = CalculateNewBarHeight(&AvailableRect);	// (corrupts AvailableRect)
	}
#endif

	m_sizeFixedLayout.cy = HeightOfBar;		// Set the desired height of the bar

	return (CControlBar::OnSizeParent((WPARAM)0, lParam));	// invoke normal method

#else
// _MFC_VER >= 0x300 ---------------------------------------------------

	AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;

	// resize and reposition this control bar based on styles
	DWORD dwStyle = (m_dwStyle & (CBRS_ALIGN_ANY|CBRS_BORDER_ANY)) |
		(GetStyle() & WS_VISIBLE);

	if ((dwStyle & WS_VISIBLE) && (dwStyle & CBRS_ALIGN_ANY) != 0)
	{
		// align the control bar
		wxRect rect;
		rect.CopyRect(&lpLayout->rect);

#ifndef STANDALONE		// The standalone version leaves HeightOfBar = 0
		{
			wxRect AvailableRect(rect);
			HeightOfBar = CalculateNewBarHeight(&AvailableRect);	// (corrupts AvailableRect)
		}
#endif

		CSize sizeAvail = rect.Size();	// maximum size available

		// get maximum requested size
		CSize size = CalcFixedLayout(lpLayout->bStretch, 
									(dwStyle & CBRS_ORIENT_HORZ) ? TRUE : FALSE);

		size.cx = min(size.cx, sizeAvail.cx);
		size.cy = HeightOfBar;

		lpLayout->sizeTotal.cy += size.cy;
		lpLayout->sizeTotal.cx = max(lpLayout->sizeTotal.cx, size.cx);
		rect.top = rect.bottom - size.cy;
		lpLayout->rect.bottom -= size.cy;

		rect.right = rect.left + size.cx;
		rect.bottom = rect.top + size.cy;

		// only resize the window if doing layout and not just rect query
		if (lpLayout->hDWP != NULL)
			AfxRepositionWindow(lpLayout, m_hWnd, &rect);
	}

	return 0;
#endif
// ---------------------------------------------------------------------
}
#endif


/********************************************************************************************

>	inline BOOL GetModifierKeyState(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Returns:	TRUE if the ColourBar push-mode modifier key ('Alt'ernative) is down

	Purpose:	Checks if the modifier key for the colour bar's push-the-strip
				mode is down. This is an inline macro to make it easy to change
				the key at later date. (Now uses KeyPress::IsAlternativePressed)

	Scope:		private (to ccolbar.cpp)

	SeeAlso:	KeyPress::IsAlternativePressed

********************************************************************************************/

inline BOOL GetModifierKeyState(void)
{
	return(KeyPress::IsAlternativePressed());
}



/********************************************************************************************

>	inline BOOL ShiftIsDown(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Returns:	TRUE if the 'Adjust' (shift) key is held is down at present

	Purpose:	Checks if SHIFT is held down - used to convert shift-left-clicks into
				"right clicks". (Now uses KeyPress::IsAdjustPressed() to determine this)
	
	Scope:		private (to ccolbar.cpp)

	SeeAlso:	KeyPress::IsAdjustPressed

********************************************************************************************/

inline BOOL ShiftIsDown(void)
{
	return(KeyPress::IsAdjustPressed());
}



/********************************************************************************************

>	void CColourBar::OnAnyButtonDown(const wxPoint &point, INT32 Modifier)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		point - co-ordinate of the click point
				Modifier - +1 if select click, -1 if adjust click
	Purpose:	Handle button clicks in the colour bar. Called by the LButton
				and RButton handlers to share the code somewhat

********************************************************************************************/

void CColourBar::OnAnyButtonDown(const wxPoint &point, INT32 Modifier)
{
	if (DragInfo.MouseCaptured)					// Ignore clicks during drags
		return;

	// Remove bubble help whenever the user clicks
//	ControlHelper::BubbleHelpDisable();

	if (m_pCurrentColourList == NULL)		// Ensure the current ColourList pointer is valid
		m_pCurrentColourList = ColourManager::GetColourList();

	if (m_pCurrentColourList == NULL)		// No DOCUMENT! No Colours! Exit immediately!
		return;

	wxRect RectMouseIsIn;
	INT32 MousePos = WhereIsMouse(point, &RectMouseIsIn);

	DragInfo.Adjust = (Modifier < 0);
	DragInfo.LastMousePos = point;				// Remember last point mouse was at
	DragInfo.DragItem = MousePos;				// and last item it was over


	if (MousePos < CLICKED_NOTHING)
	{
		BOOL ModifierKeyDown = GetModifierKeyState();

		if (ModifierKeyDown)
		{
			// If Right ALT down, allow 'push tool' dragging mode on colourstrip
			CaptureTheMouse();
			DragInfo.DragItem = CLICKED_SCROLLSTRIP;

			// AnchorOffset is now the colour cell being dragged
			DragInfo.AnchorOffset = (INT32) (LeftmostColour +
									((point.x - StripRect.x) / CellSize));
			return;
		}
		else
		{
#ifdef DISABLE_COLOUR_DRAGS
		 	CellClicked(MousePos, (Modifier < 0));
#else
			// Find the clicked colour, and start a drag of it
			IndexedColour *TheColour = NULL;
			ColourDragInformation *DragCol = NULL;
			// If the user has the preference set then include and hence show the document
			// colours
			if (ColourSGallery::ShowDocumentColours)
				TheColour = FindColourByIndex(MousePos);

			if (TheColour != NULL)
			{
				DragCol = new ColourDragInformation(TheColour, (Modifier < 0),
														Document::GetSelected());
			}
			else
			{
PORTNOTE("other","Removed ColourSGallery usage")
#if !defined(EXCLUDE_FROM_XARALX)
				// FIXED COLOURS
				// We didn't find the colour in the document colours section
				// so check any library sections that may be present
				SGDisplayLibColour *pLibColour = NULL;
				DocColour * pTheDocColour = FindLibColourByIndex(MousePos, &pLibColour);
				if (pTheDocColour && pLibColour)
				{
					// Start up a DocColour drag which should give us what we want
					String_256 Buffer;
					pLibColour->GetNameText(&Buffer);
					DragCol = new ColourDragInformation(pTheDocColour, (Modifier < 0),
														&Buffer, pLibColour->IsASpotColour());
				}
#endif
			}

			DragManagerOp::StartDrag(DragCol, this);
#endif
			return;
		}
	}

	switch(MousePos)
	{
		case CLICKED_NOCOLOURCELL:				// Set 'no colour'
#ifdef DISABLE_COLOUR_DRAGS
			CellClicked(MousePos, (Modifier < 0));
#else
			// Create an attribute of the correct type

			ColourDragInformation * DragCol;
			DragCol = new ColourDragInformation(NULL, (Modifier < 0),
												Document::GetSelected());
			DragManagerOp::StartDrag(DragCol, this);
#endif
			break;

		case CLICKED_INDICATORS:
#ifndef DISABLE_COLOUR_DRAGS
			{
				// Have clicked the line/fill colour indicator, so drag them if possible
				wxRect InnerRect(IndicatorRect);
				INT32 DeflateBy = (abs(IndicatorRect.height) * 2) / 5;
				if (DeflateBy < 4)
					DeflateBy = 4;	// Line indicator is min. of 2 pixels wide
				InnerRect.Inflate(-DeflateBy / 2, -DeflateBy / 2);

				// Resort to dragging 'no colour' if a better colour can't be found
				IndexedColour *IndexedColToDrag = NULL;

				DocColour *LineColour;
				DocColour *FillColour;
				DocColour *EndColour;
				ColourManager::GetCurrentLineAndFillColours(&LineColour, &FillColour, &EndColour);

				DocColour *DocColourToUse = NULL;

				if (InnerRect.Inside(point))
				{
					if (EndColour != NULL && point.y <= (InnerRect.y + (InnerRect.height / 2)))
						DocColourToUse = EndColour;
					else
						DocColourToUse = FillColour;
				}
				else
					DocColourToUse = LineColour;

				// If we found a doc colour to be dragged, find its IndexedColour parent (or if necessary,
				// create a new local colour to be dragged from the immediate-doccolour definition)
				if (DocColourToUse != NULL)
				{
					IndexedColToDrag = DocColourToUse->FindParentIndexedColour();

					// If the DocColour was an immediate colour (non-transparent, but
					// with no parent IndexedColour) then we'll need to create a local IndexedColour
					// to represent the colour to be dragged, or else it'll drag 'no colour'!
					// This may return NULL, but we're quite happy with that.
					if (IndexedColToDrag == NULL && !DocColourToUse->IsTransparent())
					{
						IndexedColToDrag = ColourManager::GenerateNewUnnamedColour(
															m_pCurrentColourList, DocColourToUse);
					}
				}

				ColourDragInformation *DragCol;
				DragCol = new ColourDragInformation(IndexedColToDrag, (Modifier < 0),
													Document::GetSelected());
				DragManagerOp::StartDrag(DragCol, this);
			}
#endif
			break;

		case CLICKED_LEFTSCROLL:
			ScrollTheStrip(-Modifier);			// Scroll once
			CaptureTheMouse();					// And start an autorepeat 'drag'
			SetATimer(TRUE);
			break;

		case CLICKED_RIGHTSCROLL:
			ScrollTheStrip(Modifier);			// Scroll once
			CaptureTheMouse();					// And start an autorepeat 'drag'
			SetATimer(TRUE);
			break;

		case CLICKED_SCROLLBARLEFT:				// PageRight once
			{
				UINT32 NumDisplayedColours = StripRect.width / CellSize;
				ScrollTheStrip((INT32) ((-Modifier) * NumDisplayedColours));
				CaptureTheMouse();				// And start an autorepeat 'drag'
				SetATimer(TRUE);
			}
			break;

		case CLICKED_SCROLLBARRIGHT:			// PageLeft once
			{
				UINT32 NumDisplayedColours = StripRect.width / CellSize;
				ScrollTheStrip((INT32) (Modifier * NumDisplayedColours));
				CaptureTheMouse();				// And start an autorepeat 'drag'
				SetATimer(TRUE);
			}
			break;

		case CLICKED_SCROLLBAR:
			CaptureTheMouse();					// Start drag (on MouseMoves, not Timers)

			// AnchorOffset is now the distance (pixels) of the drag anchor from
			// the center of the scroll sausage
			DragInfo.AnchorOffset = point.x -
									(RectMouseIsIn.x + (RectMouseIsIn.width / 2));
			break;

		case CLICKED_EDITBUTTON:
			{
				if (IndentedButton == CLICKED_NOTHING)		// Indent button while mouse down
				{
					TRACEUSER("Gerry", _T("Edit Clicked"));
					IndentedButton = CLICKED_EDITBUTTON;
					ForceRedrawOfRect(EditButtonRect);
					Update();							// Force immediate redraw
					CaptureMouse();							// And grab all mouse events
				}											// until the button is released

#if FALSE
/*
	// No longer needed - we want the editor to pop up in "local" mode.
				DocColour *LineColour;
				DocColour *FillColour;

				ERROR3IF(m_pCurrentColourList == NULL,
						_T("Unexpected NULL ColourList pointer in CColourBar::OnAnyButtonDown"));

				ColourManager::GetCurrentLineAndFillColours(&LineColour, &FillColour);

				// Edit the appropriate colour. If there is none, EditAColour just beeps
				// and returns without getting upset

				IndexedColour *ColToEdit = NULL;
				if (Modifier < 0)
				{
					if (LineColour != NULL)
						ColToEdit = LineColour->FindParentIndexedColour();
				}
				else
				{
					if (FillColour != NULL)
						ColToEdit = FillColour->FindParentIndexedColour();
				}

				EditAColour(m_pCurrentColourList, ColToEdit);
*/
#else
				EditAColour(NULL, NULL, (Modifier < 0));
#endif
			}
			break;


#if FALSE
/*
		case CLICKED_NEWBUTTON:
			{
				if (IndentedButton == CLICKED_NOTHING)		// Indent button while mouse down
				{
					IndentedButton = CLICKED_NEWBUTTON;
					ForceRedrawOfRect(NewButtonRect);
					UpdateWindow();							// Force immediate redraw
					SetCapture();							// And grab all mouse events
				}											// until the button is released

				ERROR3IF(m_pCurrentColourList == NULL,
						_T("Unexpected NULL ColourList pointer in CColourBar::OnAnyButtonDown"));

				IndexedColour *NewCol = NULL;

				// Create a new colour, in the current colour list, copied from the
				// current attribute/selection if possible.
				ColourList *CurrentDocColours = ColourManager::GetColourList();
				ERROR3IF(CurrentDocColours == NULL, _T("No colour list present?!"));

				if (CurrentDocColours != NULL)
				{
					NewCol = ColourManager::GenerateNewNamedColour(CurrentDocColours, NULL);

					if (NewCol != NULL)
					{
						ApplyColour(NewCol, (Modifier < 0));	// Apply col to selection
						EditAColour(m_pCurrentColourList, NewCol);	// And open editor
					}
				}
			}
			break;	
*/
#endif
	}
}



/********************************************************************************************

>	void CColourBar::OnLButtonDown(wxMouseEvent& event)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		nFlags - unused
				point - co-ordinate of the click point
	Purpose:	Handle left-button clicks in the colour bar
				Note that a shift-click is converted into the equivalent of a right-click
				by this function.
	SeeAlso:	CColourBar::OnAnyButtonDown

********************************************************************************************/

void CColourBar::OnLButtonDown(wxMouseEvent& event)
{
	TRACEUSER("Gerry", _T("CColourBar::OnLButtonDown"));
	OnAnyButtonDown(event.GetPosition(), (ShiftIsDown() ? -1 : +1));
}



/********************************************************************************************

>	void CColourBar::OnRButtonDown(wxMouseEvent& event)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		nFlags - unused
				point - co-ordinate of the click point
	Purpose:	Handle right-button clicks in the Colour Bar
	SeeAlso:	CColourBar::OnAnyButtonDown

********************************************************************************************/

void CColourBar::OnRButtonDown(wxMouseEvent& event)
{
	TRACEUSER("Gerry", _T("CColourBar::OnRButtonDown"));
	OnAnyButtonDown(event.GetPosition(), -1);
}



/********************************************************************************************

>	void CColourBar::OnAnyButtonDblClk(wxPoint point, INT32 Modifier)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		point - co-ordinate of the click point
				Modifier - should be +1 (Select) or -1 (Adjust)

	Purpose:	Handle double clicks in the colour bar. This brings up the colour
				picker on the given colour. This works on any colour cell, including
				the current colour indicators. Double clicks on scroll arrows are
				passed on to the normal single-click handler (otherwise every second
				click, when repeating fast, is lost)

********************************************************************************************/

void CColourBar::OnAnyButtonDblClk(wxPoint point, INT32 Modifier)
{
	if (m_pCurrentColourList == NULL)
		return;

	INT32 ColourCell = WhereIsMouse(point);

	if (ColourCell == CLICKED_INDICATORS)
	{
		// Have double-clicked the line/fill colour indicator, so edit them if possible
		wxRect InnerRect(IndicatorRect);
		INT32 DeflateBy = (abs(IndicatorRect.height) * 2) / 5;
		if (DeflateBy < 4)
			DeflateBy = 4;	// Line indicator is min. of 2 pixels wide
		InnerRect.Inflate(-DeflateBy / 2, -DeflateBy / 2);

//		IndexedColour *IndexedColToEdit = NULL;

		DocColour *LineColour;
		DocColour *FillColour;
		DocColour *EndColour;
		ColourManager::GetCurrentLineAndFillColours(&LineColour, &FillColour, &EndColour);

		if (InnerRect.Inside(point))
		{
			if (EndColour != NULL && point.y <= (InnerRect.y + (InnerRect.height / 2)))
				FillColour = EndColour;

			if (FillColour!= NULL)
			{
				// Find the fill colour. If it's unnamed, then we'll use NULL so that we
				// edit the local colour properly instead of doing the wrong thing
				IndexedColour *IxColour = FillColour->FindParentIndexedColour();
				if (IxColour != NULL && !IxColour->IsNamed())
					IxColour = NULL;

				EditAColour(m_pCurrentColourList, IxColour, FALSE);
			}
			else
				EditAColour(NULL, NULL, FALSE);
		}
		else
		{
			if (LineColour != NULL)
			{
				// Find the line colour. If it's unnamed, then we'll use NULL so that we
				// edit the local colour properly instead of doing the wrong thing
				IndexedColour *IxColour = LineColour->FindParentIndexedColour();
				if (IxColour != NULL && !IxColour->IsNamed())
					IxColour = NULL;

				EditAColour(m_pCurrentColourList, IxColour, TRUE);
			}
			else
				EditAColour(NULL, NULL, TRUE);
		}
	}
	else if (ColourCell == CLICKED_LEFTSCROLL || ColourCell == CLICKED_RIGHTSCROLL)
	{
		// As double-clicks are enabled, Windows weeds out every second click if the
		// user clicks repeatedly at high speed. Thus, we respond to a double-click
		// on the scroll arrows as if it were just a second ordinary click.
		OnAnyButtonDown(point, Modifier);
	}
}



/********************************************************************************************

>	void CColourBar::OnLButtonDblClk(wxMouseEvent& event)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		nFlags - unused
				point - co-ordinate of the click point
	Purpose:	Handle left-button double clicks in the colour bar

********************************************************************************************/

void CColourBar::OnLButtonDblClk(wxMouseEvent& event)
{
	OnAnyButtonDblClk(event.GetPosition(), (ShiftIsDown() ? -1 : +1));
}



/********************************************************************************************

>	void CColourBar::OnRButtonDblClk(wxMouseEvent& event)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		nFlags - unused
				point - co-ordinate of the click point
	Purpose:	Handle right-button double clicks in the Colour Bar

********************************************************************************************/

void CColourBar::OnRButtonDblClk(wxMouseEvent& event)
{
	OnAnyButtonDblClk(event.GetPosition(), -1);
}



/********************************************************************************************

>	void CColourBar::OnMouseMove(wxMouseEvent& event)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		nflags - Ignored
				point - The mouse position

	Purpose:	Processes all mouse moves over the colour bar. This currently involves
				providing bubble help and updating during drags of the scroll sausage.
				Other 'drag' updates (button autorepeats) are handled by OnTimer()

	SeeAlso:	CControlBar::OnTimer

********************************************************************************************/

void CColourBar::OnMouseMove(wxMouseEvent& event)
{
//	TRACEUSER("Gerry", _T("CColourBar::OnMouseMove"));

	if (m_pCurrentColourList == NULL)		// Ensure the current ColourList pointer is valid
		m_pCurrentColourList = ColourManager::GetColourList();

	if (m_pCurrentColourList == NULL)		// No DOCUMENT! No Colours! Exit immediately!
		return;

	wxPoint point = event.GetPosition();
	DragInfo.LastMousePos = point;	// Remember new mouse position for OnTimer code

	// Check if the push-mode modifier key is down, and if so, swap to the push cursor
	BOOL ModifierKeyDown = GetModifierKeyState();

	if (StripRect.Inside(point) && ModifierKeyDown)
	{
		// Set the cursor to the push hand shape
		if (!PushCursor)
			PushCursor = new Cursor(_R(IDC_PUSHTOOLCURSOR));

		if (PushCursor)
			PushCursor->SetActive();
	}
	else
	{
		Cursor::Arrow->SetActive();
		if (PushCursor)
		{
			delete PushCursor;
			PushCursor = NULL;
		}
	}


	if (DragInfo.MouseCaptured)
	{
		// We are dragging something
		if (DragInfo.DragItem == CLICKED_SCROLLBAR && ScrollHeight != 0)
		{
			// We are dragging the scrollbar sausage
			wxRect SausageRect;
			if (CalculateSausageRect(&SausageRect))
			{
				// We want to scroll the AnchorOffset point under the mouse posn.
				INT32 DistToScroll = point.x -
									((SausageRect.x + (SausageRect.width / 2)) +
									DragInfo.AnchorOffset);

				ENSURE(ScrollBarRect.width > 0,
						"Serious problem with the ColourBar's ScrollBarRect");

				INT32 ColoursToScroll = ((INT32)TotalNumColours * DistToScroll) / ScrollBarRect.width;

				ScrollTheStrip(ColoursToScroll);
			}
		}
		else if (DragInfo.DragItem == CLICKED_SCROLLSTRIP)
		{
			ENSURE(StripRect.width > 0,
					"Serious problem with the ColourBar's StripRect");

			INT32 NewAnchorCellIndex = (INT32) ((INT32)LeftmostColour +
									((INT32)(point.x - StripRect.x) / (INT32)CellSize));
			ScrollTheStrip((INT32) (DragInfo.AnchorOffset - NewAnchorCellIndex));
		}
	}
	else

	{
		// Not dragging, so do Bubble & StatusBar Help on the thing under the mouse,
		// but only if the colour bar is active (we have a colour display list)
		if (ColourManager::GetColourList() != NULL)
		{
			String_256 HelpText(_T(""));

			if (GetStatusLineText(&HelpText))
			{
				StatusLine *pStatusLine = GetApplication()->GetpStatusLine();
				if (pStatusLine)
					pStatusLine->UpdateText(&HelpText, STATUSLINE_SELDESC_COLBAR);

//				TRACEUSER("Gerry", _T("CColourBar updating status to '%s'"), (LPCTSTR)HelpText);
			}

			INT32 MousePos = WhereIsMouse(point);

			// Do bubble help for the colour cells and assorted icons
			if (MousePos == CLICKED_NOTHING)
			{
				// Remove bubble help, as they're over no man's land
				// This should no longer completely disable bubble help over our window,
				// but merely turn off the bubble while the pointer is not over anything
				// of any interest.
//				ControlHelper::BubbleHelpDisable();
			}
			else
			{
PORTNOTE("other","Removed ControlHelper bubblehelp usage")
#if !defined(EXCLUDE_FROM_XARALX)
				ControlHelper::DoBubbleHelpOn(m_hWnd, MousePos,
												HelpCallbackHandler, this);
#endif
			}
		}
//		else
//			ControlHelper::BubbleHelpDisable();	// No colourstrip, so no help
	}

	event.Skip();
}



/*********************************************************************************************

>	void CColourBar::OnAnyButtonUp(const wxPoint &point)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Inputs:		point - the point where the mouse was released
	Outputs:	-
	Returns:	-
	Purpose:	Processes any button-released event. This stops auto-repeat and
				drags, and releases any claims (timers, drags) we might have
	Errors:		-

**********************************************************************************************/ 

void CColourBar::OnAnyButtonUp(const wxPoint &point)
{
	DragInfo.LastMousePos = point;
	ReleaseAllClaims();

	if (IndentedButton == CLICKED_EDITBUTTON)
		ForceRedrawOfRect(EditButtonRect);
#if FALSE
	else if (IndentedButton == CLICKED_NEWBUTTON)
		ForceRedrawOfRect(NewButtonRect);
#endif

	if (IndentedButton != CLICKED_NOTHING)
	{
		IndentedButton = CLICKED_NOTHING;		// Remember that nothing is indented
	}
}



/*********************************************************************************************
>	afx_msg void CColourBar::OnLButtonUp(wxMouseEvent& event)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Inputs:		nFlags - A bit-field describing which buttons and keys were down
				when the message was generated
				point - the position of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Processes a left-button-released message
	Errors:		-

**********************************************************************************************/ 

void CColourBar::OnLButtonUp(wxMouseEvent& event)
{
	TRACEUSER("Gerry", _T("CColourBar::OnLButtonUp"));
	OnAnyButtonUp(event.GetPosition());
}



/*********************************************************************************************

>	afx_msg void CColourBar::OnRButtonUp(wxMouseEvent& event)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Inputs:		nFlags - A bit-field describing which buttons and keys were down
				when the message was generated
				point - the position of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Processes a right-button-released message
	Errors:		-

**********************************************************************************************/ 

void CColourBar::OnRButtonUp(wxMouseEvent& event)
{
	TRACEUSER("Gerry", _T("CColourBar::OnLButtonUp"));
	OnAnyButtonUp(event.GetPosition());
}



/*********************************************************************************************

>	afx_msg void CColourBar::OnTimer(wxTimerEvent& event)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Inputs:		nIDEvent - The ID number of the elapsed timer (not used)
	Outputs:	-
	Returns:	-
	Purpose:	Responds to the triggering of the timer which we set when a click
				or drag first occurred, in order to check for auto-repeat of buttons.
				This currently processes the scroll left/right and page left/right areas
				Dragging the scroll sausage is handled in OnMouseMove

	SeeAlso:	CColourbar::OnMouseMove
				
**********************************************************************************************/ 

void CColourBar::OnTimer(wxTimerEvent& event)
{
	// We aren't dragging, or this is not the timer we set
	if (!DragInfo.MouseCaptured)
		return;

	SetATimer(FALSE);	// Replace the repeat-delay timer with a repeat-speed timer

	INT32 NewMouseItem = WhereIsMouse(DragInfo.LastMousePos);

	if (NewMouseItem == DragInfo.DragItem)
	{
		switch(DragInfo.DragItem)
		{
			case CLICKED_LEFTSCROLL:
				ScrollTheStrip((DragInfo.Adjust) ? +1 : -1);
				break;

			case CLICKED_RIGHTSCROLL:
				ScrollTheStrip((DragInfo.Adjust) ? -1 : +1);
				break;

			case CLICKED_SCROLLBARLEFT:
				{
					UINT32 NumDisplayedColours = StripRect.width / CellSize;
					ScrollTheStrip((INT32) (((DragInfo.Adjust) ? 1 : -1) * NumDisplayedColours));
				}
				break;

			case CLICKED_SCROLLBARRIGHT:
				{
					UINT32 NumDisplayedColours = StripRect.width / CellSize;
					ScrollTheStrip((INT32) (((DragInfo.Adjust) ? -1 : 1) * NumDisplayedColours));
				}
				break;
		}
	}
}



/*********************************************************************************************
>	afx_msg void CColourBar::OnCancelMode(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Responds to a WM_CANCELMODE message sent by Windows, whenever the active
				application loses the focus via some method which doesn;t directly involve
				the user, eg. because a dialogue box from some background app has popped up.
				The main purpose of the routine is to make sure that if the mouse has been
				captured it is released, and if a timer has been allocated, it is freed.

**********************************************************************************************/ 

PORTNOTE("other","Removed CancelMode")
#if !defined(EXCLUDE_FROM_XARALX)

void CColourBar::OnCancelMode(void)
{
	CControlBar::OnCancelMode();
	ReleaseAllClaims();
}
#endif


/********************************************************************************************

>	void CColourBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Inputs:		pTarget - 
				bDisableIfNoHandler - 
	Outputs:	-
	Returns:	-

	Purpose:	I'm not really sure... Some sort of mystical MFC thingummy.

	Notes:		I'm not entirely sure how much of this is necessary, but it doesn't seem
				to do any harm, and things didn't work properly until I put it in...

				The ColourBar works exclusively on the SELECTED Doc. Change with care

********************************************************************************************/

PORTNOTE("other","Removed OnUpdateCmdUI")
#if !defined(EXCLUDE_FROM_XARALX)
void CColourBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler)
{
	CCmdUI state;
	state.m_pOther = this;
	state.m_nIndexMax = (UINT32)m_nCount;

	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	  state.m_nIndex++)
	{
		state.m_nID = 0;
		state.DoUpdate(pTarget, bDisableIfNoHandler);
	}

	// update the dialog controls added to the status bar (of which there are none)
	UpdateDialogControls(pTarget, bDisableIfNoHandler);
}
#endif


/********************************************************************************************

>	static BOOL CColourBar::GetStatusLineText(String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/1/95
	
	Inputs:		-
	Outputs:	Result - If the mouse pointer is currently over the colour line, this is
				returned containing appropriate help text, otherwise this is left untouched.
	Returns:	TRUE if the Result string was updated with valid help text

	Purpose:	To retrieve help text for use by the status line system

********************************************************************************************/

BOOL CColourBar::GetStatusLineText(String_256 *Result)
{
	if (TheColourBar == NULL)		// There is no colour bar!
		return(FALSE);

	if (TheColourBar->m_pCurrentColourList == NULL)	// No Colours! Exit immediately!
		return(FALSE);

//	ASSERT_VALID(TheColourBar);
	ERROR3IF(Result == NULL, _T("CColourBar::GetStatusLineText - NULL Result parameter is bad"));

	wxPoint ScreenPos = ::wxGetMousePosition();

	if (::wxChildWindowFromPoint(ScreenPos, false, -1) != TheColourBar)
		return(FALSE);

	wxPoint ClientPos = TheColourBar->ScreenToClient(ScreenPos);	// make relative to window

	static String_64 HelpStringStore;
//	StringBase *HelpString = NULL;

	INT32 Item = TheColourBar->WhereIsMouse(ClientPos);
	if (Item >= CLICKED_NOTHING)
	{
		switch(Item)
		{
			case CLICKED_SCROLLBAR:
			case CLICKED_SCROLLBARLEFT:
			case CLICKED_SCROLLBARRIGHT:
			case CLICKED_LEFTSCROLL:
			case CLICKED_RIGHTSCROLL:
				Result->MakeMsg(_R(IDS_COLBAR_SSCROLL));
				break;

			case CLICKED_INDICATORS:
				Result->MakeMsg(_R(IDS_COLBAR_HINDICATOR));
				break;

			case CLICKED_NOCOLOURCELL:
				Result->MakeMsg(_R(IDS_COLBAR_SNOCOLOUR));
				break;

			case CLICKED_EDITBUTTON:
				{
					// "Click to edit the current colour"
					Result->MakeMsg(_R(IDS_COLBAR_SEDITBUTTON));

					// If there is a selection, then chnage to using:
					// "Click to edit colour of ellipse"
					SelRange *Selection = GetApplication()->FindSelection();
					if (Selection != NULL)
					{
						if (Selection->Count() > 0)
						{
							String_256 Desc = Selection->Describe(MENU);
							Result->MakeMsg(_R(IDS_COLBAR_SEDITBUTTON2), (TCHAR *)Desc);
						}
					}					
				}
				break;
			
#if FALSE
			case CLICKED_NEWBUTTON:
				Result->MakeMsg(_R(IDS_COLBAR_SNEWBUTTON));
				break;
#endif

			case CLICKED_NOTHING:		// No useful help - return nothing
				return(FALSE);
		}
	}
	else
	{
		if (AttrFillGeometry::SelectionCount > 0)
			Result->Load(_R(IDS_COLBAR_SSTRIPGRAD));	// Set grad fill handle colours
		else
			Result->Load(_R(IDS_COLBAR_SSTRIPFLAT));	// Set normal flat fill colours
	}

	return(TRUE);
}






CC_IMPLEMENT_DYNAMIC(ColourBarMsgHandler, MessageHandler)

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	ColourBarMsgHandler::ColourBarMsgHandler()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/05/94
	Purpose:	Constructs a ColourBarMsgHandler, an object which, surprisingly,
				handles messages for its parent CColourBar.
	Errors:		-
	SeeAlso:	MessageHandler

********************************************************************************************/

ColourBarMsgHandler::ColourBarMsgHandler()
  : MessageHandler(CC_RUNTIME_CLASS(MessageHandler), TRUE)
{
}



/********************************************************************************************

>	virtual MsgResult ColourBarMsgHandler::Message(Msg* Message)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/05/94
	Inputs:		Message: The message
	Outputs:	-
	Returns:	-
	Purpose:	Process messages sent to the Colour Bar
	Errors:		-
	SeeAlso:	MessageHandler

********************************************************************************************/

MsgResult ColourBarMsgHandler::Message(Msg* Message)
{
	if (MESSAGE_IS_A(Message, ColourChangingMsg))
	{
		ColourChangingMsg *Msg = (ColourChangingMsg *) Message;
		switch ( Msg->State )
		{
			case ColourChangingMsg::LISTDESELECTED:
			case ColourChangingMsg::SELVIEWCONTEXTCHANGE:
				CColourBar::PaletteHasChanged(NULL);
				break;

			case ColourChangingMsg::LISTPAGED:
			case ColourChangingMsg::LISTUPDATED:
			case ColourChangingMsg::COLOURUPDATED:
				CColourBar::PaletteHasChanged(Msg->NewColourList);
				break;

			default:
				break;
		}
	}
	// The new CommonAttrsChanged msg replaces these two messages. It handles 
	// a change in the Current Attribute group associated with a tool as well !
	// what a bargain !
	/*
	else if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		SelChangingMsg *Msg = (SelChangingMsg *) Message;
		switch ( Msg->State )
		{
			case SelChangingMsg::SelectionState::COLOURATTCHANGED:
			case SelChangingMsg::SelectionState::SELECTIONCHANGED:
			case SelChangingMsg::SelectionState::NODECHANGED:
				CColourBar::SelectionHasChanged();
				break;
		}
	}
	else if (MESSAGE_IS_A(Message, CurrentAttrChangedMsg))
	{
		CColourBar::SelectionHasChanged();
	}
	*/
	else if (MESSAGE_IS_A(Message, CommonAttrsChangedMsg))
	{
		CColourBar::SelectionHasChanged();
	}
	else if (MESSAGE_IS_A(Message,DragMessage))
	{
		DragMessage *Msg = (DragMessage *) Message;
		
	 	if (Msg->State == DragMessage::DRAGSTARTED)
		{
			if (Msg->pInfo->IsKindOf(CC_RUNTIME_CLASS(ColourDragInformation)))
			{
				ColourDragInformation *CDI = (ColourDragInformation *)Msg->pInfo;

				if (CDI->IsLibraryColour() || CDI->GetParentDoc() == Document::GetSelected())
				{
					// Create drag targets to recieve colour drags. We don't really care if this fails,
					// and the drag system will clean up any mess afterwards.
					EditButtonDragTarget *ButtonTarget;
					ButtonTarget = new EditButtonDragTarget(CColourBar::TheColourBar,
														 &CColourBar::TheColourBar->EditButtonRect);

					// Check that the colour being dragged is in the current colour list
					IndexedColour *Col = ((ColourDragInformation *) Msg->pInfo)->GetInitiallyDraggedColour();
					if (Col != NULL && CColourBar::TheColourBar->m_pCurrentColourList->FindPosition(Col) < 0)
						Col = NULL;

					// And if it is in the list, then allow it to be rearranged in the list
					if (Col != NULL)
					{
						ColourLineDragTarget *LineTarget;
						LineTarget = new ColourLineDragTarget(CColourBar::TheColourBar,
															 &CColourBar::TheColourBar->StripRect);
					}
				}
			}
		}
	}
	else if (MESSAGE_IS_A(Message, DocViewMsg))
	{
		DocViewMsg *Msg = (DocViewMsg *) Message;

		if (Msg->State == DocViewMsg::SELCHANGED)
		{
			// Selected DocView is changing - redraw to use the new DocView's colour context
			BOOL DoRedraw = TRUE;
			if (Msg->pOldDocView != NULL && Msg->pNewDocView != NULL)
			{
				// If we know the old & new views, then see if they have the same colour
				// context attached - if they do, there's no need to redraw. This eliminates
				// flicker when swapping normal views (the most common view-swap action)
				// We only check the RGB context because we assume the screen is always RGB
				ColourContext *OldCC = Msg->pOldDocView->GetColourContext(COLOURMODEL_RGBT, TRUE);
				ColourContext *NewCC = Msg->pNewDocView->GetColourContext(COLOURMODEL_RGBT, TRUE);

				if (OldCC == NewCC)
					DoRedraw = FALSE;
			}

			if (DoRedraw)
				CColourBar::PaletteHasChanged(NULL);
		}
	}

	return OK; 
}


