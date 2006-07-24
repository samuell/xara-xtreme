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


#include "camtypes.h"

#include "camelot.h"
#include "dlgevt.h"
#include "dlgmgr.h"
#include "camframe.h"
//#include "dialogop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ctrllist.h"
#include "dropdown.h"

const TCHAR * DialogEventHandler::DefaultString = _T("[Event Name Not Found]\0");
EventNumberToName * DialogEventHandler::pHash = NULL;
DialogEventHandler * DialogEventHandler::m_GrimReaper = NULL;

/*****************************************************************************

This file contains the class for DialogEventHandler, derived from
wxEvtHandler. This contains a mapping allowing dialogs of ANY class
derived from wxDialog to call the relevant bits of DialogManager.

*****************************************************************************/

IMPLEMENT_CLASS( DialogEventHandler, wxEvtHandler )
BEGIN_EVENT_TABLE(DialogEventHandler, wxEvtHandler)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_LISTBOX_SELECTED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, DialogEventHandler::CommandEvent)	// only with WXWIN_COMPATIBILITY_EVENT_TYPES
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_TEXT_ENTER, DialogEventHandler::CommandEvent)	// only with WXWIN_COMPATIBILITY_EVENT_TYPES
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_TEXT_URL, DialogEventHandler::CommandEvent)		// only with WXWIN_COMPATIBILITY_EVENT_TYPES
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_TEXT_MAXLEN, DialogEventHandler::CommandEvent)	// only with WXWIN_COMPATIBILITY_EVENT_TYPES
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_SLIDER_UPDATED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_RADIOBOX_SELECTED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_RADIOBUTTON_SELECTED, DialogEventHandler::CommandEvent)
//	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_SCROLLBAR_UPDATED, DialogEventHandler::CommandEvent) // Obselete - see wxWVT_SCROLL
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_VLBOX_SELECTED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_COMBOBOX_SELECTED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_TOOL_RCLICKED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_TOOL_ENTER, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_SPINCTRL_UPDATED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_TREE_SEL_CHANGED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, DialogEventHandler::CommandEvent)
/*
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED, DialogEventHandler::CommandEvent)
	EVT_COMMAND	(wxID_ANY, wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING, DialogEventHandler::CommandEvent)
*/
	EVT_COMMAND_SCROLL (wxID_ANY, DialogEventHandler::ScrollEvent)
	EVT_SCROLL  (DialogEventHandler::ScrollEvent)
	EVT_CLOSE	(DialogEventHandler::CloseEvent)
	EVT_WINDOW_DESTROY	(DialogEventHandler::WindowDestroyEvent)
	EVT_MOUSE_EVENTS (DialogEventHandler::MouseEvent)
	EVT_CHILD_FOCUS( DialogEventHandler::OnSetFocus )
	EVT_CAMDIALOG_DEFERREDMSG (wxID_ANY, DialogEventHandler::CamDialogEvent)
	EVT_CAMDIALOG_GRIMREAPER (wxID_ANY, DialogEventHandler::GrimReaperEvent)
	EVT_CAMDIALOG_REDRAW (wxID_ANY, DialogEventHandler::CamDialogEvent)
	EVT_MOVE	(DialogEventHandler::MoveEvent)
	EVT_SIZE	(DialogEventHandler::SizeEvent)

END_EVENT_TABLE();

DEFINE_EVENT_TYPE(wxEVT_CAMDIALOG_DEFERREDMSG)
DEFINE_EVENT_TYPE(wxEVT_CAMDIALOG_GRIMREAPER)
DEFINE_EVENT_TYPE(wxEVT_CAMDIALOG_REDRAW)
IMPLEMENT_DYNAMIC_CLASS( wxCamDialogEvent, wxEvent )

CC_IMPLEMENT_DYNAMIC(DlgEvtTimer, KernelTimer);


