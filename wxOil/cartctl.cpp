// $Id: cartctl.cpp 662 2006-03-14 21:31:49Z alex $
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

#include <wx/wxprec.h>
#include <wx/event.h>
#include <wx/tooltip.h>

#include "cartctl.h"
#include "cartprov.h"
#include "dlgevt.h"
#include "wxmousestate.h"

/*****************************************************************************

This file contains the class for wxCamArtControl, derived from
wxEvtHandler. This contains a mapping allowing dialogs of ANY class
derived from wxDialog to call the relevant bits of DialogManager.

*****************************************************************************/

DEFINE_EVENT_TYPE(wxEVT_CAMARTCONTROL_INVOKE)
IMPLEMENT_DYNAMIC_CLASS( wxCamArtControlEvent, wxEvent )

IMPLEMENT_DYNAMIC_CLASS( wxCamArtControl, wxControl )
BEGIN_EVENT_TABLE(wxCamArtControl, wxControl)
	EVT_CAMARTCONTROL_INVOKE(wxID_ANY, wxCamArtControl::OnInvoke)
	EVT_PAINT(wxCamArtControl::OnPaint)
	EVT_MOUSE_EVENTS(wxCamArtControl::OnMouseEvent)
	EVT_TIMER(_R(IDC_WXCAMARTCONTROL_AUTOREPEAT), wxCamArtControl::OnTimer)
END_EVENT_TABLE();

IMPLEMENT_DYNAMIC_CLASS( wxCamArtControlXmlHandler, wxXmlResourceHandler)

/********************************************************************************************

>	BOOL wxCamArtControl::Create( wxWindow * parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
					 [TYPENOTE: Correct] long style, const wxValidator& validator = wxDefaultValidator,
					 wxCamArtControlStyle cacstyle = (wxCamArtControlStyle)0,
					 wxString opdesc = wxString(_T("")))

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		parent - parent window
				id - id to use
				pos - position
				size - size
				style - style
				validator - validator (up to here, all the same as wxControl)
				cacstyle - the style of CamArtControl to use, i.e. the flags passed
				opdesc - the opdescriptor to use, OR the empty string
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Initialize the control
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

BOOL wxCamArtControl::Create( wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
					 /*TYPENOTE: Correct*/ long style, const wxValidator& validator,
					 wxCamArtControlStyle cacstyle,
					 wxString opdesc)
{
	if (!wxControl::Create(parent, id, pos, size, style, validator))
		return FALSE;

	// Complete bodge to default to small button bars but large toolbars
	if (!(cacstyle & wxCACS_TOOLBACKGROUND))
		cacstyle = (wxCamArtControlStyle)(cacstyle | CAF_SMALL);

	m_CamArtControlStyle = cacstyle;
	m_OpDesc = opdesc;
	FindBitmap();
	return TRUE;
}

/********************************************************************************************

>	wxSize wxCamArtControl::DoGetBestSize() const


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		None
	Outputs:	None
	Returns:	The best size for the control
	Purpose:	Initialize resources
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

wxSize wxCamArtControl::DoGetBestSize() const
{
	((wxCamArtControl *)this)->FindBitmap();
	return m_BestSize;
}

/********************************************************************************************

>	void wxCamArtControl::FindBitmap()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Find the bitmap associated with the control, and set our cached size
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void wxCamArtControl::FindBitmap()
{
	m_BestSize=(CamArtProvider::Get())->GetSize((ResourceID)GetId(), GetArtFlags());
	return;
}

/********************************************************************************************

>	void wxCamArtControl::OnPaint(wxPaintEvent & event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		event - the event
	Outputs:	-
	Returns:	-
	Purpose:	Paints the control
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void wxCamArtControl::OnPaint(wxPaintEvent & event)
{
	wxPaintDC dc(this);

	wxRect rect = GetClientRect();
	CamArtProvider::Get()->Draw(dc, rect, GetId(), GetArtFlags());
}


/********************************************************************************************

>	void wxCamArtControl::OnMouseEvent(wxMouseEvent & event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		event - the event
	Outputs:	-
	Returns:	-
	Purpose:	Handles mouse events
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void wxCamArtControl::OnMouseEvent(wxMouseEvent & event)
{
	CamArtFlags OldState = m_State;
	BOOL ButtonClick=FALSE;

	if ((m_CamArtControlStyle & wxCACS_PUSHBUTTON) && IsEnabled())
	{
		if (event.LeftDown() || event.LeftDClick())
		{
			if (m_CamArtControlStyle & wxCACS_TOGGLEBUTTON)
				m_State = (CamArtFlags)(m_State ^ CAF_SELECTED);
			else
				m_State = (CamArtFlags)(m_State | CAF_SELECTED);
			ButtonClick=TRUE;
		}
		else if (event.LeftUp())
		{
			if (!(m_CamArtControlStyle & wxCACS_TOGGLEBUTTON))
				m_State = (CamArtFlags)(m_State & ~CAF_SELECTED);
			m_Timer.Stop();
		}
		else if (event.Leaving())
		{
			if (!(m_CamArtControlStyle & wxCACS_TOGGLEBUTTON))
				m_State = (CamArtFlags)(m_State & ~CAF_SELECTED);
			m_State = (CamArtFlags)(m_State & ~CAF_BUTTONHOVER);
			m_Timer.Stop();
		}
		else if (event.Entering())
		{
			if (m_CamArtControlStyle & wxCACS_ALLOWHOVER)
				m_State = (CamArtFlags)(m_State | CAF_BUTTONHOVER);
		}
		m_Value=(m_State & CAF_SELECTED)?1:0;
	}

	// Irritatingly wxMouseEvent does not propagate to the parent, but we expect it to.
	// so we have to fake it (sigh)
	// event.Skip(); // pretend we did not handle it, so it falls through to the dialog

	if ( !(GetExtraStyle() & wxWS_EX_BLOCK_EVENTS) )
	{
		wxWindow * pParent = GetParent();
		if (pParent && !pParent->IsBeingDeleted())
		{
			wxEvtHandler *pHandler = pParent->GetEventHandler();
			if (pHandler && pHandler->IsKindOf(CLASSINFO(DialogEventHandler))) // Only propagate to our own dialogs
			{
				pHandler->ProcessEvent(event);
			}
		}
	}

	if (m_State != OldState)
	{
		Refresh();
	}

	if (ButtonClick)
	{
		Invoke();
		if (IsEnabled() && !(m_CamArtControlStyle & wxCACS_TOGGLEBUTTON)) // Toggle buttons don't autorepeat
		{
			// We start the timer at the end, as the Op may be an INT32 one
			m_AREventPending = FALSE; // Just in case an event got lost in flight
			m_Timer.Start(m_ARDelay, TRUE);
		}
	}
}

/********************************************************************************************

>	void wxCamArtControl::OnTimer(wxTimerEvent & event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		event - the event
	Outputs:	-
	Returns:	-
	Purpose:	Handles autorepeat timer events
	Errors:		-
	SeeAlso:	-

We do not process these immediately because otherwise the application does not have a chance
to redraw between undo etc. Instead we send a deferred message back to ourselves

********************************************************************************************/

