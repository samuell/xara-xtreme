// $Id: ctrlhelp.h 662 2006-03-14 21:31:49Z alex $
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

// Code to modify the behaviour of controls on Camelot bars so that they respond
// to help messages and can be dragged about etc.

#ifndef INC_CTRLHELP
#define INC_CTRLHELP

#include "monotime.h"

class OpDescriptor;
class ControlTable;
class BarTable;
class BubbleHelpWnd;   

#define COMMIT 2

/********************************************************************************************

>	class ControlHelpInfo

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/05/94
	Purpose:	Used to specify details of a control that is to be added to the list of
				sub-classed controls. The fields are:
				MonoOn
				pOpDesc - pointer to the OpDescriptor for this control.
						  If this field is non-NULL, then all the ID fields are ignored.
						  Id this field is NULL, then the ID fields are used to define the
						  bubble help text and status bar text to be displayed for this
						  control.
				BubbleID - ID of the string resource to be used for bubble help.
				StatusID - ID of the string resource to be used for status bar text.
				ModuleID - defines where to find the above resources.
				Parent - internal use; any value you put in here will be ignored.
				MonoOff
	SeeAlso:	ControlHelper::NotifyControlCreated

********************************************************************************************/

class ControlHelpInfo
{
public:
	OpDescriptor* pOpDesc;
	UINT32 BubbleID;
	UINT32 StatusID;
	UINT32 ModuleID;
	HWND Parent;
};

/********************************************************************************************

< BubbleHelpCallback

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Comment:	This is a function that is called by the bubble help system when providing
				'ad-hoc' bubble help on a window, i.e. not on 'real' controls, but on
				GUI elements which may be just rendered, e.g. the colour bar uses this
				facility.
				You pass a function of this type into ControlHelper::DoBubbleHelpOn(), and
				this function is called back when the bubble help needs to be displayed.
				You should return a pointer to the bubble help text to display.

				MonoOn
				typedef TCHAR *(*BubbleHelpCallback)(HWND, UINT32, void*);
				MonoOff

				The parameters provide context information for the callback as there is
				usually a delay between calling DoBubbleHelpOn(), and the callback being
				called (because there is a short time delay before bubble help appears).

	Inputs:		HWND - the handle of the window that bubble help is being provided on.
				UINT32 - the 'psuedo-ID' you passed in to DoBubbleHelpOn() which indicates
					   which GUI element in the window that bubble help is being provided
					   for.  This number is determined by you - it means nothing to the
					   bubble help system, apart from when it does an equality check to
					   see if the user has moved onto another GUI element in your window.
				void* - an arbitrary piece of data associated with you system which may
						provide more assistance in working out the context of the call.
						This data is the data you pass in to DoBubbleHelpOn.

	SeeAlso:	ControlHelper::DoBubbleHelpOn

********************************************************************************************/


/********************************************************************************************

>	class ControlHelper : public SimpleCCObject

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/07/94
	Purpose:	Provide a system for subclassing controls on Camelot bars which offers
				various facilities such as bubble help/status bar text and commit handling.
				To use this system you use the Notifyxxx() functions to keep the class up
				to date with which bars/controls exist.  This module will subclass these
				controls and do some preprocessing of Windows messages in order to do
				bubble help/commit handling etc.
				You can 'fake' bubble help on a window that doesn't really have controls
				in it, by using the 'ad-hoc' bubble help function, DoBubbleHelpOn().
	SeeAlso:	ControlHelpInfo;
				ControlHelper::NotifyBarCreated; ControlHelper::NotifyBarDeleted;
				ControlHelper::NotifyBarChanged;
				ControlHelper::NotifyControlCreated; ControlHelper::NotifyControlDeleted
				ControlHelper::DoBubbleHelpOn

********************************************************************************************/

class ControlHelper : public SimpleCCObject
{
public:
	// Interface for use by the routines that manage bars + child controls.
	static BOOL Init();
	static void DeInit();

	static BOOL NotifyBarCreated(HWND);
	static BOOL NotifyBarDeleted(HWND);
	static BOOL NotifyBarChanged(HWND Old, HWND New);
	static BOOL NotifyControlCreated(HWND, ControlHelpInfo*);
	static BOOL NotifyControlCreated(HWND, OpDescriptor*);
	static BOOL NotifyControlDeleted(HWND);

	// Allow the control Bubble help to be changed on the fly
	static BOOL NotifyControlChanged(HWND, ControlHelpInfo*);

	typedef TCHAR *(*BubbleHelpCallback)(HWND, UINT32, void*);

	static void DoBubbleHelpOn(HWND, UINT32, BubbleHelpCallback, void*);

	static void ServiceBubbleHelp();
	static void BubbleHelpDisable();

