// $Id: dragtrgt.cpp 662 2006-03-14 21:31:49Z alex $
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

// dragtrgt.cpp - Implementations of global dragging system DragTarget classes

/*
*/


#include "camtypes.h"

#include "dragmgr.h"
//#include "resource.h"
//#include "mario.h"

CC_IMPLEMENT_DYNAMIC(DragTarget, ListItem)
CC_IMPLEMENT_DYNAMIC(KernelDragTarget, DragTarget)
CC_IMPLEMENT_DYNAMIC(OilDragTarget, DragTarget)


#define new CAM_DEBUG_NEW




/********************************************************************************************

>	DragTarget::DragTarget()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95

	Purpose: To ERROR3 if you are dumb enough to call the default constructor

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/


DragTarget::DragTarget()
{
	IWantAllEvents = FALSE;		// By default, targets only want events while the pointer
								// is over their target area
}



/********************************************************************************************

>	DragTarget::~DragTarget()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95

	Purpose:	Destructor
				Ensures that this drag target is no longer registered with any drag manager.
				i.e. if you want to remove a target during a drag, just destruct it.

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

DragTarget::~DragTarget()
{
//	PORTNOTE("other", "Removed DragManagerOp usage")
//#ifndef EXCLUDE_FROM_XARALX
	// Just ensure that this target is not still registered with the manager
	DragManagerOp *Mgr = DragManagerOp::GetCurrentManager();
	if (Mgr != NULL)
		Mgr->DeregisterTarget(this);
//#endif
}



/********************************************************************************************

>	virtual BOOL DragTarget::IsAKernelObject(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95

	Returns:	TRUE (This is a kernel object, not a winoil one)

	Purpose:	Virtual method to determine if a given DragTarget is of the Kernel or
				Winoil 'gender'. 
				
	Notes:		Don't override this! Derive from KernelDragTarget or OilDragTarget

				This method gives an ERROR3 - you should not derive from/use DragTarget!

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

BOOL DragTarget::IsAKernelObject(void)
{
	ERROR3("Somebody is using 'abstract' base class DragTarget!!!");
	return(TRUE);
}



/********************************************************************************************

>	BOOL DragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
										CPoint *pMousePos, KeyPress* pKeyPress)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Inputs:		Event - Specifies what type of event has occurred
				pDragInfo - Specifies a descriptor describing the currently active drag
				pMousePos - Specifies the last known mouse coordinates, in terms
				of client area coordinates within the drag target window (or screen
				coordinates if no window was specified on registering)
				pKeyPress - NULL, or a pointer to the key pressed (for KEYPRESS events)

	Purpose:	Winoil-style ProcessEvent interface
				The OilDragTarget overrides this method to handle winoil events. Non-
				winoil targets do not override this method, and thus give errors if called
				incorrectly.

	SeeAlso:	keyword DragEventType

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

BOOL DragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
									wxPoint *pMousePos, KeyPress* pKeyPress)
{
	ERROR3("Illegal call to DragTarget::ProcessEvent - I'm not a WINOIL Target!");
	return(FALSE);		// We do not claim the event
}



/********************************************************************************************

>	BOOL DragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
										OilCoord *pMousePos, KeyPress* pKeyPress)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Inputs:		Event - Specifies what type of event has occurred
				pDragInfo - Specifies a descriptor describing the currently active drag
				pMousePos - Specifies the last known mouse coordinates, in terms
				of OilCoords (Millipoint offsets within the drag target window/gadget)
				pKeyPress - NULL, or a pointer to the key pressed (for KEYPRESS events)

	Purpose:	Kernel-style ProcessEvent interface
				The KernelDragTarget overrides this method to handle kernel events. Non-
				kernel targets do not override this method, and thus give errors if called
				incorrectly.

	SeeAlso:	keyword DragEventType

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

BOOL DragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
					OilCoord *pMousePos, KeyPress* pKeyPress)
{
	ERROR3("Illegal call to DragTarget::ProcessEvent - I'm not a KERNEL Target!");
	return(FALSE);		// We do not claim the event
}



/********************************************************************************************

>	void DragTarget::GetTargetAreaInfo(HWND *ReturnedHWnd, CRect *ReturnedTargetRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/95

	Outputs:	ReturnedHWnd - Filled in with the window
				ReturnedTargetRect - Filled in with the target area rectangle in that window

	Purpose:	Winoil method for determining target-area information

	Notes:		Generates an ERROR3 - must not be called (This method is overridden by
				OilDragTargets to supply the requested information)

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

void DragTarget::GetTargetAreaInfo(wxWindow **ReturnedHWnd, wxRect *ReturnedTargetRect)
{
	*ReturnedHWnd = NULL;
	*ReturnedTargetRect = wxRect(0,0,0,0);
	ERROR3("DragTarget::GetTargetAreaInfo (WINOIL) -"
			" this is an illegal call on a KERNEL target!");
}



/********************************************************************************************

>	void DragTarget::GetTargetAreaInfo(DialogOp **ReturnedDialog, CGadgetID *ReturnedGadget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/95

	Outputs:	ReturnedDialog - Filled in with a pointer to the dialogop of the target area
				ReturnedGadget - Filled in with the gadget containing the target area

	Purpose:	Winoil method for determining target-area information

	Notes:		Generates an ERROR3 - must not be called (This method is overridden by
				KernelDragTargets to supply the requested information)

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

void DragTarget::GetTargetAreaInfo(DialogOp **ReturnedDialog, CGadgetID *ReturnedGadget)
{
	*ReturnedDialog = NULL;
	*ReturnedGadget = 0;
	ERROR3("DragTarget::GetTargetAreaInfo (KERNEL) -"
			" this is an illegal call on a WINOIL target!");
}


/********************************************************************************************

>	void DragTarget::GetCursorID()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/95
	Purpose:	Base Method to set cursor over this target


********************************************************************************************/

