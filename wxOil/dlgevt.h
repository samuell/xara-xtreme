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

#ifndef INC_DLGEVT
#define	INC_DLGEVT

#include "drawctl.h"
#include "ccobject.h"
#include "dlgtypes.h"
#include "msg.h"

class	DialogManager;
class	DialogOp;

// Need a type for the hash data or we get duplicate const error
typedef const TCHAR * EventNameString;

// Declare the hash map from ResourceID to String
WX_DECLARE_HASH_MAP( WXTYPE, EventNameString, wxIntegerHash, wxIntegerEqual, EventNumberToName );

class wxCamDialogEvent;

/*******************************************************************************************

>	class DialogEventHandler : public wxEvtHandler

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	19/12/2005
	Purpose:	A derived event handler to wire up wxDialog derived classes to DialogManager
	Notes:		In the OIL
	See Also:	

********************************************************************************************/

class DialogEventHandler : public wxEvtHandler
{
	friend class DialogManager;

public:
	DialogEventHandler();
	DialogEventHandler(DialogOp * pOp);
	~DialogEventHandler(); // Note - do not call the destructor from within an event being handled!
	void Destroy();
	void CommandEvent(wxCommandEvent &event);
	void ScrollEvent(wxScrollEvent &event);
	void CloseEvent(wxCloseEvent &event);
	void MouseEvent(wxMouseEvent &event);
	void WindowDestroyEvent(wxWindowDestroyEvent &event);
	void OnSetFocus(wxChildFocusEvent &event);
	void CamDialogEvent(wxCamDialogEvent &event);
	void MoveEvent(wxMoveEvent &event);
	void SizeEvent(wxSizeEvent &event);

	void GrimReaperEvent(wxCamDialogEvent &event);

	static inline const TCHAR * GetEventName(WXTYPE EventNum) // Return the EventName
	{
		if (!pHash) return DefaultString;
		EventNumberToName::iterator i=pHash->find(EventNum);
		const TCHAR * text = (const TCHAR *)((i==pHash->end())?NULL:i->second);
		return text?text:DefaultString;
	};

	static BOOL Init();
	static void DeInit();

private:
	DECLARE_CLASS(wxEvtHandler)
	DECLARE_EVENT_TABLE()
	wxWindow * pwxWindow;
	DialogOp * pDialogOp;
	CDlgResID ID;
	BOOL wxAUImanaged;
	BOOL m_GrimReaperSent;

	static EventNumberToName * pHash;
	static const TCHAR * DefaultString;

	static DialogEventHandler * m_GrimReaper;
};


/*******************************************************************************************

>	class wxCamDialogEvent : public wxEvent

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	19/12/2005
	Purpose:	A derived event to allow for postprocessing of dialog events
	Notes:		In the OIL
	See Also:	

********************************************************************************************/

class wxCamDialogEvent : public wxEvent
{
public:
	// Aargh note wxEvent's constructor has paramaters the otherway around from this - designed to match wxCommandEvent
	wxCamDialogEvent(wxEventType commandType = wxEVT_NULL, INT32 id=0 ) : wxEvent(id, commandType), msg(NULL, DIM_NONE, 0) {}
	wxCamDialogEvent(wxEventType commandType, INT32 id, DialogMsg m ) : wxEvent(id, commandType), msg(m) {}

	wxCamDialogEvent(const wxCamDialogEvent& event) : wxEvent(event), msg (event.msg) {}

	virtual wxEvent *Clone() const {return new wxCamDialogEvent(*this); }

	DECLARE_DYNAMIC_CLASS(wxCamDialogEvent);

	DialogMsg	msg;
};

typedef void (wxEvtHandler::*wxCamDialogEventFunction)(wxCamDialogEvent &);

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxEVT_CAMDIALOG_DEFERREDMSG, 1001)
	DECLARE_EVENT_TYPE(wxEVT_CAMDIALOG_GRIMREAPER, 1002)
	DECLARE_EVENT_TYPE(wxEVT_CAMDIALOG_REDRAW, 1003)
END_DECLARE_EVENT_TYPES()

#define EVT_CAMDIALOG_DEFERREDMSG(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_CAMDIALOG_DEFERREDMSG, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCamDialogEventFunction) &fn, (wxObject *) NULL),
#define EVT_CAMDIALOG_GRIMREAPER(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_CAMDIALOG_GRIMREAPER, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCamDialogEventFunction) &fn, (wxObject *) NULL),
#define EVT_CAMDIALOG_REDRAW(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_CAMDIALOG_REDRAW, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCamDialogEventFunction) &fn, (wxObject *) NULL),

#endif