/********************************************************************************************

>	void DialogEvtTimer::OnNotify()

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	15/05/2005
	Inputs:		None
	Outputs:	None
	Returns:	-
	Purpose:	Act on a timer event
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DlgEvtTimer::Notify()
{
	if (m_lpfnTimer)
	{
		(*m_lpfnTimer)(m_Param);
	}
	else
	{
		//	We need to send a DIM_TIMER to the DialogOp
		BROADCAST_TO_CLASS(DialogMsg(m_pEvtHandler->pwxWindow, DIM_TIMER, 0, (UINT_PTR)m_IDEvent, 0), DialogOp);
	}
}

/********************************************************************************************

>	DialogEventHandler::DialogEventHandler()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DialogEventHandler::DialogEventHandler()
{
	pwxWindow = NULL;
	pDialogOp = NULL;
	ID=0;
	wxAUImanaged=FALSE;
	m_GrimReaperSent=FALSE;
}


/********************************************************************************************

>	DialogEventHandler::DialogEventHandler(DialogOp * pOp)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DialogEventHandler::DialogEventHandler(DialogOp * pOp)
{
	pwxWindow = NULL;
	pDialogOp = pOp;
	ID=0;
	wxAUImanaged=FALSE;
	m_GrimReaperSent=FALSE;
	m_TimerHash.clear();
}

/********************************************************************************************

>	DialogEventHandler::~DialogEventHandler()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor
	Errors:		-
	SeeAlso:	-

Note we don't normally delete these directly - we Destroy() them, which is safe even within the
event handler itself

********************************************************************************************/

DialogEventHandler::~DialogEventHandler()
{
	// clear out the kernel timers so they don't go off - this should have been done by Destroy()
	while (!m_TimerHash.empty())
	{
		IntegerToKernelTimer::iterator current = m_TimerHash.begin();
		DlgEvtTimer * t=current->second;
		m_TimerHash.erase(current);
		if (t)
			delete t;
	}
	// for good measure
	m_TimerHash.clear();
}

/********************************************************************************************

>	DialogEventHandler::Destroy()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Disconnect the DialogEventHandler from its pDialogOp and schedule destruction
	Errors:		-
	SeeAlso:	-

Note we don't normally delete these directly - we Destroy() them, which is safe even within the
event handler itself

********************************************************************************************/

void DialogEventHandler::Destroy()
{
	if (this == m_GrimReaper)
	{
		// Grim Reaper cannot delete itself as its in its own event handler. It must be
		// explicitly deleted on deinit
		ERROR3("You cannot destroy the Grim Reaper that easily!");
		return;
	}

	if (!m_GrimReaperSent)
	{
		// clear out the kernel timers so they don't go off after Destroy()
		while (!m_TimerHash.empty())
		{
			IntegerToKernelTimer::iterator current = m_TimerHash.begin();
			DlgEvtTimer * t=current->second;
			m_TimerHash.erase(current);
			if (t)
				delete t;
		}
		// for good measure
		m_TimerHash.clear();

		// Disconnect from the DialogOp
		if (pDialogOp)
			pDialogOp->pEvtHandler=NULL;
		
		// Zap out all the variables
		pDialogOp = NULL;
		pwxWindow = NULL;
		ID = 0;
		wxCamDialogEvent deferredevent (wxEVT_CAMDIALOG_GRIMREAPER, 0, DialogMsg(NULL, DIM_NONE, 0, (UINT_PTR)(void *)(this),0));
		if (m_GrimReaper)
		{
			m_GrimReaper->AddPendingEvent(deferredevent);
			return;
		}
		else
		{
			ERROR3("Can't call upon the grim reaper. Leaking memory");
		}
		m_GrimReaperSent = TRUE; // Don't do another
	}
}


/********************************************************************************************

>	DialogEventHandler::GrimReaperEvent(wxCamDialogEvent& event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Called when we need to kill off a DialogEventHandler
	Errors:		-
	SeeAlso:	-

For some reason we can't declare this guy static.

********************************************************************************************/


