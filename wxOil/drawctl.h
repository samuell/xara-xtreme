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

#ifndef INC_DRAWCTL
#define	INC_DRAWCTL

#include "ctrllist.h"

enum wxCamDrawControlStyle
{
	// Caching flags start here - the art provider may have a different bitmap depending on the
	// state of these flags...

	wxCDCS_DEFAULT			= 0,
	wxCDCS_SETCLIPRECT		= 1<<0
};

/*******************************************************************************************

>	class wxCamDrawControlEvent : public wxEvent

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	19/12/2005
	Purpose:	A derived event to allow for postprocessing of autorepeat on buttons
	Notes:		In the OIL
	See Also:	

********************************************************************************************/

class wxCamDrawControlEvent : public wxEvent
{
public:
	// Aargh note wxEvent's constructor has paramaters the otherway around from this - designed to match wxCommandEvent
	wxCamDrawControlEvent(wxEventType commandType = wxEVT_NULL, INT32 id=0 ) : wxEvent(id, commandType) {}

	wxCamDrawControlEvent(const wxCamDrawControlEvent& event) : wxEvent(event) {}

	virtual wxEvent *Clone() const {return new wxCamDrawControlEvent(*this); }

	DECLARE_DYNAMIC_CLASS(wxCamDrawControlEvent);

};

typedef void (wxEvtHandler::*wxCamDrawControlEventFunction)(wxCamDrawControlEvent &);

DECLARE_EVENT_TYPE(wxEVT_CAMDRAWCONTROL_INVOKE, -1);

#define EVT_CAMDRAWCONTROL_INVOKE(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_CAMDRAWCONTROL_INVOKE, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCamDrawControlEventFunction) &fn, (wxObject *) NULL),

/*******************************************************************************************

>	class wxCamDrawControl : public wxEvtHandler

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	19/12/2005
	Purpose:	A base class for controls using the CamDrawProvider to paint themselves
	Notes:		In the OIL
	See Also:	

This class is specificly written so it will provided cached vector-drawn art.

********************************************************************************************/

class wxCamDrawControl : public wxControl
{
	DECLARE_DYNAMIC_CLASS(wxControl)
	DECLARE_EVENT_TABLE()
public:
	// Creation
	BOOL Create( wxWindow * parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
					 /*TYPENOTE: Correct*/ long style = 0, const wxValidator& validator = wxDefaultValidator,
					 wxCamDrawControlStyle cacstyle = wxCDCS_DEFAULT,
					 wxString opdesc = wxString(_T("")));

	// Common init
	void Init() {
					m_CamDrawControlStyle=wxCDCS_DEFAULT;
					m_OpDesc=wxString(_T(""));
				}

	// Constructors
	wxCamDrawControl() { Init(); }
	wxCamDrawControl( wxWindow * parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
					 /*TYPENOTE: Correct*/ long style = 0, const wxValidator& validator = wxDefaultValidator,
					 wxCamDrawControlStyle cacstyle = wxCDCS_DEFAULT,
					 wxString opdesc = wxString(_T("")))
					{
						Init();
						ControlList::Get()->NewControl(this);
						Create(parent, id, pos, size, style, validator, cacstyle, opdesc);
					}
	~wxCamDrawControl() {  }

	// EventHandlers
	void OnPaint(wxPaintEvent & event);
	void OnMouseEvent(wxMouseEvent & event);
	void OnInvoke(wxCamDrawControlEvent& event);

	virtual void SetStyle(wxCamDrawControlStyle style) { m_CamDrawControlStyle=style; }
	virtual wxCamDrawControlStyle GetStyle() { return m_CamDrawControlStyle;}

	wxString GetOpDesc() {return m_OpDesc;}
	
protected:
	wxCamDrawControlStyle m_CamDrawControlStyle;

	wxString m_OpDesc;

	void Invoke();

};

/*******************************************************************************************

>	class wxCamDrawControlXmlHandler : public wxXmlResourceHandler

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	19/12/2005
	Purpose:	A resource handler to enable wxCamDrawControl items to be loaded from XML
	Notes:		In the OIL
	See Also:	

********************************************************************************************/

class wxCamDrawControlXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxCamDrawControlXmlHandler)
public:
	wxCamDrawControlXmlHandler();
	virtual wxObject *DoCreateResource();
	virtual bool /*TYPENOTE: Correct*/ CanHandle(wxXmlNode *node);
};


#endif