void wxCamArtControl::OnTimer(wxTimerEvent & event)
{
	m_Timer.Stop();

	if (!IsEnabled())
		return; // Don't even send messages if the control is greyed

	// Sadly, what can happen is that the Op can set in place a redraw, and during the redraw
	// the button will left. But our timer gets her first. So we have to check whether the
	// button is still down
	if (!::wxGetMouseState().LeftDown())
		return;

	if (!m_AREventPending) // don't process if another event was pending
	{
		m_AREventPending = TRUE;
		wxCamArtControlEvent evt(wxEVT_CAMARTCONTROL_INVOKE, GetId());
		GetEventHandler()->AddPendingEvent(evt); // Send it off deferred
	}

	m_Timer.Start(m_ARRepeat, FALSE);
}

/********************************************************************************************

>	void wxCamArtControl::OnInvoke(wxCamArtControlEvent & event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		event - the event
	Outputs:	-
	Returns:	-
	Purpose:	Handles deferred invoke events
	Errors:		-
	SeeAlso:	-

We do not process these immediately because otherwise the application does not have a chance
to redraw between undo etc. Instead we send a deferred message back to ourselves

********************************************************************************************/

void wxCamArtControl::OnInvoke(wxCamArtControlEvent & event)
{
	m_AREventPending = FALSE;
	if (!::wxGetMouseState().LeftDown())
	{
		m_Timer.Stop();
		return;
	}
	Invoke();
}

/********************************************************************************************

>	void wxCamArtControl::Invoke()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Does the work of the button being pressed
	Errors:		-
	SeeAlso:	-

We send ourselves our COMMAND event (which DialogManager picks up) and invoke the op
descriptor attached ourselves.

********************************************************************************************/

void wxCamArtControl::Invoke()
{
	if (!IsEnabled())
	{
		m_Timer.Stop();
		return; // Don't even send messages if the control is greyed
	}

	wxCommandEvent ButtonEvent(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
	ProcessEvent(ButtonEvent);
	// If it's attached to an OpDescriptor, we invoke it.
//	ControlList::Get()->Invoke(this);
}

/********************************************************************************************

>	wxCamArtControlXmlHandler::wxCamArtControlXmlHandler()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxCamArtControlXmlHandler::wxCamArtControlXmlHandler() : wxXmlResourceHandler()
{
	XRC_ADD_STYLE(wxCACS_TOOLBACKGROUND);
	XRC_ADD_STYLE(wxCACS_PUSHBUTTON);
	XRC_ADD_STYLE(wxCACS_TOGGLEBUTTON);
	XRC_ADD_STYLE(wxCACS_ALWAYS3D);
	XRC_ADD_STYLE(wxCACS_ALLOWHOVER);
	XRC_ADD_STYLE(wxCACS_NOINTERNALBORDER);
	AddWindowStyles();
}

/********************************************************************************************

>	wxObject * wxCamArtControlXmlHandler::DoCreateResource()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a wxCamArtControl from XML
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxObject * wxCamArtControlXmlHandler::DoCreateResource()
{
	XRC_MAKE_INSTANCE(control, wxCamArtControl);
	control->Create(m_parentAsWindow,
					GetID(),
					GetPosition(),
					GetSize(),
					(GetStyle() & ~wxBORDER_MASK) | wxNO_BORDER,
					wxDefaultValidator,
					(wxCamArtControlStyle)GetStyle(_T("camartstyle")),
					GetParamValue(_T("optoken"))
					);

	SetupWindow(control);

	return control;
}

/********************************************************************************************

>	bool [TYPENOTE: Correct] wxCamArtControlXmlHandler::DoCreateResource()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		node - a pointer to the xml node
	Outputs:	-
	Returns:	TRUE if we can load it
	Purpose:	Determines whether or not we can handle an object type
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

bool /*TYPENOTE: Correct*/ wxCamArtControlXmlHandler::CanHandle(wxXmlNode *node)
{
	bool	fOurClass = node->GetPropVal(wxT("class"), wxEmptyString) == wxT("wxCamArtControl");
	return fOurClass;

	// This doesn't work on some compilers (although it is identical to what's above
	// just not in a function implemented in a header)
//	return (IsOfClass(node, wxT("wxCamArtControl")));
}