void DialogEventHandler::GrimReaperEvent(wxCamDialogEvent& event)
{
	WXTYPE EventType = event.GetEventType();
	if (!(event.IsKindOf(CLASSINFO(wxCamDialogEvent)) && (EventType == wxEVT_CAMDIALOG_GRIMREAPER)))
	{
		ERROR3("Grim Reaper called with bad parameters");
		return;
	}
	
	if (this != m_GrimReaper)
	{
		ERROR3("Grim Reaper event sent to an event handler other than the GrimReaper");
		return;
	}

	DialogEventHandler * pEvtHandler = (DialogEventHandler *)(void *)event.msg.DlgMsgParam;
	if (!pEvtHandler->IsKindOf(CLASSINFO(DialogEventHandler)))
	{
		ERROR3("Grim Reaper called with a non event handler parameter");
		return;
	}

	if (pEvtHandler == this)
	{
		ERROR3("The grim reaper cannot kill itself"); // we are in our own event handler, so this would be bad
		return;
	}

	TRACEUSER("amb", _T("Grim reaper reaped an event handler at %016llx"), pEvtHandler);
	delete pEvtHandler;
}

/********************************************************************************************

>	DialogEventHandler::CommandEvent(wxCommandEvent& event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Passes an event to DialogManager::Event
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void DialogEventHandler::CommandEvent(wxCommandEvent& event)
{
	DialogManager::Event(this, event);
}

/********************************************************************************************

>	DialogEventHandler::ScrollEvent(wxScrollEvent& event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Passes an event to DialogManager::Event
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void DialogEventHandler::ScrollEvent(wxScrollEvent& event)
{
	DialogManager::Event(this, event);
}

/********************************************************************************************

>	DialogEventHandler::MouseEvent(wxMouseEvent& event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Passes an event to DialogManager::Event
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void DialogEventHandler::MouseEvent(wxMouseEvent& event)
{
	DialogManager::Event(this, event);
}

/********************************************************************************************

>	DialogEventHandler::CloseEvent(wxCloseEvent& event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Passes an event to DialogManager::Event
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void DialogEventHandler::CloseEvent(wxCloseEvent& event)
{
	DialogManager::Event(this, event);
}

/********************************************************************************************

>	DialogEventHandler::WindowDestroyEvent(wxWindowDestroyEvent& event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Passes an event to DialogManager::Event
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void DialogEventHandler::WindowDestroyEvent(wxWindowDestroyEvent& event)
{
	if (!event.IsKindOf(CLASSINFO(wxWindowDestroyEvent)))
		return; // How did that get here?

	DialogManager::Event(this, event);

	// we are only interested in our window being destroyed from now on, as opposed to
	// children of our window
	if (event.GetEventObject() != pwxWindow)
		return;

	// Now diassociate all the window controls
	// Note this may have already been done by DialogManager, but in that case (a) pwxWindow won't
	// be non-NULL, and (b) we'll have removed the EventHandler from the queue, so we won't be
	// called anyway
	if (pwxWindow)
	{
		DropDown::KillDropDownsByWindow(pwxWindow);
		ControlList::Get()->RemoveWindowAndChildren(pwxWindow);
	}

	if (!m_GrimReaperSent)
	{
		m_GrimReaperSent= TRUE; // Stop DialogManager::Delete() from doing anything
		TRACE(_T("DialogEventHandler::CloseEvent called on window which has been closed other than through DialogManager\n"));
		TRACE(_T("This should never happen. Cleaning up the horrible mess as best we can\n"));
		if (pDialogOp)
		{
			// there is a running DialogOp. Let's try ending it
			pDialogOp->End();
		}
		m_GrimReaperSent = FALSE; // ensure Destroy() works
		Destroy(); // Destroy ourselves (as opposed to the window) if this hasn't happened already
	}
}

/********************************************************************************************

>	DialogEventHandler::OnSetFocus(wxCamDialogEvent& event)


	Author:		Luke_Hart <alex@alex.org.uk>
	Created:	02/05/06
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Passes an event to DialogManager::Event
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void DialogEventHandler::OnSetFocus(wxChildFocusEvent &event)
{
	// Check if focus is going to an always focus object (may need more tests
	// as more controls come online). If so just return allowing focus to stay
	wxWindow*	pWnd = (wxWindow*)event.GetEventObject();
	if( pWnd->IsKindOf( CLASSINFO(wxTextCtrl) ) ||
		pWnd->IsKindOf( CLASSINFO(wxSliderCombo) ) ||
		pWnd->IsKindOf( CLASSINFO(wxComboBox) ) ||
		pWnd->IsKindOf( CLASSINFO(wxOwnerDrawnComboBox) ) ||
		pWnd->IsKindOf( CLASSINFO(wxComboCtrl) ) )
	{
		return;
	}

	// Scan down ancestors looking for either wxPanels (always non-modal) and
	// wxDailogs (can be modal, so we check)
	while( NULL != pWnd && !pWnd->IsKindOf( CLASSINFO(wxPanel) ) )
	{
		// Dialogs may-be modal so check
		if( pWnd->IsKindOf( CLASSINFO(wxDialog) ) )
		{
			if( ((wxDialog*)pWnd)->IsModal() )
				return;

			// Non-modal dialog so do focus handling
			break;
		}

		pWnd = pWnd->GetParent();
	}

	// Put the focus back into active view
	TRACEUSER( "jlh92", _T("NO, that control is not allowed focus") );
	AfxGetApp().GiveActiveCanvasFocus();
}



/********************************************************************************************

>	DialogEventHandler::CamDialogEvent(wxCamDialogEvent& event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Passes an event to DialogManager::Event
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void DialogEventHandler::CamDialogEvent(wxCamDialogEvent& event)
{
	DialogManager::Event(this, event);
}

/********************************************************************************************

>	DialogEventHandler::MoveEvent(wxMoveEvent& event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Passes an event to DialogManager::Event
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void DialogEventHandler::MoveEvent(wxMoveEvent& event)
{
	DialogManager::Event(this, event);

	// We pretend we didn't handle it, since dialogs with sizers etc. will need it
	event.Skip();
}

/********************************************************************************************

>	DialogEventHandler::SizeEvent(wxSizeEvent& event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Passes an event to DialogManager::Event
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void DialogEventHandler::SizeEvent(wxSizeEvent& event)
{
	DialogManager::Event(this, event);
	
	// We pretend we didn't handle it, since dialogs with sizers etc. will need it
	event.Skip();
}

/********************************************************************************************

>	DialogEventHandler::NotebookEvent(wxNotebookEvent& event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		event - the wxEvent
	Outputs:	-
	Returns:	-
	Purpose:	Passes an event to DialogManager::Event
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void DialogEventHandler::NotebookEvent(wxNotebookEvent& event)
{
	DialogManager::Event(this, event);
}

/********************************************************************************************

>	static inline const TCHAR * DialogEventHandler::GetEventName(WXTYPE eventnum)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		event number
	Outputs:	-
	Returns:	pointer to a string containing the event number
	Purpose:	Returns a textual string corresponding with an event
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#define EVTDECODE(x) if (eventnum == x) return(_T( ## x));

/********************************************************************************************

>	static BOOL DialogEventHandler::Init()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Initialize resources
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

BOOL DialogEventHandler::Init()
{
	m_GrimReaper = new DialogEventHandler;
	if (!m_GrimReaper) return FALSE;

	pHash = new (EventNumberToName);
	if (!pHash) return FALSE;

	// Clear our own hash tables
	pHash->clear();

#if _DEBUG

	#if 0 != wxUSE_UNICODE
	#define HASHEVENTNAME(x) (*pHash)[x] = (EventNameString)camStrdup(L ## #x)
	#else
	#define HASHEVENTNAME(x) (*pHash)[x] = (EventNameString)camStrdup(#x)
	#endif

	HASHEVENTNAME(wxEVT_COMMAND_BUTTON_CLICKED);
    HASHEVENTNAME(wxEVT_COMMAND_CHECKBOX_CLICKED);
    HASHEVENTNAME(wxEVT_COMMAND_CHOICE_SELECTED);
    HASHEVENTNAME(wxEVT_COMMAND_LISTBOX_SELECTED);
    HASHEVENTNAME(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED);
    HASHEVENTNAME(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED);
    HASHEVENTNAME(wxEVT_COMMAND_TEXT_UPDATED);
    HASHEVENTNAME(wxEVT_COMMAND_TEXT_ENTER);
    HASHEVENTNAME(wxEVT_COMMAND_TEXT_URL);
    HASHEVENTNAME(wxEVT_COMMAND_TEXT_MAXLEN);
    HASHEVENTNAME(wxEVT_COMMAND_MENU_SELECTED);
    HASHEVENTNAME(wxEVT_COMMAND_SLIDER_UPDATED);
    HASHEVENTNAME(wxEVT_COMMAND_RADIOBOX_SELECTED);
    HASHEVENTNAME(wxEVT_COMMAND_RADIOBUTTON_SELECTED);
    HASHEVENTNAME(wxEVT_COMMAND_SCROLLBAR_UPDATED);
    HASHEVENTNAME(wxEVT_COMMAND_VLBOX_SELECTED);
    HASHEVENTNAME(wxEVT_COMMAND_COMBOBOX_SELECTED);
    HASHEVENTNAME(wxEVT_COMMAND_TOOL_RCLICKED);
    HASHEVENTNAME(wxEVT_COMMAND_TOOL_ENTER);
    HASHEVENTNAME(wxEVT_COMMAND_SPINCTRL_UPDATED);
    HASHEVENTNAME(wxEVT_COMMAND_TREE_SEL_CHANGED);
 	HASHEVENTNAME(wxEVT_SOCKET);
    HASHEVENTNAME(wxEVT_TIMER);
    HASHEVENTNAME(wxEVT_LEFT_DOWN);
    HASHEVENTNAME(wxEVT_LEFT_UP);
    HASHEVENTNAME(wxEVT_MIDDLE_DOWN);
    HASHEVENTNAME(wxEVT_MIDDLE_UP);
    HASHEVENTNAME(wxEVT_RIGHT_DOWN);
    HASHEVENTNAME(wxEVT_RIGHT_UP);
    HASHEVENTNAME(wxEVT_MOTION);
    HASHEVENTNAME(wxEVT_ENTER_WINDOW);
    HASHEVENTNAME(wxEVT_LEAVE_WINDOW);
    HASHEVENTNAME(wxEVT_LEFT_DCLICK);
    HASHEVENTNAME(wxEVT_MIDDLE_DCLICK);
    HASHEVENTNAME(wxEVT_RIGHT_DCLICK);
    HASHEVENTNAME(wxEVT_SET_FOCUS);
    HASHEVENTNAME(wxEVT_KILL_FOCUS);
    HASHEVENTNAME(wxEVT_CHILD_FOCUS);
    HASHEVENTNAME(wxEVT_MOUSEWHEEL);
    HASHEVENTNAME(wxEVT_NC_LEFT_DOWN);
    HASHEVENTNAME(wxEVT_NC_LEFT_UP);
    HASHEVENTNAME(wxEVT_NC_MIDDLE_DOWN);
    HASHEVENTNAME(wxEVT_NC_MIDDLE_UP);
    HASHEVENTNAME(wxEVT_NC_RIGHT_DOWN);
    HASHEVENTNAME(wxEVT_NC_RIGHT_UP);
    HASHEVENTNAME(wxEVT_NC_MOTION);
    HASHEVENTNAME(wxEVT_NC_ENTER_WINDOW);
    HASHEVENTNAME(wxEVT_NC_LEAVE_WINDOW);
    HASHEVENTNAME(wxEVT_NC_LEFT_DCLICK);
    HASHEVENTNAME(wxEVT_NC_MIDDLE_DCLICK);
    HASHEVENTNAME(wxEVT_NC_RIGHT_DCLICK);
    HASHEVENTNAME(wxEVT_CHAR);
    HASHEVENTNAME(wxEVT_CHAR_HOOK);
    HASHEVENTNAME(wxEVT_NAVIGATION_KEY);
    HASHEVENTNAME(wxEVT_KEY_DOWN);
    HASHEVENTNAME(wxEVT_KEY_UP);
//    HASHEVENTNAME(wxEVT_HOTKEY);
    HASHEVENTNAME(wxEVT_SET_CURSOR);
    HASHEVENTNAME(wxEVT_SCROLL_TOP);
    HASHEVENTNAME(wxEVT_SCROLL_BOTTOM);
    HASHEVENTNAME(wxEVT_SCROLL_LINEUP);
    HASHEVENTNAME(wxEVT_SCROLL_LINEDOWN);
    HASHEVENTNAME(wxEVT_SCROLL_PAGEUP);
    HASHEVENTNAME(wxEVT_SCROLL_PAGEDOWN);
    HASHEVENTNAME(wxEVT_SCROLL_THUMBTRACK);
    HASHEVENTNAME(wxEVT_SCROLL_THUMBRELEASE);
    HASHEVENTNAME(wxEVT_SCROLL_CHANGED);
    HASHEVENTNAME(wxEVT_SCROLLWIN_TOP);
    HASHEVENTNAME(wxEVT_SCROLLWIN_BOTTOM);
    HASHEVENTNAME(wxEVT_SCROLLWIN_LINEUP);
    HASHEVENTNAME(wxEVT_SCROLLWIN_LINEDOWN);
    HASHEVENTNAME(wxEVT_SCROLLWIN_PAGEUP);
    HASHEVENTNAME(wxEVT_SCROLLWIN_PAGEDOWN);
    HASHEVENTNAME(wxEVT_SCROLLWIN_THUMBTRACK);
    HASHEVENTNAME(wxEVT_SCROLLWIN_THUMBRELEASE);
    HASHEVENTNAME(wxEVT_SIZE);
    HASHEVENTNAME(wxEVT_MOVE);
    HASHEVENTNAME(wxEVT_CLOSE_WINDOW);
    HASHEVENTNAME(wxEVT_END_SESSION);
    HASHEVENTNAME(wxEVT_QUERY_END_SESSION);
    HASHEVENTNAME(wxEVT_ACTIVATE_APP);
//    HASHEVENTNAME(wxEVT_POWER);
    HASHEVENTNAME(wxEVT_ACTIVATE);
    HASHEVENTNAME(wxEVT_CREATE);
    HASHEVENTNAME(wxEVT_DESTROY);
    HASHEVENTNAME(wxEVT_SHOW);
    HASHEVENTNAME(wxEVT_ICONIZE);
    HASHEVENTNAME(wxEVT_MAXIMIZE);
    HASHEVENTNAME(wxEVT_MOUSE_CAPTURE_CHANGED);
    HASHEVENTNAME(wxEVT_PAINT);
    HASHEVENTNAME(wxEVT_ERASE_BACKGROUND);
    HASHEVENTNAME(wxEVT_NC_PAINT);
    HASHEVENTNAME(wxEVT_PAINT_ICON);
    HASHEVENTNAME(wxEVT_MENU_OPEN);
    HASHEVENTNAME(wxEVT_MENU_CLOSE);
    HASHEVENTNAME(wxEVT_MENU_HIGHLIGHT);
    HASHEVENTNAME(wxEVT_CONTEXT_MENU);
    HASHEVENTNAME(wxEVT_SYS_COLOUR_CHANGED);
    HASHEVENTNAME(wxEVT_DISPLAY_CHANGED);
    HASHEVENTNAME(wxEVT_SETTING_CHANGED);
    HASHEVENTNAME(wxEVT_QUERY_NEW_PALETTE);
    HASHEVENTNAME(wxEVT_PALETTE_CHANGED);
    HASHEVENTNAME(wxEVT_JOY_BUTTON_DOWN);
    HASHEVENTNAME(wxEVT_JOY_BUTTON_UP);
    HASHEVENTNAME(wxEVT_JOY_MOVE);
    HASHEVENTNAME(wxEVT_JOY_ZMOVE);
    HASHEVENTNAME(wxEVT_DROP_FILES);
    HASHEVENTNAME(wxEVT_DRAW_ITEM);
    HASHEVENTNAME(wxEVT_MEASURE_ITEM);
    HASHEVENTNAME(wxEVT_COMPARE_ITEM);
    HASHEVENTNAME(wxEVT_INIT_DIALOG);
   	HASHEVENTNAME(wxEVT_IDLE);
    HASHEVENTNAME(wxEVT_UPDATE_UI);
    HASHEVENTNAME(wxEVT_SIZING);
    HASHEVENTNAME(wxEVT_MOVING);
    HASHEVENTNAME(wxEVT_HIBERNATE);
    HASHEVENTNAME(wxEVT_COMMAND_LEFT_CLICK);
    HASHEVENTNAME(wxEVT_COMMAND_LEFT_DCLICK);
    HASHEVENTNAME(wxEVT_COMMAND_RIGHT_CLICK);
    HASHEVENTNAME(wxEVT_COMMAND_RIGHT_DCLICK);
    HASHEVENTNAME(wxEVT_COMMAND_SET_FOCUS);
    HASHEVENTNAME(wxEVT_COMMAND_KILL_FOCUS);
    HASHEVENTNAME(wxEVT_COMMAND_ENTER);
    HASHEVENTNAME(wxEVT_HELP);
    HASHEVENTNAME(wxEVT_DETAILED_HELP);

	// And now our own...
	HASHEVENTNAME(wxEVT_CAMDIALOG_DEFERREDMSG);
	HASHEVENTNAME(wxEVT_CAMDIALOG_REDRAW);

	TRACE(_T(">>> Event name %d=%s"),wxEVT_COMMAND_BUTTON_CLICKED,GetEventName(wxEVT_COMMAND_BUTTON_CLICKED));

	WXTYPE EventNum=wxEVT_COMMAND_BUTTON_CLICKED;
	EventNumberToName::iterator i=pHash->find(EventNum);
	const TCHAR * text = (const TCHAR *)((i==pHash->end())?NULL:i->second);
	//return text?text:DefaultString;
	if (text)
	{
		TRACE(_T("Found text %d=%s"),EventNum,text);
	}

#endif
	return TRUE;
}

/********************************************************************************************

>	static void DialogEventHandler::DeInit()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Initialize resources
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

void DialogEventHandler::DeInit()
{
	::wxYield(); // Allow grim repear events to filter through
	if (m_GrimReaper)
	{
		delete m_GrimReaper;
		m_GrimReaper = NULL;
	}

	if (pHash)
	{
		EventNumberToName::iterator current;
		while (!pHash->empty())
		{
			current = pHash->begin();
			EventNameString s=current->second;
			pHash->erase(current);
			free((void *)s);
		}
		// for good measure
		pHash->clear();
		delete (pHash);
		pHash=NULL;
	}
}

/********************************************************************************************

>	UINT32 DialogEventHandler::AddTimer((DialogOp * pDialogOp, UINT32 nIDEvent, UINT32 nElapse,
						void (* lpfnTimer)(void *) =  NULL, void * param=NULL, BOOL OneShot=FALSE)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		parameters for the timer
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Initialize resources
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

UINT32 DialogEventHandler::AddTimer(DialogOp * pDialogOp, UINT32 nIDEvent, UINT32 nElapse,
			void (* lpfnTimer)(void *) /*= NULL*/, void * param/*=NULL*/, BOOL OneShot/*=FALSE*/)
{
	DeleteTimer(nIDEvent);	// delete any previous timer
	DlgEvtTimer * pTimer = new DlgEvtTimer(this, pDialogOp, nIDEvent, lpfnTimer, param);
	if (!pTimer)
		return 0;

	m_TimerHash[nIDEvent]=pTimer;
	pTimer->Start(nElapse, OneShot);
	return nIDEvent;
}

/********************************************************************************************

>	UINT32 DialogEventHandler::DeleteTimer(UINT32 nIDEvent)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		ID of the timer
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Deletes a timer
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
BOOL DialogEventHandler::DeleteTimer(UINT32 nIDEvent)
{
	IntegerToKernelTimer::iterator i=m_TimerHash.find(nIDEvent);
	if (i==m_TimerHash.end())
		return FALSE;
	m_TimerHash.erase(nIDEvent);
	return TRUE;
}
