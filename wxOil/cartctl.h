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

#ifndef INC_CARTCTL
#define	INC_CARTCTL

#include <wx/wxprec.h>
#include <wx/event.h>
#include <wx/xrc/xmlres.h>

#include "cartprov.h"
#include "ctrllist.h"

enum wxCamArtControlStyle
{
	// Caching flags start here - the art provider may have a different bitmap depending on the
	// state of these flags...
	wxCACS_TOOLBACKGROUND	= CAF_TOOLBACKGROUND,
	wxCACS_PUSHBUTTON		= CAF_PUSHBUTTON,
	wxCACS_TOGGLEBUTTON		= CAF_TOGGLEBUTTON,	// wxCACS_PUSHBUTTON should also be set if this is set
	wxCACS_ALWAYS3D			= CAF_ALWAYS3D,
	wxCACS_ALLOWHOVER		= CAF_ALLOWHOVER,
	wxCACS_NOINTERNALBORDER	= CAF_NOINTERNALBORDER,

	wxCACS_DEFAULT			= 0
};

/*******************************************************************************************

>	class wxCamArtControlEvent : public wxEvent

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	19/12/2005
	Purpose:	A derived event to allow for postprocessing of autorepeat on buttons
	Notes:		In the OIL
	See Also:	

********************************************************************************************/

class wxCamArtControlEvent : public wxEvent
{
public:
	// Aargh note wxEvent's constructor has paramaters the otherway around from this - designed to match wxCommandEvent
	wxCamArtControlEvent(wxEventType commandType = wxEVT_NULL, INT32 id=0 ) : wxEvent(id, commandType) {}

	wxCamArtControlEvent(const wxCamArtControlEvent& event) : wxEvent(event) {}

	virtual wxEvent *Clone() const {return new wxCamArtControlEvent(*this); }

	DECLARE_DYNAMIC_CLASS(wxCamArtControlEvent);

};

typedef void (wxEvtHandler::*wxCamArtControlEventFunction)(wxCamArtControlEvent &);

DECLARE_EVENT_TYPE(wxEVT_CAMARTCONTROL_INVOKE, -1);

#define EVT_CAMARTCONTROL_INVOKE(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_CAMARTCONTROL_INVOKE, id, -1, \
	(wxObjectEventFunction)(wxEventFunction)(wxCamArtControlEventFunction) &fn, (wxObject *) NULL),

/*******************************************************************************************

>	class wxCamArtControl : public wxEvtHandler

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	19/12/2005
	Purpose:	A base class for controls using the CamArtProvider to paint themselves
	Notes:		In the OIL
	See Also:	

This class is specificly written so it will provided cached vector-drawn art.

********************************************************************************************/

class wxCamArtControl : public wxControl
{
	DECLARE_DYNAMIC_CLASS(wxControl)
	DECLARE_EVENT_TABLE()
public:
	// Creation
	BOOL Create( wxWindow * parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
					 /*TYPENOTE: Correct*/ long style = 0, const wxValidator& validator = wxDefaultValidator,
					 wxCamArtControlStyle cacstyle = wxCACS_DEFAULT,
					 wxString opdesc = wxString(_T("")));

	// Common init
	void Init() {
					m_Value=0;
					m_CamArtControlStyle=wxCACS_DEFAULT;
					m_BestSize=wxDefaultSize;
					m_State=CAF_DEFAULT;
					m_OpDesc=wxString(_T(""));
					m_Timer.SetOwner(this, _R(IDC_WXCAMARTCONTROL_AUTOREPEAT));
					m_ARDelay=500;
					m_ARRepeat=50;
					m_AREventPending=FALSE;
				}

	// Constructors
	wxCamArtControl() { Init(); }
	wxCamArtControl( wxWindow * parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
					 /*TYPENOTE: Correct*/ long style = 0, const wxValidator& validator = wxDefaultValidator,
					 wxCamArtControlStyle cacstyle = wxCACS_DEFAULT,
					 wxString opdesc = wxString(_T("")))
					{
						Init();
						ControlList::Get()->NewControl(this);
						Create(parent, id, pos, size, style, validator, cacstyle, opdesc);
					}
	~wxCamArtControl() { m_Timer.Stop(); }
	// Override
	wxSize DoGetBestSize() const;

	// EventHandlers
	void OnPaint(wxPaintEvent & event);
	void OnMouseEvent(wxMouseEvent & event);
	void OnTimer(wxTimerEvent& event);
	void OnInvoke(wxCamArtControlEvent& event);

	// Reload bitmap
	void NewBitmap() {FindBitmap(); Refresh();}
	
	virtual void SetStyle(wxCamArtControlStyle style) { m_CamArtControlStyle=style; NewBitmap(); }
	virtual wxCamArtControlStyle GetStyle() { return m_CamArtControlStyle;}

	virtual void SetValue(UINT32 value)
		{
			m_Value=value;
			CamArtFlags o=m_State;
			if (value) m_State=(CamArtFlags)(m_State | CAF_SELECTED); else m_State =(CamArtFlags)(m_State & ~CAF_SELECTED);
			if (o!=m_State) Refresh();
		}
	virtual UINT32 GetValue() { return m_Value;}

	// override Enable to clear hover flag when control disabled
	virtual /*TYPENOTE: Correct*/ bool Enable(bool /*TYPENOTE: Correct*/ enable = true)
		{
			if (!enable)
			{
				m_State = (CamArtFlags)(m_State & ~CAF_BUTTONHOVER);
				m_Timer.Stop(); // Don't process more events
			}
			return wxControl::Enable(enable);
		}

	wxString GetOpDesc() {return m_OpDesc;}
	
protected:
	wxCamArtControlStyle m_CamArtControlStyle;
	UINT32 m_Value;
	wxSize m_BestSize;
	CamArtFlags m_State;

	wxTimer m_Timer;
	UINT32 m_ARDelay;
	UINT32 m_ARRepeat;
	BOOL m_AREventPending;

	wxString m_OpDesc;

	void Invoke();

	void FindBitmap();
	CamArtFlags GetArtFlags() {return (CamArtFlags)(m_CamArtControlStyle | m_State | (IsEnabled()?0:CAF_GREYED));}

};

/*******************************************************************************************

>	class wxCamArtControlXmlHandler : public wxXmlResourceHandler

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	19/12/2005
	Purpose:	A resource handler to enable wxCamArtControl items to be loaded from XML
	Notes:		In the OIL
	See Also:	

********************************************************************************************/

class wxCamArtControlXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxCamArtControlXmlHandler)
public:
	wxCamArtControlXmlHandler();
	virtual wxObject *DoCreateResource();
	virtual bool /*TYPENOTE: Correct*/ CanHandle(wxXmlNode *node);
};


#endif

