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

#include "dlgevt.h"
#include "drawctl.h"

/*****************************************************************************

This file contains the class for wxCamDrawControl, derived from
wxEvtHandler. This contains a mapping allowing dialogs of ANY class
derived from wxDialog to call the relevant bits of DialogManager.

*****************************************************************************/

DEFINE_EVENT_TYPE(wxEVT_CAMDRAWCONTROL_INVOKE)
IMPLEMENT_DYNAMIC_CLASS( wxCamDrawControlEvent, wxEvent )

IMPLEMENT_DYNAMIC_CLASS( wxCamDrawControl, wxControl )
BEGIN_EVENT_TABLE(wxCamDrawControl, wxControl)
	EVT_CAMDRAWCONTROL_INVOKE(wxID_ANY, wxCamDrawControl::OnInvoke)
	EVT_PAINT(wxCamDrawControl::OnPaint)
	EVT_MOUSE_EVENTS(wxCamDrawControl::OnMouseEvent)
END_EVENT_TABLE();

IMPLEMENT_DYNAMIC_CLASS( wxCamDrawControlXmlHandler, wxXmlResourceHandler)

/********************************************************************************************

>	BOOL wxCamDrawControl::Create( wxWindow * parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
					 [TYPENOTE: Correct] long style, const wxValidator& validator = wxDefaultValidator,
					 wxCamDrawControlStyle cacstyle = (wxCamDrawControlStyle)0,
					 wxString opdesc = wxString(_T("")))

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		parent - parent window
				id - id to use
				pos - position
				size - size
				style - style
				validator - validator (up to here, all the same as wxControl)
				cacstyle - the style of CamDrawControl to use, i.e. the flags passed
				opdesc - the opdescriptor to use, OR the empty string
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE if fails
	Purpose:	Initialize the control
	Errors:		via wxMessageBox
	SeeAlso:	-

********************************************************************************************/

BOOL wxCamDrawControl::Create( wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
					 /*TYPENOTE: Correct*/ long style, const wxValidator& validator,
					 wxCamDrawControlStyle cacstyle,
					 wxString opdesc)
{
	if (!wxControl::Create(parent, id, pos, size, style, validator))
		return FALSE;

	m_CamDrawControlStyle = cacstyle;
	m_OpDesc = opdesc;
	return TRUE;
}


/********************************************************************************************

>	void wxCamDrawControl::OnPaint(wxPaintEvent & event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		event - the event
	Outputs:	-
	Returns:	-
	Purpose:	Paints the control
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void wxCamDrawControl::OnPaint(wxPaintEvent & event)
{
	wxCamDialogEvent RedrawEvent(wxEVT_CAMDIALOG_REDRAW, GetId());
	wxWindow * pParent=GetParent();
	if (pParent)
		pParent->GetEventHandler()->ProcessEvent(RedrawEvent);	
}

/********************************************************************************************

>	void wxCamDrawControl::OnMouseEvent(wxMouseEvent & event)


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		event - the event
	Outputs:	-
	Returns:	-
	Purpose:	Handles mouse events
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void wxCamDrawControl::OnMouseEvent(wxMouseEvent & event)
{
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
}



/********************************************************************************************

>	void wxCamDrawControl::OnInvoke(wxCamDrawControlEvent & event)


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

void wxCamDrawControl::OnInvoke(wxCamDrawControlEvent & event)
{
	Invoke();
}

/********************************************************************************************

>	void wxCamDrawControl::Invoke()


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

void wxCamDrawControl::Invoke()
{
	if (!IsEnabled())
	{
		return; // Don't even send messages if the control is greyed
	}

	wxCommandEvent ButtonEvent(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
	ProcessEvent(ButtonEvent);
	// If it's attached to an OpDescriptor, we invoke it.
//	ControlList::Get()->Invoke(this);
}

/********************************************************************************************

>	wxCamDrawControlXmlHandler::wxCamDrawControlXmlHandler()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxCamDrawControlXmlHandler::wxCamDrawControlXmlHandler() : wxXmlResourceHandler()
{
	XRC_ADD_STYLE(wxCDCS_DEFAULT);
	XRC_ADD_STYLE(wxCDCS_SETCLIPRECT);
	AddWindowStyles();
}

/********************************************************************************************

>	wxObject * wxCamDrawControlXmlHandler::DoCreateResource()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a wxCamDrawControl from XML
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

wxObject * wxCamDrawControlXmlHandler::DoCreateResource()
{
	XRC_MAKE_INSTANCE(control, wxCamDrawControl);
	control->Create(m_parentAsWindow,
					GetID(),
					GetPosition(),
					GetSize(),
					(GetStyle() & ~wxBORDER_MASK) | wxNO_BORDER,
					wxDefaultValidator,
					(wxCamDrawControlStyle)GetStyle(_T("camdrawstyle")),
					GetParamValue(_T("optoken"))
					);

	SetupWindow(control);

	return control;
}

/********************************************************************************************

>	bool [TYPENOTE: Correct] wxCamDrawControlXmlHandler::DoCreateResource()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	30/12/2005
	Inputs:		node - a pointer to the xml node
	Outputs:	-
	Returns:	TRUE if we can load it
	Purpose:	Determines whether or not we can handle an object type
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

bool /*TYPENOTE: Correct*/ wxCamDrawControlXmlHandler::CanHandle(wxXmlNode *node)
{
	bool	fOurClass = node->GetPropVal(wxT("class"), wxEmptyString) == wxT("wxCamDrawControl");
	return fOurClass;

	// This doesn't work on some compilers (although it is identical to what's above
	// just not in a function implemented in a header)
//	return (IsOfClass(node, wxT("wxCamDrawControl")));
}