UINT32 DragTarget::GetCursorID()
{
	 return _R(IDC_CANDROP);
}



/********************************************************************************************

>	virtual BOOL DragTarget::GetStatusLineText(String_256 * TheText)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/95
	Returns:	a cursor ID to set during this drag
	Purpose:	-
   
********************************************************************************************/

BOOL DragTarget::GetStatusLineText(String_256 * TheText)
{
	ERROR2IF(TheText==NULL,FALSE,"NULL string in GetStatusLineText()");
 	* TheText = String_256(_R(IDS_DROPSTATUSLINETEXT));
	return TRUE;
}





/********************************************************************************************

>	KernelDragTarget::KernelDragTarget(DialogOp *TheDialog, CGadgetID TheGadget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95

	Inputs:		TheDialog - NULL (see Notes), or the kernel DialogOp providing the drag target
				TheGadget - NULL (see Notes), or the ID of the gadget within that dialogue
							which is the drag target area.

	Purpose:	Constructs a KernelDragTarget, AND REGISTERS it with the current DragManagerOp
				This is used as a drag target for entities existing in the kernel (usually
				DialogOps).

	Notes:		If both parameters are NULL, then this DragTarget is not associated with
				any particular area of the screen, and will recieve all drag events (that
				are not claimed by another such global event processor). It is not
				expected that kernel drag processors will be used in this way

				If TheGadget is NULL, then the entire dialogue client area will be considered
				the target area. Otherwise, only that gadget is used as the target.

	SeeAlso:	OilDragTarget::OilDragTarget; DragManagerOp

	Errors:		In debug builds, an ERROR3 will occur if there is no active drag
				manager (no drag occurring), and the DragTarget will probably appear
				as a memory leak (as the Drag manager is normally responsible for deleting
				the drag targets registered with it)

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

KernelDragTarget::KernelDragTarget(DialogOp *TheDialog, CGadgetID TheGadget)
{
	TargetDialog = TheDialog;
	TargetGadget = TheGadget;

//	PORTNOTE("other", "Removed DragManagerOp usage")
//#ifndef EXCLUDE_FROM_XARALX
	// Register this drag target automatically with the system
	DragManagerOp *DragMan = DragManagerOp::GetCurrentManager();
	if (DragMan != NULL)
		DragMan->RegisterTarget(this, (TheDialog == NULL));
#ifdef _DEBUG
	else
		ERROR3("There is no active drag (when attempting to create a DragTarget)");
#endif
//#endif
}



/********************************************************************************************

>	virtual BOOL KernelDragTarget::IsAKernelObject(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95

	Returns:	TRUE (This is a kernel object, not a winoil one)

	Purpose:	Virtual method to determine if a given DragTarget is of the Kernel or
				Winoil 'gender'.
				
	Notes:		Don't override this! Derive from KernelDragTarget or OilDragTarget

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

BOOL KernelDragTarget::IsAKernelObject(void)
{
	return(TRUE);
}



/********************************************************************************************

>	BOOL KernelDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
										OilCoord *pMousePos, KeyPress* pKeyPress)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Inputs:		Event - Specifies what type of event has occurred
				pDragInfo - Specifies a descriptor describing the currently active drag
				pMousePos - Specifies the last known mouse coordinates, in terms
				of OilCoords (Millipoint offsets within the drag target window/gadget)
				pKeyPress - NULL, or a pointer to the key pressed (for KEYPRESS events)

	Purpose:	To process a drag-specific event. By default this method does nothing,
				just returning FALSE so that the event is left unclaimed, and is thus
				passed on to other interested drag targets.

	Notes:		Each derived DragTarget class should override this method to handle
				events in a manner appropriate for the target.

				BE VERY CAREFUL to only override the kernel parts of this interface!

	SeeAlso:	keyword DragEventType

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

BOOL KernelDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
									OilCoord *pMousePos, KeyPress* pKeyPress)
{
	return(FALSE);		// We do not claim the event
}



/********************************************************************************************

>	void KernelDragTarget::GetTargetAreaInfo(DialogOp **ReturnedDialog,
												CGadgetID *ReturnedGadget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/95

	Outputs:	ReturnedDialog - Filled in with a pointer to the dialogop of the target area
				ReturnedGadget - Filled in with the gadget containing the target area

	Purpose:	Kernel method for determining target-area information

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

void KernelDragTarget::GetTargetAreaInfo(DialogOp **ReturnedDialog, CGadgetID *ReturnedGadget)
{
	*ReturnedDialog = TargetDialog;
	*ReturnedGadget = TargetGadget;
}



















/********************************************************************************************

>	OilDragTarget::OilDragTarget(HWND TheWindow, CRect *ClientArea)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95

	Inputs:		TheWindow - the winoil hWnd of the window to be used as the drag target
				area. This may be NULL (see Notes).
				ClientArea - NULL (see Notes), or the window client-coordinates of a
				rectangle within TheWindow of the target area.

	Purpose:	Constructs a OilDragTarget AND REGISTERS it with the current DragManagerOp
				This is used as a drag target for entities existing in the winoil (usually
				windows/controls).

	Notes:		If both parameters are NULL, then this DragTarget is not associated with
				any particular area of the screen, and will recieve all drag events (that
				are not claimed by another such global event processor).

				If ClientArea is NULL, then the entire window client area will be considered
				the target area. Otherwise, only that specified region within the window
				client area is used as the target.

	Errors:		In debug builds, an ERROR3 will occur if there is no active drag
				manager (no drag occurring), and the DragTarget will probably appear
				as a memory leak (as the Drag manager is normally responsible for deleting
				the drag targets registered with it)

	SeeAlso:	OilDragTarget::OilDragTarget; DragManagerOp

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

OilDragTarget::OilDragTarget(wxWindow * TheWindow, wxRect *ClientArea)
{
	TargetHWnd = TheWindow;

	if (ClientArea == NULL)
	{
		// Get whole window rectangle in client coords
		//TargetClientArea = CRect(0, 0, 32000, 32000);	// !!!! **** ToDo
		TargetClientArea = TheWindow->GetClientSize();
	}
	else
		TargetClientArea = *ClientArea;

//	PORTNOTE("other", "Removed DragManagerOp usage")
//#ifndef EXCLUDE_FROM_XARALX
	// Register this drag target automatically with the system
	DragManagerOp *DragMan = DragManagerOp::GetCurrentManager();
	if (DragMan != NULL)
		DragMan->RegisterTarget(this, (TheWindow == NULL));
#ifdef _DEBUG
	else
		ERROR3("There is no active drag (when attempting to create a DragTarget)");
#endif
//#endif
}



/********************************************************************************************

>	virtual BOOL OilDragTarget::IsAKernelObject(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95

	Returns:	FALSE (This is a winoil object, not a kernel one)

	Purpose:	Virtual method to determine if a given DragTarget is of the Kernel or
				Winoil 'gender'.

	Notes:		Don't override this! Derive from KernelDragTarget or OilDragTarget

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

BOOL OilDragTarget::IsAKernelObject(void)
{
	return(FALSE);
}



/********************************************************************************************

>	BOOL OilDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
										CPoint *pMousePos, KeyPress* pKeyPress)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Inputs:		Event - Specifies what type of event has occurred
				pDragInfo - Specifies a descriptor describing the currently active drag
				pMousePos - Specifies the last known mouse coordinates, in terms
				of client area coordinates within the drag target window (or screen
				coordinates if no window was specified on registering)
				pKeyPress - NULL, or a pointer to the key pressed (for KEYPRESS events)

	Purpose:	To process a drag-specific event. By default this method does nothing,
				just returning FALSE so that the event is left unclaimed, and is thus
				passed on to other interested drag targets.

	Notes:		Each derived DragTarget class should override this method to handle
				events in a manner appropriate for the target.

				BE VERY CAREFUL to only override the winoil parts of this interface!

	SeeAlso:	keyword DragEventType

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

BOOL OilDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
									wxPoint *pMousePos, KeyPress* pKeyPress)
{
	return(FALSE);		// We do not claim the event
}



/********************************************************************************************

>	void OilDragTarget::GetTargetAreaInfo(HWND *ReturnedHWnd,
												CRect *ReturnedTargetRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/95

	Outputs:	ReturnedHWnd - Filled in with the window
				ReturnedTargetRect - Filled in with the target area rectangle in that window

	Purpose:	Winoil method for determining target-area information

	Documentation:	Docs\Specs\DragMan.doc

********************************************************************************************/

void OilDragTarget::GetTargetAreaInfo(wxWindow **ReturnedHWnd, wxRect *ReturnedTargetRect)
{
	*ReturnedHWnd = TargetHWnd;
	*ReturnedTargetRect = TargetClientArea;
}