	static void InformModalDialogOpened();
	static void InformModalDialogClosed();
	static BOOL ControlHelper::GetStatusLineText(String_256* ptext, HWND window);

	static BOOL IsUserInterfaceCaptured();

private:
	static void	BubbleHelpKill();

	static BOOL AddControl(HWND, ControlHelpInfo *);
	static ControlTable  *Controls;
	static BarTable      *Bars;
	static BubbleHelpWnd *BubbleWnd;

	static LRESULT CALLBACK MyWndProc(HWND, UINT32, WPARAM, LPARAM);
	static MonotonicTime PendingTimer;
	static POINT LastPos;
	static HWND LastControl;
	static BOOL ControlHasFocus;

	static HWND AdHocWindow;
	static UINT32 AdHocControl;
	static BOOL AdHocControlIsDifferent;
	static void *AdHocReference;
	static BubbleHelpCallback AdHocCallback;

	typedef enum
	{

		STATE_DEAD,				// No bubble help is displayed - cursor is not over a
								// a bar or a control.

		STATE_ACTIVE,			// Bubble help is displayed - cursor is over a control.

		STATE_INITIAL_PENDING,	// Waiting to show bubble help for the first time - the 
								// cursor is over a control.

		STATE_UPDATE_PENDING,	// Waiting to display help for a different control - i.e.
								// the user has moved from one control to another,
								// possible via a bar (STATE_DISABLED).

		STATE_INITIAL_DISABLED,	// Pointer is over bar, but no help has yet been displayed 
								// during this session.

		STATE_DISABLED			// Pointer over bar, and bubble help has been displayed
								// during this session.

	} BubbleState;

	static BubbleState BubbleHelpState;

	// Number of modal dialogs open (that are NOT managed by the kernel dialog manager)
	static INT32 ModalDialogs;

	// State control functions
	static void BubbleHelpStateMachine(POINT Pos, HWND ThisControl, WNDPROC WndProc);
	static void SetState(BubbleState NewState, HWND Window = NULL);
	static void DeadHandler();
	static void ActiveHandler(HWND Window);
	static void InitialPendingHandler();
	static void InitialDisabledHandler();
	static void UpdatePendingHandler();
	static void DisabledHandler();
};



/********************************************************************************************

>	class ControlEntry

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Purpose:	Contains the information for a particular control.
				The data members are:
				MonoOn
				HWND Window - the handle of the control.
				OpDescriptor *pOpDesc - pointer to the OpDescriptor associated with this
										control.
				WNDPROC WndProc - pointer to the normal WndProc for the subclassed control.
				MonoOff
	SeeAlso:	ControlTable; ControlHelper

********************************************************************************************/

class ControlEntry
{
public:
	HWND Window;			// Handle of the control being sublassed (may be a subwindow of
							// a control).
	HWND Parent;			// Not necessarily the immediate parent - just the handle of the
							// parent control, or none if this *is* the parent control.
	OpDescriptor *pOpDesc;	// The OpDescriptor for this control, or NULL if there isn't one.
	UINT32 BubbleID;			// String resource ID for bubble help.
	UINT32 StatusID;			// String resource ID for status bar text.
	UINT32 ModuleID;			// Module ID to be used when loading bubble help/status 
							// bar string resources.
	WNDPROC WndProc;		// Normal WndProc for this control.

	BOOL AddCommitHandling; // This flag is set to TRUE if the control is an edit or a combo 
							// it is used in the subclass procedure to determine if we
							// should respond to Enter and Tab keys being pressed
							// - Added by Simon (08/06/94)
	BOOL ControlStatusLineText(String_256* text);
};


/********************************************************************************************

>	class ControlTable : public SimpleCCObject

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Purpose:	Stores/manipulates an array of ControlEntry objects, in order to keep
				track of all the controls we have currently subclassed.
	SeeAlso:	ControlEntry; ControlHelper

********************************************************************************************/

class ControlTable : public SimpleCCObject
{
public:
	ControlTable();
	~ControlTable();
	BOOL Init();

	BOOL AddControl(HWND, ControlHelpInfo*, WNDPROC);
	WNDPROC DeleteControl(HWND Window, HWND *RealWindow);
	ControlEntry *FindControl(HWND Window);
	BOOL ChangeControl(HWND, ControlHelpInfo*);

private:
	INT32 FindControlIndex(HWND Window, BOOL IgnoreChildren = TRUE);

	enum
	{
		NotFound = -1,
		EmptySlot = 0,
		Granularity = 30,
		InitialSize = 100
	};

	ControlEntry *Table;
	INT32 TableSize;

	// State information - used when scanning the table for controls.
	HWND LastWindow;
	INT32  LastIndex;
};

#endif

